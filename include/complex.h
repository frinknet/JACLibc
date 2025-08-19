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
float       cabsf(float complex z)       { return __builtin_cabsf(z); }
double      cabs(double complex z)       { return __builtin_cabs(z); }
long double cabsl(long double complex z) { return __builtin_cabsl(z); }

float complex cargf(float complex z)     { return __builtin_cargf(z); }
double complex carg(double complex z)     { return __builtin_carg(z); }
long double complex cargl(long double complex z) { return __builtin_cargl(z); }

float complex ccosf(float complex z)     { return __builtin_ccosf(z); }
double complex ccos(double complex z)     { return __builtin_ccos(z); }
long double complex ccosl(long double complex z) { return __builtin_ccosl(z); }

float complex csinf(float complex z)     { return __builtin_csinf(z); }
double complex csin(double complex z)     { return __builtin_csin(z); }
long double complex csinl(long double complex z) { return __builtin_csinl(z); }

float complex ctanf(float complex z)     { return __builtin_ctanf(z); }
double complex ctan(double complex z)     { return __builtin_ctan(z); }
long double complex ctanl(long double complex z) { return __builtin_ctanl(z); }

float complex cacosf(float complex z)    { return __builtin_cacosf(z); }
double complex cacos(double complex z)    { return __builtin_cacos(z); }
long double complex cacosl(long double complex z) { return __builtin_cacosl(z); }

float complex casinf(float complex z)    { return __builtin_casinf(z); }
double complex casin(double complex z)    { return __builtin_casin(z); }
long double complex casinl(long double complex z) { return __builtin_casinl(z); }

float complex catanf(float complex z)    { return __builtin_catanf(z); }
double complex catan(double complex z)    { return __builtin_catan(z); }
long double complex catanl(long double complex z) { return __builtin_catanl(z); }

float complex ccoshf(float complex z)    { return __builtin_ccoshf(z); }
double complex ccosh(double complex z)    { return __builtin_ccosh(z); }
long double complex ccoshl(long double complex z) { return __builtin_ccoshl(z); }

float complex csinhf(float complex z)    { return __builtin_csinhf(z); }
double complex csinh(double complex z)    { return __builtin_csinh(z); }
long double complex csinhl(long double complex z) { return __builtin_csinhl(z); }

float complex ctanhf(float complex z)    { return __builtin_ctanhf(z); }
double complex ctanh(double complex z)    { return __builtin_ctanh(z); }
long double complex ctanhl(long double complex z) { return __builtin_ctanhl(z); }

float complex cacoshf(float complex z)   { return __builtin_cacoshf(z); }
double complex cacosh(double complex z)   { return __builtin_cacosh(z); }
long double complex cacoshl(long double complex z) { return __builtin_cacoshl(z); }

float complex casinhf(float complex z)   { return __builtin_casinhf(z); }
double complex casinh(double complex z)   { return __builtin_casinh(z); }
long double complex casinhl(long double complex z) { return __builtin_casinhl(z); }

float complex catanhf(float complex z)   { return __builtin_catanhf(z); }
double complex catanh(double complex z)   { return __builtin_catanh(z); }
long double complex catanhl(long double complex z) { return __builtin_catanhl(z); }

float complex cexpf(float complex z)     { return __builtin_cexpf(z); }
double complex cexp(double complex z)     { return __builtin_cexp(z); }
long double complex cexpl(long double complex z) { return __builtin_cexpl(z); }

float complex clogf(float complex z)     { return __builtin_clogf(z); }
double complex clog(double complex z)     { return __builtin_clog(z); }
long double complex clogl(long double complex z) { return __builtin_clogl(z); }

float complex cpowf(float complex z, float complex w)   { return __builtin_cpowf(z,w); }
double complex cpow(double complex z, double complex w)  { return __builtin_cpow(z,w); }
long double complex cpowl(long double complex z, long double complex w) { return __builtin_cpowl(z,w); }

float complex csqrtf(float complex z)    { return __builtin_csqrtf(z); }
double complex csqrt(double complex z)    { return __builtin_csqrt(z); }
long double complex csqrtl(long double complex z) { return __builtin_csqrtl(z); }

float complex cprojf(float complex z)    { return __builtin_cprojf(z); }
double complex cproj(double complex z)    { return __builtin_cproj(z); }
long double complex cprojl(long double complex z) { return __builtin_cprojl(z); }

#endif /* COMPLEX_H */

