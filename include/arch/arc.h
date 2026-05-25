/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef arc
	#define JACL_ARCH arc
	#define JACL_ARCH_ARC 1
	#define __jacl_arch_syscall __arc_syscall
	#define __jacl_arch_tls_set __arc_set_tp_register
	#define __jacl_arch_tls_get __arc_get_tp_register
	#define __jacl_arch_clone_thread __arc_clone_thread
	#define __jacl_arch_setjmp        __arc_setjmp
	#define __jacl_arch_longjmp       __arc_longjmp
	#define __jacl_arch_jmpbuf        __arc_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __arc_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6)
	{
		register long r8 __asm__("r8") = num;
		register long r0 __asm__("r0") = a1;
		register long r1 __asm__("r1") = a2;
		register long r2 __asm__("r2") = a3;
		register long r3 __asm__("r3") = a4;
		register long r4 __asm__("r4") = a5;
		register long r5 __asm__("r5") = a6;

		__asm__ volatile(
			"trap0"
			: "+r"(r0)           /* r0: result (or -errno) */
			: "r"(r8), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5)
			: "memory"
		);

		return r0;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".globl _start\n"
		"_start:\n"
		"mov fp, 0\n"
		"mov r0, sp\n"
		"and sp, sp, -16\n"
		"jl _start_main\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"mov r8, 139\n"
			"trap0\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __arc_set_tp_register(void* addr) {
		__asm__ volatile("mov r25, %0" : : "r"(addr) : "memory");
	}

	static inline void* __arc_get_tp_register(void) {
		void* result;

		__asm__("mov %0, r25" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __arc_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~3UL) - 4;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		register long r0 __asm__("r0");

		__asm__ volatile(
			"mov r0, %1\n\t"
			"mov r1, %2\n\t"
			"mov r2, 0\n\t"
			"mov r3, 0\n\t"
			"mov r4, 0\n\t"
			"mov r8, 220\n\t"
			"trap0\n\t"
			"brne r0, 0, 1f\n\t"

			"mov fp, 0\n\t"
			"mov r0, %4\n\t"
			"jl [%3]\n\t"
			"mov r8, 93\n\t"
			"trap0\n\t"

			"1:\n\t"
			: "=r"(r0)
			: "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "r1", "r2", "r3", "r4", "r8", "memory"
		);

		return (pid_t)r0;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __arc_jmpbuf[16];

__asm__(
	".text\n"
	".weak __arc_setjmp\n"
	".type __arc_setjmp, @function\n"
	"__arc_setjmp:\n"
	"st blink, [r0, 0]\n"
	"st sp, [r0, 4]\n"
	"st fp, [r0, 8]\n"
	"st gp, [r0, 12]\n"
	"st r13, [r0, 16]\n"
	"st r14, [r0, 20]\n"
	"st r15, [r0, 24]\n"
	"st r16, [r0, 28]\n"
	"st r17, [r0, 32]\n"
	"st r18, [r0, 36]\n"
	"st r19, [r0, 40]\n"
	"st r20, [r0, 44]\n"
	"st r21, [r0, 48]\n"
	"st r22, [r0, 52]\n"
	"st r23, [r0, 56]\n"
	"st r24, [r0, 60]\n"
	"mov r0, 0\n"
	"j.d [blink]\n"
	" nop\n"
	".size __arc_setjmp, .-__arc_setjmp\n"
);

__asm__(
	".text\n"
	".weak __arc_longjmp\n"
	".type __arc_longjmp, @function\n"
	"__arc_longjmp:\n"
	"ld blink, [r0, 0]\n"
	"ld sp, [r0, 4]\n"
	"ld fp, [r0, 8]\n"
	"ld gp, [r0, 12]\n"
	"ld r13, [r0, 16]\n"
	"ld r14, [r0, 20]\n"
	"ld r15, [r0, 24]\n"
	"ld r16, [r0, 28]\n"
	"ld r17, [r0, 32]\n"
	"ld r18, [r0, 36]\n"
	"ld r19, [r0, 40]\n"
	"ld r20, [r0, 44]\n"
	"ld r21, [r0, 48]\n"
	"ld r22, [r0, 52]\n"
	"ld r23, [r0, 56]\n"
	"ld r24, [r0, 60]\n"
	"mov r0, r1\n"
	"tst r0, r0\n"
	"j.d [blink]\n"
	" mov.z r0, 1\n"
	".size __arc_longjmp, .-__arc_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
}
#endif
