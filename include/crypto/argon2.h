/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _CRYPTO_ARGON2_H
#define _CRYPTO_ARGON2_H
#pragma once

/**
 * Argon2 – Memory-Hard Password Hashing (RFC 9106)
 *
 * **RECOMMENDED for all new password storage and key derivation.**
 *
 * Winner of Password Hashing Competition (2015). Provides:
 *   - Memory-hard construction (resists GPU/ASIC/FPGA attacks)
 *   - Tunable memory, iteration count, and parallelism
 *   - Three variants: Argon2d, Argon2i, Argon2id (recommended)
 *
 * **Variant selection:**
 *   - **Argon2id (RECOMMENDED):** Hybrid, resistant to both attacks
 *   - **Argon2i:** Data-independent, side-channel resistant
 *   - **Argon2d:** Data-dependent, maximum GPU resistance
 *
 * **Parameter guidance (2026):**
 *   - Memory: 64 MiB (65536 KiB) minimum, 256 MiB+ for servers
 *   - Iterations: 3-4 passes (~100ms target)
 *   - Parallelism: 1-4 lanes (single-threaded here)
 *   - Salt: 16+ bytes, random
 *   - Output: 32 bytes typical
 *
 * Namespace: argon2_*, argon2id_*, argon2i_*, argon2d_*
 */

#include <crypto/base.h>
#include <crypto/blake2.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ARGON2_BLOCK_SIZE       1024
#define ARGON2_QWORDS_IN_BLOCK  128
#define ARGON2_SYNC_POINTS      4

typedef enum {
	ARGON2_D  = 0,
	ARGON2_I  = 1,
	ARGON2_ID = 2
} argon2_type;

typedef struct {
	uint64_t v[ARGON2_QWORDS_IN_BLOCK];
} argon2_block;

// Variable-length Blake2b (H' from RFC 9106)
static inline void __jacl_argon2_blake2b_long(const uint8_t *in, size_t inlen, uint8_t *out, size_t outlen) {
	uint8_t out_buffer[64];
	uint8_t out_len_bytes[4];
	blake2b_ctx ctx;

	if (outlen <= 64) {
		blake2b(out, outlen, in, inlen, NULL, 0);

		return;
	}

	__jacl_store32_le(out_len_bytes, (uint32_t)outlen);
	blake2b_init(&ctx, 64, NULL, 0);
	blake2b_update(&ctx, out_len_bytes, 4);
	blake2b_update(&ctx, in, inlen);
	blake2b_final(&ctx, out_buffer);
	memcpy(out, out_buffer, 32);

	out += 32;

	uint32_t toproduce = (uint32_t)outlen - 32;

	while (toproduce > 64) {
		blake2b(out_buffer, 64, out_buffer, 64, NULL, 0);
		memcpy(out, out_buffer, 32);

		out += 32;
		toproduce -= 32;
	}

	blake2b(out_buffer, toproduce, out_buffer, 64, NULL, 0);
	memcpy(out, out_buffer, toproduce);
}

// fBlaMka mixing function
static inline uint64_t __jacl_argon2_fblam(uint64_t x, uint64_t y) {
	uint64_t xy = (x & 0xFFFFFFFFULL) * (y & 0xFFFFFFFFULL);

	return x + y + 2 * xy;
}

// G permutation (RFC 9106 Section 3.4)
static inline void __jacl_argon2_g(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t *d) {
	*a = __jacl_argon2_fblam(*a, *b);
	*d = rotr64(*d ^ *a, 32);
	*c = __jacl_argon2_fblam(*c, *d);
	*b = rotr64(*b ^ *c, 24);
	*a = __jacl_argon2_fblam(*a, *b);
	*d = rotr64(*d ^ *a, 16);
	*c = __jacl_argon2_fblam(*c, *d);
	*b = rotr64(*b ^ *c, 63);
}

// P permutation - treat block as 8x16 matrix of qwords
static inline void __jacl_argon2_permute(uint64_t v[ARGON2_QWORDS_IN_BLOCK]) {
	// Apply G column-wise (8 columns, 16 rows)
	for (int i = 0; i < 8; i++) {
		__jacl_argon2_g(&v[i], &v[i + 16], &v[i + 32], &v[i + 48]);
		__jacl_argon2_g(&v[i + 64], &v[i + 80], &v[i + 96], &v[i + 112]);
	}

	for (int i = 0; i < 8; i++) {
		__jacl_argon2_g(&v[i + 8], &v[i + 24], &v[i + 40], &v[i + 56]);
		__jacl_argon2_g(&v[i + 72], &v[i + 88], &v[i + 104], &v[i + 120]);
	}

	// Apply G row-wise (8 rows, 16 columns)
	for (int i = 0; i < 8; i++) {
		__jacl_argon2_g(&v[16 * i], &v[16 * i + 1], &v[16 * i + 2], &v[16 * i + 3]);
		__jacl_argon2_g(&v[16 * i + 8], &v[16 * i + 9], &v[16 * i + 10], &v[16 * i + 11]);
	}

	for (int i = 0; i < 8; i++) {
		__jacl_argon2_g(&v[16 * i + 4], &v[16 * i + 5], &v[16 * i + 6], &v[16 * i + 7]);
		__jacl_argon2_g(&v[16 * i + 12], &v[16 * i + 13], &v[16 * i + 14], &v[16 * i + 15]);
	}
}

// Block compression function (RFC 9106 Section 3.4)
static inline void __jacl_argon2_compress(argon2_block *out, const argon2_block *in1, const argon2_block *in2) {
	argon2_block R, Z;

	// R = X XOR Y
	for (int i = 0; i < ARGON2_QWORDS_IN_BLOCK; i++) R.v[i] = in1->v[i] ^ in2->v[i];

	// Z = P(R)
	memcpy(&Z, &R, sizeof(argon2_block));
	__jacl_argon2_permute(Z.v);

	// out = R XOR Z
	for (int i = 0; i < ARGON2_QWORDS_IN_BLOCK; i++) out->v[i] = R.v[i] ^ Z.v[i];
}

// Generate pseudo-random addresses for Argon2i/id
static inline void __jacl_argon2_gen_addresses(argon2_block *addr_block, uint32_t pass, uint32_t lane, uint32_t slice, uint32_t mem_blocks, uint32_t t_cost, uint32_t type, uint32_t counter) {
	argon2_block input, zero;

	memset(&zero, 0, sizeof(argon2_block));
	memset(&input, 0, sizeof(argon2_block));

	input.v[0] = pass;
	input.v[1] = lane;
	input.v[2] = slice;
	input.v[3] = mem_blocks;
	input.v[4] = t_cost;
	input.v[5] = type;
	input.v[6] = counter;

	__jacl_argon2_compress(&input, &zero, &input);
	__jacl_argon2_compress(addr_block, &zero, &input);
}

// Indexing function (RFC 9106 Section 3.3)
static inline uint32_t __jacl_argon2_index_alpha(uint32_t pass, uint32_t slice, uint32_t lane_blocks, uint32_t index, uint32_t pseudo_rand, uint32_t same_lane) {
	uint32_t ref_area_size;

	if (pass == 0) {
		if (slice == 0) {
			ref_area_size = index - 1;
		} else {
			if (same_lane) {
				ref_area_size = slice * lane_blocks / ARGON2_SYNC_POINTS + index - 1;
			} else {
				ref_area_size = slice * lane_blocks / ARGON2_SYNC_POINTS;

				if (index == 0) ref_area_size--;
			}
		}
	} else {
		if (same_lane) {
			ref_area_size = lane_blocks - lane_blocks / ARGON2_SYNC_POINTS + index - 1;
		} else {
			ref_area_size = lane_blocks - lane_blocks / ARGON2_SYNC_POINTS;

			if (index == 0) ref_area_size--;
		}
	}

	uint64_t rel = pseudo_rand;

	rel = rel * rel >> 32;
	rel = ref_area_size - 1 - ((uint64_t)ref_area_size * rel >> 32);

	uint32_t start = 0;

	if (pass != 0) start = (slice == ARGON2_SYNC_POINTS - 1) ? 0 : (slice + 1) * lane_blocks / ARGON2_SYNC_POINTS;

	return (start + (uint32_t)rel) % lane_blocks;
}

// Fill one segment
static inline void __jacl_argon2_fill_segment(argon2_block *memory, uint32_t pass, uint32_t lane, uint32_t slice, uint32_t lanes, uint32_t lane_blocks, argon2_type type, uint32_t t_cost, uint32_t mem_blocks) {
	uint32_t data_indep = (type == ARGON2_I) || (type == ARGON2_ID && pass == 0 && slice < ARGON2_SYNC_POINTS / 2);
	uint32_t addr_counter = 1;
	uint32_t start_idx = 0;
	argon2_block addr_block;

	if (pass == 0 && slice == 0) {
		start_idx = 2;

		if (data_indep) __jacl_argon2_gen_addresses(&addr_block, pass, lane, slice, mem_blocks, t_cost, type, addr_counter);
	} else if (data_indep) {
		__jacl_argon2_gen_addresses(&addr_block, pass, lane, slice, mem_blocks, t_cost, type, addr_counter);
	}

	uint32_t segment_len = lane_blocks / ARGON2_SYNC_POINTS;

	for (uint32_t i = start_idx; i < segment_len; i++) {
		uint32_t curr_offset = lane * lane_blocks + slice * segment_len + i;
		uint32_t prev_offset = (curr_offset % lane_blocks == 0) ? (curr_offset + lane_blocks - 1) : (curr_offset - 1);
		uint64_t pseudo_rand;
		uint32_t ref_lane;

		if (data_indep) {
			uint32_t addr_idx = (i - start_idx) % ARGON2_QWORDS_IN_BLOCK;

			if (addr_idx == 0 && i > start_idx) {
				addr_counter++;
				__jacl_argon2_gen_addresses(&addr_block, pass, lane, slice, mem_blocks, t_cost, type, addr_counter);
			}

			// J1 = lower 32 bits, J2 = upper 32 bits (RFC 9106 Section 3.3)
			uint64_t addr_value = addr_block.v[addr_idx];

			pseudo_rand = (uint32_t)addr_value;
			ref_lane = (uint32_t)(addr_value >> 32) % lanes;
		} else {
			pseudo_rand = memory[prev_offset].v[0];
			ref_lane = (uint32_t)(pseudo_rand >> 32) % lanes;
		}

		if (pass == 0 && slice == 0) ref_lane = lane;

		uint32_t ref_index = __jacl_argon2_index_alpha(pass, slice, lane_blocks, i, (uint32_t)pseudo_rand, ref_lane == lane);
		uint32_t ref_offset = ref_lane * lane_blocks + ref_index;

		if (pass == 0) {
			__jacl_argon2_compress(&memory[curr_offset], &memory[prev_offset], &memory[ref_offset]);
		} else {
			argon2_block tmp;

			__jacl_argon2_compress(&tmp, &memory[prev_offset], &memory[ref_offset]);

			for (int j = 0; j < ARGON2_QWORDS_IN_BLOCK; j++) memory[curr_offset].v[j] ^= tmp.v[j];
		}
	}
}

// argon2_hash – Generic Argon2 function
static inline int argon2_hash(argon2_type type, uint32_t t_cost, uint32_t m_cost, uint32_t p_cost, const uint8_t *password, size_t plen, const uint8_t *salt, size_t slen, uint8_t *out, size_t outlen) {
	if (t_cost < 1 || m_cost < 8 * p_cost || p_cost < 1 || p_cost > 0xFFFFFF) return -1;
	if (!password || !salt || !out || outlen < 4 || outlen > 0xFFFFFFFF) return -1;

	uint32_t lanes = p_cost;
	uint32_t lane_blocks = m_cost / lanes;
	uint32_t segment_blocks = lane_blocks / ARGON2_SYNC_POINTS;

	lane_blocks = segment_blocks * ARGON2_SYNC_POINTS;

	uint32_t mem_blocks = lane_blocks * lanes;
	argon2_block *memory = (argon2_block *)malloc(mem_blocks * sizeof(argon2_block));

	if (!memory) return -1;

	// Build H0
	blake2b_ctx ctx;
	uint32_t tmp32;

	blake2b_init(&ctx, 64, NULL, 0);

	tmp32 = lanes; blake2b_update(&ctx, (uint8_t*)&tmp32, 4);
	tmp32 = (uint32_t)outlen; blake2b_update(&ctx, (uint8_t*)&tmp32, 4);
	tmp32 = m_cost; blake2b_update(&ctx, (uint8_t*)&tmp32, 4);
	tmp32 = t_cost; blake2b_update(&ctx, (uint8_t*)&tmp32, 4);
	tmp32 = 0x13; blake2b_update(&ctx, (uint8_t*)&tmp32, 4);
	tmp32 = (uint32_t)type; blake2b_update(&ctx, (uint8_t*)&tmp32, 4);
	tmp32 = (uint32_t)plen; blake2b_update(&ctx, (uint8_t*)&tmp32, 4);

	blake2b_update(&ctx, password, plen);

	tmp32 = (uint32_t)slen; blake2b_update(&ctx, (uint8_t*)&tmp32, 4);

	blake2b_update(&ctx, salt, slen);

	tmp32 = 0; blake2b_update(&ctx, (uint8_t*)&tmp32, 4); // secret
	tmp32 = 0; blake2b_update(&ctx, (uint8_t*)&tmp32, 4); // ad

	uint8_t h0[64];

	blake2b_final(&ctx, h0);

	// Initialize first two blocks per lane
	uint8_t blockhash_input[72];

	memcpy(blockhash_input, h0, 64);

	for (uint32_t lane = 0; lane < lanes; lane++) {
		__jacl_store32_le(blockhash_input + 64, 0);
		__jacl_store32_le(blockhash_input + 68, lane);
		__jacl_argon2_blake2b_long(blockhash_input, 72, (uint8_t*)&memory[lane * lane_blocks], 1024);

		__jacl_store32_le(blockhash_input + 64, 1);
		__jacl_argon2_blake2b_long(blockhash_input, 72, (uint8_t*)&memory[lane * lane_blocks + 1], 1024);
	}

	// Fill memory
	for (uint32_t pass = 0; pass < t_cost; pass++) {
		for (uint32_t slice = 0; slice < ARGON2_SYNC_POINTS; slice++) {
			for (uint32_t lane = 0; lane < lanes; lane++) {
				__jacl_argon2_fill_segment(memory, pass, lane, slice, lanes, lane_blocks, type, t_cost, mem_blocks);
			}
		}
	}

	// Final XOR across lanes
	argon2_block final_block = memory[mem_blocks - 1];
	for (uint32_t lane = 0; lane < lanes - 1; lane++) {
		uint32_t last = (lane + 1) * lane_blocks - 1;

		for (int i = 0; i < ARGON2_QWORDS_IN_BLOCK; i++) final_block.v[i] ^= memory[last].v[i];
	}

	__jacl_argon2_blake2b_long((uint8_t*)final_block.v, 1024, out, outlen);
	__jacl_explicit_bzero(memory, mem_blocks * sizeof(argon2_block));
	free(memory);
	__jacl_explicit_bzero(h0, sizeof(h0));

	return 0;
}

// argon2id_hash – Argon2id (RECOMMENDED)
static inline int argon2id_hash(uint32_t t_cost, uint32_t m_cost, uint32_t p_cost, const uint8_t *password, size_t plen, const uint8_t *salt, size_t slen, uint8_t *out, size_t outlen) {
	return argon2_hash(ARGON2_ID, t_cost, m_cost, p_cost,
	                   password, plen, salt, slen, out, outlen);
}

// argon2id_hash_simple – Argon2id with defaults (64 MiB, 3 passes, 4 lanes)
static inline int argon2id_hash_simple(const uint8_t *password, size_t plen, const uint8_t *salt, size_t slen, uint8_t out[32]) {
	return argon2id_hash(3, 65536, 4, password, plen, salt, slen, out, 32);
}

#ifdef __cplusplus
}
#endif

#endif /* _CRYPTO_ARGON2_H */
