/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <inttypes.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

// Provide imaxabs if not prototyped in your C lib
#ifndef imaxabs
intmax_t imaxabs(intmax_t x) { return x < 0 ? -x : x; }
#endif

TEST_TYPE(unit)
TEST_UNIT(inttypes.h)

// -------- PRI MACROS --------
TEST_SUITE(PRI_macros)
TEST(PRI_macros_PRId8) {
    ASSERT_STR_EQ("hhd", PRId8);
}
TEST(PRI_macros_PRIi8) {
    ASSERT_STR_EQ("hhi", PRIi8);
}
TEST(PRI_macros_PRIu8) {
    ASSERT_STR_EQ("hhu", PRIu8);
}
TEST(PRI_macros_PRId16) {
    ASSERT_STR_EQ("hd", PRId16);
}
TEST(PRI_macros_PRIi16) {
    ASSERT_STR_EQ("hi", PRIi16);
}
TEST(PRI_macros_PRIu16) {
    ASSERT_STR_EQ("hu", PRIu16);
}
TEST(PRI_macros_PRId32) {
    ASSERT_STR_EQ("d", PRId32);
}
TEST(PRI_macros_PRIi32) {
    ASSERT_STR_EQ("i", PRIi32);
}
TEST(PRI_macros_PRIu32) {
    ASSERT_STR_EQ("u", PRIu32);
}
#if JACL_INTMAX_BITS >= 64
TEST(PRI_macros_PRId64) {
    ASSERT_STR_EQ("lld", PRId64);
}
TEST(PRI_macros_PRIi64) {
    ASSERT_STR_EQ("lli", PRIi64);
}
TEST(PRI_macros_PRIu64) {
    ASSERT_STR_EQ("llu", PRIu64);
}
#endif
TEST(PRI_macros_PRIdPTR) {
#if JACL_64BIT
    ASSERT_STR_EQ("lld", PRIdPTR);
#elif JACL_32BIT
    ASSERT_STR_EQ("d", PRIdPTR);
#elif JACL_16BIT
    ASSERT_STR_EQ("hd", PRIdPTR);
#else
    ASSERT_STR_EQ("hhd", PRIdPTR);
#endif
}

// -------- SCN MACROS --------
TEST_SUITE(SCN_macros)
TEST(SCN_macros_SCNd8) {
    ASSERT_STR_EQ("hhd", SCNd8);
}
TEST(SCN_macros_SCNi8) {
    ASSERT_STR_EQ("hhi", SCNi8);
}
TEST(SCN_macros_SCNu8) {
    ASSERT_STR_EQ("hhu", SCNu8);
}
TEST(SCN_macros_SCNd16) {
    ASSERT_STR_EQ("hd", SCNd16);
}
TEST(SCN_macros_SCNi16) {
    ASSERT_STR_EQ("hi", SCNi16);
}
TEST(SCN_macros_SCNu16) {
    ASSERT_STR_EQ("hu", SCNu16);
}
TEST(SCN_macros_SCNd32) {
    ASSERT_STR_EQ("d", SCNd32);
}
TEST(SCN_macros_SCNi32) {
    ASSERT_STR_EQ("i", SCNi32);
}
TEST(SCN_macros_SCNu32) {
    ASSERT_STR_EQ("u", SCNu32);
}
#if JACL_INTMAX_BITS >= 64
TEST(SCN_macros_SCNd64) {
    ASSERT_STR_EQ("lld", SCNd64);
}
TEST(SCN_macros_SCNi64) {
    ASSERT_STR_EQ("lli", SCNi64);
}
TEST(SCN_macros_SCNu64) {
    ASSERT_STR_EQ("llu", SCNu64);
}
#endif
TEST(SCN_macros_SCNdPTR) {
#if JACL_64BIT
    ASSERT_STR_EQ("lld", SCNdPTR);
#elif JACL_32BIT
    ASSERT_STR_EQ("d", SCNdPTR);
#elif JACL_16BIT
    ASSERT_STR_EQ("hd", SCNdPTR);
#else
    ASSERT_STR_EQ("hhd", SCNdPTR);
#endif
}

// -------- TYPES --------
TEST_SUITE(types)
TEST(types_intmax_t) {
    intmax_t x = 1;
    ASSERT_EQ((int)sizeof(x), (int)sizeof(intmax_t));
}
TEST(types_uintmax_t) {
    uintmax_t x = 1;
    ASSERT_EQ((int)sizeof(x), (int)sizeof(uintmax_t));
}
TEST(types_imaxdiv_t) {
    imaxdiv_t t = { 77, -23 };
    ASSERT_EQ(t.quot, 77);
    ASSERT_EQ(t.rem, -23);
}
TEST(types_imaxdiv_t_size) {
    ASSERT_EQ(sizeof(imaxdiv_t), 2 * sizeof(intmax_t));
}

// -------- snprintf/printers --------
TEST_SUITE(snprintf)
TEST(snprintf_int8) {
    char buf[16]; int8_t x = 127;
    snprintf(buf, sizeof(buf), "%" PRId8, (int)x);
    ASSERT_STR_EQ("127", buf);
}
TEST(snprintf_int16) {
    char buf[16]; int16_t x = 4123;
    snprintf(buf, sizeof(buf), "%" PRId16, (int)x);
    ASSERT_STR_EQ("4123", buf);
}
TEST(snprintf_int32) {
    char buf[32]; int32_t x = 12345678;
    snprintf(buf, sizeof(buf), "%" PRId32, (int)x);
    ASSERT_TRUE(strstr(buf, "12345678") != NULL);
}
#if JACL_INTMAX_BITS >= 64
TEST(snprintf_int64) {
    char buf[64]; int64_t x = INT64_MAX;
    snprintf(buf, sizeof(buf), "%" PRId64, (long long)x);
    ASSERT_STR_EQ("9223372036854775807", buf);
}
#endif
TEST(snprintf_ptr) {
    char buf[64]; uintptr_t p = 0xCAFEBABEULL;
    snprintf(buf, sizeof(buf), "%" PRIxPTR, (uintptr_t)p);
    ASSERT_TRUE(strstr(buf, "cafebabe") || strstr(buf, "CAFEBABE"));
}
TEST(snprintf_neg_zero) {
    char buf[16]; intmax_t x = -0;
    snprintf(buf, sizeof(buf), "%" PRIdMAX, (intmax_t)x);
    ASSERT_STR_EQ("0", buf);
}

// -------- imaxdiv --------
TEST_SUITE(imaxdiv)
TEST(imaxdiv_basic) {
    imaxdiv_t d = imaxdiv(13, 5);
    ASSERT_EQ(2, d.quot);
    ASSERT_EQ(3, d.rem);
}
TEST(imaxdiv_neg_dividend) {
    imaxdiv_t d = imaxdiv(-13, 5);
    ASSERT_EQ(-2, d.quot);
    ASSERT_EQ(-3, d.rem);
}
TEST(imaxdiv_neg_divisor) {
    imaxdiv_t d = imaxdiv(13, -5);
    ASSERT_EQ(-2, d.quot);
    ASSERT_EQ(3, d.rem);
}
TEST(imaxdiv_both_neg) {
    imaxdiv_t d = imaxdiv(-13, -5);
    ASSERT_EQ(2, d.quot);
    ASSERT_EQ(-3, d.rem);
}
TEST(imaxdiv_zero) {
    imaxdiv_t d = imaxdiv(0, 7);
    ASSERT_EQ(0, d.quot);
    ASSERT_EQ(0, d.rem);
}
TEST(imaxdiv_min_by_one) {
    imaxdiv_t d = imaxdiv(INTMAX_MIN, 1);
    ASSERT_EQ(INTMAX_MIN, d.quot);
    ASSERT_EQ(0, d.rem);
}
TEST(imaxdiv_min_by_max) {
    imaxdiv_t d = imaxdiv(INTMAX_MIN, INTMAX_MAX);
    ASSERT_EQ(-1, d.quot);
    ASSERT_EQ(-1, d.rem);
}
TEST(imaxdiv_one_by_one) {
    imaxdiv_t d = imaxdiv(1, 1);
    ASSERT_EQ(1, d.quot);
    ASSERT_EQ(0, d.rem);
}

// -------- imaxabs --------
TEST_SUITE(imaxabs)
TEST(imaxabs_positive) {
    intmax_t y = imaxabs(42);
    ASSERT_EQ(y, 42);
}
TEST(imaxabs_negative) {
    intmax_t y = imaxabs(-42);
    ASSERT_EQ(y, 42);
}
TEST(imaxabs_zero) {
    intmax_t y = imaxabs(0);
    ASSERT_EQ(y, 0);
}
TEST(imaxabs_max) {
    intmax_t y = imaxabs(INTMAX_MAX);
    ASSERT_EQ(y, INTMAX_MAX);
}

// -------- strtoimax --------
TEST_SUITE(strtoimax)
TEST(strtoimax_basic) {
    char *e = NULL;
    intmax_t x = strtoimax("42", &e, 10);
    ASSERT_EQ(42, x);
    ASSERT_TRUE(*e == '\0');
}
TEST(strtoimax_partial) {
    char buf[] = "1337h4xx0rz"; char *e = NULL;
    intmax_t x = strtoimax(buf, &e, 10);
    ASSERT_EQ(1337, x);
    ASSERT_TRUE(*e == 'h');
}
TEST(strtoimax_overflow) {
    char src[64];
		strcpy(src, "999999999999999999999999999999");
    char *e = NULL; errno = 0;
    intmax_t x = strtoimax(src, &e, 10);
    ASSERT_EQ(INTMAX_MAX, x);
    ASSERT_EQ(ERANGE, errno);
}
TEST(strtoimax_underflow) {
    char src[64];
		strcpy(src, "-999999999999999999999999999999");
    char *e = NULL; errno = 0;
    intmax_t x = strtoimax(src, &e, 10);
    ASSERT_EQ(INTMAX_MIN, x);
    ASSERT_EQ(ERANGE, errno);
}
TEST(strtoimax_hex) {
    char *e = NULL;
    intmax_t x = strtoimax("0xA5", &e, 0);
    ASSERT_EQ(0xA5, x);
}
TEST(strtoimax_octal) {
    char *e = NULL;
    intmax_t x = strtoimax("077", &e, 0);
    ASSERT_EQ(63, x);
}
TEST(strtoimax_negative) {
    char *e = NULL;
    intmax_t x = strtoimax("-42", &e, 10);
    ASSERT_EQ(-42, x);
}
TEST(strtoimax_leading_ws) {
    char *e = NULL;
    intmax_t x = strtoimax("   999", &e, 10);
    ASSERT_EQ(999, x);
}
TEST(strtoimax_only_sign) {
    char buf[] = "+"; char *e = NULL;
    intmax_t x = strtoimax(buf, &e, 10);
    ASSERT_EQ(0, x);
    ASSERT_TRUE(e == buf);
}
TEST(strtoimax_empty) {
    char *e = NULL;
    intmax_t x = strtoimax("", &e, 10);
    ASSERT_EQ(0, x);
    ASSERT_TRUE(strcmp(e, "") == 0);
}
TEST(strtoimax_max) {
    char *e = NULL; char src[64]; snprintf(src,sizeof(src),"%" PRIdMAX,INTMAX_MAX);
    intmax_t x = strtoimax(src, &e, 10);
    ASSERT_EQ(INTMAX_MAX, x);
}
TEST(strtoimax_min) {
    char *e = NULL; char src[64]; snprintf(src,sizeof(src),"%" PRIdMAX,INTMAX_MIN);
    intmax_t x = strtoimax(src, &e, 10);
    ASSERT_EQ(INTMAX_MIN, x);
}

// -------- strtoumax --------
TEST_SUITE(strtoumax)
TEST(strtoumax_basic) {
    char *e = NULL;
    uintmax_t x = strtoumax("12345", &e, 10);
    ASSERT_EQ(12345u, x);
    ASSERT_TRUE(*e == '\0');
}
TEST(strtoumax_hex) {
    char *e = NULL;
    uintmax_t x = strtoumax("0xfF", &e, 0);
    ASSERT_EQ(255u, x);
}
TEST(strtoumax_octal) {
    char *e = NULL;
    uintmax_t x = strtoumax("077", &e, 0);
    ASSERT_EQ(63u, x);
}
TEST(strtoumax_neg) {
    char *e = NULL; errno = 0;
    uintmax_t x = strtoumax("-5", &e, 10);
    ASSERT_EQ(0u, x);
}
TEST(strtoumax_junk) {
    char *e = NULL;
    uintmax_t x = strtoumax("straw", &e, 10);
    ASSERT_EQ(0u, x);
    ASSERT_TRUE(strcmp(e, "straw") == 0);
}
TEST(strtoumax_partial) {
    char buf[] = "456abc"; char *e = NULL;
    uintmax_t x = strtoumax(buf, &e, 10);
    ASSERT_EQ(456u, x);
    ASSERT_TRUE(*e == 'a');
}
TEST(strtoumax_ws) {
    char *e = NULL;
    uintmax_t x = strtoumax("  88 ", &e, 10);
    ASSERT_EQ(88u, x);
}
TEST(strtoumax_empty) {
    char *e = NULL;
    uintmax_t x = strtoumax("", &e, 10);
    ASSERT_EQ(0u, x);
    ASSERT_TRUE(strcmp(e, "") == 0);
}
TEST(strtoumax_zero) {
    char *e = NULL;
    uintmax_t x = strtoumax("0", &e, 10);
    ASSERT_EQ(0u, x);
}
TEST(strtoumax_max) {
    char src[64]; snprintf(src,sizeof(src),"%" PRIuMAX,UINTMAX_MAX);
    char *e = NULL; errno = 0;
    uintmax_t x = strtoumax(src, &e, 10);
    ASSERT_EQ(UINTMAX_MAX, x);
    ASSERT_EQ(0, errno);
}
TEST(strtoumax_overflow) {
    char src[64]; snprintf(src,sizeof(src),"%" PRIuMAX "1",UINTMAX_MAX);
    char *e = NULL; errno = 0;
    uintmax_t x = strtoumax(src, &e, 10);
    ASSERT_EQ(UINTMAX_MAX, x);
    ASSERT_EQ(ERANGE, errno);
}

TEST_MAIN()

