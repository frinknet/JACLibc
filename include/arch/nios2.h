/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef nios2
	#define JACL_ARCH nios2
	#define JACL_ARCH_NIOS2 1
	#define __jacl_arch_syscall __nios2_syscall
	#define __jacl_arch_tls_set __nios2_set_tp_register
	#define __jacl_arch_tls_get __nios2_get_tp_register
	#define __jacl_arch_clone_thread __nios2_clone_thread
	#define __jacl_arch_setjmp        __nios2_setjmp
	#define __jacl_arch_longjmp       __nios2_longjmp
	#define __jacl_arch_jmpbuf        __nios2_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __nios2_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long r2 __asm__("r2") = num;
		register long r4 __asm__("r4") = a1;
		register long r5 __asm__("r5") = a2;
		register long r6 __asm__("r6") = a3;
		register long r7 __asm__("r7") = a4;
		register long r8 __asm__("r8") = a5;
		register long r9 __asm__("r9") = a6;

		__asm__ volatile ("trap"
			: "=r"(r2)
			: "r"(r2), "r"(r4), "r"(r5), "r"(r6), "r"(r7), "r"(r8), "r"(r9)
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
		"mov fp, r0\n"
		"mov r4, sp\n"
		"movi r5, -16\n"
		"and sp, sp, r5\n"
		"call _start_main\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"movi r2, 173\n"
			"trap\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __nios2_set_tp_register(void* addr) {
		__asm__ volatile("mov r23, %0" : : "r"(addr) : "memory");
	}

	static inline void* __nios2_get_tp_register(void) {
		void* result;
		__asm__("mov %0, r23" : "=r"(result));
		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __nios2_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~3UL) - 4;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		register long r2 __asm__("r2");

		__asm__ volatile(
			"mov r4, %1\n\t"
			"mov r5, %2\n\t"
			"mov r6, r0\n\t"
			"mov r7, r0\n\t"
			"mov r8, r0\n\t"
			"movi r2, 220\n\t"
			"trap\n\t"
			"bne r2, r0, 1f\n\t"

			"mov fp, r0\n\t"
			"mov r4, %4\n\t"
			"callr %3\n\t"
			"movi r2, 93\n\t"
			"trap\n\t"

			"1:\n\t"
			: "=r"(r2)
			: "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "r4", "r5", "r6", "r7", "r8", "memory"
		);

		return (pid_t)r2;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __nios2_jmpbuf[12];

__asm__(
	".text\n"
	".weak __nios2_setjmp\n"
	".type __nios2_setjmp, @function\n"
	"__nios2_setjmp:\n"
	"stw ra, 0(r4)\n"
	"stw sp, 4(r4)\n"
	"stw fp, 8(r4)\n"
	"stw gp, 12(r4)\n"
	"stw r16, 16(r4)\n"
	"stw r17, 20(r4)\n"
	"stw r18, 24(r4)\n"
	"stw r19, 28(r4)\n"
	"stw r20, 32(r4)\n"
	"stw r21, 36(r4)\n"
	"stw r22, 40(r4)\n"
	"stw r23, 44(r4)\n"
	"mov r2, r0\n"
	"ret\n"
	".size __nios2_setjmp, .-__nios2_setjmp\n"
);

__asm__(
	".text\n"
	".weak __nios2_longjmp\n"
	".type __nios2_longjmp, @function\n"
	"__nios2_longjmp:\n"
	"ldw ra, 0(r4)\n"
	"ldw sp, 4(r4)\n"
	"ldw fp, 8(r4)\n"
	"ldw gp, 12(r4)\n"
	"ldw r16, 16(r4)\n"
	"ldw r17, 20(r4)\n"
	"ldw r18, 24(r4)\n"
	"ldw r19, 28(r4)\n"
	"ldw r20, 32(r4)\n"
	"ldw r21, 36(r4)\n"
	"ldw r22, 40(r4)\n"
	"ldw r23, 44(r4)\n"
	"mov r2, r5\n"
	"bne r5, r0, 1f\n"
	"movi r2, 1\n"
	"1:\n"
	"ret\n"
	".size __nios2_longjmp, .-__nios2_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
