// (c) 2025 FRINKnet & Friends - MIT licence
#ifndef _STDINT_H
#define _STDINT_H

#include <stddef.h>  // For size_t, ptrdiff_t, wchar_t

#ifdef __cplusplus
extern "C" {
#endif

// Fixed-width integer types
typedef signed char					 int8_t;
typedef unsigned char				 uint8_t;
typedef short								 int16_t;
typedef unsigned short			 uint16_t;
typedef int									 int32_t;
typedef unsigned int				 uint32_t;
typedef long long						 int64_t;
typedef unsigned long long	 uint64_t;

// Fastest minimum-width integer types
typedef int8_t							 int_fast8_t;
typedef uint8_t							 uint_fast8_t;
typedef int32_t							 int_fast16_t;
typedef uint32_t						 uint_fast16_t;
typedef int32_t							 int_fast32_t;
typedef uint32_t						 uint_fast32_t;
typedef int64_t							 int_fast64_t;
typedef uint64_t						 uint_fast64_t;

// Smallest integer types with at least specified width
typedef int8_t							 int_least8_t;
typedef uint8_t							 uint_least8_t;
typedef int16_t							 int_least16_t;
typedef uint16_t						 uint_least16_t;
typedef int32_t							 int_least32_t;
typedef uint32_t						 uint_least32_t;
typedef int64_t							 int_least64_t;
typedef uint64_t						 uint_least64_t;

// Integer type capable of holding object pointers
typedef int32_t							 intptr_t;
typedef uint32_t						 uintptr_t;

// Greatest-width integer types
typedef int64_t							 intmax_t;
typedef uint64_t						 uintmax_t;

// Limits of exact-width integer types
#define INT8_MIN						 (-128)
#define INT8_MAX						 127
#define UINT8_MAX						 255U
#define INT16_MIN						 (-32768)
#define INT16_MAX						 32767
#define UINT16_MAX					 65535U
#define INT32_MIN						 (-2147483647-1)
#define INT32_MAX						 2147483647
#define UINT32_MAX					 4294967295U
#define INT64_MIN						 (-9223372036854775807LL-1)
#define INT64_MAX						 9223372036854775807LL
#define UINT64_MAX					 18446744073709551615ULL

// Limits of fastest minimum-width integer types
#define INT_FAST8_MIN				 INT8_MIN
#define INT_FAST8_MAX				 INT8_MAX
#define UINT_FAST8_MAX			 UINT8_MAX
#define INT_FAST16_MIN			 INT32_MIN
#define INT_FAST16_MAX			 INT32_MAX
#define UINT_FAST16_MAX			 UINT32_MAX
#define INT_FAST32_MIN			 INT32_MIN
#define INT_FAST32_MAX			 INT32_MAX
#define UINT_FAST32_MAX			 UINT32_MAX
#define INT_FAST64_MIN			 INT64_MIN
#define INT_FAST64_MAX			 INT64_MAX
#define UINT_FAST64_MAX			 UINT64_MAX

// Limits of integer types capable of holding object pointers
#define INTPTR_MIN					 INT32_MIN
#define INTPTR_MAX					 INT32_MAX
#define UINTPTR_MAX					 UINT32_MAX

// Limits of greatest-width integer types
#define INTMAX_MIN					 INT64_MIN
#define INTMAX_MAX					 INT64_MAX
#define UINTMAX_MAX					 UINT64_MAX

// Limits of other integer types (MISSING FROM YOUR ORIGINAL)
#define PTRDIFF_MIN					 INT32_MIN
#define PTRDIFF_MAX					 INT32_MAX
#define SIG_ATOMIC_MIN			 INT32_MIN
#define SIG_ATOMIC_MAX			 INT32_MAX

#ifndef SIZE_MAX
#define SIZE_MAX						 ((size_t)-1)
#endif
#ifndef WCHAR_MIN
#define WCHAR_MIN						 0
#endif
#ifndef WCHAR_MAX
#define WCHAR_MAX						 UINT32_MAX
#endif
#ifndef WINT_MIN
#define WINT_MIN						 0
#endif
#ifndef WINT_MAX
#define WINT_MAX						 UINT32_MAX
#endif

// Function-like macros for integer constants
#define INT8_C(c)						 c
#define INT16_C(c)					 c
#define INT32_C(c)					 c
#define INT64_C(c)					 c##LL
#define UINT8_C(c)					 c##U
#define UINT16_C(c)					 c##U
#define UINT32_C(c)					 c##U
#define UINT64_C(c)					 c##ULL
#define INTMAX_C(c)					 c##LL
#define UINTMAX_C(c)				 c##ULL

#ifdef __cplusplus
}
#endif

#endif /* _STDINT_H */

