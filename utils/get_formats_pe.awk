#!/usr/bin/awk -f

BEGIN {
	print "/* PE Magic Numbers */"
	print "#define PE_MAGICS(X) \\"
}

/^#define IMAGE_DOS_SIGNATURE/ || /^#define IMAGE_NT_SIGNATURE/ {
	name = $2
	val = $3
	gsub(/U?L?$/, "", val)
	printf "\tX(%s, %s) \\\n", name, val
}

/^#define IMAGE_FILE_MACHINE_/ {
	name = $2
	val = $3
	gsub(/U?L?$/, "", val)
	if (!seen_machine) {
		print ""
		print "/* PE Machine Types */"
		print "#define PE_MACHINES(X) \\"
		seen_machine = 1
	}
	printf "\tX(%s, %s) \\\n", name, val
}

/^#define IMAGE_SUBSYSTEM_/ {
	name = $2
	val = $3
	gsub(/U?L?$/, "", val)
	if (!seen_sub) {
		print ""
		print "/* PE Subsystems */"
		print "#define PE_SUBSYSTEMS(X) \\"
		seen_sub = 1
	}
	printf "\tX(%s, %s) \\\n", name, val
}

/^#define IMAGE_SCN_/ {
	name = $2
	val = $3
	gsub(/U?L?$/, "", val)
	if (!seen_scn) {
		print ""
		print "/* PE Section Characteristics */"
		print "#define PE_SECTION_FLAGS(X) \\"
		seen_scn = 1
	}
	printf "\tX(%s, %s) \\\n", name, val
}

END {
	print ""
}

