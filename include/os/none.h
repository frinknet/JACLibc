/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __OS_CONFIG
	#undef none
	#undef elf
	#define JACL_OS none
	#define JACL_OS_NONE 1
	#define JACL_FMT elf
	#define JACL_FMT_ELF
	#define __jacl_os_syscall __none_syscall
#undef __OS_CONFIG
#endif

#ifdef __OS_SYSCALL
	// No OS means no syscalls—stub or trap
	static inline long __none_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		// Could trap to your own handler, or just fail
		return -ENOSYS;
	}
#undef __OS_SYSCALL
#endif

#ifdef __OS_INIT
	static inline void __jacl_init_os(void) {
		// No OS: set up bootstrap TLS manually
		static char _bootstrap_tls[256] __attribute__((aligned(16)));
		*(void**)_bootstrap_tls = _bootstrap_tls;  // self-pointer

		__jacl_arch_tls_set(_bootstrap_tls);
	}
#undef __OS_INIT
#endif

