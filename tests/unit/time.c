/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <time.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(time.h);

/* ============================================================================
 * BASIC TIME TESTS
 * ============================================================================ */
TEST_SUITE(basic_time);

TEST(time_current) {
	time_t t1 = time(NULL);
	ASSERT_TRUE(t1 > 0);
	
	time_t t2;
	time_t t3 = time(&t2);
	ASSERT_EQ(t2, t3);
}

TEST(time_monotonic) {
	time_t t1 = time(NULL);
	time_t t2 = time(NULL);
	
	// Should be same or t2 >= t1
	ASSERT_TRUE(t2 >= t1);
}

TEST(difftime_calculation) {
	time_t t1 = 1000;
	time_t t2 = 1500;
	
	double diff = difftime(t2, t1);
	ASSERT_FLOAT_EQ(500.0, diff, 0.001);
	
	diff = difftime(t1, t2);
	ASSERT_FLOAT_EQ(-500.0, diff, 0.001);
}

TEST(clock_non_negative) {
	clock_t c = clock();
	ASSERT_TRUE(c >= 0);
}

TEST(clock_per_sec) {
	ASSERT_EQ(1000000L, CLOCKS_PER_SEC);
}

/* ============================================================================
 * STRUCT TM TESTS
 * ============================================================================ */
TEST_SUITE(struct_tm);

TEST(gmtime_epoch) {
	time_t epoch = 0;
	tm *t = gmtime(&epoch);
	
	ASSERT_NOT_NULL(t);
	ASSERT_EQ(0, t->tm_sec);
	ASSERT_EQ(0, t->tm_min);
	ASSERT_EQ(0, t->tm_hour);
	ASSERT_EQ(1, t->tm_mday);
	ASSERT_EQ(0, t->tm_mon);
	ASSERT_EQ(70, t->tm_year);
	ASSERT_EQ(4, t->tm_wday);  // Thursday
}

TEST(gmtime_known_date) {
	time_t t = 1640995200;  // 2022-01-01 00:00:00
	tm *result = gmtime(&t);
	
	ASSERT_NOT_NULL(result);
	ASSERT_EQ(0, result->tm_sec);
	ASSERT_EQ(0, result->tm_min);
	ASSERT_EQ(0, result->tm_hour);
	ASSERT_EQ(1, result->tm_mday);
	ASSERT_EQ(0, result->tm_mon);
	ASSERT_EQ(122, result->tm_year);
}

TEST(gmtime_null_pointer) {
	tm *result = gmtime(NULL);
	
#ifndef JACL_ARCH_WASM
	ASSERT_NULL(result);
#endif
}

TEST(localtime_basic) {
	time_t t = 1640995200;
	tm *result = localtime(&t);
	
	ASSERT_NOT_NULL(result);
	// Should be same as gmtime (no timezone support)
	ASSERT_EQ(0, result->tm_sec);
	ASSERT_EQ(0, result->tm_min);
	ASSERT_EQ(0, result->tm_hour);
}

/* ============================================================================
 * MKTIME TESTS
 * ============================================================================ */
TEST_SUITE(mktime_tests);

TEST(mktime_basic) {
	tm t = {0};
	t.tm_year = 122;  // 2022
	t.tm_mon = 0;     // January
	t.tm_mday = 1;    // 1st
	t.tm_hour = 0;
	t.tm_min = 0;
	t.tm_sec = 0;
	
	time_t result = mktime(&t);
	
#ifndef JACL_ARCH_WASM
	ASSERT_EQ(1640995200, result);
#endif
}

TEST(mktime_with_time) {
	tm t = {0};
	t.tm_year = 70;   // 1970
	t.tm_mon = 0;     // January
	t.tm_mday = 1;    // 1st
	t.tm_hour = 1;
	t.tm_min = 30;
	t.tm_sec = 45;
	
	time_t result = mktime(&t);
	
#ifndef JACL_ARCH_WASM
	ASSERT_EQ(5445, result);  // 3600 + 1800 + 45
#endif
}

TEST(mktime_leap_year) {
	tm t = {0};
	t.tm_year = 120;  // 2020 (leap year)
	t.tm_mon = 1;     // February
	t.tm_mday = 29;   // 29th (valid in leap year)
	
	time_t result = mktime(&t);
	
#ifndef JACL_ARCH_WASM
	ASSERT_TRUE(result > 0);
#endif
}

TEST(mktime_null_check) {
	time_t result = mktime(NULL);
	
#ifndef JACL_ARCH_WASM
	ASSERT_EQ(-1, result);
#endif
}

/* ============================================================================
 * STRING FORMATTING TESTS
 * ============================================================================ */
TEST_SUITE(string_formatting);

TEST(asctime_format) {
	tm t = {0};
	t.tm_year = 122;  // 2022
	t.tm_mon = 0;     // January
	t.tm_mday = 1;
	t.tm_hour = 0;
	t.tm_min = 0;
	t.tm_sec = 0;
	t.tm_wday = 6;    // Saturday
	
	char *str = asctime(&t);
	ASSERT_NOT_NULL(str);
	
	// Format: "Sat Jan 01 00:00:00 2022\n"
	ASSERT_TRUE(strstr(str, "Sat") != NULL);
	ASSERT_TRUE(strstr(str, "Jan") != NULL);
}

TEST(ctime_format) {
	time_t t = 1640995200;
	char *str = ctime(&t);
	
	ASSERT_NOT_NULL(str);
	ASSERT_TRUE(strlen(str) > 0);
}

TEST(strftime_year) {
	tm t = {0};
	t.tm_year = 122;  // 2022
	
	char buf[32];
	size_t len = strftime(buf, sizeof(buf), "%Y", &t);
	
	ASSERT_EQ(4, len);
	ASSERT_STR_EQ("2022", buf);
}

TEST(strftime_month) {
	tm t = {0};
	t.tm_mon = 11;  // December (0-based)
	
	char buf[32];
	size_t len = strftime(buf, sizeof(buf), "%m", &t);
	
	ASSERT_EQ(2, len);
	ASSERT_STR_EQ("12", buf);
}

TEST(strftime_day) {
	tm t = {0};
	t.tm_mday = 15;
	
	char buf[32];
	size_t len = strftime(buf, sizeof(buf), "%d", &t);
	
	ASSERT_EQ(2, len);
	ASSERT_STR_EQ("15", buf);
}

TEST(strftime_time) {
	tm t = {0};
	t.tm_hour = 14;
	t.tm_min = 30;
	t.tm_sec = 45;
	
	char buf[32];
	size_t len = strftime(buf, sizeof(buf), "%H:%M:%S", &t);
	
	ASSERT_EQ(8, len);
	ASSERT_STR_EQ("14:30:45", buf);
}

TEST(strftime_combined) {
	tm t = {0};
	t.tm_year = 122;
	t.tm_mon = 5;
	t.tm_mday = 15;
	
	char buf[32];
	size_t len = strftime(buf, sizeof(buf), "%Y-%m-%d", &t);
	
	ASSERT_EQ(10, len);
	ASSERT_STR_EQ("2022-06-15", buf);
}

TEST(strftime_percent) {
	tm t = {0};
	char buf[32];
	size_t len = strftime(buf, sizeof(buf), "100%%", &t);
	
	ASSERT_EQ(4, len);
	ASSERT_STR_EQ("100%", buf);
}

TEST(strftime_buffer_limit) {
	tm t = {0};
	t.tm_year = 122;
	
	char buf[3];
	size_t len = strftime(buf, sizeof(buf), "%Y", &t);
	
	// Should truncate
	ASSERT_LT(len, 4);
}

TEST(strftime_null_checks) {
	tm t = {0};
	char buf[32];
	
	ASSERT_EQ(0, strftime(NULL, 10, "%Y", &t));
	ASSERT_EQ(0, strftime(buf, 10, NULL, &t));
	ASSERT_EQ(0, strftime(buf, 0, "%Y", &t));
}

/* ============================================================================
 * REENTRANT FUNCTION TESTS
 * ============================================================================ */
TEST_SUITE(reentrant);

TEST(gmtime_r_basic) {
	time_t t = 1640995200;
	tm result;
	
	tm *ptr = gmtime_r(&t, &result);
	
	ASSERT_NOT_NULL(ptr);
	ASSERT_EQ(&result, ptr);
	ASSERT_EQ(0, result.tm_sec);
	ASSERT_EQ(0, result.tm_min);
}

TEST(localtime_r_basic) {
	time_t t = 1640995200;
	tm result;
	
	tm *ptr = localtime_r(&t, &result);
	
	ASSERT_NOT_NULL(ptr);
	ASSERT_EQ(&result, ptr);
}

TEST(asctime_r_basic) {
	tm t = {0};
	t.tm_year = 122;
	t.tm_mon = 0;
	t.tm_mday = 1;
	t.tm_wday = 6;
	
	char buf[32];
	char *ptr = asctime_r(&t, buf);
	
	ASSERT_NOT_NULL(ptr);
	ASSERT_EQ(buf, ptr);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(ctime_r_basic) {
	time_t t = 1640995200;
	char buf[32];
	
	char *ptr = ctime_r(&t, buf);
	
	ASSERT_NOT_NULL(ptr);
	ASSERT_EQ(buf, ptr);
	ASSERT_TRUE(strlen(buf) > 0);
}

/* ============================================================================
 * TIMESPEC TESTS
 * ============================================================================ */
TEST_SUITE(timespec);

#if JACL_HAS_C11
TEST(timespec_get_utc) {
	timespec ts;
	int result = timespec_get(&ts, TIME_UTC);
	
	ASSERT_EQ(TIME_UTC, result);
	ASSERT_TRUE(ts.tv_sec > 0);
	ASSERT_TRUE(ts.tv_nsec >= 0);
	ASSERT_TRUE(ts.tv_nsec < 1000000000);
}

TEST(timespec_get_invalid_base) {
	timespec ts;
	int result = timespec_get(&ts, 999);
	
	ASSERT_EQ(0, result);
}

TEST(timespec_get_null) {
	int result = timespec_get(NULL, TIME_UTC);
	
	ASSERT_EQ(0, result);
}
#endif

#if JACL_HAS_C23
TEST(timespec_getres_utc) {
	timespec ts;
	int result = timespec_getres(&ts, TIME_UTC);
	
	ASSERT_EQ(TIME_UTC, result);
	ASSERT_EQ(0, ts.tv_sec);
	ASSERT_EQ(1000000, ts.tv_nsec);  // 1ms resolution
}
#endif

TEST(nanosleep_basic) {
	timespec req = {0, 1000000};  // 1ms
	timespec rem = {0, 0};
	
	int result = nanosleep(&req, &rem);
	
	ASSERT_EQ(0, result);
	ASSERT_EQ(0, rem.tv_sec);
	ASSERT_EQ(0, rem.tv_nsec);
}

TEST(nanosleep_null_check) {
	int result = nanosleep(NULL, NULL);
	
#ifndef JACL_ARCH_WASM
	ASSERT_EQ(-1, result);
#endif
}

/* ============================================================================
 * HELPER FUNCTION TESTS
 * ============================================================================ */
TEST_SUITE(helpers);

TEST(timegm_basic) {
	tm t = {0};
	t.tm_year = 70;
	t.tm_mon = 0;
	t.tm_mday = 1;
	
	time_t result = timegm(&t);
	
#ifndef JACL_ARCH_WASM
	ASSERT_EQ(0, result);
#endif
}

/* ============================================================================
 * EDGE CASE TESTS
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(year_boundaries) {
	tm t = {0};
	
	// Year 1970
	t.tm_year = 70;
	t.tm_mon = 0;
	t.tm_mday = 1;
	time_t t1 = mktime(&t);
	
#ifndef JACL_ARCH_WASM
	ASSERT_EQ(0, t1);
#endif
	
	// Year 2000
	t.tm_year = 100;
	t.tm_mon = 0;
	t.tm_mday = 1;
	time_t t2 = mktime(&t);
	
#ifndef JACL_ARCH_WASM
	ASSERT_TRUE(t2 > 0);
#endif
}

TEST(month_boundaries) {
	tm t = {0};
	t.tm_year = 122;
	
	// January (0)
	t.tm_mon = 0;
	t.tm_mday = 1;
	ASSERT_TRUE(mktime(&t) >= 0);
	
	// December (11)
	t.tm_mon = 11;
	t.tm_mday = 31;
	ASSERT_TRUE(mktime(&t) >= 0);
}

TEST(day_boundaries) {
	tm t = {0};
	t.tm_year = 122;
	t.tm_mon = 0;
	
	// First day
	t.tm_mday = 1;
	ASSERT_TRUE(mktime(&t) >= 0);
	
	// Last day of month
	t.tm_mday = 31;
	ASSERT_TRUE(mktime(&t) >= 0);
}

TEST(time_boundaries) {
	tm t = {0};
	t.tm_year = 70;
	t.tm_mon = 0;
	t.tm_mday = 1;
	
	// Midnight
	t.tm_hour = 0;
	t.tm_min = 0;
	t.tm_sec = 0;
	ASSERT_EQ(0, mktime(&t));
	
	// Last second of day
	t.tm_hour = 23;
	t.tm_min = 59;
	t.tm_sec = 59;
	ASSERT_TRUE(mktime(&t) > 0);
}

TEST(leap_year_feb_29) {
	// 2020 is a leap year
	tm t = {0};
	t.tm_year = 120;
	t.tm_mon = 1;   // February
	t.tm_mday = 29;
	
	time_t result = mktime(&t);
	
#ifndef JACL_ARCH_WASM
	ASSERT_TRUE(result > 0);
#endif
}

/* ============================================================================
 * ROUNDTRIP TESTS
 * ============================================================================ */
TEST_SUITE(roundtrip);

TEST(time_gmtime_mktime) {
	time_t original = time(NULL);
	tm *t = gmtime(&original);
	
	ASSERT_NOT_NULL(t);
	
	time_t reconstructed = mktime(t);
	
#ifndef JACL_ARCH_WASM
	// Should be close (within a few seconds)
	ASSERT_TRUE(abs(original - reconstructed) < 86400);
#endif
}

TEST(strftime_roundtrip) {
	tm t = {0};
	t.tm_year = 122;
	t.tm_mon = 5;
	t.tm_mday = 15;
	t.tm_hour = 14;
	t.tm_min = 30;
	t.tm_sec = 45;
	
	char buf[32];
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &t);
	
	ASSERT_STR_EQ("2022-06-15 14:30:45", buf);
}

TEST_MAIN()
