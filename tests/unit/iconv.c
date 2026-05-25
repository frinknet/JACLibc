/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <iconv.h>
#include <string.h>
#include <errno.h>

TEST_TYPE(unit);
TEST_UNIT(iconv.h);

/* ============================================================================ */
TEST_SUITE(iconv_t);

TEST(iconv_t_size_positive) {
	ASSERT_SIZE_MIN(iconv_t, sizeof(void *));
}

/* ============================================================================ */
TEST_SUITE(iconv_open);

TEST(iconv_open_utf8_ascii) {
	iconv_t cd = iconv_open("ASCII", "UTF-8");
	ASSERT_NOT_NULL(cd);
	ASSERT_INT_NE(cd, (iconv_t)-1);
	iconv_close(cd);
}

TEST(iconv_open_ascii_utf8) {
	iconv_t cd = iconv_open("UTF-8", "ASCII");
	ASSERT_NOT_NULL(cd);
	iconv_close(cd);
}

TEST(iconv_open_invalid_from) {
	iconv_t cd = iconv_open("UTF-8", "BOGUS");
	ASSERT_NOT_NULL(cd); /* Falls back to ASCII (0) */
	iconv_close(cd);
}

TEST(iconv_open_null_args) {
	iconv_t cd = iconv_open(NULL, NULL);
	ASSERT_NOT_NULL(cd); /* Defaults to ASCII/ASCII */
	iconv_close(cd);
}

/* ============================================================================ */
TEST_SUITE(iconv);

TEST(iconv_ascii_to_utf8) {
	iconv_t cd = iconv_open("UTF-8", "ASCII");
	char in[] = "Hello"; size_t inleft = 5;
	char out[10]; size_t outleft = 10;
	char *pin = in, *pout = out;
	size_t r = iconv(cd, &pin, &inleft, &pout, &outleft);
	ASSERT_INT_EQ(r, 5);
	ASSERT_STR_EQ(out, "Hello");
	iconv_close(cd);
}

TEST(iconv_utf8_to_ascii) {
	iconv_t cd = iconv_open("ASCII", "UTF-8");
	char in[] = "Test"; size_t inleft = 4;
	char out[10]; size_t outleft = 10;
	char *pin = in, *pout = out;
	size_t r = iconv(cd, &pin, &inleft, &pout, &outleft);
	ASSERT_INT_EQ(r, 4);
	iconv_close(cd);
}

TEST(iconv_utf8_multibyte) {
	iconv_t cd = iconv_open("UTF-32LE", "UTF-8");
	char in[] = "€"; size_t inleft = 3; /* 0xE2 0x82 0xAC */
	char out[10]; size_t outleft = 10;
	char *pin = in, *pout = out;
	size_t r = iconv(cd, &pin, &inleft, &pout, &outleft);
	ASSERT_INT_EQ(r, 1);
	ASSERT_INT_EQ((unsigned char)out[0], 0xAC);
	ASSERT_INT_EQ((unsigned char)out[1], 0x20);
	iconv_close(cd);
}

TEST(iconv_buffer_too_small) {
	iconv_t cd = iconv_open("UTF-8", "ASCII");
	char in[] = "LongString"; size_t inleft = 10;
	char out[4]; size_t outleft = 4;
	char *pin = in, *pout = out;
	iconv(cd, &pin, &inleft, &pout, &outleft);
	ASSERT_ERRNO(E2BIG);
	iconv_close(cd);
}

TEST(iconv_invalid_utf8) {
	iconv_t cd = iconv_open("UTF-8", "UTF-8");
	char in[] = { (char)0xFF, 'A', '\0' }; size_t inleft = 2;
	char out[10]; size_t outleft = 10;
	char *pin = in, *pout = out;
	size_t r = iconv(cd, &pin, &inleft, &pout, &outleft);
	ASSERT_INT_EQ(r, (size_t)-1);
	ASSERT_ERRNO(EILSEQ);
	iconv_close(cd);
}

TEST(iconv_partial_utf8) {
	iconv_t cd = iconv_open("UTF-8", "UTF-8");
	char in[] = { (char)0xC2 }; size_t inleft = 1;
	char out[10]; size_t outleft = 10;
	char *pin = in, *pout = out;
	size_t r = iconv(cd, &pin, &inleft, &pout, &outleft);
	ASSERT_INT_EQ(r, 0); /* Incomplete, needs more input */
	iconv_close(cd);
}

TEST(iconv_null_inbuf) {
	iconv_t cd = iconv_open("UTF-8", "ASCII");
	size_t inleft = 5, outleft = 10;
	char out[10]; char *pout = out;
	size_t r = iconv(cd, NULL, &inleft, &pout, &outleft);
	ASSERT_INT_EQ(r, (size_t)-1);
	ASSERT_ERRNO(EINVAL);
	iconv_close(cd);
}

TEST(iconv_null_inbytesleft) {
	iconv_t cd = iconv_open("UTF-8", "ASCII");
	char in[] = "test"; size_t inleft = 4, outleft = 10;
	char out[10]; char *pin = in, *pout = out;
	size_t r = iconv(cd, &pin, NULL, &pout, &outleft);
	ASSERT_INT_EQ(r, (size_t)-1);
	ASSERT_ERRNO(EINVAL);
	iconv_close(cd);
}

TEST(iconv_stateful_continue) {
	/* UTF-8 state machine: 0xC2 starts 2-byte sequence, needs 0xA0 */
	iconv_t cd = iconv_open("UTF-8", "UTF-8");
	char in[] = { (char)0xC2 }; size_t inleft = 1;
	char out[10]; size_t outleft = 10;
	char *pin = in, *pout = out;

	/* First call: partial sequence, consumes byte but returns 0 converted */
	size_t r1 = iconv(cd, &pin, &inleft, &pout, &outleft);
	ASSERT_INT_EQ(r1, 0);
	ASSERT_INT_EQ(inleft, 0); /* FIX: Byte was consumed to set state */

	/* Provide continuation byte */
	char in2[] = { (char)0xA0, 'B' }; size_t inleft2 = 2;
	pin = in2;
	size_t r2 = iconv(cd, &pin, &inleft2, &pout, &outleft);
	ASSERT_INT_EQ(r2, 2); /* U+00A0 + 'B' */

	iconv_close(cd);
}

TEST(iconv_utf16_surrogate_pair) {
	/* 😀 U+1F600 -> UTF-16LE: D8 3D DE 00 */
	iconv_t cd = iconv_open("UTF-16LE", "UTF-8");
	char in[] = "\xf0\x9f\x98\x80"; size_t inleft = 4;
	char out[10]; size_t outleft = 10;
	char *pin = in, *pout = out;
	size_t r = iconv(cd, &pin, &inleft, &pout, &outleft);
	ASSERT_INT_EQ(r, 1);
	ASSERT_INT_EQ((unsigned char)out[0], 0x3D);
	ASSERT_INT_EQ((unsigned char)out[1], 0xD8);
	ASSERT_INT_EQ((unsigned char)out[2], 0x00);
	ASSERT_INT_EQ((unsigned char)out[3], 0xDE);
	iconv_close(cd);
}

/* ============================================================================ */
TEST_SUITE(iconv_close);

TEST(iconv_close_valid) {
	iconv_t cd = iconv_open("UTF-8", "ASCII");
	ASSERT_INT_EQ(iconv_close(cd), 0);
}

TEST(iconv_close_null) {
	ASSERT_INT_EQ(iconv_close(NULL), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(iconv_close_invalid) {
	ASSERT_INT_EQ(iconv_close((iconv_t)-1), -1);
	ASSERT_ERRNO(EBADF);
}


/* ============================================================================ */
TEST_MAIN()
