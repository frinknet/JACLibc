/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_C99
#include <wctype.h>

TEST_TYPE(unit);
TEST_UNIT(wctype.h);

/* ============================================================================
 * CHARACTER CLASSIFICATION
 * ============================================================================ */
TEST_SUITE(classification);

TEST(iswalpha_basic) {
	ASSERT_TRUE(iswalpha(L'A'));
	ASSERT_TRUE(iswalpha(L'z'));
	ASSERT_FALSE(iswalpha(L'5'));
}

TEST(iswdigit_basic) {
	ASSERT_TRUE(iswdigit(L'0'));
	ASSERT_TRUE(iswdigit(L'9'));
	ASSERT_FALSE(iswdigit(L'A'));
}

TEST(iswalnum_basic) {
	ASSERT_TRUE(iswalnum(L'A'));
	ASSERT_TRUE(iswalnum(L'5'));
	ASSERT_FALSE(iswalnum(L'!'));
}

TEST(iswspace_basic) {
	ASSERT_TRUE(iswspace(L' '));
	ASSERT_TRUE(iswspace(L'\t'));
	ASSERT_TRUE(iswspace(L'\n'));
	ASSERT_FALSE(iswspace(L'A'));
}

TEST(iswblank_basic) {
	ASSERT_TRUE(iswblank(L' '));
	ASSERT_TRUE(iswblank(L'\t'));
	ASSERT_FALSE(iswblank(L'\n'));
}

TEST(iswupper_basic) {
	ASSERT_TRUE(iswupper(L'A'));
	ASSERT_TRUE(iswupper(L'Z'));
	ASSERT_FALSE(iswupper(L'a'));
}

TEST(iswlower_basic) {
	ASSERT_TRUE(iswlower(L'a'));
	ASSERT_TRUE(iswlower(L'z'));
	ASSERT_FALSE(iswlower(L'A'));
}

TEST(iswpunct_basic) {
	ASSERT_TRUE(iswpunct(L'!'));
	ASSERT_TRUE(iswpunct(L'.'));
	ASSERT_FALSE(iswpunct(L'A'));
}

TEST(iswxdigit_basic) {
	ASSERT_TRUE(iswxdigit(L'0'));
	ASSERT_TRUE(iswxdigit(L'9'));
	ASSERT_TRUE(iswxdigit(L'A'));
	ASSERT_TRUE(iswxdigit(L'f'));
	ASSERT_FALSE(iswxdigit(L'G'));
}

TEST(iswgraph_basic) {
	ASSERT_TRUE(iswgraph(L'A'));
	ASSERT_TRUE(iswgraph(L'!'));
	ASSERT_FALSE(iswgraph(L' '));
}

TEST(iswprint_basic) {
	ASSERT_TRUE(iswprint(L'A'));
	ASSERT_TRUE(iswprint(L' '));
	ASSERT_FALSE(iswprint(L'\n'));
}

TEST(iswcntrl_basic) {
	ASSERT_TRUE(iswcntrl(L'\n'));
	ASSERT_TRUE(iswcntrl(L'\t'));
	ASSERT_FALSE(iswcntrl(L'A'));
}

/* ============================================================================
 * CHARACTER TRANSFORMATION
 * ============================================================================ */
TEST_SUITE(transformation);

TEST(towupper_basic) {
	ASSERT_EQ(L'A', towupper(L'a'));
	ASSERT_EQ(L'Z', towupper(L'z'));
	ASSERT_EQ(L'A', towupper(L'A'));
}

TEST(towlower_basic) {
	ASSERT_EQ(L'a', towlower(L'A'));
	ASSERT_EQ(L'z', towlower(L'Z'));
	ASSERT_EQ(L'a', towlower(L'a'));
}

TEST(towupper_numbers) {
	ASSERT_EQ(L'5', towupper(L'5'));
}

TEST(towlower_numbers) {
	ASSERT_EQ(L'5', towlower(L'5'));
}

/* ============================================================================
 * ALL UPPERCASE LETTERS
 * ============================================================================ */
TEST_SUITE(uppercase_letters);

TEST(towupper_all_lowercase) {
	for (wchar_t c = L'a'; c <= L'z'; c++) {
		ASSERT_EQ(c - 32, towupper(c));
	}
}

/* ============================================================================
 * ALL LOWERCASE LETTERS
 * ============================================================================ */
TEST_SUITE(lowercase_letters);

TEST(towlower_all_uppercase) {
	for (wchar_t c = L'A'; c <= L'Z'; c++) {
		ASSERT_EQ(c + 32, towlower(c));
	}
}

/* ============================================================================
 * ALL DIGITS
 * ============================================================================ */
TEST_SUITE(digits);

TEST(iswdigit_all) {
	for (wchar_t c = L'0'; c <= L'9'; c++) {
		ASSERT_TRUE(iswdigit(c));
	}
}

/* ============================================================================
 * WHITESPACE CHARACTERS
 * ============================================================================ */
TEST_SUITE(whitespace);

TEST(iswspace_all) {
	ASSERT_TRUE(iswspace(L' '));
	ASSERT_TRUE(iswspace(L'\t'));
	ASSERT_TRUE(iswspace(L'\n'));
	ASSERT_TRUE(iswspace(L'\r'));
	ASSERT_TRUE(iswspace(L'\v'));
	ASSERT_TRUE(iswspace(L'\f'));
}

/* ============================================================================
 * HEX DIGITS
 * ============================================================================ */
TEST_SUITE(hex_digits);

TEST(iswxdigit_all) {
	for (wchar_t c = L'0'; c <= L'9'; c++) {
		ASSERT_TRUE(iswxdigit(c));
	}
	for (wchar_t c = L'A'; c <= L'F'; c++) {
		ASSERT_TRUE(iswxdigit(c));
	}
	for (wchar_t c = L'a'; c <= L'f'; c++) {
		ASSERT_TRUE(iswxdigit(c));
	}
}

TEST_MAIN()

#else

int main(void) {
	printf("wctype.h requires C99 or later\n");
	return 0;
}

#endif
