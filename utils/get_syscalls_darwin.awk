# Darwin Syscall Parser

BEGIN {
    # Nothing to initialize
}

# Skip pure comments, blank lines, and preprocessor directives
/^[[:space:]]*$/ { next }
/^[[:space:]]*;/ { next }
/^[[:space:]]*#/ { next }

# Track multi-line blocks
{
    # If we are inside a block, accumulate
    if (inblock) {
        block = block " " $0
        # Check for end of block
        if ($0 ~ /}[[:space:]]*(;.*)?$/) {
            process_block(cur_num, block)
            inblock = 0
            block = ""
        }
        next
    }

    # Look for start of a syscall entry: Line starts with Number
    if (/^[0-9]+[[:space:]]/) {
        cur_num = $1

        # Remove the number part to analyze the rest
        line = $0
        sub(/^[0-9]+[[:space:]]+/, "", line)

        # Skip if no prototype block
        if (line !~ /\{/) {
            next
        }

        # Check if single-line block
        if (line ~ /\{.*\}[[:space:]]*(;.*)?$/) {
            process_block(cur_num, line)
        } else if (line ~ /\{/) {
            # Start of multi-line
            inblock = 1
            block = line
        }
    }
}

function process_block(num, blk,    name, tmp, content, paren_pos, prefix, n, parts) {
    # Isolate content inside {}
    if (!match(blk, /\{([^}]*)\}/, tmp)) {
        return
    }
    content = tmp[1]

    # Find the opening paren
    paren_pos = index(content, "(")
    if (paren_pos == 0) return

    # Look backwards from '(' to find the name
    prefix = substr(content, 1, paren_pos - 1)

    # Extract the last word from prefix
    n = split(prefix, parts, /[[:space:]]+/)
    if (n < 1) return

    name = parts[n]

    # Clean up name
    gsub(/[^A-Za-z0-9_]/, "", name)

    # Strip prefixes
    sub(/^__/, "", name)
    sub(/^sys_/, "", name)

    # Filter out noise
    if (name == "" || name == "nosys" || name == "enosys") {
        # If it's a stub, check if we already have a real one for this number
        # If not, store it as a placeholder (though we won't print it later)
        if (!(num in syscalls)) {
            syscalls[num] = "" # Mark as seen but empty
        }
        return
    }

    # Store/Overwrite: Real implementations always win over empty strings
    syscalls[num] = name
    numbers[num] = num
}

END {
    # Sort and print only the valid ones
    n = asorti(numbers, sorted_nums, "@ind_num_asc")
    for (i = 1; i <= n; i++) {
        num = sorted_nums[i]
        name = syscalls[num]
        if (name != "") {
            printf "X(SYS_%s, %d, %s)\n", name, num, name
        }
    }
}
