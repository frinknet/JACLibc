/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <math.h>
#include <float.h>

TEST_TYPE(unit);
TEST_UNIT(math.h);

#define EPSILON 0.0001
#define TIGHT_EPSILON 0.00001

/**************************************
 * isnan
 **************************************/
TEST_SUITE(isnan);

TEST(isnan_nan) {
	ASSERT_TRUE(isnan(NAN));
}

TEST(isnan_div_zero) {
	ASSERT_TRUE(isnan(0.0 / 0.0));
}

TEST(isnan_normal) {
	ASSERT_FALSE(isnan(1.0));
}

TEST(isnan_zero) {
	ASSERT_FALSE(isnan(0.0));
}

TEST(isnan_dbl_inf) {
	ASSERT_FALSE(isnan(HUGE_VAL));
}

TEST(isnan_flt_inf) {
	ASSERT_FALSE(isnan(HUGE_VALF));
}

TEST(isnan_ldbl_inf) {
	ASSERT_FALSE(isnan(HUGE_VALL));
}

/**************************************
 * isinf
 **************************************/
TEST_SUITE(isinf);

TEST(isinf_dbl) {
	ASSERT_TRUE(isinf(HUGE_VAL));
}

TEST(isinf_flt) {
	ASSERT_TRUE(isinf(HUGE_VALF));
}

TEST(isinf_ldbl) {
	ASSERT_TRUE(isinf(HUGE_VALL));
}

TEST(isinf_dbl_neginf) {
	ASSERT_TRUE(isinf(-HUGE_VAL));
}

TEST(isinf_flt_nginf) {
	ASSERT_TRUE(isinf(-HUGE_VALF));
}

TEST(isinf_ldbl_neginf) {
	ASSERT_TRUE(isinf(-HUGE_VALL));
}

TEST(isinf_div_zero) {
	ASSERT_TRUE(isinf(1.0 / 0.0));
}

TEST(isinf_normal) {
	ASSERT_FALSE(isinf(1.0));
}

TEST(isinf_nan) {
	ASSERT_FALSE(isinf(NAN));
}

/**************************************
 * isfinite
 **************************************/
TEST_SUITE(isfinite);

TEST(isfinite_normal) {
	ASSERT_TRUE(isfinite(1.0));
}

TEST(isfinite_zero) {
	ASSERT_TRUE(isfinite(0.0));
}

TEST(isfinite_max) {
	ASSERT_TRUE(isfinite(DBL_MAX));
}

TEST(isfinite_dbl_inf) {
	ASSERT_FALSE(isfinite(HUGE_VAL));
}

TEST(isfinite_flt_inf) {
	ASSERT_FALSE(isfinite(HUGE_VALF));
}

TEST(isfinite_ldbl_inf) {
	ASSERT_FALSE(isfinite(HUGE_VALL));
}

TEST(isfinite_nan) {
	ASSERT_FALSE(isfinite(NAN));
}

/**************************************
 * fpclassify
 **************************************/
TEST_SUITE(fpclassify);

TEST(fpclassify_zero) {
	ASSERT_EQ(FP_ZERO, fpclassify(0.0));
}

TEST(fpclassify_negzero) {
	ASSERT_EQ(FP_ZERO, fpclassify(-0.0));
}

TEST(fpclassify_normal) {
	ASSERT_EQ(FP_NORMAL, fpclassify(1.0));
}

TEST(fpclassify_inf) {
	ASSERT_EQ(FP_INFINITE, fpclassify(HUGE_VALF));
}

TEST(fpclassify_nan) {
	ASSERT_EQ(FP_NAN, fpclassify(NAN));
}

TEST(fpclassify_subnormal) {
	ASSERT_EQ(FP_SUBNORMAL, fpclassify(DBL_TRUE_MIN));
}

/**************************************
 * signbit
 **************************************/
TEST_SUITE(signbit);

TEST(signbit_pos) {
	ASSERT_EQ(0, signbit(1.0));
}

TEST(signbit_neg) {
	ASSERT_NE(0, signbit(-1.0));
}

TEST(signbit_poszero) {
	ASSERT_EQ(0, signbit(0.0));
}

TEST(signbit_negzero) {
	ASSERT_NE(0, signbit(-0.0));
}

TEST(signbit_posinf) {
	ASSERT_EQ(0, signbit(HUGE_VALL));
}

TEST(signbit_neginf) {
	ASSERT_NE(0, signbit(-HUGE_VALL));
}

/**************************************
 * isnormal
 **************************************/
TEST_SUITE(isnormal);

TEST(isnormal_normal) {
	ASSERT_TRUE(isnormal(1.0));
}

TEST(isnormal_min) {
	ASSERT_TRUE(isnormal(DBL_MIN));
}

TEST(isnormal_zero) {
	ASSERT_FALSE(isnormal(0.0));
}

TEST(isnormal_subnormal) {
	ASSERT_FALSE(isnormal(DBL_TRUE_MIN));
}

TEST(isnormal_inf) {
	ASSERT_FALSE(isnormal(HUGE_VALF));
}

TEST(isnormal_nan) {
	ASSERT_FALSE(isnormal(NAN));
}

/**************************************
 * fabs
 **************************************/
TEST_SUITE(fabs);

TEST(fabs_dbl_pos) {
	ASSERT_DBL_NEAR(5.0, fabs(5.0), EPSILON);
}

TEST(fabs_dbl_neg) {
	ASSERT_DBL_NEAR(5.0, fabs(-5.0), EPSILON);
}

TEST(fabs_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, fabs(0.0), EPSILON);
}

TEST(fabs_dbl_negzero) {
	ASSERT_DBL_NEAR(0.0, fabs(-0.0), EPSILON);
}

TEST(fabs_dbl_inf) {
	ASSERT_INF(fabs(HUGE_VAL));
}

TEST(fabs_dbl_neginf) {
	ASSERT_INF(fabs(-HUGE_VAL));
}

TEST(fabs_dbl_nan) {
	ASSERT_NAN(fabs(NAN));
}

TEST(fabs_flt_pos) {
	ASSERT_FLT_NEAR(3.5f, fabsf(3.5f), EPSILON);
}

TEST(fabs_flt_neg) {
	ASSERT_FLT_NEAR(3.5f, fabsf(-3.5f), EPSILON);
}

TEST(fabs_flt_zero) {
	ASSERT_FLT_NEAR(0.0f, fabsf(0.0f), EPSILON);
}

TEST(fabs_flt_inf) {
	ASSERT_INF(fabsf(HUGE_VALF));
}

TEST(fabs_flt_nan) {
	ASSERT_NAN(fabsf(NAN));
}

#if JACL_HAS_C99
TEST(fabs_ldbl_pos) {
	ASSERT_LDBL_NEAR(7.25L, fabsl(7.25L), EPSILON);
}

TEST(fabs_ldbl_neg) {
	ASSERT_LDBL_NEAR(7.25L, fabsl(-7.25L), EPSILON);
}

TEST(fabs_ldbl_zero) {
	ASSERT_LDBL_NEAR(0.0L, fabsl(0.0L), EPSILON);
}

TEST(fabs_ldbl_inf) {
	ASSERT_INF(fabsl(HUGE_VALL));
}

TEST(fabs_ldbl_nan) {
	ASSERT_NAN(fabsl(NAN));
}
#endif

/**************************************
 * ceil
 **************************************/
TEST_SUITE(ceil);

TEST(ceil_dbl_pos) {
	ASSERT_DBL_NEAR(3.0, ceil(2.1), EPSILON);
}

TEST(ceil_dbl_exact) {
	ASSERT_DBL_NEAR(3.0, ceil(3.0), EPSILON);
}

TEST(ceil_dbl_neg) {
	ASSERT_DBL_NEAR(-2.0, ceil(-2.9), EPSILON);
}

TEST(ceil_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, ceil(0.0), EPSILON);
}

TEST(ceil_dbl_inf) {
	ASSERT_INF(ceil(HUGE_VAL));
}

TEST(ceil_dbl_nan) {
	ASSERT_NAN(ceil(NAN));
}

TEST(ceil_flt_pos) {
	ASSERT_FLT_NEAR(3.0f, ceilf(2.1f), EPSILON);
}

TEST(ceil_flt_neg) {
	ASSERT_FLT_NEAR(-2.0f, ceilf(-2.5f), EPSILON);
}

TEST(ceil_flt_inf) {
	ASSERT_INF(ceilf(HUGE_VALF));
}

TEST(ceil_flt_nan) {
	ASSERT_NAN(ceilf(NAN));
}

#if JACL_HAS_C99
TEST(ceil_ldbl_pos) {
	ASSERT_LDBL_NEAR(3.0L, ceill(2.1L), EPSILON);
}

TEST(ceil_ldbl_neg) {
	ASSERT_LDBL_NEAR(-2.0L, ceill(-2.9L), EPSILON);
}

TEST(ceil_ldbl_inf) {
	ASSERT_INF(ceill(HUGE_VALL));
}

TEST(ceil_ldbl_nan) {
	ASSERT_NAN(ceill(NAN));
}
#endif

/**************************************
 * floor
 **************************************/
TEST_SUITE(floor);

TEST(floor_dbl_pos) {
	ASSERT_DBL_NEAR(2.0, floor(2.9), EPSILON);
}

TEST(floor_dbl_exact) {
	ASSERT_DBL_NEAR(3.0, floor(3.0), EPSILON);
}

TEST(floor_dbl_neg) {
	ASSERT_DBL_NEAR(-3.0, floor(-2.1), EPSILON);
}

TEST(floor_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, floor(0.0), EPSILON);
}

TEST(floor_dbl_inf) {
	ASSERT_INF(floor(HUGE_VAL));
}

TEST(floor_dbl_nan) {
	ASSERT_NAN(floor(NAN));
}

TEST(floor_flt_pos) {
	ASSERT_FLT_NEAR(2.0f, floorf(2.9f), EPSILON);
}

TEST(floor_flt_neg) {
	ASSERT_FLT_NEAR(-3.0f, floorf(-2.5f), EPSILON);
}

TEST(floor_flt_inf) {
	ASSERT_INF(floorf(HUGE_VALF));
}

TEST(floor_flt_nan) {
	ASSERT_NAN(floorf(NAN));
}

#if JACL_HAS_C99
TEST(floor_ldbl_pos) {
	ASSERT_LDBL_NEAR(2.0L, floorl(2.9L), EPSILON);
}

TEST(floor_ldbl_neg) {
	ASSERT_LDBL_NEAR(-3.0L, floorl(-2.1L), EPSILON);
}

TEST(floor_ldbl_inf) {
	ASSERT_INF(floorl(HUGE_VALL));
}

TEST(floor_ldbl_nan) {
	ASSERT_NAN(floorl(NAN));
}
#endif

/**************************************
 * trunc
 **************************************/
TEST_SUITE(trunc);

TEST(trunc_dbl_pos) {
	ASSERT_DBL_NEAR(2.0, trunc(2.7), EPSILON);
}

TEST(trunc_dbl_neg) {
	ASSERT_DBL_NEAR(-2.0, trunc(-2.7), EPSILON);
}

TEST(trunc_dbl_half) {
	ASSERT_DBL_NEAR(0.0, trunc(0.5), EPSILON);
}

TEST(trunc_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, trunc(0.0), EPSILON);
}

TEST(trunc_dbl_inf) {
	ASSERT_INF(trunc(HUGE_VAL));
}

TEST(trunc_dbl_nan) {
	ASSERT_NAN(trunc(NAN));
}

TEST(trunc_flt_pos) {
	ASSERT_FLT_NEAR(2.0f, truncf(2.7f), EPSILON);
}

TEST(trunc_flt_neg) {
	ASSERT_FLT_NEAR(-2.0f, truncf(-2.7f), EPSILON);
}

TEST(trunc_flt_inf) {
	ASSERT_INF(truncf(HUGE_VALF));
}

TEST(trunc_flt_nan) {
	ASSERT_NAN(truncf(NAN));
}

#if JACL_HAS_C99
TEST(trunc_ldbl_pos) {
	ASSERT_LDBL_NEAR(2.0L, truncl(2.7L), EPSILON);
}

TEST(trunc_ldbl_neg) {
	ASSERT_LDBL_NEAR(-2.0L, truncl(-2.7L), EPSILON);
}

TEST(trunc_ldbl_inf) {
	ASSERT_INF(truncl(HUGE_VALL));
}

TEST(trunc_ldbl_nan) {
	ASSERT_NAN(truncl(NAN));
}
#endif

/**************************************
 * round
 **************************************/
TEST_SUITE(round);

TEST(round_dbl_half_pos) {
	ASSERT_DBL_NEAR(3.0, round(2.5), EPSILON);
}

TEST(round_dbl_half_neg) {
	ASSERT_DBL_NEAR(-3.0, round(-2.5), EPSILON);
}

TEST(round_dbl_below) {
	ASSERT_DBL_NEAR(2.0, round(2.4), EPSILON);
}

TEST(round_dbl_above) {
	ASSERT_DBL_NEAR(3.0, round(2.6), EPSILON);
}

TEST(round_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, round(0.0), EPSILON);
}

TEST(round_dbl_inf) {
	ASSERT_INF(round(HUGE_VAL));
}

TEST(round_dbl_nan) {
	ASSERT_NAN(round(NAN));
}

TEST(round_flt_half) {
	ASSERT_FLT_NEAR(3.0f, roundf(2.5f), EPSILON);
}

TEST(round_flt_inf) {
	ASSERT_INF(roundf(HUGE_VALF));
}

TEST(round_flt_nan) {
	ASSERT_NAN(roundf(NAN));
}

#if JACL_HAS_C99
TEST(round_ldbl_half) {
	ASSERT_LDBL_NEAR(3.0L, roundl(2.5L), EPSILON);
}

TEST(round_ldbl_inf) {
	ASSERT_INF(roundl(HUGE_VALL));
}

TEST(round_ldbl_nan) {
	ASSERT_NAN(roundl(NAN));
}
#endif

/**************************************
 * rint
 **************************************/
TEST_SUITE(rint);

TEST(rint_dbl_half_even) {
	ASSERT_DBL_NEAR(2.0, rint(2.5), EPSILON);
	ASSERT_DBL_NEAR(4.0, rint(3.5), EPSILON);
}

TEST(rint_dbl_below) {
	ASSERT_DBL_NEAR(2.0, rint(2.4), EPSILON);
}

TEST(rint_dbl_above) {
	ASSERT_DBL_NEAR(3.0, rint(2.6), EPSILON);
}

TEST(rint_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, rint(0.0), EPSILON);
}

TEST(rint_dbl_inf) {
	ASSERT_INF(rint(HUGE_VAL));
}

TEST(rint_dbl_nan) {
	ASSERT_NAN(rint(NAN));
}

TEST(rint_flt_half_even) {
	ASSERT_FLT_NEAR(2.0f, rintf(2.5f), EPSILON);
}

TEST(rint_flt_inf) {
	ASSERT_INF(rintf(HUGE_VALF));
}

TEST(rint_flt_nan) {
	ASSERT_NAN(rintf(NAN));
}

#if JACL_HAS_C99
TEST(rint_ldbl_half_even) {
	ASSERT_LDBL_NEAR(2.0L, rintl(2.5L), EPSILON);
}

TEST(rint_ldbl_inf) {
	ASSERT_INF(rintl(HUGE_VALL));
}

TEST(rint_ldbl_nan) {
	ASSERT_NAN(rintl(NAN));
}
#endif

/**************************************
 * nearbyint
 **************************************/
TEST_SUITE(nearbyint);

TEST(nearbyint_dbl_half_even) {
	ASSERT_DBL_NEAR(2.0, nearbyint(2.5), EPSILON);
	ASSERT_DBL_NEAR(4.0, nearbyint(3.5), EPSILON);
}

TEST(nearbyint_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, nearbyint(0.0), EPSILON);
}

TEST(nearbyint_dbl_inf) {
	ASSERT_INF(nearbyint(HUGE_VAL));
}

TEST(nearbyint_dbl_nan) {
	ASSERT_NAN(nearbyint(NAN));
}

TEST(nearbyint_flt_half_even) {
	ASSERT_FLT_NEAR(2.0f, nearbyintf(2.5f), EPSILON);
}

TEST(nearbyint_flt_inf) {
	ASSERT_INF(nearbyintf(HUGE_VALF));
}

TEST(nearbyint_flt_nan) {
	ASSERT_NAN(nearbyintf(NAN));
}

#if JACL_HAS_C99
TEST(nearbyint_ldbl_half_even) {
	ASSERT_LDBL_NEAR(2.0L, nearbyintl(2.5L), EPSILON);
}

TEST(nearbyint_ldbl_inf) {
	ASSERT_INF(nearbyintl(HUGE_VALL));
}

TEST(nearbyint_ldbl_nan) {
	ASSERT_NAN(nearbyintl(NAN));
}
#endif

/**************************************
 * lround
 **************************************/
TEST_SUITE(lround);

TEST(lround_dbl_half) {
	ASSERT_EQ(3L, lround(2.5));
}

TEST(lround_dbl_neg) {
	ASSERT_EQ(-3L, lround(-2.5));
}

TEST(lround_dbl_zero) {
	ASSERT_EQ(0L, lround(0.0));
}

TEST(lround_flt_half) {
	ASSERT_EQ(3L, lroundf(2.5f));
}

TEST(lround_flt_neg) {
	ASSERT_EQ(-3L, lroundf(-2.5f));
}

#if JACL_HAS_C99
TEST(lround_ldbl_half) {
	ASSERT_EQ(3L, lroundl(2.5L));
}

TEST(lround_ldbl_neg) {
	ASSERT_EQ(-3L, lroundl(-2.5L));
}
#endif

/**************************************
 * llround
 **************************************/
TEST_SUITE(llround);

TEST(llround_dbl_half) {
	ASSERT_EQ(3LL, llround(2.5));
}

TEST(llround_dbl_neg) {
	ASSERT_EQ(-3LL, llround(-2.5));
}

TEST(llround_flt_half) {
	ASSERT_EQ(3LL, llroundf(2.5f));
}

#if JACL_HAS_C99
TEST(llround_ldbl_half) {
	ASSERT_EQ(3LL, llroundl(2.5L));
}
#endif

/**************************************
 * lrint
 **************************************/
TEST_SUITE(lrint);

TEST(lrint_dbl_exact) {
	ASSERT_EQ(3L, lrint(3.0));
}

TEST(lrint_dbl_neg) {
	ASSERT_EQ(-5L, lrint(-5.0));
}

TEST(lrint_flt_exact) {
	ASSERT_EQ(3L, lrintf(3.0f));
}

#if JACL_HAS_C99
TEST(lrint_ldbl_exact) {
	ASSERT_EQ(3L, lrintl(3.0L));
}
#endif

/**************************************
 * llrint
 **************************************/
TEST_SUITE(llrint);

TEST(llrint_dbl_exact) {
	ASSERT_EQ(100LL, llrint(100.0));
}

TEST(llrint_flt_exact) {
	ASSERT_EQ(100LL, llrintf(100.0f));
}

#if JACL_HAS_C99
TEST(llrint_ldbl_exact) {
	ASSERT_EQ(100LL, llrintl(100.0L));
}
#endif

/**************************************
 * exp
 **************************************/
TEST_SUITE(exp);

TEST(exp_dbl_zero) {
	ASSERT_DBL_NEAR(1.0, exp(0.0), EPSILON);
}

TEST(exp_dbl_one) {
	ASSERT_DBL_NEAR(2.71828, exp(1.0), EPSILON);
}

TEST(exp_dbl_neg) {
	ASSERT_DBL_NEAR(0.36788, exp(-1.0), EPSILON);
}

TEST(exp_dbl_overflow) {
	ASSERT_INF(exp(DBL_LOG_MAX));
}

TEST(exp_dbl_underflow) {
	ASSERT_DBL_NEAR(0.0, exp(DBL_LOG_TRUE_MIN), EPSILON);
}

TEST(exp_dbl_threshold) {
	double result = exp(DBL_LOG_TRUE_MIN);
	ASSERT_DBL_GE(result, 0.0);
	ASSERT_DBL_LE(result, DBL_TRUE_MIN * 2.0);
}

TEST(exp_dbl_inf) {
	ASSERT_INF(exp(HUGE_VAL));
}

TEST(exp_dbl_nan) {
	ASSERT_NAN(exp(NAN));
}

TEST(exp_flt_zero) {
	ASSERT_FLT_NEAR(1.0f, expf(0.0f), EPSILON);
}

TEST(exp_flt_one) {
	ASSERT_FLT_NEAR(2.71828f, expf(1.0f), EPSILON);
}

TEST(exp_flt_overflow) {
	ASSERT_INF(expf(FLT_LOG_MAX));
}

TEST(exp_flt_underflow) {
	ASSERT_FLT_NEAR(0.0, expf(FLT_LOG_TRUE_MIN), EPSILON);
}

TEST(exp_flt_threshold) {
	double result = expf(FLT_LOG_TRUE_MIN);
	ASSERT_FLT_GE(result, 0.0);
	ASSERT_FLT_LE(result, FLT_TRUE_MIN * 2.0);
}

TEST(exp_flt_nan) {
	ASSERT_NAN(expf(NAN));
}

#if JACL_HAS_C99
TEST(exp_ldbl_zero) {
	ASSERT_LDBL_NEAR(1.0L, expl(0.0L), EPSILON);
}

TEST(exp_ldbl_one) {
	ASSERT_LDBL_NEAR(2.71828L, expl(1.0L), EPSILON);
}

TEST(exp_ldbl_overflow) {
	ASSERT_INF(expl(LDBL_LOG_MAX));
}

TEST(exp_ldbl_underflow) {
	ASSERT_LDBL_NEAR(0.0, expl(LDBL_LOG_TRUE_MIN), EPSILON);
}

TEST(exp_ldbl_threshold) {
	double result = expl(LDBL_LOG_TRUE_MIN);
	ASSERT_LDBL_GE(result, 0.0);
	ASSERT_LDBL_LE(result, LDBL_TRUE_MIN * 2.0);
}

TEST(exp_ldbl_nan) {
	ASSERT_NAN(expl(NAN));
}
#endif

/**************************************
 * exp2
 **************************************/
TEST_SUITE(exp2);

TEST(exp2_dbl_zero) {
	ASSERT_DBL_NEAR(1.0, exp2(0.0), EPSILON);
}

TEST(exp2_dbl_one) {
	ASSERT_DBL_NEAR(2.0, exp2(1.0), EPSILON);
}

TEST(exp2_dbl_three) {
	ASSERT_DBL_NEAR(8.0, exp2(3.0), EPSILON);
}

TEST(exp2_dbl_neg) {
	ASSERT_DBL_NEAR(0.5, exp2(-1.0), EPSILON);
}

TEST(exp2_dbl_overflow) {
	ASSERT_INF(exp2(DBL_MAX_EXP));
}

TEST(exp2_dbl_underflow) {
	ASSERT_DBL_NEAR(0.0, exp2(DBL_MIN_EXP - DBL_MANT_DIG - 1), 0.0);
}

TEST(exp2_dbl_threshold) {
	double result = exp2(DBL_MIN_EXP - DBL_MANT_DIG);
	ASSERT_DBL_GT(result, 0.0);
	ASSERT_DBL_LE(result, DBL_TRUE_MIN * 2.0);
}

TEST(exp2_dbl_nan) {
	ASSERT_NAN(exp2(NAN));
}

TEST(exp2_flt_zero) {
	ASSERT_FLT_NEAR(1.0f, exp2f(0.0f), EPSILON);
}

TEST(exp2_flt_one) {
	ASSERT_FLT_NEAR(2.0f, exp2f(1.0f), EPSILON);
}

TEST(exp2_flt_overflow) {
	ASSERT_INF(exp2f(FLT_MAX_EXP));
}

TEST(exp2_flt_underflow) {
	ASSERT_FLT_NEAR(0.0f, exp2f(FLT_MIN_EXP - FLT_MANT_DIG - 1), 0.0);
}

TEST(exp2_flt_threshold) {
	float result = exp2f(FLT_MIN_EXP - FLT_MANT_DIG);
	ASSERT_FLT_GT(result, 0.0f);
	ASSERT_FLT_LE(result, FLT_TRUE_MIN * 2.0f);
}

TEST(exp2_flt_nan) {
	ASSERT_NAN(exp2f(NAN));
}

#if JACL_HAS_C99
TEST(exp2_ldbl_zero) {
	ASSERT_LDBL_NEAR(1.0L, exp2l(0.0L), EPSILON);
}

TEST(exp2_ldbl_one) {
	ASSERT_LDBL_NEAR(2.0L, exp2l(1.0L), EPSILON);
}

TEST(exp2_ldbl_overflow) {
	ASSERT_INF(exp2l(LDBL_MAX_EXP));
}

TEST(exp2_ldbl_underflow) {
	ASSERT_LDBL_NEAR(0.0L, exp2l(LDBL_MIN_EXP - LDBL_MANT_DIG - 1), 0.0);
}

TEST(exp2_ldbl_threshold) {
	long double result = exp2l(LDBL_MIN_EXP - LDBL_MANT_DIG);
	ASSERT_LDBL_GT(result, 0.0L);
	ASSERT_LDBL_LE(result, LDBL_TRUE_MIN * 2.0L);
}

TEST(exp2_ldbl_nan) {
	ASSERT_NAN(exp2l(NAN));
}
#endif

/**************************************
 * expm1
 **************************************/
TEST_SUITE(expm1);

TEST(expm1_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, expm1(0.0), EPSILON);
}

TEST(expm1_dbl_one) {
	ASSERT_DBL_NEAR(1.71828, expm1(1.0), EPSILON);
}

TEST(expm1_dbl_small) {
	ASSERT_DBL_NEAR(0.01005, expm1(0.01), TIGHT_EPSILON);
}

TEST(expm1_dbl_nan) {
	ASSERT_NAN(expm1(NAN));
}

TEST(expm1_flt_zero) {
	ASSERT_FLT_NEAR(0.0f, expm1f(0.0f), EPSILON);
}

TEST(expm1_flt_one) {
	ASSERT_FLT_NEAR(1.71828f, expm1f(1.0f), EPSILON);
}

TEST(expm1_flt_nan) {
	ASSERT_NAN(expm1f(NAN));
}

#if JACL_HAS_C99
TEST(expm1_ldbl_zero) {
	ASSERT_LDBL_NEAR(0.0L, expm1l(0.0L), EPSILON);
}

TEST(expm1_ldbl_one) {
	ASSERT_LDBL_NEAR(1.71828L, expm1l(1.0L), EPSILON);
}

TEST(expm1_ldbl_nan) {
	ASSERT_NAN(expm1l(NAN));
}
#endif

/**************************************
 * log
 **************************************/
TEST_SUITE(log);

TEST(log_dbl_one) {
	ASSERT_DBL_NEAR(0.0, log(1.0), EPSILON);
}

TEST(log_dbl_e) {
	ASSERT_DBL_NEAR(1.0, log(M_E), EPSILON);
}

TEST(log_dbl_ten) {
	ASSERT_DBL_NEAR(2.30259, log(10.0), EPSILON);
}

TEST(log_dbl_neg) {
	ASSERT_NAN(log(-1.0));
}

TEST(log_dbl_zero) {
	ASSERT_INF(log(0.0));
}

TEST(log_dbl_inf) {
	ASSERT_INF(log(HUGE_VAL));
}

TEST(log_dbl_nan) {
	ASSERT_NAN(log(NAN));
}

TEST(log_dbl_max) {
	ASSERT_DBL_NEAR(DBL_LOG_MAX, log(DBL_MAX), 1.0);
}

TEST(log_dbl_true_min) {
	ASSERT_DBL_NEAR(DBL_LOG_TRUE_MIN, log(DBL_TRUE_MIN), 1.0);
}

TEST(log_flt_one) {
	ASSERT_FLT_NEAR(0.0f, logf(1.0f), EPSILON);
}

TEST(log_flt_neg) {
	ASSERT_NAN(logf(-1.0f));
}

TEST(log_flt_zero) {
	ASSERT_INF(logf(0.0f));
}

TEST(log_flt_nan) {
	ASSERT_NAN(logf(NAN));
}

TEST(log_flt_max) {
	ASSERT_FLT_NEAR(FLT_LOG_MAX, logf(FLT_MAX), 1.0);
}

TEST(log_flt_true_min) {
	ASSERT_FLT_NEAR(FLT_LOG_TRUE_MIN, logf(FLT_TRUE_MIN), 1.0);
}

#if JACL_HAS_C99
TEST(log_ldbl_one) {
	ASSERT_LDBL_NEAR(0.0L, logl(1.0L), EPSILON);
}

TEST(log_ldbl_neg) {
	ASSERT_NAN(logl(-1.0L));
}

TEST(log_ldbl_zero) {
	ASSERT_INF(logl(0.0L));
}

TEST(log_ldbl_nan) {
	ASSERT_NAN(logl(NAN));
}

TEST(log_ldbl_max) {
	ASSERT_LDBL_NEAR(LDBL_LOG_MAX, logl(LDBL_MAX), 1.0);
}

TEST(log_ldbl_true_min) {
	ASSERT_LDBL_NEAR(LDBL_LOG_TRUE_MIN, logl(LDBL_TRUE_MIN), 1.0);
}
#endif

/**************************************
 * log2
 **************************************/
TEST_SUITE(log2);

TEST(log2_dbl_one) {
	ASSERT_DBL_NEAR(0.0, log2(1.0), EPSILON);
}

TEST(log2_dbl_two) {
	ASSERT_DBL_NEAR(1.0, log2(2.0), EPSILON);
}

TEST(log2_dbl_eight) {
	ASSERT_DBL_NEAR(3.0, log2(8.0), EPSILON);
}

TEST(log2_dbl_half) {
	ASSERT_DBL_NEAR(-1.0, log2(0.5), EPSILON);
}

TEST(log2_dbl_neg) {
	ASSERT_NAN(log2(-1.0));
}

TEST(log2_dbl_zero) {
	ASSERT_INF(log2(0.0));
}

TEST(log2_dbl_nan) {
	ASSERT_NAN(log2(NAN));
}

TEST(log2_dbl_max) {
	ASSERT_DBL_NEAR((double)DBL_MAX_EXP, log2(DBL_MAX), 1.0);
}

TEST(log2_flt_one) {
	ASSERT_FLT_NEAR(0.0f, log2f(1.0f), EPSILON);
}

TEST(log2_flt_two) {
	ASSERT_FLT_NEAR(1.0f, log2f(2.0f), EPSILON);
}

TEST(log2_flt_neg) {
	ASSERT_NAN(log2f(-1.0f));
}

TEST(log2_flt_nan) {
	ASSERT_NAN(log2f(NAN));
}

TEST(log2_flt_max) {
	ASSERT_FLT_NEAR((float)FLT_MAX_EXP, log2f(FLT_MAX), 1.0);
}

#if JACL_HAS_C99
TEST(log2_ldbl_one) {
	ASSERT_LDBL_NEAR(0.0L, log2l(1.0L), EPSILON);
}

TEST(log2_ldbl_two) {
	ASSERT_LDBL_NEAR(1.0L, log2l(2.0L), EPSILON);
}

TEST(log2_ldbl_neg) {
	ASSERT_NAN(log2l(-1.0L));
}

TEST(log2_ldbl_nan) {
	ASSERT_NAN(log2l(NAN));
}

TEST(log2_ldbl_max) {
	ASSERT_LDBL_NEAR((long double)LDBL_MAX_EXP, log2l(LDBL_MAX), 1.0);
}
#endif

/**************************************
 * log10
 **************************************/
TEST_SUITE(log10);

TEST(log10_dbl_one) {
	ASSERT_DBL_NEAR(0.0, log10(1.0), EPSILON);
}

TEST(log10_dbl_ten) {
	ASSERT_DBL_NEAR(1.0, log10(10.0), EPSILON);
}

TEST(log10_dbl_hundred) {
	ASSERT_DBL_NEAR(2.0, log10(100.0), EPSILON);
}

TEST(log10_dbl_neg) {
	ASSERT_NAN(log10(-1.0));
}

TEST(log10_dbl_zero) {
	ASSERT_INF(log10(0.0));
}

TEST(log10_dbl_nan) {
	ASSERT_NAN(log10(NAN));
}

TEST(log10_dbl_max) {
	ASSERT_DBL_NEAR((double)DBL_MAX_10_EXP, log10(DBL_MAX), 1.0);
}

TEST(log10_flt_one) {
	ASSERT_FLT_NEAR(0.0f, log10f(1.0f), EPSILON);
}

TEST(log10_flt_ten) {
	ASSERT_FLT_NEAR(1.0f, log10f(10.0f), EPSILON);
}

TEST(log10_flt_neg) {
	ASSERT_NAN(log10f(-1.0f));
}

TEST(log10_flt_nan) {
	ASSERT_NAN(log10f(NAN));
}

TEST(log10_flt_max) {
	ASSERT_FLT_NEAR((float)FLT_MAX_10_EXP, log10f(FLT_MAX), 1.0);
}

#if JACL_HAS_C99
TEST(log10_ldbl_one) {
	ASSERT_LDBL_NEAR(0.0L, log10l(1.0L), EPSILON);
}

TEST(log10_ldbl_ten) {
	ASSERT_LDBL_NEAR(1.0L, log10l(10.0L), EPSILON);
}

TEST(log10_ldbl_neg) {
	ASSERT_NAN(log10l(-1.0L));
}

TEST(log10_ldbl_nan) {
	ASSERT_NAN(log10l(NAN));
}

TEST(log10_ldbl_max) {
	ASSERT_LDBL_NEAR((long double)LDBL_MAX_10_EXP, log10l(LDBL_MAX), 1.0);
}
#endif

/**************************************
 * log1p
 **************************************/
TEST_SUITE(log1p);

TEST(log1p_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, log1p(0.0), EPSILON);
}

TEST(log1p_dbl_one) {
	ASSERT_DBL_NEAR(0.69315, log1p(1.0), EPSILON);
}

TEST(log1p_dbl_small) {
	ASSERT_DBL_NEAR(0.00995, log1p(0.01), TIGHT_EPSILON);
}

TEST(log1p_dbl_nan) {
	ASSERT_NAN(log1p(NAN));
}

TEST(log1p_flt_zero) {
	ASSERT_FLT_NEAR(0.0f, log1pf(0.0f), EPSILON);
}

TEST(log1p_flt_one) {
	ASSERT_FLT_NEAR(0.69315f, log1pf(1.0f), EPSILON);
}

TEST(log1p_flt_nan) {
	ASSERT_NAN(log1pf(NAN));
}

#if JACL_HAS_C99
TEST(log1p_ldbl_zero) {
	ASSERT_LDBL_NEAR(0.0L, log1pl(0.0L), EPSILON);
}

TEST(log1p_ldbl_one) {
	ASSERT_LDBL_NEAR(0.69315L, log1pl(1.0L), EPSILON);
}

TEST(log1p_ldbl_nan) {
	ASSERT_NAN(log1pl(NAN));
}
#endif

/**************************************
 * pow
 **************************************/
TEST_SUITE(pow);

TEST(pow_dbl_basic) {
	ASSERT_DBL_NEAR(8.0, pow(2.0, 3.0), EPSILON);
}

TEST(pow_dbl_zero_exp) {
	ASSERT_DBL_NEAR(1.0, pow(5.0, 0.0), EPSILON);
}

TEST(pow_dbl_neg_exp) {
	ASSERT_DBL_NEAR(0.25, pow(2.0, -2.0), EPSILON);
}

TEST(pow_dbl_frac) {
	ASSERT_DBL_NEAR(2.0, pow(4.0, 0.5), EPSILON);
}

TEST(pow_dbl_neg_frac) {
	ASSERT_NAN(pow(-1.0, 0.5));
}

TEST(pow_dbl_zero_zero) {
	ASSERT_DBL_NEAR(1.0, pow(0.0, 0.0), EPSILON);
}

TEST(pow_dbl_overflow) {
	ASSERT_INF(pow(10.0, DBL_MAX_10_EXP + 1));
}

TEST(pow_dbl_underflow) {
	ASSERT_DBL_NEAR(0.0, pow(10.0, DBL_MIN_10_EXP - 1), 0.0);
}

TEST(pow_dbl_nan_base) {
	ASSERT_NAN(pow(NAN, 2.0));
}

TEST(pow_dbl_nan_exp) {
	ASSERT_NAN(pow(2.0, NAN));
}

TEST(pow_flt_basic) {
	ASSERT_FLT_NEAR(8.0f, powf(2.0f, 3.0f), EPSILON);
}

TEST(pow_flt_zero_exp) {
	ASSERT_FLT_NEAR(1.0f, powf(5.0f, 0.0f), EPSILON);
}

TEST(pow_flt_overflow) {
	ASSERT_INF(powf(10.0f, FLT_MAX_10_EXP + 1));
}

TEST(pow_flt_underflow) {
	ASSERT_FLT_NEAR(0.0, powf(10.0, FLT_MIN_10_EXP - 1), 0.0);
}

TEST(pow_flt_nan) {
	ASSERT_NAN(powf(NAN, 2.0f));
}

#if JACL_HAS_C99
TEST(pow_ldbl_basic) {
	ASSERT_LDBL_NEAR(8.0L, powl(2.0L, 3.0L), EPSILON);
}

TEST(pow_ldbl_zero_exp) {
	ASSERT_LDBL_NEAR(1.0L, powl(5.0L, 0.0L), EPSILON);
}

TEST(pow_ldbl_overflow) {
	ASSERT_INF(powl(10.0L, LDBL_MAX_10_EXP + 1));
}

TEST(pow_ldbl_underflow) {
	ASSERT_LDBL_NEAR(0.0, powl(10.0, LDBL_MIN_10_EXP - 1), 0.0);
}

TEST(pow_ldbl_nan) {
	ASSERT_NAN(powl(NAN, 2.0L));
}
#endif

/**************************************
 * sqrt
 **************************************/
TEST_SUITE(sqrt);

TEST(sqrt_dbl_four) {
	ASSERT_DBL_NEAR(2.0, sqrt(4.0), EPSILON);
}

TEST(sqrt_dbl_nine) {
	ASSERT_DBL_NEAR(3.0, sqrt(9.0), EPSILON);
}

TEST(sqrt_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, sqrt(0.0), EPSILON);
}

TEST(sqrt_dbl_neg) {
	ASSERT_NAN(sqrt(-1.0));
}

TEST(sqrt_dbl_inf) {
	ASSERT_INF(sqrt(HUGE_VAL));
}

TEST(sqrt_dbl_nan) {
	ASSERT_NAN(sqrt(NAN));
}

TEST(sqrt_dbl_subnormal) {
	double result = sqrt(DBL_TRUE_MIN);
	ASSERT_DBL_GT(result, 0.0);
	ASSERT_TRUE(fpclassify(result) == FP_NORMAL || fpclassify(result) == FP_SUBNORMAL);
}

#if JACL_HAS_C99
TEST(sqrt_flt_four) {
	ASSERT_FLT_NEAR(2.0f, sqrtf(4.0f), EPSILON);
}

TEST(sqrt_flt_neg) {
	ASSERT_NAN(sqrtf(-1.0f));
}

TEST(sqrt_flt_inf) {
	ASSERT_INF(sqrtf(HUGE_VALF));
}

TEST(sqrt_flt_nan) {
	ASSERT_NAN(sqrtf((float)NAN));
}

TEST(sqrt_flt_subnormal) {
	float result = sqrtf(FLT_TRUE_MIN);
	ASSERT_FLT_GT(result, 0.0f);
	ASSERT_TRUE(fpclassify(result) == FP_NORMAL || fpclassify(result) == FP_SUBNORMAL);
}

TEST(sqrt_ldbl_four) {
	ASSERT_LDBL_NEAR(2.0L, sqrtl(4.0L), EPSILON);
}

TEST(sqrt_ldbl_neg) {
	ASSERT_NAN(sqrtl(-1.0L));
}

TEST(sqrt_ldbl_inf) {
	ASSERT_INF(sqrtl(HUGE_VALL));
}

TEST(sqrt_ldbl_nan) {
	ASSERT_NAN(sqrtl((long double)NAN));
}

TEST(sqrt_ldbl_subnormal) {
	long double result = sqrtl(LDBL_TRUE_MIN);
	ASSERT_LDBL_GT(result, 0.0L);
	ASSERT_TRUE(fpclassify(result) == FP_NORMAL || fpclassify(result) == FP_SUBNORMAL);
}
#endif

/**************************************
 * cbrt
 **************************************/
TEST_SUITE(cbrt);

TEST(cbrt_dbl_eight) {
	ASSERT_DBL_NEAR(2.0, cbrt(8.0), EPSILON);
}

TEST(cbrt_dbl_neg) {
	ASSERT_DBL_NEAR(-2.0, cbrt(-8.0), EPSILON);
}

TEST(cbrt_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, cbrt(0.0), EPSILON);
}

TEST(cbrt_dbl_inf) {
	ASSERT_INF(cbrt(HUGE_VAL));
}

TEST(cbrt_dbl_nan) {
	ASSERT_NAN(cbrt(NAN));
}

TEST(cbrt_dbl_subnormal) {
	double result = cbrt(DBL_TRUE_MIN);
	ASSERT_DBL_GT(result, 0.0);
	ASSERT_TRUE(fpclassify(result) == FP_NORMAL || fpclassify(result) == FP_SUBNORMAL);
}

TEST(cbrt_flt_eight) {
	ASSERT_FLT_NEAR(2.0f, cbrtf(8.0f), EPSILON);
}

TEST(cbrt_flt_neg) {
	ASSERT_FLT_NEAR(-2.0f, cbrtf(-8.0f), EPSILON);
}

TEST(cbrt_flt_inf) {
	ASSERT_INF(cbrtf(HUGE_VALF));
}

TEST(cbrt_flt_nan) {
	ASSERT_NAN(cbrtf((float)NAN));
}

TEST(cbrt_flt_subnormal) {
	float result = cbrtf(FLT_TRUE_MIN);
	ASSERT_FLT_GT(result, 0.0f);
	ASSERT_TRUE(fpclassify(result) == FP_NORMAL || fpclassify(result) == FP_SUBNORMAL);
}

#if JACL_HAS_C99
TEST(cbrt_ldbl_eight) {
	ASSERT_LDBL_NEAR(2.0L, cbrtl(8.0L), EPSILON);
}

TEST(cbrt_ldbl_neg) {
	ASSERT_LDBL_NEAR(-2.0L, cbrtl(-8.0L), EPSILON);
}

TEST(cbrt_ldbl_inf) {
	ASSERT_INF(cbrtl(HUGE_VALL));
}

TEST(cbrt_ldbl_nan) {
	ASSERT_NAN(cbrtl(NAN));
}

TEST(cbrt_ldbl_subnormal) {
	long double result = cbrtl(LDBL_TRUE_MIN);
	ASSERT_LDBL_GT(result, 0.0L);
	ASSERT_TRUE(fpclassify(result) == FP_NORMAL || fpclassify(result) == FP_SUBNORMAL);
}
#endif

/**************************************
 * hypot
 **************************************/
TEST_SUITE(hypot);

TEST(hypot_dbl_345) {
	ASSERT_DBL_NEAR(5.0, hypot(3.0, 4.0), EPSILON);
}

TEST(hypot_dbl_51213) {
	ASSERT_DBL_NEAR(13.0, hypot(5.0, 12.0), EPSILON);
}

TEST(hypot_dbl_zero) {
	ASSERT_DBL_NEAR(1.0, hypot(1.0, 0.0), EPSILON);
}

TEST(hypot_dbl_inf) {
	ASSERT_INF(hypot(HUGE_VAL, 1.0));
}

TEST(hypot_dbl_nan) {
	ASSERT_NAN(hypot(NAN, 1.0));
}

TEST(hypot_flt_345) {
	ASSERT_FLT_NEAR(5.0f, hypotf(3.0f, 4.0f), EPSILON);
}

TEST(hypot_flt_inf) {
	ASSERT_INF(hypotf(HUGE_VALF, 1.0f));
}

TEST(hypot_flt_nan) {
	ASSERT_NAN(hypotf(NAN, 1.0f));
}

#if JACL_HAS_C99
TEST(hypot_ldbl_345) {
	ASSERT_LDBL_NEAR(5.0L, hypotl(3.0L, 4.0L), EPSILON);
}

TEST(hypot_ldbl_inf) {
	ASSERT_INF(hypotl(HUGE_VALL, 1.0L));
}

TEST(hypot_ldbl_nan) {
	ASSERT_NAN(hypotl(NAN, 1.0L));
}
#endif

/**************************************
 * sin
 **************************************/
TEST_SUITE(sin);

TEST(sin_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, sin(0.0), EPSILON);
}

TEST(sin_dbl_pi_2) {
	ASSERT_DBL_NEAR(1.0, sin(M_PI_2), EPSILON);
}

TEST(sin_dbl_pi) {
	ASSERT_DBL_NEAR(0.0, sin(M_PI), EPSILON);
}

TEST(sin_dbl_neg) {
	ASSERT_DBL_NEAR(-1.0, sin(3.0 * M_PI_2), EPSILON);
}

TEST(sin_dbl_inf) {
	ASSERT_NAN(sin(HUGE_VALL));
}

TEST(sin_dbl_nan) {
	ASSERT_NAN(sin(NAN));
}

TEST(sin_flt_zero) {
	ASSERT_FLT_NEAR(0.0f, sinf(0.0f), EPSILON);
}

TEST(sin_flt_inf) {
	ASSERT_NAN(sinf(HUGE_VALF));
}

TEST(sin_flt_nan) {
	ASSERT_NAN(sinf(NAN));
}

#if JACL_HAS_C99
TEST(sin_ldbl_zero) {
	ASSERT_LDBL_NEAR(0.0L, sinl(0.0L), EPSILON);
}

TEST(sin_ldbl_inf) {
	ASSERT_NAN(sinl(HUGE_VALL));
}

TEST(sin_ldbl_nan) {
	ASSERT_NAN(sinl(NAN));
}
#endif

/**************************************
 * cos
 **************************************/
TEST_SUITE(cos);

TEST(cos_dbl_zero) {
	ASSERT_DBL_NEAR(1.0, cos(0.0), EPSILON);
}

TEST(cos_dbl_pi_2) {
	ASSERT_DBL_NEAR(0.0, cos(M_PI_2), EPSILON);
}

TEST(cos_dbl_pi) {
	ASSERT_DBL_NEAR(-1.0, cos(M_PI), EPSILON);
}

TEST(cos_dbl_inf) {
	ASSERT_NAN(cos(HUGE_VAL));
}

TEST(cos_dbl_nan) {
	ASSERT_NAN(cos(NAN));
}

TEST(cos_flt_zero) {
	ASSERT_FLT_NEAR(1.0f, cosf(0.0f), EPSILON);
}

TEST(cos_flt_inf) {
	ASSERT_NAN(cosf(HUGE_VALF));
}

TEST(cos_flt_nan) {
	ASSERT_NAN(cosf(NAN));
}

#if JACL_HAS_C99
TEST(cos_ldbl_zero) {
	ASSERT_LDBL_NEAR(1.0L, cosl(0.0L), EPSILON);
}

TEST(cos_ldbl_inf) {
	ASSERT_NAN(cosl(HUGE_VALL));
}

TEST(cos_ldbl_nan) {
	ASSERT_NAN(cosl(NAN));
}
#endif

/**************************************
 * tan
 **************************************/
TEST_SUITE(tan);

TEST(tan_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, tan(0.0), EPSILON);
}

TEST(tan_dbl_pi_4) {
	ASSERT_DBL_NEAR(1.0, tan(M_PI_4), EPSILON);
}

TEST(tan_dbl_pi) {
	ASSERT_DBL_NEAR(0.0, tan(M_PI), EPSILON);
}

TEST(tan_dbl_inf) {
	ASSERT_NAN(tan(HUGE_VAL));
}

TEST(tan_dbl_nan) {
	ASSERT_NAN(tan(NAN));
}

TEST(tan_flt_zero) {
	ASSERT_FLT_NEAR(0.0f, tanf(0.0f), EPSILON);
}

TEST(tan_flt_inf) {
	ASSERT_NAN(tanf(HUGE_VALF));
}

TEST(tan_flt_nan) {
	ASSERT_NAN(tanf(NAN));
}

#if JACL_HAS_C99
TEST(tan_ldbl_zero) {
	ASSERT_LDBL_NEAR(0.0L, tanl(0.0L), EPSILON);
}

TEST(tan_ldbl_inf) {
	ASSERT_NAN(tanl(HUGE_VALL));
}

TEST(tan_ldbl_nan) {
	ASSERT_NAN(tanl(NAN));
}
#endif

/**************************************
 * asin
 **************************************/
TEST_SUITE(asin);

TEST(asin_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, asin(0.0), EPSILON);
}

TEST(asin_dbl_one) {
	ASSERT_DBL_NEAR(M_PI_2, asin(1.0), EPSILON);
}

TEST(asin_dbl_negone) {
	ASSERT_DBL_NEAR(-M_PI_2, asin(-1.0), EPSILON);
}

TEST(asin_dbl_half) {
	ASSERT_DBL_NEAR(M_PI / 6.0, asin(0.5), EPSILON);
}

TEST(asin_dbl_domain) {
	ASSERT_NAN(asin(2.0));
}

TEST(asin_dbl_nan) {
	ASSERT_NAN(asin(NAN));
}

TEST(asin_flt_zero) {
	ASSERT_FLT_NEAR(0.0f, asinf(0.0f), EPSILON);
}

TEST(asin_flt_one) {
	ASSERT_FLT_NEAR(M_PI_2, asinf(1.0f), EPSILON);
}

TEST(asin_flt_domain) {
	ASSERT_NAN(asinf(2.0f));
}

TEST(asin_flt_nan) {
	ASSERT_NAN(asinf(NAN));
}

#if JACL_HAS_C99
TEST(asin_ldbl_zero) {
	ASSERT_LDBL_NEAR(0.0L, asinl(0.0L), EPSILON);
}

TEST(asin_ldbl_one) {
	ASSERT_LDBL_NEAR(M_PI_2, asinl(1.0L), EPSILON);
}

TEST(asin_ldbl_domain) {
	ASSERT_NAN(asinl(2.0L));
}

TEST(asin_ldbl_nan) {
	ASSERT_NAN(asinl(NAN));
}
#endif

/**************************************
 * acos
 **************************************/
TEST_SUITE(acos);

TEST(acos_dbl_zero) {
	ASSERT_DBL_NEAR(M_PI_2, acos(0.0), EPSILON);
}

TEST(acos_dbl_one) {
	ASSERT_DBL_NEAR(0.0, acos(1.0), EPSILON);
}

TEST(acos_dbl_negone) {
	ASSERT_DBL_NEAR(M_PI, acos(-1.0), EPSILON);
}

TEST(acos_dbl_domain) {
	ASSERT_NAN(acos(2.0));
}

TEST(acos_dbl_nan) {
	ASSERT_NAN(acos(NAN));
}

TEST(acos_flt_zero) {
	ASSERT_FLT_NEAR(M_PI_2, acosf(0.0f), EPSILON);
}

TEST(acos_flt_one) {
	ASSERT_FLT_NEAR(0.0f, acosf(1.0f), EPSILON);
}

TEST(acos_flt_domain) {
	ASSERT_NAN(acosf(2.0f));
}

TEST(acos_flt_nan) {
	ASSERT_NAN(acosf(NAN));
}

#if JACL_HAS_C99
TEST(acos_ldbl_zero) {
	ASSERT_LDBL_NEAR(M_PI_2, acosl(0.0L), EPSILON);
}

TEST(acos_ldbl_one) {
	ASSERT_LDBL_NEAR(0.0L, acosl(1.0L), EPSILON);
}

TEST(acos_ldbl_domain) {
	ASSERT_NAN(acosl(2.0L));
}

TEST(acos_ldbl_nan) {
	ASSERT_NAN(acosl(NAN));
}
#endif

/**************************************
 * atan
 **************************************/
TEST_SUITE(atan);

TEST(atan_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, atan(0.0), EPSILON);
}

TEST(atan_dbl_one) {
	ASSERT_DBL_NEAR(M_PI_4, atan(1.0), EPSILON);
}

TEST(atan_dbl_negone) {
	ASSERT_DBL_NEAR(-M_PI_4, atan(-1.0), EPSILON);
}

TEST(atan_dbl_inf) {
	ASSERT_DBL_NEAR(M_PI_2, atan(HUGE_VAL), EPSILON);
}

TEST(atan_dbl_nan) {
	ASSERT_NAN(atan(NAN));
}

TEST(atan_flt_zero) {
	ASSERT_FLT_NEAR(0.0f, atanf(0.0f), EPSILON);
}

TEST(atan_flt_one) {
	ASSERT_FLT_NEAR(M_PI_4, atanf(1.0f), EPSILON);
}

TEST(atan_flt_inf) {
	ASSERT_FLT_NEAR(M_PI_2, atanf(HUGE_VALF), EPSILON);
}

TEST(atan_flt_nan) {
	ASSERT_NAN(atanf(NAN));
}

#if JACL_HAS_C99
TEST(atan_ldbl_zero) {
	ASSERT_LDBL_NEAR(0.0L, atanl(0.0L), EPSILON);
}

TEST(atan_ldbl_one) {
	ASSERT_LDBL_NEAR(M_PI_4, atanl(1.0L), EPSILON);
}

TEST(atan_ldbl_inf) {
	ASSERT_LDBL_NEAR(M_PI_2, atanl(HUGE_VALL), EPSILON);
}

TEST(atan_ldbl_nan) {
	ASSERT_NAN(atanl(NAN));
}
#endif

/**************************************
 * atan2
 **************************************/
TEST_SUITE(atan2);

TEST(atan2_dbl_quadrants) {
	ASSERT_DBL_NEAR(M_PI_4, atan2(1.0, 1.0), EPSILON);
	ASSERT_DBL_NEAR(3.0 * M_PI_4, atan2(1.0, -1.0), EPSILON);
}

TEST(atan2_dbl_zero_x) {
	ASSERT_DBL_NEAR(M_PI_2, atan2(1.0, 0.0), EPSILON);
}

TEST(atan2_dbl_zero_y) {
	ASSERT_DBL_NEAR(0.0, atan2(0.0, 1.0), EPSILON);
}

TEST(atan2_dbl_both_zero) {
	ASSERT_DBL_NEAR(0.0, atan2(0.0, 0.0), EPSILON);
}

TEST(atan2_flt_quadrants) {
	ASSERT_FLT_NEAR(M_PI_4, atan2f(1.0f, 1.0f), EPSILON);
}

TEST(atan2_flt_zero_x) {
	ASSERT_FLT_NEAR(M_PI_2, atan2f(1.0f, 0.0f), EPSILON);
}

#if JACL_HAS_C99
TEST(atan2_ldbl_quadrants) {
	ASSERT_LDBL_NEAR(M_PI_4, atan2l(1.0L, 1.0L), EPSILON);
}

TEST(atan2_ldbl_zero_x) {
	ASSERT_LDBL_NEAR(M_PI_2, atan2l(1.0L, 0.0L), EPSILON);
}
#endif

/**************************************
 * sinh
 **************************************/
TEST_SUITE(sinh);

TEST(sinh_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, sinh(0.0), EPSILON);
}

TEST(sinh_dbl_one) {
	ASSERT_DBL_NEAR(1.17520, sinh(1.0), EPSILON);
}

TEST(sinh_dbl_negone) {
	ASSERT_DBL_NEAR(-1.17520, sinh(-1.0), EPSILON);
}

TEST(sinh_dbl_overflow) {
	ASSERT_INF(sinh(DBL_LOG_MAX + 1.0));
}

TEST(sinh_dbl_nan) {
	ASSERT_NAN(sinh(NAN));
}

TEST(sinh_flt_zero) {
	ASSERT_FLT_NEAR(0.0f, sinhf(0.0f), EPSILON);
}

TEST(sinh_flt_one) {
	ASSERT_FLT_NEAR(1.17520f, sinhf(1.0f), EPSILON);
}

TEST(sinh_flt_overflow) {
	ASSERT_INF(sinhf(FLT_LOG_MAX + 1.0));
}

TEST(sinh_flt_nan) {
	ASSERT_NAN(sinhf(NAN));
}

#if JACL_HAS_C99
TEST(sinh_ldbl_zero) {
	ASSERT_LDBL_NEAR(0.0L, sinhl(0.0L), EPSILON);
}

TEST(sinh_ldbl_one) {
	ASSERT_LDBL_NEAR(1.17520L, sinhl(1.0L), EPSILON);
}

TEST(sinh_ldbl_overflow) {
	ASSERT_INF(sinhl(LDBL_LOG_MAX + 1.0));
}

TEST(sinh_ldbl_nan) {
	ASSERT_NAN(sinhl(NAN));
}
#endif

/**************************************
 * cosh
 **************************************/
TEST_SUITE(cosh);

TEST(cosh_dbl_zero) {
	ASSERT_DBL_NEAR(1.0, cosh(0.0), EPSILON);
}

TEST(cosh_dbl_one) {
	ASSERT_DBL_NEAR(1.54308, cosh(1.0), EPSILON);
}

TEST(cosh_dbl_negone) {
	ASSERT_DBL_NEAR(1.54308, cosh(-1.0), EPSILON);
}

TEST(cosh_dbl_overflow) {
	ASSERT_INF(cosh(DBL_LOG_MAX + 1.0));
}

TEST(cosh_dbl_nan) {
	ASSERT_NAN(cosh(NAN));
}

TEST(cosh_flt_zero) {
	ASSERT_FLT_NEAR(1.0f, coshf(0.0f), EPSILON);
}

TEST(cosh_flt_one) {
	ASSERT_FLT_NEAR(1.54308f, coshf(1.0f), EPSILON);
}

TEST(cosh_flt_overflow) {
	ASSERT_INF(coshf(FLT_LOG_MAX + 1.0));
}

TEST(cosh_flt_nan) {
	ASSERT_NAN(coshf(NAN));
}

#if JACL_HAS_C99
TEST(cosh_ldbl_zero) {
	ASSERT_LDBL_NEAR(1.0L, coshl(0.0L), EPSILON);
}

TEST(cosh_ldbl_one) {
	ASSERT_LDBL_NEAR(1.54308L, coshl(1.0L), EPSILON);
}

TEST(cosh_ldbl_overflow) {
	ASSERT_INF(coshl(LDBL_LOG_MAX + 1.0));
}

TEST(cosh_ldbl_nan) {
	ASSERT_NAN(coshl(NAN));
}
#endif

/**************************************
 * tanh
 **************************************/
TEST_SUITE(tanh);

TEST(tanh_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, tanh(0.0), EPSILON);
}

TEST(tanh_dbl_one) {
	ASSERT_DBL_NEAR(0.76159, tanh(1.0), EPSILON);
}

TEST(tanh_dbl_negone) {
	ASSERT_DBL_NEAR(-0.76159, tanh(-1.0), EPSILON);
}

TEST(tanh_dbl_large) {
	ASSERT_DBL_NEAR(1.0, tanh(100.0), EPSILON);
}

TEST(tanh_dbl_nan) {
	ASSERT_NAN(tanh(NAN));
}

TEST(tanh_flt_zero) {
	ASSERT_FLT_NEAR(0.0f, tanhf(0.0f), EPSILON);
}

TEST(tanh_flt_one) {
	ASSERT_FLT_NEAR(0.76159f, tanhf(1.0f), EPSILON);
}

TEST(tanh_flt_large) {
	ASSERT_FLT_NEAR(1.0f, tanhf(100.0f), EPSILON);
}

TEST(tanh_flt_nan) {
	ASSERT_NAN(tanhf(NAN));
}

#if JACL_HAS_C99
TEST(tanh_ldbl_zero) {
	ASSERT_LDBL_NEAR(0.0L, tanhl(0.0L), EPSILON);
}

TEST(tanh_ldbl_one) {
	ASSERT_LDBL_NEAR(0.76159L, tanhl(1.0L), EPSILON);
}

TEST(tanh_ldbl_large) {
	ASSERT_LDBL_NEAR(1.0L, tanhl(100.0L), EPSILON);
}

TEST(tanh_ldbl_nan) {
	ASSERT_NAN(tanhl(NAN));
}
#endif

/**************************************
 * asinh
 **************************************/
TEST_SUITE(asinh);

TEST(asinh_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, asinh(0.0), EPSILON);
}

TEST(asinh_dbl_one) {
	ASSERT_DBL_NEAR(0.88137, asinh(1.0), EPSILON);
}

TEST(asinh_dbl_negone) {
	ASSERT_DBL_NEAR(-0.88137, asinh(-1.0), EPSILON);
}

TEST(asinh_dbl_inf) {
	ASSERT_INF(asinh(HUGE_VAL));
}

TEST(asinh_dbl_nan) {
	ASSERT_NAN(asinh(NAN));
}

TEST(asinh_flt_zero) {
	ASSERT_FLT_NEAR(0.0f, asinhf(0.0f), EPSILON);
}

TEST(asinh_flt_one) {
	ASSERT_FLT_NEAR(0.88137f, asinhf(1.0f), EPSILON);
}

TEST(asinh_flt_inf) {
	ASSERT_INF(asinhf(HUGE_VALF));
}

TEST(asinh_flt_nan) {
	ASSERT_NAN(asinhf(NAN));
}

#if JACL_HAS_C99
TEST(asinh_ldbl_zero) {
	ASSERT_LDBL_NEAR(0.0L, asinhl(0.0L), EPSILON);
}

TEST(asinh_ldbl_one) {
	ASSERT_LDBL_NEAR(0.88137L, asinhl(1.0L), EPSILON);
}

TEST(asinh_ldbl_inf) {
	ASSERT_INF(asinhl(HUGE_VALL));
}

TEST(asinh_ldbl_nan) {
	ASSERT_NAN(asinhl(NAN));
}
#endif

/**************************************
 * acosh
 **************************************/
TEST_SUITE(acosh);

TEST(acosh_dbl_one) {
	ASSERT_DBL_NEAR(0.0, acosh(1.0), EPSILON);
}

TEST(acosh_dbl_two) {
	ASSERT_DBL_NEAR(1.31696, acosh(2.0), EPSILON);
}

TEST(acosh_dbl_domain) {
	ASSERT_NAN(acosh(0.5));
}

TEST(acosh_dbl_inf) {
	ASSERT_INF(acosh(HUGE_VAL));
}

TEST(acosh_dbl_nan) {
	ASSERT_NAN(acosh(NAN));
}

TEST(acosh_flt_one) {
	ASSERT_FLT_NEAR(0.0f, acoshf(1.0f), EPSILON);
}

TEST(acosh_flt_two) {
	ASSERT_FLT_NEAR(1.31696f, acoshf(2.0f), EPSILON);
}

TEST(acosh_flt_domain) {
	ASSERT_NAN(acoshf(0.5f));
}

TEST(acosh_flt_inf) {
	ASSERT_INF(acoshf(HUGE_VALF));
}

TEST(acosh_flt_nan) {
	ASSERT_NAN(acoshf(NAN));
}

#if JACL_HAS_C99
TEST(acosh_ldbl_one) {
	ASSERT_LDBL_NEAR(0.0L, acoshl(1.0L), EPSILON);
}

TEST(acosh_ldbl_two) {
	ASSERT_LDBL_NEAR(1.31696L, acoshl(2.0L), EPSILON);
}

TEST(acosh_ldbl_domain) {
	ASSERT_NAN(acoshl(0.5L));
}

TEST(acosh_ldbl_inf) {
	ASSERT_INF(acoshl(HUGE_VALL));
}

TEST(acosh_ldbl_nan) {
	ASSERT_NAN(acoshl(NAN));
}
#endif

/**************************************
 * atanh
 **************************************/
TEST_SUITE(atanh);

TEST(atanh_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, atanh(0.0), EPSILON);
}

TEST(atanh_dbl_half) {
	ASSERT_DBL_NEAR(0.54931, atanh(0.5), EPSILON);
}

TEST(atanh_dbl_neghalf) {
	ASSERT_DBL_NEAR(-0.54931, atanh(-0.5), EPSILON);
}

TEST(atanh_dbl_one) {
	ASSERT_INF(atanh(1.0));
}

TEST(atanh_dbl_domain) {
	ASSERT_NAN(atanh(2.0));
}

TEST(atanh_dbl_nan) {
	ASSERT_NAN(atanh(NAN));
}

TEST(atanh_flt_zero) {
	ASSERT_FLT_NEAR(0.0f, atanhf(0.0f), EPSILON);
}

TEST(atanh_flt_half) {
	ASSERT_FLT_NEAR(0.54931f, atanhf(0.5f), EPSILON);
}

TEST(atanh_flt_one) {
	ASSERT_INF(atanhf(1.0f));
}

TEST(atanh_flt_domain) {
	ASSERT_NAN(atanhf(2.0f));
}

TEST(atanh_flt_nan) {
	ASSERT_NAN(atanhf(NAN));
}

#if JACL_HAS_C99
TEST(atanh_ldbl_zero) {
	ASSERT_LDBL_NEAR(0.0L, atanhl(0.0L), EPSILON);
}

TEST(atanh_ldbl_half) {
	ASSERT_LDBL_NEAR(0.54931L, atanhl(0.5L), EPSILON);
}

TEST(atanh_ldbl_one) {
	ASSERT_INF(atanhl(1.0L));
}

TEST(atanh_ldbl_domain) {
	ASSERT_NAN(atanhl(2.0L));
}

TEST(atanh_ldbl_nan) {
	ASSERT_NAN(atanhl(NAN));
}
#endif

/**************************************
 * ldexp
 **************************************/
TEST_SUITE(ldexp);

TEST(ldexp_dbl_basic) {
	ASSERT_DBL_NEAR(8.0, ldexp(1.0, 3), EPSILON);
}

TEST(ldexp_dbl_neg_exp) {
	ASSERT_DBL_NEAR(0.5, ldexp(1.0, -1), EPSILON);
}

TEST(ldexp_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, ldexp(0.0, 10), EPSILON);
}

TEST(ldexp_dbl_inf) {
	ASSERT_INF(ldexp(HUGE_VAL, 1));
}

TEST(ldexp_dbl_nan) {
	ASSERT_NAN(ldexp(NAN, 1));
}

TEST(ldexp_dbl_overflow) {
	ASSERT_INF(ldexp(1.0, 10000));
}

TEST(ldexp_flt_basic) {
	ASSERT_FLT_NEAR(8.0f, ldexpf(1.0f, 3), EPSILON);
}

TEST(ldexp_flt_neg_exp) {
	ASSERT_FLT_NEAR(0.5f, ldexpf(1.0f, -1), EPSILON);
}

TEST(ldexp_flt_zero) {
	ASSERT_FLT_NEAR(0.0f, ldexpf(0.0f, 10), EPSILON);
}

TEST(ldexp_flt_inf) {
	ASSERT_INF(ldexpf(HUGE_VALF, 1));
}

TEST(ldexp_flt_nan) {
	ASSERT_NAN(ldexpf(NAN, 1));
}

#if JACL_HAS_C99
TEST(ldexp_ldbl_basic) {
	ASSERT_LDBL_NEAR(8.0L, ldexpl(1.0L, 3), EPSILON);
}

TEST(ldexp_ldbl_neg_exp) {
	ASSERT_LDBL_NEAR(0.5L, ldexpl(1.0L, -1), EPSILON);
}

TEST(ldexp_ldbl_zero) {
	ASSERT_LDBL_NEAR(0.0L, ldexpl(0.0L, 10), EPSILON);
}

TEST(ldexp_ldbl_inf) {
	ASSERT_INF(ldexpl(HUGE_VALL, 1));
}

TEST(ldexp_ldbl_nan) {
	ASSERT_NAN(ldexpl(NAN, 1));
}
#endif

/**************************************
 * frexp
 **************************************/
TEST_SUITE(frexp);

TEST(frexp_dbl_eight) {
	int exp;
	double m = frexp(8.0, &exp);
	ASSERT_DBL_NEAR(0.5, m, EPSILON);
	ASSERT_EQ(4, exp);
}

TEST(frexp_dbl_half) {
	int exp;
	double m = frexp(0.5, &exp);
	ASSERT_DBL_NEAR(0.5, m, EPSILON);
	ASSERT_EQ(0, exp);
}

TEST(frexp_dbl_zero) {
	int exp;
	double m = frexp(0.0, &exp);
	ASSERT_DBL_NEAR(0.0, m, EPSILON);
	ASSERT_EQ(0, exp);
}

TEST(frexp_dbl_inf) {
	int exp;
	double m = frexp(HUGE_VAL, &exp);
	ASSERT_INF(m);
}

TEST(frexp_dbl_nan) {
	int exp;
	double m = frexp(NAN, &exp);
	ASSERT_NAN(m);
}

TEST(frexp_flt_eight) {
	int exp;
	float m = frexpf(8.0f, &exp);
	ASSERT_FLT_NEAR(0.5f, m, EPSILON);
	ASSERT_EQ(4, exp);
}

TEST(frexp_flt_half) {
	int exp;
	float m = frexpf(0.5f, &exp);
	ASSERT_FLT_NEAR(0.5f, m, EPSILON);
	ASSERT_EQ(0, exp);
}

TEST(frexp_flt_zero) {
	int exp;
	float m = frexpf(0.0f, &exp);
	ASSERT_FLT_NEAR(0.0f, m, EPSILON);
	ASSERT_EQ(0, exp);
}

#if JACL_HAS_C99
TEST(frexp_ldbl_eight) {
	int exp;
	long double m = frexpl(8.0L, &exp);
	ASSERT_LDBL_NEAR(0.5L, m, EPSILON);
	ASSERT_EQ(4, exp);
}

TEST(frexp_ldbl_zero) {
	int exp;
	long double m = frexpl(0.0L, &exp);
	ASSERT_LDBL_NEAR(0.0L, m, EPSILON);
	ASSERT_EQ(0, exp);
}
#endif

/**************************************
 * scalbn
 **************************************/
TEST_SUITE(scalbn);

TEST(scalbn_dbl_basic) {
	ASSERT_DBL_NEAR(8.0, scalbn(1.0, 3), EPSILON);
}

TEST(scalbn_dbl_neg) {
	ASSERT_DBL_NEAR(0.25, scalbn(1.0, -2), EPSILON);
}

TEST(scalbn_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, scalbn(0.0, 5), EPSILON);
}

TEST(scalbn_dbl_inf) {
	ASSERT_INF(scalbn(HUGE_VAL, 1));
}

TEST(scalbn_dbl_nan) {
	ASSERT_NAN(scalbn(NAN, 1));
}

TEST(scalbn_flt_basic) {
	ASSERT_FLT_NEAR(8.0f, scalbnf(1.0f, 3), EPSILON);
}

TEST(scalbn_flt_neg) {
	ASSERT_FLT_NEAR(0.25f, scalbnf(1.0f, -2), EPSILON);
}

TEST(scalbn_flt_zero) {
	ASSERT_FLT_NEAR(0.0f, scalbnf(0.0f, 5), EPSILON);
}

#if JACL_HAS_C99
TEST(scalbn_ldbl_basic) {
	ASSERT_LDBL_NEAR(8.0L, scalbnl(1.0L, 3), EPSILON);
}

TEST(scalbn_ldbl_neg) {
	ASSERT_LDBL_NEAR(0.25L, scalbnl(1.0L, -2), EPSILON);
}

TEST(scalbn_ldbl_zero) {
	ASSERT_LDBL_NEAR(0.0L, scalbnl(0.0L, 5), EPSILON);
}
#endif

/**************************************
 * scalbln
 **************************************/
TEST_SUITE(scalbln);

TEST(scalbln_dbl_basic) {
	ASSERT_DBL_NEAR(16.0, scalbln(1.0, 4L), EPSILON);
}

TEST(scalbln_dbl_neg) {
	ASSERT_DBL_NEAR(0.125, scalbln(1.0, -3L), EPSILON);
}

TEST(scalbln_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, scalbln(0.0, 5L), EPSILON);
}

TEST(scalbln_flt_basic) {
	ASSERT_FLT_NEAR(16.0f, scalblnf(1.0f, 4L), EPSILON);
}

TEST(scalbln_flt_neg) {
	ASSERT_FLT_NEAR(0.125f, scalblnf(1.0f, -3L), EPSILON);
}

#if JACL_HAS_C99
TEST(scalbln_ldbl_basic) {
	ASSERT_LDBL_NEAR(16.0L, scalblnl(1.0L, 4L), EPSILON);
}

TEST(scalbln_ldbl_neg) {
	ASSERT_LDBL_NEAR(0.125L, scalblnl(1.0L, -3L), EPSILON);
}
#endif

/**************************************
 * logb
 **************************************/
TEST_SUITE(logb);

TEST(logb_dbl_eight) {
	ASSERT_DBL_NEAR(3.0, logb(8.0), EPSILON);
}

TEST(logb_dbl_seven) {
	ASSERT_DBL_NEAR(2.0, logb(7.0), EPSILON);
}

TEST(logb_dbl_half) {
	ASSERT_DBL_NEAR(-1.0, logb(0.5), EPSILON);
}

TEST(logb_dbl_zero) {
	ASSERT_INF(logb(0.0));
}

TEST(logb_dbl_inf) {
	ASSERT_INF(logb(HUGE_VAL));
}

TEST(logb_dbl_nan) {
	ASSERT_NAN(logb(NAN));
}

TEST(logb_flt_eight) {
	ASSERT_FLT_NEAR(3.0f, logbf(8.0f), EPSILON);
}

TEST(logb_flt_seven) {
	ASSERT_FLT_NEAR(2.0f, logbf(7.0f), EPSILON);
}

TEST(logb_flt_zero) {
	ASSERT_INF(logbf(0.0f));
}

TEST(logb_flt_nan) {
	ASSERT_NAN(logbf(NAN));
}

#if JACL_HAS_C99
TEST(logb_ldbl_eight) {
	ASSERT_LDBL_NEAR(3.0L, logbl(8.0L), EPSILON);
}

TEST(logb_ldbl_zero) {
	ASSERT_INF(logbl(0.0L));
}

TEST(logb_ldbl_nan) {
	ASSERT_NAN(logbl(NAN));
}
#endif

/**************************************
 * ilogb
 **************************************/
TEST_SUITE(ilogb);

TEST(ilogb_dbl_eight) {
	ASSERT_EQ(3, ilogb(8.0));
}

TEST(ilogb_dbl_seven) {
	ASSERT_EQ(2, ilogb(7.0));
}

TEST(ilogb_dbl_half) {
	ASSERT_EQ(-1, ilogb(0.5));
}

TEST(ilogb_dbl_zero) {
	ASSERT_EQ(FP_ILOGB0, ilogb(0.0));
}

TEST(ilogb_dbl_nan) {
	ASSERT_EQ(FP_ILOGBNAN, ilogb(NAN));
}

TEST(ilogb_flt_eight) {
	ASSERT_EQ(3, ilogbf(8.0f));
}

TEST(ilogb_flt_seven) {
	ASSERT_EQ(2, ilogbf(7.0f));
}

TEST(ilogb_flt_zero) {
	ASSERT_EQ(FP_ILOGB0, ilogbf(0.0f));
}

TEST(ilogb_flt_nan) {
	ASSERT_EQ(FP_ILOGBNAN, ilogbf(NAN));
}

#if JACL_HAS_C99
TEST(ilogb_ldbl_eight) {
	ASSERT_EQ(3, ilogbl(8.0L));
}

TEST(ilogb_ldbl_zero) {
	ASSERT_EQ(FP_ILOGB0, ilogbl(0.0L));
}

TEST(ilogb_ldbl_nan) {
	ASSERT_EQ(FP_ILOGBNAN, ilogbl(NAN));
}
#endif

/**************************************
 * fmod
 **************************************/
TEST_SUITE(fmod);

TEST(fmod_dbl_basic) {
	ASSERT_DBL_NEAR(1.0, fmod(7.0, 3.0), EPSILON);
}

TEST(fmod_dbl_exact) {
	ASSERT_DBL_NEAR(0.0, fmod(8.0, 4.0), EPSILON);
}

TEST(fmod_dbl_frac) {
	ASSERT_DBL_NEAR(0.5, fmod(5.5, 1.0), EPSILON);
}

TEST(fmod_dbl_neg_dividend) {
	ASSERT_DBL_NEAR(-1.0, fmod(-7.0, 3.0), EPSILON);
}

TEST(fmod_dbl_neg_divisor) {
	ASSERT_DBL_NEAR(1.0, fmod(7.0, -3.0), EPSILON);
}

TEST(fmod_dbl_zero_divisor) {
	ASSERT_NAN(fmod(1.0, 0.0));
}

TEST(fmod_dbl_nan) {
	ASSERT_NAN(fmod(NAN, 1.0));
}

TEST(fmod_flt_basic) {
	ASSERT_FLT_NEAR(1.0f, fmodf(7.0f, 3.0f), EPSILON);
}

TEST(fmod_flt_exact) {
	ASSERT_FLT_NEAR(0.0f, fmodf(8.0f, 4.0f), EPSILON);
}

TEST(fmod_flt_zero_divisor) {
	ASSERT_NAN(fmodf(1.0f, 0.0f));
}

TEST(fmod_flt_nan) {
	ASSERT_NAN(fmodf(NAN, 1.0f));
}

#if JACL_HAS_C99
TEST(fmod_ldbl_basic) {
	ASSERT_LDBL_NEAR(1.0L, fmodl(7.0L, 3.0L), EPSILON);
}

TEST(fmod_ldbl_exact) {
	ASSERT_LDBL_NEAR(0.0L, fmodl(8.0L, 4.0L), EPSILON);
}

TEST(fmod_ldbl_zero_divisor) {
	ASSERT_NAN(fmodl(1.0L, 0.0L));
}

TEST(fmod_ldbl_nan) {
	ASSERT_NAN(fmodl(NAN, 1.0L));
}
#endif

/**************************************
 * remainder
 **************************************/
TEST_SUITE(remainder);

TEST(remainder_dbl_basic) {
	ASSERT_DBL_NEAR(1.0, remainder(7.0, 3.0), EPSILON);
}

TEST(remainder_dbl_exact) {
	ASSERT_DBL_NEAR(0.0, remainder(8.0, 4.0), EPSILON);
}

TEST(remainder_dbl_zero_divisor) {
	ASSERT_NAN(remainder(1.0, 0.0));
}

TEST(remainder_dbl_nan) {
	ASSERT_NAN(remainder(NAN, 1.0));
}

TEST(remainder_flt_basic) {
	ASSERT_FLT_NEAR(1.0f, remainderf(7.0f, 3.0f), EPSILON);
}

TEST(remainder_flt_exact) {
	ASSERT_FLT_NEAR(0.0f, remainderf(8.0f, 4.0f), EPSILON);
}

TEST(remainder_flt_zero_divisor) {
	ASSERT_NAN(remainderf(1.0f, 0.0f));
}

TEST(remainder_flt_nan) {
	ASSERT_NAN(remainderf(NAN, 1.0f));
}

#if JACL_HAS_C99
TEST(remainder_ldbl_basic) {
	ASSERT_LDBL_NEAR(1.0L, remainderl(7.0L, 3.0L), EPSILON);
}

TEST(remainder_ldbl_exact) {
	ASSERT_LDBL_NEAR(0.0L, remainderl(8.0L, 4.0L), EPSILON);
}

TEST(remainder_ldbl_zero_divisor) {
	ASSERT_NAN(remainderl(1.0L, 0.0L));
}

TEST(remainder_ldbl_nan) {
	ASSERT_NAN(remainderl(NAN, 1.0L));
}
#endif

/**************************************
 * remquo
 **************************************/
TEST_SUITE(remquo);

TEST(remquo_dbl_basic) {
	int quo;
	double r = remquo(7.0, 3.0, &quo);
	ASSERT_DBL_NEAR(1.0, r, EPSILON);
	ASSERT_EQ(2, quo);
}

TEST(remquo_dbl_exact) {
	int quo;
	double r = remquo(8.0, 4.0, &quo);
	ASSERT_DBL_NEAR(0.0, r, EPSILON);
	ASSERT_EQ(2, quo);
}

TEST(remquo_dbl_zero_divisor) {
	int quo;
	ASSERT_NAN(remquo(1.0, 0.0, &quo));
}

TEST(remquo_flt_basic) {
	int quo;
	float r = remquof(7.0f, 3.0f, &quo);
	ASSERT_FLT_NEAR(1.0f, r, EPSILON);
	ASSERT_EQ(2, quo);
}

TEST(remquo_flt_zero_divisor) {
	int quo;
	ASSERT_NAN(remquof(1.0f, 0.0f, &quo));
}

#if JACL_HAS_C99
TEST(remquo_ldbl_basic) {
	int quo;
	long double r = remquol(7.0L, 3.0L, &quo);
	ASSERT_LDBL_NEAR(1.0L, r, EPSILON);
	ASSERT_EQ(2, quo);
}

TEST(remquo_ldbl_zero_divisor) {
	int quo;
	ASSERT_NAN(remquol(1.0L, 0.0L, &quo));
}
#endif

/**************************************
 * modf
 **************************************/
TEST_SUITE(modf);

TEST(modf_dbl_basic) {
	double i;
	double f = modf(3.14, &i);
	ASSERT_DBL_NEAR(3.0, i, EPSILON);
	ASSERT_DBL_NEAR(0.14, f, 0.01);
}

TEST(modf_dbl_neg) {
	double i;
	double f = modf(-2.7, &i);
	ASSERT_DBL_NEAR(-2.0, i, EPSILON);
	ASSERT_DBL_NEAR(-0.7, f, 0.01);
}

TEST(modf_dbl_exact) {
	double i;
	double f = modf(5.0, &i);
	ASSERT_DBL_NEAR(5.0, i, EPSILON);
	ASSERT_DBL_NEAR(0.0, f, EPSILON);
}

TEST(modf_dbl_zero) {
	double i;
	double f = modf(0.0, &i);
	ASSERT_DBL_NEAR(0.0, i, EPSILON);
	ASSERT_DBL_NEAR(0.0, f, EPSILON);
}

TEST(modf_flt_basic) {
	float i;
	float f = modff(3.14f, &i);
	ASSERT_FLT_NEAR(3.0f, i, EPSILON);
	ASSERT_FLT_NEAR(0.14f, f, 0.01);
}

TEST(modf_flt_neg) {
	float i;
	float f = modff(-2.7f, &i);
	ASSERT_FLT_NEAR(-2.0f, i, EPSILON);
	ASSERT_FLT_NEAR(-0.7f, f, 0.01);
}

#if JACL_HAS_C99
TEST(modf_ldbl_basic) {
	long double i;
	long double f = modfl(3.14L, &i);
	ASSERT_LDBL_NEAR(3.0L, i, EPSILON);
	ASSERT_LDBL_NEAR(0.14L, f, 0.01);
}

TEST(modf_ldbl_neg) {
	long double i;
	long double f = modfl(-2.7L, &i);
	ASSERT_LDBL_NEAR(-2.0L, i, EPSILON);
	ASSERT_LDBL_NEAR(-0.7L, f, 0.01);
}
#endif

/**************************************
 * fdim
 **************************************/
TEST_SUITE(fdim);

TEST(fdim_dbl_positive) {
	ASSERT_DBL_NEAR(3.0, fdim(5.0, 2.0), EPSILON);
}

TEST(fdim_dbl_negative) {
	ASSERT_DBL_NEAR(0.0, fdim(2.0, 5.0), EPSILON);
}

TEST(fdim_dbl_equal) {
	ASSERT_DBL_NEAR(0.0, fdim(3.0, 3.0), EPSILON);
}

TEST(fdim_dbl_nan) {
	ASSERT_NAN(fdim(NAN, 1.0));
}

TEST(fdim_flt_positive) {
	ASSERT_FLT_NEAR(3.0f, fdimf(5.0f, 2.0f), EPSILON);
}

TEST(fdim_flt_negative) {
	ASSERT_FLT_NEAR(0.0f, fdimf(2.0f, 5.0f), EPSILON);
}

TEST(fdim_flt_nan) {
	ASSERT_NAN(fdimf(NAN, 1.0f));
}

#if JACL_HAS_C99
TEST(fdim_ldbl_positive) {
	ASSERT_LDBL_NEAR(3.0L, fdiml(5.0L, 2.0L), EPSILON);
}

TEST(fdim_ldbl_negative) {
	ASSERT_LDBL_NEAR(0.0L, fdiml(2.0L, 5.0L), EPSILON);
}

TEST(fdim_ldbl_nan) {
	ASSERT_NAN(fdiml(NAN, 1.0L));
}
#endif

/**************************************
 * fmax
 **************************************/
TEST_SUITE(fmax);

TEST(fmax_dbl_basic) {
	ASSERT_DBL_NEAR(5.0, fmax(3.0, 5.0), EPSILON);
}

TEST(fmax_dbl_reverse) {
	ASSERT_DBL_NEAR(5.0, fmax(5.0, 3.0), EPSILON);
}

TEST(fmax_dbl_nan_first) {
	ASSERT_DBL_NEAR(3.0, fmax(NAN, 3.0), EPSILON);
}

TEST(fmax_dbl_nan_second) {
	ASSERT_DBL_NEAR(3.0, fmax(3.0, NAN), EPSILON);
}

TEST(fmax_dbl_both_nan) {
	ASSERT_NAN(fmax(NAN, NAN));
}

TEST(fmax_flt_basic) {
	ASSERT_FLT_NEAR(5.0f, fmaxf(3.0f, 5.0f), EPSILON);
}

TEST(fmax_flt_nan_first) {
	ASSERT_FLT_NEAR(3.0f, fmaxf(NAN, 3.0f), EPSILON);
}

#if JACL_HAS_C99
TEST(fmax_ldbl_basic) {
	ASSERT_LDBL_NEAR(5.0L, fmaxl(3.0L, 5.0L), EPSILON);
}

TEST(fmax_ldbl_nan_first) {
	ASSERT_LDBL_NEAR(3.0L, fmaxl(NAN, 3.0L), EPSILON);
}
#endif

/**************************************
 * fmin
 **************************************/
TEST_SUITE(fmin);

TEST(fmin_dbl_basic) {
	ASSERT_DBL_NEAR(3.0, fmin(3.0, 5.0), EPSILON);
}

TEST(fmin_dbl_reverse) {
	ASSERT_DBL_NEAR(3.0, fmin(5.0, 3.0), EPSILON);
}

TEST(fmin_dbl_nan_first) {
	ASSERT_DBL_NEAR(3.0, fmin(NAN, 3.0), EPSILON);
}

TEST(fmin_dbl_nan_second) {
	ASSERT_DBL_NEAR(3.0, fmin(3.0, NAN), EPSILON);
}

TEST(fmin_dbl_both_nan) {
	ASSERT_NAN(fmin(NAN, NAN));
}

TEST(fmin_flt_basic) {
	ASSERT_FLT_NEAR(3.0f, fminf(3.0f, 5.0f), EPSILON);
}

TEST(fmin_flt_nan_first) {
	ASSERT_FLT_NEAR(3.0f, fminf(NAN, 3.0f), EPSILON);
}

#if JACL_HAS_C99
TEST(fmin_ldbl_basic) {
	ASSERT_LDBL_NEAR(3.0L, fminl(3.0L, 5.0L), EPSILON);
}

TEST(fmin_ldbl_nan_first) {
	ASSERT_LDBL_NEAR(3.0L, fminl(NAN, 3.0L), EPSILON);
}
#endif

/**************************************
 * fma
 **************************************/
TEST_SUITE(fma);

TEST(fma_dbl_basic) {
	ASSERT_DBL_NEAR(11.0, fma(2.0, 3.0, 5.0), EPSILON);
}

TEST(fma_dbl_zero_mult) {
	ASSERT_DBL_NEAR(5.0, fma(0.0, 10.0, 5.0), EPSILON);
}

TEST(fma_dbl_precision) {
	double a = 1e308;
	double b = 2.0;
	double c = -1e308;
	double r = fma(a, b, c);
	ASSERT_DBL_NEAR(1e308, r, 0.001);
}

TEST(fma_dbl_nan) {
	ASSERT_NAN(fma(NAN, 1.0, 1.0));
}

TEST(fma_flt_basic) {
	ASSERT_FLT_NEAR(11.0f, fmaf(2.0f, 3.0f, 5.0f), EPSILON);
}

TEST(fma_flt_zero_mult) {
	ASSERT_FLT_NEAR(5.0f, fmaf(0.0f, 10.0f, 5.0f), EPSILON);
}

TEST(fma_flt_nan) {
	ASSERT_NAN(fmaf(NAN, 1.0f, 1.0f));
}

#if JACL_HAS_C99
TEST(fma_ldbl_basic) {
	ASSERT_LDBL_NEAR(11.0L, fmal(2.0L, 3.0L, 5.0L), EPSILON);
}

TEST(fma_ldbl_zero_mult) {
	ASSERT_LDBL_NEAR(5.0L, fmal(0.0L, 10.0L, 5.0L), EPSILON);
}

TEST(fma_ldbl_nan) {
	ASSERT_NAN(fmal(NAN, 1.0L, 1.0L));
}
#endif

/**************************************
 * erf
 **************************************/
TEST_SUITE(erf);

TEST(erf_dbl_zero) {
	ASSERT_DBL_NEAR(0.0, erf(0.0), EPSILON);
}

TEST(erf_dbl_one) {
	ASSERT_DBL_NEAR(0.84270, erf(1.0), 0.001);
}

TEST(erf_dbl_negone) {
	ASSERT_DBL_NEAR(-0.84270, erf(-1.0), 0.001);
}

TEST(erf_dbl_two) {
	ASSERT_DBL_NEAR(0.99532, erf(2.0), 0.001);
}

TEST(erf_dbl_nan) {
	ASSERT_NAN(erf(NAN));
}

TEST(erf_flt_zero) {
	ASSERT_FLT_NEAR(0.0f, erff(0.0f), EPSILON);
}

TEST(erf_flt_one) {
	ASSERT_FLT_NEAR(0.84270f, erff(1.0f), 0.001);
}

TEST(erf_flt_nan) {
	ASSERT_NAN(erff(NAN));
}

#if JACL_HAS_C99
TEST(erf_ldbl_zero) {
	ASSERT_LDBL_NEAR(0.0L, erfl(0.0L), EPSILON);
}

TEST(erf_ldbl_one) {
	ASSERT_LDBL_NEAR(0.84270L, erfl(1.0L), 0.001);
}

TEST(erf_ldbl_nan) {
	ASSERT_NAN(erfl(NAN));
}
#endif

/**************************************
 * erfc
 **************************************/
TEST_SUITE(erfc);

TEST(erfc_dbl_zero) {
	ASSERT_DBL_NEAR(1.0, erfc(0.0), EPSILON);
}

TEST(erfc_dbl_one) {
	ASSERT_DBL_NEAR(0.15730, erfc(1.0), 0.001);
}

TEST(erfc_dbl_negone) {
	ASSERT_DBL_NEAR(1.84270, erfc(-1.0), 0.001);
}

TEST(erfc_dbl_nan) {
	ASSERT_NAN(erfc(NAN));
}

TEST(erfc_flt_zero) {
	ASSERT_FLT_NEAR(1.0f, erfcf(0.0f), EPSILON);
}

TEST(erfc_flt_one) {
	ASSERT_FLT_NEAR(0.15730f, erfcf(1.0f), 0.001);
}

TEST(erfc_flt_nan) {
	ASSERT_NAN(erfcf(NAN));
}

#if JACL_HAS_C99
TEST(erfc_ldbl_zero) {
	ASSERT_LDBL_NEAR(1.0L, erfcl(0.0L), EPSILON);
}

TEST(erfc_ldbl_one) {
	ASSERT_LDBL_NEAR(0.15730L, erfcl(1.0L), 0.001);
}

TEST(erfc_ldbl_nan) {
	ASSERT_NAN(erfcl(NAN));
}
#endif

/**************************************
 * tgamma
 **************************************/
TEST_SUITE(tgamma);

TEST(tgamma_dbl_one) {
	ASSERT_DBL_NEAR(1.0, tgamma(1.0), 0.01);
}

TEST(tgamma_dbl_two) {
	ASSERT_DBL_NEAR(1.0, tgamma(2.0), 0.01);
}

TEST(tgamma_dbl_three) {
	ASSERT_DBL_NEAR(2.0, tgamma(3.0), 0.01);
}

TEST(tgamma_dbl_four) {
	ASSERT_DBL_NEAR(6.0, tgamma(4.0), 0.1);
}

TEST(tgamma_dbl_five) {
	ASSERT_DBL_NEAR(24.0, tgamma(5.0), 0.5);
}

TEST(tgamma_dbl_neg_int) {
	ASSERT_NAN(tgamma(-1.0));
}

TEST(tgamma_dbl_nan) {
	ASSERT_NAN(tgamma(NAN));
}

TEST(tgamma_flt_one) {
	ASSERT_FLT_NEAR(1.0f, tgammaf(1.0f), 0.01);
}

TEST(tgamma_flt_two) {
	ASSERT_FLT_NEAR(1.0f, tgammaf(2.0f), 0.01);
}

TEST(tgamma_flt_nan) {
	ASSERT_NAN(tgammaf(NAN));
}

#if JACL_HAS_C99
TEST(tgamma_ldbl_one) {
	ASSERT_LDBL_NEAR(1.0L, tgammal(1.0L), 0.01);
}

TEST(tgamma_ldbl_two) {
	ASSERT_LDBL_NEAR(1.0L, tgammal(2.0L), 0.01);
}

TEST(tgamma_ldbl_nan) {
	ASSERT_NAN(tgammal(NAN));
}
#endif

/**************************************
 * lgamma
 **************************************/
TEST_SUITE(lgamma);

TEST(lgamma_dbl_one) {
	ASSERT_DBL_NEAR(0.0, lgamma(1.0), 0.01);
}

TEST(lgamma_dbl_two) {
	ASSERT_DBL_NEAR(0.0, lgamma(2.0), 0.01);
}

TEST(lgamma_dbl_three) {
	ASSERT_DBL_NEAR(0.69315, lgamma(3.0), 0.01);
}

TEST(lgamma_dbl_four) {
	ASSERT_DBL_NEAR(1.79176, lgamma(4.0), 0.01);
}

TEST(lgamma_dbl_neg_int) {
	ASSERT_INF(lgamma(-1.0));
}

TEST(lgamma_dbl_nan) {
	ASSERT_NAN(lgamma(NAN));
}

TEST(lgamma_flt_one) {
	ASSERT_FLT_NEAR(0.0f, lgammaf(1.0f), 0.01);
}

TEST(lgamma_flt_two) {
	ASSERT_FLT_NEAR(0.0f, lgammaf(2.0f), 0.01);
}

TEST(lgamma_flt_nan) {
	ASSERT_NAN(lgammaf(NAN));
}

#if JACL_HAS_C99
TEST(lgamma_ldbl_one) {
	ASSERT_LDBL_NEAR(0.0L, lgammal(1.0L), 0.01);
}

TEST(lgamma_ldbl_two) {
	ASSERT_LDBL_NEAR(0.0L, lgammal(2.0L), 0.01);
}

TEST(lgamma_ldbl_nan) {
	ASSERT_NAN(lgammal(NAN));
}
#endif

/**************************************
 * copysign
 **************************************/
TEST_SUITE(copysign);

TEST(copysign_dbl_pos_pos) {
	ASSERT_DBL_NEAR(3.0, copysign(3.0, 1.0), EPSILON);
}

TEST(copysign_dbl_pos_neg) {
	ASSERT_DBL_NEAR(-3.0, copysign(3.0, -1.0), EPSILON);
}

TEST(copysign_dbl_neg_pos) {
	ASSERT_DBL_NEAR(3.0, copysign(-3.0, 1.0), EPSILON);
}

TEST(copysign_dbl_neg_neg) {
	ASSERT_DBL_NEAR(-3.0, copysign(-3.0, -1.0), EPSILON);
}

TEST(copysign_dbl_zero_pos) {
	ASSERT_DBL_NEAR(0.0, copysign(0.0, 1.0), EPSILON);
	ASSERT_EQ(0, signbit(copysign(0.0, 1.0)));
}

TEST(copysign_dbl_zero_neg) {
	ASSERT_DBL_NEAR(0.0, copysign(0.0, -1.0), EPSILON);
	ASSERT_NE(0, signbit(copysign(0.0, -1.0)));
}

TEST(copysign_dbl_nan) {
	double val = copysign(NAN, -1.0);
	ASSERT_NAN(val);
	ASSERT_NE(0, signbit(val));
}

TEST(copysign_flt_pos_pos) {
	ASSERT_FLT_NEAR(3.0f, copysignf(3.0f, 1.0f), EPSILON);
}

TEST(copysign_flt_pos_neg) {
	ASSERT_FLT_NEAR(-3.0f, copysignf(3.0f, -1.0f), EPSILON);
}

TEST(copysign_flt_zero_pos) {
	ASSERT_FLT_NEAR(0.0f, copysignf(0.0f, 1.0f), EPSILON);
	ASSERT_EQ(0, signbit(copysignf(0.0f, 1.0f)));
}

#if JACL_HAS_C99
TEST(copysign_ldbl_pos_pos) {
	ASSERT_LDBL_NEAR(3.0L, copysignl(3.0L, 1.0L), EPSILON);
}

TEST(copysign_ldbl_pos_neg) {
	ASSERT_LDBL_NEAR(-3.0L, copysignl(3.0L, -1.0L), EPSILON);
}

TEST(copysign_ldbl_zero_pos) {
	ASSERT_LDBL_NEAR(0.0L, copysignl(0.0L, 1.0L), EPSILON);
	ASSERT_EQ(0, signbit(copysignl(0.0L, 1.0L)));
}
#endif

/**************************************
 * nextafter
 **************************************/
TEST_SUITE(nextafter);

TEST(nextafter_dbl_up) {
	double x = 1.0;
	double n = nextafter(x, 2.0);
	ASSERT_DBL_GT(n, x);
	ASSERT_DBL_LT(n, 1.0 + 0.0001);
}

TEST(nextafter_dbl_down) {
	double x = 1.0;
	double n = nextafter(x, 0.0);
	ASSERT_TRUE(n < x);
	ASSERT_TRUE(n > 0.9999);
}

TEST(nextafter_dbl_same) {
	ASSERT_DBL_NEAR(0.0, nextafter(0.0, 0.0), 0.0);
}

TEST(nextafter_dbl_to_inf) {
	ASSERT_INF(nextafter(DBL_MAX, HUGE_VAL));
}

TEST(nextafter_dbl_nan) {
	ASSERT_NAN(nextafter(NAN, 1.0));
}

TEST(nextafter_flt_up) {
	float x = 1.0f;
	float n = nextafterf(x, 2.0f);
	ASSERT_FLT_GT(n, x);
}

TEST(nextafter_flt_down) {
	float x = 1.0f;
	float n = nextafterf(x, 0.0f);
	ASSERT_TRUE(n < x);
}

TEST(nextafter_flt_nan) {
	ASSERT_NAN(nextafterf(NAN, 1.0f));
}

#if JACL_HAS_C99
TEST(nextafter_ldbl_up) {
	long double x = 1.0L;
	long double n = nextafterl(x, 2.0L);
	ASSERT_LDBL_GT(n, x);
}

TEST(nextafter_ldbl_down) {
	long double x = 1.0L;
	long double n = nextafterl(x, 0.0L);
	ASSERT_TRUE(n < x);
}

TEST(nextafter_ldbl_nan) {
	ASSERT_NAN(nextafterl(NAN, 1.0L));
}
#endif

/**************************************
 * nexttoward
 **************************************/
#if JACL_HAS_C99
TEST_SUITE(nexttoward);

TEST(nexttoward_dbl_up) {
	double x = 1.0;
	double n = nexttoward(x, 2.0L);
	ASSERT_DBL_GT(n, x);
	ASSERT_DBL_LT(n, 1.0001);
}

TEST(nexttoward_dbl_down) {
	double x = 1.0;
	double n = nexttoward(x, 0.0L);
	ASSERT_TRUE(n < x);
}

TEST(nexttoward_dbl_nan) {
	ASSERT_NAN(nexttoward(NAN, 1.0L));
}

TEST(nexttoward_flt_up) {
	float x = 1.0f;
	float n = nexttowardf(x, 2.0L);
	ASSERT_FLT_GT(n, x);
}

TEST(nexttoward_flt_nan) {
	ASSERT_NAN(nexttowardf(NAN, 1.0L));
}

TEST(nexttoward_ldbl_up) {
	long double x = 1.0L;
	long double n = nexttowardl(x, 2.0L);
	ASSERT_LDBL_GT(n, x);
}

TEST(nexttoward_ldbl_nan) {
	ASSERT_NAN(nexttowardl(NAN, 1.0L));
}
#endif

/**************************************
 * nan
 **************************************/
TEST_SUITE(nan);

TEST(nan_dbl_generation) {
	double n = nan("");
	ASSERT_NAN(n);
}

TEST(nan_dbl_propagation) {
	double n = nan("");
	ASSERT_NAN(n + 1.0);
}

TEST(nan_flt_generation) {
	float n = nanf("");
	ASSERT_NAN(n);
}

TEST(nan_flt_propagation) {
	float n = nanf("");
	ASSERT_NAN(n + 1.0f);
}

#if JACL_HAS_C99
TEST(nan_ldbl_generation) {
	long double n = nanl("");
	ASSERT_NAN(n);
}

TEST(nan_ldbl_propagation) {
	long double n = nanl("");
	ASSERT_NAN(n + 1.0L);
}
#endif

/**************************************
 * Bessel functions (j0, j1, jn, y0, y1, yn)
 **************************************/
TEST_SUITE(bessel);

TEST(bessel_j0_zero) {
	ASSERT_DBL_NEAR(1.0, j0(0.0), EPSILON);
}

TEST(bessel_j0_three) {
	ASSERT_DBL_NEAR(-0.260052, j0(3.0), 0.001);
}

TEST(bessel_j1_zero) {
	ASSERT_DBL_NEAR(0.0, j1(0.0), EPSILON);
}

TEST(bessel_j1_three) {
	ASSERT_DBL_NEAR(0.339059, j1(3.0), 0.001);
}

TEST(bessel_jn_match_j0) {
	ASSERT_DBL_NEAR(j0(3.0), jn(0, 3.0), EPSILON);
}

TEST(bessel_jn_match_j1) {
	ASSERT_DBL_NEAR(j1(3.0), jn(1, 3.0), EPSILON);
}

TEST(bessel_jn_order2) {
	ASSERT_DBL_NEAR(0.486091, jn(2, 3.0), 0.001);
}

TEST(bessel_y0_zero) {
	ASSERT_INF(y0(0.0));
}

TEST(bessel_y0_three) {
	ASSERT_DBL_NEAR(0.376850, y0(3.0), 0.001);
}

TEST(bessel_y1_zero) {
	ASSERT_INF(y1(0.0));
}

TEST(bessel_y1_three) {
	ASSERT_DBL_NEAR(0.324674, y1(3.0), 0.001);
}

TEST(bessel_yn_match_y0) {
	ASSERT_DBL_NEAR(y0(3.0), yn(0, 3.0), EPSILON);
}

TEST(bessel_yn_match_y1) {
	ASSERT_DBL_NEAR(y1(3.0), yn(1, 3.0), EPSILON);
}

TEST(bessel_yn_order2) {
	ASSERT_DBL_NEAR(-0.160400, yn(2, 3.0), 0.001);
}

TEST_MAIN()
