#!/usr/bin/awk -f

BEGIN {
	print "/* Mach-O Magic Numbers */"
	print "#define MACH_MAGICS(X) \\"
}

/^#define MH_MAGIC/ {
	name = $2
	val = $3
	gsub(/U?L?$/, "", val)
	printf "\tX(%s, %s) \\\n", name, val
}

/^#define LC_/ {
	name = $2
	val = $3
	gsub(/U?L?$/, "", val)
	if (!seen_lc) {
		print ""
		print "/* Mach-O Load Commands */"
		print "#define MACH_LOAD_COMMANDS(X) \\"
		seen_lc = 1
	}
	printf "\tX(%s, %s) \\\n", name, val
}

/^#define MH_/ && !/MAGIC/ {
	name = $2
	val = $3
	gsub(/U?L?$/, "", val)
	if (!seen_mh) {
		print ""
		print "/* Mach-O Header Flags */"
		print "#define MACH_FLAGS(X) \\"
		seen_mh = 1
	}
	printf "\tX(%s, %s) \\\n", name, val
}

/^#define CPU_TYPE_/ || /^#define CPU_ARCH_/ {
	name = $2
	val = $3
	gsub(/U?L?$/, "", val)
	if (!seen_cpu) {
		print ""
		print "/* Mach-O CPU Types */"
		print "#define MACH_CPU_TYPES(X) \\"
		seen_cpu = 1
	}
	printf "\tX(%s, %s) \\\n", name, val
}

END {
	print ""
}

