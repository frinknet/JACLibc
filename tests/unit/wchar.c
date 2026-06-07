/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <wchar.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

TEST_TYPE(unit);
TEST_UNIT(wchar.h);

/* ============================================================================ */

TEST_SUITE(constants)

TEST(constants_WEOF) {
	ASSERT_EQ((wint_t)-1, WEOF);
}

TEST(constants_MB_CUR_MAX) {
	ASSERT_EQ(4, MB_CUR_MAX);
}

TEST(constants_WCHAR_MIN) {
	ASSERT_EQ(0, WCHAR_MIN);
}

TEST(constants_WCHAR_MAX) {
	#if INT_BIT >= 32
		ASSERT_EQ(0x10FFFF, WCHAR_MAX);
	#else
		ASSERT_EQ(0xFFFF, WCHAR_MAX);
	#endif
}

/* ============================================================================ */

TEST_SUITE(mbstate_t)

TEST(mbstate_t_initialization) {
	mbstate_t state = {0, 0, 0};
	ASSERT_EQ(0, state.wc);
	ASSERT_EQ(0, state.bytes);
	ASSERT_EQ(0, state.want);
}

/* ============================================================================ */

TEST_SUITE(mbrtowc)

TEST(mbrtowc_ascii) {
	wchar_t wc;
	size_t ret = mbrtowc(&wc, "A", 1, NULL);
	ASSERT_EQ((size_t)1, ret);
	ASSERT_EQ(L'A', wc);
}

TEST(mbrtowc_two_byte) {
	wchar_t wc;
	size_t ret = mbrtowc(&wc, "\xC3\xA9", 2, NULL);
	ASSERT_EQ((size_t)2, ret);
	ASSERT_EQ((wchar_t)0xE9, wc);
}

TEST(mbrtowc_three_byte) {
	wchar_t wc;
	size_t ret = mbrtowc(&wc, "\xE2\x82\xAC", 3, NULL);
	ASSERT_EQ((size_t)3, ret);
	ASSERT_EQ((wchar_t)0x20AC, wc);
}

TEST(mbrtowc_four_byte) {
	wchar_t wc;
	size_t ret = mbrtowc(&wc, "\xF0\x9F\x98\x80", 4, NULL);
	ASSERT_EQ((size_t)4, ret);
	ASSERT_EQ((wchar_t)0x1F600, wc);
}

TEST(mbrtowc_null_s) {
	wchar_t wc;
	size_t ret = mbrtowc(&wc, NULL, 0, NULL);
	ASSERT_EQ((size_t)0, ret);
}

TEST(mbrtowc_null_pwc) {
	size_t ret = mbrtowc(NULL, "A", 1, NULL);
	ASSERT_EQ((size_t)1, ret);
}

TEST(mbrtowc_zero_n) {
	wchar_t wc;
	size_t ret = mbrtowc(&wc, "A", 0, NULL);
	ASSERT_EQ((size_t)-2, ret);
}

TEST(mbrtowc_incomplete_two) {
	wchar_t wc;
	size_t ret = mbrtowc(&wc, "\xC3", 1, NULL);
	ASSERT_EQ((size_t)-2, ret);
}

TEST(mbrtowc_incomplete_three) {
	wchar_t wc;
	size_t ret = mbrtowc(&wc, "\xE2\x82", 2, NULL);
	ASSERT_EQ((size_t)-2, ret);
}

TEST(mbrtowc_incomplete_four) {
	wchar_t wc;
	size_t ret = mbrtowc(&wc, "\xF0\x9F\x98", 3, NULL);
	ASSERT_EQ((size_t)-2, ret);
}

TEST(mbrtowc_invalid_continuation) {
	wchar_t wc;
	errno = 0;
	size_t ret = mbrtowc(&wc, "\xC3\x20", 2, NULL);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(mbrtowc_invalid_start_byte) {
	wchar_t wc;
	errno = 0;
	size_t ret = mbrtowc(&wc, "\x80", 1, NULL);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(mbrtowc_invalid_fe_ff) {
	wchar_t wc;
	errno = 0;
	size_t ret = mbrtowc(&wc, "\xFE", 1, NULL);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(mbrtowc_overlong_two_byte) {
	wchar_t wc;
	errno = 0;
	size_t ret = mbrtowc(&wc, "\xC1\x81", 2, NULL);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(mbrtowc_overlong_two_byte_null) {
	wchar_t wc;
	errno = 0;
	size_t ret = mbrtowc(&wc, "\xC0\x80", 2, NULL);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(mbrtowc_overlong_three_byte) {
	wchar_t wc;
	errno = 0;
	size_t ret = mbrtowc(&wc, "\xE0\x80\x80", 3, NULL);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(mbrtowc_overlong_four_byte) {
	wchar_t wc;
	errno = 0;
	size_t ret = mbrtowc(&wc, "\xF0\x80\x80\x80", 4, NULL);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(mbrtowc_surrogate_high) {
	wchar_t wc;
	errno = 0;
	size_t ret = mbrtowc(&wc, "\xED\xA0\x80", 3, NULL);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(mbrtowc_surrogate_low) {
	wchar_t wc;
	errno = 0;
	size_t ret = mbrtowc(&wc, "\xED\xBF\xBF", 3, NULL);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(mbrtowc_out_of_range) {
	wchar_t wc;
	errno = 0;
	size_t ret = mbrtowc(&wc, "\xF4\x90\x80\x80", 4, NULL);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(mbrtowc_null_char) {
	wchar_t wc = L'X';
	size_t ret = mbrtowc(&wc, "", 1, NULL);
	ASSERT_EQ((size_t)0, ret);
	ASSERT_EQ(L'\0', wc);
}

/* ============================================================================ */

TEST(wcrtomb_ascii) {
	char buf[MB_CUR_MAX];
	size_t ret = wcrtomb(buf, L'A', NULL);
	ASSERT_EQ((size_t)1, ret);
	ASSERT_EQ('A', buf[0]);
}

TEST(wcrtomb_two_byte) {
	char buf[MB_CUR_MAX];
	size_t ret = wcrtomb(buf, 0xE9, NULL);
	ASSERT_EQ((size_t)2, ret);
	ASSERT_EQ((char)0xC3, buf[0]);
	ASSERT_EQ((char)0xA9, buf[1]);
}

TEST(wcrtomb_three_byte) {
	char buf[MB_CUR_MAX];
	size_t ret = wcrtomb(buf, 0x20AC, NULL);
	ASSERT_EQ((size_t)3, ret);
	ASSERT_EQ((char)0xE2, buf[0]);
}

TEST(wcrtomb_four_byte) {
	char buf[MB_CUR_MAX];
	size_t ret = wcrtomb(buf, 0x1F600, NULL);
	ASSERT_EQ((size_t)4, ret);
	ASSERT_EQ((char)0xF0, buf[0]);
}

TEST(wcrtomb_null_s) {
	size_t ret = wcrtomb(NULL, 0, NULL);
	ASSERT_EQ((size_t)1, ret);
}

TEST(wcrtomb_null_char) {
	char buf[MB_CUR_MAX];
	size_t ret = wcrtomb(buf, L'\0', NULL);
	ASSERT_EQ((size_t)1, ret);
	ASSERT_EQ('\0', buf[0]);
}

TEST(wcrtomb_surrogate_high) {
	char buf[MB_CUR_MAX];
	errno = 0;
	size_t ret = wcrtomb(buf, 0xD800, NULL);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(wcrtomb_surrogate_low) {
	char buf[MB_CUR_MAX];
	errno = 0;
	size_t ret = wcrtomb(buf, 0xDFFF, NULL);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(wcrtomb_out_of_range) {
	char buf[MB_CUR_MAX];
	errno = 0;
	size_t ret = wcrtomb(buf, 0x110000, NULL);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

/* ============================================================================ */

TEST_SUITE(mbtowc)

TEST(mbtowc_basic) {
	wchar_t wc;
	int ret = mbtowc(&wc, "A", 1);
	ASSERT_EQ(1, ret);
	ASSERT_EQ(L'A', wc);
}

TEST(mbtowc_null_s) {
	int ret = mbtowc(NULL, NULL, 0);
	ASSERT_EQ(0, ret);
}

TEST(mbtowc_zero_n) {
	wchar_t wc;
	int ret = mbtowc(&wc, "A", 0);
	ASSERT_EQ(-2, ret);
}

TEST(mbtowc_incomplete) {
	wchar_t wc;
	int ret = mbtowc(&wc, "\xC3", 1);
	ASSERT_EQ(-2, ret);
}

TEST(mbtowc_invalid) {
	wchar_t wc;
	errno = 0;
	int ret = mbtowc(&wc, "\xFF", 1);
	ASSERT_EQ(-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(mbtowc_null_char) {
	wchar_t wc = L'X';
	int ret = mbtowc(&wc, "", 1);
	ASSERT_EQ(0, ret);
	ASSERT_EQ(L'\0', wc);
}

/* ============================================================================ */

TEST_SUITE(wctomb)

TEST(wctomb_basic) {
	char buf[MB_CUR_MAX];
	int ret = wctomb(buf, L'A');
	ASSERT_EQ(1, ret);
	ASSERT_EQ('A', buf[0]);
}

TEST(wctomb_null_s) {
	int ret = wctomb(NULL, L'A');
	ASSERT_EQ(0, ret);
}

TEST(wctomb_null_char) {
	char buf[MB_CUR_MAX];
	int ret = wctomb(buf, L'\0');
	ASSERT_EQ(1, ret);
	ASSERT_EQ('\0', buf[0]);
}

TEST(wctomb_surrogate) {
	char buf[MB_CUR_MAX];
	errno = 0;
	int ret = wctomb(buf, 0xD800);
	ASSERT_EQ(-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(wctomb_out_of_range) {
	char buf[MB_CUR_MAX];
	errno = 0;
	int ret = wctomb(buf, 0x110000);
	ASSERT_EQ(-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

/* ============================================================================ */

TEST_SUITE(mbrlen)

TEST(mbrlen_ascii) {
	ASSERT_EQ((size_t)1, mbrlen("A", 1, NULL));
}

TEST(mbrlen_two_byte) {
	ASSERT_EQ((size_t)2, mbrlen("\xC3\xA9", 2, NULL));
}

TEST(mbrlen_null_s) {
	ASSERT_EQ((size_t)0, mbrlen(NULL, 0, NULL));
}

TEST(mbrlen_zero_n) {
	ASSERT_EQ((size_t)-2, mbrlen("A", 0, NULL));
}

TEST(mbrlen_incomplete) {
	ASSERT_EQ((size_t)-2, mbrlen("\xC3", 1, NULL));
}

TEST(mbrlen_invalid) {
	errno = 0;
	ASSERT_EQ((size_t)-1, mbrlen("\xFF", 1, NULL));
	ASSERT_ERRNO(EILSEQ);
}

TEST(mbrlen_overlong) {
	errno = 0;
	ASSERT_EQ((size_t)-1, mbrlen("\xC0\x80", 2, NULL));
	ASSERT_ERRNO(EILSEQ);
}

TEST(mbrlen_null_char) {
	ASSERT_EQ((size_t)0, mbrlen("", 1, NULL));
}

/* ============================================================================ */

TEST_SUITE(mbsinit)

TEST(mbsinit_null) {
	ASSERT_EQ(1, mbsinit(NULL));
}

TEST(mbsinit_zeroed) {
	mbstate_t st = {0, 0, 0};
	ASSERT_EQ(1, mbsinit(&st));
}

TEST(mbsinit_non_zero) {
	mbstate_t st = {1, 0, 0};
	ASSERT_EQ(0, mbsinit(&st));
}

/* ============================================================================ */
/* MBLEN                                                                        */
/* ============================================================================ */

TEST_SUITE(mblen)

TEST(mblen_basic) {
	ASSERT_EQ(1, mblen("A", 1));
}

TEST(mblen_null) {
	ASSERT_EQ(0, mblen(NULL, 0));
}

TEST(mblen_incomplete) {
	ASSERT_EQ(-2, mblen("\xC3", 1));
}

TEST(mblen_invalid) {
	errno = 0;
	ASSERT_EQ(-1, mblen("\xFF", 1));
	ASSERT_ERRNO(EILSEQ);
}

TEST(mblen_multibyte) {
	ASSERT_EQ(2, mblen("\xC3\xA9", 2)); // é
}

TEST(mblen_null_char) {
	ASSERT_EQ(0, mblen("", 1));
}

/* ============================================================================ */
/* MBSTOWCS                                                                     */
/* ============================================================================ */

TEST_SUITE(mbstowcs)

TEST(mbstowcs_ascii) {
	wchar_t wcs[10];
	size_t ret = mbstowcs(wcs, "Hello", 10);
	ASSERT_EQ((size_t)5, ret);
	ASSERT_EQ(0, wcscmp(wcs, L"Hello"));
}

TEST(mbstowcs_utf8) {
	wchar_t wcs[10];
	size_t ret = mbstowcs(wcs, "Caf\xC3\xA9", 10);
	ASSERT_EQ((size_t)4, ret);
	ASSERT_EQ(L'\xE9', wcs[3]);
}

TEST(mbstowcs_invalid) {
	wchar_t wcs[10];
	errno = 0;
	size_t ret = mbstowcs(wcs, "Bad\xFF", 10);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(mbstowcs_buffer_limit) {
	wchar_t wcs[3];
	size_t ret = mbstowcs(wcs, "Hello", 3);
	ASSERT_EQ((size_t)3, ret);
	ASSERT_EQ(L'H', wcs[0]);
	ASSERT_EQ(L'e', wcs[1]);
	ASSERT_EQ(L'l', wcs[2]);
}

TEST(mbstowcs_null_dst) {
	size_t ret = mbstowcs(NULL, "Hello", 10);
	ASSERT_EQ((size_t)5, ret);
}

TEST(mbstowcs_null_src) {
	wchar_t wcs[10];
	size_t ret = mbstowcs(wcs, NULL, 10);
	ASSERT_EQ((size_t)0, ret);
}

TEST(mbstowcs_zero_n) {
	wchar_t wcs[10];
	size_t ret = mbstowcs(wcs, "Hello", 0);
	ASSERT_EQ((size_t)0, ret);
}

/* ============================================================================ */
/* WCSTOMBS                                                                     */
/* ============================================================================ */

TEST_SUITE(wcstombs)

TEST(wcstombs_ascii) {
	char mbs[20];
	size_t ret = wcstombs(mbs, L"Hello", 20);
	ASSERT_EQ((size_t)5, ret);
	ASSERT_STR_EQ("Hello", mbs);
}

TEST(wcstombs_invalid) {
	char mbs[20];
	errno = 0;
	size_t ret = wcstombs(mbs, L"Bad\xD800", 20);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(wcstombs_buffer_limit) {
	char mbs[3];
	size_t ret = wcstombs(mbs, L"Hello", 3);
	ASSERT_EQ((size_t)3, ret);
}

TEST(wcstombs_null_dst) {
	size_t ret = wcstombs(NULL, L"Hello", 20);
	ASSERT_EQ((size_t)5, ret);
}

TEST(wcstombs_null_src) {
	char mbs[20];
	size_t ret = wcstombs(mbs, NULL, 20);
	ASSERT_EQ((size_t)0, ret);
}

TEST(wcstombs_zero_n) {
	char mbs[20];
	size_t ret = wcstombs(mbs, L"Hello", 0);
	ASSERT_EQ((size_t)0, ret);
}

/* ============================================================================ */

TEST_SUITE(mbsrtowcs)

TEST(mbsrtowcs_basic) {
	wchar_t dst[10];
	const char *src = "Hello";
	const char *src_ptr = src;
	mbstate_t ps = {0};
	size_t ret = mbsrtowcs(dst, &src_ptr, 10, &ps);
	ASSERT_EQ((size_t)5, ret);
	ASSERT_NULL(src_ptr);
	ASSERT_EQ(0, wcscmp(dst, L"Hello"));
}

TEST(mbsrtowcs_null_dst) {
	const char *src = "Hello";
	const char *src_ptr = src;
	mbstate_t ps = {0};
	size_t ret = mbsrtowcs(NULL, &src_ptr, 10, &ps);
	ASSERT_EQ((size_t)5, ret);
	ASSERT_NULL(src_ptr);
}

TEST(mbsrtowcs_invalid) {
	wchar_t dst[10];
	const char *src = "Bad\xFF";
	const char *src_ptr = src;
	mbstate_t ps = {0};
	errno = 0;
	size_t ret = mbsrtowcs(dst, &src_ptr, 10, &ps);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
	ASSERT_EQ(src + 3, src_ptr); // Stops exactly at the bad byte
}

TEST(mbsrtowcs_limit) {
	wchar_t dst[3];
	const char *src = "Hello";
	const char *src_ptr = src;
	mbstate_t ps = {0};
	size_t ret = mbsrtowcs(dst, &src_ptr, 3, &ps);
	ASSERT_EQ((size_t)3, ret);
	ASSERT_EQ(src + 3, src_ptr); // Points exactly to the first 'l'
}

TEST(mbsrtowcs_null_state) {
	wchar_t dst[10];
	const char *src = "Hello";
	const char *src_ptr = src;
	// POSIX: If ps is NULL, uses internal thread-local state
	size_t ret = mbsrtowcs(dst, &src_ptr, 10, NULL);
	ASSERT_EQ((size_t)5, ret);
	ASSERT_NULL(src_ptr);
}

TEST(mbsrtowcs_exact_fit) {
	wchar_t dst[6];
	const char *src = "Hello";
	const char *src_ptr = src;
	mbstate_t ps = {0};
	size_t ret = mbsrtowcs(dst, &src_ptr, 6, &ps);
	ASSERT_EQ((size_t)5, ret);
	ASSERT_NULL(src_ptr);
	ASSERT_EQ(L'\0', dst[5]); // Null terminator fits exactly
}

/* ============================================================================ */

TEST_SUITE(wcsrtombs)

TEST(wcsrtombs_basic) {
	char dst[10];
	const wchar_t *src = L"Hello";
	const wchar_t *src_ptr = src;
	mbstate_t ps = {0};
	size_t ret = wcsrtombs(dst, &src_ptr, 10, &ps);
	ASSERT_EQ((size_t)5, ret);
	ASSERT_NULL(src_ptr);
	ASSERT_STR_EQ("Hello", dst);
}

TEST(wcsrtombs_null_dst) {
	const wchar_t *src = L"Hello";
	const wchar_t *src_ptr = src;
	mbstate_t ps = {0};
	size_t ret = wcsrtombs(NULL, &src_ptr, 10, &ps);
	ASSERT_EQ((size_t)5, ret);
	ASSERT_NULL(src_ptr);
}

TEST(wcsrtombs_invalid) {
	char dst[10];
	const wchar_t *src = L"Bad\xD800";
	const wchar_t *src_ptr = src;
	mbstate_t ps = {0};
	errno = 0;
	size_t ret = wcsrtombs(dst, &src_ptr, 10, &ps);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
	ASSERT_EQ(src + 3, src_ptr);
}

TEST(wcsrtombs_limit) {
	char dst[3];
	const wchar_t *src = L"Hello";
	const wchar_t *src_ptr = src;
	mbstate_t ps = {0};
	size_t ret = wcsrtombs(dst, &src_ptr, 3, &ps);
	ASSERT_EQ((size_t)3, ret);
	ASSERT_EQ(src + 3, src_ptr); // Points exactly to the second 'l'
}

TEST(wcsrtombs_null_state) {
	char dst[10];
	const wchar_t *src = L"Hello";
	const wchar_t *src_ptr = src;
	// POSIX: If ps is NULL, uses internal thread-local state
	size_t ret = wcsrtombs(dst, &src_ptr, 10, NULL);
	ASSERT_EQ((size_t)5, ret);
	ASSERT_NULL(src_ptr);
}

/* ============================================================================ */

TEST_SUITE(mbsnrtowcs)

TEST(mbsnrtowcs_basic) {
	wchar_t dst[10];
	const char *src = "Hello";
	const char *src_ptr = src;
	mbstate_t ps = {0};
	size_t ret = mbsnrtowcs(dst, &src_ptr, 3, 10, &ps);
	ASSERT_EQ((size_t)3, ret);
	ASSERT_EQ(src + 3, src_ptr);
	ASSERT_EQ(L'H', dst[0]);
	ASSERT_EQ(L'e', dst[1]);
	ASSERT_EQ(L'l', dst[2]);
}

TEST(mbsnrtowcs_null_dst) {
	const char *src = "Hello";
	const char *src_ptr = src;
	mbstate_t ps = {0};
	size_t ret = mbsnrtowcs(NULL, &src_ptr, 3, 10, &ps);
	ASSERT_EQ((size_t)3, ret);
	ASSERT_EQ(src + 3, src_ptr);
}

TEST(mbsnrtowcs_null_state) {
	wchar_t dst[10];
	const char *src = "Hello";
	const char *src_ptr = src;
	// POSIX: NULL state uses an internal mbstate_t
	size_t ret = mbsnrtowcs(dst, &src_ptr, 3, 10, NULL);
	ASSERT_EQ((size_t)3, ret);
	ASSERT_EQ(src + 3, src_ptr);
}

TEST(mbsnrtowcs_invalid) {
	wchar_t dst[10];
	const char *src = "Bad\xFF";
	const char *src_ptr = src;
	mbstate_t ps = {0};
	errno = 0;
	size_t ret = mbsnrtowcs(dst, &src_ptr, 10, 10, &ps);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(mbsnrtowcs_exact_fit) {
	wchar_t dst[6];
	const char *src = "Hello";
	const char *src_ptr = src;
	mbstate_t ps = {0};
	// nms=10 is large enough, len=6 fits the 5 chars + null terminator
	size_t ret = mbsnrtowcs(dst, &src_ptr, 10, 6, &ps);
	ASSERT_EQ((size_t)5, ret);
	ASSERT_NULL(src_ptr); // Reached null terminator
	ASSERT_EQ(L'\0', dst[5]);
}

TEST(mbsnrtowcs_dst_limit) {
	wchar_t dst[3];
	const char *src = "Hello";
	const char *src_ptr = src;
	mbstate_t ps = {0};
	// len=3 stops the conversion before nms=10 is exhausted
	size_t ret = mbsnrtowcs(dst, &src_ptr, 10, 3, &ps);
	ASSERT_EQ((size_t)3, ret);
	ASSERT_EQ(src + 3, src_ptr); // Points to the first unconverted byte ('l')
}

TEST(mbsnrtowcs_multibyte) {
	wchar_t dst[10];
	const char *src = "Caf\xC3\xA9"; // 5 bytes, 4 wide chars
	const char *src_ptr = src;
	mbstate_t ps = {0};
	size_t ret = mbsnrtowcs(dst, &src_ptr, 5, 10, &ps);
	ASSERT_EQ((size_t)4, ret);
	ASSERT_NULL(src_ptr);
	ASSERT_EQ(L'\xE9', dst[3]);
}

/* ============================================================================ */

TEST_SUITE(wcsnrtombs)

TEST(wcsnrtombs_basic) {
	char dst[10];
	const wchar_t *src = L"Hello";
	const wchar_t *src_ptr = src;
	mbstate_t ps = {0};
	size_t ret = wcsnrtombs(dst, &src_ptr, 3, 10, &ps);
	ASSERT_EQ((size_t)3, ret);
	ASSERT_EQ(src + 3, src_ptr);
	ASSERT_EQ('H', dst[0]);
	ASSERT_EQ('e', dst[1]);
	ASSERT_EQ('l', dst[2]);
}

TEST(wcsnrtombs_invalid) {
	char dst[10];
	const wchar_t *src = L"Bad\xD800";
	const wchar_t *src_ptr = src;
	mbstate_t ps = {0};
	errno = 0;
	size_t ret = wcsnrtombs(dst, &src_ptr, 10, 10, &ps);
	ASSERT_EQ((size_t)-1, ret);
	ASSERT_ERRNO(EILSEQ);
}

TEST(wcsnrtombs_null_dst) {
	const wchar_t *src = L"Hello";
	const wchar_t *src_ptr = src;
	mbstate_t ps = {0};
	size_t ret = wcsnrtombs(NULL, &src_ptr, 3, 10, &ps);
	ASSERT_EQ((size_t)3, ret);
	ASSERT_EQ(src + 3, src_ptr);
}

TEST(wcsnrtombs_null_state) {
	char dst[10];
	const wchar_t *src = L"Hello";
	const wchar_t *src_ptr = src;
	size_t ret = wcsnrtombs(dst, &src_ptr, 3, 10, NULL);
	ASSERT_EQ((size_t)3, ret);
	ASSERT_EQ(src + 3, src_ptr);
}

TEST(wcsnrtombs_exact_fit) {
	char dst[6];
	const wchar_t *src = L"Hello";
	const wchar_t *src_ptr = src;
	mbstate_t ps = {0};
	// nwc=10 is large enough, len=6 fits the 5 bytes + null terminator
	size_t ret = wcsnrtombs(dst, &src_ptr, 10, 6, &ps);
	ASSERT_EQ((size_t)5, ret);
	ASSERT_NULL(src_ptr); // Reached null wide character
	ASSERT_EQ('\0', dst[5]);
}

TEST(wcsnrtombs_dst_limit) {
	char dst[3];
	const wchar_t *src = L"Hello";
	const wchar_t *src_ptr = src;
	mbstate_t ps = {0};
	// len=3 stops the conversion before nwc=10 is exhausted
	size_t ret = wcsnrtombs(dst, &src_ptr, 10, 3, &ps);
	ASSERT_EQ((size_t)3, ret);
	ASSERT_EQ(src + 3, src_ptr); // Points to the first unconverted character ('l')
}

TEST(wcsnrtombs_nwc_limit) {
	char dst[10];
	const wchar_t *src = L"Hello";
	const wchar_t *src_ptr = src;
	mbstate_t ps = {0};
	// nwc=2 stops the conversion before len=10 is exhausted
	size_t ret = wcsnrtombs(dst, &src_ptr, 2, 10, &ps);
	ASSERT_EQ((size_t)2, ret);
	ASSERT_EQ(src + 2, src_ptr);
	ASSERT_EQ('H', dst[0]);
	ASSERT_EQ('e', dst[1]);
}

TEST(wcsnrtombs_multibyte) {
	char dst[10];
	const wchar_t *src = L"Caf\xE9"; // 4 wide chars, 5 bytes
	const wchar_t *src_ptr = src;
	mbstate_t ps = {0};
	size_t ret = wcsnrtombs(dst, &src_ptr, 4, 10, &ps);
	ASSERT_EQ((size_t)5, ret);
	ASSERT_NULL(src_ptr);
	ASSERT_EQ((char)0xC3, dst[3]);
	ASSERT_EQ((char)0xA9, dst[4]);
}

/* ============================================================================ */

TEST_SUITE(wmemcpy)

TEST(wmemcpy_basic) {
	wchar_t src[] = L"Test";
	wchar_t dst[10];
	wchar_t *ret = wmemcpy(dst, src, 4);
	ASSERT_EQ(dst, ret);
	ASSERT_EQ(0, wmemcmp(dst, L"Test", 4));
}

TEST(wmemcpy_zero) {
	wchar_t src[] = L"Test";
	wchar_t dst[10] = {0};
	wchar_t *ret = wmemcpy(dst, src, 0);
	ASSERT_EQ(dst, ret);
	ASSERT_EQ(L'\0', dst[0]);
}

TEST(wmemcpy_null_dst) {
	ASSERT_NULL(wmemcpy(NULL, L"Test", 4));
}

TEST(wmemcpy_null_src) {
	wchar_t dst[10];
	ASSERT_NULL(wmemcpy(dst, NULL, 4));
}

/* ============================================================================ */


TEST_SUITE(wmemmove)

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

TEST(wmemmove_zero) {
	wchar_t buf[] = L"Hello";
	wmemmove(buf, buf, 0);
	ASSERT_EQ(L'H', buf[0]);
}

TEST(wmemmove_null_dst) {
	ASSERT_NULL(wmemmove(NULL, L"Test", 4));
}

TEST(wmemmove_null_src) {
	wchar_t dst[10];
	ASSERT_NULL(wmemmove(dst, NULL, 4));
}

/* ============================================================================ */
/* WMEMCHR                                                                      */
/* ============================================================================ */

TEST_SUITE(wmemchr)

TEST(wmemchr_found) {
	wchar_t str[] = L"Hello";
	wchar_t *ret = wmemchr(str, L'l', 5);
	ASSERT_NOT_NULL(ret);
	ASSERT_EQ(L'l', *ret);
}

TEST(wmemchr_not_found) {
	wchar_t str[] = L"Hello";
	ASSERT_NULL(wmemchr(str, L'x', 5));
}

TEST(wmemchr_zero_n) {
	wchar_t str[] = L"Hello";
	ASSERT_NULL(wmemchr(str, L'H', 0));
}

TEST(wmemchr_null_s) {
	ASSERT_NULL(wmemchr(NULL, L'H', 5));
}

TEST(wmemchr_first) {
	wchar_t str[] = L"Hello";
	wchar_t *ret = wmemchr(str, L'H', 5);
	ASSERT_NOT_NULL(ret);
	ASSERT_EQ(str, ret);
}

TEST(wmemchr_last) {
	wchar_t str[] = L"Hello";
	wchar_t *ret = wmemchr(str, L'o', 5);
	ASSERT_NOT_NULL(ret);
	ASSERT_EQ(L'o', *ret);
}

/* ============================================================================ */
/* WMEMCMP                                                                      */
/* ============================================================================ */

TEST_SUITE(wmemcmp)

TEST(wmemcmp_equal) {
	ASSERT_EQ(0, wmemcmp(L"Test", L"Test", 4));
}

TEST(wmemcmp_less) {
	ASSERT_TRUE(wmemcmp(L"Test", L"Zest", 4) < 0);
}

TEST(wmemcmp_greater) {
	ASSERT_TRUE(wmemcmp(L"Zest", L"Test", 4) > 0);
}

TEST(wmemcmp_zero_n) {
	ASSERT_EQ(0, wmemcmp(L"Test", L"Zest", 0));
}

TEST(wmemcmp_null_a) {
	ASSERT_NE(0, wmemcmp(NULL, L"Test", 4));
}

TEST(wmemcmp_null_b) {
	ASSERT_NE(0, wmemcmp(L"Test", NULL, 4));
}

/* ============================================================================ */
/* WMEMSET                                                                      */
/* ============================================================================ */

TEST_SUITE(wmemset)

TEST(wmemset_basic) {
	wchar_t buf[5];
	wmemset(buf, L'A', 5);
	ASSERT_EQ(L'A', buf[0]);
	ASSERT_EQ(L'A', buf[4]);
}

TEST(wmemset_zero) {
	wchar_t buf[5] = {0};
	wmemset(buf, L'A', 0);
	ASSERT_EQ(L'\0', buf[0]);
}

TEST(wmemset_null_s) {
	ASSERT_NULL(wmemset(NULL, L'A', 5));
}

TEST(wmemset_null_char) {
	wchar_t buf[5];
	wmemset(buf, L'\0', 5);
	ASSERT_EQ(L'\0', buf[0]);
	ASSERT_EQ(L'\0', buf[4]);
}

/* ============================================================================ */
/* WCSLEN                                                                       */
/* ============================================================================ */

TEST_SUITE(wcslen)

TEST(wcslen_basic) {
	ASSERT_EQ((size_t)5, wcslen(L"Hello"));
}

TEST(wcslen_empty) {
	ASSERT_EQ((size_t)0, wcslen(L""));
}

TEST(wcslen_null) {
	ASSERT_EQ((size_t)0, wcslen(NULL));
}

TEST(wcslen_unicode) {
	ASSERT_EQ((size_t)4, wcslen(L"Caf\xE9"));
}

/* ============================================================================ */
/* WCSNLEN                                                                      */
/* ============================================================================ */

TEST_SUITE(wcsnlen)

TEST(wcsnlen_basic) {
	ASSERT_EQ((size_t)3, wcsnlen(L"Hello", 3));
}

TEST(wcsnlen_short) {
	ASSERT_EQ((size_t)5, wcsnlen(L"Hello", 10));
}

TEST(wcsnlen_zero) {
	ASSERT_EQ((size_t)0, wcsnlen(L"Hello", 0));
}

TEST(wcsnlen_null) {
	ASSERT_EQ((size_t)0, wcsnlen(NULL, 10));
}

/* ============================================================================ */
/* WCSCPY                                                                       */
/* ============================================================================ */

TEST_SUITE(wcscpy)

TEST(wcscpy_basic) {
	wchar_t dst[10];
	wcscpy(dst, L"Test");
	ASSERT_EQ(0, wcscmp(dst, L"Test"));
}

TEST(wcscpy_empty) {
	wchar_t dst[10] = L"X";
	wcscpy(dst, L"");
	ASSERT_EQ(L'\0', dst[0]);
}

TEST(wcscpy_null_dst) {
	ASSERT_NULL(wcscpy(NULL, L"Test"));
}

TEST(wcscpy_null_src) {
	wchar_t dst[10];
	ASSERT_NULL(wcscpy(dst, NULL));
}

/* ============================================================================ */
/* WCSNCPY                                                                      */
/* ============================================================================ */

TEST_SUITE(wcsncpy)

TEST(wcsncpy_basic) {
	wchar_t dst[10] = {0};
	wcsncpy(dst, L"Hello", 3);
	ASSERT_EQ(0, wcsncmp(dst, L"Hel", 3));
}

TEST(wcsncpy_pads_nulls) {
	wchar_t dst[10];
	wcsncpy(dst, L"Hi", 10);
	ASSERT_EQ(L'\0', dst[2]);
	ASSERT_EQ(L'\0', dst[9]);
}

TEST(wcsncpy_no_null) {
	wchar_t dst[3];
	wcsncpy(dst, L"Hello", 3);
	ASSERT_EQ(L'H', dst[0]);
	ASSERT_EQ(L'e', dst[1]);
	ASSERT_EQ(L'l', dst[2]);
}

TEST(wcsncpy_zero_n) {
	wchar_t dst[10] = L"XXX";
	wcsncpy(dst, L"Hello", 0);
	ASSERT_EQ(L'X', dst[0]);
}

TEST(wcsncpy_null_dst) {
	ASSERT_NULL(wcsncpy(NULL, L"Test", 4));
}

TEST(wcsncpy_null_src) {
	wchar_t dst[10];
	ASSERT_NULL(wcsncpy(dst, NULL, 4));
}

TEST(wcsncpy_no_termination) {
	wchar_t dst[6] = L"XXXXXX";
	wcsncpy(dst, L"Hello", 5);
	// POSIX: If n <= wcslen(src), dst is explicitly NOT null-terminated
	ASSERT_EQ(L'H', dst[0]);
	ASSERT_EQ(L'o', dst[4]);
	ASSERT_EQ(L'X', dst[5]); // Original buffer untouched
}

/* ============================================================================ */

TEST_SUITE(wcpcpy)

TEST(wcpcpy_basic) {
	wchar_t dst[10];
	wchar_t *ret = wcpcpy(dst, L"Test");
	ASSERT_EQ(dst + 4, ret);
	ASSERT_EQ(L'\0', *ret);
	ASSERT_EQ(0, wcscmp(dst, L"Test"));
}

TEST(wcpcpy_empty) {
	wchar_t dst[10] = L"X";
	wchar_t *ret = wcpcpy(dst, L"");
	ASSERT_EQ(dst, ret);
	ASSERT_EQ(L'\0', *ret);
}

TEST(wcpcpy_null_dst) {
	ASSERT_NULL(wcpcpy(NULL, L"Test"));
}

TEST(wcpcpy_null_src) {
	wchar_t dst[10];
	ASSERT_NULL(wcpcpy(dst, NULL));
}

/* ============================================================================ */

TEST_SUITE(wcpncpy)

TEST(wcpncpy_basic) {
	wchar_t dst[10] = {0};
	wchar_t *ret = wcpncpy(dst, L"Hello", 3);
	ASSERT_EQ(dst + 3, ret);
	ASSERT_EQ(L'H', dst[0]);
	ASSERT_EQ(L'e', dst[1]);
	ASSERT_EQ(L'l', dst[2]);
}

TEST(wcpncpy_pads_nulls) {
	wchar_t dst[10] = {0};
	wchar_t *ret = wcpncpy(dst, L"Hi", 5);
	ASSERT_EQ(dst + 2, ret);
	ASSERT_EQ(L'\0', dst[2]);
	ASSERT_EQ(L'\0', dst[4]);
}

TEST(wcpncpy_zero_n) {
	wchar_t dst[10] = L"XXXXXXXXX";
	wchar_t *ret = wcpncpy(dst, L"Hello", 0);
	ASSERT_EQ(dst, ret);
	ASSERT_EQ(L'X', dst[0]);
}

TEST(wcpncpy_exact) {
	wchar_t dst[5];
	wchar_t *ret = wcpncpy(dst, L"Hello", 5);
	ASSERT_EQ(dst + 5, ret);
	ASSERT_EQ(L'o', dst[4]);
}

TEST(wcpncpy_null_dst) {
	ASSERT_NULL(wcpncpy(NULL, L"Test", 4));
}

TEST(wcpncpy_null_src) {
	wchar_t dst[10];
	ASSERT_NULL(wcpncpy(dst, NULL, 4));
}

/* ============================================================================ */

TEST_SUITE(wcslcpy)

TEST(wcslcpy_basic) {
	wchar_t dst[5];
	size_t ret = wcslcpy(dst, L"Hello World", 5);
	ASSERT_EQ((size_t)11, ret);
	ASSERT_EQ(0, wcscmp(dst, L"Hell"));
}

TEST(wcslcpy_short) {
	wchar_t dst[10];
	size_t ret = wcslcpy(dst, L"Hi", 10);
	ASSERT_EQ((size_t)2, ret);
	ASSERT_EQ(0, wcscmp(dst, L"Hi"));
}

TEST(wcslcpy_zero_dstsize) {
	wchar_t dst[5] = L"XXXX";
	size_t ret = wcslcpy(dst, L"Hello", 0);
	ASSERT_EQ((size_t)5, ret);
	ASSERT_EQ(L'X', dst[0]); // Buffer untouched
}

TEST(wcslcpy_exact_fit) {
	wchar_t dst[6];
	size_t ret = wcslcpy(dst, L"Hello", 6);
	ASSERT_EQ((size_t)5, ret);
	ASSERT_EQ(0, wcscmp(dst, L"Hello"));
}

/* ============================================================================ */

TEST_SUITE(wcslcat)

TEST(wcslcat_basic) {
	wchar_t dst[10] = L"Hello";
	size_t ret = wcslcat(dst, L" World", 10);
	ASSERT_EQ((size_t)11, ret);
	ASSERT_EQ(0, wcscmp(dst, L"Hello Wor"));
}

TEST(wcslcat_zero_dstsize) {
	wchar_t dst[10] = L"Hello";
	size_t ret = wcslcat(dst, L" World", 0);
	ASSERT_EQ((size_t)6, ret); // POSIX: 0 + wcslen(src)
	ASSERT_EQ(0, wcscmp(dst, L"Hello"));
}

TEST(wcslcat_empty_src) {
	wchar_t dst[10] = L"Hello";
	size_t ret = wcslcat(dst, L"", 10);
	ASSERT_EQ((size_t)5, ret);
	ASSERT_EQ(0, wcscmp(dst, L"Hello"));
}

TEST(wcslcat_exact_fit) {
	wchar_t dst[12] = L"Hello";
	size_t ret = wcslcat(dst, L" World", 12);
	ASSERT_EQ((size_t)11, ret);
	ASSERT_EQ(0, wcscmp(dst, L"Hello World"));
}

/* ============================================================================ */

TEST_SUITE(wcsdup)

TEST(wcsdup_basic) {
	wchar_t *dup = wcsdup(L"Test");
	ASSERT_NOT_NULL(dup);
	ASSERT_EQ(0, wcscmp(dup, L"Test"));
	free(dup);
}

TEST(wcsdup_empty) {
	wchar_t *dup = wcsdup(L"");
	ASSERT_NOT_NULL(dup);
	ASSERT_EQ(L'\0', dup[0]);
	free(dup);
}

TEST(wcsdup_unicode) {
	wchar_t *dup = wcsdup(L"Caf\xE9");
	ASSERT_NOT_NULL(dup);
	ASSERT_EQ(0, wcscmp(dup, L"Caf\xE9"));
	free(dup);
}

TEST(wcsdup_null) {
	ASSERT_NULL(wcsdup(NULL));
}

/* ============================================================================ */

TEST_SUITE(wcscmp)

TEST(wcscmp_equal) {
	ASSERT_EQ(0, wcscmp(L"Test", L"Test"));
}

TEST(wcscmp_less) {
	ASSERT_TRUE(wcscmp(L"Apple", L"Banana") < 0);
}

TEST(wcscmp_greater) {
	ASSERT_TRUE(wcscmp(L"Zebra", L"Apple") > 0);
}

TEST(wcscmp_prefix) {
	ASSERT_TRUE(wcscmp(L"App", L"Apple") < 0);
}

TEST(wcscmp_null_a) {
	ASSERT_NE(0, wcscmp(NULL, L"Test"));
}

TEST(wcscmp_null_b) {
	ASSERT_NE(0, wcscmp(L"Test", NULL));
}

/* ============================================================================ */
/* WCSNCMP                                                                      */
/* ============================================================================ */

TEST_SUITE(wcsncmp)

TEST(wcsncmp_partial) {
	ASSERT_EQ(0, wcsncmp(L"Hello", L"Help", 3));
}

TEST(wcsncmp_full) {
	ASSERT_NE(0, wcsncmp(L"Hello", L"Help", 4));
}

TEST(wcsncmp_zero) {
	ASSERT_EQ(0, wcsncmp(L"Hello", L"Help", 0));
}

TEST(wcsncmp_null_a) {
	ASSERT_NE(0, wcsncmp(NULL, L"Test", 4));
}

TEST(wcsncmp_null_b) {
	ASSERT_NE(0, wcsncmp(L"Test", NULL, 4));
}

/* ============================================================================ */
/* WCSCAT                                                                       */
/* ============================================================================ */

TEST_SUITE(wcscat)

TEST(wcscat_basic) {
	wchar_t dst[20] = L"Hello";
	wcscat(dst, L" World");
	ASSERT_EQ(0, wcscmp(dst, L"Hello World"));
}

TEST(wcscat_empty_src) {
	wchar_t dst[20] = L"Hello";
	wcscat(dst, L"");
	ASSERT_EQ(0, wcscmp(dst, L"Hello"));
}

TEST(wcscat_empty_dst) {
	wchar_t dst[20] = L"";
	wcscat(dst, L"Hello");
	ASSERT_EQ(0, wcscmp(dst, L"Hello"));
}

TEST(wcscat_null_dst) {
	ASSERT_NULL(wcscat(NULL, L"Test"));
}

TEST(wcscat_null_src) {
	wchar_t dst[20] = L"Hello";
	ASSERT_NULL(wcscat(dst, NULL));
}

/* ============================================================================ */
/* WCSNCAT                                                                      */
/* ============================================================================ */

TEST_SUITE(wcsncat)

TEST(wcsncat_basic) {
	wchar_t dst[20] = L"Hello";
	wcsncat(dst, L" World", 3);
	ASSERT_EQ(0, wcscmp(dst, L"Hello Wo"));
}

TEST(wcsncat_zero) {
	wchar_t dst[20] = L"Hello";
	wcsncat(dst, L" World", 0);
	ASSERT_EQ(0, wcscmp(dst, L"Hello"));
}

TEST(wcsncat_null_dst) {
	ASSERT_NULL(wcsncat(NULL, L"Test", 4));
}

TEST(wcsncat_null_src) {
	wchar_t dst[20] = L"Hello";
	ASSERT_NULL(wcsncat(dst, NULL, 4));
}

/* ============================================================================ */
/* WCSCHR                                                                       */
/* ============================================================================ */

TEST_SUITE(wcschr)

TEST(wcschr_found) {
	wchar_t *ret = wcschr(L"Hello", L'l');
	ASSERT_NOT_NULL(ret);
	ASSERT_EQ(L'l', *ret);
}

TEST(wcschr_not_found) {
	ASSERT_NULL(wcschr(L"Hello", L'x'));
}

TEST(wcschr_null_char) {
	wchar_t *ret = wcschr(L"Hello", L'\0');
	ASSERT_NOT_NULL(ret);
	ASSERT_EQ(L'\0', *ret);
}

TEST(wcschr_null_s) {
	ASSERT_NULL(wcschr(NULL, L'H'));
}

TEST(wcschr_first) {
	wchar_t *ret = wcschr(L"Hello", L'H');
	ASSERT_NOT_NULL(ret);
	ASSERT_EQ(L'H', *ret);
}

TEST(wcschr_last) {
	wchar_t *ret = wcschr(L"Hello", L'o');
	ASSERT_NOT_NULL(ret);
	ASSERT_EQ(L'o', *ret);
}

/* ============================================================================ */
/* WCSRCHR                                                                      */
/* ============================================================================ */

TEST_SUITE(wcsrchr)

TEST(wcsrchr_last_occurrence) {
	wchar_t *ret = wcsrchr(L"Hello", L'l');
	ASSERT_NOT_NULL(ret);
	ASSERT_EQ(L'o', *(ret + 1));
}

TEST(wcsrchr_not_found) {
	ASSERT_NULL(wcsrchr(L"Hello", L'x'));
}

TEST(wcsrchr_null_char) {
	wchar_t *ret = wcsrchr(L"Hello", L'\0');
	ASSERT_NOT_NULL(ret);
	ASSERT_EQ(L'\0', *ret);
}

TEST(wcsrchr_null_s) {
	ASSERT_NULL(wcsrchr(NULL, L'H'));
}

TEST(wcsrchr_first) {
	wchar_t *ret = wcsrchr(L"Hello", L'H');
	ASSERT_NOT_NULL(ret);
	ASSERT_EQ(L'H', *ret);
}

/* ============================================================================ */
/* WCSCSPN                                                                      */
/* ============================================================================ */

TEST_SUITE(wcscspn)

TEST(wcscspn_basic) {
	ASSERT_EQ((size_t)5, wcscspn(L"Hello", L"xy"));
}

TEST(wcscspn_match) {
	ASSERT_EQ((size_t)2, wcscspn(L"Hello", L"l"));
}

TEST(wcscspn_first) {
	ASSERT_EQ((size_t)0, wcscspn(L"Hello", L"H"));
}

TEST(wcscspn_null_s1) {
	ASSERT_EQ((size_t)0, wcscspn(NULL, L"xy"));
}

TEST(wcscspn_null_s2) {
	ASSERT_EQ((size_t)5, wcscspn(L"Hello", NULL));
}

/* ============================================================================ */
/* WCSSPN                                                                       */
/* ============================================================================ */

TEST_SUITE(wcsspn)

TEST(wcsspn_basic) {
	ASSERT_EQ((size_t)2, wcsspn(L"Hello", L"He"));
}

TEST(wcsspn_none) {
	ASSERT_EQ((size_t)0, wcsspn(L"Hello", L"xyz"));
}

TEST(wcsspn_all) {
	ASSERT_EQ((size_t)5, wcsspn(L"Hello", L"Helo"));
}

TEST(wcsspn_null_s1) {
	ASSERT_EQ((size_t)0, wcsspn(NULL, L"xy"));
}

TEST(wcsspn_null_s2) {
	ASSERT_EQ((size_t)0, wcsspn(L"Hello", NULL));
}

/* ============================================================================ */
/* WCSPBRK                                                                      */
/* ============================================================================ */

TEST_SUITE(wcspbrk)

TEST(wcspbrk_found) {
	ASSERT_NOT_NULL(wcspbrk(L"Hello", L"lo"));
}

TEST(wcspbrk_not_found) {
	ASSERT_NULL(wcspbrk(L"Hello", L"xyz"));
}

TEST(wcspbrk_null_s1) {
	ASSERT_NULL(wcspbrk(NULL, L"lo"));
}

TEST(wcspbrk_null_s2) {
	ASSERT_NULL(wcspbrk(L"Hello", NULL));
}

TEST(wcspbrk_first) {
	wchar_t *ret = wcspbrk(L"Hello", L"H");
	ASSERT_NOT_NULL(ret);
	ASSERT_EQ(L'H', *ret);
}

/* ============================================================================ */
/* WCSSTR                                                                       */
/* ============================================================================ */

TEST_SUITE(wcsstr)

TEST(wcsstr_found) {
	wchar_t *ret = wcsstr(L"Hello World", L"World");
	ASSERT_NOT_NULL(ret);
	ASSERT_EQ(0, wcscmp(ret, L"World"));
}

TEST(wcsstr_not_found) {
	ASSERT_NULL(wcsstr(L"Hello", L"xyz"));
}

TEST(wcsstr_empty_needle) {
	wchar_t str[] = L"Test";
	ASSERT_EQ(str, wcsstr(str, L""));
}

TEST(wcsstr_null_haystack) {
	ASSERT_NULL(wcsstr(NULL, L"World"));
}

TEST(wcsstr_null_needle) {
	wchar_t str[] = L"Test";
	ASSERT_NULL(wcsstr(str, NULL));
}

TEST(wcsstr_self) {
	wchar_t str[] = L"Test";
	ASSERT_EQ(str, wcsstr(str, L"Test"));
}

TEST(wcsstr_needle_longer) {
	ASSERT_NULL(wcsstr(L"Hi", L"Hello"));
}

TEST(wcsstr_multiple_matches) {
	wchar_t *ret = wcsstr(L"abcabc", L"abc");
	ASSERT_NOT_NULL(ret);
	// Must return a pointer to the FIRST occurrence
	ASSERT_EQ(L'a', ret[0]);
	ASSERT_EQ(L'b', ret[1]);
	ASSERT_EQ(L'c', ret[2]);
	ASSERT_EQ(L'a', ret[3]);
}

/* ============================================================================ */

TEST_SUITE(wcstok)

TEST(wcstok_basic) {
	wchar_t str[] = L"Hello,World";
	wchar_t *ptr;
	wchar_t *tok1 = wcstok(str, L",", &ptr);
	ASSERT_NOT_NULL(tok1);
	ASSERT_EQ(0, wcscmp(tok1, L"Hello"));

	wchar_t *tok2 = wcstok(NULL, L",", &ptr);
	ASSERT_NOT_NULL(tok2);
	ASSERT_EQ(0, wcscmp(tok2, L"World"));

	wchar_t *tok3 = wcstok(NULL, L",", &ptr);
	ASSERT_NULL(tok3);
}

TEST(wcstok_consecutive_delims) {
	wchar_t str[] = L"a,,b";
	wchar_t *ptr;
	wchar_t *tok1 = wcstok(str, L",", &ptr);
	ASSERT_NOT_NULL(tok1);
	ASSERT_EQ(0, wcscmp(tok1, L"a"));

	wchar_t *tok2 = wcstok(NULL, L",", &ptr);
	ASSERT_NOT_NULL(tok2);
	ASSERT_EQ(0, wcscmp(tok2, L"b"));
}

TEST(wcstok_empty) {
	wchar_t str[] = L",,,";
	wchar_t *ptr;
	ASSERT_NULL(wcstok(str, L",", &ptr));
}

TEST(wcstok_null_ptr) {
	wchar_t str[] = L"Hello";
	ASSERT_NULL(wcstok(str, L",", NULL));
}

TEST(wcstok_delim_change) {
	wchar_t str[] = L"a,b-c";
	wchar_t *ptr;
	ASSERT_EQ(0, wcscmp(wcstok(str, L",", &ptr), L"a"));
	ASSERT_EQ(0, wcscmp(wcstok(NULL, L"-", &ptr), L"b")); // Change delimiter mid-stream
	ASSERT_EQ(0, wcscmp(wcstok(NULL, L"-", &ptr), L"c"));
}

TEST(wcstok_empty_delim) {
	wchar_t str[] = L"Hello World";
	wchar_t *ptr;
	// An empty delimiter string means no characters are delimiters.
	// The entire string is returned as a single token.
	wchar_t *tok = wcstok(str, L"", &ptr);
	ASSERT_NOT_NULL(tok);
	ASSERT_EQ(0, wcscmp(tok, L"Hello World"));
	ASSERT_NULL(wcstok(NULL, L"", &ptr));
}

TEST(wcstok_leading_trailing) {
	wchar_t str[] = L",a,b,";
	wchar_t *ptr;
	ASSERT_EQ(0, wcscmp(wcstok(str, L",", &ptr), L"a"));
	ASSERT_EQ(0, wcscmp(wcstok(NULL, L",", &ptr), L"b"));
	ASSERT_NULL(wcstok(NULL, L",", &ptr));
}

/* ============================================================================ */

TEST_SUITE(wcscoll)

TEST(wcscoll_basic) {
	ASSERT_EQ(0, wcscoll(L"Test", L"Test"));
	ASSERT_TRUE(wcscoll(L"Apple", L"Banana") < 0);
}

TEST(wcscoll_null_a) {
	ASSERT_NE(0, wcscoll(NULL, L"Test"));
}

TEST(wcscoll_null_b) {
	ASSERT_NE(0, wcscoll(L"Test", NULL));
}

/* ============================================================================ */

TEST_SUITE(wcscoll_l)

TEST(wcscoll_l_basic) {
	ASSERT_EQ(0, wcscoll_l(L"Test", L"Test", __jacl_locale_current));
	ASSERT_TRUE(wcscoll_l(L"Apple", L"Banana", __jacl_locale_current) < 0);
}

TEST(wcscoll_l_empty) {
	ASSERT_EQ(0, wcscoll_l(L"", L"", __jacl_locale_current));
	ASSERT_TRUE(wcscoll_l(L"", L"A", __jacl_locale_current) < 0);
}

/* ============================================================================ */

TEST_SUITE(wcsxfrm)

TEST(wcsxfrm_basic) {
	wchar_t dst[20];
	size_t len = wcsxfrm(dst, L"Hello", 20);
	ASSERT_EQ((size_t)5, len);
	ASSERT_EQ(0, wcscmp(dst, L"Hello"));
}

TEST(wcsxfrm_buffer_limit) {
	wchar_t dst[3];
	size_t len = wcsxfrm(dst, L"Hello", 3);
	ASSERT_EQ((size_t)5, len);
}

TEST(wcsxfrm_null_dst) {
	size_t len = wcsxfrm(NULL, L"Hello", 20);
	ASSERT_EQ((size_t)5, len);
}

TEST(wcsxfrm_null_src) {
	wchar_t dst[20];
	size_t len = wcsxfrm(dst, NULL, 20);
	ASSERT_EQ((size_t)0, len);
}

/* ============================================================================ */

TEST_SUITE(wcsxfrm_l)

TEST(wcsxfrm_l_basic) {
	wchar_t dst[20];
	size_t len = wcsxfrm_l(dst, L"Hello", 20, __jacl_locale_current);
	ASSERT_EQ((size_t)5, len);
	ASSERT_EQ(0, wcscmp(dst, L"Hello"));
}

TEST(wcsxfrm_l_empty) {
	wchar_t dst[20];
	size_t len = wcsxfrm_l(dst, L"", 20, __jacl_locale_current);
	ASSERT_EQ((size_t)0, len);
	ASSERT_EQ(L'\0', dst[0]);
}

TEST(wcsxfrm_l_zero_n) {
	wchar_t dst[20] = L"X";
	size_t len = wcsxfrm_l(dst, L"Hello", 0, __jacl_locale_current);
	ASSERT_EQ((size_t)5, len);
	ASSERT_EQ(L'X', dst[0]);
}

TEST(wcsxfrm_l_null_dst) {
	size_t len = wcsxfrm_l(NULL, L"Hello", 20, __jacl_locale_current);
	ASSERT_EQ((size_t)5, len);
}

/* ============================================================================ */

TEST_SUITE(btowc)

TEST(btowc_ascii) {
	ASSERT_EQ(L'A', btowc('A'));
}

TEST(btowc_eof) {
	ASSERT_EQ(WEOF, btowc(EOF));
}

TEST(btowc_out_of_range) {
	ASSERT_EQ(WEOF, btowc(0x100));
}

TEST(btowc_zero) {
	ASSERT_EQ(L'\0', btowc(0));
}

TEST(btowc_negative) {
	ASSERT_EQ(WEOF, btowc(-5));
}

TEST(btowc_extended_ascii) {
	// UTF-8 strictly forbids 0x80-0xFF as single-byte characters
	ASSERT_EQ(WEOF, btowc(0x80));
	ASSERT_EQ(WEOF, btowc(0xC0));
	ASSERT_EQ(WEOF, btowc(0xFF));
}

/* ============================================================================ */

TEST_SUITE(wctob)

TEST(wctob_ascii) {
	ASSERT_EQ('Z', wctob(L'Z'));
}

TEST(wctob_weof) {
	ASSERT_EQ(EOF, wctob(WEOF));
}

TEST(wctob_out_of_range) {
	ASSERT_EQ(EOF, wctob(0x1F600));
}

TEST(wctob_null) {
	ASSERT_EQ(0, wctob(L'\0'));
}

/* ============================================================================ */

TEST_SUITE(fwide)

TEST(fwide_basic) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	ASSERT_EQ(0, fwide(f, 0));
	ASSERT_EQ(2, fwide(f, 1));
	ASSERT_EQ(2, fwide(f, -1));
	fclose(f);
}

TEST(fwide_byte_lock) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	ASSERT_EQ(1, fwide(f, -1));
	ASSERT_EQ(1, fwide(f, 1));
	fclose(f);
}

TEST(fwide_null) {
	ASSERT_EQ(0, fwide(NULL, 0));
}

TEST(fwide_wide_lock) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	ASSERT_EQ(2, fwide(f, 1));
	ASSERT_EQ(2, fwide(f, -1));
	fclose(f);
}

TEST(fwide_query) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fputwc(L'A', f);
	ASSERT_EQ(2, fwide(f, 0));
	fclose(f);
}

/* ============================================================================ */

TEST_SUITE(fputwc)

TEST(fputwc_basic) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	ASSERT_EQ(L'A', fputwc(L'A', f));
	fclose(f);
}

TEST(fputwc_multibyte) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	ASSERT_EQ(0xE9, fputwc(0xE9, f));
	fclose(f);
}

TEST(fputwc_null) {
	ASSERT_EQ(WEOF, fputwc(L'A', NULL));
}

TEST(fputwc_byte_locked) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fwide(f, -1); // Explicitly lock to byte orientation
	errno = 0;
	ASSERT_EQ(WEOF, fputwc(L'B', f));
	ASSERT_ERRNO(EINVAL);
	fclose(f);
}

/* ============================================================================ */

TEST_SUITE(fgetwc)

TEST(fgetwc_basic) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fputwc(L'A', f);
	rewind(f);
	ASSERT_EQ(L'A', fgetwc(f));
	fclose(f);
}

TEST(fgetwc_eof) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fputwc(L'A', f);
	rewind(f);
	fgetwc(f);
	ASSERT_EQ(WEOF, fgetwc(f));
	fclose(f);
}

TEST(fgetwc_null) {
	ASSERT_EQ(WEOF, fgetwc(NULL));
}

TEST(fgetwc_byte_locked) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fwide(f, -1); // Explicitly lock to byte orientation
	rewind(f);
	errno = 0;
	ASSERT_EQ(WEOF, fgetwc(f));
	ASSERT_ERRNO(EINVAL);
	fclose(f);
}

TEST(fgetwc_invalid_utf8) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fputs("\xFF", f);
	rewind(f);
	errno = 0;
	ASSERT_EQ(WEOF, fgetwc(f));
	ASSERT_ERRNO(EILSEQ);
	fclose(f);
}

TEST(fgetwc_incomplete_eof) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fputs("\xC3", f); // Incomplete 2-byte sequence at EOF
	rewind(f);
	errno = 0;
	ASSERT_EQ(WEOF, fgetwc(f));
	ASSERT_ERRNO(EILSEQ);
	fclose(f);
}

/* ============================================================================ */

TEST_SUITE(putwc)

TEST(putwc_basic) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	ASSERT_EQ(L'A', putwc(L'A', f));
	fclose(f);
}

TEST(putwc_null) {
	ASSERT_EQ(WEOF, putwc(L'A', NULL));
}

TEST(putwc_multibyte) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	ASSERT_EQ(0x4E2D, putwc(0x4E2D, f)); // 中
	fclose(f);
}

TEST(putwc_byte_locked) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fwide(f, -1);
	errno = 0;
	ASSERT_EQ(WEOF, (putwc)(L'B', f)); // Parentheses bypass the system macro
	ASSERT_ERRNO(EINVAL);
	fclose(f);
}

TEST(putwc_surrogate) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	errno = 0;
	ASSERT_EQ(WEOF, putwc(0xD800, f));
	ASSERT_ERRNO(EILSEQ);
	fclose(f);
}

/* ============================================================================ */

TEST_SUITE(getwc)

TEST(getwc_basic) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	putwc(L'A', f);
	rewind(f);
	ASSERT_EQ(L'A', getwc(f));
	fclose(f);
}

TEST(getwc_null) {
	ASSERT_EQ(WEOF, getwc(NULL));
}

TEST(getwc_multibyte) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	putwc(0x4E2D, f); // 中
	rewind(f);
	ASSERT_EQ(0x4E2D, getwc(f));
	fclose(f);
}

TEST(getwc_eof) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	putwc(L'A', f);
	rewind(f);
	getwc(f);
	ASSERT_EQ(WEOF, getwc(f));
	fclose(f);
}

TEST(getwc_byte_locked) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fwide(f, -1);
	rewind(f);
	errno = 0;
	ASSERT_EQ(WEOF, (getwc)(f)); // Parentheses bypass the system macro
	ASSERT_ERRNO(EINVAL);
	fclose(f);
}

TEST(getwc_invalid_utf8) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fputs("\xFF", f);
	rewind(f);
	errno = 0;
	ASSERT_EQ(WEOF, getwc(f));
	ASSERT_ERRNO(EILSEQ);
	fclose(f);
}

/* ============================================================================ */

TEST_SUITE(ungetwc)

TEST(ungetwc_basic) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fputwc(L'A', f);
	rewind(f);
	wint_t c = fgetwc(f);
	ASSERT_EQ(L'A', c);
	ASSERT_EQ(L'A', ungetwc(c, f));
	ASSERT_EQ(L'A', fgetwc(f));
	fclose(f);
}

TEST(ungetwc_weof) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	ASSERT_EQ(WEOF, ungetwc(WEOF, f));
	fclose(f);
}

TEST(ungetwc_null) {
	ASSERT_EQ(WEOF, ungetwc(L'A', NULL));
}

TEST(ungetwc_multiple) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fputws(L"AB", f);
	rewind(f);
	getwc(f);
	getwc(f);
	ASSERT_EQ(L'B', ungetwc(L'B', f));
	ASSERT_EQ(L'A', ungetwc(L'A', f));
	ASSERT_EQ(L'A', getwc(f));
	ASSERT_EQ(L'B', getwc(f));
	fclose(f);
}

TEST(ungetwc_different_char) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fputws(L"AB", f);
	rewind(f);
	getwc(f); // 'A'
	getwc(f); // 'B'
	ASSERT_EQ(L'X', ungetwc(L'X', f)); // Push back 'X' instead of 'B'
	ASSERT_EQ(L'X', getwc(f));
	fclose(f);
}

TEST(ungetwc_multibyte) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fputws(L"中", f); // 3-byte UTF-8 sequence
	rewind(f);
	wint_t c = fgetwc(f);
	ASSERT_EQ(0x4E2D, c);
	ASSERT_EQ(0x4E2D, ungetwc(c, f)); // Pushes back 3 bytes in reverse
	ASSERT_EQ(0x4E2D, fgetwc(f));     // Reassembles them correctly
	fclose(f);
}

/* ============================================================================ */

TEST_SUITE(putwchar)

TEST(putwchar_basic) {
	FILE *real_stdout = stdout;
	char buf[32] = {0};
	stdout = fmemopen(buf, sizeof(buf), "w");
	ASSERT_NOT_NULL(stdout);
	wint_t ret = putwchar(L'A');
	ASSERT_EQ(L'A', ret);
	fclose(stdout);
	stdout = real_stdout;
	ASSERT_EQ('A', buf[0]);
}

TEST(putwchar_surrogate) {
	FILE *real_stdout = stdout;
	char buf[32] = {0};
	stdout = fmemopen(buf, sizeof(buf), "w");
	ASSERT_NOT_NULL(stdout);
	errno = 0;
	ASSERT_EQ(WEOF, putwchar(0xD800));
	ASSERT_ERRNO(EILSEQ);
	fclose(stdout);
	stdout = real_stdout;
}

TEST(putwchar_multibyte) {
	FILE *real_stdout = stdout;
	char buf[32] = {0};
	stdout = fmemopen(buf, sizeof(buf), "w");
	ASSERT_NOT_NULL(stdout);
	ASSERT_EQ(0x4E2D, putwchar(0x4E2D)); // 中
	fclose(stdout);
	stdout = real_stdout;
	ASSERT_EQ((char)0xE4, buf[0]);
	ASSERT_EQ((char)0xB8, buf[1]);
	ASSERT_EQ((char)0xAD, buf[2]);
}

TEST(putwchar_out_of_range) {
	FILE *real_stdout = stdout;
	char buf[32] = {0};
	stdout = fmemopen(buf, sizeof(buf), "w");
	ASSERT_NOT_NULL(stdout);
	errno = 0;
	ASSERT_EQ(WEOF, putwchar(0x110000));
	ASSERT_ERRNO(EILSEQ);
	fclose(stdout);
	stdout = real_stdout;
}

/* ============================================================================ */

TEST_SUITE(getwchar)

TEST(getwchar_basic) {
	FILE *real_stdin = stdin;
	char data[] = "B";
	stdin = fmemopen(data, sizeof(data), "r");
	ASSERT_NOT_NULL(stdin);
	wint_t ret = getwchar();
	ASSERT_EQ(L'B', ret);
	fclose(stdin);
	stdin = real_stdin;
}

TEST(getwchar_multibyte) {
	FILE *real_stdin = stdin;
	char data[] = "\xE4\xB8\xAD"; // 中
	stdin = fmemopen(data, sizeof(data), "r");
	ASSERT_NOT_NULL(stdin);
	ASSERT_EQ(0x4E2D, getwchar());
	fclose(stdin);
	stdin = real_stdin;
}

TEST(getwchar_eof) {
	FILE *real_stdin = stdin;
	char data[] = "A";
	stdin = fmemopen(data, 1, "r");
	ASSERT_NOT_NULL(stdin);
	getwchar(); // Consume the single byte
	ASSERT_EQ(WEOF, getwchar()); // Now hits actual EOF
	fclose(stdin);
	stdin = real_stdin;
}

TEST(getwchar_invalid_utf8) {
	FILE *real_stdin = stdin;
	char data[] = "\xFF";
	stdin = fmemopen(data, sizeof(data), "r");
	ASSERT_NOT_NULL(stdin);
	errno = 0;
	ASSERT_EQ(WEOF, getwchar());
	ASSERT_ERRNO(EILSEQ);
	fclose(stdin);
	stdin = real_stdin;
}

/* ============================================================================ */

TEST_SUITE(fputws)

TEST(fputws_basic) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	ASSERT_EQ(0, fputws(L"Hello", f));
	fclose(f);
}

TEST(fputws_empty) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	ASSERT_EQ(0, fputws(L"", f));
	fclose(f);
}

TEST(fputws_null_stream) {
	ASSERT_EQ(-1, fputws(L"Hello", NULL));
}

TEST(fputws_null_string) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	ASSERT_EQ(-1, fputws(NULL, f));
	fclose(f);
}

TEST(fputws_unicode) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	ASSERT_EQ(0, fputws(L"Caf\xE9", f));
	fclose(f);
}

TEST(fputws_byte_locked) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fwide(f, -1);
	ASSERT_EQ(-1, fputws(L"Hi", f));
	fclose(f);
}

/* ============================================================================ */

TEST_SUITE(fgetws)

TEST(fgetws_basic) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fputws(L"Hello\n", f);
	rewind(f);
	wchar_t buf[10];
	ASSERT_NOT_NULL(fgetws(buf, 10, f));
	ASSERT_EQ(0, wcscmp(buf, L"Hello\n"));
	fclose(f);
}

TEST(fgetws_no_newline) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fputws(L"Hello", f);
	rewind(f);
	wchar_t buf[3];
	ASSERT_NOT_NULL(fgetws(buf, 3, f));
	ASSERT_EQ(0, wcscmp(buf, L"He"));
	fclose(f);
}

TEST(fgetws_null_stream) {
	wchar_t buf[10];
	ASSERT_NULL(fgetws(buf, 10, NULL));
}

TEST(fgetws_null_buf) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	ASSERT_NULL(fgetws(NULL, 10, f));
	fclose(f);
}

TEST(fgetws_zero_n) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	wchar_t buf[10];
	errno = 0;
	ASSERT_NULL(fgetws(buf, 0, f));
	ASSERT_ERRNO(EINVAL);
	fclose(f);
}

TEST(fgetws_exact_fit) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fputws(L"Hello", f);
	rewind(f);
	wchar_t buf[6];
	ASSERT_NOT_NULL(fgetws(buf, 6, f));
	ASSERT_EQ(0, wcscmp(buf, L"Hello"));
	fclose(f);
}

TEST(fgetws_incomplete_utf8) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	// Write a truncated 2-byte UTF-8 sequence (just the leading byte)
	fputc(0xC3, f);
	rewind(f);

	wchar_t buf[10];
	errno = 0;
	// Should fail, return NULL, and set EILSEQ
	ASSERT_NULL(fgetws(buf, 10, f));
	ASSERT_ERRNO(EILSEQ);
	fclose(f);
}

TEST(fgetws_byte_locked) {
	FILE *f = tmpfile();
	ASSERT_NOT_NULL(f);
	fwide(f, -1);
	wchar_t buf[10];
	ASSERT_NULL(fgetws(buf, 10, f));
	fclose(f);
}

/* ============================================================================ */

TEST_SUITE(wcwidth)

TEST(wcwidth_basic) {
	ASSERT_EQ(0, wcwidth(L'\0'));
	ASSERT_EQ(1, wcwidth(L'A'));
	ASSERT_EQ(2, wcwidth(0x4E2D));
	ASSERT_EQ(-1, wcwidth(0x07));
}

TEST(wcwidth_boundaries) {
	ASSERT_EQ(-1, wcwidth(0x1F));
	ASSERT_EQ(1, wcwidth(0x20));
	ASSERT_EQ(1, wcwidth(0x7E));
	ASSERT_EQ(-1, wcwidth(0x7F));
	ASSERT_EQ(-1, wcwidth(0x9F));
	ASSERT_EQ(1, wcwidth(0xA0));
}

TEST(wcwidth_combining) {
	ASSERT_EQ(1, wcwidth(0x0300)); // Simplified model returns 1
	ASSERT_EQ(1, wcwidth(0x0308)); // Simplified model returns 1
}

TEST(wcwidth_fullwidth) {
	ASSERT_EQ(2, wcwidth(0xFF01)); // Fullwidth Exclamation Mark
	ASSERT_EQ(2, wcwidth(0xFF1A)); // Fullwidth Colon
}

TEST(wcwidth_hangul) {
	ASSERT_EQ(2, wcwidth(0xAC00)); // Hangul Syllable Ga
	ASSERT_EQ(2, wcwidth(0xD7A3)); // Hangul Syllable Hih
}

TEST(wcwidth_control_c0) {
	ASSERT_EQ(-1, wcwidth(0x08)); // Backspace
	ASSERT_EQ(-1, wcwidth(0x1B)); // Escape
}

TEST(wcwidth_control_c1) {
	ASSERT_EQ(-1, wcwidth(0x80));
	ASSERT_EQ(-1, wcwidth(0x9F));
}

/* ============================================================================ */

TEST_SUITE(wcswidth)

TEST(wcswidth_basic) {
	ASSERT_EQ(5, wcswidth(L"Hello", 10));
	ASSERT_EQ(2, wcswidth(L"中", 10));
	ASSERT_EQ(-1, wcswidth(L"Hi\x07", 10));
}

TEST(wcswidth_zero_n) {
	ASSERT_EQ(0, wcswidth(L"Hello", 0));
}

TEST(wcswidth_null) {
	ASSERT_EQ(0, wcswidth(NULL, 10));
}

TEST(wcswidth_exact_n) {
	ASSERT_EQ(3, wcswidth(L"Hello", 3));
}

TEST(wcswidth_truncates_at_control) {
	ASSERT_EQ(-1, wcswidth(L"A\007B", 3));
}

/* ============================================================================ */

TEST_SUITE(wcscasecmp_l)

	TEST(wcscasecmp_l_equal) {
			ASSERT_EQ(0, wcscasecmp_l(L"Test", L"test", __jacl_locale_current));
	}

TEST(wcscasecmp_l_less) {
		ASSERT_TRUE(wcscasecmp_l(L"apple", L"BANANA", __jacl_locale_current) < 0);
}

TEST(wcscasecmp_l_null) {
		ASSERT_NE(0, wcscasecmp_l(NULL, L"Test", __jacl_locale_current));
}

/* ============================================================================ */

TEST_SUITE(wcsncasecmp_l)

	TEST(wcsncasecmp_l_partial) {
			ASSERT_EQ(0, wcsncasecmp_l(L"HELLO", L"help", 3, __jacl_locale_current));
	}

TEST(wcsncasecmp_l_null) {
		ASSERT_NE(0, wcsncasecmp_l(NULL, L"Test", 4, __jacl_locale_current));
}

/* ============================================================================ */

TEST_SUITE(open_wmemstream)
TEST(open_wmemstream_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(vwprintf)
TEST(vwprintf_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(wprintf)
TEST(wprintf_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(vfwprintf)
TEST(vfwprintf_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(fwprintf)
TEST(fwprintf_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(vswprintf)
TEST(vswprintf_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(swprintf)
TEST(swprintf_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(vwscanf)
TEST(vwscanf_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(wscanf)
TEST(wscanf_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(vfwscanf)
TEST(vfwscanf_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(fwscanf)
TEST(fwscanf_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(vswscanf)
TEST(vswscanf_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(swscanf)
TEST(swscanf_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(wcstol)
TEST(wcstol_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(wcstoll)
TEST(wcstoll_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(wcstoul)
TEST(wcstoul_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(wcstoull)
TEST(wcstoull_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(wcstod)
TEST(wcstod_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(wcstof)
TEST(wcstof_basic) { TEST_SKIP("not implemented"); }

TEST_SUITE(wcstold)
TEST(wcstold_basic) { TEST_SKIP("not implemented"); }


TEST_SUITE(wcsftime)
TEST(wcsftime_basic) { TEST_SKIP("not implemented"); }

TEST_MAIN_IF(JACL_HAS_C99, "wchar.h requires C99 or later\n")
