#!/bin/sh
# (c) 2026 FRINKnet & Friends – MIT licence

set -eu

ROOT="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"

# clean old generated net X-files
rm -f "$ROOT/include/x/"ip_*

# setup network urls
iana_url="https://www.iana.org/assignments"
protocols_url="$iana_url/protocol-numbers/protocol-numbers-1.csv"
ports_url="$iana_url/service-names-port-numbers/service-names-port-numbers.csv"

xfiles() {
  local lst="$1"
  local out="${ROOT}/include/x/ip_${lst}.h"
  local title=$(echo "$lst constants" | tr '[:lower:]' '[:upper:]')
  local url_var="${lst}_url"
  local url="$(eval echo "\$$url_var")"
  local tmp="${out}.tmp"

  : > "$tmp"
  printf "%-40s %s\n" "Parsing $lst constants..." "ip_${lst}.h"

  # Fetch and concat all given relative paths
  if ! curl -kfsSL "$url" >> "$tmp" 2>/dev/null; then
    echo "ERROR: Failed to fetch $url" >&2
    rm -f "$tmp"
    exit 1
  fi

  {
    echo "/**"
    echo " * ${title} IMPORTED  //  last updated: $(date -u +"%Y-%m-%d %H:%M:%S UTC")"
    for rel in "$@"; do
      echo " * ${url}"
    done
    echo " */"
    echo
    awk -F, -f "$ROOT/utils/get_ip_$lst.awk" "$tmp" | sort
  } > "$out"

  if [ ! -s "$out" ]; then
    echo "ERROR: awk failed for $lst" >&2
    rm -f "$tmp" "$out"
    exit 1
  fi

  rm -f "$tmp"
  sleep 2 || true
}

xfiles  protocols
xfiles  ports
