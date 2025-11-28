/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __OS_CONFIG
	#undef netbsd
	#undef elf
	#define JACL_OS netbsd
	#define JACL_OS_BSD 1
	#define JACL_OS_NETBSD 1
	#define JACL_FMT elf
	#define JACL_FMT_ELF
	#define __jacl_os_syscall __netbsd_syscall
#undef __OS_CONFIG
#endif

#ifdef __OS_SYSCALL
	#define __ARCH_SYSCALL
	#include JACL_HEADER(arch, JACL_ARCH)
	#define __netbsd_syscall __jacl_arch_syscall
#undef __OS_SYSCALL
#endif

#ifdef __OS_INIT
	static inline void __jacl_init_os(void) {
		/* noop */
	}
#undef __OS_INIT
#endif
