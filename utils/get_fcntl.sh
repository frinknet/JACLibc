#!/bin/sh
# (c) 2025 FRINKnet & Friends – MIT licence

set -eu

ROOT="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"

rm -f "$ROOT/include/x/"fcntl_*

# OS versions
linux_ver="v6.17"
darwin_ver="main"
freebsd_ver="main"
netbsd_ver="HEAD"
openbsd_ver="HEAD"
dragonfly_ver="master"

# Base URLs
linux_url="https://raw.githubusercontent.com/torvalds/linux/${linux_ver}"
darwin_url="https://raw.githubusercontent.com/apple-oss-distributions/xnu/${darwin_ver}"
freebsd_url="https://cgit.freebsd.org/src/plain"
netbsd_url="http://cvsweb.netbsd.org/bsdweb.cgi/src/sys/sys"
openbsd_url="https://cvsweb.openbsd.org/checkout/src"
dragonfly_url="https://gitweb.dragonflybsd.org/dragonfly.git/blob_plain/${dragonfly_ver}:"

# get X Files
xfiles() {
  os="$1"
  arch="$2"
  shift 2

  file="fcntl_${os}_${arch}.h"
  gen="${ROOT}/include/x/${file}"
  tmp="${gen}.tmp"
  title=$(echo "$os" | tr '[:lower:]' '[:upper:]')
  base_url="$(eval echo "\$${os}_url")"

  printf "%-40s %s\n" "Parsing $os $arch fcntl constants..." "$file"

  : > "$tmp"

  for rel in "$@"; do
    url="${base_url}${rel}"
    if ! curl -fksSL "$url" >> "$tmp" 2>/dev/null; then
      echo "ERROR: Failed to fetch $url" >&2
      rm -f "$tmp"
      exit 1
    fi
  done

  {
    echo "/**"
    echo " * $title FCNTL CONSTANTS  //  last updated: $(date -u +"%Y-%m-%d %H:%M:%S UTC")"
    for rel in "$@"; do
      echo " * ${base_url}${rel}"
    done
    echo " */"
    echo ""
    awk -varch="$arch" -f "$ROOT/utils/get_fcntl_${os}.awk" "$tmp"
  } > "$gen"

  if [ ! -s "$gen" ]; then
    echo "ERROR: awk failed $file from $base_url" >&2
    exit 1
  fi

  rm "$tmp"
  sleep 1
}

# gen X Stubs
xstubs() {
  os="$1"
  arch="$2"
  file="fcntl_${os}_${arch}.h"

  printf "%-40s %s\n" "Linking $os $arch fcntl constants..." "$file"
  cd "$ROOT/include/x"
  ln -sf fcntl_${os}_generic.h $file
}

# Linux
xfiles  linux      generic    "/include/uapi/asm-generic/fcntl.h" \
                              "/include/uapi/linux/fcntl.h"
xfiles  linux      sparc64    "/arch/sparc/include/uapi/asm/fcntl.h"
xfiles  linux      sparc32    "/arch/sparc/include/uapi/asm/fcntl.h"
xfiles  linux      alpha      "/arch/alpha/include/uapi/asm/fcntl.h"
xfiles  linux      hppa       "/arch/parisc/include/uapi/asm/fcntl.h"
xfiles  linux      mips64     "/arch/mips/include/uapi/asm/fcntl.h"
xfiles  linux      mips32     "/arch/mips/include/uapi/asm/fcntl.h"
xstubs  linux      x64
xstubs  linux      x86
xstubs  linux      arm32
xstubs  linux      arm64
xstubs  linux      arc
xstubs  linux      csky
xstubs  linux      hexagon
xstubs  linux      loongarch64
xstubs  linux      m68k
xstubs  linux      microblaze
xstubs  linux      nios2
xstubs  linux      or1k
xstubs  linux      riscv32
xstubs  linux      riscv64
xstubs  linux      powerpc32
xstubs  linux      powerpc64
xstubs  linux      s390x
xstubs  linux      sh4
xstubs  linux      xtensa

# Darwin
xfiles  darwin     generic    "/bsd/sys/fcntl.h"
xstubs  darwin     x64
xstubs  darwin     arm64

# FreeBSD
xfiles  freebsd    generic    "/sys/sys/fcntl.h"
xstubs  freebsd    x64
xstubs  freebsd    x86

# NetBSD
xfiles  netbsd     generic    "/fcntl.h?rev=${netbsd_ver}&content-type=text/plain"
xstubs  netbsd     alpha
xstubs  netbsd     x64
xstubs  netbsd     x86
xstubs  netbsd     arm32
xstubs  netbsd     arm64
xstubs  netbsd     powerpc32
xstubs  netbsd     powerpc64
xstubs  netbsd     sparc32
xstubs  netbsd     sparc64
xstubs  netbsd     riscv32
xstubs  netbsd     riscv64
xstubs  netbsd     ia64
xstubs  netbsd     hppa
xstubs  netbsd     vax

# OpenBSD
xfiles  openbsd    generic    "/sys/sys/fcntl.h"
xstubs  openbsd    x64
xstubs  openbsd    x86

# DragonFly
xfiles  dragonfly  generic    "/sys/sys/fcntl.h"
xstubs  dragonfly  x64
xstubs  dragonfly  x86
