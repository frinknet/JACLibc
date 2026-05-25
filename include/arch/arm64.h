/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef arm64
	#define JACL_ARCH arm64
	#define JACL_ARCH_ARM64 1
	#define __jacl_arch_syscall __arm64_syscall
	#define __jacl_arch_tls_set __arm64_set_tp_register
	#define __jacl_arch_tls_get __arm64_get_tp_register
	#define __jacl_arch_clone_thread __arm64_clone_thread
	#define __jacl_arch_setjmp        __arm64_setjmp
	#define __jacl_arch_longjmp       __arm64_longjmp
	#define __jacl_arch_jmpbuf        __arm64_jmpbuf
	#define JACL_BITS 64
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __arm64_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long x8 __asm__("x8") = num;
		register long x0 __asm__("x0") = a1;
		register long x1 __asm__("x1") = a2;
		register long x2 __asm__("x2") = a3;
		register long x3 __asm__("x3") = a4;
		register long x4 __asm__("x4") = a5;
		register long x5 __asm__("x5") = a6;

		__asm__ volatile ("svc #0"
			: "=r"(x0)
			: "r"(x8), "r"(x0), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5)
			: "memory");

		result = x0;

		return result;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".globl _start\n"
		"_start:\n"
		"mov x29, #0\n"
		"mov x30, #0\n"
		"mov x0, sp\n"
		"and sp, x0, #-16\n"
		"bl _start_main\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"mov x8, #139\n"
			"svc #0\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __arm64_set_tp_register(void* addr) {
		__asm__ volatile("msr tpidr_el0, %0" : : "r"(addr) : "memory");
	}

	static inline void* __arm64_get_tp_register(void) {
		void* result;
		__asm__("mrs %0, tpidr_el0" : "=r"(result));
		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __arm64_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~15UL) - 16;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		long ret;

		__asm__ volatile(
			"mov x0, %2\n\t"
			"mov x1, %3\n\t"
			"mov x2, xzr\n\t"
			"mov x3, xzr\n\t"
			"mov x4, xzr\n\t"
			"mov x8, #220\n\t"
			"svc #0\n\t"
			"cbnz x0, 1f\n\t"

			"mov x29, xzr\n\t"
			"mov x0, %5\n\t"
			"blr %4\n\t"
			"mov x8, #93\n\t"
			"svc #0\n\t"

			"1:\n\t"
			: "=r"(ret)
			: "r"((long)220), "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "x0", "x1", "x2", "x3", "x4", "x8", "memory"
		);

		return ret;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __arm64_jmpbuf[24];

__asm__(
	".text\n"
	".weak __arm64_setjmp\n"
	".type __arm64_setjmp, @function\n"
	"__arm64_setjmp:\n"
	"stp x19, x20, [x0, #0]\n"
	"stp x21, x22, [x0, #16]\n"
	"stp x23, x24, [x0, #32]\n"
	"stp x25, x26, [x0, #48]\n"
	"stp x27, x28, [x0, #64]\n"
	"stp x29, x30, [x0, #80]\n"
	"mov x2, sp\n"
	"str x2, [x0, #104]\n"
	"stp d8, d9, [x0, #112]\n"
	"stp d10, d11, [x0, #128]\n"
	"stp d12, d13, [x0, #144]\n"
	"stp d14, d15, [x0, #160]\n"
	"mov x0, #0\n"
	"ret\n"
	".size __arm64_setjmp, .-__arm64_setjmp\n"
);

__asm__(
	".text\n"
	".weak __arm64_longjmp\n"
	".type __arm64_longjmp, @function\n"
	"__arm64_longjmp:\n"
	"ldp x19, x20, [x0, #0]\n"
	"ldp x21, x22, [x0, #16]\n"
	"ldp x23, x24, [x0, #32]\n"
	"ldp x25, x26, [x0, #48]\n"
	"ldp x27, x28, [x0, #64]\n"
	"ldp x29, x30, [x0, #80]\n"
	"ldr x2, [x0, #104]\n"
	"mov sp, x2\n"
	"ldp d8, d9, [x0, #112]\n"
	"ldp d10, d11, [x0, #128]\n"
	"ldp d12, d13, [x0, #144]\n"
	"ldp d14, d15, [x0, #160]\n"
	"cmp w1, #0\n"
	"csinc w0, w1, wzr, ne\n"
	"br x30\n"
	".size __arm64_longjmp, .-__arm64_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
