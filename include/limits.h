// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef LIMITS_H
#define LIMITS_H

// — Pointer-sized integer limits —
#define PTRDIFF_MIN INT32_MIN
#define PTRDIFF_MAX INT32_MAX
#define SIZE_MAX		UINT32_MAX

#ifndef WCHAR_MAX
#define WCHAR_MAX		 0x10FFFF
#endif
#ifndef WCHAR_MIN
#define WCHAR_MIN		 0
#endif


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
