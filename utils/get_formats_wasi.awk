#!/usr/bin/awk -f

BEGIN {
	print "/* WASI errno values */"
	print "#define WASI_ERRNO(X) \\"
}

/^\s*\(@interface func \(export/ {
	in_func = 1
}

/^\s*\(typename \$errno/ {
	in_errno = 1
	next
}

in_errno && /^\s*\(enum/ {
	in_enum = 1
	next
}

in_enum && /^\s*\(@name/ {
	name = $0
	gsub(/.*"/, "", name)
	gsub(/".*/, "", name)
	next
}

in_enum && /^\s*u16/ {
	val = $0
	gsub(/[^0-9]/, "", val)
	if (name && val != "") {
		printf "\tX(WASI_%s, %s) \\\n", toupper(name), val
		name = ""
		val = ""
	}
}

/^\s*\)\)/ {
	if (in_enum) {
		in_enum = 0
		in_errno = 0
	}
}

/^\s*\(typename \$filetype/ {
	if (!seen_filetype) {
		print ""
		print "/* WASI file types */"
		print "#define WASI_FILETYPE(X) \\"
		seen_filetype = 1
	}
	in_filetype = 1
	next
}

in_filetype && /^\s*\(@name/ {
	name = $0
	gsub(/.*"/, "", name)
	gsub(/".*/, "", name)
	next
}

in_filetype && /^\s*u8/ {
	val = $0
	gsub(/[^0-9]/, "", val)
	if (name && val != "") {
		printf "\tX(WASI_FILETYPE_%s, %s) \\\n", toupper(name), val
		name = ""
		val = ""
	}
}

END {
	print ""
}

