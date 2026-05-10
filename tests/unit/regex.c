/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <regex.h>
#include <locale.h>

TEST_TYPE(posix)
TEST_UNIT(regex.h)

#define ASSERT_REG_BUILD(re_ptr, pattern, flags) do { \
	int __rc = regcomp((re_ptr), (pattern), (flags)); \
	if (__rc != 0) { \
		char __errmsg[256]; \
		regerror(__rc, (re_ptr), __errmsg, sizeof(__errmsg)); \
		TEST_FAIL("regcomp('%s') failed: %s", (pattern), __errmsg); \
	} \
} while (0)

#define ASSERT_REG_MATCH(re_ptr, string, nmatch, pmatch, eflags) do { \
	int __rc = regexec((re_ptr), (string), (nmatch), (pmatch), (eflags)); \
	if (__rc != 0) { \
		char __errmsg[256]; \
		regerror(__rc, (re_ptr), __errmsg, sizeof(__errmsg)); \
		TEST_FAIL("regexec('%s') failed: %s", (string), __errmsg); \
	} \
} while (0)

#define ASSERT_REG_FAILS(re_ptr, string, nmatch, pmatch, eflags) do { \
	int __rc = regexec((re_ptr), (string), (nmatch), (pmatch), (eflags)); \
	if (__rc != REG_NOMATCH) { \
		char __errmsg[256]; \
		regerror(__rc, (re_ptr), __errmsg, sizeof(__errmsg)); \
		TEST_FAIL("regexec('%s') expected 'No match' but got: %s", (string), __errmsg); \
	} \
} while (0)

/* ============================================================================ */

TEST_SUITE(anchors)

TEST(anchors_start_of_string) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "^abc", REG_RE2);
	ASSERT_REG_MATCH(&re, "abcdef", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "xyzabc", 1, NULL, 0);
	regfree(&re);
}

TEST(anchors_start_fail_mid) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "^abc", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "defabc", 1, NULL, 0);
	regfree(&re);
}

TEST(anchors_end_of_string) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "xyz$", REG_RE2);
	ASSERT_REG_MATCH(&re, "abcdefxyz", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "xyzabc", 1, NULL, 0);
	regfree(&re);
}

TEST(anchors_end_fail_preceding) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "xyz$", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "abcdefxyzq", 1, NULL, 0);
	regfree(&re);
}

TEST(anchors_both_exact) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "^exact$", REG_RE2);
	ASSERT_REG_MATCH(&re, "exact", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, " exact", 1, NULL, 0);
	regfree(&re);
}

TEST(anchors_both_fail_extra) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "^exact$", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, " extra", 1, NULL, 0);
	regfree(&re);
}

TEST(anchors_notbol_flag) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "^foo", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "foo", 1, NULL, REG_NOTBOL);
	regfree(&re);
}

TEST(anchors_noteol_flag) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "bar$", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "bar", 1, NULL, REG_NOTEOL);
	regfree(&re);
}

TEST(anchors_newline_multiline_start) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "^mid", REG_NEWLINE | REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "start\nmid\nend", 1, NULL, 0);
	regfree(&re);
}

TEST(anchors_newline_multiline_end) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "end$", REG_NEWLINE | REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "start\nend\nrest", 1, NULL, 0);
	regfree(&re);
}

TEST(anchors_word_boundary_start) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\bword", REG_RE2);
	ASSERT_REG_MATCH(&re, "test word", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "tword", 1, NULL, 0);
	regfree(&re);
}

TEST(anchors_word_boundary_fail_internal) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\bword", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "tword", 1, NULL, 0);
	regfree(&re);
}

TEST(anchors_word_boundary_end) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "word\\b", REG_RE2);
	ASSERT_REG_MATCH(&re, "word.", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "wording", 1, NULL, 0);
	regfree(&re);
}

TEST(anchors_non_word_boundary) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "word\\Bend", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "wordending", 1, NULL, 0);
	regfree(&re);
}

TEST(anchors_non_word_boundary_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "word\\Bend", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "word end", 1, NULL, 0);
	regfree(&re);
}

TEST(anchors_word_boundary_locale_aware) {
	regex_t re;
	/* \b should respect locale word character definitions */
	setlocale(LC_CTYPE, "fr_FR.UTF-8");
	ASSERT_REG_BUILD(&re, "\\bcafé\\b", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "café", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "xcafé", 1, NULL, 0);
	regfree(&re);
	setlocale(LC_CTYPE, "C");
}

TEST(anchors_word_boundary_empty_string) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\b", REG_EXTENDED);
	/* Empty string has word boundary at position 0 */
	ASSERT_REG_MATCH(&re, "", 1, NULL, 0);
	regfree(&re);
}

TEST(anchors_word_boundary_only_nonword) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\b", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "!!!", 1, NULL, 0);
	regfree(&re);
}

TEST(anchors_multiline_multiple_matches) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "^line$", REG_NEWLINE | REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "start\nline\nend", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "start\nlines\nend", 1, NULL, 0);
	regfree(&re);
}

TEST(anchors_word_boundary_unicode) {
	TEST_SKIP("currently unsupported");
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\bcafé\\b", REG_RE2);
	ASSERT_REG_MATCH(&re, "café", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "xcafé", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "caféx", 1, NULL, 0);
	regfree(&re);
}

/* ============================================================================ */

TEST_SUITE(charsets)

TEST(charsets_single_char) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[a]", REG_RE2);
	ASSERT_REG_MATCH(&re, "a", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "b", 1, NULL, 0);
	regfree(&re);
}

TEST(charsets_single_char_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[a]", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "b", 1, NULL, 0);
	regfree(&re);
}

TEST(charsets_lowercase_range) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[a-z]", REG_RE2);
	ASSERT_REG_MATCH(&re, "m", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "M", 1, NULL, 0);
	regfree(&re);
}

TEST(charsets_uppercase_range) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[A-Z]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "Q", 1, NULL, 0);
	regfree(&re);
}

TEST(charsets_digit_range) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[0-9]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "7", 1, NULL, 0);
	regfree(&re);
}

TEST(charsets_multiple_ranges) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[a-fA-F0-9]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "c", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "B", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "3", 1, NULL, 0);
	regfree(&re);
}

TEST(charsets_exclusion_basic) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[^abc]", REG_RE2);
	ASSERT_REG_MATCH(&re, "d", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "a", 1, NULL, 0);
	regfree(&re);
}

TEST(charsets_exclusion_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[^abc]", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "a", 1, NULL, 0);
	regfree(&re);
}

TEST(charsets_exclusion_digit) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[^0-9]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "x", 1, NULL, 0);
	regfree(&re);
}

TEST(charsets_literal_dash_start) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[-a]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "-", 1, NULL, 0);
	regfree(&re);
}

TEST(charsets_literal_dash_end) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[a-]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "-", 1, NULL, 0);
	regfree(&re);
}

TEST(charsets_literal_bracket) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[\\[\\]]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "[", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "]", 1, NULL, 0);
	regfree(&re);
}

TEST(charsets_literal_caret) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[^\\^a]", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "^", 1, NULL, 0);
	regfree(&re);
}

TEST(charsets_adjacent_ranges_merge) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[a-cx-z]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "b", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "y", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "d", 1, NULL, 0);
	regfree(&re);
}

TEST(charsets_locale_range) {
	regex_t re;
	/* Ranges like [a-z] should respect locale collation */
	setlocale(LC_COLLATE, "fr_FR.UTF-8");
	ASSERT_REG_BUILD(&re, "[a-z]", REG_EXTENDED);
	/* In French locale, accented chars may sort within a-z range */
	ASSERT_REG_MATCH(&re, "m", 1, NULL, 0);
	/* This is locale-dependent - may or may not match é */
	regfree(&re);
	setlocale(LC_COLLATE, "C");
}

/* ============================================================================ */

TEST_SUITE(classes)

TEST(classes_alpha_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[[:alpha:]]", REG_RE2);
	ASSERT_REG_MATCH(&re, "z", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "4", 1, NULL, 0);
	regfree(&re);
}

TEST(classes_alpha_fail_digit) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[[:alpha:]]", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "4", 1, NULL, 0);
	regfree(&re);
}

TEST(classes_digit_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[[:digit:]]", REG_RE2);
	ASSERT_REG_MATCH(&re, "8", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "A", 1, NULL, 0);
	regfree(&re);
}

TEST(classes_digit_fail_alpha) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[[:digit:]]", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "A", 1, NULL, 0);
	regfree(&re);
}

TEST(classes_alnum_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[[:alnum:]]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "k9", 1, NULL, 0);
	regfree(&re);
}

TEST(classes_alnum_fail_punct) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[[:alnum:]]", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "@", 1, NULL, 0);
	regfree(&re);
}

TEST(classes_space_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[[:space:]]", REG_RE2);
	ASSERT_REG_MATCH(&re, "\t", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "\n", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, " ", 1, NULL, 0);
	regfree(&re);
}

TEST(classes_space_fail_alpha) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[[:space:]]", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "x", 1, NULL, 0);
	regfree(&re);
}

TEST(classes_upper_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[[:upper:]]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "M", 1, NULL, 0);
	regfree(&re);
}

TEST(classes_upper_fail_lower) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[[:upper:]]", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "m", 1, NULL, 0);
	regfree(&re);
}

TEST(classes_lower_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[[:lower:]]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "m", 1, NULL, 0);
	regfree(&re);
}

TEST(classes_lower_fail_upper) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[[:lower:]]", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "M", 1, NULL, 0);
	regfree(&re);
}

TEST(classes_negated_alpha) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[^[:alpha:]]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "5", 1, NULL, 0);
	regfree(&re);
}

TEST(classes_negated_alpha_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[^[:alpha:]]", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "A", 1, NULL, 0);
	regfree(&re);
}

TEST(classes_combined_alpha_digit) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[[:alpha:][:digit:]]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "z", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "3", 1, NULL, 0);
	regfree(&re);
}

TEST(classes_alpha_with_locale) {
    regex_t re;

    ASSERT_REG_BUILD(&re, "[[:alpha:]]", REG_EXTENDED);

    setlocale(LC_CTYPE, "C");
    ASSERT_REG_MATCH(&re, "A", 1, NULL, 0);

    setlocale(LC_CTYPE, "fr_FR");
    ASSERT_REG_MATCH(&re, "é", 1, NULL, 0);

    setlocale(LC_CTYPE, "es_MX");
    ASSERT_REG_MATCH(&re, "ñ", 1, NULL, 0);

    ASSERT_REG_FAILS(&re, "5", 1, NULL, 0);

    regfree(&re);
    setlocale(LC_CTYPE, "C");
}

TEST(classesalpha_without_locale) {
    regex_t re;
    setlocale(LC_CTYPE, "C");

    ASSERT_REG_BUILD(&re, "[[:alpha:]]", REG_EXTENDED);

    ASSERT_REG_MATCH(&re, "A", 1, NULL, 0);

    ASSERT_REG_FAILS(&re, "é", 1, NULL, 0);

    regfree(&re);
    setlocale(LC_CTYPE, "C");
}

TEST(classes_digit_with_locale) {
    regex_t re;
    setlocale(LC_CTYPE, "C");

    ASSERT_REG_BUILD(&re, "[[:digit:]]", REG_EXTENDED);

    ASSERT_REG_MATCH(&re, "0", 1, NULL, 0);
    ASSERT_REG_MATCH(&re, "9", 1, NULL, 0);
    ASSERT_REG_FAILS(&re, "a", 1, NULL, 0);

    regfree(&re);
    setlocale(LC_CTYPE, "C");
}

TEST(classes_collating_symbol_single) {
	regex_t re;
	/* [.ch.] in Swiss German locale - single char fallback */
	setlocale(LC_COLLATE, "C");
	ASSERT_REG_BUILD(&re, "[[.a.]]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "a", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "b", 1, NULL, 0);
	regfree(&re);
	setlocale(LC_COLLATE, "C");
}

TEST(classes_collating_symbol_multi_char_fail) {
	regex_t re;
	/* Multi-char collating elements require special handling */
	/* For now, verify it doesn't crash on parse */
	setlocale(LC_COLLATE, "C");
	int rc = regcomp(&re, "[[.ch.]]", REG_EXTENDED);
	/* Should either parse as literal [.ch.] or fail with REG_ECOLLATE */
	if (rc == 0) {
		/* If it parses, it should match literal '[' or ']' or fail gracefully */
		regfree(&re);
	} else {
		ASSERT_EQ(REG_ECOLLATE, rc);
	}
	setlocale(LC_COLLATE, "C");
}

TEST(classes_collating_symbol_unclosed) {
	regex_t re;
	int rc = regcomp(&re, "[[.a]", REG_EXTENDED);
	ASSERT_EQ(REG_ECOLLATE, rc);
}

TEST(classes_collating_symbol_empty) {
	regex_t re;
	int rc = regcomp(&re, "[[..]]", REG_EXTENDED);
	ASSERT_EQ(REG_ECOLLATE, rc);
}

TEST(classes_equivalence_basic) {
	regex_t re;
	/* [=e=] should match locale-equivalent characters */
	setlocale(LC_CTYPE, "fr_FR.UTF-8");
	ASSERT_REG_BUILD(&re, "[[=e=]]", REG_EXTENDED);
	/* In French locale, é, è, ê should be equivalent to e */
	ASSERT_REG_MATCH(&re, "e", 1, NULL, 0);
	/* These may match depending on locale implementation */
	/* ASSERT_REG_MATCH(&re, "é", 1, NULL, 0); */
	regfree(&re);
	setlocale(LC_CTYPE, "C");
}

TEST(classes_equivalence_negated) {
	regex_t re;
	setlocale(LC_CTYPE, "C");
	ASSERT_REG_BUILD(&re, "[^[=a=]]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "b", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "a", 1, NULL, 0);
	regfree(&re);
	setlocale(LC_CTYPE, "C");
}

TEST(classes_all_posix_names) {
	regex_t re;
	const char *classes[] = {
		"alnum", "alpha", "blank", "cntrl", "digit", "graph",
		"lower", "print", "punct", "space", "upper", "xdigit", NULL
	};
	setlocale(LC_CTYPE, "C");
	for (int i = 0; classes[i]; i++) {
		char pattern[64];
		snprintf(pattern, sizeof(pattern), "[[:%s:]]", classes[i]);
		ASSERT_REG_BUILD(&re, pattern, REG_EXTENDED);
		regfree(&re);
	}
	setlocale(LC_CTYPE, "C");
}

TEST(classes_invalid_name_error) {
	regex_t re;
	int rc = regcomp(&re, "[[:notaclass:]]", REG_EXTENDED);
	ASSERT_EQ(REG_ECTYPE, rc);
}

TEST(classes_invalid_name_empty) {
	regex_t re;
	int rc = regcomp(&re, "[[:]]", REG_EXTENDED);
	ASSERT_EQ(REG_ECTYPE, rc);
}

TEST(classes_invalid_name_partial) {
	regex_t re;
	int rc = regcomp(&re, "[[:alp]]", REG_EXTENDED);
	ASSERT_EQ(REG_ECTYPE, rc);
}

TEST(classes_invalid_name_typo) {
	regex_t re;
	int rc = regcomp(&re, "[[:alph]]", REG_EXTENDED);
	ASSERT_EQ(REG_ECTYPE, rc);
}

TEST(classes_invalid_name_uppercase) {
	regex_t re;
	int rc = regcomp(&re, "[[:ALPHA:]]", REG_EXTENDED);
	ASSERT_EQ(REG_ECTYPE, rc);
}

TEST(classes_equivalence_unclosed) {
	regex_t re;
	int rc = regcomp(&re, "[[=a]", REG_EXTENDED);
	ASSERT_EQ(REG_ECOLLATE, rc);
}

TEST(classes_equivalence_empty) {
	regex_t re;
	int rc = regcomp(&re, "[[==]]", REG_EXTENDED);
	ASSERT_EQ(REG_ECOLLATE, rc);
}

TEST(classes_locale_unavailable) {
	TEST_SKIP("currently unsupported");
	regex_t re;
	char *saved = setlocale(LC_CTYPE, NULL);
	/* Try invalid locale - should fall back to C */
	setlocale(LC_CTYPE, "invalid_locale_xyz");
	ASSERT_REG_BUILD(&re, "[[:alpha:]]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "a", 1, NULL, 0);
	regfree(&re);
	setlocale(LC_CTYPE, saved);
}

TEST(classes_nested_brackets) {
	regex_t re;
	/* [[ should not crash */
	int rc = regcomp(&re, "[[a]", REG_EXTENDED);
	/* Should either parse as literal '[' or fail cleanly */
	if (rc == 0) {
		regfree(&re);
	} else {
		ASSERT_TRUE(rc == REG_EBRACK || rc == REG_BADPAT);
	}
}

/* ============================================================================ */

TEST_SUITE(escapes)

TEST(escapes_digit_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\d", REG_RE2);
	ASSERT_REG_MATCH(&re, "7", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "q", 1, NULL, 0);
	regfree(&re);
}

TEST(escapes_digit_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\d", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "q", 1, NULL, 0);
	regfree(&re);
}

TEST(escapes_word_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\w", REG_RE2);
	ASSERT_REG_MATCH(&re, "_", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "-", 1, NULL, 0);
	regfree(&re);
}

TEST(escapes_word_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\w", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "-", 1, NULL, 0);
	regfree(&re);
}

TEST(escapes_space_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\s", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "\r", 1, NULL, 0);
	regfree(&re);
}

TEST(escapes_space_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\s", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "x", 1, NULL, 0);
	regfree(&re);
}

TEST(escapes_literal_dot) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\.txt", REG_RE2);
	ASSERT_REG_MATCH(&re, ".txt", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "atxt", 1, NULL, 0);
	regfree(&re);
}

TEST(escapes_literal_dot_fail_wildcard) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\.txt", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "atxt", 1, NULL, 0);
	regfree(&re);
}

TEST(escapes_literal_paren) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\(\\)", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "()", 1, NULL, 0);
	regfree(&re);
}

TEST(escapes_literal_bracket) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\[\\]", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "[]", 1, NULL, 0);
	regfree(&re);
}

TEST(escapes_literal_plus) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a\\+b", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "a+b", 1, NULL, 0);
	regfree(&re);
}

TEST(escapes_literal_star) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a\\*b", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "a*b", 1, NULL, 0);
	regfree(&re);
}

TEST(escapes_literal_question) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a\\?b", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "a?b", 1, NULL, 0);
	regfree(&re);
}

TEST(escapes_literal_caret) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\^a", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "^a", 1, NULL, 0);
	regfree(&re);
}

TEST(escapes_literal_dollar) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a\\$", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "a$", 1, NULL, 0);
	regfree(&re);
}

TEST(escapes_locale_aware_case_fold) {
	regex_t re;
	/* Case folding should respect LC_CTYPE */
	setlocale(LC_CTYPE, "fr_FR.UTF-8");
	ASSERT_REG_BUILD(&re, "(?i)café", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "CAFÉ", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "Café", 1, NULL, 0);
	regfree(&re);
	setlocale(LC_CTYPE, "C");
}

TEST(escapes_unknown_escape_error) {
	regex_t re;
	/* Unknown escapes should fail or be treated as literal */
	int rc = regcomp(&re, "\\z", REG_EXTENDED);
	/* Either REG_EESCAPE or parse as literal 'z' */
	if (rc == 0) {
		ASSERT_REG_MATCH(&re, "z", 1, NULL, 0);
		regfree(&re);
	} else {
		ASSERT_EQ(REG_EESCAPE, rc);
	}
}

/* ============================================================================ */

TEST_SUITE(alternants)

TEST(alternants_basic_match_first) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "cat|dog", REG_RE2);
	ASSERT_REG_MATCH(&re, "cat", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "dog", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "bird", 1, NULL, 0);
	regfree(&re);
}

TEST(alternants_basic_match_second) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "cat|dog", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "dog", 1, NULL, 0);
	regfree(&re);
}

TEST(alternants_basic_fail_none) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "cat|dog", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "bird", 1, NULL, 0);
	regfree(&re);
}

TEST(alternants_multiple_options) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "red|green|blue", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "green", 1, NULL, 0);
	regfree(&re);
}

TEST(alternants_precedence_literal_vs_quant) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "ab|cd", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "ab", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "cd", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "ac", 1, NULL, 0);
	regfree(&re);
}

TEST(alternants_precedence_quantifier_scope) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "ab*|cd", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "abb", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "a", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "cd", 1, NULL, 0);
	regfree(&re);
}

TEST(alternants_with_grouping) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(a|b)c", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "ac", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "bc", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "ad", 1, NULL, 0);
	regfree(&re);
}

TEST(alternants_with_grouping_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(a|b)c", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "ad", 1, NULL, 0);
	regfree(&re);
}

TEST(alternants_with_anchors) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "^start|end$", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "startline", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "lineend", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "midline", 1, NULL, 0);
	regfree(&re);
}

TEST(alternants_empty_first) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "|b", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "b", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "xb", 1, NULL, 0);
	regfree(&re);
}

TEST(alternants_nested) {
	regex_t re;
	/* Pattern: a followed by (b OR c followed by (d OR e)) followed by f */
	ASSERT_REG_BUILD(&re, "a(b|c(d|e))f", REG_EXTENDED);

	/* Valid matches */
	ASSERT_REG_MATCH(&re, "abf", 1, NULL, 0);    /* Matches b */
	ASSERT_REG_MATCH(&re, "acdf", 1, NULL, 0);   /* Matches c then d */
	ASSERT_REG_MATCH(&re, "acef", 1, NULL, 0);   /* Matches c then e */

	/* Optional: Verify invalid inputs fail */
	ASSERT_REG_FAILS(&re, "acf", 1, NULL, 0);    /* Missing d or e after c */
	ASSERT_REG_FAILS(&re, "aef", 1, NULL, 0);    /* Missing b or c */

	regfree(&re);
}

/* ============================================================================ */

TEST_SUITE(captures)

TEST(captures_basic_match) {
	regex_t re;
	regmatch_t p[2];
	ASSERT_REG_BUILD(&re, "(foo)", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "foo", 2, p, 0);
	ASSERT_EQ(0, p[1].rm_so);
	ASSERT_EQ(3, p[1].rm_eo);
	regfree(&re);
}

TEST(captures_multiple_groups) {
	regex_t re;
	regmatch_t p[4];
	ASSERT_REG_BUILD(&re, "(a)(b)(c)", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "abc", 4, p, 0);
	ASSERT_EQ(0, p[1].rm_so);
	ASSERT_EQ(1, p[1].rm_eo);
	ASSERT_EQ(1, p[2].rm_so);
	ASSERT_EQ(2, p[2].rm_eo);
	ASSERT_EQ(2, p[3].rm_so);
	ASSERT_EQ(3, p[3].rm_eo);
	regfree(&re);
}

TEST(captures_nested_match) {
	regex_t re;
	regmatch_t p[3];
	ASSERT_REG_BUILD(&re, "((ab))", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "ab", 3, p, 0);
	ASSERT_EQ(0, p[1].rm_so);
	ASSERT_EQ(2, p[1].rm_eo);
	ASSERT_EQ(0, p[2].rm_so);
	ASSERT_EQ(2, p[2].rm_eo);
	regfree(&re);
}

TEST(captures_with_quantifier) {
	regex_t re;
	regmatch_t p[2];
	ASSERT_REG_BUILD(&re, "(ab)+", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "abab", 2, p, 0);
	ASSERT_EQ(2, p[1].rm_so);
	ASSERT_EQ(4, p[1].rm_eo);
	regfree(&re);
}

TEST(captures_optional_present) {
	regex_t re;
	regmatch_t p[2];
	ASSERT_REG_BUILD(&re, "a(b)?c", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "abc", 2, p, 0);
	ASSERT_EQ(1, p[1].rm_so);
	ASSERT_EQ(2, p[1].rm_eo);
	regfree(&re);
}

TEST(captures_optional_absent) {
	regex_t re;
	regmatch_t p[2];
	ASSERT_REG_BUILD(&re, "a(b)?c", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "ac", 2, p, 0);
	ASSERT_EQ(-1, p[1].rm_so);
	ASSERT_EQ(-1, p[1].rm_eo);
	regfree(&re);
}

TEST(captures_unused_branch) {
	regex_t re;
	regmatch_t p[3];
	ASSERT_REG_BUILD(&re, "(a)|(b)", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "a", 3, p, 0);
	ASSERT_EQ(0, p[1].rm_so);
	ASSERT_EQ(-1, p[2].rm_so);
	regfree(&re);
}

TEST(captures_zero_length_match) {
	regex_t re;
	regmatch_t p[2];
	ASSERT_REG_BUILD(&re, "()a", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "a", 2, p, 0);
	ASSERT_EQ(0, p[1].rm_so);
	ASSERT_EQ(0, p[1].rm_eo);
	regfree(&re);
}

TEST(captures_star_repetition) {
	regex_t re;
	regmatch_t p[2];
	ASSERT_REG_BUILD(&re, "(a)*", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "aaa", 2, p, 0);
	ASSERT_EQ(2, p[1].rm_so);
	ASSERT_EQ(3, p[1].rm_eo);
	regfree(&re);
}

TEST(captures_branch_reset) {
	regex_t re;
	regmatch_t p[2];
	/* In branch reset, capture indices are reset for each alternative */
	/* Group 1 captures either 'a' or 'b' */
	ASSERT_REG_BUILD(&re, "(?|(a)|(b))c", REG_PCRE2);

	ASSERT_REG_MATCH(&re, "ac", 2, p, 0);
	ASSERT_EQ(0, p[1].rm_so); /* 'a' matched at 0 */

	ASSERT_REG_MATCH(&re, "bc", 2, p, 0);
	ASSERT_EQ(0, p[1].rm_so); /* 'b' matched at 0 (would be group 2 without branch reset) */
	regfree(&re);
}

TEST(captures_atomic_group_match) {
	regex_t re;
	/* (?>a) matches 'a' but won't backtrack */
	ASSERT_REG_BUILD(&re, "(?>a)", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "a", 1, NULL, 0);
	regfree(&re);
}

TEST(captures_atomic_prevent_backtrack) {
	regex_t re;
	/* (?>a*) matches all 'a's. It refuses to give one back to match the trailing 'a'. */
	/* Result: FAIL */
	ASSERT_REG_BUILD(&re, "(?>a*)a", REG_PCRE2);
	ASSERT_REG_FAILS(&re, "aaaa", 1, NULL, 0);
	regfree(&re);
}

TEST(capture_named_groups) {
	regex_t re;
	regmatch_t p[2];
	/* Named group syntax */
	ASSERT_REG_BUILD(&re, "(?<word>\\w+)", REG_PCRE2);

	/* Should match "hello" */
	ASSERT_REG_MATCH(&re, "hello", 2, p, 0);

	/* Since 'word' is the first group, it should be at index 1 */
	ASSERT_EQ(0, p[1].rm_so);
	ASSERT_EQ(5, p[1].rm_eo);
	regfree(&re);
}

TEST(captures_named_backref) {
	regex_t re;
	/* Backreference by name using \k<name> */
	ASSERT_REG_BUILD(&re, "(?<char>.)\\k<char>", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "AA", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "  ", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "AB", 1, NULL, 0);
	regfree(&re);
}

TEST(captures_conditional_basic) {
	regex_t re;
	/* If group 1 (a) matched, then match 'b', else match 'c'. */
	/* Input "ab": Group 1 matches 'a'. Condition true. Match 'b'. Success. */
	ASSERT_REG_BUILD(&re, "(a)?(?(1)b|c)", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "ab", 1, NULL, 0);
	regfree(&re);
}

TEST(captures_conditional_alternatives) {
	regex_t re;
	/* Input "ac": Group 1 (a) fails (optional). Condition false. Match 'c'. Success. */
	/* Wait, if (a) is optional and fails, the input is just "ac". */
	/* Regex: (a)? -> fails to match 'a' (consumes nothing). Current pos: start of "ac". */
	/* (?(1)b|c) -> Group 1 didn't match. Try 'c'. Matches 'a'? No. Fails. */

	/* Better Test: a(b)?(?(1)c|d) */
	/* Input "abc": a matches. (b) matches 'b'. Cond(1) true -> match 'c'. "abc" matches. */
	/* Input "ad" : a matches. (b) fails. Cond(1) false -> match 'd'. "ad" matches. */

	ASSERT_REG_BUILD(&re, "a(b)?(?(1)c|d)", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "abc", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "ad", 1, NULL, 0);
	regfree(&re);
}

/* ============================================================================ */

TEST_SUITE(repeats)

TEST(repeats_star_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a*", REG_RE2);
	ASSERT_REG_MATCH(&re, "aaa", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_star_empty) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a*", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "bbb", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_plus_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a+", REG_RE2);
	ASSERT_REG_MATCH(&re, "aa", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_plus_fail_empty) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a+", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "bbb", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_question_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a?b", REG_RE2);
	ASSERT_REG_MATCH(&re, "ab", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "b", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_question_match_present) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a?b", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "ab", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_question_match_absent) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a?b", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "b", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_exact_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a{3}", REG_RE2);
	ASSERT_REG_MATCH(&re, "aaa", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "aa", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_exact_fail_low) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a{3}", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "aa", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_exact_fail_high) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "^a{3}$", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "aaaa", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_min_open_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a{2,}", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "aaaaa", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_min_open_fail_low) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a{2,}", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "a", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_range_match_mid) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a{2,4}", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "aaa", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_range_fail_low) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a{2,4}", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "a", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_range_fail_high) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "^a{2,4}$", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "aaaaa", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_backtracking_giveback) {
	regex_t re;
	regmatch_t p[2];
	ASSERT_REG_BUILD(&re, "(a+)b", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "aab", 2, p, 0);
	ASSERT_EQ(0, p[1].rm_so);
	ASSERT_EQ(2, p[1].rm_eo);
	regfree(&re);
}

TEST(repeats_backtracking_complex) {
	regex_t re;
	regmatch_t p[2];
	ASSERT_REG_BUILD(&re, "a*b", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "aaab", 2, p, 0);
	ASSERT_EQ(0, p[0].rm_so);
	ASSERT_EQ(4, p[0].rm_eo);
	regfree(&re);
}

TEST(repeats_classes_repetition) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "[0-9]+", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "12345", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_zero_times_exact) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a{0}", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "bbb", 1, NULL, 0);
	regfree(&re);
}

TEST(repeats_greedy_vs_lazy_same_input) {
	regex_t re_greedy, re_lazy;
	ASSERT_REG_BUILD(&re_greedy, "a+b", REG_EXTENDED);
	ASSERT_REG_BUILD(&re_lazy, "a+?b", REG_PCRE2);
	ASSERT_REG_MATCH(&re_greedy, "ab", 1, NULL, 0);
	ASSERT_REG_MATCH(&re_lazy, "ab", 1, NULL, 0);
	regfree(&re_greedy); regfree(&re_lazy);
}

TEST(repeats_zero_width_quantifier) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a*", REG_EXTENDED);
	regmatch_t p[1];
	ASSERT_REG_MATCH(&re, "b", 1, p, 0);
	ASSERT_EQ(0, p[0].rm_so);
	ASSERT_EQ(0, p[0].rm_eo);
	regfree(&re);
}

/* ============================================================================ */

TEST_SUITE(nesting)

TEST(nesting_depth_two) {
	regex_t re;
	regmatch_t p[3];
	ASSERT_REG_BUILD(&re, "((ab))", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "ab", 3, p, 0);
	regfree(&re);
}

TEST(nesting_depth_five) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(((((a)))))", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "a", 1, NULL, 0);
	regfree(&re);
}

TEST(nesting_quantified_group) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(ab)+", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "ababab", 1, NULL, 0);
	regfree(&re);
}

TEST(nesting_alternation_inside) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a(b|c)d", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "abd", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "acd", 1, NULL, 0);
	regfree(&re);
}

TEST(nesting_repetition_inside) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(a(b)*)c", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "ac", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "abc", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "abbc", 1, NULL, 0);
	regfree(&re);
}

TEST(nesting_mixed_quantifiers) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(a+)(b*)", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "aa", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "aaabbb", 1, NULL, 0);
	regfree(&re);
}

TEST(nesting_unbalanced_paren_fail) {
	regex_t re;
	int rc = regcomp(&re, "((a)", REG_EXTENDED);
	ASSERT_EQ(REG_EPAREN, rc);
}

/* ============================================================================ */

TEST_SUITE(quantifiers)

TEST(quntifiers_ungreedy_any) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a*?", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "aaa", 1, NULL, 0);
	regfree(&re);
}

TEST(quantifiers_ungreedy_more) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a+?", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "aaa", 1, NULL, 0);
	regfree(&re);
}

TEST(quantifiers_ungreedy_maybe) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a??", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "a", 1, NULL, 0);
	regfree(&re);
}

TEST(quantifiers_ungreedy_range) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a{2,5}?", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "aaaaa", 1, NULL, 0);
	regfree(&re);
}

TEST(quantifiers_ungreedy_with_literal) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a*?b", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "aaab", 1, NULL, 0);
	regfree(&re);
}

TEST(quantifiers_ungreedy_vs_greedy) {
	regex_t re;
	regmatch_t pg[2];
	regmatch_t pl[2];
	ASSERT_REG_BUILD(&re, "(a+)", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "aaab", 2, pg, 0);
	ASSERT_REG_BUILD(&re, "(a+?)", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "aaab", 2, pl, 0);
	ASSERT_EQ(3, pg[1].rm_eo);
	ASSERT_EQ(1, pl[1].rm_eo);
	regfree(&re);
}

TEST(quantifiers_greedy_any) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a*+", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "aaa", 1, NULL, 0);
	regfree(&re);
}

TEST(quantifiers_greedy_prevent_backtrack) {
	regex_t re;
	/* a++ consumes all 'a's. Fails to find trailing 'a'. */
	ASSERT_REG_BUILD(&re, "a++a", REG_PCRE2);
	ASSERT_REG_FAILS(&re, "aaaa", 1, NULL, 0);
	regfree(&re);
}

TEST(quantifiers_greedy_with_literal) {
	regex_t re;
	/* a++ consumes 'aaa', then 'b' matches 'b'. Success. */
	ASSERT_REG_BUILD(&re, "a++b", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "aaab", 1, NULL, 0);
	regfree(&re);
}

/* ============================================================================ */

TEST_SUITE(lookahead)

TEST(lookahead_positive_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "foo(?=bar)", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "foobar", 1, NULL, 0);
	regfree(&re);
}

TEST(lookahead_positive_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "foo(?=bar)", REG_PCRE2);
	ASSERT_REG_FAILS(&re, "foobaz", 1, NULL, 0);
	regfree(&re);
}

TEST(lookahead_negative_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "foo(?!bar)", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "foobaz", 1, NULL, 0);
	regfree(&re);
}

TEST(lookahead_negative_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "foo(?!bar)", REG_PCRE2);
	ASSERT_REG_FAILS(&re, "foobar", 1, NULL, 0);
	regfree(&re);
}

TEST(lookahead_with_quantifier) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a(?=b{2,3}c)", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "abbbc", 1, NULL, 0);
	regfree(&re);
}

TEST(lookahead_at_boundary) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(?=^abc)abc", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "abc", 1, NULL, 0);
	regfree(&re);
}

/* ============================================================================ */

TEST_SUITE(lookbehind)

TEST(lookbehind_positive_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(?<=foo)bar", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "foobar", 1, NULL, 0);
	regfree(&re);
}

TEST(lookbehind_positive_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(?<=foo)bar", REG_PCRE2);
	ASSERT_REG_FAILS(&re, "bazbar", 1, NULL, 0);
	regfree(&re);
}

TEST(lookbehind_negative_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(?<!foo)bar", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "bazbar", 1, NULL, 0);
	regfree(&re);
}

TEST(lookbehind_negative_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(?<!foo)bar", REG_PCRE2);
	ASSERT_REG_FAILS(&re, "foobar", 1, NULL, 0);
	regfree(&re);
}

/* ============================================================================ */

TEST_SUITE(recursion)

TEST(recursion_basic) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\((?:[^()]|(?R))*\\)", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "(abc)", 1, NULL, 0);
	regfree(&re);
}

TEST(recursion_nested) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\((?:[^()]|(?R))*\\)", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "(a(b)c)", 1, NULL, 0);
	regfree(&re);
}

TEST(recursion_unbalanced_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\((?:[^()]|(?R))*\\)", REG_PCRE2);
	ASSERT_REG_FAILS(&re, "(unclosed", 1, NULL, 0);
	regfree(&re);
}

TEST(recursion_empty_content) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\((?:[^()]|(?R))*\\)", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "()", 1, NULL, 0);
	regfree(&re);
}

TEST(recursion_call_by_number) {
	regex_t re;
	/* (?1) calls the first subpattern. */
	/* Matches balanced parentheses with content */
	/* Pattern: \( (?: [^()] | (?1) )* \) */
	/* Wait, standard recursion usually wraps the whole thing or specific groups. */
	/* Let's use the example from PCRE2 docs: \( ( (?>[^()]+) | (?1) )* \) */
	ASSERT_REG_BUILD(&re, "\\((?:[^()]++|(?1))*\\)", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "(foo)", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "(foo(bar)baz)", 1, NULL, 0);
	regfree(&re);
}

TEST(recursion_call_by_name) {
	//TEST_SKIP("causes segfault");
	regex_t re;
	regmatch_t p[2];
	ASSERT_REG_BUILD(&re, "(?<paren>\\((?:[^()]++|(?&paren))*\\))", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "(nested (stuff) here)", 2, p, 0);
	regfree(&re);
}

/* ============================================================================ */

TEST_SUITE(backrefs)

TEST(backrefs_basic_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\(a\\)\\1", REG_BASIC);
	ASSERT_REG_MATCH(&re, "aa", 1, NULL, 0);
	regfree(&re);
}

TEST(backrefs_basic_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\(a\\)\\1", REG_BASIC);
	ASSERT_REG_FAILS(&re, "ab", 1, NULL, 0);
	regfree(&re);
}

TEST(backrefs_multiple) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\(\\w\\)\\(\\w\\)\\2\\1", REG_BASIC);
	ASSERT_REG_MATCH(&re, "abba", 1, NULL, 0);
	regfree(&re);
}

TEST(backrefs_multiple_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(\\w)(\\w)\\2\\1", REG_PCRE2);
	ASSERT_REG_FAILS(&re, "abcd", 1, NULL, 0);
	regfree(&re);
}

TEST(backrefs_with_quantifier) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\(a\\)\\1\\+", REG_BASIC);
	ASSERT_REG_MATCH(&re, "aaaa", 1, NULL, 0);
	regfree(&re);
}

TEST(backrefs_with_quantifier_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(a+)\\1", REG_PCRE2);
	ASSERT_REG_FAILS(&re, "aaa", 1, NULL, 0);
	regfree(&re);
}

TEST(backrefs_alternation) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\(a\\|b\\)\\1", REG_BASIC);
	ASSERT_REG_MATCH(&re, "aa", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "bb", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "ab", 1, NULL, 0);
	regfree(&re);
}

/* ============================================================================ */

TEST_SUITE(empty)

TEST(empty_pattern_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "", REG_RE2);
	ASSERT_REG_MATCH(&re, "", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "abc", 1, NULL, 0);
	regfree(&re);
}

TEST(empty_star_match_empty) {
	regex_t re;
	regmatch_t p[1];
	ASSERT_REG_BUILD(&re, "a*", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "", 1, p, 0);
	ASSERT_EQ(0, p[0].rm_so);
	ASSERT_EQ(0, p[0].rm_eo);
	regfree(&re);
}

TEST(empty_plus_fail_empty) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a+", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "", 1, NULL, 0);
	regfree(&re);
}

TEST(empty_question_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a?", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "", 1, NULL, 0);
	regfree(&re);
}

TEST(empty_anchors_both) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "^$", REG_RE2);
	ASSERT_REG_MATCH(&re, "", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, " ", 1, NULL, 0);
	regfree(&re);
}

TEST(empty_anchors_both_fail_space) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "^$", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, " ", 1, NULL, 0);
	regfree(&re);
}

TEST(empty_mid_string_match) {
	regex_t re;
	regmatch_t p[1];
	ASSERT_REG_BUILD(&re, "b*", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "ccc", 1, p, 0);
	ASSERT_EQ(0, p[0].rm_so);
	ASSERT_EQ(0, p[0].rm_eo);
	regfree(&re);
}

TEST(empty_zero_capture) {
	regex_t re;
	regmatch_t p[2];
	ASSERT_REG_BUILD(&re, "()a", REG_EXTENDED);
	ASSERT_REG_MATCH(&re, "a", 2, p, 0);
	ASSERT_EQ(0, p[1].rm_so);
	ASSERT_EQ(0, p[1].rm_eo);
	regfree(&re);
}

TEST(empty_match_null_string) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a*", REG_RE2);
	/* NULL subject should fail gracefully */
	int rc = regexec(&re, NULL, 0, NULL, 0);
	ASSERT_EQ(REG_BADPAT, rc);
	regfree(&re);
}

/* ============================================================================ */

TEST_SUITE(flags)

TEST(flags_inline_icase) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(?i)abc", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "ABC", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "aBc", 1, NULL, 0);
	regfree(&re);
}

TEST(flags_scoped_icase) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "A(?i:b)C", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "ABC", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "AbC", 1, NULL, 0);
	ASSERT_REG_FAILS(&re, "abC", 1, NULL, 0);
	regfree(&re);
}

TEST(flags_inline_dotall) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(?s)a.b", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "a\nb", 1, NULL, 0);
	regfree(&re);
}

TEST(flags_dotall_default_fail) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "a.b", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "a\nb", 1, NULL, 0);
	regfree(&re);
}

TEST(flags_runtime_icase) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "abc", REG_EXTENDED | REG_ICASE);
	ASSERT_REG_MATCH(&re, "ABC", 1, NULL, 0);
	regfree(&re);
}

TEST(flags_runtime_newline) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "^line$", REG_EXTENDED | REG_NEWLINE);
	ASSERT_REG_MATCH(&re, "top\nline\nbot", 1, NULL, 0);
	regfree(&re);
}

TEST(flags_runtime_notbol) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "^start", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "start", 1, NULL, REG_NOTBOL);
	regfree(&re);
}

TEST(flags_runtime_noteol) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "end$", REG_EXTENDED);
	ASSERT_REG_FAILS(&re, "end", 1, NULL, REG_NOTEOL);
	regfree(&re);
}

TEST(flags_nosub_skip_match) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(a)(b)", REG_EXTENDED | REG_NOSUB);
	ASSERT_REG_MATCH(&re, "ab", 0, NULL, 0);
	regfree(&re);
}

TEST(flags_nosub_no_captures) {
	regex_t re;
	regmatch_t p[3];
	ASSERT_REG_BUILD(&re, "(a)(b)", REG_EXTENDED | REG_NOSUB);
	/* With REG_NOSUB, pmatch is ignored but should not crash */
	ASSERT_REG_MATCH(&re, "ab", 3, p, 0);
	/* p[0] may be set, but p[1], p[2] should be undefined - just verify no crash */
	regfree(&re);
}

TEST(flags_nosub_with_zero_nmatch) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(test)", REG_EXTENDED | REG_NOSUB);
	/* nmatch=0 should work with REG_NOSUB */
	ASSERT_REG_MATCH(&re, "test", 0, NULL, 0);
	regfree(&re);
}

TEST(flags_locale_change_after_compile) {
	regex_t re;
	setlocale(LC_CTYPE, "C");
	ASSERT_REG_BUILD(&re, "(?i)abc", REG_PCRE2);
	/* Change locale after regcomp */
	setlocale(LC_CTYPE, "fr_FR.UTF-8");
	/* Should still work (locale captured at compile or runtime) */
	ASSERT_REG_MATCH(&re, "ABC", 1, NULL, 0);
	regfree(&re);
	setlocale(LC_CTYPE, "C");
}

TEST(flags_nosub_with_captures_requested) {
	regex_t re;
	regmatch_t p[3];
	ASSERT_REG_BUILD(&re, "(a)(b)", REG_EXTENDED | REG_NOSUB);
	/* Requesting captures with REG_NOSUB should not crash */
	int rc = regexec(&re, "ab", 3, p, 0);
	ASSERT_EQ(0, rc);
	/* p[1], p[2] are undefined but should not crash */
	regfree(&re);
}

TEST(flags_runtime_icase_override) {
	regex_t re;
	/* Compile WITHOUT ICASE */
	ASSERT_REG_BUILD(&re, "abc", REG_EXTENDED);
	/* Match fails without flag */
	ASSERT_REG_FAILS(&re, "ABC", 1, NULL, 0);
	/* Match succeeds with runtime ICASE */
	ASSERT_REG_MATCH(&re, "ABC", 1, NULL, REG_ICASE);
	regfree(&re);
}

TEST(flags_runtime_notbol_with_newline) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "^test", REG_NEWLINE | REG_EXTENDED);
	/* ^ matches after \n by default with REG_NEWLINE */
	ASSERT_REG_MATCH(&re, "line\ntest", 1, NULL, 0);
	/* REG_NOTBOL should disable ^ matching after \n too */
	ASSERT_REG_FAILS(&re, "line\ntest", 1, NULL, REG_NOTBOL);
	regfree(&re);
}

/* ============================================================================ */

TEST_SUITE(unicode)

TEST(unicode_general_category_letter) {
	TEST_SKIP("currently unsupported");
	regex_t re;
	/* \p{L} matches any kind of letter from any language */
	ASSERT_REG_BUILD(&re, "\\p{L}+", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "Hello", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "你好", 1, NULL, 0);
	regfree(&re);
}

TEST(unicode_general_category_number) {
	TEST_SKIP("currently unsupported");
	regex_t re;
	/* \p{N} matches any kind of numeric character */
	ASSERT_REG_BUILD(&re, "\\p{N}+", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "123", 1, NULL, 0);
	/* Arabic-Indic digits etc should also match */
	regfree(&re);
}

TEST(unicode_script_greek) {
	TEST_SKIP("currently unsupported");
	regex_t re;
	/* \p{Greek} matches characters in the Greek script */
	ASSERT_REG_BUILD(&re, "\\p{Greek}", REG_PCRE2);
	ASSERT_REG_MATCH(&re, "Ω", 1, NULL, 0); /* Omega */
	ASSERT_REG_FAILS(&re, "A", 1, NULL, 0); /* Latin A is not Greek */
	regfree(&re);
}

TEST(unicode_literal) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "caf\xC3\xA9", REG_RE2);
	ASSERT_REG_MATCH(&re, "caf\xC3\xA9", 1, NULL, 0);
	regfree(&re);
}

TEST(unicode_word_char) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\w+", REG_RE2);
	ASSERT_REG_MATCH(&re, "hello", 1, NULL, 0);
	ASSERT_REG_MATCH(&re, "caf\xC3\xA9", 1, NULL, 0);
	regfree(&re);
}

TEST(unicode_mixed_scripts) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\w+", REG_RE2);
	/* Mixed Latin + CJK + emoji */
	ASSERT_REG_MATCH(&re, "Hello世界🌍", 1, NULL, 0);
	regfree(&re);
}

TEST(unicode_normalization_nfc) {
	regex_t re;
	/* NFC form: é as single codepoint U+00E9 */
	ASSERT_REG_BUILD(&re, "caf\xC3\xA9", REG_RE2);
	ASSERT_REG_MATCH(&re, "caf\xC3\xA9", 1, NULL, 0);
	regfree(&re);
}

TEST(unicode_case_fold_turkish) {
	TEST_SKIP("currently unsupported");
	regex_t re;
	setlocale(LC_CTYPE, "tr_TR.UTF-8");
	ASSERT_REG_BUILD(&re, "(?i)istanbul", REG_PCRE2);
	/* Turkish: I (U+0049) lowercases to ı (U+0131), not i */
	ASSERT_REG_MATCH(&re, "İstanbul", 1, NULL, 0);
	regfree(&re);
	setlocale(LC_CTYPE, "C");
}

/* ============================================================================ */

TEST_SUITE(errors)

TEST(errors_compile_invalid) {
	regex_t re;
	int rc = regcomp(&re, "[unclosed", REG_RE2);
	ASSERT_EQ(REG_EBRACK, rc);
}

TEST(errors_null_pattern) {
	regex_t re;
	int rc = regcomp(&re, NULL, REG_RE2);
	ASSERT_EQ(REG_BADPAT, rc);
}

TEST(errors_recursion_limit) {
	TEST_SKIP("currently unsupported");
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\((?:[^()]|(?R))*\\)", REG_PCRE2);
	/* Very deep nesting - should fail with REG_EDEPTH */
	char deep[4000] = {0};
	for(int i=0; i<2000; i++) deep[i] = '(';
	for(int i=2000; i<4000; i++) deep[i] = ')';
	int rc = regexec(&re, deep, 0, NULL, 0);
	ASSERT_TRUE(rc == REG_NOMATCH || rc == REG_EDEPTH);
	regfree(&re);
}

TEST(errors_pattern_too_long) {
	regex_t re;
	char pat[8192] = {0};
	for(int i=0; i<8191; i++) pat[i] = 'a';
	int rc = regcomp(&re, pat, REG_EXTENDED);
	/* Should compile or fail with REG_ESPACE */
	ASSERT_TRUE(rc == 0 || rc == REG_ESPACE);
	if (rc == 0) regfree(&re);
}

TEST(errors_regerror_buffer_small) {
	regex_t re;
	regcomp(&re, "[invalid", REG_EXTENDED);
	char tiny[5];
	size_t len = regerror(REG_EBRACK, &re, tiny, sizeof(tiny));
	/* Should return full length even if buffer too small */
	ASSERT_TRUE(len > sizeof(tiny));
	ASSERT_TRUE(tiny[sizeof(tiny)-1] == '\0');
	regfree(&re);
}

TEST(errors_regerror_null_buffer) {
	regex_t re;
	regcomp(&re, "[invalid", REG_EXTENDED);
	/* NULL buffer should return length without crashing */
	size_t len = regerror(REG_EBRACK, &re, NULL, 0);
	ASSERT_TRUE(len > 0);
	regfree(&re);
}

TEST(errors_regerror_unknown_code) {
	regex_t re;
	/* Unknown error code should return "Unknown error" */
	char msg[256];
	size_t len = regerror(999, &re, msg, sizeof(msg));
	ASSERT_STR_EQ("Unknown error", msg);
	/* Should also work with NULL buffer */
	len = regerror(999, &re, NULL, 0);
	ASSERT_TRUE(len > 0);
}

TEST(errors_recursion_limit_exists) {
	regex_t re;
	/* Pattern that recurses: balanced parens */
	ASSERT_REG_BUILD(&re, "\\((?:[^()]|(?R))*\\)", REG_PCRE2);

	/* Shallow nesting should work */
	ASSERT_REG_MATCH(&re, "((((()))))", 1, NULL, 0);

	/* Medium nesting should work */
	char medium[200] = {0};
	for(int i=0; i<100; i++) medium[i] = '(';
	for(int i=100; i<200; i++) medium[i] = ')';
	ASSERT_REG_MATCH(&re, medium, 1, NULL, 0);

	regfree(&re);
}

TEST(errors_recursion_limit_enforced) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "\\((?:[^()]|(?R))*\\)", REG_PCRE2);

	char deep[5000] = {0};
	for(int i=0; i<2500; i++) deep[i] = '(';
	for(int i=2500; i<5000; i++) deep[i] = ')';

	int rc = regexec(&re, deep, 0, NULL, 0);
	ASSERT_EQ(rc, REG_EDEPTH);

	regfree(&re);
}

TEST(errors_recursion_no_crash) {
	regex_t re;
	ASSERT_REG_BUILD(&re, "(?R)", REG_PCRE2);

	int rc = regexec(&re, "test", 0, NULL, 0);
	ASSERT_EQ(rc, REG_EDEPTH);

	regfree(&re);
}

/* ============================================================================ */

TEST_MAIN()
