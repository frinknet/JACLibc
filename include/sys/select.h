/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _SYS_SELECT_H
#define _SYS_SELECT_H
#pragma once

/**
 * POSIX File Descriptor Set Multiplexing
 *
 * Defines fd_set, FD_* macros, and select()/pselect().
 * Required for non-blocking network I/O polling.
 */

#include <config.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <signal.h>   /* For sigset_t */
#include <errno.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FD_SETSIZE
#define FD_SETSIZE 1024
#endif

#ifndef NFDBITS
#define NFDBITS (sizeof(unsigned long) * 8)
#endif

typedef struct {
	unsigned long fds_bits[FD_SETSIZE / (sizeof(unsigned long) * 8)];
} fd_set;

#define FD_SET(n, p)   ((p)->fds_bits[(n) / (sizeof(unsigned long) * 8)] |= (1UL << ((n) % (sizeof(unsigned long) * 8))))
#define FD_CLR(n, p)   ((p)->fds_bits[(n) / (sizeof(unsigned long) * 8)] &= ~(1UL << ((n) % (sizeof(unsigned long) * 8))))
#define FD_ISSET(n, p) (((p)->fds_bits[(n) / (sizeof(unsigned long) * 8)] & (1UL << ((n) % (sizeof(unsigned long) * 8)))) != 0)
#define FD_ZERO(p)     (memset((p), 0, sizeof(*(p))))

#if JACL_HAS_POSIX
static inline int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, struct timeval *timeout) {
	return (int)syscall(SYS_select, nfds, readfds, writefds, errorfds, timeout);
}

static inline int pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *errorfds, const struct timespec *timeout, const sigset_t *sigmask) {
	return (int)syscall(SYS_pselect6, nfds, readfds, writefds, errorfds, timeout, sigmask);
}
#else
static inline int select(int n, fd_set *r, fd_set *w, fd_set *e, struct timeval *t) { (void)n;(void)r;(void)w;(void)e;(void)t; errno=ENOSYS; return -1; }
static inline int pselect(int n, fd_set *r, fd_set *w, fd_set *e, const struct timespec *t, const sigset_t *s) { (void)n;(void)r;(void)w;(void)e;(void)t;(void)s; errno=ENOSYS; return -1; }
#endif

#ifdef __cplusplus
}
#endif
#endif /* _SYS_SELECT_H */
