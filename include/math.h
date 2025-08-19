// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef MATH_H
#define MATH_H

// Special values & constants
#define NAN         __builtin_nan("")
#define INFINITY    __builtin_inff()
#define M_E         2.7182818284590452354
#define M_LOG2E     1.4426950408889634074
#define M_LOG10E    0.43429448190325182765
#define M_LN2       0.69314718055994530942
#define M_LN10      2.30258509299404568402
#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.78539816339744830962
#define M_1_PI      0.31830988618379067154
#define M_2_PI      0.63661977236758134308
#define M_2_SQRTPI  1.12837916709551257390
#define M_SQRT2     1.41421356237309504880
#define M_SQRT1_2   0.70710678118654752440

// Classification & comparison
#define isnan(x)       __builtin_isnan(x)
#define isinf(x)       __builtin_isinf(x)
#define isfinite(x)    __builtin_isfinite(x)
#define isnormal(x)    __builtin_isnormal(x)
#define signbit(x)     __builtin_signbit(x)
#define fpclassify(x)  __builtin_fpclassify(FP_NAN,FP_INFINITE,FP_NORMAL,FP_SUBNORMAL,FP_ZERO,x)
#define isgreater(x,y)      __builtin_isgreater(x,y)
#define isgreaterequal(x,y) __builtin_isgreaterequal(x,y)
#define isless(x,y)         __builtin_isless(x,y)
#define islessequal(x,y)    __builtin_islessequal(x,y)
#define islessgreater(x,y)  __builtin_islessgreater(x,y)
#define isunordered(x,y)    __builtin_isunordered(x,y)
#define FP_INFINITE  1
#define FP_NAN       2
#define FP_NORMAL    3
#define FP_SUBNORMAL 4
#define FP_ZERO      5

// Double precision
double fabs(double x)        { return __builtin_fabs(x); }
double sqrt(double x)        { return __builtin_sqrt(x); }
double cbrt(double x)        { return __builtin_cbrt(x); }
double exp(double x)         { return __builtin_exp(x); }
double exp2(double x)        { return __builtin_exp2(x); }
double expm1(double x)       { return __builtin_expm1(x); }
double log(double x)         { return __builtin_log(x); }
double log2(double x)        { return __builtin_log2(x); }
double log10(double x)       { return __builtin_log10(x); }
double log1p(double x)       { return __builtin_log1p(x); }
double pow(double x,double y)      { return __builtin_pow(x,y); }
double hypot(double x,double y)    { return __builtin_hypot(x,y); }
double fmod(double x,double y)     { return __builtin_fmod(x,y); }
double remainder(double x,double y){ return __builtin_remainder(x,y); }
double remquo(double x,double y,int*q){return __builtin_remquo(x,y,q);}
double ceil(double x)        { return __builtin_ceil(x); }
double floor(double x)       { return __builtin_floor(x); }
double trunc(double x)       { return __builtin_trunc(x); }
double round(double x)       { return __builtin_round(x); }
double nearbyint(double x)   { return __builtin_nearbyint(x); }
double rint(double x)        { return __builtin_rint(x); }
double sin(double x)         { return __builtin_sin(x); }
double cos(double x)         { return __builtin_cos(x); }
double tan(double x)         { return __builtin_tan(x); }
double asin(double x)        { return __builtin_asin(x); }
double acos(double x)        { return __builtin_acos(x); }
double atan(double x)        { return __builtin_atan(x); }
double atan2(double y,double x){return __builtin_atan2(y,x);}
double sinh(double x)        { return __builtin_sinh(x); }
double cosh(double x)        { return __builtin_cosh(x); }
double tanh(double x)        { return __builtin_tanh(x); }
double asinh(double x)       { return __builtin_asinh(x); }
double acosh(double x)       { return __builtin_acosh(x); }
double atanh(double x)       { return __builtin_atanh(x); }
double erf(double x)         { return __builtin_erf(x); }
double erfc(double x)        { return __builtin_erfc(x); }
double tgamma(double x)      { return __builtin_tgamma(x); }
double lgamma(double x)      { return __builtin_lgamma(x); }
double copysign(double x,double y){return __builtin_copysign(x,y);}
double frexp(double x,int*e) { return __builtin_frexp(x,e); }
double ldexp(double x,int e)  { return __builtin_ldexp(x,e); }
double modf(double x,double*i){ return __builtin_modf(x,i); }

// Float precision
float fabsf(float x)         { return __builtin_fabsf(x); }
float sqrtf(float x)         { return __builtin_sqrtf(x); }
float cbrtf(float x)         { return __builtin_cbrtf(x); }
float expf(float x)          { return __builtin_expf(x); }
float exp2f(float x)         { return __builtin_exp2f(x); }
float expm1f(float x)        { return __builtin_expm1f(x); }
float logf(float x)          { return __builtin_logf(x); }
float log2f(float x)         { return __builtin_log2f(x); }
float log10f(float x)        { return __builtin_log10f(x); }
float log1pf(float x)        { return __builtin_log1pf(x); }
float powf(float x,float y)      { return __builtin_powf(x,y); }
float hypotf(float x,float y)    { return __builtin_hypotf(x,y); }
float fmodf(float x,float y)     { return __builtin_fmodf(x,y); }
float remainderf(float x,float y){ return __builtin_remainderf(x,y); }
float remquof(float x,float y,int*q){return __builtin_remquof(x,y,q);}
float ceilf(float x)         { return __builtin_ceilf(x); }
float floorf(float x)        { return __builtin_floorf(x); }
float truncf(float x)        { return __builtin_truncf(x); }
float roundf(float x)        { return __builtin_roundf(x); }
float nearbyintf(float x)    { return __builtin_nearbyintf(x); }
float rintf(float x)         { return __builtin_rintf(x); }
float sinf(float x)          { return __builtin_sinf(x); }
float cosf(float x)          { return __builtin_cosf(x); }
float tanf(float x)          { return __builtin_tanf(x); }
float asinf(float x)         { return __builtin_asinf(x); }
float acosf(float x)         { return __builtin_acosf(x); }
float atanf(float x)         { return __builtin_atanf(x); }
float atan2f(float y,float x){ return __builtin_atan2f(y,x); }
float sinhf(float x)         { return __builtin_sinhf(x); }
float coshf(float x)         { return __builtin_coshf(x); }
float tanhf(float x)         { return __builtin_tanhf(x); }
float asinhf(float x)        { return __builtin_asinhf(x); }
float acoshf(float x)        { return __builtin_acoshf(x); }
float atanhf(float x)        { return __builtin_atanhf(x); }
float erff(float x)          { return __builtin_erff(x); }
float erfcf(float x)         { return __builtin_erfcf(x); }
float tgammaf(float x)       { return __builtin_tgammaf(x); }
float lgammaf(float x)       { return __builtin_lgammaf(x); }
float copysignf(float x,float y){return __builtin_copysignf(x,y);}
float frexpf(float x,int*e)  { return __builtin_frexpf(x,e); }
float ldexpf(float x,int e)   { return __builtin_ldexpf(x,e); }
float modff(float x,float*i)  { return __builtin_modff(x,i); }

// Long double precision
long double fabsl(long double x)       { return __builtin_fabsl(x); }
long double sqrtl(long double x)       { return __builtin_sqrtl(x); }
long double cbrtl(long double x)       { return __builtin_cbrtl(x); }
long double expl(long double x)        { return __builtin_expl(x); }
long double exp2l(long double x)       { return __builtin_exp2l(x); }
long double expm1l(long double x)      { return __builtin_expm1l(x); }
long double logl(long double x)        { return __builtin_logl(x); }
long double log2l(long double x)       { return __builtin_log2l(x); }
long double log10l(long double x)      { return __builtin_log10l(x); }
long double log1pl(long double x)      { return __builtin_log1pl(x); }
long double powl(long double x,long double y){return __builtin_powl(x,y);}
long double hypotl(long double x,long double y){return __builtin_hypotl(x,y);}
long double fmodl(long double x,long double y){return __builtin_fmodl(x,y);}
long double remainderl(long double x,long double y){return __builtin_remainderl(x,y);}
long double remquol(long double x,long double y,int*q){return __builtin_remquol(x,y,q);}
long double ceill(long double x)       { return __builtin_ceill(x); }
long double floorl(long double x)      { return __builtin_floorl(x); }
long double truncl(long double x)      { return __builtin_truncl(x); }
long double roundl(long double x)      { return __builtin_roundl(x); }
long double nearbyintl(long double x)  { return __builtin_nearbyintl(x); }
long double rintl(long double x)       { return __builtin_rintl(x); }
long double sinhl(long double x)       { return __builtin_sinhl(x); }
long double coshl(long double x)       { return __builtin_coshl(x); }
long double tanhl(long double x)       { return __builtin_tanhl(x); }
long double asinhl(long double x)      { return __builtin_asinhl(x); }
long double acoshl(long double x)      { return __builtin_acoshl(x); }
long double atanhl(long double x)      { return __builtin_atanhl(x); }
long double erfl(long double x)        { return __builtin_erfl(x); }
long double erfcl(long double x)       { return __builtin_erfcl(x); }
long double tgammal(long double x)     { return __builtin_tgammal(x); }
long double lgammal(long double x)     { return __builtin_lgammal(x); }
long double copysignl(long double x,long double y){return __builtin_copysignl(x,y);}
long double frexpl(long double x,int*e){return __builtin_frexpl(x,e);}
long double ldexpl(long double x,int e){return __builtin_ldexpl(x,e);}
long double modfl(long double x,long double*i){return __builtin_modfl(x,i);}

#endif /* MATH_H */

