/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef CRYPTO_SHA2_H
#define CRYPTO_SHA2_H
#pragma once

/**
 * SHA-2 – Secure Hash Algorithm 2 (FIPS 180-4)
 *
 * Implements all six SHA-2 family variants using two internal cores.
 *
 * **Use cases:**
 *   - General-purpose cryptographic hashing (file integrity, content addressing).
 *   - HMAC for message authentication.
 *   - HKDF for key derivation (preferred over PBKDF2 for non-password keys).
 *   - Digital signatures (RSA-PSS, ECDSA).
 *
 * **Variants supported:**
 *   - SHA-224:     224-bit digest (28 bytes), uses 32-bit core
 *   - SHA-256:     256-bit digest (32 bytes), uses 32-bit core (recommended)
 *   - SHA-384:     384-bit digest (48 bytes), uses 64-bit core
 *   - SHA-512:     512-bit digest (64 bytes), uses 64-bit core
 *   - SHA-512/224: 224-bit digest (28 bytes), uses 64-bit core
 *   - SHA-512/256: 256-bit digest (32 bytes), uses 64-bit core
 *
 * Namespace: sha224_*, sha256_*, sha384_*, sha512_*, sha512_224_*, sha512_256_*
 */

#include <crypto/base.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHA224_DIGEST_SIZE      28
#define SHA256_DIGEST_SIZE      32
#define SHA384_DIGEST_SIZE      48
#define SHA512_DIGEST_SIZE      64
#define SHA512_224_DIGEST_SIZE  28
#define SHA512_256_DIGEST_SIZE  32

#define SHA256_BLOCK_SIZE       64
#define SHA512_BLOCK_SIZE       128

// ========================================================================
// INTERNAL: Round constants (FIPS 180-4)
// ========================================================================

// SHA-256 round constants (first 32 bits of fractional parts of cube roots of first 64 primes)
static const uint32_t __jacl_K256[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

// SHA-512 round constants (first 64 bits of fractional parts of cube roots of first 80 primes)
static const uint64_t __jacl_K512[80] = {
	0x428a2f98d728ae22ULL,0x7137449123ef65cdULL,0xb5c0fbcfec4d3b2fULL,0xe9b5dba58189dbbcULL,
	0x3956c25bf348b538ULL,0x59f111f1b605d019ULL,0x923f82a4af194f9bULL,0xab1c5ed5da6d8118ULL,
	0xd807aa98a3030242ULL,0x12835b0145706fbeULL,0x243185be4ee4b28cULL,0x550c7dc3d5ffb4e2ULL,
	0x72be5d74f27b896fULL,0x80deb1fe3b1696b1ULL,0x9bdc06a725c71235ULL,0xc19bf174cf692694ULL,
	0xe49b69c19ef14ad2ULL,0xefbe4786384f25e3ULL,0x0fc19dc68b8cd5b5ULL,0x240ca1cc77ac9c65ULL,
	0x2de92c6f592b0275ULL,0x4a7484aa6ea6e483ULL,0x5cb0a9dcbd41fbd4ULL,0x76f988da831153b5ULL,
	0x983e5152ee66dfabULL,0xa831c66d2db43210ULL,0xb00327c898fb213fULL,0xbf597fc7beef0ee4ULL,
	0xc6e00bf33da88fc2ULL,0xd5a79147930aa725ULL,0x06ca6351e003826fULL,0x142929670a0e6e70ULL,
	0x27b70a8546d22ffcULL,0x2e1b21385c26c926ULL,0x4d2c6dfc5ac42aedULL,0x53380d139d95b3dfULL,
	0x650a73548baf63deULL,0x766a0abb3c77b2a8ULL,0x81c2c92e47edaee6ULL,0x92722c851482353bULL,
	0xa2bfe8a14cf10364ULL,0xa81a664bbc423001ULL,0xc24b8b70d0f89791ULL,0xc76c51a30654be30ULL,
	0xd192e819d6ef5218ULL,0xd69906245565a910ULL,0xf40e35855771202aULL,0x106aa07032bbd1b8ULL,
	0x19a4c116b8d2d0c8ULL,0x1e376c085141ab53ULL,0x2748774cdf8eeb99ULL,0x34b0bcb5e19b48a8ULL,
	0x391c0cb3c5c95a63ULL,0x4ed8aa4ae3418acbULL,0x5b9cca4f7763e373ULL,0x682e6ff3d6b2b8a3ULL,
	0x748f82ee5defb2fcULL,0x78a5636f43172f60ULL,0x84c87814a1f0ab72ULL,0x8cc702081a6439ecULL,
	0x90befffa23631e28ULL,0xa4506cebde82bde9ULL,0xbef9a3f7b2c67915ULL,0xc67178f2e372532bULL,
	0xca273eceea26619cULL,0xd186b8c721c0c207ULL,0xeada7dd6cde0eb1eULL,0xf57d4f7fee6ed178ULL,
	0x06f067aa72176fbaULL,0x0a637dc5a2c898a6ULL,0x113f9804bef90daeULL,0x1b710b35131c471bULL,
	0x28db77f523047d84ULL,0x32caab7b40c72493ULL,0x3c9ebe0a15c9bebcULL,0x431d67c49c100d4cULL,
	0x4cc5d4becb3e42b6ULL,0x597f299cfc657e2aULL,0x5fcb6fab3ad6faecULL,0x6c44198c4a475817ULL
};

// ========================================================================
// INTERNAL: 32-bit family core (SHA-224, SHA-256)
// ========================================================================

typedef struct {
	uint32_t state[8];
	uint64_t bitlen;
	uint8_t  buf[64];
	size_t   len;
} __jacl_sha2_ctx32;

static inline void __jacl_sha2_transform32(__jacl_sha2_ctx32 *ctx) {
	uint32_t w[64], s[8];

	// Load message schedule
	for (int i = 0; i < 16; i++)
		w[i] = __jacl_load32_be(ctx->buf + i * 4);

	// Extend message schedule
	for (int i = 16; i < 64; i++) {
		// σ₀ = ROTR⁷ ⊕ ROTR¹⁸ ⊕ SHR³  (implemented as ROTL²⁵ ⊕ ROTL¹⁴ ⊕ SHR³)
		uint32_t s0 = rotl32(w[i-15], 25) ^ rotl32(w[i-15], 14) ^ (w[i-15] >> 3);
		// σ₁ = ROTR¹⁷ ⊕ ROTR¹⁹ ⊕ SHR¹⁰ (implemented as ROTL¹⁵ ⊕ ROTL¹³ ⊕ SHR¹⁰)
		uint32_t s1 = rotl32(w[i-2], 15) ^ rotl32(w[i-2], 13) ^ (w[i-2] >> 10);
		w[i] = w[i-16] + s0 + w[i-7] + s1;
	}

	// Initialize working variables
	memcpy(s, ctx->state, 32);

	// Main compression loop
	for (int i = 0; i < 64; i++) {
		// Σ₁ = ROTR⁶ ⊕ ROTR¹¹ ⊕ ROTR²⁵ (implemented as ROTL²⁶ ⊕ ROTL²¹ ⊕ ROTL⁷)
		uint32_t S1 = rotl32(s[4], 26) ^ rotl32(s[4], 21) ^ rotl32(s[4], 7);
		uint32_t ch = (s[4] & s[5]) ^ (~s[4] & s[6]);
		uint32_t temp1 = s[7] + S1 + ch + __jacl_K256[i] + w[i];
		// Σ₀ = ROTR² ⊕ ROTR¹³ ⊕ ROTR²² (implemented as ROTL³⁰ ⊕ ROTL¹⁹ ⊕ ROTL¹⁰)
		uint32_t S0 = rotl32(s[0], 30) ^ rotl32(s[0], 19) ^ rotl32(s[0], 10);
		uint32_t maj = (s[0] & s[1]) ^ (s[0] & s[2]) ^ (s[1] & s[2]);
		uint32_t temp2 = S0 + maj;

		s[7] = s[6]; s[6] = s[5]; s[5] = s[4]; s[4] = s[3] + temp1;
		s[3] = s[2]; s[2] = s[1]; s[1] = s[0]; s[0] = temp1 + temp2;
	}

	// Add compressed chunk to current hash value
	for (int i = 0; i < 8; i++)
		ctx->state[i] += s[i];
}

static inline void __jacl_sha2_init32(__jacl_sha2_ctx32 *ctx, const uint32_t iv[8]) {
	memcpy(ctx->state, iv, 32);
	ctx->bitlen = 0;
	ctx->len = 0;
}

static inline void __jacl_sha2_update32(__jacl_sha2_ctx32 *ctx, const uint8_t *data, size_t len) {
	for (size_t i = 0; i < len; i++) {
		ctx->buf[ctx->len++] = data[i];
		if (ctx->len == 64) {
			__jacl_sha2_transform32(ctx);
			ctx->bitlen += 512;
			ctx->len = 0;
		}
	}
}

static inline void __jacl_sha2_final32(__jacl_sha2_ctx32 *ctx, uint8_t out[32]) {
	size_t i = ctx->len;

	// Append padding bit '1' followed by zeros
	ctx->buf[i++] = 0x80;
	if (i > 56) {
		while (i < 64) ctx->buf[i++] = 0;
		__jacl_sha2_transform32(ctx);
		i = 0;
	}
	while (i < 56) ctx->buf[i++] = 0;

	// Append length in bits as 64-bit big-endian integer
	ctx->bitlen += ctx->len * 8;
	__jacl_store64_be(ctx->buf + 56, ctx->bitlen);
	__jacl_sha2_transform32(ctx);

	// Produce final hash value (big-endian)
	for (i = 0; i < 8; i++)
		__jacl_store32_be(out + i * 4, ctx->state[i]);
}

// ========================================================================
// INTERNAL: 64-bit family core (SHA-384, SHA-512, SHA-512/224, SHA-512/256)
// ========================================================================

typedef struct {
	uint64_t state[8];
	uint64_t bitlen[2];  // 128-bit counter for messages up to 2^128-1 bits
	uint8_t  buf[128];
	size_t   len;
} __jacl_sha2_ctx64;

static inline void __jacl_sha2_transform64(__jacl_sha2_ctx64 *ctx) {
	uint64_t w[80], s[8];

	// Load message schedule
	for (int i = 0; i < 16; i++)
		w[i] = __jacl_load64_be(ctx->buf + i * 8);

	// Extend message schedule
	for (int i = 16; i < 80; i++) {
		// σ₀ = ROTR¹ ⊕ ROTR⁸ ⊕ SHR⁷  (implemented as ROTL⁶³ ⊕ ROTL⁵⁶ ⊕ SHR⁷)
		uint64_t s0 = rotl64(w[i-15], 63) ^ rotl64(w[i-15], 56) ^ (w[i-15] >> 7);
		// σ₁ = ROTR¹⁹ ⊕ ROTR⁶¹ ⊕ SHR⁶ (implemented as ROTL⁴⁵ ⊕ ROTL³ ⊕ SHR⁶)
		uint64_t s1 = rotl64(w[i-2], 45) ^ rotl64(w[i-2], 3) ^ (w[i-2] >> 6);
		w[i] = w[i-16] + s0 + w[i-7] + s1;
	}

	// Initialize working variables
	memcpy(s, ctx->state, 64);

	// Main compression loop
	for (int i = 0; i < 80; i++) {
		// Σ₁ = ROTR¹⁴ ⊕ ROTR¹⁸ ⊕ ROTR⁴¹ (implemented as ROTL⁵⁰ ⊕ ROTL⁴⁶ ⊕ ROTL²³)
		uint64_t S1 = rotl64(s[4], 50) ^ rotl64(s[4], 46) ^ rotl64(s[4], 23);
		uint64_t ch = (s[4] & s[5]) ^ (~s[4] & s[6]);
		uint64_t temp1 = s[7] + S1 + ch + __jacl_K512[i] + w[i];
		// Σ₀ = ROTR²⁸ ⊕ ROTR³⁴ ⊕ ROTR³⁹ (implemented as ROTL³⁶ ⊕ ROTL³⁰ ⊕ ROTL²⁵)
		uint64_t S0 = rotl64(s[0], 36) ^ rotl64(s[0], 30) ^ rotl64(s[0], 25);
		uint64_t maj = (s[0] & s[1]) ^ (s[0] & s[2]) ^ (s[1] & s[2]);
		uint64_t temp2 = S0 + maj;

		s[7] = s[6]; s[6] = s[5]; s[5] = s[4]; s[4] = s[3] + temp1;
		s[3] = s[2]; s[2] = s[1]; s[1] = s[0]; s[0] = temp1 + temp2;
	}

	// Add compressed chunk to current hash value
	for (int i = 0; i < 8; i++)
		ctx->state[i] += s[i];
}

static inline void __jacl_sha2_init64(__jacl_sha2_ctx64 *ctx, const uint64_t iv[8]) {
	memcpy(ctx->state, iv, 64);
	ctx->bitlen[0] = ctx->bitlen[1] = 0;
	ctx->len = 0;
}

static inline void __jacl_sha2_update64(__jacl_sha2_ctx64 *ctx, const uint8_t *data, size_t len) {
	for (size_t i = 0; i < len; i++) {
		ctx->buf[ctx->len++] = data[i];
		if (ctx->len == 128) {
			__jacl_sha2_transform64(ctx);
			ctx->bitlen[0] += 1024;
			if (ctx->bitlen[0] < 1024) ctx->bitlen[1]++;  // Handle overflow
			ctx->len = 0;
		}
	}
}

static inline void __jacl_sha2_final64(__jacl_sha2_ctx64 *ctx, uint8_t out[64]) {
	size_t i = ctx->len;

	// Append padding bit '1' followed by zeros
	ctx->buf[i++] = 0x80;
	if (i > 112) {
		while (i < 128) ctx->buf[i++] = 0;
		__jacl_sha2_transform64(ctx);
		i = 0;
	}
	while (i < 112) ctx->buf[i++] = 0;

	// Append length in bits as 128-bit big-endian integer
	ctx->bitlen[0] += ctx->len * 8;
	if (ctx->bitlen[0] < (ctx->len * 8)) ctx->bitlen[1]++;
	__jacl_store64_be(ctx->buf + 112, ctx->bitlen[1]);
	__jacl_store64_be(ctx->buf + 120, ctx->bitlen[0]);
	__jacl_sha2_transform64(ctx);

	// Produce final hash value (big-endian)
	for (i = 0; i < 8; i++)
		__jacl_store64_be(out + i * 8, ctx->state[i]);
}

// ========================================================================
// INTERNAL: Variant forge macros
// ========================================================================

#define __jacl_sha2_make32(NAME, DIGEST_BYTES, ...) \
	typedef __jacl_sha2_ctx32 NAME##_ctx; \
	static inline void NAME##_init(NAME##_ctx *ctx) { \
		static const uint32_t iv[8] = {__VA_ARGS__}; \
		__jacl_sha2_init32(ctx, iv); \
	} \
	static inline void NAME##_update(NAME##_ctx *ctx, const uint8_t *data, size_t len) { \
		__jacl_sha2_update32(ctx, data, len); \
	} \
	static inline void NAME##_final(NAME##_ctx *ctx, uint8_t out[DIGEST_BYTES]) { \
		uint8_t full[32]; \
		__jacl_sha2_final32(ctx, full); \
		memcpy(out, full, DIGEST_BYTES); \
		__jacl_explicit_bzero(full, sizeof(full)); \
	} \
	static inline void NAME(const uint8_t *data, size_t len, uint8_t out[DIGEST_BYTES]) { \
		NAME##_ctx ctx; \
		NAME##_init(&ctx); \
		NAME##_update(&ctx, data, len); \
		NAME##_final(&ctx, out); \
	}

#define __jacl_sha2_make64(NAME, DIGEST_BYTES, ...) \
	typedef __jacl_sha2_ctx64 NAME##_ctx; \
	static inline void NAME##_init(NAME##_ctx *ctx) { \
		static const uint64_t iv[8] = {__VA_ARGS__}; \
		__jacl_sha2_init64(ctx, iv); \
	} \
	static inline void NAME##_update(NAME##_ctx *ctx, const uint8_t *data, size_t len) { \
		__jacl_sha2_update64(ctx, data, len); \
	} \
	static inline void NAME##_final(NAME##_ctx *ctx, uint8_t out[DIGEST_BYTES]) { \
		uint8_t full[64]; \
		__jacl_sha2_final64(ctx, full); \
		memcpy(out, full, DIGEST_BYTES); \
		__jacl_explicit_bzero(full, sizeof(full)); \
	} \
	static inline void NAME(const uint8_t *data, size_t len, uint8_t out[DIGEST_BYTES]) { \
		NAME##_ctx ctx; \
		NAME##_init(&ctx); \
		NAME##_update(&ctx, data, len); \
		NAME##_final(&ctx, out); \
	}

// ========================================================================
// PUBLIC: SHA-2 variants (generated via macros)
// ========================================================================

// SHA-224: truncated SHA-256 with different IV (RFC 3874)
__jacl_sha2_make32(sha224, 28,
	0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
	0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4
)

// SHA-256: 256-bit hash (FIPS 180-4)
__jacl_sha2_make32(sha256, 32,
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
)

// SHA-384: truncated SHA-512 with different IV (FIPS 180-4)
__jacl_sha2_make64(sha384, 48,
	0xcbbb9d5dc1059ed8ULL, 0x629a292a367cd507ULL,
	0x9159015a3070dd17ULL, 0x152fecd8f70e5939ULL,
	0x67332667ffc00b31ULL, 0x8eb44a8768581511ULL,
	0xdb0c2e0d64f98fa7ULL, 0x47b5481dbefa4fa4ULL
)

// SHA-512: 512-bit hash (FIPS 180-4)
__jacl_sha2_make64(sha512, 64,
	0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL,
	0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
	0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL,
	0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL
)

// SHA-512/224: 224-bit hash using 64-bit core (FIPS 180-4)
__jacl_sha2_make64(sha512_224, 28,
	0x8c3d37c819544da2ULL, 0x73e1996689dcd4d6ULL,
	0x1dfab7ae32ff9c82ULL, 0x679dd514582f9fcfULL,
	0x0f6d2b697bd44da8ULL, 0x77e36f7304c48942ULL,
	0x3f9d85a86a1d36c8ULL, 0x1112e6ad91d692a1ULL
)

// SHA-512/256: 256-bit hash using 64-bit core (FIPS 180-4)
__jacl_sha2_make64(sha512_256, 32,
	0x22312194fc2bf72cULL, 0x9f555fa3c84c64c2ULL,
	0x2393b86b6f53b151ULL, 0x963877195940eabdULL,
	0x96283ee2a88effe3ULL, 0xbe5e1e2553863992ULL,
	0x2b0199fc2c85b8aaULL, 0x0eb72ddc81c52ca2ULL
)

// ========================================================================
// HMAC-SHA256 / HMAC-SHA512
// ========================================================================

/**
 * sha256_hmac – HMAC-SHA256 (RFC 2104)
 *
 * Keyed-hash message authentication code using SHA-256.
 * Use for message authentication, API signing, or integrity checks where both
 * sender and receiver share a secret key.
 */
static inline void sha256_hmac(const uint8_t *key, size_t klen,
                               const uint8_t *msg, size_t mlen,
                               uint8_t out[SHA256_DIGEST_SIZE])
{
	uint8_t k[SHA256_BLOCK_SIZE], ipad[SHA256_BLOCK_SIZE], opad[SHA256_BLOCK_SIZE];
	memset(k, 0, SHA256_BLOCK_SIZE);

	// Hash key if longer than block size
	if (klen > SHA256_BLOCK_SIZE) {
		sha256(key, klen, k);
	} else {
		memcpy(k, key, klen);
	}

	// Prepare inner and outer padded keys
	for (int i = 0; i < SHA256_BLOCK_SIZE; i++) {
		ipad[i] = (uint8_t)(k[i] ^ 0x36);
		opad[i] = (uint8_t)(k[i] ^ 0x5c);
	}

	// Inner hash: H((K ⊕ ipad) || message)
	sha256_ctx ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, ipad, SHA256_BLOCK_SIZE);
	sha256_update(&ctx, msg, mlen);
	sha256_final(&ctx, out);

	// Outer hash: H((K ⊕ opad) || inner_hash)
	sha256_init(&ctx);
	sha256_update(&ctx, opad, SHA256_BLOCK_SIZE);
	sha256_update(&ctx, out, SHA256_DIGEST_SIZE);
	sha256_final(&ctx, out);

	// Wipe sensitive material
	__jacl_explicit_bzero(k, SHA256_BLOCK_SIZE);
	__jacl_explicit_bzero(ipad, SHA256_BLOCK_SIZE);
	__jacl_explicit_bzero(opad, SHA256_BLOCK_SIZE);
}

/**
 * sha512_hmac – HMAC-SHA512 (RFC 2104)
 *
 * Keyed-hash message authentication code using SHA-512.
 * Identical construction to HMAC-SHA256, but with longer output and block size.
 */
static inline void sha512_hmac(const uint8_t *key, size_t klen,
                               const uint8_t *msg, size_t mlen,
                               uint8_t out[SHA512_DIGEST_SIZE])
{
	uint8_t k[SHA512_BLOCK_SIZE], ipad[SHA512_BLOCK_SIZE], opad[SHA512_BLOCK_SIZE];
	memset(k, 0, SHA512_BLOCK_SIZE);

	// Hash key if longer than block size
	if (klen > SHA512_BLOCK_SIZE) {
		sha512(key, klen, k);
	} else {
		memcpy(k, key, klen);
	}

	// Prepare inner and outer padded keys
	for (int i = 0; i < SHA512_BLOCK_SIZE; i++) {
		ipad[i] = (uint8_t)(k[i] ^ 0x36);
		opad[i] = (uint8_t)(k[i] ^ 0x5c);
	}

	// Inner hash: H((K ⊕ ipad) || message)
	sha512_ctx ctx;
	sha512_init(&ctx);
	sha512_update(&ctx, ipad, SHA512_BLOCK_SIZE);
	sha512_update(&ctx, msg, mlen);
	sha512_final(&ctx, out);

	// Outer hash: H((K ⊕ opad) || inner_hash)
	sha512_init(&ctx);
	sha512_update(&ctx, opad, SHA512_BLOCK_SIZE);
	sha512_update(&ctx, out, SHA512_DIGEST_SIZE);
	sha512_final(&ctx, out);

	// Wipe sensitive material
	__jacl_explicit_bzero(k, SHA512_BLOCK_SIZE);
	__jacl_explicit_bzero(ipad, SHA512_BLOCK_SIZE);
	__jacl_explicit_bzero(opad, SHA512_BLOCK_SIZE);
}

// ========================================================================
// HKDF-SHA256 / HKDF-SHA512 (RFC 5869)
// ========================================================================

/**
 * sha256_hkdf_extract – HKDF-Extract using SHA-256
 *
 * Extracts a pseudorandom key (PRK) from input keying material (IKM) and optional salt.
 * This is the first step of HKDF; call sha256_hkdf_expand to derive actual keys.
 *
 * Parameters:
 *   salt – optional salt value (use NULL for default zero salt)
 *   slen – salt length (ignored if salt is NULL)
 *   ikm  – input keying material (e.g., shared secret from ECDH)
 *   ilen – ikm length
 *   prk  – output pseudorandom key (exactly 32 bytes)
 */
static inline int sha256_hkdf_extract(const uint8_t *salt, size_t slen,
                                      const uint8_t *ikm, size_t ilen,
                                      uint8_t prk[SHA256_DIGEST_SIZE])
{
	static const uint8_t zero[SHA256_DIGEST_SIZE] = {0};
	if (!salt) { salt = zero; slen = sizeof(zero); }
	sha256_hmac(salt, slen, ikm, ilen, prk);
	return 0;
}

/**
 * sha256_hkdf_expand – HKDF-Expand using SHA-256
 *
 * Expands a pseudorandom key (PRK) into output keying material (OKM) of desired length.
 * This is the second step of HKDF, after sha256_hkdf_extract.
 *
 * Parameters:
 *   prk  – pseudorandom key from extract step (exactly 32 bytes)
 *   info – optional context/application-specific info (use NULL if not needed)
 *   ilen – info length (ignored if info is NULL)
 *   okm  – output buffer for derived key material
 *   olen – desired output length (max 255 * 32 = 8160 bytes)
 *
 * Returns 0 on success, -1 if olen is too large.
 */
static inline int sha256_hkdf_expand(const uint8_t prk[SHA256_DIGEST_SIZE],
                                     const uint8_t *info, size_t ilen,
                                     uint8_t *okm, size_t olen)
{
	if (olen > 255 * SHA256_DIGEST_SIZE) return -1;

	uint8_t T[SHA256_DIGEST_SIZE] = {0}, buf[SHA256_DIGEST_SIZE + 256 + 1];
	size_t Tlen = 0, pos = 0;
	uint8_t ctr = 1;

	while (pos < olen) {
		size_t blen = 0;

		// T(i) = HMAC(PRK, T(i-1) || info || i)
		if (Tlen) { memcpy(buf, T, Tlen); blen = Tlen; }
		if (info && ilen) { memcpy(buf + blen, info, ilen); blen += ilen; }
		buf[blen++] = ctr++;

		sha256_hmac(prk, SHA256_DIGEST_SIZE, buf, blen, T);
		Tlen = SHA256_DIGEST_SIZE;

		size_t copy = (olen - pos < SHA256_DIGEST_SIZE) ? (olen - pos) : SHA256_DIGEST_SIZE;
		memcpy(okm + pos, T, copy);
		pos += copy;
	}

	__jacl_explicit_bzero(T, SHA256_DIGEST_SIZE);
	__jacl_explicit_bzero(buf, sizeof(buf));
	return 0;
}

/**
 * sha512_hkdf_extract – HKDF-Extract using SHA-512
 *
 * Identical to sha256_hkdf_extract but using SHA-512 (64-byte PRK).
 */
static inline int sha512_hkdf_extract(const uint8_t *salt, size_t slen,
                                      const uint8_t *ikm, size_t ilen,
                                      uint8_t prk[SHA512_DIGEST_SIZE])
{
	static const uint8_t zero[SHA512_DIGEST_SIZE] = {0};
	if (!salt) { salt = zero; slen = sizeof(zero); }
	sha512_hmac(salt, slen, ikm, ilen, prk);
	return 0;
}

/**
 * sha512_hkdf_expand – HKDF-Expand using SHA-512
 *
 * Identical to sha256_hkdf_expand but using SHA-512 (max output 255 * 64 = 16320 bytes).
 */
static inline int sha512_hkdf_expand(const uint8_t prk[SHA512_DIGEST_SIZE],
                                     const uint8_t *info, size_t ilen,
                                     uint8_t *okm, size_t olen)
{
	if (olen > 255 * SHA512_DIGEST_SIZE) return -1;

	uint8_t T[SHA512_DIGEST_SIZE] = {0}, buf[SHA512_DIGEST_SIZE + 256 + 1];
	size_t Tlen = 0, pos = 0;
	uint8_t ctr = 1;

	while (pos < olen) {
		size_t blen = 0;

		// T(i) = HMAC(PRK, T(i-1) || info || i)
		if (Tlen) { memcpy(buf, T, Tlen); blen = Tlen; }
		if (info && ilen) { memcpy(buf + blen, info, ilen); blen += ilen; }
		buf[blen++] = ctr++;

		sha512_hmac(prk, SHA512_DIGEST_SIZE, buf, blen, T);
		Tlen = SHA512_DIGEST_SIZE;

		size_t copy = (olen - pos < SHA512_DIGEST_SIZE) ? (olen - pos) : SHA512_DIGEST_SIZE;
		memcpy(okm + pos, T, copy);
		pos += copy;
	}

	__jacl_explicit_bzero(T, SHA512_DIGEST_SIZE);
	__jacl_explicit_bzero(buf, sizeof(buf));
	return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_SHA2_H */
