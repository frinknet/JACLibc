/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_C99
#include <complex.h>
#include <math.h>

TEST_TYPE(unit);
TEST_UNIT(complex.h);

#define ASSERT_COMPLEX_EQ(expected_real, expected_imag, actual, tolerance) do { \
	double _r = creal(actual); \
	double _i = cimag(actual); \
	ASSERT_FLOAT_EQ(expected_real, _r, tolerance); \
	ASSERT_FLOAT_EQ(expected_imag, _i, tolerance); \
} while(0)

/* ============================================================================
 * COMPLEX TYPE AND MACROS
 * ============================================================================ */
TEST_SUITE(complex_basics);

TEST(complex_keyword_defined) {
	double complex z = 1.0 + 2.0 * I;
	(void)z;
	ASSERT_TRUE(1);
}

TEST(imaginary_unit) {
	double complex z = I;
	ASSERT_FLOAT_EQ(0.0, creal(z), 1e-10);
	ASSERT_FLOAT_EQ(1.0, cimag(z), 1e-10);
}

#if JACL_HAS_C11
TEST(cmplx_macros) {
	double complex z = CMPLX(3.0, 4.0);
	ASSERT_FLOAT_EQ(3.0, creal(z), 1e-10);
	ASSERT_FLOAT_EQ(4.0, cimag(z), 1e-10);
	
	float complex zf = CMPLXF(1.0f, 2.0f);
	ASSERT_FLOAT_EQ(1.0f, crealf(zf), 1e-6);
	ASSERT_FLOAT_EQ(2.0f, cimagf(zf), 1e-6);
}
#endif

TEST(complex_construction) {
	double complex z = 3.0 + 4.0 * I;
	ASSERT_FLOAT_EQ(3.0, creal(z), 1e-10);
	ASSERT_FLOAT_EQ(4.0, cimag(z), 1e-10);
}

/* ============================================================================
 * REAL AND IMAGINARY PARTS
 * ============================================================================ */
TEST_SUITE(real_imag);

TEST(creal_basic) {
	double complex z = 3.0 + 4.0 * I;
	ASSERT_FLOAT_EQ(3.0, creal(z), 1e-10);
}

TEST(cimag_basic) {
	double complex z = 3.0 + 4.0 * I;
	ASSERT_FLOAT_EQ(4.0, cimag(z), 1e-10);
}

TEST(creal_pure_real) {
	double complex z = 5.0 + 0.0 * I;
	ASSERT_FLOAT_EQ(5.0, creal(z), 1e-10);
	ASSERT_FLOAT_EQ(0.0, cimag(z), 1e-10);
}

TEST(cimag_pure_imaginary) {
	double complex z = 0.0 + 7.0 * I;
	ASSERT_FLOAT_EQ(0.0, creal(z), 1e-10);
	ASSERT_FLOAT_EQ(7.0, cimag(z), 1e-10);
}

TEST(float_complex) {
	float complex zf = 1.5f + 2.5f * I;
	ASSERT_FLOAT_EQ(1.5f, crealf(zf), 1e-6);
	ASSERT_FLOAT_EQ(2.5f, cimagf(zf), 1e-6);
}

/* ============================================================================
 * CONJUGATE
 * ============================================================================ */
TEST_SUITE(conjugate);

TEST(conj_basic) {
	double complex z = 3.0 + 4.0 * I;
	double complex c = conj(z);
	
	ASSERT_FLOAT_EQ(3.0, creal(c), 1e-10);
	ASSERT_FLOAT_EQ(-4.0, cimag(c), 1e-10);
}

TEST(conj_pure_real) {
	double complex z = 5.0 + 0.0 * I;
	double complex c = conj(z);
	
	ASSERT_FLOAT_EQ(5.0, creal(c), 1e-10);
	ASSERT_FLOAT_EQ(0.0, cimag(c), 1e-10);
}

TEST(conj_twice) {
	double complex z = 3.0 + 4.0 * I;
	double complex c = conj(conj(z));
	
	ASSERT_COMPLEX_EQ(3.0, 4.0, c, 1e-10);
}

/* ============================================================================
 * ABSOLUTE VALUE (MAGNITUDE)
 * ============================================================================ */
TEST_SUITE(absolute_value);

TEST(cabs_basic) {
	double complex z = 3.0 + 4.0 * I;
	double magnitude = cabs(z);
	
	ASSERT_FLOAT_EQ(5.0, magnitude, 1e-10);
}

TEST(cabs_pure_real) {
	double complex z = 5.0 + 0.0 * I;
	ASSERT_FLOAT_EQ(5.0, cabs(z), 1e-10);
}

TEST(cabs_pure_imaginary) {
	double complex z = 0.0 + 5.0 * I;
	ASSERT_FLOAT_EQ(5.0, cabs(z), 1e-10);
}

TEST(cabs_negative_components) {
	double complex z = -3.0 - 4.0 * I;
	ASSERT_FLOAT_EQ(5.0, cabs(z), 1e-10);
}

/* ============================================================================
 * ARGUMENT (PHASE)
 * ============================================================================ */
TEST_SUITE(argument);

TEST(carg_positive_real) {
	double complex z = 1.0 + 0.0 * I;
	ASSERT_FLOAT_EQ(0.0, carg(z), 1e-10);
}

TEST(carg_positive_imaginary) {
	double complex z = 0.0 + 1.0 * I;
	ASSERT_FLOAT_EQ(M_PI / 2.0, carg(z), 1e-10);
}

TEST(carg_negative_real) {
	double complex z = -1.0 + 0.0 * I;
	ASSERT_FLOAT_EQ(M_PI, fabs(carg(z)), 1e-10);
}

TEST(carg_first_quadrant) {
	double complex z = 1.0 + 1.0 * I;
	ASSERT_FLOAT_EQ(M_PI / 4.0, carg(z), 1e-10);
}

/* ============================================================================
 * TRIGONOMETRIC FUNCTIONS
 * ============================================================================ */
TEST_SUITE(trigonometric);

TEST(csin_zero) {
	double complex z = 0.0 + 0.0 * I;
	double complex result = csin(z);
	
	ASSERT_COMPLEX_EQ(0.0, 0.0, result, 1e-10);
}

TEST(ccos_zero) {
	double complex z = 0.0 + 0.0 * I;
	double complex result = ccos(z);
	
	ASSERT_COMPLEX_EQ(1.0, 0.0, result, 1e-10);
}

TEST(ctan_zero) {
	double complex z = 0.0 + 0.0 * I;
	double complex result = ctan(z);
	
	ASSERT_COMPLEX_EQ(0.0, 0.0, result, 1e-10);
}

TEST(csin_pure_real) {
	double complex z = M_PI / 2.0 + 0.0 * I;
	double complex result = csin(z);
	
	ASSERT_FLOAT_EQ(1.0, creal(result), 1e-10);
	ASSERT_FLOAT_EQ(0.0, cimag(result), 1e-10);
}

TEST(ccos_pure_real) {
	double complex z = M_PI + 0.0 * I;
	double complex result = ccos(z);
	
	ASSERT_FLOAT_EQ(-1.0, creal(result), 1e-10);
	ASSERT_FLOAT_EQ(0.0, cimag(result), 1e-10);
}

/* ============================================================================
 * INVERSE TRIGONOMETRIC FUNCTIONS
 * ============================================================================ */
TEST_SUITE(inverse_trig);

TEST(casin_zero) {
	double complex z = 0.0 + 0.0 * I;
	double complex result = casin(z);
	
	ASSERT_COMPLEX_EQ(0.0, 0.0, result, 1e-10);
}

TEST(cacos_one) {
	double complex z = 1.0 + 0.0 * I;
	double complex result = cacos(z);
	
	ASSERT_FLOAT_EQ(0.0, creal(result), 1e-10);
}

TEST(catan_zero) {
	double complex z = 0.0 + 0.0 * I;
	double complex result = catan(z);
	
	ASSERT_COMPLEX_EQ(0.0, 0.0, result, 1e-10);
}

/* ============================================================================
 * HYPERBOLIC FUNCTIONS
 * ============================================================================ */
TEST_SUITE(hyperbolic);

TEST(csinh_zero) {
	double complex z = 0.0 + 0.0 * I;
	double complex result = csinh(z);
	
	ASSERT_COMPLEX_EQ(0.0, 0.0, result, 1e-10);
}

TEST(ccosh_zero) {
	double complex z = 0.0 + 0.0 * I;
	double complex result = ccosh(z);
	
	ASSERT_COMPLEX_EQ(1.0, 0.0, result, 1e-10);
}

TEST(ctanh_zero) {
	double complex z = 0.0 + 0.0 * I;
	double complex result = ctanh(z);
	
	ASSERT_COMPLEX_EQ(0.0, 0.0, result, 1e-10);
}

/* ============================================================================
 * INVERSE HYPERBOLIC FUNCTIONS
 * ============================================================================ */
TEST_SUITE(inverse_hyperbolic);

TEST(casinh_zero) {
	double complex z = 0.0 + 0.0 * I;
	double complex result = casinh(z);
	
	ASSERT_COMPLEX_EQ(0.0, 0.0, result, 1e-10);
}

TEST(cacosh_one) {
	double complex z = 1.0 + 0.0 * I;
	double complex result = cacosh(z);
	
	ASSERT_FLOAT_EQ(0.0, creal(result), 1e-9);
}

TEST(catanh_zero) {
	double complex z = 0.0 + 0.0 * I;
	double complex result = catanh(z);
	
	ASSERT_COMPLEX_EQ(0.0, 0.0, result, 1e-10);
}

/* ============================================================================
 * EXPONENTIAL FUNCTIONS
 * ============================================================================ */
TEST_SUITE(exponential);

TEST(cexp_zero) {
	double complex z = 0.0 + 0.0 * I;
	double complex result = cexp(z);
	
	ASSERT_COMPLEX_EQ(1.0, 0.0, result, 1e-10);
}

TEST(cexp_pure_imaginary) {
	double complex z = 0.0 + M_PI * I;
	double complex result = cexp(z);
	
	ASSERT_FLOAT_EQ(-1.0, creal(result), 1e-10);
	ASSERT_FLOAT_EQ(0.0, cimag(result), 1e-10);
}

TEST(clog_one) {
	double complex z = 1.0 + 0.0 * I;
	double complex result = clog(z);
	
	ASSERT_COMPLEX_EQ(0.0, 0.0, result, 1e-10);
}

TEST(clog_e) {
	double complex z = M_E + 0.0 * I;
	double complex result = clog(z);
	
	ASSERT_FLOAT_EQ(1.0, creal(result), 1e-10);
	ASSERT_FLOAT_EQ(0.0, cimag(result), 1e-10);
}

TEST(csqrt_positive_real) {
	double complex z = 4.0 + 0.0 * I;
	double complex result = csqrt(z);
	
	ASSERT_FLOAT_EQ(2.0, creal(result), 1e-10);
	ASSERT_FLOAT_EQ(0.0, cimag(result), 1e-10);
}

TEST(csqrt_negative_real) {
	double complex z = -4.0 + 0.0 * I;
	double complex result = csqrt(z);
	
	ASSERT_FLOAT_EQ(0.0, creal(result), 1e-10);
	ASSERT_FLOAT_EQ(2.0, cimag(result), 1e-10);
}

TEST(csqrt_complex) {
	double complex z = 3.0 + 4.0 * I;
	double complex result = csqrt(z);
	double complex squared = result * result;
	
	ASSERT_FLOAT_EQ(3.0, creal(squared), 1e-9);
	ASSERT_FLOAT_EQ(4.0, cimag(squared), 1e-9);
}

/* ============================================================================
 * POWER FUNCTION
 * ============================================================================ */
TEST_SUITE(power);

TEST(cpow_square) {
	double complex z = 2.0 + 0.0 * I;
	double complex w = 2.0 + 0.0 * I;
	double complex result = cpow(z, w);
	
	ASSERT_FLOAT_EQ(4.0, creal(result), 1e-9);
	ASSERT_FLOAT_EQ(0.0, cimag(result), 1e-9);
}

TEST(cpow_zero_exponent) {
	double complex z = 3.0 + 4.0 * I;
	double complex w = 0.0 + 0.0 * I;
	double complex result = cpow(z, w);
	
	ASSERT_FLOAT_EQ(1.0, creal(result), 1e-9);
	ASSERT_FLOAT_EQ(0.0, cimag(result), 1e-9);
}

TEST(cpow_one_exponent) {
	double complex z = 3.0 + 4.0 * I;
	double complex w = 1.0 + 0.0 * I;
	double complex result = cpow(z, w);
	
	ASSERT_FLOAT_EQ(3.0, creal(result), 1e-9);
	ASSERT_FLOAT_EQ(4.0, cimag(result), 1e-9);
}

/* ============================================================================
 * PROJECTION
 * ============================================================================ */
TEST_SUITE(projection);

TEST(cproj_finite) {
	double complex z = 3.0 + 4.0 * I;
	double complex result = cproj(z);
	
	ASSERT_COMPLEX_EQ(3.0, 4.0, result, 1e-10);
}

/* ============================================================================
 * ARITHMETIC OPERATIONS
 * ============================================================================ */
TEST_SUITE(arithmetic);

TEST(complex_addition) {
	double complex z1 = 3.0 + 4.0 * I;
	double complex z2 = 1.0 + 2.0 * I;
	double complex result = z1 + z2;
	
	ASSERT_COMPLEX_EQ(4.0, 6.0, result, 1e-10);
}

TEST(complex_subtraction) {
	double complex z1 = 5.0 + 7.0 * I;
	double complex z2 = 2.0 + 3.0 * I;
	double complex result = z1 - z2;
	
	ASSERT_COMPLEX_EQ(3.0, 4.0, result, 1e-10);
}

TEST(complex_multiplication) {
	double complex z1 = 2.0 + 3.0 * I;
	double complex z2 = 4.0 + 5.0 * I;
	double complex result = z1 * z2;
	
	// (2+3i)(4+5i) = 8 + 10i + 12i + 15i^2 = -7 + 22i
	ASSERT_COMPLEX_EQ(-7.0, 22.0, result, 1e-10);
}

TEST(complex_division) {
	double complex z1 = 10.0 + 0.0 * I;
	double complex z2 = 2.0 + 0.0 * I;
	double complex result = z1 / z2;
	
	ASSERT_COMPLEX_EQ(5.0, 0.0, result, 1e-10);
}

/* ============================================================================
 * IDENTITIES AND RELATIONSHIPS
 * ============================================================================ */
TEST_SUITE(identities);

TEST(euler_formula) {
	// e^(i*pi) + 1 = 0
	double complex z = 0.0 + M_PI * I;
	double complex result = cexp(z);
	
	ASSERT_FLOAT_EQ(-1.0, creal(result), 1e-10);
	ASSERT_FLOAT_EQ(0.0, cimag(result), 1e-10);
}

TEST(log_exp_inverse) {
	double complex z = 2.0 + 3.0 * I;
	double complex result = clog(cexp(z));
	
	ASSERT_FLOAT_EQ(2.0, creal(result), 1e-9);
	ASSERT_FLOAT_EQ(3.0, cimag(result), 1e-9);
}

TEST(sqrt_square) {
	double complex z = 3.0 + 4.0 * I;
	double complex sq = csqrt(z);
	double complex result = sq * sq;
	
	ASSERT_FLOAT_EQ(3.0, creal(result), 1e-9);
	ASSERT_FLOAT_EQ(4.0, cimag(result), 1e-9);
}

TEST(conjugate_product) {
	double complex z = 3.0 + 4.0 * I;
	double complex product = z * conj(z);
	
	// Should equal |z|^2
	double abs_squared = cabs(z) * cabs(z);
	ASSERT_FLOAT_EQ(abs_squared, creal(product), 1e-9);
	ASSERT_FLOAT_EQ(0.0, cimag(product), 1e-9);
}

TEST_MAIN()

#else

int main(void) {
	printf("complex.h requires C99 or later\n");
	return 0;
}

#endif
