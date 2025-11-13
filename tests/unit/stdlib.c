/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <stdlib.h>

TEST_TYPE(unit)
TEST_UNIT(stdlib.h)

/* ============================================================= */
/* atoi - stdlib.h: unit tests                                   */
/* ============================================================= */
TEST_SUITE(atoi)

TEST(atoi_positive)
{
	ASSERT_INT_EQ(atoi("123"), 123);
}

TEST(atoi_negative)
{
	ASSERT_INT_EQ(atoi("-456"), -456);
}

TEST(atoi_zero)
{
	ASSERT_INT_EQ(atoi("0"), 0);
}

TEST(atoi_whitespace)
{
	ASSERT_INT_EQ(atoi("  789"), 789);
}

TEST(atoi_partial)
{
	ASSERT_INT_EQ(atoi("123abc"), 123);
}

TEST(atoi_invalid)
{
	ASSERT_INT_EQ(atoi("abc"), 0);
}

TEST(atoi_sign_plus)
{
	ASSERT_INT_EQ(atoi("+42"), 42);
}

TEST(atoi_max)
{
	ASSERT_INT_EQ(atoi("2147483647"), INT_MAX);
}

TEST(atoi_many_leading_zeros)
{
	ASSERT_INT_EQ(atoi("000000000000000000000000000000001"), 1);
}

TEST(atoi_plus_minus_mixed)
{
	ASSERT_INT_EQ(atoi("+-123"), 0);
}

TEST(atoi_space_after_sign)
{
	ASSERT_INT_EQ(atoi("+ 123"), 0);
}

TEST(atoi_tabs_before)
{
	ASSERT_INT_EQ(atoi("\t\t\t42"), 42);
}

TEST(atoi_newline_before)
{
	ASSERT_INT_EQ(atoi("\n99"), 99);
}

TEST(atoi_vertical_tab_form_feed)
{
	ASSERT_INT_EQ(atoi("\v\f50"), 50);
}

TEST(atoi_int_min_underflow)
{
	int result = atoi("-2147483649");
}

/* ============================================================= */
/* atol - stdlib.h: unit tests                                   */
/* ============================================================= */
TEST_SUITE(atol)

TEST(atol_basic)
{
	ASSERT_INT_EQ(atol("1234"), 1234L);
}

TEST(atol_negative)
{
	ASSERT_INT_EQ(atol("-5678"), -5678L);
}

TEST(atol_large)
{
	ASSERT_INT_EQ(atol("9223372036854775807"), LLONG_MAX);
}

TEST(atol_whitespace_tabs)
{
	ASSERT_INT_EQ(atol("\t\t999"), 999L);
}

/* ============================================================= */
/* atoll - stdlib.h: unit tests                                  */
/* ============================================================= */
TEST_SUITE(atoll)

TEST(atoll_basic)
{
	ASSERT_INT_EQ(atoll("123456"), 123456LL);
}

TEST(atoll_large)
{
	ASSERT_INT_EQ(atoll("9223372036854775807"), LLONG_MAX);
}

TEST(atoll_negative)
{
	ASSERT_INT_EQ(atoll("-987654"), -987654LL);
}

/* ============================================================= */
/* atof - stdlib.h: unit tests                                   */
/* ============================================================= */
TEST_SUITE(atof)

TEST(atof_integer)
{
	ASSERT_DBL_EQ(atof("123"), 123.0);
}

TEST(atof_decimal)
{
	ASSERT_DBL_EQ(atof("3.14"), 3.14);
}

TEST(atof_scientific_upper)
{
	ASSERT_DBL_EQ(atof("1.5E2"), 150.0);
}

TEST(atof_scientific_lower)
{
	ASSERT_DBL_EQ(atof("2.5e-1"), 0.25);
}

TEST(atof_negative)
{
	ASSERT_DBL_EQ(atof("-2.71"), -2.71);
}

TEST(atof_invalid)
{
	ASSERT_DBL_EQ(atof("xyz"), 0.0);
}

/* ============================================================= */
/* strtol - stdlib.h: unit tests                                 */
/* ============================================================= */
TEST_SUITE(strtol)

TEST(strtol_base10_endptr)
{
	char *endptr;
	long result = strtol("123abc", &endptr, 10);
	ASSERT_INT_EQ(result, 123);
	ASSERT_INT_EQ(*endptr, 'a');
}

TEST(strtol_base10_nullptr)
{
	ASSERT_INT_EQ(strtol("456", NULL, 10), 456);
}

TEST(strtol_base16)
{
	ASSERT_INT_EQ(strtol("FF", NULL, 16), 255);
}

TEST(strtol_base16_lowercase)
{
	ASSERT_INT_EQ(strtol("ff", NULL, 16), 255);
}

TEST(strtol_base2)
{
	ASSERT_INT_EQ(strtol("1010", NULL, 2), 10);
}

TEST(strtol_base8)
{
	ASSERT_INT_EQ(strtol("777", NULL, 8), 511);
}

TEST(strtol_auto_base_hex)
{
	ASSERT_INT_EQ(strtol("0xFF", NULL, 0), 255);
}

TEST(strtol_auto_base_octal)
{
	ASSERT_INT_EQ(strtol("0777", NULL, 0), 511);
}

TEST(strtol_auto_base_decimal)
{
	ASSERT_INT_EQ(strtol("999", NULL, 0), 999);
}

TEST(strtol_negative)
{
	ASSERT_INT_EQ(strtol("-100", NULL, 10), -100);
}

TEST(strtol_whitespace)
{
	ASSERT_INT_EQ(strtol("  42", NULL, 10), 42);
}

TEST(strtol_base_1_invalid)
{
	long result = strtol("123", NULL, 1);
	ASSERT_ERRNO_EQ(EINVAL);
	ASSERT_INT_EQ(result, 0);
}

TEST(strtol_base_37_invalid)
{
	long result = strtol("XYZ", NULL, 37);
	ASSERT_ERRNO_EQ(EINVAL);
	ASSERT_INT_EQ(result, 0);
}

TEST(strtol_overflow_positive)
{
	long result = strtol("99999999999999999999999", NULL, 10);
	ASSERT_ERRNO_EQ(ERANGE);
	ASSERT_INT_EQ(result, LONG_MAX);
}

TEST(strtol_underflow_negative)
{
	long result = strtol("-99999999999999999999999", NULL, 10);
	ASSERT_ERRNO_EQ(ERANGE);
	ASSERT_INT_EQ(result, LONG_MIN);
}

TEST(strtol_no_conversion)
{
	char *ep;
	long result = strtol("notanumber", &ep, 10);
	ASSERT_ERRNO_EQ(0);
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

/* ============================================================= */
/* strtoll - stdlib.h: unit tests                                */
/* ============================================================= */
TEST_SUITE(strtoll)

TEST(strtoll_basic)
{
	ASSERT_INT_EQ(strtoll("555", NULL, 10), 555LL);
}

TEST(strtoll_hex)
{
	ASSERT_INT_EQ(strtoll("DEADBEEF", NULL, 16), 0xDEADBEEFLL);
}

TEST(strtoll_overflow)
{
	long long result = strtoll("9999999999999999999999999999", NULL, 10);
	ASSERT_ERRNO_EQ(ERANGE);
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

TEST(strtoll_overflow2) {
	char src[64];
	sprintf(src, "%" PRIuMAX, UINTMAX_MAX);
	char *end = NULL;
	errno = 0;
	long long v = strtoll(src, &end, 10);
	ASSERT_EQ(INTMAX_MAX, v);    // Value should be saturated at INTMAX_MAX
	ASSERT_EQ(ERANGE, errno);    // Errno should indicate overflow
}



/* ============================================================= */
/* strtoul - stdlib.h: unit tests                                */
/* ============================================================= */
TEST_SUITE(strtoul)

TEST(strtoul_basic)
{
	ASSERT_INT_EQ(strtoul("1000", NULL, 10), 1000UL);
}

TEST(strtoul_hex)
{
	ASSERT_INT_EQ(strtoul("ABCD", NULL, 16), 0xABCDUL);
}

TEST(strtoul_hex_prefix)
{
	ASSERT_INT_EQ(strtoul("0xCAFE", NULL, 16), 0xCAFEUL);
}

TEST(strtoul_overflow)
{
	unsigned long result = strtoul("99999999999999999999", NULL, 10);
	ASSERT_ERRNO_EQ(ERANGE);
	ASSERT_INT_EQ(result, ULONG_MAX);
}

/* ============================================================= */
/* strtoull - stdlib.h: unit tests                               */
/* ============================================================= */
TEST_SUITE(strtoull)

TEST(strtoull_basic)
{
	ASSERT_INT_EQ(strtoull("777", NULL, 10), 777ULL);
}

TEST(strtoull_hex)
{
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


/* ============================================================= */
/* strtod - stdlib.h: unit tests                                 */
/* ============================================================= */
TEST_SUITE(strtod)

TEST(strtod_basic)
{
	char *endptr;
	double result = strtod("3.14159", &endptr);
	ASSERT_DBL_NEAR(3.14159, result, 1e-6);
	ASSERT_INT_EQ(*endptr, '\0');
}

TEST(strtod_scientific)
{
	double result = strtod("1.23e-4", NULL);
	ASSERT_DBL_NEAR(0.000123, result, 1e-8);
}

TEST(strtod_negative_zero)
{
	double result = strtod("-0.0", NULL);
	ASSERT_DBL_EQ(result, 0.0);
	ASSERT_INT_EQ(signbit(result), 1);
}

TEST(strtod_endptr_advances)
{
	char *ep;
	strtod("42.5xyz", &ep);
	ASSERT_INT_EQ(*ep, 'x');
}

TEST(strtod_similar_special_values)
{
	double result = strtod("NAN", NULL);
	ASSERT_INT_EQ(isnan(result), 1);
}

TEST(strtod_endptr_after_e_notation)
{
	char *ep;
	strtod("1.5e10end", &ep);
	ASSERT_INT_EQ(*ep, 'e');
}

/* ============================================================= */
/* strtof - stdlib.h: unit tests                                 */
/* ============================================================= */
TEST_SUITE(strtof)

TEST(strtof_basic)
{
	double result = (double)strtof("3.14", NULL);
	ASSERT_DBL_NEAR(3.14, result, 0.0001);
}

TEST(strtof_scientific)
{
	double result = (double)strtof("1.23e2", NULL);
	ASSERT_DBL_NEAR(123.0, result, 0.01);
}

TEST(strtof_positive_infinity_CAPS)
{
	float result = strtof("INF", NULL);
	ASSERT_INT_EQ(isinf(result), 1);
	ASSERT_INT_EQ(signbit(result), 0);
}

TEST(strtof_positive_infinity_lower)
{
	float result = strtof("inf", NULL);
	ASSERT_INT_EQ(isinf(result), 1);
}

TEST(strtof_positive_infinity_FULL)
{
	float result = strtof("INFINITY", NULL);
	ASSERT_INT_EQ(isinf(result), 1);
}

TEST(strtof_negative_infinity)
{
	float result = strtof("-INF", NULL);
	ASSERT_INT_EQ(isinf(result), 1);
	ASSERT_INT_EQ(signbit(result), 1);
}

TEST(strtof_nan_CAPS)
{
	float result = strtof("NAN", NULL);
	ASSERT_INT_EQ(isnan(result), 1);
}

TEST(strtof_nan_lower)
{
	float result = strtof("nan", NULL);
	ASSERT_INT_EQ(isnan(result), 1);
}

TEST(strtof_nan_with_parens)
{
	float result = strtof("NAN(0x1234)", NULL);
	ASSERT_INT_EQ(isnan(result), 1);
}

TEST(strtof_negative_zero)
{
	float result = strtof("-0.0", NULL);
	ASSERT_INT_EQ(result, 0.0);
	ASSERT_INT_EQ(signbit(result), 1);
}

TEST(strtof_subnormal_small)
{
	float result = strtof("1e-50", NULL);
}

TEST(strtof_overflow_to_inf)
{
	float result = strtof("1e100", NULL);
	ASSERT_ERRNO_EQ(ERANGE);
	ASSERT_INT_EQ(isinf(result), 1);
}

TEST(strtof_underflow_to_zero)
{
	float result = strtof("1e-1000", NULL);
	ASSERT_ERRNO_EQ(ERANGE);
	ASSERT_INT_EQ(result, 0.0);
}

TEST(strtof_hex_float_0x_prefix)
{
	float result = strtof("0x1.0p0", NULL);
	ASSERT_DBL_NEAR(result, 1.0, 0.0001);
}

TEST(strtof_hex_float_exponent)
{
	float result = strtof("0x1.0p+4", NULL);
	ASSERT_DBL_NEAR(result, 16.0, 0.0001);
}

TEST(strtof_endptr_stops_at_invalid_char)
{
	char *ep;
	strtof("3.14@@@", &ep);
	ASSERT_INT_EQ(*ep, '@');
}

/* ============================================================= */
/* strtold - stdlib.h: unit tests                                */
/* ============================================================= */
TEST_SUITE(strtold)

TEST(strtold_basic)
{
	double result = (double)strtold("2.718", NULL);
	ASSERT_DBL_NEAR(2.718, result, 0.0001);
}

TEST(strtold_negative)
{
	double result = (double)strtold("-1.414", NULL);
	ASSERT_DBL_NEAR(-1.414, result, 0.0001);
}

TEST(strtold_similar_special_values)
{
	long double result = strtold("INF", NULL);
	ASSERT_INT_EQ(isinf(result), 1);
}

/* ============================================================= */
/* abs - stdlib.h: unit tests                                    */
/* ============================================================= */
TEST_SUITE(abs)

TEST(abs_positive)
{
	ASSERT_INT_EQ(abs(42), 42);
}

TEST(abs_negative)
{
	ASSERT_INT_EQ(abs(-42), 42);
}

TEST(abs_zero)
{
	ASSERT_INT_EQ(abs(0), 0);
}

TEST(abs_min)
{
	ASSERT_INT_EQ(abs(INT_MIN + 1), INT_MAX);
}

TEST(abs_int_min_overflow)
{
	int result = abs(INT_MIN);
}

/* ============================================================= */
/* labs - stdlib.h: unit tests                                   */
/* ============================================================= */
TEST_SUITE(labs)

TEST(labs_positive)
{
	ASSERT_INT_EQ(labs(100L), 100L);
}

TEST(labs_negative)
{
	ASSERT_INT_EQ(labs(-100L), 100L);
}

TEST(labs_long_min)
{
	long result = labs(LONG_MIN);
}

/* ============================================================= */
/* llabs - stdlib.h: unit tests                                  */
/* ============================================================= */
TEST_SUITE(llabs)

TEST(llabs_positive)
{
	ASSERT_INT_EQ(llabs(1000LL), 1000LL);
}

TEST(llabs_negative)
{
	ASSERT_INT_EQ(llabs(-1000LL), 1000LL);
}

TEST(llabs_long_long_min)
{
	long long result = llabs(LLONG_MIN);
}

/* ============================================================= */
/* div - stdlib.h: unit tests                                    */
/* ============================================================= */
TEST_SUITE(div)

TEST(div_basic)
{
	div_t r = div(10, 3);
	ASSERT_INT_EQ(r.quot, 3);
	ASSERT_INT_EQ(r.rem, 1);
}

TEST(div_negative_dividend)
{
	div_t r = div(-10, 3);
	ASSERT_INT_EQ(r.quot, -3);
	ASSERT_INT_EQ(r.rem, -1);
}

TEST(div_negative_divisor)
{
	div_t r = div(10, -3);
	ASSERT_INT_EQ(r.quot, -3);
	ASSERT_INT_EQ(r.rem, 1);
}

TEST(div_zero_remainder)
{
	div_t r = div(12, 4);
	ASSERT_INT_EQ(r.quot, 3);
	ASSERT_INT_EQ(r.rem, 0);
}

TEST(div_both_negative)
{
	div_t r = div(-10, -3);
	ASSERT_INT_EQ(r.quot, 3);
	ASSERT_INT_EQ(r.rem, -1);
}

TEST(div_zero_dividend)
{
	div_t r = div(0, 5);
	ASSERT_INT_EQ(r.quot, 0);
	ASSERT_INT_EQ(r.rem, 0);
}

TEST(div_int_min_by_minus_one)
{
	div_t r = div(INT_MIN, -1);
}

/* ============================================================= */
/* ldiv - stdlib.h: unit tests                                   */
/* ============================================================= */
TEST_SUITE(ldiv)

TEST(ldiv_basic)
{
	ldiv_t r = ldiv(100L, 7L);
	ASSERT_INT_EQ(r.quot, 14L);
	ASSERT_INT_EQ(r.rem, 2L);
}

TEST(ldiv_negative)
{
	ldiv_t r = ldiv(-100L, 7L);
	ASSERT_INT_EQ(r.quot, -14L);
	ASSERT_INT_EQ(r.rem, -2L);
}

/* ============================================================= */
/* lldiv - stdlib.h: unit tests                                  */
/* ============================================================= */
TEST_SUITE(lldiv)

TEST(lldiv_basic)
{
	lldiv_t r = lldiv(1000LL, 99LL);
	ASSERT_INT_EQ(r.quot, 10LL);
	ASSERT_INT_EQ(r.rem, 10LL);
}

TEST(lldiv_negative)
{
	lldiv_t r = lldiv(-1000LL, 99LL);
	ASSERT_INT_EQ(r.quot, -10LL);
	ASSERT_INT_EQ(r.rem, -10LL);
}

TEST(lldiv_large_numbers)
{
	lldiv_t r = lldiv(9223372036854775800LL, 1000000LL);
	ASSERT_INT_EQ(r.quot, 9223372036854LL);
	ASSERT_INT_EQ(r.rem, 775800LL);
}

/* ============================================================= */
/* qsort - stdlib.h: unit tests                                  */
/* ============================================================= */
TEST_SUITE(qsort)

static int cmp_int(const void *a, const void *b)
{
	return *(int *)a - *(int *)b;
}

static int cmp_dbl(const void *a, const void *b)
{
	double diff = *(double *)a - *(double *)b;
	return (diff < 0) ? -1 : (diff > 0) ? 1 : 0;
}

TEST(qsort_basic)
{
	int arr[] = {3, 1, 4, 1, 5};
	qsort(arr, 5, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 1);
	ASSERT_INT_EQ(arr[4], 5);
}

TEST(qsort_already_sorted)
{
	int arr[] = {1, 2, 3, 4, 5};
	qsort(arr, 5, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 1);
	ASSERT_INT_EQ(arr[4], 5);
}

TEST(qsort_reverse)
{
	int arr[] = {5, 4, 3, 2, 1};
	qsort(arr, 5, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 1);
	ASSERT_INT_EQ(arr[4], 5);
}

TEST(qsort_single)
{
	int arr[] = {42};
	qsort(arr, 1, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 42);
}

TEST(qsort_empty)
{
	int arr[] = {};
	qsort(arr, 0, sizeof(int), cmp_int);
}

TEST(qsort_duplicates)
{
	int arr[] = {3, 1, 3, 2, 1};
	qsort(arr, 5, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 1);
}

TEST(qsort_two_elements)
{
	int arr[] = {2, 1};
	qsort(arr, 2, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 1);
	ASSERT_INT_EQ(arr[1], 2);
}

TEST(qsort_floats)
{
	double arr[] = {3.14, 1.41, 2.71};
	qsort(arr, 3, sizeof(double), cmp_dbl);
	ASSERT_DBL_EQ(arr[0], 1.41);
}

TEST(qsort_all_same_elements)
{
	int arr[] = {7, 7, 7, 7, 7, 7, 7};
	qsort(arr, 7, sizeof(int), cmp_int);
	for (int i = 0; i < 7; i++) ASSERT_INT_EQ(arr[i], 7);
}

TEST(qsort_reverse_order_large)
{
	int arr[1000];
	for (int i = 0; i < 1000; i++) arr[i] = 999 - i;
	qsort(arr, 1000, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 0);
	ASSERT_INT_EQ(arr[999], 999);
}

TEST(qsort_random_pattern)
{
	int arr[] = {34, 12, 89, 1, 56, 23, 90, 5, 67, 45};
	qsort(arr, 10, sizeof(int), cmp_int);
	for (int i = 1; i < 10; i++) {
		ASSERT_INT_GE(arr[i], arr[i-1]);
	}
}

TEST(qsort_negative_numbers)
{
	int arr[] = {-50, 100, -1, 0, 50, -100, 1};
	qsort(arr, 7, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], -100);
	ASSERT_INT_EQ(arr[6], 100);
}

TEST(qsort_alternating_sign)
{
	int arr[] = {-1, 1, -2, 2, -3, 3};
	qsort(arr, 6, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], -3);
	ASSERT_INT_EQ(arr[5], 3);
}

TEST(qsort_duplicate_heavy)
{
	int arr[] = {5, 5, 5, 1, 1, 9, 9, 9, 9, 3};
	qsort(arr, 10, sizeof(int), cmp_int);
	ASSERT_INT_EQ(arr[0], 1);
	ASSERT_INT_EQ(arr[9], 9);
}

/* ============================================================= */
/* bsearch - stdlib.h: unit tests                                */
/* ============================================================= */
TEST_SUITE(bsearch)

static int cmp_int_bs(const void *a, const void *b)
{
	return *(int *)a - *(int *)b;
}

TEST(bsearch_found_middle)
{
	int arr[] = {1, 3, 5, 7, 9};
	int key = 5;
	int *result = (int *)bsearch(&key, arr, 5, sizeof(int), cmp_int_bs);
	ASSERT_NOT_NULL(result);
	ASSERT_INT_EQ(*result, 5);
}

TEST(bsearch_not_found)
{
	int arr[] = {1, 3, 5, 7, 9};
	int key = 4;
	ASSERT_NULL(bsearch(&key, arr, 5, sizeof(int), cmp_int_bs));
}

TEST(bsearch_found_first)
{
	int arr[] = {1, 3, 5, 7, 9};
	int key = 1;
	ASSERT_NOT_NULL(bsearch(&key, arr, 5, sizeof(int), cmp_int_bs));
}

TEST(bsearch_found_last)
{
	int arr[] = {1, 3, 5, 7, 9};
	int key = 9;
	ASSERT_NOT_NULL(bsearch(&key, arr, 5, sizeof(int), cmp_int_bs));
}

TEST(bsearch_single_element)
{
	int arr[] = {42};
	int key = 42;
	ASSERT_NOT_NULL(bsearch(&key, arr, 1, sizeof(int), cmp_int_bs));
}

TEST(bsearch_single_not_found)
{
	int arr[] = {42};
	int key = 99;
	ASSERT_NULL(bsearch(&key, arr, 1, sizeof(int), cmp_int_bs));
}

TEST(bsearch_unsorted_undefined)
{
	int arr[] = {5, 1, 9, 3, 7};
	int key = 5;
	int *result = (int *)bsearch(&key, arr, 5, sizeof(int), cmp_int_bs);
}

TEST(bsearch_large_sorted_array)
{
	int arr[1000];
	for (int i = 0; i < 1000; i++) arr[i] = i * 2;
	int key = 500;
	int *result = (int *)bsearch(&key, arr, 1000, sizeof(int), cmp_int_bs);
	ASSERT_NOT_NULL(result);
	ASSERT_INT_EQ(*result, 500);
}

TEST(bsearch_boundary_values)
{
	int arr[] = {INT_MIN, 0, INT_MAX};
	int key = INT_MAX;
	int *result = (int *)bsearch(&key, arr, 3, sizeof(int), cmp_int_bs);
	ASSERT_NOT_NULL(result);
}

TEST(bsearch_duplicates_any_valid)
{
	int arr[] = {1, 3, 5, 5, 5, 7, 9};
	int key = 5;
	int *result = (int *)bsearch(&key, arr, 7, sizeof(int), cmp_int_bs);
	ASSERT_NOT_NULL(result);
	ASSERT_INT_EQ(*result, 5);
}

/* ============================================================= */
/* rand/srand - stdlib.h: unit tests                             */
/* ============================================================= */
TEST_SUITE(rand)

TEST(rand_srand_basic)
{
	srand(42);
	int val1 = rand();
	srand(42);
	int val2 = rand();
	ASSERT_INT_EQ(val1, val2);
}

TEST(rand_range)
{
	int val = rand() % 100;
	ASSERT_INT_GE(val, 0);
	ASSERT_INT_LT(val, 100);
}

TEST(rand_all_values_in_range)
{
	srand(12345);
	for (int i = 0; i < 10000; i++) {
		int val = rand();
		ASSERT_INT_GE(val, 0);
		ASSERT_INT_LE(val, RAND_MAX);
	}
}

TEST(rand_sequence_reproducible)
{
	srand(999);
	int seq1[100];
	for (int i = 0; i < 100; i++) seq1[i] = rand();

	srand(999);
	for (int i = 0; i < 100; i++) {
		int val = rand();
		ASSERT_INT_EQ(val, seq1[i]);
	}
}

TEST(rand_different_seeds_diverge)
{
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

TEST(rand_seed_0_vs_1)
{
	srand(0);
	int val0 = rand();
	srand(1);
	int val1 = rand();
}

/* ============================================================= */
/* mkstemp - stdlib.h: unit tests                                */
/* ============================================================= */
TEST_SUITE(mkstemp)

TEST(mkstemp_creates_file)
{
	char template[] = "/tmp/testXXXXXX";
	int fd = mkstemp(template);
	ASSERT_INT_NE(fd, -1);
	close(fd);
	unlink(template);
}

TEST(mkstemp_unique_names)
{
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

TEST(mkstemp_invalid_short_template)
{
	char template[] = "/tmp/tXX";
	ASSERT_INT_EQ(mkstemp(template), -1);
}

TEST(mkstemp_invalid_no_x_suffix)
{
	char template[] = "/tmp/testfile";
	ASSERT_INT_EQ(mkstemp(template), -1);
}

TEST(mkstemp_file_readable_writable)
{
	char template[] = "/tmp/permXXXXXX";
	int fd = mkstemp(template);
	ASSERT_INT_NE(fd, -1);
	ssize_t written = write(fd, "test", 4);
	ASSERT_INT_EQ(written, 4);
	close(fd);
	unlink(template);
}

TEST(mkstemp_relative_path)
{
	char t[] = "testXXXXXX";
	int fd = mkstemp(t);
	if (fd >= 0) {
		close(fd);
		unlink(t);
		ASSERT_INT_NE(fd, -1);
	}
}

TEST(mkstemp_many_x_suffix)
{
	char t[] = "/tmp/prefixXXXXXXXXXXsuffix";
	int fd = mkstemp(t);
	if (fd >= 0) {
		close(fd);
		unlink(t);
	}
}

TEST(mkstemp_template_modified)
{
	char orig[] = "/tmp/aaaXXXXXX";
	char t[] = "/tmp/aaaXXXXXX";
	int fd = mkstemp(t);
	ASSERT_INT_NE(fd, -1);
	ASSERT_STR_NE(t, orig);
	close(fd);
	unlink(t);
}

TEST(mkstemp_replaces_all_x)
{
	char t[] = "/tmp/XXXXXX";
	int fd = mkstemp(t);
	ASSERT_INT_NE(fd, -1);
	for (int i = 0; i < strlen(t); i++) {
		ASSERT_INT_NE(t[i], 'X');
	}
	close(fd);
	unlink(t);
}

TEST(mkstemp_exact_six_x)
{
	char t[] = "/tmp/XXXXXX";
	int fd = mkstemp(t);
	ASSERT_INT_NE(fd, -1);
	close(fd);
	unlink(t);
}

/* ============================================================= */
/* strdup - stdlib.h: unit tests                                 */
/* ============================================================= */
TEST_SUITE(strdup)

TEST(strdup_basic)
{
	char *orig = "hello";
	char *dup = strdup(orig);
	ASSERT_STR_EQ(dup, orig);
	ASSERT_INT_NE((long)dup, (long)orig);
	free(dup);
}

TEST(strdup_empty)
{
	char *dup = strdup("");
	ASSERT_STR_EQ(dup, "");
	free(dup);
}

TEST(strdup_long_string)
{
	char *orig = "The quick brown fox jumps over the lazy dog";
	char *dup = strdup(orig);
	ASSERT_STR_EQ(dup, orig);
	free(dup);
}

TEST(strdup_embedded_null)
{
	char src[] = "hello\0world";
	char *dup = strdup(src);
	ASSERT_STR_EQ(dup, src);
	free(dup);
}

TEST(strdup_very_long_string)
{
	char *src = (char *)malloc(10000);
	memset(src, 'X', 9999);
	src[9999] = '\0';

	char *dup = strdup(src);
	ASSERT_STR_EQ(dup, src);

	free(dup);
	free(src);
}

/* ============================================================= */
/* strndup - stdlib.h: unit tests                                */
/* ============================================================= */
TEST_SUITE(strndup)

TEST(strndup_basic)
{
	char *dup = strndup("hello world", 5);
	ASSERT_STR_EQ(dup, "hello");
	free(dup);
}

TEST(strndup_truncate)
{
	char *dup = strndup("truncate", 4);
	ASSERT_STR_EQ(dup, "trun");
	free(dup);
}

TEST(strndup_zero_length)
{
	char *dup = strndup("test", 0);
	ASSERT_STR_EQ(dup, "");
	free(dup);
}

TEST(strndup_embedded_null_within_n)
{
	char src[] = "hi\0there";
	char *dup = strndup(src, 8);
	ASSERT_STR_EQ(dup, "hi");
	free(dup);
}

TEST(strndup_embedded_null_beyond_n)
{
	char src[] = "hello\0world";
	char *dup = strndup(src, 3);
	ASSERT_STR_EQ(dup, "hel");
	free(dup);
}

TEST(strndup_n_exceeds_string)
{
	char *dup = strndup("short", 1000);
	ASSERT_STR_EQ(dup, "short");
	free(dup);
}

/* ============================================================= */
/* getenv - stdlib.h: unit tests (POSIX)                         */
/* ============================================================= */
#if JACL_HAS_POSIX
TEST_SUITE(getenv)

TEST(getenv_PATH)
{
	char *path = getenv("PATH");
	ASSERT_NOT_NULL(path);
}

TEST(getenv_nonexistent)
{
	char *val = getenv("__NONEXISTENT_VAR_12345__");
	ASSERT_NULL(val);
}

TEST(getenv_empty_name)
{
	char *val = getenv("");
}

TEST(getenv_after_setenv)
{
	setenv("MY_TEST", "value1", 1);
	char *val = getenv("MY_TEST");
	ASSERT_STR_EQ(val, "value1");
	unsetenv("MY_TEST");
}

TEST(getenv_case_sensitive)
{
	setenv("TESTVAR", "upper", 1);
	char *val_lower = getenv("testvar");
	char *val_upper = getenv("TESTVAR");
	ASSERT_INT_NE((val_lower == NULL), (val_upper == NULL));
	unsetenv("TESTVAR");
}
#endif

/* ============================================================= */
/* setenv - stdlib.h: unit tests (POSIX)                         */
/* ============================================================= */
#if JACL_HAS_POSIX
TEST_SUITE(setenv)

TEST(setenv_basic)
{
	int result = setenv("TEST_VAR", "value123", 1);
	ASSERT_INT_EQ(result, 0);
	ASSERT_STR_EQ(getenv("TEST_VAR"), "value123");
	unsetenv("TEST_VAR");
}

TEST(setenv_overwrite)
{
	setenv("TEST_OW", "first", 1);
	int result = setenv("TEST_OW", "second", 1);
	ASSERT_INT_EQ(result, 0);
	ASSERT_STR_EQ(getenv("TEST_OW"), "second");
	unsetenv("TEST_OW");
}

TEST(setenv_no_overwrite)
{
	setenv("TEST_NOV", "first", 1);
	int result = setenv("TEST_NOV", "second", 0);
	ASSERT_INT_EQ(result, 0);
	ASSERT_STR_EQ(getenv("TEST_NOV"), "first");
	unsetenv("TEST_NOV");
}

TEST(setenv_invalid_name)
{
	int result = setenv("BAD=NAME", "value", 1);
	ASSERT_INT_NE(result, 0);
}

TEST(setenv_empty_value)
{
	setenv("EMPTY", "", 1);
	char *val = getenv("EMPTY");
	ASSERT_NOT_NULL(val);
	ASSERT_STR_EQ(val, "");
	unsetenv("EMPTY");
}

TEST(setenv_value_with_spaces)
{
	setenv("SPACED", "hello world test", 1);
	char *val = getenv("SPACED");
	ASSERT_STR_EQ(val, "hello world test");
	unsetenv("SPACED");
}

TEST(setenv_many_variables)
{
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

TEST(setenv_overwrite_flag_zero)
{
	setenv("TEST_OV", "first", 1);
	int result = setenv("TEST_OV", "second", 0);
	ASSERT_INT_EQ(result, 0);
	ASSERT_STR_EQ(getenv("TEST_OV"), "first");
	unsetenv("TEST_OV");
}
#endif

/* ============================================================= */
/* unsetenv - stdlib.h: unit tests (POSIX)                       */
/* ============================================================= */
#if JACL_HAS_POSIX
TEST_SUITE(unsetenv)

TEST(unsetenv_removes)
{
	setenv("TEST_UNSET", "temporary", 1);
	int result = unsetenv("TEST_UNSET");
	ASSERT_INT_EQ(result, 0);
	ASSERT_NULL(getenv("TEST_UNSET"));
}

TEST(unsetenv_nonexistent)
{
	int result = unsetenv("__NEVER_SET_VAR__");
	ASSERT_INT_EQ(result, 0);
}
#endif

/* ============================================================= */
/* system - stdlib.h: unit tests (POSIX)                         */
/* ============================================================= */
#if JACL_HAS_POSIX
TEST_SUITE(system)

TEST(system_success)
{
	int status = system("true");
	ASSERT_INT_EQ(status, 0);
}

TEST(system_failure)
{
	int status = system("false");
	ASSERT_INT_NE(status, 0);
}

TEST(system_echo)
{
	int status = system("echo test > /dev/null");
	ASSERT_INT_EQ(status, 0);
}

TEST(system_nonzero_exit)
{
	int status = system("exit 42");
	ASSERT_INT_EQ(status, 42);
}

TEST(system_command_with_args)
{
	int status = system("echo hello > /dev/null");
	ASSERT_INT_EQ(status, 0);
}

TEST(system_signal_termination)
{
	int status = system("sh -c 'kill -TERM $$'");
	ASSERT_INT_NE(status, 0);
}

TEST(system_stderr_redirect)
{
	int status = system("ls /nonexistent 2>/dev/null");
}
#endif

/* ============================================================= */
/* mblen - stdlib.h: unit tests                                  */
/* ============================================================= */
TEST_SUITE(mblen)

TEST(mblen_ascii)
{
	int len = mblen("A", 1);
	ASSERT_INT_EQ(len, 1);
}

TEST(mblen_null)
{
	int len = mblen("", 1);
	ASSERT_INT_EQ(len, 0);
}

TEST(mblen_insufficient_buffer)
{
	int len = mblen("AB", 0);
	ASSERT_INT_EQ(len, -1);
}

TEST(mblen_consistency)
{
	for (int c = 0; c < 128; c++) {
		char s[2] = {(char)c, '\0'};
		if (c != '\0') {
			int len = mblen(s, 1);
			ASSERT_INT_EQ(len, 1);
		}
	}
}

/* ============================================================= */
/* mbtowc - stdlib.h: unit tests                                 */
/* ============================================================= */
TEST_SUITE(mbtowc)

TEST(mbtowc_basic)
{
	wchar_t wc;
	int len = mbtowc(&wc, "A", 1);
	ASSERT_INT_EQ(len, 1);
	ASSERT_INT_EQ(wc, L'A');
}

TEST(mbtowc_null_string)
{
	int len = mbtowc(NULL, NULL, 0);
	ASSERT_INT_EQ(len, 0);
}

TEST(mbtowc_wctomb_roundtrip)
{
	wchar_t original = L'X';
	char buf[2];
	wctomb(buf, original);

	wchar_t restored;
	mbtowc(&restored, buf, 1);
	ASSERT_INT_EQ(restored, original);
}

/* ============================================================= */
/* wctomb - stdlib.h: unit tests                                 */
/* ============================================================= */
TEST_SUITE(wctomb)

TEST(wctomb_basic)
{
	char s[2];
	int len = wctomb(s, L'A');
	ASSERT_INT_EQ(len, 1);
	ASSERT_INT_EQ(s[0], 'A');
}

TEST(wctomb_null)
{
	int len = wctomb(NULL, L'A');
	ASSERT_INT_EQ(len, 0);
}

/* ============================================================= */
/* mbstowcs - stdlib.h: unit tests                               */
/* ============================================================= */
TEST_SUITE(mbstowcs)

TEST(mbstowcs_basic)
{
	wchar_t buf[10];
	size_t n = mbstowcs(buf, "hello", 5);
	ASSERT_INT_EQ(n, 5);
	ASSERT_INT_EQ(buf[0], L'h');
}

TEST(mbstowcs_null_termination)
{
	wchar_t buf[6];
	mbstowcs(buf, "test", 5);
	ASSERT_INT_EQ(buf[4], L'\0');
}

TEST(mbstowcs_wcstombs_roundtrip)
{
	const char *orig = "hello";
	wchar_t wbuf[10];
	char cbuf[10];

	mbstowcs(wbuf, orig, 10);
	wcstombs(cbuf, wbuf, 10);

	ASSERT_STR_EQ(cbuf, orig);
}

/* ============================================================= */
/* wcstombs - stdlib.h: unit tests                               */
/* ============================================================= */
TEST_SUITE(wcstombs)

TEST(wcstombs_basic)
{
	wchar_t src[] = L"hi";
	char buf[3];
	size_t n = wcstombs(buf, src, 3);
	ASSERT_INT_EQ(n, 2);
	ASSERT_INT_EQ(buf[0], 'h');
}

/* ============================================================= */
/* malloc - stdlib.h: unit tests                                */
/* ============================================================= */
TEST_SUITE(malloc)

TEST(malloc_basic)
{
	void *ptr = malloc(100);
	ASSERT_NOT_NULL(ptr);
	free(ptr);
}

TEST(malloc_zero)
{
	void *ptr = malloc(0);
	// Implementation-defined: may return NULL or unique pointer
	free(ptr);
}

TEST(malloc_large)
{
	void *ptr = malloc(1024 * 1024);  // 1 MB
	ASSERT_NOT_NULL(ptr);
	free(ptr);
}

TEST(malloc_write_and_read)
{
	int *ptr = (int *)malloc(sizeof(int));
	ASSERT_NOT_NULL(ptr);
	*ptr = 42;
	ASSERT_INT_EQ(*ptr, 42);
	free(ptr);
}

TEST(malloc_multiple_allocations)
{
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

TEST(malloc_alignment)
{
	void *ptr = malloc(1);
	ASSERT_NOT_NULL(ptr);
	ASSERT_INT_EQ((unsigned long)ptr % sizeof(void*), 0);
	free(ptr);
}


/* ============================================================= */
/* calloc - stdlib.h: unit tests                                */
/* ============================================================= */
TEST_SUITE(calloc)

TEST(calloc_basic)
{
	int *arr = (int *)calloc(10, sizeof(int));
	ASSERT_NOT_NULL(arr);
	for (int i = 0; i < 10; i++) {
		ASSERT_INT_EQ(arr[i], 0);
	}
	free(arr);
}

TEST(calloc_zero_elements)
{
	void *ptr = calloc(0, sizeof(int));
	free(ptr);
}

TEST(calloc_zero_size)
{
	void *ptr = calloc(10, 0);
	free(ptr);
}

TEST(calloc_large_array)
{
	char *buf = (char *)calloc(1024, 1024);  // 1 MB
	ASSERT_NOT_NULL(buf);
	// Verify first and last bytes are zeroed
	ASSERT_INT_EQ(buf[0], 0);
	ASSERT_INT_EQ(buf[1024*1024 - 1], 0);
	free(buf);
}

TEST(calloc_vs_malloc_zeroing)
{
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

/* ============================================================= */
/* realloc - stdlib.h: unit tests                               */
/* ============================================================= */
TEST_SUITE(realloc)

TEST(realloc_null_behaves_like_malloc)
{
	void *ptr = realloc(NULL, 100);
	ASSERT_NOT_NULL(ptr);
	free(ptr);
}

TEST(realloc_grow)
{
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

TEST(realloc_shrink)
{
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

TEST(realloc_zero_size)
{
	void *ptr = malloc(100);
	ASSERT_NOT_NULL(ptr);

	void *new_ptr = realloc(ptr, 0);
	// Implementation-defined: may free and return NULL, or return minimal allocation
	if (new_ptr != NULL) free(new_ptr);
}

TEST(realloc_same_size)
{
	int *arr = (int *)malloc(10 * sizeof(int));
	for (int i = 0; i < 10; i++) arr[i] = i + 100;

	int *new_arr = (int *)realloc(arr, 10 * sizeof(int));
	ASSERT_NOT_NULL(new_arr);

	for (int i = 0; i < 10; i++) {
		ASSERT_INT_EQ(new_arr[i], i + 100);
	}

	free(new_arr);
}

TEST(realloc_large_to_small_to_large)
{
	void *p1 = malloc(1000);
	ASSERT_NOT_NULL(p1);

	void *p2 = realloc(p1, 100);
	ASSERT_NOT_NULL(p2);

	void *p3 = realloc(p2, 2000);
	ASSERT_NOT_NULL(p3);

	free(p3);
}

/* ============================================================= */
/* free - stdlib.h: unit tests                                  */
/* ============================================================= */
TEST_SUITE(free)

TEST(free_null_safe)
{
	free(NULL);  // Should not crash
}

TEST(free_after_malloc)
{
	void *ptr = malloc(100);
	free(ptr);
}

TEST(free_multiple_pointers)
{
	void *p1 = malloc(10);
	void *p2 = malloc(20);
	void *p3 = malloc(30);

	free(p2);  // Free in different order
	free(p1);
	free(p3);
}

/* ============================================================= */
/* memory_stress - stdlib.h: unit tests                         */
/* ============================================================= */
TEST_SUITE(memory_stress)

TEST(allocation_pattern_interleaved)
{
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

TEST(realloc_chain)
{
	void *ptr = malloc(10);
	ASSERT_NOT_NULL(ptr);

	for (int size = 20; size <= 10000; size *= 2) {
		ptr = realloc(ptr, size);
		ASSERT_NOT_NULL(ptr);
	}

	free(ptr);
}

TEST(calloc_malloc_realloc_combination)
{
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

TEST_MAIN()
