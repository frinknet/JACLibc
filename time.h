// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef TIME_H
#define TIME_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* C99 Time Utilities */
typedef long time_t;
typedef long clock_t;

/* Clock ticks per second */
#define CLOCKS_PER_SEC 1000000L

/* Broken-down time */
struct tm {
	int tm_sec;		 /* seconds [0,61] */
	int tm_min;		 /* minutes [0,59] */
	int tm_hour;	 /* hours [0,23] */
	int tm_mday;	 /* day of month [1,31] */
	int tm_mon;		 /* month of year [0,11] */
	int tm_year;	 /* years since 1900 */
	int tm_wday;	 /* day of week [0,6] (Sunday = 0) */
	int tm_yday;	 /* day of year [0,365] */
	int tm_isdst;  /* daylight savings flag */
};

static inline clock_t clock(void) { return __builtin_clock(); }
static inline time_t time(time_t *tloc) { return __builtin_time(tloc); }
static inline double difftime(time_t time1, time_t time0) { return __builtin_difftime(time1, time0); }
static inline time_t mktime(struct tm *tm) { return __builtin_mktime(tm); }
static inline struct tm *gmtime(const time_t *timer) { return __builtin_gmtime(timer); }
static inline struct tm *localtime(const time_t *timer) {  return __builtin_localtime(timer); }
static inline char *asctime(const struct tm *tm) { return __builtin_asctime(tm); }
static inline char *ctime(const time_t *timer) { return __builtin_ctime(timer); }
static inline size_t strftime(char *s, size_t max, const char *fmt, const struct tm *tm) { return __builtin_strftime(s, max, fmt, tm); }

#ifdef __cplusplus
}
#endif

#endif /* TIME_H */
