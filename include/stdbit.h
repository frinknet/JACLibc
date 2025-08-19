// (c) 2025 FRINKnet & Friends - MIT licence
#ifndef STDBIT_H
#define STDBIT_H

#include <limits.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* — Endianness — */
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	#define STDBIT_ENDIAN_BIG		 1
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	#define STDBIT_ENDIAN_LITTLE 1
#else
	#error "Unknown endianness: __BYTE_ORDER__ not set"
#endif

enum stdbit_endian { 
#if STDBIT_ENDIAN_BIG
		stdbit_big_endian,
		stdbit_little_endian
#else
		stdbit_little_endian,
		stdbit_big_endian
#endif
};

/* Count leading zeros/ones, trailing zeros/ones, popcount */
int countl_zero(uint32_t x)		{ return x ? __builtin_clz(x) : 32; }
int countl_one(uint32_t x)		{ return x ? __builtin_clz(~x) : 32; }
int countr_zero(uint32_t x)		{ return x ? __builtin_ctz(x) : 32; }
int countr_one(uint32_t x)		{ return x ? __builtin_ctz(~x) : 32; }
int popcount(uint32_t x)			{ return __builtin_popcount(x); }

/* Bit floor/ceil/width */
uint32_t	bit_floor(uint32_t x)		{ return x ? (1u << (31 - __builtin_clz(x))) : 0; }
uint32_t	bit_ceil(uint32_t x)		{ if (!x) return 0; int lz = __builtin_clz(x - 1); return 1u << (31 - lz); }
int				bit_width(uint32_t x)		{ return x ? (32 - __builtin_clz(x)) : 0; }

/* Rotate left/right */
uint32_t	rotl(uint32_t x, int r)	{ const int m = r & 31; return (x << m) | (x >> ((32 - m) & 31)); }
uint32_t	rotr(uint32_t x, int r)	{ const int m = r & 31; return (x >> m) | (x << ((32 - m) & 31)); }

#ifdef __cplusplus
}
#endif

#endif /* STDBIT_H */
