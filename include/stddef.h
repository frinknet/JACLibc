// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef STDDEF_H
#define STDDEF_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Architecture-aware fundamental types */
#if defined(__LP64__) || defined(_WIN64) || defined(__x86_64__) || defined(__aarch64__) || defined(__riscv64)
	typedef unsigned long size_t;
	typedef long ptrdiff_t;
#else
	typedef unsigned int size_t;
	typedef int ptrdiff_t;
#endif

/* Wide character type (required by C standard) */
#ifndef __cplusplus
	#if defined(_WIN32)
		typedef unsigned short wchar_t;  /* Windows uses 16-bit wchar_t */
	#else
		typedef unsigned int wchar_t;		 /* Unix-like systems use 32-bit */
	#endif
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
#if __STDC_VERSION__ >= 201112L
	typedef struct {
		long long _ll;
		long double _ld;
		void *_ptr;
	} max_align_t;
#endif

/* C language feature compatibility */
#if __STDC_VERSION__ >= 199901L
	#define __restrict restrict
#elif defined(__GNUC__) || defined(__clang__)
	#define __restrict __restrict__
#else
	#define __restrict
#endif

#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
	#define __inline inline
#elif defined(__GNUC__) || defined(__clang__)
	#define __inline __inline__
#else
	#define __inline
#endif

#if __STDC_VERSION__ >= 201112L
	/* C11 has native _Noreturn */
#elif defined(__GNUC__) || defined(__clang__)
	#define _Noreturn __attribute__((__noreturn__))
#elif defined(_MSC_VER)
	#define _Noreturn __declspec(noreturn)
#else
	#define _Noreturn
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

/* Symbol redirection helper (GCC/Clang extension) */
#if defined(__GNUC__) || defined(__clang__)
	#define __REDIR(name, proto, alias) \
		__typeof__(proto) name __asm__(#alias)
#else
	#define __REDIR(name, proto, alias) \
		static inline __typeof__(proto) name { return alias; }
#endif

#ifdef __cplusplus
}
#endif
#endif /* STDDEF_H */
