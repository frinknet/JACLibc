// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef TGMATH_H
#define TGMATH_H

#include <math.h>
#include <complex.h>

/*
 * C99/C11 type-generic math macros:
 * float, double, long double,
 * float complex, double complex, long double complex.
 */

/* Elementary & classification */
#define fabs(x)				 _Generic((x),\
		float: fabsf, double: fabs, long double: fabsl,\
		float complex: cabsf, double complex: cabs, long double complex: cabsl)(x)
#define sqrt(x)				 _Generic((x),\
		float: sqrtf, double: sqrt, long double: sqrtl,\
		float complex: csqrtf, double complex: csqrt, long double complex: csqrtl)(x)
#define cbrt(x)				 _Generic((x),\
		float: cbrtf, double: cbrt, long double: cbrtl)(x)
#define exp(x)				 _Generic((x),\
		float: expf, double: exp, long double: expl,\
		float complex: cexpf, double complex: cexp, long double complex: cexpl)(x)
#define exp2(x)				 _Generic((x),\
		float: exp2f, double: exp2, long double: exp2l)(x)
#define expm1(x)			 _Generic((x),\
		float: expm1f, double: expm1, long double: expm1l)(x)
#define log(x)				 _Generic((x),\
		float: logf, double: log, long double: logl,\
		float complex: clogf, double complex: clog, long double complex: clogl)(x)
#define log10(x)			 _Generic((x),\
		float: log10f, double: log10, long double: log10l)(x)
#define log1p(x)			 _Generic((x),\
		float: log1pf, double: log1p, long double: log1pl)(x)
#define log2(x)				 _Generic((x),\
		float: log2f, double: log2, long double: log2l)(x)
#define logb(x)				 _Generic((x),\
		float: logbf, double: logb, long double: logbl)(x)
#define ilogb(x)			 _Generic((x),\
		float: ilogbf, double: ilogb, long double: ilogbl)(x)
#define frexp(x,e)		 _Generic((x),\
		float: frexpf, double: frexp, long double: frexpl)(x,e)
#define ldexp(x,e)		 _Generic((x),\
		float: ldexpf, double: ldexp, long double: ldexpl)(x,e)

/* Trigonometry */
#define sin(x)				 _Generic((x),\
		float: sinf, double: sin, long double: sinl,\
		float complex: csinf, double complex: csin, long double complex: csinl)(x)
#define cos(x)				 _Generic((x),\
		float: cosf, double: cos, long double: cosl,\
		float complex: ccosf, double complex: ccos, long double complex: ccosl)(x)
#define tan(x)				 _Generic((x),\
		float: tanf, double: tan, long double: tanl,\
		float complex: ctanf, double complex: ctan, long double complex: ctanl)(x)
#define asin(x)				 _Generic((x),\
		float: asinf, double: asin, long double: asinl,\
		float complex: casinf, double complex: casin, long double complex: casinl)(x)
#define acos(x)				 _Generic((x),\
		float: acosf, double: acos, long double: acosl,\
		float complex: cacosf, double complex: cacos, long double complex: cacosl)(x)
#define atan(x)				 _Generic((x),\
		float: atanf, double: atan, long double: atanl,\
		float complex: catanf, double complex: catan, long double complex: catanl)(x)
#define atan2(y,x)		 _Generic((y)+(x),\
		float: atan2f, double: atan2, long double: atan2l)(y,x)

/* Hyperbolic */
#define sinh(x)				 _Generic((x),\
		float: sinhf, double: sinh, long double: sinhl,\
		float complex: csinhf, double complex: csinh, long double complex: csinhl)(x)
#define cosh(x)				 _Generic((x),\
		float: coshf, double: cosh, long double: coshl,\
		float complex: ccoshf, double complex: ccosh, long double complex: ccoshl)(x)
#define tanh(x)				 _Generic((x),\
		float: tanhf, double: tanh, long double: tanhl,\
		float complex: ctanhf, double complex: ctanh, long double complex: ctanhl)(x)
#define asinh(x)			 _Generic((x),\
		float: asinhf, double: asinh, long double: asinhl,\
		float complex: casinhf, double complex: casinh, long double complex: casinhl)(x)
#define acosh(x)			 _Generic((x),\
		float: acoshf, double: acosh, long double: acoshl,\
		float complex: cacoshf, double complex: cacosh, long double complex: cacoshl)(x)
#define atanh(x)			 _Generic((x),\
		float: atanhf, double: atanh, long double: atanhl,\
		float complex: catanhf, double complex: catanh, long double complex: catanhl)(x)

/* Special */
#define cbrt(x)				 _Generic((x),\
		float: cbrtf, double: cbrt, long double: cbrtl)(x)
#define hypot(x,y)		 _Generic((x)+(y),\
		float: hypotf, double: hypot, long double: hypotl,\
		float complex: chypotf, double complex: chypot, long double complex: chypotl)(x,y)
#define erf(x)				 _Generic((x),\
		float: erff, double: erf, long double: erfl,\
		float complex: cerff, double complex: cerf, long double complex: cerfl)(x)
#define erfc(x)				 _Generic((x),\
		float: erfcf, double: erfc, long double: erfcl,\
		float complex: cerfcf, double complex: cerfc, long double complex: cerfcl)(x)
#define tgamma(x)			 _Generic((x),\
		float: tgammaf, double: tgamma, long double: tgammal,\
		float complex: ctgammaf, double complex: ctgamma, long double complex: ctgammal)(x)
#define lgamma(x)			 _Generic((x),\
		float: lgammaf, double: lgamma, long double: lgammal)(x)

/* Rounding & remainders */
#define ceil(x)				 _Generic((x), float: ceilf, double: ceil, long double: ceill)(x)
#define floor(x)			 _Generic((x), float: floorf, double: floor, long double: floorl)(x)
#define trunc(x)			 _Generic((x), float: truncf, double: trunc, long double: truncl)(x)
#define round(x)			 _Generic((x), float: roundf, double: round, long double: roundl)(x)
#define nearbyint(x)	 _Generic((x), float: nearbyintf, double: nearbyint, long double: nearbyintl)(x)
#define rint(x)				 _Generic((x), float: rintf, double: rint, long double: rintl)(x)
#define lrint(x)			 _Generic((x), float: lrintf, double: lrint, long double: lrintl)(x)
#define llrint(x)			 _Generic((x), float: llrintf, double: llrint, long double: llrintl)(x)
#define lround(x)			 _Generic((x), float: lroundf, double: lround, long double: lroundl)(x)
#define llround(x)		 _Generic((x), float: llroundf, double: llround, long double: llroundl)(x)

#define fmod(x,y)			 _Generic((x)+(y),\
		float: fmodf, double: fmod, long double: fmodl)(x,y)
#define remainder(x,y) _Generic((x)+(y),\
		float: remainderf, double: remainder, long double: remainderl)(x,y)
#define remquo(x,y,q)  _Generic((x)+(y),\
		float: remquof, double: remquo, long double: remquol)(x,y,q)

/* Floating-point manipulation */
#define scalbn(x,n)		 _Generic((x),\
		float: scalbnf, double: scalbn, long double: scalbnl)(x,n)
#define scalbln(x,n)	 _Generic((x),\
		float: scalblnf, double: scalbln, long double: scalblnl)(x,n)
#define copysign(x,y)  _Generic((x)+(y),\
		float: copysignf, double: copysign, long double: copysignl)(x,y)
#define nextafter(x,y) _Generic((x)+(y),\
		float: nextafterf, double: nextafter, long double: nextafterl)(x,y)
#define nexttoward(x,y)_Generic((x),\
		float: nexttowardf, double: nexttoward, long double: nexttowardl)(x,y)

/* Complex-only accessors */
#define carg(z)				 _Generic((z),\
		float complex: cargf, double complex: carg, long double complex: cargl)(z)
#define cimag(z)			 _Generic((z),\
		float complex: cimagf, double complex: cimag, long double complex: cimagl)(z)
#define creal(z)			 _Generic((z),\
		float complex: crealf, double complex: creal, long double complex: creall)(z)
#define conj(z)				 _Generic((z),\
		float complex: conjf, double complex: conj, long double complex: conjl)(z)
#define cproj(z)			 _Generic((z),\
		float complex: cprojf, double complex: cproj, long double complex: cprojl)(z)

/* Power, min/max & fused multiply-add */
#define fdim(x,y)		_Generic((x)+(y),\
		float: fdimf, double: fdim, long double: fdiml)(x,y)
#define fmax(x,y)		_Generic((x)+(y),\
		float: fmaxf, double: fmax, long double: fmaxl)(x,y)
#define fmin(x,y)		_Generic((x)+(y),\
		float: fminf, double: fmin, long double: fminl)(x,y)
#define fma(x,y,z)	_Generic((x)+(y)+(z),\
		float: fmaf, double: fma, long double: fmal)(x,y,z)

/* Classification & comparison (10 macros) */
#define fpclassify(x)				 _Generic((x),\
		float: fpclassifyf, double: fpclassify, long double: fpclassifyl)(x)
#define isfinite(x)					 _Generic((x),\
		float: isfinitef, double: isfinite, long double: isfinitel)(x)
#define isnormal(x)					 _Generic((x),\
		float: isnormalf, double: isnormal, long double: isnormall)(x)
#define isnan(x)						 _Generic((x),\
		float: isnanf, double: isnan, long double: isnanl)(x)
#define isinf(x)						 _Generic((x),\
		float: isinff, double: isinf, long double: isinfl)(x)
#define signbit(x)					 _Generic((x),\
		float: signbitf, double: signbit, long double: signbitl)(x)
#define isgreater(x,y)			 _Generic((x)+(y),\
		float: isgreaterf, double: isgreater, long double: isgreaterl)(x,y)
#define isgreaterequal(x,y)  _Generic((x)+(y),\
		float: isgreaterequalf, double: isgreaterequal, long double: isgreaterequall)(x,y)
#define isless(x,y)					 _Generic((x)+(y),\
		float: islessf, double: isless, long double: islessl)(x,y)
#define islessequal(x,y)		 _Generic((x)+(y),\
		float: islessequalf, double: islessequal, long double: islessequall)(x,y)

#endif /* TGMATH_H */
