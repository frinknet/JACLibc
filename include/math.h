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
static inline double fabs(double x)        { return __builtin_fabs(x); }
static inline double sqrt(double x)        { return __builtin_sqrt(x); }
static inline double cbrt(double x)        { return __builtin_cbrt(x); }
static inline double exp(double x)         { return __builtin_exp(x); }
static inline double exp2(double x)        { return __builtin_exp2(x); }
static inline double expm1(double x)       { return __builtin_expm1(x); }
static inline double log(double x)         { return __builtin_log(x); }
static inline double log2(double x)        { return __builtin_log2(x); }
static inline double log10(double x)       { return __builtin_log10(x); }
static inline double log1p(double x)       { return __builtin_log1p(x); }
static inline double pow(double x,double y)      { return __builtin_pow(x,y); }
static inline double hypot(double x,double y)    { return __builtin_hypot(x,y); }
static inline double fmod(double x,double y)     { return __builtin_fmod(x,y); }
static inline double remainder(double x,double y){ return __builtin_remainder(x,y); }
static inline double remquo(double x,double y,int*q){return __builtin_remquo(x,y,q);}
static inline double ceil(double x)        { return __builtin_ceil(x); }
static inline double floor(double x)       { return __builtin_floor(x); }
static inline double trunc(double x)       { return __builtin_trunc(x); }
static inline double round(double x)       { return __builtin_round(x); }
static inline double nearbyint(double x)   { return __builtin_nearbyint(x); }
static inline double rint(double x)        { return __builtin_rint(x); }
static inline double sin(double x)         { return __builtin_sin(x); }
static inline double cos(double x)         { return __builtin_cos(x); }
static inline double tan(double x)         { return __builtin_tan(x); }
static inline double asin(double x)        { return __builtin_asin(x); }
static inline double acos(double x)        { return __builtin_acos(x); }
static inline double atan(double x)        { return __builtin_atan(x); }
static inline double atan2(double y,double x){return __builtin_atan2(y,x);}
static inline double sinh(double x)        { return __builtin_sinh(x); }
static inline double cosh(double x)        { return __builtin_cosh(x); }
static inline double tanh(double x)        { return __builtin_tanh(x); }
static inline double asinh(double x)       { return __builtin_asinh(x); }
static inline double acosh(double x)       { return __builtin_acosh(x); }
static inline double atanh(double x)       { return __builtin_atanh(x); }
static inline double erf(double x)         { return __builtin_erf(x); }
static inline double erfc(double x)        { return __builtin_erfc(x); }
static inline double tgamma(double x)      { return __builtin_tgamma(x); }
static inline double lgamma(double x)      { return __builtin_lgamma(x); }
static inline double copysign(double x,double y){return __builtin_copysign(x,y);}
static inline double frexp(double x,int*e) { return __builtin_frexp(x,e); }
static inline double ldexp(double x,int e)  { return __builtin_ldexp(x,e); }
static inline double modf(double x,double*i){ return __builtin_modf(x,i); }

// Float precision
static inline float fabsf(float x)         { return __builtin_fabsf(x); }
static inline float sqrtf(float x)         { return __builtin_sqrtf(x); }
static inline float cbrtf(float x)         { return __builtin_cbrtf(x); }
static inline float expf(float x)          { return __builtin_expf(x); }
static inline float exp2f(float x)         { return __builtin_exp2f(x); }
static inline float expm1f(float x)        { return __builtin_expm1f(x); }
static inline float logf(float x)          { return __builtin_logf(x); }
static inline float log2f(float x)         { return __builtin_log2f(x); }
static inline float log10f(float x)        { return __builtin_log10f(x); }
static inline float log1pf(float x)        { return __builtin_log1pf(x); }
static inline float powf(float x,float y)      { return __builtin_powf(x,y); }
static inline float hypotf(float x,float y)    { return __builtin_hypotf(x,y); }
static inline float fmodf(float x,float y)     { return __builtin_fmodf(x,y); }
static inline float remainderf(float x,float y){ return __builtin_remainderf(x,y); }
static inline float remquof(float x,float y,int*q){return __builtin_remquof(x,y,q);}
static inline float ceilf(float x)         { return __builtin_ceilf(x); }
static inline float floorf(float x)        { return __builtin_floorf(x); }
static inline float truncf(float x)        { return __builtin_truncf(x); }
static inline float roundf(float x)        { return __builtin_roundf(x); }
static inline float nearbyintf(float x)    { return __builtin_nearbyintf(x); }
static inline float rintf(float x)         { return __builtin_rintf(x); }
static inline float sinf(float x)          { return __builtin_sinf(x); }
static inline float cosf(float x)          { return __builtin_cosf(x); }
static inline float tanf(float x)          { return __builtin_tanf(x); }
static inline float asinf(float x)         { return __builtin_asinf(x); }
static inline float acosf(float x)         { return __builtin_acosf(x); }
static inline float atanf(float x)         { return __builtin_atanf(x); }
static inline float atan2f(float y,float x){ return __builtin_atan2f(y,x); }
static inline float sinhf(float x)         { return __builtin_sinhf(x); }
static inline float coshf(float x)         { return __builtin_coshf(x); }
static inline float tanhf(float x)         { return __builtin_tanhf(x); }
static inline float asinhf(float x)        { return __builtin_asinhf(x); }
static inline float acoshf(float x)        { return __builtin_acoshf(x); }
static inline float atanhf(float x)        { return __builtin_atanhf(x); }
static inline float erff(float x)          { return __builtin_erff(x); }
static inline float erfcf(float x)         { return __builtin_erfcf(x); }
static inline float tgammaf(float x)       { return __builtin_tgammaf(x); }
static inline float lgammaf(float x)       { return __builtin_lgammaf(x); }
static inline float copysignf(float x,float y){return __builtin_copysignf(x,y);}
static inline float frexpf(float x,int*e)  { return __builtin_frexpf(x,e); }
static inline float ldexpf(float x,int e)   { return __builtin_ldexpf(x,e); }
static inline float modff(float x,float*i)  { return __builtin_modff(x,i); }

// Long double precision
static inline long double fabsl(long double x)       { return __builtin_fabsl(x); }
static inline long double sqrtl(long double x)       { return __builtin_sqrtl(x); }
static inline long double cbrtl(long double x)       { return __builtin_cbrtl(x); }
static inline long double expl(long double x)        { return __builtin_expl(x); }
static inline long double exp2l(long double x)       { return __builtin_exp2l(x); }
static inline long double expm1l(long double x)      { return __builtin_expm1l(x); }
static inline long double logl(long double x)        { return __builtin_logl(x); }
static inline long double log2l(long double x)       { return __builtin_log2l(x); }
static inline long double log10l(long double x)      { return __builtin_log10l(x); }
static inline long double log1pl(long double x)      { return __builtin_log1pl(x); }
static inline long double powl(long double x,long double y){return __builtin_powl(x,y);}
static inline long double hypotl(long double x,long double y){return __builtin_hypotl(x,y);}
static inline long double fmodl(long double x,long double y){return __builtin_fmodl(x,y);}
static inline long double remainderl(long double x,long double y){return __builtin_remainderl(x,y);}
static inline long double remquol(long double x,long double y,int*q){return __builtin_remquol(x,y,q);}
static inline long double ceill(long double x)       { return __builtin_ceill(x); }
static inline long double floorl(long double x)      { return __builtin_floorl(x); }
static inline long double truncl(long double x)      { return __builtin_truncl(x); }
static inline long double roundl(long double x)      { return __builtin_roundl(x); }
static inline long double nearbyintl(long double x)  { return __builtin_nearbyintl(x); }
static inline long double rintl(long double x)       { return __builtin_rintl(x); }
static inline long double sinhl(long double x)       { return __builtin_sinhl(x); }
static inline long double coshl(long double x)       { return __builtin_coshl(x); }
static inline long double tanhl(long double x)       { return __builtin_tanhl(x); }
static inline long double asinhl(long double x)      { return __builtin_asinhl(x); }
static inline long double acoshl(long double x)      { return __builtin_acoshl(x); }
static inline long double atanhl(long double x)      { return __builtin_atanhl(x); }
static inline long double erfl(long double x)        { return __builtin_erfl(x); }
static inline long double erfcl(long double x)       { return __builtin_erfcl(x); }
static inline long double tgammal(long double x)     { return __builtin_tgammal(x); }
static inline long double lgammal(long double x)     { return __builtin_lgammal(x); }
static inline long double copysignl(long double x,long double y){return __builtin_copysignl(x,y);}
static inline long double frexpl(long double x,int*e){return __builtin_frexpl(x,e);}
static inline long double ldexpl(long double x,int e){return __builtin_ldexpl(x,e);}
static inline long double modfl(long double x,long double*i){return __builtin_modfl(x,i);}

#endif /* MATH_H */

