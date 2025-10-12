/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <limits.h>

TEST_TYPE(unit);
TEST_UNIT(limits.h);

/* ============================================================================
 * CHAR LIMITS TESTS
 * ============================================================================ */
TEST_SUITE(char_limits);

TEST(char_bit_size) {
	ASSERT_EQ(8, CHAR_BIT);
}

TEST(schar_range) {
	ASSERT_EQ(-128, SCHAR_MIN);
	ASSERT_EQ(127, SCHAR_MAX);
}

TEST(uchar_range) {
	ASSERT_EQ(255, UCHAR_MAX);
}

TEST(char_signedness) {
	// char is either signed or unsigned
	ASSERT_TRUE(CHAR_MIN == 0 || CHAR_MIN == -128);
	ASSERT_TRUE(CHAR_MAX == 255 || CHAR_MAX == 127);
}

TEST(mb_len_max) {
	ASSERT_TRUE(MB_LEN_MAX >= 1);
	ASSERT_EQ(16, MB_LEN_MAX);
}

/* ============================================================================
 * SHORT LIMITS TESTS
 * ============================================================================ */
TEST_SUITE(short_limits);

TEST(shrt_range) {
	ASSERT_EQ(-32768, SHRT_MIN);
	ASSERT_EQ(32767, SHRT_MAX);
}

TEST(ushrt_range) {
	ASSERT_EQ(65535U, USHRT_MAX);
}

TEST(short_size_consistency) {
	ASSERT_TRUE(sizeof(short) * CHAR_BIT >= 16);
}

/* ============================================================================
 * INT LIMITS TESTS
 * ============================================================================ */
TEST_SUITE(int_limits);

TEST(int_range) {
	ASSERT_EQ(-2147483648, INT_MIN);
	ASSERT_EQ(2147483647, INT_MAX);
}

TEST(uint_range) {
	ASSERT_EQ(4294967295U, UINT_MAX);
}

TEST(int_size_consistency) {
	ASSERT_TRUE(sizeof(int) * CHAR_BIT >= 32);
}

TEST(int_boundary_values) {
	int min = INT_MIN;
	int max = INT_MAX;
	
	ASSERT_TRUE(min < 0);
	ASSERT_TRUE(max > 0);
	ASSERT_TRUE(max > min);
}

/* ============================================================================
 * LONG LIMITS TESTS
 * ============================================================================ */
TEST_SUITE(long_limits);

TEST(long_range) {
	#ifdef __LP64__
		ASSERT_EQ(-9223372036854775807L-1, LONG_MIN);
		ASSERT_EQ(9223372036854775807L, LONG_MAX);
		ASSERT_EQ(18446744073709551615UL, ULONG_MAX);
	#else
		ASSERT_EQ(-2147483648L, LONG_MIN);
		ASSERT_EQ(2147483647L, LONG_MAX);
		ASSERT_EQ(4294967295UL, ULONG_MAX);
	#endif
}

TEST(long_size_consistency) {
	ASSERT_TRUE(sizeof(long) * CHAR_BIT >= 32);
}

TEST(long_vs_int) {
	// long must be at least as large as int
	ASSERT_TRUE(LONG_MAX >= INT_MAX);
	ASSERT_TRUE(ULONG_MAX >= UINT_MAX);
}

/* ============================================================================
 * LONG LONG LIMITS TESTS
 * ============================================================================ */
TEST_SUITE(llong_limits);

#if JACL_HAS_C99
TEST(llong_range) {
	ASSERT_EQ(-9223372036854775807LL-1, LLONG_MIN);
	ASSERT_EQ(9223372036854775807LL, LLONG_MAX);
	ASSERT_EQ(18446744073709551615ULL, ULLONG_MAX);
}

TEST(llong_size_consistency) {
	ASSERT_TRUE(sizeof(long long) * CHAR_BIT >= 64);
}

TEST(llong_vs_long) {
	// long long must be at least as large as long
	ASSERT_TRUE(LLONG_MAX >= LONG_MAX);
	ASSERT_TRUE(ULLONG_MAX >= ULONG_MAX);
}
#endif

/* ============================================================================
 * TYPE HIERARCHY TESTS
 * ============================================================================ */
TEST_SUITE(type_hierarchy);

TEST(integer_size_progression) {
	// sizeof(char) <= sizeof(short) <= sizeof(int) <= sizeof(long) <= sizeof(long long)
	ASSERT_TRUE(sizeof(char) <= sizeof(short));
	ASSERT_TRUE(sizeof(short) <= sizeof(int));
	ASSERT_TRUE(sizeof(int) <= sizeof(long));
	
	#if JACL_HAS_C99
	ASSERT_TRUE(sizeof(long) <= sizeof(long long));
	#endif
}

TEST(limit_consistency) {
	// Check that signed/unsigned pairs match
	ASSERT_EQ(SCHAR_MAX, (UCHAR_MAX / 2));
	ASSERT_EQ(SHRT_MAX, (USHRT_MAX / 2));
	ASSERT_EQ(INT_MAX, (UINT_MAX / 2));
	ASSERT_EQ(LONG_MAX, (long)(ULONG_MAX / 2));
}

TEST(unsigned_max_values) {
	// Unsigned max should be all bits set
	ASSERT_EQ(UCHAR_MAX, (unsigned char)~0);
	ASSERT_EQ(USHRT_MAX, (unsigned short)~0);
	ASSERT_EQ(UINT_MAX, (unsigned int)~0);
}

/* ============================================================================
 * ARITHMETIC BOUNDARY TESTS
 * ============================================================================ */
TEST_SUITE(arithmetic_boundaries);

TEST(int_overflow_detection) {
	int max = INT_MAX;
	int min = INT_MIN;
	
	ASSERT_TRUE(max > 0);
	ASSERT_TRUE(min < 0);
}

TEST(unsigned_wraparound) {
	unsigned int max = UINT_MAX;
	
	// Unsigned overflow is well-defined (wraps to 0)
	ASSERT_EQ(0, max + 1);
}

/* ============================================================================
 * PRACTICAL USAGE TESTS
 * ============================================================================ */
TEST_SUITE(practical_usage);

TEST(safe_int_addition_check) {
	int a = 1000000;
	int b = 1000000;
	
	// Check if addition would overflow
	if (a > INT_MAX - b) {
		ASSERT_TRUE(0);  // Should not reach
	} else {
		int sum = a + b;
		ASSERT_EQ(2000000, sum);
	}
}

TEST(type_range_validation) {
	// Verify types can hold their declared ranges
	char c = CHAR_MAX;
	short s = SHRT_MAX;
	int i = INT_MAX;
	long l = LONG_MAX;
	
	ASSERT_EQ(CHAR_MAX, c);
	ASSERT_EQ(SHRT_MAX, s);
	ASSERT_EQ(INT_MAX, i);
	ASSERT_EQ(LONG_MAX, l);
}

/* ============================================================================
 * COMPILE-TIME VALIDATION TESTS
 * ============================================================================ */
TEST_SUITE(compile_time);

TEST(static_assertions) {
	_Static_assert(CHAR_BIT == 8, "char must be 8 bits");
	_Static_assert(sizeof(short) >= 2, "short must be at least 16 bits");
	_Static_assert(sizeof(int) >= 4, "int must be at least 32 bits");
	_Static_assert(sizeof(long) >= 4, "long must be at least 32 bits");
	
	ASSERT_TRUE(1);
}

/* ============================================================================
 * EDGE CASE TESTS
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(zero_handling) {
	int zero = 0;
	
	ASSERT_TRUE(zero < INT_MAX);
	ASSERT_TRUE(zero > INT_MIN);
}

TEST_MAIN()
