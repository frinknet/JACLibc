/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_C99
#include <stdint.h>

TEST_TYPE(unit);
TEST_UNIT(stdint.h);

/* ============================================================================
 * VERSION MACRO
 * ============================================================================ */
TEST_SUITE(version);

TEST(version_macro_defined) {
	#if JACL_HAS_C23
		ASSERT_EQ(202311L, __STDC_VERSION_STDINT_H__);
	#elif JACL_HAS_C11
		ASSERT_EQ(201112L, __STDC_VERSION_STDINT_H__);
	#elif JACL_HAS_C99
		ASSERT_EQ(199901L, __STDC_VERSION_STDINT_H__);
	#endif
}

/* ============================================================================
 * EXACT-WIDTH INTEGER TYPES
 * ============================================================================ */
TEST_SUITE(exact_width_types);

TEST(int8_t_defined) {
	int8_t val = 127;
	ASSERT_EQ(127, val);
	ASSERT_EQ(1, sizeof(int8_t));
}

TEST(uint8_t_defined) {
	uint8_t val = 255;
	ASSERT_EQ(255, val);
	ASSERT_EQ(1, sizeof(uint8_t));
}

TEST(int16_t_defined) {
	int16_t val = 32767;
	ASSERT_EQ(32767, val);
	ASSERT_EQ(2, sizeof(int16_t));
}

TEST(uint16_t_defined) {
	uint16_t val = 65535;
	ASSERT_EQ(65535, val);
	ASSERT_EQ(2, sizeof(uint16_t));
}

TEST(int32_t_defined) {
	int32_t val = 2147483647;
	ASSERT_EQ(2147483647, val);
	ASSERT_EQ(4, sizeof(int32_t));
}

TEST(uint32_t_defined) {
	uint32_t val = 4294967295U;
	ASSERT_EQ(4294967295U, val);
	ASSERT_EQ(4, sizeof(uint32_t));
}

TEST(int64_t_defined) {
	int64_t val = 9223372036854775807LL;
	ASSERT_EQ(9223372036854775807LL, val);
	ASSERT_EQ(8, sizeof(int64_t));
}

TEST(uint64_t_defined) {
	uint64_t val = 18446744073709551615ULL;
	ASSERT_EQ(18446744073709551615ULL, val);
	ASSERT_EQ(8, sizeof(uint64_t));
}

/* ============================================================================
 * EXACT-WIDTH LIMITS
 * ============================================================================ */
TEST_SUITE(exact_width_limits);

TEST(int8_limits) {
	ASSERT_EQ(-128, INT8_MIN);
	ASSERT_EQ(127, INT8_MAX);
	ASSERT_EQ(255U, UINT8_MAX);
}

TEST(int16_limits) {
	ASSERT_EQ(-32768, INT16_MIN);
	ASSERT_EQ(32767, INT16_MAX);
	ASSERT_EQ(65535U, UINT16_MAX);
}

TEST(int32_limits) {
	ASSERT_EQ(-2147483648, INT32_MIN);
	ASSERT_EQ(2147483647, INT32_MAX);
	ASSERT_EQ(4294967295U, UINT32_MAX);
}

TEST(int64_limits) {
	ASSERT_EQ(-9223372036854775807LL-1, INT64_MIN);
	ASSERT_EQ(9223372036854775807LL, INT64_MAX);
	ASSERT_EQ(18446744073709551615ULL, UINT64_MAX);
}

/* ============================================================================
 * FAST TYPES
 * ============================================================================ */
TEST_SUITE(fast_types);

TEST(int_fast8_t_defined) {
	int_fast8_t val = 100;
	ASSERT_EQ(100, val);
}

TEST(uint_fast8_t_defined) {
	uint_fast8_t val = 200;
	ASSERT_EQ(200, val);
}

TEST(int_fast16_t_defined) {
	int_fast16_t val = 30000;
	ASSERT_EQ(30000, val);
}

TEST(uint_fast16_t_defined) {
	uint_fast16_t val = 60000;
	ASSERT_EQ(60000, val);
}

TEST(int_fast32_t_defined) {
	int_fast32_t val = 1000000;
	ASSERT_EQ(1000000, val);
}

TEST(uint_fast32_t_defined) {
	uint_fast32_t val = 2000000U;
	ASSERT_EQ(2000000U, val);
}

TEST(int_fast64_t_defined) {
	int_fast64_t val = 1000000000LL;
	ASSERT_EQ(1000000000LL, val);
}

TEST(uint_fast64_t_defined) {
	uint_fast64_t val = 2000000000ULL;
	ASSERT_EQ(2000000000ULL, val);
}

/* ============================================================================
 * LEAST TYPES
 * ============================================================================ */
TEST_SUITE(least_types);

TEST(int_least8_t_defined) {
	int_least8_t val = 127;
	ASSERT_EQ(127, val);
}

TEST(uint_least8_t_defined) {
	uint_least8_t val = 255;
	ASSERT_EQ(255, val);
}

TEST(int_least16_t_defined) {
	int_least16_t val = 32767;
	ASSERT_EQ(32767, val);
}

TEST(uint_least16_t_defined) {
	uint_least16_t val = 65535;
	ASSERT_EQ(65535, val);
}

TEST(int_least32_t_defined) {
	int_least32_t val = 2147483647;
	ASSERT_EQ(2147483647, val);
}

TEST(uint_least32_t_defined) {
	uint_least32_t val = 4294967295U;
	ASSERT_EQ(4294967295U, val);
}

TEST(int_least64_t_defined) {
	int_least64_t val = 9223372036854775807LL;
	ASSERT_EQ(9223372036854775807LL, val);
}

TEST(uint_least64_t_defined) {
	uint_least64_t val = 18446744073709551615ULL;
	ASSERT_EQ(18446744073709551615ULL, val);
}

/* ============================================================================
 * POINTER TYPES
 * ============================================================================ */
TEST_SUITE(pointer_types);

TEST(intptr_t_defined) {
	int x = 42;
	intptr_t ptr = (intptr_t)&x;
	ASSERT_TRUE(ptr != 0);
}

TEST(uintptr_t_defined) {
	int x = 42;
	uintptr_t ptr = (uintptr_t)&x;
	ASSERT_TRUE(ptr != 0);
}

TEST(intptr_size_matches_pointer) {
	ASSERT_EQ(sizeof(void*), sizeof(intptr_t));
	ASSERT_EQ(sizeof(void*), sizeof(uintptr_t));
}

TEST(intptr_limits) {
	#if JACL_64BIT
		ASSERT_EQ(INT64_MIN, INTPTR_MIN);
		ASSERT_EQ(INT64_MAX, INTPTR_MAX);
		ASSERT_EQ(UINT64_MAX, UINTPTR_MAX);
	#else
		ASSERT_EQ(INT32_MIN, INTPTR_MIN);
		ASSERT_EQ(INT32_MAX, INTPTR_MAX);
		ASSERT_EQ(UINT32_MAX, UINTPTR_MAX);
	#endif
}

/* ============================================================================
 * MAX TYPES
 * ============================================================================ */
TEST_SUITE(max_types);

TEST(intmax_t_defined) {
	intmax_t val = 9223372036854775807LL;
	ASSERT_EQ(9223372036854775807LL, val);
}

TEST(uintmax_t_defined) {
	uintmax_t val = 18446744073709551615ULL;
	ASSERT_EQ(18446744073709551615ULL, val);
}

TEST(intmax_limits) {
	ASSERT_EQ(INT64_MIN, INTMAX_MIN);
	ASSERT_EQ(INT64_MAX, INTMAX_MAX);
	ASSERT_EQ(UINT64_MAX, UINTMAX_MAX);
}

/* ============================================================================
 * OTHER LIMITS
 * ============================================================================ */
TEST_SUITE(other_limits);

TEST(ptrdiff_limits) {
	#if JACL_64BIT
		ASSERT_EQ(INT64_MIN, PTRDIFF_MIN);
		ASSERT_EQ(INT64_MAX, PTRDIFF_MAX);
	#else
		ASSERT_EQ(INT32_MIN, PTRDIFF_MIN);
		ASSERT_EQ(INT32_MAX, PTRDIFF_MAX);
	#endif
}

TEST(sig_atomic_limits) {
	ASSERT_EQ(INT32_MIN, SIG_ATOMIC_MIN);
	ASSERT_EQ(INT32_MAX, SIG_ATOMIC_MAX);
}

TEST(size_max) {
	ASSERT_TRUE(SIZE_MAX > 0);
	ASSERT_EQ((size_t)-1, SIZE_MAX);
}

TEST(wchar_limits) {
	ASSERT_EQ(0, WCHAR_MIN);
	ASSERT_EQ(UINT32_MAX, WCHAR_MAX);
}

TEST(wint_limits) {
	ASSERT_EQ(0, WINT_MIN);
	ASSERT_EQ(UINT32_MAX, WINT_MAX);
}

/* ============================================================================
 * CONSTANT MACROS
 * ============================================================================ */
TEST_SUITE(constant_macros);

TEST(int8_c_macro) {
	int8_t val = INT8_C(127);
	ASSERT_EQ(127, val);
}

TEST(uint8_c_macro) {
	uint8_t val = UINT8_C(255);
	ASSERT_EQ(255, val);
}

TEST(int16_c_macro) {
	int16_t val = INT16_C(32767);
	ASSERT_EQ(32767, val);
}

TEST(uint16_c_macro) {
	uint16_t val = UINT16_C(65535);
	ASSERT_EQ(65535, val);
}

TEST(int32_c_macro) {
	int32_t val = INT32_C(2147483647);
	ASSERT_EQ(2147483647, val);
}

TEST(uint32_c_macro) {
	uint32_t val = UINT32_C(4294967295);
	ASSERT_EQ(4294967295U, val);
}

TEST(int64_c_macro) {
	int64_t val = INT64_C(9223372036854775807);
	ASSERT_EQ(9223372036854775807LL, val);
}

TEST(uint64_c_macro) {
	uint64_t val = UINT64_C(18446744073709551615);
	ASSERT_EQ(18446744073709551615ULL, val);
}

TEST(intmax_c_macro) {
	intmax_t val = INTMAX_C(9223372036854775807);
	ASSERT_EQ(9223372036854775807LL, val);
}

TEST(uintmax_c_macro) {
	uintmax_t val = UINTMAX_C(18446744073709551615);
	ASSERT_EQ(18446744073709551615ULL, val);
}

/* ============================================================================
 * TYPE SIGNEDNESS
 * ============================================================================ */
TEST_SUITE(type_signedness);

TEST(signed_types_negative) {
	int8_t i8 = -1;
	int16_t i16 = -1;
	int32_t i32 = -1;
	int64_t i64 = -1;
	
	ASSERT_TRUE(i8 < 0);
	ASSERT_TRUE(i16 < 0);
	ASSERT_TRUE(i32 < 0);
	ASSERT_TRUE(i64 < 0);
}

TEST(unsigned_types_positive) {
	uint8_t u8 = -1;
	uint16_t u16 = -1;
	uint32_t u32 = -1;
	uint64_t u64 = -1;
	
	ASSERT_TRUE(u8 > 0);
	ASSERT_TRUE(u16 > 0);
	ASSERT_TRUE(u32 > 0);
	ASSERT_TRUE(u64 > 0);
}

/* ============================================================================
 * TYPE CONVERSIONS
 * ============================================================================ */
TEST_SUITE(type_conversions);

TEST(upcast_safe) {
	int8_t small = 100;
	int16_t medium = small;
	int32_t large = medium;
	
	ASSERT_EQ(100, large);
}

TEST(downcast_truncates) {
	int32_t large = 1000;
	int8_t small = (int8_t)large;
	
	// 1000 doesn't fit in int8_t
	ASSERT_NE(1000, small);
}

TEST(pointer_roundtrip) {
	int x = 42;
	intptr_t ptr = (intptr_t)&x;
	int *restored = (int*)ptr;
	
	ASSERT_EQ(&x, restored);
	ASSERT_EQ(42, *restored);
}

/* ============================================================================
 * ARITHMETIC OPERATIONS
 * ============================================================================ */
TEST_SUITE(arithmetic);

TEST(uint8_overflow) {
	uint8_t val = 255;
	val++;
	ASSERT_EQ(0, val);
}

TEST(int8_overflow) {
	int8_t val = 127;
	val++;
	ASSERT_EQ(-128, val);
}

TEST(uint64_arithmetic) {
	uint64_t a = UINT64_C(10000000000);
	uint64_t b = UINT64_C(20000000000);
	uint64_t sum = a + b;
	
	ASSERT_EQ(UINT64_C(30000000000), sum);
}

/* ============================================================================
 * CONSISTENCY CHECKS
 * ============================================================================ */
TEST_SUITE(consistency);

TEST(type_hierarchy) {
	ASSERT_TRUE(sizeof(int8_t) <= sizeof(int16_t));
	ASSERT_TRUE(sizeof(int16_t) <= sizeof(int32_t));
	ASSERT_TRUE(sizeof(int32_t) <= sizeof(int64_t));
}

TEST(least_at_least_exact) {
	ASSERT_TRUE(sizeof(int_least8_t) >= sizeof(int8_t));
	ASSERT_TRUE(sizeof(int_least16_t) >= sizeof(int16_t));
	ASSERT_TRUE(sizeof(int_least32_t) >= sizeof(int32_t));
	ASSERT_TRUE(sizeof(int_least64_t) >= sizeof(int64_t));
}

TEST(fast_at_least_least) {
	ASSERT_TRUE(sizeof(int_fast8_t) >= sizeof(int_least8_t));
	ASSERT_TRUE(sizeof(int_fast16_t) >= sizeof(int_least16_t));
	ASSERT_TRUE(sizeof(int_fast32_t) >= sizeof(int_least32_t));
	ASSERT_TRUE(sizeof(int_fast64_t) >= sizeof(int_least64_t));
}

TEST(max_is_largest) {
	ASSERT_TRUE(sizeof(intmax_t) >= sizeof(int64_t));
	ASSERT_TRUE(sizeof(uintmax_t) >= sizeof(uint64_t));
}

TEST_MAIN()

#else

int main(void) {
	printf("stdint.h requires C99 or later\n");
	return 0;
}

#endif
