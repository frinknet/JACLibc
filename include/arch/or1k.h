/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef or1k
	#define JACL_ARCH or1k
	#define JACL_ARCH_OR1K 1
	#define __jacl_arch_syscall __or1k_syscall
	#define __jacl_arch_tls_set __or1k_set_tp_register
	#define __jacl_arch_tls_get __or1k_get_tp_register
	#define __jacl_arch_clone_thread __or1k_clone_thread
	#define __jacl_arch_setjmp        __or1k_setjmp
	#define __jacl_arch_longjmp       __or1k_longjmp
	#define __jacl_arch_jmpbuf        __or1k_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __or1k_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long r11 __asm__("r11") = num;
		register long r3 __asm__("r3") = a1;
		register long r4 __asm__("r4") = a2;
		register long r5 __asm__("r5") = a3;
		register long r6 __asm__("r6") = a4;
		register long r7 __asm__("r7") = a5;
		register long r8 __asm__("r8") = a6;

		__asm__ volatile ("l.sys 1"
			: "=r"(r11)
			: "r"(r11), "r"(r3), "r"(r4), "r"(r5), "r"(r6), "r"(r7), "r"(r8)
			: "memory");

		result = r11;

		return result;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".globl _start\n"
		"_start:\n"
		"l.or r2, r0, r0\n"
		"l.or r3, r1, r0\n"
		"l.andi r1, r1, -16\n"
		"l.jal _start_main\n"
		"l.nop\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"l.addi r11, r0, 173\n"
			"l.sys 1\n"
			"l.nop\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __or1k_set_tp_register(void* addr) {
		__asm__ volatile("l.or r10, %0, r0" : : "r"(addr) : "memory");
	}

	static inline void* __or1k_get_tp_register(void) {
		void* result;

		__asm__("l.or %0, r10, r0" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE && JACL_OS_LINUX
	static inline pid_t __or1k_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~15UL) - 16;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		register long r11 __asm__("r11");

		__asm__ volatile(
			"l.or r3, %1, r0\n\t" /* flags */
			"l.or r4, %2, r0\n\t" /* stack_top */
			"l.or r5, r0, r0\n\t"
			"l.or r6, r0, r0\n\t"
			"l.or r7, r0, r0\n\t"
			"l.addi r11, r0, 120\n\t" /* SYS_clone */
			"l.sys 1\n\t"
			"l.sfeqi r11, 0\n\t"
			"l.bnf 1f\n\t"
			" l.nop\n\t"

			/* Child Thread Context */
			"l.or r2, r0, r0\n\t" /* Clear FP */
			"l.or r3, %4, r0\n\t" /* arg */
			"l.jalr %3\n\t" /* fn */
			" l.nop\n\t"
			"l.addi r11, r0, 93\n\t" /* SYS_exit */
			"l.or r3, r0, r0\n\t"
			"l.sys 1\n\t"

			"1:\n\t"
			: "=r"(r11)
			: "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "r3", "r4", "r5", "r6", "r7", "memory"
		);

		return (pid_t)r11;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __or1k_jmpbuf[16];

__asm__(
	".text\n"
	".weak __or1k_setjmp\n"
	".type __or1k_setjmp, @function\n"
	"__or1k_setjmp:\n"
	"l.sw 0(r3), r1\n"
	"l.sw 4(r3), r2\n"
	"l.sw 8(r3), r9\n"
	"l.sw 12(r3), r10\n"
	"l.sw 16(r3), r14\n"
	"l.sw 20(r3), r16\n"
	"l.sw 24(r3), r18\n"
	"l.sw 28(r3), r20\n"
	"l.sw 32(r3), r22\n"
	"l.sw 36(r3), r24\n"
	"l.sw 40(r3), r26\n"
	"l.sw 44(r3), r28\n"
	"l.sw 48(r3), r30\n"
	"l.jr r9\n"
	" l.ori r11, r0, 0\n"
	".size __or1k_setjmp, .-__or1k_setjmp\n"
);

__asm__(
	".text\n"
	".weak __or1k_longjmp\n"
	".type __or1k_longjmp, @function\n"
	"__or1k_longjmp:\n"
	"l.sfeqi r4, 0\n"
	"l.bnf 1f\n"
	" l.addi r11, r4, 0\n"
	"l.ori r11, r0, 1\n"
	"1:\n"
	"l.lwz r1, 0(r3)\n"
	"l.lwz r2, 4(r3)\n"
	"l.lwz r9, 8(r3)\n"
	"l.lwz r10, 12(r3)\n"
	"l.lwz r14, 16(r3)\n"
	"l.lwz r16, 20(r3)\n"
	"l.lwz r18, 24(r3)\n"
	"l.lwz r20, 28(r3)\n"
	"l.lwz r22, 32(r3)\n"
	"l.lwz r24, 36(r3)\n"
	"l.lwz r26, 40(r3)\n"
	"l.lwz r28, 44(r3)\n"
	"l.lwz r30, 48(r3)\n"
	"l.jr r9\n"
	" l.nop\n"
	".size __or1k_longjmp, .-__or1k_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
