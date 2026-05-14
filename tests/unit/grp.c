/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

/* Define the test path before including grp.h */
#define JACL_GROUP_PATH "/tmp/jacl_test_group"

#include <grp.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

/* Helper to create a temp group file for testing */
static void __create_test_group(const char *content) {
	FILE *f = fopen(JACL_GROUP_PATH, "w");
	if (f) {
		fprintf(f, "%s", content);
		fclose(f);
	}
}

TEST_TYPE(unit);
TEST_UNIT(grp.h);

/* ============================================================================ */
TEST_SUITE(getgrgid);

TEST(getgrgid_root) {
	struct group *g = getgrgid(0);
	if (g) {
		ASSERT_STR_EQ(g->gr_name, "root");
		ASSERT_EQ(0, g->gr_gid);
	} else {
		ASSERT_TRUE(errno == ENOENT || errno == 0);
	}
}

TEST(getgrgid_nonexistent) {
	errno = 0;
	struct group *g = getgrgid(999999);
	ASSERT_NULL(g);
	ASSERT_TRUE(errno != 0);
}

TEST(getgrgid_missing_fields) {
	__create_test_group("root:x:0\n"); /* Missing mem list */

	errno = 0;
	struct group *g = getgrgid(0);
	ASSERT_NULL(g);

	unlink(JACL_GROUP_PATH);
}

TEST(getgrgid_non_numeric_gid) {
	__create_test_group("root:x:abc:\n");

	errno = 0;
	struct group *g = getgrgid(0);
	ASSERT_NULL(g);

	unlink(JACL_GROUP_PATH);
}

/* ============================================================================ */
TEST_SUITE(getgrnam);

TEST(getgrnam_root) {
	struct group *g = getgrnam("root");
	if (g) {
		ASSERT_STR_EQ(g->gr_name, "root");
	} else {
		ASSERT_TRUE(errno == ENOENT || errno == 0);
	}
}

TEST(getgrnam_nonexistent) {
	errno = 0;
	struct group *g = getgrnam("jacl_nonexistent_group_xyz");
	ASSERT_NULL(g);
	ASSERT_TRUE(errno != 0);
}

TEST(getgrnam_null_name) {
	errno = 0;
	struct group *g = getgrnam(NULL);
	ASSERT_NULL(g);
}

TEST(getgrnam_r_erange_small_buffer) {
	__create_test_group("root:x:0:\n");

	struct group grp;
	char buf[10];
	struct group *result;

	int r = getgrnam_r("root", &grp, buf, sizeof(buf), &result);

	ASSERT_EQ(ERANGE, r);
	ASSERT_NULL(result);

	unlink(JACL_GROUP_PATH);
}

/* ============================================================================ */
TEST_SUITE(getgrgid_r);

TEST(getgrgid_r_root) {
	struct group grp;
	char buf[1024];
	struct group *result;

	int r = getgrgid_r(0, &grp, buf, sizeof(buf), &result);

	if (r == 0) {
		ASSERT_NOT_NULL(result);
		ASSERT_STR_EQ(result->gr_name, "root");
		ASSERT_EQ(&grp, result);
	} else {
		ASSERT_EQ(ENOENT, r);
		ASSERT_NULL(result);
	}
}

TEST(getgrgid_r_nonexistent) {
	struct group grp;
	char buf[1024];
	struct group *result;

	int r = getgrgid_r(999999, &grp, buf, sizeof(buf), &result);

	ASSERT_EQ(ENOENT, r);
	ASSERT_NULL(result);
}

TEST(getgrgid_r_invalid_args) {
	struct group grp;
	char buf[1024];
	struct group *result;

	ASSERT_EQ(EINVAL, getgrgid_r(0, NULL, buf, sizeof(buf), &result));
	ASSERT_EQ(EINVAL, getgrgid_r(0, &grp, NULL, sizeof(buf), &result));
	ASSERT_EQ(EINVAL, getgrgid_r(0, &grp, buf, sizeof(buf), NULL));
}

TEST(getgrgid_r_very_long_line) {
	char long_line[2048];
	memset(long_line, 'a', 2047);
	long_line[2047] = '\n';
	long_line[4] = ':';
	long_line[6] = ':';
	long_line[8] = ':';

	__create_test_group(long_line);

	struct group grp;
	char buf[100];
	struct group *result;

	int r = getgrgid_r(0, &grp, buf, sizeof(buf), &result);
	ASSERT_TRUE(r == ENOENT || r == ERANGE);
	ASSERT_NULL(result);

	unlink(JACL_GROUP_PATH);
}

TEST(getgrgid_r_erange_small_buffer) {
	__create_test_group("root:x:0:\n");

	struct group grp;
	char buf[10]; /* Too small for any meaningful data + pointers */
	struct group *result;

	int r = getgrgid_r(0, &grp, buf, sizeof(buf), &result);

	/* Should return ERANGE because buffer is too small */
	ASSERT_EQ(ERANGE, r);
	ASSERT_NULL(result);

	unlink(JACL_GROUP_PATH);
}

/* ============================================================================ */
TEST_SUITE(getgrnam_r);

TEST(getgrnam_r_root) {
	struct group grp;
	char buf[1024];
	struct group *result;

	int r = getgrnam_r("root", &grp, buf, sizeof(buf), &result);

	if (r == 0) {
		ASSERT_NOT_NULL(result);
		ASSERT_STR_EQ(result->gr_name, "root");
	} else {
		ASSERT_EQ(ENOENT, r);
		ASSERT_NULL(result);
	}
}

TEST(getgrnam_r_invalid_args) {
	struct group grp;
	char buf[1024];
	struct group *result;

	ASSERT_EQ(EINVAL, getgrnam_r(NULL, &grp, buf, sizeof(buf), &result));
	ASSERT_EQ(EINVAL, getgrnam_r("root", NULL, buf, sizeof(buf), &result));
	ASSERT_EQ(EINVAL, getgrnam_r("root", &grp, NULL, sizeof(buf), &result));
	ASSERT_EQ(EINVAL, getgrnam_r("root", &grp, buf, sizeof(buf), NULL));
}

/* ============================================================================ */
TEST_SUITE(getgrent);

TEST(getgrent_empty_file) {
	__create_test_group("");

	setgrent();
	struct group *g = getgrent();
	ASSERT_NULL(g);
	endgrent();

	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_comment_only) {
	__create_test_group("# Comment\n");

	setgrent();
	struct group *g = getgrent();
	ASSERT_NULL(g);
	endgrent();

	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_no_members) {
	__create_test_group("nogroup:x:65534:\n");

	setgrent();
	struct group *g = getgrent();
	ASSERT_NOT_NULL(g);
	ASSERT_STR_EQ(g->gr_name, "nogroup");
	ASSERT_NOT_NULL(g->gr_mem);
	ASSERT_NULL(g->gr_mem[0]); /* Empty list */
	endgrent();

	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_single_member) {
	__create_test_group("users:x:100:alice\n");

	setgrent();
	struct group *g = getgrent();
	ASSERT_NOT_NULL(g);
	ASSERT_STR_EQ(g->gr_name, "users");
	ASSERT_NOT_NULL(g->gr_mem[0]);
	ASSERT_STR_EQ(g->gr_mem[0], "alice");
	ASSERT_NULL(g->gr_mem[1]);
	endgrent();

	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_multiple_members) {
	__create_test_group("devs:x:1001:alice,bob,charlie\n");

	setgrent();
	struct group *g = getgrent();
	ASSERT_NOT_NULL(g);
	ASSERT_STR_EQ(g->gr_mem[0], "alice");
	ASSERT_STR_EQ(g->gr_mem[1], "bob");
	ASSERT_STR_EQ(g->gr_mem[2], "charlie");
	ASSERT_NULL(g->gr_mem[3]);
	endgrent();

	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_basic) {
	__create_test_group("root:x:0:\ndaemon:x:1:\n");

	setgrent();
	struct group *g1 = getgrent();
	ASSERT_NOT_NULL(g1);
	ASSERT_STR_EQ(g1->gr_name, "root");

	struct group *g2 = getgrent();
	ASSERT_NOT_NULL(g2);
	ASSERT_STR_EQ(g2->gr_name, "daemon");

	ASSERT_NULL(getgrent());

	endgrent();
	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_reset) {
	__create_test_group("root:x:0:\n");

	setgrent();
	struct group *g1 = getgrent();
	endgrent();

	setgrent();
	struct group *g2 = getgrent();

	ASSERT_NOT_NULL(g1);
	ASSERT_NOT_NULL(g2);
	ASSERT_STR_EQ(g1->gr_name, g2->gr_name);

	endgrent();
	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_safe) {
	endgrent();
	endgrent();
}

TEST(getgrent_trailing_comma) {
	__create_test_group("devs:x:1001:alice,\n");

	setgrent();
	struct group *g = getgrent();
	ASSERT_NOT_NULL(g);
	ASSERT_STR_EQ(g->gr_mem[0], "alice");
	ASSERT_NULL(g->gr_mem[1]);
	endgrent();

	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_leading_comma) {
	__create_test_group("devs:x:1001:,alice\n");

	setgrent();
	struct group *g = getgrent();
	ASSERT_NOT_NULL(g);
	ASSERT_STR_EQ(g->gr_mem[0], "alice");
	ASSERT_NULL(g->gr_mem[1]);
	endgrent();

	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_duplicate_members) {
	__create_test_group("devs:x:1001:alice,alice\n");

	setgrent();
	struct group *g = getgrent();
	ASSERT_NOT_NULL(g);
	ASSERT_STR_EQ(g->gr_mem[0], "alice");
	ASSERT_STR_EQ(g->gr_mem[1], "alice");
	ASSERT_NULL(g->gr_mem[2]);
	endgrent();

	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_many_members) {
	char line[1024];
	strcpy(line, "biggroup:x:1002:");
	char *p = line + strlen(line);
	for (int i = 0; i < 50; i++) {
		if (i > 0) *p++ = ',';
		p += sprintf(p, "user%d", i);
	}
	*p++ = '\n';
	*p = '\0';

	__create_test_group(line);

	setgrent();
	struct group *g = getgrent();
	ASSERT_NOT_NULL(g);
	ASSERT_STR_EQ(g->gr_mem[0], "user0");
	ASSERT_STR_EQ(g->gr_mem[49], "user49");
	ASSERT_NULL(g->gr_mem[50]);
	endgrent();

	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_whitespace_in_members) {
	__create_test_group("devs:x:1001: alice ,bob\n");

	setgrent();
	struct group *g = getgrent();
	ASSERT_NOT_NULL(g);
	ASSERT_STR_EQ(g->gr_mem[0], " alice ");
	ASSERT_STR_EQ(g->gr_mem[1], "bob");
	endgrent();

	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_gid_zero) {
	__create_test_group("root:x:0:\n");

	setgrent();
	struct group *g = getgrent();
	ASSERT_NOT_NULL(g);
	ASSERT_EQ(0, g->gr_gid);
	endgrent();

	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_gid_max) {
	char line[64];
	sprintf(line, "biggroup:x:%lu:\n", (unsigned long)UINT_MAX);
	__create_test_group(line);

	setgrent();
	struct group *g = getgrent();
	ASSERT_NOT_NULL(g);
	ASSERT_EQ((gid_t)UINT_MAX, g->gr_gid);
	endgrent();

	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_negative_gid_string) {
	__create_test_group("badgroup:x:-1:\n");

	setgrent();
	struct group *g = getgrent();
	if (g) {
		ASSERT_TRUE(g->gr_gid == (gid_t)-1 || g->gr_gid == (gid_t)UINT_MAX);
	} else {
		ASSERT_TRUE(errno != 0);
	}
	endgrent();

	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_double_comma) {
	__create_test_group("devs:x:1001:alice,,bob\n");

	setgrent();
	struct group *g = getgrent();
	ASSERT_NOT_NULL(g);
	ASSERT_STR_EQ(g->gr_mem[0], "alice");
	ASSERT_STR_EQ(g->gr_mem[1], "bob");
	ASSERT_NULL(g->gr_mem[2]);
	endgrent();

	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_only_commas) {
	__create_test_group("devs:x:1001:,,,\n");

	setgrent();
	struct group *g = getgrent();
	ASSERT_NOT_NULL(g);
	ASSERT_NULL(g->gr_mem[0]);
	endgrent();

	unlink(JACL_GROUP_PATH);
}

TEST(getgrent_spaces_only_in_members) {
	__create_test_group("devs:x:1001:   \n");

	setgrent();
	struct group *g = getgrent();
	ASSERT_NOT_NULL(g);
	ASSERT_STR_EQ(g->gr_mem[0], "   ");
	ASSERT_NULL(g->gr_mem[1]);
	endgrent();

	unlink(JACL_GROUP_PATH);
}

/* ============================================================================ */
TEST_MAIN()
