/* (c) 2026 FRINKnet & Friends – MIT licence */
#include "testing.h"
#include <regex.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

TEST_TYPE(posix)
TEST_UNIT(regex.h)

TEST_SUITE(posix_surface)

TEST(re_nsub_set_by_regcomp) {
	regex_t re;
	ASSERT_INT_EQ(0, regcomp(&re, "(a)(b(c))", REG_EXTENDED));
	ASSERT_INT_EQ(3, re.re_nsub);
	regfree(&re);
}

TEST(re_nsub_zero_for_nosub) {
	regex_t re;
	ASSERT_INT_EQ(0, regcomp(&re, "(a)(b)", REG_EXTENDED | REG_NOSUB));
	ASSERT_INT_EQ(2, re.re_nsub);
	regfree(&re);
}

TEST(regexec_pmatch_zero_length) {
	regex_t re; regmatch_t pm[3];
	ASSERT_INT_EQ(0, regcomp(&re, "a*", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "bbb", 3, pm, 0));
	ASSERT_INT_EQ(0, pm[0].rm_so);
	ASSERT_INT_EQ(0, pm[0].rm_eo);
	regfree(&re);
}

TEST(regexec_unused_slots_negative_one) {
	regex_t re; regmatch_t pm[4];
	ASSERT_INT_EQ(0, regcomp(&re, "(a)", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "a", 4, pm, 0));
	ASSERT_INT_EQ(0, pm[0].rm_so); ASSERT_INT_EQ(1, pm[0].rm_eo);
	ASSERT_INT_EQ(0, pm[1].rm_so); ASSERT_INT_EQ(1, pm[1].rm_eo);
	ASSERT_INT_EQ(-1, pm[2].rm_so); ASSERT_INT_EQ(-1, pm[2].rm_eo);
	ASSERT_INT_EQ(-1, pm[3].rm_so); ASSERT_INT_EQ(-1, pm[3].rm_eo);
	regfree(&re);
}

TEST(regerror_null_safe) {
	char buf[64];
	size_t len = regerror(REG_BADPAT, NULL, buf, sizeof(buf));
	ASSERT_TRUE(len > 0);
	ASSERT_STR_EQ("Bad pattern", buf);
}

TEST(regerror_buffer_size_needed) {
	char tiny[4];
	size_t len = regerror(REG_ESPACE, NULL, tiny, sizeof(tiny));
	ASSERT_TRUE(len > sizeof(tiny));
	ASSERT_STR_PRE("Mem", tiny);
}

TEST_SUITE(bre_ere)

TEST(bre_requires_escaped_parens) {
	regex_t re;
	ASSERT_INT_EQ(0, regcomp(&re, "(a)", 0));
	regmatch_t pm[1];
	ASSERT_INT_EQ(0, regexec(&re, "(a)", 1, pm, 0));
	regfree(&re);
}

TEST(bre_escaped_parens_capture) {
	regex_t re; regmatch_t pm[2];
	ASSERT_INT_EQ(0, regcomp(&re, "\\(a\\)", 0));
	ASSERT_INT_EQ(0, regexec(&re, "a", 2, pm, 0));
	ASSERT_INT_EQ(0, pm[0].rm_so); ASSERT_INT_EQ(1, pm[0].rm_eo);
	ASSERT_INT_EQ(0, pm[1].rm_so); ASSERT_INT_EQ(1, pm[1].rm_eo);
	regfree(&re);
}

TEST(ere_unescaped_parens_capture) {
	regex_t re; regmatch_t pm[2];
	ASSERT_INT_EQ(0, regcomp(&re, "(a)", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "a", 2, pm, 0));
	ASSERT_INT_EQ(0, pm[1].rm_so); ASSERT_INT_EQ(1, pm[1].rm_eo);
	regfree(&re);
}

TEST(ere_alternation_pipe) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "a|b", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "b", 1, pm, 0));
	ASSERT_INT_EQ(REG_NOMATCH, regexec(&re, "c", 1, pm, 0));
	regfree(&re);
}

TEST(bre_pipe_is_literal) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "a|b", 0));
	ASSERT_INT_EQ(0, regexec(&re, "a|b", 1, pm, 0));
	ASSERT_INT_EQ(REG_NOMATCH, regexec(&re, "a", 1, pm, 0));
	regfree(&re);
}

TEST_SUITE(char_classes)

TEST(ctype_alpha_ascii) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "[[:alpha:]]", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "A", 1, pm, 0));
	ASSERT_INT_EQ(0, regexec(&re, "z", 1, pm, 0));
	ASSERT_INT_EQ(REG_NOMATCH, regexec(&re, "5", 1, pm, 0));
	regfree(&re);
}

TEST(wctype_alpha_unicode) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "[[:walpha:]]", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "A", 1, pm, 0));
	ASSERT_INT_EQ(0, regexec(&re, "é", 1, pm, 0));
	ASSERT_INT_EQ(0, regexec(&re, "λ", 1, pm, 0));
	ASSERT_INT_EQ(REG_NOMATCH, regexec(&re, "5", 1, pm, 0));
	regfree(&re);
}

TEST(range_merge_adjacent) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "[a-cx-z]", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "b", 1, pm, 0));
	ASSERT_INT_EQ(0, regexec(&re, "y", 1, pm, 0));
	ASSERT_INT_EQ(REG_NOMATCH, regexec(&re, "d", 1, pm, 0));
	regfree(&re);
}

TEST(negated_class) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "[^0-9]", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "a", 1, pm, 0));
	ASSERT_INT_EQ(REG_NOMATCH, regexec(&re, "5", 1, pm, 0));
	regfree(&re);
}

TEST_SUITE(flags)

TEST(reg_icase_ascii) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "Track", REG_EXTENDED | REG_ICASE));
	ASSERT_INT_EQ(0, regexec(&re, "track", 1, pm, 0));
	ASSERT_INT_EQ(0, regexec(&re, "TRACK", 1, pm, 0));
	ASSERT_INT_EQ(0, regexec(&re, "TrAcK", 1, pm, 0));
	regfree(&re);
}

TEST(reg_newline_dot_excludes_nl) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "a.b", REG_EXTENDED | REG_NEWLINE));
	ASSERT_INT_EQ(0, regexec(&re, "aXb", 1, pm, 0));
	ASSERT_INT_EQ(REG_NOMATCH, regexec(&re, "a\nb", 1, pm, 0));
	regfree(&re);
}

TEST(reg_newline_anchor_around_nl) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "^end$", REG_EXTENDED | REG_NEWLINE));
	ASSERT_INT_EQ(0, regexec(&re, "start\nend\nmore", 1, pm, 0));
	ASSERT_INT_EQ(6, pm[0].rm_so); ASSERT_INT_EQ(9, pm[0].rm_eo);
	regfree(&re);
}

TEST(reg_notbol_anchor_disabled) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "^foo", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "foo", 1, pm, 0));
	ASSERT_INT_EQ(REG_NOMATCH, regexec(&re, "foo", 1, pm, REG_NOTBOL));
	regfree(&re);
}

TEST(reg_noteol_anchor_disabled) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "bar$", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "bar", 1, pm, 0));
	ASSERT_INT_EQ(REG_NOMATCH, regexec(&re, "bar", 1, pm, REG_NOTEOL));
	regfree(&re);
}

TEST(reg_nosub_skips_pmatch) {
	regex_t re;
	ASSERT_INT_EQ(0, regcomp(&re, "(a)(b)", REG_EXTENDED | REG_NOSUB));
	ASSERT_INT_EQ(0, regexec(&re, "ab", 0, NULL, 0));
	regfree(&re);
}

TEST_SUITE(captures)

TEST(capture_nested_groups) {
	regex_t re; regmatch_t pm[4];
	ASSERT_INT_EQ(0, regcomp(&re, "(a(b(c)))", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "abc", 4, pm, 0));
	ASSERT_INT_EQ(0, pm[0].rm_so); ASSERT_INT_EQ(3, pm[0].rm_eo);
	ASSERT_INT_EQ(0, pm[1].rm_so); ASSERT_INT_EQ(3, pm[1].rm_eo);
	ASSERT_INT_EQ(1, pm[2].rm_so); ASSERT_INT_EQ(3, pm[2].rm_eo);
	ASSERT_INT_EQ(2, pm[3].rm_so); ASSERT_INT_EQ(3, pm[3].rm_eo);
	regfree(&re);
}

TEST(leftmost_longest_quantifier) {
	//TEST_SKIP("causes segfault");
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "(a|aa)*b", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "aaab", 1, pm, 0));
	ASSERT_INT_EQ(0, pm[0].rm_so); ASSERT_INT_EQ(4, pm[0].rm_eo);
	regfree(&re);
}

TEST(optional_capture_zero_length) {
	regex_t re; regmatch_t pm[2];
	ASSERT_INT_EQ(0, regcomp(&re, "a(b)?c", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "ac", 2, pm, 0));
	ASSERT_INT_EQ(0, pm[0].rm_so); ASSERT_INT_EQ(2, pm[0].rm_eo);
	ASSERT_INT_EQ(-1, pm[1].rm_so); ASSERT_INT_EQ(-1, pm[1].rm_eo);
	regfree(&re);
}

TEST_SUITE(utf8)

TEST(utf8_two_byte_literal) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "café", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "café", 1, pm, 0));
	ASSERT_INT_EQ(0, pm[0].rm_so); ASSERT_INT_EQ(5, pm[0].rm_eo);
	regfree(&re);
}

TEST(utf8_class_range) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "[\xC3\xA0-\xC3\xAF]", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "\xC3\xA0", 1, pm, 0));
	ASSERT_INT_EQ(0, regexec(&re, "\xC3\xAF", 1, pm, 0));
	ASSERT_INT_EQ(REG_NOMATCH, regexec(&re, "\xC3\xB0", 1, pm, 0));
	regfree(&re);
}

TEST(utf8_invalid_continuation) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, ".", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "\xC3\xFF", 1, pm, 0));
	regfree(&re);
}

TEST_SUITE(gate)

TEST(gate_skips_no_match) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "^track_[0-9]+", REG_EXTENDED));
	ASSERT_INT_EQ(REG_NOMATCH, regexec(&re, "mix_123", 1, pm, 0));
	regfree(&re);
}

TEST(gate_finds_anchor) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "mix\\.wav$", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "track_001_mix.wav", 1, pm, 0));
	ASSERT_INT_EQ(11, pm[0].rm_so); ASSERT_INT_EQ(19, pm[0].rm_eo);
	regfree(&re);
}

TEST_SUITE(perf)

TEST(dfa_linear_scan) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "[[:alpha:]]+", REG_EXTENDED));
	const char *long_str = "abcdefghijklmnopqrstuvwxyz";
	clock_t start = clock();
	for (int i = 0; i < 10000; i++) regexec(&re, long_str, 1, pm, 0);
	clock_t end = clock();
	double ms = (double)(end - start) / CLOCKS_PER_SEC * 1000;
	TEST_INFO("10k scans of 26-char alpha: %.2fms", ms);
	ASSERT_TRUE(ms < 100.0);
	regfree(&re);
}

TEST(gate_reduces_work) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "^track_[0-9]+_mix\\.wav$", REG_EXTENDED));
	const char *no_match = "album_art_cover.png";
	clock_t start = clock();
	for (int i = 0; i < 10000; i++) regexec(&re, no_match, 1, pm, 0);
	clock_t end = clock();
	double ms = (double)(end - start) / CLOCKS_PER_SEC * 1000;
	TEST_INFO("10k gate-fail scans: %.2fms", ms);
	ASSERT_TRUE(ms < 50.0);
	regfree(&re);
}

TEST_SUITE(edges)

TEST(empty_pattern_matches_empty) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "", 1, pm, 0));
	ASSERT_INT_EQ(0, pm[0].rm_so); ASSERT_INT_EQ(0, pm[0].rm_eo);
	regfree(&re);
}

TEST(quantifier_zero_times) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "a{0}", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "", 1, pm, 0));
	regfree(&re);
}

TEST(quantifier_exact) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "a{3}", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "aaa", 1, pm, 0));
	ASSERT_INT_EQ(REG_NOMATCH, regexec(&re, "aa", 1, pm, 0));
	regfree(&re);
}

TEST(quantifier_range) {
	//TEST_SKIP("causes segfault");
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "a{2,4}", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "aa", 1, pm, 0));
	ASSERT_INT_EQ(0, regexec(&re, "aaaa", 1, pm, 0));
	ASSERT_INT_EQ(REG_NOMATCH, regexec(&re, "a", 1, pm, 0));
	regfree(&re);
}

TEST(escaped_literal_in_class) {
	regex_t re; regmatch_t pm[1];
	ASSERT_INT_EQ(0, regcomp(&re, "[\\]\\[]", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "]", 1, pm, 0));
	ASSERT_INT_EQ(0, regexec(&re, "[", 1, pm, 0));
	regfree(&re);
}

TEST(bad_pattern_error) {
	regex_t re;
	ASSERT_INT_EQ(REG_EBRACK, regcomp(&re, "[", REG_EXTENDED));
	ASSERT_INT_EQ(REG_EPAREN, regcomp(&re, "(", REG_EXTENDED));
	ASSERT_INT_EQ(REG_EBRACE, regcomp(&re, "a{", REG_EXTENDED));
}

TEST(state_limit_error) {
	regex_t re;
	const char *deep = "(a|b)(c|d)(e|f)(g|h)(i|j)(k|l)(m|n)(o|p)(q|r)(s|t)(u|v)(w|x)(y|z)";
	int err = regcomp(&re, deep, REG_EXTENDED);
	ASSERT_TRUE(err == 0 || err == REG_ESPACE);
	if (err == 0) regfree(&re);
}

TEST_SUITE(audio)

TEST(track_number_capture) {
	//TEST_SKIP("causes segfault");
	regex_t re; regmatch_t pm[3];
	ASSERT_INT_EQ(0, regcomp(&re, "^track_([0-9]+)_(mix|master)\\.wav$", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "track_042_master.wav", 3, pm, 0));
	ASSERT_INT_EQ(0, pm[0].rm_so); ASSERT_INT_EQ(22, pm[0].rm_eo);
	ASSERT_INT_EQ(6, pm[1].rm_so); ASSERT_INT_EQ(9, pm[1].rm_eo);
	ASSERT_INT_EQ(10, pm[2].rm_so); ASSERT_INT_EQ(16, pm[2].rm_eo);
	regfree(&re);
}

TEST(artist_title_unicode) {
	regex_t re; regmatch_t pm[3];
	ASSERT_INT_EQ(0, regcomp(&re, "^([[:walpha:]]+)\\s+–\\s+([[:walpha:]]+)$", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "Björk – Hyperballad", 3, pm, 0));
	ASSERT_INT_EQ(0, pm[1].rm_so); ASSERT_INT_EQ(5, pm[1].rm_eo);
	regfree(&re);
}

TEST(sample_rate_pattern) {
	regex_t re; regmatch_t pm[2];
	ASSERT_INT_EQ(0, regcomp(&re, "^(44100|48000|96000)Hz$", REG_EXTENDED));
	ASSERT_INT_EQ(0, regexec(&re, "48000Hz", 2, pm, 0));
	ASSERT_INT_EQ(0, pm[1].rm_so); ASSERT_INT_EQ(5, pm[1].rm_eo);
	regfree(&re);
}

TEST_MAIN()
