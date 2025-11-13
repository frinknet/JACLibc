/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <inttypes.h>

TEST_TYPE(unit)
TEST_UNIT(inttypes.h)

/* ---- PRI Macro Suites ---- */
TEST_SUITE(macro_pri_8bit)
TEST(macro_pri_8bit)
{
    ASSERT_STR_EQ("hhd", PRId8);
    ASSERT_STR_EQ("hhi", PRIi8);
}

#if JACL_INTMAX_BITS >= 16
TEST_SUITE(macro_pri_16bit)
TEST(macro_pri_16bit)
{
    ASSERT_STR_EQ("hd", PRId16);
    ASSERT_STR_EQ("hi", PRIi16);
}
#endif

#if JACL_INTMAX_BITS >= 32
TEST_SUITE(macro_pri_32bit)
TEST(macro_pri_32bit)
{
    ASSERT_STR_EQ("d", PRId32);
    ASSERT_STR_EQ("i", PRIi32);
}
#endif

#if JACL_INTMAX_BITS >= 64
TEST_SUITE(macro_pri_64bit)
TEST(macro_pri_64bit)
{
    ASSERT_STR_EQ("lld", PRId64);
    ASSERT_STR_EQ("lli", PRIi64);
}
#endif

TEST_SUITE(macro_pri_ptr)
TEST(macro_pri_ptr)
{
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

/* ---- SCN Macro Suites ---- */
TEST_SUITE(macro_scn_8bit)
TEST(macro_scn_8bit)
{
    ASSERT_STR_EQ("hhd", SCNd8);
    ASSERT_STR_EQ("hhi", SCNi8);
}

#if JACL_INTMAX_BITS >= 16
TEST_SUITE(macro_scn_16bit)
TEST(macro_scn_16bit)
{
    ASSERT_STR_EQ("hd", SCNd16);
    ASSERT_STR_EQ("hi", SCNi16);
}
#endif

#if JACL_INTMAX_BITS >= 32
TEST_SUITE(macro_scn_32bit)
TEST(macro_scn_32bit)
{
    ASSERT_STR_EQ("d", SCNd32);
    ASSERT_STR_EQ("i", SCNi32);
}
#endif

#if JACL_INTMAX_BITS >= 64
TEST_SUITE(macro_scn_64bit)
TEST(macro_scn_64bit)
{
    ASSERT_STR_EQ("lld", SCNd64);
    ASSERT_STR_EQ("lli", SCNi64);
}
#endif

TEST_SUITE(macro_scn_ptr)
TEST(macro_scn_ptr)
{
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

/* ---- Formatted Print Suites ---- */
TEST_SUITE(formatted_8bit)
TEST(formatted_8bit)
{
    char buf[16];
    int8_t a = 127;
    snprintf(buf, sizeof(buf), "%" PRId8, a);
    ASSERT_STR_EQ("127", buf);
}

#if JACL_INTMAX_BITS >= 16
TEST_SUITE(formatted_16bit)
TEST(formatted_16bit)
{
    char buf[16];
    int16_t a = 4123;
    snprintf(buf, sizeof(buf), "%" PRId16, a);
    ASSERT_STR_EQ("4123", buf);
}
#endif

#if JACL_INTMAX_BITS >= 32
TEST_SUITE(formatted_32bit)
TEST(formatted_32bit)
{
    char buf[32];
    int32_t a = 12345678;
    snprintf(buf, sizeof(buf), "%" PRId32, a);
    ASSERT_TRUE(strstr(buf, "12345678") != NULL);
}
#endif

#if JACL_INTMAX_BITS >= 64
TEST_SUITE(formatted_64bit)
TEST(formatted_64bit)
{
    char buf[64];
    int64_t x = INT64_MAX;
    snprintf(buf, sizeof(buf), "%" PRId64, (long long)x);
    ASSERT_STR_EQ("9223372036854775807", buf);
}
#endif

TEST_SUITE(formatted_ptr)
TEST(formatted_ptr)
{
    char buf[64];
    uintptr_t p = 0xCAFEBABEULL;
    snprintf(buf, sizeof(buf), "%" PRIxPTR, p);
    ASSERT_TRUE(strstr(buf, "cafebabe") != NULL || strstr(buf, "CAFEBABE") != NULL);
}

/* ---- C99 Function/Test Suites ---- */
#if JACL_HAS_C99

TEST_SUITE(type_definitions)
TEST(type_definitions)
{
    intmax_t x = 123;
    ASSERT_EQ(123, x);
    uintmax_t y = 456u;
    ASSERT_EQ(456u, y);
    imaxdiv_t r = { .quot = 17, .rem = 3 };
    ASSERT_EQ(17, r.quot);
    ASSERT_EQ(3, r.rem);
}

TEST_SUITE(imaxdiv_basic)
TEST(imaxdiv_basic)
{
    imaxdiv_t d = imaxdiv(13, 5);
    ASSERT_EQ(2, d.quot);
    ASSERT_EQ(3, d.rem);
}

TEST_SUITE(imaxdiv_signed_cases)
TEST(imaxdiv_signed_cases)
{
    imaxdiv_t d = imaxdiv(-13, 5);
    ASSERT_EQ(-2, d.quot);
    ASSERT_EQ(-3, d.rem);
    d = imaxdiv(13, -5);
    ASSERT_EQ(-2, d.quot);
    ASSERT_EQ(3, d.rem);
    d = imaxdiv(-13, -5);
    ASSERT_EQ(2, d.quot);
    ASSERT_EQ(-3, d.rem);
}

TEST_SUITE(imaxdiv_zero)
TEST(imaxdiv_zero)
{
    imaxdiv_t d = imaxdiv(0, 7);
    ASSERT_EQ(0, d.quot);
    ASSERT_EQ(0, d.rem);
}

TEST_SUITE(imaxdiv_min_by_one)
TEST(imaxdiv_min_by_one)
{
    imaxdiv_t d = imaxdiv(INTMAX_MIN, 1);
    ASSERT_EQ(INTMAX_MIN, d.quot);
    ASSERT_EQ(0, d.rem);
}

TEST_SUITE(imaxdiv_min_by_max)
TEST(imaxdiv_min_by_max)
{
    imaxdiv_t d = imaxdiv(INTMAX_MIN, INTMAX_MAX);
    ASSERT_EQ(-1, d.quot);
    ASSERT_EQ(-1, d.rem);
}

TEST_SUITE(strtoimax_basic)
TEST(strtoimax_basic)
{
    char *end = NULL;
    const char src[] = "12345";
    intmax_t x = strtoimax(src, &end, 10);
    ASSERT_EQ(12345, x);
    ASSERT_EQ('\0', *end);
}

TEST_SUITE(strtoimax_partial)
TEST(strtoimax_partial)
{
    char buf[] = "123junk";
    char *end = NULL;
    intmax_t v = strtoimax(buf, &end, 10);
    ASSERT_EQ(v, 123);
    ASSERT_EQ(*end, 'j');
    ASSERT_TRUE(end > buf && end <= buf + strlen(buf));
}

TEST_SUITE(strtoimax_dec)
TEST(strtoimax_dec)
{
    char *end;
    intmax_t a = strtoimax("123", &end, 0);
    ASSERT_EQ(123, a);
}

TEST_SUITE(strtoimax_hex)
TEST(strtoimax_hex)
{
    char *end;
    intmax_t b = strtoimax("0x123", &end, 0);
    ASSERT_EQ(0x123, b);
}

TEST_SUITE(strtoimax_oct)
TEST(strtoimax_oct)
{
    char *end;
    intmax_t c = strtoimax("0755", &end, 0);
    ASSERT_EQ(493, c);
}

TEST_SUITE(strtoimax_negative)
TEST(strtoimax_negative)
{
    char *end;
    intmax_t v = strtoimax("-0x11", &end, 0);
    ASSERT_EQ(-17, v);
}

TEST_SUITE(strtoimax_big)
TEST(strtoimax_big)
{
    char src[64];
    snprintf(src, sizeof(src), "%" PRIuMAX, (uintmax_t)INTMAX_MAX + 1);
    char *end = NULL;
    errno = 0;
    intmax_t v = strtoimax(src, &end, 10);
    ASSERT_EQ(INTMAX_MAX, v);
    ASSERT_EQ(ERANGE, errno);
}

TEST_SUITE(strtoimax_smass)
TEST(strtoimax_smass)
{
    char src[64];
    snprintf(src, sizeof(src), "%jd", (intmax_t)INTMAX_MIN - 1);
    char *end = NULL;
    errno = 0;
    intmax_t v = strtoimax(src, &end, 10);
    ASSERT_EQ(INTMAX_MIN, v);
    ASSERT_EQ(ERANGE, errno);
}

TEST_SUITE(strtoimax_only_plus)
TEST(strtoimax_only_plus)
{
    char buf[] = "+";
    char *end = NULL;
    intmax_t v = strtoimax(buf, &end, 10);
    ASSERT_EQ(v, 0);
    ASSERT_EQ(end, buf);
}

TEST_SUITE(strtoumax_basic)
TEST(strtoumax_basic)
{
    char *end = NULL;
    const char src[] = "65536";
    uintmax_t x = strtoumax(src, &end, 10);
    ASSERT_EQ(65536u, x);
    ASSERT_EQ('\0', *end);
}

TEST_SUITE(strtoumax_hex)
TEST(strtoumax_hex)
{
    char *end;
    uintmax_t v = strtoumax("0xffff", &end, 16);
    ASSERT_EQ(0xffffu, v);
}

TEST_SUITE(strtoumax_underflow)
TEST(strtoumax_underflow)
{
    char *end = NULL;
    errno = 0;
    uintmax_t v = strtoumax("-100", &end, 10);
    ASSERT_EQ(0, v); // Underflow for unsigned types returns 0
    // Some platforms may set errno, some may not
}

TEST_SUITE(strtoumax_big)
TEST(strtoumax_big)
{
    char src[64];
    snprintf(src, sizeof(src), "%" PRIuMAX "0", UINTMAX_MAX);
    char *end = NULL;
    errno = 0;
    uintmax_t v = strtoumax(src, &end, 10);
    ASSERT_EQ(UINTMAX_MAX, v);
    ASSERT_EQ(ERANGE, errno);
}

TEST_SUITE(strtoumax_smass)
TEST(strtoumax_smass)
{
    char *end = NULL;
    errno = 0;
    uintmax_t v = strtoumax("-1", &end, 10);
    ASSERT_EQ(0, v);
}

TEST_SUITE(strtoumax_base16)
TEST(strtoumax_base16)
{
    char buf[] = "0xabcF012";
    char *end = NULL;
    uintmax_t v = strtoumax(buf, &end, 0);
    ASSERT_TRUE(v > 0);
    ASSERT_TRUE(*end == '\0' || *end == ' ' || *end == '\n');
}

#endif /* JACL_HAS_C99 */

TEST_MAIN()
