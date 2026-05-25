/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef powerpc64
	#define JACL_ARCH powerpc64
	#define JACL_ARCH_POWERPC64 1
	#define __jacl_arch_syscall __powerpc64_syscall
	#define __jacl_arch_tls_set __powerpc64_set_tp_register
	#define __jacl_arch_tls_get __powerpc64_get_tp_register
	#define __jacl_arch_clone_thread __powerpc64_clone_thread
	#define __jacl_arch_setjmp        __powerpc64_setjmp
	#define __jacl_arch_longjmp       __powerpc64_longjmp
	#define __jacl_arch_jmpbuf        __powerpc64_jmpbuf
	#define JACL_BITS 64
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __powerpc64_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long r0 __asm__("r0") = num;
		register long r3 __asm__("r3") = a1;
		register long r4 __asm__("r4") = a2;
		register long r5 __asm__("r5") = a3;
		register long r6 __asm__("r6") = a4;
		register long r7 __asm__("r7") = a5;
		register long r8 __asm__("r8") = a6;

		__asm__ volatile ("sc"
			: "=r"(r3)
			: "r"(r0), "r"(r3), "r"(r4), "r"(r5), "r"(r6), "r"(r7), "r"(r8)
			: "memory", "cr0", "r9", "r10", "r11", "r12");

		result = r3;
		return result;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".globl _start\n"
		"_start:\n"
		"li 31, 0\n"
		"mr 3, 1\n"
		"clrrdi 1, 1, 4\n"
		"bl _start_main\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"li 0, 173\n"
			"sc\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __powerpc64_set_tp_register(void* addr) {
		__asm__ volatile("mr 13, %0" : : "r"(addr) : "memory");
	}

	static inline void* __powerpc64_get_tp_register(void) {
		void* result;

		__asm__("mr %0, 13" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __powerpc64_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~15UL) - 112;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		register long ret __asm__("r3");

		__asm__ volatile(
			"mr 3, %2\n\t"
			"mr 4, %3\n\t"
			"li 5, 0\n\t"
			"li 6, 0\n\t"
			"li 7, 0\n\t"
			"li 0, 120\n\t"
			"sc\n\t"
			"cmpwi 3, 0\n\t"
			"bne 1f\n\t"

			"li 31, 0\n\t"
			"mr 3, %5\n\t"
			"mtctr %4\n\t"
			"bctrl\n\t"
			"li 0, 1\n\t"
			"sc\n\t"

			"1:\n\t"
			: "=r"(ret)
			: "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "r0", "r4", "r5", "r6", "r7", "ctr", "memory"
		);

		return (pid_t)ret;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __powerpc64_jmpbuf[64] __attribute__((__aligned__(16)));

__asm__(
	".text\n"
	".weak __powerpc64_setjmp\n"
	".type __powerpc64_setjmp, @function\n"
	"__powerpc64_setjmp:\n"
	"mr 5, 2\n"
	"mflr 0\n"
	"std 0, 0*8(3)\n"
	"mfcr 0\n"
	"std 0, 1*8(3)\n"
	"std 1, 2*8(3)\n"
	"std 5, 3*8(3)\n"
	"std 14, 4*8(3)\n"
	"std 15, 5*8(3)\n"
	"std 16, 6*8(3)\n"
	"std 17, 7*8(3)\n"
	"std 18, 8*8(3)\n"
	"std 19, 9*8(3)\n"
	"std 20, 10*8(3)\n"
	"std 21, 11*8(3)\n"
	"std 22, 12*8(3)\n"
	"std 23, 13*8(3)\n"
	"std 24, 14*8(3)\n"
	"std 25, 15*8(3)\n"
	"std 26, 16*8(3)\n"
	"std 27, 17*8(3)\n"
	"std 28, 18*8(3)\n"
	"std 29, 19*8(3)\n"
	"std 30, 20*8(3)\n"
	"std 31, 21*8(3)\n"
	"stfd 14, 22*8(3)\n"
	"stfd 15, 23*8(3)\n"
	"stfd 16, 24*8(3)\n"
	"stfd 17, 25*8(3)\n"
	"stfd 18, 26*8(3)\n"
	"stfd 19, 27*8(3)\n"
	"stfd 20, 28*8(3)\n"
	"stfd 21, 29*8(3)\n"
	"stfd 22, 30*8(3)\n"
	"stfd 23, 31*8(3)\n"
	"stfd 24, 32*8(3)\n"
	"stfd 25, 33*8(3)\n"
	"stfd 26, 34*8(3)\n"
	"stfd 27, 35*8(3)\n"
	"stfd 28, 36*8(3)\n"
	"stfd 29, 37*8(3)\n"
	"stfd 30, 38*8(3)\n"
	"stfd 31, 39*8(3)\n"
	"addi 3, 3, 40*8\n"
	"stvx 20, 0, 3 ; addi 3, 3, 16\n"
	"stvx 21, 0, 3 ; addi 3, 3, 16\n"
	"stvx 22, 0, 3 ; addi 3, 3, 16\n"
	"stvx 23, 0, 3 ; addi 3, 3, 16\n"
	"stvx 24, 0, 3 ; addi 3, 3, 16\n"
	"stvx 25, 0, 3 ; addi 3, 3, 16\n"
	"stvx 26, 0, 3 ; addi 3, 3, 16\n"
	"stvx 27, 0, 3 ; addi 3, 3, 16\n"
	"stvx 28, 0, 3 ; addi 3, 3, 16\n"
	"stvx 29, 0, 3 ; addi 3, 3, 16\n"
	"stvx 30, 0, 3 ; addi 3, 3, 16\n"
	"stvx 31, 0, 3\n"
	"li 3, 0\n"
	"blr\n"
	".size __powerpc64_setjmp, .-__powerpc64_setjmp\n"
);

__asm__(
	".text\n"
	".weak __powerpc64_longjmp\n"
	".type __powerpc64_longjmp, @function\n"
	"__powerpc64_longjmp:\n"
	"ld 0, 0*8(3)\n"
	"mtlr 0\n"
	"ld 0, 1*8(3)\n"
	"mtcr 0\n"
	"ld 1, 2*8(3)\n"
	"ld 2, 3*8(3)\n"
	"std 2, 24(1)\n"
	"ld 14, 4*8(3)\n"
	"ld 15, 5*8(3)\n"
	"ld 16, 6*8(3)\n"
	"ld 17, 7*8(3)\n"
	"ld 18, 8*8(3)\n"
	"ld 19, 9*8(3)\n"
	"ld 20, 10*8(3)\n"
	"ld 21, 11*8(3)\n"
	"ld 22, 12*8(3)\n"
	"ld 23, 13*8(3)\n"
	"ld 24, 14*8(3)\n"
	"ld 25, 15*8(3)\n"
	"ld 26, 16*8(3)\n"
	"ld 27, 17*8(3)\n"
	"ld 28, 18*8(3)\n"
	"ld 29, 19*8(3)\n"
	"ld 30, 20*8(3)\n"
	"ld 31, 21*8(3)\n"
	"lfd 14, 22*8(3)\n"
	"lfd 15, 23*8(3)\n"
	"lfd 16, 24*8(3)\n"
	"lfd 17, 25*8(3)\n"
	"lfd 18, 26*8(3)\n"
	"lfd 19, 27*8(3)\n"
	"lfd 20, 28*8(3)\n"
	"lfd 21, 29*8(3)\n"
	"lfd 22, 30*8(3)\n"
	"lfd 23, 31*8(3)\n"
	"lfd 24, 32*8(3)\n"
	"lfd 25, 33*8(3)\n"
	"lfd 26, 34*8(3)\n"
	"lfd 27, 35*8(3)\n"
	"lfd 28, 36*8(3)\n"
	"lfd 29, 37*8(3)\n"
	"lfd 30, 38*8(3)\n"
	"lfd 31, 39*8(3)\n"
	"addi 3, 3, 40*8\n"
	"lvx 20, 0, 3 ; addi 3, 3, 16\n"
	"lvx 21, 0, 3 ; addi 3, 3, 16\n"
	"lvx 22, 0, 3 ; addi 3, 3, 16\n"
	"lvx 23, 0, 3 ; addi 3, 3, 16\n"
	"lvx 24, 0, 3 ; addi 3, 3, 16\n"
	"lvx 25, 0, 3 ; addi 3, 3, 16\n"
	"lvx 26, 0, 3 ; addi 3, 3, 16\n"
	"lvx 27, 0, 3 ; addi 3, 3, 16\n"
	"lvx 28, 0, 3 ; addi 3, 3, 16\n"
	"lvx 29, 0, 3 ; addi 3, 3, 16\n"
	"lvx 30, 0, 3 ; addi 3, 3, 16\n"
	"lvx 31, 0, 3\n"
	"mr 3, 4\n"
	"cmpwi cr7, 4, 0\n"
	"bne cr7, 1f\n"
	"li 3, 1\n"
	"1:\n"
	"blr\n"
	".size __powerpc64_longjmp, .-__powerpc64_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
