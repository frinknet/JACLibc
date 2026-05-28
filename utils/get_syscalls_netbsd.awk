#!/usr/bin/awk -f
# NetBSD syscalls.master → X(SYS_name, num) converter

# ============================================================================
# PREPROCESSING: Join backslash-continued lines
# ============================================================================
{
	sub(/[[:space:]]+$/, "")
	if (sub(/\\$/, "")) {
		buf = buf $0 " "
		next
	}
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
# PARSING: Process single-line blocks (preprocessor already joined them)
# ============================================================================
/^[0-9]+[[:space:]]/ {
	if ($0 ~ /\{/) {
		process_block($1, $0)
	}
}

# ============================================================================
# EXTRACTION: Parse syscall name from block
# ============================================================================
function process_block(num, blk,    name, idx, prefix, suffix) {
	name = ""

	# 1. Check for explicit alias after the LAST closing brace
	if (match(blk, /.*\}[[:space:]]+[A-Za-z_][A-Za-z0-9_]*/)) {
		suffix = substr(blk, RSTART, RLENGTH)
		sub(/.*\}[[:space:]]+/, "", suffix)

		if (suffix != "nosys" && suffix != "__nosys" && suffix !~ /^o[a-z]/) {
			name = suffix
		}
	}

	# 2. If no alias, extract the function name right before the first '('
	if (name == "") {
		idx = index(blk, "(")
		if (idx > 0) {
			prefix = substr(blk, 1, idx - 1)
			sub(/.*[^A-Za-z0-9_]/, "", prefix)
			name = prefix
		}
	}

	# 3. Clean up internal prefixes
	sub(/^__/, "", name)
	sub(/^sys_/, "", name)

	# 4. Filter noise
	if (name == "" || name == "nosys" || name == "enosys" || name ~ /^[0-9]/) {
		return
	}

	# 5. Deduplication
	if (name in seen) return
	seen[name] = 1

	printf "X(SYS_%s, %d)\n", name, num
}
