/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _SYSLOG_H
#define _SYSLOG_H

#pragma once

#include <config.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Priorities */
#define LOG_EMERG   0
#define LOG_ALERT   1
#define LOG_CRIT    2
#define LOG_ERR     3
#define LOG_WARNING 4
#define LOG_NOTICE  5
#define LOG_INFO    6
#define LOG_DEBUG   7

/* Facilities */
#define LOG_KERN     (0<<3)
#define LOG_USER     (1<<3)
#define LOG_MAIL     (2<<3)
#define LOG_DAEMON   (3<<3)
#define LOG_AUTH     (4<<3)
#define LOG_SYSLOG   (5<<3)
#define LOG_LPR      (6<<3)
#define LOG_NEWS     (7<<3)
#define LOG_UUCP     (8<<3)
#define LOG_CRON     (9<<3)
#define LOG_AUTHPRIV (10<<3)
#define LOG_FTP      (11<<3)
#define LOG_LOCAL0   (16<<3)
#define LOG_LOCAL1   (17<<3)
#define LOG_LOCAL2   (18<<3)
#define LOG_LOCAL3   (19<<3)
#define LOG_LOCAL4   (20<<3)
#define LOG_LOCAL5   (21<<3)
#define LOG_LOCAL6   (22<<3)
#define LOG_LOCAL7   (23<<3)

#define LOG_NFACILITIES 24
#define LOG_FACMASK    0x03f8
#define LOG_PRIMASK    0x0007

#define LOG_PRI(p)     ((p) & LOG_PRIMASK)
#define LOG_FAC(p)     ((p) & LOG_FACMASK)
#define LOG_MAKEPRI(fac, pri) ((fac) | (pri))

/* Mask Macros */
#define LOG_MASK(pri)  (1 << (pri))
#define LOG_UPTO(pri)  ((1 << ((pri)+1)) - 1)

/* Options */
#define LOG_PID      0x01
#define LOG_CONS     0x02
#define LOG_ODELAY   0x04
#define LOG_NDELAY   0x08
#define LOG_NOWAIT   0x10
#define LOG_PERROR   0x20

/* Internal State */
static int __syslog_fd = -1;
static int __syslog_mask = 0xFF;
static char __syslog_ident[64] = {0};
static int __syslog_option = 0;
static int __syslog_facility = LOG_USER;

static inline void __syslog_connect(void) {
    if (__syslog_fd != -1) return;
    
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/dev/log", sizeof(addr.sun_path) - 1);

    __syslog_fd = socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (__syslog_fd < 0) return;

    if (connect(__syslog_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(__syslog_fd);
        __syslog_fd = -1;
    }
}

static inline void openlog(const char *ident, int option, int facility) {
    if (ident) strncpy(__syslog_ident, ident, sizeof(__syslog_ident) - 1);
    __syslog_option = option;
    __syslog_facility = facility;
    
    if (!(option & LOG_ODELAY)) __syslog_connect();
}

static inline void closelog(void) {
    if (__syslog_fd != -1) {
        close(__syslog_fd);
        __syslog_fd = -1;
    }
}

static inline int setlogmask(int mask) {
    int old = __syslog_mask;
    __syslog_mask = mask;
    return old;
}

static inline void vsyslog(int priority, const char *format, va_list args) {
    if (!(__syslog_mask & LOG_MASK(LOG_PRI(priority)))) return;

    char buf[1024];
    char header[128];
    int len;
    
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    if (!tm) tm = gmtime(&now);
    
    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "%b %e %H:%M:%S", tm);
    
    char hostname[64] = "unknown";
    gethostname(hostname, sizeof(hostname) - 1);

    len = snprintf(header, sizeof(header), "<%d>%s %s %s[%d]: ",
                   priority | __syslog_facility,
                   timebuf,
                   hostname,
                   __syslog_ident[0] ? __syslog_ident : "root",
                   (__syslog_option & LOG_PID) ? getpid() : 0);

    if (len < 0 || len >= sizeof(header)) return;

    int msg_len = vsnprintf(buf + len, sizeof(buf) - len, format, args);
    if (msg_len < 0) return;
    
    int total_len = len + msg_len;
    if (total_len >= sizeof(buf)) total_len = sizeof(buf) - 1;
    buf[total_len] = '\0';

    if (__syslog_fd == -1) __syslog_connect();
    
    ssize_t sent = -1;
    if (__syslog_fd != -1) {
        sent = send(__syslog_fd, buf, total_len, MSG_NOSIGNAL);
    }

    // Fallback: LOG_CONS writes to /dev/console if socket fails
    if (sent < 0 && (__syslog_option & LOG_CONS)) {
        int cons_fd = open("/dev/console", O_WRONLY | O_NOCTTY | O_CLOEXEC);
        if (cons_fd >= 0) {
            write(cons_fd, buf + len, msg_len);
            write(cons_fd, "\n", 1);
            close(cons_fd);
        }
    }

    // Fallback: LOG_PERROR writes to stderr
    if (sent < 0 || (__syslog_option & LOG_PERROR)) {
        fprintf(stderr, "%s\n", buf + len);
    }
}

static inline void syslog(int priority, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsyslog(priority, format, args);
    va_end(args);
}

#ifdef __cplusplus
}
#endif

#endif /* _SYSLOG_H */
