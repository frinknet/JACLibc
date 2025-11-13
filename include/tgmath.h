/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef TGMATH_H
#define TGMATH_H
#pragma once

#include <config.h>

#if !JACL_HAS_C11
#if JACL_HAS_C99
#error "Even though tgmath.h was included in C99 there was no general mechanism to make it work."
#endif
#error "tgmath.h requires C11 or later"
#else

#include <math.h>
#include <complex.h>

// Integer promotion helper (C11 7.25 requirement)
#define __tg_real_promote(x) _Generic((x), \
	float: (float)(x), \
	double: (double)(x), \
	long double: (long double)(x), \
	default: (double)(x))

// Type classification for real/complex dispatch
#define __tg_type_tag(x) _Generic((x), \
	float: 1.0f, \
	long double: 1.0L, \
	float complex: 1.0if, \
	long double complex: 1.0iL, \
	default: 1.0)

// Real-only dispatch with integer promotion
#define __tg_real(fn, x) _Generic(__tg_real_promote(x), \
	float: fn##f, \
	long double: fn##l, \
	default: fn)(x)

#define __tg_real2(fn, x, y) _Generic( \
	__tg_real_promote(x) + __tg_real_promote(y), \
	float: fn##f, \
	long double: fn##l, \
	default: fn)(x, y)

#define __tg_real3(fn, x, y, z) _Generic( \
	__tg_real_promote(x) + __tg_real_promote(y) + __tg_real_promote(z), \
	float: fn##f, \
	long double: fn##l, \
	default: fn)(x, y, z)

// Real/complex dispatch
#define __tg_complex(fn, x) _Generic(__tg_type_tag(x), \
	float: fn##f, \
	long double: fn##l, \
	float complex: c##fn##f, \
	long double complex: c##fn##l, \
	default: fn)(x)

#define __tg_complex2(fn, x, y) _Generic( \
	__tg_type_tag(x) + __tg_type_tag(y), \
	float: fn##f, \
	long double: fn##l, \
	float complex: c##fn##f, \
	long double complex: c##fn##l, \
	default: fn)(x, y)

// fabs - special case: complex uses cabs not cfabs
#define fabs(x) _Generic(__tg_type_tag(x), \
	float: fabsf, \
	long double: fabsl, \
	float complex: cabsf, \
	long double complex: cabsl, \
	default: fabs)(x)

// sqrt, cbrt, exp, exp2, expm1, log, log10, log1p, log2, logb
#define sqrt(x) __tg_complex(sqrt, x)
#define cbrt(x) __tg_real(cbrt, x)
#define exp(x) __tg_complex(exp, x)
#define exp2(x) __tg_real(exp2, x)
#define expm1(x) __tg_real(expm1, x)
#define log(x) __tg_complex(log, x)
#define log10(x) __tg_real(log10, x)
#define log1p(x) __tg_real(log1p, x)
#define log2(x) __tg_real(log2, x)
#define logb(x) __tg_real(logb, x)

// ilogb, frexp, ldexp - functions with extra non-generic integer parameters
#define ilogb(x) _Generic(__tg_real_promote(x), \
	float: ilogbf, \
	long double: ilogbl, \
	default: ilogb)(x)

#define frexp(x, e) _Generic(__tg_real_promote(x), \
	float: frexpf, \
	long double: frexpl, \
	default: frexp)(x, e)

#define ldexp(x, e) _Generic(__tg_real_promote(x), \
	float: ldexpf, \
	long double: ldexpl, \
	default: ldexp)(x, e)

// sin, cos, tan, asin, acos, atan, atan2
#define sin(x) __tg_complex(sin, x)
#define cos(x) __tg_complex(cos, x)
#define tan(x) __tg_complex(tan, x)
#define asin(x) __tg_complex(asin, x)
#define acos(x) __tg_complex(acos, x)
#define atan(x) __tg_complex(atan, x)
#define atan2(y, x) __tg_real2(atan2, y, x)

// sinh, cosh, tanh, asinh, acosh, atanh
#define sinh(x) __tg_complex(sinh, x)
#define cosh(x) __tg_complex(cosh, x)
#define tanh(x) __tg_complex(tanh, x)
#define asinh(x) __tg_complex(asinh, x)
#define acosh(x) __tg_complex(acosh, x)
#define atanh(x) __tg_complex(atanh, x)

// hypot, erf, erfc, tgamma, lgamma
#define hypot(x, y) __tg_real2(hypot, x, y)
#define erf(x) __tg_real(erf, x)
#define erfc(x) __tg_real(erfc, x)
#define tgamma(x) __tg_real(tgamma, x)
#define lgamma(x) __tg_real(lgamma, x)

// ceil, floor, trunc, round, nearbyint, rint
#define ceil(x) __tg_real(ceil, x)
#define floor(x) __tg_real(floor, x)
#define trunc(x) __tg_real(trunc, x)
#define round(x) __tg_real(round, x)
#define nearbyint(x) __tg_real(nearbyint, x)
#define rint(x) __tg_real(rint, x)

// fmod, remainder
#define fmod(x, y) __tg_real2(fmod, x, y)
#define remainder(x, y) __tg_real2(remainder, x, y)

// scalbn, scalbln, copysign, nextafter, nexttoward - functions with extra non-generic parameters
#define scalbn(x, n) _Generic(__tg_real_promote(x), \
	float: scalbnf, \
	long double: scalbnl, \
	default: scalbn)(x, n)

#define scalbln(x, n) _Generic(__tg_real_promote(x), \
	float: scalblnf, \
	long double: scalblnl, \
	default: scalbln)(x, n)

#define copysign(x, y) __tg_real2(copysign, x, y)
#define nextafter(x, y) __tg_real2(nextafter, x, y)

#define nexttoward(x, y) _Generic(__tg_real_promote(x), \
	float: nexttowardf, \
	long double: nexttowardl, \
	default: nexttoward)(x, y)

// carg, cimag, creal, conj, cproj
#define carg(z) _Generic((z), \
	float complex: cargf, \
	long double complex: cargl, \
	default: carg)(z)
#define cimag(z) _Generic((z), \
	float complex: cimagf, \
	long double complex: cimagl, \
	default: cimag)(z)
#define creal(z) _Generic((z), \
	float complex: crealf, \
	long double complex: creall, \
	default: creal)(z)
#define conj(z) _Generic((z), \
	float complex: conjf, \
	long double complex: conjl, \
	default: conj)(z)
#define cproj(z) _Generic((z), \
	float complex: cprojf, \
	long double complex: cprojl, \
	default: cproj)(z)

// pow, fdim, fmax, fmin, fma
#define pow(x, y) __tg_complex2(pow, x, y)
#define fdim(x, y) __tg_real2(fdim, x, y)
#define fmax(x, y) __tg_real2(fmax, x, y)
#define fmin(x, y) __tg_real2(fmin, x, y)
#define fma(x, y, z) __tg_real3(fma, x, y, z)

#endif // JACL_HAS_C11
#endif // TGMATH_H
