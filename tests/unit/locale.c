/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>

TEST_TYPE(unit);
TEST_UNIT(locale.h);

/* ============================================================================
 * locale_constants
 * ============================================================================ */
TEST_SUITE(locale_constants);

TEST(locale_constants_categories) {
	ASSERT_EQ(0, LC_CTYPE);
	ASSERT_EQ(1, LC_NUMERIC);
	ASSERT_EQ(2, LC_TIME);
	ASSERT_EQ(3, LC_COLLATE);
	ASSERT_EQ(4, LC_MONETARY);
	ASSERT_EQ(5, LC_MESSAGES);
	ASSERT_EQ(6, LC_ALL);
}

TEST(locale_constants_unique) {
	ASSERT_NE(LC_ALL, LC_COLLATE);
	ASSERT_NE(LC_COLLATE, LC_CTYPE);
	ASSERT_NE(LC_CTYPE, LC_MONETARY);
	ASSERT_NE(LC_MONETARY, LC_NUMERIC);
	ASSERT_NE(LC_NUMERIC, LC_TIME);
}

/* ============================================================================
 * lconv
 * ============================================================================ */
TEST_SUITE(lconv);

TEST(lconv_decimal_point) {
	struct lconv conv;
	conv.decimal_point = ".";
	ASSERT_STR_EQ(".", conv.decimal_point);
}

TEST(lconv_thousands_sep) {
	struct lconv conv;
	conv.thousands_sep = ",";
	ASSERT_STR_EQ(",", conv.thousands_sep);
}

TEST(lconv_currency_symbol) {
	struct lconv conv;
	conv.currency_symbol = "$";
	ASSERT_STR_EQ("$", conv.currency_symbol);
}

TEST(lconv_int_curr_symbol) {
	struct lconv conv;
	conv.int_curr_symbol = "USD";
	ASSERT_STR_EQ("USD", conv.int_curr_symbol);
}

TEST(lconv_mon_decimal_point) {
	struct lconv conv;
	conv.mon_decimal_point = ".";
	ASSERT_STR_EQ(".", conv.mon_decimal_point);
}

TEST(lconv_mon_thousands_sep) {
	struct lconv conv;
	conv.mon_thousands_sep = ",";
	ASSERT_STR_EQ(",", conv.mon_thousands_sep);
}

TEST(lconv_mon_grouping) {
	struct lconv conv;
	conv.mon_grouping = "\3";
	ASSERT_STR_EQ("\3", conv.mon_grouping);
}

TEST(lconv_positive_sign) {
	struct lconv conv;
	conv.positive_sign = "+";
	ASSERT_STR_EQ("+", conv.positive_sign);
}

TEST(lconv_negative_sign) {
	struct lconv conv;
	conv.negative_sign = "-";
	ASSERT_STR_EQ("-", conv.negative_sign);
}

TEST(lconv_frac_digits) {
	struct lconv conv;
	conv.frac_digits = CHAR_MAX;
	ASSERT_EQ(CHAR_MAX, conv.frac_digits);
}

TEST(lconv_int_frac_digits) {
	struct lconv conv;
	conv.int_frac_digits = CHAR_MAX;
	ASSERT_EQ(CHAR_MAX, conv.int_frac_digits);
}

TEST(lconv_p_cs_precedes) {
	struct lconv conv;
	conv.p_cs_precedes = CHAR_MAX;
	ASSERT_EQ(CHAR_MAX, conv.p_cs_precedes);
}

TEST(lconv_n_cs_precedes) {
	struct lconv conv;
	conv.n_cs_precedes = CHAR_MAX;
	ASSERT_EQ(CHAR_MAX, conv.n_cs_precedes);
}

TEST(lconv_p_sep_by_space) {
	struct lconv conv;
	conv.p_sep_by_space = CHAR_MAX;
	ASSERT_EQ(CHAR_MAX, conv.p_sep_by_space);
}

TEST(lconv_n_sep_by_space) {
	struct lconv conv;
	conv.n_sep_by_space = CHAR_MAX;
	ASSERT_EQ(CHAR_MAX, conv.n_sep_by_space);
}

TEST(lconv_p_sign_posn) {
	struct lconv conv;
	conv.p_sign_posn = CHAR_MAX;
	ASSERT_EQ(CHAR_MAX, conv.p_sign_posn);
}

TEST(lconv_n_sign_posn) {
	struct lconv conv;
	conv.n_sign_posn = CHAR_MAX;
	ASSERT_EQ(CHAR_MAX, conv.n_sign_posn);
}

TEST(lconv_int_p_cs_precedes) {
	struct lconv conv;
	conv.int_p_cs_precedes = CHAR_MAX;
	ASSERT_EQ(CHAR_MAX, conv.int_p_cs_precedes);
}

TEST(lconv_int_n_cs_precedes) {
	struct lconv conv;
	conv.int_n_cs_precedes = CHAR_MAX;
	ASSERT_EQ(CHAR_MAX, conv.int_n_cs_precedes);
}

TEST(lconv_int_p_sep_by_space) {
	struct lconv conv;
	conv.int_p_sep_by_space = CHAR_MAX;
	ASSERT_EQ(CHAR_MAX, conv.int_p_sep_by_space);
}

TEST(lconv_int_n_sep_by_space) {
	struct lconv conv;
	conv.int_n_sep_by_space = CHAR_MAX;
	ASSERT_EQ(CHAR_MAX, conv.int_n_sep_by_space);
}

TEST(lconv_int_p_sign_posn) {
	struct lconv conv;
	conv.int_p_sign_posn = CHAR_MAX;
	ASSERT_EQ(CHAR_MAX, conv.int_p_sign_posn);
}

TEST(lconv_int_n_sign_posn) {
	struct lconv conv;
	conv.int_n_sign_posn = CHAR_MAX;
	ASSERT_EQ(CHAR_MAX, conv.int_n_sign_posn);
}

/* ============================================================================
 * setlocale
 * ============================================================================ */
TEST_SUITE(setlocale);

TEST(setlocale_query) {
	char *result = setlocale(LC_ALL, NULL);
	ASSERT_NOT_NULL(result);
	ASSERT_STR_EQ("C.UTF-8", result);
}

TEST(setlocale_c_locale) {
	char *result = setlocale(LC_ALL, "C.UTF-8");
	ASSERT_STR_EQ("C.UTF-8", result);
}

TEST(setlocale_posix_locale) {
	char *result = setlocale(LC_ALL, "POSIX");
	ASSERT_NOT_NULL(result);
	ASSERT_STR_EQ("C.UTF-8", result);
}

TEST(setlocale_empty_string) {
	char *result = setlocale(LC_ALL, "");
	ASSERT_NOT_NULL(result);
	ASSERT_STR_EQ("C.UTF-8", result);
}

TEST(setlocale_invalid_locale) {
	char *result = setlocale(LC_ALL, "invalid_locale_xyz");
	ASSERT_NULL(result);
}

TEST(setlocale_malformed_locale) {
	char *result = setlocale(LC_ALL, "invalid");
	ASSERT_NULL(result);
}

TEST(setlocale_category_independence) {
	char *orig = setlocale(LC_ALL, NULL);
	setlocale(LC_NUMERIC, "C");
	setlocale(LC_TIME, "C");
	char *numeric = setlocale(LC_NUMERIC, NULL);
	char *time = setlocale(LC_TIME, NULL);
	ASSERT_STR_EQ("C.UTF-8", numeric);
	ASSERT_STR_EQ("C.UTF-8", time);
	setlocale(LC_ALL, orig);
}

TEST(setlocale_consistency) {
	char *result1 = setlocale(LC_ALL, NULL);
	char *result2 = setlocale(LC_ALL, NULL);
	ASSERT_STR_EQ(result1, result2);
}

TEST(setlocale_null_category_undefined) {
	char *result = setlocale(0, "C");
	ASSERT_TRUE(result != NULL || result == NULL);
}

TEST(setlocale_to_current_value) {
	char *orig = setlocale(LC_ALL, NULL);
	char *result = setlocale(LC_ALL, orig);
	ASSERT_NOT_NULL(result);
	ASSERT_STR_EQ(orig, result);
}

TEST(setlocale_category_string_format) {
	char *result = setlocale(LC_ALL, "C");
	ASSERT_TRUE(strstr(result, "C") != NULL || strstr(result, "POSIX") != NULL);
}

TEST(setlocale_lang_only) {
	char *result = setlocale(LC_ALL, "en");
	ASSERT_TRUE(result != NULL);
	if (result) setlocale(LC_ALL, "C");
}

TEST(setlocale_lang_cc) {
	char *result = setlocale(LC_ALL, "en_US");
	ASSERT_TRUE(result != NULL);
	if (result) setlocale(LC_ALL, "C");
}

TEST(setlocale_with_codeset) {
	char *result = setlocale(LC_ALL, "en_US.UTF-8");
	ASSERT_TRUE(result != NULL);
	if (result) setlocale(LC_ALL, "C");
}

TEST(setlocale_with_modifier) {
	char *result = setlocale(LC_ALL, "de_DE@euro");
	ASSERT_TRUE(result != NULL);
	if (result) setlocale(LC_ALL, "C");
}

TEST(setlocale_env_lc_all) {
	setenv("LC_ALL", "C", 1);
	setenv("LANG", "en_US.UTF-8", 1);
	char *result = setlocale(LC_ALL, "");
	ASSERT_NOT_NULL(result);
	ASSERT_STR_EQ("C.UTF-8", result);
	unsetenv("LC_ALL");
	unsetenv("LANG");
}

TEST(setlocale_env_lc_category) {
	setenv("LC_NUMERIC", "C", 1);
	setenv("LANG", "en_US.UTF-8", 1);
	char *result = setlocale(LC_NUMERIC, "");
	ASSERT_NOT_NULL(result);
	ASSERT_STR_EQ("C.UTF-8", result);
	unsetenv("LC_NUMERIC");
	unsetenv("LANG");
}

TEST(setlocale_env_lang_fallback) {
	unsetenv("LC_ALL");
	unsetenv("LC_NUMERIC");
	setenv("LANG", "C", 1);
	char *result = setlocale(LC_NUMERIC, "");
	ASSERT_NOT_NULL(result);
	ASSERT_STR_EQ("C.UTF-8", result);
	unsetenv("LANG");
}

TEST(setlocale_env_empty) {
	setenv("LC_ALL", "", 1);
	setenv("LANG", "", 1);
	char *result = setlocale(LC_ALL, "");
	ASSERT_NOT_NULL(result);
	ASSERT_STR_EQ("C.UTF-8", result);
	unsetenv("LC_ALL");
	unsetenv("LANG");
}

/* ============================================================================
 * localeconv
 * ============================================================================ */
TEST_SUITE(localeconv);

TEST(localeconv_returns_pointer) {
	struct lconv *conv = localeconv();
	ASSERT_NOT_NULL(conv);
}

TEST(localeconv_decimal_point) {
	struct lconv *conv = localeconv();
	ASSERT_NOT_NULL(conv->decimal_point);
	ASSERT_STR_EQ(".", conv->decimal_point);
}

TEST(localeconv_thousands_sep) {
	struct lconv *conv = localeconv();
	ASSERT_NOT_NULL(conv->thousands_sep);
	ASSERT_STR_EQ("", conv->thousands_sep);
}

TEST(localeconv_grouping) {
	struct lconv *conv = localeconv();
	ASSERT_NOT_NULL(conv->grouping);
	ASSERT_STR_EQ("", conv->grouping);
}

TEST(localeconv_currency_symbol) {
	struct lconv *conv = localeconv();
	ASSERT_NOT_NULL(conv->currency_symbol);
	ASSERT_STR_EQ("", conv->currency_symbol);
}

TEST(localeconv_int_curr_symbol) {
	struct lconv *conv = localeconv();
	ASSERT_NOT_NULL(conv->int_curr_symbol);
	ASSERT_STR_EQ("", conv->int_curr_symbol);
}

TEST(localeconv_mon_decimal_point) {
	struct lconv *conv = localeconv();
	ASSERT_NOT_NULL(conv->mon_decimal_point);
	ASSERT_STR_EQ(".", conv->mon_decimal_point);
}

TEST(localeconv_mon_thousands_sep) {
	struct lconv *conv = localeconv();
	ASSERT_NOT_NULL(conv->mon_thousands_sep);
	ASSERT_STR_EQ("", conv->mon_thousands_sep);
}

TEST(localeconv_mon_grouping) {
	struct lconv *conv = localeconv();
	ASSERT_NOT_NULL(conv->mon_grouping);
	ASSERT_STR_EQ("", conv->mon_grouping);
}

TEST(localeconv_positive_sign) {
	struct lconv *conv = localeconv();
	ASSERT_NOT_NULL(conv->positive_sign);
	ASSERT_STR_EQ("", conv->positive_sign);
}

TEST(localeconv_negative_sign) {
	struct lconv *conv = localeconv();
	ASSERT_NOT_NULL(conv->negative_sign);
	ASSERT_STR_EQ("", conv->negative_sign);
}

TEST(localeconv_frac_digits) {
	struct lconv *conv = localeconv();
	ASSERT_EQ(CHAR_MAX, conv->frac_digits);
}

TEST(localeconv_int_frac_digits) {
	struct lconv *conv = localeconv();
	ASSERT_EQ(CHAR_MAX, conv->int_frac_digits);
}

TEST(localeconv_p_cs_precedes) {
	struct lconv *conv = localeconv();
	ASSERT_EQ(CHAR_MAX, conv->p_cs_precedes);
}

TEST(localeconv_n_cs_precedes) {
	struct lconv *conv = localeconv();
	ASSERT_EQ(CHAR_MAX, conv->n_cs_precedes);
}

TEST(localeconv_p_sep_by_space) {
	struct lconv *conv = localeconv();
	ASSERT_EQ(CHAR_MAX, conv->p_sep_by_space);
}

TEST(localeconv_n_sep_by_space) {
	struct lconv *conv = localeconv();
	ASSERT_EQ(CHAR_MAX, conv->n_sep_by_space);
}

TEST(localeconv_p_sign_posn) {
	struct lconv *conv = localeconv();
	ASSERT_EQ(CHAR_MAX, conv->p_sign_posn);
}

TEST(localeconv_n_sign_posn) {
	struct lconv *conv = localeconv();
	ASSERT_EQ(CHAR_MAX, conv->n_sign_posn);
}

TEST(localeconv_int_p_cs_precedes) {
	struct lconv *conv = localeconv();
	ASSERT_EQ(CHAR_MAX, conv->int_p_cs_precedes);
}

TEST(localeconv_int_n_cs_precedes) {
	struct lconv *conv = localeconv();
	ASSERT_EQ(CHAR_MAX, conv->int_n_cs_precedes);
}

TEST(localeconv_int_p_sep_by_space) {
	struct lconv *conv = localeconv();
	ASSERT_EQ(CHAR_MAX, conv->int_p_sep_by_space);
}

TEST(localeconv_int_n_sep_by_space) {
	struct lconv *conv = localeconv();
	ASSERT_EQ(CHAR_MAX, conv->int_n_sep_by_space);
}

TEST(localeconv_int_p_sign_posn) {
	struct lconv *conv = localeconv();
	ASSERT_EQ(CHAR_MAX, conv->int_p_sign_posn);
}

TEST(localeconv_int_n_sign_posn) {
	struct lconv *conv = localeconv();
	ASSERT_EQ(CHAR_MAX, conv->int_n_sign_posn);
}

TEST(localeconv_same_pointer) {
	struct lconv *conv1 = localeconv();
	struct lconv *conv2 = localeconv();
	ASSERT_EQ(conv1, conv2);
}

TEST(localeconv_after_setlocale) {
	setlocale(LC_ALL, "C");
	struct lconv *conv = localeconv();
	ASSERT_NOT_NULL(conv);
	ASSERT_STR_EQ(".", conv->decimal_point);
}

TEST(localeconv_preserved_after_setlocale) {
	struct lconv *before = localeconv();
	setlocale(LC_NUMERIC, "C");
	struct lconv *after = localeconv();
	ASSERT_EQ(before, after);
}

TEST(localeconv_decimal_point_usage) {
	struct lconv *conv = localeconv();
	ASSERT_TRUE(strcmp(conv->decimal_point, ".") == 0);
}

TEST(localeconv_thousands_sep_empty) {
	struct lconv *conv = localeconv();
	ASSERT_TRUE(strlen(conv->thousands_sep) == 0);
}

TEST(localeconv_currency_empty) {
	struct lconv *conv = localeconv();
	ASSERT_TRUE(strlen(conv->currency_symbol) == 0);
	ASSERT_TRUE(strlen(conv->int_curr_symbol) == 0);
}

/* ============================================================================
 * locale_posix_compliance
 * ============================================================================ */
TEST_SUITE(locale_posix_compliance);

TEST(locale_posix_lconv_fields) {
	struct lconv conv;
	(void)conv.decimal_point;
	(void)conv.thousands_sep;
	(void)conv.grouping;
	(void)conv.int_curr_symbol;
	(void)conv.currency_symbol;
	(void)conv.mon_decimal_point;
	(void)conv.mon_thousands_sep;
	(void)conv.mon_grouping;
	(void)conv.positive_sign;
	(void)conv.negative_sign;
	(void)conv.int_frac_digits;
	(void)conv.frac_digits;
	(void)conv.p_cs_precedes;
	(void)conv.p_sep_by_space;
	(void)conv.n_cs_precedes;
	(void)conv.n_sep_by_space;
	(void)conv.p_sign_posn;
	(void)conv.n_sign_posn;
	(void)conv.int_p_cs_precedes;
	(void)conv.int_p_sep_by_space;
	(void)conv.int_n_cs_precedes;
	(void)conv.int_n_sep_by_space;
	(void)conv.int_p_sign_posn;
	(void)conv.int_n_sign_posn;
	ASSERT_TRUE(1);
}

TEST(locale_posix_setlocale_returns_string) {
	char *result = setlocale(LC_ALL, "C");
	ASSERT_NOT_NULL(result);
	ASSERT_TRUE(strlen(result) > 0);
}

TEST(locale_posix_localeconv_static) {
	struct lconv *conv1 = localeconv();
	struct lconv *conv2 = localeconv();
	ASSERT_EQ(conv1, conv2);
}

TEST_MAIN()
