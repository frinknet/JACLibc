# Converts Linux syscall.tbl to X(SYS_name, num, name)
# Filters by architecture and handles ABI aliases

BEGIN {
    # Define mapping from internal arch names to syscall.tbl ABI names
    if (arch == "x64") {
        valid_abi["64"] = 1
        valid_abi["common"] = 1
    } else if (arch == "x86") {
        valid_abi["i386"] = 1
        valid_abi["common"] = 1
    } else if (arch == "arm64") {
        valid_abi["64"] = 1
        valid_abi["common"] = 1
    } else if (arch == "arm32") {
        valid_abi["eabi"] = 1
        valid_abi["common"] = 1
    } else if (arch == "mips32") {
        valid_abi["n32"] = 1
        valid_abi["common"] = 1
    } else if (arch == "mips64") {
        valid_abi["n64"] = 1
        valid_abi["common"] = 1
    } else if (arch == "sparc32") {
        valid_abi["32"] = 1
        valid_abi["common"] = 1
    } else if (arch == "sparc64") {
        valid_abi["64"] = 1
        valid_abi["common"] = 1
    } else if (arch == "powerpc32") {
        valid_abi["32"] = 1
        valid_abi["common"] = 1
        valid_abi["nospu"] = 1
    } else if (arch == "powerpc64") {
        valid_abi["64"] = 1
        valid_abi["common"] = 1
        valid_abi["nospu"] = 1
    } else if (arch == "spu") {
        valid_abi["32"] = 1
        valid_abi["spu"] = 1
        valid_abi["common"] = 1
    } else {
        # Default: accept common and anything matching arch number
        valid_abi["common"] = 1
        valid_abi[arch] = 1
        valid_abi[64] = 1
    }
}

/^[[:space:]]*#/ { next }
/^[[:space:]]*$/ { next }

# Format: number abi name entry [native]
/^[0-9]+[[:space:]]/ {
    nr   = $1
    abi  = $2
    name = $3

    # 1. Filter by ABI
    if (!(abi in valid_abi)) {
        next
    }

    # 2. Skip empty or placeholder entries
    if (name == "" || name ~ /^sys_/) {
        # Sometimes 'name' column is empty and 'entry' has sys_name
        # But usually column 3 is the standard name.
        # If column 3 is empty, try column 4?
        if ($3 == "") name = $4
        else if ($3 ~ /^sys_/) name = $3 # Fallback
    }

    # Clean up name
    sub(/^sys_/, "", name)

    if (name == "") next

    # 3. Linux Deduplication Logic
    # On 64-bit systems, *64 syscalls are often aliases to the non-64 version.
    # We prefer the NON-64 name for cleanliness if they map to the same number logic,
    # BUT in the table they might have different numbers or be the same.
    # Actually, in the table, `fstat` and `fstat64` often have DIFFERENT numbers for compat.
    # The duplicate warning you saw implies the SAME number had two names?
    # Ah, looking at your log: "duplicate "222" in [mmap2, mmap]"
    # This means for a specific number, multiple ABIs provided different names.
    # Since we filtered by ABI, this shouldn't happen unless 'common' overlaps.

    # Fix: If we see a name ending in '64' and we already have the non-64 version
    # (or vice versa) for the SAME number? No, numbers are unique per ABI.
    # The issue is likely that `common` applies to ALL archs, so it gets added multiple times?
    # No, AWK processes line by line.

    # Real Issue: Your log shows "duplicate "222" in [mmap2, mmap]".
    # This means line A said "222 common mmap ..." and line B said "222 i386 mmap2 ..."?
    # If we are on x64, we accept 'common' and '64'. We reject 'i386'.
    # So why did 'mmap2' get in? Maybe 'mmap2' is also 'common'?
    # If both are 'common', then the table has two lines for the same number? That's invalid.

    # Most likely: You are parsing a merged table where numbers overlap across ABIs.
    # Solution: Keep track of seen NUMBERS. If we see a number again, skip it (prefer first occurrence).
    # First occurrence is usually the native/common one.

    if (nr in seen_num) {
        next
    }
    seen_num[nr] = 1

    # Optional: Prefer modern names?
    # If name is "fstat64" and we are on 64-bit, maybe rename to "fstat"?
    # Let's just output unique numbers.

    printf "X(SYS_%s, %d, %s)\n", name, nr, name
}
