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

#define VECTORIZE JACL_VECTORIZE
#define VECTOR_WIDTH(w) JACL_VECTOR_WIDTH(w)
#define UNROLL(n) JACL_UNROLL(n)

#endif /* VECTOR_H */
