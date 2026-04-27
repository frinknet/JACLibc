/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _STDDEF_H
#define _STDDEF_H
#pragma once

#include <config.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================= */
/* Standard Types                                                */
/* ============================================================= */

/* size_t - unsigned size type */
#if JACL_64BIT
	#if defined(_WIN64) || defined(__LLP64__)
		typedef unsigned long long size_t;
	#else
		typedef unsigned long size_t;
	#endif
#else
	typedef unsigned int size_t;
#endif

/* ptrdiff_t - pointer difference type */
#if JACL_64BIT
	#if defined(_WIN64) || defined(__LLP64__)
		typedef long long ptrdiff_t;
	#else
		typedef long ptrdiff_t;
	#endif
#elif JACL_32BIT
	typedef int ptrdiff_t;
#elif JACL_16BIT
	typedef short ptrdiff_t;
#elif JACL_8BIT
	typedef signed char ptrdiff_t;
#endif

/* wchar_t - wide character type */
#if defined(__WCHAR_TYPE__)
	typedef __WCHAR_TYPE__ wchar_t;
#elif JACL_OS_WINDOWS
	typedef unsigned short wchar_t;
#else
	typedef unsigned int wchar_t;
#endif

/* wint_t - wide integer type */
#if defined(__WINT_TYPE__)
	typedef __WINT_TYPE__ wint_t;
#elif JACL_OS_WINDOWS
	typedef unsigned short wint_t;
#else
	typedef unsigned int wint_t;
#endif

/**
 * NOTE: These moved to <config.h> in the Polyfill Section.
 *
 * - NULL
 * - offsetof()
 *
 * This is so JACLLibc can work smoothely with modern code.
 */

/* Maximum alignment type (C11 requirement) */
#if JACL_HAS_C11
	typedef struct {
		long long _ll;
		long double _ld;
		void *_ptr;
	} max_align_t;
#endif

/* Feature test macro normalization */
#if defined(_ALL_SOURCE) && !defined(_GNU_SOURCE)
	#define _GNU_SOURCE 1
#endif

#if defined(_DEFAULT_SOURCE) && !defined(_BSD_SOURCE)
	#define _BSD_SOURCE 1
#endif

/* Default feature exposure for non-strict environments */
#if !defined(_POSIX_SOURCE) && !defined(_POSIX_C_SOURCE) \
		&& !defined(_XOPEN_SOURCE) && !defined(_GNU_SOURCE) \
		&& !defined(_BSD_SOURCE) && !defined(__STRICT_ANSI__)
	#define _BSD_SOURCE 1
	#define _XOPEN_SOURCE 700
#endif

#ifdef __cplusplus
}
#endif
#endif /* _STDDEF_H */
