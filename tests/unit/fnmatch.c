/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <fnmatch.h>

TEST_TYPE(unit);
TEST_UNIT(fnmatch.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_fnm_flags) {
	ASSERT_EQ(0x01, FNM_PATHNAME);
	ASSERT_EQ(0x02, FNM_PERIOD);
	ASSERT_EQ(0x04, FNM_NOESCAPE);
}

TEST(constants_fnm_return) {
	ASSERT_EQ(0, FNM_MATCH);
	ASSERT_EQ(1, FNM_NOMATCH);
}

/* ============================================================================ */
TEST_SUITE(fnmatch_literal);

TEST(fnmatch_literal_match) {
	ASSERT_EQ(0, fnmatch("abc", "abc", 0));
}

TEST(fnmatch_literal_mismatch) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("abc", "abd", 0));
}

TEST(fnmatch_literal_case) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("Abc", "abc", 0));
}

TEST(fnmatch_literal_empty) {
	ASSERT_EQ(0, fnmatch("", "", 0));
}

/* ============================================================================ */
TEST_SUITE(fnmatch_escape);

TEST(fnmatch_escape_star) {
	ASSERT_EQ(0, fnmatch("\\*", "*", 0));
}

TEST(fnmatch_escape_question) {
	ASSERT_EQ(0, fnmatch("\\?", "?", 0));
}

TEST(fnmatch_escape_bracket) {
	ASSERT_EQ(0, fnmatch("\\[", "[", 0));
}

TEST(fnmatch_escape_backslash) {
	ASSERT_EQ(0, fnmatch("\\\\", "\\", 0));
}

TEST(fnmatch_noescape_star_matches_literal_star) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("\\*", "*", FNM_NOESCAPE));
}

/* ============================================================================ */
TEST_SUITE(fnmatch_qmark);

TEST(fnmatch_qmark_match) {
	ASSERT_EQ(0, fnmatch("a?c", "abc", 0));
}

TEST(fnmatch_qmark_mismatch) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("a?c", "ac", 0));
	ASSERT_EQ(FNM_NOMATCH, fnmatch("a?c", "abbc", 0));
}

TEST(fnmatch_qmark_multiple) {
	ASSERT_EQ(0, fnmatch("???", "abc", 0));
}

/* ============================================================================ */
TEST_SUITE(fnmatch_star);

TEST(fnmatch_star_basic) {
	ASSERT_EQ(0, fnmatch("a*c", "abc", 0));
}

TEST(fnmatch_star_empty_middle) {
	ASSERT_EQ(0, fnmatch("a*c", "ac", 0));
}

TEST(fnmatch_star_long_middle) {
	ASSERT_EQ(0, fnmatch("a*c", "abbbbbbc", 0));
}

TEST(fnmatch_star_only) {
	ASSERT_EQ(0, fnmatch("*", "anything", 0));
}

TEST(fnmatch_star_prefix) {
	ASSERT_EQ(0, fnmatch("a*", "abc", 0));
}

TEST(fnmatch_star_suffix) {
	ASSERT_EQ(0, fnmatch("*c", "abc", 0));
}

TEST(fnmatch_multiple_stars) {
	ASSERT_EQ(0, fnmatch("*a*b*", "xaaybz", 0));
}

/* ============================================================================ */
TEST_SUITE(fnmatch_class);

TEST(fnmatch_class_match) {
	ASSERT_EQ(0, fnmatch("[abc]", "a", 0));
}

TEST(fnmatch_class_mismatch) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("[abc]", "d", 0));
}

TEST(fnmatch_class_range_match) {
	ASSERT_EQ(0, fnmatch("[a-z]", "m", 0));
}

TEST(fnmatch_class_range_mismatch) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("[a-z]", "M", 0));
}

TEST(fnmatch_class_digit_range) {
	ASSERT_EQ(0, fnmatch("[0-9]", "5", 0));
}

TEST(fnmatch_class_negate_match) {
	ASSERT_EQ(0, fnmatch("[!abc]", "d", 0));
}

TEST(fnmatch_class_negate_mismatch) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("[!abc]", "a", 0));
}

TEST(fnmatch_class_caret_negate) {
	ASSERT_EQ(0, fnmatch("[^abc]", "d", 0));
}

/* ============================================================================ */
TEST_SUITE(fnmatch_flag);

TEST(fnmatch_flag_noescape) {
	ASSERT_EQ(0, fnmatch("\\*", "\\*", FNM_NOESCAPE));
}

TEST(fnmatch_flag_pathname_star_no_slash) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("*", "a/b", FNM_PATHNAME));
}

TEST(fnmatch_flag_pathname_question_no_slash) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("?", "/", FNM_PATHNAME));
}

TEST(fnmatch_flag_period_star_no_dotfile) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("*", ".abc", FNM_PERIOD));
}

TEST(fnmatch_flag_period_star_normal) {
	ASSERT_EQ(FNM_MATCH, fnmatch("*", "abc", FNM_PERIOD));
}

TEST(fnmatch_flag_period_star_after_slash) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("dir/*", "dir/.abc", FNM_PERIOD));
}

TEST(fnmatch_flag_period_question_no_dotfile) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("?", ".abc", FNM_PERIOD));
}

TEST(fnmatch_flag_casefold_basic) {
	ASSERT_EQ(FNM_MATCH, fnmatch("ABC", "abc", FNM_CASEFOLD));
}

TEST(fnmatch_flag_casefold_mismatch) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("ABC", "abd", FNM_CASEFOLD));
}

TEST(fnmatch_flag_casefold_star) {
	ASSERT_EQ(FNM_MATCH, fnmatch("*.TXT", "file.txt", FNM_CASEFOLD));
}

TEST(fnmatch_flag_casefold_question) {
	ASSERT_EQ(FNM_MATCH, fnmatch("A?C", "abc", FNM_CASEFOLD));
}

TEST(fnmatch_flag_casefold_class) {
	ASSERT_EQ(FNM_MATCH, fnmatch("[A-Z]", "m", FNM_CASEFOLD));
}

TEST(fnmatch_flag_leading_dir_match) {
	/* FNM_LEADING_DIR: pattern matches if it matches a leading directory */
	ASSERT_EQ(FNM_MATCH, fnmatch("foo*", "foo/bar", FNM_LEADING_DIR));
}

TEST(fnmatch_flag_leading_dir_exact) {
	ASSERT_EQ(FNM_MATCH, fnmatch("foo", "foo", FNM_LEADING_DIR));
}

TEST(fnmatch_flag_leading_dir_mismatch) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("bar*", "foo/bar", FNM_LEADING_DIR));
}

TEST(fnmatch_flag_leading_dir_with_star) {
	ASSERT_EQ(FNM_MATCH, fnmatch("foo/*", "foo/bar/baz", FNM_LEADING_DIR));
}

TEST(fnmatch_flag_pathname_period_combined) {
	/* PATHNAME + PERIOD: * should not match / or leading . */
	ASSERT_EQ(FNM_NOMATCH, fnmatch("*", ".hidden", FNM_PATHNAME | FNM_PERIOD));
	ASSERT_EQ(FNM_NOMATCH, fnmatch("*", "a/b", FNM_PATHNAME | FNM_PERIOD));
	ASSERT_EQ(FNM_MATCH, fnmatch("*", "normal", FNM_PATHNAME | FNM_PERIOD));
}

TEST(fnmatch_flag_casefold_period) {
	ASSERT_EQ(FNM_MATCH, fnmatch(".*", ".Hidden", FNM_PERIOD | FNM_CASEFOLD));
}

/* ============================================================================ */
TEST_SUITE(fnmatch_fail);

TEST(fnmatch_fail_empty_pattern_nonempty_string) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("", "a", 0));
}

TEST(fnmatch_fail_nonempty_pattern_empty_string) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("a", "", 0));
}

TEST(fnmatch_fail_null_pattern) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch(NULL, "a", 0));
}

TEST(fnmatch_fail_null_string) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("a", NULL, 0));
}

TEST(fnmatch_fail_unterminated_bracket) {
	ASSERT_EQ(FNM_NOMATCH, fnmatch("[abc", "a", 0));
}

/* ============================================================================ */
TEST_MAIN()
