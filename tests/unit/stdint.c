/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <stdint.h>

TEST_TYPE(unit);
TEST_UNIT(stdint.h);

/* ============================================================================
 * EXACT-WIDTH TYPES
 * ============================================================================ */
TEST_SUITE(int8_t);

TEST(int8_t_basic) {
	int8_t val = 127;
	ASSERT_EQ(127, val);
	ASSERT_EQ(1, sizeof(int8_t));
}

TEST_SUITE(uint8_t);

TEST(uint8_t_basic) {
	uint8_t val = 255;
	ASSERT_EQ(255, val);
	ASSERT_EQ(1, sizeof(uint8_t));
}

TEST_SUITE(int16_t);

TEST(int16_t_basic) {
	int16_t val = 32767;
	ASSERT_EQ(32767, val);
	ASSERT_EQ(2, sizeof(int16_t));
}

TEST_SUITE(uint16_t);

TEST(uint16_t_basic) {
	uint16_t val = 65535;
	ASSERT_EQ(65535, val);
	ASSERT_EQ(2, sizeof(uint16_t));
}

TEST_SUITE(int32_t);

TEST(int32_t_basic) {
	int32_t val = 2147483647;
	ASSERT_EQ(2147483647, val);
	ASSERT_EQ(4, sizeof(int32_t));
}

TEST_SUITE(uint32_t);

TEST(uint32_t_basic) {
	uint32_t val = 4294967295U;
	ASSERT_EQ(4294967295U, val);
	ASSERT_EQ(4, sizeof(uint32_t));
}

TEST_SUITE(int64_t);

TEST(int64_t_basic) {
	int64_t val = 9223372036854775807LL;
	ASSERT_EQ(9223372036854775807LL, val);
	ASSERT_EQ(8, sizeof(int64_t));
}

TEST_SUITE(uint64_t);

TEST(uint64_t_basic) {
	uint64_t val = 18446744073709551615ULL;
	ASSERT_EQ(18446744073709551615ULL, val);
	ASSERT_EQ(8, sizeof(uint64_t));
}

/* ============================================================================
 * LIMITS
 * ============================================================================ */
TEST_SUITE(limits);

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
 * POINTER TYPES
 * ============================================================================ */
TEST_SUITE(pointer_types);

TEST(intptr_t_basic) {
	int x = 42;
	intptr_t ptr = (intptr_t)&x;
	ASSERT_TRUE(ptr != 0);
}

TEST(uintptr_t_basic) {
	int x = 42;
	uintptr_t ptr = (uintptr_t)&x;
	ASSERT_TRUE(ptr != 0);
}

TEST(intptr_size_matches_pointer) {
	ASSERT_EQ(sizeof(void*), sizeof(intptr_t));
	ASSERT_EQ(sizeof(void*), sizeof(uintptr_t));
}

/* ============================================================================
 * ARITHMETIC
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
 * TYPE CONSISTENCY
 * ============================================================================ */
TEST_SUITE(type_consistency);

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

