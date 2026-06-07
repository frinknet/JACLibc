/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _MATH_H
#define _MATH_H
#pragma once

#include <config.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <float.h>
#include <fenv.h>

#ifdef __cplusplus
extern "C" {
#endif

#if JACL_HAS_FLOAT
static_assert(sizeof(double) == 8, "Assumes 64-bit IEEE-754 double");
static_assert(FLT_MANT_DIG == 24, "Assumes IEEE-754 float mantissa");
static_assert(DBL_MANT_DIG == 53, "Assumes IEEE-754 double mantissa");
#endif

#define JACL_RND_EVEN 0
#define JACL_RND_DOWN 1
#define JACL_RND_HIGH 2
#define JACL_RND_ZERO 3
#define JACL_RND_BANK 4

#if JACL_HAS_C99

// Special Values
#define HUGE_VAL   ((double)(1.0/0.0))
#define HUGE_VALF  ((float)(1.0f/0.0f))
#define HUGE_VALL  ((long double)(1.0L/0.0L))
#define NAN        ((double)(0.0/0.0))
#define INFINITY ((float)(1.0f/0.0f))

// Constants
#define FP_INFINITE	   1
#define FP_NAN         2
#define FP_NORMAL      3
#define FP_SUBNORMAL   4
#define FP_ZERO        5
#define FP_ILOGB0      INT_MIN
#define FP_ILOGBNAN    INT_MIN

#endif /* JACL_HAS_C99 */

#if JACL_HAS_C23

#define FP_INT_TONEAREST         JACL_RND_EVEN
#define FP_INT_DOWNWARD          JACL_RND_DOWN
#define FP_INT_UPWARD            JACL_RND_HIGH
#define FP_INT_TOWARDZERO        JACL_RND_ZERO
#define FP_INT_TONEARESTFROMZERO JACL_RND_BANK

#endif /* JACL_HAS_C23 */

/*TODO: FMA support*/

#define MATH_ERRNO		 1
#define MATH_ERREXCEPT 2
#define math_errhandling (MATH_ERRNO | MATH_ERREXCEPT)

#define M_El          2.7182818284590452353602874713526624977572L
#define M_EGAMMAl     0.5772156649015328606065120900824024310421L
#define M_LOG2El      1.4426950408889634073599246810018921374266L
#define M_LOG10El     0.4342944819032518276511289189166050822943L
#define M_LN2l        0.6931471805599453094172321214581765680755L
#define M_LN10l       2.3025850929940456840179914546843642076011L
#define M_PHIl        1.6180339887498948482045868343656381177203L
#define M_PIl         3.1415926535897932384626433832795028841971L
#define M_PI_2l       1.5707963267948966192313216916397514420985L
#define M_PI_4l       0.7853981633974483096156608458198757210492L
#define M_1_PIl       0.3183098861837906715377675267450287240689L
#define M_1_SQRTPIl   0.5641895835477562869480794515607725858441L
#define M_2_PIl       0.6366197723675813430755350534900574481378L
#define M_2_SQRTPIl   1.1283791670955125738961589031215451716881L
#define M_SQRT2l      1.4142135623730950488016887242096980785696L
#define M_SQRT3l      1.7320508075688772935274463415058723669428L
#define M_SQRT1_2l    0.7071067811865475244008443621048490392848L
#define M_SQRT1_3l    0.5773502691896257645091487805019574556476L

#define M_E            (double)M_El
#define M_EGAMMA       (double)M_EGAMMAl
#define M_LOG2E        (double)M_LOG2El
#define M_LOG10E       (double)M_LOG10El
#define M_LN2          (double)M_LN2l
#define M_LN10         (double)M_LN10l
#define M_PHI          (double)M_PHIl
#define M_PI           (double)M_PIl
#define M_PI_2         (double)M_PI_2l
#define M_PI_4         (double)M_PI_4l
#define M_1_PI         (double)M_1_PIl
#define M_1_SQRTPI     (double)M_1_SQRTPIl
#define M_2_PI         (double)M_2_PIl
#define M_2_SQRTPI     (double)M_2_SQRTPIl
#define M_SQRT2        (double)M_SQRT2l
#define M_SQRT3        (double)M_SQRT3l
#define M_SQRT1_2      (double)M_SQRT1_2l
#define M_SQRT1_3      (double)M_SQRT1_3l

typedef union { double f; uint64_t u; } __jacl_f64u;

#ifdef FLT_EVAL_METHOD
#if FLT_EVAL_METHOD == 0
typedef float float_t;
typedef double double_t;
#elif FLT_EVAL_METHOD == 1
typedef double float_t;
typedef double double_t;
#elif FLT_EVAL_METHOD == 2
typedef long double float_t;
typedef long double double_t;
#endif
#else
typedef float float_t;
typedef double double_t;
#endif

// ======== CLASSIFICATION VIA MACROS =======
#define isnan(x)              (sizeof(x) == sizeof(float) ? __isnanf(x) : sizeof(x) == sizeof(double) ? __isnan(x) : __isnanl(x) )
#define isinf(x)              (sizeof(x) == sizeof(float) ? __isinff(x) : sizeof(x) == sizeof(double) ? __isinf(x) : __isinfl(x) )
#define isfinite(x)           (sizeof(x) == sizeof(float) ? __isfinitef(x) : sizeof(x) == sizeof(double) ? __isfinite(x) : __isfinitel(x) )
#define isnormal(x)           (sizeof(x) == sizeof(float) ? __isnormalf(x) : sizeof(x) == sizeof(double) ? __isnormal(x) : __isnormall(x) )
#define isunordered(x, y)     (sizeof(x) == sizeof(float) ? __isunorderedf(x, y) : sizeof(x) == sizeof(double) ? __isunordered(x, y) : __isunorderedl(x, y) )
#define isgreater(x, y)       (sizeof(x) == sizeof(float) ? __isgreaterf(x, y) : sizeof(x) == sizeof(double) ? __isgreater(x, y) : __isgreaterl(x, y) )
#define isgreaterequal(x, y)  (sizeof(x) == sizeof(float) ? __isgreaterequalf(x, y) : sizeof(x) == sizeof(double) ? __isgreaterequal(x, y) : __isgreaterequall(x, y) )
#define isless(x, y)          (sizeof(x) == sizeof(float) ? __islessf(x, y) : sizeof(x) == sizeof(double) ? __isless(x, y) : __islessl(x, y) )
#define islessequal(x, y)     (sizeof(x) == sizeof(float) ? __islessequalf(x, y) : sizeof(x) == sizeof(double) ? __islessequal(x, y) : __islessequall(x, y) )
#define islessgreater(x, y)   (sizeof(x) == sizeof(float) ? __islessgreaterf(x, y) : sizeof(x) == sizeof(double) ? __islessgreater(x, y) : __islessgreaterl(x, y) )
#define fpclassify(x)         (sizeof(x) == sizeof(float) ? __fpclassifyf(x) : sizeof(x) == sizeof(double) ? __fpclassify(x) : __fpclassifyl(x) )
#define signbit(x)            ( sizeof(x) == sizeof(double) ? __signbit(x) : sizeof(x) == sizeof(float) ? __signbitf(x) : __signbitl(x) )
#define nan(s)                __nan(s)

#if JACL_HAS_C99
#define nanf(s)               __nanf(s)
#define nanl(s)               __nanl(s)
#endif

// ======== HELPERS MACRO GENERATOR =========
#if JACL_HAS_C99

#define __jacl_math(F) \
__jacl_##F(double,			 , DBL) \
__jacl_##F(float,				f, FLT) \
__jacl_##F(long double, l, LDBL)

#else

#define __jacl_math(F) \
__jacl_##F(double,			, DBL)

#endif /* JACL_HAS_C99 */

#define __jacl_math2(a,b) \
	__jacl_math(a) \
	__jacl_math(b)
#define __jacl_math3(a,b,c) \
	__jacl_math2(a,b) \
	__jacl_math(c)
#define __jacl_math4(a,b,c,d) \
	__jacl_math3(a,b,c) \
	__jacl_math(d)

// ====== CHEBYSHEV POLYNOMIAL HELPERS ======
#define __jacl_cheb_even(type, c0, u4, c2, c4, c6) (((type)(c0)) + (u4) * (((type)(c2)) + (u4) * (((type)(c4)) + (u4) * ((type)(c6)))))
#define __jacl_cheb_odd(type, u2, u4, c1, c3, c5) ((u2) * (((type)(c1)) + (u4) * (((type)(c3)) + (u4) * ((type)(c5)))))

unsigned long strtoul(const char *str, char **endptr, int base);

// Float Checks: isnan, isinf, isfinite, isnormal, isunordered, isgreater, isless, islessequal, islessgreater, fpclassify
#define __jacl_isnan(type,suf,PRE)            static inline int __isnan##suf(type x) { return x != x; }
#define __jacl_isinf(type,suf,PRE)            static inline int __isinf##suf(type x) { return (!isnan(x) && isnan((x) - (x))); }
#define __jacl_isfinite(type,suf,PRE)         static inline int __isfinite##suf(type x) { return (!isnan(x) && !isinf(x)); }
#define __jacl_isnormal(type,suf,PRE)         static inline int __isnormal##suf(type x) { type ax = __jacl_signclr_##PRE(x); return ax >= PRE##_MIN && ax <= PRE##_MAX; }
#define __jacl_isunordered(type,suf,PRE)      static inline int __isunordered##suf(type x, type y) { return (isnan(x) || isnan(y)); }
#define __jacl_isgreater(type,suf,PRE)        static inline int __isgreater##suf(type x, type y) { return (!__isunordered##suf(x, y) && x > y); }
#define __jacl_isgreaterequal(type,suf,PRE)   static inline int __isgreaterequal##suf(type x, type y) { return (!__isunordered##suf(x, y) && x >= y); }
#define __jacl_isless(type,suf,PRE)           static inline int __isless##suf(type x, type y) { return (!__isunordered##suf(x, y) && x < y); }
#define __jacl_islessequal(type,suf,PRE)      static inline int __islessequal##suf(type x, type y) { return (!__isunordered##suf(x, y) && x <= y); }
#define __jacl_islessgreater(type,suf,PRE)    static inline int __islessgreater##suf(type x, type y) { return (!__isunordered##suf(x, y) && (x < y || x > y)); }
#define __jacl_fpclassify(type,suf,PRE)       static inline int __fpclassify##suf(type x) { return ( isnan(x) ? FP_NAN : isinf(x) ? FP_INFINITE : (x) == 0.0 ? FP_ZERO : !isnormal(x) ? FP_SUBNORMAL : FP_NORMAL ); }

// Bit Manipulation: fabs, copysign, signbit, nan
#define __jacl_fabs(type,suf,PRE)             static inline type fabs##suf(type x){ return __jacl_signclr_##PRE(x); }
#define __jacl_copysign(type,suf,PRE)         static inline type copysign##suf(type x, type y){ return __jacl_signcpy_##PRE(x, y); }
#define __jacl_signbit(type,suf,PRE)          static inline int __signbit##suf(type x) { return __jacl_signget_##PRE(x); }
#define __jacl_nan(type,suf,PRE)              static inline type __nan##suf(const char *s){ \
	if (!s || !*s) return __jacl_payloadset_##PRE(0, 0); \
	char *p; \
	PRE##_UTYPE payload = (PRE##_UTYPE)strtoul(s, &p, 0); \
	if (p == s) return __jacl_payloadset_##PRE(0, 0); \
	return __jacl_payloadset_##PRE(payload, 0); \
}

// Rounding: rmodes, trunc, ceil, floor, round, rint, nearbyint, roundeven
#define __jacl_rmodes(type,suf,PRE)           static inline type __jacl_rmode##suf(type x, int mode, unsigned int width, int is_unsigned, int notify) { \
	if (!isfinite(x)) { if (width > 0) return (__errno_set(EDOM), 0); return x; } \
	if (width > 0 && (width > 64 || (is_unsigned && x < 0))) return (__errno_set(EDOM), 0); \
	type t, diff, r; \
	if (x == 0 || !(x == x)) { t = x; } \
	else if (fabs##suf(x) >= (type)9223372036854775808.0) { t = x; } \
	else { \
		long long i = (long long)x; \
		t = (type)i; \
		if (t == (type)0) t = x * (type)0.0; \
	} \
	diff = x - t; \
	switch(mode) { \
		case JACL_RND_HIGH: r = (t < x) ? t + (type)1 : t; break; \
		case JACL_RND_DOWN: r = (t > x) ? t - (type)1 : t; break; \
		case JACL_RND_ZERO: r = t; break; \
		case JACL_RND_BANK: \
			if (diff >= (type)0.5) r = t + (type)1; \
			else if (diff <= (type)-0.5) r = t - (type)1; \
			else r = t; break; \
		case JACL_RND_EVEN: \
			if (fabs##suf(diff) > (type)0.5) r = t + (diff > 0 ? 1 : -1); \
			else if (fabs##suf(diff) == (type)0.5 && fmod##suf(t, (type)2) != 0) r = t + (diff > 0 ? 1 : -1); \
			else r = t; break; \
		default: \
			if (width > 0) __errno_set(EDOM); \
			return x; \
	} \
	if (width > 0) { \
		if (is_unsigned) { \
			uintmax_t umax = (width == 64) ? UINTMAX_MAX : ((1ULL << width) - 1); \
			if (r > (type)umax) return (__errno_set(EDOM), 0); \
		} else { \
			intmax_t smin = -(1LL << (width - 1)), smax = (1LL << (width - 1)) - 1; \
			if (r < (type)smin || r > (type)smax) return (__errno_set(EDOM), 0); \
		} \
	} \
	if (notify && r != x) feraiseexcept(FE_INEXACT); \
	return r; \
}
#define __jacl_trunc(type,suf,PRE)            static inline type trunc##suf(type x) { return __jacl_rmode##suf(x, JACL_RND_ZERO, 0, 0, 0); }
#define __jacl_ceil(type,suf,PRE)             static inline type ceil##suf(type x) { return __jacl_rmode##suf(x, JACL_RND_HIGH, 0, 0, 0); }
#define __jacl_floor(type,suf,PRE)            static inline type floor##suf(type x) { return __jacl_rmode##suf(x, JACL_RND_DOWN, 0, 0, 0); }
#define __jacl_round(type,suf,PRE)            static inline type round##suf(type x) { return __jacl_rmode##suf(x, JACL_RND_BANK, 0, 0, 0); }
#define __jacl_rint(type,suf,PRE)             static inline type rint##suf(type x) { return __jacl_rmode##suf(x, fegetround(), 0, 0, 1); }
#define __jacl_nearbyint(type,suf,PRE)        static inline type nearbyint##suf(type x) { return __jacl_rmode##suf(x, fegetround(), 0, 0, 0); }
#define __jacl_roundeven(type,suf,PRE)        static inline type roundeven##suf(type x) { return __jacl_rmode##suf(x, JACL_RND_EVEN, 0, 0, 0); }

// Integer Rounding: fromfp, ufromfp, fromfpx, ufromfpx
#define __jacl_fromfp(type,suf,PRE)           static inline intmax_t fromfp##suf(type x, int round, unsigned int width) { return (intmax_t)__jacl_rmode##suf(x, round, width, 0, 0); }
#define __jacl_ufromfp(type,suf,PRE)          static inline uintmax_t ufromfp##suf(type x, int round, unsigned int width) { return (uintmax_t)__jacl_rmode##suf(x, round, width, 1, 0); }
#define __jacl_fromfpx(type,suf,PRE)          static inline intmax_t fromfpx##suf(type x, int round, unsigned int width) { return (intmax_t)__jacl_rmode##suf(x, round, width, 0, 1); }
#define __jacl_ufromfpx(type,suf,PRE)         static inline uintmax_t ufromfpx##suf(type x, int round, unsigned int width) { return (uintmax_t)__jacl_rmode##suf(x, round, width, 1, 1); }

// Long Rounding: lround, llround, lrint, llrint
#define __jacl_lround(type,suf,PRE)           static inline long lround##suf(type x){ return (long)round##suf(x); }
#define __jacl_llround(type,suf,PRE)          static inline long long llround##suf(type x){ return (long long)round##suf(x); }
#define __jacl_lrint(type,suf,PRE)            static inline long lrint##suf(type x){ return (long)rint##suf(x); }
#define __jacl_llrint(type,suf,PRE)           static inline long long llrint##suf(type x){ return (long long)rint##suf(x); }

// Relation: fdim, fmax, fmin, fma
#define __jacl_fdim(type,suf,PRE)             static inline type fdim##suf(type x, type y){ \
	JACL_SAFETY(isnan(x) || isnan(y), (type)NAN); \
	return x > y ? x - y : (type)0; \
}
#define __jacl_fmax(type,suf,PRE)             static inline type fmax##suf(type x, type y){ return (x < y || isnan(x)) ? y : x; }
#define __jacl_fmin(type,suf,PRE)             static inline type fmin##suf(type x, type y){ return (x > y || isnan(x)) ? y : x; }
#define __jacl_fma(type,suf,PRE)              static inline type fma##suf(type x, type y, type z){ \
	int ex, ey, ez; \
	frexp##suf(x, &ex); frexp##suf(y, &ey); frexp##suf(z, &ez); \
	if(ex > PRE##_MAX_EXP - 2 || ey > PRE##_MAX_EXP - 2) return ((x * (type)0.5) * y + (z * (type)0.5)) * (type)2.0; \
	if(ez > ex + ey) return ((x * (type)2.0) * y + (z * (type)2.0)) * (type)0.5; \
	return x * y + z; \
}

// Exponents: exp, exp2, expm1, log, log2, log10, log1p, pow
#define __jacl_exp(type,suf,PRE) static inline type exp##suf(type x) { \
	JACL_SAFETY(isnan(x), x); \
	JACL_SAFETY(isinf(x), x > 0? x : (type)0); \
	JACL_SAFETY(x == 0, (type)1); \
	JACL_SAFETY(x > PRE##_LOG_MAX - 2.0L, (type)INFINITY); \
	JACL_SAFETY(x < PRE##_LOG_TRUE_MIN, (type)0.0); \
	type k = x * (type)M_LOG2El + (x >= 0 ? .5 : -.5); \
	int ki = (int)k; \
	type r = x - ki * (type)M_LN2l; \
	type r2 = r * r; \
	type r3 = r2 * r; \
	type result = 1.0 + r + r2 / 2.0 + r3 * (1.0 / 6.0 + r * (1.0 / 24.0 + r * (1.0 / 120.0 + r * (1.0 / 720.0 + r / 5040.0)))); \
	return __jacl_expscal_##PRE(result, ki); \
}
#define __jacl_exp2(type,suf,PRE) static inline type exp2##suf(type x) { \
	if (x == (type)(PRE##_MIN_EXP - PRE##_MANT_DIG)) return PRE##_TRUE_MIN; \
	if (x < (type)(PRE##_MIN_EXP - PRE##_MANT_DIG)) return (type)0.0; \
	if (x >= (type)(PRE##_MIN_EXP - PRE##_MANT_DIG) && x <= (type)PRE##_MAX_EXP && (type)(int)x == x) return ldexp##suf((type)1.0, (int)x); \
	JACL_SAFETY(x > (type)(PRE##_MAX_EXP - 1), (type)INFINITY); \
	return exp##suf(x * (type)M_LN2l); \
}
#define __jacl_exp10(type,suf,PRE)            static inline type exp10##suf(type x) { \
	if (x < (type)PRE##_MIN_10_EXP) return 0.0; \
	int i = (int)x; \
	if (x == (type)i) { \
		if (i >= 0 && i <= 15) return (type)POW10[i]; \
		if (i < 0 && i >= -15) return (type)1.0 / (type)POW10[-i]; \
	} \
	return exp##suf(x * (type)M_LN10l); \
}
#define __jacl_expm1(type,suf,PRE)            static inline type expm1##suf(type x){ \
	type ax = fabs##suf(x); \
	if (ax < (type)0.1) return x + (x * x) / (type)2.0 + (x * x * x) / (type)6.0 + (x * x * x * x) / (type)24.0; \
	return exp##suf(x) - (type)1.0; \
}
#define __jacl_log(type, suf, PRE)            static inline type log##suf(type x) { \
	int e; type m, t, s; \
	JACL_SAFETY(x == 1, (type)0); \
	JACL_SAFETY(x < 0, (type)NAN); \
	JACL_SAFETY(x == 0, -((type)INFINITY)); \
	JACL_SAFETY(!isfinite(x), x); \
	m = frexp##suf(x, &e); \
	if (m < (type)M_SQRT1_2l) { m *= (type)2.0; e--; } \
	m = m - (type)1.0; \
	s = m; t = m; \
	int i = 2, n = PRE##_MANT_DIG / 2; \
	for (; i <= n; i++) { t *= -m; s += t / i; } \
	return s + (type)e * (type)M_LN2l; \
}
#define __jacl_log2(type,suf,PRE)             static inline type log2##suf(type x){ return log##suf(x)/(type)M_LN2l; }
#define __jacl_log10(type,suf,PRE)            static inline type log10##suf(type x){ return log##suf(x)/(type)M_LN10l; }
#define __jacl_log1p(type,suf,PRE)            static inline type log1p##suf(type x){ return log##suf(1+x); }
#if JACL_HAS_C99
#define __jacl_pow(type,suf,PRE) static inline type pow##suf(type x, type y) { \
	JACL_SAFETY(isnan(x) || isnan(y), (type)NAN); \
	if (y == 0 || x == 1) return (type)1; \
	if (y == 1) return x; \
	if (y == 2) return x * x; \
	if (y == 3) return x * x * x; \
	if (y == -1) return (type)1 / x; \
	if (y == 0.5) return sqrt##suf(x); \
	if (x == 10.0 && y < (type)PRE##_MIN_10_EXP) return 0.0; \
	if (x < 0 && y != (type)(int)y) return (type)NAN; \
	type ay = fabs##suf(y); \
	int i = (int)ay; \
	if (ay == (type)i && i <= 64) { \
		type result = 1; \
		type base = x; \
		int p = i; \
		while (p > 0) { \
			if (p & 1) result *= base; \
			base *= base; \
			p >>= 1; \
		} \
		return y < 0 ? 1 / result : result; \
	} \
	if (x == 2) return exp2##suf(y); \
	if (x == 10) return exp10##suf(y); \
	type t = y * log##suf(x); \
	JACL_SAFETY(t > PRE##_LOG_MAX, (type)INFINITY); \
	JACL_SAFETY(t < PRE##_LOG_TRUE_MIN, (type)0.0); \
	return exp##suf(t); \
}
#else
#define __jacl_pow(type,suf,PRE) static inline type pow##suf(type x, type y) { \
	JACL_SAFETY(isnan(x) || isnan(y), (type)NAN); \
	if (y == 0 || x == 1) return (type)1; \
	if (y == 1) return x; \
	if (y == 2) return x * x; \
	if (y == 3) return x * x * x; \
	if (y == -1) return (type)1 / x; \
	if (y == 0.5) return sqrt##suf(x); \
	if (x < 0 && y != (type)(int)y) return (type)NAN; \
	type ay = fabs##suf(y); \
	int i = (int)ay; \
	if (ay == (type)i && i <= 64) { \
		type result = 1; \
		type base = x; \
		int p = i; \
		while (p > 0) { \
			if (p & 1) result *= base; \
			base *= base; \
			p >>= 1; \
		} \
		return y < 0 ? 1 / result : result; \
	} \
	type t = y * log##suf(x); \
	JACL_SAFETY(t > PRE##_LOG_MAX, (type)INFINITY); \
	JACL_SAFETY(t < PRE##_LOG_TRUE_MIN, (type)0.0); \
	return exp##suf(t); \
}
#endif /* JACL_HAS_C99 */

// Scaling: ldexp, scalbn, scalbln, logb, ilogb, llogb, frexp
#define __jacl_ldexp(type,suf,PRE)            static inline type ldexp##suf(type x, int n) { return __jacl_expscal_##PRE(x, n); }
#define __jacl_scalbn(type,suf,PRE)           static inline type scalbn##suf(type x, int n) { return ldexp##suf(x, n); }
#define __jacl_scalbln(type,suf,PRE)          static inline type scalbln##suf(type x, long n) { return ldexp##suf(x, (int)n); }
#define __jacl_logb(type,suf,PRE)             static inline type logb##suf(type x) { \
	if (isnan(x)) return (type)NAN; \
	if (x == (type)0) return (type)(-INFINITY); \
	if (isinf(x)) return (type)INFINITY; \
	int e; frexp##suf(x, &e); \
	return (type)(e - 1); \
}
#define __jacl_ilogb(type,suf,PRE)            static inline int ilogb##suf(type x) { \
	if (!(x == x)) return FP_ILOGBNAN; \
	if (x == (type)0) return FP_ILOGB0; \
	return (int)logb##suf(x); \
}
#define __jacl_llogb(type,suf,PRE)            static inline long llogb##suf(type x) { \
	if (!(x == x)) return FP_ILOGBNAN; \
	if (x == (type)0) return FP_ILOGB0; \
	return (long)logb##suf(x); \
}
#define __jacl_frexp(type,suf,PRE) static inline type frexp##suf(type x, int *e) { \
	if (x == 0 || !isfinite(x)) { *e = 0; return x; } \
	type ax = x < 0 ? -x : x; \
	if (ax < PRE##_MIN) { \
		x = __jacl_expscal_##PRE(x, PRE##_MANT_DIG); \
		int exp_normal = __jacl_expfind_##PRE(x) + 1; \
		*e = exp_normal - PRE##_MANT_DIG; \
		return __jacl_expscal_##PRE(x, -exp_normal); \
	} \
	*e = __jacl_expfind_##PRE(x) + 1; \
	return __jacl_expscal_##PRE(x, -(*e)); \
}

// Roots: sqrt, cbrt, hypot
#define __jacl_sqrt(type,suf,PRE)             static inline type sqrt##suf(type x){ \
	JACL_SAFETY(isnan(x), x); \
	JACL_SAFETY(isinf(x) && x > 0, x); \
	JACL_SAFETY(x < 0, (type)NAN); \
	if (x == 0 || x == 1) return x; \
	int e = __jacl_expfind_##PRE(x); \
	type g = __jacl_expscal_##PRE((type)1.0, e/2); \
	for(int i=0;i<4;++i)g=(g+x/g)/2; \
	return g; \
}
#define __jacl_cbrt(type,suf,PRE)             static inline type cbrt##suf(type x){ \
	JACL_SAFETY(isnan(x), x); \
	JACL_SAFETY(isinf(x), x); \
	if (x == 0 || x == 1) return x; \
	if (x < 0) return -exp##suf(log##suf(-x) / 3.0); \
	return exp##suf(log##suf(x) / 3.0); \
}
#define __jacl_hypot(type,suf,PRE)            static inline type hypot##suf(type x,type y){ \
	JACL_SAFETY(isnan(x) || isnan(y), (type)NAN); \
	JACL_SAFETY(isinf(x) || isinf(y), ((type)INFINITY)); \
	type ax = x < 0 ? -x : x, ay = y < 0 ? -y : y, r; \
	if (ax < ay) { type t = ax; ax = ay; ay = t; } \
	if (ax == 0) return 0; \
	if (ax == ay) return ax * (type)M_SQRT2l; \
	r = ay / ax; \
	return ax * sqrt##suf((type)1 + r * r); \
}

// Triganometry: sin, cos, tan, asin, acos, atan, atan2
#define __jacl_sin(type,suf,PRE)              static inline type sin##suf(type x) { \
	JACL_SAFETY(!isfinite(x), (type)NAN);       \
	if (x == 0) return x; \
	type ax = x < 0 ? -x : x; \
	if (ax > (type)M_PIl) x = fmod##suf(x, (type)(2.0 * (type)M_PIl)); \
	type t = x, s = x; int i; \
	for(i = 3; i < 20; i += 2){ t *= -x * x / (i * (i - 1)); s += t; } \
	return s; \
}
#define __jacl_cos(type,suf,PRE)              static inline type cos##suf(type x) { \
	JACL_SAFETY(!isfinite(x), (type)NAN);       \
	if (x == 0) return (type)1; \
	type ax = x < 0 ? -x : x; \
	if (ax > (type)M_PIl) x = fmod##suf(x, (type)(2.0 * (type)M_PIl)); \
	type t = 1.0, s = 1.0, x2 = x * x; int i; \
	for(i = 2; i < 20; i += 2){ t *= -x2 / ((type) i * (i - 1)); s += t; } \
	return s; \
}
#define __jacl_tan(type,suf,PRE)              static inline type tan##suf(type x){return sin##suf(x)/cos##suf(x);}
#define __jacl_asin(type,suf,PRE)             static inline type asin##suf(type x){ \
	int i; \
	JACL_SAFETY(fabs##suf(x) > 1.0, (type)NAN); \
	if(fabs##suf(x) > 0.7) { \
		type sign = (x < 0) ? -1.0 : 1.0, ax = sign * x, z = (1.0 - ax) / 2.0, y = sqrt##suf(z), s=y, t=y, y2=y*y; \
		for(i=1;i<15;i++){ t *= y2 * (2 * i - 1) / (type)(2 * i); s += t / (2 * i + 1); } \
		return sign * ((type)M_PI_2l - 2.0 * s); \
	} \
	type s = x, t = x, x2 = x * x; \
	for(i = 1; i < 15; i++){ t *= x2 * (2 * i - 1) / (type)(2 * i); s += t / (2 * i + 1); } \
	return s; \
}
#define __jacl_acos(type,suf,PRE)              static inline type acos##suf(type x){ return (type)M_PI_2l - asin##suf(x);}
#define __jacl_atan(type,suf,PRE)              static inline type atan##suf(type x) { \
	JACL_SAFETY(isinf(x), x > 0 ? (type)M_PI_2l : (type)-M_PI_2l); \
	if (x == 0) return 0; \
	static const type atanhi[] = { \
		4.63647609000806093515e-01, \
		7.85398163397448278999e-01, \
		9.82793723247329054082e-01, \
		1.57079632679489655800e+00  \
	}; \
	static const type coeffs[] = { \
		 3.33333333333329318027e-01, \
		-1.99999999998764832476e-01, \
		 1.42857142725034663711e-01, \
		-1.11111104054623557880e-01, \
		 9.09088713343650656196e-02, \
		-7.69187620504482999495e-02, \
		 6.66107313738753120669e-02 \
	}; \
	type ax = x < 0 ? -x : x; \
	int id = -1; \
	if (ax >= 2.4375) id = 3, ax = ax > 1e308 ? 0.0 : -1.0 / ax; \
	else if (ax >= 1.1875) id = 2, ax = (ax - 1.5) / (1.0 + 1.5 * ax); \
	else if (ax >= 0.6875) id = 1, ax = (ax - 1.0) / (ax + 1.0); \
	else if (ax >= 0.4375) id = 0, ax = (2.0 * ax - 1.0) / (2.0 + ax); \
	type z = ax * ax, w = z * z; \
	type s = z * (coeffs[0] + w * (coeffs[2] + w * (coeffs[4] + w * coeffs[6]))) + w * (coeffs[1] + w * (coeffs[3] + w * coeffs[5])); \
	type result = (id < 0) ? ax - ax * s : atanhi[id] - (ax * s - ax); \
	return x < 0 ? -result : result; \
}
#define __jacl_atan2(type,suf,PRE)             static inline type atan2##suf(type y,type x){ \
	if (x == 0) { \
		if (y == 0) return __jacl_signget_##PRE(x) ? (__jacl_signget_##PRE(y) ? (type)-M_PIl : (type)M_PIl) : (__jacl_signget_##PRE(y) ? -0.0 : 0.0); \
		return __jacl_signget_##PRE(y) ? (type)-M_PI_2l : (type)M_PI_2l; \
	} \
	type a = atan##suf(y / x); \
	if (__jacl_signget_##PRE(x)) a += __jacl_signget_##PRE(y) ? (type)-M_PIl : (type)M_PIl; \
	return a; \
}

// Hyperbolic: sinh, cosh, tanh, asinh, acosh, atanh
#define __jacl_sinh(type,suf,PRE)              static inline type sinh##suf(type x){ return (exp##suf(x)-exp##suf(-x)) / 2; }
#define __jacl_cosh(type,suf,PRE)              static inline type cosh##suf(type x){ return(exp##suf(x)+exp##suf(-x)) / 2; }
#define __jacl_tanh(type,suf,PRE)              static inline type tanh##suf(type x){ \
		type s = sinh##suf(x); \
		type c = cosh##suf(x); \
		return isinf(c) ? __jacl_signcpy_##PRE((type)1.0, x) : s / c; \
}
#define __jacl_asinh(type,suf,PRE)             static inline type asinh##suf(type x){ \
	JACL_SAFETY(isinf(x), x); \
	return log##suf(x+sqrt##suf(x * x + 1.0)); \
}
#define __jacl_acosh(type,suf,PRE)             static inline type acosh##suf(type x){ \
	JACL_SAFETY(isinf(x) && x > 0, x); \
	if (x < 1.0) return (type)NAN; \
	return log##suf(x + sqrt##suf(x * x - 1.0)); \
}
#define __jacl_atanh(type,suf,PRE)             static inline type atanh##suf(type x){ \
	type ax = x < 0 ? -x : x; \
	return (ax < 1.0) \
		? 0.5 * log##suf((1.0 + x) / (1.0 - x)) \
		: (ax == 1.0) \
		? (x > 0) ? (type)INFINITY \
		: -(type)INFINITY \
		: (type)NAN; \
}

// Error and Gamma: erf, erfc, tgamma, lgamma
#define __jacl_erf(type,suf,PRE)               static inline type erf##suf(type x){ \
	type t = x, s = x; \
	int i; \
	for(i = 1; i < 15; ++i){ t *= -x * x / (type)i; s += t / (type)(2 * i + 1); } \
	return (type)M_2_SQRTPIl * s; \
}
#define __jacl_erfc(type,suf,PRE)              static inline type erfc##suf(type x){ return 1.0 - erf##suf(x); }
#define __jacl_tgamma(type,suf,PRE)            static inline type tgamma##suf(type x){ \
	JACL_SAFETY(x < 0, (type)NAN); \
	if (isinf(x)) return x; \
	if(x == 1.0 || x == 2.0) return 1.0; \
	return sqrt##suf(2.0 * (type)M_PIl / x) * pow##suf(x / (type)M_El, x) * exp##suf(1.0 / (12.0 * x)); \
}
#define __jacl_lgamma(type,suf,PRE)            static inline type lgamma##suf(type x){ \
	if ((x) < 0 && (type)(long)(x) == (x)) return (type)INFINITY; \
	if (isinf(x)) return (type)INFINITY; \
	type gamma_val = tgamma##suf(x); \
	signgam = (gamma_val < 0) ? -1 : 1; \
	return log##suf(fabs##suf(gamma_val)); \
}

#define signgam (*__signgam_ptr())
static inline int* __signgam_ptr(void) { static thread_local int signgam_val = 1;  return &signgam_val; }

// Remainder and Quotient: fmod, remainder, remquo
#define __jacl_fmod(type,suf,PRE)              static inline type fmod##suf(type x, type y){ \
	if (!isfinite(x) || !isfinite(y) || y == 0) return (type)NAN; \
	if (fabs##suf(x) < fabs##suf(y)) return x; \
	type n = trunc##suf(x / y); \
	type r = x - n * y; \
	if (r == 0) return __jacl_signcpy_##PRE((type)0.0, x); \
	return r; \
}
#define __jacl_remainder(type,suf,PRE)         static inline type remainder##suf(type x,type y){ type n = round##suf(x / y); return x - y * n; }
#define __jacl_remquo(type,suf,PRE)            static inline type remquo##suf(type x, type y, int* q){		type qq=round##suf(x / y); *q=(int)qq; return x- y * qq; }

// FP Manipulation: nextafter, nexttoward
#define __jacl_nextafter(type,suf,PRE)         static inline type nextafter##suf(type x,type y){ if (x == y) return x; return __jacl_valnext_##PRE(x, (x < y) ? 1 : -1); }
#define __jacl_nexttoward(type, suf, PRE)      static inline type nexttoward##suf(type x, long double y) { if ((long double)x == y) return (type)y; return __jacl_valnext_##PRE(x, ((long double)x < y) ? 1 : -1); }

//	Decomposition: modf
#define __jacl_modf(type, suf, PRE)            static inline type modf##suf(type x, type* iptr) { \
	*iptr = x; \
	JACL_SAFETY(isnan(x), x); \
	JACL_SAFETY(!isfinite(x), (type)0.0); \
	*iptr = trunc##suf(x); \
	return x - *iptr; \
}

// pi-trig (argument * π or result / π)
#define __jacl_sinpi(type,suf,PRE)             static inline type sinpi##suf(type x){ return sin##suf(x * (type)M_PIl); }
#define __jacl_cospi(type,suf,PRE)             static inline type cospi##suf(type x){ return cos##suf(x * (type)M_PIl); }
#define __jacl_tanpi(type,suf,PRE)             static inline type tanpi##suf(type x){ return tan##suf(x * (type)M_PIl); }
#define __jacl_asinpi(type,suf,PRE)            static inline type asinpi##suf(type x){ return asin##suf(x) / (type)M_PIl; }
#define __jacl_acospi(type,suf,PRE)            static inline type acospi##suf(type x){ return acos##suf(x) / (type)M_PIl; }
#define __jacl_atanpi(type,suf,PRE)            static inline type atanpi##suf(type x){ return atan##suf(x) / (type)M_PIl; }
#define __jacl_atan2pi(type,suf,PRE)           static inline type atan2pi##suf(type y,type x){ return atan2##suf(y, x) / (type)M_PIl; }

// integer/fractional power
#define __jacl_pown(type,suf,PRE)              static inline type pown##suf(type x, int n){ return pow##suf(x, (type)n); }
#define __jacl_powr(type,suf,PRE)              static inline type powr##suf(type x, type y) { \
	if (x < 0) return (type)NAN; \
	if (x == 0 && y <= 0) return (type)NAN; \
	return pow##suf(x, y); \
}

// rootn/compoundn
#define __jacl_rootn(type,suf,PRE)             static inline type rootn##suf(type x, int n){ return pow##suf(x, (type)1.0/n); }
#define __jacl_compoundn(type,suf,PRE)         static inline type compoundn##suf(type x, int n){ return pow##suf((type)1.0 + x, n); }

// nextup/nextdown
#define __jacl_nextup(type,suf,PRE)            static inline type nextup##suf(type x){ return nextafter##suf(x, (type)INFINITY); }
#define __jacl_nextdown(type,suf,PRE)          static inline type nextdown##suf(type x){ return nextafter##suf(x, (type)(-INFINITY)); }

// minimum/maximum
#define __jacl_fmaximum(type,suf,PRE)          static inline type fmaximum##suf(type x, type y){ return isnan(y) ? y : isnan(x) ? x : fmax##suf(x, y); }
#define __jacl_fminimum(type,suf,PRE)          static inline type fminimum##suf(type x, type y){ return isnan(y) ? y : isnan(x) ? x : fmin##suf(x, y); }
#define __jacl_fmaximum_mag(type,suf,PRE)      static inline type fmaximum_mag##suf(type x, type y){ type ax = fabs##suf(x), ay = fabs##suf(y); return isnan(y) ? y : isnan(x) ? x : ax > ay ? x : ay > ax ? y : (x > 0 ? x : y); }
#define __jacl_fminimum_mag(type,suf,PRE)      static inline type fminimum_mag##suf(type x, type y){ type ax = fabs##suf(x), ay = fabs##suf(y); return isnan(y) ? y : isnan(x) ? x : ax < ay ? x : ay < ax ? y : (x > 0 ? x : y); }
#define __jacl_fmaximum_num(type,suf,PRE)      static inline type fmaximum_num##suf(type x, type y){ return isnan(x) ? y : isnan(y) ? x : fmax##suf(x, y); }
#define __jacl_fminimum_num(type,suf,PRE)      static inline type fminimum_num##suf(type x, type y){ return isnan(x) ? y : isnan(y) ? x : fmin##suf(x, y); }
#define __jacl_fmaximum_mag_num(type,suf,PRE)  static inline type fmaximum_mag_num##suf(type x, type y){ return isnan(x) ? y : isnan(y) ? x : fmaximum_mag##suf(x, y); }
#define __jacl_fminimum_mag_num(type,suf,PRE)  static inline type fminimum_mag_num##suf(type x, type y){ return isnan(x) ? y : isnan(y) ? x : fminimum_mag##suf(x, y); }

// NAN Payload: canonicalize, getpayload, setpayload, setpayloadsig
#define __jacl_canonicalize(type,suf,PRE)      static inline int canonicalize##suf(type *cx, const type *x) { if (!cx || !x) return 1; *cx = *x; return 0; }
#define __jacl_getpayload(type,suf,PRE)        static inline type getpayload##suf(const type *x) { if (!x || !isnan(*x)) return (type)-1.0; return (type)__jacl_payloadget_##PRE(*x); }
#define __jacl_setpayload(type,suf,PRE)        static inline int setpayload##suf(type *res, type pl) { \
	if (!res || pl < 0) return 1; \
	uint64_t payload = (uint64_t)pl; \
	if ((type)payload != pl) return 1; \
	*res = __jacl_payloadset_##PRE(payload, 0); \
	return 0; \
}
#define __jacl_setpayloadsig(type,suf,PRE)     static inline int setpayloadsig##suf(type *res, type pl) { \
	if (!res || pl < 0) return 1; \
	uint64_t payload = (uint64_t)pl; \
	if ((type)payload != pl) return 1; \
	*res = __jacl_payloadset_##PRE(payload, 1); \
	return 0; \
}

#define JACL_OP_add +
#define JACL_OP_sub -
#define JACL_OP_mul *
#define JACL_OP_div /

// Narrow Arithmatic: faddf, fsubf, fmulf, faddfl, fsubf, fmulfl, fdivfl, daddl, dsubl, dmull, ddivl
#define __jacl_narrow_fn(name, fn) \
	__jacl_narrow_##fn(name, float, f, f, FLT, double) \
	__jacl_narrow_##fn(name, float, f, l, FLT, long double) \
	__jacl_narrow_##fn(name, double, d, l, DBL, long double)
#define __jacl_narrow_op(name, type, pre, suf, PRE, arg) static inline type pre##name##suf(arg x, arg y) { \
	arg r = x JACL_OP_##name y; \
	if (!isfinite(r)) return (type)r; \
	if (r > (arg)PRE##_MAX) return (type)INFINITY; \
	if (r < (arg)(-PRE##_MAX)) return (type)(-INFINITY); \
	return (type)r; \
}
#define __jacl_narrow_sqrt(name, type, pre, suf, PRE, arg) static inline type pre##name##suf(arg x) { \
	arg r = sqrt##suf(x); \
	if (!isfinite(r)) return (type)r; \
	if (r > (arg)PRE##_MAX) return (type)INFINITY; \
	return (type)r; \
}
#define __jacl_narrow_fma(name, type, pre, suf, PRE, arg) static inline type pre##name##suf(arg x, arg y, arg z) { \
	arg r = fma##suf(x, y, z); \
	if (!isfinite(r)) return (type)r; \
	if (r > (arg)PRE##_MAX) return (type)INFINITY; \
	if (r < (arg)(-PRE##_MAX)) return (type)(-INFINITY); \
	return (type)r; \
}
#define __jacl_msign(name) __jacl_narrow_fn(name, op)
#define __jacl_mnop(name) __jacl_narrow_fn(name, name)

// Array Reductions: reduc_sum, reduce_sumsq, reduc_sumabs, reduc_sumsqmag, reduce_dot
#define __jacl_reduc(name, type, suf, loop_check, operation) static inline type reduc_##name##suf(size_t n, const type p[static n]) { \
	if (n == 0) return (type)0; \
	type sum = (type)0; \
	for (size_t i = 0; i < n; i++) { loop_check; sum += operation; } \
	return sum; \
}
#define __jacl_reduc_sum(type,suf,PRE)           __jacl_reduc(sum, type, suf, if (isinf(p[i])) return p[i]; if (isnan(p[i])) return (type)NAN;, p[i])
#define __jacl_reduc_sumsq(type,suf,PRE)         __jacl_reduc(sumsq, type, suf, if (isinf(p[i])) return (type)INFINITY; if (isnan(p[i])) return (type)NAN;, p[i] * p[i])
#define __jacl_reduc_sumabs(type,suf,PRE)        __jacl_reduc(sumabs, type, suf, if (isinf(p[i])) return (type)INFINITY; if (isnan(p[i])) return (type)NAN;, fabs##suf(p[i]))
#define __jacl_reduc_sumsqmag(type,suf,PRE)      static inline type reduc_sumsqmag##suf(size_t n, const type p[static n]) { return reduc_sumsq##suf(n, p); }
#define __jacl_reduc_dot(type,suf,PRE)           static inline type reduc_dot##suf(size_t n, const type p[static n], const type q[static n]) { \
	if (n == 0) return (type)0; \
	type sum = (type)0; \
	for (size_t i = 0; i < n; i++) { \
		type prod = p[i] * q[i]; \
		if (isinf(prod) && !(isinf(p[i]) || isinf(q[i]))) return (type)NAN; \
		if (isnan(prod)) return (type)NAN; \
		sum += prod; \
	} \
	return sum; \
}

#if JACL_HAS_IMMINTRIN
#include <immintrin.h>

// Square root - hardware is always faster
#undef __jacl_sqrt
#define __jacl_sqrt(type,suf,PRE)                static inline type sqrt##suf(type x){ \
	if (sizeof(type) == sizeof(float)) return (type)_mm_cvtss_f32(_mm_sqrt_ps(_mm_set_ss((float)x))); \
	else if (sizeof(type) == sizeof(double)) return (type)_mm_cvtsd_f64(_mm_sqrt_pd(_mm_set_sd((double)x))); \
	else { \
		type g; int i, e; \
		JACL_SAFETY(x<0, (type)NAN); \
		JACL_SAFETY(x==0||x==1||!isfinite(x), x); \
		e=__jacl_expfind_##PRE(x); \
		g=__jacl_expscal_##PRE((type)1.0,e/2); \
		for(i=0;i<4;++i)g=(g+x/g)/2; \
		return g; \
	} \
}

// Rounding functions (SSE4.1+ or NEON)
#if JACL_HAS_SSE4_1 || JACL_HAS_NEON
#undef __jacl_ceil
#define __jacl_ceil(type,suf,PRE)                static inline type ceil##suf(type x){ \
	if (sizeof(type) == sizeof(float)) return (type)_mm_cvtss_f32(_mm_ceil_ps(_mm_set_ss((float)x))); \
	else if (sizeof(type) == sizeof(double)) return (type)_mm_cvtsd_f64(_mm_ceil_pd(_mm_set_sd((double)x))); \
	else { \
		long long i; type t; \
		if (!(x == x)) return x; \
		i = (long long)x; t = (type)i; \
		if (t < x) t += (type)1; \
		return t; \
	} \
}

#undef __jacl_floor
#define __jacl_floor(type,suf,PRE)               static inline type floor##suf(type x){ \
	if (sizeof(type) == sizeof(float)) return (type)_mm_cvtss_f32(_mm_floor_ps(_mm_set_ss((float)x))); \
	else if (sizeof(type) == sizeof(double)) return (type)_mm_cvtsd_f64(_mm_floor_pd(_mm_set_sd((double)x))); \
	else { \
		long long i; type t; \
		if (!(x == x)) return x; \
		i = (long long)x; t = (type)i; \
		if (t > x) t -= (type)1; \
		return t; \
	} \
}

#undef __jacl_trunc
#define __jacl_trunc(type,suf,PRE)               static inline type trunc##suf(type x){ \
	if (sizeof(type) == sizeof(float)) return (type)_mm_cvtss_f32(_mm_trunc_ps(_mm_set_ss((float)x))); \
	else if (sizeof(type) == sizeof(double)) return (type)_mm_cvtsd_f64(_mm_trunc_pd(_mm_set_sd((double)x))); \
	else { \
		long long i; type ax, t; \
		if(!(x == x) || x == 0) return x; \
		ax = x < (type)0 ? -x : x; \
		if(ax >= (type)(1ULL<<(PRE##_MANT_DIG-1)))return x; \
		i=(long long)x; t = (type)i; \
		return t == ((type)0) ? x * (type)0.0 : t; \
	} \
}

#undef __jacl_round
#define __jacl_round(type,suf,PRE)               static inline type round##suf(type x){ \
	if (sizeof(type) == sizeof(float)) return (type)_mm_cvtss_f32(_mm_round_ps(_mm_set_ss((float)x))); \
	else if (sizeof(type) == sizeof(double)) return (type)_mm_cvtsd_f64(_mm_round_pd(_mm_set_sd((double)x))); \
	else { \
		type t = truncl(x); type diff = x - t; \
		if (diff >= (type)0.5) return t + (type)1; \
		if (diff <= (type)-0.5) return t - (type)1; \
		return t; \
	} \
}
#endif /* SSE4.1 or NEON */

#endif /* JACL_HAS_IMMINTRIN */

// Now declare
__jacl_math4(isnan, isinf, isfinite, isnormal)
__jacl_math3(isunordered, isgreater, isgreaterequal)
__jacl_math4(isless, islessequal, islessgreater, fpclassify)
__jacl_math3(signbit, fabs, fmod)
__jacl_math3(rmodes, ceil, floor)
__jacl_math3(exp, ldexp, frexp)
__jacl_math3(log, log10, sqrt)

#if JACL_HAS_C99
	__jacl_math2(exp2, exp10)
#endif /* JACL_HAS_C99 */

__jacl_math4(pow, sin, cos, tan)
__jacl_math4(asin, acos, atan, atan2)
__jacl_math4(sinh, cosh, tanh, modf)

#if JACL_HAS_C99
	__jacl_math3(nan, copysign, expm1)
	__jacl_math4(log2, logb, ilogb, log1p)
	__jacl_math3(cbrt, hypot, trunc)
	__jacl_math3(round, lround, llround)
	__jacl_math4(rint, lrint, llrint, nearbyint)
	__jacl_math4(fdim, fmax, fmin, fma)
	__jacl_math2(scalbn, scalbln)
	__jacl_math3(asinh, acosh, atanh)
	__jacl_math2(erf, erfc)
	__jacl_math2(tgamma, lgamma)
	__jacl_math4(remainder, remquo, nextafter, nexttoward)
#endif /* JACL_HAS_C99 */

#if JACL_HAS_C23
	__jacl_math4(fromfp, ufromfp, fromfpx, ufromfpx)
	__jacl_math4(llogb, sinpi, cospi, tanpi)
	__jacl_math4(asinpi, acospi, atanpi, atan2pi)
	__jacl_math4(pown, powr, rootn, compoundn)
	__jacl_math3(nextup, nextdown, roundeven)

	__jacl_math4(fmaximum, fmaximum_mag, fmaximum_num, fmaximum_mag_num)
	__jacl_math4(fminimum, fminimum_mag, fminimum_num, fminimum_mag_num)
	__jacl_math4(canonicalize, getpayload, setpayload, setpayloadsig)

	__jacl_msign(add)
	__jacl_msign(sub)
	__jacl_msign(mul)
	__jacl_msign(div)
	__jacl_mnop(sqrt)
	__jacl_mnop(fma)

	__jacl_math4(reduc_sum, reduc_sumsq, reduc_sumabs, reduc_sumsqmag)
	__jacl_math(reduc_dot)
#endif /* JACL_HAS_C23 */

/* Bessel Prototypes */
static inline double j0(double x) {
	JACL_SAFETY(isnan(x), x);
	JACL_SAFETY(isinf(x), 0.0);

	double ax = fabs(x);

	if (ax < 8.0) {
		double y = x * x, n = 57568490574.0 + y * (-13362590354.0 + y * (651619640.7 + y * (-11214424.18 + y * (77392.33017 + y * (-184.9052456)))));
		double d = 57568490411.0 + y * (1029532985.0 + y * (9494680.718 + y * (59272.64853 + y * (267.8532712 + y))));

		return n / d;
	} else {
		double z = 8.0 / ax, y = z * z, xx = ax - M_PI_4, p = 1.0 + y * (-0.1098628627e-2 + y * (0.2734510407e-4 + y * (-0.2073370639e-5 + y * 0.2093887211e-6)));
		double q = -0.1562499995e-1 + y * (0.1430488765e-3 + y * (-0.6911147651e-5 + y * (0.7621095161e-6 - y * 0.934945152e-7)));

		return sqrt(M_2_PI / ax) * (p * cos(xx) - z * q * sin(xx));
	}
}
static inline double j1(double x) {
	JACL_SAFETY(isnan(x), x);
	JACL_SAFETY(isinf(x), 0.0);

	double ax = fabs(x);

	if (ax < 8.0) {
		double y = x * x, n = x * (72362614232.0 + y * (-7895059235.0 + y * (242396853.1 + y * (-2972611.439 + y * (15704.48260 + y * (-30.16036606))))));
		double d = 144725228442.0 + y * (2300535178.0 + y * (18583304.74 + y * (99447.43394 + y * (376.9991397 + y * 1.0))));

		return n / d;
	} else {
		double z = 8.0 / ax, y = z * z, xx = ax - (3.0 * M_PI_4), p = 1.0 + y * (0.183105e-2 + y * (-0.3516396496e-4 + y * (0.2457520174e-5 + y * (-0.240337019e-6))));
		double q = 0.04687499995 + y * (-0.2002690873e-3 + y * (0.8449199096e-5 + y * (-0.88228987e-6 + y * 0.105787412e-6))), ans = sqrt(M_2_PI / ax) * (p * cos(xx) - z * q * sin(xx));

		return x < 0.0 ? -ans : ans;
	}
}
static inline double jn(int n, double x) {
	JACL_SAFETY(isnan(x), x);
	JACL_SAFETY(isinf(x), 0.0);
	if (n == 0) return j0(x);
	if (n == 1) return j1(x);

	double tox = 2.0 / fabs(x), bjm = j0(x), bj = j1(x), bjp;

	for (int j = 1; j < n; j++) { bjp = j * tox * bj - bjm; bjm = bj; bj = bjp; }

	return bj;
}

static inline double y0(double x) {
	JACL_SAFETY(isnan(x), x);
	JACL_SAFETY(x < 0.0, NAN);
	JACL_SAFETY(x == 0.0, -INFINITY);
	JACL_SAFETY(isinf(x), 0.0);

	if (x < 8.0) {
		double j0val = j0(x), y = x * x, num = -2957821389.0 + y * (7062834065.0 + y * (-512359803.6 + y * (10879881.29 + y * (-86327.92757 + y * 228.4622733))));
		double den = 40076544269.0 + y * (745249964.8 + y * (7189466.438 + y * (47447.26470 + y * (226.1030244 + y * 1.0))));

		return num / den + M_2_PI * log(x) * j0val;
	} else {
		double z = 8.0/x, y = z * z, xx = x - M_PI_4, p = 1.0 + y * (-0.1098628627e-2 + y * (0.2734510407e-4 + y * (-0.2073370639e-5 + y * 0.2093887211e-6)));
		double q = -0.1562499995e-1 + y * (0.1430488765e-3 + y * (-0.6911147651e-5 + y * (0.7621095161e-6 - y * 0.934945152e-7)));

		return sqrt(M_2_PI / x) * (p * sin(xx) + z*q*cos(xx));
	}
}
static inline double y1(double x) {
	JACL_SAFETY(isnan(x), x);
	JACL_SAFETY(x < 0.0, NAN);
	JACL_SAFETY(x == 0.0, -INFINITY);
	JACL_SAFETY(isinf(x), 0.0);

	if (x < 8.0) {
		double j1val = j1(x), y = x * x, num = x * (-0.4900604943e13 + y * (0.1275274390e13 + y * (-0.5153438139e11 + y * (0.7349264551e9 + y * (-0.4237922726e7 + y * 0.8511937935e4)))));
		double den = 0.2499580570e14 + y * (0.4244419664e12 + y * (0.3733650367e10 + y * (0.2245904002e8 + y * (0.1020426050e6 + y * (0.3549632885e3 + y * 1.0)))));

		return num / den + M_2_PI * (j1val * log(x) - 1.0/x);
	} else {
		double z = 8.0/x, y = z * z, xx = x - (3.0 * M_PI_4), p = 1.0 + y * (0.183105e-2 + y * (-0.3516396496e-4 + y * (0.2457520174e-5 + y * (-0.240337019e-6))));
		double q = 0.04687499995 + y * (-0.2002690873e-3 + y * (0.8449199096e-5 + y * (-0.88228987e-6 + y * 0.105787412e-6)));

		return sqrt(M_2_PI/x) * (p*sin(xx) + z*q*cos(xx));
	}
}
static inline double yn(int n, double x) {
	JACL_SAFETY(isnan(x), x);
	JACL_SAFETY(x < 0.0, NAN);
	JACL_SAFETY(x == 0.0, -INFINITY);
	JACL_SAFETY(isinf(x), 0.0);
	if (n == 0) return y0(x);
	if (n == 1) return y1(x);

	double tox = 2.0/x, by = y1(x), bym = y0(x), byp;

	for (int j = 1; j < n; j++) { byp = j * tox * by - bym; bym = by; by = byp; }

	return by;
}

#ifdef __cplusplus
}
#endif

#endif /* _MATH_H */
