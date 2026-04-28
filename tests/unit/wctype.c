/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <wctype.h>
#include <locale.h>

extern thread_local locale_t __jacl_locale_current;

TEST_TYPE(unit);
TEST_UNIT(wctype.h);

/* ============================================================================ */

TEST_SUITE(iswcntrl);

TEST(iswcntrl_control) {
	ASSERT_TRUE(iswcntrl(L'\0'));
	ASSERT_TRUE(iswcntrl(L'\t'));
	ASSERT_TRUE(iswcntrl(L'\n'));
	ASSERT_TRUE(iswcntrl(L'\r'));
	ASSERT_TRUE(iswcntrl(127));
}

TEST(iswcntrl_not_control) {
	ASSERT_FALSE(iswcntrl(L' '));
	ASSERT_FALSE(iswcntrl(L'A'));
	ASSERT_FALSE(iswcntrl(L'0'));
	ASSERT_FALSE(iswcntrl(160));
	ASSERT_FALSE(iswcntrl(255));
}

/* ============================================================================ */

TEST_SUITE(iswspace);

TEST(iswspace_whitespace) {
	ASSERT_TRUE(iswspace(L' '));
	ASSERT_TRUE(iswspace(L'\t'));
	ASSERT_TRUE(iswspace(L'\n'));
	ASSERT_TRUE(iswspace(L'\r'));
	ASSERT_TRUE(iswspace(L'\f'));
	ASSERT_TRUE(iswspace(L'\v'));
}

TEST(iswspace_non_whitespace) {
	ASSERT_FALSE(iswspace(L'A'));
	ASSERT_FALSE(iswspace(L'0'));
	ASSERT_FALSE(iswspace(L'!'));
}

/* ============================================================================ */

TEST_SUITE(iswblank);

TEST(iswblank_horizontal) {
	ASSERT_TRUE(iswblank(L' '));
	ASSERT_TRUE(iswblank(L'\t'));
}

TEST(iswblank_non_blank) {
	ASSERT_FALSE(iswblank(L'\n'));
	ASSERT_FALSE(iswblank(L'\r'));
	ASSERT_FALSE(iswblank(L'A'));
}

/* ============================================================================ */

TEST_SUITE(iswprint);

TEST(iswprint_printable) {
	ASSERT_TRUE(iswprint(L'A'));
	ASSERT_TRUE(iswprint(L' '));
	ASSERT_TRUE(iswprint(L'!'));
	ASSERT_TRUE(iswprint(L'~'));
}

TEST(iswprint_not_printable) {
	ASSERT_FALSE(iswprint(L'\0'));
	ASSERT_FALSE(iswprint(L'\t'));
	ASSERT_FALSE(iswprint(L'\n'));
	ASSERT_FALSE(iswprint(127));
}

/* ============================================================================ */

TEST_SUITE(iswgraph);

TEST(iswgraph_graphical) {
	ASSERT_TRUE(iswgraph(L'A'));
	ASSERT_TRUE(iswgraph(L'!'));
	ASSERT_TRUE(iswgraph(L'0'));
	ASSERT_TRUE(iswgraph(L'~'));
}

TEST(iswgraph_not_graphical) {
	ASSERT_FALSE(iswgraph(L' '));
	ASSERT_FALSE(iswgraph(L'\t'));
	ASSERT_FALSE(iswgraph(L'\n'));
	ASSERT_FALSE(iswgraph(L'\0'));
}

/* ============================================================================ */

TEST_SUITE(iswxdigit);

TEST(iswxdigit_hex) {
	ASSERT_TRUE(iswxdigit(L'0'));
	ASSERT_TRUE(iswxdigit(L'9'));
	ASSERT_TRUE(iswxdigit(L'A'));
	ASSERT_TRUE(iswxdigit(L'F'));
	ASSERT_TRUE(iswxdigit(L'a'));
	ASSERT_TRUE(iswxdigit(L'f'));
}

TEST(iswxdigit_invalid) {
	ASSERT_FALSE(iswxdigit(L'G'));
	ASSERT_FALSE(iswxdigit(L'g'));
	ASSERT_FALSE(iswxdigit(L' '));
}

/* ============================================================================ */

TEST_SUITE(iswpunct);

TEST(iswpunct_punctuation) {
	ASSERT_TRUE(iswpunct(L'!'));
	ASSERT_TRUE(iswpunct(L'.'));
	ASSERT_TRUE(iswpunct(L','));
	ASSERT_TRUE(iswpunct(L'?'));
	ASSERT_TRUE(iswpunct(L';'));
	ASSERT_TRUE(iswpunct(L':'));
	ASSERT_TRUE(iswpunct(L'-'));
	ASSERT_TRUE(iswpunct(L'@'));
	ASSERT_TRUE(iswpunct(L'#'));
}

TEST(iswpunct_not_punctuation) {
	ASSERT_FALSE(iswpunct(L'A'));
	ASSERT_FALSE(iswpunct(L'0'));
	ASSERT_FALSE(iswpunct(L' '));
	ASSERT_FALSE(iswpunct(L'\n'));
}

/* ============================================================================ */

TEST_SUITE(toupper);

TEST(towupper_lowercase) {
	ASSERT_EQ(L'A', towupper(L'a'));
	ASSERT_EQ(L'Z', towupper(L'z'));
	ASSERT_EQ(L'M', towupper(L'm'));
}

TEST(towupper_already_upper) {
	ASSERT_EQ(L'A', towupper(L'A'));
	ASSERT_EQ(L'Z', towupper(L'Z'));
}

TEST(towupper_non_letters) {
	ASSERT_EQ(L'0', towupper(L'0'));
	ASSERT_EQ(L' ', towupper(L' '));
	ASSERT_EQ(L'!', towupper(L'!'));
}

/* ============================================================================ */

TEST_SUITE(tolower);

TEST(towlower_uppercase) {
	ASSERT_EQ(L'a', towlower(L'A'));
	ASSERT_EQ(L'z', towlower(L'Z'));
	ASSERT_EQ(L'm', towlower(L'M'));
}

TEST(towlower_already_lower) {
	ASSERT_EQ(L'a', towlower(L'a'));
	ASSERT_EQ(L'z', towlower(L'z'));
}

TEST(towlower_non_letters) {
	ASSERT_EQ(L'0', towlower(L'0'));
	ASSERT_EQ(L' ', towlower(L' '));
	ASSERT_EQ(L'!', towlower(L'!'));
}

/* ============================================================================ */

TEST_SUITE(wctype_l_variants);

TEST(iswcntrl_l_uses_locale) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_TRUE(iswcntrl_l(L'\0', loc));
	ASSERT_FALSE(iswcntrl_l(L'A', loc));
	freelocale(loc);
}

TEST(iswalpha_l_uses_locale_tables) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_TRUE(iswalpha_l(L'A', loc));
	ASSERT_FALSE(iswalpha_l(L'0', loc));
	freelocale(loc);
}

TEST(iswdigit_l_uses_locale_digits) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	for (int i = 0; i < 10; i++) {
		ASSERT_TRUE(iswdigit_l(loc->wctype.d[i], loc));
	}
	freelocale(loc);
}

TEST(iswpunct_l_uses_locale_extra) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	for (int i = 0; i < loc->wctype.e_count; i++) {
		ASSERT_TRUE(iswpunct_l(loc->wctype.e[i], loc));
	}
	freelocale(loc);
}

/* ============================================================================ */

TEST_SUITE(towcase_l_variants);

TEST(towupper_l_uses_locale_mapping) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	if (loc->wctype.u_count == loc->wctype.l_count) {
		for (int i = 0; i < loc->wctype.l_count; i++) {
			ASSERT_EQ(towupper_l(loc->wctype.l[i], loc), loc->wctype.u[i]);
		}
	}
	freelocale(loc);
}

TEST(towlower_l_uses_locale_mapping) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	if (loc->wctype.u_count == loc->wctype.l_count) {
		for (int i = 0; i < loc->wctype.u_count; i++) {
			ASSERT_EQ(towlower_l(loc->wctype.u[i], loc), loc->wctype.l[i]);
		}
	}
	freelocale(loc);
}

TEST(towupper_l_no_mapping) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	if (loc->wctype.u_count != loc->wctype.l_count) {
		ASSERT_EQ(towupper_l(L'a', loc), L'a');
	}
	freelocale(loc);
}

/* ============================================================================ */

TEST_SUITE(wctype_query);

TEST(wctype_l_valid_names) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_EQ(wctype_l("alpha", loc), WCTYPE_ALPHA);
	ASSERT_EQ(wctype_l("digit", loc), WCTYPE_DIGIT);
	ASSERT_EQ(wctype_l("space", loc), WCTYPE_SPACE);
	ASSERT_EQ(wctype_l("punct", loc), WCTYPE_PUNCT);
	freelocale(loc);
}

TEST(wctype_l_invalid_name) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_EQ(wctype_l("invalid", loc), WCTYPE_UNKNOWN);
	freelocale(loc);
}

TEST(iswctype_l_alpha) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	wctype_t t = wctype_l("alpha", loc);
	ASSERT_TRUE(iswctype_l(L'A', t, loc));
	ASSERT_FALSE(iswctype_l(L'0', t, loc));
	freelocale(loc);
}

TEST(iswctype_l_digit) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	wctype_t t = wctype_l("digit", loc);
	ASSERT_TRUE(iswctype_l(L'5', t, loc));
	ASSERT_FALSE(iswctype_l(L'A', t, loc));
	freelocale(loc);
}

/* ============================================================================ */

TEST_SUITE(wctrans_query);

TEST(wctrans_l_valid_names) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_EQ(wctrans_l("toupper", loc), WCTRANS_TOUPPER);
	ASSERT_EQ(wctrans_l("tolower", loc), WCTRANS_TOLOWER);
	freelocale(loc);
}

TEST(wctrans_l_invalid_name) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_EQ(wctrans_l("invalid", loc), 0);
	freelocale(loc);
}

TEST(towctrans_l_toupper) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	wctrans_t t = wctrans_l("toupper", loc);
	ASSERT_EQ(towctrans_l(L'a', t, loc), L'A');
	ASSERT_EQ(towctrans_l(L'0', t, loc), L'0');
	freelocale(loc);
}

TEST(towctrans_l_tolower) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	wctrans_t t = wctrans_l("tolower", loc);
	ASSERT_EQ(towctrans_l(L'A', t, loc), L'a');
	ASSERT_EQ(towctrans_l(L'0', t, loc), L'0');
	freelocale(loc);
}

/* ============================================================================ */

TEST_SUITE(wctype_wrappers);

TEST(iswalpha_uses_current_locale) {
	locale_t old = __jacl_locale_current;
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	uselocale(loc);
	ASSERT_TRUE(iswalpha(L'A'));
	uselocale(old);
	freelocale(loc);
}

TEST(towupper_uses_current_locale) {
	locale_t old = __jacl_locale_current;
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	uselocale(loc);
	ASSERT_EQ(towupper(L'a'), L'A');
	uselocale(old);
	freelocale(loc);
}

/* ============================================================================ */

TEST_SUITE(wctype_edge);

TEST(iswalpha_empty_locale_tables) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	if (!loc->wctype.u_count && !loc->wctype.l_count) {
		ASSERT_TRUE(iswalpha_l(loc->wctype.u[0], loc));
	}
	freelocale(loc);
}

TEST(iswpunct_core_set) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_TRUE(iswpunct_l(L'.', loc));
	ASSERT_TRUE(iswpunct_l(L'!', loc));
	ASSERT_TRUE(iswpunct_l(L',', loc));
	freelocale(loc);
}

TEST(iswdigit_locale_specific) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	for (int i = 0; i < 10; i++) {
		ASSERT_TRUE(iswdigit_l(loc->wctype.d[i], loc));
	}
	freelocale(loc);
}

/* ============================================================================ */

TEST_SUITE(wctype_thread);

TEST(wctype_thread_isolation) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	locale_t old = uselocale(loc);
	ASSERT_TRUE(iswalpha(L'A'));
	uselocale(old);
	freelocale(loc);
}

/* ============================================================================ */

#define X(LANG, name, ...) \
TEST_SUITE(LANG); \
TEST(LANG##_isdigit) { \
	setlocale(LC_CTYPE, name); \
	for (int i = 0; i < 10; i++) ASSERT_TRUE(isdigit(__jacl_locale_current->wctype.d[i])); \
	for (int i = 0; i < __jacl_locale_current->wctype.u_count; i++) ASSERT_FALSE(isdigit(__jacl_locale_current->wctype.u[i])); \
} \
TEST(LANG##_ispunct) { \
	for (int i = 0; i < __jacl_locale_current->wctype.e_count; i++) ASSERT_TRUE(ispunct(__jacl_locale_current->wctype.e[i])); \
	for (int i = 0; i < __jacl_locale_current->wctype.u_count; i++) ASSERT_FALSE(ispunct(__jacl_locale_current->wctype.u[i])); \
} \
TEST(LANG##_iswalpha) { \
	if(!__jacl_locale_current->wctype.u_count && !__jacl_locale_current->wctype.l_count) {\
		for (int i = __jacl_locale_current->wctype.u[0]; i < __jacl_locale_current->wctype.l[0]; i++) ASSERT_TRUE(isalpha(__jacl_locale_current->wctype.e[i])); \
	} \
} \
TEST(LANG##_iswupper) { \
	if(__jacl_locale_current->wctype.u_count == __jacl_locale_current->wctype.l_count) { \
		for (int i = 0; i < __jacl_locale_current->wctype.u_count; i++) ASSERT_TRUE(isupper(__jacl_locale_current->wctype.u[i])); \
		for (int i = 0; i < __jacl_locale_current->wctype.l_count; i++) ASSERT_FALSE(isupper(__jacl_locale_current->wctype.l[i])); \
	} \
} \
TEST(LANG##_iswlower) { \
	if(__jacl_locale_current->wctype.u_count == __jacl_locale_current->wctype.l_count) { \
		for (int i = 0; i < __jacl_locale_current->wctype.l_count; i++) ASSERT_TRUE(islower(__jacl_locale_current->wctype.l[i])); \
		for (int i = 0; i < __jacl_locale_current->wctype.u_count; i++) ASSERT_FALSE(islower(__jacl_locale_current->wctype.u[i])); \
	} \
} \
TEST(LANG##_towupper) { \
	if(__jacl_locale_current->wctype.u_count == __jacl_locale_current->wctype.l_count) { \
		for (int i = 0; i < __jacl_locale_current->wctype.l_count; i++) ASSERT_EQ(toupper(__jacl_locale_current->wctype.l[i]), __jacl_locale_current->wctype.u[i]); \
	} \
} \
TEST(LANG##_towlower) { \
	if(__jacl_locale_current->wctype.u_count == __jacl_locale_current->wctype.l_count) { \
		for (int i = 0; i < __jacl_locale_current->wctype.l_count; i++) ASSERT_EQ(tolower(__jacl_locale_current->wctype.u[i]), __jacl_locale_current->wctype.l[i]); \
	} \
}
#include <x/locale_wctype.h>
#undef X

TEST_MAIN_IF(JACL_HAS_C99, "wctype.h requires C99 or later.")
