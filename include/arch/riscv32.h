/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef riscv32
	#define JACL_ARCH riscv32
	#define JACL_ARCH_RISCV32 1
	#define __jacl_arch_syscall __riscv32_syscall
	#define __jacl_arch_tls_set __riscv32_set_tp_register
	#define __jacl_arch_tls_get __riscv32_get_tp_register
	#define __jacl_arch_clone_thread __riscv32_clone_thread
	#define __jacl_arch_setjmp        __riscv32_setjmp
	#define __jacl_arch_longjmp       __riscv32_longjmp
	#define __jacl_arch_jmpbuf        __riscv32_jmpbuf
	#define JACL_BITS 32
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __riscv32_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long a7 __asm__("a7") = num;
		register long a0 __asm__("a0") = a1;
		register long a1_reg __asm__("a1") = a2;
		register long a2_reg __asm__("a2") = a3;
		register long a3_reg __asm__("a3") = a4;
		register long a4_reg __asm__("a4") = a5;
		register long a5_reg __asm__("a5") = a6;

		__asm__ volatile ("ecall"
			: "=r"(a0)
			: "r"(a7), "r"(a0), "r"(a1_reg), "r"(a2_reg), "r"(a3_reg), "r"(a4_reg), "r"(a5_reg)
			: "memory");

		result = a0;

		return result;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".globl _start\n"
		"_start:\n"
		"li fp, 0\n"
		"li ra, 0\n"
		"mv a0, sp\n"
		"andi sp, sp, -16\n"
		"call _start_main\n"
	);
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __riscv32_set_tp_register(void* addr) {
		__asm__ volatile("mv tp, %0" : : "r"(addr) : "memory");
	}

	static inline void* __riscv32_get_tp_register(void) {
		void* result;

		__asm__("mv %0, tp" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __riscv32_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
		char *stack_top = (char *)stack + stack_size;
		stack_top = (char *)((uintptr_t)stack_top & ~15UL) - 16;

		int flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD;
		long ret;

		__asm__ volatile(
			"mv a0, %2\n\t"
			"mv a1, %3\n\t"
			"li a2, 0\n\t"
			"li a3, 0\n\t"
			"li a4, 0\n\t"
			"li a7, 220\n\t"
			"ecall\n\t"
			"bnez a0, 1f\n\t"

			"li s0, 0\n\t"
			"mv a0, %5\n\t"
			"jalr %4\n\t"
			"li a7, 93\n\t"
			"ecall\n\t"

			"1:\n\t"
			"mv %0, a0\n\t"
			: "=r"(ret)
			: "r"((long)220), "r"((long)flags), "r"(stack_top), "r"(fn), "r"(arg)
			: "a0", "a1", "a2", "a3", "a4", "a7", "memory"
		);

		return ret;
	}
#undef __ARCH_CLONE
#endif

#ifdef __ARCH_JUMP

typedef unsigned long __riscv32_jmpbuf[40];

__asm__(
	".text\n"
	".weak __riscv32_setjmp\n"
	".type __riscv32_setjmp, @function\n"
	"__riscv32_setjmp:\n"
	"sw s0, 0(a0)\n"
	"sw s1, 4(a0)\n"
	"sw s2, 8(a0)\n"
	"sw s3, 12(a0)\n"
	"sw s4, 16(a0)\n"
	"sw s5, 20(a0)\n"
	"sw s6, 24(a0)\n"
	"sw s7, 28(a0)\n"
	"sw s8, 32(a0)\n"
	"sw s9, 36(a0)\n"
	"sw s10, 40(a0)\n"
	"sw s11, 44(a0)\n"
	"sw sp, 48(a0)\n"
	"sw ra, 52(a0)\n"
#ifndef __riscv_float_abi_soft
#ifdef __riscv_float_abi_double
	"fsd fs0, 56(a0)\n"
	"fsd fs1, 64(a0)\n"
	"fsd fs2, 72(a0)\n"
	"fsd fs3, 80(a0)\n"
	"fsd fs4, 88(a0)\n"
	"fsd fs5, 96(a0)\n"
	"fsd fs6, 104(a0)\n"
	"fsd fs7, 112(a0)\n"
	"fsd fs8, 120(a0)\n"
	"fsd fs9, 128(a0)\n"
	"fsd fs10, 136(a0)\n"
	"fsd fs11, 144(a0)\n"
#else
	"fsw fs0, 56(a0)\n"
	"fsw fs1, 64(a0)\n"
	"fsw fs2, 72(a0)\n"
	"fsw fs3, 80(a0)\n"
	"fsw fs4, 88(a0)\n"
	"fsw fs5, 96(a0)\n"
	"fsw fs6, 104(a0)\n"
	"fsw fs7, 112(a0)\n"
	"fsw fs8, 120(a0)\n"
	"fsw fs9, 128(a0)\n"
	"fsw fs10, 136(a0)\n"
	"fsw fs11, 144(a0)\n"
#endif
#endif
	"li a0, 0\n"
	"ret\n"
	".size __riscv32_setjmp, .-__riscv32_setjmp\n"
);

__asm__(
	".text\n"
	".weak __riscv32_longjmp\n"
	".type __riscv32_longjmp, @function\n"
	"__riscv32_longjmp:\n"
	"lw s0, 0(a0)\n"
	"lw s1, 4(a0)\n"
	"lw s2, 8(a0)\n"
	"lw s3, 12(a0)\n"
	"lw s4, 16(a0)\n"
	"lw s5, 20(a0)\n"
	"lw s6, 24(a0)\n"
	"lw s7, 28(a0)\n"
	"lw s8, 32(a0)\n"
	"lw s9, 36(a0)\n"
	"lw s10, 40(a0)\n"
	"lw s11, 44(a0)\n"
	"lw sp, 48(a0)\n"
	"lw ra, 52(a0)\n"
#ifndef __riscv_float_abi_soft
#ifdef __riscv_float_abi_double
	"fld fs0, 56(a0)\n"
	"fld fs1, 64(a0)\n"
	"fld fs2, 72(a0)\n"
	"fld fs3, 80(a0)\n"
	"fld fs4, 88(a0)\n"
	"fld fs5, 96(a0)\n"
	"fld fs6, 104(a0)\n"
	"fld fs7, 112(a0)\n"
	"fld fs8, 120(a0)\n"
	"fld fs9, 128(a0)\n"
	"fld fs10, 136(a0)\n"
	"fld fs11, 144(a0)\n"
#else
	"flw fs0, 56(a0)\n"
	"flw fs1, 64(a0)\n"
	"flw fs2, 72(a0)\n"
	"flw fs3, 80(a0)\n"
	"flw fs4, 88(a0)\n"
	"flw fs5, 96(a0)\n"
	"flw fs6, 104(a0)\n"
	"flw fs7, 112(a0)\n"
	"flw fs8, 120(a0)\n"
	"flw fs9, 128(a0)\n"
	"flw fs10, 136(a0)\n"
	"flw fs11, 144(a0)\n"
#endif
#endif
	"seqz a0, a1\n"
	"add a0, a0, a1\n"
	"ret\n"
	".size __riscv32_longjmp, .-__riscv32_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
