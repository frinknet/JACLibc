/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef FLOAT_H
#define FLOAT_H
#pragma once

// — Single‐precision (float) —
#define FLT_RADIX       2
#define FLT_MANT_DIG    24
#define FLT_DIG         6
#define FLT_MIN_EXP     (-125)
#define FLT_MAX_EXP     128
#define FLT_MIN_10_EXP  (-37)
#define FLT_MAX_10_EXP  38
#define FLT_MAX         3.40282347e+38F
#define FLT_EPSILON     1.19209290e-07F
#define FLT_MIN         1.17549435e-38F
#define FLT_TRUE_MIN    1.40129846e-45F

// — Double‐precision (double) —
#define DBL_MANT_DIG    53
#define DBL_DIG         15
#define DBL_MIN_EXP     (-1021)
#define DBL_MAX_EXP     1024
#define DBL_MIN_10_EXP  (-307)
#define DBL_MAX_10_EXP  308
#define DBL_MAX         1.7976931348623157e+308
#define DBL_EPSILON     2.2204460492503131e-16
#define DBL_MIN         2.2250738585072014e-308
#define DBL_TRUE_MIN    4.9406564584124654e-324

// — Extended‐precision (long double) —
// Clang/WebAssembly treats long double as double
#define LDBL_MANT_DIG  DBL_MANT_DIG
#define LDBL_DIG       DBL_DIG
#define LDBL_MIN_EXP   DBL_MIN_EXP
#define LDBL_MAX_EXP   DBL_MAX_EXP
#define LDBL_MIN_10_EXP DBL_MIN_10_EXP
#define LDBL_MAX_10_EXP DBL_MAX_10_EXP
#define LDBL_MAX       DBL_MAX
#define LDBL_EPSILON   DBL_EPSILON
#define LDBL_MIN       DBL_MIN
#define LDBL_TRUE_MIN  DBL_TRUE_MIN

// — General —
#define DECIMAL_DIG    17
#define FLT_ROUNDS     1  // round-to-nearest

#endif /* FLOAT_H */
