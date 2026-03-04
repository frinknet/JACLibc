#!/usr/bin/awk -f

BEGIN {
	o_count = 0
	f_count = 0
	fd_count = 0
	at_count = 0
	fadv_count = 0
	seek_count = 0
	lock_count = 0
	owner_count = 0
}

# Ignore blank and comment lines
/^[[:space:]]*$/ { next }
/^[[:space:]]*\/\*/ { next }
/^[[:space:]]*\*/ { next }

# Handle #ifndef-guarded defines (for F_SETOWN_EX, F_GETOWN_EX, etc.)
/^#ifndef[ \t]+[A-Z_][A-Z0-9_]+/ {
	guard_name = $2

	# Read the next line
	if ((getline nextline) > 0) {
		# Check if it's a #define
		if (nextline ~ /^#define[ \t]+[A-Z_][A-Z0-9_]+/) {
			# Parse the #define line
			split(nextline, tokens)
			name = tokens[2]

			# Value is everything after the name
			val = ""
			for (i = 3; i <= length(tokens); i++) {
				val = val tokens[i]
			}

			# Strip comments
			sub(/\/\*.*\*\//, "", val)
			sub(/\/\/.*/, "", val)
			gsub(/^[ \t]+/, "", val)
			gsub(/[ \t]+$/, "", val)

			# Store it
			defs[name] = val
		}
	}
	next
}

# Collect all #define lines
/^#define[ \t]+[A-Z_][A-Z0-9_]+/ {
	name = $2

	# Value is everything after the name (fields 3 onward concatenated)
	val = ""
	for (i = 3; i <= NF; i++) {
		val = val $i
	}

	# Strip comments
	sub(/\/\*.*\*\//, "", val)
	sub(/\/\/.*/, "", val)
	gsub(/^[ \t]+/, "", val)
	gsub(/[ \t]+$/, "", val)

	# Store everything for later resolution
	defs[name] = val
}

function resolve_all() {
	# Iteratively resolve until stable
	max_iterations = 10
	for (iter = 0; iter < max_iterations; iter++) {
		changed = 0

		for (name in defs) {
			val = defs[name]

			# Already numeric? Skip
			if (val ~ /^-?[[:digit:]]+$/ || val ~ /^0[01234567]+$/ || val ~ /^0[xX][[:xdigit:]]+$/) {
				continue
			}

			# Simple macro reference: VAL -> resolve it
			if (val ~ /^[A-Z_][A-Z0-9_]*$/ && val in defs) {
				newval = defs[val]
				if (newval ~ /^-?[[:digit:]]+$/ || newval ~ /^0[01234567]+$/ || newval ~ /^0[xX][[:xdigit:]]+$/) {
					defs[name] = newval
					changed = 1
				}
			}

			# Handle __MACRO references (internal kernel macros with __ prefix)
			if (val ~ /^__[A-Z_][A-Z0-9_]*$/ && val in defs) {
				newval = defs[val]
				if (newval ~ /^-?[[:digit:]]+$/ || newval ~ /^0[01234567]+$/ || newval ~ /^0[xX][[:xdigit:]]+$/) {
					defs[name] = newval
					changed = 1
				}
			}

			# Handle __O_SYNC|O_DSYNC pattern (bitwise OR of two macros)
			if (val ~ /^\(?__[A-Z_][A-Z0-9_]*\|[A-Z_][A-Z0-9_]*\)?$/) {
				gsub(/[()]/, "", val)
				split(val, parts, /\|/)
				left = parts[1]
				right = parts[2]

				# Try to resolve both sides
				left_val = (left in defs) ? defs[left] : ""
				right_val = (right in defs) ? defs[right] : ""

				# If both are numeric, OR them together
				if (left_val ~ /^[01234567]+$/ && right_val ~ /^[01234567]+$/) {
					# Convert octal to decimal, OR, convert back to octal
					result = or(strtonum("0" left_val), strtonum("0" right_val))
					defs[name] = sprintf("%08o", result)
					changed = 1
				}
			}

			# Arithmetic: (MACRO+N) or MACRO+N (no spaces)
			if (val ~ /^(\()?[A-Z_][A-Z0-9_]*\+[[:digit:]]+(\))?$/) {
				gsub(/[()]/, "", val)
				split(val, parts, /\+/)
				base = parts[1]
				offset = parts[2]

				if (base in defs) {
					baseval = defs[base]
					if (baseval ~ /^[[:digit:]]+$/) {
						defs[name] = baseval + offset + 0
						changed = 1
					}
				}
			}
		}

		if (!changed) break
	}
}

function categorize_all() {
	for (name in defs) {
		val = defs[name]

		# Only categorize resolved numeric values
		if (val !~ /^-?[[:digit:]]+$/ && val !~ /^0[01234567]+$/ && val !~ /^0[xX][[:xdigit:]]+$/) {
			continue
		}

		# Categorize by prefix
		if (name ~ /^O_[A-Z0-9_]+$/) {
			o_flags[name] = val
			o_count++
		} else if (name ~ /^F_OWNER_[A-Z0-9_]+$/) {
			owner[name] = val
			owner_count++
		} else if (name ~ /^F_[A-Z]*LCK$/) {
			locks[name] = val
			lock_count++
		} else if (name ~ /^F_[A-Z0-9_]+$/) {
			f_cmds[name] = val
			f_count++
		} else if (name ~ /^FD_[A-Z0-9_]+$/) {
			fd_flags[name] = val
			fd_count++
		} else if (name ~ /^AT_[A-Z0-9_]+$/) {
			at_flags[name] = val
			at_count++
		} else if (name ~ /^POSIX_FADV_[A-Z0-9_]+$/) {
			fadv[name] = val
			fadv_count++
		} else if (name ~ /^SEEK_[A-Z0-9_]+$/) {
			seek[name] = val
			seek_count++
		}
	}
}

function emit_family(title, macro, arr,    sorted, n, i, trail) {
	if (length(arr) == 0) return
	print "/* " title " */"
	printf "#define %s(X) \\\n", macro
	n = asorti(arr, sorted, "by_value_then_name")
	for (i = 1; i <= n; i++) {
		trail = (i == n) ? "" : " \\"
		printf "\tX(%s, %s)%s\n", sorted[i], arr[sorted[i]], trail
	}
	print ""
}

function by_value_then_name(i1, v1, i2, v2,    n1, n2) {
	if (v1 ~ /^0x/) n1 = strtonum(v1); else n1 = v1 + 0
	if (v2 ~ /^0x/) n2 = strtonum(v2); else n2 = v2 + 0
	if (n1 < n2) return -1
	if (n1 > n2) return 1
	return (i1 < i2) ? -1 : (i1 > i2)
}

END {
	# Resolve all definitions iteratively
	resolve_all()

	# Categorize resolved definitions
	categorize_all()

	# Add fallbacks for missing constants
	if (!("O_RDONLY" in o_flags)) o_flags["O_RDONLY"] = "0"
	if (!("O_WRONLY" in o_flags)) o_flags["O_WRONLY"] = "1"
	if (!("O_RDWR" in o_flags)) o_flags["O_RDWR"] = "2"
	if (!("O_APPEND" in o_flags)) o_flags["O_APPEND"] = "0"
	if (!("O_CREAT" in o_flags)) o_flags["O_CREAT"] = "0"
	if (!("O_TRUNC" in o_flags)) o_flags["O_TRUNC"] = "0"
	if (!("O_EXCL" in o_flags)) o_flags["O_EXCL"] = "0"
	if (!("O_DIRECTORY" in o_flags)) o_flags["O_DIRECTORY"] = "0"
	if (!("O_CLOEXEC" in o_flags)) o_flags["O_CLOEXEC"] = "0"
	if (!("O_NONBLOCK" in o_flags)) o_flags["O_NONBLOCK"] = "0"

	if (!("F_DUPFD" in f_cmds)) f_cmds["F_DUPFD"] = "0"
	if (!("F_GETFD" in f_cmds)) f_cmds["F_GETFD"] = "1"
	if (!("F_SETFD" in f_cmds)) f_cmds["F_SETFD"] = "2"
	if (!("F_GETFL" in f_cmds)) f_cmds["F_GETFL"] = "3"
	if (!("F_SETFL" in f_cmds)) f_cmds["F_SETFL"] = "4"
	if (!("F_DUPFD_CLOEXEC" in f_cmds)) f_cmds["F_DUPFD_CLOEXEC"] = "1030"

	if (!("F_RDLCK" in locks)) locks["F_RDLCK"] = "0"
	if (!("F_WRLCK" in locks)) locks["F_WRLCK"] = "1"
	if (!("F_UNLCK" in locks)) locks["F_UNLCK"] = "2"

	if (!("FD_CLOEXEC" in fd_flags)) fd_flags["FD_CLOEXEC"] = "1"

	if (!("SEEK_SET" in seek)) seek["SEEK_SET"] = "0"
	if (!("SEEK_CUR" in seek)) seek["SEEK_CUR"] = "1"
	if (!("SEEK_END" in seek)) seek["SEEK_END"] = "2"

	if (!("AT_FDCWD" in at_flags)) at_flags["AT_FDCWD"] = "-100"

	if (!("POSIX_FADV_NORMAL" in fadv)) fadv["POSIX_FADV_NORMAL"] = "0"
	if (!("POSIX_FADV_RANDOM" in fadv)) fadv["POSIX_FADV_RANDOM"] = "1"
	if (!("POSIX_FADV_SEQUENTIAL" in fadv)) fadv["POSIX_FADV_SEQUENTIAL"] = "2"
	if (!("POSIX_FADV_WILLNEED" in fadv)) fadv["POSIX_FADV_WILLNEED"] = "3"
	if (!("POSIX_FADV_DONTNEED" in fadv)) fadv["POSIX_FADV_DONTNEED"] = "4"
	if (!("POSIX_FADV_NOREUSE" in fadv)) fadv["POSIX_FADV_NOREUSE"] = "5"

	# Add O_SYNC and O_RSYNC aliases (Linux: both equal O_DSYNC)
	if (!("O_SYNC" in o_flags) && ("O_DSYNC" in o_flags)) {
		o_flags["O_SYNC"] = o_flags["O_DSYNC"]
	}
	if (!("O_RSYNC" in o_flags) && ("O_DSYNC" in o_flags)) {
		o_flags["O_RSYNC"] = o_flags["O_DSYNC"]
	}

	# Emit all families
	emit_family("Open flags", "FCNTL_O_FLAGS", o_flags)
	emit_family("Fcntl commands", "FCNTL_F_COMMANDS", f_cmds)
	emit_family("File descriptor flags", "FCNTL_FD_FLAGS", fd_flags)
	emit_family("Lock types", "FCNTL_LOCK_TYPES", locks)
	emit_family("Owner types", "FCNTL_OWNER_TYPES", owner)
	emit_family("AT function flags", "FCNTL_AT_FLAGS", at_flags)
	emit_family("POSIX advisory flags", "FCNTL_POSIX_FADV", fadv)
	emit_family("Seek constants", "FCNTL_SEEK", seek)
}

