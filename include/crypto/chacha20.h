/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _CRYPTO_CHACHA20_H
#define _CRYPTO_CHACHA20_H
#pragma once

/**
 * ChaCha20 – Stream Cipher and ChaCha20-Poly1305 AEAD (RFC 8439)
 *
 * ChaCha20 is a fast, secure stream cipher designed by Daniel J. Bernstein.
 * It's used standalone for encryption and combined with Poly1305 for AEAD.
 *
 * **Use cases:**
 *   - ChaCha20-Poly1305 AEAD: TLS 1.3, WireGuard, SSH, QUIC (most common).
 *   - ChaCha20 stream cipher: Disk encryption, network protocols.
 *   - Mobile/embedded: Faster than AES on platforms without hardware acceleration.
 *
 * **ChaCha20 parameters:**
 *   - Key: 256 bits (32 bytes)
 *   - Nonce: 96 bits (12 bytes) for IETF variant (RFC 8439)
 *   - Counter: 32-bit block counter (allows ~256 GB per nonce)
 *
 * **ChaCha20-Poly1305 AEAD:**
 *   - Encrypts plaintext with ChaCha20 (counter starts at 1).
 *   - Authenticates AAD (additional authenticated data) + ciphertext with Poly1305.
 *   - Poly1305 key derived from ChaCha20(key, nonce, counter=0).
 *   - Produces 16-byte authentication tag.
 *
 * **Security:**
 *   - 256-bit key security (broken only if key is compromised).
 *   - Nonces MUST be unique per key (use counter or random with 96-bit nonces).
 *   - Constant-time implementation (timing-attack resistant).
 *
 * Namespace: chacha20_*, chacha20poly1305_*
 */

#include <crypto/base.h>
#include <crypto/poly1305.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CHACHA20_KEY_SIZE    32
#define CHACHA20_NONCE_SIZE  12
#define CHACHA20_BLOCK_SIZE  64

// ========================================================================
// TYPES
// ========================================================================

/**
 * chacha20_ctx – ChaCha20 stream cipher state
 */
typedef struct {
	uint32_t state[16];  // ChaCha20 state matrix (4×4)
	uint32_t counter;    // Current block counter
} chacha20_ctx;

// ========================================================================
// INTERNAL: ChaCha20 quarter-round
// ========================================================================

#define __jacl_chacha20_qr(a,b,c,d) do {       \
	a += b; d ^= a; d = rotl32(d, 16);      \
	c += d; b ^= c; b = rotl32(b, 12);      \
	a += b; d ^= a; d = rotl32(d,  8);      \
	c += d; b ^= c; b = rotl32(b,  7);      \
} while (0)

// ========================================================================
// INTERNAL: ChaCha20 block function
// ========================================================================

static inline void __jacl_chacha20_block(uint32_t out[16], const uint32_t in[16]) {
	uint32_t x[16];
	memcpy(x, in, 64);

	// 20 rounds (10 double-rounds)
	for (int i = 0; i < 10; i++) {
		// Column rounds
		__jacl_chacha20_qr(x[0], x[4], x[8],  x[12]);
		__jacl_chacha20_qr(x[1], x[5], x[9],  x[13]);
		__jacl_chacha20_qr(x[2], x[6], x[10], x[14]);
		__jacl_chacha20_qr(x[3], x[7], x[11], x[15]);
		// Diagonal rounds
		__jacl_chacha20_qr(x[0], x[5], x[10], x[15]);
		__jacl_chacha20_qr(x[1], x[6], x[11], x[12]);
		__jacl_chacha20_qr(x[2], x[7], x[8],  x[13]);
		__jacl_chacha20_qr(x[3], x[4], x[9],  x[14]);
	}

	// Add initial state
	for (int i = 0; i < 16; i++)
		out[i] = x[i] + in[i];
}

// ========================================================================
// PUBLIC API: ChaCha20 stream cipher
// ========================================================================

/**
 * chacha20_init – Initialize ChaCha20 context
 *
 * Parameters:
 *   ctx     – ChaCha20 context to initialize
 *   key     – 32-byte key
 *   nonce   – 12-byte nonce (MUST be unique per key)
 *   counter – Initial block counter (typically 0 or 1)
 */
static inline void chacha20_init(chacha20_ctx *ctx,
                                 const uint8_t key[CHACHA20_KEY_SIZE],
                                 const uint8_t nonce[CHACHA20_NONCE_SIZE],
                                 uint32_t counter) {
	// Constants "expand 32-byte k"
	ctx->state[0]  = 0x61707865U;
	ctx->state[1]  = 0x3320646eU;
	ctx->state[2]  = 0x79622d32U;
	ctx->state[3]  = 0x6b206574U;

	// Key (8 words)
	for (int i = 0; i < 8; i++)
		ctx->state[4 + i] = __jacl_load32_le(key + 4 * i);

	// Counter (1 word)
	ctx->state[12] = counter;

	// Nonce (3 words)
	ctx->state[13] = __jacl_load32_le(nonce + 0);
	ctx->state[14] = __jacl_load32_le(nonce + 4);
	ctx->state[15] = __jacl_load32_le(nonce + 8);

	ctx->counter = counter;
}

/**
 * chacha20_xor – Encrypt/decrypt data (XOR with keystream)
 *
 * Parameters:
 *   ctx – ChaCha20 context (must be initialized)
 *   in  – Input data (plaintext or ciphertext)
 *   out – Output data (ciphertext or plaintext)
 *   len – Length of data in bytes
 *
 * Note: Encryption and decryption are identical (XOR is symmetric).
 */
static inline void chacha20_xor(chacha20_ctx *ctx,
                                const uint8_t *in, uint8_t *out, size_t len) {
	uint32_t block[16];
	size_t pos = 0;

	while (pos < len) {
		// Generate keystream block
		__jacl_chacha20_block(block, ctx->state);
		ctx->state[12]++;  // Increment counter

		// XOR with input
		size_t take = (len - pos < 64) ? (len - pos) : 64;
		for (size_t i = 0; i < take; i++)
			out[pos + i] = in[pos + i] ^ ((uint8_t *)block)[i];

		pos += take;
	}

	__jacl_explicit_bzero(block, sizeof(block));
}

/**
 * chacha20 – One-shot encryption/decryption
 *
 * Convenience function for single-pass operation.
 *
 * Parameters:
 *   key     – 32-byte key
 *   nonce   – 12-byte nonce
 *   counter – Initial block counter
 *   in      – Input data
 *   out     – Output data
 *   len     – Data length in bytes
 */
static inline void chacha20(const uint8_t key[CHACHA20_KEY_SIZE],
                            const uint8_t nonce[CHACHA20_NONCE_SIZE],
                            uint32_t counter,
                            const uint8_t *in, uint8_t *out, size_t len) {
	chacha20_ctx ctx;
	chacha20_init(&ctx, key, nonce, counter);
	chacha20_xor(&ctx, in, out, len);
	__jacl_explicit_bzero(&ctx, sizeof(ctx));
}

// ========================================================================
// PUBLIC API: ChaCha20-Poly1305 AEAD
// ========================================================================

/**
 * chacha20poly1305_encrypt – Encrypt and authenticate
 *
 * Parameters:
 *   key   – 32-byte key
 *   nonce – 12-byte nonce (MUST be unique per key)
 *   aad   – Additional authenticated data (can be NULL)
 *   alen  – Length of AAD in bytes
 *   pt    – Plaintext input
 *   plen  – Plaintext length in bytes
 *   ct    – Ciphertext output (same length as plaintext)
 *   tag   – 16-byte authentication tag output
 *
 * Returns 0 on success.
 */
static inline int chacha20poly1305_encrypt(const uint8_t key[CHACHA20_KEY_SIZE],
                                           const uint8_t nonce[CHACHA20_NONCE_SIZE],
                                           const uint8_t *aad,  size_t alen,
                                           const uint8_t *pt,   size_t plen,
                                           uint8_t *ct,
                                           uint8_t tag[POLY1305_TAG_SIZE]) {
	chacha20_ctx cctx;
	poly1305_ctx pctx;
	uint8_t polykey[32] = {0}, pad[16] = {0}, lens[16];

	// Derive Poly1305 key from ChaCha20(counter=0)
	chacha20_init(&cctx, key, nonce, 0);
	chacha20_xor(&cctx, polykey, polykey, 32);

	// Encrypt plaintext with ChaCha20(counter=1)
	chacha20_init(&cctx, key, nonce, 1);
	chacha20_xor(&cctx, pt, ct, plen);

	// Authenticate: Poly1305(AAD || pad || CT || pad || lens)
	poly1305_init(&pctx, polykey);

	// AAD
	if (aad && alen) {
		poly1305_update(&pctx, aad, alen);
		if (alen & 0x0f)  // Pad to 16-byte boundary
			poly1305_update(&pctx, pad, 16 - (alen & 0x0f));
	}

	// Ciphertext
	poly1305_update(&pctx, ct, plen);
	if (plen & 0x0f)  // Pad to 16-byte boundary
		poly1305_update(&pctx, pad, 16 - (plen & 0x0f));

	// Lengths (little-endian)
	__jacl_store64_le(lens + 0, (uint64_t)alen);
	__jacl_store64_le(lens + 8, (uint64_t)plen);
	poly1305_update(&pctx, lens, sizeof(lens));

	poly1305_final(&pctx, tag);

	__jacl_explicit_bzero(polykey, sizeof(polykey));
	__jacl_explicit_bzero(&cctx, sizeof(cctx));
	return 0;
}

/**
 * chacha20poly1305_decrypt – Verify and decrypt
 *
 * Parameters:
 *   key   – 32-byte key
 *   nonce – 12-byte nonce
 *   aad   – Additional authenticated data (can be NULL)
 *   alen  – Length of AAD in bytes
 *   ct    – Ciphertext input
 *   clen  – Ciphertext length in bytes
 *   tag   – 16-byte authentication tag to verify
 *   pt    – Plaintext output (same length as ciphertext)
 *
 * Returns 0 on success, -1 if authentication fails.
 * On failure, plaintext is NOT written (to prevent oracle attacks).
 */
static inline int chacha20poly1305_decrypt(const uint8_t key[CHACHA20_KEY_SIZE],
                                           const uint8_t nonce[CHACHA20_NONCE_SIZE],
                                           const uint8_t *aad,  size_t alen,
                                           const uint8_t *ct,   size_t clen,
                                           const uint8_t tag[POLY1305_TAG_SIZE],
                                           uint8_t *pt) {
	chacha20_ctx cctx;
	poly1305_ctx pctx;
	uint8_t polykey[32] = {0}, pad[16] = {0}, lens[16], ctag[POLY1305_TAG_SIZE];

	// Derive Poly1305 key from ChaCha20(counter=0)
	chacha20_init(&cctx, key, nonce, 0);
	chacha20_xor(&cctx, polykey, polykey, 32);

	// Verify: Poly1305(AAD || pad || CT || pad || lens)
	poly1305_init(&pctx, polykey);

	// AAD
	if (aad && alen) {
		poly1305_update(&pctx, aad, alen);
		if (alen & 0x0f)
			poly1305_update(&pctx, pad, 16 - (alen & 0x0f));
	}

	// Ciphertext
	poly1305_update(&pctx, ct, clen);
	if (clen & 0x0f)
		poly1305_update(&pctx, pad, 16 - (clen & 0x0f));

	// Lengths
	__jacl_store64_le(lens + 0, (uint64_t)alen);
	__jacl_store64_le(lens + 8, (uint64_t)clen);
	poly1305_update(&pctx, lens, sizeof(lens));

	poly1305_final(&pctx, ctag);

	// Constant-time tag comparison
	if (__jacl_timingsafe_memcmp(tag, ctag, POLY1305_TAG_SIZE) != 0) {
		__jacl_explicit_bzero(polykey, sizeof(polykey));
		__jacl_explicit_bzero(&cctx, sizeof(cctx));
		__jacl_explicit_bzero(ctag, sizeof(ctag));
		return -1;  // Authentication failed
	}

	// Decrypt plaintext with ChaCha20(counter=1)
	chacha20_init(&cctx, key, nonce, 1);
	chacha20_xor(&cctx, ct, pt, clen);

	__jacl_explicit_bzero(polykey, sizeof(polykey));
	__jacl_explicit_bzero(&cctx, sizeof(cctx));
	__jacl_explicit_bzero(ctag, sizeof(ctag));
	return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* _CRYPTO_CHACHA20_H */
