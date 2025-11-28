#!/usr/bin/awk -f

function grab_val(    i, v) {
	v = $3
	for (i = 4; i <= NF; i++) v = v " " $i
	gsub(/U?L?$/, "", v)
	return v
}

/^#define[ \t]+EM_/ {
	name = $2
	val  = grab_val()
	i = ++count
	names[i] = name
	vals[i]  = val
}
END {
	print "/* ELF Machine Types */"
	print "#define ELF_MACHINES(X) \\"
	for (i = 1; i <= count; i++) {
		printf "\tX(%s, %s)", names[i], vals[i]
		if (i < count) print " \\"
		else print ""
	}
	print ""
}

