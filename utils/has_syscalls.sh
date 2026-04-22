#!/bin/sh
# (c) 2025 FRINKnet & Friends – MIT licence

set -e

ROOT="$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)"
CHECK="SYS_$1,"
MATCH=""
FILE="${ROOT}/include/x/${1}_generic.h"

if [ -n "$2" ]; then
  MATCH="$1"
  CHECK="SYS_$2,"
  FILE="${ROOT}/include/x/${1}_${2}.h"
fi

[ -f "$FILE" ] && exec awk '
/X\(/ {
  match($0, /SYS_([a-z0-9_]+), *([0-9]+), *([a-z0-9_]+)/, caps)
  sys = caps[1]; num = caps[2]; fn = caps[3]

  # Track both directions
  numlist[fn] = (fn in numlist ? numlist[fn] ", " num : num)
  fnlist[num] = (num in fnlist ? fnlist[num] ", " fn : fn)

  # Goods: unique sys==fn
  if (sys == fn) goods[num] = fn
}
END {
  # Goods first - numeric sort
  split("", gkeys); gi=0
  for (n in goods) gkeys[++gi] = n+0
  asort(gkeys)
  for (i=1; i<=gi; i++) print "🗸 " gkeys[i] " = " goods[gkeys[i]]

  # Dup names -> numbers
  split("", nkeys); ni=0
  for (f in numlist) if (split(numlist[f], _, /,/) > 1) nkeys[++ni] = f
  asort(nkeys)
  for (i=1; i<=ni; i++) print "🗴 duplicate \"" nkeys[i] "\" in [" numlist[nkeys[i]] "]"

  # Dup numbers -> names
  split("", fkeys); fi=0
  for (n in fnlist) if (split(fnlist[n], _, /,/) > 1) fkeys[++fi] = n
  asort(fkeys)
  for (i=1; i<=fi; i++) print "🗴 duplicate \"" fkeys[i] "\" in [" fnlist[fkeys[i]] "]"
}' "${ROOT}/include/x/${1}_generic.h"

# get X Files
xfind() {
  os="$1" arch="$2" shift 2
  file="${os}_${arch}.h"
  gen="${ROOT}/include/x/${file}"

  if [ -n "$MATCH" ] && [ "$MATCH" != "$os" ]; then
    return 0  # explicit success
  fi

  if [ -f "$gen" ] && grep -q "$CHECK" "$gen"; then
    echo "🗸 found   - $file"
  else
    echo "🗴 missing - $file"
  fi
}

# Linux
xfind  linux      x64
xfind  linux      x86
xfind  linux      arm32
xfind  linux      mips32
xfind  linux      mips64
xfind  linux      hppa
xfind  linux      powerpc32
xfind  linux      powerpc64
xfind  linux      s390x
xfind  linux      sparc32
xfind  linux      sparc64
xfind  linux      sh4
xfind  linux      xtensa
xfind  linux      alpha
xfind  linux      arc
xfind  linux      arm64
xfind  linux      csky
xfind  linux      hexagon
xfind  linux      loongarch64
xfind  linux      m68k
xfind  linux      microblaze
xfind  linux      nios2
xfind  linux      or1k
xfind  linux      riscv32
xfind  linux      riscv64

# Darwin
xfind  darwin     x64
xfind  darwin     arm64

# FreeBSD
xfind  freebsd    x64
xfind  freebsd    x86

# NetBSD
xfind  netbsd     alpha
xfind  netbsd     x64
xfind  netbsd     x86
xfind  netbsd     arm32
xfind  netbsd     arm64
xfind  netbsd     powerpc32
xfind  netbsd     powerpc64
xfind  netbsd     sparc32
xfind  netbsd     sparc64
xfind  netbsd     riscv32
xfind  netbsd     riscv64
xfind  netbsd     ia64
xfind  netbsd     hppa
xfind  netbsd     vax

# OpenBSD
xfind  openbsd    x64
xfind  openbsd    x86

# Dragonfly
xfind  dragonfly  x64
xfind  dragonfly  x86

# Windows
xfind  windows    x64
xfind  windows    x86
xfind  windows    arm64
