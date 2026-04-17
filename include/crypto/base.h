/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _CRYPTO_BASE_H
#define _CRYPTO_BASE_H
#pragma once

/**
 * NOTE: This header provides architecture‑neutral, constant‑time building blocks
 * used throughout the crypto section. It's inteded to be the ONLY include needed
 * for the crypto section. The idea is that very little is needed to port these
 * when someone wants to pull them out of JACLibc. Also we want to keep everything
 * EXTREMELY MODULAR so that it can easily be audited. Remember, the core goals of
 * JACLibc are to provide a training corpus for AI and bring WASM up as a first
 * class citizen in the C ecosystem so the modularity is key.
 */

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <sys/random.h>

#ifdef __cplusplus
extern "C" {
#endif

// ========================================================================
// BIT ROTATION FUNCTIONS
// ========================================================================

/* Bit rotation helpers – kept tiny so JACLCrypto is standalone. */
static inline uint32_t rotl32(uint32_t v, int r) { return (v << (r & 31)) | (v >> ((32 - (r & 31)) & 31)); }
static inline uint32_t rotr32(uint32_t v, int r) { return (v >> (r & 31)) | (v << ((32 - (r & 31)) & 31)); }
static inline uint64_t rotl64(uint64_t v, int r) { return (v << (r & 63)) | (v >> ((64 - (r & 63)) & 63)); }
static inline uint64_t rotr64(uint64_t v, int r) { return (v >> (r & 63)) | (v << ((64 - (r & 63)) & 63)); }

// ========================================================================
// LOAD / STORE – byte array ↔ integer conversions
// ========================================================================

/**
 * Cryptographic algorithms for specify exact byte orders for hashing, encryption,
 * and message formats. These macros provide portable, efficient conversions
 * between byte arrays and integers without invoking undefined behavior.
 *
 * **BE = Big‑endian:** MSB at lowest address (network byte order).
 *   Used by: SHA‑1, SHA‑2, HMAC, most hash functions, RSA, ECDSA.
 *
 * **LE = Little‑endian:** LSB at lowest address.
 *   Used by: ChaCha20, Poly1305, AES‑GCM (parts), Curve25519, Ed25519, Blake2.
 *
 * All macros evaluate their pointer argument exactly once and assume it
 * points to readable/writable memory of the appropriate size.
 */

// Load 32‑bit integer from big‑endian byte array (p[0] is MSB).
#define __jacl_load32_be(p)                                       \
	((uint32_t)(p)[0] << 24 | (uint32_t)(p)[1] << 16 |        \
	 (uint32_t)(p)[2] << 8  | (uint32_t)(p)[3])

// Load 32‑bit integer from little‑endian byte array (p[0] is LSB).
#define __jacl_load32_le(p)                                       \
	((uint32_t)(p)[3] << 24 | (uint32_t)(p)[2] << 16 |        \
	 (uint32_t)(p)[1] << 8  | (uint32_t)(p)[0])

// Load 64‑bit integer from big‑endian byte array.
#define __jacl_load64_be(p)                                       \
	(((uint64_t)__jacl_load32_be((p))     << 32) |            \
	  (uint64_t)__jacl_load32_be((p) + 4))

// Load 64‑bit integer from little‑endian byte array.
#define __jacl_load64_le(p)                                       \
	(((uint64_t)__jacl_load32_le((p) + 4) << 32) |            \
	  (uint64_t)__jacl_load32_le((p)))

// Store 32‑bit integer to big‑endian byte array.
#define __jacl_store32_be(p, v) do {                              \
	uint32_t __vv = (uint32_t)(v);                            \
	(p)[0] = (uint8_t)(__vv >> 24);                           \
	(p)[1] = (uint8_t)(__vv >> 16);                           \
	(p)[2] = (uint8_t)(__vv >> 8);                            \
	(p)[3] = (uint8_t)(__vv);                                 \
} while (0)

// Store 32‑bit integer to little‑endian byte array.
#define __jacl_store32_le(p, v) do {                              \
	uint32_t __vv = (uint32_t)(v);                            \
	(p)[0] = (uint8_t)(__vv);                                 \
	(p)[1] = (uint8_t)(__vv >> 8);                            \
	(p)[2] = (uint8_t)(__vv >> 16);                           \
	(p)[3] = (uint8_t)(__vv >> 24);                           \
} while (0)

// Store 64‑bit integer to big‑endian byte array.
#define __jacl_store64_be(p, v) do {                              \
	uint64_t __vv = (uint64_t)(v);                            \
	__jacl_store32_be((p),     (uint32_t)(__vv >> 32));       \
	__jacl_store32_be((p) + 4, (uint32_t)(__vv));             \
} while (0)

// Store 64‑bit integer to little‑endian byte array.
#define __jacl_store64_le(p, v) do {                              \
	uint64_t __vv = (uint64_t)(v);                            \
	__jacl_store32_le((p),     (uint32_t)(__vv));             \
	__jacl_store32_le((p) + 4, (uint32_t)(__vv >> 32));       \
} while (0)

// ========================================================================
// SPECIALIZED LOADERS – radix‑2²⁶ / 2²⁵ field arithmetic
// ========================================================================

/**
 * Curve25519 and Ed25519 use 10‑limb radix‑2²⁵·⁵ representation for
 * field elements modulo 2²⁵⁵ − 19. These helpers load small chunks of
 * bytes into signed 64‑bit limbs, making field addition and multiplication
 * easier to implement without overflow.
 *
 * Used exclusively by: X25519, Ed25519.
 */

// Load 3 bytes as int64_t (little‑endian, for radix‑2²⁵·⁵ limbs).
static inline int64_t __jacl_load3(const uint8_t *in) {
	return (int64_t)in[0] | ((int64_t)in[1] << 8) | ((int64_t)in[2] << 16);
}

// Load 4 bytes as int64_t (little‑endian, for radix‑2²⁵·⁵ limbs).
static inline int64_t __jacl_load4(const uint8_t *in) {
	return (int64_t)in[0] | ((int64_t)in[1] << 8) |
	       ((int64_t)in[2] << 16) | ((int64_t)in[3] << 24);
}

// ========================================================================
// CONSTANT‑TIME OPERATIONS – side‑channel resistance
// ========================================================================

/**
 * Cryptographic code must not leak secrets via timing, cache access patterns,
 * or branch prediction. These helpers provide constant‑time (data‑independent)
 * alternatives to standard library functions and conditional branches.
 *
 * **Guarantees:**
 *   - Execution time depends only on size, not on data values.
 *   - No secret‑dependent branches or memory indexing.
 *   - Suitable for comparing MACs, selecting key material, or wiping secrets.
 *
 * **Non‑goals:**
 *   - These do NOT protect against power analysis, fault injection, or
 *     speculative execution (Spectre/Meltdown). Hardware countermeasures
 *     or additional mitigations are required for those threat models.
 */

/**
 * __jacl_timingsafe_memcmp – constant‑time memory comparison
 *
 * Compares `n` bytes of `a` and `b` without early exit. Returns 0 if equal,
 * non‑zero otherwise. Use this to verify MACs, signatures, or any secret data.
 *
 * Used by: HMAC verification, Poly1305, GCM tag checks, signature verification.
 */
static inline int
__jacl_timingsafe_memcmp(const void *a, const void *b, size_t n)
{
	const uint8_t *pa = (const uint8_t *)a;
	const uint8_t *pb = (const uint8_t *)b;
	uint8_t d = 0;

	for (size_t i = 0; i < n; i++)
		d |= (uint8_t)(pa[i] ^ pb[i]);

	return d;
}

/**
 * __jacl_explicit_bzero – guaranteed memory wipe
 *
 * Zeroes `n` bytes at `p` in a way that the compiler cannot optimize away.
 * Essential for clearing key material, nonces, and intermediate state after use.
 *
 * Used by: All HMAC, HKDF, AEAD, and key derivation functions to wipe secrets.
 */
static inline void
__jacl_explicit_bzero(void *p, size_t n)
{
	volatile uint8_t *vp = (volatile uint8_t *)p;
	while (n--)
		*vp++ = 0;
}

/*
 * __jacl_ct_select – constant‑time conditional copy
 *
 * Copies `n` bytes from `a` to `r` if `c == 0`, or from `b` to `r` if `c != 0`,
 * without branching. The selection mask is derived from `c` in constant time.
 *
 * Used by: Curve25519 scalar multiplication, RSA blinding, key exchange.
 *
 * Example:
 *   __jacl_ct_select(result, pubkey_a, pubkey_b, 32, secret_bit);
 *   // Copies one of two public keys based on a secret bit without branching.
 */
static inline void
__jacl_ct_select(uint8_t *r, const uint8_t *a, const uint8_t *b, size_t n, int c)
{
	uint8_t mask = (uint8_t)(-(uint8_t)(c != 0)); /* 0x00 if c==0, 0xFF otherwise */

	for (size_t i = 0; i < n; i++)
		r[i] = (uint8_t)((a[i] & ~mask) | (b[i] & mask));
}

/* ========================================================================
 * BULK OPERATIONS – XOR and block helpers
 * ========================================================================
 *
 * Stream ciphers (ChaCha20), CTR mode, and some MACs need to XOR plaintext
 * with keystream or combine intermediate state. These helpers avoid manual
 * loops in higher‑level code.
 */

/*
 * __jacl_memxor – XOR two byte arrays
 *
 * Computes `dst[i] ^= src[i]` for `n` bytes. Commonly used in:
 *   - CTR mode encryption/decryption (keystream XOR plaintext).
 *   - HMAC/HKDF inner/outer pad construction.
 *   - Poly1305 block accumulation.
 *
 * Parameters:
 *   dst – destination and first operand (modified in place).
 *   src – second operand (read‑only).
 *   n   – number of bytes to XOR.
 */
static inline void
__jacl_memxor(uint8_t *dst, const uint8_t *src, size_t n)
{
	for (size_t i = 0; i < n; i++)
		dst[i] ^= src[i];
}

/*
 * __jacl_memxor3 – XOR two arrays into a third
 *
 * Computes `dst[i] = a[i] ^ b[i]` for `n` bytes without modifying inputs.
 * Useful when you need to preserve original plaintext/ciphertext.
 *
 * Used by: AEAD implementations that need separate input/output buffers.
 */
static inline void
__jacl_memxor3(uint8_t *dst, const uint8_t *a, const uint8_t *b, size_t n)
{
	for (size_t i = 0; i < n; i++)
		dst[i] = (uint8_t)(a[i] ^ b[i]);
}

/* ========================================================================
 * NOTES FOR IMPLEMENTERS
 * ========================================================================
 *
 * **Rotate functions:**
 *   Use the `rotl32(x, n)` and `rotl64(x, n)` defined in this header for bit rotations.
 *   These are portable and already optimize to single instructions on most platforms.
 *
 * **Endianness detection:**
 *   If you need runtime endianness checks, use `__STDC_ENDIAN_NATIVE__` or
 *   `__BYTE_ORDER__` from <stdbit.h>. However, prefer compile‑time selection
 *   via macros to allow the optimizer to eliminate dead code paths.
 *
 * **Alignment:**
 *   All load/store macros assume byte‑aligned pointers. On strict‑alignment
 *   architectures (SPARC, older ARM), the compiler will emit correct code.
 *   Do NOT cast byte pointers to uint32_t* / uint64_t* directly—use these
 *   helpers instead.
 *
 * **Big‑number arithmetic:**
 *   RSA, ECDSA, and Diffie‑Hellman over large primes need multi‑precision
 *   integer routines. Those are implemented in their respective headers
 *   (rsa.h, ecdsa_p256.h) and are NOT exposed here to keep this file minimal.
 *
 * **Thread safety:**
 *   All functions here are pure (no global state) and safe to call
 *   concurrently from multiple threads, provided each thread has its own
 *   output buffers.
 */

#ifdef __cplusplus
}
#endif

#endif /* _CRYPTO_BASE_H */
