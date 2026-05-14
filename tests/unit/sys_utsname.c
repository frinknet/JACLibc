/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/utsname.h>
#include <string.h>
#include <errno.h>

TEST_TYPE(unit);
TEST_UNIT(sys/utsname.h);

/* ============================================================================ */
TEST_SUITE(struct_utsname);

TEST(struct_utsname_size) {
	/* Ensure the struct is not empty */
	ASSERT_SIZE_MIN(struct utsname, 1);
}

TEST(struct_utsname_fields_exist) {
	struct utsname u;
	/* Verify fields exist by accessing them */
	ASSERT_NOT_NULL(u.sysname);
	ASSERT_NOT_NULL(u.nodename);
	ASSERT_NOT_NULL(u.release);
	ASSERT_NOT_NULL(u.version);
	ASSERT_NOT_NULL(u.machine);
#ifdef _POSIX_VERSION
	/* Domain name is optional in some POSIX versions but common */
	ASSERT_NOT_NULL(u.domainname);
#endif
}

/* ============================================================================ */
TEST_SUITE(uname_function);

TEST(uname_success) {
	struct utsname u;
	int r = uname(&u);
	ASSERT_EQ(0, r);
}

TEST(uname_non_null_sysname) {
	struct utsname u;
	uname(&u);
	ASSERT_TRUE(strlen(u.sysname) > 0);
}

TEST(uname_non_null_release) {
	struct utsname u;
	uname(&u);
	ASSERT_TRUE(strlen(u.release) > 0);
}

TEST(uname_non_null_machine) {
	struct utsname u;
	uname(&u);
	ASSERT_TRUE(strlen(u.machine) > 0);
}

TEST(uname_null_pointer) {
	int r = uname(NULL);
	ASSERT_EQ(-1, r);
	ASSERT_EQ(EFAULT, errno);
}

/* ============================================================================ */
TEST_MAIN()
