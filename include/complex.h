// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef COMPLEX_H
#define COMPLEX_H

#include <math.h>

/* C99 pure-complex types */
typedef float  float complex;
typedef double double complex;
typedef long double long double complex;

/* Imaginary unit */
#define I (__builtin__Complex_I)

/* Complex magnitude, phase, real, imag, conj, proj */
static inline float       cabsf(float complex z)       { return __builtin_cabsf(z); }
static inline double      cabs(double complex z)       { return __builtin_cabs(z); }
static inline long double cabsl(long double complex z) { return __builtin_cabsl(z); }

static inline float complex cargf(float complex z)     { return __builtin_cargf(z); }
static inline double complex carg(double complex z)     { return __builtin_carg(z); }
static inline long double complex cargl(long double complex z) { return __builtin_cargl(z); }

static inline float complex ccosf(float complex z)     { return __builtin_ccosf(z); }
static inline double complex ccos(double complex z)     { return __builtin_ccos(z); }
static inline long double complex ccosl(long double complex z) { return __builtin_ccosl(z); }

static inline float complex csinf(float complex z)     { return __builtin_csinf(z); }
static inline double complex csin(double complex z)     { return __builtin_csin(z); }
static inline long double complex csinl(long double complex z) { return __builtin_csinl(z); }

static inline float complex ctanf(float complex z)     { return __builtin_ctanf(z); }
static inline double complex ctan(double complex z)     { return __builtin_ctan(z); }
static inline long double complex ctanl(long double complex z) { return __builtin_ctanl(z); }

static inline float complex cacosf(float complex z)    { return __builtin_cacosf(z); }
static inline double complex cacos(double complex z)    { return __builtin_cacos(z); }
static inline long double complex cacosl(long double complex z) { return __builtin_cacosl(z); }

static inline float complex casinf(float complex z)    { return __builtin_casinf(z); }
static inline double complex casin(double complex z)    { return __builtin_casin(z); }
static inline long double complex casinl(long double complex z) { return __builtin_casinl(z); }

static inline float complex catanf(float complex z)    { return __builtin_catanf(z); }
static inline double complex catan(double complex z)    { return __builtin_catan(z); }
static inline long double complex catanl(long double complex z) { return __builtin_catanl(z); }

static inline float complex ccoshf(float complex z)    { return __builtin_ccoshf(z); }
static inline double complex ccosh(double complex z)    { return __builtin_ccosh(z); }
static inline long double complex ccoshl(long double complex z) { return __builtin_ccoshl(z); }

static inline float complex csinhf(float complex z)    { return __builtin_csinhf(z); }
static inline double complex csinh(double complex z)    { return __builtin_csinh(z); }
static inline long double complex csinhl(long double complex z) { return __builtin_csinhl(z); }

static inline float complex ctanhf(float complex z)    { return __builtin_ctanhf(z); }
static inline double complex ctanh(double complex z)    { return __builtin_ctanh(z); }
static inline long double complex ctanhl(long double complex z) { return __builtin_ctanhl(z); }

static inline float complex cacoshf(float complex z)   { return __builtin_cacoshf(z); }
static inline double complex cacosh(double complex z)   { return __builtin_cacosh(z); }
static inline long double complex cacoshl(long double complex z) { return __builtin_cacoshl(z); }

static inline float complex casinhf(float complex z)   { return __builtin_casinhf(z); }
static inline double complex casinh(double complex z)   { return __builtin_casinh(z); }
static inline long double complex casinhl(long double complex z) { return __builtin_casinhl(z); }

static inline float complex catanhf(float complex z)   { return __builtin_catanhf(z); }
static inline double complex catanh(double complex z)   { return __builtin_catanh(z); }
static inline long double complex catanhl(long double complex z) { return __builtin_catanhl(z); }

static inline float complex cexpf(float complex z)     { return __builtin_cexpf(z); }
static inline double complex cexp(double complex z)     { return __builtin_cexp(z); }
static inline long double complex cexpl(long double complex z) { return __builtin_cexpl(z); }

static inline float complex clogf(float complex z)     { return __builtin_clogf(z); }
static inline double complex clog(double complex z)     { return __builtin_clog(z); }
static inline long double complex clogl(long double complex z) { return __builtin_clogl(z); }

static inline float complex cpowf(float complex z, float complex w)   { return __builtin_cpowf(z,w); }
static inline double complex cpow(double complex z, double complex w)  { return __builtin_cpow(z,w); }
static inline long double complex cpowl(long double complex z, long double complex w) { return __builtin_cpowl(z,w); }

static inline float complex csqrtf(float complex z)    { return __builtin_csqrtf(z); }
static inline double complex csqrt(double complex z)    { return __builtin_csqrt(z); }
static inline long double complex csqrtl(long double complex z) { return __builtin_csqrtl(z); }

static inline float complex cprojf(float complex z)    { return __builtin_cprojf(z); }
static inline double complex cproj(double complex z)    { return __builtin_cproj(z); }
static inline long double complex cprojl(long double complex z) { return __builtin_cprojl(z); }

#endif /* COMPLEX_H */

