/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef FENV_H
#define FENV_H
#pragma once

#include <config.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

#if JACL_HAS_C99

/* Exception flags */
#define FE_INVALID		 (1u<<0)
#define FE_DIVBYZERO	 (1u<<1)
#define FE_OVERFLOW		 (1u<<2)
#define FE_UNDERFLOW	 (1u<<3)
#define FE_INEXACT		 (1u<<4)
#define FE_ALL_EXCEPT  (FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW|FE_UNDERFLOW|FE_INEXACT)

/* Rounding modes */
#define FE_TONEAREST	 0
#define FE_DOWNWARD		 1
#define FE_UPWARD			 2
#define FE_TOWARDZERO  3

typedef unsigned fexcept_t;
typedef struct { fexcept_t excepts; int round; } fenv_t;

/* Default environment object (C/C++ friendly) */
static const fenv_t __jacl_fenv_default = { 0u, FE_TONEAREST };
#define FE_DFL_ENV ((const fenv_t*)&__jacl_fenv_default)

/* Emulated environment storage (thread-local when available) */
#if JACL_HAS_PTHREADS
	    extern _Thread_local fenv_t __jacl_fenv;
#else
    extern fenv_t __jacl_fenv;  /* Global fallback for single-threaded */
#endif

/* Inline API functions */
static inline int feclearexcept(int e) {
	__jacl_fenv.excepts &= (fexcept_t)~(unsigned)e;
	return 0;
}

/* Store selected exception flags into *f */
static inline int fegetexceptflag(fexcept_t *f, int e) {
	if (f) *f = __jacl_fenv.excepts & (fexcept_t)e;
	return 0;
}

/* Raise (set) exception flags indicated by e */
static inline int feraiseexcept(int e) {
	__jacl_fenv.excepts |= (fexcept_t)e;
	return 0;
}

/* Set exception flags from *f, masked by e */
static inline int fesetexceptflag(const fexcept_t *f, int e) {
	if (f) {
		__jacl_fenv.excepts &= (fexcept_t)~(unsigned)e;
		__jacl_fenv.excepts |= (*f & (fexcept_t)e);
	}
	return 0;
}

/* Test which flags in e are currently set */
static inline int fetestexcept(int e) {
	return (int)(__jacl_fenv.excepts & (fexcept_t)e);
}

/* Get current rounding mode */
static inline int fegetround(void) {
	return __jacl_fenv.round;
}

/* Set rounding mode */
static inline int fesetround(int m) {
	if (m < FE_TONEAREST || m > FE_TOWARDZERO) return 1;
	__jacl_fenv.round = m;
	return 0;
}

/* Copy current env to *envp */
static inline int fegetenv(fenv_t *envp) {
	if (envp) *envp = __jacl_fenv;
	return 0;
}

/* Install environment (or default if envp==FE_DFL_ENV) */
static inline int fesetenv(const fenv_t *envp) {
	if (envp == FE_DFL_ENV || envp == (const fenv_t*)0) {
		__jacl_fenv.excepts = 0u;
		__jacl_fenv.round = FE_TONEAREST;
	} else {
		__jacl_fenv = *envp;
	}
	return 0;
}

/* Save env to *envp and clear exceptions */
static inline int feholdexcept(fenv_t *envp) {
	if (envp) *envp = __jacl_fenv;
	__jacl_fenv.excepts = 0u;
	return 0;
}

/* Merge saved env’s flags and set its rounding mode */
static inline int feupdateenv(const fenv_t *envp) {
	if (envp) {
		__jacl_fenv.round = envp->round;
		__jacl_fenv.excepts |= envp->excepts;
	}
	return 0;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* FENV_H */
