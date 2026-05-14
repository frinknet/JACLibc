/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _SYS_RESOURCE_H
#define _SYS_RESOURCE_H

#pragma once

#include <config.h>
#include <sys/types.h>
#include <sys/time.h> /* for struct timeval */
#include <errno.h>
#include <string.h>

#ifdef JACL_HAS_POSIX
#include <sys/syscall.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================ */
/* Structures                                                                   */
/* ============================================================================ */

struct rlimit {
	unsigned long rlim_cur; /* Soft limit */
	unsigned long rlim_max; /* Hard limit */
};

/* 
 * POSIX defines only ru_utime and ru_stime.
 * However, Linux syscall expects the full BSD-style struct.
 * We define the full struct to prevent kernel stack smashing.
 */
struct rusage {
	struct timeval ru_utime; /* User time used */
	struct timeval ru_stime; /* System time used */
	long   ru_maxrss;        /* Maximum resident set size */
	long   ru_ixrss;         /* Integral shared memory size */
	long   ru_idrss;         /* Integral unshared data size */
	long   ru_isrss;         /* Integral unshared stack size */
	long   ru_minflt;        /* Page reclaims */
	long   ru_majflt;        /* Page faults */
	long   ru_nswap;         /* Swaps */
	long   ru_inblock;       /* Block input operations */
	long   ru_oublock;       /* Block output operations */
	long   ru_msgsnd;        /* Messages sent */
	long   ru_msgrcv;        /* Messages received */
	long   ru_nsignals;      /* Signals received */
	long   ru_nvcsw;         /* Voluntary context switches */
	long   ru_nivcsw;        /* Involuntary context switches */
};

/* ============================================================================ */
/* Constants                                                                    */
/* ============================================================================ */

#define RLIM_INFINITY ((unsigned long)-1)
#define RLIM_SAVED_CUR RLIM_INFINITY
#define RLIM_SAVED_MAX RLIM_INFINITY

#define RUSAGE_SELF     0
#define RUSAGE_CHILDREN (-1)
#define RUSAGE_BOTH     (-2) /* Linux extension */

#define RLIMIT_CPU      0
#define RLIMIT_FSIZE    1
#define RLIMIT_DATA     2
#define RLIMIT_STACK    3
#define RLIMIT_CORE     4
#define RLIMIT_RSS      5
#define RLIMIT_NPROC    6
#define RLIMIT_NOFILE   7
#define RLIMIT_MEMLOCK  8
#define RLIMIT_AS       9
#define RLIM_NLIMITS    10

/* ============================================================================ */
/* Functions                                                                    */
/* ============================================================================ */

static inline int getrlimit(int resource, struct rlimit *rlim) {
	if (!rlim) {
		errno = EFAULT;
		return -1;
	}

#if defined(JACL_HAS_POSIX)
	return (int)syscall(SYS_getrlimit, resource, rlim);
#elif JACL_OS_WINDOWS
	rlim->rlim_cur = RLIM_INFINITY;
	rlim->rlim_max = RLIM_INFINITY;
	return 0;
#else
	/* Bare Metal / Polyglot */
	rlim->rlim_cur = RLIM_INFINITY;
	rlim->rlim_max = RLIM_INFINITY;
	return 0;
#endif
}

static inline int setrlimit(int resource, const struct rlimit *rlim) {
	if (!rlim) {
		errno = EFAULT;
		return -1;
	}

#if defined(JACL_HAS_POSIX)
	return (int)syscall(SYS_setrlimit, resource, rlim);
#elif JACL_OS_WINDOWS
	errno = ENOSYS;
	return -1;
#else
	/* Bare Metal / Polyglot */
	return 0;
#endif
}

static inline int getrusage(int who, struct rusage *usage) {
	if (!usage) {
		errno = EFAULT;
		return -1;
	}

#if defined(JACL_HAS_POSIX)
	return (int)syscall(SYS_getrusage, who, usage);
#elif JACL_OS_WINDOWS
	memset(usage, 0, sizeof(struct rusage));
	return 0;
#else
	/* Bare Metal / Polyglot */
	memset(usage, 0, sizeof(struct rusage));
	return 0;
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* _SYS_RESOURCE_H */
