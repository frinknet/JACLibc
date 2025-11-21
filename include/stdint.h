/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STDINT_H
#define STDINT_H
#pragma once

#include <config.h>
#include <limits.h>
#include <stddef.h>

#if JACL_HAS_C23
	#define __STDC_VERSION_STDINT_H__ 202311L
#elif JACL_HAS_C11
	#define __STDC_VERSION_STDINT_H__ 201112L
#elif JACL_HAS_C99
	#define __STDC_VERSION_STDINT_H__ 199901L
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================= */
/* Fixed-Width Integer Types                                     */
/* ============================================================= */

#if SHRT_BIT == 8
	#define JACL_HAS_INT8 1
	typedef signed short     int8_t;
	typedef unsigned short   uint8_t;
#elif CHAR_BIT == 8
	#define JACL_HAS_INT8 1
	typedef signed char     int8_t;
	typedef unsigned char   uint8_t;
#endif

#if JACL_HAS_INT8
	static_assert(sizeof(int8_t)   == 1,  "int8_t must be exactly 8 bits");
	static_assert(sizeof(uint8_t)  == 1,  "uint8_t must be exactly 8 bits");
#endif

#if SHRT_BIT == 16
	#define JACL_HAS_INT16 1
	typedef signed short     int16_t;
	typedef unsigned short   uint16_t;
#elif INT_BIT == 16
	#define JACL_HAS_INT16 1
	typedef int              int16_t;
	typedef unsigned int     uint16_t;
#endif

#if JACL_HAS_INT16
	static_assert(sizeof(int16_t)  == 2,  "int16_t must be exactly 16 bits");
	static_assert(sizeof(uint16_t) == 2,  "uint16_t must be exactly 16 bits");
#endif

#if INT_BIT == 32
	#define JACL_HAS_INT32 1
	typedef int              int32_t;
	typedef unsigned int     uint32_t;
#elif LONG_BIT == 32
	#define JACL_HAS_INT32 1
	typedef long             int32_t;
	typedef unsigned long    uint32_t;
#endif

#if JACL_HAS_INT32
	static_assert(sizeof(int32_t)  == 4,  "int32_t must be exactly 32 bits");
	static_assert(sizeof(uint32_t) == 4,  "uint32_t must be exactly 32 bits");
#endif

#if JACL_HAS_C99
	#if JACL_INTMAX_BITS >= 64
		#define JACL_HAS_INT64 1
		typedef long long           int64_t;
		typedef unsigned long long  uint64_t;
	#endif
#endif

#if JACL_HAS_INT64
	static_assert(sizeof(int64_t)  == 8,  "int64_t must be exactly 64 bits");
	static_assert(sizeof(uint64_t) == 8,  "uint64_t must be exactly 64 bits");
#endif

/* ============================================================= */
/* Limits of Exact-Width Integer Types                           */
/* ============================================================= */

#if JACL_HAS_INT8
	#define INT8_MIN    (-127-1)
	#define INT8_MAX    (127)
	#define UINT8_MAX   (255U)
#endif

#if JACL_HAS_INT16
	#define INT16_MIN   (-32767-1)
	#define INT16_MAX   (32767)
	#define UINT16_MAX  (65535U)
#endif

#if JACL_HAS_INT32
	#define INT32_MIN   (-2147483647-1)
	#define INT32_MAX   (2147483647)
	#define UINT32_MAX  (4294967295U)
#endif

#if JACL_HAS_INT64
	#define INT64_MIN   (-9223372036854775807LL-1)
	#define INT64_MAX   (9223372036854775807LL)
	#define UINT64_MAX  (18446744073709551615ULL)
#endif

/* ============================================================= */
/* Fastest Minimum-Width Integer Types                           */
/* ============================================================= */

typedef int                                int_fast8_t;
typedef unsigned int                       uint_fast8_t;

#define INT_FAST8_MIN                      INT_MIN
#define INT_FAST8_MAX                      INT_MAX
#define UINT_FAST8_MAX                     UINT_MAX

static_assert(sizeof(int_fast8_t)  >= 1,   "int_fast8_t must be at fast 8 bits");
static_assert(sizeof(uint_fast8_t) >= 1,   "uint_fast8_t must be at fast 8 bits");

typedef int                                int_fast16_t;
typedef unsigned int                       uint_fast16_t;

#define INT_FAST16_MIN                     INT_MIN
#define INT_FAST16_MAX                     INT_MAX
#define UINT_FAST16_MAX                    UINT_MAX

static_assert(sizeof(int_fast16_t)  >= 2,  "int_fast16_t must be at fast 16 bits");
static_assert(sizeof(uint_fast16_t) >= 2,  "uint_fast16_t must be at fast 16 bits");

#if INT_BIT >= 32

typedef int                                int_fast32_t;
typedef unsigned int                       uint_fast32_t;

#define INT_FAST32_MIN                     INT_MIN
#define INT_FAST32_MAX                     INT_MAX
#define UINT_FAST32_MAX                    UINT_MAX

#else

typedef long                               int_fast32_t;
typedef unsigned long                      uint_fast32_t;

#define INT_FAST32_MIN                     INT_MIN
#define INT_FAST32_MAX                     INT_MAX
#define UINT_FAST32_MAX                    UINT_MAX

#endif

static_assert(sizeof(int_fast32_t) >= 4,   "int_fast32_t must be at fast 32 bits");
static_assert(sizeof(uint_fast32_t) >= 4,  "uint_fast32_t must be at fast 32 bits");

#if JACL_HAS_INT64

typedef int64_t                            int_fast64_t;
typedef uint64_t                           uint_fast64_t;

#define INT_FAST64_MIN                     INT64_MIN
#define INT_FAST64_MAX                     INT64_MAX
#define UINT_FAST64_MAX                    UINT64_MAX

static_assert(sizeof(int_fast64_t) >= 8,   "int_fast64_t must be at fast 64 bits");
static_assert(sizeof(uint_fast64_t) >= 8,  "uint_fast64_t must be at fast 64 bits");

#endif

/* ============================================================= */
/* Smallest Integer Types with At Least Specified Width          */
/* ============================================================= */

#if CHAR_BIT >= 8

typedef signed char                         int_least8_t;
typedef unsigned char                       uint_least8_t;

#define INT_LEAST8_MIN                      SCHAR_MIN
#define INT_LEAST8_MAX                      SCHAR_MAX
#define UINT_LEAST8_MAX                     UCHAR_MAX

#else

typedef short                               int_least8_t;
typedef unsigned short                      uint_least8_t;

#define INT_LEAST8_MIN                      SHRT_MIN
#define INT_LEAST8_MAX                      SHRT_MAX
#define UINT_LEAST8_MAX                     USHRT_MAX

#endif

static_assert(sizeof(int_least8_t) >= 1,    "int_least8_t must be at least 8 bits");
static_assert(sizeof(uint_least8_t) >= 1,   "uint_least8_t must be at least 8 bits");

typedef short                               int_least16_t;
typedef unsigned short                      uint_least16_t;

#define INT_LEAST16_MIN                     SHRT_MIN
#define INT_LEAST16_MAX                     SHRT_MAX
#define UINT_LEAST16_MAX                    USHRT_MAX

static_assert(sizeof(int_least16_t) >= 2,   "int_least16_t must be at least 16 bits");
static_assert(sizeof(uint_least16_t) >= 2,  "uint_least16_t must be at least 16 bits");


#if JACL_32BIT || JACL_64BIT

typedef int                                 int_least32_t;
typedef unsigned int                        uint_least32_t;

#define INT_LEAST32_MIN                     INT_MIN
#define INT_LEAST32_MAX                     INT_MAX
#define UINT_LEAST32_MAX                    UINT_MAX

#else

typedef long                                int_least32_t;
typedef unsigned long                       uint_least32_t;

#define INT_LEAST32_MIN                     LONG_MIN
#define INT_LEAST32_MAX                     LONG_MAX
#define UINT_LEAST32_MAX                    ULONG_MAX

#endif

static_assert(sizeof(int_least32_t) >= 4,   "int_least32_t must be at least 32 bits");
static_assert(sizeof(uint_least32_t) >= 4,  "uint_least32_t must be at least 32 bits");

#if JACL_HAS_INT64

typedef int64_t                             int_least64_t;
typedef uint64_t                            uint_least64_t;

static_assert(sizeof(int_least64_t) >= 8,   "int_least64_t must be at least 64 bits");
static_assert(sizeof(uint_least64_t) >= 8,  "uint_least64_t must be at least 64 bits");

#define INT_LEAST64_MIN                     INT64_MIN
#define INT_LEAST64_MAX                     INT64_MAX
#define UINT_LEAST64_MAX                    UINT64_MAX

#endif

/* ============================================================= */
/* Pointer-Width Integer Types                                   */
/* ============================================================= */

#if JACL_64BIT
typedef int64_t            intptr_t;
typedef uint64_t           uintptr_t;

#define INTPTR_MIN        INT64_MIN
#define INTPTR_MAX        INT64_MAX
#define UINTPTR_MAX       UINT64_MAX
#elif JACL_32BIT
typedef int32_t           intptr_t;
typedef uint32_t          uintptr_t;

#define INTPTR_MIN        INT32_MIN
#define INTPTR_MAX        INT32_MAX
#define UINTPTR_MAX       UINT32_MAX
#elif JACL_16BIT
typedef int16_t           intptr_t;
typedef uint16_t          uintptr_t;

#define INTPTR_MIN        INT16_MIN
#define INTPTR_MAX        INT16_MAX
#define UINTPTR_MAX       UINT16_MAX
#else
typedef int8_t            intptr_t;
typedef uint8_t           uintptr_t;

#define INTPTR_MIN        INT16_MIN
#define INTPTR_MAX        INT16_MAX
#define UINTPTR_MAX       UINT16_MAX
#endif

/* ============================================================= */
/* Greatest-Width Integer Types                                  */
/* ============================================================= */

#if JACL_HAS_INT64
typedef int64_t           intmax_t;
typedef uint64_t          uintmax_t;

#define INTMAX_MIN        INT64_MIN
#define INTMAX_MAX        INT64_MAX
#define UINTMAX_MAX       UINT64_MAX
#elif JACL_HAS_INT32
typedef int32_t           intmax_t;
typedef uint32_t          uintmax_t;

#define INTMAX_MIN        INT32_MIN
#define INTMAX_MAX        INT32_MAX
#define UINTMAX_MAX       UINT32_MAX
#elif JACL_HAS_INT16
typedef int16_t           intmax_t;
typedef uint16_t          uintmax_t;

#define INTMAX_MIN        INT16_MIN
#define INTMAX_MAX        INT16_MAX
#define UINTMAX_MAX       UINT16_MAX
#else
typedef int8_t            intmax_t;
typedef uint8_t           uintmax_t;

#define INTMAX_MIN        INT8_MIN
#define INTMAX_MAX        INT8_MAX
#define UINTMAX_MAX       UINT8_MAX
#endif

/* ============================================================= */
/* Limits of Other Integer Types                                 */
/* ============================================================= */

#if JACL_64BIT
  #define PTRDIFF_MIN    LONG_MIN
  #define PTRDIFF_MAX    LONG_MAX
#elif JACL_32BIT
  #define PTRDIFF_MIN    INT_MIN
  #define PTRDIFF_MAX    INT_MAX
#elif JACL_16BIT
  #define PTRDIFF_MIN    SHRT_MIN
  #define PTRDIFF_MAX    SHRT_MAX
#else
  #define PTRDIFF_MIN    CHAR_MIN
  #define PTRDIFF_MAX    CHAR_MAX
#endif

#define SIG_ATOMIC_MAX  INT32_MAX
#define SIG_ATOMIC_MIN  INT32_MIN

#ifndef WCHAR_MIN
#define WCHAR_MIN          0
#endif

#ifndef WCHAR_MAX
#define WCHAR_MAX          UINT32_MAX
#endif

#ifndef WINT_MIN
#define WINT_MIN           0
#endif

#ifndef WINT_MAX
#define WINT_MAX           UINT32_MAX
#endif

/* ============================================================= */
/* Integer Constant Macros                                       */
/* ============================================================= */

#define INT8_C(c)          c
#define UINT8_C(c)         c##U

#define INT16_C(c)         c
#define UINT16_C(c)        c##U

#if JACL_HAS_INT32
#define INT32_C(c)         c
#define UINT32_C(c)        c##U
#endif

#if JACL_HAS_INT64
#define INT64_C(c)         c##LL
#define UINT64_C(c)        c##ULL
#endif

#if JACL_HAS_INT64
#define INTMAX_C(c)        c##LL
#define UINTMAX_C(c)       c##ULL
#elif JACL_HAS_INT32
#define INTMAX_C(c)        c
#define UINTMAX_C(c)       c##U
#else
#define INTMAX_C(c)        c
#define UINTMAX_C(c)       c##U
#endif

#ifdef __cplusplus
}
#endif

#endif /* STDINT_H */
