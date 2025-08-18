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

#ifdef __cplusplus
}
#endif

#endif /* STDDEF_H */
