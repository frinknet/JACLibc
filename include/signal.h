/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _SIGNAL_H
#define _SIGNAL_H
#pragma once

#include <config.h>
#include <errno.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/syscall.h>

/* C23 version identification (C23 §7.14) */
#if JACL_HAS_C23
#define __STDC_VERSION_SIGNAL_H__ 202311L
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* — Core ISO C Types (C99+) — */
typedef int sig_atomic_t;
typedef void (*__sighandler_t)(int);

/* Implementation-defined signal handler type name */
#if defined(__GNUC__) || defined(__clang__) || defined(_GNU_SOURCE)
typedef __sighandler_t sig_t;	/* BSD/GNU extension */
#else
typedef void (*sig_t)(int);		/* Generic fallback */
#endif

/* — ISO C Signal Handler Constants (C99+) — */
#define SIG_DFL  ((__sighandler_t) 0)
#define SIG_IGN  ((__sighandler_t) 1)
#define SIG_ERR  ((__sighandler_t) -1)

/* — ISO C Standard Signals (C99 §7.14.3) — */
#define SIGABRT  6
#define SIGFPE   8
#define SIGILL   4
#define SIGINT   2
#define SIGSEGV  11
#define SIGTERM  15

/* — POSIX Extensions (when feature test macros are defined) — */
#if JACL_HAS_POSIX

typedef struct { unsigned long __bits[2]; } sigset_t;

/* Additional POSIX signals (Linux-ish layout) */
#define SIGHUP   1
#define SIGQUIT  3
#define SIGTRAP  5
#define SIGBUS   7
#define SIGKILL  9
#define SIGUSR1  10
#define SIGUSR2  12
#define SIGPIPE  13
#define SIGALRM  14
#define SIGCHLD  17
#define SIGCONT  18
#define SIGSTOP  19
#define SIGTSTP  20
#define SIGTTIN  21
#define SIGTTOU  22
#define SIGURG   23
#define SIGXCPU  24
#define SIGXFSZ  25

#define SIGRTMIN 34
#define SIGRTMAX 64

/* Signal mask operations */
#define SIG_BLOCK    0
#define SIG_UNBLOCK  1
#define SIG_SETMASK  2

/* Signal action flags */
#define SA_NOCLDSTOP 0x00000001
#define SA_NOCLDWAIT 0x00000002
#define SA_SIGINFO   0x00000004
#define SA_RESTART   0x10000000
#define SA_NODEFER   0x40000000
#define SA_RESETHAND 0x80000000

/* Signal information codes */
#define SI_USER    0
#define SI_QUEUE  -1
#define SI_TIMER  -2
#define SI_ASYNCIO -4
#define SI_MESGQ  -3

/* — Signal Value Union — */
union sigval {
	int   sival_int;
	void *sival_ptr;
};

/* — Signal Information Structure — */
typedef struct {
	int          si_signo;
	int          si_errno;
	int          si_code;
	pid_t        si_pid;
	uid_t        si_uid;
	void        *si_addr;
	int          si_status;
	long         si_band;
	union sigval si_value;
	int          si_int;
	void        *si_ptr;
	void        *si_addr_lsb;
	short        si_addr_bnd[2];
} siginfo_t;

/* — Signal Action Structure — */
struct sigaction {
	void      (*sa_handler)(int);
	void      (*sa_sigaction)(int, siginfo_t *, void *);
	sigset_t   sa_mask;
	int        sa_flags;
	void      (*sa_restorer)(void);
};

/* — Signal Event Structure — */
struct sigevent {
	int          sigev_notify;
	int          sigev_signo;
	union sigval sigev_value;
	void       (*sigev_notify_function)(union sigval);
	void        *sigev_notify_attributes;
};

/* Event notification types */
#define SIGEV_NONE   0
#define SIGEV_SIGNAL 1
#define SIGEV_THREAD 2

static inline int kill(pid_t pid, int sig) {
	return (int)syscall(SYS_kill, (long)pid, (long)sig);
}

static inline int sigemptyset(sigset_t *set) {
	if (!set) { errno = EINVAL; return -1; }

	set->__bits[0] = 0;
	set->__bits[1] = 0;

	return 0;
}

static inline int sigfillset(sigset_t *set) {
	if (!set) { errno = EINVAL; return -1; }

	set->__bits[0] = ~0UL;
	set->__bits[1] = ~0UL;

	return 0;
}

static inline int sigaddset(sigset_t *set, int signo) {
	if (!set || signo < 1 || signo > SIGRTMAX) { errno = EINVAL; return -1; }

	unsigned idx  = (unsigned)(signo - 1) / (8U * sizeof(unsigned long));
	unsigned bit  = (unsigned)(signo - 1) % (8U * sizeof(unsigned long));

	set->__bits[idx] |= (1UL << bit);

	return 0;
}

static inline int sigdelset(sigset_t *set, int signo) {
	if (!set || signo < 1 || signo > SIGRTMAX) { errno = EINVAL; return -1; }

	unsigned idx  = (unsigned)(signo - 1) / (8U * sizeof(unsigned long));
	unsigned bit  = (unsigned)(signo - 1) % (8U * sizeof(unsigned long));

	set->__bits[idx] &= ~(1UL << bit);

	return 0;
}

static inline int sigismember(const sigset_t *set, int signo) {
	if (!set || signo < 1 || signo > SIGRTMAX) { errno = EINVAL; return -1; }

	unsigned idx  = (unsigned)(signo - 1) / (8U * sizeof(unsigned long));
	unsigned bit  = (unsigned)(signo - 1) % (8U * sizeof(unsigned long));

	return (set->__bits[idx] & (1UL << bit)) != 0;
}


static inline int sigaction(int sig, const struct sigaction *restrict act, struct sigaction *restrict oact) {
	/* rt_sigaction uses a sigset size argument; Linux expects sizeof(sigset_t) */
	return  syscall(SYS_rt_sigaction, (long)sig, (long)act, (long)oact, (long)sizeof(sigset_t));
}
static inline int sigprocmask(int how, const sigset_t *restrict set, sigset_t *restrict oset) {
	return  syscall(SYS_rt_sigprocmask, (long)how, (long)set, (long)oset, (long)sizeof(sigset_t));
}
static inline int sigpending(sigset_t *set) {
	/* rt_sigpending has a sigset size argument too */
	return syscall(SYS_rt_sigpending, (long)set, (long)sizeof(sigset_t));
}

static inline int sigsuspend(const sigset_t *mask) {
	long r = syscall(SYS_rt_sigsuspend, (long)mask, (long)sizeof(sigset_t));

	return (int)r;
}


static inline int sigwaitinfo(const sigset_t *restrict set, siginfo_t *restrict info) {
	/* rt_sigtimedwait with NULL timeout behaves like sigwaitinfo */
	return syscall(SYS_rt_sigtimedwait, (long)set, (long)info, (long)NULL, (long)sizeof(sigset_t));
}

static inline int sigtimedwait(const sigset_t *restrict set, siginfo_t *restrict info, const struct timespec *restrict timeout) {
	return syscall(SYS_rt_sigtimedwait, (long)set, (long)info, (long)timeout, (long)sizeof(sigset_t));
}

static inline int sigqueue(pid_t pid, int sig, const union sigval value) {
	siginfo_t info;

	/* Zero it so we don't leak junk into the kernel ABI */
	for (size_t i = 0; i < sizeof(info); ++i) ((unsigned char *)&info)[i] = 0;

	info.si_signo      = sig;
	info.si_code       = SI_QUEUE;
	info.si_pid        = pid;
	info.si_value      = value;

	long r = syscall(SYS_rt_sigqueueinfo, (long)pid, (long)sig, (long)&info);

	return (int)r;
}

static inline int sigwait(const sigset_t *restrict set, int *restrict sig) {
	if (!set || !sig) { errno = EINVAL; return -1; }

	siginfo_t info;
	int r = sigtimedwait(set, &info, NULL);

	if (r > 0) {
		*sig = r;	/* POSIX: return value == signal number */

		return 0;
	}

	return -1;		/* errno already set by sigtimedwait */
}

static inline int raise(int sig) {
	pid_t pid = (pid_t)syscall(SYS_getpid);
	long r = syscall(SYS_kill, (long)pid, (long)sig);

	return (r == 0) ? 0 : -1;
}

#else  /* !JACL_HAS_POSIX */

typedef unsigned long sigset_t;

#endif /* JACL_HAS_POSIX */

/* — Core ISO C Functions (C99+) — */


/* Minimal signal() built on sigaction when POSIX is present. */
#if JACL_HAS_POSIX
static inline sig_t signal(int sig, sig_t func) {
	struct sigaction act, oact;

	act.sa_handler = func;

	sigemptyset(&act.sa_mask);

	act.sa_flags = 0;

	if (sigaction(sig, &act, &oact) < 0) return SIG_ERR;

	return oact.sa_handler;
}
#else
/* Fallback stub: no real signal handling available. */
static inline sig_t signal(int sig, sig_t func) {
	(void)sig;
	(void)func;

	errno = ENOSYS;

	return SIG_ERR;
}
#endif

/* — C11+ Thread Safety Extensions — */
#if JACL_HAS_C11 && !defined(__STDC_NO_THREADS__)
#define signal_atomic_store(obj, val) ((void)(*(obj) = (val)))
#define signal_atomic_load(obj)       (*(obj))
#endif

/* — C99+ inline signal set helpers — */
#if JACL_HAS_C99 && JACL_HAS_POSIX

static inline int __sigisemptyset(const sigset_t *set) { return set ? (set->__bits[0] == 0 && set->__bits[1] == 0) : 0; }
static inline void __sigemptyset_fast(sigset_t *set) { if (set) { set->__bits[0] = 0; set->__bits[1] = 0; } }
static inline void __sigfillset_fast(sigset_t *set) { if (set) { set->__bits[0] = ~0UL; set->__bits[1] = ~0UL; } }
static inline int __sigaddset_fast(sigset_t *set, int sig) { return sigaddset(set, sig); }
static inline int __sigdelset_fast(sigset_t *set, int sig) { return sigdelset(set, sig); }

#endif /* C99+ inline optimizations */

#ifdef __cplusplus
}
#endif

#endif /* _SIGNAL_H */
