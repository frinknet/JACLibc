// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef LIMITS_H
#define LIMITS_H

// — Exact-width integer limits (from <stdint.h>) —
#define INT8_MIN		(-128)
#define INT8_MAX		127
#define UINT8_MAX		255

#define INT16_MIN		(-32768)
#define INT16_MAX		32767
#define UINT16_MAX	65535U

#define INT32_MIN		(-2147483647-1)
#define INT32_MAX		2147483647
#define UINT32_MAX	4294967295U

#define INT64_MIN		(-9223372036854775807LL-1)
#define INT64_MAX		9223372036854775807LL
#define UINT64_MAX	18446744073709551615ULL

// — Pointer-sized integer limits —
#define PTRDIFF_MIN INT32_MIN
#define PTRDIFF_MAX INT32_MAX
#define SIZE_MAX		UINT32_MAX
#define WCHAR_MAX		0x10FFFF
#define WCHAR_MIN		0

// — Limits of other types —
#define CHAR_BIT		8
#define CHAR_MIN		SCHAR_MIN
#define CHAR_MAX		SCHAR_MAX

#define SCHAR_MIN		INT8_MIN
#define SCHAR_MAX		INT8_MAX
#define UCHAR_MAX		UINT8_MAX

#define SHRT_MIN		INT16_MIN
#define SHRT_MAX		INT16_MAX
#define USHRT_MAX		UINT16_MAX

#define INT_MIN			INT32_MIN
#define INT_MAX			INT32_MAX
#define UINT_MAX		UINT32_MAX

#define LONG_MIN		INT32_MIN
#define LONG_MAX		INT32_MAX
#define ULONG_MAX		UINT32_MAX

#define LLONG_MIN		INT64_MIN
#define LLONG_MAX		INT64_MAX
#define ULLONG_MAX	UINT64_MAX

// — Multibyte and wide char —
#define MB_LEN_MAX	1

#endif /* LIMITS_H */
