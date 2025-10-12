/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_C11
#include <uchar.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(uchar.h);

/* ============================================================================
 * TYPES AND CONSTANTS
 * ============================================================================ */
TEST_SUITE(types_constants);

TEST(char16_t_defined) {
	char16_t c = 0x41;  // 'A'
	ASSERT_EQ(0x41, c);
}

TEST(char32_t_defined) {
	char32_t c = 0x1F600;  // 😀
	ASSERT_EQ(0x1F600, c);
}

TEST(char16_limits) {
	ASSERT_EQ(0, CHAR16_MIN);
	ASSERT_EQ(0xFFFFu, CHAR16_MAX);
}

TEST(char32_limits) {
	ASSERT_EQ(0, CHAR32_MIN);
	ASSERT_EQ(0xFFFFFFFFu, CHAR32_MAX);
}

TEST(char16_size) {
	ASSERT_TRUE(sizeof(char16_t) >= 2);
}

TEST(char32_size) {
	ASSERT_TRUE(sizeof(char32_t) >= 4);
}

/* ============================================================================
 * MBRTOC16 - ASCII
 * ============================================================================ */
TEST_SUITE(mbrtoc16_ascii);

TEST(mbrtoc16_ascii_basic) {
	char16_t c16;
	const char *str = "A";
	
	size_t result = mbrtoc16(&c16, str, 1, NULL);
	
	ASSERT_EQ(1, result);
	ASSERT_EQ(u'A', c16);
}

TEST(mbrtoc16_ascii_multiple) {
	char16_t c16;
	const char *str = "ABC";
	mbstate_t state = {0};
	
	ASSERT_EQ(1, mbrtoc16(&c16, str, 3, &state));
	ASSERT_EQ(u'A', c16);
	
	ASSERT_EQ(1, mbrtoc16(&c16, str + 1, 2, &state));
	ASSERT_EQ(u'B', c16);
	
	ASSERT_EQ(1, mbrtoc16(&c16, str + 2, 1, &state));
	ASSERT_EQ(u'C', c16);
}

TEST(mbrtoc16_null_char) {
	char16_t c16;
	const char *str = "\0";
	
	size_t result = mbrtoc16(&c16, str, 1, NULL);
	
	ASSERT_EQ(0, result);
	ASSERT_EQ(0, c16);
}

/* ============================================================================
 * MBRTOC16 - UTF-8 BMP
 * ============================================================================ */
TEST_SUITE(mbrtoc16_utf8_bmp);

TEST(mbrtoc16_two_byte) {
	char16_t c16;
	const char *utf8 = "\xC3\xA9";  // é (U+00E9)
	
	size_t result = mbrtoc16(&c16, utf8, 2, NULL);
	
	ASSERT_EQ(2, result);
	ASSERT_EQ(0xE9, c16);
}

TEST(mbrtoc16_three_byte) {
	char16_t c16;
	const char *utf8 = "\xE2\x82\xAC";  // € (U+20AC)
	
	size_t result = mbrtoc16(&c16, utf8, 3, NULL);
	
	ASSERT_EQ(3, result);
	ASSERT_EQ(0x20AC, c16);
}

TEST(mbrtoc16_chinese) {
	char16_t c16;
	const char *utf8 = "\xE4\xB8\xAD";  // 中 (U+4E2D)
	
	size_t result = mbrtoc16(&c16, utf8, 3, NULL);
	
	ASSERT_EQ(3, result);
	ASSERT_EQ(0x4E2D, c16);
}

/* ============================================================================
 * MBRTOC16 - SURROGATE PAIRS
 * ============================================================================ */
TEST_SUITE(mbrtoc16_surrogates);

TEST(mbrtoc16_emoji_surrogate_pair) {
	char16_t c16;
	const char *utf8 = "\xF0\x9F\x98\x80";  // 😀 (U+1F600)
	mbstate_t state = {0};
	
	// First call returns high surrogate
	size_t result1 = mbrtoc16(&c16, utf8, 4, &state);
	ASSERT_EQ(4, result1);
	ASSERT_TRUE(c16 >= 0xD800 && c16 < 0xDC00);  // High surrogate
	
	// Second call returns low surrogate
	size_t result2 = mbrtoc16(&c16, utf8, 4, &state);
	ASSERT_EQ((size_t)-3, result2);
	ASSERT_TRUE(c16 >= 0xDC00 && c16 < 0xE000);  // Low surrogate
}

TEST(mbrtoc16_four_byte_character) {
	char16_t c16_high, c16_low;
	const char *utf8 = "\xF0\x9F\x92\xA9";  // 💩 (U+1F4A9)
	mbstate_t state = {0};
	
	mbrtoc16(&c16_high, utf8, 4, &state);
	mbrtoc16(&c16_low, utf8, 4, &state);
	
	// Reconstruct code point
	uint32_t codepoint = 0x10000 + ((c16_high - 0xD800) << 10) + (c16_low - 0xDC00);
	ASSERT_EQ(0x1F4A9, codepoint);
}

/* ============================================================================
 * C16RTOMB - ASCII
 * ============================================================================ */
TEST_SUITE(c16rtomb_ascii);

TEST(c16rtomb_ascii_basic) {
	char buf[MB_CUR_MAX];
	
	size_t result = c16rtomb(buf, u'A', NULL);
	
	ASSERT_EQ(1, result);
	ASSERT_EQ('A', buf[0]);
}

TEST(c16rtomb_ascii_multiple) {
	char buf[MB_CUR_MAX];
	
	ASSERT_EQ(1, c16rtomb(buf, u'H', NULL));
	ASSERT_EQ('H', buf[0]);
	
	ASSERT_EQ(1, c16rtomb(buf, u'i', NULL));
	ASSERT_EQ('i', buf[0]);
}

TEST(c16rtomb_null) {
	char buf[MB_CUR_MAX];
	
	size_t result = c16rtomb(buf, 0, NULL);
	
	ASSERT_EQ(1, result);
	ASSERT_EQ('\0', buf[0]);
}

/* ============================================================================
 * C16RTOMB - UTF-8 BMP
 * ============================================================================ */
TEST_SUITE(c16rtomb_utf8_bmp);

TEST(c16rtomb_two_byte) {
	char buf[MB_CUR_MAX];
	
	size_t result = c16rtomb(buf, 0xE9, NULL);  // é
	
	ASSERT_EQ(2, result);
	ASSERT_EQ((char)0xC3, buf[0]);
	ASSERT_EQ((char)0xA9, buf[1]);
}

TEST(c16rtomb_three_byte) {
	char buf[MB_CUR_MAX];
	
	size_t result = c16rtomb(buf, 0x20AC, NULL);  // €
	
	ASSERT_EQ(3, result);
	ASSERT_EQ((char)0xE2, buf[0]);
	ASSERT_EQ((char)0x82, buf[1]);
	ASSERT_EQ((char)0xAC, buf[2]);
}

/* ============================================================================
 * C16RTOMB - SURROGATE PAIRS
 * ============================================================================ */
TEST_SUITE(c16rtomb_surrogates);

TEST(c16rtomb_surrogate_pair) {
	char buf[MB_CUR_MAX];
	mbstate_t state = {0};
	
	// High surrogate for 😀 (U+1F600)
	size_t result1 = c16rtomb(buf, 0xD83D, &state);
	ASSERT_EQ(0, result1);  // Waiting for low surrogate
	
	// Low surrogate
	size_t result2 = c16rtomb(buf, 0xDE00, &state);
	ASSERT_EQ(4, result2);
	
	// Verify UTF-8 encoding
	ASSERT_EQ((char)0xF0, buf[0]);
	ASSERT_EQ((char)0x9F, buf[1]);
	ASSERT_EQ((char)0x98, buf[2]);
	ASSERT_EQ((char)0x80, buf[3]);
}

TEST(c16rtomb_invalid_low_surrogate) {
	char buf[MB_CUR_MAX];
	mbstate_t state = {0};
	
	c16rtomb(buf, 0xD800, &state);  // High surrogate
	
	// Invalid low surrogate
	size_t result = c16rtomb(buf, 0x0041, &state);  // 'A' instead of low surrogate
	ASSERT_EQ((size_t)-1, result);
}

TEST(c16rtomb_unexpected_low_surrogate) {
	char buf[MB_CUR_MAX];
	
	// Low surrogate without high surrogate
	size_t result = c16rtomb(buf, 0xDC00, NULL);
	ASSERT_EQ((size_t)-1, result);
}

/* ============================================================================
 * MBRTOC32 - ASCII
 * ============================================================================ */
TEST_SUITE(mbrtoc32_ascii);

TEST(mbrtoc32_ascii_basic) {
	char32_t c32;
	const char *str = "A";
	
	size_t result = mbrtoc32(&c32, str, 1, NULL);
	
	ASSERT_EQ(1, result);
	ASSERT_EQ(U'A', c32);
}

TEST(mbrtoc32_ascii_string) {
	char32_t c32;
	const char *str = "Test";
	mbstate_t state = {0};
	
	ASSERT_EQ(1, mbrtoc32(&c32, str, 4, &state));
	ASSERT_EQ(U'T', c32);
	
	ASSERT_EQ(1, mbrtoc32(&c32, str + 1, 3, &state));
	ASSERT_EQ(U'e', c32);
}

/* ============================================================================
 * MBRTOC32 - UTF-8
 * ============================================================================ */
TEST_SUITE(mbrtoc32_utf8);

TEST(mbrtoc32_two_byte) {
	char32_t c32;
	const char *utf8 = "\xC3\xA9";  // é
	
	size_t result = mbrtoc32(&c32, utf8, 2, NULL);
	
	ASSERT_EQ(2, result);
	ASSERT_EQ(0xE9, c32);
}

TEST(mbrtoc32_three_byte) {
	char32_t c32;
	const char *utf8 = "\xE2\x82\xAC";  // €
	
	size_t result = mbrtoc32(&c32, utf8, 3, NULL);
	
	ASSERT_EQ(3, result);
	ASSERT_EQ(0x20AC, c32);
}

TEST(mbrtoc32_four_byte) {
	char32_t c32;
	const char *utf8 = "\xF0\x9F\x98\x80";  // 😀
	
	size_t result = mbrtoc32(&c32, utf8, 4, NULL);
	
	ASSERT_EQ(4, result);
	ASSERT_EQ(0x1F600, c32);
}

TEST(mbrtoc32_emoji) {
	char32_t c32;
	const char *utf8 = "\xF0\x9F\x92\xA9";  // 💩
	
	size_t result = mbrtoc32(&c32, utf8, 4, NULL);
	
	ASSERT_EQ(4, result);
	ASSERT_EQ(0x1F4A9, c32);
}

/* ============================================================================
 * C32RTOMB - ASCII
 * ============================================================================ */
TEST_SUITE(c32rtomb_ascii);

TEST(c32rtomb_ascii_basic) {
	char buf[MB_CUR_MAX];
	
	size_t result = c32rtomb(buf, U'A', NULL);
	
	ASSERT_EQ(1, result);
	ASSERT_EQ('A', buf[0]);
}

TEST(c32rtomb_null) {
	char buf[MB_CUR_MAX];
	
	size_t result = c32rtomb(buf, 0, NULL);
	
	ASSERT_EQ(1, result);
	ASSERT_EQ('\0', buf[0]);
}

/* ============================================================================
 * C32RTOMB - UTF-8
 * ============================================================================ */
TEST_SUITE(c32rtomb_utf8);

TEST(c32rtomb_two_byte) {
	char buf[MB_CUR_MAX];
	
	size_t result = c32rtomb(buf, 0xE9, NULL);  // é
	
	ASSERT_EQ(2, result);
	ASSERT_EQ((char)0xC3, buf[0]);
	ASSERT_EQ((char)0xA9, buf[1]);
}

TEST(c32rtomb_three_byte) {
	char buf[MB_CUR_MAX];
	
	size_t result = c32rtomb(buf, 0x20AC, NULL);  // €
	
	ASSERT_EQ(3, result);
	ASSERT_EQ((char)0xE2, buf[0]);
	ASSERT_EQ((char)0x82, buf[1]);
	ASSERT_EQ((char)0xAC, buf[2]);
}

TEST(c32rtomb_four_byte) {
	char buf[MB_CUR_MAX];
	
	size_t result = c32rtomb(buf, 0x1F600, NULL);  // 😀
	
	ASSERT_EQ(4, result);
	ASSERT_EQ((char)0xF0, buf[0]);
	ASSERT_EQ((char)0x9F, buf[1]);
	ASSERT_EQ((char)0x98, buf[2]);
	ASSERT_EQ((char)0x80, buf[3]);
}

/* ============================================================================
 * ROUNDTRIP TESTS
 * ============================================================================ */
TEST_SUITE(roundtrip);

TEST(c16_roundtrip_ascii) {
	char buf[MB_CUR_MAX];
	char16_t c16_in = u'Z', c16_out;
	
	c16rtomb(buf, c16_in, NULL);
	mbrtoc16(&c16_out, buf, MB_CUR_MAX, NULL);
	
	ASSERT_EQ(c16_in, c16_out);
}

TEST(c32_roundtrip_ascii) {
	char buf[MB_CUR_MAX];
	char32_t c32_in = U'X', c32_out;
	
	c32rtomb(buf, c32_in, NULL);
	mbrtoc32(&c32_out, buf, MB_CUR_MAX, NULL);
	
	ASSERT_EQ(c32_in, c32_out);
}

TEST(c32_roundtrip_emoji) {
	char buf[MB_CUR_MAX];
	char32_t c32_in = 0x1F600, c32_out;  // 😀
	
	c32rtomb(buf, c32_in, NULL);
	mbrtoc32(&c32_out, buf, MB_CUR_MAX, NULL);
	
	ASSERT_EQ(c32_in, c32_out);
}

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(mbrtoc16_null_output) {
	const char *str = "A";
	size_t result = mbrtoc16(NULL, str, 1, NULL);
	
	ASSERT_EQ(1, result);
}

TEST(mbrtoc32_null_output) {
	const char *str = "A";
	size_t result = mbrtoc32(NULL, str, 1, NULL);
	
	ASSERT_EQ(1, result);
}

TEST(c16rtomb_query) {
	size_t result = c16rtomb(NULL, 0, NULL);
	ASSERT_TRUE(result > 0);
}

TEST(c32rtomb_query) {
	size_t result = c32rtomb(NULL, 0, NULL);
	ASSERT_TRUE(result > 0);
}

TEST_MAIN()

#else

int main(void) {
	printf("uchar.h requires C11 or later\n");
	return 0;
}

#endif
