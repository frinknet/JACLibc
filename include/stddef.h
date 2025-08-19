// (c) 2025 FRINKnet & Friends - MIT licence
#ifndef STDDEF_H
#define STDDEF_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Fundamental types for WASM 32-bit */
typedef uint32_t size_t;
typedef int32_t ptrdiff_t;

/* NULL pointer constant */
#ifndef NULL
# ifdef __cplusplus
#  define NULL 0
# else
#  define NULL ((void*)0)
# endif
#endif


/* Offset of member in a struct */
#define offsetof(type, member) __builtin_offsetof(type, member)

/* Maximum alignment for any type (C11 expects a typedef) */
typedef struct { long double _; } max_align_t;

#if defined(_ALL_SOURCE) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE 1
#endif

#if defined(_DEFAULT_SOURCE) && !defined(_BSD_SOURCE)
#define _BSD_SOURCE 1
#endif

#if !defined(_POSIX_SOURCE) && !defined(_POSIX_C_SOURCE) \
	&& !defined(_XOPEN_SOURCE) && !defined(_GNU_SOURCE) \
	&& !defined(_BSD_SOURCE) && !defined(__STRICT_ANSI__)
	#define _BSD_SOURCE 1
#define _XOPEN_SOURCE 700
#endif

#if __STDC_VERSION__ >= 199901L
#define __restrict restrict
#elif !defined(__GNUC__)
#define __restrict
#endif

#if __STDC_VERSION__ >= 199901L || defined(__cplusplus)
#define __inline inline
#elif !defined(__GNUC__)
#define __inline
#endif

#if __STDC_VERSION__ >= 201112L
#elif defined(__GNUC__)
#define _Noreturn __attribute__((__noreturn__))
#else
#define _Noreturn
#endif

#define __REDIR(x,y) __typeof__(x) x __asm__(#y)

#ifdef __cplusplus
}
#endif

#endif /* STDDEF_H */
