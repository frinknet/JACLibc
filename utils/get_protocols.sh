#!/bin/sh

ROOT="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
URL="https://www.iana.org/assignments/protocol-numbers/protocol-numbers-1.csv"
OUT="$ROOT/include/x/ip_protocols.h"

{
  echo "/**"
  echo " * PROTOCOL CONSTANTS  //  last updated: $(date -u +"%Y-%m-%d %H:%M:%S UTC")"
  echo " * ${URL}"
  echo " */"
  echo
} > $OUT

curl -s $URL | tail -n +2 | awk -F, ' \
# Helper: trim leading/trailing whitespace
function trim(s) { gsub(/^[ \t]+|[ \t]+$/, "", s); return s }

$1 ~ /^[0-9]+$/ {
  decimal = $1 + 0
  keyword = trim($2)
  protocol = trim($3)

  # Clean keyword for C enum name
  kw_clean = keyword
  gsub(/\s*\(deprecated\)/, "", kw_clean)
  gsub(/[^A-Za-z0-9]/, "_", kw_clean)
  gsub(/_+/, "_", kw_clean)
  gsub(/^_+|_+$/, "", kw_clean)

  # Fallback if keyword is empty/numeric
  if (kw_clean == "" || kw_clean ~ /^[0-9]+$/) {
    if (decimal == 255) { kw_clean = "RAW" }
    else if (decimal == 254) { kw_clean = "EXP2" }
    else if (decimal == 253) { kw_clean = "EXP1" }
    else { kw_clean = "PROTO_" decimal }
  }

  # Short name (3rd arg) - trimmed, deprecated stripped
  short_name = trim(keyword)
  gsub(/\s*\(deprecated\)/, "", short_name)
  gsub(/^"+|"+$/, "", short_name)
  if (short_name == "") short_name = (keyword != "") ? keyword : kw_clean

  # Description (4th arg) - trimmed, quotes stripped, flattened
  desc = trim(protocol)
  gsub(/^"+|"+$/, "", desc)
  gsub(/\n/, " ", desc)
  gsub(/  +/, " ", desc)
  if (desc == "") {
    if (decimal == 255) desc = "Reserved"
    else if (decimal == 253 || decimal == 254) desc = "Experimentation"
    else desc = "Unassigned"
  }

  printf "X(IPPROTO_%s, %d, \"%s\", \"%s\")\n", toupper(kw_clean), decimal, short_name, desc
  next
}
$1 ~ /^[0-9]+-[0-9]+$/ {
  split($1, r, "-")
  for(i=r[1]; i<=r[2]; i++) printf "/* %d: Unassigned */\n", i
}
' >> $OUT
