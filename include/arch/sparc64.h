/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef sparc64
	#define JACL_ARCH sparc64
	#define JACL_ARCH_SPARC64 1
	#define __jacl_arch_syscall __sparc64_syscall
	#define __jacl_arch_tls_set __sparc64_set_tp_register
	#define __jacl_arch_tls_get __sparc64_get_tp_register
	#define __jacl_arch_clone_thread __sparc64_clone_thread
	#define __jacl_arch_setjmp        __sparc64_setjmp
	#define __jacl_arch_longjmp       __sparc64_longjmp
	#define __jacl_arch_jmpbuf        __sparc64_jmpbuf
	#define JACL_BITS 64
	#define JACL_ORDER 4321
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __sparc64_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long g1 __asm__("g1") = num;
		register long o0 __asm__("o0") = a1;
		register long o1 __asm__("o1") = a2;
		register long o2 __asm__("o2") = a3;
		register long o3 __asm__("o3") = a4;
		register long o4 __asm__("o4") = a5;
		register long o5 __asm__("o5") = a6;

		__asm__ volatile ("t 0x6d"
			: "=r"(o0)
			: "r"(g1), "r"(o0), "r"(o1), "r"(o2), "r"(o3), "r"(o4), "r"(o5)
			: "memory", "cc");

		result = o0;

		return result;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".globl _start\n"
		"_start:\n"
		"mov %g0, %fp\n"
		"add %sp, 2047, %o0\n" /* Pass biased SP */
		"clr %o1\n"
		"call _start_main\n"
		"nop\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"mov 139, %g1\n"
			"ta 0x6d\n"
			"nop\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __sparc64_set_tp_register(void* addr) {
		__asm__ volatile("mov %0, %%g7" : : "r"(addr) : "memory");
	}

	static inline void* __sparc64_get_tp_register(void) {
		void* result;

		__asm__("mov %%g7, %0" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __sparc64_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~15UL) - 176;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		register long o0 __asm__("o0");

		__asm__ volatile(
			"mov %1, %%o0\n\t"
			"mov %2, %%o1\n\t"
			"clr %%o2\n\t"
			"clr %%o3\n\t"
			"clr %%o4\n\t"
			"mov 120, %%g1\n\t"
			"ta 0x6d\n\t"
			"brnz,pn %%o0, 1f\n\t"
			" nop\n\t"

			"clr %%fp\n\t"
			"mov %4, %%o0\n\t"
			"call %3\n\t"
			" nop\n\t"
			"mov 1, %%g1\n\t"
			"ta 0x6d\n\t"

			"1:\n\t"
			: "=r"(o0)
			: "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "g1", "o1", "o2", "o3", "o4", "memory"
		);

		return (pid_t)o0;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __sparc64_jmpbuf[19];

__asm__(
	".text\n"
	".weak __sparc64_setjmp\n"
	".type __sparc64_setjmp, @function\n"
	"__sparc64_setjmp:\n"
	"flushw\n"
	"stx %sp, [%o0 + 0]\n"
	"stx %fp, [%o0 + 8]\n"
	"stx %o7, [%o0 + 16]\n"
	"stx %l0, [%o0 + 24]\n"
	"stx %l1, [%o0 + 32]\n"
	"stx %l2, [%o0 + 40]\n"
	"stx %l3, [%o0 + 48]\n"
	"stx %l4, [%o0 + 56]\n"
	"stx %l5, [%o0 + 64]\n"
	"stx %l6, [%o0 + 72]\n"
	"stx %l7, [%o0 + 80]\n"
	"stx %i0, [%o0 + 88]\n"
	"stx %i1, [%o0 + 96]\n"
	"stx %i2, [%o0 + 104]\n"
	"stx %i3, [%o0 + 112]\n"
	"stx %i4, [%o0 + 120]\n"
	"stx %i5, [%o0 + 128]\n"
	"stx %i6, [%o0 + 136]\n"
	"stx %i7, [%o0 + 144]\n"
	"clr %o0\n"
	"retl\n"
	"nop\n"
	".size __sparc64_setjmp, .-__sparc64_setjmp\n"
);

__asm__(
	".text\n"
	".weak __sparc64_longjmp\n"
	".type __sparc64_longjmp, @function\n"
	"__sparc64_longjmp:\n"
	"ldx [%o0 + 0], %sp\n"
	"ldx [%o0 + 8], %fp\n"
	"ldx [%o0 + 16], %g1\n"
	"ldx [%o0 + 24], %l0\n"
	"ldx [%o0 + 32], %l1\n"
	"ldx [%o0 + 40], %l2\n"
	"ldx [%o0 + 48], %l3\n"
	"ldx [%o0 + 56], %l4\n"
	"ldx [%o0 + 64], %l5\n"
	"ldx [%o0 + 72], %l6\n"
	"ldx [%o0 + 80], %l7\n"
	"ldx [%o0 + 88], %i0\n"
	"ldx [%o0 + 96], %i1\n"
	"ldx [%o0 + 104], %i2\n"
	"ldx [%o0 + 112], %i3\n"
	"ldx [%o0 + 120], %i4\n"
	"ldx [%o0 + 128], %i5\n"
	"ldx [%o0 + 136], %i6\n"
	"ldx [%o0 + 144], %i7\n"
	"mov %o1, %o0\n"
	"tst %o1\n"
	"bne 1f\n"
	"nop\n"
	"mov 1, %o0\n"
	"1:\n"
	"jmp %g1 + 8\n"
	"nop\n"
	".size __sparc64_longjmp, .-__sparc64_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
