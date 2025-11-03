/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef LIMITS_H
#define LIMITS_H
#pragma once

#include <config.h>

/* ============================================================= */
/* Character Type Limits                                         */
/* ============================================================= */

#define CHAR_BIT    8
#define SCHAR_MIN   (-128)
#define SCHAR_MAX   127
#define UCHAR_MAX   255U
#define MB_LEN_MAX  16

#if defined(__CHAR_UNSIGNED__) || defined(_CHAR_UNSIGNED)
#define CHAR_MIN    0
#define CHAR_MAX    UCHAR_MAX
#else
#define CHAR_MIN    SCHAR_MIN
#define CHAR_MAX    SCHAR_MAX
#endif

/* ============================================================= */
/* Short Integer Limits                                          */
/* ============================================================= */

#define SHRT_MIN    (-32768)
#define SHRT_MAX    32767
#define USHRT_MAX   65535U

/* ============================================================= */
/* Integer Limits                                                */
/* ============================================================= */

#define INT_MIN     (-2147483647-1)
#define INT_MAX     2147483647
#define UINT_MAX    4294967295U

/* ============================================================= */
/* Long Integer Limits                                           */
/* ============================================================= */

#ifdef __LP64__
#define LONG_MIN    (-9223372036854775807L-1)
#define LONG_MAX    9223372036854775807L
#define ULONG_MAX   18446744073709551615UL
#else
#define LONG_MIN    (-2147483647L-1)
#define LONG_MAX    2147483647L
#define ULONG_MAX   4294967295UL
#endif

/* ============================================================= */
/* Long Long Integer Limits (C99)                                */
/* ============================================================= */

#if JACL_HAS_C99
#define LLONG_MIN   (-9223372036854775807LL-1)
#define LLONG_MAX   9223372036854775807LL
#define ULLONG_MAX  18446744073709551615ULL
#endif

/* ============================================================= */
/* File System Limits (POSIX)                                    */
/* ============================================================= */

#if JACL_HAS_POSIX

#ifndef NAME_MAX
#define NAME_MAX 255
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#ifndef FILENAME_MAX
#define FILENAME_MAX PATH_MAX
#endif

#endif  /* JACL_HAS_POSIX */

#endif /* LIMITS_H */
