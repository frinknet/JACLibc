/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __OS_CONFIG
	#undef darwin
	#undef macho
	#define JACL_OS darwin
	#define JACL_OS_DARWIN 1
	#define JACL_FMT macho
	#define JACL_FMT_MACHO
	#define __jacl_os_syscall __darwin_syscall
#undef __OS_CONFIG
#endif

#ifdef __OS_SYSCALL
	#define __ARCH_SYSCALL
	#include JACL_HEADER(arch, JACL_ARCH)
	#define __darwin_syscall __jacl_arch_syscall
#undef __OS_SYSCALL
#endif

#ifdef __OS_INIT
static inline void __jacl_init_os(void) {
	/* noop */
}
#undef __OS_INIT
#endif
