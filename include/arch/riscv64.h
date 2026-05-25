/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef riscv64
	#define JACL_ARCH riscv64
	#define JACL_ARCH_RISCV64 1
	#define __jacl_arch_syscall __riscv64_syscall
	#define __jacl_arch_tls_set __riscv64_set_tp_register
	#define __jacl_arch_tls_get __riscv64_get_tp_register
	#define __jacl_arch_clone_thread __riscv64_clone_thread
	#define __jacl_arch_setjmp        __riscv64_setjmp
	#define __jacl_arch_longjmp       __riscv64_longjmp
	#define __jacl_arch_jmpbuf        __riscv64_jmpbuf
	#define JACL_BITS 64
	#define JACL_ORDER 1234
	#define JACL_SIGSIZ 8
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __riscv64_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
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
	static inline void __riscv64_set_tp_register(void* addr) {
		__asm__ volatile("mv tp, %0" : : "r"(addr) : "memory");
	}

	static inline void* __riscv64_get_tp_register(void) {
		void* result;

		__asm__("mv %0, tp" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __ARCH_CLONE
	static inline pid_t __riscv64_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
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

typedef unsigned long __riscv64_jmpbuf[32];

__asm__(
	".text\n"
	".weak __riscv64_setjmp\n"
	".type __riscv64_setjmp, @function\n"
	"__riscv64_setjmp:\n"
	"sd s0, 0(a0)\n"
	"sd s1, 8(a0)\n"
	"sd s2, 16(a0)\n"
	"sd s3, 24(a0)\n"
	"sd s4, 32(a0)\n"
	"sd s5, 40(a0)\n"
	"sd s6, 48(a0)\n"
	"sd s7, 56(a0)\n"
	"sd s8, 64(a0)\n"
	"sd s9, 72(a0)\n"
	"sd s10, 80(a0)\n"
	"sd s11, 88(a0)\n"
	"sd sp, 96(a0)\n"
	"sd ra, 104(a0)\n"
#ifndef __riscv_float_abi_soft
#ifdef __riscv_float_abi_double
	"fsd fs0, 112(a0)\n"
	"fsd fs1, 120(a0)\n"
	"fsd fs2, 128(a0)\n"
	"fsd fs3, 136(a0)\n"
	"fsd fs4, 144(a0)\n"
	"fsd fs5, 152(a0)\n"
	"fsd fs6, 160(a0)\n"
	"fsd fs7, 168(a0)\n"
	"fsd fs8, 176(a0)\n"
	"fsd fs9, 184(a0)\n"
	"fsd fs10, 192(a0)\n"
	"fsd fs11, 200(a0)\n"
#else
	"fsw fs0, 112(a0)\n"
	"fsw fs1, 120(a0)\n"
	"fsw fs2, 128(a0)\n"
	"fsw fs3, 136(a0)\n"
	"fsw fs4, 144(a0)\n"
	"fsw fs5, 152(a0)\n"
	"fsw fs6, 160(a0)\n"
	"fsw fs7, 168(a0)\n"
	"fsw fs8, 176(a0)\n"
	"fsw fs9, 184(a0)\n"
	"fsw fs10, 192(a0)\n"
	"fsw fs11, 200(a0)\n"
#endif
#endif
	"li a0, 0\n"
	"ret\n"
	".size __riscv64_setjmp, .-__riscv64_setjmp\n"
);

__asm__(
	".text\n"
	".weak __riscv64_longjmp\n"
	".type __riscv64_longjmp, @function\n"
	"__riscv64_longjmp:\n"
	"ld s0, 0(a0)\n"
	"ld s1, 8(a0)\n"
	"ld s2, 16(a0)\n"
	"ld s3, 24(a0)\n"
	"ld s4, 32(a0)\n"
	"ld s5, 40(a0)\n"
	"ld s6, 48(a0)\n"
	"ld s7, 56(a0)\n"
	"ld s8, 64(a0)\n"
	"ld s9, 72(a0)\n"
	"ld s10, 80(a0)\n"
	"ld s11, 88(a0)\n"
	"ld sp, 96(a0)\n"
	"ld ra, 104(a0)\n"
#ifndef __riscv_float_abi_soft
#ifdef __riscv_float_abi_double
	"fld fs0, 112(a0)\n"
	"fld fs1, 120(a0)\n"
	"fld fs2, 128(a0)\n"
	"fld fs3, 136(a0)\n"
	"fld fs4, 144(a0)\n"
	"fld fs5, 152(a0)\n"
	"fld fs6, 160(a0)\n"
	"fld fs7, 168(a0)\n"
	"fld fs8, 176(a0)\n"
	"fld fs9, 184(a0)\n"
	"fld fs10, 192(a0)\n"
	"fld fs11, 200(a0)\n"
#else
	"flw fs0, 112(a0)\n"
	"flw fs1, 120(a0)\n"
	"flw fs2, 128(a0)\n"
	"flw fs3, 136(a0)\n"
	"flw fs4, 144(a0)\n"
	"flw fs5, 152(a0)\n"
	"flw fs6, 160(a0)\n"
	"flw fs7, 168(a0)\n"
	"flw fs8, 176(a0)\n"
	"flw fs9, 184(a0)\n"
	"flw fs10, 192(a0)\n"
	"flw fs11, 200(a0)\n"
#endif
#endif
	"seqz a0, a1\n"
	"add a0, a0, a1\n"
	"ret\n"
	".size __riscv64_longjmp, .-__riscv64_longjmp\n"
);

#undef __ARCH_JUMP
#endif

#ifdef __cplusplus
	}
#endif
