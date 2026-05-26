# (c) 2026 FRINKnet & Friends – MIT licence

BEGIN {
	buf = ""
}

# Join backslash-continued lines
{
	while (/\\$/) {
		sub(/\\$/, "")
		sub(/[[:space:]]+$/, "")
		if ((getline nextline) > 0) {
			$0 = $0 " " nextline
		}
	}
}

# Strip C-style comments (multi-line aware)
{
	# Remove single-line // comments
	gsub(/\/\/.*$/, "")

	# Handle /* */ comments
	while (match($0, /\/\*/)) {
		before = substr($0, 1, RSTART - 1)
		$0 = substr($0, RSTART + 2)

		if (match($0, /\*\//)) {
			$0 = before substr($0, RSTART + 2)
		} else {
			$0 = before
			while ((getline nextline) > 0) {
				if (match(nextline, /\*\//)) {
					$0 = $0 substr(nextline, RSTART + 2)
					break
				}
			}
		}
	}
}

# Skip blank lines
/^[[:space:]]*$/ { next }

# Extract #define NAME VALUE
/^#define[[:space:]]+[A-Z_][A-Z0-9_]*/ {
	match($0, /^#define[[:space:]]+([A-Z_][A-Z0-9_]*)/, m)
	name = m[1]

	# Get value (everything after name)
	val = substr($0, RSTART + RLENGTH)
	gsub(/^[[:space:]]+/, "", val)
	gsub(/[[:space:]]+$/, "", val)
	gsub(/[[:space:]]*\/\*.*\*\/[[:space:]]*/, "", val)

	if (val != "") {
		defs[name] = val
	}
	next
}

function resolve_all() {
	for (iter = 0; iter < 20; iter++) {
		changed = 0

		for (name in defs) {
			val = defs[name]

			# Already resolved?
			if (val ~ /^-?[[:digit:]]+$/ || val ~ /^0[0-7]+$/ || val ~ /^0[xX][[:xdigit:]]+$/) {
				continue
			}

			# Strip parens
			gsub(/[()]/, "", val)

			# Simple reference: FOO -> resolve(FOO)
			if (val ~ /^[A-Z_][A-Z0-9_]*$/ && val in defs) {
				newval = defs[val]
				if (newval ~ /^-?[[:digit:]]+$/ || newval ~ /^0[0-7]+$/ || newval ~ /^0[xX][[:xdigit:]]+$/) {
					defs[name] = newval
					changed = 1
				}
			}

			# BASE + N
			if (match(val, /^([A-Z_][A-Z0-9_]*)[[:space:]]*\+[[:space:]]*([0-9]+)$/, parts)) {
				base = parts[1]
				offset = parts[2]
				if (base in defs && defs[base] ~ /^[0-9]+$/) {
					defs[name] = defs[base] + offset
					changed = 1
				}
			}

			# A | B (bitwise OR)
			if (match(val, /^([A-Z_][A-Z0-9_]*)[[:space:]]*\|[[:space:]]*([A-Z_][A-Z0-9_]*)$/, parts)) {
				left = parts[1]
				right = parts[2]
				if ((left in defs) && (right in defs)) {
					lval = defs[left]
					rval = defs[right]
					if ((lval ~ /^[0-9]+$/ || lval ~ /^0[0-7]+$/ || lval ~ /^0[xX][[:xdigit:]]+$/) &&
						(rval ~ /^[0-9]+$/ || rval ~ /^0[0-7]+$/ || rval ~ /^0[xX][[:xdigit:]]+$/)) {
						defs[name] = or(strtonum(lval), strtonum(rval))
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
		if (val !~ /^-?[[:digit:]]+$/ && val !~ /^0[0-7]+$/ && val !~ /^0[xX][[:xdigit:]]+$/) continue

		if (name ~ /^O_/) o_flags[name] = val
		else if (name ~ /^F_OWNER_/) owner[name] = val
		else if (name ~ /^F_[A-Z]*LCK$/) locks[name] = val
		else if (name ~ /^F_/) f_cmds[name] = val
		else if (name ~ /^FD_/) fd_flags[name] = val
		else if (name ~ /^AT_/) at_flags[name] = val
		else if (name ~ /^POSIX_FADV_/) fadv[name] = val
		else if (name ~ /^SEEK_/) seek[name] = val
		else if (name ~ /^RWH_/) rwh[name] = val
		else if (name ~ /^DN_/) dn[name] = val
		else if (name ~ /^PIDFD_/) pidfd[name] = val
	}
}

function emit(title, macro, arr,	sorted, n, i, trail) {
	if (length(arr) == 0) return
	print "/* " title " */"
	printf "#define %s(X) \\\n", macro
	n = asorti(arr, sorted, "cmp")
	for (i = 1; i <= n; i++) {
		trail = (i == n) ? "" : " \\"
		printf "\tX(%s, %s)%s\n", sorted[i], arr[sorted[i]], trail
	}
	print ""
}

function cmp(i1, v1, i2, v2,	n1, n2) {
	n1 = (v1 ~ /^0x/) ? strtonum(v1) : v1 + 0
	n2 = (v2 ~ /^0x/) ? strtonum(v2) : v2 + 0
	if (n1 < n2) return -1
	if (n1 > n2) return 1
	return (i1 < i2) ? -1 : (i1 > i2)
}

END {
	resolve_all()
	categorize_all()

	# POSIX-mandated constants (not in linux/fcntl.h, defined by the spec)
	if (!("POSIX_FADV_NORMAL" in fadv)) fadv["POSIX_FADV_NORMAL"] = "0"
	if (!("POSIX_FADV_RANDOM" in fadv)) fadv["POSIX_FADV_RANDOM"] = "1"
	if (!("POSIX_FADV_SEQUENTIAL" in fadv)) fadv["POSIX_FADV_SEQUENTIAL"] = "2"
	if (!("POSIX_FADV_WILLNEED" in fadv)) fadv["POSIX_FADV_WILLNEED"] = "3"
	if (!("POSIX_FADV_DONTNEED" in fadv)) fadv["POSIX_FADV_DONTNEED"] = "4"
	if (!("POSIX_FADV_NOREUSE" in fadv)) fadv["POSIX_FADV_NOREUSE"] = "5"

	if (!("SEEK_SET" in seek)) seek["SEEK_SET"] = "0"
	if (!("SEEK_CUR" in seek)) seek["SEEK_CUR"] = "1"
	if (!("SEEK_END" in seek)) seek["SEEK_END"] = "2"

	emit("Open flags", "FCNTL_O_FLAGS", o_flags)
	emit("Fcntl commands", "FCNTL_F_COMMANDS", f_cmds)
	emit("File descriptor flags", "FCNTL_FD_FLAGS", fd_flags)
	emit("Lock types", "FCNTL_LOCK_TYPES", locks)
	emit("Owner types", "FCNTL_OWNER_TYPES", owner)
	emit("AT function flags", "FCNTL_AT_FLAGS", at_flags)
	emit("POSIX advisory flags", "FCNTL_POSIX_FADV", fadv)
	emit("Seek constants", "FCNTL_SEEK", seek)
	emit("Write lifetime hints", "FCNTL_RWH", rwh)
	emit("Directory notifications", "FCNTL_DN", dn)
	emit("PIDFD constants", "FCNTL_PIDFD", pidfd)
}
