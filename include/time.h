/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef TIME_H
#define TIME_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef long			 time_t;
typedef long			 clock_t;
#define CLOCKS_PER_SEC 1000000L

struct tm {
		int tm_sec;
		int tm_min;
		int tm_hour;
		int tm_mday;
		int tm_mon;
		int tm_year;
		int tm_wday;
		int tm_yday;
		int tm_isdst;
};

/* Stub implementations: no-ops returning zero or NULL */

static inline clock_t clock(void) {
		return 0;
}

static inline time_t time(time_t *tloc) {
		if (tloc) *tloc = 0;
		return 0;
}

static inline double difftime(time_t time1, time_t time0) {
		(void)time1; (void)time0;
		return 0.0;
}

static inline time_t mktime(struct tm *tm) {
		(void)tm;
		return 0;
}

static inline struct tm *gmtime(const time_t *timer) {
		(void)timer;
		return NULL;
}

static inline struct tm *localtime(const time_t *timer) {
		(void)timer;
		return NULL;
}

static inline char *asctime(const struct tm *tm) {
		(void)tm;
		return NULL;
}

static inline char *ctime(const time_t *timer) {
		(void)timer;
		return NULL;
}

static inline size_t strftime(char *s, size_t max, const char *fmt, const struct tm *tm) {
		(void)s; (void)max; (void)fmt; (void)tm;
		return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* TIME_H */

