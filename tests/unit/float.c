/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <float.h>

TEST_TYPE(unit);
TEST_UNIT(float.h);

/* ============================================================================
 * FLOAT TESTS
 * ============================================================================ */
TEST_SUITE(float);

TEST(float_radix) {
	ASSERT_INT_EQ(2, FLT_RADIX);
}

TEST(float_rounds) {
	ASSERT_INT_EQ(1, FLT_ROUNDS);
}

TEST(float_mantissa) {
	ASSERT_INT_EQ(24, FLT_MANT_DIG);
	ASSERT_INT_EQ(6, FLT_DIG);
}

TEST(float_exponent_range) {
	ASSERT_INT_EQ(-125, FLT_MIN_EXP);
	ASSERT_INT_EQ(128, FLT_MAX_EXP);
	ASSERT_INT_EQ(-37, FLT_MIN_10_EXP);
	ASSERT_INT_EQ(38, FLT_MAX_10_EXP);
}

TEST(float_value_range) {
	ASSERT_FLT_NEAR(3.40282347e+38F, FLT_MAX, 1e30F);
	ASSERT_FLT_NEAR(1.17549435e-38F, FLT_MIN, 1e-40F);
	ASSERT_FLT_NEAR(1.19209290e-07F, FLT_EPSILON, 1e-10F);
}

TEST(float_true_min) {
	ASSERT_FLT_NEAR(1.40129846e-45F, FLT_TRUE_MIN, FLT_TRUE_MIN);
}

TEST(float_epsilon_property) {
	float one = 1.0f;
	float one_plus_epsilon = one + FLT_EPSILON;
	float one_plus_half_epsilon = one + (FLT_EPSILON / 2.0f);

	ASSERT_FLT_GT(one_plus_epsilon, one);
	ASSERT_FLT_EQ(one_plus_half_epsilon, one);
}

TEST(float_negative_zero) {
	float neg_zero = -0.0f;
	float pos_zero = 0.0f;

	ASSERT_FLT_EQ(neg_zero, pos_zero);
	ASSERT_INT_NE(signbit(neg_zero), signbit(pos_zero));
}

TEST(float_subnormal) {
	float tiny = FLT_TRUE_MIN;

	ASSERT_FLT_GT(tiny, 0.0f);
	ASSERT_FLT_LT(tiny, FLT_MIN);
}

TEST(float_overflow) {
	float big = FLT_MAX;
	float bigger = big * 2.0f;

	ASSERT_INF(bigger);
}

TEST(float_underflow) {
	float tiny = FLT_TRUE_MIN;
	float tinier = tiny / 2.0f;

	ASSERT_FLT_EQ(tinier, 0.0f);
}

TEST(float_infinity_arithmetic) {
	float inf = 1.0f / 0.0f;

	ASSERT_INF(inf);
	ASSERT_FLT_GT(inf, FLT_MAX);
}

TEST(float_nan_generation) {
	float nan = 0.0f / 0.0f;

	ASSERT_NAN(nan);
	ASSERT_FALSE(nan == nan);
}

/* ============================================================================
 * DOUBLE TESTS
 * ============================================================================ */
TEST_SUITE(double);

TEST(double_mantissa) {
	ASSERT_INT_EQ(53, DBL_MANT_DIG);
	ASSERT_INT_EQ(15, DBL_DIG);
}

TEST(double_decimal_digits) {
	// DECIMAL_DIG depends on the widest float type (long double)
	#if JACL_LDBL_BITS == 80
		ASSERT_INT_EQ(21, DECIMAL_DIG);
	#elif JACL_LDBL_BITS == 128
		ASSERT_INT_EQ(36, DECIMAL_DIG);
	#else
		ASSERT_INT_EQ(17, DECIMAL_DIG);
	#endif
}
TEST(double_exponent_range) {
	ASSERT_INT_EQ(-1021, DBL_MIN_EXP);
	ASSERT_INT_EQ(1024, DBL_MAX_EXP);
	ASSERT_INT_EQ(-307, DBL_MIN_10_EXP);
	ASSERT_INT_EQ(308, DBL_MAX_10_EXP);
}

TEST(double_value_range) {
	ASSERT_DBL_NEAR(1.7976931348623157e+308, DBL_MAX, 1e300);
	ASSERT_DBL_NEAR(2.2250738585072014e-308, DBL_MIN, 1e-310);
	ASSERT_DBL_NEAR(2.2204460492503131e-16, DBL_EPSILON, 1e-18);
}

TEST(double_true_min) {
	ASSERT_DBL_NEAR(4.9406564584124654e-324, DBL_TRUE_MIN, DBL_TRUE_MIN);
}

TEST(double_epsilon_property) {
	double one = 1.0;
	double one_plus_epsilon = one + DBL_EPSILON;
	double one_plus_half_epsilon = one + (DBL_EPSILON / 2.0);

	ASSERT_DBL_GT(one_plus_epsilon, one);
	ASSERT_DBL_EQ(one_plus_half_epsilon, one);
}

TEST(double_negative_zero) {
	double neg_zero = -0.0;
	double pos_zero = 0.0;

	ASSERT_DBL_EQ(neg_zero, pos_zero);
	ASSERT_INT_NE(signbit(neg_zero), signbit(pos_zero));
}

TEST(double_subnormal) {
	double tiny = DBL_TRUE_MIN;

	ASSERT_DBL_GT(tiny, 0.0);
	ASSERT_DBL_LT(tiny, DBL_MIN);
}

TEST(double_overflow) {
	double big = DBL_MAX;
	double bigger = big * 2.0;

	ASSERT_INF(bigger);
}

TEST(double_underflow) {
	double tiny = DBL_TRUE_MIN;
	double tinier = tiny / 2.0;

	ASSERT_DBL_EQ(tinier, 0.0);
}

TEST(double_infinity_arithmetic) {
	double inf = 1.0 / 0.0;

	ASSERT_INF(inf);
	ASSERT_DBL_GT(inf, DBL_MAX);
}

TEST(double_nan_generation) {
	double nan = 0.0 / 0.0;

	ASSERT_NAN(nan);
	ASSERT_FALSE(nan == nan);
}

/* ============================================================================
 * LONG DOUBLE TESTS
 * ============================================================================ */
TEST_SUITE(long_double);

TEST(long_double_properties) {
#if JACL_LDBL_BITS == 80
	ASSERT_INT_EQ(64, LDBL_MANT_DIG);
	ASSERT_INT_EQ(18, LDBL_DIG);
#elif JACL_LDBL_BITS == 128
	ASSERT_INT_EQ(113, LDBL_MANT_DIG);
	ASSERT_INT_EQ(33, LDBL_DIG);
#else
	ASSERT_INT_EQ(DBL_MANT_DIG, LDBL_MANT_DIG);
	ASSERT_INT_EQ(DBL_DIG, LDBL_DIG);
#endif
}

TEST(long_double_exponent_range) {
#if JACL_LDBL_BITS == 80 || JACL_LDBL_BITS == 128
	ASSERT_INT_EQ(-16381, LDBL_MIN_EXP);
	ASSERT_INT_EQ(16384, LDBL_MAX_EXP);
#else
	ASSERT_INT_EQ(DBL_MIN_EXP, LDBL_MIN_EXP);
	ASSERT_INT_EQ(DBL_MAX_EXP, LDBL_MAX_EXP);
#endif
}

TEST(long_double_value_range) {
#if JACL_LDBL_BITS == 80 || JACL_LDBL_BITS == 128
	ASSERT_LDBL_NEAR(1.18973149535723176502e+4932L, LDBL_MAX, 1e4920L);
	ASSERT_LDBL_NEAR(3.36210314311209350626e-4932L, LDBL_MIN, 1e-4940L);
#else
	ASSERT_LDBL_NEAR(DBL_MAX, LDBL_MAX, 1e300L);
	ASSERT_LDBL_NEAR(DBL_MIN, LDBL_MIN, 1e-310L);
#endif
	// Epsilon varies by format
#if JACL_LDBL_BITS == 80
	ASSERT_LDBL_NEAR(1.08420217248550443401e-19L, LDBL_EPSILON, 1e-25L);
#elif JACL_LDBL_BITS == 128
	ASSERT_LDBL_NEAR(1.92592994438723585305597794258492732e-34L, LDBL_EPSILON, 1e-40L);
#else
	ASSERT_LDBL_NEAR(DBL_EPSILON, LDBL_EPSILON, 1e-18L);
#endif
}

TEST(long_double_negative_zero) {
	long double neg_zero = -0.0L;
	long double pos_zero = 0.0L;

	ASSERT_LDBL_EQ(neg_zero, pos_zero);
	ASSERT_INT_NE(signbit(neg_zero), signbit(pos_zero));
}

TEST(long_double_overflow) {
	long double big = LDBL_MAX;
	long double bigger = big * 2.0L;

	ASSERT_INF(bigger);
}

TEST(long_double_infinity_arithmetic) {
	long double inf = 1.0L / 0.0L;

	ASSERT_INF(inf);
	ASSERT_LDBL_GT(inf, LDBL_MAX);
}

TEST(long_double_nan_generation) {
	long double nan = 0.0L / 0.0L;

	ASSERT_NAN(nan);
	ASSERT_FALSE(nan == nan);
}

/* ============================================================================
 * ASSUMPTIONS TESTS
 * ============================================================================ */
TEST_SUITE(assumptions);

TEST(assumptions_ieee754_float_size) {
	static_assert(sizeof(float) == 4, "float must be 32 bits");
	ASSERT_INT_EQ(4, sizeof(float));
}

TEST(assumptions_ieee754_double_size) {
	static_assert(sizeof(double) == 8, "double must be 64 bits");
	ASSERT_INT_EQ(8, sizeof(double));
}

TEST(assumptions_ieee754_float_mantissa) {
	static_assert(FLT_MANT_DIG == 24, "float mantissa must be 24 bits");
	ASSERT_INT_EQ(24, FLT_MANT_DIG);
}

TEST(assumptions_ieee754_double_mantissa) {
	static_assert(DBL_MANT_DIG == 53, "double mantissa must be 53 bits");
	ASSERT_INT_EQ(53, DBL_MANT_DIG);
}

TEST(assumptions_size_progression) {
	ASSERT_INT_LE(sizeof(float), sizeof(double));
	ASSERT_INT_LE(sizeof(double), sizeof(long double));
}

TEST(assumptions_precision_progression) {
	ASSERT_INT_LE(FLT_MANT_DIG, DBL_MANT_DIG);
	ASSERT_INT_LE(DBL_MANT_DIG, LDBL_MANT_DIG);
}

TEST(assumptions_range_progression) {
	ASSERT_FLT_LE(FLT_MAX, DBL_MAX);
	ASSERT_DBL_LE(DBL_MAX, LDBL_MAX);
	ASSERT_FLT_GE(FLT_MIN, DBL_MIN);
}

TEST_MAIN()

