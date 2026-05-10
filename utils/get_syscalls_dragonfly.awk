# Parses DragonFlyBSD syscalls.master
# Handles aliases, extpread, and deduplication of syscall #0/#198

/^[[:space:]]*#/ { next }
/^[[:space:]]*$/ { next }

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

function process_block(num, blk,    m, name, suffix, parts, n, candidate) {
	# 1. Try to find alias AFTER the closing brace }
	# Format: { ... } alias_name args_type return_type
	name = ""
	if (match(blk, /\}[[:space:]]+([A-Za-z_][A-Za-z0-9_]*)/, m)) {
		candidate = m[1]
		# Only use alias if it's not 'nosys' or generic
		if (candidate != "nosys" && candidate != "__nosys" && candidate != "lkmnosys") {
			name = candidate
		}
	}

	# 2. If no valid alias found, extract from inside { ... }
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
	if (name == "" || name == "nosys" || name == "enosys" || name == "lkmnosys") {
		return
	}

	# 5. DragonFly Specific Deduplication
	# Syscall 0 is 'syscall'. Syscall 198 is '__syscall' which often aliases to 'syscall'.
	# We keep #0 and skip #198 to avoid duplicate entries in the final list.
	if (num == 198 && name == "syscall") {
		return
	}

	# 6. Output
	printf "X(SYS_%s, %d, %s)\n", name, num, name
}
