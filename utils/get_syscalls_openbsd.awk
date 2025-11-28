# Input:  OpenBSD syscalls.master
# Output: X(SYS_name, num, name, rettype, (param_types...), castargs...)

# Skip comments and blank lines
/^[[:space:]]*#/ { next }
/^[[:space:]]*$/ { next }

# Match syscall entries: number at start
/^[0-9]+[[:space:]]/ {
	num  = $1
	line = $0

	# Grab text between { and }
	if (!match(line, /\{([^}]*)\}/, blk)) next
	sig = blk[1]

	# Clean up
	gsub(/^[[:space:]]+|[[:space:]]+$/, "", sig)
	gsub(/;[[:space:]]*$/, "", sig)

	# Parse "rettype sys_name(params)"
	if (!match(sig, /^([^(]+[[:space:]]+)([A-Za-z_][A-Za-z0-9_]*)[[:space:]]*\(([^)]*)\)/, m))
		next

	rettype = m[1]
	rawname = m[2]
	params  = m[3]

	gsub(/^[[:space:]]+|[[:space:]]+$/, "", rettype)
	gsub(/^[[:space:]]+|[[:space:]]+$/, "", rawname)
	gsub(/^[[:space:]]+|[[:space:]]+$/, "", params)

	# Strip sys_ prefix
	name = rawname
	sub(/^sys_/, "", name)

	# Build param type list: (type0, type1, ...)
	param_types = ""
	castargs    = ""

	if (params != "" && params != "void") {
		n = split(params, parr, /,[[:space:]]*/)
		argi = 0
		for (i = 1; i <= n; i++) {
			p = parr[i]
			gsub(/^[[:space:]]+|[[:space:]]+$/, "", p)

			if (p == "void" || p == "...") continue

			# Make '*' its own token so "void *buf" -> "void * buf"
			gsub(/\*/, " * ", p)

			nt = split(p, tok, /[[:space:]]+/)
			if (nt < 2) continue      # need at least type + name

			# type is everything except the last token (the name)
			ptype = ""
			for (j = 1; j < nt; j++) {
				if (ptype != "") ptype = ptype " "
				ptype = ptype tok[j]
			}

			# append to param_types
			if (param_types != "")
				param_types = param_types ", "
			param_types = param_types ptype

			# append to castargs
			if (castargs != "")
				castargs = castargs ", "
			castargs = castargs "(" ptype ")a" argi

			argi++
		}
	}

	if (param_types == "")
		param_types = "void"

	# Print X line
	if (castargs == "")
		printf "X(SYS_%s, %d, %s, %s, (%s))\n",
		       name, num, name, rettype, param_types
	else
		printf "X(SYS_%s, %d, %s, %s, (%s), %s)\n",
		       name, num, name, rettype, param_types, castargs
}
