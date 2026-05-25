/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <nl_types.h>
#include <string.h>
#include <errno.h>

TEST_TYPE(unit);
TEST_UNIT(nl_types.h);

/* ============================================================================ */
TEST_SUITE(nl_constants);

TEST(nl_constants_setid) {
	ASSERT_INT_EQ(NL_SETID, 1);
}

TEST(nl_constants_locale) {
	ASSERT_INT_EQ(NL_CAT_LOCALE, 1);
}

/* ============================================================================ */
TEST_SUITE(nl_types_t);

TEST(nl_catd_size_positive) {
	ASSERT_SIZE_MIN(nl_catd, sizeof(void *));
}

TEST(nl_item_type) {
	nl_item i = 10001;
	ASSERT_INT_EQ(i, 10001);
}

/* ============================================================================ */
TEST_SUITE(catopen);

TEST(catopen_valid_name) {
	nl_catd c = catopen("test.cat", 0);
	ASSERT_NOT_NULL(c);
	ASSERT_INT_NE(c, (nl_catd)-1);
	catclose(c);
}

TEST(catopen_null_name) {
	nl_catd c = catopen(NULL, 0);
	ASSERT_PTR_EQ(c, (nl_catd)-1);
	ASSERT_INT_EQ(errno, EINVAL);
}

/* ============================================================================ */
TEST_SUITE(catgets);

TEST(catgets_default_fallback) {
	nl_catd c = catopen("empty.cat", 0);
	ASSERT_STR_EQ(catgets(c, 1, 1, "default"), "default");
	catclose(c);
}

TEST(catgets_null_handle_fallback) {
	ASSERT_STR_EQ(catgets(NULL, 1, 1, "null_handle"), "null_handle");
}

TEST(catgets_invalid_handle_fallback) {
	ASSERT_STR_EQ(catgets((nl_catd)-1, 1, 1, "bad_handle"), "bad_handle");
}

/* ============================================================================ */
TEST_SUITE(catclose);

TEST(catclose_valid) {
	nl_catd c = catopen("close.cat", 0);
	ASSERT_INT_EQ(catclose(c), 0);
}

TEST(catclose_null_handle) {
	ASSERT_INT_EQ(catclose(NULL), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(catclose_invalid_handle) {
	ASSERT_INT_EQ(catclose((nl_catd)-1), -1);
	ASSERT_ERRNO(EBADF);
}

/* ============================================================================ */
TEST_MAIN()
