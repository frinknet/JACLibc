/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef CRYPTO_CURVE25519_H
#define CRYPTO_CURVE25519_H
#pragma once

/**
 * Curve25519 – Scalar Multiplication Primitives
 *
 * Provides scalar multiplication on Curve25519, a Montgomery curve defined by:
 *   y^2 = x^3 + 486662x^2 + x (mod 2^255 - 19)
 *
 * **Use cases:**
 *   - X25519 ECDH key exchange (see x25519.h).
 *   - Ed25519 signatures (see ed25519.h).
 *
 * **Public API:**
 *   - curve25519_scalarmult(q, n, p) – Compute q = n * p
 *   - curve25519_scalarmult_base(q, n) – Compute q = n * G (base point)
 *
 * **Internal field operations:**
 *   All field arithmetic (fe_*) is internal (__jacl_fe_*) and used by x25519.h
 *   and ed25519.h. Not intended for direct use outside this crypto library.
 *
 * Namespace: curve25519_* (public), __jacl_fe_* (internal)
 */

#include <crypto/base.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CURVE25519_KEY_SIZE  32

// ========================================================================
// INTERNAL TYPES: Field element (Fe) – integers mod 2^255 - 19
// ========================================================================

/**
 * __jacl_fe – Field element (10 limbs × 26 bits, partially reduced)
 *
 * Represents integers modulo p = 2^255 - 19.
 * Internal representation allows for some overflow (reduced lazily).
 */
typedef int32_t __jacl_fe[10];

static const int32_t __jacl_fe_121666[10] = {121666,0,0,0,0,0,0,0,0,0};

// ========================================================================
// INTERNAL: Field arithmetic primitives
// ========================================================================

static inline void __jacl_fe_0(__jacl_fe h) {
	memset(h, 0, sizeof(__jacl_fe));
}

static inline void __jacl_fe_1(__jacl_fe h) {
	h[0] = 1;
	for (int i = 1; i < 10; i++) h[i] = 0;
}

static inline void __jacl_fe_copy(__jacl_fe h, const __jacl_fe f) {
	memcpy(h, f, sizeof(__jacl_fe));
}

// Load 32 bytes (little-endian) into field element
static inline void __jacl_fe_frombytes(__jacl_fe h, const uint8_t s[32]) {
	int64_t h0 = __jacl_load32_le(s + 0);
	int64_t h1 = __jacl_load32_le(s + 4);
	int64_t h2 = __jacl_load32_le(s + 8);
	int64_t h3 = __jacl_load32_le(s + 12);
	int64_t h4 = __jacl_load32_le(s + 16);
	int64_t h5 = __jacl_load32_le(s + 20);
	int64_t h6 = __jacl_load32_le(s + 24);
	int64_t h7 = __jacl_load32_le(s + 28);

	h[0] = (int32_t)(h0       & 0x3ffffff);
	h[1] = (int32_t)((h0 >> 26 | h1 << 6)  & 0x3ffffff);
	h[2] = (int32_t)((h1 >> 20 | h2 << 12) & 0x3ffffff);
	h[3] = (int32_t)((h2 >> 14 | h3 << 18) & 0x3ffffff);
	h[4] = (int32_t)((h3 >> 8)  & 0x3ffffff);
	h[5] = (int32_t)((h3 >> 34 | h4 << 30) & 0x3ffffff);
	h[6] = (int32_t)((h4 >> 22 | h5 << 10) & 0x3ffffff);
	h[7] = (int32_t)((h5 >> 16 | h6 << 16) & 0x3ffffff);
	h[8] = (int32_t)((h6 >> 10 | h7 << 22) & 0x3ffffff);
	h[9] = (int32_t)((h7 >> 4)  & 0x1ffffff);
}

// Store field element to 32 bytes (little-endian, fully reduced)
static inline void __jacl_fe_tobytes(uint8_t s[32], const __jacl_fe h) {
	int32_t t[10];

	memcpy(t, h, sizeof(__jacl_fe));

	// Reduce modulo p = 2^255 - 19
	int32_t q = (19 * (t[9] >> 25) + (t[0] >> 26)) >> 26;

	q = (t[0] + 19 * q) >> 26;
	q = (t[1] + q) >> 26;
	q = (t[2] + q) >> 26;
	q = (t[3] + q) >> 26;
	q = (t[4] + q) >> 26;
	q = (t[5] + q) >> 26;
	q = (t[6] + q) >> 26;
	q = (t[7] + q) >> 26;
	q = (t[8] + q) >> 26;
	q = (t[9] + q) >> 25;

	t[0] += 19 * q;

	// Carry propagation
	t[1] += t[0] >> 26; t[0] &= 0x3ffffff;
	t[2] += t[1] >> 26; t[1] &= 0x3ffffff;
	t[3] += t[2] >> 26; t[2] &= 0x3ffffff;
	t[4] += t[3] >> 26; t[3] &= 0x3ffffff;
	t[5] += t[4] >> 26; t[4] &= 0x3ffffff;
	t[6] += t[5] >> 26; t[5] &= 0x3ffffff;
	t[7] += t[6] >> 26; t[6] &= 0x3ffffff;
	t[8] += t[7] >> 26; t[7] &= 0x3ffffff;
	t[9] += t[8] >> 26; t[8] &= 0x3ffffff;
	                    t[9] &= 0x1ffffff;

	// Pack into bytes
	s[0]  = (uint8_t)(t[0]);
	s[1]  = (uint8_t)(t[0] >> 8);
	s[2]  = (uint8_t)(t[0] >> 16);
	s[3]  = (uint8_t)((t[0] >> 24) | (t[1] << 2));
	s[4]  = (uint8_t)(t[1] >> 6);
	s[5]  = (uint8_t)(t[1] >> 14);
	s[6]  = (uint8_t)((t[1] >> 22) | (t[2] << 4));
	s[7]  = (uint8_t)(t[2] >> 4);
	s[8]  = (uint8_t)(t[2] >> 12);
	s[9]  = (uint8_t)((t[2] >> 20) | (t[3] << 6));
	s[10] = (uint8_t)(t[3] >> 2);
	s[11] = (uint8_t)(t[3] >> 10);
	s[12] = (uint8_t)(t[3] >> 18);
	s[13] = (uint8_t)(t[4]);
	s[14] = (uint8_t)(t[4] >> 8);
	s[15] = (uint8_t)(t[4] >> 16);
	s[16] = (uint8_t)((t[4] >> 24) | (t[5] << 2));
	s[17] = (uint8_t)(t[5] >> 6);
	s[18] = (uint8_t)(t[5] >> 14);
	s[19] = (uint8_t)((t[5] >> 22) | (t[6] << 4));
	s[20] = (uint8_t)(t[6] >> 4);
	s[21] = (uint8_t)(t[6] >> 12);
	s[22] = (uint8_t)((t[6] >> 20) | (t[7] << 6));
	s[23] = (uint8_t)(t[7] >> 2);
	s[24] = (uint8_t)(t[7] >> 10);
	s[25] = (uint8_t)(t[7] >> 18);
	s[26] = (uint8_t)(t[8]);
	s[27] = (uint8_t)(t[8] >> 8);
	s[28] = (uint8_t)(t[8] >> 16);
	s[29] = (uint8_t)((t[8] >> 24) | (t[9] << 2));
	s[30] = (uint8_t)(t[9] >> 6);
	s[31] = (uint8_t)(t[9] >> 14);
}

// Addition: h = f + g
static inline void __jacl_fe_add(__jacl_fe h, const __jacl_fe f, const __jacl_fe g) {
	for (int i = 0; i < 10; i++) h[i] = f[i] + g[i];
}

// Subtraction: h = f - g
static inline void __jacl_fe_sub(__jacl_fe h, const __jacl_fe f, const __jacl_fe g) {
	for (int i = 0; i < 10; i++) h[i] = f[i] - g[i];
}

// Multiplication: h = f * g (mod 2^255 - 19)
static inline void __jacl_fe_mul(__jacl_fe h, const __jacl_fe f, const __jacl_fe g) {
	int64_t t[19];

	t[0]  = (int64_t)f[0] * g[0];
	t[1]  = (int64_t)f[0] * g[1] + (int64_t)f[1] * g[0];
	t[2]  = (int64_t)f[0] * g[2] + (int64_t)f[1] * g[1] + (int64_t)f[2] * g[0];
	t[3]  = (int64_t)f[0] * g[3] + (int64_t)f[1] * g[2] + (int64_t)f[2] * g[1] + (int64_t)f[3] * g[0];
	t[4]  = (int64_t)f[0] * g[4] + (int64_t)f[1] * g[3] + (int64_t)f[2] * g[2] + (int64_t)f[3] * g[1] + (int64_t)f[4] * g[0];
	t[5]  = (int64_t)f[0] * g[5] + (int64_t)f[1] * g[4] + (int64_t)f[2] * g[3] + (int64_t)f[3] * g[2] + (int64_t)f[4] * g[1] + (int64_t)f[5] * g[0];
	t[6]  = (int64_t)f[0] * g[6] + (int64_t)f[1] * g[5] + (int64_t)f[2] * g[4] + (int64_t)f[3] * g[3] + (int64_t)f[4] * g[2] + (int64_t)f[5] * g[1] + (int64_t)f[6] * g[0];
	t[7]  = (int64_t)f[0] * g[7] + (int64_t)f[1] * g[6] + (int64_t)f[2] * g[5] + (int64_t)f[3] * g[4] + (int64_t)f[4] * g[3] + (int64_t)f[5] * g[2] + (int64_t)f[6] * g[1] + (int64_t)f[7] * g[0];
	t[8]  = (int64_t)f[0] * g[8] + (int64_t)f[1] * g[7] + (int64_t)f[2] * g[6] + (int64_t)f[3] * g[5] + (int64_t)f[4] * g[4] + (int64_t)f[5] * g[3] + (int64_t)f[6] * g[2] + (int64_t)f[7] * g[1] + (int64_t)f[8] * g[0];
	t[9]  = (int64_t)f[0] * g[9] + (int64_t)f[1] * g[8] + (int64_t)f[2] * g[7] + (int64_t)f[3] * g[6] + (int64_t)f[4] * g[5] + (int64_t)f[5] * g[4] + (int64_t)f[6] * g[3] + (int64_t)f[7] * g[2] + (int64_t)f[8] * g[1] + (int64_t)f[9] * g[0];
	t[10] = (int64_t)f[1] * g[9] + (int64_t)f[2] * g[8] + (int64_t)f[3] * g[7] + (int64_t)f[4] * g[6] + (int64_t)f[5] * g[5] + (int64_t)f[6] * g[4] + (int64_t)f[7] * g[3] + (int64_t)f[8] * g[2] + (int64_t)f[9] * g[1];
	t[11] = (int64_t)f[2] * g[9] + (int64_t)f[3] * g[8] + (int64_t)f[4] * g[7] + (int64_t)f[5] * g[6] + (int64_t)f[6] * g[5] + (int64_t)f[7] * g[4] + (int64_t)f[8] * g[3] + (int64_t)f[9] * g[2];
	t[12] = (int64_t)f[3] * g[9] + (int64_t)f[4] * g[8] + (int64_t)f[5] * g[7] + (int64_t)f[6] * g[6] + (int64_t)f[7] * g[5] + (int64_t)f[8] * g[4] + (int64_t)f[9] * g[3];
	t[13] = (int64_t)f[4] * g[9] + (int64_t)f[5] * g[8] + (int64_t)f[6] * g[7] + (int64_t)f[7] * g[6] + (int64_t)f[8] * g[5] + (int64_t)f[9] * g[4];
	t[14] = (int64_t)f[5] * g[9] + (int64_t)f[6] * g[8] + (int64_t)f[7] * g[7] + (int64_t)f[8] * g[6] + (int64_t)f[9] * g[5];
	t[15] = (int64_t)f[6] * g[9] + (int64_t)f[7] * g[8] + (int64_t)f[8] * g[7] + (int64_t)f[9] * g[6];
	t[16] = (int64_t)f[7] * g[9] + (int64_t)f[8] * g[8] + (int64_t)f[9] * g[7];
	t[17] = (int64_t)f[8] * g[9] + (int64_t)f[9] * g[8];
	t[18] = (int64_t)f[9] * g[9];

	// Reduce: t[10..18] * 2^256 ≡ t[10..18] * 38 (mod 2^255-19)
	t[0] += t[10] * 38;
	t[1] += t[11] * 38;
	t[2] += t[12] * 38;
	t[3] += t[13] * 38;
	t[4] += t[14] * 38;
	t[5] += t[15] * 38;
	t[6] += t[16] * 38;
	t[7] += t[17] * 38;
	t[8] += t[18] * 38;

	// Carry propagation
	int64_t c;

	c = t[0] >> 26; h[0] = (int32_t)(t[0] & 0x3ffffff); t[1] += c;
	c = t[1] >> 26; h[1] = (int32_t)(t[1] & 0x3ffffff); t[2] += c;
	c = t[2] >> 26; h[2] = (int32_t)(t[2] & 0x3ffffff); t[3] += c;
	c = t[3] >> 26; h[3] = (int32_t)(t[3] & 0x3ffffff); t[4] += c;
	c = t[4] >> 26; h[4] = (int32_t)(t[4] & 0x3ffffff); t[5] += c;
	c = t[5] >> 26; h[5] = (int32_t)(t[5] & 0x3ffffff); t[6] += c;
	c = t[6] >> 26; h[6] = (int32_t)(t[6] & 0x3ffffff); t[7] += c;
	c = t[7] >> 26; h[7] = (int32_t)(t[7] & 0x3ffffff); t[8] += c;
	c = t[8] >> 26; h[8] = (int32_t)(t[8] & 0x3ffffff); t[9] += c;
	c = t[9] >> 25; h[9] = (int32_t)(t[9] & 0x1ffffff); h[0] += (int32_t)(c * 19);
	c = h[0] >> 26; h[0] &= 0x3ffffff; h[1] += (int32_t)c;
}

// Squaring: h = f^2 (mod 2^255 - 19)
static inline void __jacl_fe_sq(__jacl_fe h, const __jacl_fe f) {
	__jacl_fe_mul(h, f, f);
}

// Inversion: h = f^(-1) (mod 2^255 - 19) via Fermat's little theorem
static inline void __jacl_fe_invert(__jacl_fe out, const __jacl_fe z) {
	__jacl_fe t0, t1, t2, t3;
	__jacl_fe_sq(t0, z);
	__jacl_fe_sq(t1, t0);
	__jacl_fe_sq(t1, t1);
	__jacl_fe_mul(t1, z, t1);
	__jacl_fe_mul(t0, t0, t1);
	__jacl_fe_sq(t2, t0);
	__jacl_fe_mul(t1, t1, t2);
	__jacl_fe_sq(t2, t1);

	for (int i = 1; i < 5; i++) __jacl_fe_sq(t2, t2);

	__jacl_fe_mul(t1, t2, t1);
	__jacl_fe_sq(t2, t1);

	for (int i = 1; i < 10; i++) __jacl_fe_sq(t2, t2);

	__jacl_fe_mul(t2, t2, t1);
	__jacl_fe_sq(t3, t2);

	for (int i = 1; i < 20; i++) __jacl_fe_sq(t3, t3);

	__jacl_fe_mul(t2, t3, t2);
	__jacl_fe_sq(t2, t2);

	for (int i = 1; i < 10; i++) __jacl_fe_sq(t2, t2);

	__jacl_fe_mul(t1, t2, t1);
	__jacl_fe_sq(t2, t1);

	for (int i = 1; i < 50; i++) __jacl_fe_sq(t2, t2);

	__jacl_fe_mul(t2, t2, t1);
	__jacl_fe_sq(t3, t2);

	for (int i = 1; i < 100; i++) __jacl_fe_sq(t3, t3);

	__jacl_fe_mul(t2, t3, t2);
	__jacl_fe_sq(t2, t2);

	for (int i = 1; i < 50; i++) __jacl_fe_sq(t2, t2);

	__jacl_fe_mul(t1, t2, t1);
	__jacl_fe_sq(t1, t1);

	for (int i = 1; i < 5; i++) __jacl_fe_sq(t1, t1);

	__jacl_fe_mul(out, t1, t0);
}

// Conditional swap (constant-time): swap f and g if b == 1
static inline void __jacl_fe_cswap(__jacl_fe f, __jacl_fe g, int b) {
	int32_t mask = -(int32_t)b;

	for (int i = 0; i < 10; i++) {
		int32_t x = (f[i] ^ g[i]) & mask;

		f[i] ^= x;
		g[i] ^= x;
	}
}

// ========================================================================
// PUBLIC API: Scalar multiplication (Montgomery ladder)
// ========================================================================

/**
 * curve25519_scalarmult – Scalar multiplication on Curve25519
 *
 * Computes q = n * p using the Montgomery ladder (constant-time).
 *
 * Parameters:
 *   q – Output point (32 bytes, x-coordinate only)
 *   n – Scalar (32 bytes, clamped to valid range)
 *   p – Input point (32 bytes, x-coordinate only)
 *
 * Used by X25519 for ECDH: shared_secret = my_secret * their_public.
 */
static inline void curve25519_scalarmult(uint8_t q[32], const uint8_t n[32], const uint8_t p[32]) {
	__jacl_fe x1, x2, z2, x3, z3, tmp0, tmp1;
	uint8_t e[32];

	memcpy(e, n, 32);

	e[0]  &= 248;
	e[31] &= 127;
	e[31] |= 64;

	__jacl_fe_frombytes(x1, p);
	__jacl_fe_1(x2);
	__jacl_fe_0(z2);
	__jacl_fe_copy(x3, x1);
	__jacl_fe_1(z3);

	int swap = 0;

	for (int pos = 254; pos >= 0; pos--) {
		int b = (e[pos / 8] >> (pos & 7)) & 1;

		swap ^= b;

		__jacl_fe_cswap(x2, x3, swap);
		__jacl_fe_cswap(z2, z3, swap);

		swap = b;

		__jacl_fe_sub(tmp0, x3, z3);
		__jacl_fe_sub(tmp1, x2, z2);
		__jacl_fe_add(x2, x2, z2);
		__jacl_fe_add(z2, x3, z3);
		__jacl_fe_mul(z3, tmp0, x2);
		__jacl_fe_mul(z2, z2, tmp1);
		__jacl_fe_sq(tmp0, tmp1);
		__jacl_fe_sq(tmp1, x2);
		__jacl_fe_add(x3, z3, z2);
		__jacl_fe_sub(z2, z3, z2);
		__jacl_fe_mul(x2, tmp1, tmp0);
		__jacl_fe_sub(tmp1, tmp1, tmp0);
		__jacl_fe_sq(z2, z2);


		__jacl_fe_mul(z3, tmp1, __jacl_fe_121666);
		__jacl_fe_sq(x3, x3);
		__jacl_fe_add(tmp0, tmp0, z3);
		__jacl_fe_mul(z3, x1, z2);
		__jacl_fe_mul(z2, tmp1, tmp0);
	}
	__jacl_fe_cswap(x2, x3, swap);
	__jacl_fe_cswap(z2, z3, swap);

	__jacl_fe_invert(z2, z2);
	__jacl_fe_mul(x2, x2, z2);
	__jacl_fe_tobytes(q, x2);
}

/**
 * curve25519_scalarmult_base – Scalar multiplication with base point
 *
 * Computes q = n * G where G is the Curve25519 base point (9).
 *
 * Parameters:
 *   q – Output point (32 bytes)
 *   n – Scalar (32 bytes, clamped to valid range)
 *
 * Used by X25519 to generate public keys: public = secret * G.
 */
static inline void curve25519_scalarmult_base(uint8_t q[32], const uint8_t n[32]) {
	static const uint8_t basepoint[32] = {9};

	curve25519_scalarmult(q, n, basepoint);
}

#ifdef __cplusplus
}
#endif
#endif /* CRYPTO_CURVE25519_H */
