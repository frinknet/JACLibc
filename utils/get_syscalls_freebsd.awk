# Converts FreeBSD/NetBSD syscalls.master to X(SYS_name, num, name)
# Handles compat aliases and deduplication

BEGIN {
    # Track seen names to avoid duplicates
}

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
    # Example: { int accept(...) } oaccept accept_args int
    name = ""
    if (match(blk, /\}[[:space:]]+([A-Za-z_][A-Za-z0-9_]*)/, m)) {
        candidate = m[1]
        # Validate candidate isn't noise
        if (candidate != "nosys" && candidate != "__nosys") {
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

    # 5. FreeBSD Deduplication Logic
    # If we have already seen this name, skip this entry (it's likely an older compat version)
    # Exception: Allow specific known pairs if needed, but generally first occurrence (lowest number) is modern.
    # Actually, in FreeBSD master file, MODERN entries usually come LATER (higher numbers) for some, 
    # but COMPAT entries often have prefixes like 'o' or 'freebsd4_'.
    
    # Strategy: If the name looks like a compat wrapper (starts with 'o', 'freebsd', 'compat'),
    # we might want to skip it IN FAVOR of the clean name.
    # BUT, our extraction logic above pulls the ALIAS if present.
    # So if line is: { ... } oaccept ..., name becomes 'oaccept'.
    # If line is: { ... } accept ..., name becomes 'accept'.
    
    # To get UNIQUE clean names:
    # If name starts with 'o' AND the substring after 'o' exists in seen[], SKIP this 'o' version.
    if (name ~ /^o[a-z]/) {
        clean_name = substr(name, 2)
        if (clean_name in seen) {
            return # Skip old version, we have the new one
        }
        # If we don't have the new one yet, keep the old one as fallback? 
        # Better: Just store it. But let's prefer non-'o' names.
        # Let's just track all names. If we see 'accept' later, it will overwrite or be added.
        # The user wants NO duplicates in the output list.
    }
    
    # Simpler Approach for Clean List:
    # Only print if we haven't printed this exact name before.
    if (name in seen) {
        return
    }
    
    # Special handling: If this name is a compat variant (e.g. freebsd4_sigaction),
    # maybe we want to map it? No, let's just list unique strings.
    # The duplicates in your log are because 30 and 99 BOTH resolved to "accept".
    # This means the alias extraction didn't catch "oaccept" for #99.
    # Let's check the source again:
    # 99 AUE_ACCEPT COMPAT|CAPENABLED { int accept(...) } oaccept
    # Our regex /\}[[:space:]]+([A-Za-z_...])/ should catch "oaccept".
    # If it didn't, maybe there's no space? Or the format is slightly different.
    
    # Refined Regex for Alias:
    # Split the part after '}' by space. First token is alias.
    split(blk, parts, /\}/)
    if (length(parts) > 1) {
        suffix = parts[2]
        gsub(/^[[:space:]]+/, "", suffix)
        split(suffix, sparts, /[[:space:]]+/)
        if (sparts[1] != "" && sparts[1] !~ /^;/) {
             # Found an alias! Use it instead of the function name inside {}
             # This prevents 'accept' collision. #30 has no alias -> 'accept'. #99 has 'oaccept'.
             # Wait, if #99 uses 'oaccept', then name='oaccept', which is unique!
             # Why did you get duplicate 'accept'?
             # Ah, maybe the alias is NOT being picked up by the current regex.
             # Let's force the alias pickup.
             name = sparts[1]
             sub(/^__/, "", name)
             sub(/^sys_/, "", name)
        }
    }
    
    # Final Check: If still duplicate, skip.
    if (name in seen) {
        return
    }
    
    seen[name] = 1
    printf "X(SYS_%s, %d, %s)\n", name, num, name
}
