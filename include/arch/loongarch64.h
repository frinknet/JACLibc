/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef loongarch64
	#define JACL_ARCH loongarch64
	#define JACL_ARCH_LOONGARCH64 1
	#define __jacl_arch_syscall __loongarch64_syscall
	#define __jacl_arch_tls_set __loongarch64_set_tp_register
	#define __jacl_arch_tls_get __loongarch64_get_tp_register
	#define __jacl_arch_clone_thread __loongarch64_clone_thread
	#define __jacl_arch_setjmp        __loongarch64_setjmp
	#define __jacl_arch_longjmp       __loongarch64_longjmp
	#define __jacl_arch_jmpbuf        __loongarch64_jmpbuf
	#define JACL_BITS 64
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __loongarch64_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long a7 __asm__("$a7") = num;
		register long a0 __asm__("$a0") = a1;
		register long a1_reg __asm__("$a1") = a2;
		register long a2_reg __asm__("$a2") = a3;
		register long a3_reg __asm__("$a3") = a4;
		register long a4_reg __asm__("$a4") = a5;
		register long a5_reg __asm__("$a5") = a6;

		__asm__ volatile ("syscall 0"
			: "=r"(a0)
			: "r"(a7), "r"(a0), "r"(a1_reg), "r"(a2_reg), "r"(a3_reg), "r"(a4_reg), "r"(a5_reg)
			: "memory", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8");

		result = a0;

		return result;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".globl _start\n"
		"_start:\n"
		"move $fp, $zero\n"
		"move $a0, $sp\n"
		"bstrins.d $sp, $zero, 3, 0\n"
		"bl _start_main\n"
	);
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __loongarch64_set_tp_register(void* addr) {
		__asm__ volatile("move $tp, %0" : : "r"(addr) : "memory");
	}

	static inline void* __loongarch64_get_tp_register(void) {
		void* result;

		__asm__("move %0, $tp" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __loongarch64_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~15UL) - 16;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		register long ret __asm__("$a0");

		__asm__ volatile(
			"move $a0, %2\n\t"
			"move $a1, %3\n\t"
			"move $a2, $zero\n\t"
			"move $a3, $zero\n\t"
			"move $a4, $zero\n\t"
			"li.w $a7, 220\n\t"
			"syscall 0\n\t"
			"bnez $a0, 1f\n\t"

			"move $fp, $zero\n\t"
			"move $a0, %5\n\t"
			"jirl $ra, %4, 0\n\t"
			"li.w $a7, 93\n\t"
			"syscall 0\n\t"

			"1:\n\t"
			: "=r"(ret)
			: "r"((long)220), "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "memory", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$ra"
		);

		return (pid_t)ret;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __loongarch64_jmpbuf[22];

__asm__(
	".text\n"
	".weak __loongarch64_setjmp\n"
	".type __loongarch64_setjmp, @function\n"
	"__loongarch64_setjmp:\n"
	"st.d $ra, $a0, 0\n"
	"st.d $sp, $a0, 8\n"
	"st.d $r21, $a0, 16\n"
	"st.d $fp, $a0, 24\n"
	"st.d $s0, $a0, 32\n"
	"st.d $s1, $a0, 40\n"
	"st.d $s2, $a0, 48\n"
	"st.d $s3, $a0, 56\n"
	"st.d $s4, $a0, 64\n"
	"st.d $s5, $a0, 72\n"
	"st.d $s6, $a0, 80\n"
	"st.d $s7, $a0, 88\n"
	"st.d $s8, $a0, 96\n"
#ifndef __loongarch_soft_float
	"fst.d $fs0, $a0, 104\n"
	"fst.d $fs1, $a0, 112\n"
	"fst.d $fs2, $a0, 120\n"
	"fst.d $fs3, $a0, 128\n"
	"fst.d $fs4, $a0, 136\n"
	"fst.d $fs5, $a0, 144\n"
	"fst.d $fs6, $a0, 152\n"
	"fst.d $fs7, $a0, 160\n"
#endif
	"move $a0, $zero\n"
	"jr $ra\n"
	".size __loongarch64_setjmp, .-__loongarch64_setjmp\n"
);

__asm__(
	".text\n"
	".weak __loongarch64_longjmp\n"
	".type __loongarch64_longjmp, @function\n"
	"__loongarch64_longjmp:\n"
	"ld.d $ra, $a0, 0\n"
	"ld.d $sp, $a0, 8\n"
	"ld.d $r21, $a0, 16\n"
	"ld.d $fp, $a0, 24\n"
	"ld.d $s0, $a0, 32\n"
	"ld.d $s1, $a0, 40\n"
	"ld.d $s2, $a0, 48\n"
	"ld.d $s3, $a0, 56\n"
	"ld.d $s4, $a0, 64\n"
	"ld.d $s5, $a0, 72\n"
	"ld.d $s6, $a0, 80\n"
	"ld.d $s7, $a0, 88\n"
	"ld.d $s8, $a0, 96\n"
#ifndef __loongarch_soft_float
	"fld.d $fs0, $a0, 104\n"
	"fld.d $fs1, $a0, 112\n"
	"fld.d $fs2, $a0, 120\n"
	"fld.d $fs3, $a0, 128\n"
	"fld.d $fs4, $a0, 136\n"
	"fld.d $fs5, $a0, 144\n"
	"fld.d $fs6, $a0, 152\n"
	"fld.d $fs7, $a0, 160\n"
#endif
	"sltui $a0, $a1, 1\n"
	"add.d $a0, $a0, $a1\n"
	"jr $ra\n"
	".size __loongarch64_longjmp, .-__loongarch64_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
