// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef _LIMITS_H
#define _LIMITS_H

// — Character type limits —
#define CHAR_BIT				8
#define SCHAR_MIN				(-128)
#define SCHAR_MAX				127
#define UCHAR_MAX				255U
#define CHAR_MIN				SCHAR_MIN				 // Assuming signed char
#define CHAR_MAX				SCHAR_MAX
#define MB_LEN_MAX			16							 // Maximum multibyte character length

// — Short integer limits —
#define SHRT_MIN				(-32768)
#define SHRT_MAX				32767
#define USHRT_MAX				65535U

// — Integer limits —
#define INT_MIN					(-2147483647-1)
#define INT_MAX					2147483647
#define UINT_MAX				4294967295U

// — Long integer limits —
#define LONG_MIN				(-2147483647L-1)
#define LONG_MAX				2147483647L
#define ULONG_MAX				4294967295UL

// — Long long integer limits (C99+) —
#define LLONG_MIN				(-9223372036854775807LL-1)
#define LLONG_MAX				9223372036854775807LL
#define ULLONG_MAX			18446744073709551615ULL

// — Wide character limits (with guards to avoid conflicts) —
#ifndef WCHAR_MIN
#define WCHAR_MIN				0
#endif
#ifndef WCHAR_MAX
#define WCHAR_MAX				0x10FFFF
#endif

#endif /* _LIMITS_H */
