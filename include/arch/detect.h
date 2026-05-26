/* (c) 2025-2026 FRINKnet & Friends – MIT licence */

#if defined(JACL_ARCH)
	#include JACL_HEADER(arch, JACL_ARCH)
#elif defined(__x86_64__) || defined(__amd64__) || defined(_M_X64)
	#include <arch/x64.h>
#elif defined(__i386__) || defined(__i386) || defined(_M_IX86)
	#include <arch/x86.h>
#elif defined(__aarch64__) || defined(_M_ARM64)
	#include <arch/arm64.h>
#elif defined(__arm__) || defined(_M_ARM)
	#include <arch/arm32.h>
#elif defined(__riscv) && __riscv_xlen == 64
	#include <arch/riscv64.h>
#elif defined(__riscv) && __riscv_xlen == 32
	#include <arch/riscv32.h>
#elif defined(__loongarch__) && __loongarch_grlen == 64
	#include <arch/loongarch64.h>
#elif defined(__mips__) && defined(_ABIN32)
	#include <arch/mipsn32.h>
#elif defined(__mips__) && (__mips == 64 || defined(__mips64))
	#include <arch/mips64.h>
#elif defined(__mips__)
	#include <arch/mips32.h>
#elif defined(__spu__) || defined(__SPU__)
	#include <arch/spu.h>
#elif defined(__powerpc64__) || defined(_ARCH_PPC64)
	#include <arch/powerpc64.h>
#elif defined(__powerpc__) || defined(_ARCH_PPC)
	#include <arch/powerpc32.h>
#elif defined(__s390x__)
	#include <arch/s390x.h>
#elif defined(__sparc__) && defined(__arch64__)
	#include <arch/sparc64.h>
#elif defined(__sparc__)
	#include <arch/sparc32.h>
#elif defined(__ia64__)
	#include <arch/ia64.h>
#elif defined(__hppa__)
	#include <arch/hppa.h>
#elif defined(__alpha__)
	#include <arch/alpha.h>
#elif defined(__m68k__)
	#include <arch/m68k.h>
#elif defined(__kvx__)
	#include <arch/kvx.h>
#elif defined(__sh__)
	#include <arch/sh.h>
#elif defined(__or1k__)
	#include <arch/or1k.h>
#elif defined(__nios2__)
	#include <arch/nios2.h>
#elif defined(__hexagon__)
	#include <arch/hexagon.h>
#elif defined(__xtensa__)
	#include <arch/xtensa.h>
#elif defined(__csky__)
	#include <arch/csky.h>
#elif defined(__arc__)
	#include <arch/arc.h>
#elif defined(__vax__)
	#include <arch/vax.h>
#elif defined(__microblaze__)
	#include <arch/microblaze.h>
#elif defined(__wasm) || defined(__wasm32__) || defined(__wasm64__)
	#include <arch/wasm.h>
#else
	#error "Unsupported Architecture"
#endif
