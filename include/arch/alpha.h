/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef alpha
	#define JACL_ARCH alpha
	#define JACL_ARCH_ALPHA 1
	#define __jacl_arch_syscall __alpha_syscall
	#define __jacl_arch_tls_set __alpha_set_tp_register
	#define __jacl_arch_tls_get __alpha_get_tp_register
	#define __jacl_arch_clone_thread __alpha_clone_thread
	#define __jacl_arch_setjmp        __alpha_setjmp
	#define __jacl_arch_longjmp       __alpha_longjmp
	#define __jacl_arch_jmpbuf        __alpha_jmpbuf
	#define JACL_BITS 64
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __alpha_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long v0 __asm__("$0") = num;
		register long a0 __asm__("$16") = a1;
		register long a1r __asm__("$17") = a2;
		register long a2r __asm__("$18") = a3;
		register long a3r __asm__("$19") = a4;
		register long a4r __asm__("$20") = a5;
		register long a5r __asm__("$21") = a6;

		__asm__ volatile ("callsys"
			: "=r"(v0)
			: "r"(v0), "r"(a0), "r"(a1r), "r"(a2r), "r"(a3r), "r"(a4r), "r"(a5r)
			: "memory", "$1", "$22", "$23", "$24", "$25", "$27", "$28");

		result = v0;

		return result;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".globl _start\n"
		"_start:\n"
		"mov $31, $15\n"
		"mov $30, $16\n"
		"lda $30, -16($30)\n"
		"bsr $26, _start_main\n"
	);

	#if JACL_OS_LINUX
		__asm__(
			".text\n"
			".globl __restore_rt\n"
			".type __restore_rt,@function\n"
			"__restore_rt:\n"
			"lda $0, 298\n"
			"callsys\n"
		);
	#endif
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __alpha_set_tp_register(void* addr) {
		__asm__ volatile("mov %0, $0; call_pal 158" : : "r"(addr) : "memory", "$0");
	}

	static inline void* __alpha_get_tp_register(void) {
		void* result;

		__asm__("call_pal 9; mov $0, %0" : "=r"(result) : : "$0");

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __alpha_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~15UL) - 16;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		long ret;

		__asm__ volatile(
			"mov %2, $16\n\t"
			"mov %3, $17\n\t"
			"clr $18\n\t"
			"clr $19\n\t"
			"clr $20\n\t"
			"lda $0, 120\n\t"
			"call_pal 0x83\n\t"
			"bne $0, 1f\n\t"

			"clr $15\n\t"
			"mov %5, $16\n\t"
			"jsr $26, (%4)\n\t"
			"lda $0, 1\n\t"
			"call_pal 0x83\n\t"

			"1:\n\t"
			: "=r"(ret)
			: "r"((long)120), "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "$0", "$16", "$17", "$18", "$19", "$20", "$26", "memory"
		);

		return ret;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __alpha_jmpbuf[32];

__asm__(
	".text\n"
	".weak __alpha_setjmp\n"
	".type __alpha_setjmp, @function\n"
	"__alpha_setjmp:\n"
	"stq $s0, 0*8($16)\n"
	"stq $s1, 1*8($16)\n"
	"stq $s2, 2*8($16)\n"
	"stq $s3, 3*8($16)\n"
	"stq $s4, 4*8($16)\n"
	"stq $s5, 5*8($16)\n"
	"stq $ra, 6*8($16)\n"
	"stq $sp, 7*8($16)\n"
	"stq $fp, 8*8($16)\n"
	"stt $f2, 9*8($16)\n"
	"stt $f3, 10*8($16)\n"
	"stt $f4, 11*8($16)\n"
	"stt $f5, 12*8($16)\n"
	"stt $f6, 13*8($16)\n"
	"stt $f7, 14*8($16)\n"
	"stt $f8, 15*8($16)\n"
	"stt $f9, 16*8($16)\n"
	"bis $31, $31, $0\n"
	"ret\n"
	".size __alpha_setjmp, .-__alpha_setjmp\n"
);

__asm__(
	".text\n"
	".weak __alpha_longjmp\n"
	".type __alpha_longjmp, @function\n"
	"__alpha_longjmp:\n"
	"ldq $s0, 0*8($16)\n"
	"ldq $s1, 1*8($16)\n"
	"ldq $s2, 2*8($16)\n"
	"ldq $s3, 3*8($16)\n"
	"ldq $s4, 4*8($16)\n"
	"ldq $s5, 5*8($16)\n"
	"ldq $ra, 6*8($16)\n"
	"ldq $sp, 7*8($16)\n"
	"ldq $fp, 8*8($16)\n"
	"ldt $f2, 9*8($16)\n"
	"ldt $f3, 10*8($16)\n"
	"ldt $f4, 11*8($16)\n"
	"ldt $f5, 12*8($16)\n"
	"ldt $f6, 13*8($16)\n"
	"ldt $f7, 14*8($16)\n"
	"ldt $f8, 15*8($16)\n"
	"ldt $f9, 16*8($16)\n"
	"cmpeq $17, 0, $1\n"
	"addl $17, $1, $0\n"
	"ret\n"
	".size __alpha_longjmp, .-__alpha_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
