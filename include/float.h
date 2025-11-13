/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef FLOAT_H
#define FLOAT_H
#pragma once

#include <config.h>
#include <stdint.h>
#include <stdbit.h>

#ifdef __cplusplus
extern "C" {
#endif

// ========= SINGLE PRECISION FLOAT =========
#define FLT_RADIX       2
#define FLT_MANT_DIG    24
#define FLT_DIG         6
#define FLT_MIN_EXP     (-125)
#define FLT_MAX_EXP     128
#define FLT_MIN_10_EXP  (-37)
#define FLT_MAX_10_EXP  38
#define FLT_MAX         3.40282347e+38F
#define FLT_EPSILON     1.19209290e-07F
#define FLT_MIN         1.17549435e-38F
#define FLT_TRUE_MIN    1.40129846e-45F
#define FLT_UTYPE       uint32_t
#define FLT_NAN_BASE       0x7fc00000U
#define FLT_PAYLOAD_MASK   0x7fffffU

#if JACL_HAS_C11
#define FLT_DECIMAL_DIG  9
#define FLT_HAS_SUBNORM  1
#endif

// ========= DOUBLE PRECISION FLOAT =========
#define DBL_MANT_DIG    53
#define DBL_DIG         15
#define DBL_MIN_EXP     (-1021)
#define DBL_MAX_EXP     1024
#define DBL_MIN_10_EXP  (-307)
#define DBL_MAX_10_EXP  308
#define DBL_MAX         1.7976931348623157e+308
#define DBL_EPSILON     2.2204460492503131e-16
#define DBL_MIN         2.2250738585072014e-308
#define DBL_TRUE_MIN    4.9406564584124654e-324
#define DBL_UTYPE       uint64_t
#define DBL_NAN_BASE       0x7ff8000000000000ULL
#define DBL_PAYLOAD_MASK   0x7ffffffffffffULL

#if JACL_HAS_C11
#define DBL_DECIMAL_DIG  17
#define DBL_HAS_SUBNORM  1
#endif

// ========= LONG DOUBLE PRECISION ==========
#if JACL_LDBL_BITS == 128
	#define LDBL_MANT_DIG   113
	#define LDBL_DIG        33
	#define LDBL_MIN_EXP    (-16381)
	#define LDBL_MAX_EXP    16384
	#define LDBL_MIN_10_EXP (-4931)
	#define LDBL_MAX_10_EXP 4932
	#define LDBL_MAX        1.18973149535723176502e+4932L
	#define LDBL_EPSILON    1.92592994438723585305597794258492732e-34L
	#define LDBL_MIN        3.36210314311209350626e-4932L
	#define LDBL_TRUE_MIN   6.47517511943802511092443895822764655e-4966L
	#define LDBL_UTYPE      uint64_t
	#define LDBL_NAN_BASE       0x7fff8000000000000000000000000000ULL
	#define LDBL_PAYLOAD_MASK   0x7fffffffffffffffffffffffffffULL

	#if JACL_HAS_C11
	#define LDBL_DECIMAL_DIG 36
	#define LDBL_HAS_SUBNORM 1
	#endif

#elif JACL_LDBL_BITS == 80
	#define LDBL_MANT_DIG   64
	#define LDBL_DIG        18
	#define LDBL_MIN_EXP    (-16381)
	#define LDBL_MAX_EXP    16384
	#define LDBL_MIN_10_EXP (-4931)
	#define LDBL_MAX_10_EXP 4932
	#define LDBL_MAX        1.18973149535723176502e+4932L
	#define LDBL_EPSILON    1.08420217248550443401e-19L
	#define LDBL_MIN        3.36210314311209350626e-4932L
	#define LDBL_TRUE_MIN   3.64519953188247460253e-4951L
	#define LDBL_UTYPE      uint64_t
	#define LDBL_NAN_BASE       0x7fff8000000000000000ULL
	#define LDBL_PAYLOAD_MASK   0x3fffffffffffffffULL

	#if JACL_HAS_C11
	#define LDBL_DECIMAL_DIG 21
	#define LDBL_HAS_SUBNORM 1
	#endif

#else
	#define LDBL_MANT_DIG   DBL_MANT_DIG
	#define LDBL_DIG        DBL_DIG
	#define LDBL_MIN_EXP    DBL_MIN_EXP
	#define LDBL_MAX_EXP    DBL_MAX_EXP
	#define LDBL_MIN_10_EXP DBL_MIN_10_EXP
	#define LDBL_MAX_10_EXP DBL_MAX_10_EXP
	#define LDBL_MAX        DBL_MAX
	#define LDBL_EPSILON    DBL_EPSILON
	#define LDBL_MIN        DBL_MIN
	#define LDBL_TRUE_MIN   DBL_TRUE_MIN
	#define LDBL_UTYPE      DBL_UTYPE
	#define LDBL_NAN_BASE       DBL_NAN_BASE
	#define LDBL_PAYLOAD_MASK   DBL_PAYLOAD_MASK

	#if JACL_HAS_C11
	#define LDBL_DECIMAL_DIG DBL_DECIMAL_DIG
	#define LDBL_HAS_SUBNORM DBL_HAS_SUBNORM
	#endif
#endif

#if JACL_LDBL_BITS == 128

typedef struct {
	uint64_t lo;
	uint64_t hi;
}__jacl_ldbl128_t;

#elif JACL_LDBL_BITS == 80

typedef struct __attribute__((packed)) {
	uint64_t mantissa;
	uint16_t exp_sign;
} __jacl_ldbl80_t;

#endif

// ======== GENERAL FLOAT PRECISION =========
#define DECIMAL_DIG     LDBL_DECIMAL_DIG
#define FLT_ROUNDS      1

#if JACL_HAS_C99
#define FLT_EVAL_METHOD 0
#endif

// ======= SING MANIPULATION HELPERS ========
static inline int __jacl_signget_FLT(float x) {
	union { float f; uint32_t u; } a;

	a.f = x;

	return (a.u >> 31);
}
static inline float __jacl_signclr_FLT(float x) {
	union { float f; uint32_t u; } a;

	a.f = x;
	a.u &= 0x7FFFFFFFU;

	return a.f;
}
static inline float __jacl_signset_FLT(float x) {
	union { float f; uint32_t u; } a;

	a.f = x;
	a.u |= 0x80000000U;

	return a.f;
}
static inline float __jacl_signcpy_FLT(float x, float y) {
	union { float f; uint32_t u; } a, b;

	a.f = x;
	b.f = y;
	a.u = (a.u & 0x7FFFFFFFU) | (b.u & 0x80000000U);

	return a.f;
}

static inline int __jacl_signget_DBL(double x) {
	union { double f; uint64_t u; } a;

	a.f = x;

	return (a.u >> 63);
}
static inline double __jacl_signclr_DBL(double x) {
	union { double f; uint64_t u; } a;

	a.f = x;
	a.u &= 0x7FFFFFFFFFFFFFFFULL;

	return a.f;
}
static inline double __jacl_signset_DBL(double x) {
	union { double f; uint64_t u; } a;

	a.f = x;
	a.u |= 0x8000000000000000ULL;

	return a.f;
}
static inline double __jacl_signcpy_DBL(double x, double y) {
	union { double f; uint64_t u; } a, b;

	a.f = x;
	b.f = y;
	a.u = (a.u & 0x7FFFFFFFFFFFFFFFULL) | (b.u & 0x8000000000000000ULL);

	return a.f;
}

#if JACL_LDBL_BITS == 128

static inline int __jacl_signget_LDBL(long double x) {
	union { long double f; __jacl_ldbl128_t u; } a;

	a.f = x;

	return (a.u.hi >> 63);
}
static inline long double __jacl_signclr_LDBL(long double x) {
	union { long double f; __jacl_ldbl128_t u; } a;

	a.f = x;
	a.u.hi &= 0x7FFFFFFFFFFFFFFFULL;

	return a.f;
}
static inline long double __jacl_signset_LDBL(long double x) {
	union { long double f; __jacl_ldbl128_t u; } a;

	a.f = x;
	a.u.hi |= 0x8000000000000000ULL;

	return a.f;
}
static inline long double __jacl_signcpy_LDBL(long double x, long double y) {
	union { long double f; __jacl_ldbl128_t u; } a, b;

	a.f = x;
	b.f = y;
	a.u.hi = (a.u.hi & 0x7FFFFFFFFFFFFFFFULL) | (b.u.hi & 0x8000000000000000ULL);

	return a.f;
}

#elif JACL_LDBL_BITS == 80

static inline int __jacl_signget_LDBL(long double x) {
	union { long double f; __jacl_ldbl80_t u; } a;

	a.f = x;

	return (a.u.exp_sign >> 15);
}
static inline long double __jacl_signclr_LDBL(long double x) {
	union { long double f; __jacl_ldbl80_t u; } a;

	a.f = x;
	a.u.exp_sign &= 0x7FFFU;

	return a.f;
}
static inline long double __jacl_signset_LDBL(long double x) {
	union { long double f; __jacl_ldbl80_t u; } a;

	a.f = x;
	a.u.exp_sign |= 0x8000U;

	return a.f;
}
static inline long double __jacl_signcpy_LDBL(long double x, long double y) {
	union { long double f; __jacl_ldbl80_t u; } a, b;

	a.f = x;
	b.f = y;
	a.u.exp_sign = (a.u.exp_sign & 0x7FFFU) | (b.u.exp_sign & 0x8000U);

	return a.f;
}

#else

#define __jacl_signget_LDBL  __jacl_signget_DBL
#define __jacl_signclr_LDBL  __jacl_signclr_DBL
#define __jacl_signset_LDBL  __jacl_signset_DBL
#define __jacl_signcpy_LDBL  __jacl_signcpy_DBL

#endif

// ===== MANTISSA MANIPULATION HELPERS ======
static inline uint32_t __jacl_mantget_FLT(float x) {
	union { float f; uint32_t u; } a;

	a.f = x;

	return a.u & 0x7FFFFFU;  // 23-bit mantissa (implicit leading bit not stored)
}
static inline int __jacl_mantctz_FLT(float x) {
	uint32_t m = __jacl_mantget_FLT(x);

	return m ? __jacl_ctz32(m) : 23;
}

static inline uint64_t __jacl_mantget_DBL(double x) {
	union { double f; uint64_t u; } a;

	a.f = x;

	return a.u & 0xFFFFFFFFFFFFFULL;  // 52-bit mantissa
}
static inline int __jacl_mantctz_DBL(double x) {
	uint64_t m = __jacl_mantget_DBL(x);

	return m ? __jacl_ctz64(m) : 52;
}

#if JACL_LDBL_BITS == 128

static inline __jacl_ldbl128_t __jacl_mantget_LDBL(long double x) {
	union { long double f; __jacl_ldbl128_t u; } a;

	a.f = x;

	return (__jacl_ldbl128_t){
		.lo = a.u.lo,
		.hi = a.u.hi & 0xFFFFFFFFFFFFULL  // 112-bit mantissa (mask off exp/sign)
	};
}
static inline int __jacl_mantctz_LDBL(long double x) {
	union { long double f; __jacl_ldbl128_t u; } a;

	a.f = x;

	if (a.u.lo) {
		return __jacl_ctz64(a.u.lo);
	} else {
		uint64_t hi_mant = a.u.hi & 0xFFFFFFFFFFFFULL;

		return hi_mant ? 64 + __jacl_ctz64(hi_mant) : 112;
	}
}

#elif JACL_LDBL_BITS == 80

static inline uint64_t __jacl_mantget_LDBL(long double x) {
	union { long double f; __jacl_ldbl80_t u; } a;

	a.f = x;

	return a.u.mantissa;  // 64-bit mantissa (explicit leading bit included on x87)
}
static inline int __jacl_mantctz_LDBL(long double x) {
	union { long double f; __jacl_ldbl80_t u; } a;

	a.f = x;

	return a.u.mantissa ? __jacl_ctz64(a.u.mantissa) : 64;
}

#else  /* JACL_LDBL_BITS == 64 */

#define __jacl_mantget_LDBL __jacl_mantget_DBL
#define __jacl_mantctz_LDBL __jacl_mantctz_DBL

#endif

// ===== EXPONENT MANIPULATION HELPERS ======
static inline int __jacl_expfind_FLT(float x) {
	union { float f; uint32_t u; } a;

	a.f = x;

	return ((a.u >> 23) & 0xFF) - 127;
}
static inline float __jacl_expscal_FLT(float x, int n) {
	union { float f; uint32_t u; } a;
	int e;

	a.f = x;
	e = ((a.u >> 23) & 0xFF);

	if (e == 0xFF) return x;

	if (e == 0) {
		uint32_t mant = a.u & 0x7FFFFFU;

		if (mant == 0) return x;

		int lz = __jacl_clz32(mant) - 8;

		mant = (mant << (lz + 1)) & 0x7FFFFFU;
		e = 1 - lz;
		a.u = (a.u & 0x80000000U) | mant;
	}

	e += n;

	if (e <= 0) {
		uint32_t mant = a.u & 0x7FFFFFU;

		if (e < -23 || mant == 0) return 0.0f * x;

		a.u = (a.u & 0x80000000U) | ((mant | 0x800000U) >> (1 - e));

		return a.f;
	}

	if (e >= 0xFF) return x * (1.0f / 0.0f);

	a.u = (a.u & 0x807FFFFFU) | ((uint32_t)e << 23);

	return a.f;
}
static inline float __jacl_valnext_FLT(float x, int dir) {
	union { float f; uint32_t u; } a;

	if (x != x) return x;

	a.f = x;

	if (dir > 0) a.u++; else a.u--;

	return a.f;
}

static inline int __jacl_expfind_DBL(double x) {
	union { double f; uint64_t u; } a;
	a.f = x;

	return ((a.u >> 52) & 0x7FF) - 1023;
}
static inline double __jacl_expscal_DBL(double x, int n) {
	union { double f; uint64_t u; } a;
	int e;

	a.f = x;
	e = ((a.u >> 52) & 0x7FF);

	if (e == 0x7FF) return x;

	if (e == 0) {
		uint64_t mant = a.u & 0xFFFFFFFFFFFFFULL;

		if (mant == 0) return x;

		int lz = __jacl_clz64(mant) - 11;

		mant = (mant << (lz + 1)) & 0xFFFFFFFFFFFFFULL;
		e = 1 - lz;
		a.u = (a.u & 0x8000000000000000ULL) | mant;
	}

	e += n;

	if (e <= 0) {
		uint64_t mant = a.u & 0xFFFFFFFFFFFFFULL;

		if (e < -52 || mant == 0) return 0.0 * x;

		a.u = (a.u & 0x8000000000000000ULL) | ((mant | 0x10000000000000ULL) >> (1 - e));

		return a.f;
	}

	if (e >= 0x7FF) return x * (1.0 / 0.0);

	a.u = (a.u & 0x800FFFFFFFFFFFFFULL) | ((uint64_t)e << 52);

	return a.f;
}
static inline double __jacl_valnext_DBL(double x, int dir) {
	union { double f; uint64_t u; } a;

	if (x != x) return x;

	a.f = x;

	if (dir > 0) a.u++; else a.u--;

	return a.f;
}

#if JACL_LDBL_BITS == 128

static inline int __jacl_expfind_LDBL(long double x) {
	union { long double f; __jacl_ldbl128_t u; } a;

	a.f = x;

	return ((a.u.hi >> 48) & 0x7FFF) - 16383;
}
static inline long double __jacl_expscal_LDBL(long double x, int n) {
	union { long double f; __jacl_ldbl128_t u; } a;
	int e;

	a.f = x;
	e = ((a.u.hi >> 48) & 0x7FFF);

	if (e == 0x7FFF) return x;

	if (e == 0) {
		uint64_t mant_hi = a.u.hi & 0xFFFFFFFFFFFFULL;
		uint64_t mant_lo = a.u.lo;
		int lz;

		if (mant_hi == 0 && mant_lo == 0) return x;
		if (mant_hi) lz = __jacl_clz64(mant_hi) - 16;
		else lz = __jacl_clz64(mant_lo) + 48;

		if (lz < 64) {
			mant_hi = (mant_hi << (lz + 1)) | (mant_lo >> (63 - lz));
			mant_lo = mant_lo << (lz + 1);
		} else {
			mant_hi = mant_lo << (lz - 63);
			mant_lo = 0;
		}

		e = 1 - lz;
		a.u.hi = (a.u.hi & 0x8000000000000000ULL) | (mant_hi & 0xFFFFFFFFFFFFULL);
		a.u.lo = mant_lo;
	}

	e += n;

	if (e <= 0) {
		uint64_t sign = a.u.hi & 0x8000000000000000ULL;
		uint64_t mant_hi = a.u.hi & 0xFFFFFFFFFFFFULL;
		uint64_t mant_lo = a.u.lo;
		int shift;

		if (e < -111 || (mant_hi == 0 && mant_lo == 0)) return 0.0L * x;

		mant_hi |= 0x1000000000000ULL;

		shift = 1 - e;

		if (shift < 64) {
			a.u.lo = (mant_lo >> shift) | (mant_hi << (64 - shift));
			a.u.hi = (mant_hi >> shift) | sign;
		} else if (shift < 113) {
			a.u.lo = mant_hi >> (shift - 64);
			a.u.hi = sign;
		} else {
			return 0.0L * x;
		}

		return a.f;
	}

	if (e >= 0x7FFF) return x * (1.0L / 0.0L);

	a.u.hi = (a.u.hi & 0x8000FFFFFFFFFFFFULL) | ((uint64_t)e << 48);

	return a.f;
}
static inline long double __jacl_valnext_LDBL(long double x, int dir) {
	union { long double f; __jacl_ldbl128_t u; } a;

	if (x != x) return x;

	a.f = x;

	if (dir > 0) {
		if (a.u.lo == 0xFFFFFFFFFFFFFFFFULL) {
			a.u.lo = 0;
			a.u.hi++;
		} else {
			a.u.lo++;
		}
	} else {
		if (a.u.lo == 0) {
			a.u.lo = 0xFFFFFFFFFFFFFFFFULL;
			a.u.hi--;
		} else {
			a.u.lo--;
		}
	}

	return a.f;
}

#elif JACL_LDBL_BITS == 80

static inline int __jacl_expfind_LDBL(long double x) {
	union { long double f; __jacl_ldbl80_t u; } a;

	a.f = x;

	return (a.u.exp_sign & 0x7FFF) - 16383;
}
static inline long double __jacl_expscal_LDBL(long double x, int n) {
	union { long double f; __jacl_ldbl80_t u; } a;
	int e;

	a.f = x;
	e = (a.u.exp_sign & 0x7FFF);

	if (e == 0x7FFF) return x;

	if (e == 0) {
		uint64_t mant = a.u.mantissa;

		if (mant == 0) return x;

		int lz = __jacl_clz64(mant);

		mant = (mant << (lz + 1)) | 0x8000000000000000ULL;
		e = 1 - lz;
		a.u.mantissa = mant;
	}

	e += n;

	if (e <= 0) {
		uint64_t mant = a.u.mantissa;

		if (e < -63 || mant == 0) return 0.0L * x;

		a.u.mantissa = (mant | 0x8000000000000000ULL) >> (1 - e);
		a.u.exp_sign &= 0x8000U;

		return a.f;
	}

	if (e >= 0x7FFF) return x * (1.0L / 0.0L);

	a.u.exp_sign = (a.u.exp_sign & 0x8000U) | (uint16_t)e;

	return a.f;
}
static inline long double __jacl_valnext_LDBL(long double x, int dir) {
	union { long double f; __jacl_ldbl80_t u; } a;

	if (x != x) return x;

	a.f = x;

	if (dir > 0) {
		if (a.u.mantissa == 0xFFFFFFFFFFFFFFFFULL) {
			a.u.mantissa = 0x8000000000000000ULL;
			a.u.exp_sign++;
		} else {
			a.u.mantissa++;
		}
	} else {
		if (a.u.mantissa == 0x8000000000000000ULL) {
			a.u.mantissa = 0xFFFFFFFFFFFFFFFFULL;
			a.u.exp_sign--;
		} else {
			a.u.mantissa--;
		}
	}

	return a.f;
}

#else /* JACL_LDBL_BITS = 64 */

#define __jacl_expfind_LDBL  __jacl_expfind_DBL
#define __jacl_expscal_LDBL  __jacl_expscal_DBL
#define __jacl_valnext_LDBL  __jacl_valnext_DBL

#endif /* JACL_LDBL_BITS */

// ===== PAYLOAD MANIPULATION HELPERS =======
static inline uint32_t __jacl_payloadget_FLT(float x) {
	union { float f; uint32_t u; } a;

	a.f = x;

	return a.u & FLT_PAYLOAD_MASK;
}
static inline float __jacl_payloadset_FLT(uint32_t payload, int signaling) {
	union { float f; uint32_t u; } a;

	if (signaling) {
		// Signaling NaN: quiet bit = 0
		a.u = 0x7f800000U | (payload & 0x3fffffU);

		if ((a.u & 0x7fffffU) == 0) a.u |= 1;  // Must have non-zero payload
	} else {
		// Quiet NaN: quiet bit = 1
		a.u = FLT_NAN_BASE | (payload & FLT_PAYLOAD_MASK);
	}

	return a.f;
}
static inline uint64_t __jacl_payloadget_DBL(double x) {
	union { double f; uint64_t u; } a;

	a.f = x;

	return a.u & DBL_PAYLOAD_MASK;
}
static inline double __jacl_payloadset_DBL(uint64_t payload, int signaling) {
	union { double f; uint64_t u; } a;

	if (signaling) {
		a.u = 0x7ff0000000000000ULL | (payload & 0x7ffffffffffffULL);

		if ((a.u & 0xfffffffffffffULL) == 0) a.u |= 1;
	} else {
		a.u = DBL_NAN_BASE | (payload & DBL_PAYLOAD_MASK);
	}

	return a.f;
}

#if JACL_LDBL_BITS == 128

static inline uint64_t __jacl_payloadget_LDBL(long double x) {
	union { long double f; __jacl_ldbl128_t u; } a;

	a.f = x;

	// Return only low 64 bits of payload for now
	return a.u.lo;
}
static inline long double __jacl_payloadset_LDBL(uint64_t payload, int signaling) {
	union { long double f; __jacl_ldbl128_t u; } a;

	if (signaling) {
		a.u.hi = 0x7fff000000000000ULL;
		a.u.lo = payload ? payload : 1;
	} else {
		a.u.hi = 0x7fff800000000000ULL;
		a.u.lo = payload;
	}

	return a.f;
}

#elif JACL_LDBL_BITS == 80

static inline uint64_t __jacl_payloadget_LDBL(long double x) {
	union { long double f; __jacl_ldbl80_t u; } a;

	a.f = x;

	return a.u.mantissa & LDBL_PAYLOAD_MASK;
}
static inline long double __jacl_payloadset_LDBL(uint64_t payload, int signaling) {
	union { long double f; __jacl_ldbl80_t u; } a;

	a.u.exp_sign = 0x7fff;

	if (signaling) {
		a.u.mantissa = 0x8000000000000000ULL | (payload & 0x3fffffffffffffffULL);

		if ((a.u.mantissa & 0x3fffffffffffffffULL) == 0) a.u.mantissa |= 1;
	} else {
		a.u.mantissa = 0xc000000000000000ULL | (payload & LDBL_PAYLOAD_MASK);
	}

	return a.f;
}

#else

#define __jacl_payloadget_LDBL __jacl_payloadget_DBL
#define __jacl_payloadset_LDBL __jacl_payloadset_DBL

#endif

// ======== FLOAT DIGIT EXTRACTION ==========
#if defined(__SIZEOF_INT128__) && (LDBL_MANT_DIG > 64)
	#define JACL_LDBL_INT __uint128_t
#else
	#define JACL_LDBL_INT uint64_t
#endif

static const double POW10[] = { 1e0, 1e1, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10, 1e11, 1e12, 1e13, 1e14, 1e15 };

typedef struct {
	char digits[40];   // 40 bytes - ASCII digits '0'-'9' (forward order)
	uint8_t start;     // 1 byte  - first significant digit index
	uint8_t end;       // 1 byte  - past last digit
	int16_t exp;       // 2 bytes - decimal exponent
	uint8_t sign;      // 1 byte  - 0=positive, 1=negative
	uint8_t _pad;      // 1 byte  - alignment
} __jacl_fdigits_t;  // Total: 46 bytes

static inline __jacl_fdigits_t __jacl_fdigits(long double val, int prec, int sig, int is_ldbl) {
	int exp, needed, pos = 39, max = (is_ldbl) ? LDBL_DIG : DBL_DIG;
	__jacl_fdigits_t fdigits = {0};
	JACL_LDBL_INT scaled;
	long double scale = 1.0L;

	fdigits.sign = __jacl_signget_LDBL(val);

	if (fdigits.sign) val = __jacl_signclr_LDBL(val);
	if (val == 0.0L) { fdigits.digits[0] = '0'; fdigits.end = 1; return fdigits; }
	if (val != val) { fdigits.digits[0] = 'n'; return fdigits; }
	if ((val - val) != (val - val)) { fdigits.digits[0] = 'i'; return fdigits; }

	// Normalize exponent
	while (val >= 10.0L) { val /= 10.0L; fdigits.exp++; }
	while (val < 1.0L && val > 0.0L) { val *= 10.0L; fdigits.exp--; }

	// Calculate needed digits
	if (sig > 0) {
		needed = sig;
	} else {
		// For %f: need (integer_digits + decimal_places)
		if (fdigits.exp >= 0) {
			needed = (fdigits.exp + 1) + prec;
		} else {
			// For small numbers, check if rounds to zero
			int round_pos = -(prec + 1);

			if (fdigits.exp < round_pos) {
				fdigits.digits[0] = '0';
				fdigits.end = 1;
				fdigits.exp = 0;

				return fdigits;
			} else if (fdigits.exp == round_pos) {
				if ((int)val >= 5) {
					fdigits.digits[0] = '1';
					fdigits.end = 1;
					fdigits.exp = -prec;

					return fdigits;
				}

				fdigits.digits[0] = '0';
				fdigits.end = 1;

				return fdigits;
			}

			needed = (-fdigits.exp) + prec;
		}
	}

	// Clamp to capacity
  if (needed > max) needed = max;
	if (needed < 1) needed = 1;

	exp = needed - 1;

	while (exp >= 16) { scale *= 1e16L; exp -= 16; }
	if (exp > 0) scale *= (long double)POW10[exp];

	scaled = (JACL_LDBL_INT)(val * scale + 0.5L);

	// Check for overflow from rounding
	if (scaled >= (JACL_LDBL_INT)(scale * 10.0L)) { scaled /= 10; fdigits.exp++; }
	if (scaled == 0) fdigits.digits[pos--] = '0';
	else while (scaled > 0 && pos >= 0) { fdigits.digits[pos--] = '0' + (scaled % 10); scaled /= 10; }

	fdigits.start = pos + 1;
	fdigits.end = 40;

	return fdigits;
}

#ifdef __cplusplus
}
#endif

#endif /* FLOAT_H */
