/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef sparc32
	#define JACL_ARCH sparc32
	#define JACL_ARCH_SPARC32 1
	#define __jacl_arch_syscall __sparc32_syscall
	#define __jacl_arch_tls_set __sparc32_set_tp_register
	#define __jacl_arch_tls_get __sparc32_get_tp_register
	#define __jacl_arch_clone_thread __sparc32_clone_thread
	#define __jacl_arch_setjmp        __sparc32_setjmp
	#define __jacl_arch_longjmp       __sparc32_longjmp
	#define __jacl_arch_jmpbuf        __sparc32_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 4321
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __sparc32_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long g1 __asm__("g1") = num;
		register long o0 __asm__("o0") = a1;
		register long o1 __asm__("o1") = a2;
		register long o2 __asm__("o2") = a3;
		register long o3 __asm__("o3") = a4;
		register long o4 __asm__("o4") = a5;
		register long o5 __asm__("o5") = a6;

		__asm__ volatile ("t 0x10"
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
		"mov %sp, %o0\n"
		"and %sp, -16, %sp\n"
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
			"t 0x10\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __sparc32_set_tp_register(void* addr) {
		__asm__ volatile("mov %0, %%g7" : : "r"(addr) : "memory");
	}

	static inline void* __sparc32_get_tp_register(void) {
		void* result;

		__asm__("mov %%g7, %0" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __sparc32_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~7UL) - 8;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		register long o0 __asm__("o0");

		__asm__ volatile(
			"mov %1, %%o0\n\t"
			"mov %2, %%o1\n\t"
			"clr %%o2\n\t"
			"clr %%o3\n\t"
			"clr %%o4\n\t"
			"mov 120, %%g1\n\t"
			"t 0x10\n\t"
			"bne 1f\n\t"
			" nop\n\t"

			"clr %%fp\n\t"
			"mov %4, %%o0\n\t"
			"call %3\n\t"
			" nop\n\t"
			"mov 1, %%g1\n\t"
			"t 0x10\n\t"

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

typedef unsigned long __sparc32_jmpbuf[19];

__asm__(
	".text\n"
	".weak __sparc32_setjmp\n"
	".type __sparc32_setjmp, @function\n"
	"__sparc32_setjmp:\n"
	"t 0x03\n" /* flushw */
	"st %sp, [%o0 + 0]\n"
	"st %fp, [%o0 + 4]\n"
	"st %o7, [%o0 + 8]\n"
	"st %l0, [%o0 + 12]\n"
	"st %l1, [%o0 + 16]\n"
	"st %l2, [%o0 + 20]\n"
	"st %l3, [%o0 + 24]\n"
	"st %l4, [%o0 + 28]\n"
	"st %l5, [%o0 + 32]\n"
	"st %l6, [%o0 + 36]\n"
	"st %l7, [%o0 + 40]\n"
	"st %i0, [%o0 + 44]\n"
	"st %i1, [%o0 + 48]\n"
	"st %i2, [%o0 + 52]\n"
	"st %i3, [%o0 + 56]\n"
	"st %i4, [%o0 + 60]\n"
	"st %i5, [%o0 + 64]\n"
	"st %i6, [%o0 + 68]\n"
	"st %i7, [%o0 + 72]\n"
	"clr %o0\n"
	"retl\n"
	"nop\n"
	".size __sparc32_setjmp, .-__sparc32_setjmp\n"
);

__asm__(
	".text\n"
	".weak __sparc32_longjmp\n"
	".type __sparc32_longjmp, @function\n"
	"__sparc32_longjmp:\n"
	"ld [%o0 + 0], %sp\n"
	"ld [%o0 + 4], %fp\n"
	"ld [%o0 + 8], %g1\n" /* Load return address into scratch */
	"ld [%o0 + 12], %l0\n"
	"ld [%o0 + 16], %l1\n"
	"ld [%o0 + 20], %l2\n"
	"ld [%o0 + 24], %l3\n"
	"ld [%o0 + 28], %l4\n"
	"ld [%o0 + 32], %l5\n"
	"ld [%o0 + 36], %l6\n"
	"ld [%o0 + 40], %l7\n"
	"ld [%o0 + 44], %i0\n"
	"ld [%o0 + 48], %i1\n"
	"ld [%o0 + 52], %i2\n"
	"ld [%o0 + 56], %i3\n"
	"ld [%o0 + 60], %i4\n"
	"ld [%o0 + 64], %i5\n"
	"ld [%o0 + 68], %i6\n"
	"ld [%o0 + 72], %i7\n"
	"mov %o1, %o0\n"
	"tst %o1\n"
	"bne 1f\n"
	"nop\n"
	"mov 1, %o0\n"
	"1:\n"
	"jmp %g1 + 8\n" /* Jump to return address + 8 */
	"nop\n"
	".size __sparc32_longjmp, .-__sparc32_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
