#!/usr/bin/awk -f

function grab_val(    i, v) {
	v = $3
	for (i = 4; i <= NF; i++) v = v " " $i
	gsub(/U?L?$/, "", v)
	return v
}

/^#define[ \t]+AT_/ {
	name = $2
	val  = grab_val()

	if (!( "AUXV" in count )) count["AUXV"] = 0
	i = ++count["AUXV"]
	names["AUXV", i] = name
	vals["AUXV", i]  = val
}

END {
	print "/* ELF Auxiliary Vector */"
	print "#define ELF_AUXV(X) \\"
	for (i = 1; i <= count["AUXV"]; i++) {
		printf "\tX(%s, %s)", names["AUXV", i], vals["AUXV", i]
		if (i < count["AUXV"]) print " \\"
		else print ""
	}
	print ""
}

