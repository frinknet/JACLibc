/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/times.h>
#include <unistd.h> /* for sysconf */

TEST_TYPE(unit);
TEST_UNIT(sys/times.h);

/* ============================================================================ */
TEST_SUITE(struct_tms);

TEST(struct_tms_fields) {
	struct tms t;
	t.tms_utime = 1;
	t.tms_stime = 2;
	t.tms_cutime = 3;
	t.tms_cstime = 4;
	
	ASSERT_EQ(1, t.tms_utime);
	ASSERT_EQ(2, t.tms_stime);
	ASSERT_EQ(3, t.tms_cutime);
	ASSERT_EQ(4, t.tms_cstime);
}

/* ============================================================================ */
TEST_SUITE(times_function);

TEST(times_success) {
	struct tms t;
	clock_t r = times(&t);
	
	/* Should not fail */
	ASSERT_TRUE(r != (clock_t)-1);
	
	/* Initial values should be reasonable (>= 0) */
	ASSERT_TRUE(t.tms_utime >= 0);
	ASSERT_TRUE(t.tms_stime >= 0);
}

TEST(times_null_buffer) {
	clock_t r = times(NULL);
	
	/* Should fail with -1 */
	ASSERT_EQ((clock_t)-1, r);
}

TEST(times_monotonic) {
	struct tms t1, t2;
	
	times(&t1);
	
	/* Busy wait to consume some CPU time */
	volatile int i;
	for (i = 0; i < 1000000; i++) {}
	
	times(&t2);
	
	/* User time should have increased or stayed same (if resolution is low) */
	ASSERT_TRUE(t2.tms_utime >= t1.tms_utime);
}

/* ============================================================================ */
TEST_MAIN()
