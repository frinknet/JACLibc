/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef TIME_H
#define TIME_H
#pragma once

#include <config.h>
#include <errno.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/syscall.h>

#ifdef __cplusplus
extern "C" {
#endif

/* C23 feature test macro */
#if JACL_HAS_C23
#define __STDC_VERSION_TIME_H__ 202311L
#endif

/* ISO C constants */
#define CLOCKS_PER_SEC 1000000L

/* C11/C23 timespec_get constants */
#define TIME_UTC           1
#define TIME_MONOTONIC     2
#define TIME_ACTIVE        3
#define TIME_THREAD_ACTIVE 4

/* POSIX timespec structure */
typedef struct timespec {
	time_t tv_sec;   /* Seconds */
	long  tv_nsec;   /* Nanoseconds */
} timespec;

/* Standard tm structure */
typedef struct tm {
	int tm_sec;    /* Seconds (0-60) */
	int tm_min;    /* Minutes (0-59) */
	int tm_hour;   /* Hours (0-23) */
	int tm_mday;   /* Day of month (1-31) */
	int tm_mon;    /* Month (0-11) */
	int tm_year;   /* Years since 1900 */
	int tm_wday;   /* Day of week (0-6, Sunday = 0) */
	int tm_yday;   /* Day of year (0-365) */
	int tm_isdst;  /* Daylight saving time flag */
} tm;

/* POSIX clock IDs (Linux-compatible core subset) */
#define CLOCK_REALTIME           0
#define CLOCK_MONOTONIC          1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID  3

/* ================================================================ */
/* Helper routines (calendar math, string helpers)                  */
/* ================================================================ */

static inline int __jacl_is_leap_year(int year) {
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static inline int __jacl_days_in_month(int month, int year) {
	static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	if (month == 1 && __jacl_is_leap_year(year)) return 29;

	return days_in_month[month];
}

static inline void __jacl_num_to_str(char *buf, int num, int width) {
	for (int i = width - 1; i >= 0; i--) {
		buf[i] = (char)('0' + (num % 10));
		num /= 10;
	}
}

static inline void __jacl_str_copy(char *dest, const char *src, int len) {
	for (int i = 0; i < len && src[i]; i++) dest[i] = src[i];
}

/* ================================================================ */
/* Core time sources                                                */
/* ================================================================ */

/* Linux: use clock_gettime via syscall layer where possible */
#if JACL_OS_LINUX

static inline int clock_gettime(clockid_t clk_id, struct timespec *tp) {
	if (!tp) { errno = EINVAL; return -1; }

	long r = syscall(SYS_clock_gettime, (long)clk_id, (long)tp);

	return (int)r;
}
static inline int clock_getres(clockid_t clk_id, struct timespec *tp) {
	if (!tp) { errno = EINVAL; return -1;	}

	long r = syscall(SYS_clock_getres, (long)clk_id, (long)tp);

	return (int)r;
}
static inline int nanosleep(const struct timespec *req, struct timespec *rem) {
	if (!req || req->tv_sec < 0 || req->tv_nsec < 0 || req->tv_nsec >= 1000000000L) { errno = EINVAL; return -1; }

	long r = syscall(SYS_nanosleep, (long)req, (long)rem);

	return (int)r;
}

/* clock(): use CLOCK_PROCESS_CPUTIME_ID if available, else CLOCK_MONOTONIC.
   Convert seconds+nanoseconds to CLOCKS_PER_SEC ticks. */
static inline clock_t clock(void) {
	struct timespec ts;

	if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) != 0) {
		if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) return (clock_t)-1;
	}

	long double seconds = (long double)ts.tv_sec + (long double)ts.tv_nsec / 1000000000.0L;
	long double ticks = seconds * (long double)CLOCKS_PER_SEC;

	if (ticks > (long double)((clock_t)-1)) return (clock_t)-1;

	return (clock_t)ticks;
}

/* time(): use CLOCK_REALTIME and ignore subsecond part. */
static inline time_t time(time_t *tloc) {
	struct timespec ts;

	if (clock_gettime(CLOCK_REALTIME, &ts) != 0) return (time_t)-1;
	if (tloc) *tloc = ts.tv_sec;

	return ts.tv_sec;
}

#elif JACL_OS_WINDOWS

/* Windows time via Win32 APIs */

__declspec(dllimport) void __stdcall Sleep(unsigned long dwMilliseconds);
__declspec(dllimport) void __stdcall GetSystemTimeAsFileTime(void *lpSystemTimeAsFileTime);

/* FILETIME epoch offset: 1601-01-01 to 1970-01-01 in 100ns units */
#define __JACL_WIN_EPOCH_DIFF 116444736000000000ULL

static inline clock_t clock(void) {
	/* Fallback: use GetSystemTimeAsFileTime granularity ~ms, scaled to CLOCKS_PER_SEC */
	union { unsigned long long ll; struct { unsigned long lo, hi; } s; } ft;

	GetSystemTimeAsFileTime(&ft);

	unsigned long long t100ns = ft.ll - __JACL_WIN_EPOCH_DIFF;
	long double seconds = (long double)t100ns / 10000000.0L;
	long double ticks   = seconds * (long double)CLOCKS_PER_SEC;

	if (ticks > (long double)((clock_t)-1)) return (clock_t)-1;

	return (clock_t)ticks;
}

static inline time_t time(time_t *tloc) {
	union { unsigned long long ll; struct { unsigned long lo, hi; } s; } ft;

	GetSystemTimeAsFileTime(&ft);

	unsigned long long t100ns = ft.ll - __JACL_WIN_EPOCH_DIFF;
	time_t result = (time_t)(t100ns / 10000000ULL);

	if (tloc) *tloc = result;

	return result;
}

static inline int nanosleep(const struct timespec *req, struct timespec *rem) {
	if (!req || req->tv_sec < 0 || req->tv_nsec < 0 || req->tv_nsec >= 1000000000L) { errno = EINVAL; return -1; }

	unsigned long ms = (unsigned long)(req->tv_sec * 1000 + req->tv_nsec / 1000000);

	if (ms == 0 && (req->tv_sec > 0 || req->tv_nsec > 0)) ms = 1;

	Sleep(ms);

	if (rem) { rem->tv_sec = 0; rem->tv_nsec = 0; }

	return 0;
}

static inline int clock_gettime(clockid_t clk_id, struct timespec *tp) {
	(void)clk_id;

	if (!tp) { errno = EINVAL; return -1; }

	time_t now = time(NULL);

	if (now == (time_t)-1) return -1;

	tp->tv_sec  = now;
	tp->tv_nsec = 0;

	return 0;
}

static inline int clock_getres(clockid_t clk_id, struct timespec *tp) {
	(void)clk_id;

	if (!tp) { errno = EINVAL; return -1; }

	tp->tv_sec  = 0;
	tp->tv_nsec = 1000000L; /* assume 1ms resolution */

	return 0;
}

#elif JACL_ARCH_WASM

/* WASM: still stubby, but clearly marked and consistent */

#define WASM_EPOCH_TIME 1640995200 /* 2022-01-01 00:00:00 UTC */

static tm __jacl_wasm_static_tm = {
	0, 0, 0, 1, 0, 122, 6, 0, 0
};

static inline clock_t clock(void) { return 0; }

static inline time_t time(time_t *tloc) {
	if (tloc) *tloc = WASM_EPOCH_TIME;
	return WASM_EPOCH_TIME;
}

static inline int nanosleep(const struct timespec *req, struct timespec *rem) {
	(void)req;

	if (rem) { rem->tv_sec = 0; rem->tv_nsec = 0; }

	return 0;
}

static inline int clock_gettime(clockid_t clk_id, struct timespec *tp) {
	(void)clk_id;

	if (!tp) { errno = EINVAL; return -1; }

	tp->tv_sec  = WASM_EPOCH_TIME;
	tp->tv_nsec = 0;

	return 0;
}

static inline int clock_getres(clockid_t clk_id, struct timespec *tp) {
	(void)clk_id;

	if (!tp) { errno = EINVAL; return -1; }

	tp->tv_sec  = 0;
	tp->tv_nsec = 1000000L;

	return 0;
}

#else

/* Generic fallback: very coarse, but not totally fake */

static inline clock_t clock(void) {
	static clock_t ticks = 0;

	return ++ticks * (CLOCKS_PER_SEC / 1000);
}

static inline time_t time(time_t *tloc) {
	static time_t base = 1640995200; /* arbitrary fixed base */
	static unsigned long counter = 0;
	time_t t = base + (time_t)(counter++ / CLOCKS_PER_SEC);

	if (tloc) *tloc = t;

	return t;
}

static inline int nanosleep(const struct timespec *req, struct timespec *rem) {
	if (!req) { errno = EINVAL; return -1; }

	volatile long long spin = req->tv_sec * 1000000LL + req->tv_nsec / 1000;

	while (spin-- > 0) { /* busy wait */ }
	if (rem) { rem->tv_sec = 0; rem->tv_nsec = 0; }

	return 0;
}

static inline int clock_gettime(clockid_t clk_id, struct timespec *tp) {
	(void)clk_id;

	if (!tp) { errno = EINVAL; return -1; }

	time_t now = time(NULL);

	if (now == (time_t)-1) return -1;

	tp->tv_sec  = now;
	tp->tv_nsec = 0;

	return 0;
}

static inline int clock_getres(clockid_t clk_id, struct timespec *tp) {
	(void)clk_id;

	if (!tp) { errno = EINVAL; return -1; }

	tp->tv_sec  = 0;
	tp->tv_nsec = 1000000L;

	return 0;
}

#endif /* time source selection */

/* ================================================================ */
/* ISO C utilities: difftime, calendar conversions                  */
/* ================================================================ */

static inline double difftime(time_t time1, time_t time0) {
	return (double)(time1 - time0);
}

static inline time_t mktime(tm *t) {
	if (!t) return (time_t)-1;

	int year  = t->tm_year + 1900;
	int month = t->tm_mon;
	int mday  = t->tm_mday;

	if (year < 1970 || month < 0 || month > 11 || mday < 1 || mday > 31) return (time_t)-1;

	long days = 0;

	for (int y = 1970; y < year; y++) days += __jacl_is_leap_year(y) ? 366 : 365;
	for (int m = 0; m < month; m++) days += __jacl_days_in_month(m, year);

	days += (mday - 1);

	time_t result = (time_t)(days * 86400L + t->tm_hour * 3600L + t->tm_min  * 60L + t->tm_sec);

	t->tm_wday = (int)((days + 4) % 7); /* 1970-01-01 was Thursday (4) */
	t->tm_yday = 0;

	for (int m = 0; m < month; m++) t->tm_yday += __jacl_days_in_month(m, year);

	t->tm_yday += (mday - 1);
	t->tm_isdst = 0;

	return result;
}

/* Static storage for non-reentrant gmtime/localtime */
static tm __jacl_static_tm;

/* gmtime: UTC conversion from time_t */
static inline tm *gmtime(const time_t *timer) {
	if (!timer) return NULL;

	time_t t = *timer;

	if (t < 0) return NULL;

	long days = (long)(t / 86400L);
	long rem  = (long)(t % 86400L);

	if (rem < 0) { rem += 86400L; days--; }

	__jacl_static_tm.tm_hour = (int)(rem / 3600L);

	rem %= 3600L;

	__jacl_static_tm.tm_min  = (int)(rem / 60L);
	__jacl_static_tm.tm_sec  = (int)(rem % 60L);

	__jacl_static_tm.tm_wday = (int)((days + 4) % 7);

	if (__jacl_static_tm.tm_wday < 0) __jacl_static_tm.tm_wday += 7;

	int year = 1970;

	while (1) {
		int dyear = __jacl_is_leap_year(year) ? 366 : 365;

		if (days < dyear) break;

		days -= dyear;
		year++;
	}

	__jacl_static_tm.tm_year = year - 1900;
	__jacl_static_tm.tm_yday = (int)days;

	int month = 0;

	while (month < 12) {
		int dmon = __jacl_days_in_month(month, year);

		if (days < dmon) break;

		days -= dmon;
		month++;
	}

	__jacl_static_tm.tm_mon  = month;
	__jacl_static_tm.tm_mday = (int)days + 1;
	__jacl_static_tm.tm_isdst = 0;

	return &__jacl_static_tm;
}

/* localtime: for now, just UTC alias (no timezone/DST handling yet) */
static inline tm *localtime(const time_t *timer) {
	return gmtime(timer);
}

/* Reentrant variants */
static inline tm *gmtime_r(const time_t *timer, tm *result) {
	if (!result) return NULL;

	tm *tmp = gmtime(timer);

	if (!tmp) return NULL;

	*result = *tmp;

	return result;
}

static inline tm *localtime_r(const time_t *timer, tm *result) {
	if (!result) return NULL;

	tm *tmp = localtime(timer);

	if (!tmp) return NULL;

	*result = *tmp;

	return result;
}

/* timegm: treat tm as UTC and convert; same as mktime for this libc */
static inline time_t timegm(tm *t) {
	return mktime(t);
}

/* ================================================================ */
/* Formatting: asctime / ctime / strftime                          */
/* ================================================================ */

static char __jacl_time_buf[32];

#if JACL_HAS_C23
__deprecated_msg("use strftime() instead")
#endif
static inline char *asctime(const tm *t) {
	if (!t) return NULL;

	static const char *day_names[]   = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	static const char *month_names[] = {"Jan","Feb","Mar","Apr","May","Jun", "Jul","Aug","Sep","Oct","Nov","Dec"};
	int wday = (t->tm_wday >= 0 && t->tm_wday < 7)  ? t->tm_wday : 0;
	int mon  = (t->tm_mon  >= 0 && t->tm_mon  < 12) ? t->tm_mon  : 0;
	int year = t->tm_year + 1900;
	char *p = __jacl_time_buf;

	__jacl_str_copy(p, day_names[wday], 3); p += 3; *p++ = ' ';
	__jacl_str_copy(p, month_names[mon], 3); p += 3; *p++ = ' ';
	__jacl_num_to_str(p, t->tm_mday, 2); p += 2; *p++ = ' ';
	__jacl_num_to_str(p, t->tm_hour, 2); p += 2; *p++ = ':';
	__jacl_num_to_str(p, t->tm_min, 2);  p += 2; *p++ = ':';
	__jacl_num_to_str(p, t->tm_sec, 2);  p += 2; *p++ = ' ';
	__jacl_num_to_str(p, year, 4);       p += 4;

	*p++ = '\n';
	*p   = '\0';

	return __jacl_time_buf;
}

#if JACL_HAS_C23
__deprecated_msg("use strftime() instead")
#endif
static inline char *ctime(const time_t *timer) {
	tm *t = localtime(timer);

	return t ? asctime(t) : NULL;
}

static inline size_t strftime(char *s, size_t max, const char *fmt, const tm *t) {
	if (!s || !fmt || !t || max == 0) return 0;

	size_t pos = 0;
	char tmp[16];

	while (*fmt && pos < max - 1) {
		if (*fmt != '%') { s[pos++] = *fmt++; continue; }

		fmt++;

		switch (*fmt ? *fmt++ : '\0') {
		case 'Y':
			__jacl_num_to_str(tmp, t->tm_year + 1900, 4);

			for (int i = 0; i < 4 && pos < max - 1; i++) s[pos++] = tmp[i];

			break;
		case 'm':
			__jacl_num_to_str(tmp, t->tm_mon + 1, 2);

			for (int i = 0; i < 2 && pos < max - 1; i++) s[pos++] = tmp[i];

			break;
		case 'd':
			__jacl_num_to_str(tmp, t->tm_mday, 2);

			for (int i = 0; i < 2 && pos < max - 1; i++) s[pos++] = tmp[i];

			break;
		case 'H':
			__jacl_num_to_str(tmp, t->tm_hour, 2);

			for (int i = 0; i < 2 && pos < max - 1; i++) s[pos++] = tmp[i];

			break;
		case 'M':
			__jacl_num_to_str(tmp, t->tm_min, 2);

			for (int i = 0; i < 2 && pos < max - 1; i++) s[pos++] = tmp[i];

			break;
		case 'S':
			__jacl_num_to_str(tmp, t->tm_sec, 2);

			for (int i = 0; i < 2 && pos < max - 1; i++) s[pos++] = tmp[i];

			break;
		case '%':
			s[pos++] = '%';

			break;
		default:
			if (pos < max - 1) s[pos++] = fmt[-1];

			break;
		}
	}

	s[pos] = '\0';

	return pos;
}

/* Reentrant asctime/ctime */

#if JACL_HAS_C23
__deprecated_msg("use strftime() instead")
#endif
static inline char *asctime_r(const tm *t, char *buf) {
	if (!t || !buf) return NULL;

	char *tmp = asctime(t);

	if (!tmp) return NULL;
	for (int i = 0; i < 26; i++) buf[i] = tmp[i];

	return buf;
}

#if JACL_HAS_C23
__deprecated_msg("use strftime() instead")
#endif
static inline char *ctime_r(const time_t *timer, char *buf) {
	if (!buf) return NULL;

	char *tmp = ctime(timer);

	if (!tmp) return NULL;
	for (int i = 0; i < 26; i++) buf[i] = tmp[i];

	return buf;
}

/* ================================================================ */
/* C11/C23: timespec_get / timespec_getres                          */
/* ================================================================ */

#if JACL_HAS_C11
static inline int timespec_get(struct timespec *ts, int base) {
	if (!ts) return 0;
	if (base == TIME_UTC) {
		if (clock_gettime(CLOCK_REALTIME, ts) != 0) return 0;

		return TIME_UTC;
	}
#if JACL_OS_LINUX
	if (base == TIME_MONOTONIC) {
		if (clock_gettime(CLOCK_MONOTONIC, ts) != 0) return 0;

		return TIME_MONOTONIC;
	}
#endif
	return 0;
}
#endif /* C11 */

#if JACL_HAS_C23
static inline int timespec_getres(struct timespec *ts, int base) {
	if (!ts) return 0;
	if (base == TIME_UTC) {
		if (clock_getres(CLOCK_REALTIME, ts) != 0) return 0;

		return TIME_UTC;
	}
#if JACL_OS_LINUX
	if (base == TIME_MONOTONIC) {
		if (clock_getres(CLOCK_MONOTONIC, ts) != 0) return 0;

		return TIME_MONOTONIC;
	}
#endif
	return 0;
}
#endif /* C23 */

#ifdef __cplusplus
}
#endif

#endif /* TIME_H */
