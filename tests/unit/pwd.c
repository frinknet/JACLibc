/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

/* Define the test path before including pwd.h */
#define JACL_PASSWD_PATH "/tmp/jacl_test_passwd"

#include <pwd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

/* Helper to create a temp passwd file for testing */
static void __create_test_passwd(const char *content) {
	FILE *f = fopen(JACL_PASSWD_PATH, "w");
	if (f) {
		fprintf(f, "%s", content);
		fclose(f);
	}
}

TEST_TYPE(unit);
TEST_UNIT(pwd.h);

/* ============================================================================ */
TEST_SUITE(getpwuid);

TEST(getpwuid_root) {
	struct passwd *p = getpwuid(0);
	if (p) {
		ASSERT_STR_EQ(p->pw_name, "root");
		ASSERT_EQ(0, p->pw_uid);
		ASSERT_EQ(0, p->pw_gid);
	} else {
		ASSERT_TRUE(errno == ENOENT || errno == 0);
	}
}

TEST(getpwuid_nonexistent) {
	errno = 0;
	struct passwd *p = getpwuid(999999);
	ASSERT_NULL(p);
	ASSERT_TRUE(errno != 0);
}

/* ============================================================================ */
TEST_SUITE(getpwnam);

TEST(getpwnam_root) {
	struct passwd *p = getpwnam("root");
	if (p) {
		ASSERT_STR_EQ(p->pw_name, "root");
	} else {
		ASSERT_TRUE(errno == ENOENT || errno == 0);
	}
}

TEST(getpwnam_nonexistent) {
	errno = 0;
	struct passwd *p = getpwnam("jacl_nonexistent_user_xyz");
	ASSERT_NULL(p);
	ASSERT_TRUE(errno != 0);
}

TEST(getpwnam_null_name) {
	errno = 0;
	struct passwd *p = getpwnam(NULL);
	ASSERT_NULL(p);
	ASSERT_TRUE(errno == EINVAL || errno == ENOENT || errno == 0);
}

/* ============================================================================ */
TEST_SUITE(getpwuid_r);

TEST(getpwuid_r_root) {
	struct passwd pwd;
	char buf[1024];
	struct passwd *result;
	
	int r = getpwuid_r(0, &pwd, buf, sizeof(buf), &result);
	
	if (r == 0) {
		ASSERT_NOT_NULL(result);
		ASSERT_STR_EQ(result->pw_name, "root");
		ASSERT_EQ(&pwd, result);
	} else {
		ASSERT_EQ(ENOENT, r);
		ASSERT_NULL(result);
	}
}

TEST(getpwuid_r_nonexistent) {
	struct passwd pwd;
	char buf[1024];
	struct passwd *result;
	
	int r = getpwuid_r(999999, &pwd, buf, sizeof(buf), &result);
	
	ASSERT_EQ(ENOENT, r);
	ASSERT_NULL(result);
}

TEST(getpwuid_r_invalid_args) {
	struct passwd pwd;
	char buf[1024];
	struct passwd *result;
	
	ASSERT_EQ(EINVAL, getpwuid_r(0, NULL, buf, sizeof(buf), &result));
	ASSERT_EQ(EINVAL, getpwuid_r(0, &pwd, NULL, sizeof(buf), &result));
	ASSERT_EQ(EINVAL, getpwuid_r(0, &pwd, buf, sizeof(buf), NULL));
}

TEST(getpwuid_r_small_buffer) {
	struct passwd pwd;
	char buf[1];
	struct passwd *result;
	
	int r = getpwuid_r(0, &pwd, buf, sizeof(buf), &result);
	ASSERT_TRUE(r == ENOENT || r == ERANGE);
	ASSERT_NULL(result);
}

/* ============================================================================ */
TEST_SUITE(getpwnam_r);

TEST(getpwnam_r_root) {
	struct passwd pwd;
	char buf[1024];
	struct passwd *result;
	
	int r = getpwnam_r("root", &pwd, buf, sizeof(buf), &result);
	
	if (r == 0) {
		ASSERT_NOT_NULL(result);
		ASSERT_STR_EQ(result->pw_name, "root");
	} else {
		ASSERT_EQ(ENOENT, r);
		ASSERT_NULL(result);
	}
}

TEST(getpwnam_r_nonexistent) {
	struct passwd pwd;
	char buf[1024];
	struct passwd *result;
	
	int r = getpwnam_r("jacl_nonexistent", &pwd, buf, sizeof(buf), &result);
	
	ASSERT_EQ(ENOENT, r);
	ASSERT_NULL(result);
}

TEST(getpwnam_r_invalid_args) {
	struct passwd pwd;
	char buf[1024];
	struct passwd *result;
	
	ASSERT_EQ(EINVAL, getpwnam_r(NULL, &pwd, buf, sizeof(buf), &result));
	ASSERT_EQ(EINVAL, getpwnam_r("root", NULL, buf, sizeof(buf), &result));
	ASSERT_EQ(EINVAL, getpwnam_r("root", &pwd, NULL, sizeof(buf), &result));
	ASSERT_EQ(EINVAL, getpwnam_r("root", &pwd, buf, sizeof(buf), NULL));
}

/* ============================================================================ */
TEST_SUITE(parser_sad_paths);

TEST(parser_missing_fields) {
	__create_test_passwd("root:x:0\n");
	
	errno = 0;
	struct passwd *p = getpwuid(0);
	ASSERT_NULL(p);
	
	unlink(JACL_PASSWD_PATH);
}

TEST(parser_non_numeric_uid) {
	__create_test_passwd("root:x:abc:0:Root:/root:/bin/sh\n");
	
	errno = 0;
	struct passwd *p = getpwuid(0);
	ASSERT_NULL(p);
	
	unlink(JACL_PASSWD_PATH);
}

TEST(parser_empty_file) {
	__create_test_passwd("");
	
	setpwent();
	struct passwd *p = getpwent();
	ASSERT_NULL(p);
	endpwent();
	
	unlink(JACL_PASSWD_PATH);
}

TEST(parser_comment_only) {
	__create_test_passwd("# This is a comment\n# Another comment\n");
	
	setpwent();
	struct passwd *p = getpwent();
	ASSERT_NULL(p);
	endpwent();
	
	unlink(JACL_PASSWD_PATH);
}

TEST(parser_long_line_truncation) {
	char long_line[2048];
	memset(long_line, 'a', 2047);
	long_line[2047] = '\n';
	long_line[10] = ':';
	
	__create_test_passwd(long_line);
	
	setpwent();
	struct passwd *p = getpwent();
	ASSERT_NULL(p);
	endpwent();
	
	unlink(JACL_PASSWD_PATH);
}

/* ============================================================================ */
TEST_SUITE(buffer_overflow_protection);

TEST(getpwuid_r_zero_length_buffer) {
	struct passwd pwd;
	char buf[1];
	struct passwd *result;
	
	int r = getpwuid_r(0, &pwd, buf, 0, &result);
	ASSERT_TRUE(r == ENOENT || r == ERANGE);
	ASSERT_NULL(result);
}

TEST(getpwuid_r_null_buffer_with_size) {
	struct passwd pwd;
	struct passwd *result;
	
	int r = getpwuid_r(0, &pwd, NULL, 1024, &result);
	ASSERT_EQ(EINVAL, r);
	ASSERT_NULL(result);
}

/* ============================================================================ */
TEST_SUITE(iteration);

TEST(iteration_basic) {
	__create_test_passwd("root:x:0:0:Root:/root:/bin/sh\nnobody:x:65534:65534:Nobody:/:/sbin/nologin\n");
	
	setpwent();
	struct passwd *p1 = getpwent();
	ASSERT_NOT_NULL(p1);
	ASSERT_STR_EQ(p1->pw_name, "root");
	
	struct passwd *p2 = getpwent();
	ASSERT_NOT_NULL(p2);
	ASSERT_STR_EQ(p2->pw_name, "nobody");
	
	struct passwd *p3 = getpwent();
	ASSERT_NULL(p3);
	
	endpwent();
	unlink(JACL_PASSWD_PATH);
}

TEST(iteration_reset) {
	__create_test_passwd("root:x:0:0:Root:/root:/bin/sh\n");
	
	setpwent();
	struct passwd *p1 = getpwent();
	endpwent();
	
	setpwent();
	struct passwd *p2 = getpwent();
	
	ASSERT_NOT_NULL(p1);
	ASSERT_NOT_NULL(p2);
	ASSERT_STR_EQ(p1->pw_name, p2->pw_name);
	
	endpwent();
	unlink(JACL_PASSWD_PATH);
}

TEST(endpwent_safe) {
	endpwent();
	endpwent();
}

/* ============================================================================ */
TEST_SUITE(edge_cases);

TEST(parser_whitespace_handling) {
	__create_test_passwd("root:x:0:0:Root User:/root:/bin/sh\n");
	
	setpwent();
	struct passwd *p = getpwent();
	ASSERT_NOT_NULL(p);
	ASSERT_STR_EQ(p->pw_name, "root");
	ASSERT_STR_EQ(p->pw_gecos, "Root User");
	endpwent();
	
	unlink(JACL_PASSWD_PATH);
}

TEST(parser_duplicate_uid) {
	__create_test_passwd("root:x:0:0:Root:/root:/bin/sh\nadmin:x:0:0:Admin:/home/admin:/bin/bash\n");
	
	struct passwd *p = getpwuid(0);
	ASSERT_NOT_NULL(p);
	ASSERT_STR_EQ(p->pw_name, "root");
	
	unlink(JACL_PASSWD_PATH);
}

TEST(parser_malformed_gid) {
	__create_test_passwd("root:x:0:abc:Root:/root:/bin/sh\n");
	
	errno = 0;
	struct passwd *p = getpwuid(0);
	ASSERT_NULL(p);
	
	unlink(JACL_PASSWD_PATH);
}

TEST(parser_missing_shell) {
	__create_test_passwd("root:x:0:0:Root:/root\n");
	
	errno = 0;
	struct passwd *p = getpwuid(0);
	ASSERT_NULL(p);
	
	unlink(JACL_PASSWD_PATH);
}

TEST(iteration_with_comments_and_blanks) {
	__create_test_passwd("# Comment\n\nroot:x:0:0:Root:/root:/bin/sh\n\n# Another comment\n");
	
	setpwent();
	struct passwd *p = getpwent();
	ASSERT_NOT_NULL(p);
	ASSERT_STR_EQ(p->pw_name, "root");
	
	ASSERT_NULL(getpwent());
	
	endpwent();
	unlink(JACL_PASSWD_PATH);
}

/* ============================================================================ */
TEST_MAIN()
