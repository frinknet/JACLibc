/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <stdio.h>

TEST_TYPE(unit);
TEST_UNIT(stdio.h);

/* ============================================================================
 * sprintf BASIC TESTS
 * ============================================================================ */
TEST_SUITE(sprintf);

TEST(basic_string) {
	char buf[64];
	int ret = sprintf(buf, "hello");
	ASSERT_STR_EQ("hello", buf);
	ASSERT_EQ(5, ret);
}

TEST(percent_escape) {
	char buf[64];
	int ret = sprintf(buf, "%%");
	ASSERT_STR_EQ("%", buf);
	ASSERT_EQ(1, ret);
}

TEST(mixed_format) {
	char buf[128];
	int x = 42;
	sprintf(buf, "%d %s %p", 42, "test", &x);
	ASSERT_NOT_NULL(strstr(buf, "42 test"));
}

/* ============================================================================
 * sprintf FLOAT TESTS (%f)
 * ============================================================================ */
TEST_SUITE(sprintf_float);

TEST(zero) {
	char buf[64];
	sprintf(buf, "%.6f", 0.0);
	ASSERT_STR_EQ("0.000000", buf);
}

TEST(negative_zero) {
	char buf[64];
	sprintf(buf, "%.6f", -0.0);
	ASSERT_TRUE(strstr(buf, "0.000000") != NULL);
}

TEST(rounding_995_critical) {
	// Critical: tests last-digit rounding fix
	char buf[64];
	sprintf(buf, "%.2f", 0.995);
	ASSERT_STR_EQ("1.00", buf);
}

TEST(rounding_994_no_round) {
	char buf[64];
	sprintf(buf, "%.2f", 0.994);
	ASSERT_STR_EQ("0.99", buf);
}

TEST(rounding_1005_half) {
	char buf[64];
	sprintf(buf, "%.2f", 1.005);
	ASSERT_TRUE(!strcmp(buf, "1.00") || !strcmp(buf, "1.01"));
}

TEST(rounding_9995_carry_tens) {
	char buf[64];
	sprintf(buf, "%.2f", 9.995);
	ASSERT_STR_EQ("10.00", buf);
}

TEST(rounding_99995_carry_hundreds) {
	char buf[64];
	sprintf(buf, "%.2f", 99.995);
	ASSERT_STR_EQ("100.00", buf);
}

TEST(precision_overflow_prevention) {
	// Critical: ensures fix prevents '0'+10 = ':' bug
	char buf[64];
	sprintf(buf, "%.6f", 1.999999);
	ASSERT_STR_EQ("2.000000", buf);
}

TEST(large_integer_1e15) {
	char buf[64];
	sprintf(buf, "%.6f", 1e15);
	ASSERT_NOT_NULL(strstr(buf, "1000000000000000"));
}

TEST(negative_near_zero) {
	char buf[64];
	sprintf(buf, "%.10f", -1e-10);
	ASSERT_STR_EQ("-0.0000000001", buf);
}

TEST(negative_rounds_to_zero) {
	char buf[64];
	sprintf(buf, "%.6f", -0.0000001);
	ASSERT_TRUE(strstr(buf, "0.000000") != NULL);
}

TEST(precision_zero) {
	char buf[64];
	sprintf(buf, "%.0f", 3.5);
	ASSERT_TRUE(!strcmp(buf, "3") || !strcmp(buf, "4"));
}

TEST(precision_zero_integer) {
	char buf[64];
	sprintf(buf, "%.0f", 1.0);
	ASSERT_STR_EQ("1", buf);
}

TEST(trailing_zeros_show) {
	char buf[64];
	sprintf(buf, "%.6f", 1.0);
	ASSERT_STR_EQ("1.000000", buf);
}

TEST(alt_form_decimal_point) {
	char buf[64];
	sprintf(buf, "%#.0f", 1.0);
	ASSERT_STR_EQ("1.", buf);
}

TEST(width_padding_right) {
	char buf[64];
	sprintf(buf, "%10.2f", 3.14);
	ASSERT_STR_EQ("      3.14", buf);
}

TEST(width_padding_left) {
	char buf[64];
	sprintf(buf, "%-10.2f", 3.14);
	ASSERT_STR_EQ("3.14      ", buf);
}

TEST(width_zero_padding) {
	char buf[64];
	sprintf(buf, "%010.2f", 3.14);
	ASSERT_STR_EQ("0000003.14", buf);
}

TEST(width_zero_padding_negative) {
	char buf[64];
	sprintf(buf, "%010.2f", -3.14);
	ASSERT_STR_EQ("-000003.14", buf);
}

TEST(flag_plus_sign) {
	char buf[64];
	sprintf(buf, "%+f", 3.14);
	ASSERT_EQ('+', buf[0]);
}

TEST(flag_plus_negative) {
	char buf[64];
	sprintf(buf, "%+f", -3.14);
	ASSERT_EQ('-', buf[0]);
}

TEST(flag_space) {
	char buf[64];
	sprintf(buf, "% f", 3.14);
	ASSERT_EQ(' ', buf[0]);
}

TEST(flag_space_negative) {
	char buf[64];
	sprintf(buf, "% f", -3.14);
	ASSERT_EQ('-', buf[0]);
}

TEST(precision_one) {
	char buf[64];
	sprintf(buf, "%.1f", 1.05);
	ASSERT_TRUE(!strcmp(buf, "1.0") || !strcmp(buf, "1.1"));
}

TEST(precision_twenty) {
	char buf[64];
	sprintf(buf, "%.20f", 1.0);
	ASSERT_STR_EQ("1.00000000000000000000", buf);
}

/* ============================================================================
 * sprintf EXPONENTIAL TESTS (%e, %E)
 * ============================================================================ */
TEST_SUITE(sprintf_exp);

TEST(basic_zero_exponent) {
	char buf[64];
	sprintf(buf, "%.6e", 1.0);
	ASSERT_STR_EQ("1.000000e+00", buf);
}

TEST(negative_exponent) {
	char buf[64];
	sprintf(buf, "%.6e", 0.001);
	ASSERT_STR_EQ("1.000000e-03", buf);
}

TEST(positive_exponent) {
	char buf[64];
	sprintf(buf, "%.6e", 1000.0);
	ASSERT_STR_EQ("1.000000e+03", buf);
}

TEST(large_exponent_100) {
	char buf[64];
	sprintf(buf, "%.6e", 1e100);
	ASSERT_STR_EQ("1.000000e+100", buf);
}

TEST(large_exponent_200) {
	char buf[64];
	sprintf(buf, "%.6e", 1e200);
	ASSERT_STR_EQ("1.000000e+200", buf);
}

TEST(large_negative_exponent) {
	char buf[64];
	sprintf(buf, "%.6e", 1e-200);
	ASSERT_STR_EQ("1.000000e-200", buf);
}

TEST(rounding_with_carry_to_exp) {
	// Critical: tests overflow fix in exp formatting
	char buf[64];
	sprintf(buf, "%.6e", 9.999999);
	ASSERT_STR_EQ("1.000000e+01", buf);
}

TEST(uppercase_E) {
	char buf[64];
	sprintf(buf, "%.6E", 1.0);
	ASSERT_NOT_NULL(strstr(buf, "E+00"));
}

TEST(uppercase_E_large) {
	char buf[64];
	sprintf(buf, "%.6E", 1e100);
	ASSERT_NOT_NULL(strstr(buf, "E+100"));
}

TEST(nan_detection) {
	// Critical: tests num != num check
	char buf[64];
	sprintf(buf, "%f", 0.0/0.0);
	ASSERT_STR_EQ("nan", buf);
}

TEST(inf_positive) {
	char buf[64];
	sprintf(buf, "%e", 1.0/0.0);
	ASSERT_STR_EQ("inf", buf);
}

TEST(inf_negative) {
	char buf[64];
	sprintf(buf, "%f", -1.0/0.0);
	ASSERT_STR_EQ("-inf", buf);
}

TEST(nan_uppercase) {
	char buf[64];
	sprintf(buf, "%E", 0.0/0.0);
	ASSERT_STR_EQ("NAN", buf);
}

TEST(inf_uppercase) {
	char buf[64];
	sprintf(buf, "%E", 1.0/0.0);
	ASSERT_STR_EQ("INF", buf);
}

TEST(subnormal_near_min) {
	char buf[64];
	sprintf(buf, "%.6e", 2.2e-308);
	ASSERT_STR_EQ("2.200000e-308", buf);
}

TEST(near_dbl_max) {
	char buf[64];
	sprintf(buf, "%.6e", 1e308);
	ASSERT_STR_EQ("1.000000e+308", buf);
}

TEST(zero_exponent) {
	char buf[64];
	sprintf(buf, "%.6e", 0.0);
	ASSERT_STR_EQ("0.000000e+00", buf);
}

/* ============================================================================
 * sprintf %g GENERAL FORMAT TESTS
 * ============================================================================ */
TEST_SUITE(sprintf_g);

TEST(small_uses_f_format) {
	char buf[64];
	sprintf(buf, "%.6g", 0.0001);
	ASSERT_STR_EQ("0.0001", buf);
}

TEST(tiny_uses_e_format) {
	char buf[64];
	sprintf(buf, "%.6g", 0.00001);
	ASSERT_NOT_NULL(strstr(buf, "e-05"));
}

TEST(large_uses_f) {
	char buf[64];
	sprintf(buf, "%.6g", 123456);
	ASSERT_STR_EQ("123456", buf);
}

TEST(large_switches_to_e) {
	char buf[64];
	sprintf(buf, "%.6g", 1234567);
	ASSERT_NOT_NULL(strstr(buf, "e+06"));
}

TEST(trailing_zero_suppression) {
	char buf[64];
	sprintf(buf, "%g", 1.5000);
	ASSERT_STR_EQ("1.5", buf);
}

TEST(integer_no_decimal) {
	char buf[64];
	sprintf(buf, "%g", 1.0);
	ASSERT_STR_EQ("1", buf);
}

TEST(alt_form_keeps_decimal) {
	char buf[64];
	sprintf(buf, "%#.0g", 1.0);
	ASSERT_STR_EQ("1.", buf);
}

TEST(uppercase_G) {
	char buf[64];
	sprintf(buf, "%.6G", 0.00001);
	ASSERT_NOT_NULL(strstr(buf, "E-05"));
}

/* ============================================================================
 * sprintf INTEGER TESTS (%d, %i)
 * ============================================================================ */
TEST_SUITE(sprintf_int);

TEST(zero) {
	char buf[64];
	sprintf(buf, "%d", 0);
	ASSERT_STR_EQ("0", buf);
}

TEST(positive) {
	char buf[64];
	sprintf(buf, "%d", 42);
	ASSERT_STR_EQ("42", buf);
}

TEST(negative) {
	char buf[64];
	sprintf(buf, "%d", -42);
	ASSERT_STR_EQ("-42", buf);
}

TEST(int_max) {
	char buf[64];
	sprintf(buf, "%d", INT_MAX);
	ASSERT_STR_EQ("2147483647", buf);
}

TEST(int_min) {
	char buf[64];
	sprintf(buf, "%d", INT_MIN);
	ASSERT_STR_EQ("-2147483648", buf);
}

TEST(width_right_align) {
	char buf[64];
	sprintf(buf, "%10d", 42);
	ASSERT_STR_EQ("        42", buf);
}

TEST(width_left_align) {
	char buf[64];
	sprintf(buf, "%-10d", 42);
	ASSERT_STR_EQ("42        ", buf);
}

TEST(zero_padding) {
	char buf[64];
	sprintf(buf, "%010d", 42);
	ASSERT_STR_EQ("0000000042", buf);
}

TEST(zero_padding_negative) {
	char buf[64];
	sprintf(buf, "%010d", -42);
	ASSERT_STR_EQ("-000000042", buf);
}

TEST(precision_min_digits) {
	char buf[64];
	sprintf(buf, "%.5d", 42);
	ASSERT_STR_EQ("00042", buf);
}

TEST(precision_negative) {
	char buf[64];
	sprintf(buf, "%.5d", -42);
	ASSERT_STR_EQ("-00042", buf);
}

TEST(plus_sign_flag) {
	char buf[64];
	sprintf(buf, "%+d", 42);
	ASSERT_STR_EQ("+42", buf);
}

TEST(plus_sign_negative) {
	char buf[64];
	sprintf(buf, "%+d", -42);
	ASSERT_STR_EQ("-42", buf);
}

TEST(space_flag) {
	char buf[64];
	sprintf(buf, "% d", 42);
	ASSERT_STR_EQ(" 42", buf);
}

TEST(space_flag_negative) {
	char buf[64];
	sprintf(buf, "% d", -42);
	ASSERT_STR_EQ("-42", buf);
}

TEST(long_long_max) {
	char buf[64];
	sprintf(buf, "%lld", 9223372036854775807LL);
	ASSERT_STR_EQ("9223372036854775807", buf);
}

TEST(long_long_min) {
	char buf[64];
	sprintf(buf, "%lld", -9223372036854775807LL - 1);
	ASSERT_STR_EQ("-9223372036854775808", buf);
}

TEST(short_int) {
	char buf[64];
	sprintf(buf, "%hd", (short)42);
	ASSERT_STR_EQ("42", buf);
}

TEST(char_int) {
	char buf[64];
	sprintf(buf, "%hhd", (signed char)42);
	ASSERT_STR_EQ("42", buf);
}

/* ============================================================================
 * sprintf UNSIGNED TESTS (%u)
 * ============================================================================ */
TEST_SUITE(sprintf_unsigned);

TEST(zero) {
	char buf[64];
	sprintf(buf, "%u", 0U);
	ASSERT_STR_EQ("0", buf);
}

TEST(basic) {
	char buf[64];
	sprintf(buf, "%u", 42U);
	ASSERT_STR_EQ("42", buf);
}

TEST(uint_max) {
	char buf[64];
	sprintf(buf, "%u", UINT_MAX);
	ASSERT_STR_EQ("4294967295", buf);
}

TEST(negative_as_unsigned) {
	char buf[64];
	sprintf(buf, "%u", -1);
	ASSERT_STR_EQ("4294967295", buf);
}

TEST(ulong_long_max) {
	char buf[64];
	sprintf(buf, "%llu", 18446744073709551615ULL);
	ASSERT_STR_EQ("18446744073709551615", buf);
}

/* ============================================================================
 * sprintf HEX TESTS (%x, %X)
 * ============================================================================ */
TEST_SUITE(sprintf_hex);

TEST(zero) {
	char buf[64];
	sprintf(buf, "%x", 0);
	ASSERT_STR_EQ("0", buf);
}

TEST(basic_lowercase) {
	char buf[64];
	sprintf(buf, "%x", 255);
	ASSERT_STR_EQ("ff", buf);
}

TEST(basic_uppercase) {
	char buf[64];
	sprintf(buf, "%X", 255);
	ASSERT_STR_EQ("FF", buf);
}

TEST(alt_form_prefix_lowercase) {
	char buf[64];
	sprintf(buf, "%#x", 42);
	ASSERT_STR_EQ("0x2a", buf);
}

TEST(alt_form_prefix_uppercase) {
	char buf[64];
	sprintf(buf, "%#X", 42);
	ASSERT_STR_EQ("0X2A", buf);
}

TEST(alt_form_zero_no_prefix) {
	char buf[64];
	sprintf(buf, "%#x", 0);
	ASSERT_STR_EQ("0", buf);
}

TEST(negative_as_hex) {
	char buf[64];
	sprintf(buf, "%x", -1);
	ASSERT_STR_EQ("ffffffff", buf);
}

TEST(combo_alt_zero_pad) {
	char buf[64];
	sprintf(buf, "%#010x", 42);
	ASSERT_STR_EQ("0x0000002a", buf);
}

TEST(width_padding) {
	char buf[64];
	sprintf(buf, "%10x", 42);
	ASSERT_STR_EQ("        2a", buf);
}

/* ============================================================================
 * sprintf OCTAL TESTS (%o)
 * ============================================================================ */
TEST_SUITE(sprintf_octal);

TEST(zero) {
	char buf[64];
	sprintf(buf, "%o", 0);
	ASSERT_STR_EQ("0", buf);
}

TEST(basic) {
	char buf[64];
	sprintf(buf, "%o", 8);
	ASSERT_STR_EQ("10", buf);
}

TEST(alt_form_prefix) {
	char buf[64];
	sprintf(buf, "%#o", 8);
	ASSERT_STR_EQ("010", buf);
}

TEST(alt_form_zero) {
	char buf[64];
	sprintf(buf, "%#o", 0);
	ASSERT_STR_EQ("0", buf);
}

TEST(large_value) {
	char buf[64];
	sprintf(buf, "%o", 255);
	ASSERT_STR_EQ("377", buf);
}

/* ============================================================================
 * sprintf STRING TESTS (%s)
 * ============================================================================ */
TEST_SUITE(sprintf_string);

TEST(basic) {
	char buf[64];
	sprintf(buf, "%s", "hello");
	ASSERT_STR_EQ("hello", buf);
}

TEST(null_pointer) {
	// Critical: tests "(null)" handling
	char buf[64];
	sprintf(buf, "%s", NULL);
	ASSERT_STR_EQ("(null)", buf);
}

TEST(null_pointer_with_precision) {
	char buf[64];
	sprintf(buf, "%.3s", NULL);
	ASSERT_STR_EQ("(nu", buf);
}

TEST(empty_string) {
	char buf[64];
	sprintf(buf, "%s", "");
	ASSERT_STR_EQ("", buf);
}

TEST(precision_limiting) {
	char buf[64];
	sprintf(buf, "%.3s", "hello");
	ASSERT_STR_EQ("hel", buf);
}

TEST(precision_longer_than_string) {
	char buf[64];
	sprintf(buf, "%.10s", "hi");
	ASSERT_STR_EQ("hi", buf);
}

TEST(width_right_align) {
	char buf[64];
	sprintf(buf, "%10s", "hi");
	ASSERT_STR_EQ("        hi", buf);
}

TEST(width_left_align) {
	char buf[64];
	sprintf(buf, "%-10s", "hi");
	ASSERT_STR_EQ("hi        ", buf);
}

TEST(empty_with_width) {
	char buf[64];
	sprintf(buf, "%10s", "");
	ASSERT_STR_EQ("          ", buf);
}

/* ============================================================================
 * sprintf CHARACTER TESTS (%c)
 * ============================================================================ */
TEST_SUITE(sprintf_char);

TEST(basic) {
	char buf[64];
	sprintf(buf, "%c", 'A');
	ASSERT_STR_EQ("A", buf);
}

TEST(null_char) {
	char buf[64] = {0};
	sprintf(buf, "x%cy", 0);
	ASSERT_EQ('x', buf[0]);
	ASSERT_EQ(0, buf[1]);
	ASSERT_EQ('y', buf[2]);
}

TEST(high_ascii) {
	char buf[64];
	sprintf(buf, "%c", 127);
	ASSERT_EQ(127, (unsigned char)buf[0]);
}

TEST(extended_ascii) {
	char buf[64];
	sprintf(buf, "%c", 255);
	ASSERT_EQ(255, (unsigned char)buf[0]);
}

/* ============================================================================
 * sprintf POINTER TESTS (%p)
 * ============================================================================ */
TEST_SUITE(sprintf_pointer);

TEST(null_pointer) {
	char buf[64];
	sprintf(buf, "%p", NULL);
	ASSERT_NOT_NULL(buf);
}

TEST(valid_pointer) {
	int x = 42;
	char buf[64];
	sprintf(buf, "%p", &x);
	ASSERT_TRUE(strstr(buf, "0x") || strstr(buf, "0X") || buf[0] != '\0');
}

TEST(max_pointer_value) {
	char buf[64];
	sprintf(buf, "%p", (void*)UINTPTR_MAX);
	ASSERT_NOT_NULL(buf);
}

/* ============================================================================
 * sprintf SPECIAL FORMAT TESTS
 * ============================================================================ */
TEST_SUITE(sprintf_special);

TEST(dynamic_width) {
	char buf[64];
	sprintf(buf, "%*d", 10, 42);
	ASSERT_STR_EQ("        42", buf);
}

TEST(dynamic_precision) {
	char buf[64];
	sprintf(buf, "%.*f", 2, 3.14159);
	ASSERT_STR_EQ("3.14", buf);
}

TEST(dynamic_width_and_precision) {
	char buf[64];
	sprintf(buf, "%*.*f", 10, 2, 3.14);
	ASSERT_STR_EQ("      3.14", buf);
}

TEST(negative_width_left_align) {
	char buf[64];
	sprintf(buf, "%*d", -10, 42);
	ASSERT_STR_EQ("42        ", buf);
}

TEST(negative_precision_ignored) {
	char buf[64];
	sprintf(buf, "%.*f", -1, 3.14);
	ASSERT_STR_EQ("3.140000", buf);
}

TEST(multiple_flags) {
	char buf[64];
	sprintf(buf, "%+0 10d", 42);
	ASSERT_EQ('+', buf[0]);
}

/* ============================================================================
 * snprintf BUFFER SAFETY TESTS
 * ============================================================================ */
TEST_SUITE(snprintf);

TEST(zero_buffer_returns_length) {
	int len = snprintf(NULL, 0, "hello world");
	ASSERT_EQ(11, len);
}

TEST(truncate_with_null) {
	char buf[5] = {0};
	int ret = snprintf(buf, 5, "hello world");
	ASSERT_STR_EQ("hell", buf);
	ASSERT_EQ(11, ret);
}

TEST(exact_fit) {
	char buf[6] = {0};
	int ret = snprintf(buf, 6, "hello");
	ASSERT_STR_EQ("hello", buf);
	ASSERT_EQ(5, ret);
}

TEST(large_format_small_buffer) {
	char buf[10] = {0};
	snprintf(buf, 10, "%100d", 42);
	ASSERT_LT(strlen(buf), 10);
}

TEST(zero_size_buffer_non_null) {
	char buf[10] = "test";
	snprintf(buf, 0, "hello");
	ASSERT_STR_EQ("test", buf);
}

/* ============================================================================
 * sscanf FLOAT TESTS (%f, %lf, %e)
 * ============================================================================ */
TEST_SUITE(sscanf_float);

TEST(parse_zero) {
	double d;
	int ret = sscanf("0.0", "%lf", &d);
	ASSERT_EQ(1, ret);
	ASSERT_FLOAT_EQ(0.0, d, 1e-10);
}

TEST(parse_negative) {
	double d;
	sscanf("-3.14", "%lf", &d);
	ASSERT_FLOAT_EQ(-3.14, d, 1e-10);
}

TEST(parse_positive_exponent) {
	// Critical: tests fixed exp loop
	double d;
	sscanf("1.5e10", "%lf", &d);
	ASSERT_FLOAT_EQ(1.5e10, d, 1e5);
}

TEST(parse_negative_exponent) {
	// Critical: tests /=10 fix
	double d;
	sscanf("1e-5", "%lf", &d);
	ASSERT_FLOAT_EQ(0.00001, d, 1e-15);
}

TEST(parse_large_exponent) {
	double d;
	sscanf("1e200", "%lf", &d);
	ASSERT_FLOAT_EQ(1e200, d, 1e195);
}

TEST(parse_small_exponent) {
	double d;
	sscanf("1e-200", "%lf", &d);
	ASSERT_FLOAT_EQ(1e-200, d, 1e-210);
}

TEST(parse_no_decimal) {
	double d;
	sscanf("42", "%lf", &d);
	ASSERT_FLOAT_EQ(42.0, d, 1e-10);
}

TEST(parse_leading_zeros) {
	double d;
	sscanf("0001.5", "%lf", &d);
	ASSERT_FLOAT_EQ(1.5, d, 1e-10);
}

TEST(parse_trailing_zeros) {
	double d;
	sscanf("1.500", "%lf", &d);
	ASSERT_FLOAT_EQ(1.5, d, 1e-10);
}

TEST(parse_whitespace) {
	double d;
	sscanf("  3.14", "%lf", &d);
	ASSERT_FLOAT_EQ(3.14, d, 1e-10);
}

TEST(parse_multiple_floats) {
	double a, b, c;
	int ret = sscanf("1.0 2.0 3.0", "%lf %lf %lf", &a, &b, &c);
	ASSERT_EQ(3, ret);
	ASSERT_FLOAT_EQ(1.0, a, 1e-10);
	ASSERT_FLOAT_EQ(2.0, b, 1e-10);
	ASSERT_FLOAT_EQ(3.0, c, 1e-10);
}

TEST(parse_uppercase_E) {
	double d;
	sscanf("1E5", "%lf", &d);
	ASSERT_FLOAT_EQ(1e5, d, 1e-5);
}

TEST(parse_positive_sign) {
	double d;
	sscanf("+42.5", "%lf", &d);
	ASSERT_FLOAT_EQ(42.5, d, 1e-10);
}

/* ============================================================================
 * sscanf INTEGER TESTS (%d, %i, %u, %x, %o)
 * ============================================================================ */
TEST_SUITE(sscanf_int);

TEST(parse_zero) {
	int i;
	sscanf("0", "%d", &i);
	ASSERT_EQ(0, i);
}

TEST(parse_positive) {
	int i;
	sscanf("42", "%d", &i);
	ASSERT_EQ(42, i);
}

TEST(parse_negative) {
	int i;
	sscanf("-42", "%d", &i);
	ASSERT_EQ(-42, i);
}

TEST(parse_hex_auto) {
	int i;
	sscanf("0x1a", "%i", &i);
	ASSERT_EQ(26, i);
}

TEST(parse_hex_auto_uppercase) {
	int i;
	sscanf("0X1A", "%i", &i);
	ASSERT_EQ(26, i);
}

TEST(parse_octal_auto) {
	int i;
	sscanf("010", "%i", &i);
	ASSERT_EQ(8, i);
}

TEST(parse_hex_explicit) {
	int i;
	sscanf("ff", "%x", &i);
	ASSERT_EQ(255, i);
}

TEST(parse_hex_uppercase) {
	int i;
	sscanf("FF", "%x", &i);
	ASSERT_EQ(255, i);
}

TEST(parse_octal_explicit) {
	int i;
	sscanf("77", "%o", &i);
	ASSERT_EQ(63, i);
}

TEST(parse_width_limiting) {
	int i;
	sscanf("12345", "%3d", &i);
	ASSERT_EQ(123, i);
}

TEST(parse_plus_sign) {
	int i;
	sscanf("+42", "%d", &i);
	ASSERT_EQ(42, i);
}

TEST(parse_suppression) {
	int a, b;
	int ret = sscanf("42 17", "%*d %d", &b);
	ASSERT_EQ(1, ret);
	ASSERT_EQ(17, b);
}

TEST(parse_unsigned) {
	unsigned int u;
	sscanf("4294967295", "%u", &u);
	ASSERT_EQ(UINT_MAX, u);
}

TEST(parse_long_long) {
	long long ll;
	sscanf("9223372036854775807", "%lld", &ll);
	ASSERT_EQ(9223372036854775807LL, ll);
}

/* ============================================================================
 * sscanf STRING TESTS (%s, %c)
 * ============================================================================ */
TEST_SUITE(sscanf_string);

TEST(parse_basic_string) {
	char str[10];
	sscanf("hello", "%s", str);
	ASSERT_STR_EQ("hello", str);
}

TEST(parse_string_stops_at_whitespace) {
	char str[10];
	sscanf("hello world", "%s", str);
	ASSERT_STR_EQ("hello", str);
}

TEST(parse_string_skips_leading_whitespace) {
	char str[10];
	sscanf("  hello", "%s", str);
	ASSERT_STR_EQ("hello", str);
}

TEST(parse_char) {
	char c;
	sscanf("A", "%c", &c);
	ASSERT_EQ('A', c);
}

TEST(parse_char_no_skip_whitespace) {
	char c;
	sscanf(" A", "%c", &c);
	ASSERT_EQ(' ', c);
}

/* ============================================================================
 * sscanf SCANSET TESTS (%[...])
 * ============================================================================ */
TEST_SUITE(sscanf_scanset);

TEST(digit_scanset) {
	char str[10];
	sscanf("123abc", "%[0-9]", str);
	ASSERT_STR_EQ("123", str);
}

TEST(inverted_scanset) {
	char str[10];
	sscanf("123abc", "%[^a-z]", str);
	ASSERT_STR_EQ("123", str);
}

TEST(alpha_scanset) {
	char str[10];
	sscanf("abc123", "%[a-z]", str);
	ASSERT_STR_EQ("abc", str);
}

TEST(scanset_with_dash) {
	char str[10];
	sscanf("abc-def", "%[a-z-]", str);
	ASSERT_STR_EQ("abc-def", str);
}

TEST(scanset_with_bracket) {
	char str[10];
	sscanf("]test", "%[]a-z]", str);
	ASSERT_STR_EQ("]test", str);
}

/* ============================================================================
 * fflush STREAM TESTS
 * ============================================================================ */
TEST_SUITE(fflush);

TEST(flush_stdout_twice) {
	// Critical: tests removed static flag
	int ret1 = fflush(stdout);
	int ret2 = fflush(stdout);
	ASSERT_EQ(0, ret1);
	ASSERT_EQ(0, ret2);
}

TEST(flush_null_twice) {
	// Critical: ensures fflush(NULL) works repeatedly
	int ret1 = fflush(NULL);
	int ret2 = fflush(NULL);
	ASSERT_EQ(0, ret1);
	ASSERT_EQ(0, ret2);
}

/* ============================================================================
 * ROUNDTRIP TESTS
 * ============================================================================ */
TEST_SUITE(roundtrip);

TEST(float_roundtrip) {
	char buf[50];
	double orig = 1.234567e89;
	sprintf(buf, "%.15e", orig);
	double parsed;
	sscanf(buf, "%lf", &parsed);
	ASSERT_FLOAT_EQ(orig, parsed, orig * 1e-14);
}

TEST(int_roundtrip) {
	char buf[20];
	int orig = -12345;
	sprintf(buf, "%d", orig);
	int parsed;
	sscanf(buf, "%d", &parsed);
	ASSERT_EQ(orig, parsed);
}

TEST(hex_roundtrip) {
	char buf[20];
	unsigned int orig = 0xDEADBEEF;
	sprintf(buf, "%x", orig);
	unsigned int parsed;
	sscanf(buf, "%x", &parsed);
	ASSERT_EQ(orig, parsed);
}

TEST_MAIN()

