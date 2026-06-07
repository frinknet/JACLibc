/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <wctype.h>
#include <locale.h>

extern thread_local locale_t __jacl_locale_current;

TEST_TYPE(unit);
TEST_UNIT(wctype.h);

#if JACL_HAS_C99

/* ============================================================================ */
/* iswalnum                                                                     */
/* ============================================================================ */
TEST_SUITE(iswalnum);
TEST(iswalnum_alpha) { ASSERT_TRUE(iswalnum(L'A')); }
TEST(iswalnum_digit) { ASSERT_TRUE(iswalnum(L'0')); }
TEST(iswalnum_punct) { ASSERT_FALSE(iswalnum(L'!')); }
TEST(iswalnum_weof) { ASSERT_FALSE(iswalnum(WEOF)); }

/* ============================================================================ */
/* iswalnum_l                                                                   */
/* ============================================================================ */
TEST_SUITE(iswalnum_l);
TEST(iswalnum_l_alpha) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_TRUE(iswalnum_l(L'A', loc));
	freelocale(loc);
}
TEST(iswalnum_l_digit) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_TRUE(iswalnum_l(L'0', loc));
	freelocale(loc);
}

/* ============================================================================ */
/* iswalpha                                                                     */
/* ============================================================================ */
TEST_SUITE(iswalpha);
TEST(iswalpha_uses_current_locale) {
	locale_t old = uselocale(newlocale(LC_CTYPE_MASK, "C", NULL));
	ASSERT_TRUE(iswalpha(L'A'));
	uselocale(old);
}
TEST(iswalpha_empty_locale_tables) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	if (!loc->wctype.u_count && !loc->wctype.l_count) {
		// Edge case handling for empty tables
	}
	freelocale(loc);
}
TEST(iswalpha_weof) { ASSERT_FALSE(iswalpha(WEOF)); }

/* ============================================================================ */
/* iswalpha_l                                                                   */
/* ============================================================================ */
TEST_SUITE(iswalpha_l);
TEST(iswalpha_l_uses_locale_tables) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_TRUE(iswalpha_l(L'A', loc));
	ASSERT_FALSE(iswalpha_l(L'0', loc));
	freelocale(loc);
}

/* ============================================================================ */
/* iswblank                                                                     */
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
TEST(iswblank_weof) { ASSERT_FALSE(iswblank(WEOF)); }

/* ============================================================================ */
/* iswblank_l                                                                   */
/* ============================================================================ */
TEST_SUITE(iswblank_l);
TEST(iswblank_l_basic) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_TRUE(iswblank_l(L' ', loc));
	ASSERT_FALSE(iswblank_l(L'\n', loc));
	freelocale(loc);
}

/* ============================================================================ */
/* iswcntrl                                                                     */
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
TEST(iswcntrl_weof) { ASSERT_FALSE(iswcntrl(WEOF)); }

/* ============================================================================ */
/* iswcntrl_l                                                                   */
/* ============================================================================ */
TEST_SUITE(iswcntrl_l);
TEST(iswcntrl_l_uses_locale) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_TRUE(iswcntrl_l(L'\0', loc));
	ASSERT_FALSE(iswcntrl_l(L'A', loc));
	freelocale(loc);
}

/* ============================================================================ */
/* iswctype                                                                     */
/* ============================================================================ */
TEST_SUITE(iswctype);
TEST(iswctype_unknown) { ASSERT_FALSE(iswctype(L'A', WCTYPE_UNKNOWN)); }
TEST(iswctype_weof) { ASSERT_FALSE(iswctype(WEOF, WCTYPE_ALPHA)); }

/* ============================================================================ */
/* iswctype_l                                                                   */
/* ============================================================================ */
TEST_SUITE(iswctype_l);
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
TEST(iswctype_l_unknown) { ASSERT_FALSE(iswctype_l(L'A', WCTYPE_UNKNOWN, __jacl_locale_current)); }

/* ============================================================================ */
/* iswdigit                                                                     */
/* ============================================================================ */
TEST_SUITE(iswdigit);
TEST(iswdigit_basic) {
	ASSERT_TRUE(iswdigit(L'0'));
	ASSERT_TRUE(iswdigit(L'9'));
	ASSERT_FALSE(iswdigit(L'A'));
}
TEST(iswdigit_weof) { ASSERT_FALSE(iswdigit(WEOF)); }

/* ============================================================================ */
/* iswdigit_l                                                                   */
/* ============================================================================ */
TEST_SUITE(iswdigit_l);
TEST(iswdigit_l_uses_locale_digits) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	for (int i = 0; i < 10; i++) {
		ASSERT_TRUE(iswdigit_l(loc->wctype.d[i], loc));
	}
	freelocale(loc);
}
TEST(iswdigit_l_locale_specific) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	for (int i = 0; i < 10; i++) {
		ASSERT_TRUE(iswdigit_l(loc->wctype.d[i], loc));
	}
	freelocale(loc);
}

/* ============================================================================ */
/* iswgraph                                                                     */
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
TEST(iswgraph_weof) { ASSERT_FALSE(iswgraph(WEOF)); }

/* ============================================================================ */
/* iswgraph_l                                                                   */
/* ============================================================================ */
TEST_SUITE(iswgraph_l);
TEST(iswgraph_l_basic) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_TRUE(iswgraph_l(L'A', loc));
	ASSERT_FALSE(iswgraph_l(L' ', loc));
	freelocale(loc);
}

/* ============================================================================ */
/* iswlower                                                                     */
/* ============================================================================ */
TEST_SUITE(iswlower);
TEST(iswlower_basic) {
	ASSERT_TRUE(iswlower(L'a'));
	ASSERT_FALSE(iswlower(L'A'));
}
TEST(iswlower_weof) { ASSERT_FALSE(iswlower(WEOF)); }

/* ============================================================================ */
/* iswlower_l                                                                   */
/* ============================================================================ */
TEST_SUITE(iswlower_l);
TEST(iswlower_l_basic) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_TRUE(iswlower_l(L'a', loc));
	ASSERT_FALSE(iswlower_l(L'A', loc));
	freelocale(loc);
}

/* ============================================================================ */
/* iswprint                                                                     */
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
TEST(iswprint_weof) { ASSERT_FALSE(iswprint(WEOF)); }

/* ============================================================================ */
/* iswprint_l                                                                   */
/* ============================================================================ */
TEST_SUITE(iswprint_l);
TEST(iswprint_l_basic) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_TRUE(iswprint_l(L'A', loc));
	ASSERT_FALSE(iswprint_l(L'\0', loc));
	freelocale(loc);
}

/* ============================================================================ */
/* iswpunct                                                                     */
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
TEST(iswpunct_weof) { ASSERT_FALSE(iswpunct(WEOF)); }

/* ============================================================================ */
/* iswpunct_l                                                                   */
/* ============================================================================ */
TEST_SUITE(iswpunct_l);
TEST(iswpunct_l_uses_locale_extra) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	for (size_t i = 0; i < loc->wctype.e_count; i++) {
		ASSERT_TRUE(iswpunct_l(loc->wctype.e[i], loc));
	}
	freelocale(loc);
}
TEST(iswpunct_l_core_set) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_TRUE(iswpunct_l(L'.', loc));
	ASSERT_TRUE(iswpunct_l(L'!', loc));
	ASSERT_TRUE(iswpunct_l(L',', loc));
	freelocale(loc);
}

/* ============================================================================ */
/* iswspace                                                                     */
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
TEST(iswspace_weof) { ASSERT_FALSE(iswspace(WEOF)); }

/* ============================================================================ */
/* iswspace_l                                                                   */
/* ============================================================================ */
TEST_SUITE(iswspace_l);
TEST(iswspace_l_basic) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_TRUE(iswspace_l(L' ', loc));
	ASSERT_FALSE(iswspace_l(L'A', loc));
	freelocale(loc);
}

/* ============================================================================ */
/* iswupper                                                                     */
/* ============================================================================ */
TEST_SUITE(iswupper);
TEST(iswupper_basic) {
	ASSERT_TRUE(iswupper(L'A'));
	ASSERT_FALSE(iswupper(L'a'));
}
TEST(iswupper_weof) { ASSERT_FALSE(iswupper(WEOF)); }

/* ============================================================================ */
/* iswupper_l                                                                   */
/* ============================================================================ */
TEST_SUITE(iswupper_l);
TEST(iswupper_l_basic) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_TRUE(iswupper_l(L'A', loc));
	ASSERT_FALSE(iswupper_l(L'a', loc));
	freelocale(loc);
}

/* ============================================================================ */
/* iswxdigit                                                                    */
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
TEST(iswxdigit_weof) { ASSERT_FALSE(iswxdigit(WEOF)); }

/* ============================================================================ */
/* iswxdigit_l                                                                  */
/* ============================================================================ */
TEST_SUITE(iswxdigit_l);
TEST(iswxdigit_l_basic) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_TRUE(iswxdigit_l(L'A', loc));
	ASSERT_FALSE(iswxdigit_l(L'G', loc));
	freelocale(loc);
}

/* ============================================================================ */
/* towctrans                                                                    */
/* ============================================================================ */
TEST_SUITE(towctrans);
TEST(towctrans_unknown) { ASSERT_EQ(L'A', towctrans(L'A', WCTRANS_UNKNOWN)); }
TEST(towctrans_weof) { ASSERT_EQ(WEOF, towctrans(WEOF, WCTRANS_TOUPPER)); }

/* ============================================================================ */
/* towctrans_l                                                                  */
/* ============================================================================ */
TEST_SUITE(towctrans_l);
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
TEST(towctrans_l_unknown) { ASSERT_EQ(L'A', towctrans_l(L'A', WCTRANS_UNKNOWN, __jacl_locale_current)); }

/* ============================================================================ */
/* towlower                                                                     */
/* ============================================================================ */
TEST_SUITE(towlower);
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
TEST(towlower_uses_current_locale) {
	locale_t old = uselocale(newlocale(LC_CTYPE_MASK, "C", NULL));
	ASSERT_EQ(towlower(L'A'), L'a');
	uselocale(old);
}
TEST(towlower_weof) { ASSERT_EQ(WEOF, towlower(WEOF)); }

/* ============================================================================ */
/* towlower_l                                                                   */
/* ============================================================================ */
TEST_SUITE(towlower_l);
TEST(towlower_l_uses_locale_mapping) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	if (loc->wctype.u_count == loc->wctype.l_count) {
		for (size_t i = 0; i < loc->wctype.u_count; i++) {
			ASSERT_EQ(towlower_l(loc->wctype.u[i], loc), loc->wctype.l[i]);
		}
	}
	freelocale(loc);
}

/* ============================================================================ */
/* towupper                                                                     */
/* ============================================================================ */
TEST_SUITE(towupper);
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
TEST(towupper_uses_current_locale) {
	locale_t old = uselocale(newlocale(LC_CTYPE_MASK, "C", NULL));
	ASSERT_EQ(towupper(L'a'), L'A');
	uselocale(old);
}
TEST(towupper_weof) { ASSERT_EQ(WEOF, towupper(WEOF)); }

/* ============================================================================ */
/* towupper_l                                                                   */
/* ============================================================================ */
TEST_SUITE(towupper_l);
TEST(towupper_l_uses_locale_mapping) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	if (loc->wctype.u_count == loc->wctype.l_count) {
		for (size_t i = 0; i < loc->wctype.l_count; i++) {
			ASSERT_EQ(towupper_l(loc->wctype.l[i], loc), loc->wctype.u[i]);
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
/* wctrans                                                                      */
/* ============================================================================ */
TEST_SUITE(wctrans);
TEST(wctrans_valid) {
	ASSERT_EQ(WCTRANS_TOUPPER, wctrans("toupper"));
	ASSERT_EQ(WCTRANS_TOLOWER, wctrans("tolower"));
}
TEST(wctrans_invalid) { ASSERT_EQ(WCTRANS_UNKNOWN, wctrans("invalid")); }
TEST(wctrans_null) { ASSERT_EQ(WCTRANS_UNKNOWN, wctrans(NULL)); }

/* ============================================================================ */
/* wctrans_l                                                                    */
/* ============================================================================ */
TEST_SUITE(wctrans_l);
TEST(wctrans_l_valid_names) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_EQ(wctrans_l("toupper", loc), WCTRANS_TOUPPER);
	ASSERT_EQ(wctrans_l("tolower", loc), WCTRANS_TOLOWER);
	freelocale(loc);
}
TEST(wctrans_l_invalid_name) {
	locale_t loc = newlocale(LC_CTYPE_MASK, "C", NULL);
	ASSERT_EQ(wctrans_l("invalid", loc), WCTRANS_UNKNOWN);
	freelocale(loc);
}
TEST(wctrans_l_null) { ASSERT_EQ(WCTRANS_UNKNOWN, wctrans_l(NULL, __jacl_locale_current)); }

/* ============================================================================ */
/* wctype                                                                       */
/* ============================================================================ */
TEST_SUITE(wctype);
TEST(wctype_valid) {
	ASSERT_EQ(WCTYPE_ALPHA, wctype("alpha"));
	ASSERT_EQ(WCTYPE_DIGIT, wctype("digit"));
}
TEST(wctype_invalid) { ASSERT_EQ(WCTYPE_UNKNOWN, wctype("invalid")); }
TEST(wctype_null) { ASSERT_EQ(WCTYPE_UNKNOWN, wctype(NULL)); }

/* ============================================================================ */
/* wctype_l                                                                     */
/* ============================================================================ */
TEST_SUITE(wctype_l);
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
TEST(wctype_l_null) { ASSERT_EQ(WCTYPE_UNKNOWN, wctype_l(NULL, __jacl_locale_current)); }

/* ============================================================================ */
/* LOCALE TABLE INTEGRATION (Narrow ctype mapped via wctype tables)             */
/* ============================================================================ */

#define X(LANG, name, ...) \
TEST_SUITE(LANG); \
TEST(LANG##_isdigit) { \
	setlocale(LC_CTYPE, name); \
	for (int i = 0; i < 10; i++) ASSERT_TRUE(isdigit(__jacl_locale_current->wctype.d[i])); \
	for (size_t i = 0; i < __jacl_locale_current->wctype.u_count; i++) ASSERT_FALSE(isdigit(__jacl_locale_current->wctype.u[i])); \
} \
TEST(LANG##_ispunct) { \
	for (size_t i = 0; i < __jacl_locale_current->wctype.e_count; i++) ASSERT_TRUE(ispunct(__jacl_locale_current->wctype.e[i])); \
	for (size_t i = 0; i < __jacl_locale_current->wctype.u_count; i++) ASSERT_FALSE(ispunct(__jacl_locale_current->wctype.u[i])); \
} \
TEST(LANG##_isalpha) { \
	if(!__jacl_locale_current->wctype.u_count && !__jacl_locale_current->wctype.l_count) {\
		for (int i = __jacl_locale_current->wctype.u[0]; i < __jacl_locale_current->wctype.l[0]; i++) ASSERT_TRUE(isalpha(__jacl_locale_current->wctype.e[i])); \
	} \
} \
TEST(LANG##_isupper) { \
	if(__jacl_locale_current->wctype.u_count == __jacl_locale_current->wctype.l_count) { \
		for (size_t i = 0; i < __jacl_locale_current->wctype.u_count; i++) ASSERT_TRUE(isupper(__jacl_locale_current->wctype.u[i])); \
		for (size_t i = 0; i < __jacl_locale_current->wctype.l_count; i++) ASSERT_FALSE(isupper(__jacl_locale_current->wctype.l[i])); \
	} \
} \
TEST(LANG##_islower) { \
	if(__jacl_locale_current->wctype.u_count == __jacl_locale_current->wctype.l_count) { \
		for (size_t i = 0; i < __jacl_locale_current->wctype.l_count; i++) ASSERT_TRUE(islower(__jacl_locale_current->wctype.l[i])); \
		for (size_t i = 0; i < __jacl_locale_current->wctype.u_count; i++) ASSERT_FALSE(islower(__jacl_locale_current->wctype.u[i])); \
	} \
} \
TEST(LANG##_toupper) { \
	if(__jacl_locale_current->wctype.u_count == __jacl_locale_current->wctype.l_count) { \
		for (size_t i = 0; i < __jacl_locale_current->wctype.l_count; i++) ASSERT_EQ(toupper(__jacl_locale_current->wctype.l[i]), __jacl_locale_current->wctype.u[i]); \
	} \
} \
TEST(LANG##_tolower) { \
	if(__jacl_locale_current->wctype.u_count == __jacl_locale_current->wctype.l_count) { \
		for (size_t i = 0; i < __jacl_locale_current->wctype.l_count; i++) ASSERT_EQ(tolower(__jacl_locale_current->wctype.u[i]), __jacl_locale_current->wctype.l[i]); \
	} \
}
#include <x/locale_wctype.h>
#undef X

#endif

TEST_MAIN_IF(JACL_HAS_C99, "wctype.h requires C99 or later.")
