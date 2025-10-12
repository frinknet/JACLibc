/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_C99
#include <wchar.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(wchar.h);

/* ============================================================================
 * CONSTANTS AND TYPES
 * ============================================================================ */
TEST_SUITE(constants_types);

TEST(weof_defined) {
	ASSERT_EQ(-1, WEOF);
}

TEST(mb_cur_max_defined) {
	ASSERT_EQ(4, MB_CUR_MAX);
}

TEST(wchar_limits) {
	ASSERT_EQ(0, WCHAR_MIN);
	ASSERT_EQ(0xFFFFFFFFu, WCHAR_MAX);
}

TEST(wint_t_defined) {
	wint_t val = 0x1F4A9;
	ASSERT_EQ(0x1F4A9, val);
}

TEST(mbstate_t_defined) {
	mbstate_t state = {0, 0, 0};
	ASSERT_EQ(0, state.wc);
}

/* ============================================================================
 * BASIC WCHAR STRING OPERATIONS
 * ============================================================================ */
TEST_SUITE(wchar_strings);

TEST(wcslen_basic) {
	wchar_t str[] = L"Hello";
	ASSERT_EQ(5, wcslen(str));
}

TEST(wcslen_empty) {
	wchar_t str[] = L"";
	ASSERT_EQ(0, wcslen(str));
}

TEST(wcscpy_basic) {
	wchar_t src[] = L"Test";
	wchar_t dst[10];
	
	wcscpy(dst, src);
	ASSERT_EQ(0, wcscmp(dst, L"Test"));
}

TEST(wcsncpy_basic) {
	wchar_t src[] = L"Hello";
	wchar_t dst[10] = {0};
	
	wcsncpy(dst, src, 3);
	ASSERT_EQ(0, wcsncmp(dst, L"Hel", 3));
}

TEST(wcsncpy_pads_nulls) {
	wchar_t dst[10];
	wcsncpy(dst, L"Hi", 10);
	
	ASSERT_EQ(L'\0', dst[9]);
}

/* ============================================================================
 * WCHAR COMPARISON
 * ============================================================================ */
TEST_SUITE(wchar_comparison);

TEST(wcscmp_equal) {
	ASSERT_EQ(0, wcscmp(L"Test", L"Test"));
}

TEST(wcscmp_less) {
	ASSERT_TRUE(wcscmp(L"Apple", L"Banana") < 0);
}

TEST(wcscmp_greater) {
	ASSERT_TRUE(wcscmp(L"Zebra", L"Apple") > 0);
}

TEST(wcsncmp_partial) {
	ASSERT_EQ(0, wcsncmp(L"Hello", L"Help", 3));
}

TEST(wcsncmp_full) {
	ASSERT_NE(0, wcsncmp(L"Hello", L"Help", 4));
}

/* ============================================================================
 * WCHAR SEARCH
 * ============================================================================ */
TEST_SUITE(wchar_search);

TEST(wcschr_found) {
	wchar_t *result = wcschr(L"Hello", L'l');
	ASSERT_NOT_NULL(result);
	ASSERT_EQ(L'l', *result);
}

TEST(wcschr_not_found) {
	wchar_t *result = wcschr(L"Hello", L'x');
	ASSERT_NULL(result);
}

TEST(wcsrchr_last_occurrence) {
	wchar_t *result = wcsrchr(L"Hello", L'l');
	ASSERT_NOT_NULL(result);
	ASSERT_EQ(L'l', *result);
	ASSERT_EQ(L'o', *(result + 1));
}

TEST(wcsstr_found) {
	wchar_t *result = wcsstr(L"Hello World", L"World");
	ASSERT_NOT_NULL(result);
	ASSERT_EQ(0, wcscmp(result, L"World"));
}

TEST(wcsstr_not_found) {
	wchar_t *result = wcsstr(L"Hello", L"xyz");
	ASSERT_NULL(result);
}

TEST(wcsstr_empty_needle) {
	wchar_t str[] = L"Test";
	wchar_t *result = wcsstr(str, L"");
	ASSERT_EQ(str, result);
}

/* ============================================================================
 * WMEM OPERATIONS
 * ============================================================================ */
TEST_SUITE(wmem_operations);

TEST(wmemcpy_basic) {
	wchar_t src[] = L"Test";
	wchar_t dst[10];
	
	wmemcpy(dst, src, 4);
	ASSERT_EQ(0, wmemcmp(dst, L"Test", 4));
}

TEST(wmemmove_forward) {
	wchar_t buf[] = L"Hello";
	wmemmove(buf + 1, buf, 4);
	
	ASSERT_EQ(L'H', buf[1]);
}

TEST(wmemmove_backward) {
	wchar_t buf[] = L"Hello";
	wmemmove(buf, buf + 1, 4);
	
	ASSERT_EQ(L'e', buf[0]);
}

TEST(wmemchr_found) {
	wchar_t str[] = L"Hello";
	wchar_t *result = wmemchr(str, L'l', 5);
	
	ASSERT_NOT_NULL(result);
	ASSERT_EQ(L'l', *result);
}

TEST(wmemchr_not_found) {
	wchar_t str[] = L"Hello";
	wchar_t *result = wmemchr(str, L'x', 5);
	
	ASSERT_NULL(result);
}

TEST(wmemcmp_equal) {
	ASSERT_EQ(0, wmemcmp(L"Test", L"Test", 4));
}

TEST(wmemcmp_different) {
	ASSERT_NE(0, wmemcmp(L"Test", L"Best", 4));
}

TEST(wmemset_basic) {
	wchar_t buf[5];
	wmemset(buf, L'A', 5);
	
	ASSERT_EQ(L'A', buf[0]);
	ASSERT_EQ(L'A', buf[4]);
}

/* ============================================================================
 * UTF-8 CONVERSION - MBRTOWC
 * ============================================================================ */
TEST_SUITE(mbrtowc);

TEST(mbrtowc_ascii) {
	wchar_t wc;
	int result = mbrtowc(&wc, "A", 1, NULL);
	
	ASSERT_EQ(1, result);
	ASSERT_EQ(L'A', wc);
}

TEST(mbrtowc_two_byte) {
	wchar_t wc;
	const char *utf8 = "\xC3\xA9";  // é
	int result = mbrtowc(&wc, utf8, 2, NULL);
	
	ASSERT_EQ(2, result);
	ASSERT_EQ(0xE9, wc);
}

TEST(mbrtowc_three_byte) {
	wchar_t wc;
	const char *utf8 = "\xE2\x82\xAC";  // €
	int result = mbrtowc(&wc, utf8, 3, NULL);
	
	ASSERT_EQ(3, result);
	ASSERT_EQ(0x20AC, wc);
}

TEST(mbrtowc_four_byte) {
	wchar_t wc;
	const char *utf8 = "\xF0\x9F\x98\x80";  // 😀
	int result = mbrtowc(&wc, utf8, 4, NULL);
	
	ASSERT_EQ(4, result);
	ASSERT_EQ(0x1F600, wc);
}

TEST(mbrtowc_null_input) {
	wchar_t wc;
	int result = mbrtowc(&wc, NULL, 0, NULL);
	
	ASSERT_EQ(0, result);
}

/* ============================================================================
 * UTF-8 CONVERSION - WCRTOMB
 * ============================================================================ */
TEST_SUITE(wcrtomb);

TEST(wcrtomb_ascii) {
	char buf[MB_CUR_MAX];
	int result = wcrtomb(buf, L'A', NULL);
	
	ASSERT_EQ(1, result);
	ASSERT_EQ('A', buf[0]);
}

TEST(wcrtomb_two_byte) {
	char buf[MB_CUR_MAX];
	int result = wcrtomb(buf, 0xE9, NULL);  // é
	
	ASSERT_EQ(2, result);
	ASSERT_EQ((char)0xC3, buf[0]);
	ASSERT_EQ((char)0xA9, buf[1]);
}

TEST(wcrtomb_three_byte) {
	char buf[MB_CUR_MAX];
	int result = wcrtomb(buf, 0x20AC, NULL);  // €
	
	ASSERT_EQ(3, result);
	ASSERT_EQ((char)0xE2, buf[0]);
	ASSERT_EQ((char)0x82, buf[1]);
	ASSERT_EQ((char)0xAC, buf[2]);
}

TEST(wcrtomb_four_byte) {
	char buf[MB_CUR_MAX];
	int result = wcrtomb(buf, 0x1F600, NULL);  // 😀
	
	ASSERT_EQ(4, result);
	ASSERT_EQ((char)0xF0, buf[0]);
	ASSERT_EQ((char)0x9F, buf[1]);
	ASSERT_EQ((char)0x98, buf[2]);
	ASSERT_EQ((char)0x80, buf[3]);
}

TEST(wcrtomb_query) {
	int result = wcrtomb(NULL, 0, NULL);
	ASSERT_EQ(1, result);
}

/* ============================================================================
 * MULTIBYTE STRING CONVERSIONS
 * ============================================================================ */
TEST_SUITE(multibyte_strings);

TEST(mbstowcs_ascii) {
	wchar_t wcs[10];
	size_t result = mbstowcs(wcs, "Hello", 10);
	
	ASSERT_EQ(5, result);
	ASSERT_EQ(0, wcscmp(wcs, L"Hello"));
}

TEST(wcstombs_ascii) {
	char mbs[20];
	size_t result = wcstombs(mbs, L"Hello", 20);
	
	ASSERT_EQ(5, result);
	ASSERT_STR_EQ("Hello", mbs);
}

TEST(mbstowcs_utf8) {
	wchar_t wcs[10];
	const char *utf8 = "Café";
	size_t result = mbstowcs(wcs, utf8, 10);
	
	ASSERT_TRUE(result >= 4);
}

/* ============================================================================
 * BTOWC / WCTOB
 * ============================================================================ */
TEST_SUITE(btowc_wctob);

TEST(btowc_ascii) {
	wint_t result = btowc('A');
	ASSERT_EQ(L'A', result);
}

TEST(btowc_eof) {
	wint_t result = btowc(EOF);
	ASSERT_EQ(WEOF, result);
}

TEST(wctob_ascii) {
	int result = wctob(L'Z');
	ASSERT_EQ('Z', result);
}

TEST(wctob_weof) {
	int result = wctob(WEOF);
	ASSERT_EQ(EOF, result);
}

TEST(wctob_out_of_range) {
	int result = wctob(0x1F600);
	ASSERT_EQ(EOF, result);
}

/* ============================================================================
 * COLLATION
 * ============================================================================ */
TEST_SUITE(collation);

TEST(wcscoll_basic) {
	ASSERT_EQ(0, wcscoll(L"Test", L"Test"));
	ASSERT_TRUE(wcscoll(L"Apple", L"Banana") < 0);
}

TEST(wcsxfrm_basic) {
	wchar_t dst[20];
	size_t len = wcsxfrm(dst, L"Hello", 20);
	
	ASSERT_EQ(5, len);
	ASSERT_EQ(0, wcscmp(dst, L"Hello"));
}

TEST_MAIN()

#else

int main(void) {
	printf("wchar.h requires C99 or later\n");
	return 0;
}

#endif
