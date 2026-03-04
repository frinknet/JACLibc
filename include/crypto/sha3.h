/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef CRYPTO_SHA3_H
#define CRYPTO_SHA3_H
#pragma once

/**
 * SHA-3 – Secure Hash Algorithm 3 (FIPS 202)
 *
 * Implements the four fixed-output SHA-3 hash functions based on the Keccak
 * sponge construction. For variable-length output (XOF), see shake.h.
 *
 * **Use cases:**
 *   - Modern cryptographic hashing (alternative to SHA-2).
 *   - Post-quantum signature schemes (Dilithium, SPHINCS+).
 *   - Randomness extraction and key derivation.
 *   - Applications requiring domain separation from SHA-2.
 *
 * **Variants supported:**
 *   - SHA3-224: 224-bit digest (28 bytes), rate=144 bytes
 *   - SHA3-256: 256-bit digest (32 bytes), rate=136 bytes (recommended)
 *   - SHA3-384: 384-bit digest (48 bytes), rate=104 bytes
 *   - SHA3-512: 512-bit digest (64 bytes), rate=72 bytes
 *
 * **Differences from SHA-2:**
 *   - Different internal structure (sponge vs Merkle-Damgård).
 *   - Inherently resistant to length-extension attacks.
 *   - Larger security margin against cryptanalysis.
 *   - Slower on most platforms (not hardware-accelerated like SHA-256).
 *
 * Namespace: sha3_224_*, sha3_256_*, sha3_384_*, sha3_512_*
 */

#include <crypto/keccak.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHA3_224_DIGEST_SIZE  28
#define SHA3_256_DIGEST_SIZE  32
#define SHA3_384_DIGEST_SIZE  48
#define SHA3_512_DIGEST_SIZE  64

// SHA-3 uses domain separation byte 0x06
#define __JACL_SHA3_DELIM  0x06

// ========================================================================
// INTERNAL: Variant forge macro
// ========================================================================

#define __jacl_sha3_make(NAME, DIGEST_BYTES, RATE_BYTES) \
	typedef keccak_ctx NAME##_ctx; \
	\
	static inline void NAME##_init(NAME##_ctx *ctx) { \
		keccak_init(ctx, RATE_BYTES); \
	} \
	\
	static inline void NAME##_update(NAME##_ctx *ctx, const uint8_t *data, size_t len) { \
		keccak_absorb(ctx, data, len); \
	} \
	\
	static inline void NAME##_final(NAME##_ctx *ctx, uint8_t out[DIGEST_BYTES]) { \
		keccak_pad(ctx, __JACL_SHA3_DELIM); \
		keccak_squeeze(ctx, out, DIGEST_BYTES); \
	} \
	\
	static inline void NAME(const uint8_t *data, size_t len, uint8_t out[DIGEST_BYTES]) { \
		NAME##_ctx ctx; \
		NAME##_init(&ctx); \
		NAME##_update(&ctx, data, len); \
		NAME##_final(&ctx, out); \
	}

// ========================================================================
// PUBLIC: SHA-3 variants (generated via macro)
// ========================================================================

// SHA3-224: 224-bit hash, rate=1152 bits (144 bytes)
__jacl_sha3_make(sha3_224, 28, 144)

// SHA3-256: 256-bit hash, rate=1088 bits (136 bytes) [recommended]
__jacl_sha3_make(sha3_256, 32, 136)

// SHA3-384: 384-bit hash, rate=832 bits (104 bytes)
__jacl_sha3_make(sha3_384, 48, 104)

// SHA3-512: 512-bit hash, rate=576 bits (72 bytes)
__jacl_sha3_make(sha3_512, 64, 72)

#ifdef __cplusplus
}
#endif
#endif /* CRYPTO_SHA3_H */

