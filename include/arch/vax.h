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
	#define JACL_BITS 32
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __vax_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long r0_reg __asm__("r0") = num;
		register long r1_reg __asm__("r1") = a1;
		register long r2_reg __asm__("r2") = a2;
		register long r3_reg __asm__("r3") = a3;
		register long r4_reg __asm__("r4") = a4;
		register long r5_reg __asm__("r5") = a5;

		(void)a6;

		__asm__ volatile ("chmk %1"
			: "=r"(r0_reg)
			: "i"(0), "r"(r0_reg), "r"(r1_reg), "r"(r2_reg), "r"(r3_reg), "r"(r4_reg), "r"(r5_reg)
			: "memory");

		result = r0_reg;

		return result;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".global _start\n"
		"_start:\n"
		"clrl %fp\n"
		"movl %sp, %r0\n"
		"bicl3 $15, %sp, %sp\n"
		"calls $1, _start_main\n"
	);
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __vax_set_tp_register(void* addr) {
		__asm__ volatile("movl %0, %%r6" : : "r"(addr) : "memory");
	}

	static inline void* __vax_get_tp_register(void) {
		void* result;

		__asm__("movl %%r6, %0" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __cplusplus
	}
#endif

