/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef CRYPTO_PBKDF2_H
#define CRYPTO_PBKDF2_H
#pragma once

/**
 * PBKDF2 – Password-Based Key Derivation Function 2 (RFC 8018)
 *
 * Derives cryptographic keys from passwords using iterative HMAC.
 *
 * **Use cases:**
 *   - Key derivation from user passwords for encryption keys
 *   - Legacy password storage (prefer Argon2id for new systems)
 *   - Protocol compatibility (WPA2, LUKS, some VPNs)
 *
 * **WARNING: For new password storage, use Argon2id instead.**
 *   - PBKDF2 is not memory-hard and vulnerable to GPU/ASIC attacks
 *   - Argon2id provides better resistance to parallel cracking
 *   - PBKDF2 is still acceptable for key derivation from high-entropy secrets
 *
 * **Iteration count guidance (2026):**
 *   - Minimum for passwords: 600,000 iterations (OWASP)
 *   - Recommended: 1,000,000+ for HMAC-SHA256
 *   - Tune based on acceptable delay (~100ms on target hardware)
 *
 * Namespace: pbkdf2_*
 */

#include <crypto/base.h>
#include <crypto/sha2.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * pbkdf2 – Generic PBKDF2 with custom HMAC
 *
 * Parameters:
 *   hmac_fn  – HMAC function: void (*)(key, klen, msg, mlen, out)
 *   hmac_len – HMAC output length (32 for SHA-256, 64 for SHA-512)
 *   password – User password
 *   plen     – Password length
 *   salt     – Random salt (at least 16 bytes recommended)
 *   slen     – Salt length
 *   iters    – Iteration count (600k+ for passwords)
 *   out      – Output buffer for derived key
 *   outlen   – Desired key length
 *
 * Returns 0 on success, -1 on invalid parameters.
 */
static inline int pbkdf2(
	void (*hmac_fn)(const uint8_t*, size_t, const uint8_t*, size_t, uint8_t*),
	size_t hmac_len,
	const uint8_t *password, size_t plen,
	const uint8_t *salt, size_t slen,
	uint32_t iters,
	uint8_t *out, size_t outlen)
{
	if (!hmac_fn || !password || !salt || !out || iters == 0 || outlen == 0)
		return -1;
	if (hmac_len > 64 || slen > 252)
		return -1;

	uint8_t block[64];
	uint8_t tmp[64];
	uint8_t salt_block[256];

	memcpy(salt_block, salt, slen);

	uint32_t num_blocks = (uint32_t)((outlen + hmac_len - 1) / hmac_len);

	for (uint32_t blk = 1; blk <= num_blocks; blk++) {
		__jacl_store32_be(salt_block + slen, blk);

		hmac_fn(password, plen, salt_block, slen + 4, block);
		memcpy(tmp, block, hmac_len);

		for (uint32_t i = 1; i < iters; i++) {
			hmac_fn(password, plen, tmp, hmac_len, tmp);
			for (size_t j = 0; j < hmac_len; j++)
				block[j] ^= tmp[j];
		}

		size_t offset = (blk - 1) * hmac_len;
		size_t cpylen = (offset + hmac_len > outlen) ? (outlen - offset) : hmac_len;
		memcpy(out + offset, block, cpylen);
	}

	__jacl_explicit_bzero(block, sizeof(block));
	__jacl_explicit_bzero(tmp, sizeof(tmp));
	__jacl_explicit_bzero(salt_block, sizeof(salt_block));

	return 0;
}

/**
 * pbkdf2_hmac_sha256 – PBKDF2 with HMAC-SHA256
 *
 * Most common variant. Use 600k+ iterations for password hashing.
 */
static inline int pbkdf2_hmac_sha256(
	const uint8_t *password, size_t plen,
	const uint8_t *salt, size_t slen,
	uint32_t iters,
	uint8_t *out, size_t outlen)
{
	return pbkdf2(sha256_hmac, SHA256_DIGEST_SIZE,
	              password, plen, salt, slen, iters, out, outlen);
}

/**
 * pbkdf2_hmac_sha512 – PBKDF2 with HMAC-SHA512
 *
 * Slower but higher security margin. Double iteration count vs SHA-256.
 */
static inline int pbkdf2_hmac_sha512(
	const uint8_t *password, size_t plen,
	const uint8_t *salt, size_t slen,
	uint32_t iters,
	uint8_t *out, size_t outlen)
{
	return pbkdf2(sha512_hmac, SHA512_DIGEST_SIZE,
	              password, plen, salt, slen, iters, out, outlen);
}

#ifdef __cplusplus
}
#endif

#endif /* CRYPTO_PBKDF2_H */
