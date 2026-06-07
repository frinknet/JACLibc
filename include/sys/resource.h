/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
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
/* Types                                                                        */
/* ============================================================================ */

typedef unsigned long rlim_t;

#ifndef __id_t_defined
typedef unsigned int id_t;
#define __id_t_defined
#endif

/* ============================================================================ */
/* Structures                                                                   */
/* ============================================================================ */

struct rlimit {
	rlim_t rlim_cur; /* Soft limit */
	rlim_t rlim_max; /* Hard limit */
};

/*
 * POSIX defines only ru_utime and ru_stime.
 * However, Linux/BSD syscalls expect the full BSD-style struct.
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

/* getpriority / setpriority which */
#define PRIO_PROCESS 0
#define PRIO_PGRP    1
#define PRIO_USER    2

/* rlimit constants */
#define RLIM_INFINITY  ((rlim_t)-1)
#define RLIM_SAVED_CUR RLIM_INFINITY
#define RLIM_SAVED_MAX RLIM_INFINITY

/* getrusage who */
#define RUSAGE_SELF     0
#define RUSAGE_CHILDREN (-1)
#define RUSAGE_BOTH     (-2) /* Linux extension */

/* getrlimit / setrlimit resource */
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

static inline int getpriority(int which, id_t who) {
	#if defined(JACL_HAS_POSIX)
		__errno_clr();

		int ret = (int)syscall(SYS_getpriority, which, who);

		if (ret == -1 && !__errno_chk(0)) return -1;

		#if JACL_OS_LINUX
			/* The Linux kernel returns (20 - nice) to avoid negative return values. */
			return 20 - ret;
		#else
			/* BSD and Darwin kernels return the actual nice value directly. */
			return ret;
		#endif
	#else
		(void)which; (void)who;

		return (__errno_set(ENOSYS), -1);
	#endif
}

static inline int setpriority(int which, id_t who, int prio) {
	#if defined(JACL_HAS_POSIX)
		return (int)syscall(SYS_setpriority, which, who, prio);
	#else
		(void)which; (void)who; (void)prio;

		return (__errno_set(ENOSYS), -1);
	#endif
}

static inline int getrlimit(int resource, struct rlimit *rlim) {
	if (!rlim) return (__errno_set(EFAULT), -1);

	#if defined(JACL_HAS_POSIX)
		return (int)syscall(SYS_getrlimit, resource, rlim);
	#else
		(void)resource;
		rlim->rlim_cur = RLIM_INFINITY;
		rlim->rlim_max = RLIM_INFINITY;
		return 0;
	#endif
}

static inline int setrlimit(int resource, const struct rlimit *rlim) {
	if (!rlim) return (__errno_set(EFAULT), -1);

	#if defined(JACL_HAS_POSIX)
		return (int)syscall(SYS_setrlimit, resource, rlim);
	#else
		(void)resource; (void)rlim;

		return (__errno_set(ENOSYS), -1);
	#endif
}

static inline int getrusage(int who, struct rusage *usage) {
	if (!usage) return (__errno_set(EFAULT), -1);

	#if defined(JACL_HAS_POSIX)
		return (int)syscall(SYS_getrusage, who, usage);
	#else
		(void)who;
		memset(usage, 0, sizeof(struct rusage));
		return 0;
	#endif
}

#ifdef __cplusplus
}
#endif

#endif /* _SYS_RESOURCE_H */
