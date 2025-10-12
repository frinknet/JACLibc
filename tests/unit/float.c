/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <float.h>

TEST_TYPE(unit);
TEST_UNIT(float.h);

/* ============================================================================
 * FLOAT PROPERTIES TESTS
 * ============================================================================ */
TEST_SUITE(float_properties);

TEST(flt_radix) {
	ASSERT_EQ(2, FLT_RADIX);
}

TEST(flt_rounds) {
	ASSERT_EQ(1, FLT_ROUNDS);  // round-to-nearest
}

TEST(decimal_dig) {
	ASSERT_EQ(17, DECIMAL_DIG);
}

TEST(flt_mantissa) {
	ASSERT_EQ(24, FLT_MANT_DIG);
	ASSERT_EQ(6, FLT_DIG);
}

TEST(dbl_mantissa) {
	ASSERT_EQ(53, DBL_MANT_DIG);
	ASSERT_EQ(15, DBL_DIG);
}

/* ============================================================================
 * FLOAT RANGE TESTS
 * ============================================================================ */
TEST_SUITE(float_range);

TEST(flt_exponent_range) {
	ASSERT_EQ(-125, FLT_MIN_EXP);
	ASSERT_EQ(128, FLT_MAX_EXP);
	ASSERT_EQ(-37, FLT_MIN_10_EXP);
	ASSERT_EQ(38, FLT_MAX_10_EXP);
}

TEST(flt_value_range) {
	ASSERT_FLOAT_EQ(3.40282347e+38F, FLT_MAX, 1e30);
	ASSERT_FLOAT_EQ(1.17549435e-38F, FLT_MIN, 1e-40);
	ASSERT_FLOAT_EQ(1.19209290e-07F, FLT_EPSILON, 1e-10);
}

TEST(flt_true_min) {
	ASSERT_FLOAT_EQ(1.40129846e-45F, FLT_TRUE_MIN, 1e-47);
}

/* ============================================================================
 * DOUBLE RANGE TESTS
 * ============================================================================ */
TEST_SUITE(double_range);

TEST(dbl_exponent_range) {
	ASSERT_EQ(-1021, DBL_MIN_EXP);
	ASSERT_EQ(1024, DBL_MAX_EXP);
	ASSERT_EQ(-307, DBL_MIN_10_EXP);
	ASSERT_EQ(308, DBL_MAX_10_EXP);
}

TEST(dbl_value_range) {
	ASSERT_FLOAT_EQ(1.7976931348623157e+308, DBL_MAX, 1e300);
	ASSERT_FLOAT_EQ(2.2250738585072014e-308, DBL_MIN, 1e-310);
	ASSERT_FLOAT_EQ(2.2204460492503131e-16, DBL_EPSILON, 1e-18);
}

TEST(dbl_true_min) {
	ASSERT_FLOAT_EQ(4.9406564584124654e-324, DBL_TRUE_MIN, 1e-326);
}

/* ============================================================================
 * LONG DOUBLE TESTS
 * ============================================================================ */
TEST_SUITE(long_double);

TEST(ldbl_properties) {
	// In this implementation, long double == double
	ASSERT_EQ(DBL_MANT_DIG, LDBL_MANT_DIG);
	ASSERT_EQ(DBL_DIG, LDBL_DIG);
}

TEST(ldbl_exponent_range) {
	ASSERT_EQ(DBL_MIN_EXP, LDBL_MIN_EXP);
	ASSERT_EQ(DBL_MAX_EXP, LDBL_MAX_EXP);
}

TEST(ldbl_value_range) {
	ASSERT_FLOAT_EQ(DBL_MAX, LDBL_MAX, 1e300);
	ASSERT_FLOAT_EQ(DBL_MIN, LDBL_MIN, 1e-310);
	ASSERT_FLOAT_EQ(DBL_EPSILON, LDBL_EPSILON, 1e-18);
}

/* ============================================================================
 * FLOAT HIERARCHY TESTS
 * ============================================================================ */
TEST_SUITE(float_hierarchy);

TEST(float_size_progression) {
	ASSERT_TRUE(sizeof(float) <= sizeof(double));
	ASSERT_TRUE(sizeof(double) <= sizeof(long double));
}

TEST(float_precision_hierarchy) {
	ASSERT_TRUE(FLT_MANT_DIG <= DBL_MANT_DIG);
	ASSERT_TRUE(DBL_MANT_DIG <= LDBL_MANT_DIG);
}

TEST(float_range_hierarchy) {
	ASSERT_TRUE(FLT_MAX <= DBL_MAX);
	ASSERT_TRUE(DBL_MAX <= LDBL_MAX);
	ASSERT_TRUE(FLT_MIN >= DBL_MIN);
}

/* ============================================================================
 * EPSILON TESTS
 * ============================================================================ */
TEST_SUITE(epsilon);

TEST(flt_epsilon_property) {
	float one = 1.0f;
	float one_plus_epsilon = one + FLT_EPSILON;
	float one_plus_half_epsilon = one + (FLT_EPSILON / 2.0f);
	
	ASSERT_TRUE(one_plus_epsilon > one);
	ASSERT_TRUE(one_plus_half_epsilon == one);
}

TEST(dbl_epsilon_property) {
	double one = 1.0;
	double one_plus_epsilon = one + DBL_EPSILON;
	double one_plus_half_epsilon = one + (DBL_EPSILON / 2.0);
	
	ASSERT_TRUE(one_plus_epsilon > one);
	ASSERT_TRUE(one_plus_half_epsilon == one);
}

/* ============================================================================
 * ARITHMETIC BOUNDARY TESTS
 * ============================================================================ */
TEST_SUITE(arithmetic_boundaries);

TEST(float_infinity) {
	float big = FLT_MAX;
	float bigger = big * 2.0f;
	
	ASSERT_TRUE(bigger > big);
}

/* ============================================================================
 * PRACTICAL USAGE TESTS
 * ============================================================================ */
TEST_SUITE(practical_usage);

TEST(float_comparison_epsilon) {
	float a = 1.0f;
	float b = 1.0f + FLT_EPSILON / 2.0f;
	
	ASSERT_FLOAT_EQ(a, b, FLT_EPSILON);
}

/* ============================================================================
 * COMPILE-TIME VALIDATION TESTS
 * ============================================================================ */
TEST_SUITE(compile_time);

TEST(ieee754_assumptions) {
	_Static_assert(sizeof(float) == 4, "float must be 32 bits");
	_Static_assert(sizeof(double) == 8, "double must be 64 bits");
	_Static_assert(FLT_MANT_DIG == 24, "float mantissa must be 24 bits");
	_Static_assert(DBL_MANT_DIG == 53, "double mantissa must be 53 bits");
	
	ASSERT_TRUE(1);
}

/* ============================================================================
 * EDGE CASE TESTS
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(zero_handling) {
	float fzero = 0.0f;
	
	ASSERT_TRUE(fzero < FLT_MAX);
	ASSERT_TRUE(fzero > -FLT_MAX);
}

TEST(negative_zero_float) {
	float neg_zero = -0.0f;
	float pos_zero = 0.0f;
	
	ASSERT_FLOAT_EQ(neg_zero, pos_zero, 0.0f);
}

TEST(subnormal_handling) {
	float tiny = FLT_TRUE_MIN;
	
	ASSERT_TRUE(tiny > 0.0f);
	ASSERT_TRUE(tiny < FLT_MIN);
}

TEST_MAIN()
