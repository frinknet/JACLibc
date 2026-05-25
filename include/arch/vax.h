/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef vax
	#define JACL_ARCH vax
	#define JACL_ARCH_VAX 1
	#define __jacl_arch_syscall __vax_syscall
	#define __jacl_arch_tls_set __vax_set_tp_register
	#define __jacl_arch_tls_get __vax_get_tp_register
	#define __jacl_arch_clone_thread __vax_clone_thread
	#define __jacl_arch_setjmp        __vax_setjmp
	#define __jacl_arch_longjmp       __vax_longjmp
	#define __jacl_arch_jmpbuf        __vax_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 2143
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __vax_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		register long r0 __asm__("r0") = num;

		__asm__ volatile (
			"pushl %6\n\t"
			"pushl %5\n\t"
			"pushl %4\n\t"
			"pushl %3\n\t"
			"pushl %2\n\t"
			"pushl %1\n\t"
			"chmk $0\n\t"
			"addl3 $24, sp, sp\n\t"
			: "+r"(r0)
			: "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6)
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
		"clrl fp\n"
		"movl sp, r0\n"
		"bicl3 $15, sp, sp\n"
		"calls $1, _start_main\n"
	);
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __vax_set_tp_register(void* addr) {
		__asm__ volatile("movl %0, r6" : : "r"(addr) : "memory");
	}

	static inline void* __vax_get_tp_register(void) {
		void* result;
		__asm__("movl r6, %0" : "=r"(result));
		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE && JACL_OS_LINUX
	static inline pid_t __vax_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~3UL) - 4;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		register long r0 __asm__("r0");

		__asm__ volatile(
			"pushl %4\n\t"
			"pushl %3\n\t"
			"pushl %2\n\t"
			"pushl %1\n\t"
			"pushl $0\n\t"
			"pushl $0\n\t"
			"pushl $0\n\t"
			"movl $120, r0\n\t"
			"chmk $0\n\t"
			"addl3 $28, sp, sp\n\t"
			"tstl r0\n\t"
			"bleq 1f\n\t"

			"clrl fp\n\t"
			"pushl %5\n\t"
			"movl %3, r1\n\t"
			"calls $1, (r1)\n\t"
			"movl $1, r0\n\t"
			"chmk $0\n\t"

			"1:\n\t"
			: "=r"(r0)
			: "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "r1", "memory"
		);

		return (pid_t)r0;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __vax_jmpbuf[12];

__asm__(
	".text\n"
	".weak __vax_setjmp\n"
	".type __vax_setjmp, @function\n"
	"__vax_setjmp:\n"
	"movl 4(ap), r0\n"
	"movl sp, 0(r0)\n"
	"movl fp, 4(r0)\n"
	"movl ap, 8(r0)\n"
	"movl 16(fp), 12(r0)\n"
	"movl r2, 16(r0)\n"
	"movl r3, 20(r0)\n"
	"movl r4, 24(r0)\n"
	"movl r5, 28(r0)\n"
	"movl r6, 32(r0)\n"
	"movl r7, 36(r0)\n"
	"movl r8, 40(r0)\n"
	"movl r9, 44(r0)\n"
	"clrl r0\n"
	"ret\n"
	".size __vax_setjmp, .-__vax_setjmp\n"
);

__asm__(
	".text\n"
	".weak __vax_longjmp\n"
	".type __vax_longjmp, @function\n"
	"__vax_longjmp:\n"
	"movl 4(ap), r0\n"
	"movl 8(ap), r1\n"
	"movl 0(r0), sp\n"
	"movl 4(r0), fp\n"
	"movl 8(r0), ap\n"
	"movl 12(r0), 16(fp)\n"
	"movl 16(r0), r2\n"
	"movl 20(r0), r3\n"
	"movl 24(r0), r4\n"
	"movl 28(r0), r5\n"
	"movl 32(r0), r6\n"
	"movl 36(r0), r7\n"
	"movl 40(r0), r8\n"
	"movl 44(r0), r9\n"
	"movl r1, r0\n"
	"tstl r1\n"
	"bneq 1f\n"
	"movl $1, r0\n"
	"1:\n"
	"ret\n"
	".size __vax_longjmp, .-__vax_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif

