#!/bin/sh
set -eu

ROOT="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"

rm -rf "$ROOT/include/x/"format*

# OS versions
linux_ver="v6.17"
darwin_ver="main"
wine_ver="master"

# Base URLs
linux_url="https://raw.githubusercontent.com/torvalds/linux/${linux_ver}"
darwin_url="https://raw.githubusercontent.com/apple-oss-distributions/xnu/${darwin_ver}"
wine_url="https://raw.githubusercontent.com/wine-mirror/wine/${wine_ver}"

xfiles() {
  os="$1"
  fmt="$2"
  url="$(eval echo "\$${os}_url")$3"
  gen="${ROOT}/include/x/format_${fmt}.h"
  title=$(echo "$os $fmt" | tr '[:lower:]' '[:upper:]')

  printf "%-40s %s\n" "Fetching $fmt format..." "format_${fmt}.h"

  if ! curl -fsSL "$url" > "${gen}.tmp" 2>/dev/null; then
    echo "ERROR: Failed to fetch $fmt from $url" >&2
    rm -f "${gen}.tmp"
    exit 1
  fi

  {
    echo "/**"
    echo " * $title FORMAT IMPORTED  //  last updated: $(date -u +"%Y-%m-%d %H:%M:%S UTC")"
    echo " * $url"
    echo " */"
    echo ""
    awk -f "$ROOT/utils/get_formats_${fmt}.awk" "${gen}.tmp"
  } > "$gen"

  if [ ! -s "$gen" ]; then
    echo "ERROR: awk failed for $fmt" >&2
    rm -f "${gen}.tmp"
    exit 1
  fi

  rm "${gen}.tmp"
  sleep 1
}

# Fetch formats
xfiles  linux   elf     "/include/uapi/linux/elf.h"
xfiles  linux   em      "/include/uapi/linux/elf-em.h"
xfiles  linux   auxv    "/include/uapi/linux/auxvec.h"
xfiles  darwin  macho   "/EXTERNAL_HEADERS/mach-o/loader.h"
xfiles  wine    pe      "/include/winnt.h"
