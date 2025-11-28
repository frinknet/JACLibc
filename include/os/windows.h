/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __OS_CONFIG
	#undef windows
	#define JACL_OS windows
	#define JACL_OS_WINDOWS 1
	#define __jacl_os_syscall __windows_syscall
#undef __OS_CONFIG
#endif

#ifdef __OS_SYSCALL
#undef __OS_SYSCALL
#endif

#ifdef __OS_INIT
	static inline void __jacl_init_os(void) {
		/* noop */
	}
#undef __OS_INIT
#endif
