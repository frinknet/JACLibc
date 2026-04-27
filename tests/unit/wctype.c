/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <wctype.h>
#include <locale.h>

extern thread_local __jacl_wctype_t __jacl_wctype;

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

#define X(LANG, name, ...) \
TEST_SUITE(LANG); \
TEST(LANG##_isdigit) { \
	setlocale(LC_CTYPE, name); \
	for (int i = 0; i < 10; i++) ASSERT_TRUE(isdigit(__jacl_wctype.d[i])); \
	for (int i = 0; i < __jacl_wctype.u_count; i++) ASSERT_FALSE(isdigit(__jacl_wctype.u[i])); \
} \
TEST(LANG##_ispunct) { \
	for (int i = 0; i < __jacl_wctype.e_count; i++) ASSERT_TRUE(ispunct(__jacl_wctype.e[i])); \
	for (int i = 0; i < __jacl_wctype.u_count; i++) ASSERT_FALSE(ispunct(__jacl_wctype.u[i])); \
} \
TEST(LANG##_iswalpha) { \
	if(!__jacl_wctype.u_count && !__jacl_wctype.l_count) {\
		for (int i = __jacl_wctype.u[0]; i < __jacl_wctype.l[0]; i++) ASSERT_TRUE(isalpha(__jacl_wctype.e[i])); \
	} \
} \
TEST(LANG##_iswupper) { \
	if(__jacl_wctype.u_count == __jacl_wctype.l_count) { \
		for (int i = 0; i < __jacl_wctype.u_count; i++) ASSERT_TRUE(isupper(__jacl_wctype.u[i])); \
		for (int i = 0; i < __jacl_wctype.l_count; i++) ASSERT_FALSE(isupper(__jacl_wctype.l[i])); \
	} \
} \
TEST(LANG##_iswlower) { \
	if(__jacl_wctype.u_count == __jacl_wctype.l_count) { \
		for (int i = 0; i < __jacl_wctype.l_count; i++) ASSERT_TRUE(islower(__jacl_wctype.l[i])); \
		for (int i = 0; i < __jacl_wctype.u_count; i++) ASSERT_FALSE(islower(__jacl_wctype.u[i])); \
	} \
} \
TEST(LANG##_towupper) { \
	if(__jacl_wctype.u_count == __jacl_wctype.l_count) { \
		for (int i = 0; i < __jacl_wctype.l_count; i++) ASSERT_EQ(toupper(__jacl_wctype.l[i]), __jacl_wctype.u[i]); \
	} \
} \
TEST(LANG##_towlower) { \
	if(__jacl_wctype.u_count == __jacl_wctype.l_count) { \
		for (int i = 0; i < __jacl_wctype.l_count; i++) ASSERT_EQ(tolower(__jacl_wctype.u[i]), __jacl_wctype.l[i]); \
	} \
}
#include <x/locale_wctype.h>
#undef X

TEST_MAIN_IF(JACL_HAS_C99, "wctype.h requires C99 or later\n")
