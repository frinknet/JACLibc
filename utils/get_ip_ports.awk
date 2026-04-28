#!/usr/bin/awk -f
# (c) 2026 FRINKnet & Friends – MIT licence

function trim(s) {
    gsub(/^[ \t]+|[ \t]+$/, "", s)
    return s
}

BEGIN { OFS = "" }

{
  # Clean fields
  for(i=1; i<=NF; i++) {
    gsub(/^"+|"+$/, "", $i)
  }

  $1 = trim($1); $2 = trim($2); $3 = trim($3); $4 = trim($4)
}

$2 ~ /^[0-9]+$/ {
  port = $2 + 0
  svc = $1
  proto = tolower($3)
  desc = $4

  # Treat each port as a key; favor first non‑empty desc
  if ( ! seen_port[port] ) {
    # Build clean keyword
    kw_clean = svc

    gsub(/ *\(deprecated\).*/, "", kw_clean)
    gsub(/[^A-Za-z0-9]/, "_", kw_clean)
    gsub(/_+/, "_", kw_clean)

    kw_clean = toupper(kw_clean)

    if (kw_clean == "" || kw_clean ~ /^[0-9]+$/) {
      if (port == 0) kw_clean = "PORT_0"
      else kw_clean = "PORT_" port
    }

    short_name = svc

    gsub(/ *\(deprecated\).*/, "", short_name)
    gsub(/^"+|"+$/, "", short_name)

    if (short_name == "") short_name = (svc != "") ? svc : kw_clean

    desc = trim(desc)

    gsub(/^"+|"+$/, "", desc)
    gsub(/\n/, " ", desc)
    gsub(/  +/, " ", desc)

    if (desc == "") desc = "Unassigned"

    printf "X(IPPORT_%s, %d, \"%s\", \"%s\")\n", kw_clean, port, short_name, desc

    seen_port[port] = 1
  }
}
