/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef LIMITS_H
#define LIMITS_H
#pragma once

#include <config.h>

/* ============================================================= */
/* Type Bit-Width Definitions                                    */
/* ============================================================= */

#ifndef CHAR_BIT
	#ifdef __CHAR_BIT__
		#define CHAR_BIT __CHAR_BIT__
	#else
		#define CHAR_BIT 8
	#endif
#endif

#ifndef SCHAR_BIT
	#define SCHAR_BIT   CHAR_BIT
#endif

#ifndef SHRT_BIT
	#ifdef __SIZEOF_SHORT__
  	#define SHRT_BIT (__SIZEOF_SHORT__ * CHAR_BIT)
	#elif defined(JACL_8BIT)
		#define SHRT_BIT  (1 * CHAR_BIT)
	#else
		#define SHRT_BIT  (2 * CHAR_BIT)
	#endif
#endif

#ifndef INT_BIT
	#ifdef __SIZEOF_INT__
  	#define INT_BIT (__SIZEOF_INT__ * CHAR_BIT)
	#elif defined(JACL_16BIT)
		#define INT_BIT    (2 * CHAR_BIT)
	#elif defined(JACL_8BIT)
		#define INT_BIT    (1 * CHAR_BIT)
	#else
		#define INT_BIT    (4 * CHAR_BIT)
	#endif
#endif

#ifndef LONG_BIT
	#ifdef __SIZEOF_LONG__
  	#define LONG_BIT (__SIZEOF_LONG__ * CHAR_BIT)
	#elif defined(__LP64__) || defined(JACL_64BIT)
		#define LONG_BIT   (8 * CHAR_BIT)
	#else
		#define LONG_BIT   (4 * CHAR_BIT)
	#endif
#endif

#ifndef LLONG_BIT
	#ifdef __SIZEOF_LONG_LONG__
  	#define LLONG_BIT (__SIZEOF_LONG_LONG__ * CHAR_BIT)
	#elif JACL_HAS_C99
		#define LLONG_BIT  (8 * CHAR_BIT)
	#endif
#endif

#ifndef MB_LEN_MAX
	#define MB_LEN_MAX  16
#endif

/* ============================================================= */
/* Character Type Limits                                         */
/* ============================================================= */

#if defined(__SCHAR_MAX__)
	# define SCHAR_MAX  __SCHAR_MAX__
#else
	# define SCHAR_MAX  127
#endif

#if defined(__SCHAR_MIN__)
	#define SCHAR_MIN   __SCHAR_MIN__
#else
  #define SCHAR_MIN  (-SCHAR_MAX - 1)
#endif

#if defined(__UCHAR_MAX__)
  #define UCHAR_MAX  __UCHAR_MAX__
#else
  #define UCHAR_MAX  (2U * SCHAR_MAX + 1U)
#endif

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

#if defined(__SHRT_MAX__)
	#define SHRT_MAX   __SHRT_MAX__
#elif SHRT_BIT == 8
	#define SHRT_MAX   127
#elif SHRT_BIT == 16
	#define SHRT_MAX   32767
#endif

#if defined(__SHRT_MIN__)
	#define SHRT_MIN   __SHRT_MIN__
#else
  #define SHRT_MIN  (-SHRT_MAX - 1)
#endif

#if defined(__USHRT_MAX__)
  #define USHRT_MAX  __USHRT_MAX__
#else
  #define USHRT_MAX  (2U * SHRT_MAX + 1U)
#endif

/* ============================================================= */
/* Integer Limits                                                */
/* ============================================================= */

#if defined(__INT_MAX__)
	#define INT_MAX   __INT_MAX__
#elif INT_BIT == 8
	#define INT_MAX   127
#elif INT_BIT == 16
	#define INT_MAX   32767
#elif INT_BIT == 32
	#define INT_MAX   2147483647
#endif

#if defined(__INT_MIN__)
	#define INT_MIN   __INT_MIN__
#else
  #define INT_MIN  (-INT_MAX - 1)
#endif

#if defined(__UINT_MAX__)
  #define UINT_MAX  __UINT_MAX__
#else
  #define UINT_MAX  (2U * INT_MAX + 1U)
#endif

/* ============================================================= */
/* Long Integer Limits                                           */
/* ============================================================= */

#if defined(__LONG_MAX__)
	#define LONG_MAX   __LONG_MAX__
#elif LONG_BIT == 64
	#define LONG_MAX   9223372036854775807L
#elif LONG_BIT == 32
	#define LONG_MAX   2147483647L
#elif LONG_BIT == 16
	#define LONG_MAX   32767L
#endif

#if defined(__LONG_MIN__)
	#define LONG_MIN   __LONG_MIN__
#else
  #define LONG_MIN  (-LONG_MAX - 1)
#endif

#if defined(__ULONG_MAX__)
  #define ULONG_MAX  __ULONG_MAX__
#else
  #define ULONG_MAX  (2UL * LONG_MAX + 1UL)
#endif

/* ============================================================= */
/* Long Long Integer Limits (C99)                                */
/* ============================================================= */

#if defined(JACL_HAS_C99) || defined(__LLONG_MAX__)
	#if defined(__LLONG_MAX__)
		#define LLONG_MAX   __LLONG_MAX__
	#elif LLONG_BIT == 64
		#define LLONG_MAX   9223372036854775807LL
	#elif LLONG_BIT == 32
		#define LLONG_MAX   2147483647LL
	#elif LLONG_BIT == 16
		#define LLONG_MAX   32767LL
	#endif

	#if defined(__LLONG_MIN__)
		#define LLONG_MIN   __LLONG_MIN__
	#else
	  #define LLONG_MIN  (-LLONG_MAX - 1)
	#endif

	#if defined(__ULLONG_MAX__)
	  #define ULLONG_MAX  __ULLONG_MAX__
	#else
	  #define ULLONG_MAX  (2ULL * LLONG_MAX + 1ULL)
	#endif
#endif

/* ============================================================= */
/* Signed and Unsigned Size Type                                 */
/* ============================================================= */

#ifndef SIZE_MAX
	#if JACL_64BIT
		#define SIZE_MAX ULONG_MAX
	#else
		#define SIZE_MAX UINT_MAX
	#endif
#endif

#ifndef SSIZE_MAX
	#if JACL_64BIT
		#define SSIZE_MAX LONG_MAX
	#else
		#define SSIZE_MAX INT_MAX
	#endif
#endif

#ifndef SSIZE_MIN
	#if JACL_64BIT
		#define SSIZE_MIN LONG_MIN
	#else
		#define SSIZE_MIN INT_MIN
	#endif
#endif

/* ============================================================= */
/* System Limits                                                 */
/* ============================================================= */

#ifndef NAME_MAX
	#define NAME_MAX 255
#endif

#ifndef PATH_MAX
	#define PATH_MAX 4096
#endif

#ifndef FILENAME_MAX
	#define FILENAME_MAX PATH_MAX
#endif

#ifndef ARG_MAX
	#define ARG_MAX 131072
#endif

#ifndef OPEN_MAX
	#define OPEN_MAX 1024
#endif

#ifndef PIPE_BUF
	#define PIPE_BUF 4096
#endif

#endif /* LIMITS_H */
