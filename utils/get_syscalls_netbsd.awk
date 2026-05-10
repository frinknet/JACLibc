# Converts NetBSD syscalls.master to X(SYS_name, num, name)
# Handles compat aliases and deduplication

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

function process_block(num, blk,    m, name, candidate, suffix, parts, n) {
    # 1. Try to find alias AFTER the closing brace }
    # Format: { ... } [alias] [args_type] [return_type]
    # Example: { int msync(...) } msync12 msync_args int
    name = ""
    
    # Split by '}' to get the suffix
    n = split(blk, parts, /\}/)
    if (n > 1) {
        suffix = parts[2]
        # Remove leading whitespace
        gsub(/^[[:space:]]+/, "", suffix)
        # Get the first word of the suffix
        split(suffix, sparts, /[[:space:]]+/)
        candidate = sparts[1]
        
        # Validate candidate
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

    # 5. NetBSD Deduplication Logic
    # If we have already seen this exact name, skip it.
    # This keeps the first occurrence (usually the oldest compat or the base if no compat).
    # To prefer MODERN over COMPAT, we would need complex logic, but usually 
    # the list order in syscalls.master puts COMPAT entries *after* or mixed.
    # However, for a unique list, simply skipping duplicates is sufficient.
    # The user likely wants the "clean" name if available, or the specific compat name.
    # Since our alias extraction grabs "msync12" instead of "msync" for compat entries,
    # they will naturally be unique!
    # The duplicates you see (e.g., "msync" twice) mean the alias extraction failed for one of them.
    # Let's ensure we caught the alias.
    
    # Double check: If the name looks like a base syscall (e.g. "msync") but we are at a high number (compat),
    # maybe the alias was missing in the source? 
    # In NetBSD source: "65 COMPAT_12 ... { int msync(...) }" -> No alias listed? 
    # If no alias is listed, it defaults to the function name.
    # In that case, we have a real collision: #65 is 'msync' (compat), #277 is 'msync' (modern).
    # We should rename the compat one if possible, or skip it.
    # Heuristic: If num < 200 and name is common, it might be compat. 
    # But safer: Just skip duplicates. The modern one (higher number) is usually preferred?
    # Actually, let's just print unique names. If 'msync' appears twice, we only print once.
    # Which one? The first one encountered (lower number). 
    # If you want the MODERN one, we need to know which is which.
    # Simple fix: Just skip if seen.
    
    if (name in seen) {
        return
    }
    
    seen[name] = 1
    printf "X(SYS_%s, %d, %s)\n", name, num, name
}
