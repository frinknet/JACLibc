/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _SETJMP_H
#define _SETJMP_H
#pragma once

/**
 * AVOID IF POSSIBLE - This is probably not the right tool for you...
 *
 *   - It bypasses normal control flow, making code unreadable spaghetti
 *   - It breaks stack unwinding (C++ destructors not called)
 *   - Any non-volatile variable modified between setjmp/longjmp is Undefined Behavior
 *   - Debugging crashes from longjmp corruption takes hours of forensics
 *
 * If you can use ANYTHING else you should DO THAT INSTEAD. But thee are legitimate
 * cases where we still need to use it. Here is when, where and why:
 *
 * SAFE CASES:
 *
 * 1. Deep error exit from library callback (no cleanup needed)
 * 2. Coroutine/yield implementation (explicit stack switch)
 * 3. Signal handler async jump to main thread
 * 4. State machine resumption across function boundaries
 *
 * UNSAFE PATTERNS:
 *
 * 1. Breaking out of nested loops (use labels + break instead)
 * 2. Bypassing RAII/resource management (call close() explicitly)
 * 3. Jumping into/out of auto-allocated structs (stack frame invalid)
 *
 * While it is wise to avoid this, you are the developer and this is your jam.
 */

#include <config.h>
#include <stdnoreturn.h>
#include <stddef.h>
#include <stdint.h>
#include <signal.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __ARCH_JUMP
#include JACL_ARCH_FILE

typedef struct {
	__jacl_arch_jmpbuf buf;
	unsigned long saved;
	sigset_t sigset;
} jmp_buf[1];

typedef jmp_buf sigjmp_buf;

extern int __jacl_arch_setjmp(jmp_buf env);
extern noreturn void __jacl_arch_longjmp(jmp_buf env, int val);

/* ============================================================================ */
/* Public API                                                                   */
/* ============================================================================ */

#define setjmp(env) __jacl_arch_setjmp((env))
#define longjmp(env, val) do { \
	__jacl_arch_longjmp((env), (val)); \
	unreachable(); \
} while(0)

#define sigsetjmp(env, savemask) ( \
	(env)->saved = (savemask), \
	(savemask) ? sigprocmask(SIG_SETMASK, NULL, &(env)->sigset) : 0, \
	__jacl_arch_setjmp(env) \
)

#define siglongjmp(env, val) do { \
	if ((env)->saved) sigprocmask(SIG_SETMASK, &(env)->sigset, NULL); \
	__jacl_arch_longjmp((env), (val)); \
	unreachable(); \
} while(0)


#ifdef __cplusplus
}
#endif

#endif /* _SETJMP_H */
