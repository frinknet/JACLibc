/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

TEST_TYPE(unit);
TEST_UNIT(tgmath);

#if JACL_HAS_C11
#include <tgmath.h>

#define EPSILON 0.0001f
#define DEPSILON 0.0001

/* ============================================================================
 * TYPE DISPATCH
 * ============================================================================ */
TEST_SUITE(type_dispatch);

TEST(type_dispatch_fabs_selects_variant) {
	float f = -3.5f;
	double d = -3.5;
	long double ld = -3.5L;

	ASSERT_FLT_NEAR(3.5f, fabs(f), EPSILON);
	ASSERT_DBL_NEAR(3.5, fabs(d), DEPSILON);
	ASSERT_LDBL_NEAR(3.5L, fabs(ld), DEPSILON);
}

TEST(type_dispatch_sqrt_selects_variant) {
	float f = 4.0f;
	double d = 4.0;
	long double ld = 4.0L;

	ASSERT_FLT_NEAR(2.0f, sqrt(f), EPSILON);
	ASSERT_DBL_NEAR(2.0, sqrt(d), DEPSILON);
	ASSERT_LDBL_NEAR(2.0L, sqrt(ld), DEPSILON);
}

TEST(type_dispatch_sin_selects_variant) {
	float f = 0.0f;
	double d = 0.0;
	long double ld = 0.0L;

	ASSERT_FLT_NEAR(0.0f, sin(f), EPSILON);
	ASSERT_DBL_NEAR(0.0, sin(d), DEPSILON);
	ASSERT_LDBL_NEAR(0.0L, sin(ld), DEPSILON);
}

TEST(type_dispatch_mixed_types_promote) {
	float base_f = 2.0f;
	double exp_d = 3.0;
	double result = pow(base_f, exp_d);

	ASSERT_DBL_NEAR(8.0, result, DEPSILON);
}

/* ============================================================================
 * EXPONENTIAL AND LOGARITHMIC
 * ============================================================================ */
TEST_SUITE(exp_log);

TEST(exp_log_exp_family) {
	ASSERT_FLT_NEAR(1.0f, exp(0.0f), EPSILON);
	ASSERT_DBL_NEAR(2.71828, exp(1.0), 0.001);
	ASSERT_FLT_NEAR(2.0f, exp2(1.0f), EPSILON);
	ASSERT_DBL_NEAR(8.0, exp2(3.0), DEPSILON);
}

TEST(exp_log_log_family) {
	ASSERT_FLT_NEAR(0.0f, log(1.0f), EPSILON);
	ASSERT_DBL_NEAR(1.0, log(M_E), 0.001);
	ASSERT_FLT_NEAR(0.0f, log2(1.0f), EPSILON);
	ASSERT_DBL_NEAR(3.0, log2(8.0), DEPSILON);
	ASSERT_DBL_NEAR(1.0, log10(10.0), DEPSILON);
}

TEST(exp_log_pow_and_roots) {
	ASSERT_FLT_NEAR(8.0f, pow(2.0f, 3.0f), EPSILON);
	ASSERT_DBL_NEAR(9.0, pow(3.0, 2.0), DEPSILON);
	ASSERT_FLT_NEAR(2.0f, cbrt(8.0f), EPSILON);
	ASSERT_DBL_NEAR(3.0, cbrt(27.0), DEPSILON);
}

/* ============================================================================
 * TRIGONOMETRIC
 * ============================================================================ */
TEST_SUITE(trig);

TEST(trig_sin_cos_tan) {
	ASSERT_FLT_NEAR(0.0f, sin(0.0f), EPSILON);
	ASSERT_DBL_NEAR(1.0, sin(M_PI_2), 0.001);
	ASSERT_FLT_NEAR(1.0f, cos(0.0f), EPSILON);
	ASSERT_DBL_NEAR(0.0, cos(M_PI_2), 0.001);
	ASSERT_FLT_NEAR(0.0f, tan(0.0f), EPSILON);
	ASSERT_DBL_NEAR(1.0, tan(M_PI_4), 0.001);
}

TEST(trig_asin_acos_atan) {
	ASSERT_FLT_NEAR(0.0f, asin(0.0f), EPSILON);
	ASSERT_DBL_NEAR(M_PI_2, asin(1.0), 0.001);
	ASSERT_FLT_NEAR((float)M_PI_2, acos(0.0f), 0.001f);
	ASSERT_DBL_NEAR(0.0, acos(1.0), DEPSILON);
	ASSERT_FLT_NEAR(0.0f, atan(0.0f), EPSILON);
	ASSERT_DBL_NEAR(M_PI_4, atan(1.0), 0.001);
}

TEST(trig_atan2_quadrants) {
	ASSERT_FLT_NEAR((float)M_PI_4, atan2(1.0f, 1.0f), 0.001f);
	ASSERT_DBL_NEAR(M_PI_2, atan2(1.0, 0.0), 0.001);
	ASSERT_DBL_NEAR(-M_PI_2, atan2(-1.0, 0.0), 0.001);
}

/* ============================================================================
 * HYPERBOLIC
 * ============================================================================ */
TEST_SUITE(hyperbolic);

TEST(hyperbolic_sinh_cosh_tanh) {
	ASSERT_FLT_NEAR(0.0f, sinh(0.0f), EPSILON);
	ASSERT_DBL_NEAR(1.17520, sinh(1.0), 0.001);
	ASSERT_FLT_NEAR(1.0f, cosh(0.0f), EPSILON);
	ASSERT_DBL_NEAR(1.54308, cosh(1.0), 0.001);
	ASSERT_FLT_NEAR(0.0f, tanh(0.0f), EPSILON);
	ASSERT_DBL_NEAR(0.76159, tanh(1.0), 0.001);
}

TEST(hyperbolic_asinh_acosh_atanh) {
	ASSERT_FLT_NEAR(0.0f, asinh(0.0f), EPSILON);
	ASSERT_DBL_NEAR(0.88137, asinh(1.0), 0.001);
	ASSERT_DBL_NEAR(0.0, acosh(1.0), DEPSILON);
	ASSERT_DBL_NEAR(1.31696, acosh(2.0), 0.001);
	ASSERT_FLT_NEAR(0.0f, atanh(0.0f), EPSILON);
	ASSERT_DBL_NEAR(0.54931, atanh(0.5), 0.001);
}

/* ============================================================================
 * ROUNDING AND REMAINDER
 * ============================================================================ */
TEST_SUITE(rounding);

TEST(rounding_ceil_floor) {
	ASSERT_FLT_NEAR(3.0f, ceil(2.1f), EPSILON);
	ASSERT_DBL_NEAR(3.0, ceil(2.9), DEPSILON);
	ASSERT_FLT_NEAR(2.0f, floor(2.9f), EPSILON);
	ASSERT_DBL_NEAR(2.0, floor(2.1), DEPSILON);
}

TEST(rounding_trunc_round) {
	ASSERT_FLT_NEAR(2.0f, trunc(2.7f), EPSILON);
	ASSERT_DBL_NEAR(-2.0, trunc(-2.7), DEPSILON);
	ASSERT_FLT_NEAR(3.0f, round(2.5f), EPSILON);
	ASSERT_DBL_NEAR(2.0, round(2.4), DEPSILON);
}

TEST(rounding_rint_nearbyint) {
	ASSERT_FLT_NEAR(3.0f, rint(2.5f), EPSILON);
	ASSERT_DBL_NEAR(2.0, rint(2.4), DEPSILON);
	ASSERT_FLT_NEAR(3.0f, nearbyint(2.5f), EPSILON);
	ASSERT_DBL_NEAR(2.0, nearbyint(2.4), DEPSILON);
}

TEST(rounding_fmod_remainder) {
	ASSERT_FLT_NEAR(1.0f, fmod(7.0f, 3.0f), EPSILON);
	ASSERT_DBL_NEAR(0.0, fmod(8.0, 4.0), DEPSILON);
	ASSERT_DBL_NEAR(2.5, fmod(12.5, 5.0), DEPSILON);
	ASSERT_FLT_NEAR(1.0f, remainder(7.0f, 3.0f), EPSILON);
	ASSERT_DBL_NEAR(0.0, remainder(8.0, 4.0), DEPSILON);
}

/* ============================================================================
 * SPECIAL FUNCTIONS
 * ============================================================================ */
TEST_SUITE(special);

TEST(special_hypot) {
	/* Use explicit function calls to avoid complex variant lookup */
	ASSERT_FLT_NEAR(5.0f, hypotf(3.0f, 4.0f), EPSILON);
	ASSERT_DBL_NEAR(13.0, hypot(5.0, 12.0), DEPSILON);
}

TEST(special_erf_erfc) {
	ASSERT_FLT_NEAR(0.0f, erf(0.0f), EPSILON);
	ASSERT_DBL_NEAR(0.84270, erf(1.0), 0.01);
	ASSERT_FLT_NEAR(1.0f, erfc(0.0f), EPSILON);
	ASSERT_DBL_NEAR(0.15730, erfc(1.0), 0.01);
}

TEST(special_tgamma_lgamma) {
	ASSERT_FLT_NEAR(1.0f, tgamma(1.0f), 0.1f);
	ASSERT_DBL_NEAR(2.0, tgamma(3.0), 0.1);
	ASSERT_FLT_NEAR(0.0f, lgamma(1.0f), 0.1f);
	ASSERT_DBL_NEAR(0.0, lgamma(2.0), 0.1);
}

/* ============================================================================
 * MANIPULATION
 * ============================================================================ */
TEST_SUITE(manipulation);

TEST(manipulation_scalbn_scalbln) {
	ASSERT_FLT_NEAR(8.0f, scalbn(1.0f, 3), EPSILON);
	ASSERT_DBL_NEAR(0.25, scalbn(1.0, -2), DEPSILON);
	ASSERT_FLT_NEAR(16.0f, scalbln(1.0f, 4L), EPSILON);
	ASSERT_DBL_NEAR(0.125, scalbln(1.0, -3L), DEPSILON);
}

TEST(manipulation_copysign) {
	ASSERT_FLT_NEAR(3.0f, copysign(3.0f, 1.0f), EPSILON);
	ASSERT_DBL_NEAR(-3.0, copysign(3.0, -1.0), DEPSILON);
	ASSERT_FLT_NEAR(-2.5f, copysign(-2.5f, -1.0f), EPSILON);
}

TEST(manipulation_nextafter) {
	float f = 1.0f;
	double d = 1.0;

	ASSERT_TRUE(nextafter(f, 2.0f) > f);
	ASSERT_TRUE(nextafter(d, 2.0) > d);
	ASSERT_TRUE(nextafter(f, 0.0f) < f);
}

TEST(manipulation_ldexp_frexp) {
	ASSERT_FLT_NEAR(8.0f, ldexp(1.0f, 3), EPSILON);
	ASSERT_DBL_NEAR(0.5, ldexp(1.0, -1), DEPSILON);

	int exp;
	float mantissa_f = frexp(8.0f, &exp);
	ASSERT_FLT_NEAR(0.5f, mantissa_f, EPSILON);
	ASSERT_EQ(4, exp);

	double mantissa_d = frexp(8.0, &exp);
	ASSERT_DBL_NEAR(0.5, mantissa_d, DEPSILON);
	ASSERT_EQ(4, exp);
}

/* ============================================================================
 * COMPARISON AND ARITHMETIC
 * ============================================================================ */
TEST_SUITE(comparison);

TEST(comparison_fmax_fmin) {
	ASSERT_FLT_NEAR(5.0f, fmax(3.0f, 5.0f), EPSILON);
	ASSERT_DBL_NEAR(5.0, fmax(3.0, 5.0), DEPSILON);
	ASSERT_FLT_NEAR(3.0f, fmin(3.0f, 5.0f), EPSILON);
	ASSERT_DBL_NEAR(3.0, fmin(3.0, 5.0), DEPSILON);
}

TEST(comparison_fdim) {
	ASSERT_FLT_NEAR(3.0f, fdim(5.0f, 2.0f), EPSILON);
	ASSERT_DBL_NEAR(0.0, fdim(2.0, 5.0), DEPSILON);
}

TEST(comparison_fma) {
	ASSERT_FLT_NEAR(11.0f, fma(2.0f, 3.0f, 5.0f), EPSILON);
	ASSERT_DBL_NEAR(5.0, fma(0.0, 10.0, 5.0), DEPSILON);
	ASSERT_DBL_NEAR(14.0, fma(3.0, 4.0, 2.0), DEPSILON);
}

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(edge_cases_zero_inputs) {
	ASSERT_FLT_NEAR(0.0f, sin(0.0f), EPSILON);
	ASSERT_DBL_NEAR(1.0, exp(0.0), DEPSILON);
	ASSERT_FLT_NEAR(1.0f, cos(0.0f), EPSILON);
	ASSERT_DBL_NEAR(0.0, log(1.0), DEPSILON);
}

TEST(edge_cases_nan_results) {
	ASSERT_TRUE(isnan(sqrt(-1.0f)));
	ASSERT_TRUE(isnan(log(-1.0)));
	ASSERT_TRUE(isnan(asin(2.0f)));
	ASSERT_TRUE(isnan(acos(-2.0)));
}

TEST(edge_cases_inf_results) {
	ASSERT_TRUE(isinf(exp(1000.0f)));
	ASSERT_TRUE(isinf(log(0.0)));
	ASSERT_TRUE(isinf(pow(10.0, 1000.0)));
}

TEST(edge_cases_const_qualified) {
	const float cf = 4.0f;
	const double cd = 9.0;

	ASSERT_FLT_NEAR(2.0f, sqrt(cf), EPSILON);
	ASSERT_DBL_NEAR(3.0, sqrt(cd), DEPSILON);
}

#else

TEST_SUITE(tgmath_basics);

TEST(tgmath_not_avaialable) {
	TEST_SKIP("NO C11 SUPPORT");
}

#endif

TEST_MAIN()

