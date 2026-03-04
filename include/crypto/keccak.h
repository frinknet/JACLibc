/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef CRYPTO_KECCAK_H
#define CRYPTO_KECCAK_H
#pragma once

/**
 * Keccak – Sponge Construction Primitives (FIPS 202 / SHA-3 Family)
 *
 * Provides the raw Keccak-f[1600] permutation and sponge construction primitives
 * used by SHA-3, SHAKE, cSHAKE, TupleHash, ParallelHash, and other Keccak-based
 * algorithms.
 *
 * **Not for direct use in most applications.** Use sha3.h or shake.h instead.
 *
 * **Use cases for raw Keccak:**
 *   - Implementing custom sponge-based constructions.
 *   - Post-quantum cryptography (Kyber, Dilithium internals).
 *   - KangarooTwelve (K12) and other Keccak variants.
 *   - cSHAKE, TupleHash, ParallelHash (NIST SP 800-185).
 *
 * **Keccak sponge structure:**
 *   - State: 1600 bits (25 × 64-bit lanes, arranged as 5×5 array).
 *   - Rate (r): Number of bits absorbed/squeezed per permutation.
 *   - Capacity (c): Security parameter (1600 - rate).
 *   - For n-bit security: capacity ≥ 2n bits.
 *
 * **Domain separation:**
 *   - SHA-3: 0x06
 *   - SHAKE: 0x1F
 *   - cSHAKE: 0x04
 *   - RawSHAKE: 0x07
 *
 * Namespace: keccak_*
 */

#include <crypto/base.h>

#ifdef __cplusplus
extern "C" {
#endif

// ========================================================================
// CONSTANTS: Keccak-f[1600] round function
// ========================================================================

// Round constants for ι (Iota) step (24 rounds)
static const uint64_t __jacl_keccak_rc[24] = {
	0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL,
	0x8000000080008000ULL, 0x000000000000808bULL, 0x0000000080000001ULL,
	0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008aULL,
	0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
	0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL,
	0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
	0x000000000000800aULL, 0x800000008000000aULL, 0x8000000080008081ULL,
	0x8000000080008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};

// Rotation offsets for ρ (Rho) step
static const int __jacl_keccak_rotc[24] = {
	1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 2, 14,
	27, 41, 56, 8, 25, 43, 62, 18, 39, 61, 20, 44
};

// Lane permutation indices for π (Pi) step
static const int __jacl_keccak_piln[24] = {
	10, 7, 11, 17, 18, 3, 5, 16, 8, 21, 24, 4,
	15, 23, 19, 13, 12, 2, 20, 14, 22, 9, 6, 1
};

// ========================================================================
// TYPES: Keccak sponge state
// ========================================================================

/**
 * keccak_ctx – Keccak sponge state
 *
 * Fields:
 *   st       – 1600-bit state (25 lanes of 64 bits each)
 *   buf      – Input buffer (up to 200 bytes = 1600 bits)
 *   buf_len  – Current bytes in buffer
 *   rate     – Rate in bytes (r/8)
 *   absorbing – 1 if absorbing, 0 if squeezing
 */
typedef struct {
	uint64_t st[25];     // 5×5 array of 64-bit lanes
	uint8_t  buf[200];   // Input buffer (1600 bits max)
	size_t   buf_len;    // Bytes in buffer
	size_t   rate;       // Rate in bytes (r/8)
	int      absorbing;  // 1=absorbing, 0=squeezing
} keccak_ctx;

// ========================================================================
// CORE: Keccak-f[1600] permutation
// ========================================================================

/**
 * keccak_f1600 – Keccak-f[1600] permutation (24 rounds)
 *
 * Applies the full Keccak-f[1600] permutation to the state.
 * This is the core cryptographic primitive for all Keccak-based algorithms.
 *
 * Parameters:
 *   st – Pointer to 25-element uint64_t array (5×5 state)
 */
static inline void keccak_f1600(uint64_t st[25]) {
	uint64_t bc[5], t;
	
	for (int round = 0; round < 24; round++) {
		// θ (Theta) step – column parity mixing
		for (int i = 0; i < 5; i++)
			bc[i] = st[i] ^ st[i + 5] ^ st[i + 10] ^ st[i + 15] ^ st[i + 20];
		
		for (int i = 0; i < 5; i++) {
			t = bc[(i + 4) % 5] ^ rotl64(bc[(i + 1) % 5], 1);
			for (int j = 0; j < 25; j += 5)
				st[j + i] ^= t;
		}
		
		// ρ (Rho) and π (Pi) steps – rotate and permute lanes
		t = st[1];
		for (int i = 0; i < 24; i++) {
			int j = __jacl_keccak_piln[i];
			bc[0] = st[j];
			st[j] = rotl64(t, __jacl_keccak_rotc[i]);
			t = bc[0];
		}
		
		// χ (Chi) step – nonlinear mixing
		for (int j = 0; j < 25; j += 5) {
			for (int i = 0; i < 5; i++)
				bc[i] = st[j + i];
			for (int i = 0; i < 5; i++)
				st[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
		}
		
		// ι (Iota) step – break symmetry
		st[0] ^= __jacl_keccak_rc[round];
	}
}

// ========================================================================
// SPONGE: Absorb and squeeze operations
// ========================================================================

/**
 * keccak_init – Initialize Keccak sponge state
 *
 * Parameters:
 *   ctx  – Keccak context to initialize
 *   rate – Rate in bytes (e.g., 136 for SHA3-256, 168 for SHAKE128)
 */
static inline void keccak_init(keccak_ctx *ctx, size_t rate) {
	memset(ctx, 0, sizeof(keccak_ctx));
	ctx->rate = rate;
	ctx->absorbing = 1;
}

/**
 * keccak_absorb – Absorb data into sponge (can be called multiple times)
 *
 * Parameters:
 *   ctx  – Keccak context
 *   data – Data to absorb
 *   len  – Length of data in bytes
 */
static inline void keccak_absorb(keccak_ctx *ctx, const uint8_t *data, size_t len) {
	for (size_t i = 0; i < len; i++) {
		ctx->buf[ctx->buf_len++] = data[i];
		
		if (ctx->buf_len == ctx->rate) {
			// XOR block into state (little-endian)
			for (size_t j = 0; j < ctx->rate / 8; j++)
				ctx->st[j] ^= __jacl_load64_le(ctx->buf + j * 8);
			
			keccak_f1600(ctx->st);
			ctx->buf_len = 0;
		}
	}
}

/**
 * keccak_pad – Finalize absorption with padding and domain separation
 *
 * Parameters:
 *   ctx   – Keccak context
 *   delim – Domain separation byte (0x06 for SHA-3, 0x1F for SHAKE, etc.)
 */
static inline void keccak_pad(keccak_ctx *ctx, uint8_t delim) {
	// Append delimiter
	ctx->buf[ctx->buf_len++] = delim;
	
	// Pad with zeros
	while (ctx->buf_len < ctx->rate)
		ctx->buf[ctx->buf_len++] = 0;
	
	// Set final bit (pad10*1 rule)
	ctx->buf[ctx->rate - 1] |= 0x80;
	
	// XOR final block into state
	for (size_t i = 0; i < ctx->rate / 8; i++)
		ctx->st[i] ^= __jacl_load64_le(ctx->buf + i * 8);
	
	keccak_f1600(ctx->st);
	
	// Switch to squeezing mode
	ctx->absorbing = 0;
	ctx->buf_len = 0;
}

/**
 * keccak_squeeze – Squeeze output from sponge (can be called multiple times)
 *
 * Parameters:
 *   ctx    – Keccak context (must have called keccak_pad first)
 *   out    – Output buffer
 *   outlen – Number of bytes to squeeze
 */
static inline void keccak_squeeze(keccak_ctx *ctx, uint8_t *out, size_t outlen) {
	for (size_t i = 0; i < outlen; i++) {
		// If buffer empty, permute and refill
		if (ctx->buf_len == 0) {
			if (i > 0)  // Don't permute on first squeeze (already done in pad)
				keccak_f1600(ctx->st);
			
			// Extract one rate-sized block
			for (size_t j = 0; j < ctx->rate / 8; j++)
				__jacl_store64_le(ctx->buf + j * 8, ctx->st[j]);
			
			ctx->buf_len = ctx->rate;
		}
		
		// Output one byte
		out[i] = ctx->buf[ctx->rate - ctx->buf_len];
		ctx->buf_len--;
	}
}

#ifdef __cplusplus
}
#endif
#endif /* CRYPTO_KECCAK_H */

