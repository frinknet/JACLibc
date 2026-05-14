/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/resource.h>
#include <errno.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(sys/resource.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(rlimit_constants) {
	ASSERT_EQ(0, RLIMIT_CPU);
	ASSERT_EQ(7, RLIMIT_NOFILE);
	ASSERT_TRUE(RLIM_INFINITY > 0);
}

TEST(rusage_constants) {
	ASSERT_EQ(0, RUSAGE_SELF);
	ASSERT_EQ(-1, RUSAGE_CHILDREN);
}

/* ============================================================================ */
TEST_SUITE(getrlimit);

TEST(getrlimit_success_nofile) {
	struct rlimit rl;
	int r = getrlimit(RLIMIT_NOFILE, &rl);
	
	ASSERT_EQ(0, r);
	ASSERT_TRUE(rl.rlim_cur <= rl.rlim_max);
}

TEST(getrlimit_success_cpu) {
	struct rlimit rl;
	int r = getrlimit(RLIMIT_CPU, &rl);
	
	ASSERT_EQ(0, r);
}

TEST(getrlimit_null_ptr) {
	int r = getrlimit(RLIMIT_NOFILE, NULL);
	ASSERT_EQ(-1, r);
	ASSERT_EQ(EFAULT, errno);
}

/* ============================================================================ */
TEST_SUITE(setrlimit);

TEST(setrlimit_success_nofile) {
	struct rlimit rl;
	getrlimit(RLIMIT_NOFILE, &rl);
	
	int r = setrlimit(RLIMIT_NOFILE, &rl);
	
	/* May fail with EPERM if not root, or succeed */
	ASSERT_TRUE(r == 0 || errno == EPERM);
}

TEST(setrlimit_null_ptr) {
	int r = setrlimit(RLIMIT_NOFILE, NULL);
	ASSERT_EQ(-1, r);
	ASSERT_EQ(EFAULT, errno);
}

/* ============================================================================ */
TEST_SUITE(getrusage);

TEST(getrusage_success_self) {
	struct rusage ru;
	int r = getrusage(RUSAGE_SELF, &ru);
	
	ASSERT_EQ(0, r);
	ASSERT_TRUE(ru.ru_utime.tv_sec >= 0);
}

TEST(getrusage_success_children) {
	struct rusage ru;
	int r = getrusage(RUSAGE_CHILDREN, &ru);
	
	ASSERT_EQ(0, r);
}

TEST(getrusage_null_ptr) {
	int r = getrusage(RUSAGE_SELF, NULL);
	ASSERT_EQ(-1, r);
	ASSERT_EQ(EFAULT, errno);
}

/* ============================================================================ */
TEST_MAIN()
