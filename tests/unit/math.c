/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <math.h>
#include <float.h>
#include <stdint.h>

TEST_TYPE(unit);
TEST_UNIT(math.h);

#define EPS 0.0001
#define T_EPS 0.00001

/* =========================================
 * BASE MACROS (Always available)
 * ========================================= */

#define GEN_CLASSIFICATION(S, FSUF, T, INF_VAL, MIN_VAL, TRUE_MIN_VAL) \
TEST_SUITE(isnan##S); \
TEST(isnan##S##_norm) { ASSERT_FALSE(isnan((T)1.0)); } \
TEST(isnan##S##_zero) { ASSERT_FALSE(isnan((T)0.0)); } \
TEST(isnan##S##_nzero) { ASSERT_FALSE(isnan((T)-0.0)); } \
TEST(isnan##S##_inf) { ASSERT_FALSE(isnan(INF_VAL)); } \
TEST(isnan##S##_ninf) { ASSERT_FALSE(isnan(-INF_VAL)); } \
TEST(isnan##S##_nan) { ASSERT_TRUE(isnan((T)NAN)); } \
TEST(isnan##S##_nnan) { ASSERT_TRUE(isnan(-(T)NAN)); } \
TEST(isnan##S##_div0) { ASSERT_TRUE(isnan((T)0.0/(T)0.0)); } \
TEST(isnan##S##_min) { ASSERT_FALSE(isnan(MIN_VAL)); } \
TEST(isnan##S##_tmin) { ASSERT_FALSE(isnan(TRUE_MIN_VAL)); } \
TEST_SUITE(isinf##S); \
TEST(isinf##S##_norm) { ASSERT_FALSE(isinf((T)1.0)); } \
TEST(isinf##S##_inf) { ASSERT_TRUE(isinf(INF_VAL)); } \
TEST(isinf##S##_ninf) { ASSERT_TRUE(isinf(-INF_VAL)); } \
TEST(isinf##S##_nan) { ASSERT_FALSE(isinf((T)NAN)); } \
TEST(isinf##S##_zero) { ASSERT_FALSE(isinf((T)0.0)); } \
TEST(isinf##S##_max) { ASSERT_FALSE(isinf(MIN_VAL)); } \
TEST_SUITE(isfinite##S); \
TEST(isfinite##S##_norm) { ASSERT_TRUE(isfinite((T)1.0)); } \
TEST(isfinite##S##_zero) { ASSERT_TRUE(isfinite((T)0.0)); } \
TEST(isfinite##S##_nzero) { ASSERT_TRUE(isfinite((T)-0.0)); } \
TEST(isfinite##S##_max) { ASSERT_TRUE(isfinite(MIN_VAL)); } \
TEST(isfinite##S##_inf) { ASSERT_FALSE(isfinite(INF_VAL)); } \
TEST(isfinite##S##_ninf) { ASSERT_FALSE(isfinite(-INF_VAL)); } \
TEST(isfinite##S##_nan) { ASSERT_FALSE(isfinite((T)NAN)); } \
TEST_SUITE(fpclassify##S); \
TEST(fpclassify##S##_zero) { ASSERT_EQ(FP_ZERO, fpclassify((T)0.0)); } \
TEST(fpclassify##S##_nzero) { ASSERT_EQ(FP_ZERO, fpclassify((T)-0.0)); } \
TEST(fpclassify##S##_norm) { ASSERT_EQ(FP_NORMAL, fpclassify((T)1.0)); } \
TEST(fpclassify##S##_inf) { ASSERT_EQ(FP_INFINITE, fpclassify(INF_VAL)); } \
TEST(fpclassify##S##_ninf) { ASSERT_EQ(FP_INFINITE, fpclassify(-INF_VAL)); } \
TEST(fpclassify##S##_nan) { ASSERT_EQ(FP_NAN, fpclassify((T)NAN)); } \
TEST(fpclassify##S##_sub) { ASSERT_EQ(FP_SUBNORMAL, fpclassify(TRUE_MIN_VAL)); } \
TEST_SUITE(signbit##S); \
TEST(signbit##S##_pos) { ASSERT_EQ(0, signbit((T)1.0)); } \
TEST(signbit##S##_neg) { ASSERT_NE(0, signbit((T)-1.0)); } \
TEST(signbit##S##_pzero) { ASSERT_EQ(0, signbit((T)0.0)); } \
TEST(signbit##S##_nzero) { ASSERT_NE(0, signbit((T)-0.0)); } \
TEST(signbit##S##_pinf) { ASSERT_EQ(0, signbit(INF_VAL)); } \
TEST(signbit##S##_ninf) { ASSERT_NE(0, signbit(-INF_VAL)); } \
TEST(signbit##S##_nan) { ASSERT_EQ(0, signbit((T)NAN)); } \
TEST(signbit##S##_nnan) { ASSERT_NE(0, signbit(-(T)NAN)); } \
TEST_SUITE(isnormal##S); \
TEST(isnormal##S##_norm) { ASSERT_TRUE(isnormal((T)1.0)); } \
TEST(isnormal##S##_min) { ASSERT_TRUE(isnormal(MIN_VAL)); } \
TEST(isnormal##S##_zero) { ASSERT_FALSE(isnormal((T)0.0)); } \
TEST(isnormal##S##_nzero) { ASSERT_FALSE(isnormal((T)-0.0)); } \
TEST(isnormal##S##_sub) { ASSERT_FALSE(isnormal(TRUE_MIN_VAL)); } \
TEST(isnormal##S##_inf) { ASSERT_FALSE(isnormal(INF_VAL)); } \
TEST(isnormal##S##_nan) { ASSERT_FALSE(isnormal((T)NAN)); }

#define GEN_RELATIONAL(S, FSUF, T) \
TEST_SUITE(isunordered##S); \
TEST(isunordered##S##_t) { ASSERT_TRUE(isunordered((T)NAN, (T)1.0)); } \
TEST(isunordered##S##_f) { ASSERT_FALSE(isunordered((T)1.0, (T)2.0)); } \
TEST(isunordered##S##_both) { ASSERT_TRUE(isunordered((T)NAN, (T)NAN)); } \
TEST_SUITE(isgreater##S); \
TEST(isgreater##S##_t) { ASSERT_TRUE(isgreater((T)2.0, (T)1.0)); } \
TEST(isgreater##S##_f) { ASSERT_FALSE(isgreater((T)1.0, (T)2.0)); } \
TEST(isgreater##S##_eq) { ASSERT_FALSE(isgreater((T)1.0, (T)1.0)); } \
TEST(isgreater##S##_nan) { ASSERT_FALSE(isgreater((T)NAN, (T)1.0)); } \
TEST_SUITE(isgreaterequal##S); \
TEST(isgreaterequal##S##_t) { ASSERT_TRUE(isgreaterequal((T)2.0, (T)1.0)); } \
TEST(isgreaterequal##S##_eq) { ASSERT_TRUE(isgreaterequal((T)2.0, (T)2.0)); } \
TEST(isgreaterequal##S##_f) { ASSERT_FALSE(isgreaterequal((T)1.0, (T)2.0)); } \
TEST_SUITE(isless##S); \
TEST(isless##S##_t) { ASSERT_TRUE(isless((T)1.0, (T)2.0)); } \
TEST(isless##S##_f) { ASSERT_FALSE(isless((T)2.0, (T)1.0)); } \
TEST(isless##S##_eq) { ASSERT_FALSE(isless((T)1.0, (T)1.0)); } \
TEST(isless##S##_nan) { ASSERT_FALSE(isless((T)NAN, (T)1.0)); } \
TEST_SUITE(islessequal##S); \
TEST(islessequal##S##_t) { ASSERT_TRUE(islessequal((T)1.0, (T)2.0)); } \
TEST(islessequal##S##_eq) { ASSERT_TRUE(islessequal((T)2.0, (T)2.0)); } \
TEST(islessequal##S##_f) { ASSERT_FALSE(islessequal((T)2.0, (T)1.0)); } \
TEST_SUITE(islessgreater##S); \
TEST(islessgreater##S##_t) { ASSERT_TRUE(islessgreater((T)1.0, (T)2.0)); } \
TEST(islessgreater##S##_eq) { ASSERT_FALSE(islessgreater((T)2.0, (T)2.0)); } \
TEST(islessgreater##S##_nan) { ASSERT_FALSE(islessgreater((T)NAN, (T)1.0)); }

#define GEN_FABS_FMOD(S, FSUF, T, E, N, INF_VAL) \
TEST_SUITE(fabs##S); \
TEST(fabs##S##_pos) { N((T)5.0, fabs##FSUF((T)5.0), E); } \
TEST(fabs##S##_neg) { N((T)5.0, fabs##FSUF((T)-5.0), E); } \
TEST(fabs##S##_zero) { N((T)0.0, fabs##FSUF((T)0.0), E); } \
TEST(fabs##S##_nzero) { N((T)0.0, fabs##FSUF((T)-0.0), E); } \
TEST(fabs##S##_inf) { ASSERT_INF(fabs##FSUF(INF_VAL)); } \
TEST(fabs##S##_ninf) { ASSERT_INF(fabs##FSUF(-INF_VAL)); } \
TEST(fabs##S##_nan) { ASSERT_NAN(fabs##FSUF((T)NAN)); } \
TEST_SUITE(fmod##S); \
TEST(fmod##S##_basic) { N((T)1.0, fmod##FSUF((T)7.0, (T)3.0), E); } \
TEST(fmod##S##_exact) { N((T)0.0, fmod##FSUF((T)8.0, (T)4.0), E); } \
TEST(fmod##S##_frac) { N((T)0.5, fmod##FSUF((T)5.5, (T)1.0), E); } \
TEST(fmod##S##_ndiv) { N((T)-1.0, fmod##FSUF((T)-7.0, (T)3.0), E); } \
TEST(fmod##S##_ndivs) { N((T)1.0, fmod##FSUF((T)7.0, (T)-3.0), E); } \
TEST(fmod##S##_zdiv) { ASSERT_NAN(fmod##FSUF((T)1.0, (T)0.0)); } \
TEST(fmod##S##_nan) { ASSERT_NAN(fmod##FSUF((T)NAN, (T)1.0)); } \
TEST(fmod##S##_inf) { ASSERT_NAN(fmod##FSUF(INF_VAL, (T)1.0)); } \
TEST(fmod##S##_zero) { N((T)0.0, fmod##FSUF((T)0.0, (T)1.0), E); } \
TEST(fmod##S##_neg_pos) { N((T)-0.5, fmod##FSUF((T)-1.5, (T)1.0), E); } \
TEST(fmod##S##_pos_neg) { N((T)0.5, fmod##FSUF((T)1.5, (T)-1.0), E); }

#define GEN_CEIL_FLOOR(S, FSUF, T, E, N, INF_VAL) \
TEST_SUITE(ceil##S); \
TEST(ceil##S##_pos) { N((T)3.0, ceil##FSUF((T)2.1), E); } \
TEST(ceil##S##_exact) { N((T)3.0, ceil##FSUF((T)3.0), E); } \
TEST(ceil##S##_neg) { N((T)-2.0, ceil##FSUF((T)-2.9), E); } \
TEST(ceil##S##_zero) { N((T)0.0, ceil##FSUF((T)0.0), E); } \
TEST(ceil##S##_nzero) { N((T)-0.0, ceil##FSUF((T)-0.0), E); } \
TEST(ceil##S##_inf) { ASSERT_INF(ceil##FSUF(INF_VAL)); } \
TEST(ceil##S##_ninf) { ASSERT_INF(ceil##FSUF(-INF_VAL)); } \
TEST(ceil##S##_nan) { ASSERT_NAN(ceil##FSUF((T)NAN)); } \
TEST_SUITE(floor##S); \
TEST(floor##S##_pos) { N((T)2.0, floor##FSUF((T)2.9), E); } \
TEST(floor##S##_exact) { N((T)3.0, floor##FSUF((T)3.0), E); } \
TEST(floor##S##_neg) { N((T)-3.0, floor##FSUF((T)-2.1), E); } \
TEST(floor##S##_zero) { N((T)0.0, floor##FSUF((T)0.0), E); } \
TEST(floor##S##_nzero) { N((T)-0.0, floor##FSUF((T)-0.0), E); } \
TEST(floor##S##_inf) { ASSERT_INF(floor##FSUF(INF_VAL)); } \
TEST(floor##S##_ninf) { ASSERT_INF(floor##FSUF(-INF_VAL)); } \
TEST(floor##S##_nan) { ASSERT_NAN(floor##FSUF((T)NAN)); }

#define GEN_EXP_LOG_BASE(S, FSUF, T, E, N, INF_VAL, LOG_MAX, LOG_TRUE_MIN, MAX_EXP, MANT_DIG, MIN_EXP, TRUE_MIN_VAL) \
TEST_SUITE(exp##S); \
TEST(exp##S##_zero) { N((T)1.0, exp##FSUF((T)0.0), E); } \
TEST(exp##S##_nzero) { N((T)1.0, exp##FSUF((T)-0.0), E); } \
TEST(exp##S##_one) { N((T)2.71828, exp##FSUF((T)1.0), E); } \
TEST(exp##S##_neg) { N((T)0.36788, exp##FSUF((T)-1.0), E); } \
TEST(exp##S##_over) { ASSERT_INF(exp##FSUF((T)LOG_MAX)); } \
TEST(exp##S##_under) { N((T)0.0, exp##FSUF((T)LOG_TRUE_MIN), E); } \
TEST(exp##S##_inf) { ASSERT_INF(exp##FSUF(INF_VAL)); } \
TEST(exp##S##_ninf) { N((T)0.0, exp##FSUF(-INF_VAL), E); } \
TEST(exp##S##_nan) { ASSERT_NAN(exp##FSUF((T)NAN)); } \
TEST(exp##S##_large) { ASSERT_INF(exp##FSUF((T)(LOG_MAX + 1.0))); } \
TEST_SUITE(ldexp##S); \
TEST(ldexp##S##_basic) { N((T)8.0, ldexp##FSUF((T)1.0, 3), E); } \
TEST(ldexp##S##_nexp) { N((T)0.5, ldexp##FSUF((T)1.0, -1), E); } \
TEST(ldexp##S##_zero) { N((T)0.0, ldexp##FSUF((T)0.0, 10), E); } \
TEST(ldexp##S##_inf) { ASSERT_INF(ldexp##FSUF(INF_VAL, 1)); } \
TEST(ldexp##S##_ninf) { ASSERT_INF(ldexp##FSUF(-INF_VAL, 1)); } \
TEST(ldexp##S##_nan) { ASSERT_NAN(ldexp##FSUF((T)NAN, 1)); } \
TEST(ldexp##S##_over) { ASSERT_INF(ldexp##FSUF((T)1.0, MAX_EXP + 1)); } \
TEST_SUITE(frexp##S); \
TEST(frexp##S##_eight) { int e; T m = frexp##FSUF((T)8.0, &e); N((T)0.5, m, E); ASSERT_EQ(4, e); } \
TEST(frexp##S##_half) { int e; T m = frexp##FSUF((T)0.5, &e); N((T)0.5, m, E); ASSERT_EQ(0, e); } \
TEST(frexp##S##_zero) { int e; T m = frexp##FSUF((T)0.0, &e); N((T)0.0, m, E); ASSERT_EQ(0, e); } \
TEST(frexp##S##_nzero) { int e; T m = frexp##FSUF((T)-0.0, &e); N((T)-0.0, m, E); ASSERT_EQ(0, e); } \
TEST(frexp##S##_inf) { int e; ASSERT_INF(frexp##FSUF(INF_VAL, &e)); } \
TEST(frexp##S##_nan) { int e; ASSERT_NAN(frexp##FSUF((T)NAN, &e)); } \
TEST_SUITE(log##S); \
TEST(log##S##_one) { N((T)0.0, log##FSUF((T)1.0), E); } \
TEST(log##S##_e) { N((T)1.0, log##FSUF((T)M_E), E); } \
TEST(log##S##_ten) { N((T)2.30259, log##FSUF((T)10.0), E); } \
TEST(log##S##_neg) { ASSERT_NAN(log##FSUF((T)-1.0)); } \
TEST(log##S##_zero) { ASSERT_INF(log##FSUF((T)0.0)); } \
TEST(log##S##_nzero) { ASSERT_INF(log##FSUF((T)-0.0)); } \
TEST(log##S##_inf) { ASSERT_INF(log##FSUF(INF_VAL)); } \
TEST(log##S##_ninf) { ASSERT_NAN(log##FSUF(-INF_VAL)); } \
TEST(log##S##_nan) { ASSERT_NAN(log##FSUF((T)NAN)); } \
TEST(log##S##_sub) { ASSERT_TRUE(log##FSUF(TRUE_MIN_VAL) <= LOG_TRUE_MIN); } \
TEST_SUITE(log10##S); \
TEST(log10##S##_one) { N((T)0.0, log10##FSUF((T)1.0), E); } \
TEST(log10##S##_ten) { N((T)1.0, log10##FSUF((T)10.0), E); } \
TEST(log10##S##_hun) { N((T)2.0, log10##FSUF((T)100.0), E); } \
TEST(log10##S##_neg) { ASSERT_NAN(log10##FSUF((T)-1.0)); } \
TEST(log10##S##_zero) { ASSERT_INF(log10##FSUF((T)0.0)); } \
TEST(log10##S##_nan) { ASSERT_NAN(log10##FSUF((T)NAN)); } \
TEST_SUITE(sqrt##S); \
TEST(sqrt##S##_four) { N((T)2.0, sqrt##FSUF((T)4.0), E); } \
TEST(sqrt##S##_nine) { N((T)3.0, sqrt##FSUF((T)9.0), E); } \
TEST(sqrt##S##_zero) { N((T)0.0, sqrt##FSUF((T)0.0), E); } \
TEST(sqrt##S##_nzero) { N((T)-0.0, sqrt##FSUF((T)-0.0), E); } \
TEST(sqrt##S##_neg) { ASSERT_NAN(sqrt##FSUF((T)-1.0)); } \
TEST(sqrt##S##_inf) { ASSERT_INF(sqrt##FSUF(INF_VAL)); } \
TEST(sqrt##S##_ninf) { ASSERT_NAN(sqrt##FSUF(-INF_VAL)); } \
TEST(sqrt##S##_nan) { ASSERT_NAN(sqrt##FSUF((T)NAN)); }

#define GEN_POW_TRIG_BASE(S, FSUF, T, E, N, INF_VAL, MAX_10_EXP, MIN_10_EXP, TRUE_MIN_VAL) \
TEST_SUITE(pow##S); \
TEST(pow##S##_basic) { N((T)8.0, pow##FSUF((T)2.0, (T)3.0), E); } \
TEST(pow##S##_zexp) { N((T)1.0, pow##FSUF((T)5.0, (T)0.0), E); } \
TEST(pow##S##_nexp) { N((T)0.25, pow##FSUF((T)2.0, (T)-2.0), E); } \
TEST(pow##S##_frac) { N((T)2.0, pow##FSUF((T)4.0, (T)0.5), E); } \
TEST(pow##S##_nfrac) { ASSERT_NAN(pow##FSUF((T)-1.0, (T)0.5)); } \
TEST(pow##S##_zz) { N((T)1.0, pow##FSUF((T)0.0, (T)0.0), E); } \
TEST(pow##S##_z_neg) { ASSERT_INF(pow##FSUF((T)0.0, (T)-1.0)); } \
TEST(pow##S##_one_nan) { N((T)1.0, pow##FSUF((T)1.0, (T)NAN), E); } \
TEST(pow##S##_nan_zero) { N((T)1.0, pow##FSUF((T)NAN, (T)0.0), E); } \
TEST(pow##S##_over) { ASSERT_INF(pow##FSUF((T)10.0, (T)(MAX_10_EXP + 1))); } \
TEST(pow##S##_under) { N((T)0.0, pow##FSUF((T)10.0, (T)(MIN_10_EXP - 1)), 0.0); } \
TEST(pow##S##_nanb) { ASSERT_NAN(pow##FSUF((T)NAN, (T)2.0)); } \
TEST(pow##S##_nane) { ASSERT_NAN(pow##FSUF((T)2.0, (T)NAN)); } \
TEST(pow##S##_0_ninf) { ASSERT_INF(pow##FSUF((T)0.0, -INF_VAL)); } \
TEST(pow##S##_0_pinf) { N((T)0.0, pow##FSUF((T)0.0, INF_VAL), E); } \
TEST(pow##S##_1_inf) { N((T)1.0, pow##FSUF((T)1.0, INF_VAL), E); } \
TEST(pow##S##_inf_0) { N((T)1.0, pow##FSUF(INF_VAL, (T)0.0), E); } \
TEST(pow##S##_neg_2_2) { N((T)4.0, pow##FSUF((T)-2.0, (T)2.0), E); } \
TEST(pow##S##_neg_2_3) { N((T)-8.0, pow##FSUF((T)-2.0, (T)3.0), E); } \
TEST_SUITE(sin##S); \
TEST(sin##S##_zero) { N((T)0.0, sin##FSUF((T)0.0), E); } \
TEST(sin##S##_nzero) { N((T)-0.0, sin##FSUF((T)-0.0), E); } \
TEST(sin##S##_pi2) { N((T)1.0, sin##FSUF((T)M_PI_2), E); } \
TEST(sin##S##_pi) { N((T)0.0, sin##FSUF((T)M_PI), E); } \
TEST(sin##S##_neg) { N((T)-1.0, sin##FSUF((T)3.0 * M_PI_2), E); } \
TEST(sin##S##_inf) { ASSERT_NAN(sin##FSUF(INF_VAL)); } \
TEST(sin##S##_ninf) { ASSERT_NAN(sin##FSUF(-INF_VAL)); } \
TEST(sin##S##_nan) { ASSERT_NAN(sin##FSUF((T)NAN)); } \
TEST(sin##S##_large) { ASSERT_TRUE(fabs##FSUF(sin##FSUF((T)1e20)) <= 1.0); } \
TEST_SUITE(cos##S); \
TEST(cos##S##_zero) { N((T)1.0, cos##FSUF((T)0.0), E); } \
TEST(cos##S##_nzero) { N((T)1.0, cos##FSUF((T)-0.0), E); } \
TEST(cos##S##_pi2) { N((T)0.0, cos##FSUF((T)M_PI_2), E); } \
TEST(cos##S##_pi) { N((T)-1.0, cos##FSUF((T)M_PI), E); } \
TEST(cos##S##_inf) { ASSERT_NAN(cos##FSUF(INF_VAL)); } \
TEST(cos##S##_nan) { ASSERT_NAN(cos##FSUF((T)NAN)); } \
TEST(cos##S##_large) { ASSERT_TRUE(fabs##FSUF(cos##FSUF((T)1e20)) <= 1.0); } \
TEST_SUITE(tan##S); \
TEST(tan##S##_zero) { N((T)0.0, tan##FSUF((T)0.0), E); } \
TEST(tan##S##_nzero) { N((T)-0.0, tan##FSUF((T)-0.0), E); } \
TEST(tan##S##_pi4) { N((T)1.0, tan##FSUF((T)M_PI_4), E); } \
TEST(tan##S##_pi) { N((T)0.0, tan##FSUF((T)M_PI), E); } \
TEST(tan##S##_inf) { ASSERT_NAN(tan##FSUF(INF_VAL)); } \
TEST(tan##S##_nan) { ASSERT_NAN(tan##FSUF((T)NAN)); } \
TEST(tan##S##_pi2) { ASSERT_TRUE(fabs##FSUF(tan##FSUF((T)M_PI_2)) > (T)1e6); } \
TEST_SUITE(asin##S); \
TEST(asin##S##_zero) { N((T)0.0, asin##FSUF((T)0.0), E); } \
TEST(asin##S##_nzero) { N((T)-0.0, asin##FSUF((T)-0.0), E); } \
TEST(asin##S##_one) { N((T)M_PI_2, asin##FSUF((T)1.0), E); } \
TEST(asin##S##_none) { N((T)-M_PI_2, asin##FSUF((T)-1.0), E); } \
TEST(asin##S##_half) { N((T)M_PI / 6.0, asin##FSUF((T)0.5), E); } \
TEST(asin##S##_dom) { ASSERT_NAN(asin##FSUF((T)2.0)); } \
TEST(asin##S##_nan) { ASSERT_NAN(asin##FSUF((T)NAN)); } \
TEST_SUITE(acos##S); \
TEST(acos##S##_zero) { N((T)M_PI_2, acos##FSUF((T)0.0), E); } \
TEST(acos##S##_one) { N((T)0.0, acos##FSUF((T)1.0), E); } \
TEST(acos##S##_none) { N((T)M_PI, acos##FSUF((T)-1.0), E); } \
TEST(acos##S##_dom) { ASSERT_NAN(acos##FSUF((T)2.0)); } \
TEST(acos##S##_nan) { ASSERT_NAN(acos##FSUF((T)NAN)); } \
TEST_SUITE(atan##S); \
TEST(atan##S##_zero) { N((T)0.0, atan##FSUF((T)0.0), E); } \
TEST(atan##S##_nzero) { N((T)-0.0, atan##FSUF((T)-0.0), E); } \
TEST(atan##S##_one) { N((T)M_PI_4, atan##FSUF((T)1.0), E); } \
TEST(atan##S##_none) { N((T)-M_PI_4, atan##FSUF((T)-1.0), E); } \
TEST(atan##S##_inf) { N((T)M_PI_2, atan##FSUF(INF_VAL), E); } \
TEST(atan##S##_ninf) { N((T)-M_PI_2, atan##FSUF(-INF_VAL), E); } \
TEST(atan##S##_nan) { ASSERT_NAN(atan##FSUF((T)NAN)); } \
TEST_SUITE(atan2##S); \
TEST(atan2##S##_quad) { N((T)M_PI_4, atan2##FSUF((T)1.0, (T)1.0), E); N((T)3.0 * M_PI_4, atan2##FSUF((T)1.0, (T)-1.0), E); } \
TEST(atan2##S##_zx) { N((T)M_PI_2, atan2##FSUF((T)1.0, (T)0.0), E); } \
TEST(atan2##S##_zy) { N((T)0.0, atan2##FSUF((T)0.0, (T)1.0), E); } \
TEST(atan2##S##_zz) { N((T)0.0, atan2##FSUF((T)0.0, (T)0.0), E); } \
TEST(atan2##S##_nzy) { N((T)-0.0, atan2##FSUF((T)-0.0, (T)1.0), E); } \
TEST(atan2##S##_0_n0) { N((T)M_PI, atan2##FSUF((T)0.0, (T)-0.0), E); } \
TEST(atan2##S##_n0_0) { N((T)-0.0, atan2##FSUF((T)-0.0, (T)0.0), E); } \
TEST(atan2##S##_n0_n0) { N((T)-M_PI, atan2##FSUF((T)-0.0, (T)-0.0), E); } \
TEST(atan2##S##_pos_0) { N((T)M_PI_2, atan2##FSUF((T)1.0, (T)0.0), E); } \
TEST(atan2##S##_neg_0) { N((T)-M_PI_2, atan2##FSUF((T)-1.0, (T)0.0), E); }

#define GEN_HYPER_BASE(S, FSUF, T, E, N, INF_VAL, LOG_MAX) \
TEST_SUITE(sinh##S); \
TEST(sinh##S##_zero) { N((T)0.0, sinh##FSUF((T)0.0), E); } \
TEST(sinh##S##_nzero) { N((T)-0.0, sinh##FSUF((T)-0.0), E); } \
TEST(sinh##S##_one) { N((T)1.17520, sinh##FSUF((T)1.0), E); } \
TEST(sinh##S##_none) { N((T)-1.17520, sinh##FSUF((T)-1.0), E); } \
TEST(sinh##S##_over) { ASSERT_INF(sinh##FSUF((T)(LOG_MAX + 1.0))); } \
TEST(sinh##S##_inf) { ASSERT_INF(sinh##FSUF(INF_VAL)); } \
TEST(sinh##S##_ninf) { ASSERT_INF(sinh##FSUF(-INF_VAL)); } \
TEST(sinh##S##_nan) { ASSERT_NAN(sinh##FSUF((T)NAN)); } \
TEST(sinh##S##_large) { ASSERT_INF(sinh##FSUF((T)(LOG_MAX + 1.0))); } \
TEST_SUITE(cosh##S); \
TEST(cosh##S##_zero) { N((T)1.0, cosh##FSUF((T)0.0), E); } \
TEST(cosh##S##_nzero) { N((T)1.0, cosh##FSUF((T)-0.0), E); } \
TEST(cosh##S##_one) { N((T)1.54308, cosh##FSUF((T)1.0), E); } \
TEST(cosh##S##_none) { N((T)1.54308, cosh##FSUF((T)-1.0), E); } \
TEST(cosh##S##_over) { ASSERT_INF(cosh##FSUF((T)(LOG_MAX + 1.0))); } \
TEST(cosh##S##_inf) { ASSERT_INF(cosh##FSUF(INF_VAL)); } \
TEST(cosh##S##_nan) { ASSERT_NAN(cosh##FSUF((T)NAN)); } \
TEST(cosh##S##_large) { ASSERT_INF(cosh##FSUF((T)(LOG_MAX + 1.0))); } \
TEST_SUITE(tanh##S); \
TEST(tanh##S##_zero) { N((T)0.0, tanh##FSUF((T)0.0), E); } \
TEST(tanh##S##_nzero) { N((T)-0.0, tanh##FSUF((T)-0.0), E); } \
TEST(tanh##S##_one) { N((T)0.76159, tanh##FSUF((T)1.0), E); } \
TEST(tanh##S##_none) { N((T)-0.76159, tanh##FSUF((T)-1.0), E); } \
TEST(tanh##S##_large) { N((T)1.0, tanh##FSUF((T)100.0), E); } \
TEST(tanh##S##_inf) { N((T)1.0, tanh##FSUF(INF_VAL), E); } \
TEST(tanh##S##_ninf) { N((T)-1.0, tanh##FSUF(-INF_VAL), E); } \
TEST(tanh##S##_nan) { ASSERT_NAN(tanh##FSUF((T)NAN)); } \
TEST(tanh##S##_vlarge) { ASSERT_EQ((T)1.0, tanh##FSUF(INF_VAL)); } \
TEST(tanh##S##_nvlarge) { ASSERT_EQ((T)-1.0, tanh##FSUF(-INF_VAL)); }

#define GEN_MODF_BASE(S, FSUF, T, E, N) \
TEST_SUITE(modf##S); \
TEST(modf##S##_basic) { T i; T f = modf##FSUF((T)3.14, &i); N((T)3.0, i, E); N((T)0.14, f, 0.01); } \
TEST(modf##S##_neg) { T i; T f = modf##FSUF((T)-2.7, &i); N((T)-2.0, i, E); N((T)-0.7, f, 0.01); } \
TEST(modf##S##_exact) { T i; T f = modf##FSUF((T)5.0, &i); N((T)5.0, i, E); N((T)0.0, f, E); } \
TEST(modf##S##_zero) { T i; T f = modf##FSUF((T)0.0, &i); N((T)0.0, i, E); N((T)0.0, f, E); } \
TEST(modf##S##_nzero) { T i; T f = modf##FSUF((T)-0.0, &i); N((T)-0.0, i, E); N((T)-0.0, f, E); } \
TEST(modf##S##_inf) { T i; T f = modf##FSUF(INFINITY, &i); ASSERT_INF(i); N((T)0.0, f, E); }

/* =========================================
 * C99 MACROS
 * ========================================= */
#if JACL_HAS_C99

#define GEN_C99_ROUNDING(S, FSUF, T, E, N, INF_VAL) \
TEST_SUITE(trunc##S); \
TEST(trunc##S##_pos) { N((T)2.0, trunc##FSUF((T)2.7), E); } \
TEST(trunc##S##_neg) { N((T)-2.0, trunc##FSUF((T)-2.7), E); } \
TEST(trunc##S##_half) { N((T)0.0, trunc##FSUF((T)0.5), E); } \
TEST(trunc##S##_zero) { N((T)0.0, trunc##FSUF((T)0.0), E); } \
TEST(trunc##S##_nzero) { N((T)-0.0, trunc##FSUF((T)-0.0), E); } \
TEST(trunc##S##_inf) { ASSERT_INF(trunc##FSUF(INF_VAL)); } \
TEST(trunc##S##_ninf) { ASSERT_INF(trunc##FSUF(-INF_VAL)); } \
TEST(trunc##S##_nan) { ASSERT_NAN(trunc##FSUF((T)NAN)); } \
TEST(trunc##S##_small) { N((T)0.0, trunc##FSUF((T)0.1), E); } \
TEST(trunc##S##_nsmall) { N((T)-0.0, trunc##FSUF((T)-0.1), E); } \
TEST_SUITE(round##S); \
TEST(round##S##_hpos) { N((T)3.0, round##FSUF((T)2.5), E); } \
TEST(round##S##_hneg) { N((T)-3.0, round##FSUF((T)-2.5), E); } \
TEST(round##S##_below) { N((T)2.0, round##FSUF((T)2.4), E); } \
TEST(round##S##_above) { N((T)3.0, round##FSUF((T)2.6), E); } \
TEST(round##S##_zero) { N((T)0.0, round##FSUF((T)0.0), E); } \
TEST(round##S##_inf) { ASSERT_INF(round##FSUF(INF_VAL)); } \
TEST(round##S##_nan) { ASSERT_NAN(round##FSUF((T)NAN)); } \
TEST_SUITE(lround##S); \
TEST(lround##S##_half) { ASSERT_EQ(3L, lround##FSUF((T)2.5)); } \
TEST(lround##S##_neg) { ASSERT_EQ(-3L, lround##FSUF((T)-2.5)); } \
TEST(lround##S##_zero) { ASSERT_EQ(0L, lround##FSUF((T)0.0)); } \
TEST_SUITE(llround##S); \
TEST(llround##S##_half) { ASSERT_EQ(3LL, llround##FSUF((T)2.5)); } \
TEST(llround##S##_neg) { ASSERT_EQ(-3LL, llround##FSUF((T)-2.5)); } \
TEST_SUITE(rint##S); \
TEST(rint##S##_heven) { N((T)2.0, rint##FSUF((T)2.5), E); N((T)4.0, rint##FSUF((T)3.5), E); } \
TEST(rint##S##_below) { N((T)2.0, rint##FSUF((T)2.4), E); } \
TEST(rint##S##_above) { N((T)3.0, rint##FSUF((T)2.6), E); } \
TEST(rint##S##_zero) { N((T)0.0, rint##FSUF((T)0.0), E); } \
TEST(rint##S##_inf) { ASSERT_INF(rint##FSUF(INF_VAL)); } \
TEST(rint##S##_nan) { ASSERT_NAN(rint##FSUF((T)NAN)); } \
TEST(rint##S##_max) { ASSERT_EQ((T)LONG_MAX, rint##FSUF((T)LONG_MAX)); } \
TEST_SUITE(lrint##S); \
TEST(lrint##S##_exact) { ASSERT_EQ(3L, lrint##FSUF((T)3.0)); } \
TEST(lrint##S##_neg) { ASSERT_EQ(-5L, lrint##FSUF((T)-5.0)); } \
TEST_SUITE(llrint##S); \
TEST(llrint##S##_exact) { ASSERT_EQ(100LL, llrint##FSUF((T)100.0)); } \
TEST_SUITE(nearbyint##S); \
TEST(nearbyint##S##_heven) { N((T)2.0, nearbyint##FSUF((T)2.5), E); N((T)4.0, nearbyint##FSUF((T)3.5), E); } \
TEST(nearbyint##S##_zero) { N((T)0.0, nearbyint##FSUF((T)0.0), E); } \
TEST(nearbyint##S##_inf) { ASSERT_INF(nearbyint##FSUF(INF_VAL)); } \
TEST(nearbyint##S##_nan) { ASSERT_NAN(nearbyint##FSUF((T)NAN)); } \
TEST(nearbyint##S##_max) { ASSERT_EQ((T)LONG_MAX, nearbyint##FSUF((T)LONG_MAX)); }

#define GEN_C99_EXP_LOG(S, FSUF, T, E, N, INF_VAL, LOG_MAX, LOG_TRUE_MIN, MAX_EXP, MANT_DIG, MIN_EXP) \
TEST_SUITE(exp2##S); \
TEST(exp2##S##_zero) { N((T)1.0, exp2##FSUF((T)0.0), E); } \
TEST(exp2##S##_one) { N((T)2.0, exp2##FSUF((T)1.0), E); } \
TEST(exp2##S##_three) { N((T)8.0, exp2##FSUF((T)3.0), E); } \
TEST(exp2##S##_neg) { N((T)0.5, exp2##FSUF((T)-1.0), E); } \
TEST(exp2##S##_over) { ASSERT_INF(exp2##FSUF((T)MAX_EXP)); } \
TEST(exp2##S##_under) { N((T)0.0, exp2##FSUF((T)(MIN_EXP - MANT_DIG - 1)), 0.0); } \
TEST(exp2##S##_nan) { ASSERT_NAN(exp2##FSUF((T)NAN)); } \
TEST(exp2##S##_large) { ASSERT_INF(exp2##FSUF((T)(MAX_EXP + 1))); } \
TEST_SUITE(exp10##S); \
TEST(exp10##S##_zero) { N((T)1.0, exp10##FSUF((T)0.0), E); } \
TEST(exp10##S##_one) { N((T)10.0, exp10##FSUF((T)1.0), E); } \
TEST_SUITE(expm1##S); \
TEST(expm1##S##_zero) { N((T)0.0, expm1##FSUF((T)0.0), E); } \
TEST(expm1##S##_one) { N((T)1.71828, expm1##FSUF((T)1.0), E); } \
TEST(expm1##S##_small) { N((T)0.01005, expm1##FSUF((T)0.01), T_EPS); } \
TEST(expm1##S##_nan) { ASSERT_NAN(expm1##FSUF((T)NAN)); } \
TEST(expm1##S##_tiny) { ASSERT_TRUE(fabs##FSUF(expm1##FSUF((T)1e-5) - (T)1e-5) < (T)1e-9); } \
TEST_SUITE(log2##S); \
TEST(log2##S##_one) { N((T)0.0, log2##FSUF((T)1.0), E); } \
TEST(log2##S##_two) { N((T)1.0, log2##FSUF((T)2.0), E); } \
TEST(log2##S##_eight) { N((T)3.0, log2##FSUF((T)8.0), E); } \
TEST(log2##S##_half) { N((T)-1.0, log2##FSUF((T)0.5), E); } \
TEST(log2##S##_neg) { ASSERT_NAN(log2##FSUF((T)-1.0)); } \
TEST(log2##S##_zero) { ASSERT_INF(log2##FSUF((T)0.0)); } \
TEST(log2##S##_nan) { ASSERT_NAN(log2##FSUF((T)NAN)); } \
TEST_SUITE(logb##S); \
TEST(logb##S##_eight) { N((T)3.0, logb##FSUF((T)8.0), E); } \
TEST(logb##S##_seven) { N((T)2.0, logb##FSUF((T)7.0), E); } \
TEST(logb##S##_half) { N((T)-1.0, logb##FSUF((T)0.5), E); } \
TEST(logb##S##_zero) { ASSERT_INF(logb##FSUF((T)0.0)); } \
TEST(logb##S##_inf) { ASSERT_INF(logb##FSUF(INF_VAL)); } \
TEST(logb##S##_nan) { ASSERT_NAN(logb##FSUF((T)NAN)); } \
TEST_SUITE(ilogb##S); \
TEST(ilogb##S##_eight) { ASSERT_EQ(3, ilogb##FSUF((T)8.0)); } \
TEST(ilogb##S##_seven) { ASSERT_EQ(2, ilogb##FSUF((T)7.0)); } \
TEST(ilogb##S##_half) { ASSERT_EQ(-1, ilogb##FSUF((T)0.5)); } \
TEST(ilogb##S##_zero) { ASSERT_EQ(FP_ILOGB0, ilogb##FSUF((T)0.0)); } \
TEST(ilogb##S##_nan) { ASSERT_EQ(FP_ILOGBNAN, ilogb##FSUF((T)NAN)); } \
TEST_SUITE(log1p##S); \
TEST(log1p##S##_zero) { N((T)0.0, log1p##FSUF((T)0.0), E); } \
TEST(log1p##S##_one) { N((T)0.69315, log1p##FSUF((T)1.0), E); } \
TEST(log1p##S##_small) { N((T)0.00995, log1p##FSUF((T)0.01), T_EPS); } \
TEST(log1p##S##_nan) { ASSERT_NAN(log1p##FSUF((T)NAN)); }

#define GEN_C99_ROOTS(S, FSUF, T, E, N, INF_VAL, TRUE_MIN_VAL) \
TEST_SUITE(cbrt##S); \
TEST(cbrt##S##_eight) { N((T)2.0, cbrt##FSUF((T)8.0), E); } \
TEST(cbrt##S##_neg) { N((T)-2.0, cbrt##FSUF((T)-8.0), E); } \
TEST(cbrt##S##_zero) { N((T)0.0, cbrt##FSUF((T)0.0), E); } \
TEST(cbrt##S##_inf) { ASSERT_INF(cbrt##FSUF(INF_VAL)); } \
TEST(cbrt##S##_ninf) { ASSERT_INF(cbrt##FSUF(-INF_VAL)); } \
TEST(cbrt##S##_nan) { ASSERT_NAN(cbrt##FSUF((T)NAN)); } \
TEST_SUITE(hypot##S); \
TEST(hypot##S##_345) { N((T)5.0, hypot##FSUF((T)3.0, (T)4.0), E); } \
TEST(hypot##S##_512) { N((T)13.0, hypot##FSUF((T)5.0, (T)12.0), E); } \
TEST(hypot##S##_zero) { N((T)1.0, hypot##FSUF((T)1.0, (T)0.0), E); } \
TEST(hypot##S##_inf) { ASSERT_INF(hypot##FSUF(INF_VAL, (T)1.0)); } \
TEST(hypot##S##_ninf) { ASSERT_INF(hypot##FSUF(-INF_VAL, (T)1.0)); } \
TEST(hypot##S##_nan) { ASSERT_NAN(hypot##FSUF((T)NAN, (T)1.0)); }

#define GEN_C99_MINMAX_FMA(S, FSUF, T, E, N, INF_VAL) \
TEST_SUITE(fdim##S); \
TEST(fdim##S##_pos) { N((T)3.0, fdim##FSUF((T)5.0, (T)2.0), E); } \
TEST(fdim##S##_neg) { N((T)0.0, fdim##FSUF((T)2.0, (T)5.0), E); } \
TEST(fdim##S##_eq) { N((T)0.0, fdim##FSUF((T)3.0, (T)3.0), E); } \
TEST(fdim##S##_nan) { ASSERT_NAN(fdim##FSUF((T)NAN, (T)1.0)); } \
TEST_SUITE(fmax##S); \
TEST(fmax##S##_basic) { N((T)5.0, fmax##FSUF((T)3.0, (T)5.0), E); } \
TEST(fmax##S##_rev) { N((T)5.0, fmax##FSUF((T)5.0, (T)3.0), E); } \
TEST(fmax##S##_nan1) { N((T)3.0, fmax##FSUF((T)NAN, (T)3.0), E); } \
TEST(fmax##S##_nan2) { N((T)3.0, fmax##FSUF((T)3.0, (T)NAN), E); } \
TEST(fmax##S##_nann) { ASSERT_NAN(fmax##FSUF((T)NAN, (T)NAN)); } \
TEST_SUITE(fmin##S); \
TEST(fmin##S##_basic) { N((T)3.0, fmin##FSUF((T)3.0, (T)5.0), E); } \
TEST(fmin##S##_rev) { N((T)3.0, fmin##FSUF((T)5.0, (T)3.0), E); } \
TEST(fmin##S##_nan1) { N((T)3.0, fmin##FSUF((T)NAN, (T)3.0), E); } \
TEST(fmin##S##_nan2) { N((T)3.0, fmin##FSUF((T)3.0, (T)NAN), E); } \
TEST(fmin##S##_nann) { ASSERT_NAN(fmin##FSUF((T)NAN, (T)NAN)); } \
TEST_SUITE(fma##S); \
TEST(fma##S##_basic) { N((T)11.0, fma##FSUF((T)2.0, (T)3.0, (T)5.0), E); } \
TEST(fma##S##_zmult) { N((T)5.0, fma##FSUF((T)0.0, (T)10.0, (T)5.0), E); } \
TEST(fma##S##_nan) { ASSERT_NAN(fma##FSUF((T)NAN, (T)1.0, (T)1.0)); }

#define GEN_C99_SCALING(S, FSUF, T, E, N, INF_VAL) \
TEST_SUITE(scalbn##S); \
TEST(scalbn##S##_basic) { N((T)8.0, scalbn##FSUF((T)1.0, 3), E); } \
TEST(scalbn##S##_neg) { N((T)0.25, scalbn##FSUF((T)1.0, -2), E); } \
TEST(scalbn##S##_zero) { N((T)0.0, scalbn##FSUF((T)0.0, 5), E); } \
TEST(scalbn##S##_inf) { ASSERT_INF(scalbn##FSUF(INF_VAL, 1)); } \
TEST(scalbn##S##_nan) { ASSERT_NAN(scalbn##FSUF((T)NAN, 1)); } \
TEST_SUITE(scalbln##S); \
TEST(scalbln##S##_basic) { N((T)16.0, scalbln##FSUF((T)1.0, 4L), E); } \
TEST(scalbln##S##_neg) { N((T)0.125, scalbln##FSUF((T)1.0, -3L), E); } \
TEST(scalbln##S##_zero) { N((T)0.0, scalbln##FSUF((T)0.0, 5L), E); }

#define GEN_C99_HYPER_INV(S, FSUF, T, E, N, INF_VAL, LOG_MAX) \
TEST_SUITE(asinh##S); \
TEST(asinh##S##_zero) { N((T)0.0, asinh##FSUF((T)0.0), E); } \
TEST(asinh##S##_one) { N((T)0.88137, asinh##FSUF((T)1.0), E); } \
TEST(asinh##S##_none) { N((T)-0.88137, asinh##FSUF((T)-1.0), E); } \
TEST(asinh##S##_inf) { ASSERT_INF(asinh##FSUF(INF_VAL)); } \
TEST(asinh##S##_ninf) { ASSERT_INF(asinh##FSUF(-INF_VAL)); } \
TEST(asinh##S##_nan) { ASSERT_NAN(asinh##FSUF((T)NAN)); } \
TEST(asinh##S##_large) { ASSERT_INF(asinh##FSUF(INF_VAL)); } \
TEST_SUITE(acosh##S); \
TEST(acosh##S##_one) { N((T)0.0, acosh##FSUF((T)1.0), E); } \
TEST(acosh##S##_two) { N((T)1.31696, acosh##FSUF((T)2.0), E); } \
TEST(acosh##S##_dom) { ASSERT_NAN(acosh##FSUF((T)0.5)); } \
TEST(acosh##S##_inf) { ASSERT_INF(acosh##FSUF(INF_VAL)); } \
TEST(acosh##S##_ninf) { ASSERT_NAN(acosh##FSUF(-INF_VAL)); } \
TEST(acosh##S##_nan) { ASSERT_NAN(acosh##FSUF((T)NAN)); } \
TEST(acosh##S##_large) { ASSERT_INF(acosh##FSUF(INF_VAL)); } \
TEST_SUITE(atanh##S); \
TEST(atanh##S##_zero) { N((T)0.0, atanh##FSUF((T)0.0), E); } \
TEST(atanh##S##_half) { N((T)0.54931, atanh##FSUF((T)0.5), E); } \
TEST(atanh##S##_nhalf) { N((T)-0.54931, atanh##FSUF((T)-0.5), E); } \
TEST(atanh##S##_one) { ASSERT_INF(atanh##FSUF((T)1.0)); } \
TEST(atanh##S##_none) { ASSERT_INF(atanh##FSUF((T)-1.0)); } \
TEST(atanh##S##_dom) { ASSERT_NAN(atanh##FSUF((T)2.0)); } \
TEST(atanh##S##_nan) { ASSERT_NAN(atanh##FSUF((T)NAN)); } \
TEST(atanh##S##_large) { ASSERT_INF(atanh##FSUF((T)1.0)); } \
TEST(atanh##S##_nlarge) { ASSERT_INF(atanh##FSUF((T)-1.0)); }

#define GEN_C99_ERF_GAMMA(S, FSUF, T, E, N, INF_VAL) \
TEST_SUITE(erf##S); \
TEST(erf##S##_zero) { N((T)0.0, erf##FSUF((T)0.0), E); } \
TEST(erf##S##_one) { N((T)0.84270, erf##FSUF((T)1.0), 0.001); } \
TEST(erf##S##_none) { N((T)-0.84270, erf##FSUF((T)-1.0), 0.001); } \
TEST(erf##S##_two) { N((T)0.99532, erf##FSUF((T)2.0), 0.001); } \
TEST(erf##S##_inf) { N((T)1.0, erf##FSUF(INF_VAL), 0.001); } \
TEST(erf##S##_ninf) { N((T)-1.0, erf##FSUF(-INF_VAL), 0.001); } \
TEST(erf##S##_nan) { ASSERT_NAN(erf##FSUF((T)NAN)); } \
TEST_SUITE(erfc##S); \
TEST(erfc##S##_zero) { N((T)1.0, erfc##FSUF((T)0.0), E); } \
TEST(erfc##S##_one) { N((T)0.15730, erfc##FSUF((T)1.0), 0.001); } \
TEST(erfc##S##_none) { N((T)1.84270, erfc##FSUF((T)-1.0), 0.001); } \
TEST(erfc##S##_inf) { N((T)0.0, erfc##FSUF(INF_VAL), 0.001); } \
TEST(erfc##S##_nan) { ASSERT_NAN(erfc##FSUF((T)NAN)); } \
TEST_SUITE(tgamma##S); \
TEST(tgamma##S##_one) { N((T)1.0, tgamma##FSUF((T)1.0), 0.01); } \
TEST(tgamma##S##_two) { N((T)1.0, tgamma##FSUF((T)2.0), 0.01); } \
TEST(tgamma##S##_three) { N((T)2.0, tgamma##FSUF((T)3.0), 0.01); } \
TEST(tgamma##S##_four) { N((T)6.0, tgamma##FSUF((T)4.0), 0.1); } \
TEST(tgamma##S##_five) { N((T)24.0, tgamma##FSUF((T)5.0), 0.5); } \
TEST(tgamma##S##_nint) { ASSERT_NAN(tgamma##FSUF((T)-1.0)); } \
TEST(tgamma##S##_inf) { ASSERT_INF(tgamma##FSUF(INF_VAL)); } \
TEST(tgamma##S##_ninf) { ASSERT_NAN(tgamma##FSUF(-INF_VAL)); } \
TEST(tgamma##S##_nan) { ASSERT_NAN(tgamma##FSUF((T)NAN)); } \
TEST_SUITE(lgamma##S); \
TEST(lgamma##S##_one) { N((T)0.0, lgamma##FSUF((T)1.0), 0.01); } \
TEST(lgamma##S##_two) { N((T)0.0, lgamma##FSUF((T)2.0), 0.01); } \
TEST(lgamma##S##_three) { N((T)0.69315, lgamma##FSUF((T)3.0), 0.01); } \
TEST(lgamma##S##_four) { N((T)1.79176, lgamma##FSUF((T)4.0), 0.01); } \
TEST(lgamma##S##_nint) { ASSERT_INF(lgamma##FSUF((T)-1.0)); } \
TEST(lgamma##S##_inf) { ASSERT_INF(lgamma##FSUF(INF_VAL)); } \
TEST(lgamma##S##_nan) { ASSERT_NAN(lgamma##FSUF((T)NAN)); }

#define GEN_C99_REMAINDER(S, FSUF, T, E, N, INF_VAL) \
TEST_SUITE(remainder##S); \
TEST(remainder##S##_basic) { N((T)1.0, remainder##FSUF((T)7.0, (T)3.0), E); } \
TEST(remainder##S##_exact) { N((T)0.0, remainder##FSUF((T)8.0, (T)4.0), E); } \
TEST(remainder##S##_zdiv) { ASSERT_NAN(remainder##FSUF((T)1.0, (T)0.0)); } \
TEST(remainder##S##_nan) { ASSERT_NAN(remainder##FSUF((T)NAN, (T)1.0)); } \
TEST(remainder##S##_inf) { ASSERT_NAN(remainder##FSUF(INF_VAL, (T)1.0)); } \
TEST_SUITE(remquo##S); \
TEST(remquo##S##_basic) { int q; N((T)1.0, remquo##FSUF((T)7.0, (T)3.0, &q), E); ASSERT_EQ(2, q); } \
TEST(remquo##S##_exact) { int q; N((T)0.0, remquo##FSUF((T)8.0, (T)4.0, &q), E); ASSERT_EQ(2, q); } \
TEST(remquo##S##_zdiv) { int q; ASSERT_NAN(remquo##FSUF((T)1.0, (T)0.0, &q)); }

#define GEN_C99_NEXT_NAN(S, FSUF, T, E, N, INF_VAL, MAX_VAL) \
TEST_SUITE(nextafter##S); \
TEST(nextafter##S##_up) { T x = 1.0; ASSERT_TRUE(nextafter##FSUF(x, (T)2.0) > x); } \
TEST(nextafter##S##_down) { T x = 1.0; ASSERT_TRUE(nextafter##FSUF(x, (T)0.0) < x); } \
TEST(nextafter##S##_same) { N((T)0.0, nextafter##FSUF((T)0.0, (T)0.0), 0.0); } \
TEST(nextafter##S##_inf) { ASSERT_INF(nextafter##FSUF((T)MAX_VAL, INF_VAL)); } \
TEST(nextafter##S##_nan) { ASSERT_NAN(nextafter##FSUF((T)NAN, (T)1.0)); } \
TEST_SUITE(nexttoward##S); \
TEST(nexttoward##S##_up) { T x = 1.0; ASSERT_TRUE(nexttoward##FSUF(x, 2.0L) > x); } \
TEST(nexttoward##S##_down) { T x = 1.0; ASSERT_TRUE(nexttoward##FSUF(x, 0.0L) < x); } \
TEST(nexttoward##S##_nan) { ASSERT_NAN(nexttoward##FSUF((T)NAN, 1.0L)); } \
TEST_SUITE(nan##S); \
TEST(nan##S##_gen) { ASSERT_NAN(nan##FSUF("")); } \
TEST(nan##S##_prop) { ASSERT_NAN(nan##FSUF("") + (T)1.0); } \
TEST_SUITE(copysign##S); \
TEST(copysign##S##_pp) { N((T)3.0, copysign##FSUF((T)3.0, (T)1.0), E); } \
TEST(copysign##S##_pn) { N((T)-3.0, copysign##FSUF((T)3.0, (T)-1.0), E); } \
TEST(copysign##S##_np) { N((T)3.0, copysign##FSUF((T)-3.0, (T)1.0), E); } \
TEST(copysign##S##_nn) { N((T)-3.0, copysign##FSUF((T)-3.0, (T)-1.0), E); } \
TEST(copysign##S##_zp) { N((T)0.0, copysign##FSUF((T)0.0, (T)1.0), E); ASSERT_EQ(0, signbit(copysign##FSUF((T)0.0, (T)1.0))); } \
TEST(copysign##S##_zn) { N((T)0.0, copysign##FSUF((T)0.0, (T)-1.0), E); ASSERT_NE(0, signbit(copysign##FSUF((T)0.0, (T)-1.0))); } \
TEST(copysign##S##_nan) { T v = copysign##FSUF((T)NAN, (T)-1.0); ASSERT_NAN(v); ASSERT_NE(0, signbit(v)); }

#endif /* JACL_HAS_C99 */

/* =========================================
 * C23 MACROS
 * ========================================= */
#if JACL_HAS_C23

#define GEN_C23_FROMFP(S, FSUF, T) \
TEST_SUITE(fromfp##S); \
TEST(fromfp##S##_basic) { ASSERT_EQ(3, fromfp##FSUF((T)2.5, FP_INT_TONEAREST, 32)); } \
TEST_SUITE(ufromfp##S); \
TEST(ufromfp##S##_basic) { ASSERT_EQ(3, ufromfp##FSUF((T)2.5, FP_INT_TONEAREST, 32)); } \
TEST_SUITE(fromfpx##S); \
TEST(fromfpx##S##_basic) { ASSERT_EQ(3, fromfpx##FSUF((T)2.5, FP_INT_TONEAREST, 32)); } \
TEST_SUITE(ufromfpx##S); \
TEST(ufromfpx##S##_basic) { ASSERT_EQ(3, ufromfpx##FSUF((T)2.5, FP_INT_TONEAREST, 32)); }

#define GEN_C23_LLOGB(S, FSUF, T) \
TEST_SUITE(llogb##S); \
TEST(llogb##S##_eight) { ASSERT_EQ(3L, llogb##FSUF((T)8.0)); } \
TEST(llogb##S##_zero) { ASSERT_EQ(FP_ILOGB0, llogb##FSUF((T)0.0)); }

#define GEN_C23_PI_TRIG(S, FSUF, T, E, N) \
TEST_SUITE(sinpi##S); \
TEST(sinpi##S##_half) { N((T)1.0, sinpi##FSUF((T)0.5), E); } \
TEST(sinpi##S##_zero) { N((T)0.0, sinpi##FSUF((T)0.0), E); } \
TEST_SUITE(cospi##S); \
TEST(cospi##S##_zero) { N((T)1.0, cospi##FSUF((T)0.0), E); } \
TEST(cospi##S##_half) { N((T)0.0, cospi##FSUF((T)0.5), E); } \
TEST_SUITE(tanpi##S); \
TEST(tanpi##S##_qtr) { N((T)1.0, tanpi##FSUF((T)0.25), E); } \
TEST(tanpi##S##_zero) { N((T)0.0, tanpi##FSUF((T)0.0), E); } \
TEST_SUITE(asinpi##S); \
TEST(asinpi##S##_one) { N((T)0.5, asinpi##FSUF((T)1.0), E); } \
TEST_SUITE(acospi##S); \
TEST(acospi##S##_one) { N((T)0.0, acospi##FSUF((T)1.0), E); } \
TEST_SUITE(atanpi##S); \
TEST(atanpi##S##_one) { N((T)0.25, atanpi##FSUF((T)1.0), E); } \
TEST_SUITE(atan2pi##S); \
TEST(atan2pi##S##_quad) { N((T)0.25, atan2pi##FSUF((T)1.0, (T)1.0), E); }

#define GEN_C23_POW_ROOT(S, FSUF, T, E, N) \
TEST_SUITE(pown##S); \
TEST(pown##S##_basic) { N((T)8.0, pown##FSUF((T)2.0, 3), E); } \
TEST(pown##S##_zero) { N((T)1.0, pown##FSUF((T)2.0, 0), E); } \
TEST_SUITE(powr##S); \
TEST(powr##S##_basic) { N((T)8.0, powr##FSUF((T)2.0, (T)3.0), E); } \
TEST_SUITE(rootn##S); \
TEST(rootn##S##_basic) { N((T)2.0, rootn##FSUF((T)8.0, 3), E); } \
TEST_SUITE(compoundn##S); \
TEST(compoundn##S##_basic) { N((T)8.0, compoundn##FSUF((T)1.0, 3), E); }

#define GEN_C23_NEXT(S, FSUF, T) \
TEST_SUITE(nextup##S); \
TEST(nextup##S##_basic) { T x = 1.0; ASSERT_TRUE(nextup##FSUF(x) > x); } \
TEST(nextup##S##_ninf) { T x = -INFINITY; ASSERT_TRUE(nextup##FSUF(x) > x); } \
TEST_SUITE(nextdown##S); \
TEST(nextdown##S##_basic) { T x = 1.0; ASSERT_TRUE(nextdown##FSUF(x) < x); } \
TEST(nextdown##S##_inf) { T x = INFINITY; ASSERT_TRUE(nextdown##FSUF(x) < x); }

#define GEN_C23_ROUNDEVEN(S, FSUF, T, E, N) \
TEST_SUITE(roundeven##S); \
TEST(roundeven##S##_h) { N((T)2.0, roundeven##FSUF((T)2.5), E); } \
TEST(roundeven##S##_odd) { N((T)4.0, roundeven##FSUF((T)3.5), E); } \
TEST(roundeven##S##_nh) { N((T)-2.0, roundeven##FSUF((T)-2.5), E); }

#define GEN_C23_MINMAX(S, FSUF, T, E, N) \
TEST_SUITE(fmaximum##S); \
TEST(fmaximum##S##_basic) { N((T)5.0, fmaximum##FSUF((T)3.0, (T)5.0), E); } \
TEST(fmaximum##S##_nan) { ASSERT_NAN(fmaximum##FSUF((T)NAN, (T)3.0)); } \
TEST_SUITE(fminimum##S); \
TEST(fminimum##S##_basic) { N((T)3.0, fminimum##FSUF((T)3.0, (T)5.0), E); } \
TEST(fminimum##S##_nan) { ASSERT_NAN(fminimum##FSUF((T)NAN, (T)3.0)); } \
TEST_SUITE(fmaximum_mag##S); \
TEST(fmaximum_mag##S##_basic) { N((T)-5.0, fmaximum_mag##FSUF((T)-5.0, (T)3.0), E); } \
TEST_SUITE(fminimum_mag##S); \
TEST(fminimum_mag##S##_basic) { N((T)3.0, fminimum_mag##FSUF((T)-5.0, (T)3.0), E); } \
TEST_SUITE(fmaximum_num##S); \
TEST(fmaximum_num##S##_nan) { N((T)3.0, fmaximum_num##FSUF((T)NAN, (T)3.0), E); } \
TEST_SUITE(fminimum_num##S); \
TEST(fminimum_num##S##_nan) { N((T)3.0, fminimum_num##FSUF((T)NAN, (T)3.0), E); }

#define GEN_C23_PAYLOAD(S, FSUF, T) \
TEST_SUITE(canonicalize##S); \
TEST(canonicalize##S##_basic) { T cx, x = 1.0; ASSERT_EQ(0, canonicalize##FSUF(&cx, &x)); } \
TEST_SUITE(setpayload##S); \
TEST(setpayload##S##_basic) { T x; setpayload##FSUF(&x, (T)42.0); ASSERT_EQ(42, (int)getpayload##FSUF(&x)); } \
TEST_SUITE(setpayloadsig##S); \
TEST(setpayloadsig##S##_basic) { T x; setpayloadsig##FSUF(&x, (T)42.0); ASSERT_NAN(x); }

#define GEN_C23_NARROW(S, FSUF, T) \
TEST_SUITE(faddf##S); \
TEST(faddf##S##_basic) { ASSERT_FLT_NEAR(3.0f, faddf(1.0, 2.0), EPS); } \
TEST_SUITE(fsubf##S); \
TEST(fsubf##S##_basic) { ASSERT_FLT_NEAR(-1.0f, fsubf(1.0, 2.0), EPS); } \
TEST_SUITE(fmulf##S); \
TEST(fmulf##S##_basic) { ASSERT_FLT_NEAR(2.0f, fmulf(1.0, 2.0), EPS); } \
TEST_SUITE(fdivf##S); \
TEST(fdivf##S##_basic) { ASSERT_FLT_NEAR(0.5f, fdivf(1.0, 2.0), EPS); }

#define GEN_C23_REDUC(S, FSUF, T, E, N) \
TEST_SUITE(reduc_sum##S); \
TEST(reduc_sum##S##_basic) { T p[] = {1.0, 2.0, 3.0}; N((T)6.0, reduc_sum##FSUF(3, p), E); } \
TEST(reduc_sum##S##_empty) { N((T)0.0, reduc_sum##FSUF(0, NULL), E); } \
TEST_SUITE(reduc_sumsq##S); \
TEST(reduc_sumsq##S##_basic) { T p[] = {1.0, 2.0, 3.0}; N((T)14.0, reduc_sumsq##FSUF(3, p), E); } \
TEST_SUITE(reduc_sumabs##S); \
TEST(reduc_sumabs##S##_basic) { T p[] = {-1.0, 2.0, -3.0}; N((T)6.0, reduc_sumabs##FSUF(3, p), E); } \
TEST_SUITE(reduc_dot##S); \
TEST(reduc_dot##S##_basic) { T p[] = {1.0, 2.0}, q[] = {3.0, 4.0}; N((T)11.0, reduc_dot##FSUF(2, p, q), E); }

#endif /* JACL_HAS_C23 */

/* =========================================
 * BASE SUITE INVOCATIONS
 * ========================================= */

GEN_CLASSIFICATION(_dbl, , double, HUGE_VAL, DBL_MIN, DBL_TRUE_MIN)
GEN_CLASSIFICATION(_flt, f, float, HUGE_VALF, FLT_MIN, FLT_TRUE_MIN)
#if JACL_HAS_C99
GEN_CLASSIFICATION(_ldbl, l, long double, HUGE_VALL, LDBL_MIN, LDBL_TRUE_MIN)
#endif

GEN_RELATIONAL(_dbl, , double)
GEN_RELATIONAL(_flt, f, float)
#if JACL_HAS_C99
GEN_RELATIONAL(_ldbl, l, long double)
#endif

GEN_FABS_FMOD(_dbl, , double, EPS, ASSERT_DBL_NEAR, HUGE_VAL)
GEN_FABS_FMOD(_flt, f, float, EPS, ASSERT_FLT_NEAR, HUGE_VALF)
#if JACL_HAS_C99
GEN_FABS_FMOD(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR, HUGE_VALL)
#endif

GEN_CEIL_FLOOR(_dbl, , double, EPS, ASSERT_DBL_NEAR, HUGE_VAL)
GEN_CEIL_FLOOR(_flt, f, float, EPS, ASSERT_FLT_NEAR, HUGE_VALF)
#if JACL_HAS_C99
GEN_CEIL_FLOOR(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR, HUGE_VALL)
#endif

GEN_EXP_LOG_BASE(_dbl, , double, EPS, ASSERT_DBL_NEAR, HUGE_VAL, DBL_LOG_MAX, DBL_LOG_TRUE_MIN, DBL_MAX_EXP, DBL_MANT_DIG, DBL_MIN_EXP, DBL_TRUE_MIN)
GEN_EXP_LOG_BASE(_flt, f, float, EPS, ASSERT_FLT_NEAR, HUGE_VALF, FLT_LOG_MAX, FLT_LOG_TRUE_MIN, FLT_MAX_EXP, FLT_MANT_DIG, FLT_MIN_EXP, FLT_TRUE_MIN)
#if JACL_HAS_C99
GEN_EXP_LOG_BASE(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR, HUGE_VALL, LDBL_LOG_MAX, LDBL_LOG_TRUE_MIN, LDBL_MAX_EXP, LDBL_MANT_DIG, LDBL_MIN_EXP, LDBL_TRUE_MIN)
#endif

GEN_POW_TRIG_BASE(_dbl, , double, EPS, ASSERT_DBL_NEAR, HUGE_VAL, DBL_MAX_10_EXP, DBL_MIN_10_EXP, DBL_TRUE_MIN)
GEN_POW_TRIG_BASE(_flt, f, float, EPS, ASSERT_FLT_NEAR, HUGE_VALF, FLT_MAX_10_EXP, FLT_MIN_10_EXP, FLT_TRUE_MIN)
#if JACL_HAS_C99
GEN_POW_TRIG_BASE(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR, HUGE_VALL, LDBL_MAX_10_EXP, LDBL_MIN_10_EXP, LDBL_TRUE_MIN)
#endif

GEN_HYPER_BASE(_dbl, , double, EPS, ASSERT_DBL_NEAR, HUGE_VAL, DBL_LOG_MAX)
GEN_HYPER_BASE(_flt, f, float, EPS, ASSERT_FLT_NEAR, HUGE_VALF, FLT_LOG_MAX)
#if JACL_HAS_C99
GEN_HYPER_BASE(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR, HUGE_VALL, LDBL_LOG_MAX)
#endif

GEN_MODF_BASE(_dbl, , double, EPS, ASSERT_DBL_NEAR)
GEN_MODF_BASE(_flt, f, float, EPS, ASSERT_FLT_NEAR)
#if JACL_HAS_C99
GEN_MODF_BASE(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR)
#endif

/* =========================================
 * C99 SUITE INVOCATIONS
 * ========================================= */
#if JACL_HAS_C99

GEN_C99_ROUNDING(_dbl, , double, EPS, ASSERT_DBL_NEAR, HUGE_VAL)
GEN_C99_ROUNDING(_flt, f, float, EPS, ASSERT_FLT_NEAR, HUGE_VALF)
GEN_C99_ROUNDING(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR, HUGE_VALL)

GEN_C99_EXP_LOG(_dbl, , double, EPS, ASSERT_DBL_NEAR, HUGE_VAL, DBL_LOG_MAX, DBL_LOG_TRUE_MIN, DBL_MAX_EXP, DBL_MANT_DIG, DBL_MIN_EXP)
GEN_C99_EXP_LOG(_flt, f, float, EPS, ASSERT_FLT_NEAR, HUGE_VALF, FLT_LOG_MAX, FLT_LOG_TRUE_MIN, FLT_MAX_EXP, FLT_MANT_DIG, FLT_MIN_EXP)
GEN_C99_EXP_LOG(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR, HUGE_VALL, LDBL_LOG_MAX, LDBL_LOG_TRUE_MIN, LDBL_MAX_EXP, LDBL_MANT_DIG, LDBL_MIN_EXP)

GEN_C99_ROOTS(_dbl, , double, EPS, ASSERT_DBL_NEAR, HUGE_VAL, DBL_TRUE_MIN)
GEN_C99_ROOTS(_flt, f, float, EPS, ASSERT_FLT_NEAR, HUGE_VALF, FLT_TRUE_MIN)
GEN_C99_ROOTS(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR, HUGE_VALL, LDBL_TRUE_MIN)

GEN_C99_MINMAX_FMA(_dbl, , double, EPS, ASSERT_DBL_NEAR, HUGE_VAL)
GEN_C99_MINMAX_FMA(_flt, f, float, EPS, ASSERT_FLT_NEAR, HUGE_VALF)
GEN_C99_MINMAX_FMA(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR, HUGE_VALL)

GEN_C99_SCALING(_dbl, , double, EPS, ASSERT_DBL_NEAR, HUGE_VAL)
GEN_C99_SCALING(_flt, f, float, EPS, ASSERT_FLT_NEAR, HUGE_VALF)
GEN_C99_SCALING(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR, HUGE_VALL)

GEN_C99_HYPER_INV(_dbl, , double, EPS, ASSERT_DBL_NEAR, HUGE_VAL, DBL_LOG_MAX)
GEN_C99_HYPER_INV(_flt, f, float, EPS, ASSERT_FLT_NEAR, HUGE_VALF, FLT_LOG_MAX)
GEN_C99_HYPER_INV(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR, HUGE_VALL, LDBL_LOG_MAX)

GEN_C99_ERF_GAMMA(_dbl, , double, EPS, ASSERT_DBL_NEAR, HUGE_VAL)
GEN_C99_ERF_GAMMA(_flt, f, float, EPS, ASSERT_FLT_NEAR, HUGE_VALF)
GEN_C99_ERF_GAMMA(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR, HUGE_VALL)

GEN_C99_REMAINDER(_dbl, , double, EPS, ASSERT_DBL_NEAR, HUGE_VAL)
GEN_C99_REMAINDER(_flt, f, float, EPS, ASSERT_FLT_NEAR, HUGE_VALF)
GEN_C99_REMAINDER(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR, HUGE_VALL)

GEN_C99_NEXT_NAN(_dbl, , double, EPS, ASSERT_DBL_NEAR, HUGE_VAL, DBL_MAX)
GEN_C99_NEXT_NAN(_flt, f, float, EPS, ASSERT_FLT_NEAR, HUGE_VALF, FLT_MAX)
GEN_C99_NEXT_NAN(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR, HUGE_VALL, LDBL_MAX)

#endif /* JACL_HAS_C99 */

/* =========================================
 * C23 SUITE INVOCATIONS
 * ========================================= */
#if JACL_HAS_C23

GEN_C23_FROMFP(_dbl, , double)
GEN_C23_FROMFP(_flt, f, float)
GEN_C23_FROMFP(_ldbl, l, long double)

GEN_C23_LLOGB(_dbl, , double)
GEN_C23_LLOGB(_flt, f, float)
GEN_C23_LLOGB(_ldbl, l, long double)

GEN_C23_PI_TRIG(_dbl, , double, EPS, ASSERT_DBL_NEAR)
GEN_C23_PI_TRIG(_flt, f, float, EPS, ASSERT_FLT_NEAR)
GEN_C23_PI_TRIG(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR)

GEN_C23_POW_ROOT(_dbl, , double, EPS, ASSERT_DBL_NEAR)
GEN_C23_POW_ROOT(_flt, f, float, EPS, ASSERT_FLT_NEAR)
GEN_C23_POW_ROOT(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR)

GEN_C23_NEXT(_dbl, , double)
GEN_C23_NEXT(_flt, f, float)
GEN_C23_NEXT(_ldbl, l, long double)

GEN_C23_ROUNDEVEN(_dbl, , double, EPS, ASSERT_DBL_NEAR)
GEN_C23_ROUNDEVEN(_flt, f, float, EPS, ASSERT_FLT_NEAR)
GEN_C23_ROUNDEVEN(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR)

GEN_C23_MINMAX(_dbl, , double, EPS, ASSERT_DBL_NEAR)
GEN_C23_MINMAX(_flt, f, float, EPS, ASSERT_FLT_NEAR)
GEN_C23_MINMAX(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR)

GEN_C23_PAYLOAD(_dbl, , double)
GEN_C23_PAYLOAD(_flt, f, float)
GEN_C23_PAYLOAD(_ldbl, l, long double)

GEN_C23_NARROW(_dbl, , double)

GEN_C23_REDUC(_dbl, , double, EPS, ASSERT_DBL_NEAR)
GEN_C23_REDUC(_flt, f, float, EPS, ASSERT_FLT_NEAR)
GEN_C23_REDUC(_ldbl, l, long double, EPS, ASSERT_LDBL_NEAR)

#endif /* JACL_HAS_C23 */

/* BESSEL (Double Only, Base/POSIX) */
TEST_SUITE(bessel);
TEST(bessel_j0_zero) { ASSERT_DBL_NEAR(1.0, j0(0.0), EPS); }
TEST(bessel_j0_three) { ASSERT_DBL_NEAR(-0.260052, j0(3.0), 0.001); }
TEST(bessel_j1_zero) { ASSERT_DBL_NEAR(0.0, j1(0.0), EPS); }
TEST(bessel_j1_three) { ASSERT_DBL_NEAR(0.339059, j1(3.0), 0.001); }
TEST(bessel_jn_match0) { ASSERT_DBL_NEAR(j0(3.0), jn(0, 3.0), EPS); }
TEST(bessel_jn_match1) { ASSERT_DBL_NEAR(j1(3.0), jn(1, 3.0), EPS); }
TEST(bessel_jn_order2) { ASSERT_DBL_NEAR(0.486091, jn(2, 3.0), 0.001); }
TEST(bessel_y0_zero) { ASSERT_INF(y0(0.0)); }
TEST(bessel_y0_three) { ASSERT_DBL_NEAR(0.376850, y0(3.0), 0.001); }
TEST(bessel_y1_zero) { ASSERT_INF(y1(0.0)); }
TEST(bessel_y1_three) { ASSERT_DBL_NEAR(0.324674, y1(3.0), 0.001); }
TEST(bessel_yn_match0) { ASSERT_DBL_NEAR(y0(3.0), yn(0, 3.0), EPS); }
TEST(bessel_yn_match1) { ASSERT_DBL_NEAR(y1(3.0), yn(1, 3.0), EPS); }
TEST(bessel_yn_order2) { ASSERT_DBL_NEAR(-0.160400, yn(2, 3.0), 0.001); }

TEST_MAIN()
