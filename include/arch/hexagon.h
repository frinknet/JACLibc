/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef hexagon
	#define JACL_ARCH hexagon
	#define JACL_ARCH_HEXAGON 1
	#define __jacl_arch_syscall __hexagon_syscall
	#define __jacl_arch_tls_set __hexagon_set_tp_register
	#define __jacl_arch_tls_get __hexagon_get_tp_register
	#define __jacl_arch_clone_thread __hexagon_clone_thread
	#define __jacl_arch_setjmp        __hexagon_setjmp
	#define __jacl_arch_longjmp       __hexagon_longjmp
	#define __jacl_arch_jmpbuf        __hexagon_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __hexagon_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long r6 __asm__("r6") = num;
		register long r0 __asm__("r0") = a1;
		register long r1 __asm__("r1") = a2;
		register long r2 __asm__("r2") = a3;
		register long r3 __asm__("r3") = a4;
		register long r4 __asm__("r4") = a5;
		register long r5 __asm__("r5") = a6;

		__asm__ volatile ("trap0(#0)"
			: "=r"(r0)
			: "r"(r6), "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5)
			: "memory");

		result = r0;

		return result;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".globl _start\n"
		"_start:\n"
		"r29 = #0\n"
		"r0 = r29\n"
		"r29 = and(r29, #-16)\n"
		"call _start_main\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"r6 = #139\n"
			"trap0(#1)\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __hexagon_set_tp_register(void* addr) {
		__asm__ volatile("ugp = %0" : : "r"(addr) : "memory");
	}

	static inline void* __hexagon_get_tp_register(void) {
		void* result;

		__asm__("%0 = ugp" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __hexagon_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~7UL) - 8;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		register long r0 __asm__("r0");

		__asm__ volatile(
			"r0 = %1\n\t"
			"r1 = %2\n\t"
			"r2 = #0\n\t"
			"r3 = #0\n\t"
			"r4 = #0\n\t"
			"r6 = #220\n\t"
			"trap0(#1)\n\t"
			"p0 = cmp.eq(r0, #0)\n\t"
			"if (!p0) jump 1f\n\t"

			"r29 = #0\n\t"
			"r0 = %4\n\t"
			"callr %3\n\t"
			"r6 = #93\n\t"
			"trap0(#1)\n\t"

			"1:\n\t"
			: "=r"(r0)
			: "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "r1", "r2", "r3", "r4", "r6", "r29", "memory"
		);

		return (pid_t)r0;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __hexagon_jmpbuf[16];

__asm__(
	".text\n"
	".weak __hexagon_setjmp\n"
	".type __hexagon_setjmp, @function\n"
	"__hexagon_setjmp:\n"
	"memd(r0 + #0)  = r17:16\n"
	"memd(r0 + #8)  = r19:18\n"
	"memd(r0 + #16) = r21:20\n"
	"memd(r0 + #24) = r23:22\n"
	"memd(r0 + #32) = r25:24\n"
	"memd(r0 + #40) = r27:26\n"
	"memd(r0 + #48) = r31:29\n" /* Saves LR (r31) and FP (r29) */
	"r0 = #0\n"
	"jumpr r31\n"
	".size __hexagon_setjmp, .-__hexagon_setjmp\n"
);

__asm__(
	".text\n"
	".weak __hexagon_longjmp\n"
	".type __hexagon_longjmp, @function\n"
	"__hexagon_longjmp:\n"
	"r17:16 = memd(r0 + #0)\n"
	"r19:18 = memd(r0 + #8)\n"
	"r21:20 = memd(r0 + #16)\n"
	"r23:22 = memd(r0 + #24)\n"
	"r25:24 = memd(r0 + #32)\n"
	"r27:26 = memd(r0 + #40)\n"
	"r31:29 = memd(r0 + #48)\n"
	"p0 = cmp.eq(r1, #0)\n"
	"if (p0) r1 = #1\n"
	"r0 = r1\n"
	"jumpr r31\n"
	".size __hexagon_longjmp, .-__hexagon_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
