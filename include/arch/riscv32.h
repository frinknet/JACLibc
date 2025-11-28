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
	#define JACL_BITS 32
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
		".global _start\n"
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

#ifdef __cplusplus
	}
#endif
