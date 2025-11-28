/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __cplusplus
	extern "C" {
#endif

#ifdef __ARCH_CONFIG
	#undef ia64
	#define JACL_ARCH ia64
	#define JACL_ARCH_IA64 1
	#define __jacl_arch_syscall __ia64_syscall
	#define __jacl_arch_tls_set __ia64_set_tp_register
	#define __jacl_arch_tls_get __ia64_get_tp_register
	#define JACL_BITS 64
#undef __ARCH_CONFIG
#endif

#ifdef __ARCH_SYSCALL
	static inline long __ia64_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long r15 __asm__("r15") = num;
		register long out0 __asm__("out0") = a1;
		register long out1 __asm__("out1") = a2;
		register long out2 __asm__("out2") = a3;
		register long out3 __asm__("out3") = a4;
		register long out4 __asm__("out4") = a5;
		register long out5 __asm__("out5") = a6;

		__asm__ volatile ("break 0x100000"
			: "=r"(r8), "=r"(r10)
			: "r"(r15), "r"(out0), "r"(out1), "r"(out2), "r"(out3), "r"(out4), "r"(out5)
			: "memory");

		result = r8;

		return result;
	}
#undef __ARCH_SYSCALL
#endif

#ifdef __ARCH_START
	__asm__(
		".global _start\n"
		"_start:\n"
		"{ .mlx\n"
		"alloc r2 = ar.pfs, 0, 0, 1, 0\n"
		"movl r32 = r12\n"
		";;\n"
		"}\n"
		"{ .mib\n"
		"mov r0 = 0\n"
		"br.call.sptk.many b0 = _start_main\n"
		";;\n"
		"}\n"
	);
#undef __ARCH_START
#endif

#ifdef __ARCH_TLS
	static inline void __ia64_set_tp_register(void* addr) {
		__asm__ volatile("mov r13 = %0" : : "r"(addr) : "memory");
	}

	static inline void* __ia64_get_tp_register(void) {
		void* result;

		__asm__("mov %0 = r13" : "=r"(result));

		return result;
	}
#undef __ARCH_TLS
#endif

#ifdef __cplusplus
	}
#endif

