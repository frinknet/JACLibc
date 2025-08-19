/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef SETJMP_H
#define SETJMP_H

#include <stddef.h>
#include <signal.h>

/* Max register storage—tweak per‐ABI if needed */
#ifndef __JB_SIZE
#  if defined(__x86_64__) || defined(_M_X64)
#	 define __JB_SIZE 8
#  else
#	 define __JB_SIZE 16
#  endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* C99 jmp_buf */
typedef struct {
	long	 __jmpbuf[__JB_SIZE];		/* CPU registers */
	int		 __mask_was_saved;			/* did we save signal mask? */
	sigset_t __saved_mask;				/* saved signal mask */
} jmp_buf[1];

/* POSIX sigjmp_buf (same as jmp_buf) */
typedef jmp_buf sigjmp_buf[1];

/* setjmp/longjmp */
int setjmp(jmp_buf env) { return __builtin_setjmp(env); }
void longjmp(jmp_buf env, int val) { __builtin_longjmp(env, val); }

/* sigsetjmp/siglongjmp: if save==0, behave like setjmp */
int sigsetjmp(sigjmp_buf env, int save) {
	int r = __builtin_setjmp(env);

	if (save && r == 0) {
		env[0].__saved_mask = 0;
		sigprocmask(SIG_SETMASK, NULL, &env[0].__saved_mask);
		env[0].__mask_was_saved = 1;
	}

	return r;
}
void siglongjmp(sigjmp_buf env, int val) {
	if (env[0].__mask_was_saved) {
		sigprocmask(SIG_SETMASK, &env[0].__saved_mask, NULL);
	}

	__builtin_longjmp(env, val);
}

#ifdef __cplusplus
}
#endif

#endif /* SETJMP_H */ 
