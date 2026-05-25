/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/types.h>
#include <limits.h>

TEST_TYPE(unit);
TEST_UNIT(sys/types.h);

/* ============================================================================
 * CONSTANTS
 * ============================================================================ */
TEST_SUITE(constants);

TEST(constants_max) {
	ASSERT_TRUE(TIME_MAX > 0);
	ASSERT_TRUE(CLOCK_MAX > 0);
	ASSERT_TRUE(OFF_MAX > 0);
	ASSERT_TRUE(PID_MAX > 0);
	ASSERT_TRUE(UID_MAX > 0);
	ASSERT_TRUE(GID_MAX > 0);
	ASSERT_TRUE(MODE_MAX > 0);
	ASSERT_TRUE(INO_MAX > 0);
	ASSERT_TRUE(DEV_MAX > 0);
	ASSERT_TRUE(NLINK_MAX > 0);
	ASSERT_TRUE(BLKCNT_MAX > 0);
	ASSERT_TRUE(BLKSIZE_MAX > 0);
	ASSERT_TRUE(FSBLKCNT_MAX > 0);
	ASSERT_TRUE(FSFILCNT_MAX > 0);
	ASSERT_TRUE(CLOCKID_MAX > 0);
	ASSERT_TRUE(USECONDS_MAX > 0);
	ASSERT_TRUE(SUSECONDS_MAX > 0);
	ASSERT_TRUE(SOCKLEN_MAX > 0);
	ASSERT_TRUE(SA_FAMILY_MAX > 0);
	ASSERT_TRUE(KEY_MAX > 0);
	ASSERT_TRUE(SSIZE_MAX > 0);
#if JACL_HAS_LFS
	ASSERT_TRUE(OFF64_MAX > 0);
	ASSERT_TRUE(BLKCNT64_MAX > 0);
#endif
}

TEST(constants_min) {
	ASSERT_TRUE(TIME_MIN < 0);
	ASSERT_TRUE(CLOCK_MIN < 0);
	ASSERT_TRUE(OFF_MIN < 0);
	ASSERT_TRUE(PID_MIN < 0);
	ASSERT_TRUE(BLKCNT_MIN < 0);
	ASSERT_TRUE(SUSECONDS_MIN < 0);
	ASSERT_TRUE(KEY_MIN < 0);
	ASSERT_TRUE(SSIZE_MIN < 0);
#if JACL_HAS_LFS
	ASSERT_TRUE(OFF64_MIN < 0);
	ASSERT_TRUE(BLKCNT64_MIN < 0);
#endif
}

/* ============================================================================
 * BASIC TYPE MACRO
 * ============================================================================ */
#define TEST_BASIC_TYPE(type_name, signed_type, unsigned_type) \
	TEST_SUITE(type_name); \
	TEST(type_name##_size) { ASSERT_TRUE(sizeof(type_name) > 0); } \
	TEST(type_name##_sign) { \
		type_name neg = (type_name)-1; \
		if (signed_type) { ASSERT_TRUE(neg < 0); } \
		else { ASSERT_TRUE(neg > 0); } \
	} \
	TEST(type_name##_min) { \
		if (signed_type) { \
			type_name min = (type_name)((unsigned_type)1 << (sizeof(type_name) * 8 - 1)); \
			ASSERT_TRUE(min < 0); \
		} else { \
			type_name min = 0; \
			ASSERT_EQ(0, min); \
		} \
	} \
	TEST(type_name##_max) { \
		type_name max = (type_name)((unsigned_type)-1); \
		if (signed_type) { max = (type_name)(((unsigned_type)-1) >> 1); } \
		ASSERT_TRUE(max > 0); \
	}

/* ============================================================================
 * BASIC TYPES
 * ============================================================================ */

TEST_BASIC_TYPE(pid_t, 1, unsigned int)
TEST_BASIC_TYPE(uid_t, 0, unsigned int)
TEST_BASIC_TYPE(gid_t, 0, unsigned int)
TEST_BASIC_TYPE(id_t, 0, unsigned int)
TEST_BASIC_TYPE(mode_t, 0, unsigned int)
TEST_BASIC_TYPE(ino_t, 0, unsigned long)
TEST_BASIC_TYPE(dev_t, 0, unsigned int)
TEST_BASIC_TYPE(nlink_t, 0, unsigned int)
TEST_BASIC_TYPE(blkcnt_t, 1, unsigned long)
TEST_BASIC_TYPE(blksize_t, 1, unsigned long)
TEST_BASIC_TYPE(fsblkcnt_t, 0, unsigned long)
TEST_BASIC_TYPE(fsfilcnt_t, 0, unsigned long)
TEST_BASIC_TYPE(reclen_t, 0, unsigned short)
TEST_BASIC_TYPE(clockid_t, 1, unsigned int)
TEST_BASIC_TYPE(useconds_t, 0, unsigned int)
TEST_BASIC_TYPE(suseconds_t, 1, unsigned int)
TEST_BASIC_TYPE(socklen_t, 0, unsigned int)
TEST_BASIC_TYPE(sa_family_t, 0, unsigned short)
TEST_BASIC_TYPE(key_t, 1, unsigned int)
TEST_BASIC_TYPE(timer_t, 1, unsigned int)

#if JACL_HAS_C99
TEST_SUITE(time_t);
TEST(time_t_size) { ASSERT_TRUE(sizeof(time_t) >= 8); } /* Issue 8: width >= 64 */
TEST(time_t_sign) { time_t neg = -1; ASSERT_TRUE(neg < 0); }
TEST(time_t_min) { ASSERT_TRUE(TIME_MIN < 0); }
TEST(time_t_max) { ASSERT_TRUE(TIME_MAX > 0); }

TEST_SUITE(clock_t);
TEST(clock_t_size) { ASSERT_TRUE(sizeof(clock_t) >= 8); }
TEST(clock_t_sign) { clock_t neg = -1; ASSERT_TRUE(neg < 0); }
TEST(clock_t_min) { ASSERT_TRUE(CLOCK_MIN < 0); }
TEST(clock_t_max) { ASSERT_TRUE(CLOCK_MAX > 0); }
#endif

TEST_SUITE(off_t);
TEST(off_t_size) { ASSERT_TRUE(sizeof(off_t) >= 4); }
TEST(off_t_sign) { off_t neg = -1; ASSERT_TRUE(neg < 0); }
TEST(off_t_min) { ASSERT_TRUE(OFF_MIN < 0); }
TEST(off_t_max) { ASSERT_TRUE(OFF_MAX > 0); }

TEST_SUITE(ssize_t);
TEST(ssize_t_size) { ASSERT_EQ(sizeof(size_t), sizeof(ssize_t)); }
TEST(ssize_t_sign) { ssize_t neg = -1; ASSERT_TRUE(neg < 0); }
TEST(ssize_t_min) { ASSERT_TRUE(SSIZE_MIN < 0); }
TEST(ssize_t_max) { ASSERT_TRUE(SSIZE_MAX > 0); }

#if JACL_HAS_LFS
TEST_BASIC_TYPE(off64_t, 1, unsigned long long)
TEST_BASIC_TYPE(blkcnt64_t, 1, unsigned long long)
#endif

/* ============================================================================
 * PTHREAD STRUCTURE TYPES
 * ============================================================================ */

TEST_SUITE(pthread_mutexattr_t);
TEST(pthread_mutexattr_t_has_type) { pthread_mutexattr_t a; a.type = 42; ASSERT_EQ(42, a.type); }
TEST(pthread_mutexattr_t_has_robust) { pthread_mutexattr_t a; a.robust = 1; ASSERT_EQ(1, a.robust); }
TEST(pthread_mutexattr_t_has_pshared) { pthread_mutexattr_t a; a.pshared = 42; ASSERT_EQ(42, a.pshared); }

TEST_SUITE(pthread_condattr_t);
TEST(pthread_condattr_t_has_pshared) { pthread_condattr_t a; a.pshared = 42; ASSERT_EQ(42, a.pshared); }
TEST(pthread_condattr_t_has_clock_id) { pthread_condattr_t a; a.clock_id = 1; ASSERT_EQ(1, a.clock_id); }

TEST_SUITE(pthread_once_t);
TEST(pthread_once_t_has_done) { pthread_once_t o; o.done = 1; ASSERT_EQ(1, o.done); }

/* ============================================================================
 * COMPARISONS
 * ============================================================================ */
TEST_SUITE(comparisons);

TEST(pid_t_comparison) {
	pid_t p1 = 100;
	pid_t p2 = 200;
	ASSERT_TRUE(p1 < p2);
	ASSERT_TRUE(p2 > p1);
	ASSERT_FALSE(p1 == p2);
}

TEST(off_t_comparison) {
	off_t o1 = 1000;
	off_t o2 = 2000;
	ASSERT_TRUE(o1 < o2);
}

TEST(uid_gid_comparison) {
	uid_t u1 = 100;
	uid_t u2 = 100;
	ASSERT_TRUE(u1 == u2);
}

TEST_MAIN()
