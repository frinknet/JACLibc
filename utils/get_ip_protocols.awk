#!/usr/bin/awk -f
# (c) 2026 FRINKnet & Friends – MIT licence

# Helper: trim leading/trailing whitespace
function trim(s) { gsub(/^[ \t]+|[ \t]+$/, "", s); return s }

BEGIN {
    # POSIX constant names
    override[0]   = "HOPOPTS"
    override[1]   = "ICMP"
    override[2]   = "IGMP"
    override[4]   = "IPV4"
    override[6]   = "TCP"
    override[17]  = "UDP"
    override[41]  = "IPV6"
    override[43]  = "ROUTING"
    override[44]  = "FRAGMENT"
    override[47]  = "GRE"
    override[50]  = "ESP"
    override[51]  = "AH"
    override[58]  = "ICMPV6"
    override[59]  = "NONE"
    override[60]  = "DSTOPTS"
    override[255]  = "RAW"
}

$1 ~ /^[0-9]+$/ {
  decimal = $1 + 0
  keyword = trim($2)
  protocol = trim($3)

  # Apply POSIX override if available
  if (decimal in override) {
    kw_clean = override[decimal]
  } else {
    # Fallback: clean IANA keyword for C enum name
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
