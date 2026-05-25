/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef sh
	#define JACL_ARCH sh
	#define JACL_ARCH_SH 1
	#define __jacl_arch_syscall __sh_syscall
	#define __jacl_arch_tls_set __sh_set_tp_register
	#define __jacl_arch_tls_get __sh_get_tp_register
	#define __jacl_arch_clone_thread __sh_clone_thread
	#define __jacl_arch_setjmp       __sh_setjmp
	#define __jacl_arch_longjmp      __sh_longjmp
	#define __jacl_arch_jmpbuf       __sh_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 4321
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __sh_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long r3 __asm__("r3") = num;
		register long r4 __asm__("r4") = a1;
		register long r5 __asm__("r5") = a2;
		register long r6 __asm__("r6") = a3;
		register long r7 __asm__("r7") = a4;
		register long r0 __asm__("r0") = a5;
		register long r1 __asm__("r1") = a6;

		__asm__ volatile ("trapa #31"
			: "=r"(r0)
			: "r"(r3), "r"(r4), "r"(r5), "r"(r6), "r"(r7), "r"(r0), "r"(r1)
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
		"mov #0, r14\n"
		"mov r15, r4\n"
		"mov #-16, r0\n"
		"and r0, r15\n"
		"bsr _start_main\n"
		"nop\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"mov #173, r3\n"
			"trapa #31\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __sh_set_tp_register(void* addr) {
		__asm__ volatile("ldc %0, gbr" : : "r"(addr) : "memory");
	}

	static inline void* __sh_get_tp_register(void) {
		void* result;

		__asm__("stc gbr, %0" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __sh_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~7UL) - 8;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		register long r0 __asm__("r0");

		__asm__ volatile(
			"mov %1, r4\n\t"
			"mov %2, r5\n\t"
			"mov #0, r6\n\t"
			"mov #0, r7\n\t"
			"mov #0, r0\n\t"
			"mov #120, r3\n\t"
			"trapa #31\n\t"
			"tst r0, r0\n\t"
			"bf 1f\n\t"

			"mov #0, r14\n\t"
			"mov %4, r4\n\t"
			"jsr @%3\n\t"
			" mov #1, r3\n\t"
			"trapa #31\n\t"

			"1:\n\t"
			: "=r"(r0)
			: "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "r3", "r4", "r5", "r6", "r7", "r14", "memory"
		);

		return (pid_t)r0;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __sh_jmpbuf[16];

__asm__(
	".text\n"
	".weak __sh_setjmp\n"
	".type __sh_setjmp, @function\n"
	"__sh_setjmp:\n"
#if defined(__SH_FPU_ANY__) || defined(__SH4__)
	"add #52, r4\n"
	"fmov.s fr15, @-r4\n"
	"fmov.s fr14, @-r4\n"
	"fmov.s fr13, @-r4\n"
	"fmov.s fr12, @-r4\n"
#else
	"add #36, r4\n"
#endif
	"sts.l pr, @-r4\n"
	"mov.l r15, @-r4\n"
	"mov.l r14, @-r4\n"
	"mov.l r13, @-r4\n"
	"mov.l r12, @-r4\n"
	"mov.l r11, @-r4\n"
	"mov.l r10, @-r4\n"
	"mov.l r9, @-r4\n"
	"mov.l r8, @-r4\n"
	"rts\n"
	" mov #0, r0\n"
	".size __sh_setjmp, .-__sh_setjmp\n"
);

__asm__(
	".text\n"
	".weak __sh_longjmp\n"
	".type __sh_longjmp, @function\n"
	"__sh_longjmp:\n"
	"mov.l @r4+, r8\n"
	"mov.l @r4+, r9\n"
	"mov.l @r4+, r10\n"
	"mov.l @r4+, r11\n"
	"mov.l @r4+, r12\n"
	"mov.l @r4+, r13\n"
	"mov.l @r4+, r14\n"
	"mov.l @r4+, r15\n"
	"lds.l @r4+, pr\n"
#if defined(__SH_FPU_ANY__) || defined(__SH4__)
	"fmov.s @r4+, fr12\n"
	"fmov.s @r4+, fr13\n"
	"fmov.s @r4+, fr14\n"
	"fmov.s @r4+, fr15\n"
#endif
	"tst r5, r5\n"
	"movt r0\n"
	"add r5, r0\n"
	"rts\n"
	" nop\n"
	".size __sh_longjmp, .-__sh_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
