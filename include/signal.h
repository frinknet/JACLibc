/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _SIGNAL_H
#define _SIGNAL_H
#pragma once

#include <config.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>

#if JACL_HAS_POSIX
#include <sys/syscall.h>
#endif

#if JACL_HAS_C23
#define __STDC_VERSION_SIGNAL_H__ 202311L
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ISO C Core */
typedef int sig_atomic_t;
typedef void (*__sighandler_t)(int);
typedef __sighandler_t sig_t;

#define SIG_DFL  ((__sighandler_t) 0)
#define SIG_IGN  ((__sighandler_t) 1)
#define SIG_ERR  ((__sighandler_t) -1)

/* Windows Backend */
#if JACL_OS_WINDOWS || JACL_ARCH_SPU

#define SIGABRT 22
#define SIGFPE  8
#define SIGILL  4
#define SIGINT  2
#define SIGSEGV 11
#define SIGTERM 15
#define NSIG 23

extern __sighandler_t signal(int sig, __sighandler_t func);
extern int raise(int sig);

typedef unsigned long sigset_t;

union sigval { int sival_int; void *sival_ptr; };
struct sigevent {
	int sigev_notify;
	int sigev_signo;
	union sigval sigev_value;
	void (*sigev_notify_function)(union sigval);
	pthread_attr_t *sigev_notify_attributes;
};

static inline int sigemptyset(sigset_t *set) { if(!set) return (__errno_set(EINVAL), -1); *set=0; return 0; }
static inline int sigfillset(sigset_t *set) { if(!set) return (__errno_set(EINVAL), -1); *set=~0UL; return 0; }
static inline int sigaddset(sigset_t *set, int sig) { (void)sig; if(!set) return (__errno_set(EINVAL), -1); return 0; }
static inline int sigdelset(sigset_t *set, int sig) { (void)sig; if(!set) return (__errno_set(EINVAL), -1); return 0; }
static inline int sigismember(const sigset_t *set, int sig) { (void)sig; if(!set) return (__errno_set(EINVAL), -1); return 0; }
static inline int kill(pid_t pid, int sig) { (void)pid; (void)sig; return (__errno_set(ENOSYS), -1); }

#else /* POSIX Systems (Linux, Darwin, BSD) */

/* POSIX Core Definitions */
#define SS_ONSTACK      1
#define SS_DISABLE      2
#define MINSIGSTKSZ     2048
#define SIGSTKSZ        8192
#define SIG2STR_MAX     40

#include JACL_X_SIGNALS
#define X_ENUM(name, val) name = (val),
enum { SIGNAL_NUMBERS(X_ENUM) NSIG };
enum { SIGNAL_ACTION_FLAGS(X_ENUM) };
enum { SIGNAL_MASK_OPS(X_ENUM) };
enum { SIGNAL_INFO_CODES(X_ENUM) };
enum { SIGNAL_ILL_CODES(X_ENUM) };
enum { SIGNAL_FPE_CODES(X_ENUM) };
enum { SIGNAL_SEGV_CODES(X_ENUM) };
enum { SIGNAL_BUS_CODES(X_ENUM) };
enum { SIGNAL_TRAP_CODES(X_ENUM) };
enum { SIGNAL_CLD_CODES(X_ENUM) };
enum { SIGNAL_POLL_CODES(X_ENUM) };
enum { SIGNAL_EVENT_TYPES(X_ENUM) };
#undef X_ENUM

#if JACL_OS_LINUX
	#if !(JACL_ARCH_RISCV32 || JACL_ARCH_RISCV64 || JACL_ARCH_LOONGARCH64)
		#define JACL_RESTORER 0x04000000
	#endif
	#define JACL_SIGSIZ 8
#endif

typedef struct { unsigned long __bits[128/sizeof(long)]; } sigset_t;
#define JACL_SIGSET_MAX 128

union sigval {
	int   sival_int;
	void *sival_ptr;
};

typedef struct {
	void  *ss_sp;
	size_t ss_size;
	int    ss_flags;
} stack_t;

/* OS-Specific ABI */
#if JACL_OS_LINUX
typedef struct {
	int si_signo;
	int si_errno;
	int si_code;
#if JACL_64BIT
	int __pad0;
#endif
	union {
		int _pad[(128 - (JACL_64BIT ? 16 : 12)) / sizeof(int)];
		struct { pid_t _pid; uid_t _uid; union sigval _sigval; } _rt;
		struct { void *_addr; short _addr_lsb; struct { void *_lower; void *_upper; } _addr_bnd; } _sigfault;
		struct { long _band; int _fd; } _sigpoll;
		struct { pid_t _pid; uid_t _uid; int _status; long _utime; long _stime; } _sigchld;
	} _sifields;
} siginfo_t;

#define si_pid     _sifields._rt._pid
#define si_uid     _sifields._rt._uid
#define si_value   _sifields._rt._sigval
#define si_addr    _sifields._sigfault._addr
#define si_status  _sifields._sigchld._status
#define si_band    _sifields._sigpoll._band
#define si_int     si_value.sival_int
#define si_ptr     si_value.sival_ptr

#else /* Darwin / BSD */
typedef struct {
	int si_signo;
	int si_errno;
	int si_code;
	pid_t si_pid;
	uid_t si_uid;
	int si_status;
	void *si_addr;
	union sigval si_value;
	long si_band;
	unsigned long __pad[7];
} siginfo_t;
#endif

/* OS-Specific ABI */
#if JACL_OS_LINUX
	struct sigaction {
		union {
			void (*_sa_handler)(int);
			void (*_sa_sigaction)(int, siginfo_t *, void *);
		} _u;
		unsigned long sa_flags;
		void (*sa_restorer)(void);
		sigset_t sa_mask;
	};
	#define sa_handler _u._sa_handler
	#define sa_sigaction _u._sa_sigaction
#elif JACL_OS_DARWIN
	struct sigaction {
		union {
			void (*_sa_handler)(int);
			void (*_sa_sigaction)(int, siginfo_t *, void *);
		} _u;
		void (*sa_tramp)(void *, int, int, siginfo_t *, void *);
		sigset_t sa_mask;
		int sa_flags;
	};
	#define sa_handler _u._sa_handler
	#define sa_sigaction _u._sa_sigaction
#elif JACL_OS_BSD
	struct sigaction {
		union {
			void (*_sa_handler)(int);
			void (*_sa_sigaction)(int, siginfo_t *, void *);
		} _u;
		int sa_flags;
		sigset_t sa_mask;
	};
	#define sa_handler _u._sa_handler
	#define sa_sigaction _u._sa_sigaction
#endif

struct sigevent {
    int          sigev_notify;
    int          sigev_signo;
    union sigval sigev_value;
    void       (*sigev_notify_function)(union sigval);
    pthread_attr_t *sigev_notify_attributes;
};

/* POSIX Syscall Wrappers */
static inline int kill(pid_t pid, int sig) {
	return (int)syscall(SYS_kill, pid, sig);
}

static inline int __sigset_op(sigset_t *set, unsigned long val) {
	if (!set) return (__errno_set(EINVAL), -1);

	size_t limit = sizeof(set->__bits) / sizeof(unsigned long);

	for (size_t i = 0; i < limit; i++) set->__bits[i] = val;

	return 0;
}

static inline int sigemptyset(sigset_t *set) { return __sigset_op(set, 0); }
static inline int sigfillset(sigset_t *set) { return __sigset_op(set, ~0UL); }
static inline int sigaddset(sigset_t *set, int signo) {
	if (!set || signo < 1 || signo > JACL_SIGSET_MAX) return (__errno_set(EINVAL), -1);

	unsigned idx = (unsigned)(signo - 1) / (8U * sizeof(unsigned long));
	unsigned bit = (unsigned)(signo - 1) % (8U * sizeof(unsigned long));

	set->__bits[idx] |= (1UL << bit);

	return 0;
}
static inline int sigdelset(sigset_t *set, int signo) {
	if (!set || signo < 1 || signo > JACL_SIGSET_MAX) return (__errno_set(EINVAL), -1);

	unsigned idx = (unsigned)(signo - 1) / (8U * sizeof(unsigned long));
	unsigned bit = (unsigned)(signo - 1) % (8U * sizeof(unsigned long));

	set->__bits[idx] &= ~(1UL << bit);

	return 0;
}
static inline int sigismember(const sigset_t *set, int signo) {
	if (!set || signo < 1 || signo > JACL_SIGSET_MAX) return (__errno_set(EINVAL), -1);

	unsigned idx = (unsigned)(signo - 1) / (8U * sizeof(unsigned long));
	unsigned bit = (unsigned)(signo - 1) % (8U * sizeof(unsigned long));

	return (set->__bits[idx] & (1UL << bit)) != 0;
}

#if JACL_RESTORER
/* Linux requires signal return trampoline - defined in arch files */
extern void __restore_rt(void);
#endif

static inline int sigaction(int sig, const struct sigaction *restrict act, struct sigaction *restrict oact) {
	#if JACL_OS_LINUX
		struct sigaction k_act;
		const struct sigaction *k_ptr = act;

		if (act) {
			k_act = *act;

			#ifdef JACL_RESTORER
				if (!(k_act.sa_flags & JACL_RESTORER)) {
					k_act.sa_flags |= JACL_RESTORER;
					k_act.sa_restorer = &__restore_rt;
				}
			#endif

			k_ptr = &k_act;
		}

		return syscall(SYS_rt_sigaction, sig, k_ptr, oact, JACL_SIGSIZ);
	#else
		return syscall(SYS_sigaction, sig, act, oact);
	#endif
}

static inline int sigprocmask(int how, const sigset_t *restrict set, sigset_t *restrict old) {
	if ((unsigned)how > 2U) return (__errno_set(EINVAL), -1);

	#if JACL_OS_LINUX
		return (int)syscall(SYS_rt_sigprocmask, how, set, old, JACL_SIGSIZ);
	#else
		return (int)syscall(SYS_sigprocmask, how, set, old);
	#endif
}

static inline int sigpending(sigset_t *set) {
	if (!set) return (__errno_set(EINVAL), -1);

	#if JACL_OS_LINUX
		return (int)syscall(SYS_rt_sigpending, set, JACL_SIGSIZ);
	#else
		return (int)syscall(SYS_sigpending, set);
	#endif
}

static inline int sigsuspend(const sigset_t *mask) {
	if (!mask) return (__errno_set(EINVAL), -1);

	#if JACL_OS_LINUX
		return (int)syscall(SYS_rt_sigsuspend, mask, JACL_SIGSIZ);
	#else
		return (int)syscall(SYS_sigsuspend, mask);
	#endif
}

static inline int sigqueue(pid_t pid, int sig, const union sigval value) {
	siginfo_t info;

	for (size_t i = 0; i < sizeof(info); ++i) ((unsigned char *)&info)[i] = 0;

	info.si_signo = sig;
	info.si_code = SI_QUEUE;
	info.si_pid = (pid_t)syscall(SYS_getpid); /* <-- SENDER'S PID */
	info.si_value = value;

	#if JACL_OS_LINUX
		return (int)syscall(SYS_rt_sigqueueinfo, pid, sig, &info);
	#elif JACL_OS_BSD
		return (int)syscall(SYS_sigqueue, pid, sig, &info);
	#else
		return (__errno_set(ENOSYS), -1);
	#endif
}

static inline int raise(int sig) {
	pid_t pid = (pid_t)syscall(SYS_getpid);

	long r = syscall(SYS_kill, pid, sig);

	return (r == 0) ? 0 : -1;
}

/* Signal Restorer & signal() Implementation */

static inline sig_t signal(int sig, sig_t func) {
	struct sigaction act, oact;

	act.sa_handler = func;

	sigemptyset(&act.sa_mask);

	act.sa_flags = SA_RESTART;

	#if JACL_OS_LINUX && defined(JACL_RESTORER)
		act.sa_flags |= JACL_RESTORER;
		act.sa_restorer = &__restore_rt;
	#endif

	if (sigaction(sig, &act, &oact) < 0) return SIG_ERR;

	return oact.sa_handler;
}

/* POSIX Extensions (C99+) */

#if JACL_HAS_C99

static inline int sigisemptyset(const sigset_t *set) {
	if (!set) return (__errno_set(EINVAL), -1); /* <--- THE TRAP */

	for (size_t i = 0; i < sizeof(*set) / sizeof(unsigned long); i++) {
		if (set->__bits[i]) return 0;
	}

	return 1;
}

static inline int sigaltstack(const stack_t *restrict ss, stack_t *restrict oss) {
	if (!ss && !oss) return (__errno_set(EINVAL), -1);
	if (ss && (ss->ss_flags & ~SS_DISABLE)) return (__errno_set(EINVAL), -1);

	#if JACL_OS_LINUX && JACL_64BIT && JACL_HAS_LE
		/*
		 * Linux Kernel ABI for LE 64-bit:
		 * struct { void *ss_sp; int ss_flags; size_t ss_size; }
		 *
		 * POSIX/glibc ABI:
		 * struct { void *ss_sp; size_t ss_size; int ss_flags; }
		 */

		/* Compile-time safety check for struct layout */
		static_assert(sizeof(struct { void *sp; int flags; size_t size; }) == sizeof(stack_t),
			"Kernel stack_t layout mismatch");

		struct {
			void *k_ss_sp;
			int k_ss_flags;
			size_t k_ss_size;
		} k_ss, k_oss;

		if (ss) {
			k_ss.k_ss_sp = ss->ss_sp;
			k_ss.k_ss_flags = ss->ss_flags;
			k_ss.k_ss_size = ss->ss_size;
		}

		long r = syscall(SYS_sigaltstack, ss ? &k_ss : NULL, oss ? &k_oss : NULL);

		if (r == 0 && oss) {
			oss->ss_sp = k_oss.k_ss_sp;
			oss->ss_flags = k_oss.k_ss_flags;
			oss->ss_size = k_oss.k_ss_size;
		}

		return (int)r;
	#else
		/* On BE, 32-bit, or non-Linux, layouts typically match or are handled by kernel compat */
		return (int)syscall(SYS_sigaltstack, (uintptr_t)ss, (uintptr_t)oss);
	#endif
}

static inline int sigtimedwait(const sigset_t *restrict set, siginfo_t *restrict info, const struct timespec *restrict timeout) {
	if (!set || sigisemptyset(set)) return (__errno_set(EINVAL), -1);

	/* Strict timeout validation to prevent kernel hangs */
	if (timeout) {
		if (timeout->tv_sec < 0 || timeout->tv_nsec < 0 || timeout->tv_nsec >= 1000000000L) {
			return (__errno_set(EINVAL), -1);
		}
	}

	#if JACL_OS_LINUX
		return syscall(SYS_rt_sigtimedwait, set, info, timeout, JACL_SIGSIZ);
	#elif JACL_OS_BSD
		return syscall(SYS_sigtimedwait, set, info, timeout);
	#else
		return (__errno_set(ENOSYS), -1);
	#endif
}

static inline int sigwaitinfo(const sigset_t *restrict set, siginfo_t *restrict info) {
	if (!set || sigisemptyset(set)) return (__errno_set(EINVAL), -1);

	/* Sanitize: Check if any signal in 'set' is currently blocked */
	sigset_t current_mask;

	if (sigprocmask(SIG_BLOCK, NULL, &current_mask) != 0) return -1;

	int has_blocked = 0;

	for (int i = 1; i <= NSIG; i++) {
		if (sigismember(set, i) && sigismember(&current_mask, i)) {
			has_blocked = 1;

			break;
		}
	}

	if (!has_blocked) return (__errno_set(EINVAL), -1);

	#if JACL_OS_LINUX
		return syscall(SYS_rt_sigtimedwait, set, info, NULL, JACL_SIGSIZ);
	#elif JACL_OS_BSD
		return syscall(SYS_sigwaitinfo, set, info);
	#else
		return (__errno_set(ENOSYS), -1);
	#endif
}

static inline int sigwait(const sigset_t *restrict set, int *restrict sig) {
	if (!set || !sig || sigisemptyset(set)) return __errno_set(EINVAL);

	/* Fast-path: Check intersection using bitwise AND on the internal bits */
	sigset_t current_mask;

	if (sigprocmask(SIG_BLOCK, NULL, &current_mask) != 0) return errno;

	int has_blocked = 0;

	for (size_t i = 0; i < sizeof(set->__bits) / sizeof(unsigned long); i++) {
		if (set->__bits[i] & current_mask.__bits[i]) {
			has_blocked = 1;

			break;
		}
	}

	if (!has_blocked) return __errno_set(EINVAL);

	siginfo_t info;

	int r = sigtimedwait(set, &info, NULL);

	if (r > 0) {
		*sig = r;

		return 0;
	}

	return errno;
}

/* ======================================================================== */
/* POSIX String Conversion Implementations                                  */
/* ======================================================================== */

static inline int sig2str(int signo, char *buf) {
	if (!buf || signo < 1 || signo > NSIG) return (__errno_set(EINVAL), -1);

	const char *result_name = NULL;

	#define X_SIG2STR(name, val) case val: result_name = #name; break;
	switch (signo) {
		SIGNAL_NUMBERS(X_SIG2STR)
		default: break;
	}
	#undef X_SIG2STR

	if (!result_name) return (__errno_set(EINVAL), -1);

	/* Manual copy to avoid <string.h> dependency */
	int i = 0;
	while (result_name[i] != '\0') {
		if (i >= SIG2STR_MAX - 1) return (__errno_set(ERANGE), -1);
		buf[i] = result_name[i];
		i++;
	}
	buf[i] = '\0';
	return 0;
}

static inline int str2sig(const char *name, int *sigp) {
	if (!name || !sigp) return (__errno_set(EINVAL), -1);

	/* Skip "SIG" prefix if present */
	if (name[0] == 'S' && name[1] == 'I' && name[2] == 'G') {
		name += 3;
	}

	int found = 0;
	int result = 0;

	/* Generate a string comparison loop from our X-Macro enum */
	#define X_STR2SIG(enum_name, val) \
		{ \
			const char *cmp_name = #enum_name; \
			if (cmp_name[0] == 'S' && cmp_name[1] == 'I' && cmp_name[2] == 'G') cmp_name += 3; \
			int match = 1; \
			int j = 0; \
			while (name[j] != '\0' && cmp_name[j] != '\0') { \
				if (name[j] != cmp_name[j]) { match = 0; break; } \
				j++; \
			} \
			if (match && name[j] == '\0' && cmp_name[j] == '\0') { \
				result = val; \
				found = 1; \
			} \
		}

	SIGNAL_NUMBERS(X_STR2SIG)
	#undef X_STR2SIG

	if (!found) return (__errno_set(EINVAL), -1);

	*sigp = result;
	return 0;
}

static inline void psignal(int signum, const char *s) {
	char buf[SIG2STR_MAX];
	const char *name = "Unknown";

	/* Try to get the name; if it fails, we stick with "Unknown" */
	if (signum > 0 && signum <= NSIG) {
		if (sig2str(signum, buf) == 0) name = buf;
	}

	if (s && *s) fprintf(stderr, "%s: %s\n", s, name);
	else fprintf(stderr, "%s\n", name);
}

static inline void psiginfo(const siginfo_t *pinfo, const char *s) {
	if (!pinfo) return;

	if (s && *s) fprintf(stderr, "%s: ", s);

	/* Print basic siginfo details */
	fprintf(stderr, "Signal %d, code %d", pinfo->si_signo, pinfo->si_code);

	/* Optionally print pid/uid if it's a user/queue signal */
	if (pinfo->si_code == SI_USER || pinfo->si_code == SI_QUEUE) fprintf(stderr, ", from pid %d, uid %d", pinfo->si_pid, pinfo->si_uid);

	fprintf(stderr, "\n");
}
#endif /* JACL_HAS_C99 */

static inline int killpg(pid_t pgrp, int sig) { return kill(-pgrp, sig); }

#endif /* JACL_OS_WINDOWS / POSIX split */



/* ======================================================================== */
/* C11 Thread Safety Extensions                                             */
/* ======================================================================== */
#if JACL_HAS_C11 && !defined(__STDC_NO_THREADS__)
#define signal_atomic_store(obj, val) ((void)(*(obj) = (val)))
#define signal_atomic_load(obj)       (*(obj))
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SIGNAL_H */
