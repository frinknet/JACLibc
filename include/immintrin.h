// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef IMMINTRIN_H
#define IMMINTRIN_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration macros (avoid circular dependency with math.h)
#define __jacl_imath_x(name) float name##f(float x); double name(double x);
#define __jacl_imath_xx(name) float name##f(float x, float y); double name(double x, double y);

__jacl_imath_x(sin)
__jacl_imath_x(cos)
__jacl_imath_x(tan)
__jacl_imath_x(exp)
__jacl_imath_x(log)
__jacl_imath_x(sqrt)
__jacl_imath_x(sinh)
__jacl_imath_x(cosh)
__jacl_imath_x(tanh)
__jacl_imath_x(asinh)
__jacl_imath_x(acosh)
__jacl_imath_x(atanh)
__jacl_imath_x(erf)
__jacl_imath_x(tgamma)

__jacl_imath_xx(pow)
__jacl_imath_xx(hypot)

#undef __jacl_imath_x
#undef __jacl_imath_xx

// Vector types
typedef float  __m128  __attribute__((vector_size(16)));
typedef double __m128d __attribute__((vector_size(16)));

// Scalar ↔ vector moves
static inline __m128	_mm_set_ss(float	x) { return (__m128){ x,0,0,0 }; }
static inline __m128d _mm_set_sd(double x) { return (__m128d){ x,0 }; }

static inline float  _mm_cvtss_f32(__m128  v) { return v[0]; }
static inline double _mm_cvtsd_f64(__m128d v) { return v[0]; }

// Trigonometry
static inline __m128	_mm_sin_ps(__m128  v) { return (__m128){ sinf(v[0]), sinf(v[1]), sinf(v[2]), sinf(v[3]) }; }
static inline __m128d _mm_sin_pd(__m128d v) { return (__m128d){ sin(v[0]), sin(v[1]) }; }

static inline __m128	_mm_cos_ps(__m128  v) { return (__m128){ cosf(v[0]), cosf(v[1]), cosf(v[2]), cosf(v[3]) }; }
static inline __m128d _mm_cos_pd(__m128d v) { return (__m128d){ cos(v[0]), cos(v[1]) }; }

static inline __m128	_mm_tan_ps(__m128  v) { return (__m128){ tanf(v[0]), tanf(v[1]), tanf(v[2]), tanf(v[3]) }; }
static inline __m128d _mm_tan_pd(__m128d v) { return (__m128d){ tan(v[0]), tan(v[1]) }; }

// Exponential & logarithm
static inline __m128	_mm_exp_ps(__m128  v) { return (__m128){ expf(v[0]), expf(v[1]), expf(v[2]), expf(v[3]) }; }
static inline __m128d _mm_exp_pd(__m128d v) { return (__m128d){ exp(v[0]), exp(v[1]) }; }

static inline __m128	_mm_log_ps(__m128  v) { return (__m128){ logf(v[0]), logf(v[1]), logf(v[2]), logf(v[3]) }; }
static inline __m128d _mm_log_pd(__m128d v) { return (__m128d){ log(v[0]), log(v[1]) }; }

// Power, square‐root, hypotenuse
static inline __m128	_mm_pow_ps(__m128  a,__m128  b){ return (__m128){ powf(a[0],b[0]), powf(a[1],b[1]), powf(a[2],b[2]), powf(a[3],b[3]) }; }
static inline __m128d _mm_pow_pd(__m128d a,__m128d b){ return (__m128d){ pow(a[0],b[0]), pow(a[1],b[1]) }; }

static inline __m128	_mm_sqrt_ps(__m128	v) { return (__m128){ sqrtf(v[0]), sqrtf(v[1]), sqrtf(v[2]), sqrtf(v[3]) }; }
static inline __m128d _mm_sqrt_pd(__m128d v) { return (__m128d){ sqrt(v[0]), sqrt(v[1]) }; }

static inline __m128	_mm_hypot_ps(__m128  a,__m128  b){ return (__m128){ hypotf(a[0],b[0]), hypotf(a[1],b[1]), hypotf(a[2],b[2]), hypotf(a[3],b[3]) }; }
static inline __m128d _mm_hypot_pd(__m128d a,__m128d b){ return (__m128d){ hypot(a[0],b[0]), hypot(a[1],b[1]) }; }

// Hyperbolic
#define __jacl_svml_hyp(name) \
static inline __m128	_mm_##name##_ps(__m128	v){return (__m128){ name##f(v[0]),	name##f(v[1]),	name##f(v[2]),	name##f(v[3])  };} \
static inline __m128d _mm_##name##_pd(__m128d v){return (__m128d){ name(v[0]),	 name(v[1])		};}

__jacl_svml_hyp(sinh)
__jacl_svml_hyp(cosh)
__jacl_svml_hyp(tanh)
__jacl_svml_hyp(asinh)
__jacl_svml_hyp(acosh)
__jacl_svml_hyp(atanh)

#undef __jacl_svml_hyp

// Error and gamma
static inline __m128	_mm_erf_ps(__m128  v){ return (__m128){ erff(v[0]), erff(v[1]), erff(v[2]), erff(v[3]) }; }
static inline __m128d _mm_erf_pd(__m128d v){ return (__m128d){ erf(v[0]), erf(v[1]) }; }

static inline __m128	_mm_tgamma_ps(__m128	v){ return (__m128){ tgammaf(v[0]), tgammaf(v[1]), tgammaf(v[2]), tgammaf(v[3]) }; }
static inline __m128d _mm_tgamma_pd(__m128d v){ return (__m128d){ tgamma(v[0]), tgamma(v[1]) }; }

// Fused multiply‐add
static inline __m128	_mm_fmadd_ps(__m128  a,__m128  b,__m128  c){ return a*b + c; }
static inline __m128d _mm_fmadd_pd(__m128d a,__m128d b,__m128d c){ return a*b + c; }

#ifdef __cplusplus
}
#endif

#endif /* IMMINTRIN_H */
