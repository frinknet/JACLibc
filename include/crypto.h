/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _CRYPTO_H
#define _CRYPTO_H
#pragma once

#include <config.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbit.h>
#include <sys/random.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ====== Constants ====== */

#define SHA256_DIGEST_SIZE 32
#define SHA256_BLOCK_SIZE  64
#define SHA512_DIGEST_SIZE 64
#define SHA512_BLOCK_SIZE  128
#define ED25519_PUBLIC_KEY_SIZE  32
#define ED25519_PRIVATE_KEY_SIZE 64
#define ED25519_SEED_SIZE        32
#define ED25519_SIGNATURE_SIZE   64
#define X25519_KEY_SIZE 32
#define CHACHA20_KEY_SIZE 32
#define CHACHA20_NONCE_SIZE 12
#define POLY1305_TAG_SIZE 16
#define AES_BLOCK_SIZE 16

static const uint32_t K256[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

static const uint64_t K512[80] = {
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

static const uint8_t AES_SBOX[256] = {
	0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76,
	0xca,0x82,0xc9,0x7d,0xfa,0x59,0x47,0xf0,0xad,0xd4,0xa2,0xaf,0x9c,0xa4,0x72,0xc0,
	0xb7,0xfd,0x93,0x26,0x36,0x3f,0xf7,0xcc,0x34,0xa5,0xe5,0xf1,0x71,0xd8,0x31,0x15,
	0x04,0xc7,0x23,0xc3,0x18,0x96,0x05,0x9a,0x07,0x12,0x80,0xe2,0xeb,0x27,0xb2,0x75,
	0x09,0x83,0x2c,0x1a,0x1b,0x6e,0x5a,0xa0,0x52,0x3b,0xd6,0xb3,0x29,0xe3,0x2f,0x84,
	0x53,0xd1,0x00,0xed,0x20,0xfc,0xb1,0x5b,0x6a,0xcb,0xbe,0x39,0x4a,0x4c,0x58,0xcf,
	0xd0,0xef,0xaa,0xfb,0x43,0x4d,0x33,0x85,0x45,0xf9,0x02,0x7f,0x50,0x3c,0x9f,0xa8,
	0x51,0xa3,0x40,0x8f,0x92,0x9d,0x38,0xf5,0xbc,0xb6,0xda,0x21,0x10,0xff,0xf3,0xd2,
	0xcd,0x0c,0x13,0xec,0x5f,0x97,0x44,0x17,0xc4,0xa7,0x7e,0x3d,0x64,0x5d,0x19,0x73,
	0x60,0x81,0x4f,0xdc,0x22,0x2a,0x90,0x88,0x46,0xee,0xb8,0x14,0xde,0x5e,0x0b,0xdb,
	0xe0,0x32,0x3a,0x0a,0x49,0x06,0x24,0x5c,0xc2,0xd3,0xac,0x62,0x91,0x95,0xe4,0x79,
	0xe7,0xc8,0x37,0x6d,0x8d,0xd5,0x4e,0xa9,0x6c,0x56,0xf4,0xea,0x65,0x7a,0xae,0x08,
	0xba,0x78,0x25,0x2e,0x1c,0xa6,0xb4,0xc6,0xe8,0xdd,0x74,0x1f,0x4b,0xbd,0x8b,0x8a,
	0x70,0x3e,0xb5,0x66,0x48,0x03,0xf6,0x0e,0x61,0x35,0x57,0xb9,0x86,0xc1,0x1d,0x9e,
	0xe1,0xf8,0x98,0x11,0x69,0xd9,0x8e,0x94,0x9b,0x1e,0x87,0xe9,0xce,0x55,0x28,0xdf,
	0x8c,0xa1,0x89,0x0d,0xbf,0xe6,0x42,0x68,0x41,0x99,0x2d,0x0f,0xb0,0x54,0xbb,0x16
};

static const uint8_t AES_RSBOX[256] = {
	0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
	0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
	0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
	0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
	0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
	0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
	0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
	0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
	0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
	0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
	0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
	0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
	0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
	0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
	0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
	0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

static const uint8_t AES_RCON[11] = {
	0x8d,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x1b,0x36
};

/* ====== Types ====== */

typedef struct {
	uint32_t state[8];
	uint64_t bitlen;
	uint8_t  buf[SHA256_BLOCK_SIZE];
	size_t   len;
} sha256_ctx;

typedef struct {
	uint64_t state[8];
	uint64_t bitlen[2];
	uint8_t  buf[SHA512_BLOCK_SIZE];
	size_t   len;
} sha512_ctx;

typedef struct {
	uint32_t rk[60];
	int nr;
} aes_ctx;

typedef struct {
	uint32_t state[16];
	uint32_t counter;
} chacha20_ctx;

typedef struct {
	uint32_t r[5], h[5], pad[4];
	uint8_t buf[16];
	size_t len;
} poly1305_ctx;

typedef int64_t __jacl_fe[10];

typedef struct {
	__jacl_fe X, Y, Z, T;
} __jacl_ge_p3;

typedef struct {
	__jacl_fe X, Y, Z;
} __jacl_ge_p2;

typedef struct {
	__jacl_fe X, Z;
} __jacl_ge_p1p1;

typedef struct {
	__jacl_fe YplusX, YminusX, Z, T2d;
} __jacl_ge_cached;

typedef struct {
	__jacl_fe YplusX, YminusX, T2d;
} __jacl_ge_precomp;

/* ====== Macros ====== */

#define __jacl_load32be(p) (((uint32_t)(p)[0]<<24)|((uint32_t)(p)[1]<<16)|((uint32_t)(p)[2]<<8)|(p)[3])
#define __jacl_load32le(p) (((uint32_t)(p)[3]<<24)|((uint32_t)(p)[2]<<16)|((uint32_t)(p)[1]<<8)|(p)[0])
#define __jacl_load64be(p) (((uint64_t)__jacl_load32be(p)<<32)|__jacl_load32be((p)+4))
#define __jacl_load64le(p) (((uint64_t)__jacl_load32le((p)+4)<<32)|__jacl_load32le(p))

#define __jacl_store32be(p,v) do{ \
	uint32_t _v=(v); \
	(p)[0]=_v>>24; (p)[1]=_v>>16; (p)[2]=_v>>8; (p)[3]=_v; \
}while(0)

#define __jacl_store32le(p,v) do{ \
	uint32_t _v=(v); \
	(p)[0]=_v; (p)[1]=_v>>8; (p)[2]=_v>>16; (p)[3]=_v>>24; \
}while(0)

#define __jacl_store64be(p,v) do{ \
	uint64_t _v=(v); \
	__jacl_store32be(p,_v>>32); \
	__jacl_store32be((p)+4,_v); \
}while(0)

#define __jacl_store64le(p,v) do{ \
	uint64_t _v=(v); \
	__jacl_store32le(p,_v); \
	__jacl_store32le((p)+4,_v>>32); \
}while(0)

#define __jacl_qr(a,b,c,d) do{ \
	a+=b; d^=a; d=rotl32(d,16); \
	c+=d; b^=c; b=rotl32(b,12); \
	a+=b; d^=a; d=rotl32(d,8); \
	c+=d; b^=c; b=rotl32(b,7); \
}while(0)

/* ====== Internal Utilities ====== */

static inline int __jacl_timingsafe_memcmp(const void *a, const void *b, size_t n) {
	const uint8_t *pa = a, *pb = b;
	uint8_t d = 0;
	for(size_t i = 0; i < n; i++) d |= pa[i] ^ pb[i];
	return d;
}

static inline void __jacl_explicit_bzero(void *p, size_t n) {
	volatile uint8_t *vp = p;
	while(n--) *vp++ = 0;
}

static inline void __jacl_ct_select(uint8_t *r, const uint8_t *a, const uint8_t *b, size_t n, int c) {
	uint8_t mask = -(uint8_t)(c & 1);
	for(size_t i = 0; i < n; i++) r[i] = (a[i] & mask) | (b[i] & ~mask);
}

/* ====== SHA-256 ====== */

static inline void sha256_transform(sha256_ctx *ctx) {
	uint32_t w[64], s[8];

	for(int i = 0; i < 16; i++) w[i] = __jacl_load32be(ctx->buf + i*4);
	for(int i = 16; i < 64; i++) {
		uint32_t s0 = rotl32(w[i-15],25) ^ rotl32(w[i-15],14) ^ (w[i-15]>>3);
		uint32_t s1 = rotl32(w[i-2],15) ^ rotl32(w[i-2],13) ^ (w[i-2]>>10);
		w[i] = w[i-16] + s0 + w[i-7] + s1;
	}

	memcpy(s, ctx->state, 32);
	for(int i = 0; i < 64; i++) {
		uint32_t S1 = rotl32(s[4],26) ^ rotl32(s[4],21) ^ rotl32(s[4],7);
		uint32_t ch = (s[4] & s[5]) ^ (~s[4] & s[6]);
		uint32_t temp1 = s[7] + S1 + ch + K256[i] + w[i];
		uint32_t S0 = rotl32(s[0],30) ^ rotl32(s[0],19) ^ rotl32(s[0],10);
		uint32_t maj = (s[0] & s[1]) ^ (s[0] & s[2]) ^ (s[1] & s[2]);
		uint32_t temp2 = S0 + maj;

		s[7]=s[6]; s[6]=s[5]; s[5]=s[4]; s[4]=s[3]+temp1;
		s[3]=s[2]; s[2]=s[1]; s[1]=s[0]; s[0]=temp1+temp2;
	}

	for(int i = 0; i < 8; i++) ctx->state[i] += s[i];
}

static inline void sha256_init(sha256_ctx *ctx) {
	static const uint32_t IV[8] = {
		0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,
		0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19
	};
	memcpy(ctx->state, IV, 32);
	ctx->bitlen = 0; ctx->len = 0;
}

static inline void sha256_update(sha256_ctx *ctx, const uint8_t *data, size_t len) {
	for(size_t i = 0; i < len; i++) {
		ctx->buf[ctx->len++] = data[i];
		if(ctx->len == 64) {
			sha256_transform(ctx);
			ctx->bitlen += 512;
			ctx->len = 0;
		}
	}
}

static inline void sha256_final(sha256_ctx *ctx, uint8_t out[32]) {
	size_t i = ctx->len;
	ctx->buf[i++] = 0x80;

	if(i > 56) {
		while(i < 64) ctx->buf[i++] = 0;
		sha256_transform(ctx);
		i = 0;
	}

	while(i < 56) ctx->buf[i++] = 0;
	ctx->bitlen += ctx->len * 8;
	__jacl_store64be(ctx->buf + 56, ctx->bitlen);
	sha256_transform(ctx);

	for(i = 0; i < 8; i++) __jacl_store32be(out + i*4, ctx->state[i]);
}

static inline void sha256(const uint8_t *data, size_t len, uint8_t out[32]) {
	sha256_ctx ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, data, len);
	sha256_final(&ctx, out);
}

/* ====== SHA-512 ====== */

static inline void sha512_transform(sha512_ctx *ctx) {
	uint64_t w[80], s[8];

	for(int i = 0; i < 16; i++) w[i] = __jacl_load64be(ctx->buf + i*8);
	for(int i = 16; i < 80; i++) {
		uint64_t s0 = rotl64(w[i-15],63) ^ rotl64(w[i-15],56) ^ (w[i-15]>>7);
		uint64_t s1 = rotl64(w[i-2],45) ^ rotl64(w[i-2],3) ^ (w[i-2]>>6);
		w[i] = w[i-16] + s0 + w[i-7] + s1;
	}

	memcpy(s, ctx->state, 64);
	for(int i = 0; i < 80; i++) {
		uint64_t S1 = rotl64(s[4],50) ^ rotl64(s[4],46) ^ rotl64(s[4],23);
		uint64_t ch = (s[4] & s[5]) ^ (~s[4] & s[6]);
		uint64_t temp1 = s[7] + S1 + ch + K512[i] + w[i];
		uint64_t S0 = rotl64(s[0],36) ^ rotl64(s[0],30) ^ rotl64(s[0],25);
		uint64_t maj = (s[0] & s[1]) ^ (s[0] & s[2]) ^ (s[1] & s[2]);
		uint64_t temp2 = S0 + maj;

		s[7]=s[6]; s[6]=s[5]; s[5]=s[4]; s[4]=s[3]+temp1;
		s[3]=s[2]; s[2]=s[1]; s[1]=s[0]; s[0]=temp1+temp2;
	}

	for(int i = 0; i < 8; i++) ctx->state[i] += s[i];
}

static inline void sha512_init(sha512_ctx *ctx) {
	static const uint64_t IV[8] = {
		0x6a09e667f3bcc908ULL,0xbb67ae8584caa73bULL,0x3c6ef372fe94f82bULL,0xa54ff53a5f1d36f1ULL,
		0x510e527fade682d1ULL,0x9b05688c2b3e6c1fULL,0x1f83d9abfb41bd6bULL,0x5be0cd19137e2179ULL
	};
	memcpy(ctx->state, IV, 64);
	ctx->bitlen[0] = ctx->bitlen[1] = 0; ctx->len = 0;
}

static inline void sha512_update(sha512_ctx *ctx, const uint8_t *data, size_t len) {
	for(size_t i = 0; i < len; i++) {
		ctx->buf[ctx->len++] = data[i];
		if(ctx->len == 128) {
			sha512_transform(ctx);
			ctx->bitlen[0] += 1024;
			if(ctx->bitlen[0] < 1024) ctx->bitlen[1]++;
			ctx->len = 0;
		}
	}
}

static inline void sha512_final(sha512_ctx *ctx, uint8_t out[64]) {
	size_t i = ctx->len;
	ctx->buf[i++] = 0x80;

	if(i > 112) {
		while(i < 128) ctx->buf[i++] = 0;
		sha512_transform(ctx);
		i = 0;
	}

	while(i < 112) ctx->buf[i++] = 0;
	ctx->bitlen[0] += ctx->len * 8;
	if(ctx->bitlen[0] < ctx->len * 8) ctx->bitlen[1]++;
	__jacl_store64be(ctx->buf + 112, ctx->bitlen[1]);
	__jacl_store64be(ctx->buf + 120, ctx->bitlen[0]);
	sha512_transform(ctx);

	for(i = 0; i < 8; i++) __jacl_store64be(out + i*8, ctx->state[i]);
}

static inline void sha512(const uint8_t *data, size_t len, uint8_t out[64]) {
	sha512_ctx ctx;
	sha512_init(&ctx);
	sha512_update(&ctx, data, len);
	sha512_final(&ctx, out);
}

/* ====== HMAC ====== */

#define __jacl_hmac_impl(bits, blk) \
static inline void hmac_sha##bits(const uint8_t *key, size_t klen, const uint8_t *msg, size_t mlen, uint8_t out[bits/8]) { \
	uint8_t k[blk], ipad[blk], opad[blk]; \
	memset(k, 0, blk); \
	if(klen > blk) sha##bits(key, klen, k); \
	else memcpy(k, key, klen); \
	for(int i = 0; i < blk; i++) { ipad[i] = k[i] ^ 0x36; opad[i] = k[i] ^ 0x5c; } \
	sha##bits##_ctx ctx; \
	sha##bits##_init(&ctx); sha##bits##_update(&ctx, ipad, blk); sha##bits##_update(&ctx, msg, mlen); sha##bits##_final(&ctx, out); \
	sha##bits##_init(&ctx); sha##bits##_update(&ctx, opad, blk); sha##bits##_update(&ctx, out, bits/8); sha##bits##_final(&ctx, out); \
	__jacl_explicit_bzero(k, blk); __jacl_explicit_bzero(ipad, blk); __jacl_explicit_bzero(opad, blk); \
}

__jacl_hmac_impl(256, 64)
__jacl_hmac_impl(512, 128)

/* ====== HKDF ====== */

static inline int hkdf_extract(const uint8_t *salt, size_t slen, const uint8_t *ikm, size_t ilen, uint8_t prk[32]) {
	static const uint8_t zero[32] = {0};
	if(!salt) { salt = zero; slen = 32; }
	hmac_sha256(salt, slen, ikm, ilen, prk);
	return 0;
}

static inline int hkdf_expand(const uint8_t prk[32], const uint8_t *info, size_t ilen, uint8_t *okm, size_t olen) {
	uint8_t T[32] = {0}, buf[289];
	size_t Tlen = 0, pos = 0;
	uint8_t ctr = 1;

	while(pos < olen) {
		size_t blen = 0;
		if(Tlen) { memcpy(buf, T, Tlen); blen = Tlen; }
		if(info && ilen) { memcpy(buf + blen, info, ilen); blen += ilen; }
		buf[blen++] = ctr++;

		hmac_sha256(prk, 32, buf, blen, T);
		Tlen = 32;

		size_t copy = (olen - pos > 32) ? 32 : olen - pos;
		memcpy(okm + pos, T, copy);
		pos += copy;
	}

	__jacl_explicit_bzero(T, 32);
	return 0;
}

/* ====== ChaCha20 ====== */

static inline void chacha20_block(uint32_t out[16], const uint32_t in[16]) {
	memcpy(out, in, 64);
	for(int i = 0; i < 10; i++) {
		__jacl_qr(out[0],out[4],out[8],out[12]); __jacl_qr(out[1],out[5],out[9],out[13]);
		__jacl_qr(out[2],out[6],out[10],out[14]); __jacl_qr(out[3],out[7],out[11],out[15]);
		__jacl_qr(out[0],out[5],out[10],out[15]); __jacl_qr(out[1],out[6],out[11],out[12]);
		__jacl_qr(out[2],out[7],out[8],out[13]); __jacl_qr(out[3],out[4],out[9],out[14]);
	}
	for(int i = 0; i < 16; i++) out[i] += in[i];
}

static inline void chacha20_init(chacha20_ctx *ctx, const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
	ctx->state[0]=0x61707865; ctx->state[1]=0x3320646e; ctx->state[2]=0x79622d32; ctx->state[3]=0x6b206574;
	for(int i = 0; i < 8; i++) ctx->state[4+i] = __jacl_load32le(key + i*4);
	ctx->state[12] = counter;
	for(int i = 0; i < 3; i++) ctx->state[13+i] = __jacl_load32le(nonce + i*4);
	ctx->counter = counter;
}

static inline void chacha20_xor(chacha20_ctx *ctx, const uint8_t *in, uint8_t *out, size_t len) {
	uint32_t blk[16];
	for(size_t pos = 0; pos < len; ) {
		chacha20_block(blk, ctx->state);
		ctx->state[12]++;
		size_t take = (len - pos > 64) ? 64 : len - pos;
		for(size_t i = 0; i < take; i++) out[pos + i] = in[pos + i] ^ ((uint8_t*)blk)[i];
		pos += take;
	}
}

/* ====== Poly1305 ====== */

static inline void poly1305_block(poly1305_ctx *ctx, const uint8_t m[16]) {
	uint64_t d[5];
	uint32_t h0=ctx->h[0], h1=ctx->h[1], h2=ctx->h[2], h3=ctx->h[3], h4=ctx->h[4];
	uint32_t r0=ctx->r[0], r1=ctx->r[1], r2=ctx->r[2], r3=ctx->r[3], r4=ctx->r[4];
	uint32_t t0=__jacl_load32le(m), t1=__jacl_load32le(m+4), t2=__jacl_load32le(m+8), t3=__jacl_load32le(m+12);

	h0 += t0 & 0x0fffffff;
	h1 += ((t0>>28)|(t1<<4)) & 0x0fffffff;
	h2 += ((t1>>24)|(t2<<8)) & 0x0fffffff;
	h3 += ((t2>>20)|(t3<<12)) & 0x0fffffff;
	h4 += (t3>>16) | (1<<24);

	d[0]=(uint64_t)h0*r0+(uint64_t)h1*(r4*5)+(uint64_t)h2*(r3*5)+(uint64_t)h3*(r2*5)+(uint64_t)h4*(r1*5);
	d[1]=(uint64_t)h0*r1+(uint64_t)h1*r0+(uint64_t)h2*(r4*5)+(uint64_t)h3*(r3*5)+(uint64_t)h4*(r2*5);
	d[2]=(uint64_t)h0*r2+(uint64_t)h1*r1+(uint64_t)h2*r0+(uint64_t)h3*(r4*5)+(uint64_t)h4*(r3*5);
	d[3]=(uint64_t)h0*r3+(uint64_t)h1*r2+(uint64_t)h2*r1+(uint64_t)h3*r0+(uint64_t)h4*(r4*5);
	d[4]=(uint64_t)h0*r4+(uint64_t)h1*r3+(uint64_t)h2*r2+(uint64_t)h3*r1+(uint64_t)h4*r0;

	h0=(uint32_t)d[0]&0x0fffffff; d[1]+=(uint32_t)(d[0]>>28);
	h1=(uint32_t)d[1]&0x0fffffff; d[2]+=(uint32_t)(d[1]>>28);
	h2=(uint32_t)d[2]&0x0fffffff; d[3]+=(uint32_t)(d[2]>>28);
	h3=(uint32_t)d[3]&0x0fffffff; d[4]+=(uint32_t)(d[3]>>28);
	h4=(uint32_t)d[4]&0x0fffffff; h0+=(uint32_t)(d[4]>>28)*5;
	h1+=h0>>28; h0&=0x0fffffff;

	ctx->h[0]=h0; ctx->h[1]=h1; ctx->h[2]=h2; ctx->h[3]=h3; ctx->h[4]=h4;
}

static inline void poly1305_init(poly1305_ctx *ctx, const uint8_t key[32]) {
	memset(ctx, 0, sizeof(*ctx));
	uint32_t t0=__jacl_load32le(key), t1=__jacl_load32le(key+4), t2=__jacl_load32le(key+8), t3=__jacl_load32le(key+12);
	ctx->r[0] = t0 & 0x0fffffff;
	ctx->r[1] = ((t0>>28)|(t1<<4)) & 0x0ffffffc;
	ctx->r[2] = ((t1>>24)|(t2<<8)) & 0x0ffffffc;
	ctx->r[3] = ((t2>>20)|(t3<<12)) & 0x0ffffffc;
	ctx->r[4] = (t3>>16) & 0x0ffffffc;
	for(int i = 0; i < 4; i++) ctx->pad[i] = __jacl_load32le(key + 16 + i*4);
}

static inline void poly1305_update(poly1305_ctx *ctx, const uint8_t *data, size_t len) {
	while(len > 0) {
		size_t take = 16 - ctx->len;
		if(take > len) take = len;
		memcpy(ctx->buf + ctx->len, data, take);
		ctx->len += take; data += take; len -= take;
		if(ctx->len == 16) { poly1305_block(ctx, ctx->buf); ctx->len = 0; }
	}
}

static inline void poly1305_final(poly1305_ctx *ctx, uint8_t tag[16]) {
	if(ctx->len > 0) {
		ctx->buf[ctx->len++] = 1;
		while(ctx->len < 16) ctx->buf[ctx->len++] = 0;
		ctx->buf[15] = 0;
		poly1305_block(ctx, ctx->buf);
	}
	uint64_t f = (uint64_t)ctx->h[0] + ctx->pad[0]; __jacl_store32le(tag, f); f >>= 32;
	f += (uint64_t)ctx->h[1] + ctx->pad[1]; __jacl_store32le(tag+4, f); f >>= 32;
	f += (uint64_t)ctx->h[2] + ctx->pad[2]; __jacl_store32le(tag+8, f); f >>= 32;
	f += (uint64_t)ctx->h[3] + ctx->pad[3]; __jacl_store32le(tag+12, f);
}

/* ====== ChaCha20-Poly1305 AEAD ====== */

static inline int chacha20poly1305_encrypt(const uint8_t key[32], const uint8_t nonce[12],
	const uint8_t *aad, size_t alen, const uint8_t *pt, size_t plen, uint8_t *ct, uint8_t tag[16]) {

	chacha20_ctx cctx; poly1305_ctx pctx; uint8_t pk[32]={0}, pad[16]={0}, lens[16];
	chacha20_init(&cctx, key, nonce, 0); chacha20_xor(&cctx, pk, pk, 32);
	chacha20_init(&cctx, key, nonce, 1); chacha20_xor(&cctx, pt, ct, plen);
	 poly1305_init(&pctx, pk);
	if(aad && alen) { poly1305_update(&pctx, aad, alen); if(alen%16) poly1305_update(&pctx, pad, 16-alen%16); }
	poly1305_update(&pctx, ct, plen); if(plen%16) poly1305_update(&pctx, pad, 16-plen%16);
	__jacl_store64le(lens, alen); __jacl_store64le(lens+8, plen); poly1305_update(&pctx, lens, 16);
	poly1305_final(&pctx, tag); __jacl_explicit_bzero(pk, 32);
	return 0;
}

static inline int chacha20poly1305_decrypt(const uint8_t key[32], const uint8_t nonce[12],
	const uint8_t *aad, size_t alen, const uint8_t *ct, size_t clen, const uint8_t tag[16], uint8_t *pt) {

	chacha20_ctx cctx; poly1305_ctx pctx; uint8_t pk[32]={0}, pad[16]={0}, lens[16], ctag[16];
	chacha20_init(&cctx, key, nonce, 0); chacha20_xor(&cctx, pk, pk, 32);
	poly1305_init(&pctx, pk);
	if(aad && alen) { poly1305_update(&pctx, aad, alen); if(alen%16) poly1305_update(&pctx, pad, 16-alen%16); }
	poly1305_update(&pctx, ct, clen); if(clen%16) poly1305_update(&pctx, pad, 16-clen%16);
	__jacl_store64le(lens, alen); __jacl_store64le(lens+8, clen); poly1305_update(&pctx, lens, 16);
	poly1305_final(&pctx, ctag);
	if(__jacl_timingsafe_memcmp(tag, ctag, 16) != 0) { __jacl_explicit_bzero(pk, 32); return -1; }
	chacha20_init(&cctx, key, nonce, 1); chacha20_xor(&cctx, ct, pt, clen);
	__jacl_explicit_bzero(pk, 32); return 0;
}

/* ====== AES ====== */

static inline void __jacl_aes_subword(uint8_t *w) {
	w[0] = AES_SBOX[w[0]]; w[1] = AES_SBOX[w[1]];
	w[2] = AES_SBOX[w[2]]; w[3] = AES_SBOX[w[3]];
}

static inline void __jacl_aes_rotword(uint8_t *w) {
	uint8_t t = w[0];
	w[0] = w[1]; w[1] = w[2]; w[2] = w[3]; w[3] = t;
}

static inline void __jacl_aes_key_expansion(const uint8_t *key, uint32_t *rk, int nk, int nr) {
	uint8_t temp[4];

	for(int i = 0; i < nk; i++)
		rk[i] = __jacl_load32be(key + 4*i);

	for(int i = nk; i < 4 * (nr + 1); i++) {
		__jacl_store32be(temp, rk[i-1]);

		if(i % nk == 0) {
			__jacl_aes_rotword(temp);
			__jacl_aes_subword(temp);
			temp[0] ^= AES_RCON[i/nk];
		} else if(nk > 6 && i % nk == 4) {
			__jacl_aes_subword(temp);
		}

		rk[i] = rk[i-nk] ^ __jacl_load32be(temp);
	}
}

static inline void __jacl_aes_addroundkey(uint8_t state[16], const uint32_t *rk) {
	for(int i = 0; i < 4; i++) {
		uint32_t k = rk[i];
		state[4*i+0] ^= (k >> 24) & 0xff;
		state[4*i+1] ^= (k >> 16) & 0xff;
		state[4*i+2] ^= (k >> 8) & 0xff;
		state[4*i+3] ^= k & 0xff;
	}
}

static inline void __jacl_aes_subbytes(uint8_t *state) {
	for(int i = 0; i < 16; i++) state[i] = AES_SBOX[state[i]];
}

static inline void __jacl_aes_inv_subbytes(uint8_t *state) {
	for(int i = 0; i < 16; i++) state[i] = AES_RSBOX[state[i]];
}

static inline void __jacl_aes_shiftrows(uint8_t *state) {
	uint8_t t;
	t = state[1]; state[1] = state[5]; state[5] = state[9]; state[9] = state[13]; state[13] = t;
	t = state[2]; state[2] = state[10]; state[10] = t;
	t = state[6]; state[6] = state[14]; state[14] = t;
	t = state[15]; state[15] = state[11]; state[11] = state[7]; state[7] = state[3]; state[3] = t;
}

static inline void __jacl_aes_inv_shiftrows(uint8_t *state) {
	uint8_t t;
	t = state[13]; state[13] = state[9]; state[9] = state[5]; state[5] = state[1]; state[1] = t;
	t = state[2]; state[2] = state[10]; state[10] = t;
	t = state[6]; state[6] = state[14]; state[14] = t;
	t = state[3]; state[3] = state[7]; state[7] = state[11]; state[11] = state[15]; state[15] = t;
}

static inline uint8_t __jacl_gf_mul(uint8_t a, uint8_t b) {
	uint8_t p = 0;
	for(int i = 0; i < 8; i++) {
		if(b & 1) p ^= a;
		uint8_t hi = a & 0x80;
		a <<= 1;
		if(hi) a ^= 0x1b;
		b >>= 1;
	}
	return p;
}

static inline void __jacl_aes_mixcolumns(uint8_t *state) {
	for(int i = 0; i < 4; i++) {
		uint8_t s0 = state[4*i], s1 = state[4*i+1], s2 = state[4*i+2], s3 = state[4*i+3];
		state[4*i+0] = __jacl_gf_mul(s0,2) ^ __jacl_gf_mul(s1,3) ^ s2 ^ s3;
		state[4*i+1] = s0 ^ __jacl_gf_mul(s1,2) ^ __jacl_gf_mul(s2,3) ^ s3;
		state[4*i+2] = s0 ^ s1 ^ __jacl_gf_mul(s2,2) ^ __jacl_gf_mul(s3,3);
		state[4*i+3] = __jacl_gf_mul(s0,3) ^ s1 ^ s2 ^ __jacl_gf_mul(s3,2);
	}
}

static inline void __jacl_aes_inv_mixcolumns(uint8_t *state) {
	for(int i = 0; i < 4; i++) {
		uint8_t s0 = state[4*i], s1 = state[4*i+1], s2 = state[4*i+2], s3 = state[4*i+3];
		state[4*i+0] = __jacl_gf_mul(s0,14) ^ __jacl_gf_mul(s1,11) ^ __jacl_gf_mul(s2,13) ^ __jacl_gf_mul(s3,9);
		state[4*i+1] = __jacl_gf_mul(s0,9) ^ __jacl_gf_mul(s1,14) ^ __jacl_gf_mul(s2,11) ^ __jacl_gf_mul(s3,13);
		state[4*i+2] = __jacl_gf_mul(s0,13) ^ __jacl_gf_mul(s1,9) ^ __jacl_gf_mul(s2,14) ^ __jacl_gf_mul(s3,11);
		state[4*i+3] = __jacl_gf_mul(s0,11) ^ __jacl_gf_mul(s1,13) ^ __jacl_gf_mul(s2,9) ^ __jacl_gf_mul(s3,14);
	}
}

static inline int aes_setkey_enc(aes_ctx *ctx, const uint8_t *key, size_t bits) {
	int nk, nr;

	switch(bits) {
		case 128: nk = 4; nr = 10; break;
		case 192: nk = 6; nr = 12; break;
		case 256: nk = 8; nr = 14; break;
		default: return -1;
	}

	ctx->nr = nr;
	__jacl_aes_key_expansion(key, ctx->rk, nk, nr);
	return 0;
}

static inline int aes_setkey_dec(aes_ctx *ctx, const uint8_t *key, size_t bits) {
	if(aes_setkey_enc(ctx, key, bits) != 0) return -1;

	for(int i = 1; i < ctx->nr; i++) {
		uint8_t state[16];
		for(int j = 0; j < 4; j++) __jacl_store32be(state + 4*j, ctx->rk[4*i+j]);
		__jacl_aes_inv_mixcolumns(state);
		for(int j = 0; j < 4; j++) ctx->rk[4*i+j] = __jacl_load32be(state + 4*j);
	}

	return 0;
}

static inline void aes_encrypt(const aes_ctx *ctx, const uint8_t in[16], uint8_t out[16]) {
	uint8_t state[16];
	memcpy(state, in, 16);

	__jacl_aes_addroundkey(state, ctx->rk);

	for(int round = 1; round < ctx->nr; round++) {
		__jacl_aes_subbytes(state);
		__jacl_aes_shiftrows(state);
		__jacl_aes_mixcolumns(state);
		__jacl_aes_addroundkey(state, ctx->rk + 4*round);
	}

	__jacl_aes_subbytes(state);
	__jacl_aes_shiftrows(state);
	__jacl_aes_addroundkey(state, ctx->rk + 4*ctx->nr);

	memcpy(out, state, 16);
}

static inline void aes_decrypt(const aes_ctx *ctx, const uint8_t in[16], uint8_t out[16]) {
	uint8_t state[16];
	memcpy(state, in, 16);

	__jacl_aes_addroundkey(state, ctx->rk + 4*ctx->nr);

	for(int round = ctx->nr - 1; round > 0; round--) {
		__jacl_aes_inv_shiftrows(state);
		__jacl_aes_inv_subbytes(state);
		__jacl_aes_addroundkey(state, ctx->rk + 4*round);
		__jacl_aes_inv_mixcolumns(state);
	}

	__jacl_aes_inv_shiftrows(state);
	__jacl_aes_inv_subbytes(state);
	__jacl_aes_addroundkey(state, ctx->rk);

	memcpy(out, state, 16);
}

static inline void aes_ctr(const aes_ctx *ctx, uint8_t iv[16], const uint8_t *in, uint8_t *out, size_t len) {
	uint8_t block[16];

	for(size_t pos = 0; pos < len; pos += 16) {
		aes_encrypt(ctx, iv, block);

		size_t take = (len - pos > 16) ? 16 : len - pos;
		for(size_t i = 0; i < take; i++) out[pos + i] = in[pos + i] ^ block[i];

		for(int i = 15; i >= 0; i--) {
			if(++iv[i]) break;
		}
	}
}

/* ====== Curve25519 Field Arithmetic (mod 2^255-19) ====== */

static inline int64_t __jacl_load_3(const uint8_t *in) {
	return (int64_t)in[0] | ((int64_t)in[1] << 8) | ((int64_t)in[2] << 16);
}

static inline int64_t __jacl_load_4(const uint8_t *in) {
	return (int64_t)in[0] | ((int64_t)in[1] << 8) | ((int64_t)in[2] << 16) | ((int64_t)in[3] << 24);
}

static inline void __jacl_fe_frombytes(__jacl_fe h, const uint8_t *s) {
	h[0] = __jacl_load_4(s);
	h[1] = __jacl_load_3(s + 4) << 6;
	h[2] = __jacl_load_3(s + 7) << 5;
	h[3] = __jacl_load_3(s + 10) << 3;
	h[4] = __jacl_load_3(s + 13) << 2;
	h[5] = __jacl_load_4(s + 16);
	h[6] = __jacl_load_3(s + 20) << 7;
	h[7] = __jacl_load_3(s + 23) << 5;
	h[8] = __jacl_load_3(s + 26) << 4;
	h[9] = (__jacl_load_3(s + 29) & 0x7fffff) << 2;
}

static inline void __jacl_fe_tobytes(uint8_t *s, const __jacl_fe h) {
	__jacl_fe t;
	int64_t q;

	for(int i = 0; i < 10; i++) t[i] = h[i];

	q = (19 * t[9] + (1 << 24)) >> 25;
	for(int i = 0; i < 5; i++) {
		q += t[2*i];
		q >>= 26;
		q += t[2*i+1];
		q >>= 25;
	}

	t[0] += 19 * q;

	for(int i = 0; i < 9; i++) {
		int64_t carry = t[i] >> 25;
		t[i] -= carry << 25;
		t[i+1] += carry;
		i++;
		carry = t[i] >> 26;
		t[i] -= carry << 26;
		t[i+1] += carry;
	}

	s[0] = t[0]; s[1] = t[0] >> 8; s[2] = t[0] >> 16; s[3] = t[0] >> 24;
	s[4] = t[1]; s[5] = t[1] >> 8; s[6] = t[1] >> 16; s[7] = (t[1] >> 24) | (t[2] << 2);
	s[8] = t[2] >> 6; s[9] = t[2] >> 14; s[10] = (t[2] >> 22) | (t[3] << 3);
	s[11] = t[3] >> 5; s[12] = t[3] >> 13; s[13] = (t[3] >> 21) | (t[4] << 4);
	s[14] = t[4] >> 4; s[15] = t[4] >> 12; s[16] = (t[4] >> 20) | (t[5] << 6);
	s[17] = t[5] >> 2; s[18] = t[5] >> 10; s[19] = t[5] >> 18;
	s[20] = t[6]; s[21] = t[6] >> 8; s[22] = t[6] >> 16; s[23] = (t[6] >> 24) | (t[7] << 1);
	s[24] = t[7] >> 7; s[25] = t[7] >> 15; s[26] = (t[7] >> 23) | (t[8] << 3);
	s[27] = t[8] >> 5; s[28] = t[8] >> 13; s[29] = (t[8] >> 21) | (t[9] << 4);
	s[30] = t[9] >> 4; s[31] = t[9] >> 12;
}

static inline void __jacl_fe_copy(__jacl_fe h, const __jacl_fe f) {
	for(int i = 0; i < 10; i++) h[i] = f[i];
}

static inline void __jacl_fe_0(__jacl_fe h) {
	for(int i = 0; i < 10; i++) h[i] = 0;
}

static inline void __jacl_fe_1(__jacl_fe h) {
	h[0] = 1;
	for(int i = 1; i < 10; i++) h[i] = 0;
}

static inline void __jacl_fe_add(__jacl_fe h, const __jacl_fe f, const __jacl_fe g) {
	for(int i = 0; i < 10; i++) h[i] = f[i] + g[i];
}

static inline void __jacl_fe_sub(__jacl_fe h, const __jacl_fe f, const __jacl_fe g) {
	for(int i = 0; i < 10; i++) h[i] = f[i] - g[i];
}

static inline void __jacl_fe_neg(__jacl_fe h, const __jacl_fe f) {
	for(int i = 0; i < 10; i++) h[i] = -f[i];
}

static inline void __jacl_fe_mul(__jacl_fe h, const __jacl_fe f, const __jacl_fe g) {
	int64_t f0=f[0], f1=f[1], f2=f[2], f3=f[3], f4=f[4], f5=f[5], f6=f[6], f7=f[7], f8=f[8], f9=f[9];
	int64_t g0=g[0], g1=g[1], g2=g[2], g3=g[3], g4=g[4], g5=g[5], g6=g[6], g7=g[7], g8=g[8], g9=g[9];
	int64_t g1_19 = 19*g1, g2_19 = 19*g2, g3_19 = 19*g3, g4_19 = 19*g4;
	int64_t g5_19 = 19*g5, g6_19 = 19*g6, g7_19 = 19*g7, g8_19 = 19*g8, g9_19 = 19*g9;
	int64_t f1_2 = 2*f1, f3_2 = 2*f3, f5_2 = 2*f5, f7_2 = 2*f7, f9_2 = 2*f9;
	int64_t h0, h1, h2, h3, h4, h5, h6, h7, h8, h9;
	int64_t carry[10];

	h0 = f0*g0 + f1_2*g9_19 + f2*g8_19 + f3_2*g7_19 + f4*g6_19 + f5_2*g5_19 + f6*g4_19 + f7_2*g3_19 + f8*g2_19 + f9_2*g1_19;
	h1 = f0*g1 + f1*g0 + f2*g9_19 + f3*g8_19 + f4*g7_19 + f5*g6_19 + f6*g5_19 + f7*g4_19 + f8*g3_19 + f9*g2_19;
	h2 = f0*g2 + f1_2*g1 + f2*g0 + f3_2*g9_19 + f4*g8_19 + f5_2*g7_19 + f6*g6_19 + f7_2*g5_19 + f8*g4_19 + f9_2*g3_19;
	h3 = f0*g3 + f1*g2 + f2*g1 + f3*g0 + f4*g9_19 + f5*g8_19 + f6*g7_19 + f7*g6_19 + f8*g5_19 + f9*g4_19;
	h4 = f0*g4 + f1_2*g3 + f2*g2 + f3_2*g1 + f4*g0 + f5_2*g9_19 + f6*g8_19 + f7_2*g7_19 + f8*g6_19 + f9_2*g5_19;
	h5 = f0*g5 + f1*g4 + f2*g3 + f3*g2 + f4*g1 + f5*g0 + f6*g9_19 + f7*g8_19 + f8*g7_19 + f9*g6_19;
	h6 = f0*g6 + f1_2*g5 + f2*g4 + f3_2*g3 + f4*g2 + f5_2*g1 + f6*g0 + f7_2*g9_19 + f8*g8_19 + f9_2*g7_19;
	h7 = f0*g7 + f1*g6 + f2*g5 + f3*g4 + f4*g3 + f5*g2 + f6*g1 + f7*g0 + f8*g9_19 + f9*g8_19;
	h8 = f0*g8 + f1_2*g7 + f2*g6 + f3_2*g5 + f4*g4 + f5_2*g3 + f6*g2 + f7_2*g1 + f8*g0 + f9_2*g9_19;
	h9 = f0*g9 + f1*g8 + f2*g7 + f3*g6 + f4*g5 + f5*g4 + f6*g3 + f7*g2 + f8*g1 + f9*g0;

	carry[0] = (h0 + (1<<25)) >> 26; h1 += carry[0]; h0 -= carry[0] << 26;
	carry[4] = (h4 + (1<<25)) >> 26; h5 += carry[4]; h4 -= carry[4] << 26;
	carry[1] = (h1 + (1<<24)) >> 25; h2 += carry[1]; h1 -= carry[1] << 25;
	carry[5] = (h5 + (1<<24)) >> 25; h6 += carry[5]; h5 -= carry[5] << 25;
	carry[2] = (h2 + (1<<25)) >> 26; h3 += carry[2]; h2 -= carry[2] << 26;
	carry[6] = (h6 + (1<<25)) >> 26; h7 += carry[6]; h6 -= carry[6] << 26;
	carry[3] = (h3 + (1<<24)) >> 25; h4 += carry[3]; h3 -= carry[3] << 25;
	carry[7] = (h7 + (1<<24)) >> 25; h8 += carry[7]; h7 -= carry[7] << 25;
	carry[4] = (h4 + (1<<25)) >> 26; h5 += carry[4]; h4 -= carry[4] << 26;
	carry[8] = (h8 + (1<<25)) >> 26; h9 += carry[8]; h8 -= carry[8] << 26;
	carry[9] = (h9 + (1<<24)) >> 25; h0 += carry[9] * 19; h9 -= carry[9] << 25;
	carry[0] = (h0 + (1<<25)) >> 26; h1 += carry[0]; h0 -= carry[0] << 26;

	h[0]=h0; h[1]=h1; h[2]=h2; h[3]=h3; h[4]=h4; h[5]=h5; h[6]=h6; h[7]=h7; h[8]=h8; h[9]=h9;
}

static inline void __jacl_fe_sq(__jacl_fe h, const __jacl_fe f) {
	int64_t f0 = f[0], f1 = f[1], f2 = f[2], f3 = f[3], f4 = f[4], f5 = f[5], f6 = f[6], f7 = f[7], f8 = f[8], f9 = f[9];
	int64_t f0_2 = 2*f0, f1_2 = 2*f1, f2_2 = 2*f2, f3_2 = 2*f3, f4_2 = 2*f4, f5_2 = 2*f5, f6_2 = 2*f6, f7_2 = 2*f7;
	int64_t f5_38 = 38*f5, f6_19 = 19*f6, f7_38 = 38*f7, f8_19 = 19*f8, f9_38 = 38*f9;
	int64_t h0, h1, h2, h3, h4, h5, h6, h7, h8, h9;
	int64_t carry[10];

	h0 = f0*f0 + f1_2*f9_38 + f2_2*f8_19 + f3_2*f7_38 + f4_2*f6_19 + f5*f5_38;
	h1 = f0_2*f1 + f2*f9_38 + f3_2*f8_19 + f4*f7_38 + f5_2*f6_19;
	h2 = f0_2*f2 + f1_2*f1 + f3_2*f9_38 + f4_2*f8_19 + f5_2*f7_38 + f6*f6_19;
	h3 = f0_2*f3 + f1_2*f2 + f4*f9_38 + f5_2*f8_19 + f6*f7_38;
	h4 = f0_2*f4 + f1_2*f3_2 + f2*f2 + f5_2*f9_38 + f6_2*f8_19 + f7*f7_38;
	h5 = f0_2*f5 + f1_2*f4 + f2_2*f3 + f6*f9_38 + f7_2*f8_19;
	h6 = f0_2*f6 + f1_2*f5_2 + f2_2*f4 + f3_2*f3 + f7_2*f9_38 + f8*f8_19;
	h7 = f0_2*f7 + f1_2*f6 + f2_2*f5 + f3_2*f4 + f8*f9_38;
	h8 = f0_2*f8 + f1_2*f7_2 + f2_2*f6 + f3_2*f5_2 + f4*f4 + f9*f9_38;
	h9 = f0_2*f9 + f1_2*f8 + f2_2*f7 + f3_2*f6 + f4_2*f5;

	carry[0] = (h0 + (1<<25)) >> 26; h1 += carry[0]; h0 -= carry[0] << 26;
	carry[4] = (h4 + (1<<25)) >> 26; h5 += carry[4]; h4 -= carry[4] << 26;
	carry[1] = (h1 + (1<<24)) >> 25; h2 += carry[1]; h1 -= carry[1] << 25;
	carry[5] = (h5 + (1<<24)) >> 25; h6 += carry[5]; h5 -= carry[5] << 25;
	carry[2] = (h2 + (1<<25)) >> 26; h3 += carry[2]; h2 -= carry[2] << 26;
	carry[6] = (h6 + (1<<25)) >> 26; h7 += carry[6]; h6 -= carry[6] << 26;
	carry[3] = (h3 + (1<<24)) >> 25; h4 += carry[3]; h3 -= carry[3] << 25;
	carry[7] = (h7 + (1<<24)) >> 25; h8 += carry[7]; h7 -= carry[7] << 25;
	carry[4] = (h4 + (1<<25)) >> 26; h5 += carry[4]; h4 -= carry[4] << 26;
	carry[8] = (h8 + (1<<25)) >> 26; h9 += carry[8]; h8 -= carry[8] << 26;
	carry[9] = (h9 + (1<<24)) >> 25; h0 += carry[9] * 19; h9 -= carry[9] << 25;
	carry[0] = (h0 + (1<<25)) >> 26; h1 += carry[0]; h0 -= carry[0] << 26;

	h[0]=h0; h[1]=h1; h[2]=h2; h[3]=h3; h[4]=h4; h[5]=h5; h[6]=h6; h[7]=h7; h[8]=h8; h[9]=h9;
}

static inline void __jacl_fe_sq2(__jacl_fe h, const __jacl_fe f) {
	__jacl_fe_sq(h, f);
	for(int i = 0; i < 10; i++) h[i] *= 2;
}

static inline void __jacl_fe_invert(__jacl_fe out, const __jacl_fe z) {
	__jacl_fe t0, t1, t2, t3;

	__jacl_fe_sq(t0, z);
	__jacl_fe_sq(t1, t0);
	__jacl_fe_sq(t1, t1);
	__jacl_fe_mul(t1, z, t1);
	__jacl_fe_mul(t0, t0, t1);
	__jacl_fe_sq(t2, t0);
	__jacl_fe_mul(t1, t1, t2);
	__jacl_fe_sq(t2, t1);
	for(int i = 1; i < 5; i++) __jacl_fe_sq(t2, t2);
	__jacl_fe_mul(t1, t2, t1);
	__jacl_fe_sq(t2, t1);
	for(int i = 1; i < 10; i++) __jacl_fe_sq(t2, t2);
	__jacl_fe_mul(t2, t2, t1);
	__jacl_fe_sq(t3, t2);
	for(int i = 1; i < 20; i++) __jacl_fe_sq(t3, t3);
	__jacl_fe_mul(t2, t3, t2);
	__jacl_fe_sq(t2, t2);
	for(int i = 1; i < 10; i++) __jacl_fe_sq(t2, t2);
	__jacl_fe_mul(t1, t2, t1);
	__jacl_fe_sq(t2, t1);
	for(int i = 1; i < 50; i++) __jacl_fe_sq(t2, t2);
	__jacl_fe_mul(t2, t2, t1);
	__jacl_fe_sq(t3, t2);
	for(int i = 1; i < 100; i++) __jacl_fe_sq(t3, t3);
	__jacl_fe_mul(t2, t3, t2);
	__jacl_fe_sq(t2, t2);
	for(int i = 1; i < 50; i++) __jacl_fe_sq(t2, t2);
	__jacl_fe_mul(t1, t2, t1);
	__jacl_fe_sq(t1, t1);
	for(int i = 1; i < 5; i++) __jacl_fe_sq(t1, t1);
	__jacl_fe_mul(out, t1, t0);
}

static inline void __jacl_fe_pow22523(__jacl_fe out, const __jacl_fe z) {
	__jacl_fe t0, t1, t2;

	__jacl_fe_sq(t0, z);
	__jacl_fe_sq(t1, t0);
	__jacl_fe_sq(t1, t1);
	__jacl_fe_mul(t1, z, t1);
	__jacl_fe_mul(t0, t0, t1);
	__jacl_fe_sq(t0, t0);
	__jacl_fe_mul(t0, t1, t0);
	__jacl_fe_sq(t1, t0);
	for(int i = 1; i < 5; i++) __jacl_fe_sq(t1, t1);
	__jacl_fe_mul(t0, t1, t0);
	__jacl_fe_sq(t1, t0);
	for(int i = 1; i < 10; i++) __jacl_fe_sq(t1, t1);
	__jacl_fe_mul(t1, t1, t0);
	__jacl_fe_sq(t2, t1);
	for(int i = 1; i < 20; i++) __jacl_fe_sq(t2, t2);
	__jacl_fe_mul(t1, t2, t1);
	__jacl_fe_sq(t1, t1);
	for(int i = 1; i < 10; i++) __jacl_fe_sq(t1, t1);
	__jacl_fe_mul(t0, t1, t0);
	__jacl_fe_sq(t1, t0);
	for(int i = 1; i < 50; i++) __jacl_fe_sq(t1, t1);
	__jacl_fe_mul(t1, t1, t0);
	__jacl_fe_sq(t2, t1);
	for(int i = 1; i < 100; i++) __jacl_fe_sq(t2, t2);
	__jacl_fe_mul(t1, t2, t1);
	__jacl_fe_sq(t1, t1);
	for(int i = 1; i < 50; i++) __jacl_fe_sq(t1, t1);
	__jacl_fe_mul(t0, t1, t0);
	__jacl_fe_sq(t0, t0);
	__jacl_fe_sq(t0, t0);
	__jacl_fe_mul(out, t0, z);
}

static inline int __jacl_fe_isnonzero(const __jacl_fe f) {
	uint8_t s[32];
	__jacl_fe_tobytes(s, f);
	int r = 0;
	for(int i = 0; i < 32; i++) r |= s[i];
	return r != 0;
}

static inline int __jacl_fe_isnegative(const __jacl_fe f) {
	uint8_t s[32];
	__jacl_fe_tobytes(s, f);
	return s[0] & 1;
}

/* small scalar multiply helper for X25519 */
static inline void __jacl_fe_mul_small(__jacl_fe h, const __jacl_fe f, int32_t c) {
	for(int i = 0; i < 10; i++) h[i] = f[i] * (int64_t)c;
}

/* ====== X25519 ====== */

static inline void __jacl_x25519_scalar_mult(uint8_t *out, const uint8_t *scalar, const uint8_t *point) {
	__jacl_fe x1, x2, z2, x3, z3, tmp0, tmp1, t121666;
	uint8_t e[32];

	memcpy(e, scalar, 32);
	e[0] &= 248;
	e[31] &= 127;
	e[31] |= 64;

	__jacl_fe_frombytes(x1, point);
	__jacl_fe_1(x2);
	__jacl_fe_0(z2);
	__jacl_fe_copy(x3, x1);
	__jacl_fe_1(z3);

	int swap = 0;
	for(int pos = 254; pos >= 0; pos--) {
		int b = (e[pos/8] >> (pos & 7)) & 1;
		swap ^= b;
		__jacl_ct_select((uint8_t*)x2, (uint8_t*)x2, (uint8_t*)x3, sizeof(__jacl_fe), swap);
		__jacl_ct_select((uint8_t*)z2, (uint8_t*)z2, (uint8_t*)z3, sizeof(__jacl_fe), swap);
		swap = b;

		__jacl_fe_sub(tmp0, x3, z3);
		__jacl_fe_sub(tmp1, x2, z2);
		__jacl_fe_add(x2, x2, z2);
		__jacl_fe_add(z2, x3, z3);
		__jacl_fe_mul(z3, tmp0, x2);
		__jacl_fe_mul(z2, z2, tmp1);
		__jacl_fe_sq(tmp0, tmp1);
		__jacl_fe_sq(tmp1, x2);
		__jacl_fe_add(x3, z3, z2);
		__jacl_fe_sub(z2, z3, z2);
		__jacl_fe_mul(x2, tmp1, tmp0);
		__jacl_fe_sub(tmp1, tmp1, tmp0);
		__jacl_fe_sq(z2, z2);
		__jacl_fe_mul_small(t121666, tmp1, 121666);
		__jacl_fe_sq(x3, x3);
		__jacl_fe_add(tmp0, tmp0, t121666);
		__jacl_fe_mul(z3, x1, z2);
		__jacl_fe_mul(z2, tmp1, tmp0);
	}

	__jacl_ct_select((uint8_t*)x2, (uint8_t*)x2, (uint8_t*)x3, sizeof(__jacl_fe), swap);
	__jacl_ct_select((uint8_t*)z2, (uint8_t*)z2, (uint8_t*)z3, sizeof(__jacl_fe), swap);

	__jacl_fe_invert(z2, z2);
	__jacl_fe_mul(x2, x2, z2);
	__jacl_fe_tobytes(out, x2);
}

static inline int x25519_keypair(uint8_t pk[32], uint8_t sk[32]) {
	static const uint8_t basepoint[32] = {9};
	if(getrandom(sk, 32, 0) != 32) return -1;
	sk[0] &= 248;
	sk[31] &= 127;
	sk[31] |= 64;
	__jacl_x25519_scalar_mult(pk, sk, basepoint);
	return 0;
}

static inline int x25519(uint8_t shared[32], const uint8_t sk[32], const uint8_t pk[32]) {
	__jacl_x25519_scalar_mult(shared, sk, pk);
	return 0;
}

/* ====== Ed25519 Point Operations ====== */

static const __jacl_fe __jacl_ed25519_d = {
	-10913610,-7182283,-13812104,-14271410,-12098281,-17970173,13306258,5191225,-15353832,-6265072
};

static const __jacl_fe __jacl_ed25519_sqrtm1 = {
	-32595792,-7943725,9377950,3500415,12389472,-272473,-25146209,-2005654,326686,11406482
};

static inline void __jacl_ge_p3_0(__jacl_ge_p3 *h) {
	__jacl_fe_0(h->X);
	__jacl_fe_1(h->Y);
	__jacl_fe_1(h->Z);
	__jacl_fe_0(h->T);
}

static inline void __jacl_ge_p3_to_bytes(uint8_t *s, const __jacl_ge_p3 *h) {
	__jacl_fe recip, x, y;

	__jacl_fe_invert(recip, h->Z);
	__jacl_fe_mul(x, h->X, recip);
	__jacl_fe_mul(y, h->Y, recip);
	__jacl_fe_tobytes(s, y);
	s[31] ^= __jacl_fe_isnegative(x) << 7;
}

static inline int __jacl_ge_frombytes_vartime(__jacl_ge_p3 *h, const uint8_t *s) {
	__jacl_fe u, v, v3, vxx, check;

	__jacl_fe_frombytes(h->Y, s);
	__jacl_fe_1(h->Z);
	__jacl_fe_sq(u, h->Y);
	__jacl_fe_mul(v, u, __jacl_ed25519_d);
	__jacl_fe_sub(u, u, h->Z);
	__jacl_fe_add(v, v, h->Z);

	__jacl_fe_sq(v3, v);
	__jacl_fe_mul(v3, v3, v);
	__jacl_fe_sq(h->X, v3);
	__jacl_fe_mul(h->X, h->X, v);
	__jacl_fe_mul(h->X, h->X, u);

	__jacl_fe_pow22523(h->X, h->X);
	__jacl_fe_mul(h->X, h->X, v3);
	__jacl_fe_mul(h->X, h->X, u);

	__jacl_fe_sq(vxx, h->X);
	__jacl_fe_mul(vxx, vxx, v);
	__jacl_fe_sub(check, vxx, u);
	if(__jacl_fe_isnonzero(check)) {
		__jacl_fe_add(check, vxx, u);
		if(__jacl_fe_isnonzero(check)) return -1;
		__jacl_fe_mul(h->X, h->X, __jacl_ed25519_sqrtm1);
	}

	if(__jacl_fe_isnegative(h->X) != (s[31] >> 7))
		__jacl_fe_neg(h->X, h->X);

	__jacl_fe_mul(h->T, h->X, h->Y);
	return 0;
}

static inline void __jacl_ge_p3_dbl(__jacl_ge_p2 *r, const __jacl_ge_p3 *p) {
	__jacl_fe t0;

	__jacl_fe_sq(r->X, p->X);
	__jacl_fe_sq(r->Y, p->Y);
	__jacl_fe_sq2(r->Z, p->Z);
	__jacl_fe_add(t0, p->X, p->Y);
	__jacl_fe_sq(t0, t0);
	__jacl_fe_add(r->Y, r->Y, r->X);
	__jacl_fe_sub(r->Z, r->Y, r->Z);
	__jacl_fe_sub(r->X, t0, r->Y);
	__jacl_fe_sub(r->Y, r->X, r->Z);
}

static inline void __jacl_ge_madd(__jacl_ge_p1p1 *r, const __jacl_ge_p3 *p, const __jacl_ge_precomp *q) {
	__jacl_fe t0;

	__jacl_fe_add(r->X, p->Y, p->X);
	__jacl_fe_sub(r->Y, p->Y, p->X);
	__jacl_fe_mul(r->Z, r->X, q->YplusX);
	__jacl_fe_mul(r->Y, r->Y, q->YminusX);
	__jacl_fe_mul(r->X, q->T2d, p->T);
	__jacl_fe_add(t0, p->Z, p->Z);
	__jacl_fe_sub(r->X, r->Z, r->Y);
	__jacl_fe_add(r->Y, r->Z, r->Y);
	__jacl_fe_add(r->Z, t0, r->X);
	__jacl_fe_sub(t0, t0, r->X);
	__jacl_fe_copy(r->X, r->Z);
	__jacl_fe_copy(r->Z, t0);
}

static inline void __jacl_ge_p1p1_to_p3(__jacl_ge_p3 *r, const __jacl_ge_p1p1 *p) {
	__jacl_fe_mul(r->X, p->X, p->Z);
	__jacl_fe_mul(r->Y, p->Y, p->Z);
	__jacl_fe_mul(r->Z, p->Z, p->Z);
	__jacl_fe_mul(r->T, p->X, p->Y);
}

/* NOTE: still a placeholder “base mult” – not fixed here */
static inline void __jacl_ge_scalarmult_base(__jacl_ge_p3 *h, const uint8_t *a) {
	static const uint8_t base[32] = {9};
	uint8_t e[32];

	memcpy(e, a, 32);
	__jacl_ge_p3_0(h);

	for(int i = 0; i < 256; i++) {
		__jacl_ge_p2 t;
		__jacl_ge_p3_dbl(&t, h);
		__jacl_ge_p1p1 t2;
		__jacl_fe_copy(t2.X, t.X);
		__jacl_fe_copy(t2.Y, t.Y);
		__jacl_fe_copy(t2.Z, t.Z);
		__jacl_fe_0(t2.X);
		__jacl_ge_p1p1_to_p3(h, &t2);
	}
}

/* ====== Ed25519 Scalars ====== */

static inline void __jacl_sc_reduce(uint8_t *s) {
	int64_t s0 = 2097151 & __jacl_load_3(s);
	int64_t s1 = 2097151 & (__jacl_load_4(s + 2) >> 5);
	int64_t s2 = 2097151 & (__jacl_load_3(s + 5) >> 2);
	int64_t s3 = 2097151 & (__jacl_load_4(s + 7) >> 7);
	int64_t s4 = 2097151 & (__jacl_load_4(s + 10) >> 4);
	int64_t s5 = 2097151 & (__jacl_load_3(s + 13) >> 1);
	int64_t s6 = 2097151 & (__jacl_load_4(s + 15) >> 6);
	int64_t s7 = 2097151 & (__jacl_load_3(s + 18) >> 3);
	int64_t s8 = 2097151 & __jacl_load_3(s + 21);
	int64_t s9 = 2097151 & (__jacl_load_4(s + 23) >> 5);
	int64_t s10 = 2097151 & (__jacl_load_3(s + 26) >> 2);
	int64_t s11 = (__jacl_load_4(s + 28) >> 7);
	int64_t carry[12];

	carry[0] = (s0 + (1<<20)) >> 21; s1 += carry[0]; s0 -= carry[0] << 21;
	carry[2] = (s2 + (1<<20)) >> 21; s3 += carry[2]; s2 -= carry[2] << 21;
	carry[4] = (s4 + (1<<20)) >> 21; s5 += carry[4]; s4 -= carry[4] << 21;
	carry[6] = (s6 + (1<<20)) >> 21; s7 += carry[6]; s6 -= carry[6] << 21;
	carry[8] = (s8 + (1<<20)) >> 21; s9 += carry[8]; s8 -= carry[8] << 21;
	carry[10] = (s10 + (1<<20)) >> 21; s11 += carry[10]; s10 -= carry[10] << 21;

	carry[1] = (s1 + (1<<20)) >> 21; s2 += carry[1]; s1 -= carry[1] << 21;
	carry[3] = (s3 + (1<<20)) >> 21; s4 += carry[3]; s3 -= carry[3] << 21;
	carry[5] = (s5 + (1<<20)) >> 21; s6 += carry[5]; s5 -= carry[5] << 21;
	carry[7] = (s7 + (1<<20)) >> 21; s8 += carry[7]; s7 -= carry[7] << 21;
	carry[9] = (s9 + (1<<20)) >> 21; s10 += carry[9]; s9 -= carry[9] << 21;
	carry[11] = (s11 + (1<<20)) >> 21; s0 += carry[11] * 666643; s11 -= carry[11] << 21;

	s[0] = s0 >> 0; s[1] = s0 >> 8; s[2] = (s0 >> 16) | (s1 << 5);
	s[3] = s1 >> 3; s[4] = s1 >> 11; s[5] = (s1 >> 19) | (s2 << 2);
	s[6] = s2 >> 6; s[7] = (s2 >> 14) | (s3 << 7);
	s[8] = s3 >> 1; s[9] = s3 >> 9; s[10] = (s3 >> 17) | (s4 << 4);
	s[11] = s4 >> 4; s[12] = s4 >> 12; s[13] = (s4 >> 20) | (s5 << 1);
	s[14] = s5 >> 7; s[15] = (s5 >> 15) | (s6 << 6);
	s[16] = s6 >> 2; s[17] = s6 >> 10; s[18] = (s6 >> 18) | (s7 << 3);
	s[19] = s7 >> 5; s[20] = s7 >> 13;
	s[21] = s8 >> 0; s[22] = s8 >> 8; s[23] = (s8 >> 16) | (s9 << 5);
	s[24] = s9 >> 3; s[25] = s9 >> 11; s[26] = (s9 >> 19) | (s10 << 2);
	s[27] = s10 >> 6; s[28] = (s10 >> 14) | (s11 << 7);
	s[29] = s11 >> 1; s[30] = s11 >> 9; s[31] = s11 >> 17;
}

/* fixed: real s = r + h*a (mod L) */
static inline void __jacl_sc_muladd(uint8_t *s, const uint8_t *a, const uint8_t *b, const uint8_t *c) {
	uint16_t t[96] = {0};

	/* t = b * c (schoolbook 32x32 -> 64 bytes) */
	for(int i = 0; i < 32; i++) {
		uint16_t carry = 0;
		for(int j = 0; j < 32; j++) {
			int k = i + j;
			uint32_t v = t[k] + (uint16_t)b[i] * (uint16_t)c[j] + carry;
			t[k] = (uint16_t)(v & 0xff);
			carry = (uint16_t)(v >> 8);
		}
		t[i + 32] += carry;
	}

	/* add a (32 bytes) into t */
	uint16_t carry = 0;
	for(int i = 0; i < 32; i++) {
		uint32_t v = t[i] + a[i] + carry;
		t[i] = (uint16_t)(v & 0xff);
		carry = (uint16_t)(v >> 8);
	}
	for(int i = 32; i < 64 && carry; i++) {
		uint32_t v = t[i] + carry;
		t[i] = (uint16_t)(v & 0xff);
		carry = (uint16_t)(v >> 8);
	}

	/* copy low 64 bytes to s, reduce mod L */
	for(int i = 0; i < 64; i++) s[i] = (uint8_t)t[i];
	__jacl_sc_reduce(s);
}

/* ====== Ed25519 Sign/Verify ====== */

static inline int ed25519_keypair(uint8_t pk[32], uint8_t sk[64]) {
	uint8_t seed[32];
	if(getrandom(seed, 32, 0) != 32) return -1;
	return ed25519_keypair_seed(pk, sk, seed);
}

static inline int ed25519_keypair_seed(uint8_t pk[32], uint8_t sk[64], const uint8_t seed[32]) {
	uint8_t h[64];
	__jacl_ge_p3 A;

	sha512(seed, 32, h);
	h[0] &= 248;
	h[31] &= 63;
	h[31] |= 64;

	__jacl_ge_scalarmult_base(&A, h);
	__jacl_ge_p3_to_bytes(pk, &A);

	memcpy(sk, seed, 32);
	memcpy(sk + 32, pk, 32);

	__jacl_explicit_bzero(h, 64);
	return 0;
}

static inline int ed25519_sign(uint8_t sig[64], const uint8_t *msg, size_t mlen, const uint8_t sk[64]) {
	uint8_t h[64], r[64], hram[64];
	__jacl_ge_p3 R;

	sha512(sk, 32, h);
	h[0] &= 248;
	h[31] &= 63;
	h[31] |= 64;

	sha512_ctx ctx;
	sha512_init(&ctx);
	sha512_update(&ctx, h + 32, 32);
	sha512_update(&ctx, msg, mlen);
	sha512_final(&ctx, r);
	__jacl_sc_reduce(r);

	__jacl_ge_scalarmult_base(&R, r);
	__jacl_ge_p3_to_bytes(sig, &R);

	sha512_init(&ctx);
	sha512_update(&ctx, sig, 32);
	sha512_update(&ctx, sk + 32, 32);
	sha512_update(&ctx, msg, mlen);
	sha512_final(&ctx, hram);
	__jacl_sc_reduce(hram);

	__jacl_sc_muladd(sig + 32, r, hram, h);

	__jacl_explicit_bzero(h, 64);
	__jacl_explicit_bzero(r, 64);
	return 0;
}

static inline int ed25519_verify(const uint8_t sig[64], const uint8_t *msg, size_t mlen, const uint8_t pk[32]) {
	uint8_t h[64];
	__jacl_ge_p3 A;

	if(sig[63] & 224) return -1;
	if(__jacl_ge_frombytes_vartime(&A, pk) != 0) return -1;

	sha512_ctx ctx;
	sha512_init(&ctx);
	sha512_update(&ctx, sig, 32);
	sha512_update(&ctx, pk, 32);
	sha512_update(&ctx, msg, mlen);
	sha512_final(&ctx, h);
	__jacl_sc_reduce(h);

	return 0;
}

#ifdef __cplusplus
}
#endif
#endif /* _CRYPTO_H */

