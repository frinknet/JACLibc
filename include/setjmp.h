/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef SETJMP_H
#define SETJMP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <signal.h>

#ifdef __wasm__
typedef struct { int dummy; } __jb_t;
typedef __jb_t jmp_buf[1];

#define setjmp(env)				0
#define longjmp(env, val) __builtin_trap()
#define sigsetjmp(env,s)	0  
#define siglongjmp(env,v) __builtin_trap()

#else

/* Native platforms */
#if defined(__x86_64__) || defined(_M_X64)
		#define JB_COUNT 8
#else
		#define JB_COUNT 16
#endif

typedef struct {
		long		 __jmpbuf[JB_COUNT];
		int			 __mask_was_saved;
		sigset_t __saved_mask;
} __jb_t;
typedef __jb_t jmp_buf[1];

#define setjmp(env)				__builtin_setjmp(env)
#define longjmp(env, val) __builtin_longjmp(env, val)
#define sigsetjmp(env,s)	__builtin_sigsetjmp(env,s)
#define siglongjmp(env,v) __builtin_siglongjmp(env,v)
#endif

#ifdef __cplusplus
}
#endif

#endif /* SETJMP_H */
