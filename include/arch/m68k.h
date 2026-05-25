/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef m68k
	#define JACL_ARCH m68k
	#define JACL_ARCH_M68K 1
	#define __jacl_arch_syscall __m68k_syscall
	#define __jacl_arch_tls_set __m68k_set_tp_register
	#define __jacl_arch_tls_get __m68k_get_tp_register
	#define __jacl_arch_clone_thread __m68k_clone_thread
	#define __jacl_arch_setjmp        __m68k_setjmp
	#define __jacl_arch_longjmp       __m68k_longjmp
	#define __jacl_arch_jmpbuf        __m68k_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 4321
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __m68k_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long d0 __asm__("d0") = num;
		register long d1 __asm__("d1") = a1;
		register long d2 __asm__("d2") = a2;
		register long d3 __asm__("d3") = a3;
		register long d4 __asm__("d4") = a4;
		register long d5 __asm__("d5") = a5;
		register long a0 __asm__("a0") = a6;

		__asm__ volatile ("trap #0"
			: "=r"(d0)
			: "r"(d0), "r"(d1), "r"(d2), "r"(d3), "r"(d4), "r"(d5), "a"(a0)
			: "memory");

		result = d0;

		return result;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".globl _start\n"
		"_start:\n"
		"move.l #0, %fp\n"
		"move.l %sp, %d0\n"
		"and.l #-16, %sp\n"
		"move.l %d0, -(%sp)\n"
		"jsr _start_main\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"move.l #173, %d0\n"
			"trap #0\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __m68k_set_tp_register(void* addr) {
		__asm__ volatile("move.l %0, %%a5" : : "r"(addr) : "memory");
	}

	static inline void* __m68k_get_tp_register(void) {
		void* result;

		__asm__("move.l %%a5, %0" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE && JACL_OS_LINUX
	static inline pid_t __m68k_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~3UL) - 4;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;

		register long d0_sys __asm__("d0") = 120; /* SYS_clone */
		register long d1_sys __asm__("d1") = flags;
		register long d2_sys __asm__("d2") = (long)stack_top;
		register long d3_sys __asm__("d3") = 0;
		register long d4_sys __asm__("d4") = 0;
		register long d5_sys __asm__("d5") = 0;

		__asm__ volatile(
			"trap #0\n\t"
			"tst.l %%d0\n\t"
			"bne 1f\n\t"

			/* Child Runtime Trail */
			"suba.l %%a6, %%a6\n\t"
			"move.l %2, %%d0\n\t"
			"jsr (%1)\n\t"
			"move.l #1, %%d0\n\t"     /* SYS_exit */
			"trap #0\n\t"

			"1:\n\t"
			: "+r"(d0_sys)
			: "a"(fn), "g"(arg), "r"(d1_sys), "r"(d2_sys), "r"(d3_sys), "r"(d4_sys), "r"(d5_sys)
			: "a6", "memory"
		);

		return (pid_t)d0_sys;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __m68k_jmpbuf[32];

__asm__(
	".text\n"
	".weak __m68k_setjmp\n"
	".type __m68k_setjmp, @function\n"
	"__m68k_setjmp:\n"
	"movea.l 4(%sp), %a0\n"
	"movem.l %d2-%d7/%a2-%a7, (%a0)\n"
	"move.l (%sp), 48(%a0)\n"
#if !defined(__mcoldfire__) && !defined(__m68k_soft_float__)
	"fmovem.x %fp2-%fp7, 52(%a0)\n"
#endif
	"clr.l %d0\n"
	"rts\n"
	".size __m68k_setjmp, .-__m68k_setjmp\n"
);

__asm__(
	".text\n"
	".weak __m68k_longjmp\n"
	".type __m68k_longjmp, @function\n"
	"__m68k_longjmp:\n"
	"movea.l 4(%sp), %a0\n"
	"move.l 8(%sp), %d0\n"
	"bne 1f\n"
	"move.l #1, %d0\n"
	"1:\n"
	"movem.l (%a0), %d2-%d7/%a2-%a7\n"
#if !defined(__mcoldfire__) && !defined(__m68k_soft_float__)
	"fmovem.x 52(%a0), %fp2-%fp7\n"
#endif
	"move.l 48(%a0), (%sp)\n"
	"rts\n"
	".size __m68k_longjmp, .-__m68k_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
