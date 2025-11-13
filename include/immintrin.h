/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef IMMINTRIN_H
#define IMMINTRIN_H
#pragma once

/**
 * NOTE: This is intentionally FAKE realizing that the big compilers will optimize with -O2 or -O3.
 * You are better at writing code than wrangling architecture features so we play to your strength.
 * However, we do check that the compiler is going to optimize this for you...
 */

#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================ */
/* SIMD Capability Detection                                    */
/* ============================================================ */

#if defined(__SSE2__) || defined(_M_X64) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)
  #define JACL_HAS_SSE2 1
#else
  #define JACL_HAS_SSE2 0
#endif

#if defined(__SSE4_1__)
  #define JACL_HAS_SSE4_1 1
#else
  #define JACL_HAS_SSE4_1 0
#endif



#if defined(__FMA__)
  #define JACL_HAS_FMA 1
#else
  #define JACL_HAS_FMA 0
#endif

#if defined(__ARM_NEON) || defined(__ARM_NEON__)
  #define JACL_HAS_NEON 1
#else
  #define JACL_HAS_NEON 0
#endif

#if defined(__wasm_simd128__)
  #define JACL_HAS_WASM_SIMD 1
#else
  #define JACL_HAS_WASM_SIMD 0
#endif

#if JACL_HAS_SSE2 || JACL_HAS_NEON || JACL_HAS_WASM_SIMD
  #define JACL_HAS_SIMD 1
#else
  #define JACL_HAS_SIMD 0
#endif

#if !JACL_HAS_C99
  #error "immintrin.h requires C99 or later"
#endif

#if !__has_attribute(vector_size)
  #error "immintrin.h requires a modern compiler with vector_size attribute for vector types"
#endif

#if !JACL_HAS_SIMD
  #if defined(JACL_ARCH_X86) || defined(JACL_ARCH_X64)
    #error "Compile with -msse2 or -march=native for x86/x64 targets"
  #elif defined(JACL_ARCH_ARM32) || defined(JACL_ARCH_ARM64)
    #error "Compile with -mfpu=neon (ARM32) or enable NEON (ARM64)"
  #elif defined(JACL_ARCH_WASM)
    #error "Compile with -msimd128 for WebAssembly SIMD"
  #else
    #error "No SIMD support detected for this architecture"
  #endif
#endif

/* ============================================================ */
/* Forward Declarations (Avoid circular dependency with math.h) */
/* ============================================================ */

#define __jacl_imath_x(name) \
		static inline float name##f(float x); \
		static inline double name(double x); \
		static inline long double name##l(long double x);

#define __jacl_imath_xx(name) \
		static inline float name##f(float x, float y); \
		static inline double name(double x, double y); \
		static inline long double name##l(long double x, long double y);

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
__jacl_imath_x(ceil)
__jacl_imath_x(floor)
__jacl_imath_x(trunc)
__jacl_imath_x(round)
__jacl_imath_xx(pow)
__jacl_imath_xx(hypot)
#undef __jacl_imath_x
#undef __jacl_imath_xx

/* ============================================================ */
// Vector Types (Generic - Platform Specific Below)             */
/* ============================================================ */

#if JACL_HAS_NEON
  #include <arm_neon.h>
  typedef float32x4_t __m128;
  typedef float64x2_t __m128d;
#else
  typedef float  __m128  __attribute__((vector_size(16)));
  typedef double __m128d __attribute__((vector_size(16)));
#endif /* JACL_HAS_NEON */

static_assert(sizeof(__m128) == 16, "Vector size mismatch");
static_assert(sizeof(__m128d) == 16, "Vector size mismatch");

/* ============================================================ */
/* Vector Initialization                                        */
/* ============================================================ */

static inline __m128  _mm_set1_ps(float x)  { return (__m128){ x, x, x, x }; }
static inline __m128d _mm_set1_pd(double x) { return (__m128d){ x, x }; }
static inline __m128  _mm_set_ss(float  x) { return (__m128){ x, 0, 0, 0 }; }
static inline __m128d _mm_set_sd(double x) { return (__m128d){ x, 0 }; }
static inline __m128  _mm_set_ps(float e3, float e2, float e1, float e0) { return (__m128){ e0, e1, e2, e3 }; }
static inline __m128d _mm_set_pd(double e1, double e0) { return (__m128d){ e0, e1 }; }
static inline __m128  _mm_setzero_ps(void) { return (__m128){ 0, 0, 0, 0 }; }
static inline __m128d _mm_setzero_pd(void) { return (__m128d){ 0, 0 }; }

/* ============================================================ */
/* Scalar ↔ Vector Moves                                        */
/* ============================================================ */

static inline float  _mm_cvtss_f32(__m128  v) { return v[0]; }
static inline double _mm_cvtsd_f64(__m128d v) { return v[0]; }

/* ============================================================ */
/* Memory Operations                                            */
/* ============================================================ */

static inline __m128  _mm_load_ps(const float *p) { return *(__m128*)p; }
static inline __m128d _mm_load_pd(const double *p) { return *(__m128d*)p; }
static inline __m128  _mm_loadu_ps(const float *p) { return (__m128){ p[0], p[1], p[2], p[3] }; }
static inline __m128d _mm_loadu_pd(const double *p) { return (__m128d){ p[0], p[1] }; }
static inline void _mm_store_ps(float *p, __m128 v) { *(__m128*)p = v; }
static inline void _mm_store_pd(double *p, __m128d v) { *(__m128d*)p = v; }
static inline void _mm_storeu_ps(float *p, __m128 v) { p[0] = v[0]; p[1] = v[1]; p[2] = v[2]; p[3] = v[3]; }
static inline void _mm_storeu_pd(double *p, __m128d v) { p[0] = v[0]; p[1] = v[1]; }

/* ============================================================ */
/* Basic Arithmetic (Always Hardware-Accelerated)               */
/* ============================================================ */

static inline __m128  _mm_add_ps(__m128  a, __m128  b) { return a + b; }
static inline __m128d _mm_add_pd(__m128d a, __m128d b) { return a + b; }
static inline __m128  _mm_sub_ps(__m128  a, __m128  b) { return a - b; }
static inline __m128d _mm_sub_pd(__m128d a, __m128d b) { return a - b; }
static inline __m128  _mm_mul_ps(__m128  a, __m128  b) { return a * b; }
static inline __m128d _mm_mul_pd(__m128d a, __m128d b) { return a * b; }
static inline __m128  _mm_div_ps(__m128  a, __m128  b) { return a / b; }
static inline __m128d _mm_div_pd(__m128d a, __m128d b) { return a / b; }

/* ============================================================ */
/* Min/Max                                                      */
/* ============================================================ */

static inline __m128  _mm_min_ps(__m128 a, __m128 b) { return (__m128){a[0]<b[0]?a[0]:b[0], a[1]<b[1]?a[1]:b[1], a[2]<b[2]?a[2]:b[2], a[3]<b[3]?a[3]:b[3]}; }
static inline __m128d _mm_min_pd(__m128d a, __m128d b) { return (__m128d){a[0]<b[0]?a[0]:b[0], a[1]<b[1]?a[1]:b[1]}; }
static inline __m128  _mm_max_ps(__m128 a, __m128 b) { return (__m128){a[0]>b[0]?a[0]:b[0], a[1]>b[1]?a[1]:b[1], a[2]>b[2]?a[2]:b[2], a[3]>b[3]?a[3]:b[3]}; }
static inline __m128d _mm_max_pd(__m128d a, __m128d b) { return (__m128d){a[0]>b[0]?a[0]:b[0], a[1]>b[1]?a[1]:b[1]}; }

/* ============================================================ */
/* Comparisons (Return all-ones or all-zeros per lane)          */
/* ============================================================ */

static inline __m128  _mm_cmpeq_ps(__m128  a, __m128  b) { return (__m128)(a == b); }
static inline __m128d _mm_cmpeq_pd(__m128d a, __m128d b) { return (__m128d)(a == b); }
static inline __m128  _mm_cmplt_ps(__m128  a, __m128  b) { return (__m128)(a < b); }
static inline __m128d _mm_cmplt_pd(__m128d a, __m128d b) { return (__m128d)(a < b); }
static inline __m128  _mm_cmple_ps(__m128  a, __m128  b) { return (__m128)(a <= b); }
static inline __m128d _mm_cmple_pd(__m128d a, __m128d b) { return (__m128d)(a <= b); }
static inline __m128  _mm_cmpgt_ps(__m128  a, __m128  b) { return (__m128)(a > b); }
static inline __m128d _mm_cmpgt_pd(__m128d a, __m128d b) { return (__m128d)(a > b); }
static inline __m128  _mm_cmpge_ps(__m128  a, __m128  b) { return (__m128)(a >= b); }
static inline __m128d _mm_cmpge_pd(__m128d a, __m128d b) { return (__m128d)(a >= b); }

/* ============================================================ */
/* Bitwise Operations (Logical AND/OR/XOR/NOT)                  */
/* ============================================================ */

static inline __m128  _mm_and_ps(__m128 a, __m128 b) { typedef int v4si __attribute__((vector_size(16))); return (__m128)((v4si)a & (v4si)b); }
static inline __m128d _mm_and_pd(__m128d a, __m128d b) { typedef long long v2di __attribute__((vector_size(16))); return (__m128d)((v2di)a & (v2di)b); }
static inline __m128  _mm_or_ps(__m128 a, __m128 b) { typedef int v4si __attribute__((vector_size(16))); return (__m128)((v4si)a | (v4si)b); }
static inline __m128d _mm_or_pd(__m128d a, __m128d b) { typedef long long v2di __attribute__((vector_size(16))); return (__m128d)((v2di)a | (v2di)b); }
static inline __m128  _mm_xor_ps(__m128 a, __m128 b) { typedef int v4si __attribute__((vector_size(16))); return (__m128)((v4si)a ^ (v4si)b); }
static inline __m128d _mm_xor_pd(__m128d a, __m128d b) { typedef long long v2di __attribute__((vector_size(16))); return (__m128d)((v2di)a ^ (v2di)b); }
static inline __m128  _mm_andnot_ps(__m128 a, __m128 b) { typedef int v4si __attribute__((vector_size(16))); return (__m128)((~(v4si)a) & (v4si)b); }
static inline __m128d _mm_andnot_pd(__m128d a, __m128d b) { typedef long long v2di __attribute__((vector_size(16))); return (__m128d)((~(v2di)a) & (v2di)b); }

/* ============================================================ */
/* Horizontal Operations (Reduce across lanes)                  */
/* ============================================================ */

static inline float  _mm_reduce_add_ps(__m128 v) { return v[0] + v[1] + v[2] + v[3]; }
static inline double _mm_reduce_add_pd(__m128d v) { return v[0] + v[1]; }
static inline float  _mm_reduce_max_ps(__m128 v) { float m = v[0]; if (v[1] > m) m = v[1]; if (v[2] > m) m = v[2]; if (v[3] > m) m = v[3]; return m; }
static inline double _mm_reduce_max_pd(__m128d v) { return v[0] > v[1] ? v[0] : v[1]; }
static inline float  _mm_reduce_min_ps(__m128 v) { float m = v[0]; if (v[1] < m) m = v[1]; if (v[2] < m) m = v[2]; if (v[3] < m) m = v[3]; return m; }
static inline double _mm_reduce_min_pd(__m128d v) { return v[0] < v[1] ? v[0] : v[1]; }

/* ============================================================ */
/* Hardware-Accelerated Square Root                             */
/* ============================================================ */

#if JACL_HAS_NEON
  static inline __m128  _mm_sqrt_ps(__m128 v) { return vsqrtq_f32(v); }
  static inline __m128d _mm_sqrt_pd(__m128d v) { return vsqrtq_f64(v); }
#elif JACL_HAS_SSE2
  static inline __m128  _mm_sqrt_ps(__m128 v) { return __builtin_ia32_sqrtps(v); }
  static inline __m128d _mm_sqrt_pd(__m128d v) { return __builtin_ia32_sqrtpd(v); }
#else
  static inline __m128  _mm_sqrt_ps(__m128 v) { return (__m128){ sqrtf(v[0]), sqrtf(v[1]), sqrtf(v[2]), sqrtf(v[3]) }; }
  static inline __m128d _mm_sqrt_pd(__m128d v) { return (__m128d){ sqrt(v[0]), sqrt(v[1]) }; }
#endif

/* ============================================================ */
/* Fused Multiply-Add (FMA)                                     */
/* ============================================================ */

#if JACL_HAS_NEON
  static inline __m128  _mm_fmadd_ps(__m128 a, __m128 b, __m128 c) { return vfmaq_f32(c, a, b); }
  static inline __m128d _mm_fmadd_pd(__m128d a, __m128d b, __m128d c) { return vfmaq_f64(c, a, b); }
#elif JACL_HAS_FMA
  static inline __m128  _mm_fmadd_ps(__m128 a, __m128 b, __m128 c) { return __builtin_ia32_vfmaddps(a, b, c); }
  static inline __m128d _mm_fmadd_pd(__m128d a, __m128d b, __m128d c) { return __builtin_ia32_vfmaddpd(a, b, c); }
#else
  static inline __m128  _mm_fmadd_ps(__m128  a, __m128  b, __m128  c) { return a*b + c; }
  static inline __m128d _mm_fmadd_pd(__m128d a, __m128d b, __m128d c) { return a*b + c; }
#endif

/* ============================================================ */
/* Rounding Functions (SSE4.1+ / NEON)                          */
/* ============================================================ */

#if JACL_HAS_NEON
  static inline __m128  _mm_ceil_ps(__m128 v)  { return vrndpq_f32(v); }
  static inline __m128d _mm_ceil_pd(__m128d v) { return vrndpq_f64(v); }
  static inline __m128  _mm_floor_ps(__m128 v)  { return vrndmq_f32(v); }
  static inline __m128d _mm_floor_pd(__m128d v) { return vrndmq_f64(v); }
  static inline __m128  _mm_trunc_ps(__m128 v)  { return vrndq_f32(v); }
  static inline __m128d _mm_trunc_pd(__m128d v) { return vrndq_f64(v); }
  static inline __m128  _mm_round_ps(__m128 v)  { return vrndnq_f32(v); }
  static inline __m128d _mm_round_pd(__m128d v) { return vrndnq_f64(v); }
#elif JACL_HAS_SSE4_1
  static inline __m128  _mm_ceil_ps(__m128 v)  { return __builtin_ia32_roundps(v, 0x02); }
  static inline __m128d _mm_ceil_pd(__m128d v) { return __builtin_ia32_roundpd(v, 0x02); }
  static inline __m128  _mm_floor_ps(__m128 v)  { return __builtin_ia32_roundps(v, 0x01); }
  static inline __m128d _mm_floor_pd(__m128d v) { return __builtin_ia32_roundpd(v, 0x01); }
  static inline __m128  _mm_trunc_ps(__m128 v)  { return __builtin_ia32_roundps(v, 0x03); }
  static inline __m128d _mm_trunc_pd(__m128d v) { return __builtin_ia32_roundpd(v, 0x03); }
  static inline __m128  _mm_round_ps(__m128 v)  { return __builtin_ia32_roundps(v, 0x00); }
  static inline __m128d _mm_round_pd(__m128d v) { return __builtin_ia32_roundpd(v, 0x00); }
#else
  // Fallback: auto-vectorize scalar calls
  static inline __m128  _mm_ceil_ps(__m128 v)  { return (__m128){ ceilf(v[0]), ceilf(v[1]), ceilf(v[2]), ceilf(v[3]) }; }
  static inline __m128d _mm_ceil_pd(__m128d v) { return (__m128d){ ceil(v[0]), ceil(v[1]) }; }
  static inline __m128  _mm_floor_ps(__m128 v)  { return (__m128){ floorf(v[0]), floorf(v[1]), floorf(v[2]), floorf(v[3]) }; }
  static inline __m128d _mm_floor_pd(__m128d v) { return (__m128d){ floor(v[0]), floor(v[1]) }; }
  static inline __m128  _mm_trunc_ps(__m128 v)  { return (__m128){ truncf(v[0]), truncf(v[1]), truncf(v[2]), truncf(v[3]) }; }
  static inline __m128d _mm_trunc_pd(__m128d v) { return (__m128d){ trunc(v[0]), trunc(v[1]) }; }
  static inline __m128  _mm_round_ps(__m128 v)  { return (__m128){ roundf(v[0]), roundf(v[1]), roundf(v[2]), roundf(v[3]) }; }
  static inline __m128d _mm_round_pd(__m128d v) { return (__m128d){ round(v[0]), round(v[1]) }; }
#endif

/* ============================================================ */
/* Trigonometry (Auto-Vectorized)                               */
/* ============================================================ */

static inline __m128  _mm_sin_ps(__m128 v) { return (__m128){ sinf(v[0]), sinf(v[1]), sinf(v[2]), sinf(v[3]) }; }
static inline __m128d _mm_sin_pd(__m128d v) { return (__m128d){ sin(v[0]), sin(v[1]) }; }
static inline __m128  _mm_cos_ps(__m128 v) { return (__m128){ cosf(v[0]), cosf(v[1]), cosf(v[2]), cosf(v[3]) }; }
static inline __m128d _mm_cos_pd(__m128d v) { return (__m128d){ cos(v[0]), cos(v[1]) }; }
static inline __m128  _mm_tan_ps(__m128 v) { return (__m128){ tanf(v[0]), tanf(v[1]), tanf(v[2]), tanf(v[3]) }; }
static inline __m128d _mm_tan_pd(__m128d v) { return (__m128d){ tan(v[0]), tan(v[1]) }; }

/* ============================================================ */
/* Exponential & Logarithm (Auto-Vectorized)                    */
/* ============================================================ */

static inline __m128  _mm_exp_ps(__m128 v) { return (__m128){ expf(v[0]), expf(v[1]), expf(v[2]), expf(v[3]) }; }
static inline __m128d _mm_exp_pd(__m128d v) { return (__m128d){ exp(v[0]), exp(v[1]) }; }
static inline __m128  _mm_log_ps(__m128 v) { return (__m128){ logf(v[0]), logf(v[1]), logf(v[2]), logf(v[3]) }; }
static inline __m128d _mm_log_pd(__m128d v) { return (__m128d){ log(v[0]), log(v[1]) }; }


/* ============================================================ */
/* Power & Hypotenuse (Auto-Vectorized)                         */
/* ============================================================ */

static inline __m128  _mm_pow_ps(__m128 a, __m128 b) { return (__m128){ powf(a[0],b[0]), powf(a[1],b[1]), powf(a[2],b[2]), powf(a[3],b[3]) }; }
static inline __m128d _mm_pow_pd(__m128d a, __m128d b) { return (__m128d){ pow(a[0],b[0]), pow(a[1],b[1]) }; }
static inline __m128  _mm_hypot_ps(__m128 a, __m128 b) { return (__m128){ hypotf(a[0],b[0]), hypotf(a[1],b[1]), hypotf(a[2],b[2]), hypotf(a[3],b[3]) }; }
static inline __m128d _mm_hypot_pd(__m128d a, __m128d b) { return (__m128d){ hypot(a[0],b[0]), hypot(a[1],b[1]) }; }

/* ============================================================ */
/* Hyperbolic Functions (Auto-Vectorized)                       */
/* ============================================================ */

#define __jacl_svml_hyp(name) \
static inline __m128 _mm_##name##_ps(__m128 v) { return (__m128){ name##f(v[0]), name##f(v[1]), name##f(v[2]), name##f(v[3]) }; } \
static inline __m128d _mm_##name##_pd(__m128d v) { return (__m128d){ name(v[0]), name(v[1]) }; }
__jacl_svml_hyp(sinh) __jacl_svml_hyp(cosh) __jacl_svml_hyp(tanh) __jacl_svml_hyp(asinh) __jacl_svml_hyp(acosh) __jacl_svml_hyp(atanh)
#undef __jacl_svml_hyp

/* ============================================================ */
/* Error & Gamma Functions (Auto-Vectorized)                    */
/* ============================================================ */

static inline __m128  _mm_erf_ps(__m128 v) { return (__m128){ erff(v[0]), erff(v[1]), erff(v[2]), erff(v[3]) }; }
static inline __m128d _mm_erf_pd(__m128d v) { return (__m128d){ erf(v[0]), erf(v[1]) }; }
static inline __m128  _mm_tgamma_ps(__m128 v) { return (__m128){ tgammaf(v[0]), tgammaf(v[1]), tgammaf(v[2]), tgammaf(v[3]) }; }
static inline __m128d _mm_tgamma_pd(__m128d v) { return (__m128d){ tgamma(v[0]), tgamma(v[1]) }; }

#ifdef __cplusplus
}
#endif

#endif /* IMMINTRIN_H */
