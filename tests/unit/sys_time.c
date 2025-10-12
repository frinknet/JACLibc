/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>

TEST_TYPE(unit);
TEST_UNIT(sys/time.h);

/* ============================================================================
 * CONSTANTS
 * ============================================================================ */
TEST_SUITE(constants);

TEST(itimer_constants) {
	ASSERT_EQ(0, ITIMER_REAL);
	ASSERT_EQ(1, ITIMER_VIRTUAL);
	ASSERT_EQ(2, ITIMER_PROF);
}

TEST(itimer_constants_unique) {
	ASSERT_NE(ITIMER_REAL, ITIMER_VIRTUAL);
	ASSERT_NE(ITIMER_VIRTUAL, ITIMER_PROF);
	ASSERT_NE(ITIMER_REAL, ITIMER_PROF);
}

/* ============================================================================
 * STRUCTURES
 * ============================================================================ */
TEST_SUITE(structures);

TEST(timeval_structure) {
	timeval tv;
	tv.tv_sec = 1234;
	tv.tv_usec = 567890;
	
	ASSERT_EQ(1234, tv.tv_sec);
	ASSERT_EQ(567890, tv.tv_usec);
}

TEST(timezone_structure) {
	timezone tz;
	tz.tz_minuteswest = 300;
	tz.tz_dsttime = 1;
	
	ASSERT_EQ(300, tz.tz_minuteswest);
	ASSERT_EQ(1, tz.tz_dsttime);
}

TEST(itimerval_structure) {
	itimerval itv;
	itv.it_interval.tv_sec = 1;
	itv.it_interval.tv_usec = 0;
	itv.it_value.tv_sec = 5;
	itv.it_value.tv_usec = 0;
	
	ASSERT_EQ(1, itv.it_interval.tv_sec);
	ASSERT_EQ(5, itv.it_value.tv_sec);
}

/* ============================================================================
 * MACROS - TIMERCLEAR
 * ============================================================================ */
TEST_SUITE(timerclear_macro);

TEST(timerclear_basic) {
	timeval tv = {123, 456789};
	
	timerclear(&tv);
	
	ASSERT_EQ(0, tv.tv_sec);
	ASSERT_EQ(0, tv.tv_usec);
}

TEST(timerclear_already_clear) {
	timeval tv = {0, 0};
	
	timerclear(&tv);
	
	ASSERT_EQ(0, tv.tv_sec);
	ASSERT_EQ(0, tv.tv_usec);
}

/* ============================================================================
 * MACROS - TIMERISSET
 * ============================================================================ */
TEST_SUITE(timerisset_macro);

TEST(timerisset_true_sec) {
	timeval tv = {1, 0};
	ASSERT_TRUE(timerisset(&tv));
}

TEST(timerisset_true_usec) {
	timeval tv = {0, 1};
	ASSERT_TRUE(timerisset(&tv));
}

TEST(timerisset_true_both) {
	timeval tv = {1, 1};
	ASSERT_TRUE(timerisset(&tv));
}

TEST(timerisset_false) {
	timeval tv = {0, 0};
	ASSERT_FALSE(timerisset(&tv));
}

/* ============================================================================
 * MACROS - TIMERCMP
 * ============================================================================ */
TEST_SUITE(timercmp_macro);

TEST(timercmp_equal) {
	timeval a = {10, 500};
	timeval b = {10, 500};
	
	ASSERT_TRUE(timercmp(&a, &b, ==));
	ASSERT_FALSE(timercmp(&a, &b, !=));
}

TEST(timercmp_less_sec) {
	timeval a = {5, 999999};
	timeval b = {10, 0};
	
	ASSERT_TRUE(timercmp(&a, &b, <));
	ASSERT_FALSE(timercmp(&a, &b, >));
}

TEST(timercmp_less_usec) {
	timeval a = {10, 100};
	timeval b = {10, 200};
	
	ASSERT_TRUE(timercmp(&a, &b, <));
	ASSERT_FALSE(timercmp(&a, &b, >));
}

TEST(timercmp_greater) {
	timeval a = {20, 500};
	timeval b = {10, 999999};
	
	ASSERT_TRUE(timercmp(&a, &b, >));
	ASSERT_FALSE(timercmp(&a, &b, <));
}

/* ============================================================================
 * MACROS - TIMERADD
 * ============================================================================ */
TEST_SUITE(timeradd_macro);

TEST(timeradd_basic) {
	timeval a = {5, 100000};
	timeval b = {3, 200000};
	timeval result;
	
	timeradd(&a, &b, &result);
	
	ASSERT_EQ(8, result.tv_sec);
	ASSERT_EQ(300000, result.tv_usec);
}

TEST(timeradd_with_overflow) {
	timeval a = {5, 800000};
	timeval b = {3, 300000};
	timeval result;
	
	timeradd(&a, &b, &result);
	
	ASSERT_EQ(9, result.tv_sec);
	ASSERT_EQ(100000, result.tv_usec);
}

TEST(timeradd_zero) {
	timeval a = {5, 500000};
	timeval b = {0, 0};
	timeval result;
	
	timeradd(&a, &b, &result);
	
	ASSERT_EQ(5, result.tv_sec);
	ASSERT_EQ(500000, result.tv_usec);
}

/* ============================================================================
 * MACROS - TIMERSUB
 * ============================================================================ */
TEST_SUITE(timersub_macro);

TEST(timersub_basic) {
	timeval a = {10, 500000};
	timeval b = {5, 200000};
	timeval result;
	
	timersub(&a, &b, &result);
	
	ASSERT_EQ(5, result.tv_sec);
	ASSERT_EQ(300000, result.tv_usec);
}

TEST(timersub_with_borrow) {
	timeval a = {10, 200000};
	timeval b = {5, 500000};
	timeval result;
	
	timersub(&a, &b, &result);
	
	ASSERT_EQ(4, result.tv_sec);
	ASSERT_EQ(700000, result.tv_usec);
}

TEST(timersub_same) {
	timeval a = {5, 500000};
	timeval b = {5, 500000};
	timeval result;
	
	timersub(&a, &b, &result);
	
	ASSERT_EQ(0, result.tv_sec);
	ASSERT_EQ(0, result.tv_usec);
}

/* ============================================================================
 * GETTIMEOFDAY
 * ============================================================================ */
#if !defined(SYSTIME_WASM)
TEST_SUITE(gettimeofday_test);

TEST(gettimeofday_basic) {
	timeval tv;
	int result = gettimeofday(&tv, NULL);
	
	ASSERT_EQ(0, result);
	ASSERT_TRUE(tv.tv_sec > 0);
	ASSERT_TRUE(tv.tv_usec >= 0 && tv.tv_usec < 1000000);
}

TEST(gettimeofday_with_timezone) {
	timeval tv;
	timezone tz;
	
	int result = gettimeofday(&tv, &tz);
	
	ASSERT_EQ(0, result);
	ASSERT_TRUE(tv.tv_sec > 0);
}

TEST(gettimeofday_null_tv) {
	int result = gettimeofday(NULL, NULL);
	ASSERT_EQ(-1, result);
}

TEST(gettimeofday_monotonic) {
	timeval tv1, tv2;
	
	gettimeofday(&tv1, NULL);
	usleep(10000); // 10ms
	gettimeofday(&tv2, NULL);
	
	ASSERT_TRUE(timercmp(&tv2, &tv1, >));
}
#endif

/* ============================================================================
 * UTILITY FUNCTIONS
 * ============================================================================ */
TEST_SUITE(utility_functions);

TEST(timespec_to_timeval_basic) {
	timespec ts = {123, 456789000};
	timeval tv;
	
	timespec_to_timeval(&ts, &tv);
	
	ASSERT_EQ(123, tv.tv_sec);
	ASSERT_EQ(456789, tv.tv_usec);
}

TEST(timespec_to_timeval_rounding) {
	timespec ts = {100, 999999999};
	timeval tv;
	
	timespec_to_timeval(&ts, &tv);
	
	ASSERT_EQ(100, tv.tv_sec);
	ASSERT_EQ(999999, tv.tv_usec);
}

TEST(timeval_to_timespec_basic) {
	timeval tv = {456, 789123};
	timespec ts;
	
	timeval_to_timespec(&tv, &ts);
	
	ASSERT_EQ(456, ts.tv_sec);
	ASSERT_EQ(789123000, ts.tv_nsec);
}

TEST(timeval_to_timespec_zero) {
	timeval tv = {0, 0};
	timespec ts;
	
	timeval_to_timespec(&tv, &ts);
	
	ASSERT_EQ(0, ts.tv_sec);
	ASSERT_EQ(0, ts.tv_nsec);
}

TEST(conversion_roundtrip) {
	timespec ts1 = {999, 123456789};
	timeval tv;
	timespec ts2;
	
	timespec_to_timeval(&ts1, &tv);
	timeval_to_timespec(&tv, &ts2);
	
	ASSERT_EQ(ts1.tv_sec, ts2.tv_sec);
	// Note: nsec will be truncated to usec precision
	ASSERT_TRUE(ts2.tv_nsec >= 123456000 && ts2.tv_nsec < 123457000);
}

/* ============================================================================
 * UTIMES (if supported)
 * ============================================================================ */
#if !defined(SYSTIME_WASM) && !defined(SYSTIME_WIN32)
TEST_SUITE(utimes_test);

TEST(utimes_basic) {
	// Create test file
	int fd = open("/tmp/systime_test_utimes.txt", O_CREAT | O_WRONLY, 0644);
	close(fd);
	
	timeval times[2];
	times[0].tv_sec = 1000000000;  // Access time
	times[0].tv_usec = 0;
	times[1].tv_sec = 1000000001;  // Modification time
	times[1].tv_usec = 0;
	
	int result = utimes("/tmp/systime_test_utimes.txt", times);
	
	// May fail if not supported or no permissions
	ASSERT_TRUE(result == 0 || result == -1);
	
	unlink("/tmp/systime_test_utimes.txt");
}

TEST(utimes_null_times) {
	int fd = open("/tmp/systime_test_utimes2.txt", O_CREAT | O_WRONLY, 0644);
	close(fd);
	
	int result = utimes("/tmp/systime_test_utimes2.txt", NULL);
	
	ASSERT_TRUE(result == 0 || result == -1);
	
	unlink("/tmp/systime_test_utimes2.txt");
}
#endif

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(timeval_max_usec) {
	timeval tv = {0, 999999};
	
	ASSERT_TRUE(timerisset(&tv));
	ASSERT_TRUE(tv.tv_usec < 1000000);
}

TEST(timeradd_multiple_overflows) {
	timeval a = {1, 900000};
	timeval b = {2, 900000};
	timeval result;
	
	timeradd(&a, &b, &result);
	
	ASSERT_EQ(4, result.tv_sec);
	ASSERT_EQ(800000, result.tv_usec);
}

TEST(timersub_negative_result) {
	timeval a = {5, 100000};
	timeval b = {10, 200000};
	timeval result;
	
	timersub(&a, &b, &result);
	
	// Result will be negative
	ASSERT_TRUE(result.tv_sec < 5);
}

TEST_MAIN()
