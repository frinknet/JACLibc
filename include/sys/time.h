/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef SYS_TIME_H
#define SYS_TIME_H
#pragma once

#include <config.h>
#include <sys/types.h>
#include <sys/syscall.h>   /* SYS_gettimeofday, SYS_settimeofday, etc. */
#include <time.h>          /* for clock_gettime, CLOCK_REALTIME */

#if JACL_OS_WINDOWS
	#define SYSTIME_WIN32
	#include <windows.h>
	#include <winsock2.h>  /* For struct timeval */
#elif JACL_ARCH_WASM
	#define SYSTIME_WASM
#else
	#define SYSTIME_POSIX
	#include <sys/syscall.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================ */
/* Time structures                                                  */
/* ================================================================ */

#ifndef SYSTIME_WIN32
/* Standard timeval structure (Windows gets it from winsock2.h) */
typedef struct timeval {
	time_t tv_sec;      /* Seconds */
	suseconds_t tv_usec; /* Microseconds */
} timeval;
#else
typedef struct timeval timeval;
#endif

/* Time zone structure */
typedef struct timezone {
	int tz_minuteswest; /* Minutes west of Greenwich */
	int tz_dsttime;     /* Type of DST correction */
} timezone;

/* Interval timer structures */
typedef struct itimerval {
	timeval it_interval; /* Timer interval */
	timeval it_value;    /* Current value */
} itimerval;


/* Timer types for setitimer/getitimer */
#define ITIMER_REAL    0  /* Real time timer */
#define ITIMER_VIRTUAL 1  /* Virtual time timer */
#define ITIMER_PROF    2  /* Profiling timer */

/* ================================================================ */
/* Time manipulation macros                                         */
/* ================================================================ */

/* Clear a timeval */
#define timerclear(tvp) ((tvp)->tv_sec = (tvp)->tv_usec = 0)

/* Test if a timeval is set */
#define timerisset(tvp) ((tvp)->tv_sec || (tvp)->tv_usec)

/* Compare timevals */
#define timercmp(a, b, CMP) \
	(((a)->tv_sec == (b)->tv_sec) ? \
	 ((a)->tv_usec CMP (b)->tv_usec) : \
	 ((a)->tv_sec CMP (b)->tv_sec))

/* Add timevals */
#define timeradd(a, b, result) \
	do { \
		(result)->tv_sec = (a)->tv_sec + (b)->tv_sec; \
		(result)->tv_usec = (a)->tv_usec + (b)->tv_usec; \
		if ((result)->tv_usec >= 1000000) { \
			(result)->tv_sec++; \
			(result)->tv_usec -= 1000000; \
		} \
	} while (0)

/* Subtract timevals */
#define timersub(a, b, result) \
	do { \
		(result)->tv_sec = (a)->tv_sec - (b)->tv_sec; \
		(result)->tv_usec = (a)->tv_usec - (b)->tv_usec; \
		if ((result)->tv_usec < 0) { \
			(result)->tv_sec--; \
			(result)->tv_usec += 1000000; \
		} \
	} while (0)

/* ================================================================ */
/* Function implementations                                         */
/* ================================================================ */

#ifdef SYSTIME_WIN32
/* ================================================================ */
/* Windows implementation                                           */
/* ================================================================ */

static inline int gettimeofday(timeval *tv, timezone *tz) {
	if (!tv) return -1;

	FILETIME ft;
	ULARGE_INTEGER ull;

	GetSystemTimeAsFileTime(&ft);
	ull.LowPart = ft.dwLowDateTime;
	ull.HighPart = ft.dwHighDateTime;

	/* Windows epoch: Jan 1, 1601; Unix epoch: Jan 1, 1970 */
	/* Difference: 116444736000000000 * 100ns intervals */
	ull.QuadPart -= 116444736000000000ULL;

	tv->tv_sec = (time_t)(ull.QuadPart / 10000000ULL);
	tv->tv_usec = (suseconds_t)((ull.QuadPart % 10000000ULL) / 10);

	if (tz) {
		TIME_ZONE_INFORMATION tzi;
		DWORD tzres = GetTimeZoneInformation(&tzi);
		if (tzres != TIME_ZONE_ID_INVALID) {
			tz->tz_minuteswest = tzi.Bias;
			tz->tz_dsttime = (tzres == TIME_ZONE_ID_DAYLIGHT) ? 1 : 0;
		} else {
			tz->tz_minuteswest = 0;
			tz->tz_dsttime = 0;
		}
	}

	return 0;
}

static inline int settimeofday(const timeval *tv, const timezone *tz) {
	(void)tz; /* Windows doesn't support setting timezone this way */

	if (!tv) return -1;

	/* Convert to Windows FILETIME */
	ULARGE_INTEGER ull;
	ull.QuadPart = ((ULONGLONG)tv->tv_sec * 10000000ULL) +
	               ((ULONGLONG)tv->tv_usec * 10ULL) +
	               116444736000000000ULL;

	FILETIME ft;
	ft.dwLowDateTime = ull.LowPart;
	ft.dwHighDateTime = ull.HighPart;

	SYSTEMTIME st;
	if (!FileTimeToSystemTime(&ft, &st)) return -1;

	return SetSystemTime(&st) ? 0 : -1;
}

static inline int getitimer(int which, itimerval *curr_value) {
	(void)which; (void)curr_value;

	return -1; /* Not supported on Windows */
}

static inline int setitimer(int which, const itimerval *new_value, itimerval *old_value) {
	(void)which; (void)new_value; (void)old_value;

	return -1; /* Not supported on Windows */
}

static inline int utimes(const char *pathname, const timeval times[2]) {
	if (!pathname) return -1;

	HANDLE hFile = CreateFileA(pathname, FILE_WRITE_ATTRIBUTES,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

	if (hFile == INVALID_HANDLE_VALUE) return -1;

	FILETIME atime, mtime;
	if (times) {
		/* Convert timeval to FILETIME */
		ULARGE_INTEGER ull;

		ull.QuadPart = ((ULONGLONG)times[0].tv_sec * 10000000ULL) +
		               ((ULONGLONG)times[0].tv_usec * 10ULL) +
		               116444736000000000ULL;
		atime.dwLowDateTime = ull.LowPart;
		atime.dwHighDateTime = ull.HighPart;

		ull.QuadPart = ((ULONGLONG)times[1].tv_sec * 10000000ULL) +
		               ((ULONGLONG)times[1].tv_usec * 10ULL) +
		               116444736000000000ULL;
		mtime.dwLowDateTime = ull.LowPart;
		mtime.dwHighDateTime = ull.HighPart;
	} else {
		/* Use current time */
		GetSystemTimeAsFileTime(&atime);
		mtime = atime;
	}

	BOOL result = SetFileTime(hFile, NULL, &atime, &mtime);
	CloseHandle(hFile);

	return result ? 0 : -1;
}

#elif defined(SYSTIME_WASM)
/* ================================================================ */
/* WebAssembly - Limited time support                              */
/* ================================================================ */

static inline int gettimeofday(timeval *tv, timezone *tz) {
	(void)tv; (void)tz;

	return -1; /* Not supported */
}

static inline int settimeofday(const timeval *tv, const timezone *tz) {
	(void)tv; (void)tz;

	return -1; /* Not supported */
}

static inline int getitimer(int which, itimerval *curr_value) {
	(void)which; (void)curr_value;

	return -1; /* Not supported */
}

static inline int setitimer(int which, const itimerval *new_value, itimerval *old_value) {
	(void)which; (void)new_value; (void)old_value;

	return -1; /* Not supported */
}

static inline int utimes(const char *pathname, const timeval times[2]) {
	(void)pathname; (void)times;

	return -1; /* Not supported */
}

#else
/* ================================================================ */
/* POSIX implementation                                             */
/* ================================================================ */

static inline int gettimeofday(timeval *tv, timezone *tz) {
	if (!tv) return -1;

	#if defined(SYS_gettimeofday)
		return (int)syscall(SYS_gettimeofday, tv, tz);
	#endif

	/* Fallback using clock_gettime */
	timespec ts;

	if (clock_gettime(CLOCK_REALTIME, &ts) != 0) return -1;

	tv->tv_sec = ts.tv_sec;
	tv->tv_usec = ts.tv_nsec / 1000;

	if (tz) {
		/* Simplified timezone handling */
		tz->tz_minuteswest = 0;
		tz->tz_dsttime = 0;
	}

	return 0;
}

static inline int settimeofday(const timeval *tv, const timezone *tz) {
	(void)tz; /* Timezone setting deprecated */

	if (!tv) return -1;

	#if defined(SYS_settimeofday)
		return (int)syscall(SYS_settimeofday, tv, NULL);
	#endif

	/* Fallback using clock_settime */
	timespec ts;

	ts.tv_sec = tv->tv_sec;
	ts.tv_nsec = tv->tv_usec * 1000;

	return clock_settime(CLOCK_REALTIME, &ts);
}

static inline int getitimer(int which, itimerval *curr_value) {
	if (!curr_value) return -1;

	#if defined(SYS_getitimer)
		return (int)syscall(SYS_getitimer, which, curr_value);
	#else
		(void)which;
		(void)curr_value;

		errno = ENOSYS;

		return -1;
	#endif
}

static inline int setitimer(int which, const itimerval *new_value, itimerval *old_value) {
	if (!new_value) return -1;

	#if defined(SYS_setitimer)
		return (int)syscall(SYS_setitimer, which, new_value, old_value);
	#else
		(void)which; (void)old_value;

		errno = ENOSYS;

		return -1;
	#endif
}

static inline int utimes(const char *pathname, const timeval times[2]) {
	if (!pathname) return -1;

	#if defined(SYS_utimes)
		return (int)syscall(SYS_utimes, pathname, times);
	#else
		(void)times;

		errno = ENOSYS;

		return -1;
	#endif
}

/* Additional POSIX.1-2008 functions */
/* Additional POSIX.1-2008 functions */
static inline int futimes(int fd, const timeval times[2]) {
	#if defined(SYS_futimes)
		return (int)syscall(SYS_futimes, fd, times);
	#else
		(void)fd;
		(void)times;

		errno = ENOSYS;

		return -1;
	#endif
}

static inline int lutimes(const char *pathname, const timeval times[2]) {
	if (!pathname) return -1;

	#if defined(SYS_lutimes)
		return (int)syscall(SYS_lutimes, pathname, times);
	#else
		(void)times;

		errno = ENOSYS;

		return -1;
	#endif
}

#endif

/* ================================================================ */
/* Additional utility functions                                     */
/* ================================================================ */

/* Convert timespec to timeval */
static inline void timespec_to_timeval(const timespec *ts, timeval *tv) {
	if (ts && tv) {
		tv->tv_sec = ts->tv_sec;
		tv->tv_usec = ts->tv_nsec / 1000;
	}
}

/* Convert timeval to timespec */
static inline void timeval_to_timespec(const timeval *tv, timespec *ts) {
	if (tv && ts) {
		ts->tv_sec = tv->tv_sec;
		ts->tv_nsec = tv->tv_usec * 1000;
	}
}

#ifdef __cplusplus
}
#endif

#endif // SYS_TIME_H
