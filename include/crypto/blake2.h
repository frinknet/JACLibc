/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef CRYPTO_BLAKE2_H
#define CRYPTO_BLAKE2_H
#pragma once

/**
 * BLAKE2 – Fast cryptographic hash function
 *
 * BLAKE2 is a high-speed hash function faster than MD5, SHA-1, and SHA-2, yet
 * more secure. It's used in Argon2, WireGuard, Zcash, and many modern protocols.
 *
 * **Two variants:**
 *   - BLAKE2b: 64-bit, up to 64-byte output (optimized for 64-bit platforms)
 *   - BLAKE2s: 32-bit, up to 32-byte output (optimized for 32-bit platforms)
 *
 * **Features:**
 *   - Configurable output length (1-64 bytes for b, 1-32 bytes for s)
 *   - Optional keyed MAC mode (like HMAC but faster)
 *   - Salt and personalization parameters
 *   - Tree hashing mode (not implemented here)
 *
 * **Use cases:**
 *   - General-purpose hashing (faster than SHA-2)
 *   - Message authentication (keyed mode)
 *   - Key derivation (with salt/personalization)
 *   - Password hashing (used in Argon2)
 *   - Checksums and integrity checks
 *
 * Namespace: blake2b_*, blake2s_*
 */

#include <crypto/base.h>

#ifdef __cplusplus
extern "C" {
#endif

// ========================================================================
// BLAKE2b (64-bit) – for 64-bit platforms
// ========================================================================

#define BLAKE2B_BLOCKBYTES 128
#define BLAKE2B_OUTBYTES   64
#define BLAKE2B_KEYBYTES   64

typedef struct {
	uint64_t h[8];
	uint64_t t[2];
	uint64_t f[2];
	uint8_t  buf[BLAKE2B_BLOCKBYTES];
	size_t   buflen;
	size_t   outlen;
} blake2b_ctx;

// ========================================================================
// BLAKE2s (32-bit) – for 32-bit platforms and constrained environments
// ========================================================================

#define BLAKE2S_BLOCKBYTES 64
#define BLAKE2S_OUTBYTES   32
#define BLAKE2S_KEYBYTES   32

typedef struct {
	uint32_t h[8];
	uint32_t t[2];
	uint32_t f[2];
	uint8_t  buf[BLAKE2S_BLOCKBYTES];
	size_t   buflen;
	size_t   outlen;
} blake2s_ctx;

// Forward declarations (needed because init calls update)
static inline void blake2b_update(blake2b_ctx *ctx, const void *in, size_t inlen);
static inline void blake2s_update(blake2s_ctx *ctx, const void *in, size_t inlen);

// ========================================================================
// BLAKE2b implementation
// ========================================================================

static const uint64_t __jacl_blake2b_iv[8] = {
	0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL,
	0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
	0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL,
	0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL
};

static const uint8_t __jacl_blake2b_sigma[12][16] = {
	{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
	{14,10,4,8,9,15,13,6,1,12,0,2,11,7,5,3},
	{11,8,12,0,5,2,15,13,10,14,3,6,7,1,9,4},
	{7,9,3,1,13,12,11,14,2,6,5,10,4,0,15,8},
	{9,0,5,7,2,4,10,15,14,1,11,12,6,8,3,13},
	{2,12,6,10,0,11,8,3,4,13,7,5,15,14,1,9},
	{12,5,1,15,14,13,4,10,0,7,6,3,9,2,8,11},
	{13,11,7,14,12,1,3,9,5,0,15,4,8,6,2,10},
	{6,15,14,9,11,3,0,8,12,2,13,7,1,4,10,5},
	{10,2,8,4,7,6,1,5,15,11,9,14,3,12,13,0},
	{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
	{14,10,4,8,9,15,13,6,1,12,0,2,11,7,5,3}
};

#define __jacl_blake2b_g(r,i,a,b,c,d) do { \
	a = a + b + m[__jacl_blake2b_sigma[r][2*i+0]]; \
	d = rotr64(d ^ a, 32); \
	c = c + d; \
	b = rotr64(b ^ c, 24); \
	a = a + b + m[__jacl_blake2b_sigma[r][2*i+1]]; \
	d = rotr64(d ^ a, 16); \
	c = c + d; \
	b = rotr64(b ^ c, 63); \
} while(0)

#define __jacl_blake2b_round(r) do { \
	__jacl_blake2b_g(r,0,v[0],v[4],v[8],v[12]); \
	__jacl_blake2b_g(r,1,v[1],v[5],v[9],v[13]); \
	__jacl_blake2b_g(r,2,v[2],v[6],v[10],v[14]); \
	__jacl_blake2b_g(r,3,v[3],v[7],v[11],v[15]); \
	__jacl_blake2b_g(r,4,v[0],v[5],v[10],v[15]); \
	__jacl_blake2b_g(r,5,v[1],v[6],v[11],v[12]); \
	__jacl_blake2b_g(r,6,v[2],v[7],v[8],v[13]); \
	__jacl_blake2b_g(r,7,v[3],v[4],v[9],v[14]); \
} while(0)

static inline void __jacl_blake2b_compress(blake2b_ctx *ctx, const uint8_t block[BLAKE2B_BLOCKBYTES]) {
	uint64_t m[16];
	uint64_t v[16];

	for (int i = 0; i < 16; i++) m[i] = __jacl_load64_le(block + i * 8);
	for (int i = 0; i < 8; i++) v[i] = ctx->h[i];

	v[8]  = __jacl_blake2b_iv[0];
	v[9]  = __jacl_blake2b_iv[1];
	v[10] = __jacl_blake2b_iv[2];
	v[11] = __jacl_blake2b_iv[3];
	v[12] = __jacl_blake2b_iv[4] ^ ctx->t[0];
	v[13] = __jacl_blake2b_iv[5] ^ ctx->t[1];
	v[14] = __jacl_blake2b_iv[6] ^ ctx->f[0];
	v[15] = __jacl_blake2b_iv[7] ^ ctx->f[1];

	__jacl_blake2b_round(0);
	__jacl_blake2b_round(1);
	__jacl_blake2b_round(2);
	__jacl_blake2b_round(3);
	__jacl_blake2b_round(4);
	__jacl_blake2b_round(5);
	__jacl_blake2b_round(6);
	__jacl_blake2b_round(7);
	__jacl_blake2b_round(8);
	__jacl_blake2b_round(9);
	__jacl_blake2b_round(10);
	__jacl_blake2b_round(11);

	for (int i = 0; i < 8; i++) ctx->h[i] ^= v[i] ^ v[i + 8];
}

#undef __jacl_blake2b_g
#undef __jacl_blake2b_round

// blake2b_init – Initialize BLAKE2b context
static inline int blake2b_init(blake2b_ctx *ctx, size_t outlen, const void *key, size_t keylen) {
	if (outlen == 0 || outlen > BLAKE2B_OUTBYTES) return -1;
	if (keylen > BLAKE2B_KEYBYTES) return -1;
	if (keylen && !key) return -1;
	for (int i = 0; i < 8; i++) ctx->h[i] = __jacl_blake2b_iv[i];

	ctx->h[0] ^= 0x01010000 ^ (keylen << 8) ^ outlen;
	ctx->t[0] = ctx->t[1] = 0;
	ctx->f[0] = ctx->f[1] = 0;
	ctx->buflen = 0;
	ctx->outlen = outlen;

	if (keylen > 0) {
		uint8_t block[BLAKE2B_BLOCKBYTES] = {0};

		memcpy(block, key, keylen);
		blake2b_update(ctx, block, BLAKE2B_BLOCKBYTES);
		__jacl_explicit_bzero(block, BLAKE2B_BLOCKBYTES);
	}

	return 0;
}

// blake2b_update – Add data to hash
static inline void blake2b_update(blake2b_ctx *ctx, const void *in, size_t inlen) {
	const uint8_t *pin = (const uint8_t *)in;

	if (inlen > 0) {
		size_t left = ctx->buflen;
		size_t fill = BLAKE2B_BLOCKBYTES - left;

		if (inlen > fill) {
			ctx->buflen = 0;
			memcpy(ctx->buf + left, pin, fill);

			ctx->t[0] += BLAKE2B_BLOCKBYTES;

			if (ctx->t[0] < BLAKE2B_BLOCKBYTES) ctx->t[1]++;

			__jacl_blake2b_compress(ctx, ctx->buf);

			pin += fill;
			inlen -= fill;

			while (inlen > BLAKE2B_BLOCKBYTES) {
				ctx->t[0] += BLAKE2B_BLOCKBYTES;

				if (ctx->t[0] < BLAKE2B_BLOCKBYTES) ctx->t[1]++;

				__jacl_blake2b_compress(ctx, pin);

				pin += BLAKE2B_BLOCKBYTES;
				inlen -= BLAKE2B_BLOCKBYTES;
			}
		}

		memcpy(ctx->buf + ctx->buflen, pin, inlen);

		ctx->buflen += inlen;
	}
}

// blake2b_final – Finalize hash and extract output
static inline void blake2b_final(blake2b_ctx *ctx, void *out) {
	ctx->t[0] += ctx->buflen;

	if (ctx->t[0] < ctx->buflen) ctx->t[1]++;

	ctx->f[0] = (uint64_t)-1;

	memset(ctx->buf + ctx->buflen, 0, BLAKE2B_BLOCKBYTES - ctx->buflen);
	__jacl_blake2b_compress(ctx, ctx->buf);

	uint8_t *pout = (uint8_t *)out;

	for (size_t i = 0; i < ctx->outlen; i++) pout[i] = (ctx->h[i >> 3] >> (8 * (i & 7))) & 0xFF;

	__jacl_explicit_bzero(ctx, sizeof(blake2b_ctx));
}

// blake2b – One-shot hash
static inline int blake2b(void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen) {
	blake2b_ctx ctx;

	if (blake2b_init(&ctx, outlen, key, keylen) < 0) return -1;

	blake2b_update(&ctx, in, inlen);
	blake2b_final(&ctx, out);

	return 0;
}

// ========================================================================
// BLAKE2s implementation
// ========================================================================

static const uint32_t __jacl_blake2s_iv[8] = {
	0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
	0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19
};

static const uint8_t __jacl_blake2s_sigma[10][16] = {
	{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
	{14,10,4,8,9,15,13,6,1,12,0,2,11,7,5,3},
	{11,8,12,0,5,2,15,13,10,14,3,6,7,1,9,4},
	{7,9,3,1,13,12,11,14,2,6,5,10,4,0,15,8},
	{9,0,5,7,2,4,10,15,14,1,11,12,6,8,3,13},
	{2,12,6,10,0,11,8,3,4,13,7,5,15,14,1,9},
	{12,5,1,15,14,13,4,10,0,7,6,3,9,2,8,11},
	{13,11,7,14,12,1,3,9,5,0,15,4,8,6,2,10},
	{6,15,14,9,11,3,0,8,12,2,13,7,1,4,10,5},
	{10,2,8,4,7,6,1,5,15,11,9,14,3,12,13,0}
};

#define __jacl_blake2s_g(r,i,a,b,c,d) do { \
	a = a + b + m[__jacl_blake2s_sigma[r][2*i+0]]; \
	d = rotr32(d ^ a, 16); \
	c = c + d; \
	b = rotr32(b ^ c, 12); \
	a = a + b + m[__jacl_blake2s_sigma[r][2*i+1]]; \
	d = rotr32(d ^ a, 8); \
	c = c + d; \
	b = rotr32(b ^ c, 7); \
} while(0)

#define __jacl_blake2s_round(r) do { \
	__jacl_blake2s_g(r,0,v[0],v[4],v[8],v[12]); \
	__jacl_blake2s_g(r,1,v[1],v[5],v[9],v[13]); \
	__jacl_blake2s_g(r,2,v[2],v[6],v[10],v[14]); \
	__jacl_blake2s_g(r,3,v[3],v[7],v[11],v[15]); \
	__jacl_blake2s_g(r,4,v[0],v[5],v[10],v[15]); \
	__jacl_blake2s_g(r,5,v[1],v[6],v[11],v[12]); \
	__jacl_blake2s_g(r,6,v[2],v[7],v[8],v[13]); \
	__jacl_blake2s_g(r,7,v[3],v[4],v[9],v[14]); \
} while(0)

static inline void __jacl_blake2s_compress(blake2s_ctx *ctx, const uint8_t block[BLAKE2S_BLOCKBYTES]) {
	uint32_t m[16];
	uint32_t v[16];

	for (int i = 0; i < 16; i++) m[i] = __jacl_load32_le(block + i * 4);
	for (int i = 0; i < 8; i++) v[i] = ctx->h[i];

	v[8]  = __jacl_blake2s_iv[0];
	v[9]  = __jacl_blake2s_iv[1];
	v[10] = __jacl_blake2s_iv[2];
	v[11] = __jacl_blake2s_iv[3];
	v[12] = __jacl_blake2s_iv[4] ^ ctx->t[0];
	v[13] = __jacl_blake2s_iv[5] ^ ctx->t[1];
	v[14] = __jacl_blake2s_iv[6] ^ ctx->f[0];
	v[15] = __jacl_blake2s_iv[7] ^ ctx->f[1];

	__jacl_blake2s_round(0);
	__jacl_blake2s_round(1);
	__jacl_blake2s_round(2);
	__jacl_blake2s_round(3);
	__jacl_blake2s_round(4);
	__jacl_blake2s_round(5);
	__jacl_blake2s_round(6);
	__jacl_blake2s_round(7);
	__jacl_blake2s_round(8);
	__jacl_blake2s_round(9);

	for (int i = 0; i < 8; i++) ctx->h[i] ^= v[i] ^ v[i + 8];
}

#undef __jacl_blake2s_g
#undef __jacl_blake2s_round

// blake2s_init – Initialize BLAKE2s context
static inline int blake2s_init(blake2s_ctx *ctx, size_t outlen, const void *key, size_t keylen) {
	if (outlen == 0 || outlen > BLAKE2S_OUTBYTES) return -1;
	if (keylen > BLAKE2S_KEYBYTES) return -1;
	if (keylen && !key) return -1;

	for (int i = 0; i < 8; i++) ctx->h[i] = __jacl_blake2s_iv[i];

	ctx->h[0] ^= 0x01010000 ^ (keylen << 8) ^ outlen;
	ctx->t[0] = ctx->t[1] = 0;
	ctx->f[0] = ctx->f[1] = 0;
	ctx->buflen = 0;
	ctx->outlen = outlen;

	if (keylen > 0) {
		uint8_t block[BLAKE2S_BLOCKBYTES] = {0};

		memcpy(block, key, keylen);
		blake2s_update(ctx, block, BLAKE2S_BLOCKBYTES);
		__jacl_explicit_bzero(block, BLAKE2S_BLOCKBYTES);
	}

	return 0;
}

// blake2s_update – Add data to hash
static inline void blake2s_update(blake2s_ctx *ctx, const void *in, size_t inlen) {
	const uint8_t *pin = (const uint8_t *)in;

	if (inlen > 0) {
		size_t left = ctx->buflen;
		size_t fill = BLAKE2S_BLOCKBYTES - left;

		if (inlen > fill) {
			ctx->buflen = 0;

			memcpy(ctx->buf + left, pin, fill);

			ctx->t[0] += BLAKE2S_BLOCKBYTES;

			if (ctx->t[0] < BLAKE2S_BLOCKBYTES) ctx->t[1]++;

			__jacl_blake2s_compress(ctx, ctx->buf);

			pin += fill;
			inlen -= fill;

			while (inlen > BLAKE2S_BLOCKBYTES) {
				ctx->t[0] += BLAKE2S_BLOCKBYTES;

				if (ctx->t[0] < BLAKE2S_BLOCKBYTES) ctx->t[1]++;

				__jacl_blake2s_compress(ctx, pin);

				pin += BLAKE2S_BLOCKBYTES;
				inlen -= BLAKE2S_BLOCKBYTES;
			}
		}

		memcpy(ctx->buf + ctx->buflen, pin, inlen);
		ctx->buflen += inlen;
	}
}

// blake2s_final – Finalize hash and extract output
static inline void blake2s_final(blake2s_ctx *ctx, void *out) {
	ctx->t[0] += ctx->buflen;

	if (ctx->t[0] < ctx->buflen) ctx->t[1]++;

	ctx->f[0] = (uint32_t)-1;

	memset(ctx->buf + ctx->buflen, 0, BLAKE2S_BLOCKBYTES - ctx->buflen);
	__jacl_blake2s_compress(ctx, ctx->buf);

	uint8_t *pout = (uint8_t *)out;

	for (size_t i = 0; i < ctx->outlen; i++) pout[i] = (ctx->h[i >> 2] >> (8 * (i & 3))) & 0xFF;

	__jacl_explicit_bzero(ctx, sizeof(blake2s_ctx));
}

// blake2s – One-shot hash
static inline int blake2s(void *out, size_t outlen, const void *in, size_t inlen, const void *key, size_t keylen) {
	blake2s_ctx ctx;

	if (blake2s_init(&ctx, outlen, key, keylen) < 0) return -1;

	blake2s_update(&ctx, in, inlen);
	blake2s_final(&ctx, out);

	return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_BLAKE2_H */
