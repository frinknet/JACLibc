/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <wordexp.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

TEST_TYPE(unit);
TEST_UNIT(wordexp.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_flags) {
	ASSERT_INT_EQ(WRDE_APPEND,  1);
	ASSERT_INT_EQ(WRDE_DOOFFS,  2);
	ASSERT_INT_EQ(WRDE_NOCMD,   4);
	ASSERT_INT_EQ(WRDE_REUSE,   8);
	ASSERT_INT_EQ(WRDE_SHOWERR,16);
	ASSERT_INT_EQ(WRDE_UNDEF,  32);
	ASSERT_INT_NE(WRDE_APPEND, WRDE_DOOFFS);
	ASSERT_INT_NE(WRDE_NOCMD,  WRDE_REUSE);
	ASSERT_TRUE((WRDE_APPEND | WRDE_DOOFFS) == 3);
}

TEST(constants_errors) {
	ASSERT_INT_EQ(WRDE_BADCHAR, 1);
	ASSERT_INT_EQ(WRDE_BADVAL,  2);
	ASSERT_INT_EQ(WRDE_CMDSUB,  3);
	ASSERT_INT_EQ(WRDE_NOSPACE, 4);
	ASSERT_INT_EQ(WRDE_SYNTAX,  5);
}

/* ============================================================================ */
TEST_SUITE(wordexp_t);

TEST(wordexp_t_size_positive) {
	ASSERT_SIZE_MIN(wordexp_t, sizeof(size_t) * 2 + sizeof(char **));
}

TEST(wordexp_t_we_wordc_offset) {
	ASSERT_OFFSET(wordexp_t, we_wordc, 0);
}

TEST(wordexp_t_we_wordv_offset) {
	ASSERT_OFFSET_AFTER(wordexp_t, we_wordv, we_wordc);
}

TEST(wordexp_t_we_offs_offset) {
	ASSERT_OFFSET_AFTER(wordexp_t, we_offs, we_wordv);
}

TEST(wordexp_t_zero_init) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(w.we_wordc, 0);
	ASSERT_NULL(w.we_wordv);
	ASSERT_INT_EQ(w.we_offs, 0);
}

TEST(wordexp_t_designated_init) {
	char *v[] = {"a", "b"};
	wordexp_t w = {.we_wordc = 2, .we_wordv = v, .we_offs = 1};
	ASSERT_INT_EQ(w.we_wordc, 2);
	ASSERT_PTR_EQ(w.we_wordv, v);
	ASSERT_INT_EQ(w.we_offs, 1);
}

/* ============================================================================ */
TEST_SUITE(wordfree);

TEST(wordfree_null_safe) {
	wordfree(NULL);
	wordexp_t w = {0};
	wordfree(&w);
	ASSERT_TRUE(1);
}

TEST(wordfree_frees_allocated) {
	wordexp_t w = {.we_wordc = 2, .we_wordv = malloc(3 * sizeof(char *))};
	w.we_wordv[0] = strdup("x");
	w.we_wordv[1] = strdup("y");
	w.we_wordv[2] = NULL;
	wordfree(&w);
	ASSERT_NULL(w.we_wordv);
	ASSERT_INT_EQ(w.we_wordc, 0);
}

TEST(wordfree_idempotent) {
	wordexp_t w = {.we_wordc = 1, .we_wordv = malloc(2 * sizeof(char *))};
	w.we_wordv[0] = strdup("z");
	w.we_wordv[1] = NULL;
	wordfree(&w);
	wordfree(&w);
	ASSERT_TRUE(1);
}

/* ============================================================================ */
TEST_SUITE(wordexp_happy);

TEST(wordexp_single_word) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("alpha", &w, 0), 0);
	ASSERT_INT_EQ(w.we_wordc, 1);
	ASSERT_STR_EQ(w.we_wordv[0], "alpha");
	wordfree(&w);
}

TEST(wordexp_multiple_words) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("one two three", &w, 0), 0);
	ASSERT_INT_EQ(w.we_wordc, 3);
	ASSERT_STR_EQ(w.we_wordv[0], "one");
	ASSERT_STR_EQ(w.we_wordv[1], "two");
	ASSERT_STR_EQ(w.we_wordv[2], "three");
	wordfree(&w);
}

TEST(wordexp_single_quotes) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("'hello world'", &w, 0), 0);
	ASSERT_INT_EQ(w.we_wordc, 1);
	ASSERT_STR_EQ(w.we_wordv[0], "hello world");
	wordfree(&w);
}

TEST(wordexp_double_quotes) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("\"a b\" c", &w, 0), 0);
	ASSERT_INT_EQ(w.we_wordc, 2);
	ASSERT_STR_EQ(w.we_wordv[0], "a b");
	ASSERT_STR_EQ(w.we_wordv[1], "c");
	wordfree(&w);
}

TEST(wordexp_mixed_quotes) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("'single' \"double\" plain", &w, 0), 0);
	ASSERT_INT_EQ(w.we_wordc, 3);
	ASSERT_STR_EQ(w.we_wordv[0], "single");
	ASSERT_STR_EQ(w.we_wordv[1], "double");
	ASSERT_STR_EQ(w.we_wordv[2], "plain");
	wordfree(&w);
}

TEST(wordexp_escape_chars) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("a\\ b", &w, 0), 0);
	ASSERT_STR_EQ(w.we_wordv[0], "a b");
	wordfree(&w);
}

TEST(wordexp_mixed_quote_nesting) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("'\"nested\"'", &w, 0), 0);
	ASSERT_STR_EQ(w.we_wordv[0], "\"nested\"");
	wordfree(&w);
}

TEST(wordexp_empty_quotes) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("'' \"\"", &w, 0), 0);
	ASSERT_INT_EQ(w.we_wordc, 2);
	ASSERT_STR_EQ(w.we_wordv[0], "");
	ASSERT_STR_EQ(w.we_wordv[1], "");
	wordfree(&w);
}

TEST(wordexp_tilde_expansion) {
	setenv("HOME", "/home/test", 1);
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("~/file", &w, 0), 0);
	ASSERT_STR_EQ(w.we_wordv[0], "/home/test/file");
	wordfree(&w);
	unsetenv("HOME");
}

TEST(wordexp_tilde_not_at_start) {
	setenv("HOME", "/home", 1);
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("foo~/bar", &w, 0), 0);
	ASSERT_STR_EQ(w.we_wordv[0], "foo~/bar");
	wordfree(&w);
	unsetenv("HOME");
}

TEST(wordexp_var_expansion) {
	setenv("MYVAR", "expanded", 1);
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("prefix-$MYVAR-suffix", &w, 0), 0);
	ASSERT_STR_EQ(w.we_wordv[0], "prefix-expanded-suffix");
	wordfree(&w);
	unsetenv("MYVAR");
}

TEST(wordexp_var_in_quotes_not_expanded) {
	setenv("NOEXP", "value", 1);
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("'$NOEXP'", &w, 0), 0);
	ASSERT_STR_EQ(w.we_wordv[0], "$NOEXP");
	wordfree(&w);
	unsetenv("NOEXP");
}

TEST(wordexp_cmd_sub_paren) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("foo$(echo bar)", &w, 0), 0);
	ASSERT_INT_EQ(w.we_wordc, 1);
	ASSERT_STR_EQ(w.we_wordv[0], "foobar");
	wordfree(&w);
}

TEST(wordexp_cmd_sub_backtick) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("foo`echo bar`", &w, 0), 0);
	ASSERT_INT_EQ(w.we_wordc, 1);
	ASSERT_STR_EQ(w.we_wordv[0], "foobar");
	wordfree(&w);
}

/* ============================================================================ */
TEST_SUITE(wordexp_sad);

TEST(wordexp_null_words) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp(NULL, &w, 0), WRDE_SYNTAX);
}

TEST(wordexp_null_pwordexp) {
	ASSERT_INT_EQ(wordexp("test", NULL, 0), WRDE_SYNTAX);
}

TEST(wordexp_unclosed_single_quote) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("'unclosed", &w, 0), WRDE_SYNTAX);
	wordfree(&w);
}

TEST(wordexp_unclosed_double_quote) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("\"unclosed", &w, 0), WRDE_SYNTAX);
	wordfree(&w);
}

TEST(wordexp_backslash_at_end) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("test\\", &w, 0), WRDE_SYNTAX);
	wordfree(&w);
}

TEST(wordexp_badchar_pipe) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("foo|bar", &w, 0), WRDE_BADCHAR);
	wordfree(&w);
}

TEST(wordexp_badchar_ampersand) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("foo&bar", &w, 0), WRDE_BADCHAR);
	wordfree(&w);
}

TEST(wordexp_badchar_semicolon) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("foo;bar", &w, 0), WRDE_BADCHAR);
	wordfree(&w);
}

TEST(wordexp_badchar_redir_in) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("foo<bar", &w, 0), WRDE_BADCHAR);
	wordfree(&w);
}

TEST(wordexp_badchar_redir_out) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("foo>bar", &w, 0), WRDE_BADCHAR);
	wordfree(&w);
}

TEST(wordexp_badchar_hash) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("foo#bar", &w, 0), WRDE_BADCHAR);
	wordfree(&w);
}

TEST(wordexp_badchar_newline) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("foo\nbar", &w, 0), WRDE_BADCHAR);
	wordfree(&w);
}

TEST(wordexp_badchar_paren) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("foo(bar)", &w, 0), WRDE_BADCHAR);
	wordfree(&w);
}

TEST(wordexp_badchar_brace) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("foo{bar}", &w, 0), WRDE_BADCHAR);
	wordfree(&w);
}

TEST(wordexp_badchar_inside_quotes_safe) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("'|&;<>#()'", &w, 0), 0);
	ASSERT_STR_EQ(w.we_wordv[0], "|&;<>#()");
	wordfree(&w);
}

/* ============================================================================ */
TEST_SUITE(wordexp_cmd_sad);

TEST(wordexp_cmd_nocmd_paren) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("$(echo x)", &w, WRDE_NOCMD), WRDE_CMDSUB);
	wordfree(&w);
}

TEST(wordexp_cmd_nocmd_backtick) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("`echo x`", &w, WRDE_NOCMD), WRDE_CMDSUB);
	wordfree(&w);
}

TEST(wordexp_cmd_unclosed_paren) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("$(echo", &w, 0), WRDE_SYNTAX);
	wordfree(&w);
}

TEST(wordexp_cmd_unclosed_backtick) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("`echo", &w, 0), WRDE_SYNTAX);
	wordfree(&w);
}

TEST(wordexp_cmd_empty) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("foo$()bar", &w, 0), 0);
	ASSERT_STR_EQ(w.we_wordv[0], "foobar");
	wordfree(&w);
}

TEST(wordexp_cmd_nested_quotes) {
	//TEST_SKIP("causes segfault");
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("$(echo \"a b\")", &w, 0), 0);
	ASSERT_STR_EQ(w.we_wordv[0], "a b");
	wordfree(&w);
}

/* ============================================================================ */
TEST_SUITE(wordexp_var_sad);

TEST(wordexp_undefined_var_with_undef) {
	unsetenv("UNDEFINED_VAR_XYZ");
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("$UNDEFINED_VAR_XYZ", &w, WRDE_UNDEF), WRDE_BADVAL);
	wordfree(&w);
}

TEST(wordexp_undefined_var_without_undef) {
	unsetenv("UNDEFINED_VAR_XYZ");
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("$UNDEFINED_VAR_XYZ", &w, 0), 0);
	ASSERT_INT_EQ(w.we_wordc, 1);
	ASSERT_STR_EQ(w.we_wordv[0], "");
	wordfree(&w);
}

TEST(wordexp_var_invalid_chars_stops) {
	setenv("VALID", "yes", 1);
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("$VALID!end", &w, 0), 0);
	ASSERT_INT_EQ(w.we_wordc, 1);
	ASSERT_STR_EQ(w.we_wordv[0], "yes!end");
	wordfree(&w);
	unsetenv("VALID");
}

/* ============================================================================ */
TEST_SUITE(wordexp_flags);

TEST(wordexp_empty_input) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("", &w, 0), 0);
	ASSERT_INT_EQ(w.we_wordc, 0);
	wordfree(&w);
}

TEST(wordexp_whitespace_only) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("   \t  ", &w, 0), 0);
	ASSERT_INT_EQ(w.we_wordc, 0);
	wordfree(&w);
}

TEST(wordexp_consecutive_spaces) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("a   b\t\tc", &w, 0), 0);
	ASSERT_INT_EQ(w.we_wordc, 3);
	ASSERT_STR_EQ(w.we_wordv[0], "a");
	ASSERT_STR_EQ(w.we_wordv[1], "b");
	ASSERT_STR_EQ(w.we_wordv[2], "c");
	wordfree(&w);
}

TEST(wordexp_leading_trailing_space) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("  trim  ", &w, 0), 0);
	ASSERT_INT_EQ(w.we_wordc, 1);
	ASSERT_STR_EQ(w.we_wordv[0], "trim");
	wordfree(&w);
}

TEST(wordexp_doofts_offset) {
	wordexp_t w = {.we_offs = 2};
	ASSERT_INT_EQ(wordexp("a b", &w, WRDE_DOOFFS), 0);
	ASSERT_INT_EQ(w.we_wordc, 4);
	ASSERT_NULL(w.we_wordv[0]);
	ASSERT_NULL(w.we_wordv[1]);
	ASSERT_STR_EQ(w.we_wordv[2], "a");
	ASSERT_STR_EQ(w.we_wordv[3], "b");
	wordfree(&w);
}

TEST(wordexp_doofts_without_offset) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("a", &w, WRDE_DOOFFS), 0);
	ASSERT_INT_EQ(w.we_wordc, 1);
	ASSERT_STR_EQ(w.we_wordv[0], "a");
	wordfree(&w);
}

TEST(wordexp_append) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("first", &w, 0), 0);
	ASSERT_INT_EQ(wordexp("second", &w, WRDE_APPEND), 0);
	ASSERT_INT_EQ(w.we_wordc, 2);
	ASSERT_STR_EQ(w.we_wordv[0], "first");
	ASSERT_STR_EQ(w.we_wordv[1], "second");
	wordfree(&w);
}

TEST(wordexp_append_null_init) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("x", &w, WRDE_APPEND), 0);
	ASSERT_STR_EQ(w.we_wordv[0], "x");
	wordfree(&w);
}

TEST(wordexp_reuse) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("old", &w, 0), 0);
	ASSERT_INT_EQ(wordexp("new", &w, WRDE_REUSE), 0);
	ASSERT_INT_EQ(w.we_wordc, 1);
	ASSERT_STR_EQ(w.we_wordv[0], "new");
	wordfree(&w);
}

TEST(wordexp_reuse_without_prior) {
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("y", &w, WRDE_REUSE), 0);
	ASSERT_STR_EQ(w.we_wordv[0], "y");
	wordfree(&w);
}

TEST(wordexp_flag_append_reuse_conflict) {
	wordexp_t w = {.we_wordc = 1, .we_wordv = malloc(2*sizeof(char*))};
	w.we_wordv[0] = strdup("old"); w.we_wordv[1] = NULL;
	ASSERT_INT_EQ(wordexp("new", &w, WRDE_APPEND | WRDE_REUSE), 0);
	ASSERT_STR_EQ(w.we_wordv[0], "new");
	wordfree(&w);
}

TEST(wordexp_flag_combinations) {
	wordexp_t w = {.we_offs = 1};
	ASSERT_INT_EQ(wordexp("a", &w, WRDE_DOOFFS | WRDE_APPEND), 0);
	ASSERT_INT_EQ(w.we_wordc, 2);
	ASSERT_NULL(w.we_wordv[0]);
	ASSERT_STR_EQ(w.we_wordv[1], "a");
	wordfree(&w);
}

/* ============================================================================ */
TEST_SUITE(wordexp_edge);

TEST(wordexp_large_word_count) {
	wordexp_t w = {0};
	char buf[1024] = {0};
	for (int i = 0; i < 100; i++) strcat(buf, "w ");
	ASSERT_INT_EQ(wordexp(buf, &w, 0), 0);
	ASSERT_INT_EQ(w.we_wordc, 100);
	wordfree(&w);
}

TEST(wordexp_long_word) {
	wordexp_t w = {0};
	char longw[512];
	memset(longw, 'x', 511);
	longw[511] = '\0';
	ASSERT_INT_EQ(wordexp(longw, &w, 0), 0);
	ASSERT_INT_EQ(strlen(w.we_wordv[0]), 511);
	wordfree(&w);
}

TEST(wordexp_home_unset) {
	unsetenv("HOME");
	wordexp_t w = {0};
	ASSERT_INT_EQ(wordexp("~/test", &w, 0), 0);
	ASSERT_STR_EQ(w.we_wordv[0], "~/test");
	wordfree(&w);
}

TEST(wordexp_arithmetic_sub_bad) {
	wordexp_t w = {0};
	int r = wordexp("$((1+1))", &w, WRDE_NOCMD);
	ASSERT_TRUE(r == WRDE_CMDSUB || r == WRDE_SYNTAX);
	wordfree(&w);
}

/* ============================================================================ */
TEST_MAIN()
