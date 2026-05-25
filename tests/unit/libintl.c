/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <libintl.h>
#include <locale.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(libintl.h);

/* ============================================================================ */
TEST_SUITE(gettext_l);

TEST(gettext_l_passthrough) {
    const char *msg = "Hello";
    locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    ASSERT_STR_EQ(gettext_l(msg, loc), msg);
    freelocale(loc);
}

TEST(gettext_l_null_locale) {
    ASSERT_STR_EQ(gettext_l("World", NULL), "World");
}

/* ============================================================================ */
TEST_SUITE(gettext);

TEST(gettext_wrapper) {
    ASSERT_STR_EQ(gettext("Test"), gettext_l("Test", NULL));
}

/* ============================================================================ */
TEST_SUITE(ngettext_l);

TEST(ngettext_l_singular) {
    locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    ASSERT_STR_EQ(ngettext_l("%d file", "%d files", 1, loc), "%d file");
    freelocale(loc);
}

TEST(ngettext_l_plural) {
    locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    ASSERT_STR_EQ(ngettext_l("%d file", "%d files", 5, loc), "%d files");
    freelocale(loc);
}

TEST(ngettext_l_zero_count) {
    locale_t loc = newlocale(LC_ALL_MASK, "C", NULL);
    ASSERT_STR_EQ(ngettext_l("%d file", "%d files", 0, loc), "%d files");
    freelocale(loc);
}

/* ============================================================================ */
TEST_SUITE(ngettext);

TEST(ngettext_wrapper) {
    ASSERT_STR_EQ(ngettext("A", "B", 1), ngettext_l("A", "B", 1, NULL));
}

/* ============================================================================ */
TEST_SUITE(domain_management);

TEST(textdomain_returns_default) {
    char *s = textdomain("myapp");
    ASSERT_STR_EQ(s, "messages");
}

TEST(bindtextdomain_returns_default) {
    char *s = bindtextdomain("myapp", "/custom/locale");
    ASSERT_STR_EQ(s, "/usr/share/locale"); /* Stub behavior */
}

TEST(bind_textdomain_codeset_returns_utf8) {
    char *s = bind_textdomain_codeset("myapp", "ISO-8859-1");
    ASSERT_STR_EQ(s, "UTF-8"); /* Stub behavior */
}

/* ============================================================================ */
TEST_SUITE(integration);

TEST(integration_gettext_with_locale) {
    locale_t loc = newlocale(LC_MESSAGES_MASK, "C", NULL);
    const char *msg = "Error";
    ASSERT_STR_EQ(dgettext_l("libc", msg, loc), msg);
    freelocale(loc);
}

/* ============================================================================ */
TEST_MAIN()
