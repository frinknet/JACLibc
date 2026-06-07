/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(sys/resource.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(rlimit_constants) {
	ASSERT_EQ(0, RLIMIT_CPU);
	ASSERT_EQ(1, RLIMIT_FSIZE);
	ASSERT_EQ(2, RLIMIT_DATA);
	ASSERT_EQ(3, RLIMIT_STACK);
	ASSERT_EQ(4, RLIMIT_CORE);
	ASSERT_EQ(7, RLIMIT_NOFILE);
	ASSERT_EQ(9, RLIMIT_AS);
	ASSERT_TRUE(RLIM_INFINITY > 0);
	ASSERT_EQ(RLIM_SAVED_CUR, RLIM_INFINITY);
	ASSERT_EQ(RLIM_SAVED_MAX, RLIM_INFINITY);
}

TEST(rusage_constants) {
	ASSERT_EQ(0, RUSAGE_SELF);
	ASSERT_EQ(-1, RUSAGE_CHILDREN);
}

TEST(priority_constants) {
	ASSERT_EQ(0, PRIO_PROCESS);
	ASSERT_EQ(1, PRIO_PGRP);
	ASSERT_EQ(2, PRIO_USER);
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

TEST(getrlimit_success_stack) {
	struct rlimit rl;
	int r = getrlimit(RLIMIT_STACK, &rl);
	ASSERT_EQ(0, r);
	ASSERT_TRUE(rl.rlim_cur > 0);
}

TEST(getrlimit_null_ptr) {
	errno = 0;
	int r = getrlimit(RLIMIT_NOFILE, NULL);
	ASSERT_EQ(-1, r);
	ASSERT_ERRNO(EFAULT);
}

TEST(getrlimit_invalid_resource) {
	struct rlimit rl;
	errno = 0;
	int r = getrlimit(9999, &rl);
	ASSERT_EQ(-1, r);
	ASSERT_ERRNO(EINVAL);
}

TEST(getrlimit_negative_resource) {
	struct rlimit rl;
	errno = 0;
	int r = getrlimit(-1, &rl);
	ASSERT_EQ(-1, r);
	ASSERT_ERRNO(EINVAL);
}

/* ============================================================================ */
TEST_SUITE(setrlimit);

TEST(setrlimit_success_nofile) {
	struct rlimit rl;
	getrlimit(RLIMIT_NOFILE, &rl);
	errno = 0;
	int r = setrlimit(RLIMIT_NOFILE, &rl);
	ASSERT_TRUE(r == 0 || errno == EPERM);
}

TEST(setrlimit_null_ptr) {
	errno = 0;
	int r = setrlimit(RLIMIT_NOFILE, NULL);
	ASSERT_EQ(-1, r);
	ASSERT_ERRNO(EFAULT);
}

TEST(setrlimit_invalid_resource) {
	struct rlimit rl = {100, 100};
	errno = 0;
	int r = setrlimit(9999, &rl);
	ASSERT_EQ(-1, r);
	ASSERT_ERRNO(EINVAL);
}

TEST(setrlimit_cur_greater_than_max) {
	struct rlimit rl;
	getrlimit(RLIMIT_NOFILE, &rl);
	rl.rlim_cur = rl.rlim_max + 1;
	errno = 0;
	int r = setrlimit(RLIMIT_NOFILE, &rl);
	ASSERT_EQ(-1, r);
	ASSERT_ERRNO(EINVAL);
}

TEST(setrlimit_lower_cur) {
	struct rlimit rl, orig;
	getrlimit(RLIMIT_NOFILE, &orig);
	rl = orig;
	if (rl.rlim_cur > 10) {
		rl.rlim_cur = 10;
		int r = setrlimit(RLIMIT_NOFILE, &rl);
		ASSERT_EQ(0, r);
		setrlimit(RLIMIT_NOFILE, &orig); /* restore */
	} else {
		TEST_SKIP("current limit too low to test lowering");
	}
}

/* ============================================================================ */
TEST_SUITE(getrusage);

TEST(getrusage_success_self) {
	struct rusage ru;
	int r = getrusage(RUSAGE_SELF, &ru);
	ASSERT_EQ(0, r);
	ASSERT_TRUE(ru.ru_utime.tv_sec >= 0);
	ASSERT_TRUE(ru.ru_utime.tv_usec >= 0);
}

TEST(getrusage_success_children) {
	struct rusage ru;
	int r = getrusage(RUSAGE_CHILDREN, &ru);
	ASSERT_EQ(0, r);
}

TEST(getrusage_null_ptr) {
	errno = 0;
	int r = getrusage(RUSAGE_SELF, NULL);
	ASSERT_EQ(-1, r);
	ASSERT_ERRNO(EFAULT);
}

TEST(getrusage_invalid_who) {
	struct rusage ru;
	errno = 0;
	int r = getrusage(9999, &ru);
	ASSERT_EQ(-1, r);
	ASSERT_ERRNO(EINVAL);
}

/* ============================================================================ */
TEST_SUITE(getpriority);

TEST(getpriority_process) {
	errno = 0;
	int prio = getpriority(PRIO_PROCESS, 0);
	if (prio == -1) {
		ASSERT_EQ(errno, 0);
	} else {
		ASSERT_TRUE(prio >= -20 && prio <= 20);
	}
}

TEST(getpriority_pgrp) {
	errno = 0;
	int prio = getpriority(PRIO_PGRP, 0);
	if (prio == -1) {
		ASSERT_EQ(errno, 0);
	} else {
		ASSERT_TRUE(prio >= -20 && prio <= 20);
	}
}

TEST(getpriority_user) {
	errno = 0;
	int prio = getpriority(PRIO_USER, 0);
	if (prio == -1) {
		ASSERT_EQ(errno, 0);
	} else {
		ASSERT_TRUE(prio >= -20 && prio <= 20);
	}
}

TEST(getpriority_invalid_which) {
	errno = 0;
	int r = getpriority(999, 0);
	ASSERT_EQ(-1, r);
	ASSERT_ERRNO(EINVAL);
}

TEST(getpriority_invalid_who) {
	errno = 0;
	int r = getpriority(PRIO_PROCESS, 999999999);
	ASSERT_EQ(-1, r);
	ASSERT_ERRNO(ESRCH);
}

/* ============================================================================ */
TEST_SUITE(setpriority);

TEST(setpriority_no_change) {
	errno = 0;
	int old = getpriority(PRIO_PROCESS, 0);
	if (old == -1 && errno != 0) { TEST_SKIP("getpriority failed"); return; }
	
	errno = 0;
	int r = setpriority(PRIO_PROCESS, 0, old);
	ASSERT_EQ(0, r);
}

TEST(setpriority_increase_nice) {
	errno = 0;
	int old = getpriority(PRIO_PROCESS, 0);
	if (old == -1 && errno != 0) { TEST_SKIP("getpriority failed"); return; }
	
	if (old < 19) {
		errno = 0;
		int r = setpriority(PRIO_PROCESS, 0, old + 1);
		if (r == -1) {
			ASSERT_TRUE(errno == EPERM || errno == EACCES);
		} else {
			ASSERT_EQ(0, r);
		}
	} else {
		TEST_SKIP("already at max nice");
	}
}

TEST(setpriority_invalid_which) {
	errno = 0;
	int r = setpriority(999, 0, 0);
	ASSERT_EQ(-1, r);
	ASSERT_ERRNO(EINVAL);
}

TEST(setpriority_invalid_who) {
	errno = 0;
	int r = setpriority(PRIO_PROCESS, 999999999, 0);
	ASSERT_EQ(-1, r);
	ASSERT_ERRNO(ESRCH);
}

TEST(setpriority_no_perm) {
	if (getuid() == 0) {
		TEST_SKIP("running as root");
		return;
	}
	errno = 0;
	int r = setpriority(PRIO_USER, 0, -1); 
	ASSERT_EQ(-1, r);
	ASSERT_TRUE(errno == EPERM || errno == EACCES);
}

/* ============================================================================ */
TEST_MAIN()
