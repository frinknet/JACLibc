/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef CRYPTO_ED25519_H
#define CRYPTO_ED25519_H
#pragma once

#include <crypto/curve25519.h>
#include <crypto/sha2.h>

#ifdef __cplusplus
extern "C" {
#endif

// ========================================================================
// Types and constants
// ========================================================================

typedef struct { __jacl_fe X, Y, Z, T; } __jacl_ge_p3;
typedef struct { __jacl_fe X, Y, Z, T; } __jacl_ge_p1p1;
typedef struct { __jacl_fe X, Y, Z; } __jacl_ge_p2;
typedef struct { __jacl_fe YplusX, YminusX, Z, T2d; } __jacl_ge_cached;

static const __jacl_fe __jacl_ed25519_d = {
	-10913610, 13857413, -15372611, 6949391, 114729,
	-8787816, -6275908, -3247719, -18696448, -12055116
};

static const __jacl_fe __jacl_ed25519_d2 = {
	-21827239, -5839606, -30745221, 13898782, 229458,
	15978800, -12551817, -6495438, 29715968, 9444199
};

static const __jacl_fe __jacl_ed25519_sqrtm1 = {
	-32595792, -7943725, 9377950, 3500415, 12389472,
	-272473, -25146209, -2005654, 326686, 11406482
};

static const __jacl_ge_p3 __jacl_ed25519_base = {
	{15112221, -25300017, 5540086, 13697942, 21897556, -9603066, -12541208, -8608938, 14324289, 8266939},
	{26843798, 7645883, -11081449, 10857855, -1596721, -4805065, 23048729, 3061454, 3899824, -3351711},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{-1106003, 6755241, 9099226, -5643147, -6867341, -13135106, -12628327, 7048921, -1344773, 6957743}
};

// ========================================================================
// Compact scalar arithmetic modulo L
// ========================================================================

static inline void __jacl_sc_reduce(uint8_t s[64]) {
	int64_t t[24] = {0};

	for (int i = 0; i < 24; i++) t[i] = 2097151 & (__jacl_load32_le(s + (i * 21 / 8)) >> ((i * 21) % 8));

	t[23] = (__jacl_load32_le(s + 60) >> 3);

	for (int i = 23; i >= 12; i--) {
		t[i-12] += t[i] * 666643;
		t[i-11] += t[i] * 470296;
		t[i-10] += t[i] * 654183;
		t[i-9]  -= t[i] * 997805;
		t[i-8]  += t[i] * 136657;
		t[i-7]  -= t[i] * 683901;
	}

	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 11; i += 2) {
			int64_t carry = (t[i] + (1 << 20)) >> 21;

			t[i+1] += carry;
			t[i] -= carry << 21;
		}

		for (int i = 1; i < 12; i += 2) {
			int64_t carry = (t[i] + (1 << 20)) >> 21;

			t[i+1] += carry;
			t[i] -= carry << 21;
		}
	}

	for (int i = 0; i < 32; i++) s[i] = 0;

	s[0] = t[0]; s[1] = t[0] >> 8; s[2] = (t[0] >> 16) | (t[1] << 5);
	s[3] = t[1] >> 3; s[4] = t[1] >> 11; s[5] = (t[1] >> 19) | (t[2] << 2);
	s[6] = t[2] >> 6; s[7] = (t[2] >> 14) | (t[3] << 7);
	s[8] = t[3] >> 1; s[9] = t[3] >> 9; s[10] = (t[3] >> 17) | (t[4] << 4);
	s[11] = t[4] >> 4; s[12] = t[4] >> 12; s[13] = (t[4] >> 20) | (t[5] << 1);
	s[14] = t[5] >> 7; s[15] = (t[5] >> 15) | (t[6] << 6);
	s[16] = t[6] >> 2; s[17] = t[6] >> 10; s[18] = (t[6] >> 18) | (t[7] << 3);
	s[19] = t[7] >> 5; s[20] = t[7] >> 13;
	s[21] = t[8]; s[22] = t[8] >> 8; s[23] = (t[8] >> 16) | (t[9] << 5);
	s[24] = t[9] >> 3; s[25] = t[9] >> 11; s[26] = (t[9] >> 19) | (t[10] << 2);
	s[27] = t[10] >> 6; s[28] = (t[10] >> 14) | (t[11] << 7);
	s[29] = t[11] >> 1; s[30] = t[11] >> 9; s[31] = t[11] >> 17;
}

static inline void __jacl_sc_muladd(uint8_t s[32], const uint8_t a[32], const uint8_t b[32], const uint8_t c[32]) {
	int64_t at[12], bt[12], ct[12], st[24] = {0};

	for (int i = 0; i < 12; i++) {
		at[i] = 2097151 & (__jacl_load32_le(a + (i * 21 / 8)) >> ((i * 21) % 8));
		bt[i] = 2097151 & (__jacl_load32_le(b + (i * 21 / 8)) >> ((i * 21) % 8));
		ct[i] = 2097151 & (__jacl_load32_le(c + (i * 21 / 8)) >> ((i * 21) % 8));
	}

	at[11] = __jacl_load32_le(a + 28) >> 7;
	bt[11] = __jacl_load32_le(b + 28) >> 7;
	ct[11] = __jacl_load32_le(c + 28) >> 7;

	for (int i = 0; i < 12; i++) {
		for (int j = 0; j < 12; j++) st[i + j] += at[i] * bt[j];
	}

	for (int i = 0; i < 12; i++) st[i] += ct[i];

	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 23; i += 2) {
			int64_t carry = (st[i] + (1 << 20)) >> 21;

			st[i+1] += carry;
			st[i] -= carry << 21;
		}

		for (int i = 1; i < 23; i += 2) {
			int64_t carry = (st[i] + (1 << 20)) >> 21;

			st[i+1] += carry;
			st[i] -= carry << 21;
		}
	}

	for (int i = 23; i >= 12; i--) {
		st[i-12] += st[i] * 666643;
		st[i-11] += st[i] * 470296;
		st[i-10] += st[i] * 654183;
		st[i-9]  -= st[i] * 997805;
		st[i-8]  += st[i] * 136657;
		st[i-7]  -= st[i] * 683901;
	}

	for (int j = 0; j < 2; j++) {
		for (int i = 0; i < 11; i += 2) {
			int64_t carry = (st[i] + (1 << 20)) >> 21;

			st[i+1] += carry;
			st[i] -= carry << 21;
		}

		for (int i = 1; i < 12; i += 2) {
			int64_t carry = (st[i] + (1 << 20)) >> 21;

			st[i+1] += carry;
			st[i] -= carry << 21;
		}
	}

	s[0] = st[0]; s[1] = st[0] >> 8; s[2] = (st[0] >> 16) | (st[1] << 5);
	s[3] = st[1] >> 3; s[4] = st[1] >> 11; s[5] = (st[1] >> 19) | (st[2] << 2);
	s[6] = st[2] >> 6; s[7] = (st[2] >> 14) | (st[3] << 7);
	s[8] = st[3] >> 1; s[9] = st[3] >> 9; s[10] = (st[3] >> 17) | (st[4] << 4);
	s[11] = st[4] >> 4; s[12] = st[4] >> 12; s[13] = (st[4] >> 20) | (st[5] << 1);
	s[14] = st[5] >> 7; s[15] = (st[5] >> 15) | (st[6] << 6);
	s[16] = st[6] >> 2; s[17] = st[6] >> 10; s[18] = (st[6] >> 18) | (st[7] << 3);
	s[19] = st[7] >> 5; s[20] = st[7] >> 13;
	s[21] = st[8]; s[22] = st[8] >> 8; s[23] = (st[8] >> 16) | (st[9] << 5);
	s[24] = st[9] >> 3; s[25] = st[9] >> 11; s[26] = (st[9] >> 19) | (st[10] << 2);
	s[27] = st[10] >> 6; s[28] = (st[10] >> 14) | (st[11] << 7);
	s[29] = st[11] >> 1; s[30] = st[11] >> 9; s[31] = st[11] >> 17;
}

// ========================================================================
// Field and point helpers
// ========================================================================

static inline int __jacl_fe_isnegative(const __jacl_fe f) {
	uint8_t s[32];
	__jacl_fe_tobytes(s, f);

	return s[0] & 1;
}

static inline int __jacl_fe_isnonzero(const __jacl_fe f) {
	uint8_t s[32];
	__jacl_fe_tobytes(s, f);
	int r = 0;

	for (int i = 0; i < 32; i++) r |= s[i];

	return r != 0;
}

static inline void __jacl_fe_neg(__jacl_fe h, const __jacl_fe f) {
	for (int i = 0; i < 10; i++) h[i] = -f[i];
}

static inline void __jacl_fe_pow22523(__jacl_fe out, const __jacl_fe z) {
	__jacl_fe t0, t1, t2;
	__jacl_fe_sq(t0, z);
	__jacl_fe_sq(t1, t0);
	__jacl_fe_sq(t1, t1);
	__jacl_fe_mul(t1, z, t1);
	__jacl_fe_mul(t0, t0, t1);
	__jacl_fe_sq(t0, t0);
	__jacl_fe_mul(t0, t1, t0);
	__jacl_fe_sq(t1, t0);

	for (int i = 1; i < 5; i++) __jacl_fe_sq(t1, t1);

	__jacl_fe_mul(t0, t1, t0);
	__jacl_fe_sq(t1, t0);

	for (int i = 1; i < 10; i++) __jacl_fe_sq(t1, t1);

	__jacl_fe_mul(t1, t1, t0);
	__jacl_fe_sq(t2, t1);

	for (int i = 1; i < 20; i++) __jacl_fe_sq(t2, t2);

	__jacl_fe_mul(t1, t2, t1);
	__jacl_fe_sq(t1, t1);

	for (int i = 1; i < 10; i++) __jacl_fe_sq(t1, t1);

	__jacl_fe_mul(t0, t1, t0);
	__jacl_fe_sq(t1, t0);

	for (int i = 1; i < 50; i++) __jacl_fe_sq(t1, t1);

	__jacl_fe_mul(t1, t1, t0);
	__jacl_fe_sq(t2, t1);

	for (int i = 1; i < 100; i++) __jacl_fe_sq(t2, t2);

	__jacl_fe_mul(t1, t2, t1);
	__jacl_fe_sq(t1, t1);

	for (int i = 1; i < 50; i++) __jacl_fe_sq(t1, t1);

	__jacl_fe_mul(t0, t1, t0);
	__jacl_fe_sq(t0, t0);
	__jacl_fe_sq(t0, t0);
	__jacl_fe_mul(out, t0, z);
}

static inline void __jacl_ge_p3_0(__jacl_ge_p3 *h) {
	__jacl_fe_0(h->X);
	__jacl_fe_1(h->Y);
	__jacl_fe_1(h->Z);
	__jacl_fe_0(h->T);
}

static inline void __jacl_ge_p2_0(__jacl_ge_p2 *h) {
	__jacl_fe_0(h->X);
	__jacl_fe_1(h->Y);
	__jacl_fe_1(h->Z);
}

static inline void __jacl_ge_p3_to_p2(__jacl_ge_p2 *r, const __jacl_ge_p3 *p) {
	__jacl_fe_copy(r->X, p->X);
	__jacl_fe_copy(r->Y, p->Y);
	__jacl_fe_copy(r->Z, p->Z);
}

static inline void __jacl_ge_p2_to_p3(__jacl_ge_p3 *r, const __jacl_ge_p2 *p) {
	__jacl_fe_copy(r->X, p->X);
	__jacl_fe_copy(r->Y, p->Y);
	__jacl_fe_copy(r->Z, p->Z);
	__jacl_fe_mul(r->T, p->X, p->Y);
}

static inline void __jacl_ge_p1p1_to_p2(__jacl_ge_p2 *r, const __jacl_ge_p1p1 *p) {
	__jacl_fe_mul(r->X, p->X, p->T);
	__jacl_fe_mul(r->Y, p->Y, p->Z);
	__jacl_fe_mul(r->Z, p->Z, p->T);
}

static inline void __jacl_ge_p1p1_to_p3(__jacl_ge_p3 *r, const __jacl_ge_p1p1 *p) {
	__jacl_fe_mul(r->X, p->X, p->T);
	__jacl_fe_mul(r->Y, p->Y, p->Z);
	__jacl_fe_mul(r->Z, p->Z, p->T);
	__jacl_fe_mul(r->T, p->X, p->Y);
}

static inline void __jacl_ge_p2_dbl(__jacl_ge_p1p1 *r, const __jacl_ge_p2 *p) {
	__jacl_fe t0;
	__jacl_fe_sq(r->X, p->X);
	__jacl_fe_sq(r->Z, p->Y);
	__jacl_fe_sq(r->T, p->Z);
	__jacl_fe_add(r->T, r->T, r->T);
	__jacl_fe_add(r->Y, p->X, p->Y);
	__jacl_fe_sq(t0, r->Y);
	__jacl_fe_add(r->Y, r->Z, r->X);
	__jacl_fe_sub(r->Z, r->Z, r->X);
	__jacl_fe_sub(r->X, t0, r->Y);
	__jacl_fe_sub(r->T, r->T, r->Z);
}

static inline void __jacl_ge_p3_to_cached(__jacl_ge_cached *r, const __jacl_ge_p3 *p) {
	__jacl_fe_add(r->YplusX, p->Y, p->X);
	__jacl_fe_sub(r->YminusX, p->Y, p->X);
	__jacl_fe_copy(r->Z, p->Z);
	__jacl_fe_mul(r->T2d, p->T, __jacl_ed25519_d2);
}

static inline void __jacl_ge_add(__jacl_ge_p1p1 *r, const __jacl_ge_p3 *p, const __jacl_ge_cached *q) {
	__jacl_fe t0;
	__jacl_fe_add(r->X, p->Y, p->X);
	__jacl_fe_sub(r->Y, p->Y, p->X);
	__jacl_fe_mul(r->Z, r->X, q->YplusX);
	__jacl_fe_mul(r->Y, r->Y, q->YminusX);
	__jacl_fe_mul(r->T, q->T2d, p->T);
	__jacl_fe_mul(r->X, p->Z, q->Z);
	__jacl_fe_add(t0, r->X, r->X);
	__jacl_fe_sub(r->X, r->Z, r->Y);
	__jacl_fe_add(r->Y, r->Z, r->Y);
	__jacl_fe_add(r->Z, t0, r->T);
	__jacl_fe_sub(r->T, t0, r->T);
}

static inline void __jacl_ge_sub(__jacl_ge_p1p1 *r, const __jacl_ge_p3 *p, const __jacl_ge_cached *q) {
	__jacl_fe t0;
	__jacl_fe_add(r->X, p->Y, p->X);
	__jacl_fe_sub(r->Y, p->Y, p->X);
	__jacl_fe_mul(r->Z, r->X, q->YminusX);
	__jacl_fe_mul(r->Y, r->Y, q->YplusX);
	__jacl_fe_mul(r->T, q->T2d, p->T);
	__jacl_fe_mul(r->X, p->Z, q->Z);
	__jacl_fe_add(t0, r->X, r->X);
	__jacl_fe_sub(r->X, r->Z, r->Y);
	__jacl_fe_add(r->Y, r->Z, r->Y);
	__jacl_fe_sub(r->Z, t0, r->T);
	__jacl_fe_add(r->T, t0, r->T);
}

static inline void __jacl_ge_p3_tobytes(uint8_t s[32], const __jacl_ge_p3 *h) {
	__jacl_fe recip, x, y;

	__jacl_fe_invert(recip, h->Z);
	__jacl_fe_mul(x, h->X, recip);
	__jacl_fe_mul(y, h->Y, recip);
	__jacl_fe_tobytes(s, y);

	s[31] ^= __jacl_fe_isnegative(x) << 7;
}

static inline void __jacl_ge_tobytes(uint8_t s[32], const __jacl_ge_p2 *h) {
	__jacl_fe recip, x, y;

	__jacl_fe_invert(recip, h->Z);
	__jacl_fe_mul(x, h->X, recip);
	__jacl_fe_mul(y, h->Y, recip);
	__jacl_fe_tobytes(s, y);

	s[31] ^= __jacl_fe_isnegative(x) << 7;
}

static inline int __jacl_ge_frombytes_negate_vartime(__jacl_ge_p3 *h, const uint8_t s[32]) {
	__jacl_fe u, v, v3, vxx, check;

	__jacl_fe_frombytes(h->Y, s);
	__jacl_fe_1(h->Z);
	__jacl_fe_sq(u, h->Y);
	__jacl_fe_mul(v, u, __jacl_ed25519_d);
	__jacl_fe_sub(u, u, h->Z);
	__jacl_fe_add(v, v, h->Z);
	__jacl_fe_sq(v3, v);
	__jacl_fe_mul(v3, v3, v);
	__jacl_fe_sq(h->X, v3);
	__jacl_fe_mul(h->X, h->X, v);
	__jacl_fe_mul(h->X, h->X, u);
	__jacl_fe_pow22523(h->X, h->X);
	__jacl_fe_mul(h->X, h->X, v3);
	__jacl_fe_mul(h->X, h->X, u);
	__jacl_fe_sq(vxx, h->X);
	__jacl_fe_mul(vxx, vxx, v);
	__jacl_fe_sub(check, vxx, u);

	if (__jacl_fe_isnonzero(check)) {
		__jacl_fe_add(check, vxx, u);

		if (__jacl_fe_isnonzero(check)) return -1;

		__jacl_fe_mul(h->X, h->X, __jacl_ed25519_sqrtm1);
	}

	if (__jacl_fe_isnegative(h->X) == (s[31] >> 7)) __jacl_fe_neg(h->X, h->X);

	__jacl_fe_mul(h->T, h->X, h->Y);

	return 0;
}

static inline void __jacl_ge_scalarmult_base(__jacl_ge_p3 *h, const uint8_t a[32]) {
	__jacl_ge_p3_0(h);

	for (int i = 255; i >= 0; i--) {
		__jacl_ge_p2 r2;
		__jacl_ge_p1p1 r1;
		__jacl_ge_p3_to_p2(&r2, h);
		__jacl_ge_p2_dbl(&r1, &r2);
		__jacl_ge_p1p1_to_p3(h, &r1);

		if ((a[i / 8] >> (i % 8)) & 1) {
			__jacl_ge_cached cached;
			__jacl_ge_p3_to_cached(&cached, &__jacl_ed25519_base);
			__jacl_ge_add(&r1, h, &cached);
			__jacl_ge_p1p1_to_p3(h, &r1);
		}
	}
}

static inline void __jacl_ge_double_scalarmult_vartime(__jacl_ge_p2 *r, const uint8_t a[32], const __jacl_ge_p3 *A, const uint8_t b[32]) {
	__jacl_ge_p3 sum; __jacl_ge_p2_0(r);

	for (int i = 255; i >= 0; i--) {
		__jacl_ge_p1p1 t;

		__jacl_ge_p2_dbl(&t, r);
		__jacl_ge_p1p1_to_p2(r, &t);

		int abit = (a[i/8] >> (i%8)) & 1, bbit = (b[i/8] >> (i%8)) & 1;

		if (abit || bbit) {
			__jacl_ge_p2_to_p3(&sum, r);

			if (abit) {
				__jacl_ge_cached bc;
				__jacl_ge_p3_to_cached(&bc, &__jacl_ed25519_base);
				__jacl_ge_add(&t, &sum, &bc);
				__jacl_ge_p1p1_to_p3(&sum, &t);
			}

			if (bbit) {
				__jacl_ge_cached ac;
				__jacl_ge_p3_to_cached(&ac, A);
				__jacl_ge_add(&t, &sum, &ac);
				__jacl_ge_p1p1_to_p3(&sum, &t);
			}

			__jacl_ge_p3_to_p2(r, &sum);
		}
	}
}

static inline int __jacl_crypto_verify_32(const uint8_t x[32], const uint8_t y[32]) {
	uint32_t d = 0;

	for (int i = 0; i < 32; i++) d |= x[i] ^ y[i];

	return (1 & ((d - 1) >> 8)) - 1;
}

// ========================================================================
// PUBLIC API
// ========================================================================

static inline void ed25519_public_key(uint8_t public_key[32], const uint8_t secret_key[32]) {
	uint8_t hash[64]; __jacl_ge_p3 A;

	sha512(secret_key, 32, hash);
	hash[0] &= 248;
	hash[31] &= 63;
	hash[31] |= 64;

	__jacl_ge_scalarmult_base(&A, hash);
	__jacl_ge_p3_tobytes(public_key, &A);
	__jacl_explicit_bzero(hash, sizeof(hash));
}

static inline void ed25519_sign(uint8_t signature[64], const uint8_t *message, size_t mlen, const uint8_t secret_key[32], const uint8_t public_key[32]) {
	uint8_t hash[64], nonce[64], hram[64]; __jacl_ge_p3 R;
	sha512_ctx ctx;

	sha512(secret_key, 32, hash);

	hash[0] &= 248; hash[31] &= 63; hash[31] |= 64;

	sha512_init(&ctx); sha512_update(&ctx, hash + 32, 32);
	sha512_update(&ctx, message, mlen); sha512_final(&ctx, nonce);

	__jacl_sc_reduce(nonce); __jacl_ge_scalarmult_base(&R, nonce);
	__jacl_ge_p3_tobytes(signature, &R);

	sha512_init(&ctx); sha512_update(&ctx, signature, 32);
	sha512_update(&ctx, public_key, 32); sha512_update(&ctx, message, mlen);
	sha512_final(&ctx, hram); __jacl_sc_reduce(hram);

	__jacl_sc_muladd(signature + 32, hram, hash, nonce);
	__jacl_explicit_bzero(hash, sizeof(hash));
	__jacl_explicit_bzero(nonce, sizeof(nonce));
	__jacl_explicit_bzero(hram, sizeof(hram));
}

static inline int ed25519_verify(const uint8_t signature[64], const uint8_t *message, size_t mlen, const uint8_t public_key[32]) {
	uint8_t h[64], rcheck[32];
	__jacl_ge_p3 A;
	__jacl_ge_p2 R;
	sha512_ctx ctx;

	if (signature[63] & 224) return 0;
	if (__jacl_ge_frombytes_negate_vartime(&A, public_key) != 0) return 0;

	sha512_init(&ctx);
	sha512_update(&ctx, signature, 32);
	sha512_update(&ctx, public_key, 32);
	sha512_update(&ctx, message, mlen);
	sha512_final(&ctx, h);
	__jacl_sc_reduce(h);
	__jacl_ge_double_scalarmult_vartime(&R, h, &A, signature + 32);
	__jacl_ge_tobytes(rcheck, &R);

	return __jacl_crypto_verify_32(rcheck, signature) == 0;
}

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_ED25519_H */
