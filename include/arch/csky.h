/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef csky
	#define JACL_ARCH csky
	#define JACL_ARCH_CSKY 1
	#define __jacl_arch_syscall __csky_syscall
	#define __jacl_arch_tls_set __csky_set_tp_register
	#define __jacl_arch_tls_get __csky_get_tp_register
	#define __jacl_arch_clone_thread __csky_clone_thread
	#define __jacl_arch_setjmp        __csky_setjmp
	#define __jacl_arch_longjmp       __csky_longjmp
	#define __jacl_arch_jmpbuf        __csky_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __csky_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long r7 __asm__("r7") = num;
		register long r0 __asm__("r0") = a1;
		register long r1 __asm__("r1") = a2;
		register long r2 __asm__("r2") = a3;
		register long r3 __asm__("r3") = a4;
		register long r4 __asm__("r4") = a5;
		register long r5 __asm__("r5") = a6;

		__asm__ volatile ("trap 0"
			: "=r"(r0)
			: "r"(r7), "r"(r0), "r"(r1), "r"(r2), "r"(r3), "r"(r4), "r"(r5)
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
		"movi r8, 0\n"
		"mov r0, sp\n"
		"andi sp, sp, -16\n"
		"jsr _start_main\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"movi r7, 139\n"
			"trap 0\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __csky_set_tp_register(void* addr) {
		__asm__ volatile("mov r31, %0" : : "r"(addr) : "memory");
	}

	static inline void* __csky_get_tp_register(void) {
		void* result;

		__asm__("mov %0, r31" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __csky_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~7UL) - 8;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		register long r0 __asm__("r0");

		__asm__ volatile(
			"mov r0, %1\n\t"
			"mov r1, %2\n\t"
			"movi r2, 0\n\t"
			"movi r3, 0\n\t"
			"movi r4, 0\n\t"
			"movi r7, 220\n\t"
			"trap 0\n\t"
			"cmpnei r0, 0\n\t"
			"bt 1f\n\t"

			"movi r8, 0\n\t"
			"mov r0, %4\n\t"
			"jsr %3\n\t"
			"movi r7, 93\n\t"
			"trap 0\n\t"

			"1:\n\t"
			: "=r"(r0)
			: "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "r1", "r2", "r3", "r4", "r7", "r8", "memory"
		);

		return (pid_t)r0;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __csky_jmpbuf[17];

__asm__(
	".text\n"
	".weak __csky_setjmp\n"
	".type __csky_setjmp, @function\n"
	"__csky_setjmp:\n"
	"stw sp, (r0, 0)\n"
	"stw lr, (r0, 4)\n"
	"stw r4, (r0, 8)\n"
	"stw r5, (r0, 12)\n"
	"stw r6, (r0, 16)\n"
	"stw r7, (r0, 20)\n"
	"stw r8, (r0, 24)\n"
	"stw r9, (r0, 28)\n"
	"stw r10, (r0, 32)\n"
	"stw r11, (r0, 36)\n"
	"stw r16, (r0, 40)\n"
	"stw r17, (r0, 44)\n"
	"stw r26, (r0, 48)\n"
	"stw r27, (r0, 52)\n"
	"stw r28, (r0, 56)\n"
	"stw r29, (r0, 60)\n"
	"stw r30, (r0, 64)\n"
	"movi r0, 0\n"
	"jmp lr\n"
	".size __csky_setjmp, .-__csky_setjmp\n"
);

__asm__(
	".text\n"
	".weak __csky_longjmp\n"
	".type __csky_longjmp, @function\n"
	"__csky_longjmp:\n"
	"mov r2, r0\n"
	"mov r0, r1\n"
	"cmpnei r0, 0\n"
	"bt 1f\n"
	"movi r0, 1\n"
	"1:\n"
	"ldw sp, (r2, 0)\n"
	"ldw lr, (r2, 4)\n"
	"ldw r4, (r2, 8)\n"
	"ldw r5, (r2, 12)\n"
	"ldw r6, (r2, 16)\n"
	"ldw r7, (r2, 20)\n"
	"ldw r8, (r2, 24)\n"
	"ldw r9, (r2, 28)\n"
	"ldw r10, (r2, 32)\n"
	"ldw r11, (r2, 36)\n"
	"ldw r16, (r2, 40)\n"
	"ldw r17, (r2, 44)\n"
	"ldw r26, (r2, 48)\n"
	"ldw r27, (r2, 52)\n"
	"ldw r28, (r2, 56)\n"
	"ldw r29, (r2, 60)\n"
	"ldw r30, (r2, 64)\n"
	"jmp lr\n"
	".size __csky_longjmp, .-__csky_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
}
#endif
