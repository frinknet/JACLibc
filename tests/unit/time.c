/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <time.h>
#include <string.h>

TEST_TYPE(unit)
TEST_UNIT(time.h)

/* ============================================================================
 * time() TESTS
 * ============================================================================ */
TEST_SUITE(time)

TEST(time_returns_positive_value) {
	time_t t1 = time(NULL);
	ASSERT_GT(t1, 0);
}

TEST(time_pointer_and_return_match) {
	time_t t2;
	time_t t3 = time(&t2);
	ASSERT_EQ(t2, t3);
}

TEST(time_monotonic_increasing) {
	time_t t1 = time(NULL);
	time_t t2 = time(NULL);
	ASSERT_GE(t2, t1);
}

/* ============================================================================
 * clock() TESTS
 * ============================================================================ */
TEST_SUITE(clock)

TEST(clock_returns_non_negative) {
	clock_t c = clock();
	ASSERT_GE(c, 0);
}

TEST(clock_clocks_per_sec_defined) {
	ASSERT_EQ(1000000L, CLOCKS_PER_SEC);
}

/* ============================================================================
 * difftime() TESTS
 * ============================================================================ */
TEST_SUITE(difftime)

TEST(difftime_positive_difference) {
	time_t t1 = 1000;
	time_t t2 = 1500;
	double diff = difftime(t2, t1);
	ASSERT_DBL_NEAR(500.0, diff, 0.001);
}

TEST(difftime_negative_difference) {
	time_t t1 = 1000;
	time_t t2 = 1500;
	double diff = difftime(t1, t2);
	ASSERT_DBL_NEAR(-500.0, diff, 0.001);
}

/* ============================================================================
 * gmtime() TESTS
 * ============================================================================ */
TEST_SUITE(gmtime)

TEST(gmtime_epoch_zero) {
	time_t epoch = 0;
	tm *t = gmtime(&epoch);

	ASSERT_NOT_NULL(t);
	ASSERT_EQ(0, t->tm_sec);
	ASSERT_EQ(0, t->tm_min);
	ASSERT_EQ(0, t->tm_hour);
	ASSERT_EQ(1, t->tm_mday);
	ASSERT_EQ(0, t->tm_mon);
	ASSERT_EQ(70, t->tm_year);
	ASSERT_EQ(4, t->tm_wday);
}

TEST(gmtime_known_date_2022) {
	time_t t = 1640995200;
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

/* ============================================================================
 * gmtime_r() TESTS
 * ============================================================================ */
TEST_SUITE(gmtime_r)

TEST(gmtime_r_basic_usage) {
	time_t t = 1640995200;
	tm result;

	tm *ptr = gmtime_r(&t, &result);

	ASSERT_NOT_NULL(ptr);
	ASSERT_EQ(&result, ptr);
	ASSERT_EQ(0, result.tm_sec);
	ASSERT_EQ(0, result.tm_min);
}

/* ============================================================================
 * localtime() TESTS
 * ============================================================================ */
TEST_SUITE(localtime)

TEST(localtime_basic_usage) {
	time_t t = 1640995200;
	tm *result = localtime(&t);

	ASSERT_NOT_NULL(result);
	ASSERT_EQ(0, result->tm_sec);
	ASSERT_EQ(0, result->tm_min);
	ASSERT_EQ(0, result->tm_hour);
}

/* ============================================================================
 * localtime_r() TESTS
 * ============================================================================ */
TEST_SUITE(localtime_r)

TEST(localtime_r_basic_usage) {
	time_t t = 1640995200;
	tm result;

	tm *ptr = localtime_r(&t, &result);

	ASSERT_NOT_NULL(ptr);
	ASSERT_EQ(&result, ptr);
}

/* ============================================================================
 * mktime() TESTS
 * ============================================================================ */
TEST_SUITE(mktime)

TEST(mktime_basic_conversion) {
	tm t = {0};
	t.tm_year = 122;
	t.tm_mon = 0;
	t.tm_mday = 1;
	t.tm_hour = 0;
	t.tm_min = 0;
	t.tm_sec = 0;

	time_t result = mktime(&t);

#ifndef JACL_ARCH_WASM
	ASSERT_EQ(1640995200, result);
#endif
}

TEST(mktime_with_time_components) {
	tm t = {0};
	t.tm_year = 70;
	t.tm_mon = 0;
	t.tm_mday = 1;
	t.tm_hour = 1;
	t.tm_min = 30;
	t.tm_sec = 45;

	time_t result = mktime(&t);

#ifndef JACL_ARCH_WASM
	ASSERT_EQ(5445, result);
#endif
}

TEST(mktime_leap_year_feb_29) {
	tm t = {0};
	t.tm_year = 120;
	t.tm_mon = 1;
	t.tm_mday = 29;

	time_t result = mktime(&t);

#ifndef JACL_ARCH_WASM
	ASSERT_GT(result, 0);
#endif
}

TEST(mktime_null_pointer) {
	time_t result = mktime(NULL);

#ifndef JACL_ARCH_WASM
	ASSERT_EQ(-1, result);
#endif
}

TEST(mktime_year_1970_epoch) {
	tm t = {0};
	t.tm_year = 70;
	t.tm_mon = 0;
	t.tm_mday = 1;
	time_t result = mktime(&t);

#ifndef JACL_ARCH_WASM
	ASSERT_EQ(0, result);
#endif
}

TEST(mktime_year_2000) {
	tm t = {0};
	t.tm_year = 100;
	t.tm_mon = 0;
	t.tm_mday = 1;
	time_t result = mktime(&t);

#ifndef JACL_ARCH_WASM
	ASSERT_GT(result, 0);
#endif
}

TEST(mktime_month_boundaries) {
	tm t = {0};
	t.tm_year = 122;

	t.tm_mon = 0;
	t.tm_mday = 1;
	ASSERT_GE(mktime(&t), 0);

	t.tm_mon = 11;
	t.tm_mday = 31;
	ASSERT_GE(mktime(&t), 0);
}

TEST(mktime_day_boundaries) {
	tm t = {0};
	t.tm_year = 122;
	t.tm_mon = 0;

	t.tm_mday = 1;
	ASSERT_GE(mktime(&t), 0);

	t.tm_mday = 31;
	ASSERT_GE(mktime(&t), 0);
}

TEST(mktime_time_boundaries) {
	tm t = {0};
	t.tm_year = 70;
	t.tm_mon = 0;
	t.tm_mday = 1;

	t.tm_hour = 0;
	t.tm_min = 0;
	t.tm_sec = 0;
	ASSERT_EQ(0, mktime(&t));

	t.tm_hour = 23;
	t.tm_min = 59;
	t.tm_sec = 59;
	ASSERT_GT(mktime(&t), 0);
}

TEST(mktime_roundtrip_with_gmtime) {
	time_t original = time(NULL);
	tm *t = gmtime(&original);

	ASSERT_NOT_NULL(t);

	time_t reconstructed = mktime(t);

#ifndef JACL_ARCH_WASM
	time_t diff = original > reconstructed ? original - reconstructed : reconstructed - original;
	ASSERT_LT(diff, 86400);
#endif
}

/* ============================================================================
 * asctime() TESTS
 * ============================================================================ */
TEST_SUITE(asctime)

TEST(asctime_format_output) {
	tm t = {0};
	t.tm_year = 122;
	t.tm_mon = 0;
	t.tm_mday = 1;
	t.tm_hour = 0;
	t.tm_min = 0;
	t.tm_sec = 0;
	t.tm_wday = 6;

	char *str = asctime(&t);
	ASSERT_NOT_NULL(str);
	ASSERT_STR_HAS("Sat", str);
	ASSERT_STR_HAS("Jan", str);
}

/* ============================================================================
 * asctime_r() TESTS
 * ============================================================================ */
TEST_SUITE(asctime_r)

TEST(asctime_r_basic_usage) {
	tm t = {0};
	t.tm_year = 122;
	t.tm_mon = 0;
	t.tm_mday = 1;
	t.tm_wday = 6;

	char buf[32];
	char *ptr = asctime_r(&t, buf);

	ASSERT_NOT_NULL(ptr);
	ASSERT_EQ(buf, ptr);
	ASSERT_GT(strlen(buf), 0);
}

/* ============================================================================
 * ctime() TESTS
 * ============================================================================ */
TEST_SUITE(ctime)

TEST(ctime_basic_usage) {
	time_t t = 1640995200;
	char *str = ctime(&t);

	ASSERT_NOT_NULL(str);
	ASSERT_GT(strlen(str), 0);
}

/* ============================================================================
 * ctime_r() TESTS
 * ============================================================================ */
TEST_SUITE(ctime_r)

TEST(ctime_r_basic_usage) {
	time_t t = 1640995200;
	char buf[32];

	char *ptr = ctime_r(&t, buf);

	ASSERT_NOT_NULL(ptr);
	ASSERT_EQ(buf, ptr);
	ASSERT_GT(strlen(buf), 0);
}

/* ============================================================================
 * strftime() TESTS
 * ============================================================================ */
TEST_SUITE(strftime)

TEST(strftime_format_year) {
	tm t = {0};
	t.tm_year = 122;

	char buf[32];
	size_t len = strftime(buf, sizeof(buf), "%Y", &t);

	ASSERT_EQ(4, len);
	ASSERT_STR_EQ("2022", buf);
}

TEST(strftime_format_month) {
	tm t = {0};
	t.tm_mon = 11;

	char buf[32];
	size_t len = strftime(buf, sizeof(buf), "%m", &t);

	ASSERT_EQ(2, len);
	ASSERT_STR_EQ("12", buf);
}

TEST(strftime_format_day) {
	tm t = {0};
	t.tm_mday = 15;

	char buf[32];
	size_t len = strftime(buf, sizeof(buf), "%d", &t);

	ASSERT_EQ(2, len);
	ASSERT_STR_EQ("15", buf);
}

TEST(strftime_format_time) {
	tm t = {0};
	t.tm_hour = 14;
	t.tm_min = 30;
	t.tm_sec = 45;

	char buf[32];
	size_t len = strftime(buf, sizeof(buf), "%H:%M:%S", &t);

	ASSERT_EQ(8, len);
	ASSERT_STR_EQ("14:30:45", buf);
}

TEST(strftime_format_combined) {
	tm t = {0};
	t.tm_year = 122;
	t.tm_mon = 5;
	t.tm_mday = 15;

	char buf[32];
	size_t len = strftime(buf, sizeof(buf), "%Y-%m-%d", &t);

	ASSERT_EQ(10, len);
	ASSERT_STR_EQ("2022-06-15", buf);
}

TEST(strftime_format_percent) {
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

	ASSERT_LT(len, 4);
}

TEST(strftime_null_checks) {
	tm t = {0};
	char buf[32];

	ASSERT_EQ(0, strftime(NULL, 10, "%Y", &t));
	ASSERT_EQ(0, strftime(buf, 10, NULL, &t));
	ASSERT_EQ(0, strftime(buf, 0, "%Y", &t));
}

TEST(strftime_roundtrip_full_datetime) {
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

/* ============================================================================
 * nanosleep() TESTS
 * ============================================================================ */
TEST_SUITE(nanosleep)

TEST(nanosleep_basic_sleep) {
	timespec req = {0, 1000000};
	timespec rem = {0, 0};

	int result = nanosleep(&req, &rem);

	ASSERT_EQ(0, result);
	ASSERT_EQ(0, rem.tv_sec);
	ASSERT_EQ(0, rem.tv_nsec);
}

TEST(nanosleep_null_pointer) {
	int result = nanosleep(NULL, NULL);

#ifndef JACL_ARCH_WASM
	ASSERT_EQ(-1, result);
#endif
}

/* ============================================================================
 * timespec_get() TESTS (C11)
 * ============================================================================ */
#if JACL_HAS_C11
TEST_SUITE(timespec_get)

TEST(timespec_get_utc_base) {
	timespec ts;
	int result = timespec_get(&ts, TIME_UTC);

	ASSERT_EQ(TIME_UTC, result);
	ASSERT_GT(ts.tv_sec, 0);
	ASSERT_GE(ts.tv_nsec, 0);
	ASSERT_LT(ts.tv_nsec, 1000000000);
}

TEST(timespec_get_invalid_base) {
	timespec ts;
	int result = timespec_get(&ts, 999);

	ASSERT_EQ(0, result);
}

TEST(timespec_get_null_pointer) {
	int result = timespec_get(NULL, TIME_UTC);

	ASSERT_EQ(0, result);
}
#endif

/* ============================================================================
 * timespec_getres() TESTS (C23)
 * ============================================================================ */
#if JACL_HAS_C23
TEST_SUITE(timespec_getres)

TEST(timespec_getres_utc_base) {
	timespec ts;
	int result = timespec_getres(&ts, TIME_UTC);

	ASSERT_EQ(TIME_UTC, result);
	ASSERT_EQ(0, ts.tv_sec);
	ASSERT_EQ(1000000, ts.tv_nsec);
}
#endif

/* ============================================================================
 * timegm() TESTS
 * ============================================================================ */
TEST_SUITE(timegm)

TEST(timegm_epoch_conversion) {
	tm t = {0};
	t.tm_year = 70;
	t.tm_mon = 0;
	t.tm_mday = 1;

	time_t result = timegm(&t);

#ifndef JACL_ARCH_WASM
	ASSERT_EQ(0, result);
#endif
}

TEST_MAIN()

