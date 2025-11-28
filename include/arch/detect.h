/* (c) 2025 FRINKnet & Friends – MIT licence */

#if defined(JACL_ARCH)
	#include JACL_HEADER(arch,JACL_ARCH)
#elif defined(__x86_64__) || defined(__amd64__)
	#include <arch/x64.h>
#elif defined(__i386__)
	#include <arch/x86.h>
#elif defined(__aarch64__)
	#include <arch/arm64.h>
#elif defined(__arm__)
	#include <arch/arm32.h>
#elif defined(__riscv) && defined(__riscv_xlen) && __riscv_xlen == 64
	#include <arch/riscv64.h>
#elif defined(__riscv)
	#include <arch/riscv32.h>
#elif defined(__wasm__)
	#include <arch/wasm.h>
#else
	#error "Unsupported architecture"
#endif
