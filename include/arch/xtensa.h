/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef xtensa
	#define JACL_ARCH xtensa
	#define JACL_ARCH_XTENSA 1
	#define __jacl_arch_syscall __xtensa_syscall
	#define __jacl_arch_tls_set __xtensa_set_tp_register
	#define __jacl_arch_tls_get __xtensa_get_tp_register
	#define __jacl_arch_clone_thread __xtensa_clone_thread
	#define __jacl_arch_setjmp        __xtensa_setjmp
	#define __jacl_arch_longjmp       __xtensa_longjmp
	#define __jacl_arch_jmpbuf        __xtensa_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __xtensa_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		register long a2_reg __asm__("a2") = num;
		register long a3_reg __asm__("a3") = a1;
		register long a4_reg __asm__("a4") = a2;
		register long a5_reg __asm__("a5") = a3;
		register long a6_reg __asm__("a6") = a4;
		register long a7_reg __asm__("a7") = a5;
		register long a8_reg __asm__("a8") = a6;

		__asm__ volatile ("syscall"
			: "+r"(a2_reg)
			: "r"(a3_reg), "r"(a4_reg), "r"(a5_reg), "r"(a6_reg), "r"(a7_reg), "r"(a8_reg)
			: "memory");

		return a2_reg;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".text\n"
		".globl _start\n"
		"_start:\n"
		"entry sp, 16\n"
		"movi a2, 0\n"
		"mov a4, a1\n" /* Pass SP in a4 (first arg of new window) */
		"movi a5, _start_main\n"
		"callx4 a5\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"movi a2, 139\n"
			"syscall\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __xtensa_set_tp_register(void* addr) {
		__asm__ volatile("wrtp %0" : : "r"(addr) : "memory");
	}

	static inline void* __xtensa_get_tp_register(void) {
		void* result;
		__asm__("rrtp %0" : "=r"(result));
		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE && JACL_OS_LINUX
	static inline pid_t __xtensa_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~15UL) - 16;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		register long a2 __asm__("a2");

		__asm__ volatile(
			"mov a6, %1\n\t" /* flags */
			"mov a3, %2\n\t" /* stack_top */
			"movi a4, 0\n\t"
			"movi a5, 0\n\t"
			"movi a7, 0\n\t"
			"movi a2, 220\n\t" /* SYS_clone */
			"syscall\n\t"
			"bnez a2, 1f\n\t"

			/* Child Thread Context */
			"movi a2, 0\n\t"
			"wrtp a2\n\t" /* Reset TP for child (optional, depends on policy) */
			"mov a6, %4\n\t" /* arg */
			"callx0 %3\n\t" /* fn - using callx0 as fn might be a naked pointer */
			"movi a2, 93\n\t" /* SYS_exit */
			"syscall\n\t"

			"1:\n\t"
			: "=r"(a2)
			: "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "a3", "a4", "a5", "a6", "a7", "memory"
		);

		return (pid_t)a2;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __xtensa_jmpbuf[6];

__asm__(
	".text\n"
	".weak __xtensa_setjmp\n"
	".type __xtensa_setjmp, %function\n"
	"__xtensa_setjmp:\n"
	"spillw\n"
	"s32i a0, a2, 0\n"
	"s32i a1, a2, 4\n"
	"s32i a12, a2, 8\n"
	"s32i a13, a2, 12\n"
	"s32i a14, a2, 16\n"
	"s32i a15, a2, 20\n"
	"movi a2, 0\n"
	"retw\n"
	".size __xtensa_setjmp, .-__xtensa_setjmp\n"
);

__asm__(
	".text\n"
	".weak __xtensa_longjmp\n"
	".type __xtensa_longjmp, %function\n"
	"__xtensa_longjmp:\n"
	"l32i a0, a2, 0\n"
	"l32i a1, a2, 4\n"
	"l32i a12, a2, 8\n"
	"l32i a13, a2, 12\n"
	"l32i a14, a2, 16\n"
	"l32i a15, a2, 20\n"
	"mov a2, a3\n"
	"nez a3, 1f\n"
	"movi a2, 1\n"
	"1:\n"
	"retw\n"
	".size __xtensa_longjmp, .-__xtensa_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
