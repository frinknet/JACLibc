/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STDDEF_H
#define STDDEF_H
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
	typedef unsigned long size_t;
#else
	typedef unsigned int size_t;
#endif

/* ssize_t - signed size type (POSIX) */
#if JACL_64BIT
	typedef long ssize_t;
#else
	typedef int ssize_t;
#endif

/* ptrdiff_t - pointer difference type */
#if JACL_64BIT
	typedef long ptrdiff_t;
#elif JACL_32BIT
	typedef int ptrdiff_t;
#elif JACL_16BIT
	typedef short ptrdiff_t;
#elif JACL_8BIT
	typedef char ptrdiff_t;
#endif

/* wchar_t - wide character type */
#ifndef __cplusplus
	typedef int wchar_t;
#endif

/* NULL pointer constant */
#ifndef NULL
	#ifdef __cplusplus
		#define NULL nullptr
	#else
		#define NULL ((void*)0)
	#endif
#endif

/* Offset of member in a struct (portable, builtin-free) */
#ifndef offsetof
	#define offsetof(type, member) ((size_t)&(((type*)0)->member))
#endif

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

/* Symbol redirection helper */
#define __REDIR(name, proto, alias) static inline __typeof__(proto) name { return alias; }

#ifdef __cplusplus
}
#endif
#endif /* STDDEF_H */
