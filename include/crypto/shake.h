/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef CRYPTO_SHAKE_H
#define CRYPTO_SHAKE_H
#pragma once

/**
 * SHAKE – SHA-3 Extendable-Output Functions (FIPS 202)
 *
 * Implements SHAKE128 and SHAKE256, which are XOFs (extendable-output functions)
 * based on the Keccak sponge. Unlike SHA-3's fixed-output hashes, SHAKE can
 * produce arbitrary-length output by squeezing the sponge multiple times.
 *
 * **Use cases:**
 *   - Key derivation (alternative to HKDF).
 *   - Stream cipher construction (with proper domain separation).
 *   - Randomness expansion for cryptographic protocols.
 *   - Mask generation functions (e.g., RSA-OAEP, RSA-PSS).
 *   - Post-quantum schemes (Kyber, Dilithium use SHAKE for internal operations).
 *
 * **Variants:**
 *   - SHAKE128: 128-bit security, rate=1344 bits (168 bytes), arbitrary output
 *   - SHAKE256: 256-bit security, rate=1088 bits (136 bytes), arbitrary output
 *
 * **API pattern:**
 *   1. shake128_init() or shake256_init()
 *   2. shake128_update() or shake256_update() — absorb input (can call multiple times)
 *   3. shake128_final() or shake256_final() — finalize and begin squeezing
 *   4. shake128_squeeze() or shake256_squeeze() — squeeze arbitrary output length
 *
 * Namespace: shake128_*, shake256_*
 */

#include <crypto/keccak.h>

#ifdef __cplusplus
extern "C" {
#endif

// SHAKE uses domain separation byte 0x1F
#define __JACL_SHAKE_DELIM  0x1F

// ========================================================================
// INTERNAL: Variant forge macro
// ========================================================================

#define __jacl_shake_make(NAME, RATE_BYTES) \
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
	static inline void NAME##_final(NAME##_ctx *ctx) { \
		keccak_pad(ctx, __JACL_SHAKE_DELIM); \
	} \
	\
	static inline void NAME##_squeeze(NAME##_ctx *ctx, uint8_t *out, size_t outlen) { \
		keccak_squeeze(ctx, out, outlen); \
	} \
	\
	static inline void NAME(const uint8_t *data, size_t dlen, uint8_t *out, size_t outlen) { \
		NAME##_ctx ctx; \
		NAME##_init(&ctx); \
		NAME##_update(&ctx, data, dlen); \
		NAME##_final(&ctx); \
		NAME##_squeeze(&ctx, out, outlen); \
	}

// ========================================================================
// PUBLIC: SHAKE variants (generated via macro)
// ========================================================================

// SHAKE128: 128-bit security, rate=1344 bits (168 bytes), arbitrary output
__jacl_shake_make(shake128, 168)

// SHAKE256: 256-bit security, rate=1088 bits (136 bytes), arbitrary output
__jacl_shake_make(shake256, 136)

#ifdef __cplusplus
}
#endif
#endif /* CRYPTO_SHAKE_H */

