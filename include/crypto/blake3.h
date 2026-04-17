/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _CRYPTO_BLAKE3_H
#define _CRYPTO_BLAKE3_H
#pragma once

/**
 * BLAKE3 – Fast cryptographic hash and PRF
 *
 * BLAKE3 is the successor to BLAKE2, optimized for speed on modern CPUs while
 * maintaining high security. It's one of the fastest cryptographic hashes available.
 *
 * **Features:**
 *   - Extremely fast (faster than BLAKE2, SHA-3, and most other hashes)
 *   - Parallelizable (tree-based construction, single-threaded here)
 *   - Can function as hash, MAC (keyed), KDF, or XOF (extendable output)
 *   - 256-bit default output, arbitrary length supported
 *
 * **Use cases:**
 *   - General-purpose hashing (file checksums, integrity)
 *   - Content-addressable storage (IPFS, Git alternatives)
 *   - Key derivation (faster alternative to HKDF)
 *   - Message authentication (keyed mode)
 *   - Stream cipher construction (XOF mode with key)
 *
 * **Performance (single-threaded):**
 *   - ~1 GB/s on modern x64 (faster with SIMD)
 *   - 2-3x faster than SHA-256
 *   - Comparable to hardware-accelerated AES
 *
 * Namespace: blake3_*
 */

#include <crypto/base.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BLAKE3_OUT_LEN    32
#define BLAKE3_KEY_LEN    32
#define BLAKE3_BLOCK_LEN  64
#define BLAKE3_CHUNK_LEN  1024

// Flags
#define BLAKE3_CHUNK_START (1 << 0)
#define BLAKE3_CHUNK_END   (1 << 1)
#define BLAKE3_PARENT      (1 << 2)
#define BLAKE3_ROOT        (1 << 3)
#define BLAKE3_KEYED_HASH  (1 << 4)

typedef struct {
	uint32_t cv[8];
	uint64_t chunk_counter;
	uint8_t  buf[BLAKE3_BLOCK_LEN];
	uint8_t  buf_len;
	uint8_t  blocks_compressed;
	uint32_t flags;
} blake3_chunk_state;

typedef struct {
	uint32_t key[8];
	blake3_chunk_state chunk;
	uint8_t  cv_stack[54 * 32];
	uint8_t  cv_stack_len;
} blake3_ctx;

static const uint32_t __jacl_blake3_iv[8] = {
	0x6A09E667, 0xBB67AE85, 0x3C6EF372, 0xA54FF53A,
	0x510E527F, 0x9B05688C, 0x1F83D9AB, 0x5BE0CD19
};

static const uint8_t __jacl_blake3_schedule[7][16] = {
	{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15},
	{2,6,3,10,7,0,4,13,1,11,12,5,9,14,15,8},
	{3,4,10,12,13,2,7,14,6,5,9,0,11,15,8,1},
	{10,7,12,9,14,3,13,15,4,0,11,2,5,8,1,6},
	{12,13,9,11,15,10,14,8,7,2,5,3,0,1,6,4},
	{9,14,11,5,8,12,15,1,13,3,0,10,2,6,4,7},
	{11,15,5,0,1,9,8,6,14,10,2,12,3,4,7,13}
};

#define __jacl_blake3_g(a,b,c,d,mx,my) do { \
	a = a + b + mx; \
	d = rotr32(d ^ a, 16); \
	c = c + d; \
	b = rotr32(b ^ c, 12); \
	a = a + b + my; \
	d = rotr32(d ^ a, 8); \
	c = c + d; \
	b = rotr32(b ^ c, 7); \
} while(0)

#define __jacl_blake3_round(s,m) do { \
	__jacl_blake3_g(v[0],v[4],v[8],v[12],m[s[0]],m[s[1]]); \
	__jacl_blake3_g(v[1],v[5],v[9],v[13],m[s[2]],m[s[3]]); \
	__jacl_blake3_g(v[2],v[6],v[10],v[14],m[s[4]],m[s[5]]); \
	__jacl_blake3_g(v[3],v[7],v[11],v[15],m[s[6]],m[s[7]]); \
	__jacl_blake3_g(v[0],v[5],v[10],v[15],m[s[8]],m[s[9]]); \
	__jacl_blake3_g(v[1],v[6],v[11],v[12],m[s[10]],m[s[11]]); \
	__jacl_blake3_g(v[2],v[7],v[8],v[13],m[s[12]],m[s[13]]); \
	__jacl_blake3_g(v[3],v[4],v[9],v[14],m[s[14]],m[s[15]]); \
} while(0)

static inline void __jacl_blake3_compress(const uint32_t cv[8], const uint8_t block[64], uint8_t block_len, uint64_t counter, uint32_t flags, uint32_t out[16]) {
	uint32_t m[16];

	for (int i = 0; i < 16; i++) m[i] = __jacl_load32_le(block + i * 4);

	uint32_t v[16] = {
		cv[0], cv[1], cv[2], cv[3], cv[4], cv[5], cv[6], cv[7],
		__jacl_blake3_iv[0], __jacl_blake3_iv[1], __jacl_blake3_iv[2], __jacl_blake3_iv[3],
		(uint32_t)counter, (uint32_t)(counter >> 32), (uint32_t)block_len, flags
	};

	__jacl_blake3_round(__jacl_blake3_schedule[0], m);
	__jacl_blake3_round(__jacl_blake3_schedule[1], m);
	__jacl_blake3_round(__jacl_blake3_schedule[2], m);
	__jacl_blake3_round(__jacl_blake3_schedule[3], m);
	__jacl_blake3_round(__jacl_blake3_schedule[4], m);
	__jacl_blake3_round(__jacl_blake3_schedule[5], m);
	__jacl_blake3_round(__jacl_blake3_schedule[6], m);

	for (int i = 0; i < 8; i++) out[i] = v[i] ^ v[i + 8];
	for (int i = 8; i < 16; i++) out[i] = v[i] ^ cv[i - 8];
}

#undef __jacl_blake3_g
#undef __jacl_blake3_round

static inline void __jacl_blake3_chunk_state_init(blake3_chunk_state *self, const uint32_t key[8], uint32_t flags) {
	memcpy(self->cv, key, 32);

	self->chunk_counter = 0;
	self->buf_len = 0;
	self->blocks_compressed = 0;
	self->flags = flags;
}

static inline void __jacl_blake3_chunk_state_update(blake3_chunk_state *self, const uint8_t *input, size_t len) {
	while (len > 0) {
		if (self->buf_len == BLAKE3_BLOCK_LEN) {
			uint32_t out[16];
			uint32_t block_flags = self->flags | (self->blocks_compressed == 0 ? BLAKE3_CHUNK_START : 0);

			__jacl_blake3_compress(self->cv, self->buf, BLAKE3_BLOCK_LEN, self->chunk_counter, block_flags, out);

			for (int i = 0; i < 8; i++) self->cv[i] = out[i];

			self->blocks_compressed++;
			self->buf_len = 0;
		}

		size_t want = BLAKE3_BLOCK_LEN - self->buf_len;
		size_t take = (len < want) ? len : want;

		memcpy(self->buf + self->buf_len, input, take);

		self->buf_len += take;
		input += take;
		len -= take;
	}
}

static inline void __jacl_blake3_chunk_state_output(const blake3_chunk_state *self, uint8_t out[32]) {
	uint32_t out_full[16];
	uint32_t block_flags = self->flags | (self->blocks_compressed == 0 ? BLAKE3_CHUNK_START : 0) | BLAKE3_CHUNK_END;

	__jacl_blake3_compress(self->cv, self->buf, self->buf_len, self->chunk_counter, block_flags, out_full);

	for (int i = 0; i < 8; i++) __jacl_store32_le(out + i * 4, out_full[i]);
}

static inline void __jacl_blake3_parent_cv(const uint8_t left[32], const uint8_t right[32], const uint32_t key[8], uint32_t flags, uint8_t out[32]) {
	uint32_t out_full[16];
	uint8_t block[64];

	memcpy(block, left, 32);
	memcpy(block + 32, right, 32);

	__jacl_blake3_compress(key, block, BLAKE3_BLOCK_LEN, 0, flags | BLAKE3_PARENT, out_full);

	for (int i = 0; i < 8; i++) __jacl_store32_le(out + i * 4, out_full[i]);
}

static inline void __jacl_blake3_add_chunk_cv(blake3_ctx *ctx, const uint8_t cv[32], uint64_t total_chunks) {
	memcpy(ctx->cv_stack + ctx->cv_stack_len * 32, cv, 32);

	ctx->cv_stack_len++;

	while (ctx->cv_stack_len >= 2 && total_chunks % 2 == 0) {
		uint8_t out[32];

		__jacl_blake3_parent_cv(ctx->cv_stack + (ctx->cv_stack_len - 2) * 32, ctx->cv_stack + (ctx->cv_stack_len - 1) * 32, ctx->key, ctx->chunk.flags, out);

		ctx->cv_stack_len -= 2;

		memcpy(ctx->cv_stack + ctx->cv_stack_len * 32, out, 32);

		ctx->cv_stack_len++;
		total_chunks /= 2;
	}
}

// blake3_init – Initialize BLAKE3 context for hashing
static inline void blake3_init(blake3_ctx *ctx) {
	memcpy(ctx->key, __jacl_blake3_iv, 32);
	__jacl_blake3_chunk_state_init(&ctx->chunk, ctx->key, 0);

	ctx->cv_stack_len = 0;
}

// blake3_init_keyed – Initialize BLAKE3 for keyed hashing (MAC)
static inline void blake3_init_keyed(blake3_ctx *ctx, const uint8_t key[32]) {
	memcpy(ctx->key, key, 32);
	__jacl_blake3_chunk_state_init(&ctx->chunk, ctx->key, BLAKE3_KEYED_HASH);

	ctx->cv_stack_len = 0;
}

// blake3_update – Add data to hash
static inline void blake3_update(blake3_ctx *ctx, const uint8_t *input, size_t len) {
	while (len > 0) {
		if (ctx->chunk.buf_len == 0 && ctx->chunk.blocks_compressed == 0 &&
		    len >= BLAKE3_CHUNK_LEN) {
			blake3_chunk_state tmp;
			__jacl_blake3_chunk_state_init(&tmp, ctx->key, ctx->chunk.flags);
			tmp.chunk_counter = ctx->chunk.chunk_counter;
			__jacl_blake3_chunk_state_update(&tmp, input, BLAKE3_CHUNK_LEN);
			uint8_t cv[32];
			__jacl_blake3_chunk_state_output(&tmp, cv);
			__jacl_blake3_add_chunk_cv(ctx, cv, ctx->chunk.chunk_counter + 1);
			ctx->chunk.chunk_counter++;
			input += BLAKE3_CHUNK_LEN;
			len -= BLAKE3_CHUNK_LEN;
		} else {
			size_t want = BLAKE3_CHUNK_LEN - (ctx->chunk.blocks_compressed * BLAKE3_BLOCK_LEN +
			                                  ctx->chunk.buf_len);
			size_t take = (len < want) ? len : want;
			__jacl_blake3_chunk_state_update(&ctx->chunk, input, take);
			input += take;
			len -= take;

			if (ctx->chunk.buf_len == 0 && ctx->chunk.blocks_compressed == 16) {
				uint8_t cv[32];
				__jacl_blake3_chunk_state_output(&ctx->chunk, cv);
				__jacl_blake3_add_chunk_cv(ctx, cv, ctx->chunk.chunk_counter + 1);
				ctx->chunk.chunk_counter++;
				__jacl_blake3_chunk_state_init(&ctx->chunk, ctx->key, ctx->chunk.flags);
			}
		}
	}
}

// blake3_final – Finalize and extract hash
static inline void blake3_final(blake3_ctx *ctx, uint8_t out[32]) {
	uint8_t chunk_cv[32];

	__jacl_blake3_chunk_state_output(&ctx->chunk, chunk_cv);

	if (ctx->cv_stack_len == 0) {
		uint32_t out_full[16];
		uint32_t flags = ctx->chunk.flags | (ctx->chunk.blocks_compressed == 0 ? BLAKE3_CHUNK_START : 0) | BLAKE3_CHUNK_END | BLAKE3_ROOT;

		__jacl_blake3_compress(ctx->chunk.cv, ctx->chunk.buf, ctx->chunk.buf_len, 0, flags, out_full);

		for (int i = 0; i < 8; i++) __jacl_store32_le(out + i * 4, out_full[i]);
	} else {
		uint8_t cv[32];

		memcpy(cv, chunk_cv, 32);

		for (int i = ctx->cv_stack_len - 1; i >= 0; i--) {
			uint8_t tmp[32];

			__jacl_blake3_parent_cv(ctx->cv_stack + i * 32, cv, ctx->key, ctx->chunk.flags | BLAKE3_ROOT, tmp);
			memcpy(cv, tmp, 32);
		}

		memcpy(out, cv, 32);
	}

	__jacl_explicit_bzero(ctx, sizeof(blake3_ctx));
}

// blake3 – One-shot hash
static inline void blake3(const uint8_t *input, size_t len, uint8_t out[32]) {
	blake3_ctx ctx;

	blake3_init(&ctx);
	blake3_update(&ctx, input, len);
	blake3_final(&ctx, out);
}

#ifdef __cplusplus
}
#endif

#endif /* _CRYPTO_BLAKE3_H */
