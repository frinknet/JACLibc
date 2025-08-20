/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef SETJMP_H
#define SETJMP_H

#include <stddef.h>
#include <signal.h>
#include <jsio.h>

#ifndef __JB_SIZE
#  if defined(__x86_64__) || defined(_M_X64)
#		 define __JB_SIZE 8
#  else
#		 define __JB_SIZE 16
#  endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* C99 jmp_buf */
typedef struct {
		long		 __jmpbuf[__JB_SIZE];
		int			 __mask_was_saved;
		sigset_t __saved_mask;
} jmp_buf[1];

/* Signal‐aware variants via JS hooks */
#ifdef __wasm__
	#define setjmp(env) \
		((__builtin_setjmp(env) == 0) ? (js_pause(), 0) : 1)

	#define longjmp(env,val) \
		(__builtin_longjmp(env,val), (void)0)

	#define sigsetjmp(env, save) \
		((__builtin_setjmp(env) == 0)						 \
			? ((save) ? (js_pause(), 0) : 0)				\
			: 1)

	#define siglongjmp(env,val) \
		longjmp(env,val)
#endif

#ifdef __cplusplus
}
#endif

#endif /* SETJMP_H */
