/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef SETJMP_H
#define SETJMP_H
#pragma once

/**
 * NOTE: setjmp and friends are a hack that has been banned by most projects.
 * However, it still exists in the C Standard so we provide stubs that will
 * error out at compile time. This is a conscious choice to save you from
 * yourself and hours of debugging hell. You will probably thank us later.
 *
 * INSTEAD USE:
 *
 *  - error codes (set errno and return -1)
 *  - goto for cleanup (although that is still hard to maintain...)
 *  - or state machines (Ask perplexity if you don't know.)
 */

#include <stdnoreturn.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	unsigned long __opaque[8];
} jmp_buf[1];

typedef jmp_buf sigjmp_buf;

/**
 * If you insist on using these you'll need an external implementation
 */
extern int setjmp(jmp_buf env);
extern void longjmp(jmp_buf env, int val);
extern int sigsetjmp(sigjmp_buf env, int savemask);
extern void siglongjmp(sigjmp_buf env, int val);

#ifdef __cplusplus
}
#endif

#endif /* SETJMP_H */
