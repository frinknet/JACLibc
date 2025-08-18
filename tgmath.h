// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef TGMATH_H
#define TGMATH_H

#include <math.h>
#include <complex.h>

/*
 * C99 type-generic math macros:
 * float, double, long double,
 * float complex, double complex, long double complex.
 */

// Elementary & classification
#define fabs(x)    _Generic((x),\
    float: fabsf, double: fabs, long double: fabsl,\
    float complex: cabsf, double complex: cabs, long double complex: cabsl)(x)

#define sqrt(x)    _Generic((x),\
    float: sqrtf, double: sqrt, long double: sqrtl,\
    float complex: csqrtf, double complex: csqrt, long double complex: csqrtl)(x)

#define exp(x)     _Generic((x),\
    float: expf, double: exp, long double: expl,\
    float complex: cexpf, double complex: cexp, long double complex: cexpl)(x)

#define log(x)     _Generic((x),\
    float: logf, double: log, long double: logl,\
    float complex: clogf, double complex: clog, long double complex: clogl)(x)

// Trigonometry
#define sin(x)     _Generic((x),\
    float: sinf, double: sin, long double: sinl,\
    float complex: csinf, double complex: csin, long double complex: csinl)(x)

#define cos(x)     _Generic((x),\
    float: cosf, double: cos, long double: cosl,\
    float complex: ccosf, double complex: ccos, long double complex: ccosl)(x)

#define tan(x)     _Generic((x),\
    float: tanf, double: tan, long double: tanl,\
    float complex: ctanf, double complex: ctan, long double complex: ctanl)(x)

// Hyperbolic & special
#define sinh(x)    _Generic((x),\
    float: sinhf, double: sinh, long double: sinhl,\
    float complex: csinhf, double complex: csinh, long double complex: csinhl)(x)

#define cosh(x)    _Generic((x),\
    float: coshf, double: cosh, long double: coshl,\
    float complex: ccoshf, double complex: ccosh, long double complex: ccoshl)(x)

#define tanh(x)    _Generic((x),\
    float: tanhf, double: tanh, long double: tanhl,\
    float complex: ctanhf, double complex: ctanh, long double complex: ctanhl)(x)

#define erf(x)     _Generic((x),\
    float: erff, double: erf, long double: erfl,\
    float complex: cerff, double complex: cerf, long double complex: cerfl)(x)

#define tgamma(x)  _Generic((x),\
    float: tgammaf, double: tgamma, long double: tgammal,\
    float complex: ctgammaf, double complex: ctgamma, long double complex: ctgammal)(x)

// Power & remainder
#define pow(x,y)   _Generic((x)+(y),\
    float: powf, double: pow, long double: powl,\
    float complex: cpowf, double complex: cpow, long double complex: cpowl)(x,y)

#define hypot(x,y) _Generic((x)+(y),\
    float: hypotf, double: hypot, long double: hypotl,\
    float complex: chypotf, double complex: chypot, long double complex: chypotl)(x,y)

#define fmod(x,y)  _Generic((x)+(y),\
    float: fmodf, double: fmod, long double: fmodl)(x,y)

// Rounding & misc
#define ceil(x)    _Generic((x), float: ceilf, double: ceil, long double: ceill)(x)
#define floor(x)   _Generic((x), float: floorf,double: floor,long double: floorl)(x)
#define trunc(x)   _Generic((x), float: truncf,double: trunc,long double: truncl)(x)
#define round(x)   _Generic((x), float: roundf,double: round,long double: roundl)(x)
#define copysign(x,y) _Generic((x)+(y),\
    float: copysignf, double: copysign, long double: copysignl)(x,y)

#endif /* TGMATH_H */

