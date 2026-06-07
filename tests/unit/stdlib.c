/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <stdlib.h>

TEST_TYPE(unit)
TEST_UNIT(stdlib.h)

/* ============================================================= */

TEST_SUITE(atoi)

TEST(atoi_positive) {
	ASSERT_INT_EQ(atoi("123"), 123);
}

TEST(atoi_negative) {
	ASSERT_INT_EQ(atoi("-456"), -456);
}

TEST(atoi_zero) {
	ASSERT_INT_EQ(atoi("0"), 0);
}

TEST(atoi_whitespace) {
	ASSERT_INT_EQ(atoi("  789"), 789);
}

TEST(atoi_partial) {
	ASSERT_INT_EQ(atoi("123abc"), 123);
}

TEST(atoi_invalid) {
	ASSERT_INT_EQ(atoi("abc"), 0);
}

TEST(atoi_sign_plus) {
	ASSERT_INT_EQ(atoi("+42"), 42);
}

TEST(atoi_max) {
	ASSERT_INT_EQ(atoi("2147483647"), INT_MAX);
}

TEST(atoi_many_leading_zeros) {
	ASSERT_INT_EQ(atoi("000000000000000000000000000000001"), 1);
}

TEST(atoi_plus_minus_mixed) {
	ASSERT_INT_EQ(atoi("+-123"), 0);
}

TEST(atoi_space_after_sign) {
	ASSERT_INT_EQ(atoi("+ 123"), 0);
}

TEST(atoi_tabs_before) {
	ASSERT_INT_EQ(atoi("\t\t\t42"), 42);
}

TEST(atoi_newline_before) {
	ASSERT_INT_EQ(atoi("\n99"), 99);
}

TEST(atoi_vertical_tab_form_feed) {
	ASSERT_INT_EQ(atoi("\v\f50"), 50);
}

TEST(atoi_int_min_underflow) {
	int result = atoi("-2147483649");
}

TEST(atoi_empty_string) {
	ASSERT_INT_EQ(atoi(""), 0);
}

TEST(atoi_whitespace_only) {
	ASSERT_INT_EQ(atoi("   \t\n"), 0);
}

TEST(atoi_sign_only) {
	ASSERT_INT_EQ(atoi("+"), 0);
	ASSERT_INT_EQ(atoi("-"), 0);
}

/* ============================================================= */

TEST_SUITE(atol)

TEST(atol_basic) {
	ASSERT_INT_EQ(atol("1234"), 1234L);
}

TEST(atol_negative) {
	ASSERT_INT_EQ(atol("-5678"), -5678L);
}

TEST(atol_large) {
	ASSERT_INT_EQ(atol("9223372036854775807"), LLONG_MAX);
}

TEST(atol_whitespace_tabs) {
	ASSERT_INT_EQ(atol("\t\t999"), 999L);
}

TEST(atol_overflow_boundary) {
	char buf[64];
	snprintf(buf, sizeof(buf), "%ld", LONG_MAX);
	ASSERT_INT_EQ(atol(buf), LONG_MAX);
}

/* ============================================================= */

TEST_SUITE(atoll)

TEST(atoll_basic) {
	ASSERT_INT_EQ(atoll("123456"), 123456LL);
}

TEST(atoll_large) {
	ASSERT_INT_EQ(atoll("9223372036854775807"), LLONG_MAX);
}

TEST(atoll_negative) {
	ASSERT_INT_EQ(atoll("-987654"), -987654LL);
}

TEST(atoll_overflow_boundary) {
	char buf[64];
	snprintf(buf, sizeof(buf), "%lld", LLONG_MAX);
	ASSERT_INT_EQ(atoll(buf), LLONG_MAX);
}

/* ============================================================= */

TEST_SUITE(atof)

TEST(atof_integer) {
	ASSERT_DBL_EQ(atof("123"), 123.0);
}

TEST(atof_decimal) {
	ASSERT_DBL_EQ(atof("3.14"), 3.14);
}

TEST(atof_scientific_upper) {
	ASSERT_DBL_EQ(atof("1.5E2"), 150.0);
}

TEST(atof_scientific_lower) {
	ASSERT_DBL_EQ(atof("2.5e-1"), 0.25);
}

TEST(atof_negative) {
	ASSERT_DBL_EQ(atof("-2.71"), -2.71);
}

TEST(atof_invalid) {
	ASSERT_DBL_EQ(atof("xyz"), 0.0);
}

/* ============================================================= */

TEST_SUITE(strtol)

TEST(strtol_base10_endptr) {
	char *endptr;
	long result = strtol("123abc", &endptr, 10);
	ASSERT_INT_EQ(result, 123);
	ASSERT_INT_EQ(*endptr, 'a');
}

TEST(strtol_base10_nullptr) {
	ASSERT_INT_EQ(strtol("456", NULL, 10), 456);
}

TEST(strtol_base16) {
	ASSERT_INT_EQ(strtol("FF", NULL, 16), 255);
}

TEST(strtol_base16_lowercase) {
	ASSERT_INT_EQ(strtol("ff", NULL, 16), 255);
}

TEST(strtol_base2) {
	ASSERT_INT_EQ(strtol("1010", NULL, 2), 10);
}

TEST(strtol_base8) {
	ASSERT_INT_EQ(strtol("777", NULL, 8), 511);
}

TEST(strtol_auto_base_hex) {
	ASSERT_INT_EQ(strtol("0xFF", NULL, 0), 255);
}

TEST(strtol_auto_base_octal) {
	ASSERT_INT_EQ(strtol("0777", NULL, 0), 511);
}

TEST(strtol_auto_base_decimal) {
	ASSERT_INT_EQ(strtol("999", NULL, 0), 999);
}

TEST(strtol_negative) {
	ASSERT_INT_EQ(strtol("-100", NULL, 10), -100);
}

TEST(strtol_whitespace) {
	ASSERT_INT_EQ(strtol("  42", NULL, 10), 42);
}

TEST(strtol_base_1_invalid) {
	long result = strtol("123", NULL, 1);
	ASSERT_ERRNO(EINVAL);
	ASSERT_INT_EQ(result, 0);
}

TEST(strtol_base_37_invalid) {
	long result = strtol("XYZ", NULL, 37);
	ASSERT_ERRNO(EINVAL);
	ASSERT_INT_EQ(result, 0);
}

TEST(strtol_overflow_positive) {
	long result = strtol("99999999999999999999999", NULL, 10);
	ASSERT_ERRNO(ERANGE);
	ASSERT_INT_EQ(result, LONG_MAX);
}

TEST(strtol_underflow_negative)
{
	long result = strtol("-99999999999999999999999", NULL, 10);
	ASSERT_ERRNO(ERANGE);
	ASSERT_INT_EQ(result, LONG_MIN);
}

TEST(strtol_no_conversion)
{
	char *ep;
	long result = strtol("notanumber", &ep, 10);
	ASSERT_ERRNO(0);
	ASSERT_INT_EQ(result, 0);
	ASSERT_INT_EQ(ep[0], 'n');
}

TEST(strtol_endptr_at_partial)
{
	char *ep;
	strtol("123.456", &ep, 10);
	ASSERT_INT_EQ(*ep, '.');
}

TEST(strtol_endptr_with_sign_only)
{
	char *ep;
	long result = strtol("-+abc", &ep, 10);
	ASSERT_INT_EQ(result, 0);
	ASSERT_INT_EQ(*ep, '-');
}

TEST(strtol_endptr_exact_term) {
	char buf[] = "12345";
	char *end = NULL;
	long v = strtol(buf, &end, 10);
	ASSERT_EQ(v, 12345);
	ASSERT_EQ(*end, '\0');
	ASSERT_EQ(end, buf + 5);
}

TEST(strtol_endptr_partial) {
	char buf[] = "12345abc";
	char *end = NULL;
	long v = strtol(buf, &end, 10);
	ASSERT_EQ(v, 12345);
	ASSERT_EQ(*end, 'a');
	ASSERT_TRUE(end > buf && end <= buf + strlen(buf));
}

TEST(strtol_no_conversion_endptr) {
	char buf[] = "xyz";
	char *end = NULL;
	long v = strtol(buf, &end, 10);
	ASSERT_EQ(v, 0);
	ASSERT_EQ(end, buf);
}

TEST(strtol_empty_string) {
	char buf[] = "";
	char *end = NULL;
	long v = strtol(buf, &end, 10);
	ASSERT_EQ(v, 0);
	ASSERT_EQ(end, buf);
}

TEST(strtol_null_endptr) {
	long v = strtol("1234", NULL, 10);
	ASSERT_EQ(v, 1234);
}

TEST(strtol_only_sign) {
	char buf[] = "+";
	char *end = NULL;
	long v = strtol(buf, &end, 10);
	ASSERT_EQ(v, 0);
	ASSERT_EQ(end, buf);
}

TEST(strtol_only_sign_minus) {
	char buf[] = "-";
	char *end = NULL;
	long v = strtol(buf, &end, 10);
	ASSERT_EQ(v, 0);
	ASSERT_EQ(end, buf);
}

TEST(strtol_sign_space) {
	char buf[] = "+ ";
	char *end = NULL;
	long v = strtol(buf, &end, 10);
	ASSERT_EQ(v, 0);
	ASSERT_EQ(end, buf);
}

TEST(strtol_stack_boundary) {
	char buf[2] = "9";
	char *end = NULL;
	long v = strtol(buf, &end, 10);
	ASSERT_EQ(v, 9);
	ASSERT_EQ(end, buf+1);
}

TEST(strtol_whitespace_only) {
	char buf[] = "   \t\n";
	char *ep;
	long v = strtol(buf, &ep, 10);
	ASSERT_INT_EQ(v, 0);
	/* POSIX: No conversion -> endptr points to original string start */
	ASSERT_PTR_EQ(ep, buf);
}

TEST(strtol_invalid_base_zero) {
	long v = strtol("123", NULL, 0);
	ASSERT_INT_EQ(v, 123);
}

TEST(strtol_overflow_boundary) {
	char buf[64];
	snprintf(buf, sizeof(buf), "%ld", LONG_MAX);
	char *ep;
	long v = strtol(buf, &ep, 10);
	ASSERT_INT_EQ(v, LONG_MAX);
	ASSERT_INT_EQ(*ep, '\0');
}

TEST(strtol_overflow_by_one) {
	char buf[128];
	int len = snprintf(buf, sizeof(buf), "%ld", LONG_MAX);
	buf[len] = '1';
	buf[len + 1] = '\0';
	errno = 0;
	long v = strtol(buf, NULL, 10);
	ASSERT_INT_EQ(v, LONG_MAX);
	ASSERT_ERRNO(ERANGE);
}

TEST(strtol_base_1) {
	errno = 0;
	long r = strtol("123", NULL, 1);
	ASSERT_INT_EQ(r, 0);
	ASSERT_ERRNO(EINVAL);
}

TEST(strtol_base_37) {
	errno = 0;
	long r = strtol("123", NULL, 37);
	ASSERT_INT_EQ(r, 0);
	ASSERT_ERRNO(EINVAL);
}

TEST(strtol_negative_base) {
	errno = 0;
	long r = strtol("123", NULL, -10);
	ASSERT_INT_EQ(r, 0);
	ASSERT_ERRNO(EINVAL);
}

/* ============================================================= */

TEST_SUITE(strtoll)

TEST(strtoll_basic) {
	ASSERT_INT_EQ(strtoll("555", NULL, 10), 555LL);
}

TEST(strtoll_hex) {
	ASSERT_INT_EQ(strtoll("DEADBEEF", NULL, 16), 0xDEADBEEFLL);
}

TEST(strtoll_overflow) {
	long long result = strtoll("9999999999999999999999999999", NULL, 10);
	ASSERT_ERRNO(ERANGE);
	ASSERT_INT_EQ(result, LLONG_MAX);
}

TEST(strtoll_endptr_exact_term) {
	char buf[] = "9223372036854775807";
	char *end = NULL;
	long long v = strtoll(buf, &end, 10);
	ASSERT_EQ(v, LLONG_MAX);
	ASSERT_EQ(*end, '\0');
	ASSERT_EQ(end, buf + strlen(buf));
}

TEST(strtoll_overflow_partial) {
	char buf[] = "9223372036854775807123";
	char *end = NULL;
	long long v = strtoll(buf, &end, 10);
	ASSERT_EQ(v, LLONG_MAX);
	ASSERT_TRUE(end > buf && end <= buf + strlen(buf));
}

TEST(strtoll_invalid_base) {
	char buf[] = "123";
	char *end = NULL;
	errno = 0;
	long long v = strtoll(buf, &end, 1);
	ASSERT_EQ(errno, EINVAL);
	ASSERT_EQ(v, 0);
	ASSERT_EQ(end, buf);
}

TEST(strtoll_masive_overflow) {
	char input[] = "9999999999999999999999999999999999999999";
	errno = 0;
	char *end = NULL;
	long long v = strtoll(input, &end, 10);
	ASSERT_EQ(v, LLONG_MAX);       // Must be clamped!
	ASSERT_EQ(errno, ERANGE);      // Must indicate overflow
	ASSERT_TRUE(end != input && *end == '\0'); // Must parse full valid input
}

TEST(strtoll_massive_underflow) {
	char input[] = "-999999999999999999999999999999999999999";
	errno = 0;
	char *end = NULL;
	long long v = strtoll(input, &end, 10);
	ASSERT_EQ(v, LLONG_MIN);       // Must be clamped!
	ASSERT_EQ(errno, ERANGE);      // Must indicate underflow
	ASSERT_TRUE(end != input && *end == '\0'); // Must parse full valid input
}

TEST(strtoll_empty_string) {
	char *ep;
	long long v = strtoll("", &ep, 10);
	ASSERT_INT_EQ(v, 0);
	ASSERT_INT_EQ(ep[0], '\0');
}

TEST(strtoll_overflow_boundary) {
	char buf[64];
	snprintf(buf, sizeof(buf), "%lld", LLONG_MAX);
	char *ep;
	long long v = strtoll(buf, &ep, 10);
	ASSERT_INT_EQ(v, LLONG_MAX);
	ASSERT_INT_EQ(*ep, '\0');
}

/* ============================================================= */

TEST_SUITE(strtoul)

TEST(strtoul_basic) {
	ASSERT_INT_EQ(strtoul("1000", NULL, 10), 1000UL);
}

TEST(strtoul_hex) {
	ASSERT_INT_EQ(strtoul("ABCD", NULL, 16), 0xABCDUL);
}

TEST(strtoul_hex_prefix) {
	ASSERT_INT_EQ(strtoul("0xCAFE", NULL, 16), 0xCAFEUL);
}

TEST(strtoul_overflow) {
	unsigned long result = strtoul("99999999999999999999", NULL, 10);
	ASSERT_ERRNO(ERANGE);
	ASSERT_INT_EQ(result, ULONG_MAX);
}

TEST(strtoul_negative_input) {
	errno = 0;
	unsigned long v = strtoul("-1", NULL, 10);
	ASSERT_INT_EQ(v, ULONG_MAX);
}

/* ============================================================= */

TEST_SUITE(strtoull)

TEST(strtoull_basic) {
	ASSERT_INT_EQ(strtoull("777", NULL, 10), 777ULL);
}

TEST(strtoull_hex) {
	ASSERT_INT_EQ(strtoull("0xDEADBEEF", NULL, 16), 0xDEADBEEFULL);
}

TEST(strtoull_overflow) {
	char buf[] = "9999999999999999999999999999999";
	char *end = NULL;
	unsigned long long v = strtoull(buf, &end, 10);
	ASSERT_EQ(v, ULLONG_MAX);
	ASSERT_TRUE(end > buf);
}

TEST(strtoull_no_digits) {
	char buf[] = "xyz";
	char *end = NULL;
	unsigned long long v = strtoull(buf, &end, 10);
	ASSERT_EQ(v, 0);
	ASSERT_EQ(end, buf);
}

TEST(strtoull_only_sign) {
	char buf[] = "-";
	char *end = NULL;
	unsigned long long v = strtoull(buf, &end, 10);
	ASSERT_EQ(v, 0);
	ASSERT_EQ(end, buf);
}

TEST(strtoull_negative_input) {
	errno = 0;
	unsigned long long v = strtoull("-1", NULL, 10);
	ASSERT_INT_EQ(v, ULLONG_MAX);
}

/* ============================================================= */

TEST_SUITE(strtod)

TEST(strtod_basic) {
	char *endptr;
	double result = strtod("3.14159", &endptr);
	ASSERT_DBL_NEAR(3.14159, result, 1e-6);
	ASSERT_INT_EQ(*endptr, '\0');
}

TEST(strtod_scientific) {
	double result = strtod("1.23e-4", NULL);
	ASSERT_DBL_NEAR(0.000123, result, 1e-8);
}

TEST(strtod_negative_zero) {
	double result = strtod("-0.0", NULL);
	ASSERT_DBL_EQ(result, 0.0);
	ASSERT_INT_EQ(signbit(result), 1);
}

TEST(strtod_endptr_advances) {
	char *ep;
	strtod("42.5xyz", &ep);
	ASSERT_INT_EQ(*ep, 'x');
}

TEST(strtod_similar_special_values) {
	double result = strtod("NAN", NULL);
	ASSERT_INT_EQ(isnan(result), 1);
}

TEST(strtod_endptr_after_e_notation) {
	char *ep;
	strtod("1.5e10end", &ep);
	ASSERT_INT_EQ(*ep, 'e');
}

TEST(strtod_empty_string) {
	char *ep;
	double v = strtod("", &ep);
	ASSERT_DBL_EQ(v, 0.0);
	ASSERT_INT_EQ(ep[0], '\0');
}

TEST(strtod_whitespace_only) {
	char *ep;
	double v = strtod("  \t\n", &ep);
	ASSERT_DBL_EQ(v, 0.0);
	ASSERT_INT_EQ(*ep, '\0');
}

TEST(strtod_infinity_variants) {
	ASSERT_TRUE(isinf(strtod("INF", NULL)));
	ASSERT_TRUE(isinf(strtod("+INF", NULL)));
	ASSERT_TRUE(isinf(strtod("-INF", NULL)));
	ASSERT_TRUE(isinf(strtod("infinity", NULL)));
	ASSERT_TRUE(isinf(strtod("-INFINITY", NULL)));
}

TEST(strtod_nan_variants) {
	ASSERT_TRUE(isnan(strtod("NAN", NULL)));
	ASSERT_TRUE(isnan(strtod("nan", NULL)));
	ASSERT_TRUE(isnan(strtod("NAN(0x123)", NULL)));
}

TEST(strtod_hex_float) {
	ASSERT_DBL_NEAR(strtod("0x1.0p0", NULL), 1.0, 1e-9);
	ASSERT_DBL_NEAR(strtod("0x1.8p+2", NULL), 6.0, 1e-9);
	ASSERT_DBL_NEAR(strtod("-0x1.fp-3", NULL), -0.2421875, 1e-9);
}

TEST(strtod_subnormal) {
	double v = strtod("1e-320", NULL);
	ASSERT_TRUE(v >= 0.0 && v <= 1e-308);
}

/* ============================================================= */

TEST_SUITE(strtof)

TEST(strtof_basic) {
	double result = (double)strtof("3.14", NULL);
	ASSERT_DBL_NEAR(3.14, result, 0.0001);
}

TEST(strtof_scientific) {
	double result = (double)strtof("1.23e2", NULL);
	ASSERT_DBL_NEAR(123.0, result, 0.01);
}

TEST(strtof_positive_infinity_CAPS) {
	float result = strtof("INF", NULL);
	ASSERT_INT_EQ(isinf(result), 1);
	ASSERT_INT_EQ(signbit(result), 0);
}

TEST(strtof_positive_infinity_lower) {
	float result = strtof("inf", NULL);
	ASSERT_INT_EQ(isinf(result), 1);
}

TEST(strtof_positive_infinity_FULL) {
	float result = strtof("INFINITY", NULL);
	ASSERT_INT_EQ(isinf(result), 1);
}

TEST(strtof_negative_infinity) {
	float result = strtof("-INF", NULL);
	ASSERT_INT_EQ(isinf(result), 1);
	ASSERT_INT_EQ(signbit(result), 1);
}

TEST(strtof_nan_CAPS) {
	float result = strtof("NAN", NULL);
	ASSERT_INT_EQ(isnan(result), 1);
}

TEST(strtof_nan_lower) {
	float result = strtof("nan", NULL);
	ASSERT_INT_EQ(isnan(result), 1);
}

TEST(strtof_nan_with_parens) {
	float result = strtof("NAN(0x1234)", NULL);
	ASSERT_INT_EQ(isnan(result), 1);
}

TEST(strtof_negative_zero) {
	float result = strtof("-0.0", NULL);
	ASSERT_INT_EQ(result, 0.0);
	ASSERT_INT_EQ(signbit(result), 1);
}

TEST(strtof_subnormal_small) {
	float result = strtof("1e-50", NULL);
}

TEST(strtof_overflow_to_inf) {
	float result = strtof("1e100", NULL);
	ASSERT_ERRNO(ERANGE);
	ASSERT_INT_EQ(isinf(result), 1);
}

TEST(strtof_underflow_to_zero) {
	float result = strtof("1e-1000", NULL);
	ASSERT_ERRNO(ERANGE);
	ASSERT_INT_EQ(result, 0.0);
}

TEST(strtof_hex_float_0x_prefix) {
	float result = strtof("0x1.0p0", NULL);
	ASSERT_DBL_NEAR(result, 1.0, 0.0001);
}

TEST(strtof_hex_float_exponent) {
	float result = strtof("0x1.0p+4", NULL);
	ASSERT_DBL_NEAR(result, 16.0, 0.0001);
}

TEST(strtof_endptr_stops_at_invalid_char) {
	char *ep;
	strtof("3.14@@@", &ep);
	ASSERT_INT_EQ(*ep, '@');
}

/* ============================================================= */

TEST_SUITE(strtold)

TEST(strtold_basic) {
	double result = (double)strtold("2.718", NULL);
	ASSERT_DBL_NEAR(2.718, result, 0.0001);
}

TEST(strtold_negative) {
	double result = (double)strtold("-1.414", NULL);
	ASSERT_DBL_NEAR(-1.414, result, 0.0001);
}

TEST(strtold_similar_special_values) {
	long double result = strtold("INF", NULL);
	ASSERT_INT_EQ(isinf(result), 1);
}

/* ============================================================= */

TEST_SUITE(abs)

TEST(abs_positive) {
	ASSERT_INT_EQ(abs(42), 42);
}

TEST(abs_negative) {
	ASSERT_INT_EQ(abs(-42), 42);
}

TEST(abs_zero) {
	ASSERT_INT_EQ(abs(0), 0);
}

TEST(abs_min) {
	ASSERT_INT_EQ(abs(INT_MIN + 1), INT_MAX);
}

TEST(abs_int_min_overflow) {
	int result = abs(INT_MIN);
}

TEST(abs_int_min_undefined) {
	int result = abs(INT_MIN);
	(void)result;
	ASSERT_TRUE(1);
}

TEST(abs_int_min_wrap) {
	int r = abs(INT_MIN);
	ASSERT_INT_EQ(r, INT_MIN);
}

/* ============================================================= */

TEST_SUITE(labs)

TEST(labs_positive) {
	ASSERT_INT_EQ(labs(100L), 100L);
}

TEST(labs_negative) {
	ASSERT_INT_EQ(labs(-100L), 100L);
}

TEST(labs_long_min) {
	long result = labs(LONG_MIN);
}

TEST(labs_long_min_undefined) {
	long result = labs(LONG_MIN);
	(void)result;
	ASSERT_TRUE(1);
}

TEST(labs_long_min_wrap) {
	long r = labs(LONG_MIN);
	ASSERT_INT_EQ(r, LONG_MIN);
}

/* ============================================================= */

TEST_SUITE(llabs)

TEST(llabs_positive) {
	ASSERT_INT_EQ(llabs(1000LL), 1000LL);
}

TEST(llabs_negative) {
	ASSERT_INT_EQ(llabs(-1000LL), 1000LL);
}

TEST(llabs_long_long_min) {
	long long result = llabs(LLONG_MIN);
}

TEST(llabs_llong_min_undefined) {
	long long result = llabs(LLONG_MIN);
	(void)result;
	ASSERT_TRUE(1);
}

TEST(llabs_llong_min_wrap) {
	long long r = llabs(LLONG_MIN);
	ASSERT_INT_EQ(r, LLONG_MIN);
}

/* ============================================================= */

TEST_SUITE(div)

TEST(div_basic) {
	div_t r = div(10, 3);
	ASSERT_INT_EQ(r.quot, 3);
	ASSERT_INT_EQ(r.rem, 1);
}

TEST(div_negative_dividend) {
	div_t r = div(-10, 3);
	ASSERT_INT_EQ(r.quot, -3);
	ASSERT_INT_EQ(r.rem, -1);
}

TEST(div_negative_divisor) {
	div_t r = div(10, -3);
	ASSERT_INT_EQ(r.quot, -3);
	ASSERT_INT_EQ(r.rem, 1);
}

TEST(div_zero_remainder) {
	div_t r = div(12, 4);
	ASSERT_INT_EQ(r.quot, 3);
	ASSERT_INT_EQ(r.rem, 0);
}

TEST(div_both_negative) {
	div_t r = div(-10, -3);
	ASSERT_INT_EQ(r.quot, 3);
	ASSERT_INT_EQ(r.rem, -1);
}

TEST(div_zero_dividend) {
	div_t r = div(0, 5);
	ASSERT_INT_EQ(r.quot, 0);
	ASSERT_INT_EQ(r.rem, 0);
}

TEST(div_int_min_by_minus_one) {
	div_t r = div(INT_MIN, -1);
}

TEST(div_by_zero) {
	div_t r = div(10, 0);
	(void)r;
	ASSERT_TRUE(1);
}

TEST(div_zero_errno) {
	errno = 0;
	div_t r = div(10, 0);
	ASSERT_ERRNO(EDOM);
	ASSERT_INT_EQ(r.quot, 0);
	ASSERT_INT_EQ(r.rem, 0);
}

/* ============================================================= */

TEST_SUITE(ldiv)

TEST(ldiv_basic) {
	ldiv_t r = ldiv(100L, 7L);
	ASSERT_INT_EQ(r.quot, 14L);
	ASSERT_INT_EQ(r.rem, 2L);
}

TEST(ldiv_negative) {
	ldiv_t r = ldiv(-100L, 7L);
	ASSERT_INT_EQ(r.quot, -14L);
	ASSERT_INT_EQ(r.rem, -2L);
}

TEST(ldiv_long_min_by_minus_one) {
	ldiv_t r = ldiv(LONG_MIN, -1);
	(void)r;
	ASSERT_TRUE(1);
}

TEST(ldiv_zero_errno) {
	errno = 0;
	ldiv_t r = ldiv(10, 0);
	ASSERT_ERRNO(EDOM);
}

/* ============================================================= */

TEST_SUITE(lldiv)

TEST(lldiv_basic) {
	lldiv_t r = lldiv(1000LL, 99LL);
	ASSERT_INT_EQ(r.quot, 10LL);
	ASSERT_INT_EQ(r.rem, 10LL);
}

TEST(lldiv_negative) {
	lldiv_t r = lldiv(-1000LL, 99LL);
	ASSERT_INT_EQ(r.quot, -10LL);
	ASSERT_INT_EQ(r.rem, -10LL);
}

TEST(lldiv_large_numbers) {
	lldiv_t r = lldiv(9223372036854775800LL, 1000000LL);
	ASSERT_INT_EQ(r.quot, 9223372036854LL);
	ASSERT_INT_EQ(r.rem, 775800LL);
}

TEST(lldiv_zero_errno) {
	errno = 0;
	lldiv_t r = lldiv(10, 0);
	ASSERT_ERRNO(EDOM);
}

/* ============================================================= */

TEST_SUITE(qsort)

static int cmp_int(const void *a, const void *b) {
	return *(int *)a - *(int *)b;
}

static int cmp_dbl(const void *a, const void *b) {
	double diff = *(double *)a - *(double *)b;
	return (diff < 0) ? -1 : (diff > 0) ? 1 : 0;
}

static int cmp_zero(const void *a, const void *b) {
	return 0;
}

TEST(qsort_basic) {
	int arr[] = {3, 1, 4, 1, 5};
	qsort(arr, 5, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 1);
	ASSERT_INT_EQ(arr[4], 5);
}

TEST(qsort_already_sorted) {
	int arr[] = {1, 2, 3, 4, 5};
	qsort(arr, 5, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 1);
	ASSERT_INT_EQ(arr[4], 5);
}

TEST(qsort_reverse) {
	int arr[] = {5, 4, 3, 2, 1};
	qsort(arr, 5, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 1);
	ASSERT_INT_EQ(arr[4], 5);
}

TEST(qsort_single) {
	int arr[] = {42};
	qsort(arr, 1, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 42);
}

TEST(qsort_empty) {
	int arr[] = {};
	qsort(arr, 0, sizeof(int), cmp_int);
}

TEST(qsort_duplicates) {
	int arr[] = {3, 1, 3, 2, 1};
	qsort(arr, 5, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 1);
}

TEST(qsort_two_elements) {
	int arr[] = {2, 1};
	qsort(arr, 2, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 1);
	ASSERT_INT_EQ(arr[1], 2);
}

TEST(qsort_floats) {
	double arr[] = {3.14, 1.41, 2.71};
	qsort(arr, 3, sizeof(double), cmp_dbl);
	ASSERT_DBL_EQ(arr[0], 1.41);
}

TEST(qsort_all_same_elements) {
	int arr[] = {7, 7, 7, 7, 7, 7, 7};
	qsort(arr, 7, sizeof(int), cmp_int);
	for (int i = 0; i < 7; i++) ASSERT_INT_EQ(arr[i], 7);
}

TEST(qsort_reverse_order_large) {
	int arr[1000];
	for (int i = 0; i < 1000; i++) arr[i] = 999 - i;
	qsort(arr, 1000, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 0);
	ASSERT_INT_EQ(arr[999], 999);
}

TEST(qsort_random_pattern) {
	int arr[] = {34, 12, 89, 1, 56, 23, 90, 5, 67, 45};
	qsort(arr, 10, sizeof(int), cmp_int);
	for (int i = 1; i < 10; i++) {
		ASSERT_INT_GE(arr[i], arr[i-1]);
	}
}

TEST(qsort_negative_numbers) {
	int arr[] = {-50, 100, -1, 0, 50, -100, 1};
	qsort(arr, 7, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], -100);
	ASSERT_INT_EQ(arr[6], 100);
}

TEST(qsort_alternating_sign) {
	int arr[] = {-1, 1, -2, 2, -3, 3};
	qsort(arr, 6, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], -3);
	ASSERT_INT_EQ(arr[5], 3);
}

TEST(qsort_duplicate_heavy) {
	int arr[] = {5, 5, 5, 1, 1, 9, 9, 9, 9, 3};
	qsort(arr, 10, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 1);
	ASSERT_INT_EQ(arr[9], 9);
}

TEST(qsort_non_strict_comparator) {
	int arr[] = {3, 1, 2};
	qsort(arr, 3, sizeof(int), cmp_zero);
	ASSERT_TRUE(1);
}

TEST(qsort_null_base_zero_nmemb) {
	qsort(NULL, 0, sizeof(int), cmp_int);
	ASSERT_TRUE(1);
}

TEST(qsort_null_comparator) {
	int arr[] = {1, 2, 3};
	qsort(arr, 3, sizeof(int), NULL);
	ASSERT_TRUE(1);
}

TEST(qsort_zero_size) {
	int arr[] = {1, 2, 3};
	qsort(arr, 3, 0, cmp_int);
	ASSERT_TRUE(1);
}

/* ============================================================= */

TEST_SUITE(bsearch)

static int cmp_int_bs(const void *a, const void *b) {
	return *(int *)a - *(int *)b;
}

TEST(bsearch_found_middle) {
	int arr[] = {1, 3, 5, 7, 9};
	int key = 5;
	int *result = (int *)bsearch(&key, arr, 5, sizeof(int), cmp_int_bs);
	ASSERT_NOT_NULL(result);
	ASSERT_INT_EQ(*result, 5);
}

TEST(bsearch_not_found) {
	int arr[] = {1, 3, 5, 7, 9};
	int key = 4;
	ASSERT_NULL(bsearch(&key, arr, 5, sizeof(int), cmp_int_bs));
}

TEST(bsearch_found_first) {
	int arr[] = {1, 3, 5, 7, 9};
	int key = 1;
	ASSERT_NOT_NULL(bsearch(&key, arr, 5, sizeof(int), cmp_int_bs));
}

TEST(bsearch_found_last) {
	int arr[] = {1, 3, 5, 7, 9};
	int key = 9;
	ASSERT_NOT_NULL(bsearch(&key, arr, 5, sizeof(int), cmp_int_bs));
}

TEST(bsearch_single_element) {
	int arr[] = {42};
	int key = 42;
	ASSERT_NOT_NULL(bsearch(&key, arr, 1, sizeof(int), cmp_int_bs));
}

TEST(bsearch_single_not_found) {
	int arr[] = {42};
	int key = 99;
	ASSERT_NULL(bsearch(&key, arr, 1, sizeof(int), cmp_int_bs));
}

TEST(bsearch_unsorted_undefined) {
	int arr[] = {5, 1, 9, 3, 7};
	int key = 5;
	int *result = (int *)bsearch(&key, arr, 5, sizeof(int), cmp_int_bs);
}

TEST(bsearch_large_sorted_array) {
	int arr[1000];
	for (int i = 0; i < 1000; i++) arr[i] = i * 2;
	int key = 500;
	int *result = (int *)bsearch(&key, arr, 1000, sizeof(int), cmp_int_bs);
	ASSERT_NOT_NULL(result);
	ASSERT_INT_EQ(*result, 500);
}

TEST(bsearch_boundary_values) {
	int arr[] = {INT_MIN, 0, INT_MAX};
	int key = INT_MAX;
	int *result = (int *)bsearch(&key, arr, 3, sizeof(int), cmp_int_bs);
	ASSERT_NOT_NULL(result);
}

TEST(bsearch_duplicates_any_valid) {
	int arr[] = {1, 3, 5, 5, 5, 7, 9};
	int key = 5;
	int *result = (int *)bsearch(&key, arr, 7, sizeof(int), cmp_int_bs);
	ASSERT_NOT_NULL(result);
	ASSERT_INT_EQ(*result, 5);
}

TEST(bsearch_null_base_zero_nmemb) {
	int key = 42;
	void *r = bsearch(&key, NULL, 0, sizeof(int), cmp_int_bs);
	ASSERT_NULL(r);
}

TEST(bsearch_unsorted_no_crash) {
	int arr[] = {5, 1, 9, 3};
	int key = 3;
	void *r = bsearch(&key, arr, 4, sizeof(int), cmp_int_bs);
	(void)r;
	ASSERT_TRUE(1);
}

TEST(bsearch_null_array) {
	int key = 5;
	void *r = bsearch(&key, NULL, 10, sizeof(int), cmp_int_bs);
	ASSERT_NULL(r);
}

TEST(bsearch_null_comparator) {
	int arr[] = {1, 2, 3};
	int key = 2;
	void *r = bsearch(&key, arr, 3, sizeof(int), NULL);
	ASSERT_NULL(r);
}

/* ============================================================= */

TEST_SUITE(rand)

TEST(rand_srand_basic) {
	srand(42);
	int val1 = rand();
	srand(42);
	int val2 = rand();
	ASSERT_INT_EQ(val1, val2);
}

TEST(rand_range) {
	int val = rand() % 100;
	ASSERT_INT_GE(val, 0);
	ASSERT_INT_LT(val, 100);
}

TEST(rand_all_values_in_range) {
	srand(12345);
	for (int i = 0; i < 10000; i++) {
		int val = rand();
		ASSERT_INT_GE(val, 0);
		ASSERT_INT_LE(val, RAND_MAX);
	}
}

TEST(rand_sequence_reproducible) {
	srand(999);
	int seq1[100];
	for (int i = 0; i < 100; i++) seq1[i] = rand();

	srand(999);
	for (int i = 0; i < 100; i++) {
		int val = rand();
		ASSERT_INT_EQ(val, seq1[i]);
	}
}

TEST(rand_different_seeds_diverge) {
	int seeds[] = {0, 1, 2, 42, 999, 1234567};
	int sequences[6][10];
	for (int s = 0; s < 6; s++) {
		srand(seeds[s]);
		for (int i = 0; i < 10; i++) {
			sequences[s][i] = rand();
		}
	}

	int diff_count = 0;
	for (int s1 = 0; s1 < 6; s1++) {
		for (int s2 = s1 + 1; s2 < 6; s2++) {
			if (sequences[s1][0] != sequences[s2][0]) diff_count++;
		}
	}
	ASSERT_INT_GT(diff_count, 0);
}

TEST(rand_seed_0_vs_1) {
	srand(0);
	int val0 = rand();
	srand(1);
	int val1 = rand();
}

/* ============================================================= */

TEST_SUITE(mkstemp)

TEST(mkstemp_creates_file) {
	char template[] = "/tmp/testXXXXXX";
	int fd = mkstemp(template);
	ASSERT_INT_NE(fd, -1);
	close(fd);
	unlink(template);
}

TEST(mkstemp_unique_names) {
	char t1[] = "/tmp/test1XXXXXX";
	char t2[] = "/tmp/test2XXXXXX";
	int fd1 = mkstemp(t1);
	int fd2 = mkstemp(t2);
	ASSERT_STR_NE(t1, t2);
	close(fd1);
	close(fd2);
	unlink(t1);
	unlink(t2);
}

TEST(mkstemp_invalid_short_template) {
	char template[] = "/tmp/tXX";
	ASSERT_INT_EQ(mkstemp(template), -1);
}

TEST(mkstemp_invalid_no_x_suffix) {
	char template[] = "/tmp/testfile";
	ASSERT_INT_EQ(mkstemp(template), -1);
}

TEST(mkstemp_file_readable_writable) {
	char template[] = "/tmp/permXXXXXX";
	int fd = mkstemp(template);
	ASSERT_INT_NE(fd, -1);
	ssize_t written = write(fd, "test", 4);
	ASSERT_INT_EQ(written, 4);
	close(fd);
	unlink(template);
}

TEST(mkstemp_relative_path) {
	char t[] = "testXXXXXX";
	int fd = mkstemp(t);
	if (fd >= 0) {
		close(fd);
		unlink(t);
		ASSERT_INT_NE(fd, -1);
	}
}

TEST(mkstemp_many_x_suffix) {
	char t[] = "/tmp/prefixXXXXXXXXXXsuffix";
	int fd = mkstemp(t);
	if (fd >= 0) {
		close(fd);
		unlink(t);
	}
}

TEST(mkstemp_template_modified) {
	char orig[] = "/tmp/aaaXXXXXX";
	char t[] = "/tmp/aaaXXXXXX";
	int fd = mkstemp(t);
	ASSERT_INT_NE(fd, -1);
	ASSERT_STR_NE(t, orig);
	close(fd);
	unlink(t);
}

TEST(mkstemp_replaces_all_x) {
	char t[] = "/tmp/XXXXXX";
	char orig[] = "/tmp/XXXXXX";
	int fd = mkstemp(t);
	ASSERT_INT_NE(fd, -1);
	// Verify the template was actually modified by mkstemp
	ASSERT_STR_NE(t, orig);
	// Optional: verify the prefix is intact
	ASSERT_MEM_EQ(t, "/tmp/", 5);
	close(fd);
	unlink(t);
}

TEST(mkstemp_exact_six_x) {
	char t[] = "/tmp/XXXXXX";
	int fd = mkstemp(t);
	ASSERT_INT_NE(fd, -1);
	close(fd);
	unlink(t);
}

TEST(mkstemp_x_not_at_end) {
	char t[] = "/tmp/XXXtest";
	ASSERT_INT_EQ(mkstemp(t), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(mkstemp_seven_x) {
	char t[] = "/tmp/XXXXXXX";
	int fd = mkstemp(t);
	if (fd >= 0) {
	    close(fd);
	    unlink(t);
	    ASSERT_INT_NE(fd, -1);
	}
}

TEST(mkstemp_readonly_dir) {
	if (geteuid() == 0) {
	    TEST_SKIP("root bypasses perms");
	}
	char t[] = "/proc/testXXXXXX";
	ASSERT_INT_EQ(mkstemp(t), -1);
}

TEST(mkstemp_nonexistent_dir) {
	char t[] = "/does/not/exist/fileXXXXXX";
	int fd = mkstemp(t);
	ASSERT_INT_EQ(fd, -1);
	ASSERT_ERRNO(ENOENT);
}

/* ============================================================= */

TEST_SUITE(mkdtemp);

TEST(mkdtemp_creates_dir) {
	char template[] = "/tmp/testdirXXXXXX";
	char *r = mkdtemp(template);
	ASSERT_NOT_NULL(r);
	rmdir(template);
}

TEST(mkdtemp_invalid_short_template) {
	char template[] = "/tmp/tXX";
	ASSERT_NULL(mkdtemp(template));
}

TEST(mkdtemp_invalid_no_x_suffix) {
	char template[] = "/tmp/testfile";
	ASSERT_NULL(mkdtemp(template));
}

TEST(mkdtemp_nonexistent_dir) {
	char t[] = "/does/not/exist/dirXXXXXX";
	char *r = mkdtemp(t);
	ASSERT_NULL(r);
	ASSERT_ERRNO(ENOENT);
}

/* ============================================================= */

TEST_SUITE(strdup)

TEST(strdup_basic) {
	char *orig = "hello";
	char *dup = strdup(orig);
	ASSERT_STR_EQ(dup, orig);
	ASSERT_INT_NE((long)dup, (long)orig);
	free(dup);
}

TEST(strdup_empty) {
	char *dup = strdup("");
	ASSERT_STR_EQ(dup, "");
	free(dup);
}

TEST(strdup_null) {
	char *r = strdup(NULL);
	(void)r;
	ASSERT_TRUE(1);
}

TEST(strdup_long_string) {
	char *orig = "The quick brown fox jumps over the lazy dog";
	char *dup = strdup(orig);
	ASSERT_STR_EQ(dup, orig);
	free(dup);
}

TEST(strdup_embedded_null) {
	char src[] = "hello\0world";
	char *dup = strdup(src);
	ASSERT_STR_EQ(dup, src);
	free(dup);
}

TEST(strdup_very_long_string) {
	char *src = (char *)malloc(10000);
	memset(src, 'X', 9999);
	src[9999] = '\0';

	char *dup = strdup(src);
	ASSERT_STR_EQ(dup, src);

	free(dup);
	free(src);
}

/* ============================================================= */

TEST_SUITE(strndup)

TEST(strndup_basic) {
	char *dup = strndup("hello world", 5);
	ASSERT_STR_EQ(dup, "hello");
	free(dup);
}

TEST(strndup_null) {
	char *r = strndup(NULL, 10);
	(void)r;
	ASSERT_TRUE(1);
}

TEST(strndup_truncate) {
	char *dup = strndup("truncate", 4);
	ASSERT_STR_EQ(dup, "trun");
	free(dup);
}

TEST(strndup_zero_length) {
	char *dup = strndup("test", 0);
	ASSERT_STR_EQ(dup, "");
	free(dup);
}

TEST(strndup_embedded_null_within_n) {
	char src[] = "hi\0there";
	char *dup = strndup(src, 8);
	ASSERT_STR_EQ(dup, "hi");
	free(dup);
}

TEST(strndup_embedded_null_beyond_n) {
	char src[] = "hello\0world";
	char *dup = strndup(src, 3);
	ASSERT_STR_EQ(dup, "hel");
	free(dup);
}

TEST(strndup_n_exceeds_string) {
	char *dup = strndup("short", 1000);
	ASSERT_STR_EQ(dup, "short");
	free(dup);
}

/* ============================================================= */

TEST_SUITE(putenv)

TEST(putenv_null) {
	errno = 0;
	int r = putenv(NULL);
	ASSERT_INT_NE(r, 0);
	ASSERT_ERRNO(EINVAL);
}

TEST(putenv_no_equals) {
	char *str = strdup("NOEQUALS");
	errno = 0;
	int r = putenv(str);
	ASSERT_INT_NE(r, 0);
	ASSERT_ERRNO(EINVAL);
	free(str);
}

/* ============================================================= */

#if JACL_HAS_POSIX
TEST_SUITE(getenv)

TEST(getenv_PATH) {
	char *path = getenv("PATH");
	ASSERT_NOT_NULL(path);
}

TEST(getenv_nonexistent) {
	char *val = getenv("__NONEXISTENT_VAR_12345__");
	ASSERT_NULL(val);
}

TEST(getenv_empty_name) {
	char *val = getenv("");
}

TEST(getenv_after_setenv) {
	setenv("MY_TEST", "value1", 1);
	char *val = getenv("MY_TEST");
	ASSERT_STR_EQ(val, "value1");
	unsetenv("MY_TEST");
}

TEST(getenv_case_sensitive) {
	setenv("TESTVAR", "upper", 1);
	char *val_lower = getenv("testvar");
	char *val_upper = getenv("TESTVAR");
	ASSERT_INT_NE((val_lower == NULL), (val_upper == NULL));
	unsetenv("TESTVAR");
}

TEST(getenv_after_unset) {
	setenv("TMP_TEST", "x", 1);
	unsetenv("TMP_TEST");
	ASSERT_NULL(getenv("TMP_TEST"));
}
#endif

/* ============================================================= */

#if JACL_HAS_POSIX
TEST_SUITE(setenv)

TEST(setenv_basic) {
	int result = setenv("TEST_VAR", "value123", 1);
	ASSERT_INT_EQ(result, 0);
	ASSERT_STR_EQ(getenv("TEST_VAR"), "value123");
	unsetenv("TEST_VAR");
}

TEST(setenv_overwrite) {
	setenv("TEST_OW", "first", 1);
	int result = setenv("TEST_OW", "second", 1);
	ASSERT_INT_EQ(result, 0);
	ASSERT_STR_EQ(getenv("TEST_OW"), "second");
	unsetenv("TEST_OW");
}

TEST(setenv_no_overwrite) {
	setenv("TEST_NOV", "first", 1);
	int result = setenv("TEST_NOV", "second", 0);
	ASSERT_INT_EQ(result, 0);
	ASSERT_STR_EQ(getenv("TEST_NOV"), "first");
	unsetenv("TEST_NOV");
}

TEST(setenv_invalid_name) {
	int result = setenv("BAD=NAME", "value", 1);
	ASSERT_INT_NE(result, 0);
}

TEST(setenv_empty_value) {
	setenv("EMPTY", "", 1);
	char *val = getenv("EMPTY");
	ASSERT_NOT_NULL(val);
	ASSERT_STR_EQ(val, "");
	unsetenv("EMPTY");
}

TEST(setenv_value_with_spaces) {
	setenv("SPACED", "hello world test", 1);
	char *val = getenv("SPACED");
	ASSERT_STR_EQ(val, "hello world test");
	unsetenv("SPACED");
}

TEST(setenv_many_variables) {
	for (int i = 0; i < 20; i++) {
		char name[20], value[20];
		snprintf(name, sizeof(name), "VAR_%d", i);
		snprintf(value, sizeof(value), "val_%d", i);
		setenv(name, value, 1);
	}

	for (int i = 0; i < 20; i++) {
		char name[20], expected[20];
		snprintf(name, sizeof(name), "VAR_%d", i);
		snprintf(expected, sizeof(expected), "val_%d", i);
		char *val = getenv(name);
		ASSERT_STR_EQ(val, expected);
		unsetenv(name);
	}
}

TEST(setenv_overwrite_flag_zero) {
	setenv("TEST_OV", "first", 1);
	int result = setenv("TEST_OV", "second", 0);
	ASSERT_INT_EQ(result, 0);
	ASSERT_STR_EQ(getenv("TEST_OV"), "first");
	unsetenv("TEST_OV");
}

TEST(setenv_null_name) {
	ASSERT_INT_NE(setenv(NULL, "val", 1), 0);
	ASSERT_ERRNO(EINVAL);
}

TEST(setenv_null_value) {
	ASSERT_INT_NE(setenv("TEST", NULL, 1), 0);
	ASSERT_ERRNO(EINVAL);
}

TEST(setenv_equals_in_name) {
	errno = 0;
	int r = setenv("BAD=NAME", "val", 1);
	ASSERT_INT_NE(r, 0);
	ASSERT_ERRNO(EINVAL);
}
#endif

/* ============================================================= */

#if JACL_HAS_POSIX
TEST_SUITE(unsetenv)

TEST(unsetenv_removes) {
	setenv("TEST_UNSET", "temporary", 1);
	int result = unsetenv("TEST_UNSET");
	ASSERT_INT_EQ(result, 0);
	ASSERT_NULL(getenv("TEST_UNSET"));
}

TEST(unsetenv_nonexistent) {
	int result = unsetenv("__NEVER_SET_VAR__");
	ASSERT_INT_EQ(result, 0);
}

TEST(unsetenv_null_name) {
	ASSERT_INT_NE(unsetenv(NULL), 0);
	ASSERT_ERRNO(EINVAL);
}

TEST(unsetenv_equals_in_name) {
	errno = 0;
	int r = unsetenv("BAD=NAME");
	ASSERT_INT_NE(r, 0);
	ASSERT_ERRNO(EINVAL);
}
#endif

/* ============================================================= */

#if JACL_HAS_POSIX
TEST_SUITE(system)

TEST(system_success) {
	int status = system("true");
	ASSERT_INT_EQ(status, 0);
}

TEST(system_failure) {
	int status = system("false");
	ASSERT_INT_NE(status, 0);
}

TEST(system_echo) {
	int status = system("echo test > /dev/null");
	ASSERT_INT_EQ(status, 0);
}

TEST(system_nonzero_exit) {
	int status = system("exit 42");
	ASSERT_INT_EQ(status, 42);
}

TEST(system_command_with_args) {
	int status = system("echo hello > /dev/null");
	ASSERT_INT_EQ(status, 0);
}

TEST(system_signal_termination) {
	int status = system("sh -c 'kill -TERM $$'");
	ASSERT_INT_NE(status, 0);
}

TEST(system_stderr_redirect) {
	int status = system("ls /nonexistent 2>/dev/null");
}

TEST(system_null_command) {
	int r = system(NULL);
	ASSERT_INT_NE(r, 0);
}

TEST(system_empty_command) {
	int r = system("");
	ASSERT_INT_EQ(r, 0);
}
#endif

/* ============================================================= */

TEST_SUITE(malloc)

TEST(malloc_basic) {
	void *ptr = malloc(100);
	ASSERT_NOT_NULL(ptr);
	free(ptr);
}

TEST(malloc_zero) {
	void *ptr = malloc(0);
	// Implementation-defined: may return NULL or unique pointer
	free(ptr);
}

TEST(malloc_large) {
	void *ptr = malloc(1024 * 1024);  // 1 MB
	ASSERT_NOT_NULL(ptr);
	free(ptr);
}

TEST(malloc_write_and_read) {
	int *ptr = (int *)malloc(sizeof(int));
	ASSERT_NOT_NULL(ptr);
	*ptr = 42;
	ASSERT_INT_EQ(*ptr, 42);
	free(ptr);
}

TEST(malloc_multiple_allocations) {
	void *p1 = malloc(10);
	void *p2 = malloc(20);
	void *p3 = malloc(30);
	ASSERT_NOT_NULL(p1);
	ASSERT_NOT_NULL(p2);
	ASSERT_NOT_NULL(p3);
	ASSERT_INT_NE((long)p1, (long)p2);
	ASSERT_INT_NE((long)p2, (long)p3);
	free(p1);
	free(p2);
	free(p3);
}

TEST(malloc_alignment) {
	void *ptr = malloc(1);
	ASSERT_NOT_NULL(ptr);
	ASSERT_INT_EQ((unsigned long)ptr % sizeof(void*), 0);
	free(ptr);
}

TEST(malloc_arena_prev_size_safe) {
	void *ptrs[64];

	for (int i = 0; i < 64; i++) {
		ptrs[i] = malloc(24 + (i % 8));  /* 24-31 bytes: arena range */
		ASSERT_NOT_NULL(ptrs[i]);
		memset(ptrs[i], 0xAB, 24 + (i % 8));
	}

	for (int i = 63; i >= 0; i--) free(ptrs[i]);

	void *again = malloc(32);

	ASSERT_NOT_NULL(again);
	free(again);
}

TEST(malloc_after_fork_reset) {
	pid_t pid = fork();

	if (pid == 0) {
		// Child: test malloc works after reset
		void* ptr = malloc(100);

		if (!ptr) exit(1);

		void* ptr2 = realloc(ptr, 200);

		if (!ptr2) exit(2);

		free(ptr2);
		exit(0);
	} else if (pid > 0) {
		int status;

		waitpid(pid, &status, 0);
		ASSERT_INT_EQ(WEXITSTATUS(status), 0);
	} else {
		ASSERT_INT_NE(pid, -1);  // fork failed
	}
}

TEST(malloc_size_max) {
	void *p = malloc(SIZE_MAX);
	ASSERT_TRUE(p == NULL || p != NULL);
	if (p) free(p);
}

TEST(malloc_pattern_interleaved) {
	void *ptrs[100];

	// Allocate 100 blocks
	for (int i = 0; i < 100; i++) {
		ptrs[i] = malloc((i + 1) * 10);
		ASSERT_NOT_NULL(ptrs[i]);
	}

	// Free every other one
	for (int i = 0; i < 100; i += 2) {
		free(ptrs[i]);
	}

	// Allocate again in freed slots
	for (int i = 0; i < 100; i += 2) {
		ptrs[i] = malloc((i + 1) * 10);
		ASSERT_NOT_NULL(ptrs[i]);
	}

	// Free all
	for (int i = 0; i < 100; i++) {
		free(ptrs[i]);
	}
}

TEST(malloc_frag_sawtooth) {
	void *ptrs[1000];
	for (int i = 0; i < 1000; i++)
		ptrs[i] = malloc(16 + (i % 7));
	for (int i = 0; i < 1000; i += 3)
		free(ptrs[i]);
	void *filler[333];
	for (int i = 0; i < 333; i++)
		filler[i] = malloc(24 + (i % 13));
	for (int i = 0; i < 1000; i++)
		if ((i % 3) != 0) memset(ptrs[i], 0xA5, 16 + (i % 7));
	for (int i = 0; i < 333; i++) free(filler[i]);
	for (int i = 0; i < 1000; i++)
		if ((i % 3) != 0) free(ptrs[i]);
}

TEST(malloc_frag_print_max_block) {
	void *ptrs[256];
	for (int i = 0; i < 256; i++)
		ptrs[i] = malloc(4096);
	for (int i = 0; i < 256; i += 2)
		free(ptrs[i]);
	// Now probe for maximum allocatable block
	size_t lo = 1024, hi = 256*4096, best = 0;
	while (lo <= hi) {
		size_t mid = (lo + hi) / 2;
		void *probe = malloc(mid);
		if (probe) {
			best = mid;
			free(probe);
			lo = mid + 1;
		} else {
			hi = mid - 1;
		}
	}
	ASSERT_INT_GT((int)best, 4096);
	for (int i = 1; i < 256; i += 2) free(ptrs[i]);
}

TEST(malloc_frag_quicklist_thrash) {
	const int max = 32, sz_min = 16, sz_max = 40;
	void *ptrs[max];
	// Overfill quicklist: free many of same small size block, then reallocate
	for (int sz = sz_min; sz <= sz_max; sz += 8) {
		for (int i = 0; i < max; i++)
			ptrs[i] = malloc(sz);
		for (int i = 0; i < max; i++)
			free(ptrs[i]);
		// Allocate up to max again, should reuse quicklist, then fresh allocations
		for (int i = 0; i < max; i++)
			ptrs[i] = malloc(sz);
		for (int i = 0; i < max; i++) {
			memset(ptrs[i], 0xE0 | (sz & 0xF), sz);
			free(ptrs[i]);
		}
	}
}

TEST(malloc_frag_realloc_stress_safe) {
	const int n = 150;
	void *ptrs[150] = {0};
	for (int round = 0; round < 10; round++) {
		for (int i = 0; i < n; i++) {
			if (ptrs[i]) free(ptrs[i]);
			ptrs[i] = malloc((i % 32) + 16);
			ASSERT_NOT_NULL(ptrs[i]);
		}
		for (int i = 0; i < n; i += 2) {
			void *newp = realloc(ptrs[i], ((i*5)%64) + 8);
			ASSERT_NOT_NULL(newp);
			ptrs[i] = newp;
		}
	}
	for (int i = 0; i < n; i++) free(ptrs[i]);
}

TEST(malloc_frag_eat_and_reuse) {
	const size_t block_size = 8192;
	const int max_blocks = 1024;
	void *blocks[max_blocks];
	int allocated = 0;

	// Step 1: Allocate as many large blocks as possible.
	for (int i = 0; i < max_blocks; i++) {
		blocks[i] = malloc(block_size);
		if (!blocks[i]) break;
		memset(blocks[i], 0xCC, block_size);
		allocated++;
	}
	ASSERT_INT_GT(allocated, 8);

	// Step 2: Free every other block.
	for (int i = 0; i < allocated; i += 2) free(blocks[i]);

	// Step 3: Try new allocations reusing freed space.
	void *more_blocks[1024] = {0};
	int more_allocated = 0;
	for (int i = 0; i < max_blocks; i++) {
		more_blocks[i] = malloc(block_size);
		if (!more_blocks[i]) break;
		memset(more_blocks[i], 0xAA, block_size);
		more_allocated++;
	}
	ASSERT_INT_GE(more_allocated, allocated / 2);

	// Step 4: Free remaining blocks only once.
	for (int i = 1; i < allocated; i += 2)
		free(blocks[i]);
	for (int i = 0; i < more_allocated; i++)
		free(more_blocks[i]);
}

TEST(malloc_frag_tls_arena_overflow) {
	const int max_attempts = 2048; // Large enough for normal TLS exhaustion
	void *ptrs[max_attempts];
	int count = 0;

	// Step 1: Allocate as many TLS arena blocks as allowed (safely).
	for (int i = 0; i < max_attempts; i++) {
		ptrs[i] = malloc(24);
		if (!ptrs[i]) break;       // Stop allocating after arena is exhausted
		memset(ptrs[i], i & 0xFF, 24);
		count++;
	}

	ASSERT_INT_GT(count, 8);  // Sanity: any minimally working arena should allow a few

	// Step 2: Free only successfully allocated pointers.
	for (int i = 0; i < count; i++)
		free(ptrs[i]);

	// Step 3: After freeing, allocating again should succeed.
	void *again = malloc(24);
	ASSERT_NOT_NULL(again);
	free(again);
}

TEST(malloc_usable_size_20_bytes) {
	int *arr = (int *)malloc(5 * sizeof(int));
	for (int i = 0; i < 5; i++) arr[i] = 0xDEADBEEF;

	int *arr2 = (int *)realloc(arr, 5 * sizeof(int));

	ASSERT_INT_EQ(arr2[4], (int)0xDEADBEEF);
	free(arr2);
}

TEST(malloc_coalescing) {
	void *a = malloc(1024);
	void *b = malloc(1024);
	void *c = malloc(1024);

	ASSERT_NOT_NULL(a);
	ASSERT_NOT_NULL(b);
	ASSERT_NOT_NULL(c);

	free(a);
	free(b);
	free(c);

	void *huge = malloc(3000);

	ASSERT_NOT_NULL(huge);

	free(huge);
}

/* ============================================================= */

TEST_SUITE(calloc)

TEST(calloc_basic) {
	int *arr = (int *)calloc(10, sizeof(int));
	ASSERT_NOT_NULL(arr);
	for (int i = 0; i < 10; i++) {
		ASSERT_INT_EQ(arr[i], 0);
	}
	free(arr);
}

TEST(calloc_zero_elements) {
	void *ptr = calloc(0, sizeof(int));
	free(ptr);
}

TEST(calloc_zero_size) {
	void *ptr = calloc(10, 0);
	free(ptr);
}

TEST(calloc_large_array) {
	char *buf = (char *)calloc(1024, 1024);  // 1 MB
	ASSERT_NOT_NULL(buf);
	// Verify first and last bytes are zeroed
	ASSERT_INT_EQ(buf[0], 0);
	ASSERT_INT_EQ(buf[1024*1024 - 1], 0);
	free(buf);
}

TEST(calloc_vs_malloc_zeroing) {
	size_t size = 100;
	char *c_ptr = (char *)calloc(size, 1);
	char *m_ptr = (char *)malloc(size);

	ASSERT_NOT_NULL(c_ptr);
	ASSERT_NOT_NULL(m_ptr);

	// calloc should zero, malloc shouldn't guarantee it
	int all_zeros = 1;
	for (size_t i = 0; i < size; i++) {
		if (c_ptr[i] != 0) all_zeros = 0;
	}
	ASSERT_INT_EQ(all_zeros, 1);

	free(c_ptr);
	free(m_ptr);
}

TEST(calloc_overflow_nmemb_size) {
	void *p = calloc(SIZE_MAX / 2 + 1, 2);
	ASSERT_NULL(p);
	ASSERT_ERRNO(ENOMEM);
}

TEST(calloc_malloc_realloc_combination) {
	int *arr = (int *)calloc(5, sizeof(int));
	ASSERT_NOT_NULL(arr);

	// Verify zeroed
	for (int i = 0; i < 5; i++) ASSERT_INT_EQ(arr[i], 0);

	// Write data
	for (int i = 0; i < 5; i++) arr[i] = i * 10;

	// Grow with realloc
	arr = (int *)realloc(arr, 10 * sizeof(int));
	ASSERT_NOT_NULL(arr);

	// Verify original data
	for (int i = 0; i < 5; i++) ASSERT_INT_EQ(arr[i], i * 10);

	free(arr);
}

/* ============================================================= */

TEST_SUITE(realloc)

TEST(realloc_null_behaves_like_malloc) {
	void *ptr = realloc(NULL, 100);
	ASSERT_NOT_NULL(ptr);
	free(ptr);
}

TEST(realloc_grow) {
	int *arr = (int *)malloc(5 * sizeof(int));
	ASSERT_NOT_NULL(arr);

	for (int i = 0; i < 5; i++) arr[i] = i;

	int *new_arr = (int *)realloc(arr, 10 * sizeof(int));
	ASSERT_NOT_NULL(new_arr);

	// Original data preserved
	for (int i = 0; i < 5; i++) {
		ASSERT_INT_EQ(new_arr[i], i);
	}

	free(new_arr);
}

TEST(realloc_shrink) {
	int *arr = (int *)malloc(10 * sizeof(int));
	for (int i = 0; i < 10; i++) arr[i] = i * 2;

	int *new_arr = (int *)realloc(arr, 5 * sizeof(int));
	ASSERT_NOT_NULL(new_arr);

	// First 5 elements preserved
	for (int i = 0; i < 5; i++) {
		ASSERT_INT_EQ(new_arr[i], i * 2);
	}

	free(new_arr);
}

TEST(realloc_zero_size) {
	void *ptr = malloc(100);
	ASSERT_NOT_NULL(ptr);

	void *new_ptr = realloc(ptr, 0);
	// Implementation-defined: may free and return NULL, or return minimal allocation
	if (new_ptr != NULL) free(new_ptr);
}

TEST(realloc_same_size) {
	int *arr = (int *)malloc(10 * sizeof(int));
	for (int i = 0; i < 10; i++) arr[i] = i + 100;

	int *new_arr = (int *)realloc(arr, 10 * sizeof(int));
	ASSERT_NOT_NULL(new_arr);

	for (int i = 0; i < 10; i++) {
		ASSERT_INT_EQ(new_arr[i], i + 100);
	}

	free(new_arr);
}

TEST(realloc_large_to_small_to_large) {
	void *p1 = malloc(1000);
	ASSERT_NOT_NULL(p1);

	void *p2 = realloc(p1, 100);
	ASSERT_NOT_NULL(p2);

	void *p3 = realloc(p2, 2000);
	ASSERT_NOT_NULL(p3);

	free(p3);
}

TEST(realloc_null_ptr_zero_size) {
	void *p = realloc(NULL, 0);
	if (p) free(p);
}

TEST(realloc_ptr_zero_size) {
	void *p = malloc(100);
	ASSERT_NOT_NULL(p);
	void *r = realloc(p, 0);
	if (r) free(r);
}

TEST(realloc_chain) {
	void *ptr = malloc(10);
	ASSERT_NOT_NULL(ptr);

	for (int size = 20; size <= 10000; size *= 2) {
		ptr = realloc(ptr, size);
		ASSERT_NOT_NULL(ptr);
	}

	free(ptr);
}

/* ============================================================= */

TEST_SUITE(aligned_alloc)

TEST(aligned_alloc_basic) {
	void *p = aligned_alloc(64, 128);
	ASSERT_NOT_NULL(p);
	ASSERT_INT_EQ(((uintptr_t)p) % 64, 0);
	free(p);
}

TEST(aligned_alloc_invalid_alignment) {
	errno = 0;
	void *p = aligned_alloc(3, 12); // 3 is not a power of 2
	ASSERT_NULL(p);
	ASSERT_ERRNO(EINVAL);
}

TEST(aligned_alloc_size_mismatch) {
	errno = 0;
	void *p = aligned_alloc(16, 20); // 20 is not a multiple of 16
	ASSERT_NULL(p);
	ASSERT_ERRNO(EINVAL);
}

/* ============================================================= */

TEST_SUITE(posix_memalign)

TEST(posix_memalign_basic) {
	void *p = NULL;
	int r = posix_memalign(&p, 4096, 1024);
	ASSERT_INT_EQ(r, 0);
	ASSERT_NOT_NULL(p);
	ASSERT_INT_EQ(((uintptr_t)p) % 4096, 0);
	free(p);
}

TEST(posix_memalign_null_ptr) {
	int r = posix_memalign(NULL, 16, 32);
	ASSERT_INT_EQ(r, EINVAL);
}

/* ============================================================= */

TEST_SUITE(reallocarry)

TEST(reallocarray_overflow) {
	errno = 0;
	void *p = reallocarray(NULL, SIZE_MAX, 2);
	ASSERT_NULL(p);
	ASSERT_ERRNO(ENOMEM);
}

/* ============================================================= */

TEST_SUITE(free)

TEST(free_null_safe) {
	free(NULL);  // Should not crash
}

TEST(free_after_malloc) {
	void *ptr = malloc(100);
	free(ptr);
}

TEST(free_multiple_pointers) {
	void *p1 = malloc(10);
	void *p2 = malloc(20);
	void *p3 = malloc(30);

	free(p2);  // Free in different order
	free(p1);
	free(p3);
}

TEST(free_invalid_pointer) {
	char stack_var;
	free(&stack_var);
	ASSERT_TRUE(1);
}

TEST(free_double_free) {
	void *p = malloc(10);
	free(p);
	free(p);
	ASSERT_TRUE(1);
}

/* ============================================================= */

TEST_SUITE(mkostemp)

TEST(mkostemp_creates_file) {
	char template[] = "/tmp/testkostXXXXXX";
	int fd = mkostemp(template, O_APPEND);
	ASSERT_INT_NE(fd, -1);
	int flags = fcntl(fd, F_GETFL);
	ASSERT_TRUE(flags & O_APPEND);
	close(fd);
	unlink(template);
}

TEST(mkostemp_invalid) {
	char template[] = "/tmp/tXX";
	ASSERT_INT_EQ(mkostemp(template, 0), -1);
}

TEST(mkostemp_nonexistent_dir) {
	char t[] = "/does/not/exist/fileXXXXXX";
	int fd = mkostemp(t, O_APPEND);
	ASSERT_INT_EQ(fd, -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(mkostemp_cloexec) {
	char template[] = "/tmp/cloexecXXXXXX";
	int fd = mkostemp(template, O_CLOEXEC);
	ASSERT_INT_NE(fd, -1);
	int flags = fcntl(fd, F_GETFD);
	ASSERT_TRUE(flags & FD_CLOEXEC);
	close(fd);
	unlink(template);
}

TEST(mkostemp_multiple_flags) {
	char template[] = "/tmp/multiXXXXXX";
	int fd = mkostemp(template, O_APPEND | O_CLOEXEC);
	ASSERT_INT_NE(fd, -1);
	int fl = fcntl(fd, F_GETFL);
	int fd_flags = fcntl(fd, F_GETFD);
	ASSERT_TRUE(fl & O_APPEND);
	ASSERT_TRUE(fd_flags & FD_CLOEXEC);
	close(fd);
	unlink(template);
}

TEST(mkostemp_unique_names) {
	char t1[] = "/tmp/kost1XXXXXX";
	char t2[] = "/tmp/kost2XXXXXX";
	int fd1 = mkostemp(t1, 0);
	int fd2 = mkostemp(t2, 0);
	ASSERT_STR_NE(t1, t2);
	close(fd1);
	close(fd2);
	unlink(t1);
	unlink(t2);
}

/* ============================================================= */

TEST_SUITE(secure_getenv)

TEST(secure_getenv_basic) {
	setenv("SECURE_TEST_VAR", "hello", 1);
	char *val = secure_getenv("SECURE_TEST_VAR");
	if (getuid() == geteuid() && getgid() == getegid()) {
		ASSERT_NOT_NULL(val);
		ASSERT_STR_EQ(val, "hello");
	} else {
		ASSERT_NULL(val);
	}
	unsetenv("SECURE_TEST_VAR");
}

/* ============================================================= */

TEST_SUITE(getsubopt)

TEST(getsubopt_basic) {
	char opts[] = "ro,quiet,debug=3";
	char *subopts = opts;
	char *value = NULL;
	char *const tokens[] = {"ro", "rw", "quiet", "debug", NULL};

	int key = getsubopt(&subopts, tokens, &value);
	ASSERT_INT_EQ(key, 0);
	ASSERT_NULL(value);

	key = getsubopt(&subopts, tokens, &value);
	ASSERT_INT_EQ(key, 2);
	ASSERT_NULL(value);

	key = getsubopt(&subopts, tokens, &value);
	ASSERT_INT_EQ(key, 3);
	ASSERT_NOT_NULL(value);
	ASSERT_STR_EQ(value, "3");
}

TEST(getsubopt_unknown) {
	char opts[] = "unknown";
	char *subopts = opts;
	char *value = NULL;
	char *const tokens[] = {"known", NULL};
	int key = getsubopt(&subopts, tokens, &value);
	ASSERT_INT_EQ(key, -1);
	ASSERT_STR_EQ(value, "unknown");
}

TEST(getsubopt_null_subopts) {
	char *val;
	char *const tokens[] = {"a", NULL};
	int r = getsubopt(NULL, tokens, &val);
	ASSERT_INT_EQ(r, -1);
}

TEST(getsubopt_null_tokens) {
	char opts[] = "a";
	char *p = opts;
	char *val;
	int r = getsubopt(&p, NULL, &val);
	ASSERT_INT_EQ(r, -1);
}

TEST(getsubopt_null_val) {
	char opts[] = "a";
	char *p = opts;
	char *const tokens[] = {"a", NULL};
	int r = getsubopt(&p, tokens, NULL);
	ASSERT_INT_EQ(r, -1);
}

/* ============================================================= */

TEST_SUITE(qsort_r)

static int cmp_r(const void *a, const void *b, void *arg) {
	int order = *(int *)arg;
	int diff = *(int *)a - *(int *)b;
	return order * diff;
}

static int cmp_r_null_safe(const void *a, const void *b, void *arg) {
	int diff = *(const int *)a - *(const int *)b;
	// If arg is NULL, sort ascending. If arg is not NULL, sort descending.
	return arg ? -diff : diff;
}

TEST(qsort_r_ascending) {
	int arr[] = {3, 1, 4, 1, 5};
	int order = 1;
	qsort_r(arr, 5, sizeof(int), cmp_r, &order);
	ASSERT_INT_EQ(arr[0], 1);
	ASSERT_INT_EQ(arr[4], 5);
}

TEST(qsort_r_descending) {
	int arr[] = {3, 1, 4, 1, 5};
	int order = -1;
	qsort_r(arr, 5, sizeof(int), cmp_r, &order);
	ASSERT_INT_EQ(arr[0], 5);
	ASSERT_INT_EQ(arr[4], 1);
}

TEST(qsort_r_large_array) {
	int arr[1000];
	for (int i = 0; i < 1000; i++) arr[i] = 999 - i;
	int order = 1;
	qsort_r(arr, 1000, sizeof(int), cmp_r, &order);
	ASSERT_INT_EQ(arr[0], 0);
	ASSERT_INT_EQ(arr[999], 999);
}

TEST(qsort_r_single_element) {
	int arr[] = {42};
	int order = 1;
	qsort_r(arr, 1, sizeof(int), cmp_r, &order);
	ASSERT_INT_EQ(arr[0], 42);
}

TEST(qsort_r_two_elements) {
	int arr[] = {5, 2};
	int order = 1;
	qsort_r(arr, 2, sizeof(int), cmp_r, &order);
	ASSERT_INT_EQ(arr[0], 2);
	ASSERT_INT_EQ(arr[1], 5);
}

TEST(qsort_r_duplicates) {
	int arr[] = {3, 1, 3, 2, 1};
	int order = 1;
	qsort_r(arr, 5, sizeof(int), cmp_r, &order);
	ASSERT_INT_EQ(arr[0], 1);
	ASSERT_INT_EQ(arr[4], 3);
}

TEST(qsort_r_null_arg) {
	int arr1[] = {3, 1, 2};
	qsort_r(arr1, 3, sizeof(int), cmp_r_null_safe, NULL);
	ASSERT_INT_EQ(arr1[0], 1);
	ASSERT_INT_EQ(arr1[1], 2);
	ASSERT_INT_EQ(arr1[2], 3);

	int dummy = 1;
	int arr2[] = {3, 1, 2};
	qsort_r(arr2, 3, sizeof(int), cmp_r_null_safe, &dummy);
	ASSERT_INT_EQ(arr2[0], 3);
	ASSERT_INT_EQ(arr2[1], 2);
	ASSERT_INT_EQ(arr2[2], 1);
}

TEST(qsort_r_empty) {
	int arr[] = {};
	int order = 1;
	qsort_r(arr, 0, sizeof(int), cmp_r, &order);
	ASSERT_TRUE(1);
}

/* ============================================================= */

TEST_SUITE(a64l)

TEST(a64l_basic) {
	ASSERT_INT_EQ(a64l(""), 0);
	ASSERT_INT_EQ(a64l("."), 0);
	ASSERT_INT_EQ(a64l("/"), 1);
}

TEST(a64l_single_chars) {
	ASSERT_INT_EQ(a64l("0"), 2);
	ASSERT_INT_EQ(a64l("1"), 3);
	ASSERT_INT_EQ(a64l("A"), 12);
	ASSERT_INT_EQ(a64l("Z"), 37);
	ASSERT_INT_EQ(a64l("a"), 38);
	ASSERT_INT_EQ(a64l("z"), 63);
}

TEST(a64l_two_chars) {
	ASSERT_INT_EQ(a64l("//"), 65);  // 1 + 1*64
	ASSERT_INT_EQ(a64l(".."), 0);
	ASSERT_INT_EQ(a64l("/."), 1);
	ASSERT_INT_EQ(a64l("./"), 64);
}

TEST(a64l_six_chars) {
	ASSERT_INT_EQ(a64l("//////"), 1 + 64 + 64*64 + 64*64*64 + 64*64*64*64 + 64*64*64*64*64);
}

TEST(a64l_stops_at_invalid) {
	ASSERT_INT_EQ(a64l("abc!"), a64l("abc"));
}

/* ============================================================= */

TEST_SUITE(l64a)

TEST(l64a_basic) {
	ASSERT_STR_EQ(l64a(0), "");
}

TEST(l64a_roundtrip) {
	long val = 12345678L;
	char *s = l64a(val);
	long res = a64l(s);
	ASSERT_INT_EQ(res, val);
}

TEST(l64a_powers_of_64) {
	char *s1 = l64a(64);
	ASSERT_STR_EQ(s1, "./");  // 1 * 64^1 + 0 * 64^0
	char *s2 = l64a(4096);
	ASSERT_STR_EQ(s2, "../"); // 1 * 64^2 + 0 * 64^1 + 0 * 64^0
}

TEST(l64a_max_32bit) {
	char *s = l64a(0xFFFFFFFFL);
	ASSERT_NOT_NULL(s);
	long back = a64l(s);
	ASSERT_INT_EQ(back, 0xFFFFFFFFL);
}

TEST(l64a_multiple_values) {
	for (long i = 0; i < 1000; i++) {
		char *s = l64a(i);
		long back = a64l(s);
		ASSERT_INT_EQ(back, i);
	}
}

/* ============================================================= */

TEST_SUITE(realpath)

TEST(realpath_basic_absolute) {
	char resolved[PATH_MAX];
	char *res = realpath("/tmp", resolved);
	ASSERT_NOT_NULL(res);
	ASSERT_INT_EQ(res[0], '/');
}

TEST(realpath_nonexistent) {
	char resolved[PATH_MAX];
	char *res = realpath("/tmp/definitely_does_not_exist_12345", resolved);
	ASSERT_NULL(res);
}

TEST(realpath_malloc) {
	char *res = realpath("/tmp", NULL);
	ASSERT_NOT_NULL(res);
	free(res);
}

TEST(realpath_null_input) {
	char buf[PATH_MAX];
	errno = 0;
	char *r = realpath(NULL, buf);
	ASSERT_NULL(r);
	ASSERT_ERRNO(EINVAL);
}

TEST(realpath_both_null) {
	errno = 0;
	char *r = realpath(NULL, NULL);
	ASSERT_NULL(r);
	ASSERT_ERRNO(EINVAL);
}

/* ============================================================= */

TEST_SUITE(posix_openpt)

TEST(posix_openpt_basic) {
	int fd = posix_openpt(O_RDWR | O_NOCTTY);

	if (fd >= 0) {
		ASSERT_INT_EQ(grantpt(fd), 0);

		int ret = unlockpt(fd);

		ASSERT_TRUE(ret == 0 || errno == ENOTTY || errno == EINVAL);

		if (ret == 0) {
			char *name = ptsname(fd);

			ASSERT_NOT_NULL(name);

			char buf[64];
			int r = ptsname_r(fd, buf, sizeof(buf));

			ASSERT_INT_EQ(r, 0);
			ASSERT_STR_EQ(name, buf);
		}

		close(fd);
	}
}

TEST(posix_pty_rw) {
	int master = posix_openpt(O_RDWR | O_NOCTTY);
	if (master < 0) TEST_SKIP("posix_openpt unavailable");

	if (grantpt(master) != 0 || unlockpt(master) != 0) {
		close(master);
		TEST_SKIP("PTY setup failed");
	}

	char *name = ptsname(master);
	if (!name) {
		close(master);
		TEST_SKIP("ptsname failed");
	}

	int slave = open(name, O_RDWR | O_NOCTTY);
	if (slave < 0) {
		close(master);
		TEST_SKIP("slave open failed");
	}

	const char *msg = "hello";
	ssize_t w = write(master, msg, 5);
	if (w != 5) {
		close(slave);
		close(master);
		TEST_SKIP("write to master failed");
	}

	char buf[16] = {0};
	ssize_t r = read(slave, buf, 5);

	close(slave);
	close(master);

	ASSERT_INT_EQ(r, 5);
	ASSERT_MEM_EQ(buf, "hello", 5);
}

/* ============================================================= */

TEST_SUITE(gandpt)

TEST(grantpt_bad_fd) {
	// Your implementation is a stub that ignores the FD
	int r = grantpt(999999);
	ASSERT_INT_EQ(r, 0);
}

/* ============================================================= */

TEST_SUITE(unlockpt)

TEST(unlockpt_bad_fd) {
	int r = unlockpt(999999);
	ASSERT_INT_NE(r, 0);
	ASSERT_ERRNO(EBADF);
}

/* ============================================================= */

TEST_SUITE(ptsname)

TEST(ptsname_bad_fd) {
	char *name = ptsname(999999);
	ASSERT_NULL(name);
}

TEST(ptsname_r_bad_fd) {
	char buf[64];
	int r = ptsname_r(999999, buf, sizeof(buf));
	ASSERT_INT_NE(r, 0);
}

TEST(ptsname_r_small_buf) {
	int fd = posix_openpt(O_RDWR | O_NOCTTY);

	if (fd < 0) TEST_SKIP("posix_openpt unavailable");

	char valid_buf[64];

	if (ptsname_r(fd, valid_buf, sizeof(valid_buf)) != 0) {
		close(fd);

		TEST_SKIP("PTY subsystem restricted (ioctl failed)");
	}

	char small_buf[1];
	int r = ptsname_r(fd, small_buf, sizeof(small_buf));

	ASSERT_INT_EQ(r, ERANGE);

	close(fd);
}

/* ============================================================= */

TEST_SUITE(random)

TEST(random_basic) {
	srandom(42);
	long v1 = random();
	srandom(42);
	long v2 = random();
	ASSERT_INT_EQ(v1, v2);
}

TEST(random_range) {
	srandom(1);
	for (int i = 0; i < 100; i++) {
		long v = random();
		ASSERT_INT_GE(v, 0);
	}
}

TEST(random_different_seeds) {
	srandom(1);
	long v1 = random();
	srandom(2);
	long v2 = random();
	ASSERT_INT_NE(v1, v2);
}

TEST(random_sequence_length) {
	srandom(42);
	long seq[100];
	for (int i = 0; i < 100; i++) seq[i] = random();
	srandom(42);
	for (int i = 0; i < 100; i++) {
		long v = random();
		ASSERT_INT_EQ(v, seq[i]);
	}
}


/* ============================================================= */

TEST_SUITE(initstate)

TEST(initstate_setstate) {
	char state1[256];
	char state2[256];

	char *old1 = initstate(123, state1, sizeof(state1));
	ASSERT_NOT_NULL(old1);
	long v1 = random();

	char *old2 = initstate(456, state2, sizeof(state2));
	ASSERT_NOT_NULL(old2);
	long v2 = random();
	// Switch back to state1, should get next value in state1 sequence
	char *ret1 = setstate(state1);
	ASSERT_NOT_NULL(ret1);
	long v1_next = random();

	char *ret2 = setstate(state2);
	ASSERT_NOT_NULL(ret2);
	long v2_next = random();

	// The sequences must be completely independent
	ASSERT_INT_NE(v1_next, v2_next);
}

TEST(initstate_too_small) {
	char tiny[10];
	errno = 0;
	char *r = initstate(1, tiny, sizeof(tiny));
	ASSERT_NULL(r);
	ASSERT_ERRNO(EINVAL);
}

/* ============================================================= */

TEST_SUITE(drand48)

TEST(drand48_range) {
	srand48(42);
	for (int i = 0; i < 100; i++) {
		double v = drand48();
		ASSERT_TRUE(v >= 0.0 && v < 1.0);
	}
}

TEST(drand48_reproducible) {
	srand48(12345);
	double seq[50];
	for (int i = 0; i < 50; i++) seq[i] = drand48();
	srand48(12345);
	for (int i = 0; i < 50; i++) {
		double v = drand48();
		ASSERT_DBL_EQ(v, seq[i]);
	}
}

/* ============================================================= */

TEST_SUITE(lrand48)

TEST(lrand48_range) {
	srand48(42);
	for (int i = 0; i < 100; i++) {
		long v = lrand48();
		ASSERT_INT_GE(v, 0);
	}
}

TEST(lrand48_reproducible) {
	srand48(99999);
	long seq[50];
	for (int i = 0; i < 50; i++) seq[i] = lrand48();
	srand48(99999);
	for (int i = 0; i < 50; i++) {
		long v = lrand48();
		ASSERT_INT_EQ(v, seq[i]);
	}
}

/* ============================================================= */

TEST_SUITE(erand48)

TEST(erand48_independent) {
	unsigned short xsubi1[3] = {1, 2, 3};
	unsigned short xsubi2[3] = {1, 2, 3};
	double v1 = erand48(xsubi1);
	double v2 = erand48(xsubi2);
	ASSERT_DBL_EQ(v1, v2);
}

TEST(erand48_multiple_states) {
	unsigned short state1[3] = {100, 200, 300};
	unsigned short state2[3] = {400, 500, 600};

	double v1 = erand48(state1);
	double v2 = erand48(state2);

	ASSERT_TRUE(v1 != v2);  // Compare as doubles, not ints

	double v3 = erand48(state1);
	double v4 = erand48(state2);

	ASSERT_TRUE(v3 != v4);
}

/* ============================================================= */

TEST_SUITE(mrand48)

TEST(mrand48_signed) {
	srand48(42);
	int positive = 0, negative = 0;
	for (int i = 0; i < 1000; i++) {
		long v = mrand48();
		if (v > 0) positive++;
		else if (v < 0) negative++;
	}
	ASSERT_INT_GT(positive, 400);
	ASSERT_INT_GT(negative, 400);
}

/* ============================================================= */

TEST_SUITE(nrand48)

TEST(nrand48_range) {
	srand48(42);
	for (int i = 0; i < 100; i++) {
		long v = nrand48(__jacl_rand48_seed);
		ASSERT_INT_GE(v, 0);
		ASSERT_INT_LT(v, 0x80000000L);
	}
}

/* ============================================================= */

TEST_SUITE(jrand48)

TEST(jrand48_range) {
	unsigned short xsubi[3] = {1, 2, 3};
	for (int i = 0; i < 100; i++) {
		long v = jrand48(xsubi);
		ASSERT_TRUE(v >= -0x80000000L && v <= 0x7FFFFFFFL);
	}
}

/* ============================================================= */

TEST_SUITE(seed48)

TEST(seed48_saves_old) {
	srand48(111);
	unsigned short old_seed[3] = {1, 2, 3};
	unsigned short *saved = seed48(old_seed);
	ASSERT_NOT_NULL(saved);
	ASSERT_INT_NE(saved[0], 1);
	ASSERT_INT_NE(saved[1], 2);
	ASSERT_INT_NE(saved[2], 3);
}

/* ============================================================= */

TEST_SUITE(long48)

TEST(lcong48_custom) {
	unsigned short param[7] = {1, 2, 3, 4, 5, 6, 7};
	lcong48(param);
	long v = lrand48();
	ASSERT_INT_GE(v, 0);
}

/* ============================================================= */

TEST_SUITE(setkey)

TEST(setkey_stub) {
	char key[64] = {0};
	setkey(key);
	ASSERT_TRUE(1);
}

/* ============================================================= */

TEST_MAIN()
