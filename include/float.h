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

#if JACL_HAS_FLOAT

#ifdef JACL_ARCH_FLOAT

// provide your arch float definition
#include JACL_ARCH_FLOAT

#define JACL_HAS_IEEE754 0

#else /* IEEE-754 COMPLIANT */

#define JACL_HAS_IEEE754 1

#endif /* JACL_ARCH_FLOAT */

#ifndef FLT_RADIX
#define FLT_RADIX 2
#endif

// ========= SINGLE PRECISION FLOAT =========
#ifndef FLT_TOTAL_BITS
	#define FLT_TOTAL_BITS      32
	#define FLT_TOTAL_BYTES     4
	#define FLT_MANT_DIG        24
	#define FLT_DIG             6
	#define FLT_MIN_EXP         (-125)
	#define FLT_MAX_EXP         128
	#define FLT_MIN_10_EXP      (-37)
	#define FLT_MAX_10_EXP      38
	#define FLT_MIN             1.17549435e-38F
	#define FLT_MAX             3.40282347e+38F
	#define FLT_EPSILON         1.19209290e-07F
	#define FLT_SIGN_BIT        31
	#define FLT_EXP_BITS        8
	#define FLT_EXP_MASK        0xFFU
	#define FLT_EXP_BIAS        127
	#define FLT_EXP_LSB         23
	#define FLT_EXP_MSB         30
	#define FLT_TRUE_MIN        1.40129846e-45F
	#define FLT_LOG_MAX         88.72283905206835f
	#define FLT_LOG_TRUE_MIN    -103.27892990343185f
	#define FLT_UTYPE           uint32_t
	#define FLT_NAN_BASE        0x7fc00000U
	#define FLT_PAYLOAD_MASK    0x7fffffU
	#define FLT_DECIMAL_DIG     9
	#define FLT_HAS_SUBNORM     1

	static_assert(sizeof(float) * 8 == FLT_TOTAL_BITS, "Expected IEEE-754 32-bit float");
#endif

// ========= DOUBLE PRECISION FLOAT =========
#ifndef DBL_TOTAL_BITS
	#define DBL_TOTAL_BITS      64
	#define DBL_TOTAL_BYTES     8
	#define DBL_MANT_DIG        53
	#define DBL_DIG             15
	#define DBL_MIN_EXP         (-1021)
	#define DBL_MAX_EXP         1024
	#define DBL_MIN_10_EXP      (-307)
	#define DBL_MAX_10_EXP      308
	#define DBL_MIN             2.2250738585072014e-308
	#define DBL_MAX             1.7976931348623157e+308
	#define DBL_EPSILON         2.2204460492503131e-16
	#define DBL_SIGN_BIT        63
	#define DBL_EXP_BITS        11
	#define DBL_EXP_MASK        0x7FFULL
	#define DBL_EXP_BIAS        1023
	#define DBL_EXP_LSB         52
	#define DBL_EXP_MSB         62
	#define DBL_TRUE_MIN        4.9406564584124654e-324
	#define DBL_LOG_MAX         709.782712893384
	#define DBL_LOG_TRUE_MIN    -744.4400719213812
	#define DBL_UTYPE           uint64_t
	#define DBL_NAN_BASE        0x7ff8000000000000ULL
	#define DBL_PAYLOAD_MASK    0x7ffffffffffffULL
	#define DBL_DECIMAL_DIG     17
	#define DBL_HAS_SUBNORM     1

	static_assert(sizeof(double) * 8 == DBL_TOTAL_BITS, "Expected IEEE-754 64-bit double");
#endif

// ========= LONG DOUBLE PRECISION ==========
#ifndef LDBL_TOTAL_BITS
#if JACL_LDBL_BITS == 128
	#define LDBL_TOTAL_BITS      128
	#define LDBL_TOTAL_BYTES     16
	#define LDBL_MANT_DIG        113
	#define LDBL_DIG             33
	#define LDBL_MIN_EXP         (-16381)
	#define LDBL_MAX_EXP         16384
	#define LDBL_MIN_10_EXP      (-4931)
	#define LDBL_MAX_10_EXP      4932
	#define LDBL_MIN             3.36210314311209350626267781732175260e-4932L
	#define LDBL_MAX             1.18973149535723176508575932662800702e+4932L
	#define LDBL_EPSILON         1.92592994438723585305597794258492732e-34L
	#define LDBL_SIGN_BIT        127
	#define LDBL_EXP_BITS        15
	#define LDBL_EXP_MASK        0x7FFFULL
	#define LDBL_EXP_BIAS        16383
	#define LDBL_EXP_LSB         112
	#define LDBL_EXP_MSB         126
	#define LDBL_TRUE_MIN        6.47517511943802511092443895822764655e-4966L
	#define LDBL_LOG_MAX         11356.523406294143949491L
	#define LDBL_LOG_TRUE_MIN    -11398.80538430829983554L
	#define LDBL_UTYPE           uint64_t
	#define LDBL_NAN_BASE        0x7fff8000000000000000000000000000ULL
	#define LDBL_PAYLOAD_MASK    0x7fffffffffffffffffffffffffffULL
	#define LDBL_DECIMAL_DIG     36
	#define LDBL_HAS_SUBNORM     1

	static_assert(sizeof(long double) * 8 == LDBL_TOTAL_BITS, "Expected IEEE-754 128-bit quad long double");
#elif JACL_LDBL_BITS == 80
	#define LDBL_TOTAL_BITS      80
	#define LDBL_TOTAL_BYTES     10
	#define LDBL_MANT_DIG        64
	#define LDBL_DIG             18
	#define LDBL_MIN_EXP         (-16381)
	#define LDBL_MAX_EXP         16384
	#define LDBL_MIN_10_EXP      (-4931)
	#define LDBL_MAX_10_EXP      4932
	#define LDBL_MIN             3.36210314311209350626e-4932L
	#define LDBL_MAX             1.18973149535723176502e+4932L
	#define LDBL_EPSILON         1.08420217248550443401e-19L
	#define LDBL_SIGN_BIT        79
	#define LDBL_EXP_BITS        15
	#define LDBL_EXP_MASK        0x7FFFU
	#define LDBL_EXP_BIAS        16383
	#define LDBL_EXP_LSB         64
	#define LDBL_EXP_MSB         78
	#define LDBL_TRUE_MIN        3.64519953188247460253e-4951L
	#define LDBL_LOG_MAX         11356.523406294143L
	#define LDBL_LOG_TRUE_MIN    -11398.80538430830L
	#define LDBL_UTYPE           uint64_t
	#define LDBL_NAN_BASE        0x7fff8000000000000000ULL
	#define LDBL_PAYLOAD_MASK    0x3fffffffffffffffULL
	#define LDBL_DECIMAL_DIG     21
	#define LDBL_HAS_SUBNORM     1

	static_assert(sizeof(long double) * 8 >= LDBL_TOTAL_BITS, "Expected 80-bit x87 long double");
#else /* JACL_LDBL_BITS = 64 */
	#define LDBL_TOTAL_BITS      DBL_TOTAL_BITS
	#define LDBL_TOTAL_BYTES     DBL_TOTAL_BYTES
	#define LDBL_MANT_DIG        DBL_MANT_DIG
	#define LDBL_DIG             DBL_DIG
	#define LDBL_MIN_EXP         DBL_MIN_EXP
	#define LDBL_MAX_EXP         DBL_MAX_EXP
	#define LDBL_MIN_10_EXP      DBL_MIN_10_EXP
	#define LDBL_MAX_10_EXP      DBL_MAX_10_EXP
	#define LDBL_MIN             DBL_MIN
	#define LDBL_MAX             DBL_MAX
	#define LDBL_EPSILON         DBL_EPSILON
	#define LDBL_SIGN_BIT        DBL_SIGN_BIT
	#define LDBL_EXP_BITS        DBL_EXP_BITS
	#define LDBL_EXP_MASK        DBL_EXP_MASK
	#define LDBL_EXP_BIAS        DBL_EXP_BIAS
	#define LDBL_EXP_LSB         DBL_EXP_LSB
	#define LDBL_EXP_MSB         DBL_EXP_MSB
	#define LDBL_TRUE_MIN        DBL_TRUE_MIN
	#define LDBL_LOG_MAX         DBL_LOG_MAX
	#define LDBL_LOG_TRUE_MIN    DBL_LOG_TRUE_MIN
	#define LDBL_UTYPE           DBL_UTYPE
	#define LDBL_NAN_BASE        DBL_NAN_BASE
	#define LDBL_PAYLOAD_MASK    DBL_PAYLOAD_MASK
	#define LDBL_DECIMAL_DIG     DBL_DECIMAL_DIG
	#define LDBL_HAS_SUBNORM     DBL_HAS_SUBNORM

	static_assert(sizeof(long double) * 8 == LDBL_TOTAL_BITS, "Expected long double == double");
#endif /* JACL_LDBL_BITS */
#endif /* LDBL_TOTAL_BITS */

// ======= FLOAT DECONSTRUCTION TYPE ========
#ifdef JACL_ARCH_FLOAT

// use structure from arch float

#elif JACL_LDBL_BITS == 128

typedef struct {
	LDBL_UTYPE lo;
	LDBL_UTYPE hi;
}__jacl_ldbl128_t;

#elif JACL_LDBL_BITS == 80

typedef struct __attribute__((packed)) {
	LDBL_UTYPE mantissa;
	uint16_t exp_sign;
} __jacl_ldbl80_t;

#endif

// ======== GENERAL FLOAT PRECISION =========
#define DECIMAL_DIG  LDBL_DECIMAL_DIG
#define FLT_ROUNDS   1

#if JACL_HAS_C99
	#define FLT_EVAL_METHOD 0
#endif

// ======= SING MANIPULATION HELPERS ========
static inline int __jacl_signget_FLT(float x) {
	union { float f; FLT_UTYPE u; } a;

	a.f = x;

	return (a.u >> FLT_SIGN_BIT);
}
static inline float __jacl_signclr_FLT(float x) {
	union { float f; FLT_UTYPE u; } a;

	a.f = x;
	a.u &= ~(1U << FLT_SIGN_BIT);

	return a.f;
}
static inline float __jacl_signset_FLT(float x) {
	union { float f; FLT_UTYPE u; } a;

	a.f = x;
	a.u |= (1U << FLT_SIGN_BIT);

	return a.f;
}
static inline float __jacl_signcpy_FLT(float x, float y) {
	union { float f; FLT_UTYPE u; } a, b;

	a.f = x;
	b.f = y;
	a.u = (a.u & ~(1U << FLT_SIGN_BIT)) | (b.u & (1U << FLT_SIGN_BIT));

	return a.f;
}

static inline int __jacl_signget_DBL(double x) {
	union { double f; DBL_UTYPE u; } a;

	a.f = x;

	return (a.u >> DBL_SIGN_BIT);
}
static inline double __jacl_signclr_DBL(double x) {
	union { double f; DBL_UTYPE u; } a;

	a.f = x;
	a.u &= ~(1ULL << DBL_SIGN_BIT);

	return a.f;
}
static inline double __jacl_signset_DBL(double x) {
	union { double f; DBL_UTYPE u; } a;

	a.f = x;
	a.u |= (1ULL << DBL_SIGN_BIT);

	return a.f;
}
static inline double __jacl_signcpy_DBL(double x, double y) {
	union { double f; DBL_UTYPE u; } a, b;

	a.f = x;
	b.f = y;
	a.u = (a.u & ~(1ULL << DBL_SIGN_BIT)) | (b.u & (1ULL << DBL_SIGN_BIT));

	return a.f;
}

#if JACL_LDBL_BITS == 128

static inline int __jacl_signget_LDBL(long double x) {
	union { long double f; __jacl_ldbl128_t u; } a;

	a.f = x;

	return (a.u.hi >> LDBL_SIGN_BIT - 64);
}
static inline long double __jacl_signclr_LDBL(long double x) {
	union { long double f; __jacl_ldbl128_t u; } a;

	a.f = x;
	a.u.hi &= ~(1ULL << (LDBL_SIGN_BIT - 64));

	return a.f;
}
static inline long double __jacl_signset_LDBL(long double x) {
	union { long double f; __jacl_ldbl128_t u; } a;

	a.f = x;
	a.u.hi |= (1ULL << (LDBL_SIGN_BIT - 64));

	return a.f;
}
static inline long double __jacl_signcpy_LDBL(long double x, long double y) {
	union { long double f; __jacl_ldbl128_t u; } a, b;

	a.f = x;
	b.f = y;
	a.u.hi = (a.u.hi & ~(1ULL << (LDBL_SIGN_BIT - 64))) | (b.u.hi & (1ULL << (LDBL_SIGN_BIT - 64)));

	return a.f;
}

#elif JACL_LDBL_BITS == 80

static inline int __jacl_signget_LDBL(long double x) {
	union { long double f; __jacl_ldbl80_t u; } a;

	a.f = x;

	return (a.u.exp_sign >> (LDBL_SIGN_BIT - 64));
}
static inline long double __jacl_signclr_LDBL(long double x) {
	union { long double f; __jacl_ldbl80_t u; } a;

	a.f = x;
	a.u.exp_sign &= ~(1U << (LDBL_SIGN_BIT - 64));

	return a.f;
}
static inline long double __jacl_signset_LDBL(long double x) {
	union { long double f; __jacl_ldbl80_t u; } a;

	a.f = x;
	a.u.exp_sign |= (1U << (LDBL_SIGN_BIT - 64));

	return a.f;
}
static inline long double __jacl_signcpy_LDBL(long double x, long double y) {
	union { long double f; __jacl_ldbl80_t u; } a, b;

	a.f = x;
	b.f = y;
	a.u.exp_sign = (a.u.exp_sign & ~(1U << (LDBL_SIGN_BIT - 64))) | (b.u.exp_sign & (1U << (LDBL_SIGN_BIT - 64)));

	return a.f;
}

#else

#define __jacl_signget_LDBL  __jacl_signget_DBL
#define __jacl_signclr_LDBL  __jacl_signclr_DBL
#define __jacl_signset_LDBL  __jacl_signset_DBL
#define __jacl_signcpy_LDBL  __jacl_signcpy_DBL

#endif

// ===== MANTISSA MANIPULATION HELPERS ======
static inline FLT_UTYPE __jacl_mantget_FLT(float x) {
	union { float f; FLT_UTYPE u; } a;

	a.f = x;

	return a.u & ((1U << FLT_EXP_LSB) - 1);
}
static inline int __jacl_mantctz_FLT(float x) {
	FLT_UTYPE m = __jacl_mantget_FLT(x);

	return m ? __jacl_ctz32(m) : FLT_EXP_LSB;
}

static inline DBL_UTYPE __jacl_mantget_DBL(double x) {
	union { double f; DBL_UTYPE u; } a;

	a.f = x;

	return a.u & ((1ULL << DBL_EXP_LSB) - 1);
}
static inline int __jacl_mantctz_DBL(double x) {
	DBL_UTYPE m = __jacl_mantget_DBL(x);

	return m ? __jacl_ctz64(m) : DBL_EXP_LSB;
}

#if JACL_LDBL_BITS == 128

static inline __jacl_ldbl128_t __jacl_mantget_LDBL(long double x) {
	union { long double f; __jacl_ldbl128_t u; } a;

	a.f = x;

	// Extract mantissa: all of .lo, and lower bits of .hi (mask off exponent/sign)
	return (__jacl_ldbl128_t){
		.lo = a.u.lo,
		.hi = a.u.hi & ((1ULL << (LDBL_EXP_LSB - 64)) - 1)
	};
}
static inline int __jacl_mantctz_LDBL(long double x) {
	union { long double f; __jacl_ldbl128_t u; } a;

	a.f = x;

	if (a.u.lo) {
		// Trailing zeros in lower 64 bits
		return __jacl_ctz64(a.u.lo);
	} else {
		// Lower 64 bits are zero; check upper mantissa bits
		DBL_UTYPE hi_mant = a.u.hi & ((1ULL << (LDBL_EXP_LSB - 64)) - 1);

		return hi_mant ? 64 + __jacl_ctz64(hi_mant) : (LDBL_MANT_DIG - 1);
	}
}

#elif JACL_LDBL_BITS == 80

static inline DBL_UTYPE __jacl_mantget_LDBL(long double x) {
	union { long double f; __jacl_ldbl80_t u; } a;

	a.f = x;

	return a.u.mantissa;
}
static inline int __jacl_mantctz_LDBL(long double x) {
	union { long double f; __jacl_ldbl80_t u; } a;

	a.f = x;

	return a.u.mantissa ? __jacl_ctz64(a.u.mantissa) : (LDBL_MANT_DIG);
}

#else  /* JACL_LDBL_BITS == 64 */

#define __jacl_mantget_LDBL __jacl_mantget_DBL
#define __jacl_mantctz_LDBL __jacl_mantctz_DBL

#endif /* JACL_LDBL_BITS */

// ===== EXPONENT MANIPULATION HELPERS ======
static inline int __jacl_expfind_FLT(float x) {
	union { float f; FLT_UTYPE u; } a;

	a.f = x;

	return ((a.u >> FLT_EXP_LSB) & FLT_EXP_MASK) - FLT_EXP_BIAS;
}
static inline float __jacl_expscal_FLT(float x, int n) {
	union { float f; FLT_UTYPE u; } a;
	int e;

	a.f = x;
	e = ((a.u >> FLT_EXP_LSB) & FLT_EXP_MASK);

	// Infinity/NaN
	if (e == FLT_EXP_MASK) return x;

	// Subnormal/zero normalization
	if (e == 0) {
		FLT_UTYPE mant = a.u & ((1U << FLT_EXP_LSB) - 1);

		if (mant == 0) return x;

		int lz = __jacl_clz32(mant) - (FLT_TOTAL_BITS - FLT_MANT_DIG);

		mant = (mant << (lz + 1)) & ((1U << FLT_EXP_LSB) - 1);
		e = 1 - lz;
		a.u = (a.u & (1U << FLT_SIGN_BIT)) | mant;
	}

	e += n;

	// Subnormal result
	if (e <= 0) {
		FLT_UTYPE mant = a.u & ((1U << FLT_EXP_LSB) - 1);

		if (e < -FLT_EXP_LSB || mant == 0) return 0.0f * x;

		a.u = (a.u & (1U << FLT_SIGN_BIT)) | (((mant | (1U << FLT_EXP_LSB)) >> (1 - e)));

		return a.f;
	}

	// Overflow to infinity
	if (e >= FLT_EXP_MASK) return x * (1.0f / 0.0f);

	a.u = (a.u & ~(FLT_EXP_MASK << FLT_EXP_LSB)) | ((FLT_UTYPE)e << FLT_EXP_LSB);

	return a.f;
}
static inline float __jacl_valnext_FLT(float x, int dir) {
	union { float f; FLT_UTYPE u; } a;

	if (x != x) return x;

	a.f = x;

	if (dir > 0) a.u++;
	else a.u--;

	return a.f;
}

static inline int __jacl_expfind_DBL(double x) {
	union { double f; DBL_UTYPE u; } a;

	a.f = x;

	return ((a.u >> DBL_EXP_LSB) & DBL_EXP_MASK) - DBL_EXP_BIAS;
}
static inline double __jacl_expscal_DBL(double x, int n) {
	union { double f; DBL_UTYPE u; } a;
	int e;

	a.f = x;
	e = ((a.u >> DBL_EXP_LSB) & DBL_EXP_MASK);

	// Infinity/NaN case
	if (e == DBL_EXP_MASK) return x;

	// Subnormal/zero normalization
	if (e == 0) {
		DBL_UTYPE mant = a.u & ((1ULL << DBL_EXP_LSB) - 1);

		if (mant == 0) return x;

		int lz = __jacl_clz64(mant) - (DBL_TOTAL_BITS - DBL_MANT_DIG);

		mant = (mant << (lz + 1)) & ((1ULL << DBL_EXP_LSB) - 1);
		e = 1 - lz;
		a.u = (a.u & (1ULL << DBL_SIGN_BIT)) | mant;
	}

	e += n;

	// Subnormal result
	if (e <= 0) {
		DBL_UTYPE mant = a.u & ((1ULL << DBL_EXP_LSB) - 1);

		// Underflow to zero
		if (e < -DBL_EXP_LSB || mant == 0) return 0.0 * x;

		a.u = (a.u & (1ULL << DBL_SIGN_BIT)) | (((mant | (1ULL << DBL_EXP_LSB)) >> (1 - e)));

		return a.f;
	}

	// Overflow to infinity
	if (e >= DBL_EXP_MASK) return x * (1.0 / 0.0);

	a.u = (a.u & ~(DBL_EXP_MASK << DBL_EXP_LSB)) | ((DBL_UTYPE)e << DBL_EXP_LSB);

	return a.f;
}
static inline double __jacl_valnext_DBL(double x, int dir) {
	union { double f; DBL_UTYPE u; } a;

	if (x != x) return x;

	a.f = x;

	if (dir > 0) a.u++; else a.u--;

	return a.f;
}

#if JACL_LDBL_BITS == 128

static inline int __jacl_expfind_LDBL(long double x) {
	union { long double f; __jacl_ldbl128_t u; } a;

	a.f = x;

	return ((a.u.hi >> (LDBL_EXP_LSB - 64)) & LDBL_EXP_MASK) - LDBL_EXP_BIAS;
}
static inline long double __jacl_expscal_LDBL(long double x, int n) {
	union { long double f; __jacl_ldbl128_t u; } a;
	int e;

	a.f = x;
	e = ((a.u.hi >> (LDBL_EXP_LSB - 64)) & LDBL_EXP_MASK);

	// Infinity/NaN
	if (e == LDBL_EXP_MASK) return x;

	// Subnormal/zero normalization
	if (e == 0) {
		LDBL_UTYPE mant_hi = a.u.hi & ((1ULL << (LDBL_EXP_LSB - 64)) - 1);
		LDBL_UTYPE mant_lo = a.u.lo;
		int lz;

		if (mant_hi == 0 && mant_lo == 0) return x;
		if (mant_hi) lz = __jacl_clz64(mant_hi) - (128 - LDBL_MANT_DIG);
		else lz = __jacl_clz64(mant_lo) + (LDBL_EXP_LSB - 64);

		// Left-justify mantissa
		if (lz < 64) {
			mant_hi = (mant_hi << (lz + 1)) | (mant_lo >> (63 - lz));
			mant_lo = mant_lo << (lz + 1);
		} else {
			mant_hi = mant_lo << (lz - 63);
			mant_lo = 0;
		}

		e = 1 - lz;
		a.u.hi = (a.u.hi & (1ULL << (LDBL_SIGN_BIT - 64))) | (mant_hi & ((1ULL << (LDBL_EXP_LSB - 64)) - 1));
		a.u.lo = mant_lo;
	}

	e += n;

	// Underflow to zero
	if (e <= 0) {
		LDBL_UTYPE sign = a.u.hi & (1ULL << (LDBL_SIGN_BIT - 64));
		LDBL_UTYPE mant_hi = a.u.hi & ((1ULL << (LDBL_EXP_LSB - 64)) - 1);
		LDBL_UTYPE mant_lo = a.u.lo;

		int shift;

		if (e < -LDBL_EXP_LSB || (mant_hi == 0 && mant_lo == 0)) return 0.0L * x;

		mant_hi |= (1ULL << (LDBL_EXP_LSB - 64));
		shift = 1 - e;

		if (shift < 64) {
			a.u.lo = (mant_lo >> shift) | (mant_hi << (64 - shift));
			a.u.hi = (mant_hi >> shift) | sign;
		} else if (shift < (LDBL_MANT_DIG - 1)) {
			a.u.lo = mant_hi >> (shift - 64);
			a.u.hi = sign;
		} else {
			return 0.0L * x;
		}

		return a.f;
	}

	// Overflow to infinity
	if (e >= LDBL_EXP_MASK) return x * (1.0L / 0.0L);

	a.u.hi = (a.u.hi & ~((LDBL_EXP_MASK << (LDBL_EXP_LSB - 64)))) | ((LDBL_UTYPE)e << (LDBL_EXP_LSB - 64));

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

	return (a.u.exp_sign & LDBL_EXP_MASK) - LDBL_EXP_BIAS;
}
static inline long double __jacl_expscal_LDBL(long double x, int n) {
	union { long double f; __jacl_ldbl80_t u; } a;
	int e;

	a.f = x;
	e = (a.u.exp_sign & LDBL_EXP_MASK);

	// Infinity/NaN: all exponent bits set
	if (e == LDBL_EXP_MASK) return x;

	// Subnormal/zero normalization
	if (e == 0) {
		LDBL_UTYPE mant = a.u.mantissa;

		if (mant == 0) return x;

		int lz = __jacl_clz64(mant);

		// Normalize: shift up, set hidden bit, set min exponent
		mant = (mant << (lz + 1)) | (1ULL << (LDBL_MANT_DIG - 1));
		e = 1 - lz;
		a.u.mantissa = mant;
	}

	// Adjust exponent
	e += n;

	// Underflow: zero or denormalized
	if (e <= 0) {
		LDBL_UTYPE mant = a.u.mantissa;

		if (e < -LDBL_MANT_DIG || mant == 0) return 0.0L * x;

		a.u.mantissa = (mant | (1ULL << (LDBL_MANT_DIG - 1))) >> (1 - e);
		a.u.exp_sign &= (1U << (LDBL_SIGN_BIT - 64));  // keep sign only

		return a.f;
	}

	// Overflow to infinity
	if (e >= LDBL_EXP_MASK) return x * (1.0L / 0.0L);

	// Insert new exponent (preserve sign)
	a.u.exp_sign = (a.u.exp_sign & (1U << (LDBL_SIGN_BIT - 64))) | (uint16_t)e;

	return a.f;
}
static inline long double __jacl_valnext_LDBL(long double x, int dir) {
	union { long double f; __jacl_ldbl80_t u; } a;

	if (x != x) return x;

	a.f = x;

	if (dir > 0) {
		if (a.u.mantissa == UINT64_MAX) {
			a.u.mantissa = 1ULL << (LDBL_MANT_DIG - 1);
			a.u.exp_sign++;
		} else {
			a.u.mantissa++;
		}
	} else {
		if (a.u.mantissa == (1ULL << (LDBL_MANT_DIG - 1))) {
			a.u.mantissa = UINT64_MAX;
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
static inline FLT_UTYPE __jacl_payloadget_FLT(float x) {
	union { float f; FLT_UTYPE u; } a;

	a.f = x;

	return a.u & FLT_PAYLOAD_MASK;
}
static inline float __jacl_payloadset_FLT(FLT_UTYPE payload, int signaling) {
	union { float f; FLT_UTYPE u; } a;

	if (signaling) {
		// Signaling NaN: exponent all 1s, quiet bit = 0
		a.u = (FLT_EXP_MASK << FLT_EXP_LSB) | (payload & ((1U << (FLT_EXP_LSB - 1)) - 1));

		// Must have non-zero payload or it's infinity, not NaN
		if ((a.u & ((1U << FLT_EXP_LSB) - 1)) == 0) a.u |= 1;
	} else {
		// Quiet NaN: use canonical base and payload
		a.u = FLT_NAN_BASE | (payload & FLT_PAYLOAD_MASK);
	}

	return a.f;
}

static inline DBL_UTYPE __jacl_payloadget_DBL(double x) {
	union { double f; DBL_UTYPE u; } a;

	a.f = x;

	return a.u & DBL_PAYLOAD_MASK;
}
static inline double __jacl_payloadset_DBL(DBL_UTYPE payload, int signaling) {
	union { double f; DBL_UTYPE u; } a;

	if (signaling) {
		// Signaling NaN: exponent = all 1s, quiet bit = 0
		a.u = (DBL_EXP_MASK << DBL_EXP_LSB) | (payload & ((1ULL << (DBL_EXP_LSB - 1)) - 1));

		// Must have non-zero payload or it's infinity, not NaN
		if ((a.u & DBL_PAYLOAD_MASK) == 0) a.u |= 1;
	} else {
		// Quiet NaN: use canonical base and payload
		a.u = DBL_NAN_BASE | (payload & DBL_PAYLOAD_MASK);
	}

	return a.f;
}

#if JACL_LDBL_BITS == 128

static inline DBL_UTYPE __jacl_payloadget_LDBL(long double x) {
	union { long double f; __jacl_ldbl128_t u; } a;

	a.f = x;

	// Return only low 64 bits of payload for now
	return a.u.lo;
}
static inline long double __jacl_payloadset_LDBL(DBL_UTYPE payload, int signaling) {
	union { long double f; __jacl_ldbl128_t u; } a;

	if (signaling) {
		a.u.hi = (LDBL_EXP_MASK << (LDBL_EXP_LSB - 64));
		a.u.lo = payload ? payload : 1;
	} else {
		a.u.hi = (LDBL_EXP_MASK << (LDBL_EXP_LSB - 64)) | (1ULL << (LDBL_EXP_LSB - 65));
		a.u.lo = payload;
	}

	return a.f;
}

#elif JACL_LDBL_BITS == 80

static inline DBL_UTYPE __jacl_payloadget_LDBL(long double x) {
	union { long double f; __jacl_ldbl80_t u; } a;

	a.f = x;

	return a.u.mantissa & LDBL_PAYLOAD_MASK;
}
static inline long double __jacl_payloadset_LDBL(DBL_UTYPE payload, int signaling) {
	union { long double f; __jacl_ldbl80_t u; } a;

	a.u.exp_sign = (LDBL_EXP_MASK);

	if (signaling) {
		a.u.mantissa = (1ULL << (LDBL_MANT_DIG - 1)) | (payload & ((1ULL << (LDBL_MANT_DIG - 1)) - 1));

		if ((a.u.mantissa & ((1ULL << (LDBL_MANT_DIG - 1)) - 1)) == 0) a.u.mantissa |= 1;
	} else {
		a.u.mantissa = (3ULL << (LDBL_MANT_DIG - 2)) | (payload & LDBL_PAYLOAD_MASK);
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
	#define JACL_LDBL_INT LDBL_UTYPE
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

#endif /* JACL_HAS_FLOAT */

#ifdef __cplusplus
}
#endif

#endif /* FLOAT_H */
