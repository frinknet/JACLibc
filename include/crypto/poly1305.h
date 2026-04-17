/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _CRYPTO_POLY1305_H
#define _CRYPTO_POLY1305_H
#pragma once

/**
 * Poly1305 – One-Time Message Authentication Code (RFC 8439)
 *
 * Poly1305 is a fast, secure MAC designed by Daniel J. Bernstein. It produces
 * a 16-byte (128-bit) authentication tag from a message and a one-time 32-byte key.
 *
 * **CRITICAL: Keys must NEVER be reused. Use a unique key for each message.**
 *
 * **Use cases:**
 *   - ChaCha20-Poly1305 AEAD (most common use, see chacha20.h).
 *   - AES-GCM alternatives with AES-CTR + Poly1305.
 *   - SSH packet authentication (chacha20-poly1305@openssh.com).
 *   - TLS 1.3 cipher suites (with ChaCha20).
 *
 * **Key requirements:**
 *   - 32 bytes (256 bits), typically derived from cipher keystream.
 *   - For ChaCha20-Poly1305: first 32 bytes of ChaCha20(key, nonce, counter=0).
 *   - MUST be unique per message (one-time key).
 *
 * **Security properties:**
 *   - Information-theoretically secure (if key is truly one-time).
 *   - Constant-time implementation (timing-attack resistant).
 *   - 128-bit security against forgery.
 *
 * Namespace: poly1305_*
 */

#include <crypto/base.h>

#ifdef __cplusplus
extern "C" {
#endif

#define POLY1305_KEY_SIZE  32
#define POLY1305_TAG_SIZE  16

// ========================================================================
// TYPES
// ========================================================================

/**
 * poly1305_ctx – Poly1305 state (incremental API)
 *
 * Internal representation uses 26-bit limbs for efficient 32-bit arithmetic.
 */
typedef struct {
	uint32_t r[5];      // Clamped key (26-bit limbs)
	uint32_t h[5];      // Accumulator (26-bit limbs)
	uint32_t pad[4];    // One-time pad (for final tag)
	uint8_t  buf[16];   // Input buffer
	size_t   len;       // Bytes in buffer
} poly1305_ctx;

// ========================================================================
// INTERNAL: Poly1305 block processing
// ========================================================================

static inline void __jacl_poly1305_block(poly1305_ctx *ctx, const uint8_t m[16]) {
	uint64_t d[5];
	uint32_t h0 = ctx->h[0], h1 = ctx->h[1], h2 = ctx->h[2],
	         h3 = ctx->h[3], h4 = ctx->h[4];
	uint32_t r0 = ctx->r[0], r1 = ctx->r[1], r2 = ctx->r[2],
	         r3 = ctx->r[3], r4 = ctx->r[4];

	// Load message block into 26-bit limbs with high bit set
	uint32_t t0 = __jacl_load32_le(m + 0);
	uint32_t t1 = __jacl_load32_le(m + 4);
	uint32_t t2 = __jacl_load32_le(m + 8);
	uint32_t t3 = __jacl_load32_le(m + 12);

	h0 += (t0       & 0x3ffffffU);
	h1 += ((t0 >> 26 | t1 << 6)  & 0x3ffffffU);
	h2 += ((t1 >> 20 | t2 << 12) & 0x3ffffffU);
	h3 += ((t2 >> 14 | t3 << 18) & 0x3ffffffU);
	h4 += ((t3 >> 8)             | 0x01000000U);

	// Multiply: h = (h + m) * r (mod 2^130 - 5)
	uint64_t r1_5 = (uint64_t)r1 * 5;
	uint64_t r2_5 = (uint64_t)r2 * 5;
	uint64_t r3_5 = (uint64_t)r3 * 5;
	uint64_t r4_5 = (uint64_t)r4 * 5;

	d[0] = (uint64_t)h0*r0 + (uint64_t)h1*r4_5 + (uint64_t)h2*r3_5 + (uint64_t)h3*r2_5 + (uint64_t)h4*r1_5;
	d[1] = (uint64_t)h0*r1 + (uint64_t)h1*r0   + (uint64_t)h2*r4_5 + (uint64_t)h3*r3_5 + (uint64_t)h4*r2_5;
	d[2] = (uint64_t)h0*r2 + (uint64_t)h1*r1   + (uint64_t)h2*r0   + (uint64_t)h3*r4_5 + (uint64_t)h4*r3_5;
	d[3] = (uint64_t)h0*r3 + (uint64_t)h1*r2   + (uint64_t)h2*r1   + (uint64_t)h3*r0   + (uint64_t)h4*r4_5;
	d[4] = (uint64_t)h0*r4 + (uint64_t)h1*r3   + (uint64_t)h2*r2   + (uint64_t)h3*r1   + (uint64_t)h4*r0;

	// Carry propagation
	uint32_t c;
	c  = (uint32_t)(d[0] >> 26); h0 = (uint32_t)d[0] & 0x3ffffffU; d[1] += c;
	c  = (uint32_t)(d[1] >> 26); h1 = (uint32_t)d[1] & 0x3ffffffU; d[2] += c;
	c  = (uint32_t)(d[2] >> 26); h2 = (uint32_t)d[2] & 0x3ffffffU; d[3] += c;
	c  = (uint32_t)(d[3] >> 26); h3 = (uint32_t)d[3] & 0x3ffffffU; d[4] += c;
	c  = (uint32_t)(d[4] >> 26); h4 = (uint32_t)d[4] & 0x3ffffffU;
	h0 += c * 5;
	c  = h0 >> 26; h0 &= 0x3ffffffU; h1 += c;

	ctx->h[0] = h0;
	ctx->h[1] = h1;
	ctx->h[2] = h2;
	ctx->h[3] = h3;
	ctx->h[4] = h4;
}

// ========================================================================
// PUBLIC API
// ========================================================================

/**
 * poly1305_init – Initialize Poly1305 context with one-time key
 *
 * Parameters:
 *   ctx – Poly1305 context to initialize
 *   key – 32-byte one-time key (MUST be unique per message)
 *
 * Key format (RFC 8439):
 *   - Bytes 0-15:  r (clamped to specific bit pattern)
 *   - Bytes 16-31: s (one-time pad for final tag)
 */
static inline void poly1305_init(poly1305_ctx *ctx, const uint8_t key[POLY1305_KEY_SIZE]) {
	memset(ctx, 0, sizeof(poly1305_ctx));

	// Load and clamp r (first 16 bytes)
	uint32_t t0 = __jacl_load32_le(key + 0);
	uint32_t t1 = __jacl_load32_le(key + 4);
	uint32_t t2 = __jacl_load32_le(key + 8);
	uint32_t t3 = __jacl_load32_le(key + 12);

	ctx->r[0] =   t0       & 0x0fffffffU;
	ctx->r[1] = ((t0 >> 28) | (t1 << 4))  & 0x0ffffffcU;
	ctx->r[2] = ((t1 >> 24) | (t2 << 8))  & 0x0ffffffcU;
	ctx->r[3] = ((t2 >> 20) | (t3 << 12)) & 0x0ffffffcU;
	ctx->r[4] =  (t3 >> 16)               & 0x0ffffffcU;

	// Load s (second 16 bytes, used as one-time pad)
	for (int i = 0; i < 4; i++)
		ctx->pad[i] = __jacl_load32_le(key + 16 + 4 * i);
}

/**
 * poly1305_update – Process message data (incremental)
 *
 * Can be called multiple times to process message in chunks.
 *
 * Parameters:
 *   ctx  – Poly1305 context (must be initialized)
 *   data – Message data
 *   len  – Length of data in bytes
 */
static inline void poly1305_update(poly1305_ctx *ctx, const uint8_t *data, size_t len) {
	while (len > 0) {
		size_t take = 16 - ctx->len;
		if (take > len) take = len;

		memcpy(ctx->buf + ctx->len, data, take);
		ctx->len += take;
		data     += take;
		len      -= take;

		if (ctx->len == 16) {
			__jacl_poly1305_block(ctx, ctx->buf);
			ctx->len = 0;
		}
	}
}

/**
 * poly1305_final – Finalize and produce 16-byte authentication tag
 *
 * Parameters:
 *   ctx – Poly1305 context
 *   tag – Output buffer for 16-byte tag
 */
static inline void poly1305_final(poly1305_ctx *ctx, uint8_t tag[POLY1305_TAG_SIZE]) {
	// Process remaining buffered data
	if (ctx->len) {
		ctx->buf[ctx->len] = 1;
		for (size_t i = ctx->len + 1; i < 16; i++)
			ctx->buf[i] = 0;
		__jacl_poly1305_block(ctx, ctx->buf);
	}

	// Fully carry h
	uint32_t h0 = ctx->h[0];
	uint32_t h1 = ctx->h[1];
	uint32_t h2 = ctx->h[2];
	uint32_t h3 = ctx->h[3];
	uint32_t h4 = ctx->h[4];

	uint32_t c = h1 >> 26; h1 &= 0x3ffffffU; h2 += c;
	c = h2 >> 26; h2 &= 0x3ffffffU; h3 += c;
	c = h3 >> 26; h3 &= 0x3ffffffU; h4 += c;
	c = h4 >> 26; h4 &= 0x3ffffffU; h0 += c * 5;
	c = h0 >> 26; h0 &= 0x3ffffffU; h1 += c;

	// Compute h + 5 and select h or h+5 based on overflow
	uint32_t g0 = h0 + 5;
	uint32_t g1 = h1 + (g0 >> 26); g0 &= 0x3ffffffU;
	uint32_t g2 = h2 + (g1 >> 26); g1 &= 0x3ffffffU;
	uint32_t g3 = h3 + (g2 >> 26); g2 &= 0x3ffffffU;
	uint32_t g4 = h4 + (g3 >> 26); g3 &= 0x3ffffffU;

	// Constant-time selection mask
	uint32_t mask = (g4 >> 2) - 1;  // All 1s if g4 >= 4, else all 0s
	uint32_t nmask = ~mask;

	h0 = (h0 & nmask) | (g0 & mask);
	h1 = (h1 & nmask) | (g1 & mask);
	h2 = (h2 & nmask) | (g2 & mask);
	h3 = (h3 & nmask) | (g3 & mask);
	h4 = (h4 & nmask) | (g4 & mask);

	// Add one-time pad (s)
	uint64_t f0 = (uint64_t)h0      | ((uint64_t)h1 << 26);
	uint64_t f1 = (uint64_t)h2      | ((uint64_t)h3 << 26) | ((uint64_t)h4 << 52);

	f0 += ctx->pad[0] + ((uint64_t)ctx->pad[1] << 32);
	f1 += ctx->pad[2] + ((uint64_t)ctx->pad[3] << 32);

	__jacl_store32_le(tag + 0,  (uint32_t)f0);
	__jacl_store32_le(tag + 4,  (uint32_t)(f0 >> 32));
	__jacl_store32_le(tag + 8,  (uint32_t)f1);
	__jacl_store32_le(tag + 12, (uint32_t)(f1 >> 32));

	// Wipe sensitive state
	__jacl_explicit_bzero(ctx, sizeof(poly1305_ctx));
}

/**
 * poly1305 – One-shot MAC computation
 *
 * Convenience function for single-pass authentication.
 *
 * Parameters:
 *   key  – 32-byte one-time key
 *   data – Message to authenticate
 *   len  – Message length in bytes
 *   tag  – Output buffer for 16-byte tag
 */
static inline void poly1305(const uint8_t key[POLY1305_KEY_SIZE],
                            const uint8_t *data, size_t len,
                            uint8_t tag[POLY1305_TAG_SIZE]) {
	poly1305_ctx ctx;
	poly1305_init(&ctx, key);
	poly1305_update(&ctx, data, len);
	poly1305_final(&ctx, tag);
}

#ifdef __cplusplus
}
#endif

#endif /* _CRYPTO_POLY1305_H */
