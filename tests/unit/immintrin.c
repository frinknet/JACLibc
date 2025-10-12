/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_C99 && __has_attribute(vector_size)
#include <immintrin.h>
#include <math.h>

TEST_TYPE(unit);
TEST_UNIT(immintrin.h);

/* ============================================================================
 * VECTOR TYPES
 * ============================================================================ */
TEST_SUITE(vector_types);

TEST(m128_type_exists) {
	__m128 v = {0.0f, 0.0f, 0.0f, 0.0f};
	ASSERT_EQ(0.0f, v[0]);
}

TEST(m128d_type_exists) {
	__m128d v = {0.0, 0.0};
	ASSERT_EQ(0.0, v[0]);
}

TEST(m128_size) {
	ASSERT_EQ(16, sizeof(__m128));
}

TEST(m128d_size) {
	ASSERT_EQ(16, sizeof(__m128d));
}

/* ============================================================================
 * SCALAR TO VECTOR
 * ============================================================================ */
TEST_SUITE(scalar_to_vector);

TEST(mm_set_ss) {
	__m128 v = _mm_set_ss(3.14f);
	ASSERT_FLOAT_EQ(3.14f, v[0], 0.001f);
	ASSERT_EQ(0.0f, v[1]);
	ASSERT_EQ(0.0f, v[2]);
	ASSERT_EQ(0.0f, v[3]);
}

TEST(mm_set_sd) {
	__m128d v = _mm_set_sd(2.718);
	ASSERT_FLOAT_EQ(2.718, v[0], 0.001);
	ASSERT_EQ(0.0, v[1]);
}

/* ============================================================================
 * VECTOR TO SCALAR
 * ============================================================================ */
TEST_SUITE(vector_to_scalar);

TEST(mm_cvtss_f32) {
	__m128 v = {1.5f, 2.5f, 3.5f, 4.5f};
	float result = _mm_cvtss_f32(v);
	ASSERT_FLOAT_EQ(1.5f, result, 0.001f);
}

TEST(mm_cvtsd_f64) {
	__m128d v = {1.23, 4.56};
	double result = _mm_cvtsd_f64(v);
	ASSERT_FLOAT_EQ(1.23, result, 0.001);
}

/* ============================================================================
 * TRIGONOMETRY - SINE
 * ============================================================================ */
TEST_SUITE(trig_sine);

TEST(mm_sin_ps) {
	__m128 v = {0.0f, 1.57f, 3.14f, 4.71f};  // 0, π/2, π, 3π/2
	__m128 result = _mm_sin_ps(v);
	
	ASSERT_FLOAT_EQ(0.0f, result[0], 0.01f);
	ASSERT_FLOAT_EQ(1.0f, result[1], 0.01f);
	ASSERT_FLOAT_EQ(0.0f, result[2], 0.01f);
	ASSERT_FLOAT_EQ(-1.0f, result[3], 0.01f);
}

TEST(mm_sin_pd) {
	__m128d v = {0.0, 1.5708};  // 0, π/2
	__m128d result = _mm_sin_pd(v);
	
	ASSERT_FLOAT_EQ(0.0, result[0], 0.01);
	ASSERT_FLOAT_EQ(1.0, result[1], 0.01);
}

/* ============================================================================
 * TRIGONOMETRY - COSINE
 * ============================================================================ */
TEST_SUITE(trig_cosine);

TEST(mm_cos_ps) {
	__m128 v = {0.0f, 1.57f, 3.14f, 4.71f};
	__m128 result = _mm_cos_ps(v);
	
	ASSERT_FLOAT_EQ(1.0f, result[0], 0.01f);
	ASSERT_FLOAT_EQ(0.0f, result[1], 0.01f);
	ASSERT_FLOAT_EQ(-1.0f, result[2], 0.01f);
}

TEST(mm_cos_pd) {
	__m128d v = {0.0, 3.14159};
	__m128d result = _mm_cos_pd(v);
	
	ASSERT_FLOAT_EQ(1.0, result[0], 0.01);
	ASSERT_FLOAT_EQ(-1.0, result[1], 0.01);
}

/* ============================================================================
 * TRIGONOMETRY - TANGENT
 * ============================================================================ */
TEST_SUITE(trig_tangent);

TEST(mm_tan_ps) {
	__m128 v = {0.0f, 0.785f, -0.785f, 1.0f};
	__m128 result = _mm_tan_ps(v);
	
	ASSERT_FLOAT_EQ(0.0f, result[0], 0.01f);
	ASSERT_FLOAT_EQ(1.0f, result[1], 0.1f);
}

TEST(mm_tan_pd) {
	__m128d v = {0.0, 0.7854};  // 0, π/4
	__m128d result = _mm_tan_pd(v);
	
	ASSERT_FLOAT_EQ(0.0, result[0], 0.01);
	ASSERT_FLOAT_EQ(1.0, result[1], 0.1);
}

/* ============================================================================
 * EXPONENTIAL
 * ============================================================================ */
TEST_SUITE(exponential);

TEST(mm_exp_ps) {
	__m128 v = {0.0f, 1.0f, 2.0f, 3.0f};
	__m128 result = _mm_exp_ps(v);
	
	ASSERT_FLOAT_EQ(1.0f, result[0], 0.01f);
	ASSERT_FLOAT_EQ(2.718f, result[1], 0.01f);
	ASSERT_FLOAT_EQ(7.389f, result[2], 0.01f);
}

TEST(mm_exp_pd) {
	__m128d v = {0.0, 1.0};
	__m128d result = _mm_exp_pd(v);
	
	ASSERT_FLOAT_EQ(1.0, result[0], 0.01);
	ASSERT_FLOAT_EQ(2.718, result[1], 0.01);
}

/* ============================================================================
 * LOGARITHM
 * ============================================================================ */
TEST_SUITE(logarithm);

TEST(mm_log_ps) {
	__m128 v = {1.0f, 2.718f, 7.389f, 10.0f};
	__m128 result = _mm_log_ps(v);
	
	ASSERT_FLOAT_EQ(0.0f, result[0], 0.01f);
	ASSERT_FLOAT_EQ(1.0f, result[1], 0.01f);
	ASSERT_FLOAT_EQ(2.0f, result[2], 0.01f);
}

TEST(mm_log_pd) {
	__m128d v = {1.0, 2.718};
	__m128d result = _mm_log_pd(v);
	
	ASSERT_FLOAT_EQ(0.0, result[0], 0.01);
	ASSERT_FLOAT_EQ(1.0, result[1], 0.01);
}

/* ============================================================================
 * SQUARE ROOT
 * ============================================================================ */
TEST_SUITE(square_root);

TEST(mm_sqrt_ps) {
	__m128 v = {1.0f, 4.0f, 9.0f, 16.0f};
	__m128 result = _mm_sqrt_ps(v);
	
	ASSERT_FLOAT_EQ(1.0f, result[0], 0.01f);
	ASSERT_FLOAT_EQ(2.0f, result[1], 0.01f);
	ASSERT_FLOAT_EQ(3.0f, result[2], 0.01f);
	ASSERT_FLOAT_EQ(4.0f, result[3], 0.01f);
}

TEST(mm_sqrt_pd) {
	__m128d v = {4.0, 9.0};
	__m128d result = _mm_sqrt_pd(v);
	
	ASSERT_FLOAT_EQ(2.0, result[0], 0.01);
	ASSERT_FLOAT_EQ(3.0, result[1], 0.01);
}

/* ============================================================================
 * POWER
 * ============================================================================ */
TEST_SUITE(power);

TEST(mm_pow_ps) {
	__m128 base = {2.0f, 3.0f, 4.0f, 5.0f};
	__m128 exp = {2.0f, 2.0f, 2.0f, 2.0f};
	__m128 result = _mm_pow_ps(base, exp);
	
	ASSERT_FLOAT_EQ(4.0f, result[0], 0.01f);
	ASSERT_FLOAT_EQ(9.0f, result[1], 0.01f);
	ASSERT_FLOAT_EQ(16.0f, result[2], 0.01f);
	ASSERT_FLOAT_EQ(25.0f, result[3], 0.01f);
}

TEST(mm_pow_pd) {
	__m128d base = {2.0, 10.0};
	__m128d exp = {3.0, 2.0};
	__m128d result = _mm_pow_pd(base, exp);
	
	ASSERT_FLOAT_EQ(8.0, result[0], 0.01);
	ASSERT_FLOAT_EQ(100.0, result[1], 0.01);
}

/* ============================================================================
 * HYPOT
 * ============================================================================ */
TEST_SUITE(hypot_test);

TEST(mm_hypot_ps) {
	__m128 a = {3.0f, 5.0f, 8.0f, 12.0f};
	__m128 b = {4.0f, 12.0f, 15.0f, 5.0f};
	__m128 result = _mm_hypot_ps(a, b);
	
	ASSERT_FLOAT_EQ(5.0f, result[0], 0.01f);
	ASSERT_FLOAT_EQ(13.0f, result[1], 0.01f);
	ASSERT_FLOAT_EQ(17.0f, result[2], 0.01f);
	ASSERT_FLOAT_EQ(13.0f, result[3], 0.01f);
}

TEST(mm_hypot_pd) {
	__m128d a = {3.0, 5.0};
	__m128d b = {4.0, 12.0};
	__m128d result = _mm_hypot_pd(a, b);
	
	ASSERT_FLOAT_EQ(5.0, result[0], 0.01);
	ASSERT_FLOAT_EQ(13.0, result[1], 0.01);
}

/* ============================================================================
 * HYPERBOLIC - SINH
 * ============================================================================ */
TEST_SUITE(hyperbolic_sinh);

TEST(mm_sinh_ps) {
	__m128 v = {0.0f, 1.0f, -1.0f, 2.0f};
	__m128 result = _mm_sinh_ps(v);
	
	ASSERT_FLOAT_EQ(0.0f, result[0], 0.01f);
	ASSERT_FLOAT_EQ(1.175f, result[1], 0.01f);
	ASSERT_FLOAT_EQ(-1.175f, result[2], 0.01f);
}

TEST(mm_sinh_pd) {
	__m128d v = {0.0, 1.0};
	__m128d result = _mm_sinh_pd(v);
	
	ASSERT_FLOAT_EQ(0.0, result[0], 0.01);
	ASSERT_FLOAT_EQ(1.175, result[1], 0.01);
}

/* ============================================================================
 * HYPERBOLIC - COSH
 * ============================================================================ */
TEST_SUITE(hyperbolic_cosh);

TEST(mm_cosh_ps) {
	__m128 v = {0.0f, 1.0f, -1.0f, 2.0f};
	__m128 result = _mm_cosh_ps(v);
	
	ASSERT_FLOAT_EQ(1.0f, result[0], 0.01f);
	ASSERT_FLOAT_EQ(1.543f, result[1], 0.01f);
	ASSERT_FLOAT_EQ(1.543f, result[2], 0.01f);
}

TEST(mm_cosh_pd) {
	__m128d v = {0.0, 1.0};
	__m128d result = _mm_cosh_pd(v);
	
	ASSERT_FLOAT_EQ(1.0, result[0], 0.01);
	ASSERT_FLOAT_EQ(1.543, result[1], 0.01);
}

/* ============================================================================
 * ERROR FUNCTION
 * ============================================================================ */
TEST_SUITE(error_function);

TEST(mm_erf_ps) {
	__m128 v = {0.0f, 0.5f, 1.0f, 2.0f};
	__m128 result = _mm_erf_ps(v);
	
	ASSERT_FLOAT_EQ(0.0f, result[0], 0.01f);
	ASSERT_FLOAT_EQ(0.520f, result[1], 0.01f);
	ASSERT_FLOAT_EQ(0.843f, result[2], 0.01f);
}

TEST(mm_erf_pd) {
	__m128d v = {0.0, 1.0};
	__m128d result = _mm_erf_pd(v);
	
	ASSERT_FLOAT_EQ(0.0, result[0], 0.01);
	ASSERT_FLOAT_EQ(0.843, result[1], 0.01);
}

/* ============================================================================
 * FUSED MULTIPLY-ADD
 * ============================================================================ */
TEST_SUITE(fused_multiply_add);

TEST(mm_fmadd_ps) {
	__m128 a = {2.0f, 3.0f, 4.0f, 5.0f};
	__m128 b = {3.0f, 4.0f, 5.0f, 6.0f};
	__m128 c = {1.0f, 2.0f, 3.0f, 4.0f};
	__m128 result = _mm_fmadd_ps(a, b, c);
	
	// a * b + c
	ASSERT_FLOAT_EQ(7.0f, result[0], 0.01f);   // 2*3 + 1 = 7
	ASSERT_FLOAT_EQ(14.0f, result[1], 0.01f);  // 3*4 + 2 = 14
	ASSERT_FLOAT_EQ(23.0f, result[2], 0.01f);  // 4*5 + 3 = 23
	ASSERT_FLOAT_EQ(34.0f, result[3], 0.01f);  // 5*6 + 4 = 34
}

TEST(mm_fmadd_pd) {
	__m128d a = {2.0, 3.0};
	__m128d b = {4.0, 5.0};
	__m128d c = {1.0, 2.0};
	__m128d result = _mm_fmadd_pd(a, b, c);
	
	ASSERT_FLOAT_EQ(9.0, result[0], 0.01);   // 2*4 + 1 = 9
	ASSERT_FLOAT_EQ(17.0, result[1], 0.01);  // 3*5 + 2 = 17
}

TEST_MAIN()

#else

int main(void) {
	printf("immintrin.h requires C99 and vector_size attribute support\n");
	return 0;
}

#endif
