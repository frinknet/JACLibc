#!/bin/bash
# (c) 2026 FRINKnet & Friends – MIT licence

cdate=0

cnotice() {
  local f=$1 y=$2
  local num=$( [[ $f =~ \.(sh|awk)$ ]] && echo 2 || echo 1 )
  local line=$(sed -n "${num}p" "$f")

  if [[ $line =~ \(c\)\ ([0-9]{4})(-[0-9]{4})? ]]; then
    local old=${BASH_REMATCH[0]}
    local beg=${BASH_REMATCH[1]}
    local end=${y:-$(git log -1 --format='%ad' --date=format:%Y --follow "$f" 2>/dev/null || date +%Y)}
    local new="(c) $beg-$end"

    if [[ "$beg" != "$end" ]] && [[ "$old" != "$new" ]]; then
      sed -i.bak "${num}s|$old|$new|" "$f" && rm "$f.bak"
      echo "$f: $new"

      [ $end -gt $cdate ] && cdate=$end
    else
      echo "$f: $old"
    fi
  else
    echo "$f: --- no copyright"
  fi
}

if [ $# -gt 0 ]; then
  files=("$@")
else
  files=( $(
    git ls-files | \
    grep -E '\\.(c|h|js|sh|awk)$' | sort -u
  ) )
fi

for f in "${files[@]}"; do
  [ -f "$f" ] && cnotice "$f"
done

# LICENSE sync
[ $# -eq 0 ] && [ -f LICENSE ] && cnotice LICENSE "$cdate"
