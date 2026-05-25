/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <monetary.h>
#include <locale.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(monetary.h);

/* ============================================================================ */
TEST_SUITE(strfmon_l);

TEST(strfmon_l_basic_currency) {
    char buf[64] = {0};
    locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    ssize_t r = strfmon_l(buf, sizeof(buf), loc, "%n", 1234.56);
    ASSERT_TRUE(r > 0);
    ASSERT_STR_HAS("1234.56", buf);
    freelocale(loc);
}

TEST(strfmon_l_international) {
    char buf[64] = {0};
    locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    ssize_t r = strfmon_l(buf, sizeof(buf), loc, "%i", 100.00);
    ASSERT_TRUE(r > 0);
    ASSERT_STR_HAS("100.00", buf);
    freelocale(loc);
}

TEST(strfmon_l_escape_percent) {
    char buf[64] = {0};
    ssize_t r = strfmon_l(buf, sizeof(buf), NULL, "%%test");
    ASSERT_EQ(r, 5);
    ASSERT_STR_EQ(buf, "%test");
}

/* ============================================================================ */
TEST_SUITE(strfmon);

TEST(strfmon_basic) {
    char buf[64] = {0};
    ssize_t r = strfmon(buf, sizeof(buf), "%n", 50.0);
    ASSERT_TRUE(r > 0);
    ASSERT_STR_HAS("50.00", buf);
}

TEST(strfmon_explicit_precision) {
    char buf[64] = {0};
    ssize_t r = strfmon(buf, sizeof(buf), "%.3n", 99.999);
    ASSERT_TRUE(r > 0);
    ASSERT_STR_HAS("99.999", buf);
}

TEST(strfmon_null_s) {
    ssize_t r = strfmon(NULL, 10, "%n", 1.0);
    ASSERT_EQ(r, -1);
}

TEST(strfmon_zero_size) {
    char buf[64];
    ssize_t r = strfmon(buf, 0, "%n", 1.0);
    ASSERT_EQ(r, -1);
}

/* ============================================================================ */
TEST_SUITE(monetary_locales);

TEST(monetary_locale_symbols_us) {
    /* en_US should have a currency symbol (usually $) */
    locale_t loc = newlocale(LC_ALL_MASK, "en_US.UTF-8", NULL);
    if (loc) {
        char buf[64] = {0};
        ssize_t r = strfmon_l(buf, sizeof(buf), loc, "%n", 100.00);
        ASSERT_TRUE(r > 0);
        freelocale(loc);
    }
}

TEST(monetary_all_countries_format) {
    #define X(CC, name, code) \
        if (CC != CC_NONE && CC != CC_ERR) { \
            locale_t loc = newlocale(LC_MONETARY_MASK, "en_" name ".UTF-8", NULL); \
            if (loc) { \
                char buf[64]; \
                ssize_t r = strfmon_l(buf, sizeof(buf), loc, "%i", 99.99); \
                ASSERT_TRUE(r > 0); \
                freelocale(loc); \
            } \
        }
    X_COUNTRIES
    #undef X
}

TEST(monetary_all_languages_format) {
    #define X(LANG, name, code2, code3) \
        if (LANG != LANG_C && LANG != LANG_ERR) { \
            /* Use a generic country like US for the language test to isolate language effects */ \
            locale_t loc = newlocale(LC_MONETARY_MASK, name "_US.UTF-8", NULL); \
            if (loc) { \
                char buf[64]; \
                ssize_t r = strfmon_l(buf, sizeof(buf), loc, "%n", 1.23); \
                ASSERT_TRUE(r > 0); \
                freelocale(loc); \
            } \
        }
    X_LANGUAGES
    #undef X
}

/* ============================================================================ */
TEST_SUITE(locale_integration);

TEST(strfmon_respects_lconv_defaults) {
    char buf[64] = {0};
    setlocale(LC_MONETARY, "C");
    ssize_t r = strfmon(buf, sizeof(buf), "%n", 12.5);
    ASSERT_TRUE(r > 0); /* Should not crash with CHAR_MAX frac_digits */
}

/* ============================================================================ */
TEST_MAIN()
