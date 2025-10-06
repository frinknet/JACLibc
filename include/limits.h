/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef LIMITS_H
#define LIMITS_H
#pragma once

#include <config.h>

// — Character type limits —
#define CHAR_BIT				8
#define SCHAR_MIN				(-128)
#define SCHAR_MAX				127
#define UCHAR_MAX				255U
#define MB_LEN_MAX			16

#if defined(__CHAR_UNSIGNED__) || defined(_CHAR_UNSIGNED)
  #define CHAR_MIN 0
  #define CHAR_MAX UCHAR_MAX
#else
  #define CHAR_MIN SCHAR_MIN
  #define CHAR_MAX SCHAR_MAX
#endif


// — Short integer limits —
#define SHRT_MIN				(-32768)
#define SHRT_MAX				32767
#define USHRT_MAX				65535U

// — Integer limits —
#define INT_MIN					(-2147483647-1)
#define INT_MAX					2147483647
#define UINT_MAX				4294967295U

// — Long integer limits —
#ifdef __LP64__
  #define LONG_MIN  (-9223372036854775807L-1)
  #define LONG_MAX  9223372036854775807L
  #define ULONG_MAX 18446744073709551615UL
#else
  #define LONG_MIN  (-2147483647L-1)
  #define LONG_MAX  2147483647L
  #define ULONG_MAX 4294967295UL
#endif

// — Long long integer limits (C99+) —
#if JACL_HAS_C99
	#define LLONG_MIN   (-9223372036854775807LL-1)
	#define LLONG_MAX   9223372036854775807LL
	#define ULLONG_MAX  18446744073709551615ULL
#endif

#endif // LIMITS_H
