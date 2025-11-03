/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef VECTOR_H
#define VECTOR_H
#pragma once

/**
 * Compiler vectorization hints and diagnostics for loop optimization.
 *
 * To enable vectorization reports, compile with:
 *   -DVECTOR_REPORT
 *
 * Example usage:
 *
 *   VECTORIZE
 *   VECTOR_WIDTH(4)
 *   for (int i = 0; i < n; i++) {
 *     out[i] = sinf(in[i]) * 2.0f;
 *   }
 */

#include <config.h>

/* ============================================================ */
/* Compiler Vectorization Hints (use before loops)             */
/* ============================================================ */

#ifdef __clang__
  #define JACL_VECTORIZE _Pragma("clang loop vectorize(enable)")
  #define JACL_VECTOR_WIDTH(w) _Pragma(JACL_QUOTE(clang loop vectorize_width(w)))
  #define JACL_UNROLL(n) _Pragma(JACL_QUOTE(clang loop unroll_count(n)))
#elif defined(__GNUC__)
  #define JACL_VECTORIZE _Pragma("GCC ivdep")
  #define JACL_VECTOR_WIDTH(w)
  #define JACL_UNROLL(n) _Pragma(JACL_QUOTE(GCC unroll n))
#elif defined(__INTEL_COMPILER)
  #define JACL_VECTORIZE _Pragma("vector always")
  #define JACL_VECTOR_WIDTH(w) _Pragma(JACL_QUOTE(vector vectorlength(w)))
  #define JACL_UNROLL(n) _Pragma(JACL_QUOTE(unroll(n)))
#else /* no vector helpers */
  #define JACL_VECTORIZE
  #define JACL_VECTOR_WIDTH(w)
  #define JACL_UNROLL(n)
#endif /* compiler check */

/* ============================================================ */
/* Vectorization Reporting (compile with -DJACL_VECTOR_REPORT) */
/* ============================================================ */

#ifdef JACL_VECTOR_REPORT
  #if defined(__clang__)
    #pragma clang diagnostic warning "-Rpass=loop-vectorize"
    #pragma clang diagnostic warning "-Rpass-missed=loop-vectorize"
    #pragma clang diagnostic warning "-Rpass-analysis=loop-vectorize"
  #elif defined(__GNUC__)
    #pragma GCC push_options
    #pragma GCC optimize("ftree-vectorize")
    #pragma message "GCC vectorization enabled - compile with -fopt-info-vec to see details"
  #endif /* compiler check */
#endif /* JACL_VECTOR_REPORT */


#define VECTORIZE JACL_VECTORIZE
#define VECTOR_WIDTH(w) JACL_VECTOR_WIDTH(w)
#define UNROLL(n) JACL_UNROLL(n)

#endif /* VECTOR_H */
