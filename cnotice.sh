#!/bin/bash
(c) 2025-2026

set -e

cnotice() {
  local f=$1
  local num=$( [[ $f =~ \.(sh|awk)$ ]] && echo 2 || echo 1 )
  local line=$(sed -n "${num}p" "$f")

  echo "num: $num"
  echo "line: $line"

  if [[ $old =~ \(c\)\ ([0-9]{4})(-[0-9]{4})? ]]; then
    local old=${BASH_REMATCH[0]}
    local beg=${BASH_REMATCH[1]}
    local end=$(date +%Y)
    local new="(c) $beg-$end"

    echo "old: $old"
    echo "new: $new"
    echo "beg: $beg"
    echo "end: $end"

    [[ "$beg" == "$end" ]] && return 0  # unchanged
    [[ "$old" == "$new" ]] && return 0  # unchanged

    sed -i.bak "${num}s|.*|$new|" "$f" && rm "$f.bak"
    git add "$f"
    echo "$f: $old → $new"
  fi
}

if [ $# -eq 1 ]; then
  files=("$@")
else
  files=( $(
    git log --since="1 year ago" --name-only --pretty=format: --diff-filter=AM | \
    grep -E '\\.(c|h|sh|awk)$' | sort -u
  ) )
fi

for f in "${files[@]}"; do
  [ -f "$f" ] && cnotice "$f"
done
