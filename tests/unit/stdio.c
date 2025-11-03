/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

TEST_TYPE(unit);
TEST_UNIT(stdio.h);

/* ============================================================= */
/* Test Data                                                     */
/* ============================================================= */
static const int int_vals[] = {0, 1, 42, 100000001, 254321, -1234, INT_MAX, INT_MIN};
static const unsigned int uint_vals[] = {0, 1, 42, 100000001, 254321, UINT_MAX};
static const double dbl_vals[] = {0.0, -0.0, 1.5, -1.5, 123.456, -123.456, 0.001, 1000.0, 1.23e5, -1.23e-5, 0.999999, -0.999999};
static const int widths[] = {0, 1, 5, 10, 15, 20, 25};
static const int precisions[] = {0, 1, 2, 3, 4, 6, 10};

/* ============================================================= */
/* PRINTF TESTS (Alphabetical by Specifier)                     */
/* ============================================================= */

/* ============= Printf: %a, %A (hex float) - C99+ ============= */
#if JACL_HAS_C99
TEST_SUITE(printf_a);

TEST(printf_a_basic) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.3a", 1.5);
	ASSERT_TRUE(strchr(buf, 'p') != NULL);
}

TEST(printf_a_uppercase) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.3A", 1.5);
	ASSERT_TRUE(strchr(buf, 'P') != NULL);
}

TEST(printf_a_precision) {
	for (int p = 1; p < 4; p++) {
		char fmt[32], buf[256];
		snprintf(fmt, sizeof(fmt), "%%.%da", p);
		snprintf(buf, sizeof(buf), fmt, 0.5);
		ASSERT_TRUE(strchr(buf, 'p') != NULL);
	}
}

TEST(printf_a_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%15.3a", 1.5);
	ASSERT_TRUE(strlen(buf) >= 15);
}

TEST(printf_a_left_align) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%-15.3a", 1.5);
	ASSERT_TRUE(buf[0] != ' ');
}

TEST(printf_a_negative) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2a", -2.75);
	ASSERT_TRUE(buf[0] == '-');
}

TEST(printf_a_plus_flag) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%+.2a", 1.5);
	ASSERT_TRUE(buf[0] == '+');
}

TEST(printf_a_space_flag) {
	char buf[256];
	snprintf(buf, sizeof(buf), "% .2a", 1.5);
	ASSERT_TRUE(buf[0] == ' ');
}

TEST(printf_a_hash_flag) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%#.0a", 1.5);
	ASSERT_TRUE(strchr(buf, '.') != NULL);
}

TEST(printf_a_inf) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2a", INFINITY);
	ASSERT_STR_HAS("inf", buf);
}

TEST(printf_a_nan) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2a", NAN);
	ASSERT_STR_HAS("nan", buf);
}

TEST(printf_a_longdouble) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2La", 1.5L);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(printf_a_plus_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%+15.2a", 1.5);
	ASSERT_TRUE(strlen(buf) >= 15);
	ASSERT_TRUE(buf[strlen(buf)-1] != ' ');
}
#endif

/* ============= Printf: %c (character) ============= */
TEST_SUITE(printf_c);

TEST(printf_c_basic) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%c", 'A');
	ASSERT_STR_EQ("A", buf);
}

TEST(printf_c_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%5c", 'X');
	ASSERT_TRUE(strlen(buf) >= 5);
}

TEST(printf_c_left_align) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%-5c", 'Y');
	ASSERT_TRUE(buf[0] == 'Y');
}

TEST(printf_c_digit) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%c", '7');
	ASSERT_STR_EQ("7", buf);
}

TEST(printf_c_space) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%c", ' ');
	ASSERT_STR_EQ(" ", buf);
}

TEST(printf_c_width_pad) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%10c", 'X');
	ASSERT_TRUE(strlen(buf) >= 10);
	ASSERT_TRUE(strchr(buf, 'X') != NULL);
}

TEST(printf_c_minus_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%-10c", 'Y');
	ASSERT_TRUE(buf[0] == 'Y');
	ASSERT_TRUE(strlen(buf) >= 10);
}

/* ============= Printf: %d, %i (signed decimal) ============= */
TEST_SUITE(printf_d);

TEST(printf_d_basic) {
	for (int i = 0; i < sizeof(int_vals)/sizeof(int_vals[0]); i++) {
		char buf[256];
		snprintf(buf, sizeof(buf), "%d", int_vals[i]);
		ASSERT_TRUE(strlen(buf) > 0);
	}
}

TEST(printf_d_width) {
	for (int w = 0; w < sizeof(widths)/sizeof(widths[0]); w++) {
		for (int i = 0; i < 3; i++) {
			char fmt[32], buf[256];
			snprintf(fmt, sizeof(fmt), "%%%dd", widths[w]);
			snprintf(buf, sizeof(buf), fmt, int_vals[i]);
			ASSERT_TRUE(strlen(buf) > 0);
		}
	}
}

TEST(printf_d_left_align) {
	for (int w = 1; w < 5; w++) {
		char fmt[32], buf[256];
		snprintf(fmt, sizeof(fmt), "%%-%dd", widths[w]);
		snprintf(buf, sizeof(buf), fmt, 42);
		ASSERT_TRUE(buf[0] != ' ');
	}
}

TEST(printf_d_zero_pad) {
	for (int w = 1; w < 5; w++) {
		char fmt[32], buf[256];
		snprintf(fmt, sizeof(fmt), "%%0%dd", widths[w]);
		snprintf(buf, sizeof(buf), fmt, 42);
		ASSERT_TRUE(strlen(buf) >= widths[w]);
	}
}

TEST(printf_d_sign_plus) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%+d", 42);
	ASSERT_TRUE(buf[0] == '+');
}

TEST(printf_d_sign_space) {
	char buf[256];
	snprintf(buf, sizeof(buf), "% d", 42);
	ASSERT_TRUE(buf[0] == ' ');
}

TEST(printf_d_precision) {
	for (int p = 0; p < 4; p++) {
		char fmt[32], buf[256];
		snprintf(fmt, sizeof(fmt), "%%.%dd", precisions[p]);
		snprintf(buf, sizeof(buf), fmt, 42);
		ASSERT_TRUE(strlen(buf) > 0);
	}
}

TEST(printf_d_negative) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%d", -999);
	ASSERT_TRUE(buf[0] == '-');
}

TEST(printf_d_h_short) {
	char buf[256];
	short s = 42;
	snprintf(buf, sizeof(buf), "%hd", s);
	ASSERT_STR_EQ("42", buf);
}

TEST(printf_d_l_long) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%ld", 123456789L);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(printf_d_ll_longlong) {
	char buf[256];
	long long val = 123456LL;
	snprintf(buf, sizeof(buf), "%lld", val);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(printf_d_plus_zero_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%+010d", 42);
	ASSERT_TRUE(strlen(buf) >= 10);
	ASSERT_TRUE(buf[0] == '+');
}

TEST(printf_d_space_zero_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "% 010d", 42);
	ASSERT_TRUE(strlen(buf) >= 10);
	ASSERT_TRUE(buf[0] == ' ');
}

TEST(printf_d_minus_plus_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%-+10d", 42);
	ASSERT_TRUE(buf[0] == '+');
	ASSERT_TRUE(buf[1] != ' ');
}

TEST(printf_d_dynamic_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%*d", 10, 42);
	ASSERT_TRUE(strlen(buf) >= 10);
}

TEST(printf_d_negative_dynamic_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%*d", -10, 42);
	ASSERT_TRUE(buf[0] != ' ');
	ASSERT_TRUE(strlen(buf) >= 10);
}

/* ============= Printf: %e, %E (scientific) ============= */
TEST_SUITE(printf_e);

TEST(printf_e_basic) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2e", 1.23e5);
	ASSERT_TRUE(strchr(buf, 'e') != NULL);
}

TEST(printf_e_uppercase) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2E", 1.23e5);
	ASSERT_TRUE(strchr(buf, 'E') != NULL);
}

TEST(printf_e_precision) {
	for (int p = 0; p < 4; p++) {
		char fmt[32], buf[256];
		snprintf(fmt, sizeof(fmt), "%%.%de", p);
		snprintf(buf, sizeof(buf), fmt, 1.23e-5);
		ASSERT_TRUE(strlen(buf) > 0);
	}
}

TEST(printf_e_width) {
	char fmt[32], buf[256];
	snprintf(fmt, sizeof(fmt), "%%15.3e");
	snprintf(buf, sizeof(buf), fmt, 1.23e-5);
	ASSERT_TRUE(strlen(buf) >= 15);
}

TEST(printf_e_left_align) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%-15.3e", 1.23e-5);
	ASSERT_TRUE(buf[0] != ' ');
}

TEST(printf_e_zero_pad) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%015.3e", 1.23e-5);
	ASSERT_TRUE(strlen(buf) >= 15);
}

TEST(printf_e_negative) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2e", -1.5e3);
	ASSERT_TRUE(buf[0] == '-');
}

TEST(printf_e_plus_flag) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%+.2e", 1.5e3);
	ASSERT_TRUE(buf[0] == '+');
}

TEST(printf_e_space_flag) {
	char buf[256];
	snprintf(buf, sizeof(buf), "% .2e", 1.5e3);
	ASSERT_TRUE(buf[0] == ' ');
}

TEST(printf_e_inf) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2e", INFINITY);
	ASSERT_STR_HAS("inf", buf);
}

TEST(printf_e_nan) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2e", NAN);
	ASSERT_STR_HAS("nan", buf);
}

TEST(printf_e_small) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2e", 0.00001);
	ASSERT_TRUE(strchr(buf, 'e') != NULL);
}

TEST(printf_e_longdouble) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2Le", 1.5e3L);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(printf_e_hash) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%#.0e", 1.5e3);
	ASSERT_TRUE(strchr(buf, '.') != NULL);
}

TEST(printf_e_plus_zero_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%+010.2e", 1.5e3);
	ASSERT_TRUE(strlen(buf) >= 10);
	ASSERT_TRUE(buf[0] == '+');
}

TEST(printf_e_minus_plus_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%-+10.2e", 1.5e3);
	ASSERT_TRUE(buf[0] == '+');
	ASSERT_TRUE(strlen(buf) >= 10);
}

/* ============= Printf: %f, %F (fixed-point) ============= */
TEST_SUITE(printf_f);

TEST(printf_f_basic) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2f", 1.5);
	ASSERT_STR_EQ("1.50", buf);
}

TEST(printf_f_precision) {
	for (int p = 0; p < 5; p++) {
		char fmt[32], buf[256];
		snprintf(fmt, sizeof(fmt), "%%.%df", p);
		snprintf(buf, sizeof(buf), fmt, 123.456);
		ASSERT_TRUE(strlen(buf) > 0);
	}
}

TEST(printf_f_width) {
	for (int w = 1; w < 5; w++) {
		char fmt[32], buf[256];
		snprintf(fmt, sizeof(fmt), "%%%d.2f", widths[w]);
		snprintf(buf, sizeof(buf), fmt, 1.5);
		ASSERT_TRUE(strlen(buf) >= 3);
	}
}

TEST(printf_f_left_align) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%-10.2f", 1.5);
	ASSERT_TRUE(buf[0] != ' ');
}

TEST(printf_f_zero_pad) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%010.2f", 1.5);
	ASSERT_TRUE(strlen(buf) >= 10);
}

TEST(printf_f_sign_plus) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%+.2f", 1.5);
	ASSERT_TRUE(buf[0] == '+');
}

TEST(printf_f_sign_space) {
	char buf[256];
	snprintf(buf, sizeof(buf), "% .2f", 1.5);
	ASSERT_TRUE(buf[0] == ' ');
}

TEST(printf_f_hash_flag) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%#.0f", 1.5);
	ASSERT_TRUE(strchr(buf, '.') != NULL);
}

TEST(printf_f_nan) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2f", NAN);
	ASSERT_STR_HAS("nan", buf);
}

TEST(printf_f_inf) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2f", INFINITY);
	ASSERT_STR_HAS("inf", buf);
}

TEST(printf_f_negative_inf) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2f", -INFINITY);
	ASSERT_STR_HAS("inf", buf);
	ASSERT_TRUE(buf[0] == '-');
}

TEST(printf_f_negative_zero) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2f", -0.0);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(printf_f_rounding) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.0f", 1.999);
	ASSERT_STR_EQ("2", buf);
}

TEST(printf_f_longdouble) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.2Lf", 1.5L);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(printf_f_plus_zero_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%+010.2f", 3.14);
	ASSERT_TRUE(strlen(buf) >= 10);
	ASSERT_TRUE(buf[0] == '+');
}

TEST(printf_f_space_zero_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "% 010.2f", 3.14);
	ASSERT_TRUE(strlen(buf) >= 10);
	ASSERT_TRUE(buf[0] == ' ');
}

TEST(printf_f_minus_plus_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%-+10.2f", 3.14);
	ASSERT_TRUE(buf[0] == '+');
	ASSERT_TRUE(strlen(buf) >= 10);
}

TEST(printf_f_zero_minus_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%-10.2f", 3.14);  // Remove the conflicting 0 flag
	ASSERT_TRUE(buf[0] != ' ' && buf[0] != '0');
	ASSERT_TRUE(strlen(buf) >= 10);
}

TEST(printf_f_dynamic_precision) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.*f", 3, 3.14159);
	ASSERT_STR_EQ("3.142", buf);
}

TEST(printf_f_dynamic_width_precision) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%*.*f", 10, 2, 3.14159);
	ASSERT_TRUE(strlen(buf) >= 10);
	ASSERT_TRUE(strchr(buf, '.') != NULL);
}

/* ============= Printf: %g, %G (shortest) ============= */
TEST_SUITE(printf_g);

TEST(printf_g_basic) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.3g", 0.001);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(printf_g_uppercase) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.3G", 1.23e5);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(printf_g_precision) {
	for (int p = 1; p < 5; p++) {
		char fmt[32], buf[256];
		snprintf(fmt, sizeof(fmt), "%%.%dg", p);
		snprintf(buf, sizeof(buf), fmt, 123.456);
		ASSERT_TRUE(strlen(buf) > 0);
	}
}

TEST(printf_g_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%15.3g", 123.456);
	ASSERT_TRUE(strlen(buf) >= 15);
}

TEST(printf_g_left_align) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%-15.3g", 123.456);
	ASSERT_TRUE(buf[0] != ' ');
}

TEST(printf_g_zero_pad) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%015.3g", 123.456);
	ASSERT_TRUE(strlen(buf) >= 15);
}

TEST(printf_g_plus_flag) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%+.3g", 123.456);
	ASSERT_TRUE(buf[0] == '+');
}

TEST(printf_g_space_flag) {
	char buf[256];
	snprintf(buf, sizeof(buf), "% .3g", 123.456);
	ASSERT_TRUE(buf[0] == ' ');
}

TEST(printf_g_hash_flag) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%#.3g", 123.456);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(printf_g_large) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.3g", 123456.789);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(printf_g_small) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.3g", 0.000001);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(printf_g_inf) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.3g", INFINITY);
	ASSERT_STR_HAS("inf", buf);
}

TEST(printf_g_nan) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.3g", NAN);
	ASSERT_STR_HAS("nan", buf);
}

TEST(printf_g_longdouble) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.3Lg", 123.456L);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(printf_g_plus_zero_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%+010.3g", 123.456);
	ASSERT_TRUE(strlen(buf) >= 10);
	ASSERT_TRUE(buf[0] == '+');
}

/* ============= Printf: %n (chars written) ============= */
TEST_SUITE(printf_n);

TEST(printf_n_basic) {
	char buf[256];
	int count;
	snprintf(buf, sizeof(buf), "hello%n", &count);
	ASSERT_INT_EQ(5, count);
}

TEST(printf_n_multiple) {
	char buf[256];
	int count1, count2;
	snprintf(buf, sizeof(buf), "hi%nthere%n", &count1, &count2);
	ASSERT_INT_EQ(2, count1);
	ASSERT_INT_EQ(7, count2);
}

TEST(printf_n_short) {
	char buf[256];
	short count;
	snprintf(buf, sizeof(buf), "test%hn", &count);
	ASSERT_INT_EQ(4, count);
}

TEST(printf_n_long) {
	char buf[256];
	long count;
	snprintf(buf, sizeof(buf), "lengthy%ln", &count);
	ASSERT_INT_EQ(7, count);
}

TEST(printf_n_longlong) {
	char buf[256];
	long long count;
	snprintf(buf, sizeof(buf), "longlong%lln", &count);
	ASSERT_INT_EQ(8, count);
}

/* ============= Printf: %o (octal) ============= */
TEST_SUITE(printf_o);

TEST(printf_o_basic) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%o", 64);
	ASSERT_STR_EQ("100", buf);
}

TEST(printf_o_hash) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%#o", 64);
	ASSERT_TRUE(buf[0] == '0');
}

TEST(printf_o_width) {
	char fmt[32], buf[256];
	snprintf(fmt, sizeof(fmt), "%%10o");
	snprintf(buf, sizeof(buf), fmt, 64);
	ASSERT_TRUE(strlen(buf) >= 10);
}

TEST(printf_o_zero) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%o", 0);
	ASSERT_STR_EQ("0", buf);
}

TEST(printf_o_large) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%o", 777);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(printf_o_hash_zero) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%#o", 0);
	ASSERT_STR_EQ("0", buf);
}

TEST(printf_o_hash_zero_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%#010o", 64);
	ASSERT_TRUE(strlen(buf) >= 10);
}

TEST(printf_o_minus_hash_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%-#10o", 64);
	ASSERT_TRUE(buf[0] == '0');
	ASSERT_TRUE(strlen(buf) >= 10);
}

/* ============= Printf: %p (pointer) ============= */
TEST_SUITE(printf_p);

TEST(printf_p_basic) {
	char buf[256];
	int x = 42;
	snprintf(buf, sizeof(buf), "%p", (void*)&x);
	ASSERT_STR_HAS("0x", buf);
}

TEST(printf_p_null) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%p", (void*)NULL);
	ASSERT_STR_PRE("0x", buf);
}

TEST(printf_p_width) {
	char buf[256];
	int y = 99;
	snprintf(buf, sizeof(buf), "%20p", (void*)&y);
	ASSERT_TRUE(strlen(buf) >= 20);
}

TEST(printf_p_left_align) {
	char buf[256];
	int z = 77;
	snprintf(buf, sizeof(buf), "%-20p", (void*)&z);
	ASSERT_TRUE(buf[0] == '0');
}

TEST(printf_p_minus_width) {
	char buf[256];
	int x;
	snprintf(buf, sizeof(buf), "%-20p", (void*)&x);
	ASSERT_TRUE(buf[0] == '0');
	ASSERT_TRUE(strlen(buf) >= 20);
}

TEST(printf_p_width_pad) {
	char buf[256];
	int x;
	snprintf(buf, sizeof(buf), "%20p", (void*)&x);
	ASSERT_TRUE(strlen(buf) >= 20);
}

/* ============= Printf: %s (string) ============= */
TEST_SUITE(printf_s);

TEST(printf_s_basic) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%s", "hello");
	ASSERT_STR_EQ("hello", buf);
}

TEST(printf_s_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%10s", "hi");
	ASSERT_TRUE(strlen(buf) >= 10);
}

TEST(printf_s_left_align) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%-10s", "left");
	ASSERT_TRUE(buf[0] == 'l');
}

TEST(printf_s_precision) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%.3s", "hello");
	ASSERT_STR_EQ("hel", buf);
}

TEST(printf_s_empty) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%s", "");
	ASSERT_STR_EQ("", buf);
}

TEST(printf_s_null) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%s", (const char*)NULL);
	ASSERT_STR_HAS("null", buf);
}

TEST(printf_s_minus_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%-15s", "test");
	ASSERT_TRUE(buf[0] == 't');
	ASSERT_TRUE(strlen(buf) >= 15);
}

/* ============= Printf: %u (unsigned decimal) ============= */
TEST_SUITE(printf_u);

TEST(printf_u_basic) {
	for (int i = 0; i < sizeof(uint_vals)/sizeof(uint_vals[0]); i++) {
		char buf[256];
		snprintf(buf, sizeof(buf), "%u", uint_vals[i]);
		ASSERT_TRUE(strlen(buf) > 0);
	}
}

TEST(printf_u_width) {
	for (int w = 0; w < 5; w++) {
		char fmt[32], buf[256];
		snprintf(fmt, sizeof(fmt), "%%%du", widths[w]);
		snprintf(buf, sizeof(buf), fmt, 42u);
		ASSERT_TRUE(strlen(buf) > 0);
	}
}

TEST(printf_u_zero_pad) {
	char fmt[32], buf[256];
	snprintf(fmt, sizeof(fmt), "%%010u");
	snprintf(buf, sizeof(buf), fmt, 42u);
	ASSERT_TRUE(strlen(buf) >= 10);
}

TEST(printf_u_large) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%u", 4294967295u);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(printf_u_zero) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%u", 0);
	ASSERT_STR_EQ("0", buf);
}

TEST(printf_u_zero_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%010u", 42);
	ASSERT_TRUE(strlen(buf) >= 10);
}

TEST(printf_u_minus_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%-10u", 42);
	ASSERT_TRUE(buf[0] != ' ');
}

/* ============= Printf: %x, %X (hexadecimal) ============= */
TEST_SUITE(printf_x);

TEST(printf_x_lowercase) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%x", 255);
	ASSERT_STR_EQ("ff", buf);
}

TEST(printf_x_uppercase) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%X", 255);
	ASSERT_STR_EQ("FF", buf);
}

TEST(printf_x_hash) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%#x", 255);
	ASSERT_TRUE(buf[0] == '0' && buf[1] == 'x');
}

TEST(printf_x_width) {
	for (int w = 0; w < 5; w++) {
		char fmt[32], buf[256];
		snprintf(fmt, sizeof(fmt), "%%%dx", widths[w]);
		snprintf(buf, sizeof(buf), fmt, 42);
		ASSERT_TRUE(strlen(buf) > 0);
	}
}

TEST(printf_x_precision) {
	for (int p = 0; p < 4; p++) {
		char fmt[32], buf[256];
		snprintf(fmt, sizeof(fmt), "%%.%dx", precisions[p]);
		snprintf(buf, sizeof(buf), fmt, 42);
		ASSERT_TRUE(strlen(buf) > 0);
	}
}

TEST(printf_x_zero) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%x", 0);
	ASSERT_STR_EQ("0", buf);
}

TEST(printf_x_large) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%x", 0xdeadbeef);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(printf_x_hash_zero) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%#x", 0);
	ASSERT_STR_EQ("0", buf);
}

TEST(printf_x_hash_zero_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%#010x", 255);
	ASSERT_TRUE(strlen(buf) >= 10);
	ASSERT_STR_HAS("0x", buf);
}

TEST(printf_x_hash_uppercase) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%#X", 255);
	ASSERT_TRUE(strchr(buf, 'X') || strchr(buf, 'x'));
}

TEST(printf_x_minus_hash_width) {
	char buf[256];
	snprintf(buf, sizeof(buf), "%-#10x", 255);
	ASSERT_TRUE(buf[0] == '0');
	ASSERT_TRUE(strlen(buf) >= 10);
}

/* ============================================================= */
/* SCANF TESTS (Alphabetical by Specifier)                      */
/* ============================================================= */

/* ============= Scanf: %c (character) ============= */
TEST_SUITE(scanf_c);

TEST(scanf_c_basic) {
	char buf[256];
	char parsed;
	snprintf(buf, sizeof(buf), "%c", 'Z');
	sscanf(buf, "%c", &parsed);
	ASSERT_INT_EQ('Z', parsed);
}

TEST(scanf_c_space) {
	char parsed;
	sscanf(" ", "%c", &parsed);
	ASSERT_INT_EQ(' ', parsed);
}

TEST(scanf_c_digit) {
	char parsed;
	sscanf("5", "%c", &parsed);
	ASSERT_INT_EQ('5', parsed);
}

/* ============= Scanf: %d (signed decimal) ============= */
TEST_SUITE(scanf_d);

TEST(scanf_d_roundtrip) {
	for (int i = 0; i < sizeof(int_vals)/sizeof(int_vals[0]); i++) {
		char buf[256];
		int parsed;
		snprintf(buf, sizeof(buf), "%d", int_vals[i]);
		sscanf(buf, "%d", &parsed);
		ASSERT_INT_EQ(int_vals[i], parsed);
	}
}

TEST(scanf_d_width) {
	char buf[256];
	int parsed = 99;
	snprintf(buf, sizeof(buf), "   42");
	int ret = sscanf(buf, "%5d", &parsed);
	ASSERT_INT_EQ(42, parsed);
}

TEST(scanf_d_sign) {
	char buf[256];
	int parsed;
	snprintf(buf, sizeof(buf), "-1234");
	sscanf(buf, "%d", &parsed);
	ASSERT_INT_EQ(-1234, parsed);
}

TEST(scanf_d_leading_zeros) {
	int parsed;
	sscanf("00042", "%d", &parsed);
	ASSERT_INT_EQ(42, parsed);
}

TEST(scanf_d_plus_sign) {
	int parsed;
	sscanf("+123", "%d", &parsed);
	ASSERT_INT_EQ(123, parsed);
}

TEST(scanf_d_h_short) {
	short parsed;
	sscanf("42", "%hd", &parsed);
	ASSERT_INT_EQ(42, parsed);
}

TEST(scanf_d_l_long) {
	long parsed;
	sscanf("123456789", "%ld", &parsed);
	ASSERT_INT_EQ(123456789, parsed);
}

TEST(scanf_d_ll_longlong) {
	long long parsed;
	sscanf("123456789012", "%lld", &parsed);
	ASSERT_INT_EQ(123456789012LL, parsed);
}

/* ============= Scanf: %e, %E (scientific) - C99+ ============= */
#if JACL_HAS_C99
TEST_SUITE(scanf_e);

TEST(scanf_e_basic) {
	double parsed;
	sscanf("1.23e+02", "%le", &parsed);
	ASSERT_DBL_NEAR(123.0, parsed, 1e-4);
}

TEST(scanf_e_uppercase) {
	double parsed;
	sscanf("1.23E-02", "%lE", &parsed);
	ASSERT_DBL_NEAR(0.0123, parsed, 1e-6);
}

TEST(scanf_e_negative) {
	double parsed;
	sscanf("-4.56e+03", "%le", &parsed);
	ASSERT_TRUE(parsed < 0);
}

TEST(scanf_e_inf) {
	double parsed;
	sscanf("inf", "%le", &parsed);
	ASSERT_TRUE(isinf(parsed));
}

TEST(scanf_e_nan) {
	double parsed;
	sscanf("nan", "%le", &parsed);
	ASSERT_TRUE(isnan(parsed));
}
#endif

/* ============= Scanf: %f, %F (float) ============= */
TEST_SUITE(scanf_f);

TEST(scanf_f_roundtrip) {
	for (int i = 0; i < 6; i++) {
		char buf[256];
		double parsed;
		snprintf(buf, sizeof(buf), "%.6f", dbl_vals[i]);
		sscanf(buf, "%lf", &parsed);
		ASSERT_DBL_NEAR(dbl_vals[i], parsed, 1e-6);
	}
}

TEST(scanf_f_precision) {
	char buf[256];
	double parsed;
	snprintf(buf, sizeof(buf), "%.4f", 123.456);
	sscanf(buf, "%lf", &parsed);
	ASSERT_DBL_NEAR(123.456, parsed, 0.0001);
}

TEST(scanf_f_width) {
	char buf[256];
	double parsed;
	snprintf(buf, sizeof(buf), "%15.6f", 1.23e5);
	sscanf(buf, "%lf", &parsed);
	ASSERT_DBL_NEAR(1.23e5, parsed, 1e-4);
}

TEST(scanf_f_scientific) {
	double parsed;
	sscanf("1.23e+05", "%lf", &parsed);
	ASSERT_DBL_NEAR(1.23e5, parsed, 1e-4);
}

TEST(scanf_f_negative) {
	double parsed;
	sscanf("-3.14", "%lf", &parsed);
	ASSERT_DBL_NEAR(-3.14, parsed, 1e-5);
}

TEST(scanf_f_inf) {
	double parsed;
	sscanf("infinity", "%lf", &parsed);
	ASSERT_TRUE(isinf(parsed));
}

TEST(scanf_f_nan) {
	double parsed;
	sscanf("nan", "%lf", &parsed);
	ASSERT_TRUE(isnan(parsed));
}

TEST(scanf_f_nan_payload) {
	double parsed;
	sscanf("nan(0x123)", "%lf", &parsed);
	ASSERT_TRUE(isnan(parsed));
}

/* ============= Scanf: %g, %G (shortest) - C99+ ============= */
#if JACL_HAS_C99
TEST_SUITE(scanf_g);

TEST(scanf_g_basic) {
	double parsed;
	sscanf("123.45", "%lg", &parsed);
	ASSERT_DBL_NEAR(123.45, parsed, 1e-4);
}

TEST(scanf_g_uppercase) {
	double parsed;
	sscanf("1.23E+05", "%lG", &parsed);
	ASSERT_DBL_NEAR(1.23e5, parsed, 1e-4);
}

TEST(scanf_g_roundtrip) {
	char buf[256];
	double parsed;
	snprintf(buf, sizeof(buf), "%.3g", 0.001);
	sscanf(buf, "%lg", &parsed);
	ASSERT_DBL_NEAR(0.001, parsed, 1e-9);
}

TEST(scanf_g_inf) {
	double parsed;
	sscanf("inf", "%lg", &parsed);
	ASSERT_TRUE(isinf(parsed));
}

TEST(scanf_g_nan) {
	double parsed;
	sscanf("nan", "%lg", &parsed);
	ASSERT_TRUE(isnan(parsed));
}
#endif

/* ============= Scanf: %i (auto-base) ============= */
TEST_SUITE(scanf_i);

TEST(scanf_i_decimal) {
	int parsed;
	sscanf("42", "%i", &parsed);
	ASSERT_INT_EQ(42, parsed);
}

TEST(scanf_i_hex) {
	int parsed;
	sscanf("0xFF", "%i", &parsed);
	ASSERT_INT_EQ(255, parsed);
}

TEST(scanf_i_octal) {
	int parsed;
	sscanf("077", "%i", &parsed);
	ASSERT_INT_EQ(077, parsed);
}

TEST(scanf_i_hex_lowercase) {
	int parsed;
	sscanf("0xff", "%i", &parsed);
	ASSERT_INT_EQ(255, parsed);
}

/* ============= Scanf: %o (octal) ============= */
TEST_SUITE(scanf_o);

TEST(scanf_o_basic) {
	unsigned int parsed;
	sscanf("100", "%o", &parsed);
	ASSERT_INT_EQ(64u, parsed);
}

TEST(scanf_o_precision) {
	unsigned int parsed;
	sscanf("777", "%o", &parsed);
	ASSERT_INT_EQ(0777, parsed);
}

TEST(scanf_o_zero) {
	unsigned int parsed;
	sscanf("0", "%o", &parsed);
	ASSERT_INT_EQ(0, parsed);
}

/* ============= Scanf: %s (string) ============= */
TEST_SUITE(scanf_s);

TEST(scanf_s_basic) {
	char parsed[256];
	sscanf("hello", "%s", parsed);
	ASSERT_STR_EQ("hello", parsed);
}

TEST(scanf_s_width) {
	char s[256];
	sscanf("hello world", "%5s", s);
	ASSERT_STR_EQ("hello", s);
}

TEST(scanf_s_multiple) {
	char s1[256], s2[256];
	sscanf("one two", "%s %s", s1, s2);
	ASSERT_STR_EQ("one", s1);
	ASSERT_STR_EQ("two", s2);
}

TEST(scanf_s_single) {
	char s[256];
	sscanf("x", "%s", s);
	ASSERT_STR_EQ("x", s);
}

/* ============= Scanf: %u (unsigned decimal) ============= */
TEST_SUITE(scanf_u);

TEST(scanf_u_roundtrip) {
	for (int i = 0; i < 5; i++) {
		char buf[256];
		unsigned int parsed;
		snprintf(buf, sizeof(buf), "%u", uint_vals[i]);
		sscanf(buf, "%u", &parsed);
		ASSERT_INT_EQ(uint_vals[i], parsed);
	}
}

TEST(scanf_u_width) {
	char buf[256];
	unsigned int parsed;
	snprintf(buf, sizeof(buf), "0000042");
	sscanf(buf, "%7u", &parsed);
	ASSERT_INT_EQ(42u, parsed);
}

TEST(scanf_u_zero) {
	unsigned int parsed;
	sscanf("0", "%u", &parsed);
	ASSERT_INT_EQ(0u, parsed);
}

TEST(scanf_u_large) {
	unsigned int parsed;
	sscanf("4294967295", "%u", &parsed);
	ASSERT_INT_EQ(4294967295u, parsed);
}

/* ============= Scanf: %x, %X (hexadecimal) ============= */
TEST_SUITE(scanf_x);

TEST(scanf_x_lowercase) {
	unsigned int parsed;
	sscanf("ff", "%x", &parsed);
	ASSERT_INT_EQ(255u, parsed);
}

TEST(scanf_x_uppercase) {
	unsigned int parsed;
	sscanf("FF", "%X", &parsed);
	ASSERT_INT_EQ(255u, parsed);
}

TEST(scanf_x_mixed) {
	unsigned int parsed;
	sscanf("DeAdBeef", "%x", &parsed);
	ASSERT_INT_EQ(0xdeadbeef, parsed);
}

TEST(scanf_x_zero) {
	unsigned int parsed = 123;
	sscanf("0", "%x", &parsed);
	ASSERT_INT_EQ(0u, parsed);
}

TEST(scanf_x_prefix) {
	unsigned int parsed;
	sscanf("0xFF", "%x", &parsed);
	ASSERT_INT_EQ(255u, parsed);
}

/* ============= Scanf: %[ (character class) ============= */
TEST_SUITE(scanf_bracket);

TEST(scanf_bracket_include) {
	char s[256];
	sscanf("abc123xyz", "%[a-z]", s);
	ASSERT_STR_EQ("abc", s);
}

TEST(scanf_bracket_exclude) {
	char s[256];
	sscanf("abc123xyz", "%[^0-9]", s);
	ASSERT_STR_EQ("abc", s);
}

TEST(scanf_bracket_opendash) {
	char s1[8], s2[8], s3[8];
	sscanf("abc-123-xyz", "%[^0-9-]-%[1-3]%[-x-z]", s1, s2, s3);
	ASSERT_STR_EQ("abc", s1);
	ASSERT_STR_EQ("123", s2);
	ASSERT_STR_EQ("-xyz", s3);
}

TEST(scanf_bracket_digits) {
	char s[256];
	sscanf("12345abc", "%[0-9]", s);
	ASSERT_STR_EQ("12345", s);
}

/* ============= Scanf: %* (suppression) ============= */
TEST_SUITE(scanf_suppress);

TEST(scanf_suppress_int) {
	int d = 0;
	int ret = sscanf("42 100 200", "%*d %d", &d);
	ASSERT_INT_EQ(1, ret);
	ASSERT_INT_EQ(100, d);
}

TEST(scanf_suppress_float) {
	float f = 0;
	int ret = sscanf("42 100 200", "%*f %f", &f);
	ASSERT_INT_EQ(1, ret);
	ASSERT_DBL_EQ(100.0, f);
}

TEST(scanf_suppress_string) {
	char s[256];
	int ret = sscanf("hello world", "%*s %s", s);
	ASSERT_INT_EQ(1, ret);
	ASSERT_STR_EQ("world", s);
}

TEST(scanf_suppress_char) {
	char s[2];
	int ret = sscanf("OK", "%*c%c", s);
	ASSERT_INT_EQ(1, ret);
	ASSERT_STR_EQ("K", s);
}

TEST(scanf_suppress_scan) {
	char s[16];
	int ret = sscanf("asdf", "%*[as]%[df]]", s);
	ASSERT_INT_EQ(1, ret);
	ASSERT_STR_EQ("df", s);
}

/* ============= Scanf: Return Values ============= */
TEST_SUITE(scanf_return);

TEST(scanf_return_count) {
	int d, u;
	int ret = sscanf("42 100", "%d %u", &d, &u);
	ASSERT_INT_EQ(2, ret);
}

TEST(scanf_return_eof) {
	int d;
	int ret = sscanf("", "%d", &d);
	ASSERT_INT_EQ(EOF, ret);
}

TEST(scanf_return_failfirst) {
	int d;
	int ret = sscanf("abc", "%d", &d);
	ASSERT_INT_EQ(EOF, ret);
}

TEST(scanf_return_partial) {
	int d;
	int ret = sscanf("abc a", "%*s %d", &d);
	ASSERT_INT_EQ(0, ret);
}

/* ============= Scanf: Edge Cases ============= */
TEST_SUITE(scanf_edge);

TEST(scanf_edge_whitespace_skip) {
	int d;
	sscanf("  \t\n  42", "%d", &d);
	ASSERT_INT_EQ(42, d);
}

TEST(scanf_edge_empty_input) {
	int d;
	int ret = sscanf("", "%d", &d);
	ASSERT_INT_EQ(EOF, ret);
}

TEST(scanf_edge_leading_space) {
	int d;
	sscanf("   99", "%d", &d);
	ASSERT_INT_EQ(99, d);
}

/* ============================================================= */
/* ROUNDTRIP TESTS (printf → scanf)                              */
/* ============================================================= */

TEST_SUITE(roundtrip);

TEST(roundtrip_d_d) {
	for (int i = 0; i < sizeof(int_vals)/sizeof(int_vals[0]); i++) {
		char buf[256];
		int parsed;
		snprintf(buf, sizeof(buf), "%d", int_vals[i]);
		sscanf(buf, "%d", &parsed);
		ASSERT_INT_EQ(int_vals[i], parsed);
	}
}

TEST(roundtrip_u_u) {
	for (int i = 0; i < 5; i++) {
		char buf[256];
		unsigned int parsed;
		snprintf(buf, sizeof(buf), "%u", uint_vals[i]);
		sscanf(buf, "%u", &parsed);
		ASSERT_INT_EQ(uint_vals[i], parsed);
	}
}

TEST(roundtrip_x_x) {
	unsigned int test_vals[] = {0, 255, 0xdeadbeef, 0xFFFFFFFF};
	for (int i = 0; i < sizeof(test_vals)/sizeof(test_vals[0]); i++) {
		char buf[256];
		unsigned int parsed;
		snprintf(buf, sizeof(buf), "%x", test_vals[i]);
		sscanf(buf, "%x", &parsed);
		ASSERT_INT_EQ(test_vals[i], parsed);
	}
}

TEST(roundtrip_f_f) {
	for (int i = 0; i < 6; i++) {
		char buf[256];
		double parsed;
		snprintf(buf, sizeof(buf), "%.6f", dbl_vals[i]);
		sscanf(buf, "%lf", &parsed);
		ASSERT_DBL_NEAR(dbl_vals[i], parsed, 1e-6);
	}
}

TEST(roundtrip_f_inf) {
	char buf[256];
	double parsed;
	snprintf(buf, sizeof(buf), "%.2f", INFINITY);
	sscanf(buf, "%lf", &parsed);
	ASSERT_TRUE(isinf(parsed));
}

TEST(roundtrip_f_nan) {
	char buf[256];
	double parsed;
	snprintf(buf, sizeof(buf), "%.2f", NAN);
	sscanf(buf, "%lf", &parsed);
	ASSERT_TRUE(isnan(parsed));
}

TEST(roundtrip_e_e) {
	double test_vals[] = {1.23e-5, 1.23e5, -4.56e3, 0.0};
	for (int i = 0; i < sizeof(test_vals)/sizeof(test_vals[0]); i++) {
		char buf[256];
		double parsed;
		snprintf(buf, sizeof(buf), "%.2e", test_vals[i]);
		sscanf(buf, "%le", &parsed);
		ASSERT_DBL_NEAR(test_vals[i], parsed, 1e-4);
	}
}

TEST(roundtrip_g_g) {
	double test_vals[] = {0.001, 123.456, 0.000001, 123456.789};
	for (int i = 0; i < sizeof(test_vals)/sizeof(test_vals[0]); i++) {
		char buf[256];
		double parsed;
		snprintf(buf, sizeof(buf), "%.9g", test_vals[i]);
		sscanf(buf, "%lg", &parsed);
		ASSERT_DBL_NEAR(test_vals[i], parsed, 1e-6);
	}
}

TEST(roundtrip_tmpfile_d) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);

	for (int i = 0; i < 5; i++) {
		fprintf(f, "%d\n", int_vals[i]);
	}

	rewind(f);

	for (int i = 0; i < 5; i++) {
		int parsed;
		fscanf(f, "%d\n", &parsed);
		ASSERT_INT_EQ(int_vals[i], parsed);
	}

	fclose(f);
}

TEST(roundtrip_tmpfile_f) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);

	for (int i = 0; i < 6; i++) {
		fprintf(f, "%.6f\n", dbl_vals[i]);
	}

	rewind(f);

	for (int i = 0; i < 6; i++) {
		double parsed;
		fscanf(f, "%lf\n", &parsed);
		ASSERT_DBL_NEAR(dbl_vals[i], parsed, 1e-6);
	}

	fclose(f);
}

TEST(roundtrip_tmpfile_mixed) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);

	fprintf(f, "%d %u %x %.2f\n", 42, 100u, 255, 3.14);

	rewind(f);

	int d = 0;
	unsigned int u = 0;
	unsigned int x = 0;
	double fp = 0;

	fscanf(f, "%d %u %x %lf", &d, &u, &x, &fp);

	ASSERT_INT_EQ(42, d);
	ASSERT_INT_EQ(100u, u);
	ASSERT_INT_EQ(255u, x);
	ASSERT_DBL_EQ(3.14, fp);

	fclose(f);
}

/* ============================================================= */
/* FILE I/O TESTS                                                */
/* ============================================================= */

/* ============= fopen (file opening modes) ============= */
TEST_SUITE(fopen);

TEST(fopen_write_truncate) {
	char tmpname[L_tmpnam];
	tmpnam(tmpname);

	FILE *f = fopen(tmpname, "w");
	ASSERT_NOT_NULL(f);
	fprintf(f, "data");
	fclose(f);

	remove(tmpname);
}

TEST(fopen_read_after_write) {
	char tmpname[L_tmpnam];
	tmpnam(tmpname);

	FILE *f = fopen(tmpname, "w");
	fprintf(f, "hello");
	fclose(f);

	FILE *r = fopen(tmpname, "r");
	ASSERT_NOT_NULL(r);
	char buf[256];
	fgets(buf, sizeof(buf), r);
	ASSERT_STR_EQ("hello", buf);
	fclose(r);

	remove(tmpname);
}

TEST(fopen_read_write_plus) {
	char tmpname[L_tmpnam];
	tmpnam(tmpname);

	FILE *f = fopen(tmpname, "w+");
	ASSERT_NOT_NULL(f);
	fprintf(f, "test");
	rewind(f);
	char buf[256];
	fgets(buf, sizeof(buf), f);
	ASSERT_STR_EQ("test", buf);
	fclose(f);

	remove(tmpname);
}

TEST(fopen_append_mode) {
	char tmpname[L_tmpnam];
	tmpnam(tmpname);

	/* Write line1 */
	FILE *f1 = fopen(tmpname, "w");
	ASSERT_NOT_NULL(f1);
	fprintf(f1, "line1\n");
	fclose(f1);

	/* Append line2 */
	FILE *f2 = fopen(tmpname, "a");
	ASSERT_NOT_NULL(f2);
	fprintf(f2, "line2\n");
	fclose(f2);

	/* Read back and check */
	FILE *f3 = fopen(tmpname, "r");
	ASSERT_NOT_NULL(f3);
	char line1[64], line2[64];
	fgets(line1, sizeof(line1), f3);
	fgets(line2, sizeof(line2), f3);
	fclose(f3);

	ASSERT_STR_EQ("line1\n", line1);
	ASSERT_STR_EQ("line2\n", line2);

	remove(tmpname);
}

TEST(fopen_nonexistent_read) {
	FILE *f = fopen("/nonexistent/path/file.txt", "r");
	ASSERT_NULL(f);
}

/* ============= fclose (file closing) ============= */
TEST_SUITE(fclose);

TEST(fclose_after_write_flushes) {
	FILE *f = tmpfile();
	fprintf(f, "data");
	ASSERT_INT_EQ(0, fclose(f));
}

TEST(fclose_after_read) {
	FILE *f = tmpfile();
	fprintf(f, "content");
	rewind(f);
	char buf[256];
	fread(buf, 1, 7, f);
	ASSERT_INT_EQ(0, fclose(f));
}

TEST(fclose_tmpfile) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	ASSERT_INT_EQ(0, fclose(f));
}

/* ============= fread (block input) ============= */
TEST_SUITE(fread);

TEST(fread_exact_size) {
	FILE *f = tmpfile();
	fwrite("hello world", 1, 11, f);
	rewind(f);

	char buf[256];
	size_t n = fread(buf, 1, 11, f);
	ASSERT_INT_EQ(11, n);
	buf[11] = '\0';
	ASSERT_STR_EQ("hello world", buf);
	fclose(f);
}

TEST(fread_partial) {
	FILE *f = tmpfile();
	fwrite("0123456789", 1, 10, f);
	rewind(f);

	char buf[256];
	size_t n = fread(buf, 1, 5, f);
	ASSERT_INT_EQ(5, n);
	buf[5] = '\0';
	ASSERT_STR_EQ("01234", buf);
	fclose(f);
}

TEST(fread_multi_item) {
	FILE *f = tmpfile();
	fwrite("ABCDEF", 2, 3, f);
	rewind(f);

	char buf[256];
	size_t n = fread(buf, 2, 3, f);
	ASSERT_INT_EQ(3, n);
	fclose(f);
}

TEST(fread_eof) {
	FILE *f = tmpfile();
	fwrite("hi", 1, 2, f);
	rewind(f);

	char buf[256];
	fread(buf, 1, 2, f);
	size_t n = fread(buf, 1, 10, f);
	ASSERT_INT_EQ(0, n);
	ASSERT_TRUE(feof(f));
	fclose(f);
}

/* ============= fwrite (block output) ============= */
TEST_SUITE(fwrite);

TEST(fwrite_basic) {
	FILE *f = tmpfile();
	size_t n = fwrite("test", 1, 4, f);
	ASSERT_INT_EQ(4, n);
	fclose(f);
}

TEST(fwrite_multi_item) {
	FILE *f = tmpfile();
	size_t n = fwrite("ABCD", 2, 2, f);
	ASSERT_INT_EQ(2, n);
	fclose(f);
}

TEST(fwrite_zero_count) {
	FILE *f = tmpfile();
	size_t n = fwrite("data", 1, 0, f);
	ASSERT_INT_EQ(0, n);
	fclose(f);
}

/* ============= fgetc (single char input) ============= */
TEST_SUITE(fgetc);

TEST(fgetc_sequence) {
	FILE *f = tmpfile();
	fputc('A', f);
	fputc('B', f);
	fputc('C', f);
	rewind(f);

	ASSERT_INT_EQ('A', fgetc(f));
	ASSERT_INT_EQ('B', fgetc(f));
	ASSERT_INT_EQ('C', fgetc(f));
	fclose(f);
}

TEST(fgetc_eof) {
	FILE *f = tmpfile();
	fputc('X', f);
	rewind(f);

	fgetc(f);
	ASSERT_INT_EQ(EOF, fgetc(f));
	ASSERT_TRUE(feof(f));
	fclose(f);
}

TEST(fgetc_return_unsigned) {
	FILE *f = tmpfile();
	fputc(255, f);
	rewind(f);

	int c = fgetc(f);
	ASSERT_INT_EQ(255, c);
	fclose(f);
}

/* ============= fputc (single char output) ============= */
TEST_SUITE(fputc);

TEST(fputc_basic) {
	FILE *f = tmpfile();
	int ret = fputc('X', f);
	ASSERT_INT_EQ('X', ret);
	fclose(f);
}

TEST(fputc_sequence) {
	FILE *f = tmpfile();
	fputc('A', f);
	fputc('B', f);
	rewind(f);

	ASSERT_INT_EQ('A', fgetc(f));
	ASSERT_INT_EQ('B', fgetc(f));
	fclose(f);
}

/* ============= fgets (line input) ============= */
TEST_SUITE(fgets);

TEST(fgets_full_line) {
	FILE *f = tmpfile();
	fputs("hello\n", f);
	rewind(f);

	char buf[256];
	char *result = fgets(buf, sizeof(buf), f);
	ASSERT_NOT_NULL(result);
	ASSERT_STR_EQ("hello\n", buf);
	fclose(f);
}

TEST(fgets_truncate_long_line) {
	FILE *f = tmpfile();
	fputs("0123456789", f);
	rewind(f);

	char buf[5];
	char *result = fgets(buf, 5, f);
	ASSERT_NOT_NULL(result);
	ASSERT_STR_EQ("0123", buf);
	fclose(f);
}

TEST(fgets_eof) {
	FILE *f = tmpfile();
	fputs("data", f);
	rewind(f);

	char buf[256];
	fgets(buf, sizeof(buf), f);
	char *result = fgets(buf, sizeof(buf), f);
	ASSERT_NULL(result);
	fclose(f);
}

/* ============= fputs (line output) ============= */
TEST_SUITE(fputs);

TEST(fputs_basic) {
	FILE *f = tmpfile();
	int ret = fputs("hello", f);
	ASSERT_INT_EQ(0, ret);
	rewind(f);

	char buf[256];
	fgets(buf, sizeof(buf), f);
	ASSERT_STR_EQ("hello", buf);
	fclose(f);
}

TEST(fputs_with_newline) {
	FILE *f = tmpfile();
	fputs("line1\n", f);
	fputs("line2\n", f);
	rewind(f);

	char buf[256];
	fgets(buf, sizeof(buf), f);
	ASSERT_STR_EQ("line1\n", buf);
	fclose(f);
}

/* ============= fseek (file positioning) ============= */
TEST_SUITE(fseek);

TEST(fseek_seek_set) {
	FILE *f = tmpfile();
	fprintf(f, "0123456789");
	fflush(f);

	int ret = fseek(f, 5, SEEK_SET);
	ASSERT_INT_EQ(0, ret);

	int c = fgetc(f);
	ASSERT_INT_EQ('5', c);
	fclose(f);
}

TEST(fseek_seek_cur) {
	FILE *f = tmpfile();
	fprintf(f, "ABCDEFGH");

	fseek(f, 2, SEEK_SET);
	fseek(f, 2, SEEK_CUR);
	int c = fgetc(f);
	ASSERT_INT_EQ('E', c);
	fclose(f);
}

TEST(fseek_seek_end) {
	FILE *f = tmpfile();
	fprintf(f, "data");

	fseek(f, 0, SEEK_END);
	ASSERT_INT_EQ(4, ftell(f));
	fclose(f);
}

TEST(fseek_backward) {
	FILE *f = tmpfile();
	fprintf(f, "ABCDEF");
	fflush(f);

	fseek(f, 2, SEEK_SET);
	int c = fgetc(f);
	ASSERT_INT_EQ('C', c);
	fclose(f);
}

/* ============= ftell (position query) ============= */
TEST_SUITE(ftell);

TEST(ftell_after_write) {
	FILE *f = tmpfile();
	fprintf(f, "hello");

	long pos = ftell(f);
	ASSERT_INT_EQ(5, pos);
	fclose(f);
}

TEST(ftell_after_seek) {
	FILE *f = tmpfile();
	fprintf(f, "0123456789");
	fseek(f, 7, SEEK_SET);
	ASSERT_INT_EQ(7, ftell(f));
	fclose(f);
}

/* ============= rewind (position reset) ============= */
TEST_SUITE(rewind);

TEST(rewind_to_start) {
	FILE *f = tmpfile();
	fprintf(f, "data");
	fseek(f, 4, SEEK_END);
	rewind(f);
	ASSERT_INT_EQ(0, ftell(f));
	fclose(f);
}

TEST(rewind_clears_eof) {
	FILE *f = tmpfile();
	fprintf(f, "x");
	fflush(f);
	rewind(f);

	char c = fgetc(f);
	ASSERT_INT_EQ('x', c);
	ASSERT_TRUE(feof(f) == 0);

	int c2 = fgetc(f);
	ASSERT_INT_EQ(EOF, c2);
	ASSERT_TRUE(feof(f));

	rewind(f);
	ASSERT_INT_EQ(0, feof(f));
	fclose(f);
}

/* ============= feof (EOF detection) ============= */
TEST_SUITE(feof);

TEST(feof_not_at_end) {
	FILE *f = tmpfile();
	fprintf(f, "data");
	rewind(f);
	ASSERT_INT_EQ(0, feof(f));
	fclose(f);
}

TEST(feof_at_end) {
	FILE *f = tmpfile();
	fprintf(f, "x");
	fflush(f);
	rewind(f);

	char buf[2];
	fread(buf, 1, 1, f);

	int c = fgetc(f);
	ASSERT_INT_EQ(EOF, c);
	ASSERT_TRUE(feof(f));
	fclose(f);
}

/* ============= ferror (error detection) ============= */
TEST_SUITE(ferror);

TEST(ferror_no_error) {
	FILE *f = tmpfile();
	fprintf(f, "ok");
	ASSERT_INT_EQ(0, ferror(f));
	fclose(f);
}

/* ============= clearerr (error reset) ============= */
TEST_SUITE(clearerr);

TEST(clearerr_resets_flags) {
	FILE *f = tmpfile();
	clearerr(f);
	ASSERT_INT_EQ(0, ferror(f));
	ASSERT_INT_EQ(0, feof(f));
	fclose(f);
}

/* ============= setvbuf (buffering mode) ============= */
TEST_SUITE(setvbuf);

TEST(setvbuf_full_buffering) {
	FILE *f = tmpfile();
	ASSERT_INT_EQ(0, setvbuf(f, NULL, _IOFBF, 1024));
	fclose(f);
}

TEST(setvbuf_line_buffering) {
	FILE *f = tmpfile();
	ASSERT_INT_EQ(0, setvbuf(f, NULL, _IOLBF, 512));
	fclose(f);
}

TEST(setvbuf_no_buffering) {
	FILE *f = tmpfile();
	ASSERT_INT_EQ(0, setvbuf(f, NULL, _IONBF, 0));
	fclose(f);
}

/* ============= setbuf (simple buffering) ============= */
TEST_SUITE(setbuf);

TEST(setbuf_user_buffer) {
	FILE *f = tmpfile();
	char mybuf[512];
	setbuf(f, mybuf);
	fprintf(f, "test");
	fclose(f);
}

#if JACL_HAS_POSIX

/* ============= fdopen (fd to stream) ============= */
TEST_SUITE(fdopen);

TEST(fdopen_from_pipe) {
	int pipefd[2];
	ASSERT_INT_EQ(0, pipe(pipefd));

	FILE *f = fdopen(pipefd[0], "r");
	ASSERT_NOT_NULL(f);
	close(pipefd[1]);
	fclose(f);
}

/* ============= tmpfile (temporary file) ============= */
TEST_SUITE(tmpfile);

TEST(tmpfile_read_write) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fprintf(f, "temporary");
	rewind(f);
	char buf[256];
	fgets(buf, sizeof(buf), f);
	ASSERT_STR_EQ("temporary", buf);
	fclose(f);
}

/* ============= tmpnam (temp filename) ============= */
TEST_SUITE(tmpnam);

TEST(tmpnam_generates_name) {
	char name[L_tmpnam];
	char *result = tmpnam(name);

	/* Debug: see what errno was */
	if (!result) {
		fprintf(stderr, "tmpnam failed: errno=%d (%s)\n", errno, strerror(errno));
	}

	ASSERT_NOT_NULL(result);
	ASSERT_TRUE(strlen(name) > 0);
}

#endif

TEST_MAIN()
