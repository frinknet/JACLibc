/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STDINT_H
#define STDINT_H
#pragma once

#include <config.h>
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

typedef signed char        int8_t;
typedef unsigned char      uint8_t;

typedef short              int16_t;
typedef unsigned short     uint16_t;

#if defined(__INT32_TYPE__)
#define JACL_HAS_INT32 1
typedef __INT32_TYPE__ int32_t;
typedef __UINT32_TYPE__ uint32_t;
#elif (sizeof(int) == 4)
#define JACL_HAS_INT32 1
typedef int int32_t;
typedef unsigned int uint32_t;
#elif (sizeof(long) == 4)
#define JACL_HAS_INT32 1
typedef long int32_t;
typedef unsigned long uint32_t;
#endif

#if JACL_HAS_C99
#if defined(__INT64_TYPE__)
#define JACL_HAS_INT64 1
typedef __INT64_TYPE__ int64_t;
typedef __UINT64_TYPE__ uint64_t;
#elif JACL_INTMAX_BITS >= 64
#define JACL_HAS_INT64 1
typedef long long          int64_t;
typedef unsigned long long uint64_t;
#endif
#endif

/* ============================================================= */
/* Fastest Minimum-Width Integer Types                           */
/* ============================================================= */

typedef int8_t             int_fast8_t;
typedef uint8_t            uint_fast8_t;

typedef int32_t            int_fast16_t;
typedef uint32_t           uint_fast16_t;

typedef int32_t int_fast32_t;
typedef uint32_t uint_fast32_t;

#if JACL_HAS_INT64
typedef int64_t int_fast64_t;
typedef uint64_t uint_fast64_t;
#endif

/* ============================================================= */
/* Smallest Integer Types with At Least Specified Width         */
/* ============================================================= */

typedef int8_t             int_least8_t;
typedef uint8_t            uint_least8_t;

typedef int16_t            int_least16_t;
typedef uint16_t           uint_least16_t;

typedef int32_t int_least32_t;
typedef uint32_t uint_least32_t;

#if JACL_HAS_INT64
typedef int64_t int_least64_t;
typedef uint64_t uint_least64_t;
#endif

/* ============================================================= */
/* Pointer-Width Integer Types                                   */
/* ============================================================= */

#if JACL_64BIT
typedef int64_t  intptr_t;
typedef uint64_t uintptr_t;
#elif JACL_32BIT
typedef int32_t  intptr_t;
typedef uint32_t uintptr_t;
#elif JACL_16BIT
typedef int16_t  intptr_t;
typedef uint16_t uintptr_t;
#else
typedef int8_t  intptr_t;
typedef uint8_t uintptr_t;
#endif

/* ============================================================= */
/* Greatest-Width Integer Types                                  */
/* ============================================================= */

#if defined(__INTMAX_TYPE__)
typedef __INTMAX_TYPE__ intmax_t;
typedef __UINTMAX_TYPE__ uintmax_t;
#elif JACL_HAS_INT64
typedef int64_t            intmax_t;
typedef uint64_t           uintmax_t;
#elif JACL_HAS_INT32
typedef int32_t            intmax_t;
typedef uint32_t           uintmax_t;
#else
typedef int16_t            intmax_t;
typedef uint16_t           uintmax_t;
#endif

/* ============================================================= */
/* Limits of Exact-Width Integer Types                           */
/* ============================================================= */

#define UINT8_MAX   255U
#define INT8_MAX    127
#define INT8_MIN    (-128)

#if JACL_INTMAX_BITS >= 16
#define UINT16_MAX  65535U
#define INT16_MAX   32767
#define INT16_MIN   (-32768)
#endif

#if JACL_INTMAX_BITS >= 32
#define UINT32_MAX  4294967295U
#define INT32_MAX   2147483647
#define INT32_MIN   (-2147483647-1)
#endif

#if JACL_INTMAX_BITS >= 64
#define UINT64_MAX  18446744073709551615ULL
#define INT64_MAX   9223372036854775807LL
#define INT64_MIN   (-9223372036854775807LL-1)
#endif

/* ============================================================= */
/* Limits of Fastest Minimum-Width Integer Types                 */
/* ============================================================= */

#define INT_FAST8_MIN      INT8_MIN
#define INT_FAST8_MAX      INT8_MAX
#define UINT_FAST8_MAX     UINT8_MAX

#define INT_FAST16_MIN     INT32_MIN
#define INT_FAST16_MAX     INT32_MAX
#define UINT_FAST16_MAX    UINT32_MAX

#if JACL_HAS_INT32
#define INT_FAST32_MIN     INT32_MIN
#define INT_FAST32_MAX     INT32_MAX
#define UINT_FAST32_MAX    UINT32_MAX
#endif

#if JACL_HAS_INT64
#define INT_FAST64_MIN     INT64_MIN
#define INT_FAST64_MAX     INT64_MAX
#define UINT_FAST64_MAX    UINT64_MAX
#endif

/* ============================================================= */
/* Limits of Smallest Width Integer Types                        */
/* ============================================================= */

#define INT_LEAST8_MIN     INT8_MIN
#define INT_LEAST8_MAX     INT8_MAX
#define UINT_LEAST8_MAX    UINT8_MAX

#define INT_LEAST16_MIN    INT16_MIN
#define INT_LEAST16_MAX    INT16_MAX
#define UINT_LEAST16_MAX   UINT16_MAX

#if JACL_HAS_INT32
#define INT_LEAST32_MIN    INT32_MIN
#define INT_LEAST32_MAX    INT32_MAX
#define UINT_LEAST32_MAX   UINT32_MAX
#endif

#if JACL_HAS_INT64
#define INT_LEAST64_MIN    INT64_MIN
#define INT_LEAST64_MAX    INT64_MAX
#define UINT_LEAST64_MAX   UINT64_MAX
#endif

/* ============================================================= */
/* Limits of Pointer-Width Integer Types                         */
/* ============================================================= */

#if JACL_64BIT
  #define UINTPTR_MAX  UINT64_MAX
  #define INTPTR_MAX   INT64_MAX
  #define INTPTR_MIN   INT64_MIN
#elif JACL_32BIT
  #define UINTPTR_MAX  UINT32_MAX
  #define INTPTR_MAX   INT32_MAX
  #define INTPTR_MIN   INT32_MIN
#elif JACL_16BIT
  #define UINTPTR_MAX  UINT16_MAX
  #define INTPTR_MAX   INT16_MAX
  #define INTPTR_MIN   INT16_MIN
#else
  #define UINTPTR_MAX  UINT8_MAX
  #define INTPTR_MAX   INT8_MAX
  #define INTPTR_MIN   INT8_MIN
#endif

/* ============================================================= */
/* Limits of Greatest-Width Integer Types                        */
/* ============================================================= */

#if JACL_HAS_INT64
  #define UINTMAX_MAX  UINT64_MAX
  #define INTMAX_MAX   INT64_MAX
  #define INTMAX_MIN   INT64_MIN
#elif JACL_HAS_INT32
  #define UINTMAX_MAX  UINT32_MAX
  #define INTMAX_MAX   INT32_MAX
  #define INTMAX_MIN   INT32_MIN
#else
  #define UINTMAX_MAX  UINT16_MAX
  #define INTMAX_MAX   INT16_MAX
  #define INTMAX_MIN   INT16_MIN
#endif
/* ============================================================= */
/* Limits of Other Integer Types                                 */
/* ============================================================= */

#if JACL_64BIT
  #define PTRDIFF_MAX  INT64_MAX
  #define PTRDIFF_MIN  INT64_MIN
#elif JACL_32BIT
  #define PTRDIFF_MAX  INT32_MAX
  #define PTRDIFF_MIN  INT32_MIN
#elif JACL_16BIT
  #define PTRDIFF_MAX  INT16_MAX
  #define PTRDIFF_MIN  INT16_MIN
#else
  #define PTRDIFF_MAX  INT8_MAX
  #define PTRDIFF_MIN  INT8_MIN
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

/* ============================================================= */
/* Sanity Checks for Type System                                 */
/* ============================================================= */

static_assert(sizeof(int8_t)   == 1,  "int8_t must be 8 bits");
static_assert(sizeof(uint8_t)  == 1,  "uint8_t must be 8 bits");

static_assert(sizeof(int16_t)  == 2,  "int16_t must be 16 bits");
static_assert(sizeof(uint16_t) == 2,  "uint16_t must be 16 bits");

#if JACL_HAS_INT32
static_assert(sizeof(int32_t)  == 4,  "int32_t must be 32 bits");
static_assert(sizeof(uint32_t) == 4,  "uint32_t must be 32 bits");
#endif

#if JACL_HAS_INT64
static_assert(sizeof(int64_t)  == 8,  "int64_t must be 64 bits");
static_assert(sizeof(uint64_t) == 8,  "uint64_t must be 64 bits");
#endif

#ifdef __cplusplus
}
#endif

#endif /* STDINT_H */
