/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef s390x
	#define JACL_ARCH s390x
	#define JACL_ARCH_S390X 1
	#define __jacl_arch_syscall __s390x_syscall
	#define __jacl_arch_tls_set __s390x_set_tp_register
	#define __jacl_arch_tls_get __s390x_get_tp_register
	#define __jacl_arch_clone_thread __s390x_clone_thread
	#define __jacl_arch_setjmp        __s390x_setjmp
	#define __jacl_arch_longjmp       __s390x_longjmp
	#define __jacl_arch_jmpbuf        __s390x_jmpbuf
	#define JACL_BITS 64
	#define JACL_ORDER 4321
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __s390x_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long r1 __asm__("1") = num;
		register long r2 __asm__("2") = a1;
		register long r3 __asm__("3") = a2;
		register long r4 __asm__("4") = a3;
		register long r5 __asm__("5") = a4;
		register long r6 __asm__("6") = a5;
		register long r7 __asm__("7") = a6;

		__asm__ volatile ("svc 0"
			: "=r"(r2)
			: "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5), "r"(r6), "r"(r7)
			: "memory");

		result = r2;

		return result;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".globl _start\n"
		"_start:\n"
		"lghi %r11, 0\n"
		"lgr %r2, %r15\n"
		"aghi %r15, -160\n"
		"nill %r15, 0xfff0\n"
		"brasl %r14, _start_main\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"lghi %r1, 173\n"
			"svc 0\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __s390x_set_tp_register(void* addr) {
		__asm__ volatile("lgr %%r0, %0; ear %%r0, %%a0; sar %%a0, %%r0" : : "r"(addr) : "memory", "r0");
	}

	static inline void* __s390x_get_tp_register(void) {
		void* result;

		__asm__("ear %0, %%a0" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __s390x_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~7UL) - 160;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		register long r2 __asm__("2");

		__asm__ volatile(
			"lgr %%r2, %1\n\t"
			"lgr %%r3, %2\n\t"
			"lghi %%r4, 0\n\t"
			"lghi %%r5, 0\n\t"
			"lghi %%r6, 0\n\t"
			"svc 120\n\t"
			"ltgr %%r2, %%r2\n\t"
			"jne 1f\n\t"

			"lghi %%r11, 0\n\t"
			"lgr %%r2, %4\n\t"
			"basr %%r14, %3\n\t"
			"svc 1\n\t"

			"1:\n\t"
			: "=r"(r2)
			: "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "r3", "r4", "r5", "r6", "memory"
		);

		return (pid_t)r2;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __s390x_jmpbuf[18];

__asm__(
	".text\n"
	".weak __s390x_setjmp\n"
	".type __s390x_setjmp, @function\n"
	"__s390x_setjmp:\n"
	"stmg %r6, %r15, 0(%r2)\n"
	"std %f8, 10*8(%r2)\n"
	"std %f9, 11*8(%r2)\n"
	"std %f10, 12*8(%r2)\n"
	"std %f11, 13*8(%r2)\n"
	"std %f12, 14*8(%r2)\n"
	"std %f13, 15*8(%r2)\n"
	"std %f14, 16*8(%r2)\n"
	"std %f15, 17*8(%r2)\n"
	"lghi %r2, 0\n"
	"br %r14\n"
	".size __s390x_setjmp, .-__s390x_setjmp\n"
);

__asm__(
	".text\n"
	".weak __s390x_longjmp\n"
	".type __s390x_longjmp, @function\n"
	"__s390x_longjmp:\n"
	"lmg %r6, %r15, 0(%r2)\n"
	"ld %f8, 10*8(%r2)\n"
	"ld %f9, 11*8(%r2)\n"
	"ld %f10, 12*8(%r2)\n"
	"ld %f11, 13*8(%r2)\n"
	"ld %f12, 14*8(%r2)\n"
	"ld %f13, 15*8(%r2)\n"
	"ld %f14, 16*8(%r2)\n"
	"ld %f15, 17*8(%r2)\n"
	"ltgr %r2, %r3\n"
	"bnzr %r14\n"
	"lhi %r2, 1\n"
	"br %r14\n"
	".size __s390x_longjmp, .-__s390x_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
