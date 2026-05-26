#!/usr/bin/awk -f
# NetBSD syscalls.master → X(SYS_name, num) converter

# ============================================================================
# PREPROCESSING: Join backslash-continued lines
# ============================================================================
{
	# Strip trailing whitespace
	sub(/[[:space:]]+$/, "")

	# If line ends with \, strip the \, save to buffer, and read next line
	if (sub(/\\$/, "")) {
		buf = buf $0 " "
		next
	}

	# No backslash: prepend buffer to current line and clear buffer
	if (buf != "") {
		$0 = buf $0
		buf = ""
	}
}

# ============================================================================
# FILTERING: Skip comments and blank lines
# ============================================================================
/^[[:space:]]*#/ { next }
/^[[:space:]]*$/ { next }

# ============================================================================
# PARSING: Handle multi-line { } blocks
# ============================================================================
{
	if (inblock) {
		block = block " " $0
		if ($0 ~ /}[[:space:]]*(;.*)?$/) {
			process_block(cur_num, block)
			inblock = 0
			block = ""
		}
		next
	}

	if (/^[0-9]+[[:space:]]/) {
		cur_num = $1
		line = $0
		sub(/^[0-9]+[[:space:]]+/, "", line)

		if (line !~ /\{/) {
			next
		}

		if (line ~ /\{.*\}[[:space:]]*(;.*)?$/) {
			process_block(cur_num, line)
		} else if (line ~ /\{/) {
			inblock = 1
			block = line
		}
	}
}

# ============================================================================
# EXTRACTION: Parse syscall name from block
# ============================================================================
function process_block(num, blk,    m, name, candidate, suffix, parts, n, sparts) {
	# 1. Try to find alias AFTER the closing brace }
	name = ""
	
	n = split(blk, parts, /\}/)
	if (n > 1) {
		suffix = parts[2]
		gsub(/^[[:space:]]+/, "", suffix)
		split(suffix, sparts, /[[:space:]]+/)
		candidate = sparts[1]
		
		if (candidate != "" && candidate !~ /^;/ && candidate != "nosys" && candidate != "__nosys") {
			name = candidate
		}
	}

	# 2. If no alias found, extract from inside { ... }
	if (name == "") {
		if (!match(blk, /([A-Za-z_][A-Za-z0-9_]*)[[:space:]]*\(/, m)) {
			return
		}
		name = m[1]
	}

	# 3. Clean up prefixes
	sub(/^__/, "", name)
	sub(/^sys_/, "", name)

	# 4. Filter noise
	if (name == "" || name == "nosys" || name == "enosys") {
		return
	}

	# 5. Deduplication (keep first occurrence)
	if (name in seen) {
		return
	}
	
	seen[name] = 1
	printf "X(SYS_%s, %d)\n", name, num
}
