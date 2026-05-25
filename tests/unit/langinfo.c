/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <langinfo.h>
#include <locale.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(langinfo.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_enum_values) {
    ASSERT_EQ(CODESET, 1);
    ASSERT_EQ(D_T_FMT, 2);
    ASSERT_EQ(CRNCYSTR, 79);
    ASSERT_NE(CODESET, D_T_FMT);
}

/* ============================================================================ */
TEST_SUITE(nl_langinfo_l);

TEST(nl_langinfo_l_codeset_c_locale) {
    locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    char *s = nl_langinfo_l(CODESET, loc);
    ASSERT_STR_EQ(s, "UTF-8");
    freelocale(loc);
}

TEST(nl_langinfo_l_time_formats) {
    locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    ASSERT_STR_EQ(nl_langinfo_l(D_FMT, loc), "%m/%d/%y");
    ASSERT_STR_EQ(nl_langinfo_l(T_FMT, loc), "%H:%M:%S");
    freelocale(loc);
}

TEST(nl_langinfo_l_days) {
    locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    ASSERT_STR_EQ(nl_langinfo_l(DAY_1, loc), "Sunday");
    ASSERT_STR_EQ(nl_langinfo_l(ABDAY_1, loc), "Sun");
    freelocale(loc);
}

TEST(nl_langinfo_l_months) {
    locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    ASSERT_STR_EQ(nl_langinfo_l(MON_1, loc), "January");
    ASSERT_STR_EQ(nl_langinfo_l(ABMON_1, loc), "Jan");
    freelocale(loc);
}

TEST(nl_langinfo_l_numeric) {
    locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    ASSERT_STR_EQ(nl_langinfo_l(RADIXCHAR, loc), ".");
    /* POSIX C locale has empty thousands_sep */
    ASSERT_STR_EQ(nl_langinfo_l(THOUSEP, loc), "");
    freelocale(loc);
}

TEST(nl_langinfo_l_invalid_item) {
    locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    char *s = nl_langinfo_l(9999, loc);
    ASSERT_STR_EQ(s, "");
    freelocale(loc);
}

TEST(nl_langinfo_l_null_locale) {
    char *s = nl_langinfo_l(CODESET, NULL);
    ASSERT_NOT_NULL(s);
}

/* ============================================================================ */
TEST_SUITE(nl_langinfo);

TEST(nl_langinfo_wrapper) {
    char *s1 = nl_langinfo(CODESET);
    char *s2 = nl_langinfo_l(CODESET, NULL);
    ASSERT_STR_EQ(s1, s2);
}

TEST(nl_langinfo_after_setlocale) {
    setlocale(LC_TIME, "C");
    ASSERT_STR_EQ(nl_langinfo(DAY_1), "Sunday");
}

/* ============================================================================ */
TEST_SUITE(integration);

TEST(integration_locale_change_updates_langinfo) {
    locale_t en = newlocale(LC_TIME_MASK, "en_US", NULL);
    locale_t old = uselocale(en);
    char *day = nl_langinfo(DAY_1);
    ASSERT_STR_EQ(day, "Sunday");
    uselocale(old);
    freelocale(en);
}

/* ============================================================================ */
TEST_SUITE(all_languages);

TEST(all_languages_codeset) {
    #define X(LANG, name, code2, code3) \
        if (LANG != LANG_C) { \
            locale_t loc = newlocale(LC_ALL_MASK, name "_US.UTF-8", NULL); \
            if (loc) { \
                char *s = nl_langinfo_l(CODESET, loc); \
                ASSERT_NOT_NULL(s); \
                freelocale(loc); \
            } \
        }
    X_LANGUAGES
    #undef X
}

TEST(all_languages_day_1) {
    #define X(LANG, name, code2, code3) \
        if (LANG != LANG_C) { \
            locale_t loc = newlocale(LC_ALL_MASK, name "_US.UTF-8", NULL); \
            if (loc) { \
                char *s = nl_langinfo_l(DAY_1, loc); \
                ASSERT_NOT_NULL(s); \
                freelocale(loc); \
            } \
        }
    X_LANGUAGES
    #undef X
}

TEST(all_languages_mon_1) {
    #define X(LANG, name, code2, code3) \
        if (LANG != LANG_C) { \
            locale_t loc = newlocale(LC_ALL_MASK, name "_US.UTF-8", NULL); \
            if (loc) { \
                char *s = nl_langinfo_l(MON_1, loc); \
                ASSERT_NOT_NULL(s); \
                freelocale(loc); \
            } \
        }
    X_LANGUAGES
    #undef X
}

/* ============================================================================ */
TEST_MAIN()
