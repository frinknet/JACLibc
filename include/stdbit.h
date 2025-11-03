/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STDBIT_H
#define STDBIT_H
#pragma once

#include <config.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if JACL_HAS_C23
#  define __STDC_VERSION_STDBIT_H__ 202311L

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#  define __STDC_ENDIAN_BIG__		 1
#  define __STDC_ENDIAN_LITTLE__ 0
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#  define __STDC_ENDIAN_BIG__		 0
#  define __STDC_ENDIAN_LITTLE__ 1
#else
#  define __STDC_ENDIAN_BIG__		 0
#  define __STDC_ENDIAN_LITTLE__ 0
#endif
#define __STDC_ENDIAN_NATIVE__	 (__STDC_ENDIAN_BIG__ ? 4321 : 1234)

#endif

static inline int __jacl_ctz32(uint32_t x) {
		if (!x) return 32;

		static const int table[32] = {
				0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
				31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
		};

		return table[((x & -x) * 0x077CB531U) >> 27];
}

static inline int __jacl_ctz64(uint64_t x) {
	if (!x) return 64;

	static const int table[64] = {
		0,  1,  2, 53,  3,  7, 54, 27, 4, 38, 41,  8, 34, 55, 48, 28,
		62,  5, 39, 46, 44, 42, 22,  9, 24, 35, 59, 56, 49, 18, 29, 11,
		63, 52,  6, 26, 37, 40, 33, 47, 61, 45, 43, 21, 23, 58, 17, 10,
		51, 25, 36, 32, 60, 20, 57, 16, 50, 31, 19, 15, 30, 14, 13, 12
	};

	return table[((x & -x) * 0x022fdd63cc95386dULL) >> 58];
}

static inline int __jacl_clz32(uint32_t x) {
		if (!x) return 32;

		static const int table[32] = {
				31, 22, 30, 21, 18, 10, 29, 2, 20, 17, 15, 13, 9, 6, 28, 1,
				23, 19, 11, 3, 16, 14, 7, 24, 12, 4, 8, 25, 5, 26, 27, 0
		};

		x |= x >> 1;	x |= x >> 2;	x |= x >> 4;
		x |= x >> 8;	x |= x >> 16;
		return table[(x * 0x07C4ACDDU) >> 27];
}

static inline int __jacl_clz64(uint64_t x) {
	if (!x) return 64;

	static const int table[64] = {
		63, 16, 62,  7, 15, 36, 61,  3,  6, 14, 22, 26, 35, 47, 60,  2,
		 9,  5, 28, 11, 13, 21, 42, 19, 25, 31, 34, 40, 46, 52, 59,  1,
		17,  8, 37,  4, 23, 27, 48, 10, 29, 12, 43, 20, 32, 41, 53, 18,
		38, 24, 49, 30, 44, 33, 54, 39, 50, 45, 55, 51, 56, 57, 58,  0
	};

	x |= x >> 1;  x |= x >> 2;  x |= x >> 4;
	x |= x >> 8;  x |= x >> 16; x |= x >> 32;
	return table[(x * 0x03f79d71b4cb0a89ULL) >> 58];
}

static inline int __jacl_pop32(uint32_t x) {
	x = x - ((x >> 1) & 0x55555555u);
	x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
	x = (x + (x >> 4)) & 0x0F0F0F0Fu;
	x = x + (x >> 8);
	x = x + (x >> 16);

	return x & 0x3Fu;
}

static inline int __jacl_pop64(uint64_t x) {
	x = x - ((x >> 1) & 0x5555555555555555ULL);
	x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
	x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
	x = x + (x >> 8);
	x = x + (x >> 16);
	x = x + (x >> 32);
	return x & 0x7F;
}

#if JACL_HAS_C23

#define __jacl_bitgen(width,type,suf) \
static inline int stdc_leading_zeros_##suf(type v){ if (!v) return width; if (width <= 32) return __jacl_clz32((uint32_t)v) - (32 - width); else return __jacl_clz64((uint64_t)v); } \
static inline int stdc_leading_ones_##suf(type v){ return stdc_leading_zeros_##suf((type)~v);} \
static inline int stdc_trailing_zeros_##suf(type v){ if (!v) return width; if (width <= 32) return __jacl_ctz32((uint32_t)v); else return __jacl_ctz64((uint64_t)v); } \
static inline int stdc_trailing_ones_##suf(type v){ return stdc_trailing_zeros_##suf((type)~v); } \
static inline int stdc_count_ones_##suf(type v){ if (width <= 32) return __jacl_pop32((uint32_t)v); else return __jacl_pop64((uint64_t)v); } \
static inline int stdc_count_zeros_##suf(type v){ return (int)width - stdc_count_ones_##suf(v); } \
static inline int stdc_bit_width_##suf(type v){ return v ? (int)width - stdc_leading_zeros_##suf(v) : 0; } \
static inline type stdc_bit_floor_##suf(type v){ return v ? (type)1 << (stdc_bit_width_##suf(v)-1) : 0; } \
static inline type stdc_bit_ceil_##suf(type v){ if(!v) return 0; return (type)1 << stdc_bit_width_##suf((type)(v-1)); } \
static inline int stdc_has_single_bit_##suf(type v){ return ((v) && !(((v)-1u)&(v))); }

__jacl_bitgen( 8, unsigned char,      uc)
__jacl_bitgen(16, unsigned short,     us)
__jacl_bitgen(32, unsigned int,       ui)
__jacl_bitgen(32, unsigned long,      ul)	 /* ILP32 / LLP64 */
__jacl_bitgen(64, unsigned long long,	ull)

#define __jacl_tgm(fn,x) _Generic((x),								 \
	unsigned char:				 fn##_uc,									 \
	unsigned short:				 fn##_us,									 \
	unsigned int:					 fn##_ui,									 \
	unsigned long:				 fn##_ul,									 \
	unsigned long long:		 fn##_ull									 \
)(x)

#define stdc_leading_zeros(x)		__jacl_tgm(stdc_leading_zeros,	x)
#define stdc_leading_ones(x)		__jacl_tgm(stdc_leading_ones,		x)
#define stdc_trailing_zeros(x)	__jacl_tgm(stdc_trailing_zeros, x)
#define stdc_trailing_ones(x)		__jacl_tgm(stdc_trailing_ones,	x)
#define stdc_count_ones(x)			__jacl_tgm(stdc_count_ones,			x)
#define stdc_count_zeros(x)			__jacl_tgm(stdc_count_zeros,		x)
#define stdc_bit_width(x)				__jacl_tgm(stdc_bit_width,			x)
#define stdc_bit_floor(x)				__jacl_tgm(stdc_bit_floor,			x)
#define stdc_bit_ceil(x)				__jacl_tgm(stdc_bit_ceil,				x)
#define stdc_has_single_bit(x)	__jacl_tgm(stdc_has_single_bit, x)

#endif /* JACL_HAS_C23 */

/* Non-standard but handy rotates */
static inline uint32_t rotl32(uint32_t v, int r) { return (v << (r&31)) | (v >> (32-(r&31))); }
static inline uint32_t rotr32(uint32_t v, int r) { return (v >> (r&31)) | (v << (32-(r&31))); }
static inline uint64_t rotl64(uint64_t v, int r) { return (v << (r&63)) | (v >> (64-(r&63))); }
static inline uint64_t rotr64(uint64_t v, int r) { return (v >> (r&63)) | (v << (64-(r&63))); }

#ifdef __cplusplus
}
#endif
#endif /* STDBIT_H */
