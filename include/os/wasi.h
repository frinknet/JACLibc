/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __OS_CONFIG
	#undef wasi
	#define JACL_OS wasi
	#define JACL_OS_WASI 1
	#define JACL_BIN wasm
	#define JACL_BIN_WASM
	#define __jacl_os_init     __wasi_init
	#define __jacl_os_syscall  __wasi_syscall
#undef __OS_CONFIG
#endif

#ifdef __OS_SYSCALL
#include <jsio.h>  // For js_read/js_write

static inline long __wasi_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
  switch ((int)num) {
    case SYS_read:
      return js_read((int)a1, (void*)a2, (size_t)a3);
    case SYS_write:
      return js_write((int)a1, (const void*)a2, (size_t)a3);

    // Add more cases as you implement them
    default:
      return 0; // Or return -1/ENOSYS for unsupported
  }
}
#undef __OS_SYSCALL
#endif

#ifdef __OS_INIT
	static inline void __wasi_init(void) {
		/* noop */
	}
#undef __OS_INIT
#endif
