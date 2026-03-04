/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef CRYPTO_MD5_H
#define CRYPTO_MD5_H
#pragma once

/**
 * MD5 – Message Digest Algorithm 5 (RFC 1321)
 *
 * **WARNING: MD5 is cryptographically broken. Use only for:**
 *   - Non-cryptographic checksums (file integrity, deduplication).
 *   - Legacy protocol compatibility where you have no choice.
 *
 * **Never use MD5 for:**
 *   - Password hashing (use Argon2id).
 *   - Digital signatures or MACs (use HMAC-SHA256 or better).
 *   - Any new system design.
 *
 * Namespace: md5_*
 */

#include <crypto/base.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MD5_DIGEST_SIZE 16
#define MD5_BLOCK_SIZE  64

typedef struct {
	uint32_t state[4];
	uint64_t bitlen;
	uint8_t  buf[MD5_BLOCK_SIZE];
	size_t   len;
} md5_ctx;

static inline void md5_transform(md5_ctx *ctx) {
	uint32_t a = ctx->state[0];
	uint32_t b = ctx->state[1];
	uint32_t c = ctx->state[2];
	uint32_t d = ctx->state[3];
	uint32_t x[16];

	// MD5 uses little-endian byte order
	for (int i = 0; i < 16; i++)
		x[i] = __jacl_load32_le(&ctx->buf[i * 4]);

	#define F(x,y,z) ((x & y) | (~x & z))
	#define G(x,y,z) ((x & z) | (y & ~z))
	#define H(x,y,z) (x ^ y ^ z)
	#define I(x,y,z) (y ^ (x | ~z))

	#define STEP(f,a,b,c,d,xk,s,ti) \
		a += f(b,c,d) + xk + ti; \
		a = b + rotl32(a, s)

	// Round 1
	STEP(F,a,b,c,d,x[ 0], 7,0xd76aa478); STEP(F,d,a,b,c,x[ 1],12,0xe8c7b756);
	STEP(F,c,d,a,b,x[ 2],17,0x242070db); STEP(F,b,c,d,a,x[ 3],22,0xc1bdceee);
	STEP(F,a,b,c,d,x[ 4], 7,0xf57c0faf); STEP(F,d,a,b,c,x[ 5],12,0x4787c62a);
	STEP(F,c,d,a,b,x[ 6],17,0xa8304613); STEP(F,b,c,d,a,x[ 7],22,0xfd469501);
	STEP(F,a,b,c,d,x[ 8], 7,0x698098d8); STEP(F,d,a,b,c,x[ 9],12,0x8b44f7af);
	STEP(F,c,d,a,b,x[10],17,0xffff5bb1); STEP(F,b,c,d,a,x[11],22,0x895cd7be);
	STEP(F,a,b,c,d,x[12], 7,0x6b901122); STEP(F,d,a,b,c,x[13],12,0xfd987193);
	STEP(F,c,d,a,b,x[14],17,0xa679438e); STEP(F,b,c,d,a,x[15],22,0x49b40821);

	// Round 2
	STEP(G,a,b,c,d,x[ 1], 5,0xf61e2562); STEP(G,d,a,b,c,x[ 6], 9,0xc040b340);
	STEP(G,c,d,a,b,x[11],14,0x265e5a51); STEP(G,b,c,d,a,x[ 0],20,0xe9b6c7aa);
	STEP(G,a,b,c,d,x[ 5], 5,0xd62f105d); STEP(G,d,a,b,c,x[10], 9,0x02441453);
	STEP(G,c,d,a,b,x[15],14,0xd8a1e681); STEP(G,b,c,d,a,x[ 4],20,0xe7d3fbc8);
	STEP(G,a,b,c,d,x[ 9], 5,0x21e1cde6); STEP(G,d,a,b,c,x[14], 9,0xc33707d6);
	STEP(G,c,d,a,b,x[ 3],14,0xf4d50d87); STEP(G,b,c,d,a,x[ 8],20,0x455a14ed);
	STEP(G,a,b,c,d,x[13], 5,0xa9e3e905); STEP(G,d,a,b,c,x[ 2], 9,0xfcefa3f8);
	STEP(G,c,d,a,b,x[ 7],14,0x676f02d9); STEP(G,b,c,d,a,x[12],20,0x8d2a4c8a);

	// Round 3
	STEP(H,a,b,c,d,x[ 5], 4,0xfffa3942); STEP(H,d,a,b,c,x[ 8],11,0x8771f681);
	STEP(H,c,d,a,b,x[11],16,0x6d9d6122); STEP(H,b,c,d,a,x[14],23,0xfde5380c);
	STEP(H,a,b,c,d,x[ 1], 4,0xa4beea44); STEP(H,d,a,b,c,x[ 4],11,0x4bdecfa9);
	STEP(H,c,d,a,b,x[ 7],16,0xf6bb4b60); STEP(H,b,c,d,a,x[10],23,0xbebfbc70);
	STEP(H,a,b,c,d,x[13], 4,0x289b7ec6); STEP(H,d,a,b,c,x[ 0],11,0xeaa127fa);
	STEP(H,c,d,a,b,x[ 3],16,0xd4ef3085); STEP(H,b,c,d,a,x[ 6],23,0x04881d05);
	STEP(H,a,b,c,d,x[ 9], 4,0xd9d4d039); STEP(H,d,a,b,c,x[12],11,0xe6db99e5);
	STEP(H,c,d,a,b,x[15],16,0x1fa27cf8); STEP(H,b,c,d,a,x[ 2],23,0xc4ac5665);

	// Round 4
	STEP(I,a,b,c,d,x[ 0], 6,0xf4292244); STEP(I,d,a,b,c,x[ 7],10,0x432aff97);
	STEP(I,c,d,a,b,x[14],15,0xab9423a7); STEP(I,b,c,d,a,x[ 5],21,0xfc93a039);
	STEP(I,a,b,c,d,x[12], 6,0x655b59c3); STEP(I,d,a,b,c,x[ 3],10,0x8f0ccc92);
	STEP(I,c,d,a,b,x[10],15,0xffeff47d); STEP(I,b,c,d,a,x[ 1],21,0x85845dd1);
	STEP(I,a,b,c,d,x[ 8], 6,0x6fa87e4f); STEP(I,d,a,b,c,x[15],10,0xfe2ce6e0);
	STEP(I,c,d,a,b,x[ 6],15,0xa3014314); STEP(I,b,c,d,a,x[13],21,0x4e0811a1);
	STEP(I,a,b,c,d,x[ 4], 6,0xf7537e82); STEP(I,d,a,b,c,x[11],10,0xbd3af235);
	STEP(I,c,d,a,b,x[ 2],15,0x2ad7d2bb); STEP(I,b,c,d,a,x[ 9],21,0xeb86d391);

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;

	#undef STEP
	#undef F
	#undef G
	#undef H
	#undef I
}

static inline void md5_init(md5_ctx *ctx) {
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xefcdab89;
	ctx->state[2] = 0x98badcfe;
	ctx->state[3] = 0x10325476;
	ctx->bitlen = 0;
	ctx->len = 0;
}

static inline void md5_update(md5_ctx *ctx, const uint8_t *data, size_t len) {
	for (size_t i = 0; i < len; i++) {
		ctx->buf[ctx->len++] = data[i];
		if (ctx->len == 64) {
			md5_transform(ctx);
			ctx->bitlen += 512;
			ctx->len = 0;
		}
	}
}

static inline void md5_final(md5_ctx *ctx, uint8_t out[MD5_DIGEST_SIZE]) {
	size_t i = ctx->len;
	ctx->buf[i++] = 0x80;
	if (i > 56) {
		while (i < 64) ctx->buf[i++] = 0;
		md5_transform(ctx);
		i = 0;
	}
	while (i < 56) ctx->buf[i++] = 0;
	
	ctx->bitlen += ctx->len * 8;
	__jacl_store64_le(&ctx->buf[56], ctx->bitlen);
	md5_transform(ctx);
	
	for (i = 0; i < 4; i++)
		__jacl_store32_le(&out[i * 4], ctx->state[i]);
}

static inline void md5(const uint8_t *data, size_t len, uint8_t out[MD5_DIGEST_SIZE]) {
	md5_ctx ctx;
	md5_init(&ctx);
	md5_update(&ctx, data, len);
	md5_final(&ctx, out);
}

/**
 * md5_hmac – HMAC-MD5 (RFC 2104)
 *
 * **Legacy only.** Provided for compatibility with old protocols.
 * New designs should use HMAC-SHA256 or better.
 */
static inline void md5_hmac(const uint8_t *key, size_t klen,
                            const uint8_t *msg, size_t mlen,
                            uint8_t out[MD5_DIGEST_SIZE])
{
	uint8_t k[MD5_BLOCK_SIZE], ipad[MD5_BLOCK_SIZE], opad[MD5_BLOCK_SIZE];
	memset(k, 0, MD5_BLOCK_SIZE);
	
	if (klen > MD5_BLOCK_SIZE) {
		md5(key, klen, k);
	} else {
		memcpy(k, key, klen);
	}
	
	for (int i = 0; i < MD5_BLOCK_SIZE; i++) {
		ipad[i] = k[i] ^ 0x36;
		opad[i] = k[i] ^ 0x5c;
	}
	
	md5_ctx ctx;
	md5_init(&ctx);
	md5_update(&ctx, ipad, MD5_BLOCK_SIZE);
	md5_update(&ctx, msg, mlen);
	md5_final(&ctx, out);

	md5_init(&ctx);
	md5_update(&ctx, opad, MD5_BLOCK_SIZE);
	md5_update(&ctx, out, MD5_DIGEST_SIZE);
	md5_final(&ctx, out);

	__jacl_explicit_bzero(k, MD5_BLOCK_SIZE);
	__jacl_explicit_bzero(ipad, MD5_BLOCK_SIZE);
	__jacl_explicit_bzero(opad, MD5_BLOCK_SIZE);
}

#ifdef __cplusplus
}
#endif
#endif /* CRYPTO_MD5_H */

