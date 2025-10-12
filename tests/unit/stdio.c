/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <stdio.h>

TEST_TYPE(unit);
TEST_UNIT(stdio.h);

/* ============================================================================
 * printf I/O TESTS
 * ============================================================================ */
TEST_SUITE(printf_io);

TEST(printf_to_stdout) {
	// Redirect stdout to capture output
	int stdout_backup = dup(STDOUT_FILENO);
	int pipefd[2];
	pipe(pipefd);
	dup2(pipefd[1], STDOUT_FILENO);

	int len = printf("test %d", 42);
	fflush(stdout);

	char buf[20];
	read(pipefd[0], buf, 7);
	buf[7] = '\0';

	// Restore stdout
	dup2(stdout_backup, STDOUT_FILENO);
	close(stdout_backup);
	close(pipefd[0]);
	close(pipefd[1]);

	ASSERT_EQ(7, len);
	ASSERT_STR_EQ("test 42", buf);
}

TEST(fprintf_to_file) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);

	int len = fprintf(f, "hello %d", 42);
	ASSERT_EQ(8, len);

	rewind(f);
	char buf[20];
	fgets(buf, 20, f);
	ASSERT_STR_EQ("hello 42", buf);

	fclose(f);
}

TEST(fprintf_multiple_writes) {
	FILE *f = tmpfile();

	fprintf(f, "line1\n");
	fprintf(f, "line2\n");
	fprintf(f, "line3\n");

	rewind(f);
	char buf[100];
	size_t n = fread(buf, 1, 100, f);
	buf[n] = '\0';

	ASSERT_STR_EQ("line1\nline2\nline3\n", buf);
	fclose(f);
}

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

TEST(huge_precision_clamping) {
	char buf[100];
	int len = snprintf(buf, 100, "%.500f", 1.5);
	ASSERT_GT(len, 100);  // Should report full needed
}

TEST(negative_rounds_correctly) {
	char buf[20];
	sprintf(buf, "%.1f", -0.05);
	ASSERT_TRUE(!strcmp(buf, "-0.0") || !strcmp(buf, "-0.1"));
}

TEST(denormal_float) {
	char buf[64];
	sprintf(buf, "%.50e", 1e-320);
	// Should handle gracefully (may underflow to 0)
	ASSERT_NOT_NULL(buf);
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

TEST(zero_precision_zero_value) {
	char buf[20];
	sprintf(buf, "%.0d", 0);
	ASSERT_STR_EQ("", buf);  // Or "0"? Check spec
}

TEST(precision_with_width) {
	char buf[20];
	sprintf(buf, "%10.5d", 42);
	ASSERT_STR_EQ("     00042", buf);
}

TEST(size_t_max) {
	char buf[64];
	sprintf(buf, "%zu", SIZE_MAX);
	ASSERT_GT(strlen(buf), 0);
}

TEST(ssize_t_negative) {
	char buf[64];
	sprintf(buf, "%zd", (ssize_t)-1);
	ASSERT_STR_EQ("-1", buf);
}

TEST(ptrdiff_t_max) {
	char buf[64];
	sprintf(buf, "%td", PTRDIFF_MAX);
	ASSERT_GT(strlen(buf), 0);
}

TEST(intmax_t_max) {
	char buf[64];
	sprintf(buf, "%jd", INTMAX_MAX);
	ASSERT_GT(strlen(buf), 0);
}

TEST(uintmax_t_max) {
	char buf[64];
	sprintf(buf, "%ju", UINTMAX_MAX);
	ASSERT_GT(strlen(buf), 0);
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

TEST(alt_form_with_precision) {
	char buf[20];
	sprintf(buf, "%#.8x", 42);
	ASSERT_STR_EQ("0x0000002a", buf);
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

TEST(precision_zero) {
	char buf[20];
	sprintf(buf, "%.0s", "hello");
	ASSERT_STR_EQ("", buf);
}

TEST(width_and_precision) {
	char buf[20];
	sprintf(buf, "%10.3s", "hello");
	ASSERT_STR_EQ("       hel", buf);
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

TEST(char_with_width) {
	char buf[20];
	sprintf(buf, "%5c", 'A');
	ASSERT_STR_EQ("    A", buf);
}

TEST(char_left_align_width) {
	char buf[20];
	sprintf(buf, "%-5c", 'A');
	ASSERT_STR_EQ("A    ", buf);
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

TEST(width_exceeds_bufsiz) {
	// Critical: tests BUFSIZ truncation fix
	char buf[20];
	int len = snprintf(buf, 20, "%2000d", 42);
	ASSERT_EQ(2000, len);  // Reports full length
	ASSERT_LT(strlen(buf), 20);  // But truncates
}

TEST(null_format_string) {
	char buf[20];
	int len = sprintf(buf, NULL);
	ASSERT_EQ(-1, len);
}

TEST(percent_n_position) {
	char buf[20];
	int pos = -1;
	sprintf(buf, "Hello%n world", &pos);
	ASSERT_EQ(5, pos);
}

TEST(multiple_percent_escapes) {
	char buf[20];
	sprintf(buf, "%%%%");
	ASSERT_STR_EQ("%%", buf);
}

TEST(conflicting_flags_plus_space) {
	char buf[20];
	sprintf(buf, "%+ d", 42);
	ASSERT_EQ('+', buf[0]);  // + takes precedence
}

TEST(conflicting_flags_minus_zero) {
	char buf[20];
	sprintf(buf, "%-010d", 42);
	ASSERT_STR_EQ("42        ", buf);  // - overrides 0
}

TEST(empty_format_string) {
	char buf[10];
	int len = sprintf(buf, "");
	ASSERT_STR_EQ("", buf);
	ASSERT_EQ(0, len);
}

TEST(width_zero_explicit) {
	char buf[20];
	sprintf(buf, "%0d", 42);
	ASSERT_STR_EQ("42", buf);  // Zero width = no effect
}

TEST(very_long_output) {
	char buf[100];
	char longstr[200];
	memset(longstr, 'A', 199);
	longstr[199] = '\0';

	int len = snprintf(buf, 100, "%s", longstr);
	ASSERT_EQ(199, len);
	ASSERT_LT(strlen(buf), 100);
}

TEST(mixed_width_precision) {
	char buf[50];
	sprintf(buf, "%10.3f %5.2f", 3.14159, 2.7);
	ASSERT_NOT_NULL(strstr(buf, "3.142"));
}

TEST(extreme_nesting) {
	char buf[50];
	sprintf(buf, "%*.*f", 15, 5, 3.14159);
	ASSERT_EQ(15, strlen(buf));
}

TEST(percent_at_end) {
	char buf[20];
	int len = sprintf(buf, "test%");
	// Undefined behavior - just ensure no crash
	ASSERT_GE(len, 4);
}

TEST(incomplete_format) {
	char buf[20];
	sprintf(buf, "%");
	// Should handle gracefully
	ASSERT_NOT_NULL(buf);
}

#if JACL_HAS_C99
TEST(size_t_format) {
	char buf[64];
	size_t val = 12345;
	sprintf(buf, "%zu", val);
	ASSERT_STR_EQ("12345", buf);
}

TEST(ptrdiff_t_format) {
	char buf[64];
	ptrdiff_t val = -12345;
	sprintf(buf, "%td", val);
	ASSERT_STR_EQ("-12345", buf);
}

TEST(intmax_t_format) {
	char buf[64];
	intmax_t val = 9223372036854775807LL;
	sprintf(buf, "%jd", val);
	ASSERT_STR_EQ("9223372036854775807", buf);
}

TEST(hex_float_lowercase) {
	// %a format - C99 hex float
	char buf[64];
	sprintf(buf, "%a", 1.5);
	// Result varies by platform: 0x1.8p+0 or similar
	ASSERT_NOT_NULL(strstr(buf, "0x") || strstr(buf, "0X"));
}

TEST(hex_float_uppercase) {
	char buf[64];
	sprintf(buf, "%A", 1.5);
	ASSERT_NOT_NULL(strstr(buf, "0X") || strstr(buf, "0x"));
}

static int vasprintf_helper(char **strp, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int r = vasprintf(strp, fmt, ap);
	va_end(ap);
	return r;
}

TEST(vasprintf_complete) {
	char *result = NULL;
	int r = vasprintf_helper(&result, "test %d", 42);
	ASSERT_EQ(7, r);
	ASSERT_STR_EQ("test 42", result);
	free(result);
}

TEST(percent_n_variants) {
	char buf[50];
	short h; int d; long l;

	sprintf(buf, "abc%hn def%n ghi%ln", &h, &d, &l);
	ASSERT_EQ(3, h);
	ASSERT_EQ(7, d);
	ASSERT_EQ(11, l);
}

TEST(asprintf_null_format) {
	char *str = (char*)0x1;  // Non-null sentinel
	int ret = asprintf(&str, NULL);
	ASSERT_EQ(-1, ret);
}
#endif

#if JACL_HAS_C23
TEST(binary_format_lowercase) {
	// %b format - C23 binary
	char buf[64];
	sprintf(buf, "%b", 5);
	ASSERT_STR_EQ("101", buf);
}

TEST(binary_format_uppercase) {
	char buf[64];
	sprintf(buf, "%B", 5);
	ASSERT_STR_EQ("101", buf);
}

TEST(binary_alt_form) {
	char buf[64];
	sprintf(buf, "%#b", 5);
	ASSERT_STR_EQ("0b101", buf);
}
#endif

TEST(long_double_format) {
	char buf[64];
	long double val = 3.14159265358979323846L;
	sprintf(buf, "%Lf", val);
	ASSERT_NOT_NULL(strstr(buf, "3.14159"));
}

TEST(long_double_exp_format) {
	char buf[64];
	long double val = 1e100L;
	sprintf(buf, "%Le", val);
	ASSERT_NOT_NULL(strstr(buf, "e+100") || strstr(buf, "E+100"));
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
 * ADD NEW SUITE: dprintf/asprintf Tests
 * ============================================================================ */
TEST_SUITE(dprintf_asprintf);

#if JACL_HAS_C99  // These require C99 in your implementation

TEST(dprintf_basic) {
	int pipefd[2];
	pipe(pipefd);

	int len = dprintf(pipefd[1], "test %d", 42);
	ASSERT_EQ(7, len);

	char buf[20];
	read(pipefd[0], buf, 7);
	buf[7] = '\0';
	ASSERT_STR_EQ("test 42", buf);

	close(pipefd[0]);
	close(pipefd[1]);
}

TEST(dprintf_truncation) {
	// Test BUFSIZ boundary like regular printf
	int pipefd[2];
	pipe(pipefd);

	char big[2000];
	memset(big, 'A', 1999);
	big[1999] = '\0';

	int len = dprintf(pipefd[1], "%s", big);
	ASSERT_EQ(1999, len);  // Should report full length

	close(pipefd[0]);
	close(pipefd[1]);
}

TEST(asprintf_basic) {
	char *str = NULL;
	int len = asprintf(&str, "hello %d", 42);

	ASSERT_EQ(8, len);
	ASSERT_STR_EQ("hello 42", str);
	free(str);
}

TEST(asprintf_large_output) {
	char *str = NULL;
	int len = asprintf(&str, "%2000d", 42);

	ASSERT_EQ(2000, len);
	ASSERT_NOT_NULL(str);
	ASSERT_EQ(2000, strlen(str));
	free(str);
}

TEST(vasprintf_basic) {
	// Wrapper to test variadic version
	char *result = NULL;

	// Helper function would go here
	// int r = test_vasprintf_helper(&result, "test %d", 42);

	// ASSERT_EQ(7, r);
	// ASSERT_STR_EQ("test 42", result);
	// free(result);
}

#endif /* JACL_HAS_C99 */

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

TEST(parse_no_digits) {
	double d = 42.0;
	int ret = sscanf(".", "%lf", &d);
	ASSERT_EQ(0, ret);
	ASSERT_FLOAT_EQ(42.0, d, 1e-10);  // Unchanged
}

TEST(parse_only_exponent) {
	double d = 42.0;
	int ret = sscanf("e10", "%lf", &d);
	ASSERT_EQ(0, ret);
}

TEST(parse_decimal_only) {
	double d;
	sscanf(".5", "%lf", &d);
	ASSERT_FLOAT_EQ(0.5, d, 1e-10);
}

TEST(parse_trailing_decimal) {
	double d;
	sscanf("5.", "%lf", &d);
	ASSERT_FLOAT_EQ(5.0, d, 1e-10);
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

TEST(parse_whitespace_only) {
	int i = 42;
	int ret = sscanf("   ", "%d", &i);
	ASSERT_EQ(0, ret);  // No match
	ASSERT_EQ(42, i);   // Unchanged
}

TEST(parse_non_numeric) {
	int i = 42;
	int ret = sscanf("abc", "%d", &i);
	ASSERT_EQ(0, ret);
	ASSERT_EQ(42, i);
}

TEST(parse_partial_match) {
	int a = -1, b = -1;
	int ret = sscanf("42 xyz", "%d %d", &a, &b);
	ASSERT_EQ(1, ret);
	ASSERT_EQ(42, a);
	ASSERT_EQ(-1, b);  // Unchanged
}

TEST(parse_overflow_behavior) {
	int i;
	sscanf("99999999999999", "%d", &i);
	// Just verify it doesn't crash; result is undefined
	ASSERT_TRUE(1);
}

TEST(parse_leading_plus_hex) {
	int i;
	int ret = sscanf("+0x10", "%i", &i);
	// Spec: + not allowed before 0x prefix
	ASSERT_TRUE(ret == 0 || ret == 1);  // Implementation-defined
}

TEST(parse_multiple_signs) {
	int i = 42;
	int ret = sscanf("--5", "%d", &i);
	ASSERT_EQ(0, ret);  // Invalid
	ASSERT_EQ(42, i);   // Unchanged
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

TEST(parse_empty_input) {
	char str[10] = "test";
	int ret = sscanf("", "%s", str);
	ASSERT_EQ(0, ret);  // Or EOF?
	ASSERT_STR_EQ("test", str);  // Unchanged
}

TEST(parse_width_exact) {
	char str[10];
	sscanf("hello", "%5s", str);
	ASSERT_STR_EQ("hello", str);
}

TEST(parse_width_clip) {
	char str[10];
	sscanf("helloworld", "%5s", str);
	ASSERT_STR_EQ("hello", str);
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

TEST(scanset_empty) {
	char str[10] = "test";
	int ret = sscanf("abc", "%[]", str);
	ASSERT_EQ(0, ret);  // Empty scanset
}

TEST(scanset_inverted_matches_all) {
	char str[10];
	sscanf("abc", "%[^]", str);
	// Inverted empty set matches everything
	ASSERT_STR_EQ("abc", str);
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

/* ============================================================================
 * FILE POSITIONING TESTS
 * ============================================================================ */
TEST_SUITE(positioning);

TEST(fseek_basic_set) {
	FILE *f = tmpfile();
	fputs("hello world", f);

	fseek(f, 0, SEEK_SET);
	int c = fgetc(f);
	ASSERT_EQ('h', c);

	fclose(f);
}

TEST(fseek_cur) {
	FILE *f = tmpfile();
	fputs("hello world", f);

	fseek(f, 0, SEEK_SET);
	fseek(f, 6, SEEK_CUR);  // Skip to 'w'
	int c = fgetc(f);
	ASSERT_EQ('w', c);

	fclose(f);
}

TEST(fseek_end) {
	FILE *f = tmpfile();
	fputs("hello", f);

	fseek(f, 0, SEEK_END);
	long pos = ftell(f);
	ASSERT_EQ(5, pos);

	fclose(f);
}

TEST(ftell_after_write) {
	FILE *f = tmpfile();
	fputs("test", f);
	long pos = ftell(f);
	ASSERT_EQ(4, pos);
	fclose(f);
}

TEST(rewind_basic) {
	FILE *f = tmpfile();
	fputs("hello", f);

	rewind(f);
	int c = fgetc(f);
	ASSERT_EQ('h', c);
	ASSERT_EQ(0, ferror(f));  // rewind clears errors

	fclose(f);
}

TEST(fgetpos_fsetpos) {
	FILE *f = tmpfile();
	fputs("hello world", f);

	fseek(f, 6, SEEK_SET);
	fpos_t pos;
	fgetpos(f, &pos);

	fseek(f, 0, SEEK_SET);  // Go back
	fsetpos(f, &pos);       // Restore position

	int c = fgetc(f);
	ASSERT_EQ('w', c);

	fclose(f);
}

/* ============================================================================
 * ERROR HANDLING TESTS
 * ============================================================================ */
TEST_SUITE(error_handling);

TEST(feof_after_read) {
	FILE *f = tmpfile();
	fputs("x", f);
	rewind(f);

	fgetc(f);  // Read 'x'
	ASSERT_EQ(0, feof(f));  // Not EOF yet

	fgetc(f);  // Try to read past end
	ASSERT_NE(0, feof(f));  // Now EOF

	fclose(f);
}

TEST(ferror_after_bad_write) {
	FILE *f = fopen("/dev/full", "w");  // Linux only - always full
	if (!f) return;  // Skip if not available

	fputc('x', f);
	fflush(f);

	// May set error on /dev/full
	// ASSERT_NE(0, ferror(f));  // Platform-dependent

	fclose(f);
}

TEST(clearerr_clears_flags) {
	FILE *f = tmpfile();
	fgetc(f);  // Cause EOF

	ASSERT_NE(0, feof(f));
	clearerr(f);
	ASSERT_EQ(0, feof(f));

	fclose(f);
}

TEST(perror_basic) {
	// Just ensure it doesn't crash
	errno = ENOENT;
	perror("Test error");
	ASSERT_TRUE(1);
}

/* ============================================================================
 * BUFFERING TESTS
 * ============================================================================ */
TEST_SUITE(buffering);

TEST(setvbuf_full) {
	FILE *f = tmpfile();
	char buf[128];

	int ret = setvbuf(f, buf, _IOFBF, 128);
	ASSERT_EQ(0, ret);

	fputs("test", f);  // Buffered, not flushed yet
	// Would need to check internal state

	fclose(f);
}

TEST(setvbuf_line) {
	FILE *f = tmpfile();

	setvbuf(f, NULL, _IOLBF, BUFSIZ);
	fputs("line1\n", f);  // Should flush on \n

	fclose(f);
}

TEST(setvbuf_unbuffered) {
	FILE *f = tmpfile();

	setvbuf(f, NULL, _IONBF, 0);
	fputs("test", f);  // Should write immediately

	fclose(f);
}

TEST(setbuf_null) {
	FILE *f = tmpfile();
	setbuf(f, NULL);  // Unbuffered

	fputs("test", f);

	fclose(f);
}

/* ============================================================================
 * CHARACTER I/O EDGE CASES
 * ============================================================================ */
TEST_SUITE(char_io);

TEST(ungetc_basic) {
	FILE *f = tmpfile();
	fputs("abc", f);
	rewind(f);

	int c1 = fgetc(f);  // 'a'
	ungetc(c1, f);
	int c2 = fgetc(f);  // Should re-read 'a'

	ASSERT_EQ('a', c1);
	ASSERT_EQ('a', c2);

	fclose(f);
}

TEST(ungetc_multiple) {
	FILE *f = tmpfile();
	fputs("abc", f);
	rewind(f);

	fgetc(f);  // 'a'
	ungetc('z', f);
	ungetc('y', f);

	ASSERT_EQ('y', fgetc(f));
	ASSERT_EQ('z', fgetc(f));
	ASSERT_EQ('b', fgetc(f));  // Back to normal stream

	fclose(f);
}

TEST(getchar_putchar) {
	// Just verify they don't crash
	putchar('x');
	ASSERT_TRUE(1);
}

/* ============================================================================
 * BUFFER I/O TESTS
 * ============================================================================ */
TEST_SUITE(buffer_io);

TEST(fread_partial) {
	FILE *f = tmpfile();
	fputs("hello", f);
	rewind(f);

	char buf[10];
	size_t n = fread(buf, 1, 3, f);  // Read only 3 bytes

	ASSERT_EQ(3, n);
	ASSERT_EQ('h', buf[0]);
	ASSERT_EQ('e', buf[1]);
	ASSERT_EQ('l', buf[2]);

	fclose(f);
}

TEST(fread_exact_size) {
	FILE *f = tmpfile();
	int data[] = {1, 2, 3, 4};
	fwrite(data, sizeof(int), 4, f);
	rewind(f);

	int result[4];
	size_t n = fread(result, sizeof(int), 4, f);

	ASSERT_EQ(4, n);
	ASSERT_EQ(1, result[0]);
	ASSERT_EQ(4, result[3]);

	fclose(f);
}

TEST(fwrite_binary) {
	FILE *f = tmpfile();
	unsigned char data[] = {0x00, 0xFF, 0x42};

	size_t n = fwrite(data, 1, 3, f);
	ASSERT_EQ(3, n);

	rewind(f);
	unsigned char result[3];
	fread(result, 1, 3, f);

	ASSERT_EQ(0x00, result[0]);
	ASSERT_EQ(0xFF, result[1]);
	ASSERT_EQ(0x42, result[2]);

	fclose(f);
}

/* ============================================================================
 * FILE MANIPULATION TESTS
 * ============================================================================ */
TEST_SUITE(file_ops);

TEST(remove_file) {
	FILE *f = fopen("/tmp/test_remove.txt", "w");
	fputs("test", f);
	fclose(f);

	int ret = remove("/tmp/test_remove.txt");
	ASSERT_EQ(0, ret);

	// Verify it's gone
	f = fopen("/tmp/test_remove.txt", "r");
	ASSERT_EQ(NULL, f);
}

TEST(rename_file) {
	FILE *f = fopen("/tmp/test_old.txt", "w");
	fputs("test", f);
	fclose(f);

	int ret = rename("/tmp/test_old.txt", "/tmp/test_new.txt");
	ASSERT_EQ(0, ret);

	// Verify new exists
	f = fopen("/tmp/test_new.txt", "r");
	ASSERT_NOT_NULL(f);
	fclose(f);

	remove("/tmp/test_new.txt");
}

TEST(tmpnam_generates_name) {
	char name1[L_tmpnam];
	char name2[L_tmpnam];

	tmpnam(name1);
	tmpnam(name2);

	ASSERT_NE(0, strcmp(name1, name2));  // Different names
	ASSERT_GT(strlen(name1), 0);
}

/* ============================================================================
 * POSIX EXTENSION TESTS
 * ============================================================================ */
TEST_SUITE(posix_ext);

#if JACL_HAS_POSIX

TEST(fdopen_basic) {
	int pipefd[2];
	pipe(pipefd);

	FILE *f = fdopen(pipefd[1], "w");
	ASSERT_NOT_NULL(f);

	fputs("test", f);
	fclose(f);

	char buf[10];
	read(pipefd[0], buf, 4);
	buf[4] = '\0';
	ASSERT_STR_EQ("test", buf);

	close(pipefd[0]);
}

TEST(popen_read) {
	FILE *f = popen("echo hello", "r");
	ASSERT_NOT_NULL(f);

	char buf[20];
	fgets(buf, 20, f);

	ASSERT_NOT_NULL(strstr(buf, "hello"));

	int status = pclose(f);
	ASSERT_EQ(0, status);
}

TEST(popen_write) {
	FILE *f = popen("cat > /tmp/test_popen.txt", "w");
	ASSERT_NOT_NULL(f);

	fputs("test data\n", f);
	pclose(f);

	// Verify it wrote
	FILE *check = fopen("/tmp/test_popen.txt", "r");
	char buf[20];
	fgets(buf, 20, check);
	ASSERT_STR_EQ("test data\n", buf);
	fclose(check);
	remove("/tmp/test_popen.txt");
}

TEST(fileno_basic) {
	FILE *f = tmpfile();
	int fd = fileno(f);

	ASSERT_GE(fd, 0);  // Valid fd

	fclose(f);
}

TEST(getline_basic) {
	FILE *f = tmpfile();
	fputs("line1\nline2\n", f);
	rewind(f);

	char *line = NULL;
	size_t len = 0;
	ssize_t n = getline(&line, &len, f);

	ASSERT_GT(n, 0);
	ASSERT_STR_EQ("line1\n", line);

	free(line);
	fclose(f);
}

TEST(getdelim_custom) {
	FILE *f = tmpfile();
	fputs("a:b:c:", f);
	rewind(f);

	char *line = NULL;
	size_t len = 0;
	ssize_t n = getdelim(&line, &len, ':', f);

	ASSERT_GT(n, 0);
	ASSERT_STR_EQ("a:", line);

	free(line);
	fclose(f);
}

#endif /* JACL_HAS_POSIX */

/* ============================================================================
 * FREOPEN TESTS
 * ============================================================================ */
TEST_SUITE(freopen);

TEST(freopen_basic) {
	FILE *f = fopen("/tmp/test_freopen.txt", "w");
	fputs("original", f);

	FILE *f2 = freopen("/tmp/test_freopen2.txt", "w", f);
	ASSERT_NOT_NULL(f2);
	ASSERT_EQ(f, f2);  // Same FILE pointer

	fputs("reopened", f2);
	fclose(f2);

	// Verify second file exists
	FILE *check = fopen("/tmp/test_freopen2.txt", "r");
	char buf[20];
	fgets(buf, 20, check);
	ASSERT_STR_EQ("reopened", buf);
	fclose(check);

	remove("/tmp/test_freopen.txt");
	remove("/tmp/test_freopen2.txt");
}

TEST(freopen_null_path) {
	// Reopen same file with different mode (POSIX extension)
	FILE *f = fopen("/tmp/test_freopen3.txt", "w");
	fputs("test", f);

	#ifdef __linux__
	FILE *f2 = freopen(NULL, "r", f);  // Reopen in read mode
	ASSERT_NOT_NULL(f2);

	char buf[20];
	fgets(buf, 20, f2);
	ASSERT_STR_EQ("test", buf);
	fclose(f2);
	#else
	fclose(f);
	#endif

	remove("/tmp/test_freopen3.txt");
}

/* ============================================================================
 * SCANF/FSCANF TESTS (actual input, not just sscanf)
 * ============================================================================ */
TEST_SUITE(scanf_io);

TEST(fscanf_basic) {
	FILE *f = tmpfile();
	fputs("42 hello 3.14", f);
	rewind(f);

	int i;
	char str[20];
	float fl;

	int ret = fscanf(f, "%d %s %f", &i, str, &fl);

	ASSERT_EQ(3, ret);
	ASSERT_EQ(42, i);
	ASSERT_STR_EQ("hello", str);
	ASSERT_FLOAT_EQ(3.14, fl, 0.01);

	fclose(f);
}

TEST(fscanf_partial) {
	FILE *f = tmpfile();
	fputs("123 abc", f);
	rewind(f);

	int i;
	float fl;  // Will fail

	int ret = fscanf(f, "%d %f", &i, &fl);

	ASSERT_EQ(1, ret);  // Only first field matched
	ASSERT_EQ(123, i);

	fclose(f);
}

/* ============================================================================
 * FOPEN MODES TESTS
 * ============================================================================ */
TEST_SUITE(fopen_modes);

TEST(mode_read_only) {
	FILE *f = fopen("/tmp/test_modes.txt", "w");
	fputs("test", f);
	fclose(f);

	f = fopen("/tmp/test_modes.txt", "r");
	ASSERT_NOT_NULL(f);

	char buf[10];
	fgets(buf, 10, f);
	ASSERT_STR_EQ("test", buf);

	// Write should fail (but may not return error immediately)
	fputc('x', f);
	fflush(f);
	// ASSERT_NE(0, ferror(f));  // Platform-dependent

	fclose(f);
	remove("/tmp/test_modes.txt");
}

TEST(mode_write_truncates) {
	FILE *f = fopen("/tmp/test_truncate.txt", "w");
	fputs("original content", f);
	fclose(f);

	f = fopen("/tmp/test_truncate.txt", "w");  // Should truncate
	fputs("new", f);
	fclose(f);

	f = fopen("/tmp/test_truncate.txt", "r");
	char buf[20];
	fgets(buf, 20, f);
	ASSERT_STR_EQ("new", buf);
	fclose(f);

	remove("/tmp/test_truncate.txt");
}

TEST(mode_append) {
	FILE *f = fopen("/tmp/test_append.txt", "w");
	fputs("first", f);
	fclose(f);

	f = fopen("/tmp/test_append.txt", "a");  // Append mode
	fputs("second", f);
	fclose(f);

	f = fopen("/tmp/test_append.txt", "r");
	char buf[20];
	fgets(buf, 20, f);
	ASSERT_STR_EQ("firstsecond", buf);
	fclose(f);

	remove("/tmp/test_append.txt");
}

TEST(mode_read_plus) {
	FILE *f = fopen("/tmp/test_rplus.txt", "w");
	fputs("test", f);
	fclose(f);

	f = fopen("/tmp/test_rplus.txt", "r+");  // Read + write
	ASSERT_NOT_NULL(f);

	char buf[10];
	fgets(buf, 10, f);  // Read
	ASSERT_STR_EQ("test", buf);

	rewind(f);
	fputs("best", f);  // Write (should work)
	fclose(f);

	f = fopen("/tmp/test_rplus.txt", "r");
	fgets(buf, 10, f);
	ASSERT_STR_EQ("best", buf);
	fclose(f);

	remove("/tmp/test_rplus.txt");
}

TEST(mode_write_plus) {
	FILE *f = fopen("/tmp/test_wplus.txt", "w+");  // Read + write, truncate
	fputs("test", f);
	rewind(f);

	char buf[10];
	fgets(buf, 10, f);  // Read should work
	ASSERT_STR_EQ("test", buf);

	fclose(f);
	remove("/tmp/test_wplus.txt");
}

TEST(mode_binary) {
	FILE *f = fopen("/tmp/test_binary.bin", "wb");
	unsigned char data[] = {0x00, 0x0A, 0x0D, 0xFF};  // Including \n and \r
	fwrite(data, 1, 4, f);
	fclose(f);

	f = fopen("/tmp/test_binary.bin", "rb");
	unsigned char result[4];
	fread(result, 1, 4, f);

	ASSERT_EQ(0x00, result[0]);
	ASSERT_EQ(0x0A, result[1]);
	ASSERT_EQ(0x0D, result[2]);
	ASSERT_EQ(0xFF, result[3]);

	fclose(f);
	remove("/tmp/test_binary.bin");
}

/* ============================================================================
 * STREAM STATE TESTS
 * ============================================================================ */
TEST_SUITE(stream_state);

TEST(fclose_null) {
	int ret = fclose(NULL);
	ASSERT_EQ(EOF, ret);
}

TEST(operations_after_error) {
	FILE *f = tmpfile();
	fgetc(f);  // Cause EOF

	ASSERT_NE(0, feof(f));

	// Operations should still work after clearerr
	clearerr(f);
	fputs("test", f);
	fflush(f);

	ASSERT_EQ(0, ferror(f));

	fclose(f);
}

TEST(fopen_nonexistent) {
	FILE *f = fopen("/nonexistent/path/file.txt", "r");
	ASSERT_EQ(NULL, f);
	ASSERT_NE(0, errno);
}

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(very_long_line) {
	FILE *f = tmpfile();

	// Write 10KB line
	for (int i = 0; i < 10000; i++) {
		fputc('A', f);
	}
	fputc('\n', f);
	rewind(f);

	char buf[20000];
	char *ret = fgets(buf, 20000, f);

	ASSERT_NOT_NULL(ret);
	ASSERT_EQ(10001, strlen(buf));  // 10000 + \n

	fclose(f);
}

TEST(alternating_read_write) {
	FILE *f = tmpfile();

	fputs("line1\n", f);
	rewind(f);

	char buf[10];
	fgets(buf, 10, f);
	ASSERT_STR_EQ("line1\n", buf);

	fputs("line2\n", f);
	rewind(f);

	fgets(buf, 10, f);  // Read first line
	fgets(buf, 10, f);  // Read second line
	ASSERT_STR_EQ("line2\n", buf);

	fclose(f);
}

TEST(fread_fwrite_zero) {
	FILE *f = tmpfile();

	size_t n1 = fwrite("test", 0, 5, f);  // size = 0
	ASSERT_EQ(0, n1);

	size_t n2 = fwrite("test", 5, 0, f);  // nmemb = 0
	ASSERT_EQ(0, n2);

	char buf[10];
	size_t n3 = fread(buf, 0, 5, f);
	ASSERT_EQ(0, n3);

	fclose(f);
}

TEST(tmpfile_multiple) {
	FILE *f1 = tmpfile();
	FILE *f2 = tmpfile();
	FILE *f3 = tmpfile();

	ASSERT_NOT_NULL(f1);
	ASSERT_NOT_NULL(f2);
	ASSERT_NOT_NULL(f3);
	ASSERT_NE(f1, f2);
	ASSERT_NE(f2, f3);

	fputs("file1", f1);
	fputs("file2", f2);
	fputs("file3", f3);

	fclose(f1);
	fclose(f2);
	fclose(f3);
}

TEST_MAIN()
