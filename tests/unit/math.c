/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <math.h>

TEST_TYPE(unit);
TEST_UNIT(math.h);

#define EPSILON 0.0001
#define TIGHT_EPSILON 0.00001

/* ============================================================= */
/* Classification Tests                                          */
/* ============================================================= */
TEST_SUITE(classification);

TEST(classification_isnan_detection) {
	ASSERT_TRUE(isnan(NAN));
	ASSERT_TRUE(isnan(0.0 / 0.0));
	ASSERT_FALSE(isnan(0.0));
	ASSERT_FALSE(isnan(1.0));
	ASSERT_FALSE(isnan(INFINITY));
	ASSERT_FALSE(isnan(-INFINITY));
}

TEST(classification_isinf_detection) {
	ASSERT_TRUE(isinf(INFINITY));
	ASSERT_TRUE(isinf(-INFINITY));
	ASSERT_TRUE(isinf(1.0 / 0.0));
	ASSERT_TRUE(isinf(-1.0 / 0.0));
	ASSERT_FALSE(isinf(0.0));
	ASSERT_FALSE(isinf(1.0));
	ASSERT_FALSE(isinf(NAN));
}

TEST(classification_isfinite_detection) {
	ASSERT_TRUE(isfinite(0.0));
	ASSERT_TRUE(isfinite(1.0));
	ASSERT_TRUE(isfinite(-100.5));
	ASSERT_TRUE(isfinite(DBL_MAX));
	ASSERT_FALSE(isfinite(INFINITY));
	ASSERT_FALSE(isfinite(-INFINITY));
	ASSERT_FALSE(isfinite(NAN));
}

TEST(classification_fpclassify_types) {
	ASSERT_EQ(FP_ZERO, fpclassify(0.0));
	ASSERT_EQ(FP_ZERO, fpclassify(-0.0));
	ASSERT_EQ(FP_NORMAL, fpclassify(1.0));
	ASSERT_EQ(FP_INFINITE, fpclassify(INFINITY));
	ASSERT_EQ(FP_INFINITE, fpclassify(-INFINITY));
	ASSERT_EQ(FP_NAN, fpclassify(NAN));
}

TEST(classification_signbit_detection) {
	ASSERT_EQ(0, signbit(0.0));
	ASSERT_NE(0, signbit(-0.0));
	ASSERT_EQ(0, signbit(1.0));
	ASSERT_NE(0, signbit(-1.0));
	ASSERT_EQ(0, signbit(INFINITY));
	ASSERT_NE(0, signbit(-INFINITY));
}

TEST(classification_isnormal_detection) {
	ASSERT_TRUE(isnormal(1.0));
	ASSERT_TRUE(isnormal(-1.0));
	ASSERT_TRUE(isnormal(DBL_MIN));
	ASSERT_FALSE(isnormal(0.0));
	ASSERT_FALSE(isnormal(INFINITY));
	ASSERT_FALSE(isnormal(NAN));
}

TEST(classification_subnormal_detection) {
	ASSERT_EQ(FP_SUBNORMAL, fpclassify(DBL_TRUE_MIN));
	ASSERT_FALSE(isnormal(DBL_TRUE_MIN));
	ASSERT_TRUE(isfinite(DBL_TRUE_MIN));
}

/* ============================================================= */
/* Absolute Value Tests                                          */
/* ============================================================= */
TEST_SUITE(absolute_value);

TEST(abs_fabs_basic) {
	ASSERT_DBL_NEAR(5.0, fabs(5.0), EPSILON);
	ASSERT_DBL_NEAR(5.0, fabs(-5.0), EPSILON);
	ASSERT_DBL_NEAR(0.0, fabs(0.0), EPSILON);
	ASSERT_DBL_NEAR(0.0, fabs(-0.0), EPSILON);
}

TEST(abs_fabs_special) {
	ASSERT_INF(fabs(INFINITY));
	ASSERT_INF(fabs(-INFINITY));
	ASSERT_NAN(fabs(NAN));
}

TEST(abs_fabsf_basic) {
	ASSERT_DBL_NEAR(3.5f, fabsf(3.5f), EPSILON);
	ASSERT_DBL_NEAR(3.5f, fabsf(-3.5f), EPSILON);
	ASSERT_DBL_NEAR(0.0f, fabsf(0.0f), EPSILON);
}

#if JACL_HAS_C99
TEST(abs_fabsl_basic) {
	ASSERT_LDBL_NEAR(7.25L, fabsl(7.25L), EPSILON);
	ASSERT_LDBL_NEAR(7.25L, fabsl(-7.25L), EPSILON);
}
#endif

/* ============================================================= */
/* Rounding Tests                                                */
/* ============================================================= */
TEST_SUITE(rounding);

TEST(round_ceil_basic) {
	ASSERT_DBL_NEAR(3.0, ceil(2.1), EPSILON);
	ASSERT_DBL_NEAR(3.0, ceil(2.9), EPSILON);
	ASSERT_DBL_NEAR(3.0, ceil(3.0), EPSILON);
	ASSERT_DBL_NEAR(-2.0, ceil(-2.9), EPSILON);
	ASSERT_DBL_NEAR(-2.0, ceil(-2.1), EPSILON);
	ASSERT_DBL_NEAR(0.0, ceil(0.0), EPSILON);
	ASSERT_DBL_NEAR(0.0, ceil(-0.0), EPSILON);
}

TEST(round_floor_basic) {
	ASSERT_DBL_NEAR(2.0, floor(2.1), EPSILON);
	ASSERT_DBL_NEAR(2.0, floor(2.9), EPSILON);
	ASSERT_DBL_NEAR(3.0, floor(3.0), EPSILON);
	ASSERT_DBL_NEAR(-3.0, floor(-2.1), EPSILON);
	ASSERT_DBL_NEAR(-3.0, floor(-2.9), EPSILON);
	ASSERT_DBL_NEAR(0.0, floor(0.0), EPSILON);
}

TEST(round_trunc_basic) {
	ASSERT_DBL_NEAR(2.0, trunc(2.7), EPSILON);
	ASSERT_DBL_NEAR(-2.0, trunc(-2.7), EPSILON);
	ASSERT_DBL_NEAR(0.0, trunc(0.5), EPSILON);
	ASSERT_DBL_NEAR(0.0, trunc(-0.5), EPSILON);
	ASSERT_DBL_NEAR(3.0, trunc(3.0), EPSILON);
	ASSERT_DBL_NEAR(0.0, trunc(0.0), EPSILON);
}

TEST(round_round_basic) {
	ASSERT_DBL_NEAR(3.0, round(2.5), EPSILON);
	ASSERT_DBL_NEAR(2.0, round(2.4), EPSILON);
	ASSERT_DBL_NEAR(3.0, round(2.6), EPSILON);
	ASSERT_DBL_NEAR(-3.0, round(-2.5), EPSILON);
	ASSERT_DBL_NEAR(-2.0, round(-2.4), EPSILON);
	ASSERT_DBL_NEAR(0.0, round(0.0), EPSILON);
}

TEST(round_rint_bankers) {
	ASSERT_DBL_NEAR(2.0, rint(2.5), EPSILON);
	ASSERT_DBL_NEAR(2.0, rint(2.4), EPSILON);
	ASSERT_DBL_NEAR(3.0, rint(2.6), EPSILON);
	ASSERT_DBL_NEAR(4.0, rint(3.5), EPSILON);
	ASSERT_DBL_NEAR(4.0, rint(4.5), EPSILON);
	ASSERT_DBL_NEAR(6.0, rint(5.5), EPSILON);
}

TEST(round_nearbyint_bankers) {
	ASSERT_DBL_NEAR(2.0, nearbyint(2.5), EPSILON);
	ASSERT_DBL_NEAR(2.0, nearbyint(2.4), EPSILON);
	ASSERT_DBL_NEAR(4.0, nearbyint(3.5), EPSILON);
	ASSERT_DBL_NEAR(4.0, nearbyint(4.5), EPSILON);
}

TEST(round_exact_halfway) {
	ASSERT_DBL_NEAR(0.0, rint(-0.5), EPSILON);
	ASSERT_DBL_NEAR(2.0, rint(1.5), EPSILON);
	ASSERT_DBL_NEAR(2.0, rint(2.5), EPSILON);
	ASSERT_DBL_NEAR(4.0, rint(3.5), EPSILON);
}

#if JACL_HAS_C23
TEST(round_roundeven_basic) {
	ASSERT_DBL_NEAR(2.0, roundeven(2.5), EPSILON);  // Round to even
	ASSERT_DBL_NEAR(4.0, roundeven(3.5), EPSILON);  // Round to even
}
#endif

TEST(round_lround_conversion) {
	ASSERT_EQ(3L, lround(2.5));
	ASSERT_EQ(2L, lround(2.4));
	ASSERT_EQ(3L, lround(2.6));
	ASSERT_EQ(-3L, lround(-2.5));
	ASSERT_EQ(0L, lround(0.0));
}

TEST(round_llround_conversion) {
	ASSERT_EQ(3LL, llround(2.5));
	ASSERT_EQ(2LL, llround(2.4));
	ASSERT_EQ(-3LL, llround(-2.5));
}

TEST(round_lrint_exact) {
	ASSERT_EQ(3L, lrint(3.0));
	ASSERT_EQ(-5L, lrint(-5.0));
	ASSERT_EQ(0L, lrint(0.0));
}

TEST(round_llrint_exact) {
	ASSERT_EQ(100LL, llrint(100.0));
	ASSERT_EQ(-50LL, llrint(-50.0));
}

TEST(round_float_variants) {
	ASSERT_DBL_NEAR(3.0f, ceilf(2.1f), EPSILON);
	ASSERT_DBL_NEAR(2.0f, floorf(2.9f), EPSILON);
	ASSERT_DBL_NEAR(2.0f, truncf(2.7f), EPSILON);
	ASSERT_DBL_NEAR(3.0f, roundf(2.5f), EPSILON);
	ASSERT_DBL_NEAR(-2.0f, ceilf(-2.5f), EPSILON);
	ASSERT_DBL_NEAR(-3.0f, floorf(-2.5f), EPSILON);
}

/* ============================================================= */
/* Exponential Tests                                             */
/* ============================================================= */
TEST_SUITE(exponential);

TEST(exp_exp_basic) {
	ASSERT_DBL_NEAR(2.71828, exp(1.0), EPSILON);
	ASSERT_DBL_NEAR(7.38906, exp(2.0), EPSILON);
	ASSERT_DBL_NEAR(1.0, exp(0.0), EPSILON);
	ASSERT_DBL_NEAR(0.36788, exp(-1.0), EPSILON);
}

TEST(exp_exp_overflow) {
	ASSERT_INF(exp(1000.0));
	ASSERT_INF(exp(800.0));
}

TEST(exp_exp_underflow) {
	ASSERT_DBL_NEAR(0.0, exp(-1000.0), EPSILON);
	ASSERT_DBL_NEAR(0.0, exp(-800.0), EPSILON);
}

TEST(exp_exp2_powers) {
	ASSERT_DBL_NEAR(2.0, exp2(1.0), EPSILON);
	ASSERT_DBL_NEAR(4.0, exp2(2.0), EPSILON);
	ASSERT_DBL_NEAR(8.0, exp2(3.0), EPSILON);
	ASSERT_DBL_NEAR(1.0, exp2(0.0), EPSILON);
	ASSERT_DBL_NEAR(0.5, exp2(-1.0), EPSILON);
	ASSERT_DBL_NEAR(0.25, exp2(-2.0), EPSILON);
}

TEST(exp_expm1_small_values) {
	ASSERT_DBL_NEAR(0.0, expm1(0.0), EPSILON);
	ASSERT_DBL_NEAR(1.71828, expm1(1.0), EPSILON);
	ASSERT_DBL_NEAR(0.01005, expm1(0.01), TIGHT_EPSILON);
}

TEST(exp_float_variants) {
	ASSERT_DBL_NEAR(2.71828f, expf(1.0f), EPSILON);
	ASSERT_DBL_NEAR(2.0f, exp2f(1.0f), EPSILON);
}

#if JACL_HAS_C99
TEST(exp_expl_basic) {
	ASSERT_LDBL_NEAR(2.71828L, expl(1.0L), EPSILON);
}
#endif

/* ============================================================= */
/* Logarithm Tests                                               */
/* ============================================================= */
TEST_SUITE(logarithm);

TEST(log_log_basic) {
	ASSERT_DBL_NEAR(0.0, log(1.0), EPSILON);
	ASSERT_DBL_NEAR(1.0, log(M_E), EPSILON);
	ASSERT_DBL_NEAR(2.30259, log(10.0), EPSILON);
}

TEST(log_log_invalid) {
	ASSERT_NAN(log(-1.0));
	ASSERT_NAN(log(-INFINITY));
	ASSERT_INF(log(0.0));
	ASSERT_NAN(log(NAN));
}

TEST(log_log2_powers) {
	ASSERT_DBL_NEAR(0.0, log2(1.0), EPSILON);
	ASSERT_DBL_NEAR(1.0, log2(2.0), EPSILON);
	ASSERT_DBL_NEAR(3.0, log2(8.0), EPSILON);
	ASSERT_DBL_NEAR(-1.0, log2(0.5), EPSILON);
	ASSERT_DBL_NEAR(-2.0, log2(0.25), EPSILON);
}

TEST(log_log10_basic) {
	ASSERT_DBL_NEAR(0.0, log10(1.0), EPSILON);
	ASSERT_DBL_NEAR(1.0, log10(10.0), EPSILON);
	ASSERT_DBL_NEAR(2.0, log10(100.0), EPSILON);
	ASSERT_DBL_NEAR(3.0, log10(1000.0), EPSILON);
}

TEST(log_log1p_small_values) {
	ASSERT_DBL_NEAR(0.0, log1p(0.0), EPSILON);
	ASSERT_DBL_NEAR(0.69315, log1p(1.0), EPSILON);
	ASSERT_DBL_NEAR(0.00995, log1p(0.01), TIGHT_EPSILON);
}

TEST(log_float_variants) {
	ASSERT_DBL_NEAR(0.0f, logf(1.0f), EPSILON);
	ASSERT_DBL_NEAR(1.0f, log2f(2.0f), EPSILON);
	ASSERT_DBL_NEAR(1.0f, log10f(10.0f), EPSILON);
	ASSERT_NAN(logf(-1.0f));
}

#if JACL_HAS_C99
TEST(log_logl_basic) {
	ASSERT_LDBL_NEAR(0.0L, logl(1.0L), EPSILON);
}
#endif

/* ============================================================= */
/* Power Tests                                                   */
/* ============================================================= */
TEST_SUITE(power);

TEST(pow_pow_basic) {
	ASSERT_DBL_NEAR(8.0, pow(2.0, 3.0), EPSILON);
	ASSERT_DBL_NEAR(9.0, pow(3.0, 2.0), EPSILON);
	ASSERT_DBL_NEAR(1.0, pow(5.0, 0.0), EPSILON);
	ASSERT_DBL_NEAR(0.25, pow(2.0, -2.0), EPSILON);
}

TEST(pow_pow_fractional) {
	ASSERT_DBL_NEAR(2.0, pow(4.0, 0.5), EPSILON);
	ASSERT_DBL_NEAR(3.0, pow(27.0, 1.0/3.0), EPSILON);
}

TEST(pow_pow_overflow) {
	ASSERT_INF(pow(10.0, 400.0));
	ASSERT_INF(pow(2.0, 10000.0));
}

TEST(pow_pow_special) {
	ASSERT_DBL_NEAR(1.0, pow(0.0, 0.0), EPSILON);
	ASSERT_NAN(pow(-1.0, 0.5));
	ASSERT_DBL_NEAR(1.0, pow(-1.0, 2.0), EPSILON);
}

TEST(pow_sqrt_basic) {
	ASSERT_DBL_NEAR(2.0, sqrt(4.0), EPSILON);
	ASSERT_DBL_NEAR(3.0, sqrt(9.0), EPSILON);
	ASSERT_DBL_NEAR(5.0, sqrt(25.0), EPSILON);
	ASSERT_DBL_NEAR(0.0, sqrt(0.0), EPSILON);
	ASSERT_DBL_NEAR(10.0, sqrt(100.0), EPSILON);
}

TEST(pow_sqrt_invalid) {
	ASSERT_NAN(sqrt(-1.0));
	ASSERT_NAN(sqrt(-100.0));
}

TEST(pow_cbrt_basic) {
	ASSERT_DBL_NEAR(2.0, cbrt(8.0), EPSILON);
	ASSERT_DBL_NEAR(3.0, cbrt(27.0), EPSILON);
	ASSERT_DBL_NEAR(-2.0, cbrt(-8.0), EPSILON);
	ASSERT_DBL_NEAR(0.0, cbrt(0.0), EPSILON);
}

TEST(pow_hypot_basic) {
	ASSERT_DBL_NEAR(5.0, hypot(3.0, 4.0), EPSILON);
	ASSERT_DBL_NEAR(13.0, hypot(5.0, 12.0), EPSILON);
	ASSERT_DBL_NEAR(1.0, hypot(1.0, 0.0), EPSILON);
	ASSERT_DBL_NEAR(1.0, hypot(0.0, 1.0), EPSILON);
}

TEST(pow_float_variants) {
	ASSERT_DBL_NEAR(8.0f, powf(2.0f, 3.0f), EPSILON);
	ASSERT_DBL_NEAR(2.0f, sqrtf(4.0f), EPSILON);
	ASSERT_DBL_NEAR(2.0f, cbrtf(8.0f), EPSILON);
	ASSERT_DBL_NEAR(5.0f, hypotf(3.0f, 4.0f), EPSILON);
}

#if JACL_HAS_C99
TEST(pow_sqrtl_basic) {
	ASSERT_LDBL_NEAR(2.0L, sqrtl(4.0L), EPSILON);
	ASSERT_LDBL_NEAR(3.0L, sqrtl(9.0L), EPSILON);
}

TEST(pow_powl_basic) {
	ASSERT_LDBL_NEAR(8.0L, powl(2.0L, 3.0L), EPSILON);
}
#endif

/* ============================================================= */
/* Trigonometric Tests                                           */
/* ============================================================= */
TEST_SUITE(trigonometric);

TEST(trig_sin_basic) {
	ASSERT_DBL_NEAR(0.0, sin(0.0), EPSILON);
	ASSERT_DBL_NEAR(1.0, sin(M_PI_2), EPSILON);
	ASSERT_DBL_NEAR(0.0, sin(M_PI), EPSILON);
	ASSERT_DBL_NEAR(-1.0, sin(3.0 * M_PI_2), EPSILON);
	ASSERT_DBL_NEAR(0.0, sin(2.0 * M_PI), EPSILON);
}

TEST(trig_cos_basic) {
	ASSERT_DBL_NEAR(1.0, cos(0.0), EPSILON);
	ASSERT_DBL_NEAR(0.0, cos(M_PI_2), EPSILON);
	ASSERT_DBL_NEAR(-1.0, cos(M_PI), EPSILON);
	ASSERT_DBL_NEAR(0.0, cos(3.0 * M_PI_2), EPSILON);
	ASSERT_DBL_NEAR(1.0, cos(2.0 * M_PI), EPSILON);
}

TEST(trig_tan_basic) {
	ASSERT_DBL_NEAR(0.0, tan(0.0), EPSILON);
	ASSERT_DBL_NEAR(1.0, tan(M_PI_4), EPSILON);
	ASSERT_DBL_NEAR(0.0, tan(M_PI), EPSILON);
	ASSERT_DBL_NEAR(-1.0, tan(-M_PI_4), EPSILON);
}

TEST(trig_asin_basic) {
	ASSERT_DBL_NEAR(0.0, asin(0.0), EPSILON);
	ASSERT_DBL_NEAR(M_PI_2, asin(1.0), EPSILON);
	ASSERT_DBL_NEAR(-M_PI_2, asin(-1.0), EPSILON);
	ASSERT_DBL_NEAR(M_PI / 6.0, asin(0.5), EPSILON);
}

TEST(trig_asin_invalid) {
	ASSERT_NAN(asin(1.5));
	ASSERT_NAN(asin(-1.5));
	ASSERT_NAN(asin(2.0));
}

TEST(trig_acos_basic) {
	ASSERT_DBL_NEAR(M_PI_2, acos(0.0), EPSILON);
	ASSERT_DBL_NEAR(0.0, acos(1.0), EPSILON);
	ASSERT_DBL_NEAR(M_PI, acos(-1.0), EPSILON);
}

TEST(trig_acos_invalid) {
	ASSERT_NAN(acos(1.5));
	ASSERT_NAN(acos(-1.5));
}

TEST(trig_atan_basic) {
	ASSERT_DBL_NEAR(0.0, atan(0.0), EPSILON);
	ASSERT_DBL_NEAR(M_PI_4, atan(1.0), EPSILON);
	ASSERT_DBL_NEAR(-M_PI_4, atan(-1.0), EPSILON);
}

TEST(trig_atan2_quadrants) {
	ASSERT_DBL_NEAR(M_PI_4, atan2(1.0, 1.0), EPSILON);
	ASSERT_DBL_NEAR(3.0 * M_PI_4, atan2(1.0, -1.0), EPSILON);
	ASSERT_DBL_NEAR(-3.0 * M_PI_4, atan2(-1.0, -1.0), EPSILON);
	ASSERT_DBL_NEAR(-M_PI_4, atan2(-1.0, 1.0), EPSILON);
	ASSERT_DBL_NEAR(M_PI_2, atan2(1.0, 0.0), EPSILON);
	ASSERT_DBL_NEAR(-M_PI_2, atan2(-1.0, 0.0), EPSILON);
}

TEST(trig_float_variants) {
	ASSERT_DBL_NEAR(0.0f, sinf(0.0f), EPSILON);
	ASSERT_DBL_NEAR(1.0f, cosf(0.0f), EPSILON);
	ASSERT_DBL_NEAR(0.0f, tanf(0.0f), EPSILON);
}

/* ============================================================= */
/* Hyperbolic Tests                                              */
/* ============================================================= */
TEST_SUITE(hyperbolic);

TEST(hyp_sinh_basic) {
	ASSERT_DBL_NEAR(0.0, sinh(0.0), EPSILON);
	ASSERT_DBL_NEAR(1.17520, sinh(1.0), EPSILON);
	ASSERT_DBL_NEAR(-1.17520, sinh(-1.0), EPSILON);
}

TEST(hyp_cosh_basic) {
	ASSERT_DBL_NEAR(1.0, cosh(0.0), EPSILON);
	ASSERT_DBL_NEAR(1.54308, cosh(1.0), EPSILON);
	ASSERT_DBL_NEAR(1.54308, cosh(-1.0), EPSILON);
}

TEST(hyp_tanh_basic) {
	ASSERT_DBL_NEAR(0.0, tanh(0.0), EPSILON);
	ASSERT_DBL_NEAR(0.76159, tanh(1.0), EPSILON);
	ASSERT_DBL_NEAR(-0.76159, tanh(-1.0), EPSILON);
}

TEST(hyp_asinh_basic) {
	ASSERT_DBL_NEAR(0.0, asinh(0.0), EPSILON);
	ASSERT_DBL_NEAR(0.88137, asinh(1.0), EPSILON);
	ASSERT_DBL_NEAR(-0.88137, asinh(-1.0), EPSILON);
}

TEST(hyp_acosh_basic) {
	ASSERT_DBL_NEAR(0.0, acosh(1.0), EPSILON);
	ASSERT_DBL_NEAR(1.31696, acosh(2.0), EPSILON);
	ASSERT_DBL_NEAR(2.29243, acosh(5.0), EPSILON);
}

TEST(hyp_acosh_invalid) {
	ASSERT_NAN(acosh(0.5));
	ASSERT_NAN(acosh(0.0));
	ASSERT_NAN(acosh(-1.0));
}

TEST(hyp_atanh_basic) {
	ASSERT_DBL_NEAR(0.0, atanh(0.0), EPSILON);
	ASSERT_DBL_NEAR(0.54931, atanh(0.5), EPSILON);
	ASSERT_DBL_NEAR(-0.54931, atanh(-0.5), EPSILON);
}

TEST(hyp_atanh_invalid) {
	ASSERT_NAN(atanh(2.0));
	ASSERT_NAN(atanh(-2.0));
	ASSERT_INF(atanh(1.0));
	ASSERT_INF(atanh(-1.0));
}

TEST(hyp_float_variants) {
	ASSERT_DBL_NEAR(0.0f, sinhf(0.0f), EPSILON);
	ASSERT_DBL_NEAR(1.0f, coshf(0.0f), EPSILON);
	ASSERT_DBL_NEAR(0.0f, tanhf(0.0f), EPSILON);
}

/* ============================================================= */
/* Scaling Tests                                                 */
/* ============================================================= */
TEST_SUITE(scaling);

TEST(scale_ldexp_basic) {
	ASSERT_DBL_NEAR(8.0, ldexp(1.0, 3), EPSILON);
	ASSERT_DBL_NEAR(0.5, ldexp(1.0, -1), EPSILON);
	ASSERT_DBL_NEAR(16.0, ldexp(2.0, 3), EPSILON);
	ASSERT_DBL_NEAR(0.0, ldexp(0.0, 10), EPSILON);
}

TEST(scale_frexp_decompose) {
	int exp;
	double mantissa;

	mantissa = frexp(8.0, &exp);
	ASSERT_DBL_NEAR(0.5, mantissa, EPSILON);
	ASSERT_EQ(4, exp);

	mantissa = frexp(0.5, &exp);
	ASSERT_DBL_NEAR(0.5, mantissa, EPSILON);
	ASSERT_EQ(0, exp);

	mantissa = frexp(1.0, &exp);
	ASSERT_DBL_NEAR(0.5, mantissa, EPSILON);
	ASSERT_EQ(1, exp);
}

TEST(scale_scalbn_basic) {
	ASSERT_DBL_NEAR(8.0, scalbn(1.0, 3), EPSILON);
	ASSERT_DBL_NEAR(0.25, scalbn(1.0, -2), EPSILON);
	ASSERT_DBL_NEAR(64.0, scalbn(2.0, 5), EPSILON);
}

TEST(scale_scalbln_basic) {
	ASSERT_DBL_NEAR(16.0, scalbln(1.0, 4L), EPSILON);
	ASSERT_DBL_NEAR(0.125, scalbln(1.0, -3L), EPSILON);
}

TEST(scale_logb_extract_exponent) {
	ASSERT_DBL_NEAR(3.0, logb(8.0), EPSILON);
	ASSERT_DBL_NEAR(2.0, logb(7.0), EPSILON);
	ASSERT_DBL_NEAR(-1.0, logb(0.5), EPSILON);
	ASSERT_INF(logb(0.0));
}

TEST(scale_ilogb_extract_exponent) {
	ASSERT_EQ(3, ilogb(8.0));
	ASSERT_EQ(2, ilogb(7.0));
	ASSERT_EQ(-1, ilogb(0.5));
	ASSERT_EQ(FP_ILOGB0, ilogb(0.0));
	ASSERT_EQ(FP_ILOGBNAN, ilogb(NAN));
}

TEST(scale_float_variants) {
	ASSERT_DBL_NEAR(8.0f, ldexpf(1.0f, 3), EPSILON);
	ASSERT_DBL_NEAR(8.0f, scalbnf(1.0f, 3), EPSILON);
	ASSERT_DBL_NEAR(3.0f, logbf(8.0f), EPSILON);
	ASSERT_EQ(3, ilogbf(8.0f));
}

/* ============================================================= */
/* Modulo Tests                                                  */
/* ============================================================= */
TEST_SUITE(modulo);

TEST(mod_fmod_basic) {
	ASSERT_DBL_NEAR(1.0, fmod(7.0, 3.0), EPSILON);
	ASSERT_DBL_NEAR(0.0, fmod(8.0, 4.0), EPSILON);
	ASSERT_DBL_NEAR(0.5, fmod(5.5, 1.0), EPSILON);
	ASSERT_DBL_NEAR(2.0, fmod(10.0, 4.0), EPSILON);
}

TEST(mod_fmod_negative) {
	ASSERT_DBL_NEAR(-1.0, fmod(-7.0, 3.0), EPSILON);
	ASSERT_DBL_NEAR(1.0, fmod(7.0, -3.0), EPSILON);
	ASSERT_DBL_NEAR(-1.0, fmod(-7.0, -3.0), EPSILON);
}

TEST(mod_remainder_basic) {
	ASSERT_DBL_NEAR(1.0, remainder(7.0, 3.0), EPSILON);
	ASSERT_DBL_NEAR(0.0, remainder(8.0, 4.0), EPSILON);
	ASSERT_DBL_NEAR(1.0, remainder(9.0, 4.0), EPSILON);
}

TEST(mod_remquo_basic) {
	int quo;
	double rem;

	rem = remquo(7.0, 3.0, &quo);
	ASSERT_DBL_NEAR(1.0, rem, EPSILON);
	ASSERT_EQ(2, quo);

	rem = remquo(8.0, 4.0, &quo);
	ASSERT_DBL_NEAR(0.0, rem, EPSILON);
	ASSERT_EQ(2, quo);
}

TEST(mod_modf_decompose) {
	double ipart;
	double fpart;

	fpart = modf(3.14, &ipart);
	ASSERT_DBL_NEAR(3.0, ipart, EPSILON);
	ASSERT_DBL_NEAR(0.14, fpart, 0.01);

	fpart = modf(-2.7, &ipart);
	ASSERT_DBL_NEAR(-2.0, ipart, EPSILON);
	ASSERT_DBL_NEAR(-0.7, fpart, 0.01);

	fpart = modf(5.0, &ipart);
	ASSERT_DBL_NEAR(5.0, ipart, EPSILON);
	ASSERT_DBL_NEAR(0.0, fpart, EPSILON);
}

TEST(mod_float_variants) {
	ASSERT_DBL_NEAR(1.0f, fmodf(7.0f, 3.0f), EPSILON);
	ASSERT_DBL_NEAR(1.0f, remainderf(7.0f, 3.0f), EPSILON);
}

/* ============================================================= */
/* Comparison Tests                                              */
/* ============================================================= */
TEST_SUITE(comparison);

TEST(cmp_fdim_positive_difference) {
	ASSERT_DBL_NEAR(3.0, fdim(5.0, 2.0), EPSILON);
	ASSERT_DBL_NEAR(0.0, fdim(2.0, 5.0), EPSILON);
	ASSERT_DBL_NEAR(0.0, fdim(3.0, 3.0), EPSILON);
}

TEST(cmp_fmax_maximum) {
	ASSERT_DBL_NEAR(5.0, fmax(3.0, 5.0), EPSILON);
	ASSERT_DBL_NEAR(5.0, fmax(5.0, 3.0), EPSILON);
	ASSERT_DBL_NEAR(3.0, fmax(3.0, NAN), EPSILON);
	ASSERT_DBL_NEAR(3.0, fmax(NAN, 3.0), EPSILON);
}

TEST(cmp_fmin_minimum) {
	ASSERT_DBL_NEAR(3.0, fmin(3.0, 5.0), EPSILON);
	ASSERT_DBL_NEAR(3.0, fmin(5.0, 3.0), EPSILON);
	ASSERT_DBL_NEAR(3.0, fmin(3.0, NAN), EPSILON);
	ASSERT_DBL_NEAR(3.0, fmin(NAN, 3.0), EPSILON);
}

TEST(cmp_fma_multiply_add) {
	ASSERT_DBL_NEAR(11.0, fma(2.0, 3.0, 5.0), EPSILON);
	ASSERT_DBL_NEAR(5.0, fma(0.0, 10.0, 5.0), EPSILON);
	ASSERT_DBL_NEAR(23.0, fma(4.0, 5.0, 3.0), EPSILON);
}

TEST(cmp_fma_precision) {
	double a = 1e308;
	double b = 2.0;
	double c = -1e308;
	double precise = fma(a, b, c);
	ASSERT_DBL_NEAR(1e308, precise, 0.001);
}

TEST(cmp_float_variants) {
	ASSERT_DBL_NEAR(3.0f, fdimf(5.0f, 2.0f), EPSILON);
	ASSERT_DBL_NEAR(5.0f, fmaxf(3.0f, 5.0f), EPSILON);
	ASSERT_DBL_NEAR(3.0f, fminf(3.0f, 5.0f), EPSILON);
	ASSERT_DBL_NEAR(11.0f, fmaf(2.0f, 3.0f, 5.0f), EPSILON);
}

/* ============================================================= */
/* Error Function Tests                                          */
/* ============================================================= */
TEST_SUITE(error_functions);

TEST(erf_erf_basic) {
	ASSERT_DBL_NEAR(0.0, erf(0.0), EPSILON);
	ASSERT_DBL_NEAR(0.84270, erf(1.0), 0.001);
	ASSERT_DBL_NEAR(-0.84270, erf(-1.0), 0.001);
	ASSERT_DBL_NEAR(0.99532, erf(2.0), 0.001);
}

TEST(erf_erfc_complement) {
	ASSERT_DBL_NEAR(1.0, erfc(0.0), EPSILON);
	ASSERT_DBL_NEAR(0.15730, erfc(1.0), 0.001);
	ASSERT_DBL_NEAR(1.84270, erfc(-1.0), 0.001);
}

TEST(erf_float_variants) {
	ASSERT_DBL_NEAR(0.0f, erff(0.0f), EPSILON);
	ASSERT_DBL_NEAR(1.0f, erfcf(0.0f), EPSILON);
}

/* ============================================================= */
/* Gamma Function Tests                                          */
/* ============================================================= */
TEST_SUITE(gamma_functions);

TEST(gamma_tgamma_basic) {
	ASSERT_DBL_NEAR(1.0, tgamma(1.0), 0.01);
	ASSERT_DBL_NEAR(1.0, tgamma(2.0), 0.01);
	ASSERT_DBL_NEAR(2.0, tgamma(3.0), 0.01);
	ASSERT_DBL_NEAR(6.0, tgamma(4.0), 0.1);
	ASSERT_DBL_NEAR(24.0, tgamma(5.0), 0.5);
}

TEST(gamma_lgamma_basic) {
	ASSERT_DBL_NEAR(0.0, lgamma(1.0), 0.01);
	ASSERT_DBL_NEAR(0.0, lgamma(2.0), 0.01);
	ASSERT_DBL_NEAR(0.69315, lgamma(3.0), 0.01);
	ASSERT_DBL_NEAR(1.79176, lgamma(4.0), 0.01);
}

TEST(gamma_float_variants) {
	ASSERT_DBL_NEAR(1.0f, tgammaf(1.0f), 0.01);
	ASSERT_DBL_NEAR(0.0f, lgammaf(1.0f), 0.01);
}

/* ============================================================= */
/* Manipulation Tests                                            */
/* ============================================================= */
TEST_SUITE(manipulation);

TEST(manip_copysign_basic) {
	ASSERT_DBL_NEAR(3.0, copysign(3.0, 1.0), EPSILON);
	ASSERT_DBL_NEAR(-3.0, copysign(3.0, -1.0), EPSILON);
	ASSERT_DBL_NEAR(3.0, copysign(-3.0, 1.0), EPSILON);
	ASSERT_DBL_NEAR(-3.0, copysign(-3.0, -1.0), EPSILON);
	ASSERT_DBL_NEAR(0.0, copysign(0.0, 1.0), EPSILON);
	ASSERT_NE(0, signbit(copysign(0.0, -1.0)));
}

TEST(manip_copysign_zero_edge) {
	ASSERT_EQ(0, signbit(copysign(0.0, 1.0)));
	ASSERT_NE(0, signbit(copysign(0.0, -1.0)));
	ASSERT_EQ(0, signbit(copysign(-0.0, 1.0)));
	ASSERT_NE(0, signbit(copysign(-0.0, -1.0)));
}

TEST(manip_nextafter_increment) {
	double x = 1.0;
	double next = nextafter(x, 2.0);
	ASSERT_DBL_GT(next, x);
	ASSERT_DBL_LT(next, 1.0 + 0.0001);

	double prev = nextafter(x, 0.0);
	ASSERT_TRUE(prev < x);
	ASSERT_TRUE(prev > 0.9999);
}

TEST(manip_nextafter_edges) {
	ASSERT_DBL_NEAR(0.0, nextafter(0.0, 0.0), 0.0);
	ASSERT_DBL_GT(nextafter(DBL_MAX, INFINITY), DBL_MAX);
	ASSERT_INF(nextafter(DBL_MAX, INFINITY));
	ASSERT_DBL_NEAR(0.0, nextafter(DBL_TRUE_MIN, 0.0), 0.0);
}

#if JACL_HAS_C99
TEST(manip_nexttoward_increment) {
	double x = 1.0;
	double next = nexttoward(x, 2.0L);
	ASSERT_DBL_GT(next, x);
	ASSERT_DBL_LT(next, 1.0001);
}
#endif

TEST(manip_nan_generation) {
	double n = nan("");
	ASSERT_NAN(n);
}

TEST(manip_float_variants) {
	ASSERT_DBL_NEAR(3.0f, copysignf(3.0f, 1.0f), EPSILON);
	ASSERT_NAN(nanf(""));
}

/* ============================================================= */
/* Special Value Tests                                           */
/* ============================================================= */
TEST_SUITE(special_values);

TEST(special_nan_propagation) {
	ASSERT_NAN(sin(NAN));
	ASSERT_NAN(cos(NAN));
	ASSERT_NAN(tan(NAN));
	ASSERT_NAN(sqrt(NAN));
	ASSERT_NAN(log(NAN));
	ASSERT_NAN(exp(NAN));
	ASSERT_NAN(pow(NAN, 2.0));
	ASSERT_NAN(pow(2.0, NAN));
	ASSERT_NAN(fmod(NAN, 1.0));
}

TEST(special_infinity_operations) {
	ASSERT_INF(INFINITY + 1.0);
	ASSERT_INF(INFINITY * 2.0);
	ASSERT_INF(INFINITY / 2.0);
	ASSERT_NAN(INFINITY - INFINITY);
	ASSERT_NAN(INFINITY / INFINITY);
	ASSERT_DBL_NEAR(0.0, 1.0 / INFINITY, EPSILON);
	ASSERT_INF(1.0 / 0.0);
}

TEST(special_zero_handling) {
	ASSERT_DBL_NEAR(0.0, sin(0.0), EPSILON);
	ASSERT_DBL_NEAR(0.0, log1p(0.0), EPSILON);
	ASSERT_DBL_NEAR(1.0, exp(0.0), EPSILON);
	ASSERT_DBL_NEAR(1.0, cos(0.0), EPSILON);
	ASSERT_DBL_NEAR(0.0, tan(0.0), EPSILON);
}

TEST(special_negative_zero) {
	ASSERT_EQ(0, signbit(0.0));
	ASSERT_NE(0, signbit(-0.0));
	ASSERT_DBL_NEAR(0.0, fabs(-0.0), EPSILON);
	ASSERT_DBL_NEAR(-0.0, -0.0, EPSILON);
}

TEST(special_subnormal_operations) {
	double tiny = DBL_TRUE_MIN;
	ASSERT_TRUE(tiny > 0.0);
	ASSERT_TRUE(tiny < DBL_MIN);
	ASSERT_DBL_NEAR(tiny, fabs(tiny), 0.0);
	ASSERT_FINITE(tiny);
	ASSERT_FALSE(isnormal(tiny));
}

TEST(special_quiet_nan) {
	double qnan = nan("");
	ASSERT_NAN(qnan);
	ASSERT_NAN(qnan + 1.0);
}

#ifdef FP_FAST_FMA
TEST(special_fma_performance) {
	ASSERT_DBL_NEAR(11.0, fma(2.0, 3.0, 5.0), EPSILON);
}
#endif

TEST(special_bessel_j0) {
	ASSERT_DBL_NEAR(1.0, j0(0.0), EPSILON);
	ASSERT_DBL_NEAR(-0.260052, j0(3.0), 0.001);
	ASSERT_DBL_NEAR(0.765198, j0(1.0), 0.001);
	ASSERT_DBL_NEAR(-0.177597, j0(5.0), 0.001);
}

TEST(special_bessel_j1) {
	ASSERT_DBL_NEAR(0.0, j1(0.0), EPSILON);
	ASSERT_DBL_NEAR(0.339059, j1(3.0), 0.001);
	ASSERT_DBL_NEAR(0.440051, j1(1.0), 0.001);
	ASSERT_DBL_NEAR(-0.327579, j1(5.0), 0.001);
}

TEST(special_bessel_jn) {
	// jn(0, x) should equal j0(x)
	ASSERT_DBL_NEAR(j0(3.0), jn(0, 3.0), EPSILON);

	// jn(1, x) should equal j1(x)
	ASSERT_DBL_NEAR(j1(3.0), jn(1, 3.0), EPSILON);

	// jn(2, 3.0) reference value
	ASSERT_DBL_NEAR(0.486091, jn(2, 3.0), 0.001);

	// jn(3, 5.0) reference value
	ASSERT_DBL_NEAR(0.364831, jn(3, 5.0), 0.001);
}

TEST(special_bessel_y0) {
	ASSERT_INF(y0(0.0));
	ASSERT_DBL_LT(y0(0.0), 0);
	ASSERT_DBL_NEAR(0.376850, y0(3.0), 0.001);
	ASSERT_DBL_NEAR(0.088257, y0(1.0), 0.001);
	ASSERT_DBL_NEAR(-0.308518, y0(5.0), 0.001);
}

TEST(special_bessel_y1) {
	ASSERT_INF(y1(0.0));
	ASSERT_DBL_LT(y1(0.0), 0);
	ASSERT_DBL_NEAR(0.324674, y1(3.0), 0.001);
	ASSERT_DBL_NEAR(-0.781213, y1(1.0), 0.001);
	ASSERT_DBL_NEAR(0.147863, y1(5.0), 0.001);
}

TEST(special_bessel_yn) {
	// yn(0, x) should equal y0(x)
	ASSERT_DBL_NEAR(y0(3.0), yn(0, 3.0), EPSILON);

	// yn(1, x) should equal y1(x)
	ASSERT_DBL_NEAR(y1(3.0), yn(1, 3.0), EPSILON);

	// yn(2, 3.0) reference value
	ASSERT_DBL_NEAR(-0.160400, yn(2, 3.0), 0.001);

	// yn(3, 5.0) reference value
	ASSERT_DBL_NEAR(0.14627, yn(3, 5.0), 0.001);
}

/* ============================================================= */
/* Edge Case Tests                                               */
/* ============================================================= */
TEST_SUITE(edge_cases);

TEST(edge_overflow_multiplication) {
	ASSERT_INF(DBL_MAX * 2.0);
	ASSERT_INF(DBL_MAX * DBL_MAX);
}

TEST(edge_underflow_multiplication) {
	ASSERT_DBL_NEAR(0.0, DBL_MIN / 1e308, EPSILON);
}

TEST(edge_very_large_numbers) {
	double large = 1e308;
	ASSERT_FINITE(large);
	ASSERT_DBL_GT(large, 0);
	ASSERT_DBL_LT(large, INFINITY);
}

TEST(edge_very_small_numbers) {
	double small = 1e-308;
	ASSERT_FINITE(small);
	ASSERT_DBL_GT(small, 0);
}

TEST(edge_precision_loss) {
	double x = 1.0 + 1e-16;
	double y = 1.0 + DBL_EPSILON;
	ASSERT_DBL_EQ(x, 1.0);
	ASSERT_DBL_GT(y, 1.0);
}

TEST(edge_domain_boundaries) {
	ASSERT_FINITE(asin(1.0));
	ASSERT_FINITE(asin(-1.0));
	ASSERT_FINITE(acos(1.0));
	ASSERT_FINITE(acos(-1.0));
	ASSERT_FINITE(acosh(1.0));
	ASSERT_FINITE(atanh(0.9999));
}

#if JACL_HAS_C99
TEST(edge_mixed_precision) {
	float f = 1.5f;
	double d = sqrt(f);
	ASSERT_DBL_NEAR(1.22474, d, EPSILON);

	double d2 = 4.0;
	long double ld = sqrtl(d2);
	ASSERT_LDBL_NEAR(2.0L, ld, EPSILON);
}
#endif

TEST(edge_denormal_arithmetic) {
	double tiny = DBL_TRUE_MIN;
	double result = tiny + tiny;
	ASSERT_DBL_NEAR(tiny * 2.0, result, 0.0);

	double large = 1.0 / tiny;
	ASSERT_INF(large);
}

TEST_MAIN()
