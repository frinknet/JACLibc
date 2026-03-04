/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef CRYPTO_X25519_H
#define CRYPTO_X25519_H
#pragma once

/**
 * X25519 – Elliptic Curve Diffie-Hellman Key Exchange (RFC 7748)
 *
 * X25519 is a high-speed, high-security ECDH function built on Curve25519.
 * It's used for key agreement in TLS 1.3, WireGuard, Signal, SSH, and many
 * modern cryptographic protocols.
 *
 * **Use cases:**
 *   - Diffie-Hellman key exchange (establish shared secrets).
 *   - Forward secrecy in TLS/QUIC/SSH connections.
 *   - Encrypted messaging (Signal Protocol, Matrix).
 *   - VPN key agreement (WireGuard).
 *
 * **Key generation:**
 *   1. Generate 32 random bytes as secret key.
 *   2. Compute public key: public = x25519_public_key(secret).
 *
 * **Shared secret derivation:**
 *   Alice and Bob exchange public keys, then:
 *   - Alice computes: shared = x25519(alice_secret, bob_public)
 *   - Bob computes: shared = x25519(bob_secret, alice_public)
 *   Both get the same 32-byte shared secret.
 *
 * **Security notes:**
 *   - Secret keys are clamped automatically (bits 0,1,2 cleared; bit 254 set; bit 255 cleared).
 *   - Shared secrets should be passed through a KDF (HKDF, BLAKE2, etc.) before use.
 *   - Validation happens during x25519() - returns -1 for invalid public keys.
 *   - Constant-time implementation (timing-attack resistant).
 *
 * **Key sizes:**
 *   - Secret key: 32 bytes (256 bits)
 *   - Public key: 32 bytes (256 bits, x-coordinate only)
 *   - Shared secret: 32 bytes (256 bits)
 *
 * Namespace: x25519_*
 */

#include <crypto/curve25519.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * x25519_public_key – Generate public key from secret key
 *
 * Computes public_key = secret_key * G (base point multiplication).
 *
 * Parameters:
 *   public_key – Output buffer (32 bytes)
 *   secret_key – Secret key (32 bytes, should be from CSPRNG)
 *
 * Example:
 *   uint8_t secret[32], public[32];
 *   getrandom(secret, 32, 0);  // Linux
 *   x25519_public_key(public, secret);
 */
static inline void x25519_public_key(uint8_t public_key[32], const uint8_t secret_key[32]) {
	curve25519_scalarmult_base(public_key, secret_key);
}

/**
 * x25519 – Compute shared secret (Diffie-Hellman)
 *
 * Computes shared_secret = my_secret * their_public.
 *
 * Parameters:
 *   shared_secret – Output buffer (32 bytes)
 *   my_secret     – My secret key (32 bytes)
 *   their_public  – Their public key (32 bytes)
 *
 * Returns 0 on success, -1 if result is all-zeros (invalid public key).
 *
 * SECURITY: Always check return value. If -1, abort the exchange.
 * After success, pass shared_secret through a KDF before using as encryption key.
 *
 * Example:
 *   uint8_t alice_secret[32], alice_public[32];
 *   uint8_t bob_public[32], shared[32];
 *
 *   getrandom(alice_secret, 32, 0);
 *   x25519_public_key(alice_public, alice_secret);
 *
 *   // ... receive bob_public from network ...
 *
 *   if (x25519(shared, alice_secret, bob_public) != 0) {
 *       return -1;  // Invalid public key
 *   }
 *
 *   // Derive encryption key
 *   uint8_t key[32];
 *   hkdf_sha256(key, 32, shared, 32, "my-app", 6, NULL, 0);
 */
static inline int x25519(uint8_t shared_secret[32], const uint8_t my_secret[32], const uint8_t their_public[32]) {
	curve25519_scalarmult(shared_secret, my_secret, their_public);

	// Check for all-zeros result (weak public key)
	int is_zero = 1;

	for (int i = 0; i < 32; i++) is_zero &= (shared_secret[i] == 0);

	if (is_zero) {
		__jacl_explicit_bzero(shared_secret, 32);

		return -1;
	}

	return 0;
}

#ifdef __cplusplus
}
#endif
#endif /* CRYPTO_X25519_H */
