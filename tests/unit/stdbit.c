/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <stdbit.h>

TEST_TYPE(unit);
TEST_UNIT(stdbit.h);

#if JACL_HAS_C23

/* ============================================================================
 * ENDIANNESS MACROS
 * ============================================================================ */
TEST_SUITE(endianness);

TEST(endian_macros_defined) {
	// At least one should be set
	ASSERT_TRUE(__STDC_ENDIAN_BIG__ == 0 || __STDC_ENDIAN_BIG__ == 1);
	ASSERT_TRUE(__STDC_ENDIAN_LITTLE__ == 0 || __STDC_ENDIAN_LITTLE__ == 1);
}

TEST(endian_exclusive) {
	// Can't be both big and little endian
	ASSERT_FALSE(__STDC_ENDIAN_BIG__ && __STDC_ENDIAN_LITTLE__);
}

TEST(endian_native) {
	if (__STDC_ENDIAN_BIG__) {
		ASSERT_EQ(4321, __STDC_ENDIAN_NATIVE__);
	} else {
		ASSERT_EQ(1234, __STDC_ENDIAN_NATIVE__);
	}
}

/* ============================================================================
 * LEADING ZEROS TESTS
 * ============================================================================ */
TEST_SUITE(leading_zeros);

TEST(clz_zero) {
	ASSERT_EQ(32, stdc_leading_zeros(0U));
	ASSERT_EQ(16, stdc_leading_zeros((unsigned short)0));
	ASSERT_EQ(8, stdc_leading_zeros((unsigned char)0));
}

TEST(clz_one) {
	ASSERT_EQ(31, stdc_leading_zeros(1U));
}

TEST(clz_powers_of_two) {
	ASSERT_EQ(30, stdc_leading_zeros(2U));
	ASSERT_EQ(29, stdc_leading_zeros(4U));
	ASSERT_EQ(28, stdc_leading_zeros(8U));
	ASSERT_EQ(27, stdc_leading_zeros(16U));
	ASSERT_EQ(0, stdc_leading_zeros(0x80000000U));
}

TEST(clz_max_values) {
	ASSERT_EQ(0, stdc_leading_zeros(UINT32_MAX));
	ASSERT_EQ(0, stdc_leading_zeros((unsigned short)UINT16_MAX));
	ASSERT_EQ(0, stdc_leading_zeros((unsigned char)UINT8_MAX));
}

TEST(clz_mixed_bits) {
	ASSERT_EQ(1, stdc_leading_zeros(0x7FFFFFFFU));
	ASSERT_EQ(2, stdc_leading_zeros(0x3FFFFFFFU));
	ASSERT_EQ(16, stdc_leading_zeros(0x0000FFFFU));
}

/* ============================================================================
 * LEADING ONES TESTS
 * ============================================================================ */
TEST_SUITE(leading_ones);

TEST(clo_zero) {
	ASSERT_EQ(0, stdc_leading_ones(0U));
}

TEST(clo_max) {
	ASSERT_EQ(32, stdc_leading_ones(UINT32_MAX));
	ASSERT_EQ(16, stdc_leading_ones((unsigned short)UINT16_MAX));
	ASSERT_EQ(8, stdc_leading_ones((unsigned char)UINT8_MAX));
}

TEST(clo_high_bits) {
	ASSERT_EQ(1, stdc_leading_ones(0x80000000U));
	ASSERT_EQ(2, stdc_leading_ones(0xC0000000U));
	ASSERT_EQ(8, stdc_leading_ones(0xFF000000U));
}

TEST(clo_mixed) {
	ASSERT_EQ(16, stdc_leading_ones(0xFFFF0000U));
	ASSERT_EQ(24, stdc_leading_ones(0xFFFFFF00U));
}

/* ============================================================================
 * TRAILING ZEROS TESTS
 * ============================================================================ */
TEST_SUITE(trailing_zeros);

TEST(ctz_zero) {
	ASSERT_EQ(32, stdc_trailing_zeros(0U));
	ASSERT_EQ(16, stdc_trailing_zeros((unsigned short)0));
	ASSERT_EQ(8, stdc_trailing_zeros((unsigned char)0));
}

TEST(ctz_one) {
	ASSERT_EQ(0, stdc_trailing_zeros(1U));
}

TEST(ctz_powers_of_two) {
	ASSERT_EQ(1, stdc_trailing_zeros(2U));
	ASSERT_EQ(2, stdc_trailing_zeros(4U));
	ASSERT_EQ(3, stdc_trailing_zeros(8U));
	ASSERT_EQ(4, stdc_trailing_zeros(16U));
	ASSERT_EQ(31, stdc_trailing_zeros(0x80000000U));
}

TEST(ctz_even_numbers) {
	ASSERT_EQ(1, stdc_trailing_zeros(6U));   // 0b110
	ASSERT_EQ(2, stdc_trailing_zeros(12U));  // 0b1100
	ASSERT_EQ(3, stdc_trailing_zeros(24U));  // 0b11000
}

TEST(ctz_mixed_bits) {
	ASSERT_EQ(0, stdc_trailing_zeros(0xFFFFFFFFU));
	ASSERT_EQ(8, stdc_trailing_zeros(0x0000FF00U));
	ASSERT_EQ(16, stdc_trailing_zeros(0xFFFF0000U));
}

/* ============================================================================
 * TRAILING ONES TESTS
 * ============================================================================ */
TEST_SUITE(trailing_ones);

TEST(cto_zero) {
	ASSERT_EQ(0, stdc_trailing_ones(0U));
}

TEST(cto_max) {
	ASSERT_EQ(32, stdc_trailing_ones(UINT32_MAX));
	ASSERT_EQ(16, stdc_trailing_ones((unsigned short)UINT16_MAX));
	ASSERT_EQ(8, stdc_trailing_ones((unsigned char)UINT8_MAX));
}

TEST(cto_low_bits) {
	ASSERT_EQ(1, stdc_trailing_ones(1U));
	ASSERT_EQ(2, stdc_trailing_ones(3U));
	ASSERT_EQ(3, stdc_trailing_ones(7U));
	ASSERT_EQ(4, stdc_trailing_ones(15U));
}

TEST(cto_mixed) {
	ASSERT_EQ(8, stdc_trailing_ones(0x000000FFU));
	ASSERT_EQ(16, stdc_trailing_ones(0x0000FFFFU));
}

/* ============================================================================
 * COUNT ONES (POPCOUNT) TESTS
 * ============================================================================ */
TEST_SUITE(count_ones);

TEST(popcount_zero) {
	ASSERT_EQ(0, stdc_count_ones(0U));
}

TEST(popcount_one) {
	ASSERT_EQ(1, stdc_count_ones(1U));
}

TEST(popcount_max) {
	ASSERT_EQ(32, stdc_count_ones(UINT32_MAX));
	ASSERT_EQ(16, stdc_count_ones((unsigned short)UINT16_MAX));
	ASSERT_EQ(8, stdc_count_ones((unsigned char)UINT8_MAX));
}

TEST(popcount_powers_of_two) {
	ASSERT_EQ(1, stdc_count_ones(1U));
	ASSERT_EQ(1, stdc_count_ones(2U));
	ASSERT_EQ(1, stdc_count_ones(4U));
	ASSERT_EQ(1, stdc_count_ones(8U));
	ASSERT_EQ(1, stdc_count_ones(0x80000000U));
}

TEST(popcount_alternating) {
	ASSERT_EQ(16, stdc_count_ones(0xAAAAAAAAU));
	ASSERT_EQ(16, stdc_count_ones(0x55555555U));
}

TEST(popcount_mixed) {
	ASSERT_EQ(8, stdc_count_ones(0x000000FFU));
	ASSERT_EQ(16, stdc_count_ones(0x0000FFFFU));
	ASSERT_EQ(24, stdc_count_ones(0x00FFFFFFU));
}

/* ============================================================================
 * COUNT ZEROS TESTS
 * ============================================================================ */
TEST_SUITE(count_zeros);

TEST(count_zeros_zero) {
	ASSERT_EQ(32, stdc_count_zeros(0U));
}

TEST(count_zeros_max) {
	ASSERT_EQ(0, stdc_count_zeros(UINT32_MAX));
}

TEST(count_zeros_powers_of_two) {
	ASSERT_EQ(31, stdc_count_zeros(1U));
	ASSERT_EQ(31, stdc_count_zeros(2U));
	ASSERT_EQ(31, stdc_count_zeros(0x80000000U));
}

TEST(count_zeros_consistency) {
	unsigned int val = 0x12345678U;
	ASSERT_EQ(32, stdc_count_ones(val) + stdc_count_zeros(val));
}

/* ============================================================================
 * BIT WIDTH TESTS
 * ============================================================================ */
TEST_SUITE(bit_width);

TEST(bit_width_zero) {
	ASSERT_EQ(0, stdc_bit_width(0U));
}

TEST(bit_width_one) {
	ASSERT_EQ(1, stdc_bit_width(1U));
}

TEST(bit_width_powers_of_two) {
	ASSERT_EQ(2, stdc_bit_width(2U));
	ASSERT_EQ(3, stdc_bit_width(4U));
	ASSERT_EQ(4, stdc_bit_width(8U));
	ASSERT_EQ(5, stdc_bit_width(16U));
	ASSERT_EQ(32, stdc_bit_width(0x80000000U));
}

TEST(bit_width_ranges) {
	ASSERT_EQ(8, stdc_bit_width(255U));
	ASSERT_EQ(9, stdc_bit_width(256U));
	ASSERT_EQ(16, stdc_bit_width(65535U));
	ASSERT_EQ(17, stdc_bit_width(65536U));
}

TEST(bit_width_max) {
	ASSERT_EQ(32, stdc_bit_width(UINT32_MAX));
	ASSERT_EQ(16, stdc_bit_width((unsigned short)UINT16_MAX));
	ASSERT_EQ(8, stdc_bit_width((unsigned char)UINT8_MAX));
}

/* ============================================================================
 * BIT FLOOR TESTS
 * ============================================================================ */
TEST_SUITE(bit_floor);

TEST(bit_floor_zero) {
	ASSERT_EQ(0U, stdc_bit_floor(0U));
}

TEST(bit_floor_one) {
	ASSERT_EQ(1U, stdc_bit_floor(1U));
}

TEST(bit_floor_powers_of_two) {
	ASSERT_EQ(2U, stdc_bit_floor(2U));
	ASSERT_EQ(4U, stdc_bit_floor(4U));
	ASSERT_EQ(8U, stdc_bit_floor(8U));
	ASSERT_EQ(16U, stdc_bit_floor(16U));
}

TEST(bit_floor_non_powers) {
	ASSERT_EQ(2U, stdc_bit_floor(3U));
	ASSERT_EQ(4U, stdc_bit_floor(5U));
	ASSERT_EQ(4U, stdc_bit_floor(7U));
	ASSERT_EQ(8U, stdc_bit_floor(15U));
}

TEST(bit_floor_large) {
	ASSERT_EQ(128U, stdc_bit_floor(255U));
	ASSERT_EQ(0x80000000U, stdc_bit_floor(UINT32_MAX));
}

/* ============================================================================
 * BIT CEIL TESTS
 * ============================================================================ */
TEST_SUITE(bit_ceil);

TEST(bit_ceil_zero) {
	ASSERT_EQ(0U, stdc_bit_ceil(0U));
}

TEST(bit_ceil_one) {
	ASSERT_EQ(1U, stdc_bit_ceil(1U));
}

TEST(bit_ceil_powers_of_two) {
	ASSERT_EQ(2U, stdc_bit_ceil(2U));
	ASSERT_EQ(4U, stdc_bit_ceil(4U));
	ASSERT_EQ(8U, stdc_bit_ceil(8U));
	ASSERT_EQ(16U, stdc_bit_ceil(16U));
}

TEST(bit_ceil_non_powers) {
	ASSERT_EQ(4U, stdc_bit_ceil(3U));
	ASSERT_EQ(8U, stdc_bit_ceil(5U));
	ASSERT_EQ(8U, stdc_bit_ceil(7U));
	ASSERT_EQ(16U, stdc_bit_ceil(9U));
}

TEST(bit_ceil_large) {
	ASSERT_EQ(256U, stdc_bit_ceil(255U));
	ASSERT_EQ(256U, stdc_bit_ceil(256U));
}

/* ============================================================================
 * HAS SINGLE BIT (POWER OF 2) TESTS
 * ============================================================================ */
TEST_SUITE(has_single_bit);

TEST(has_single_bit_zero) {
	ASSERT_FALSE(stdc_has_single_bit(0U));
}

TEST(has_single_bit_one) {
	ASSERT_TRUE(stdc_has_single_bit(1U));
}

TEST(has_single_bit_powers_of_two) {
	ASSERT_TRUE(stdc_has_single_bit(2U));
	ASSERT_TRUE(stdc_has_single_bit(4U));
	ASSERT_TRUE(stdc_has_single_bit(8U));
	ASSERT_TRUE(stdc_has_single_bit(16U));
	ASSERT_TRUE(stdc_has_single_bit(0x80000000U));
}

TEST(has_single_bit_non_powers) {
	ASSERT_FALSE(stdc_has_single_bit(3U));
	ASSERT_FALSE(stdc_has_single_bit(5U));
	ASSERT_FALSE(stdc_has_single_bit(6U));
	ASSERT_FALSE(stdc_has_single_bit(7U));
	ASSERT_FALSE(stdc_has_single_bit(15U));
}

TEST(has_single_bit_max) {
	ASSERT_FALSE(stdc_has_single_bit(UINT32_MAX));
}

/* ============================================================================
 * TYPE-SPECIFIC TESTS
 * ============================================================================ */
TEST_SUITE(type_specific);

TEST(unsigned_char_operations) {
	unsigned char val = 0xF0;
	
	ASSERT_EQ(4, stdc_count_ones(val));
	ASSERT_EQ(0, stdc_leading_zeros(val));
	ASSERT_EQ(4, stdc_trailing_zeros(val));
	ASSERT_EQ(8, stdc_bit_width(val));
}

TEST(unsigned_short_operations) {
	unsigned short val = 0xF00F;
	
	ASSERT_EQ(8, stdc_count_ones(val));
	ASSERT_EQ(0, stdc_leading_zeros(val));
	ASSERT_EQ(0, stdc_trailing_zeros(val));
	ASSERT_EQ(16, stdc_bit_width(val));
}

TEST(unsigned_int_operations) {
	unsigned int val = 0xDEADBEEF;
	
	ASSERT_TRUE(stdc_count_ones(val) > 0);
	ASSERT_EQ(0, stdc_leading_zeros(val));
	ASSERT_EQ(32, stdc_bit_width(val));
}

TEST(unsigned_long_operations) {
	unsigned long val = 0x12345678UL;
	
	ASSERT_TRUE(stdc_count_ones(val) > 0);
	ASSERT_TRUE(stdc_bit_width(val) > 0);
}

/* ============================================================================
 * ROTATE TESTS
 * ============================================================================ */
TEST_SUITE(rotations);

TEST(rotl32_basic) {
	ASSERT_EQ(2U, rotl32(1U, 1));
	ASSERT_EQ(4U, rotl32(1U, 2));
	ASSERT_EQ(8U, rotl32(1U, 3));
}

TEST(rotl32_wrap) {
	ASSERT_EQ(1U, rotl32(0x80000000U, 1));
	ASSERT_EQ(1U, rotl32(1U, 32));
	ASSERT_EQ(2U, rotl32(1U, 33));
}

TEST(rotr32_basic) {
	ASSERT_EQ(1U, rotr32(2U, 1));
	ASSERT_EQ(1U, rotr32(4U, 2));
	ASSERT_EQ(1U, rotr32(8U, 3));
}

TEST(rotr32_wrap) {
	ASSERT_EQ(0x80000000U, rotr32(1U, 1));
	ASSERT_EQ(1U, rotr32(1U, 32));
	ASSERT_EQ(0x80000000U, rotr32(1U, 33));
}

TEST(rotate_inverse) {
	unsigned int val = 0x12345678U;
	
	ASSERT_EQ(val, rotr32(rotl32(val, 5), 5));
	ASSERT_EQ(val, rotl32(rotr32(val, 7), 7));
}

TEST(rotate_pattern) {
	unsigned int val = 0xAAAAAAAAU;
	
	unsigned int rotated = rotl32(val, 1);
	ASSERT_EQ(0x55555555U, rotated);
}

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(all_bits_set) {
	unsigned int all = UINT32_MAX;
	
	ASSERT_EQ(32, stdc_count_ones(all));
	ASSERT_EQ(0, stdc_count_zeros(all));
	ASSERT_EQ(32, stdc_leading_ones(all));
	ASSERT_EQ(32, stdc_trailing_ones(all));
	ASSERT_EQ(32, stdc_bit_width(all));
}

TEST(single_bit_set) {
	for (int i = 0; i < 32; i++) {
		unsigned int val = 1U << i;
		ASSERT_EQ(1, stdc_count_ones(val));
		ASSERT_TRUE(stdc_has_single_bit(val));
	}
}

TEST(alternating_bits) {
	unsigned int pattern1 = 0xAAAAAAAAU;
	unsigned int pattern2 = 0x55555555U;
	
	ASSERT_EQ(16, stdc_count_ones(pattern1));
	ASSERT_EQ(16, stdc_count_ones(pattern2));
}

/* ============================================================================
 * CONSISTENCY TESTS
 * ============================================================================ */
TEST_SUITE(consistency);

TEST(leading_trailing_sum) {
	unsigned int val = 0x0000FF00U;
	
	int lead = stdc_leading_zeros(val);
	int trail = stdc_trailing_zeros(val);
	int ones = stdc_count_ones(val);
	
	ASSERT_EQ(32, lead + trail + ones);
}

TEST(bit_operations_consistency) {
	unsigned int val = 42U;
	
	int width = stdc_bit_width(val);
	unsigned int floor = stdc_bit_floor(val);
	unsigned int ceil = stdc_bit_ceil(val);
	
	ASSERT_TRUE(floor <= val);
	ASSERT_TRUE(val <= ceil);
	ASSERT_TRUE(width > 0);
}

TEST_MAIN()

#else

// Stub for pre-C23
int main(void) {
	printf("stdbit.h requires C23\n");
	return 0;
}

#endif
