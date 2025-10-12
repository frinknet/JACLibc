/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_C11
#include <tgmath.h>

TEST_TYPE(unit);
TEST_UNIT(tgmath.h);

#define EPSILON 0.0001f
#define DEPSILON 0.0001

/* ============================================================================
 * TYPE DISPATCH TESTS - Verify correct function selection
 * ============================================================================ */
TEST_SUITE(type_dispatch);

TEST(fabs_type_selection) {
	float f = -3.5f;
	double d = -3.5;
	long double ld = -3.5L;
	
	// All should dispatch to correct type
	ASSERT_FLOAT_EQ(3.5f, fabs(f), EPSILON);
	ASSERT_FLOAT_EQ(3.5, fabs(d), DEPSILON);
	ASSERT_FLOAT_EQ(3.5L, fabs(ld), DEPSILON);
}

TEST(sqrt_type_selection) {
	float f = 4.0f;
	double d = 4.0;
	long double ld = 4.0L;
	
	ASSERT_FLOAT_EQ(2.0f, sqrt(f), EPSILON);
	ASSERT_FLOAT_EQ(2.0, sqrt(d), DEPSILON);
	ASSERT_FLOAT_EQ(2.0L, sqrt(ld), DEPSILON);
}

TEST(sin_type_selection) {
	float f = 0.0f;
	double d = 0.0;
	long double ld = 0.0L;
	
	ASSERT_FLOAT_EQ(0.0f, sin(f), EPSILON);
	ASSERT_FLOAT_EQ(0.0, sin(d), DEPSILON);
	ASSERT_FLOAT_EQ(0.0L, sin(ld), DEPSILON);
}

TEST(pow_mixed_types) {
	// Test with mixed float types
	float base_f = 2.0f;
	double exp_d = 3.0;
	
	// Should promote to common type
	double result = pow(base_f, exp_d);
	ASSERT_FLOAT_EQ(8.0, result, DEPSILON);
}

/* ============================================================================
 * ELEMENTARY FUNCTION TESTS
 * ============================================================================ */
TEST_SUITE(elementary);

TEST(exp_generic) {
	ASSERT_FLOAT_EQ(1.0f, exp(0.0f), EPSILON);
	ASSERT_FLOAT_EQ(2.71828, exp(1.0), 0.001);
}

TEST(log_generic) {
	ASSERT_FLOAT_EQ(0.0f, log(1.0f), EPSILON);
	ASSERT_FLOAT_EQ(1.0, log(M_E), 0.001);
}

TEST(exp2_generic) {
	ASSERT_FLOAT_EQ(2.0f, exp2(1.0f), EPSILON);
	ASSERT_FLOAT_EQ(8.0, exp2(3.0), DEPSILON);
}

TEST(log2_generic) {
	ASSERT_FLOAT_EQ(0.0f, log2(1.0f), EPSILON);
	ASSERT_FLOAT_EQ(3.0, log2(8.0), DEPSILON);
}

TEST(cbrt_generic) {
	ASSERT_FLOAT_EQ(2.0f, cbrt(8.0f), EPSILON);
	ASSERT_FLOAT_EQ(3.0, cbrt(27.0), DEPSILON);
}

/* ============================================================================
 * TRIGONOMETRIC TESTS
 * ============================================================================ */
TEST_SUITE(trigonometric);

TEST(sin_generic) {
	ASSERT_FLOAT_EQ(0.0f, sin(0.0f), EPSILON);
	ASSERT_FLOAT_EQ(1.0, sin(M_PI_2), 0.001);
}

TEST(cos_generic) {
	ASSERT_FLOAT_EQ(1.0f, cos(0.0f), EPSILON);
	ASSERT_FLOAT_EQ(0.0, cos(M_PI_2), 0.001);
}

TEST(tan_generic) {
	ASSERT_FLOAT_EQ(0.0f, tan(0.0f), EPSILON);
	ASSERT_FLOAT_EQ(1.0, tan(M_PI_4), 0.001);
}

TEST(asin_generic) {
	ASSERT_FLOAT_EQ(0.0f, asin(0.0f), EPSILON);
	ASSERT_FLOAT_EQ(M_PI_2, asin(1.0), 0.001);
}

TEST(acos_generic) {
	ASSERT_FLOAT_EQ(M_PI_2, acos(0.0f), 0.001);
	ASSERT_FLOAT_EQ(0.0, acos(1.0), DEPSILON);
}

TEST(atan_generic) {
	ASSERT_FLOAT_EQ(0.0f, atan(0.0f), EPSILON);
	ASSERT_FLOAT_EQ(M_PI_4, atan(1.0), 0.001);
}

TEST(atan2_generic) {
	ASSERT_FLOAT_EQ(M_PI_4, atan2(1.0f, 1.0f), 0.001);
	ASSERT_FLOAT_EQ(M_PI_2, atan2(1.0, 0.0), 0.001);
}

/* ============================================================================
 * HYPERBOLIC TESTS
 * ============================================================================ */
TEST_SUITE(hyperbolic);

TEST(sinh_generic) {
	ASSERT_FLOAT_EQ(0.0f, sinh(0.0f), EPSILON);
	ASSERT_FLOAT_EQ(1.17520, sinh(1.0), 0.001);
}

TEST(cosh_generic) {
	ASSERT_FLOAT_EQ(1.0f, cosh(0.0f), EPSILON);
	ASSERT_FLOAT_EQ(1.54308, cosh(1.0), 0.001);
}

TEST(tanh_generic) {
	ASSERT_FLOAT_EQ(0.0f, tanh(0.0f), EPSILON);
	ASSERT_FLOAT_EQ(0.76159, tanh(1.0), 0.001);
}

TEST(asinh_generic) {
	ASSERT_FLOAT_EQ(0.0f, asinh(0.0f), EPSILON);
	ASSERT_FLOAT_EQ(0.88137, asinh(1.0), 0.001);
}

TEST(acosh_generic) {
	ASSERT_FLOAT_EQ(0.0, acosh(1.0), DEPSILON);
	ASSERT_FLOAT_EQ(1.31696, acosh(2.0), 0.001);
}

TEST(atanh_generic) {
	ASSERT_FLOAT_EQ(0.0f, atanh(0.0f), EPSILON);
	ASSERT_FLOAT_EQ(0.54931, atanh(0.5), 0.001);
}

/* ============================================================================
 * ROUNDING TESTS
 * ============================================================================ */
TEST_SUITE(rounding);

TEST(ceil_generic) {
	ASSERT_FLOAT_EQ(3.0f, ceil(2.1f), EPSILON);
	ASSERT_FLOAT_EQ(3.0, ceil(2.9), DEPSILON);
}

TEST(floor_generic) {
	ASSERT_FLOAT_EQ(2.0f, floor(2.9f), EPSILON);
	ASSERT_FLOAT_EQ(2.0, floor(2.1), DEPSILON);
}

TEST(trunc_generic) {
	ASSERT_FLOAT_EQ(2.0f, trunc(2.7f), EPSILON);
	ASSERT_FLOAT_EQ(-2.0, trunc(-2.7), DEPSILON);
}

TEST(round_generic) {
	ASSERT_FLOAT_EQ(3.0f, round(2.5f), EPSILON);
	ASSERT_FLOAT_EQ(2.0, round(2.4), DEPSILON);
}

TEST(rint_generic) {
	ASSERT_FLOAT_EQ(3.0f, rint(2.5f), EPSILON);
	ASSERT_FLOAT_EQ(2.0, rint(2.4), DEPSILON);
}

TEST(nearbyint_generic) {
	ASSERT_FLOAT_EQ(3.0f, nearbyint(2.5f), EPSILON);
	ASSERT_FLOAT_EQ(2.0, nearbyint(2.4), DEPSILON);
}

/* ============================================================================
 * SPECIAL FUNCTION TESTS
 * ============================================================================ */
TEST_SUITE(special);

TEST(hypot_generic) {
	ASSERT_FLOAT_EQ(5.0f, hypot(3.0f, 4.0f), EPSILON);
	ASSERT_FLOAT_EQ(13.0, hypot(5.0, 12.0), DEPSILON);
}

TEST(erf_generic) {
	ASSERT_FLOAT_EQ(0.0f, erf(0.0f), EPSILON);
	ASSERT_FLOAT_EQ(0.84270, erf(1.0), 0.01);
}

TEST(erfc_generic) {
	ASSERT_FLOAT_EQ(1.0f, erfc(0.0f), EPSILON);
	ASSERT_FLOAT_EQ(0.15730, erfc(1.0), 0.01);
}

TEST(tgamma_generic) {
	ASSERT_FLOAT_EQ(1.0f, tgamma(1.0f), 0.1);
	ASSERT_FLOAT_EQ(2.0, tgamma(3.0), 0.1);
}

TEST(lgamma_generic) {
	ASSERT_FLOAT_EQ(0.0f, lgamma(1.0f), 0.1);
	ASSERT_FLOAT_EQ(0.0, lgamma(2.0), 0.1);
}

/* ============================================================================
 * REMAINDER AND MODULO TESTS
 * ============================================================================ */
TEST_SUITE(remainder);

TEST(fmod_generic) {
	ASSERT_FLOAT_EQ(1.0f, fmod(7.0f, 3.0f), EPSILON);
	ASSERT_FLOAT_EQ(0.0, fmod(8.0, 4.0), DEPSILON);
}

TEST(remainder_generic) {
	ASSERT_FLOAT_EQ(1.0f, remainder(7.0f, 3.0f), EPSILON);
	ASSERT_FLOAT_EQ(0.0, remainder(8.0, 4.0), DEPSILON);
}

/* ============================================================================
 * FP MANIPULATION TESTS
 * ============================================================================ */
TEST_SUITE(fp_manipulation);

TEST(scalbn_generic) {
	ASSERT_FLOAT_EQ(8.0f, scalbn(1.0f, 3), EPSILON);
	ASSERT_FLOAT_EQ(0.25, scalbn(1.0, -2), DEPSILON);
}

TEST(scalbln_generic) {
	ASSERT_FLOAT_EQ(16.0f, scalbln(1.0f, 4L), EPSILON);
	ASSERT_FLOAT_EQ(0.125, scalbln(1.0, -3L), DEPSILON);
}

TEST(copysign_generic) {
	ASSERT_FLOAT_EQ(3.0f, copysign(3.0f, 1.0f), EPSILON);
	ASSERT_FLOAT_EQ(-3.0, copysign(3.0, -1.0), DEPSILON);
}

TEST(nextafter_generic) {
	float f = 1.0f;
	double d = 1.0;
	
	ASSERT_TRUE(nextafter(f, 2.0f) > f);
	ASSERT_TRUE(nextafter(d, 2.0) > d);
}

TEST(ldexp_generic) {
	ASSERT_FLOAT_EQ(8.0f, ldexp(1.0f, 3), EPSILON);
	ASSERT_FLOAT_EQ(0.5, ldexp(1.0, -1), DEPSILON);
}

TEST(frexp_generic) {
	int exp;
	float mantissa_f = frexp(8.0f, &exp);
	ASSERT_FLOAT_EQ(0.5f, mantissa_f, EPSILON);
	ASSERT_EQ(4, exp);
	
	double mantissa_d = frexp(8.0, &exp);
	ASSERT_FLOAT_EQ(0.5, mantissa_d, DEPSILON);
	ASSERT_EQ(4, exp);
}

/* ============================================================================
 * MIN/MAX TESTS
 * ============================================================================ */
TEST_SUITE(minmax);

TEST(fmax_generic) {
	ASSERT_FLOAT_EQ(5.0f, fmax(3.0f, 5.0f), EPSILON);
	ASSERT_FLOAT_EQ(5.0, fmax(3.0, 5.0), DEPSILON);
}

TEST(fmin_generic) {
	ASSERT_FLOAT_EQ(3.0f, fmin(3.0f, 5.0f), EPSILON);
	ASSERT_FLOAT_EQ(3.0, fmin(3.0, 5.0), DEPSILON);
}

TEST(fdim_generic) {
	ASSERT_FLOAT_EQ(3.0f, fdim(5.0f, 2.0f), EPSILON);
	ASSERT_FLOAT_EQ(0.0, fdim(2.0, 5.0), DEPSILON);
}

TEST(fma_generic) {
	ASSERT_FLOAT_EQ(11.0f, fma(2.0f, 3.0f, 5.0f), EPSILON);
	ASSERT_FLOAT_EQ(5.0, fma(0.0, 10.0, 5.0), DEPSILON);
}

/* ============================================================================
 * LITERAL TYPE INFERENCE TESTS
 * ============================================================================ */
TEST_SUITE(literal_inference);

TEST(integer_literals) {
	// Integer literals should promote to double
	double result = sqrt(4);
	ASSERT_FLOAT_EQ(2.0, result, DEPSILON);
	
	result = pow(2, 3);
	ASSERT_FLOAT_EQ(8.0, result, DEPSILON);
}

TEST(float_literals) {
	// Explicit float literals
	float result = sqrt(4.0f);
	ASSERT_FLOAT_EQ(2.0f, result, EPSILON);
}

TEST(mixed_operations) {
	// Mixed types in arithmetic
	float f = 2.0f;
	double d = 3.0;
	
	// Should work with both
	ASSERT_FLOAT_EQ(8.0f, pow(f, 3.0f), 0.01f);
	ASSERT_FLOAT_EQ(9.0, pow(d, 2.0), 0.01);
}

/* ============================================================================
 * EDGE CASE TESTS
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(zero_values) {
	ASSERT_FLOAT_EQ(0.0f, sin(0.0f), EPSILON);
	ASSERT_FLOAT_EQ(1.0, exp(0.0), DEPSILON);
	ASSERT_FLOAT_EQ(1.0f, cos(0.0f), EPSILON);
}

TEST(special_values) {
	ASSERT_TRUE(isnan(sqrt(-1.0f)));
	ASSERT_TRUE(isnan(log(-1.0)));
	ASSERT_TRUE(isinf(exp(1000.0f)));
}

TEST(const_expressions) {
	// Test with const qualified types
	const float cf = 4.0f;
	const double cd = 9.0;
	
	ASSERT_FLOAT_EQ(2.0f, sqrt(cf), EPSILON);
	ASSERT_FLOAT_EQ(3.0, sqrt(cd), DEPSILON);
}

TEST_MAIN()

#else

// Stub for pre-C11
int main(void) {
	printf("tgmath.h requires C11 or later\n");
	return 0;
}

#endif
