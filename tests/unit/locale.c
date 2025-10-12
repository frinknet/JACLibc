/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <locale.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(locale.h);

/* ============================================================================
 * CONSTANTS
 * ============================================================================ */
TEST_SUITE(constants);

TEST(locale_categories) {
	ASSERT_EQ(0, LC_ALL);
	ASSERT_EQ(1, LC_COLLATE);
	ASSERT_EQ(2, LC_CTYPE);
	ASSERT_EQ(3, LC_MONETARY);
	ASSERT_EQ(4, LC_NUMERIC);
	ASSERT_EQ(5, LC_TIME);
}

TEST(categories_unique) {
	ASSERT_NE(LC_ALL, LC_COLLATE);
	ASSERT_NE(LC_COLLATE, LC_CTYPE);
	ASSERT_NE(LC_CTYPE, LC_MONETARY);
	ASSERT_NE(LC_MONETARY, LC_NUMERIC);
	ASSERT_NE(LC_NUMERIC, LC_TIME);
}

/* ============================================================================
 * LCONV STRUCTURE
 * ============================================================================ */
TEST_SUITE(lconv_structure);

TEST(lconv_has_decimal_point) {
	lconv conv;
	conv.decimal_point = ".";
	ASSERT_STR_EQ(".", conv.decimal_point);
}

TEST(lconv_has_thousands_sep) {
	lconv conv;
	conv.thousands_sep = ",";
	ASSERT_STR_EQ(",", conv.thousands_sep);
}

TEST(lconv_has_currency_fields) {
	lconv conv;
	conv.currency_symbol = "$";
	conv.int_curr_symbol = "USD";
	
	ASSERT_STR_EQ("$", conv.currency_symbol);
	ASSERT_STR_EQ("USD", conv.int_curr_symbol);
}

/* ============================================================================
 * SETLOCALE
 * ============================================================================ */
TEST_SUITE(setlocale_test);

TEST(setlocale_query) {
	char *result = setlocale(LC_ALL, NULL);
	
	ASSERT_NOT_NULL(result);
	ASSERT_STR_EQ("C", result);
}

TEST(setlocale_c_locale) {
	char *result = setlocale(LC_ALL, "C");
	
	ASSERT_NOT_NULL(result);
	ASSERT_STR_EQ("C", result);
}

TEST(setlocale_empty_string) {
	char *result = setlocale(LC_ALL, "");
	
	ASSERT_NOT_NULL(result);
	ASSERT_STR_EQ("C", result);
}

TEST(setlocale_invalid_locale) {
	char *result = setlocale(LC_ALL, "invalid_locale_xyz");
	
	ASSERT_NULL(result);
}

TEST(setlocale_different_categories) {
	ASSERT_NOT_NULL(setlocale(LC_COLLATE, "C"));
	ASSERT_NOT_NULL(setlocale(LC_CTYPE, "C"));
	ASSERT_NOT_NULL(setlocale(LC_MONETARY, "C"));
	ASSERT_NOT_NULL(setlocale(LC_NUMERIC, "C"));
	ASSERT_NOT_NULL(setlocale(LC_TIME, "C"));
}

TEST(setlocale_consistency) {
	char *result1 = setlocale(LC_ALL, NULL);
	char *result2 = setlocale(LC_ALL, NULL);
	
	ASSERT_STR_EQ(result1, result2);
}

/* ============================================================================
 * LOCALECONV
 * ============================================================================ */
TEST_SUITE(localeconv_test);

TEST(localeconv_returns_pointer) {
	lconv *conv = localeconv();
	
	ASSERT_NOT_NULL(conv);
}

TEST(localeconv_decimal_point) {
	lconv *conv = localeconv();
	
	ASSERT_NOT_NULL(conv->decimal_point);
	ASSERT_STR_EQ(".", conv->decimal_point);
}

TEST(localeconv_thousands_sep) {
	lconv *conv = localeconv();
	
	ASSERT_NOT_NULL(conv->thousands_sep);
	ASSERT_STR_EQ("", conv->thousands_sep);
}

TEST(localeconv_grouping) {
	lconv *conv = localeconv();
	
	ASSERT_NOT_NULL(conv->grouping);
	ASSERT_STR_EQ("", conv->grouping);
}

TEST(localeconv_currency_symbol) {
	lconv *conv = localeconv();
	
	ASSERT_NOT_NULL(conv->currency_symbol);
	ASSERT_STR_EQ("", conv->currency_symbol);
}

TEST(localeconv_int_curr_symbol) {
	lconv *conv = localeconv();
	
	ASSERT_NOT_NULL(conv->int_curr_symbol);
	ASSERT_STR_EQ("", conv->int_curr_symbol);
}

TEST(localeconv_mon_decimal_point) {
	lconv *conv = localeconv();
	
	ASSERT_NOT_NULL(conv->mon_decimal_point);
}

TEST(localeconv_positive_sign) {
	lconv *conv = localeconv();
	
	ASSERT_NOT_NULL(conv->positive_sign);
	ASSERT_STR_EQ("", conv->positive_sign);
}

TEST(localeconv_negative_sign) {
	lconv *conv = localeconv();
	
	ASSERT_NOT_NULL(conv->negative_sign);
	ASSERT_STR_EQ("", conv->negative_sign);
}

/* ============================================================================
 * LCONV CHAR FIELDS
 * ============================================================================ */
TEST_SUITE(lconv_char_fields);

TEST(localeconv_frac_digits) {
	lconv *conv = localeconv();
	
	ASSERT_EQ(CHAR_MAX, conv->frac_digits);
}

TEST(localeconv_int_frac_digits) {
	lconv *conv = localeconv();
	
	ASSERT_EQ(CHAR_MAX, conv->int_frac_digits);
}

TEST(localeconv_p_cs_precedes) {
	lconv *conv = localeconv();
	
	ASSERT_EQ(CHAR_MAX, conv->p_cs_precedes);
}

TEST(localeconv_n_cs_precedes) {
	lconv *conv = localeconv();
	
	ASSERT_EQ(CHAR_MAX, conv->n_cs_precedes);
}

TEST(localeconv_p_sep_by_space) {
	lconv *conv = localeconv();
	
	ASSERT_EQ(CHAR_MAX, conv->p_sep_by_space);
}

TEST(localeconv_n_sep_by_space) {
	lconv *conv = localeconv();
	
	ASSERT_EQ(CHAR_MAX, conv->n_sep_by_space);
}

TEST(localeconv_p_sign_posn) {
	lconv *conv = localeconv();
	
	ASSERT_EQ(CHAR_MAX, conv->p_sign_posn);
}

TEST(localeconv_n_sign_posn) {
	lconv *conv = localeconv();
	
	ASSERT_EQ(CHAR_MAX, conv->n_sign_posn);
}

/* ============================================================================
 * CONSISTENCY TESTS
 * ============================================================================ */
TEST_SUITE(consistency);

TEST(localeconv_same_pointer) {
	lconv *conv1 = localeconv();
	lconv *conv2 = localeconv();
	
	ASSERT_EQ(conv1, conv2);
}

TEST(setlocale_localeconv_interaction) {
	setlocale(LC_ALL, "C");
	lconv *conv = localeconv();
	
	ASSERT_NOT_NULL(conv);
	ASSERT_STR_EQ(".", conv->decimal_point);
}

/* ============================================================================
 * PRACTICAL USAGE
 * ============================================================================ */
TEST_SUITE(practical_usage);

TEST(decimal_point_for_numbers) {
	lconv *conv = localeconv();
	
	// Should use "." for C locale
	ASSERT_TRUE(strcmp(conv->decimal_point, ".") == 0);
}

TEST(thousands_separator_empty) {
	lconv *conv = localeconv();
	
	// C locale has no thousands separator
	ASSERT_TRUE(strlen(conv->thousands_sep) == 0);
}

TEST_MAIN()
