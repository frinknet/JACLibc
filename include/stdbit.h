	// (c) 2025 FRINKnet & Friends – MIT licence
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
#endif

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

#if defined(__clang__) || defined(__GNUC__)
#  define __JACL_HAVE_BUILTIN_CLZ		1
#  define __JACL_HAVE_BUILTIN_CTZ		1
#  define __JACL_HAVE_BUILTIN_POP		1
#elif defined(_MSC_VER)
#  include <intrin.h>
#endif

static inline int __jacl_ctz32(uint32_t x) {
		if (!x) return 32;
		static const int table[32] = {
				0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
				31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
		};
		return table[((x & -x) * 0x077CB531U) >> 27];
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

static inline int __jacl_pop32(uint32_t x) {
	x = x - ((x >> 1) & 0x55555555u);
	x = (x & 0x33333333u) + ((x >> 2) & 0x33333333u);
	x = (x + (x >> 4)) & 0x0F0F0F0Fu;
	x = x + (x >> 8);
	x = x + (x >> 16);

	return x & 0x3Fu;
}

#define __jacl_bitgen(width,type,suf) \
static inline int stdc_leading_zeros_##suf(type v){ \
		if (!v) return width; \
		if (width <= 32) { \
				return __jacl_clz32((uint32_t)v) - (32 - width); \
		} else { \
				uint32_t hi = (uint32_t)(v >> 32); \
				return hi ? __jacl_clz32(hi) : 32 + __jacl_clz32((uint32_t)v); \
		} \
} \
static inline int stdc_leading_ones_##suf(type v){ \
		return stdc_leading_zeros_##suf((type)~v); \
} \
static inline int stdc_trailing_zeros_##suf(type v){ \
		if (!v) return width; \
		if (width <= 32) { \
				return __jacl_ctz32((uint32_t)v); \
		} else { \
				uint32_t lo = (uint32_t)v; \
				return lo ? __jacl_ctz32(lo) : 32 + __jacl_ctz32((uint32_t)(v>>32)); \
		} \
} \
static inline int stdc_trailing_ones_##suf(type v){ \
		return stdc_trailing_zeros_##suf((type)~v); \
} \
static inline int stdc_count_ones_##suf(type v){ \
		if (width <= 32) { \
				return __jacl_pop32((uint32_t)v); \
		} else { \
				return __jacl_pop32((uint32_t)v) + __jacl_pop32((uint32_t)(v>>32)); \
		} \
} \
static inline int stdc_count_zeros_##suf(type v){															 \
		return (int)width - stdc_count_ones_##suf(v);															 \
}																																								\
static inline int stdc_bit_width_##suf(type v){																 \
		return v ? (int)width - stdc_leading_zeros_##suf(v) : 0;									 \
}																																								\
static inline type stdc_bit_floor_##suf(type v){															 \
		return v ? (type)1 << (stdc_bit_width_##suf(v)-1) : 0;										 \
}																																								\
static inline type stdc_bit_ceil_##suf(type v){																 \
		if(!v) return 0;																													 \
		return (type)1 << stdc_bit_width_##suf((type)(v-1));											 \
}																																								\
static inline int stdc_has_single_bit_##suf(type v){													 \
		return ((v) && !(((v)-1u)&(v)));																					 \
}

__jacl_bitgen( 8, unsigned char,				 uc)
__jacl_bitgen(16, unsigned short,			 us)
__jacl_bitgen(32, unsigned int,				 ui)
__jacl_bitgen(32, unsigned long,				 ul)	 /* ILP32 / LLP64 */
__jacl_bitgen(64, unsigned long long,	ull)

#undef __jacl_bitgen

#if JACL_HAS_C11
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

	#undef __jacl_tgm
#endif

/* 5	Non-standard but handy rotates */
static inline uint32_t rotl32(uint32_t v,int r){return (v<< (r&31))|(v>>(32-(r&31)));}
static inline uint32_t rotr32(uint32_t v,int r){return (v>>(r&31))|(v<<(32-(r&31)));}

#ifdef __cplusplus
}
#endif
#endif /* STDBIT_H */
