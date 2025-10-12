/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <math.h>

TEST_TYPE(unit);
TEST_UNIT(math.h);

#define EPSILON 0.0001

/* ============================================================================
 * CLASSIFICATION TESTS
 * ============================================================================ */
TEST_SUITE(classification);

TEST(isnan_detection) {
	ASSERT_TRUE(isnan(NAN));
	ASSERT_TRUE(isnan(0.0 / 0.0));
	ASSERT_FALSE(isnan(0.0));
	ASSERT_FALSE(isnan(1.0));
	ASSERT_FALSE(isnan(INFINITY));
}

TEST(isinf_detection) {
	ASSERT_TRUE(isinf(INFINITY));
	ASSERT_TRUE(isinf(-INFINITY));
	ASSERT_TRUE(isinf(1.0 / 0.0));
	ASSERT_FALSE(isinf(0.0));
	ASSERT_FALSE(isinf(1.0));
	ASSERT_FALSE(isinf(NAN));
}

TEST(isfinite_detection) {
	ASSERT_TRUE(isfinite(0.0));
	ASSERT_TRUE(isfinite(1.0));
	ASSERT_TRUE(isfinite(-100.5));
	ASSERT_FALSE(isfinite(INFINITY));
	ASSERT_FALSE(isfinite(NAN));
}

TEST(fpclassify_types) {
	ASSERT_EQ(FP_ZERO, fpclassify(0.0));
	ASSERT_EQ(FP_ZERO, fpclassify(-0.0));
	ASSERT_EQ(FP_NORMAL, fpclassify(1.0));
	ASSERT_EQ(FP_INFINITE, fpclassify(INFINITY));
	ASSERT_EQ(FP_NAN, fpclassify(NAN));
}

TEST(signbit_detection) {
	ASSERT_EQ(0, signbit(0.0));
	ASSERT_NE(0, signbit(-0.0));
	ASSERT_EQ(0, signbit(1.0));
	ASSERT_NE(0, signbit(-1.0));
}

TEST(isnormal_detection) {
	ASSERT_TRUE(isnormal(1.0));
	ASSERT_TRUE(isnormal(-1.0));
	ASSERT_FALSE(isnormal(0.0));
	ASSERT_FALSE(isnormal(INFINITY));
	ASSERT_FALSE(isnormal(NAN));
}

/* ============================================================================
 * ABSOLUTE VALUE TESTS
 * ============================================================================ */
TEST_SUITE(absolute_value);

TEST(fabs_basic) {
	ASSERT_FLOAT_EQ(5.0, fabs(5.0), EPSILON);
	ASSERT_FLOAT_EQ(5.0, fabs(-5.0), EPSILON);
	ASSERT_FLOAT_EQ(0.0, fabs(0.0), EPSILON);
	ASSERT_FLOAT_EQ(0.0, fabs(-0.0), EPSILON);
}

TEST(fabs_special) {
	ASSERT_TRUE(isinf(fabs(INFINITY)));
	ASSERT_TRUE(isinf(fabs(-INFINITY)));
	ASSERT_TRUE(isnan(fabs(NAN)));
}

TEST(fabsf_basic) {
	ASSERT_FLOAT_EQ(3.5f, fabsf(3.5f), EPSILON);
	ASSERT_FLOAT_EQ(3.5f, fabsf(-3.5f), EPSILON);
}

/* ============================================================================
 * ROUNDING TESTS
 * ============================================================================ */
TEST_SUITE(rounding);

TEST(ceil_basic) {
	ASSERT_FLOAT_EQ(3.0, ceil(2.1), EPSILON);
	ASSERT_FLOAT_EQ(3.0, ceil(2.9), EPSILON);
	ASSERT_FLOAT_EQ(3.0, ceil(3.0), EPSILON);
	ASSERT_FLOAT_EQ(-2.0, ceil(-2.9), EPSILON);
	ASSERT_FLOAT_EQ(0.0, ceil(0.0), EPSILON);
}

TEST(floor_basic) {
	ASSERT_FLOAT_EQ(2.0, floor(2.1), EPSILON);
	ASSERT_FLOAT_EQ(2.0, floor(2.9), EPSILON);
	ASSERT_FLOAT_EQ(3.0, floor(3.0), EPSILON);
	ASSERT_FLOAT_EQ(-3.0, floor(-2.1), EPSILON);
	ASSERT_FLOAT_EQ(0.0, floor(0.0), EPSILON);
}

TEST(trunc_basic) {
	ASSERT_FLOAT_EQ(2.0, trunc(2.7), EPSILON);
	ASSERT_FLOAT_EQ(-2.0, trunc(-2.7), EPSILON);
	ASSERT_FLOAT_EQ(0.0, trunc(0.5), EPSILON);
	ASSERT_FLOAT_EQ(0.0, trunc(-0.5), EPSILON);
	ASSERT_FLOAT_EQ(3.0, trunc(3.0), EPSILON);
}

TEST(round_basic) {
	ASSERT_FLOAT_EQ(3.0, round(2.5), EPSILON);
	ASSERT_FLOAT_EQ(2.0, round(2.4), EPSILON);
	ASSERT_FLOAT_EQ(3.0, round(2.6), EPSILON);
	ASSERT_FLOAT_EQ(-3.0, round(-2.5), EPSILON);
	ASSERT_FLOAT_EQ(0.0, round(0.0), EPSILON);
}

TEST(rint_basic) {
	ASSERT_FLOAT_EQ(3.0, rint(2.5), EPSILON);
	ASSERT_FLOAT_EQ(2.0, rint(2.4), EPSILON);
	ASSERT_FLOAT_EQ(3.0, rint(2.6), EPSILON);
}

TEST(nearbyint_basic) {
	ASSERT_FLOAT_EQ(3.0, nearbyint(2.5), EPSILON);
	ASSERT_FLOAT_EQ(2.0, nearbyint(2.4), EPSILON);
}

TEST(lround_conversion) {
	ASSERT_EQ(3L, lround(2.5));
	ASSERT_EQ(2L, lround(2.4));
	ASSERT_EQ(-3L, lround(-2.5));
}

TEST(llround_conversion) {
	ASSERT_EQ(3LL, llround(2.5));
	ASSERT_EQ(2LL, llround(2.4));
}

/* ============================================================================
 * EXPONENTIAL TESTS
 * ============================================================================ */
TEST_SUITE(exponential);

TEST(exp_basic) {
	ASSERT_FLOAT_EQ(2.71828, exp(1.0), EPSILON);
	ASSERT_FLOAT_EQ(7.38906, exp(2.0), EPSILON);
	ASSERT_FLOAT_EQ(1.0, exp(0.0), EPSILON);
	ASSERT_FLOAT_EQ(0.36788, exp(-1.0), EPSILON);
}

TEST(exp2_powers) {
	ASSERT_FLOAT_EQ(2.0, exp2(1.0), EPSILON);
	ASSERT_FLOAT_EQ(4.0, exp2(2.0), EPSILON);
	ASSERT_FLOAT_EQ(8.0, exp2(3.0), EPSILON);
	ASSERT_FLOAT_EQ(1.0, exp2(0.0), EPSILON);
	ASSERT_FLOAT_EQ(0.5, exp2(-1.0), EPSILON);
}

TEST(expm1_small_values) {
	ASSERT_FLOAT_EQ(0.0, expm1(0.0), EPSILON);
	ASSERT_FLOAT_EQ(1.71828, expm1(1.0), EPSILON);
	ASSERT_FLOAT_EQ(0.01005, expm1(0.01), 0.00001);
}

/* ============================================================================
 * LOGARITHM TESTS
 * ============================================================================ */
TEST_SUITE(logarithm);

TEST(log_basic) {
	ASSERT_FLOAT_EQ(0.0, log(1.0), EPSILON);
	ASSERT_FLOAT_EQ(1.0, log(M_E), EPSILON);
	ASSERT_FLOAT_EQ(2.30259, log(10.0), EPSILON);
}

TEST(log_invalid) {
	ASSERT_TRUE(isnan(log(-1.0)));
	ASSERT_TRUE(isnan(log(0.0)));
}

TEST(log2_powers) {
	ASSERT_FLOAT_EQ(0.0, log2(1.0), EPSILON);
	ASSERT_FLOAT_EQ(1.0, log2(2.0), EPSILON);
	ASSERT_FLOAT_EQ(3.0, log2(8.0), EPSILON);
	ASSERT_FLOAT_EQ(-1.0, log2(0.5), EPSILON);
}

TEST(log10_basic) {
	ASSERT_FLOAT_EQ(0.0, log10(1.0), EPSILON);
	ASSERT_FLOAT_EQ(1.0, log10(10.0), EPSILON);
	ASSERT_FLOAT_EQ(2.0, log10(100.0), EPSILON);
}

TEST(log1p_small_values) {
	ASSERT_FLOAT_EQ(0.0, log1p(0.0), EPSILON);
	ASSERT_FLOAT_EQ(0.69315, log1p(1.0), EPSILON);
}

/* ============================================================================
 * POWER TESTS
 * ============================================================================ */
TEST_SUITE(power);

TEST(pow_basic) {
	ASSERT_FLOAT_EQ(8.0, pow(2.0, 3.0), EPSILON);
	ASSERT_FLOAT_EQ(9.0, pow(3.0, 2.0), EPSILON);
	ASSERT_FLOAT_EQ(1.0, pow(5.0, 0.0), EPSILON);
	ASSERT_FLOAT_EQ(0.25, pow(2.0, -2.0), EPSILON);
}

TEST(pow_fractional) {
	ASSERT_FLOAT_EQ(2.0, pow(4.0, 0.5), EPSILON);
	ASSERT_FLOAT_EQ(3.0, pow(27.0, 1.0/3.0), EPSILON);
}

TEST(sqrt_basic) {
	ASSERT_FLOAT_EQ(2.0, sqrt(4.0), EPSILON);
	ASSERT_FLOAT_EQ(3.0, sqrt(9.0), EPSILON);
	ASSERT_FLOAT_EQ(5.0, sqrt(25.0), EPSILON);
	ASSERT_FLOAT_EQ(0.0, sqrt(0.0), EPSILON);
}

TEST(sqrt_invalid) {
	ASSERT_TRUE(isnan(sqrt(-1.0)));
}

TEST(cbrt_basic) {
	ASSERT_FLOAT_EQ(2.0, cbrt(8.0), EPSILON);
	ASSERT_FLOAT_EQ(3.0, cbrt(27.0), EPSILON);
	ASSERT_FLOAT_EQ(-2.0, cbrt(-8.0), EPSILON);
}

TEST(hypot_basic) {
	ASSERT_FLOAT_EQ(5.0, hypot(3.0, 4.0), EPSILON);
	ASSERT_FLOAT_EQ(13.0, hypot(5.0, 12.0), EPSILON);
	ASSERT_FLOAT_EQ(1.0, hypot(1.0, 0.0), EPSILON);
}

/* ============================================================================
 * TRIGONOMETRIC TESTS
 * ============================================================================ */
TEST_SUITE(trigonometric);

TEST(sin_basic) {
	ASSERT_FLOAT_EQ(0.0, sin(0.0), EPSILON);
	ASSERT_FLOAT_EQ(1.0, sin(M_PI_2), EPSILON);
	ASSERT_FLOAT_EQ(0.0, sin(M_PI), EPSILON);
	ASSERT_FLOAT_EQ(-1.0, sin(3.0 * M_PI_2), EPSILON);
}

TEST(cos_basic) {
	ASSERT_FLOAT_EQ(1.0, cos(0.0), EPSILON);
	ASSERT_FLOAT_EQ(0.0, cos(M_PI_2), EPSILON);
	ASSERT_FLOAT_EQ(-1.0, cos(M_PI), EPSILON);
	ASSERT_FLOAT_EQ(0.0, cos(3.0 * M_PI_2), EPSILON);
}

TEST(tan_basic) {
	ASSERT_FLOAT_EQ(0.0, tan(0.0), EPSILON);
	ASSERT_FLOAT_EQ(1.0, tan(M_PI_4), EPSILON);
	ASSERT_FLOAT_EQ(0.0, tan(M_PI), EPSILON);
}

TEST(asin_basic) {
	ASSERT_FLOAT_EQ(0.0, asin(0.0), EPSILON);
	ASSERT_FLOAT_EQ(M_PI_2, asin(1.0), EPSILON);
	ASSERT_FLOAT_EQ(-M_PI_2, asin(-1.0), EPSILON);
}

TEST(asin_invalid) {
	ASSERT_TRUE(isnan(asin(1.5)));
	ASSERT_TRUE(isnan(asin(-1.5)));
}

TEST(acos_basic) {
	ASSERT_FLOAT_EQ(M_PI_2, acos(0.0), EPSILON);
	ASSERT_FLOAT_EQ(0.0, acos(1.0), EPSILON);
	ASSERT_FLOAT_EQ(M_PI, acos(-1.0), EPSILON);
}

TEST(atan_basic) {
	ASSERT_FLOAT_EQ(0.0, atan(0.0), EPSILON);
	ASSERT_FLOAT_EQ(M_PI_4, atan(1.0), EPSILON);
	ASSERT_FLOAT_EQ(-M_PI_4, atan(-1.0), EPSILON);
}

TEST(atan2_quadrants) {
	ASSERT_FLOAT_EQ(M_PI_4, atan2(1.0, 1.0), EPSILON);
	ASSERT_FLOAT_EQ(3.0 * M_PI_4, atan2(1.0, -1.0), EPSILON);
	ASSERT_FLOAT_EQ(-3.0 * M_PI_4, atan2(-1.0, -1.0), EPSILON);
	ASSERT_FLOAT_EQ(-M_PI_4, atan2(-1.0, 1.0), EPSILON);
	ASSERT_FLOAT_EQ(M_PI_2, atan2(1.0, 0.0), EPSILON);
}

/* ============================================================================
 * HYPERBOLIC TESTS
 * ============================================================================ */
TEST_SUITE(hyperbolic);

TEST(sinh_basic) {
	ASSERT_FLOAT_EQ(0.0, sinh(0.0), EPSILON);
	ASSERT_FLOAT_EQ(1.17520, sinh(1.0), EPSILON);
	ASSERT_FLOAT_EQ(-1.17520, sinh(-1.0), EPSILON);
}

TEST(cosh_basic) {
	ASSERT_FLOAT_EQ(1.0, cosh(0.0), EPSILON);
	ASSERT_FLOAT_EQ(1.54308, cosh(1.0), EPSILON);
	ASSERT_FLOAT_EQ(1.54308, cosh(-1.0), EPSILON);
}

TEST(tanh_basic) {
	ASSERT_FLOAT_EQ(0.0, tanh(0.0), EPSILON);
	ASSERT_FLOAT_EQ(0.76159, tanh(1.0), EPSILON);
	ASSERT_FLOAT_EQ(-0.76159, tanh(-1.0), EPSILON);
}

TEST(asinh_basic) {
	ASSERT_FLOAT_EQ(0.0, asinh(0.0), EPSILON);
	ASSERT_FLOAT_EQ(0.88137, asinh(1.0), EPSILON);
	ASSERT_FLOAT_EQ(-0.88137, asinh(-1.0), EPSILON);
}

TEST(acosh_basic) {
	ASSERT_FLOAT_EQ(0.0, acosh(1.0), EPSILON);
	ASSERT_FLOAT_EQ(1.31696, acosh(2.0), EPSILON);
}

TEST(atanh_basic) {
	ASSERT_FLOAT_EQ(0.0, atanh(0.0), EPSILON);
	ASSERT_FLOAT_EQ(0.54931, atanh(0.5), EPSILON);
	ASSERT_FLOAT_EQ(-0.54931, atanh(-0.5), EPSILON);
}

/* ============================================================================
 * SCALING TESTS
 * ============================================================================ */
TEST_SUITE(scaling);

TEST(ldexp_basic) {
	ASSERT_FLOAT_EQ(8.0, ldexp(1.0, 3), EPSILON);
	ASSERT_FLOAT_EQ(0.5, ldexp(1.0, -1), EPSILON);
	ASSERT_FLOAT_EQ(16.0, ldexp(2.0, 3), EPSILON);
	ASSERT_FLOAT_EQ(0.0, ldexp(0.0, 10), EPSILON);
}

TEST(frexp_decompose) {
	int exp;
	double mantissa;
	
	mantissa = frexp(8.0, &exp);
	ASSERT_FLOAT_EQ(0.5, mantissa, EPSILON);
	ASSERT_EQ(4, exp);
	
	mantissa = frexp(0.5, &exp);
	ASSERT_FLOAT_EQ(0.5, mantissa, EPSILON);
	ASSERT_EQ(0, exp);
}

TEST(scalbn_basic) {
	ASSERT_FLOAT_EQ(8.0, scalbn(1.0, 3), EPSILON);
	ASSERT_FLOAT_EQ(0.25, scalbn(1.0, -2), EPSILON);
}

TEST(scalbln_basic) {
	ASSERT_FLOAT_EQ(16.0, scalbln(1.0, 4L), EPSILON);
	ASSERT_FLOAT_EQ(0.125, scalbln(1.0, -3L), EPSILON);
}

TEST(logb_extract_exponent) {
	ASSERT_FLOAT_EQ(3.0, logb(8.0), EPSILON);
	ASSERT_FLOAT_EQ(2.0, logb(7.0), EPSILON);
	ASSERT_FLOAT_EQ(-1.0, logb(0.5), EPSILON);
	ASSERT_TRUE(isinf(logb(0.0)));
}

TEST(ilogb_extract_exponent) {
	ASSERT_EQ(3, ilogb(8.0));
	ASSERT_EQ(2, ilogb(7.0));
	ASSERT_EQ(-1, ilogb(0.5));
	ASSERT_EQ(FP_ILOGB0, ilogb(0.0));
	ASSERT_EQ(FP_ILOGBNAN, ilogb(NAN));
}

/* ============================================================================
 * MODULO TESTS
 * ============================================================================ */
TEST_SUITE(modulo);

TEST(fmod_basic) {
	ASSERT_FLOAT_EQ(1.0, fmod(7.0, 3.0), EPSILON);
	ASSERT_FLOAT_EQ(0.0, fmod(8.0, 4.0), EPSILON);
	ASSERT_FLOAT_EQ(0.5, fmod(5.5, 1.0), EPSILON);
}

TEST(fmod_negative) {
	ASSERT_FLOAT_EQ(-1.0, fmod(-7.0, 3.0), EPSILON);
	ASSERT_FLOAT_EQ(1.0, fmod(7.0, -3.0), EPSILON);
}

TEST(remainder_basic) {
	ASSERT_FLOAT_EQ(1.0, remainder(7.0, 3.0), EPSILON);
	ASSERT_FLOAT_EQ(0.0, remainder(8.0, 4.0), EPSILON);
}

TEST(remquo_basic) {
	int quo;
	double rem;
	
	rem = remquo(7.0, 3.0, &quo);
	ASSERT_FLOAT_EQ(1.0, rem, EPSILON);
	ASSERT_EQ(2, quo);
	
	rem = remquo(8.0, 4.0, &quo);
	ASSERT_FLOAT_EQ(0.0, rem, EPSILON);
	ASSERT_EQ(2, quo);
}

TEST(modf_decompose) {
	double ipart;
	double fpart;
	
	fpart = modf(3.14, &ipart);
	ASSERT_FLOAT_EQ(3.0, ipart, EPSILON);
	ASSERT_FLOAT_EQ(0.14, fpart, EPSILON);
	
	fpart = modf(-2.7, &ipart);
	ASSERT_FLOAT_EQ(-2.0, ipart, EPSILON);
	ASSERT_FLOAT_EQ(-0.7, fpart, EPSILON);
}

/* ============================================================================
 * COMPARISON TESTS
 * ============================================================================ */
TEST_SUITE(comparison);

TEST(fdim_positive_difference) {
	ASSERT_FLOAT_EQ(3.0, fdim(5.0, 2.0), EPSILON);
	ASSERT_FLOAT_EQ(0.0, fdim(2.0, 5.0), EPSILON);
	ASSERT_FLOAT_EQ(0.0, fdim(3.0, 3.0), EPSILON);
}

TEST(fmax_maximum) {
	ASSERT_FLOAT_EQ(5.0, fmax(3.0, 5.0), EPSILON);
	ASSERT_FLOAT_EQ(5.0, fmax(5.0, 3.0), EPSILON);
	ASSERT_FLOAT_EQ(3.0, fmax(3.0, NAN), EPSILON);
	ASSERT_FLOAT_EQ(3.0, fmax(NAN, 3.0), EPSILON);
}

TEST(fmin_minimum) {
	ASSERT_FLOAT_EQ(3.0, fmin(3.0, 5.0), EPSILON);
	ASSERT_FLOAT_EQ(3.0, fmin(5.0, 3.0), EPSILON);
	ASSERT_FLOAT_EQ(3.0, fmin(3.0, NAN), EPSILON);
	ASSERT_FLOAT_EQ(3.0, fmin(NAN, 3.0), EPSILON);
}

TEST(fma_multiply_add) {
	ASSERT_FLOAT_EQ(11.0, fma(2.0, 3.0, 5.0), EPSILON);
	ASSERT_FLOAT_EQ(5.0, fma(0.0, 10.0, 5.0), EPSILON);
}

/* ============================================================================
 * ERROR FUNCTION TESTS
 * ============================================================================ */
TEST_SUITE(error_functions);

TEST(erf_basic) {
	ASSERT_FLOAT_EQ(0.0, erf(0.0), EPSILON);
	ASSERT_FLOAT_EQ(0.84270, erf(1.0), 0.001);
	ASSERT_FLOAT_EQ(-0.84270, erf(-1.0), 0.001);
}

TEST(erfc_complement) {
	ASSERT_FLOAT_EQ(1.0, erfc(0.0), EPSILON);
	ASSERT_FLOAT_EQ(0.15730, erfc(1.0), 0.001);
}

/* ============================================================================
 * GAMMA FUNCTION TESTS
 * ============================================================================ */
TEST_SUITE(gamma_functions);

TEST(tgamma_basic) {
	ASSERT_FLOAT_EQ(1.0, tgamma(1.0), 0.01);
	ASSERT_FLOAT_EQ(1.0, tgamma(2.0), 0.01);
	ASSERT_FLOAT_EQ(2.0, tgamma(3.0), 0.01);
	ASSERT_FLOAT_EQ(6.0, tgamma(4.0), 0.1);
}

TEST(lgamma_basic) {
	ASSERT_FLOAT_EQ(0.0, lgamma(1.0), 0.01);
	ASSERT_FLOAT_EQ(0.0, lgamma(2.0), 0.01);
}

/* ============================================================================
 * MANIPULATION TESTS
 * ============================================================================ */
TEST_SUITE(manipulation);

TEST(copysign_basic) {
	ASSERT_FLOAT_EQ(3.0, copysign(3.0, 1.0), EPSILON);
	ASSERT_FLOAT_EQ(-3.0, copysign(3.0, -1.0), EPSILON);
	ASSERT_FLOAT_EQ(3.0, copysign(-3.0, 1.0), EPSILON);
	ASSERT_FLOAT_EQ(-3.0, copysign(-3.0, -1.0), EPSILON);
}

TEST(nextafter_increment) {
	double x = 1.0;
	double next = nextafter(x, 2.0);
	
	ASSERT_TRUE(next > x);
	ASSERT_TRUE(next < 1.0 + 0.0001);
	
	double prev = nextafter(x, 0.0);
	ASSERT_TRUE(prev < x);
}

TEST(nan_generation) {
	double n = nan("");
	ASSERT_TRUE(isnan(n));
}

/* ============================================================================
 * BESSEL FUNCTION TESTS
 * ============================================================================ */
TEST_SUITE(bessel_functions);

TEST(j0_basic) {
	ASSERT_FLOAT_EQ(1.0, j0(0.0), EPSILON);
	ASSERT_FLOAT_EQ(-0.17760, j0(5.0), 0.001);
}

TEST(j1_basic) {
	ASSERT_FLOAT_EQ(0.0, j1(0.0), EPSILON);
	ASSERT_FLOAT_EQ(0.44005, j1(1.0), 0.001);
}

TEST(jn_basic) {
	ASSERT_FLOAT_EQ(0.0, jn(2, 0.0), EPSILON);
	ASSERT_FLOAT_EQ(0.11490, jn(2, 1.0), 0.001);
}

TEST(y0_basic) {
	ASSERT_FLOAT_EQ(-0.08826, y0(1.0), 0.001);
}

TEST(y1_basic) {
	ASSERT_FLOAT_EQ(-0.78122, y1(1.0), 0.001);
}

/* ============================================================================
 * EDGE CASE TESTS
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(zero_handling) {
	ASSERT_FLOAT_EQ(0.0, sin(0.0), EPSILON);
	ASSERT_FLOAT_EQ(0.0, log1p(0.0), EPSILON);
	ASSERT_FLOAT_EQ(1.0, exp(0.0), EPSILON);
	ASSERT_FLOAT_EQ(1.0, cos(0.0), EPSILON);
}

TEST(infinity_handling) {
	ASSERT_TRUE(isinf(exp(1000.0)));
	ASSERT_TRUE(isnan(log(0.0)));
}

TEST(negative_zero) {
	ASSERT_EQ(0, signbit(0.0));
	ASSERT_NE(0, signbit(-0.0));
	ASSERT_FLOAT_EQ(0.0, fabs(-0.0), EPSILON);
}

TEST_MAIN()
