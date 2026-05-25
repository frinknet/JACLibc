/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef kvx
	#define JACL_ARCH kvx
	#define JACL_ARCH_KVX 1
	#define __jacl_arch_syscall __kvx_syscall
	#define __jacl_arch_tls_set __kvx_set_tp_register
	#define __jacl_arch_tls_get __kvx_get_tp_register
	#define __jacl_arch_clone_thread __kvx_clone_thread
	#define __jacl_arch_setjmp        __kvx_setjmp
	#define __jacl_arch_longjmp       __kvx_longjmp
	#define __jacl_arch_jmpbuf        __kvx_jmpbuf
	#define JACL_BITS 64
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __kvx_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		register long r10 __asm__("r10") = num;
		register long r0 __asm__("r0") = a1;
		register long r1 __asm__("r1") = a2;
		register long r2 __asm__("r2") = a3;
		register long r3 __asm__("r3") = a4;
		register long r4 __asm__("r4") = a5;
		register long r5 __asm__("r5") = a6;

		__asm__ volatile (
			"scall"
			: "+r"(r0)
			: "r"(r10), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5)
			: "memory"
		);

		return r0;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".text\n"
		".globl _start\n"
		"_start:\n"
		"make r13 = 0\n" /* Clear Frame Pointer */
		"make r0 = r12\n" /* Pass SP in r0 */
		"and r12 = r12, -16\n" /* Align SP */
		"call _start_main\n"
		";;\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"make r10 = 139\n" /* SYS_rt_sigreturn */
			"scall\n"
			";;\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __kvx_set_tp_register(void* addr) {
		__asm__ volatile("make r11 = %0" : : "r"(addr) : "memory");
	}

	static inline void* __kvx_get_tp_register(void) {
		void* result;
		__asm__("make %0 = r11" : "=r"(result));
		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE && JACL_OS_LINUX
	static inline pid_t __kvx_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~15UL) - 16;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		register long r0 __asm__("r0");

		__asm__ volatile(
			"make r10 = 220\n\t" /* SYS_clone */
			"make r0 = %1\n\t"   /* flags */
			"make r1 = %2\n\t"   /* stack_top */
			"make r2 = 0\n\t"
			"make r3 = 0\n\t"
			"make r4 = 0\n\t"
			"scall\n\t"
			";;\n\t"
			"cb.nez r0, 1f\n\t"  /* If parent (r0 != 0), jump to end */
			";;\n\t"

			/* Child Thread Context */
			"make r11 = 0\n\t"   /* Clear TP for child */
			"make r0 = %4\n\t"   /* arg */
			"call %3\n\t"        /* fn */
			";;\n\t"
			"make r10 = 93\n\t"  /* SYS_exit */
			"make r0 = 0\n\t"
			"scall\n\t"
			";;\n\t"

			"1:\n\t"
			: "=r"(r0)
			: "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "r1", "r2", "r3", "r4", "r10", "memory"
		);

		return (pid_t)r0;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __kvx_jmpbuf[24];

__asm__(
	".text\n"
	".weak __kvx_setjmp\n"
	".type __kvx_setjmp, @function\n"
	"__kvx_setjmp:\n"
	/* r0 is env pointer */
	"sq 0[r0]   = r12r13r14r15\n" /* SP, FP, LR, r15 */
	"sq 32[r0]  = r16r17r18r19\n"
	"sq 64[r0]  = r20r21r22r23\n"
	"sq 96[r0]  = r24r25r26r27\n"
	"sq 128[r0] = r28r29r30r31\n"
	";;\n"
	"make r0 = 0\n"
	"ret\n"
	";;\n"
	".size __kvx_setjmp, .-__kvx_setjmp\n"
);

__asm__(
	".text\n"
	".weak __kvx_longjmp\n"
	".type __kvx_longjmp, @function\n"
	"__kvx_longjmp:\n"
	/* r0 is env, r1 is val */
	"lq r12r13r14r15 = 0[r0]\n"
	"lq r16r17r18r19 = 32[r0]\n"
	"lq r20r21r22r23 = 64[r0]\n"
	"lq r24r25r26r27 = 96[r0]\n"
	"lq r28r29r30r31 = 128[r0]\n"
	";;\n"
	"make r0 = r1\n"
	";;\n"
	"cb.eqz r1, 1f\n"
	";;\n"
	"ret\n"
	";;\n"
	"1:\n"
	"make r0 = 1\n"
	"ret\n"
	";;\n"
	".size __kvx_longjmp, .-__kvx_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
