/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef SETJMP_H
#define SETJMP_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <signal.h>

#ifdef __wasm__
/* WebAssembly: No real setjmp/longjmp support */
typedef struct { int dummy; } jmp_buf[1];
typedef jmp_buf sigjmp_buf;

#define setjmp(env) 0
#define longjmp(env, val) __builtin_trap()
#define sigsetjmp(env, savemask) 0	
#define siglongjmp(env, val) __builtin_trap()

#else /* Native platforms */

/* Architecture-specific jmp_buf layouts */
#if defined(__x86_64__) || defined(_M_X64)
typedef struct {
	unsigned long __rip;	/* return address */
	unsigned long __rsp;	/* stack pointer */
	unsigned long __rbp;	/* frame pointer */
	unsigned long __rbx;	/* callee-saved registers */
	unsigned long __r12;
	unsigned long __r13;
	unsigned long __r14;
	unsigned long __r15;
	int __mask_was_saved;
	sigset_t __saved_mask;
} jmp_buf[1];

#elif defined(__i386__) || defined(_M_IX86)
typedef struct {
	unsigned int __eip;		/* return address */
	unsigned int __esp;		/* stack pointer */
	unsigned int __ebp;		/* frame pointer */
	unsigned int __ebx;		/* callee-saved registers */
	unsigned int __esi;
	unsigned int __edi;
	int __mask_was_saved;
	sigset_t __saved_mask;
} jmp_buf[1];

#elif defined(__aarch64__)
typedef struct {
	unsigned long __x19_x28[10]; /* x19-x28 callee-saved */
	unsigned long __lr;			 /* x30 link register */
	unsigned long __sp;			 /* x31 stack pointer */
	double __d8_d15[8];			 /* d8-d15 callee-saved FP */
	int __mask_was_saved;
	sigset_t __saved_mask;
} jmp_buf[1];

#elif defined(__riscv)
typedef struct {
	unsigned long __ra;		/* return address */
	unsigned long __sp;		/* stack pointer */
	unsigned long __s0_s11[12]; /* s0-s11 callee-saved */
	double __fs0_fs11[12];	/* fs0-fs11 callee-saved FP */
	int __mask_was_saved;
	sigset_t __saved_mask;
} jmp_buf[1];

#else
#error "setjmp/longjmp not implemented for this architecture"
#endif

typedef jmp_buf sigjmp_buf;

/* x86_64 implementation */
#if defined(__x86_64__) || defined(_M_X64)
static inline int setjmp(jmp_buf env) {
	int result;
	__asm__ volatile (
		"movq	 %%rsp, 8(%0)\n\t"		/* save stack pointer */
		"movq	 %%rbp, 16(%0)\n\t"		/* save frame pointer */
		"movq	 %%rbx, 24(%0)\n\t"		/* save callee-saved registers */
		"movq	 %%r12, 32(%0)\n\t"
		"movq	 %%r13, 40(%0)\n\t"
		"movq	 %%r14, 48(%0)\n\t"
		"movq	 %%r15, 56(%0)\n\t"
		"movq	 $1f, (%0)\n\t"			/* save return address */
		"xorl	 %%eax, %%eax\n\t"		/* return 0 first time */
		"jmp	 2f\n"
		"1:\n\t"						/* longjmp returns here */
		"movl	 $1, %%eax\n"			/* return 1 from longjmp */
		"2:"
		: "=m" (*env), "=a" (result)
		:
		: "memory"
	);
	return result;
}

static inline void longjmp(jmp_buf env, int val) {
	__asm__ volatile (
		"movq	 56(%0), %%r15\n\t"		/* restore registers */
		"movq	 48(%0), %%r14\n\t"
		"movq	 40(%0), %%r13\n\t"
		"movq	 32(%0), %%r12\n\t"
		"movq	 24(%0), %%rbx\n\t"
		"movq	 16(%0), %%rbp\n\t"
		"movq	 8(%0), %%rsp\n\t"		/* restore stack pointer */
		"testl	 %1, %1\n\t"			/* check if val == 0 */
		"jnz	 1f\n\t"
		"movl	 $1, %1\n"				/* if val == 0, make it 1 */
		"1:\n\t"
		"movl	 %1, %%eax\n\t"			/* set return value */
		"jmpq	 *(%0)"					/* jump to saved address */
		:
		: "r" (env), "r" (val)
		: "rax", "memory"
	);
	__builtin_unreachable();
}

/* i386 implementation */  
#elif defined(__i386__) || defined(_M_IX86)
static inline int setjmp(jmp_buf env) {
	int result;
	__asm__ volatile (
		"movl	 %%esp, 4(%0)\n\t"		/* save stack pointer */
		"movl	 %%ebp, 8(%0)\n\t"		/* save frame pointer */
		"movl	 %%ebx, 12(%0)\n\t"		/* save callee-saved registers */
		"movl	 %%esi, 16(%0)\n\t"
		"movl	 %%edi, 20(%0)\n\t"
		"movl	 $1f, (%0)\n\t"			/* save return address */
		"xorl	 %%eax, %%eax\n\t"		/* return 0 first time */
		"jmp	 2f\n"
		"1:\n\t"						/* longjmp returns here */
		"movl	 $1, %%eax\n"			/* return 1 from longjmp */
		"2:"
		: "=m" (*env), "=a" (result)
		:
		: "memory"
	);
	return result;
}

static inline void longjmp(jmp_buf env, int val) {
	__asm__ volatile (
		"movl	 20(%0), %%edi\n\t"		/* restore registers */
		"movl	 16(%0), %%esi\n\t"
		"movl	 12(%0), %%ebx\n\t"
		"movl	 8(%0), %%ebp\n\t"
		"movl	 4(%0), %%esp\n\t"		/* restore stack pointer */
		"testl	 %1, %1\n\t"			/* check if val == 0 */
		"jnz	 1f\n\t"
		"movl	 $1, %1\n"				/* if val == 0, make it 1 */
		"1:\n\t"
		"movl	 %1, %%eax\n\t"			/* set return value */
		"jmpl	 *(%0)"					/* jump to saved address */
		:
		: "r" (env), "r" (val)
		: "eax", "memory"
	);
	__builtin_unreachable();
}

/* AArch64 implementation */
#elif defined(__aarch64__)
static inline int setjmp(jmp_buf env) {
	int result;
	__asm__ volatile (
		"stp x19, x20, [%0, #0]\n\t"
		"stp x21, x22, [%0, #16]\n\t"
		"stp x23, x24, [%0, #32]\n\t"
		"stp x25, x26, [%0, #48]\n\t"
		"stp x27, x28, [%0, #64]\n\t"
		"str x30, [%0, #80]\n\t"		/* save link register */
		"mov x1, sp\n\t"
		"str x1, [%0, #88]\n\t"			/* save stack pointer */
		"stp d8,	d9,  [%0, #96]\n\t"	/* save FP registers */
		"stp d10, d11, [%0, #112]\n\t"
		"stp d12, d13, [%0, #128]\n\t"
		"stp d14, d15, [%0, #144]\n\t"
		"mov %w1, #0"					/* return 0 */
		: "=r" (env), "=r" (result)
		:
		: "x1", "memory"
	);
	return result;
}

static inline void longjmp(jmp_buf env, int val) {
	__asm__ volatile (
		"ldp x19, x20, [%0, #0]\n\t"
		"ldp x21, x22, [%0, #16]\n\t"
		"ldp x23, x24, [%0, #32]\n\t"
		"ldp x25, x26, [%0, #48]\n\t"
		"ldp x27, x28, [%0, #64]\n\t"
		"ldr x30, [%0, #80]\n\t"		/* restore link register */
		"ldr x2, [%0, #88]\n\t"
		"mov sp, x2\n\t"				/* restore stack pointer */
		"ldp d8,	d9,  [%0, #96]\n\t"	/* restore FP registers */
		"ldp d10, d11, [%0, #112]\n\t"
		"ldp d12, d13, [%0, #128]\n\t"
		"ldp d14, d15, [%0, #144]\n\t"
		"cmp %w1, #0\n\t"				/* check if val == 0 */
		"csinc w0, %w1, wzr, ne\n\t"	/* w0 = val != 0 ? val : 1 */
		"ret"							/* return via x30 */
		:
		: "r" (env), "r" (val)
		: "x0", "x2", "memory"
	);
	__builtin_unreachable();
}

/* RISC-V implementation */
#elif defined(__riscv)
static inline int setjmp(jmp_buf env) {
	__asm__ volatile (
		"sd ra,	0(%0)\n\t"
		"sd sp,	8(%0)\n\t"
		"sd s0,  16(%0)\n\t"
		"sd s1,  24(%0)\n\t"
		"sd s2,  32(%0)\n\t"
		"sd s3,  40(%0)\n\t"
		"sd s4,  48(%0)\n\t"
		"sd s5,  56(%0)\n\t"
		"sd s6,  64(%0)\n\t"
		"sd s7,  72(%0)\n\t"
		"sd s8,  80(%0)\n\t"
		"sd s9,  88(%0)\n\t"
		"sd s10, 96(%0)\n\t"
		"sd s11, 104(%0)\n\t"
		"li a0, 0"
		:
		: "r" (env)
		: "a0", "memory"
	);
	return 0;
}

static inline void longjmp(jmp_buf env, int val) {
	__asm__ volatile (
		"ld ra,	0(%0)\n\t"
		"ld sp,	8(%0)\n\t" 
		"ld s0,  16(%0)\n\t"
		"ld s1,  24(%0)\n\t"
		"ld s2,  32(%0)\n\t"
		"ld s3,  40(%0)\n\t"
		"ld s4,  48(%0)\n\t"
		"ld s5,  56(%0)\n\t"
		"ld s6,  64(%0)\n\t"
		"ld s7,  72(%0)\n\t"
		"ld s8,  80(%0)\n\t"
		"ld s9,  88(%0)\n\t"
		"ld s10, 96(%0)\n\t"
		"ld s11, 104(%0)\n\t"
		"seqz a0, %1\n\t"		 /* a0 = val == 0 ? 1 : 0 */
		"add a0, a0, %1\n\t"	 /* a0 = val == 0 ? 1 : val */
		"jr ra"
		:
		: "r" (env), "r" (val)
		: "a0", "memory"
	);
	__builtin_unreachable();
}

#endif

/* Signal versions (simplified - don't actually handle signals) */
#define sigsetjmp(env, savemask) setjmp(env)
#define siglongjmp(env, val) longjmp(env, val)

#endif /* !__wasm__ */

#ifdef __cplusplus
}
#endif
#endif /* SETJMP_H */

