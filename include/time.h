/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef TIME_H
#define TIME_H
#pragma once

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* C23 feature test macro */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
#  define __STDC_VERSION_TIME_H__ 202311L
#  define __JACL_DEPRECATED [[deprecated]]
#  define __JACL_DEPRECATED_MSG(msg) [[deprecated(msg)]]
#else
#  define __JACL_DEPRECATED
#  define __JACL_DEPRECATED_MSG(msg)
#endif

#define CLOCKS_PER_SEC 1000000L

/* C11/C23 timespec_get constants */
#define TIME_UTC           1
#define TIME_MONOTONIC     2
#define TIME_ACTIVE        3  
#define TIME_THREAD_ACTIVE 4

/* POSIX timespec structure */
struct timespec {
    time_t tv_sec;   /* Seconds */
    long tv_nsec;    /* Nanoseconds */
};

/* Standard tm structure */
struct tm {
    int tm_sec;     /* Seconds (0-60) */
    int tm_min;     /* Minutes (0-59) */
    int tm_hour;    /* Hours (0-23) */
    int tm_mday;    /* Day of month (1-31) */
    int tm_mon;     /* Month (0-11) */
    int tm_year;    /* Years since 1900 */
    int tm_wday;    /* Day of week (0-6, Sunday = 0) */
    int tm_yday;    /* Day of year (0-365) */
    int tm_isdst;   /* Daylight saving time flag */
};

#define CLOCK_REALTIME  0
#define CLOCK_MONOTONIC 1

#ifdef __wasm__
/* ================================================================ */
/* WebAssembly - Honest stubs (no system time available)           */
/* ================================================================ */

#define WASM_EPOCH_TIME 1640995200

static struct tm wasm_static_tm = {
    0, 0, 0, 1, 0, 122, 6, 0, 0
};

/* Basic time functions */
static inline clock_t clock(void) { return 0; }
static inline time_t time(time_t *tloc) { 
    if (tloc) *tloc = WASM_EPOCH_TIME; 
    return WASM_EPOCH_TIME; 
}
static inline double difftime(time_t time1, time_t time0) { 
    return (double)(time1 - time0); 
}
static inline time_t mktime(struct tm *tm) { 
    (void)tm; return WASM_EPOCH_TIME;
}
static inline struct tm *gmtime(const time_t *timer) { 
    (void)timer; return &wasm_static_tm;
}
static inline struct tm *localtime(const time_t *timer) { 
    (void)timer; return &wasm_static_tm;
}

__JACL_DEPRECATED_MSG("Use strftime() instead")
static inline char *asctime(const struct tm *tm) { 
    (void)tm; 
    static char wasm_timestr[] = "Sat Jan 01 00:00:00 2022\n";
    return wasm_timestr; 
}

__JACL_DEPRECATED_MSG("Use strftime() instead")
static inline char *ctime(const time_t *timer) { 
    (void)timer; 
    static char wasm_timestr[] = "Sat Jan 01 00:00:00 2022\n";
    return wasm_timestr; 
}

static inline size_t strftime(char *s, size_t max, const char *fmt, const struct tm *tm) {
    (void)tm;
    if (!s || !fmt || max == 0) return 0;
    
    const char *src = fmt;
    size_t pos = 0;
    
    while (*src && pos < max - 1) {
        if (*src == '%' && *(src + 1)) {
            src++;
            switch (*src) {
                case 'Y': if (pos + 4 < max) { s[pos++] = '2'; s[pos++] = '0'; s[pos++] = '2'; s[pos++] = '2'; } break;
                case 'm': if (pos + 2 < max) { s[pos++] = '0'; s[pos++] = '1'; } break;
                case 'd': if (pos + 2 < max) { s[pos++] = '0'; s[pos++] = '1'; } break;
                case 'H': if (pos + 2 < max) { s[pos++] = '0'; s[pos++] = '0'; } break;
                case 'M': if (pos + 2 < max) { s[pos++] = '0'; s[pos++] = '0'; } break;
                case 'S': if (pos + 2 < max) { s[pos++] = '0'; s[pos++] = '0'; } break;
                case '%': s[pos++] = '%'; break;
                default: s[pos++] = *src; break;
            }
        } else {
            s[pos++] = *src;
        }
        src++;
    }
    
    s[pos] = '\0';
    return pos;
}

static inline int nanosleep(const struct timespec *req, struct timespec *rem) {
    (void)req; 
    if (rem) { rem->tv_sec = 0; rem->tv_nsec = 0; }
    return 0;
}

/* C11+ functions */
static inline int timespec_get(struct timespec *ts, int base) {
    if (!ts || base != TIME_UTC) return 0;
    ts->tv_sec = WASM_EPOCH_TIME;
    ts->tv_nsec = 0;
    return TIME_UTC;
}

/* C23+ functions */
static inline int timespec_getres(struct timespec *ts, int base) {
    if (!ts || base != TIME_UTC) return 0;
    ts->tv_sec = 0;
    ts->tv_nsec = 1000000; /* 1ms resolution */
    return TIME_UTC;
}

static inline time_t timegm(struct tm *tm) {
    (void)tm;
    return WASM_EPOCH_TIME;
}

static char wasm_r_buffer[32];

static inline struct tm *gmtime_r(const time_t *timer, struct tm *result) {
    (void)timer;
    if (result) *result = wasm_static_tm;
    return result;
}

static inline struct tm *localtime_r(const time_t *timer, struct tm *result) {
    (void)timer;
    if (result) *result = wasm_static_tm;
    return result;
}

__JACL_DEPRECATED_MSG("Use strftime() instead")
static inline char *asctime_r(const struct tm *tm, char *buf) {
    (void)tm;
    if (buf) {
        const char *str = "Sat Jan 01 00:00:00 2022\n";
        for (int i = 0; i < 26; i++) buf[i] = str[i];
    }
    return buf;
}

__JACL_DEPRECATED_MSG("Use strftime() instead")
static inline char *ctime_r(const time_t *timer, char *buf) {
    (void)timer;
    return asctime_r(&wasm_static_tm, buf);
}

#else
/* ================================================================ */
/* Native Platforms - Real implementations                         */
/* ================================================================ */

/* Static storage for time functions */
static struct tm static_tm_storage;
static char time_string_buffer[32];

/* Helper functions */
static inline int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static inline int get_days_in_month(int month, int year) {
    static const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 1 && is_leap_year(year)) return 29;
    return days_in_month[month];
}

static inline void num_to_str(char *buf, int num, int width) {
    for (int i = width - 1; i >= 0; i--) {
        buf[i] = '0' + (num % 10);
        num /= 10;
    }
}

static inline void str_copy(char *dest, const char *src, int len) {
    for (int i = 0; i < len && src[i]; i++) {
        dest[i] = src[i];
    }
}

/* Basic time functions */
static inline clock_t clock(void) {
#if defined(_WIN32)
    __declspec(dllimport) unsigned long __stdcall GetTickCount(void);
    return (clock_t)GetTickCount() * (CLOCKS_PER_SEC / 1000);
#else
    static clock_t counter = 0;
    return ++counter * (CLOCKS_PER_SEC / 1000);
#endif
}

static inline time_t time(time_t *tloc) {
#if defined(_WIN32)
    __declspec(dllimport) void __stdcall GetSystemTimeAsFileTime(void* lpSystemTimeAsFileTime);
    union { long long ll; struct { unsigned long l, h; } s; } ft;
    GetSystemTimeAsFileTime(&ft);
    time_t result = (time_t)((ft.ll - 116444736000000000ULL) / 10000000ULL);
    if (tloc) *tloc = result;
    return result;
#else
    static time_t base_time = 1640995200; /* 2022-01-01 as fallback */
    static unsigned long counter = 0;
    time_t result = base_time + (counter++ / 1000000);
    if (tloc) *tloc = result;
    return result;
#endif
}

static inline double difftime(time_t time1, time_t time0) {
    return (double)(time1 - time0);
}

static inline time_t mktime(struct tm *tm) {
    if (!tm) return -1;
    
    int year = tm->tm_year + 1900;
    int month = tm->tm_mon;
    int day = tm->tm_mday;
    
    if (year < 1970 || month < 0 || month > 11 || day < 1 || day > 31) return -1;
    
    long days = 0;
    for (int y = 1970; y < year; y++) {
        days += is_leap_year(y) ? 366 : 365;
    }
    
    for (int m = 0; m < month; m++) {
        days += get_days_in_month(m, year);
    }
    
    days += day - 1;
    
    time_t result = days * 86400 + tm->tm_hour * 3600 + tm->tm_min * 60 + tm->tm_sec;
    
    tm->tm_wday = (days + 4) % 7;
    tm->tm_yday = 0;
    for (int m = 0; m < month; m++) {
        tm->tm_yday += get_days_in_month(m, year);
    }
    tm->tm_yday += day - 1;
    
    return result;
}

static inline struct tm *gmtime(const time_t *timer) {
    if (!timer) return NULL;
    
    time_t t = *timer;
    if (t < 0) return NULL;
    
    static_tm_storage.tm_sec = (int)(t % 60); t /= 60;
    static_tm_storage.tm_min = (int)(t % 60); t /= 60;
    static_tm_storage.tm_hour = (int)(t % 24); t /= 24;
    
    static_tm_storage.tm_wday = (int)((t + 4) % 7);
    
    int year = 1970;
    long days = t;
    while (1) {
        int days_this_year = is_leap_year(year) ? 366 : 365;
        if (days < days_this_year) break;
        days -= days_this_year;
        year++;
    }
    static_tm_storage.tm_year = year - 1900;
    static_tm_storage.tm_yday = (int)days;
    
    int month = 0;
    while (month < 12) {
        int days_this_month = get_days_in_month(month, year);
        if (days < days_this_month) break;
        days -= days_this_month;
        month++;
    }
    static_tm_storage.tm_mon = month;
    static_tm_storage.tm_mday = (int)days + 1;
    static_tm_storage.tm_isdst = 0;
    
    return &static_tm_storage;
}

static inline struct tm *localtime(const time_t *timer) {
    return gmtime(timer);
}

__JACL_DEPRECATED_MSG("Use strftime() instead")
static inline char *asctime(const struct tm *tm) {
    if (!tm) return NULL;
    
    static const char *day_names[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    static const char *month_names[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    
    int wday = (tm->tm_wday >= 0 && tm->tm_wday < 7) ? tm->tm_wday : 0;
    int mon = (tm->tm_mon >= 0 && tm->tm_mon < 12) ? tm->tm_mon : 0;
    int year = tm->tm_year + 1900;
    
    char *p = time_string_buffer;
    
    str_copy(p, day_names[wday], 3); p += 3; *p++ = ' ';
    str_copy(p, month_names[mon], 3); p += 3; *p++ = ' ';
    num_to_str(p, tm->tm_mday, 2); p += 2; *p++ = ' ';
    num_to_str(p, tm->tm_hour, 2); p += 2; *p++ = ':';
    num_to_str(p, tm->tm_min, 2); p += 2; *p++ = ':';
    num_to_str(p, tm->tm_sec, 2); p += 2; *p++ = ' ';
    num_to_str(p, year, 4); p += 4;
    *p++ = '\n'; *p = '\0';
    
    return time_string_buffer;
}

__JACL_DEPRECATED_MSG("Use strftime() instead")
static inline char *ctime(const time_t *timer) {
    struct tm *tm = localtime(timer);
    return tm ? asctime(tm) : NULL;
}

static inline size_t strftime(char *s, size_t max, const char *fmt, const struct tm *tm) {
    if (!s || !fmt || !tm || max == 0) return 0;
    
    size_t pos = 0;
    char temp[16];
    
    while (*fmt && pos < max - 1) {
        if (*fmt != '%') {
            s[pos++] = *fmt++;
            continue;
        }
        
        fmt++;
        switch (*fmt++) {
            case 'Y':
                num_to_str(temp, tm->tm_year + 1900, 4);
                for (int i = 0; i < 4 && pos < max - 1; i++) s[pos++] = temp[i];
                break;
            case 'm':
                num_to_str(temp, tm->tm_mon + 1, 2);
                for (int i = 0; i < 2 && pos < max - 1; i++) s[pos++] = temp[i];
                break;
            case 'd':
                num_to_str(temp, tm->tm_mday, 2);
                for (int i = 0; i < 2 && pos < max - 1; i++) s[pos++] = temp[i];
                break;
            case 'H':
                num_to_str(temp, tm->tm_hour, 2);
                for (int i = 0; i < 2 && pos < max - 1; i++) s[pos++] = temp[i];
                break;
            case 'M':
                num_to_str(temp, tm->tm_min, 2);
                for (int i = 0; i < 2 && pos < max - 1; i++) s[pos++] = temp[i];
                break;
            case 'S':
                num_to_str(temp, tm->tm_sec, 2);
                for (int i = 0; i < 2 && pos < max - 1; i++) s[pos++] = temp[i];
                break;
            case '%':
                s[pos++] = '%';
                break;
            default:
                if (pos < max - 1) s[pos++] = *(fmt-1);
                break;
        }
    }
    
    s[pos] = '\0';
    return pos;
}

static inline int nanosleep(const struct timespec *req, struct timespec *rem) {
    if (!req) return -1;
    
#if defined(_WIN32)
    __declspec(dllimport) void __stdcall Sleep(unsigned long dwMilliseconds);
    unsigned long ms = (unsigned long)(req->tv_sec * 1000 + req->tv_nsec / 1000000);
    if (ms == 0 && req->tv_nsec > 0) ms = 1;
    Sleep(ms);
    if (rem) { rem->tv_sec = 0; rem->tv_nsec = 0; }
    return 0;
#else
    volatile long iterations = req->tv_sec * 1000000000L + req->tv_nsec;
    while (iterations-- > 0) { /* Busy wait */ }
    if (rem) { rem->tv_sec = 0; rem->tv_nsec = 0; }
    return 0;
#endif
}

/* C11+ functions */
static inline int timespec_get(struct timespec *ts, int base) {
    if (!ts || base != TIME_UTC) return 0;
    
#if defined(_WIN32)
    __declspec(dllimport) void __stdcall GetSystemTimeAsFileTime(void*);
    union { long long ll; struct { unsigned long l, h; } s; } ft;
    GetSystemTimeAsFileTime(&ft);
    ts->tv_sec = (time_t)((ft.ll - 116444736000000000ULL) / 10000000ULL);
    ts->tv_nsec = (long)(((ft.ll - 116444736000000000ULL) % 10000000ULL) * 100);
    return TIME_UTC;
#else
    ts->tv_sec = time(NULL);
    ts->tv_nsec = 0;
    return TIME_UTC;
#endif
}

/* C23+ functions */
static inline int timespec_getres(struct timespec *ts, int base) {
    if (!ts || base != TIME_UTC) return 0;
    ts->tv_sec = 0;
    ts->tv_nsec = 1000000; /* 1ms resolution */
    return TIME_UTC;
}

static inline time_t timegm(struct tm *tm) {
    /* Same as mktime but assume UTC */
    return mktime(tm);
}

static inline struct tm *gmtime_r(const time_t *timer, struct tm *result) {
    if (!result) return NULL;
    struct tm *tmp = gmtime(timer);
    if (tmp) *result = *tmp;
    return tmp ? result : NULL;
}

static inline struct tm *localtime_r(const time_t *timer, struct tm *result) {
    if (!result) return NULL;
    struct tm *tmp = localtime(timer);
    if (tmp) *result = *tmp;
    return tmp ? result : NULL;
}

__JACL_DEPRECATED_MSG("Use strftime() instead")
static inline char *asctime_r(const struct tm *tm, char *buf) {
    if (!buf) return NULL;
    char *tmp = asctime(tm);
    if (tmp) {
        for (int i = 0; i < 26; i++) buf[i] = tmp[i];
        return buf;
    }
    return NULL;
}

__JACL_DEPRECATED_MSG("Use strftime() instead")
static inline char *ctime_r(const time_t *timer, char *buf) {
    if (!buf) return NULL;
    char *tmp = ctime(timer);
    if (tmp) {
        for (int i = 0; i < 26; i++) buf[i] = tmp[i];
        return buf;
    }
    return NULL;
}

#endif /* !__wasm__ */

#ifdef __cplusplus
}
#endif
#endif /* TIME_H */
