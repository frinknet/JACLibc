#!/usr/bin/awk -f

function grab_val(    i, v) {
	v = $3
	for (i = 4; i <= NF; i++) v = v " " $i
	gsub(/U?L?$/, "", v)
	return v
}

/^#define[ \t]+(EI_|ELFMAG|ELFCLASS|ELFDATA|EV_|ET_|PT_|PF_|SHT_|SHF_|STB_|STT_|STV_|DT_)/ {
	name = $2
	val  = grab_val()

	# Skip string-valued macros (e.g. ELFMAG "\177ELF")
	if (val ~ /^"/) {
		next
	}

	# Decide group by prefix
	if (name ~ /^EI_/) {
		group = "IDENT"
	} else if (name ~ /^ELFMAG/ || name ~ /^ELFCLASS/ || name ~ /^ELFDATA/ || name ~ /^EV_/) {
		group = "CONSTANTS"
	} else if (name ~ /^ET_/) {
		group = "TYPES"
	} else if (name ~ /^PT_/) {
		group = "PHDRS"
	} else if (name ~ /^PF_/) {
		group = "PHDR_FLAGS"
	} else if (name ~ /^SHT_/) {
		group = "SECTIONS"
	} else if (name ~ /^SHF_/) {
		group = "SECTION_FLAGS"
	} else if (name ~ /^STB_/ || name ~ /^STT_/ || name ~ /^STV_/) {
		group = "SYMBOLS"
	} else if (name ~ /^DT_/) {
		group = "DYNAMIC"
	} else {
		next
	}

	# Store
	if (!(group in count)) count[group] = 0
	i = ++count[group]
	names[group, i] = name
	vals[group, i]  = val
}

END {
	print "/* ELF Identification */"
	print "#define ELF_IDENT(X) \\"
	for (i = 1; i <= count["IDENT"]; i++) {
		printf "\tX(%s, %s)", names["IDENT", i], vals["IDENT", i]
		if (i < count["IDENT"]) print " \\"
		else print ""
	}

	print ""
	print "/* ELF Constants */"
	print "#define ELF_CONSTANTS(X) \\"
	for (i = 1; i <= count["CONSTANTS"]; i++) {
		printf "\tX(%s, %s)", names["CONSTANTS", i], vals["CONSTANTS", i]
		if (i < count["CONSTANTS"]) print " \\"
		else print ""
	}

	print ""
	print "/* ELF File Types */"
	print "#define ELF_TYPES(X) \\"
	for (i = 1; i <= count["TYPES"]; i++) {
		printf "\tX(%s, %s)", names["TYPES", i], vals["TYPES", i]
		if (i < count["TYPES"]) print " \\"
		else print ""
	}

	print ""
	print "/* ELF Program Header Types */"
	print "#define ELF_PHDRS(X) \\"
	for (i = 1; i <= count["PHDRS"]; i++) {
		printf "\tX(%s, %s)", names["PHDRS", i], vals["PHDRS", i]
		if (i < count["PHDRS"]) print " \\"
		else print ""
	}

	print ""
	print "/* ELF Program Header Flags */"
	print "#define ELF_PHDR_FLAGS(X) \\"
	for (i = 1; i <= count["PHDR_FLAGS"]; i++) {
		printf "\tX(%s, %s)", names["PHDR_FLAGS", i], vals["PHDR_FLAGS", i]
		if (i < count["PHDR_FLAGS"]) print " \\"
		else print ""
	}

	print ""
	print "/* ELF Section Types */"
	print "#define ELF_SECTIONS(X) \\"
	for (i = 1; i <= count["SECTIONS"]; i++) {
		printf "\tX(%s, %s)", names["SECTIONS", i], vals["SECTIONS", i]
		if (i < count["SECTIONS"]) print " \\"
		else print ""
	}

	print ""
	print "/* ELF Section Flags */"
	print "#define ELF_SECTION_FLAGS(X) \\"
	for (i = 1; i <= count["SECTION_FLAGS"]; i++) {
		printf "\tX(%s, %s)", names["SECTION_FLAGS", i], vals["SECTION_FLAGS", i]
		if (i < count["SECTION_FLAGS"]) print " \\"
		else print ""
	}

	print ""
	print "/* ELF Symbol Info */"
	print "#define ELF_SYMBOLS(X) \\"
	for (i = 1; i <= count["SYMBOLS"]; i++) {
		printf "\tX(%s, %s)", names["SYMBOLS", i], vals["SYMBOLS", i]
		if (i < count["SYMBOLS"]) print " \\"
		else print ""
	}

	print ""
	print "/* ELF Dynamic Tags */"
	print "#define ELF_DYNAMIC(X) \\"
	for (i = 1; i <= count["DYNAMIC"]; i++) {
		printf "\tX(%s, %s)", names["DYNAMIC", i], vals["DYNAMIC", i]
		if (i < count["DYNAMIC"]) print " \\"
		else print ""
	}

	print ""
}

