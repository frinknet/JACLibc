// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef MATH_H
#define MATH_H
#pragma once

#include <fenv.h>
#include <stdint.h>
#include <limits.h>
#include <float.h>
#include <errno.h>

// Add static assertions here (after includes, before extern "C")
_Static_assert(sizeof(float) == 4, "Assumes 32-bit IEEE-754 float");
_Static_assert(sizeof(double) == 8, "Assumes 64-bit IEEE-754 double");
_Static_assert(FLT_MANT_DIG == 24, "Assumes IEEE-754 float mantissa");
_Static_assert(DBL_MANT_DIG == 53, "Assumes IEEE-754 double mantissa");

#ifdef __cplusplus
extern "C" {
#endif

// Builtin Value
#define HUGE_VAL	 (1.0/0.0)				/* double  +∞  */
#define HUGE_VALF  (1.0f/0.0f)			/* float	 +∞  */
#define HUGE_VALL  (1.0L/0.0L)			/* long-double +∞  */
#define INFINITY	 HUGE_VALF				/* Of type float */
#define NAN				 (0.0f/0.0f)			/* quiet NaN, type float */

// Constants
#define FP_INFINITE		 1
#define FP_NAN				 2
#define FP_NORMAL			 3
#define FP_SUBNORMAL	 4
#define FP_ZERO				 5

#define FP_ILOGB0			 INT_MIN
#define FP_ILOGBNAN		 INT_MIN

#define MATH_ERRNO		 1
#define MATH_ERREXCEPT 2
#define math_errhandling (MATH_ERRNO | MATH_ERREXCEPT)

#define M_E					2.71828182845904523536
#define M_LOG2E			1.44269504088896340736
#define M_LOG10E		0.43429448190325182765
#define M_LN2				0.69314718055994530942
#define M_LN10			2.30258509299404568402
#define M_PI				3.14159265358979323846
#define M_PI_2			1.57079632679489661923
#define M_PI_4			0.78539816339744830962
#define M_1_PI			0.31830988618379067154
#define M_2_PI			0.63661977236758134308
#define M_2_SQRTPI	1.12837916709551257390
#define M_SQRT2			1.41421356237309504880
#define M_SQRT1_2		0.70710678118654752440

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
	// Assume usual case
	typedef float float_t;
	typedef double double_t;
#endif

// Classification via macros
#define isnan(x)				 ((x) != (x))
#define isinf(x)				 (!isnan(x) && isnan((x)-(x)))
#define isfinite(x)			 (!isnan(x) && !isinf(x))
#define isunordered(x,y) (isnan(x) || isnan(y))

#define signbit(x) ((int)(((const unsigned char*)&(x))[sizeof(x)-1] >> 7))
#define isnormal(x)  (__jacl_isnormal(x))
static inline int __jacl_isnormal(double x) {
		double ax=x<0?-x:x; return ax>=DBL_MIN && ax<DBL_MAX;
}
#define fpclassify(x) __jacl_fpclassify(x)
static inline int __jacl_fpclassify(double x) {
		if (isnan(x)) return FP_NAN;
		if (isinf(x)) return FP_INFINITE;
		if (x==0.0) return FP_ZERO;
		if (!isnormal(x)) return FP_SUBNORMAL;
		return FP_NORMAL;
}
#define isgreater(x, y)			 (!isunordered(x, y) && (x) >  (y))
#define isgreaterequal(x, y) (!isunordered(x, y) && (x) >= (y))
#define isless(x, y)				 (!isunordered(x, y) && (x) <  (y))
#define islessequal(x, y)		 (!isunordered(x, y) && (x) <= (y))
#define islessgreater(x, y)  (!isunordered(x, y) && ((x) < (y) || (x) > (y)))

// Helpers: Macro Generator
#define __jacl_math(F) \
	__jacl_##F(double,			) \
	__jacl_##F(float,				f) \
	__jacl_##F(long double, l)

#define __jacl_mbit(F) \
	__jacl_##F(double,			, 52) \
	__jacl_##F(float,				f, 23) \
	__jacl_##F(long double, l, 52)

#define __jacl_umeta(F) \
	__jacl_##F(double,			, uint64_t, 0x8000000000000000ULL, 0x7FFFFFFFFFFFFFFFULL) \
	__jacl_##F(float,				f, uint32_t, 0x80000000U,					 0x7FFFFFFFU) \
	__jacl_##F(long double, l, uint64_t, 0x8000000000000000ULL, 0x7FFFFFFFFFFFFFFFULL)

#define __jacl_unext(F) \
	__jacl_##F(double,			, uint64_t) \
	__jacl_##F(float,				f, uint32_t) \
	__jacl_##F(long double, l, uint64_t)

// Definitions: nan, fabs, copysign
#define __jacl_nan(type, suf) static inline type nan##suf(const char*s){(void)s; __jacl_f64u u={.u=0x7ff8000000000000ULL}; return (type)u.f;}
#define __jacl_fabs(type, suf) static inline type fabs##suf(type x){ type y=x<(type)0?-x:x; return y==(type)0?(type)0:y;}
#define __jacl_copysign(type,suf,UT,SIGN,ABS) static inline type copysign##suf(type x, type y){ union{type f; UT u;} a={x}, b={y}; a.u = (a.u & (UT)(ABS)) | (b.u & (UT)(SIGN)); return a.f; }

// Rounding: ceil, floor, trunc, round, rint, nearbyint
#define __jacl_ceil(type, suf, mant_bits) static inline type ceil##suf(type x) { type t = trunc##suf(x); return (t < x) ? t + (type)1 : t; }
#define __jacl_floor(type, suf, mant_bits) static inline type floor##suf(type x) { type t = trunc##suf(x); return (t > x) ? t - (type)1 : t; }
#define __jacl_trunc(type, suf, mant_bits)static inline type trunc##suf(type x) { \
	if (!(x == x)) return x; \
	type ax = x < (type)0 ? -x : x; \
	if (ax >= (type)(1ULL << (mant_bits))) return x; \
	long long i = (long long)x; \
	type t = (type)i; \
	/* preserve sign of zero */ \
	return t == (type)0 ? x * (type)0.0 : t; \
}
#define __jacl_round(type, suf, mant_bits) \
static inline type round##suf(type x) { \
	type t = trunc##suf(x); \
	type diff = x - t; \
	if (diff > (type)0.5 || (diff == (type)0.5 && fmod##suf(t, (type)2) != (type)0)) \
		t += (x > (type)0 ? (type)1 : (type)-1); \
	return t; \
}
#define __jacl_rint(type, suf) static inline type rint##suf(type x){return round##suf(x);}
#define __jacl_nearbyint(type, suf) static inline type nearbyint##suf(type x){return round##suf(x);}

// Relation: fdim, fmax, fmin, fma
#define __jacl_fdim(type, suf) static inline type fdim##suf(type x, type y){ return x>y ? x-y : (type)0; }
#define __jacl_fmax(type, suf) static inline type fmax##suf(type x, type y){ return (x < y || isnan(x)) ? y : x; }
#define __jacl_fmin(type, suf) static inline type fmin##suf(type x, type y){ return (x > y || isnan(x)) ? y : x; }
#define __jacl_fma(type, suf) static inline type fma##suf(type x,type y,type z){ return x*y+z; }

// Exponents: exp, exp2, 2xpm1, log, log2, log10, log1p, pow
#define __jacl_exp(type,suf) static inline type exp##suf(type x){ type s=1.0, t=1.0; for (int i=1;i<20;++i){t*=x/i;s+=t;} return s; }
#define __jacl_exp2(type,suf) static inline type exp2##suf(type x){ return exp##suf(x*M_LN2); }
#define __jacl_expm1(type,suf) static inline type expm1##suf(type x){ return exp##suf(x)-1.0; }
#define __jacl_log(type,suf) static inline type log##suf(type x){ if (x<=0.0) return (type)NAN; type y = (x-1.0)/(x+1.0), t = y*y, s = y; for (int i=3;i<20;i+=2){y*=t;s+=y/i;}return 2.0*s; }
#define __jacl_log2(type,suf) static inline type log2##suf(type x){ return log##suf(x)/M_LN2; }
#define __jacl_log10(type,suf) static inline type log10##suf(type x){ return log##suf(x)/M_LN10; }
#define __jacl_log1p(type,suf) static inline type log1p##suf(type x){ return log##suf(1+x); }
#define __jacl_pow(type,suf) static inline type pow##suf(type x,type y){ return exp##suf(y*log##suf(x)); }

// Scaling: ldexp, scalbn, scalbln, logb, ilogb, frexp
#define __jacl_ldexp(type,suf) static inline type ldexp##suf(type x, int e) { return x * exp2##suf((type)e); }
#define __jacl_scalbn(type,suf) static inline type scalbn##suf(type x, int n) { return ldexp##suf(x, n); }
#define __jacl_scalbln(type,suf) static inline type scalbln##suf(type x, long n) { return ldexp##suf(x, (int)n); }
#define __jacl_logb(type,suf) static inline type logb##suf(type x) { \
	if (!(x == x)) return (type)NAN; \
	if (x == (type)0) return (type)(-INFINITY); \
	if (!isfinite(x)) return (type)INFINITY; \
	type ax = x < (type)0 ? -x : x; \
	return floor##suf(log2##suf(ax)); \
}
#define __jacl_ilogb(type,suf) static inline int ilogb##suf(type x) { if (!(x == x)) return FP_ILOGBNAN; if (x == (type)0) return FP_ILOGB0; return (int)logb##suf(x); }
#define __jacl_frexp(type,suf) static inline type frexp##suf(type x, int *e) { \
	if (x == (type)0 || !(x == x) || !isfinite(x)) { *e = 0; return x; } \
	int k = (int)floor##suf(log2##suf((x < (type)0 ? -x : x))); \
	type m = x * exp2##suf((type)-k); \
	if (fabs##suf(m) < (type)0.5) { m *= (type)2; --k; } \
	*e = k + 1; \
	return m; \
}

// Roots: sqrt, cbrt, hypot
#define __jacl_sqrt(type,suf) static inline type sqrt##suf(type x){	if(x<0) return (type)NAN; type g=x; for(int i=0;i<8;++i) g=(g+x/g)/2; return g; }
#define __jacl_cbrt(type,suf) static inline type cbrt##suf(type x){return pow##suf(x,1.0/3.0);}
#define __jacl_hypot(type,suf) static inline type hypot##suf(type x,type y){return sqrt##suf(x*x+y*y);}

// Triganometry: sin, cos, tan, asin, acos, atan, atan2
#define __jacl_sin(type,suf) static inline type sin##suf(type x){ type t=x, s=x; for (int i=3;i<20;i+=2){t*=-x*x/(i*(i-1));s+=t;} return s; }
#define __jacl_cos(type,suf) static inline type cos##suf(type x){return sin##suf(x+M_PI_2);}
#define __jacl_tan(type,suf) static inline type tan##suf(type x){return sin##suf(x)/cos##suf(x);}
#define __jacl_asin(type,suf) static inline type asin##suf(type x){ if(fabs##suf(x)>1.0) return (type)NAN; type s=x, t=x; for (int i=3;i<20;i+=2){t*=x*x*(i-2)/(i*(i-1));s+=t/i;} return s; }
#define __jacl_acos(type,suf) static inline type acos##suf(type x){ return M_PI_2-asin##suf(x);}
#define __jacl_atan(type,suf) static inline type atan##suf(type x){ return asin##suf(x/sqrt##suf(1.0+x*x)); }
#define __jacl_atan2(type,suf) static inline type atan2##suf(type y,type x){ if(x==0) return copysign##suf(M_PI_2,y); type a=atan##suf(y/x); if(x<0)a+=(y>=0?M_PI:-M_PI); return a; }

// Hyperbolic: sinh, cosh, tanh, asinh, acosh, atanh
#define __jacl_sinh(type,suf) static inline type sinh##suf(type x){return(exp##suf(x)-exp##suf(-x))/2;}
#define __jacl_cosh(type,suf) static inline type cosh##suf(type x){return(exp##suf(x)+exp##suf(-x))/2;}
#define __jacl_tanh(type,suf) static inline type tanh##suf(type x){ return sinh##suf(x)/cosh##suf(x);}
#define __jacl_asinh(type,suf) static inline type asinh##suf(type x){ return log##suf(x+sqrt##suf(x*x+1.0)); }
#define __jacl_acosh(type,suf) static inline type acosh##suf(type x){ return log##suf(x+sqrt##suf(x*x-1.0)); }
#define __jacl_atanh(type,suf) static inline type atanh##suf(type x){ return 0.5*log##suf((1.0+x)/(1.0-x)); }

// Error and Gamma: erf, erfc, tgamma, lgamma
#define __jacl_erf(type,suf) static inline type erf##suf(type x){		type t=1.0,s=x; for(int i=1;i<10;++i){t*=-x*x/i;s+=t/(2*i+1);} return (2.0/sqrt##suf(M_PI))*s;}
#define __jacl_erfc(type,suf) static inline type erfc##suf(type x){ return 1.0-erf##suf(x); }
#define __jacl_tgamma(type,suf) static inline type tgamma##suf(type x){		if(x<0) return (type)NAN; if(x==1.0||x==2.0) return 1.0;	return sqrt##suf(2.0*M_PI/x)*pow##suf(x/M_E,x)*exp##suf(1.0/(12.0*x)); }
#define __jacl_lgamma(type,suf) static inline type lgamma##suf(type x){ return log##suf(tgamma##suf(x)); }

// Remainder and Quotient: fmod, remainder, remquo
#define __jacl_fmod(type,suf) static inline type fmod##suf(type x,type y){ return x-y*trunc##suf(x/y);}
#define __jacl_remainder(type,suf) static inline type remainder##suf(type x,type y){return x-y*round##suf(x/y);}
#define __jacl_remquo(type,suf) static inline type remquo##suf(type x,type y,int* q){		type qq=round##suf(x/y); *q=(int)qq; return x-y*qq; }

/* FP Manipulation: nextafter, nexttoward */
#define __jacl_nextafter(type,suf,UT) static inline type nextafter##suf(type x, type y){	if (x==y) return x;		union{type f; UT u;} a={x};		if (x<y) ++a.u; else --a.u;		return a.f; }
#define __jacl_nexttoward(type,suf) static inline type nexttoward##suf(type x, long double y){ return nextafter##suf(x,(type)y); }

//	Decomposition: modf
#define __jacl_modf(type,suf) static inline type modf##suf(type x, type* iptr){		type t = trunc##suf(x);		*iptr = t;	return x - t; }

#ifdef USE_FAST_MATH

/* Estrin-evaluated minimax exp for double */
#undef __jacl_exp
#define __jacl_exp(type,suf) static inline type exp##suf(type x) { \
	/* range-reduce x = n·ln2 + r, r∈[−½ln2,½ln2] */ \
	int n = (int)floor##suf(x * M_LOG2E + (x>0?0.5:-0.5)); \
	type r = x - (type)n * M_LN2; \
	type r2 = r*r; \
	/* coeffs e0–e5 precomputed for double */ \
	const type e0 = 1.0, e1 = 1.0, e2 = 0.5, e3 = 1.0/6, e4 = 1.0/24, e5 = 1.0/120; \
	type A = e0 + r2*(e2 + r2*e4); \
	type B = r*(e1 + r2*(e3 + r2*e5)); \
	type y = A + B; \
	/* fast pow2 via ldexp */ \
	return ldexp##suf(y, n); \
}

/* Estrin-evaluated minimax sin for double */
#undef __jacl_sin
#define __jacl_sin(type,suf) static inline type sin##suf(type x) { \
	/* range-reduce to r∈[−π/2,π/2] */ \
	int q; \
	type r = remquo##suf(x, M_PI, &q); \
	if (q & 1) r = (q&2)? -r : (M_PI - r); \
	type r2 = r*r; \
	/* coeffs s1–s5 precomputed for double */ \
	const type s1 = -1.0/6, s2 = +1.0/120, s3 = -1.0/5040, s4 = +1.0/362880, \
						 s5 = -1.0/39916800; \
	/* Estrin grouping */ \
	type E0 = 1 + r2*(s1 + r2*s2); \
	type E1 = r2*(s3 + r2*s4) + r2*r5*s5; \
	return r * (E0 + E1); \
}

#endif /* USE_FAST_MATH */

#ifdef USE_SIMD
#include <immintrin.h>

// Trig
#undef __jacl_tan
#define __jacl_tan(type,suf) static inline type tan##suf(type x) { return _mm_cvtss_f32(_mm_tan_ps(_mm_set_ss((float)x))); }

// Hyperbolic
#undef __jacl_sinh
#define __jacl_sinh(type,suf) static inline type sinh##suf(type x) { return _mm_cvtss_f32(_mm_sinh_ps(_mm_set_ss((float)x))); }
#undef __jacl_cosh
#define __jacl_cosh(type,suf) static inline type cosh##suf(type x) { return _mm_cvtss_f32(_mm_cosh_ps(_mm_set_ss((float)x))); }

// Exp/log
#undef __jacl_log
#define __jacl_log(type,suf) static inline type log##suf(type x) {	return _mm_cvtsd_f64(_mm_log_pd(_mm_set_sd((double)x))); }

// Power, sqrt, hypot
#undef __jacl_sqrt
#define __jacl_sqrt(type,suf) static inline type sqrt##suf(type x) { return _mm_cvtsd_f64(_mm_sqrt_pd(_mm_set_sd((double)x))); }
#undef __jacl_hypot
#define __jacl_hypot(type,suf) static inline type hypot##suf(type x,type y) { __m128d a = _mm_set_sd(x), b = _mm_set_sd(y); return _mm_cvtsd_f64(_mm_hypot_pd(a,b)); }

// Special functions
#undef __jacl_pow
#define __jacl_pow(type,suf) static inline type pow##suf(type x,type y) { __m128d a = _mm_set_sd(x), b = _mm_set_sd(y); return _mm_cvtsd_f64(_mm_pow_pd(a,b)); }
#undef __jacl_erf
#define __jacl_erf(type,suf) static inline type erf##suf(type x) { return _mm_cvtss_f32(_mm_erf_ps(_mm_set_ss((float)x))); }
#undef __jacl_tgamma
#define __jacl_tgamma(type,suf) static inline type tgamma##suf(type x) { return _mm_cvtss_f32(_mm_tgamma_ps(_mm_set_ss((float)x))); }

#endif /* USE_SIMD */

// Now declare
__jacl_math(nan)
__jacl_math(fabs)
__jacl_umeta(copysign)

__jacl_mbit(trunc)
__jacl_math(fmod)

__jacl_mbit(ceil)
__jacl_mbit(floor)
__jacl_mbit(round)

__jacl_math(rint)
__jacl_math(nearbyint)
__jacl_math(fdim)
__jacl_math(fmax)
__jacl_math(fmin)
__jacl_math(fma)

__jacl_math(exp)
__jacl_math(exp2)
__jacl_math(expm1)
__jacl_math(log)
__jacl_math(log2)
__jacl_math(log10)
__jacl_math(log1p)
__jacl_math(pow)

__jacl_math(ldexp)
__jacl_math(scalbn)
__jacl_math(scalbln)
__jacl_math(logb)
__jacl_math(ilogb)
__jacl_math(frexp)

__jacl_math(sqrt)
__jacl_math(cbrt)
__jacl_math(hypot)

__jacl_math(sin)
__jacl_math(cos)
__jacl_math(tan)
__jacl_math(asin)
__jacl_math(acos)
__jacl_math(atan)
__jacl_math(atan2)

__jacl_math(sinh)
__jacl_math(cosh)
__jacl_math(tanh)
__jacl_math(asinh)
__jacl_math(acosh)
__jacl_math(atanh)

__jacl_math(erf)
__jacl_math(erfc)
__jacl_math(tgamma)
__jacl_math(lgamma)

__jacl_math(remainder)
__jacl_math(remquo)

__jacl_unext(nextafter)
__jacl_math(nexttoward)
__jacl_math(modf)

// Integer Rounding
static inline long lround(double x){ return (long)round(x); }
static inline long long llround(double x){ return (long long)round(x); }
static inline long lrint(double x){ return (long)rint(x); }
static inline long long llrint(double x){ return (long long)rint(x);}

// Bessel Prototypes
double j0(double x);
double j1(double x);
double jn(int n, double x);
double y0(double x);
double y1(double x);
double yn(int n, double x);

// signgam for lgamma
extern int signgam;

#ifdef __cplusplus
}
#endif

#endif /* MATH_H */
