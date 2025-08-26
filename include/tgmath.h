// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef TGMATH_H
#define TGMATH_H

#include <math.h>
#include <complex.h>

#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L
#error "tgmath.h requires C11 (_Generic)"
#endif

/*
 * C99/C11 type-generic math macros:
 * float, double, long double,
 * float complex, double complex, long double complex.
 */

// Absolute value
#define fabs(x)					_Generic((x), \
		float: fabsf, double: fabs, long double: fabsl, \
		float complex: cabsf, double complex: cabs, long double complex: cabsl)(x)

// Elementary
#define sqrt(x)					_Generic((x), \
		float: sqrtf, double: sqrt, long double: sqrtl, \
		float complex: csqrtf, double complex: csqrt, long double complex: csqrtl)(x)
#define cbrt(x)					_Generic((x), \
		float: cbrtf, double: cbrt, long double: cbrtl)(x)
#define exp(x)					_Generic((x), \
		float: expf, double: exp, long double: expl, \
		float complex: cexpf, double complex: cexp, long double complex: cexpl)(x)
#define exp2(x)					_Generic((x), \
		float: exp2f, double: exp2, long double: exp2l)(x)
#define expm1(x)				_Generic((x), \
		float: expm1f, double: expm1, long double: expm1l)(x)
#define log(x)					_Generic((x), \
		float: logf, double: log, long double: logl, \
		float complex: clogf, double complex: clog, long double complex: clogl)(x)
#define log10(x)				_Generic((x), \
		float: log10f, double: log10, long double: log10l)(x)
#define log1p(x)				_Generic((x), \
		float: log1pf, double: log1p, long double: log1pl)(x)
#define log2(x)					_Generic((x), \
		float: log2f, double: log2, long double: log2l)(x)
#define logb(x)					_Generic((x), \
		float: logbf, double: logb, long double: logbl)(x)
#define ilogb(x)				_Generic((x), \
		float: ilogbf, double: ilogb, long double: ilogbl)(x)
#define frexp(x,e)			_Generic((x), \
		float: frexpf, double: frexp, long double: frexpl)(x,e)
#define ldexp(x,e)			_Generic((x), \
		float: ldexpf, double: ldexp, long double: ldexpl)(x,e)

// Trigonometric
#define sin(x)					_Generic((x), \
		float: sinf, double: sin, long double: sinl, \
		float complex: csinf, double complex: csin, long double complex: csinl)(x)
#define cos(x)					_Generic((x), \
		float: cosf, double: cos, long double: cosl, \
		float complex: ccosf, double complex: ccos, long double complex: ccosl)(x)
#define tan(x)					_Generic((x), \
		float: tanf, double: tan, long double: tanl, \
		float complex: ctanf, double complex: ctan, long double complex: ctanl)(x)
#define asin(x)					_Generic((x), \
		float: asinf, double: asin, long double: asinl, \
		float complex: casinf, double complex: casin, long double complex: casinl)(x)
#define acos(x)					_Generic((x), \
		float: acosf, double: acos, long double: acosl, \
		float complex: cacosf, double complex: cacos, long double complex: cacosl)(x)
#define atan(x)					_Generic((x), \
		float: atanf, double: atan, long double: atanl, \
		float complex: catanf, double complex: catan, long double complex: catanl)(x)
#define atan2(y,x)			_Generic((y)+(x), \
		float: atan2f, double: atan2, long double: atan2l)(y,x)

// Hyperbolic
#define sinh(x)					_Generic((x), \
		float: sinhf, double: sinh, long double: sinhl, \
		float complex: csinhf, double complex: csinh, long double complex: csinhl)(x)
#define cosh(x)					_Generic((x), \
		float: coshf, double: cosh, long double: coshl, \
		float complex: ccoshf, double complex: ccosh, long double complex: ccoshl)(x)
#define tanh(x)					_Generic((x), \
		float: tanhf, double: tanh, long double: tanhl, \
		float complex: ctanhf, double complex: ctanh, long double complex: ctanhl)(x)
#define asinh(x)				_Generic((x), \
		float: asinhf, double: asinh, long double: asinhl, \
		float complex: casinhf, double complex: casinh, long double complex: casinhl)(x)
#define acosh(x)				_Generic((x), \
		float: acoshf, double: acosh, long double: acoshl, \
		float complex: cacoshf, double complex: cacosh, long double complex: cacoshl)(x)
#define atanh(x)				_Generic((x), \
		float: atanhf, double: atanh, long double: atanhl, \
		float complex: catanhf, double complex: catanh, long double complex: catanhl)(x)

// Special
#define hypot(x,y)			_Generic((x)+(y), \
		float: hypotf, double: hypot, long double: hypotl, \
		float complex: chypotf, double complex: chypot, long double complex: chypotl)(x,y)
#define erf(x)					_Generic((x), \
		float: erff, double: erf, long double: erfl)(x)
#define erfc(x)					_Generic((x), \
		float: erfcf, double: erfc, long double: erfcl)(x)
#define tgamma(x)				_Generic((x), \
		float: tgammaf, double: tgamma, long double: tgammal)(x)
#define lgamma(x)				_Generic((x), \
		float: lgammaf, double: lgamma, long double: lgammal)(x)

// Rounding and remainder
#define ceil(x)					_Generic((x), float: ceilf, double: ceil, long double: ceill)(x)
#define floor(x)				_Generic((x), float: floorf, double: floor, long double: floorl)(x)
#define trunc(x)				_Generic((x), float: truncf, double: trunc, long double: truncl)(x)
#define round(x)				_Generic((x), float: roundf, double: round, long double: roundl)(x)
#define nearbyint(x)		_Generic((x), float: nearbyintf, double: nearbyint, long double: nearbyintl)(x)
#define rint(x)					_Generic((x), float: rintf, double: rint, long double: rintl)(x)

#define fmod(x,y)				_Generic((x)+(y), float: fmodf, double: fmod, long double: fmodl)(x,y)
#define remainder(x,y)	_Generic((x)+(y), float: remainderf, double: remainder, long double: remainderl)(x,y)

// FP manipulation
#define scalbn(x,n)			_Generic((x), float: scalbnf, double: scalbn, long double: scalbnl)(x,n)
#define scalbln(x,n)		_Generic((x), float: scalblnf, double: scalbln, long double: scalblnl)(x,n)
#define copysign(x,y)		_Generic((x)+(y), float: copysignf, double: copysign, long double: copysignl)(x,y)
#define nextafter(x,y)	_Generic((x)+(y), float: nextafterf, double: nextafter, long double: nextafterl)(x,y)
#define nexttoward(x,y) _Generic((x), float: nexttowardf, double: nexttoward, long double: nexttowardl)(x,y)

// Complex accessors
#define carg(z)					_Generic((z), float complex: cargf, double complex: carg, long double complex: cargl)(z)
#define cimag(z)				_Generic((z), float complex: cimagf, double complex: cimag, long double complex: cimagl)(z)
#define creal(z)				_Generic((z), float complex: crealf, double complex: creal, long double complex: creall)(z)
#define conj(z)					_Generic((z), float complex: conjf, double complex: conj, long double complex: conjl)(z)
#define cproj(z)				_Generic((z), float complex: cprojf, double complex: cproj, long double complex: cprojl)(z)

// Power, min/max
#define pow(x,y)				_Generic((x)+(y), \
		float: powf, double: pow, long double: powl, \
		float complex: cpowf, double complex: cpow, long double complex: cpowl)(x,y)
#define fdim(x,y)				_Generic((x)+(y), float: fdimf, double: fdim, long double: fdiml)(x,y)
#define fmax(x,y)				_Generic((x)+(y), float: fmaxf, double: fmax, long double: fmaxl)(x,y)
#define fmin(x,y)				_Generic((x)+(y), float: fminf, double: fmin, long double: fminl)(x,y)
#define fma(x,y,z)			_Generic((x)+(y)+(z), float: fmaf, double: fma, long double: fmal)(x,y,z)

#endif /* TGMATH_H */

