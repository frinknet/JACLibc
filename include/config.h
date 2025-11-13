/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef CONFIG_H
#define CONFIG_H
#pragma once

#define JACL_VERSION 1.83
#define JACL_RELEASE 202511L

// avoid compile screams
#if defined(__GNUC__) || defined(__clang__)
  #pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
  #pragma GCC diagnostic ignored "-Wincompatible-library-redeclaration"
#endif /* __GNUC__ || __clang__ */

// useful macros
#define JACL_QUOTE(x) #x
#define JACL_EXPAND(x) x
#define JACL_CONCAT(a, b, space) a##space##b
#define JACL_CONCAT_EXPAND(a, b, space) JACL_CONCAT(a,b,space)
#define JACL_INIT(name) static void __attribute__((constructor)) JACL_CONCAT_EXPAND(__jacl_init_##name, __LINE__, _)(void)
#define JACL_HEADER(dir, file) <dir/file.h>

// standard version
#if defined(__STDC_VERSION__)
	#if __STDC_VERSION__ >= 202311L
		#define JACL_STDC_VERSION 2023
	#elif __STDC_VERSION__ >= 201710L
		#define JACL_STDC_VERSION 2017
	#elif __STDC_VERSION__ >= 201112L
		#define JACL_STDC_VERSION 2011
	#elif __STDC_VERSION__ >= 199901L
		#define JACL_STDC_VERSION 1999
	#else /* too old to care */
		#define JACL_STDC_VERSION 1989
	#endif /* __STDC_VERSION__ */
#else
	#define JACL_STDC_VERSION 1989
#endif /* __STDC_VERSION__ */

// has standard
#define JACL_HAS_C99 (JACL_STDC_VERSION >= 1999)
#define JACL_HAS_C11 (JACL_STDC_VERSION >= 2011)
#define JACL_HAS_C17 (JACL_STDC_VERSION >= 2017)
#define JACL_HAS_C23 (JACL_STDC_VERSION >= 2023)

// C99 features polyfill
#if !JACL_HAS_C99
  #define inline

	#if defined(__GNUC__) || defined(__clang__)
    #define restrict __restrict__
	#elif defined(_MSC_VER)
    #define restrict __restrict
	#else /* no restrict */
    #define restrict
	#endif /* __GNUC__ || __clang__ */
#endif /* JACL_HAS_C99 */

// C11 features polyfill
#if !JACL_HAS_C11
	#define _Static_assert(cond, msg) typedef char JACL_CONCAT(static_assertion_,__LINE__)[(cond) ? 1 : -1]
	#define _Atomic volatile

  #if defined(__GNUC__) || defined(__clang__) || defined(__SUNPRO_C) || defined(__TINYC__)
    #define _Thread_local __thread
  #elif defined(_MSC_VER)
    #define _Thread_local __declspec(thread)
  #else /* no thread local */
    #define _Thread_local
  #endif /* __GNUC__ || __clang__ || __SUNPRO_C || __TINY_CC__ */

	#if defined(_WIN32) || defined(_WIN64)
		#define _Alignas(x) __declspec(align(x))
		#define _Alignof(t) __alignof(t)
	#elif defined(__GNUC__) || defined(__clang__)
		#define _Alignas(x) __attribute__((aligned(x)))
		#define _Alignof(t) __alignof__(t)
	#else /* default alignment */
		#define aoffset(type, member) ((unsigned long)&(((type*)0)->member))
		#define _Alignas(x)
		#define _Alignof(t) aoffset(struct { char c; t m; }, m)
	#endif /* allignment */
#endif /* !JACL_HAS_C11 */

// C23 features
#if !JACL_HAS_C23
	#ifndef __cplusplus
		#define static_assert _Static_assert
	#endif

	#if !defined(__cplusplus)
  	#define thread_local _Thread_local
	#endif /* !__cplusplus */
#endif /* !JACL_HAS_C23 */

// thread support detection
#if defined(_POSIX_THREADS) || defined(__pthreads__)
  #define JACL_HAS_PTHREADS 1
#else /* no pthreads */
  #define JACL_HAS_PTHREADS 0
#endif /* pthreads */

// Feature attribute detection
#ifndef __has_attribute
  #define __has_attribute(x) 0
#endif /* !__has_attribute */

// Feature attribute detection
#ifndef __has_builtin
  #define __has_builtin(x) 0
#endif /* !__has_builtin */

// Check for atomic builtin support
#if __has_builtin(__atomic_load_n) && __has_builtin(__atomic_compare_exchange_n) && __has_builtin(__atomic_store_n)
	#define JACL_HAS_ATOMIC_BUILTINS 1
#else /* no atomic builtins */
	#define JACL_HAS_ATOMIC_BUILTINS 0
#endif /*atomic builtins */

// architecture
#if defined(__i386__)
	#undef x86
	#define JACL_ARCH x86
	#define JACL_ARCH_X86 1
	#define __jacl_arch_syscall __x86_syscall
	#define JACL_BITS 32
#elif defined(__x86_64__) || defined(__amd64__)
	#undef x64
	#define JACL_ARCH x64
	#define JACL_ARCH_X64 1
	#define __jacl_arch_syscall __x64_syscall
	#define JACL_BITS 64
#elif defined(__aarch64__)
	#undef arm64
	#define JACL_ARCH arm64
	#define JACL_ARCH_ARM64 1
	#define __jacl_arch_syscall __arm64_syscall
	#define JACL_BITS 64
#elif defined(__arm__) || defined(_ARM_) || defined(_M_ARM)
	#undef arm32
	#define JACL_ARCH arm32
	#define JACL_ARCH_ARM32 1
	#define __jacl_arch_syscall __arm32_syscall
	#define JACL_BITS 32
#elif defined(__riscv) || defined(__riscv__)
	#undef riscv
	#define JACL_ARCH riscv
	#define JACL_ARCH_RISCV 1
	#define __jacl_arch_syscall __riscv_syscall
	#if defined(__riscv_xlen) && __riscv_xlen == 64
		#define JACL_BITS 64
	#else /* 32bit risc */
		#define JACL_BITS 32
	#endif /* bit check */
#elif defined(__wasm__)
	#undef wasm
	#define JACL_ARCH wasm
	#define JACL_ARCH_WASM 1
	#define __jacl_arch_syscall __wasm_syscall
	#define JACL_BITS 32
	#include <arch/wasm_helpers.h>
#else /* unknown */
	#error "JACLibc - Unsupported Architecture"
#endif /* arch check */

// operating system
#if defined(__linux__)
	#undef linux
	#define JACL_OS linux
	#define JACL_OS_LINUX 1
	#define __jacl_os_syscall __linux_syscall
#elif defined(_WIN32)
	#undef windows
	#define JACL_OS windows
	#define JACL_OS_WINDOWS 1
	#define __jacl_os_syscall __windows_syscall
#elif defined(__APPLE__) && defined(__MACH__)
	#undef darwin
	#define JACL_OS darwin
	#define JACL_OS_DARWIN 1
	#define __jacl_os_syscall __darwin_syscall
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
	#undef bsd
	#define JACL_OS bsd
	#define JACL_OS_BSD 1
	#define __jacl_os_syscall __bsd_syscall
#elif defined(__wasi__)
	#undef wasi
	#define JACL_OS wasi
	#define JACL_OS_WASI 1
	#define __jacl_os_syscall __wasi_syscall
#elif defined(__wasm__)
	#undef jsrun
	#define JACL_OS jsrun
	#define JACL_OS_JSRUN 1
	#define __jacl_os_syscall __jsrun_syscall
#else /* unknown */
	#error "JACLibc - Unsupported Operating System"
#endif /* os check */

// bit depth
#define JACL_16BIT (JACL_BITS == 16)
#define JACL_32BIT (JACL_BITS == 32)
#define JACL_64BIT (JACL_BITS == 64)

// Long double precision bits
#if defined(JACL_ARCH_X86) || defined(JACL_ARCH_X64)
  #if defined(_MSC_VER)
    #define JACL_LDBL_BITS 64   // MSVC: long double == double
  #else /* linux has 80bit long doubles */
    #define JACL_LDBL_BITS 80   // GCC/Clang: 80-bit extended
  #endif
#elif defined(JACL_ARCH_ARM64) && defined(JACL_OS_LINUX) && defined(__LONG_DOUBLE_128__)
  #define JACL_LDBL_BITS 128    // ARM64 quad precision
#else /* arm64 has 128bit long doubles but regular arm has 64bit */
  #define JACL_LDBL_BITS 64     // Everywhere else: double
#endif /* long double bits check */

// Intenter max precision bits
#ifdef __INTMAX_WIDTH__
  #define JACL_INTMAX_BITS __INTMAX_WIDTH__
#else
  #if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
    #define JACL_INTMAX_BITS 64
  #elif defined(__LONG_LONG_MAX__)
    #define JACL_INTMAX_BITS 64
  #elif defined(LONG_MAX) && LONG_MAX > 2147483647L
    #define JACL_INTMAX_BITS 64
  #else
    #define JACL_INTMAX_BITS 32
  #endif
#endif

// HAS large file support
#if JACL_64BIT
  #define JACL_HAS_LFS 1
#elif defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)
  #define JACL_HAS_LFS 1
#else /* no large file support */
  #define JACL_HAS_LFS 0
#endif /* large file support check */

// Has POSIX
#ifdef JACL_OS_WINDOWS
	#define JACL_HAS_POSIX 0
#else /* everything else */
	#define JACL_HAS_POSIX 1
#endif /* posix check */

//Compiler Optimization
#if __has_builtin(__builtin_expect)
		#define JACL_LIKELY(x)   __builtin_expect(!!(x), 1)
		#define JACL_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else /* notexpectation builtin */
		#define JACL_LIKELY(x)   (x)
		#define JACL_UNLIKELY(x) (x)
#endif /* __builtin_expect */

// Builtin Prefetch Polyfill
#if !__has_builtin(__builtin_prefetch)
	#define __builtin_prefetch(addr, rw, locality) ((void)(addr))
#endif /* __builtin_prefetch */

// Builtin Trap Polyfill
#if !__has_builtin(__builtin_trap)
	#if JACL_ARCH_WASM
		#define __builtin_trap() __asm__ volatile("unreachable")
	#else
		#define __builtin_trap() do { *(volatile int*)0 = 0; } while(0)
	#endif
#endif /* __builtin_trap */

// Builtin Popcount Polyfill
#if !__has_builtin(__builtin_popcount)
static inline int __builtin_popcount(unsigned int x) {
	x = x - ((x >> 1) & 0x55555555);
	x = (x & 0x33333333) + ((x >> 2) & 0x33333333);

	return (((x + (x >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}
#endif /* __builtin_popcount */

// Builtin Frame Polyfill
#if !__has_builtin(__builtin_frame_address)
static inline void* __jacl_frame_address(int level) {
	if (level == 0) {
		volatile char frame_marker;
		return (void*)&frame_marker;
	}
	return NULL;
}
#endif /* __builtin_frame_address */

// Fast math detection
#ifndef JACL_FAST_MATH
  #if defined(__FAST_MATH__) || defined(__FINITE_MATH_ONLY__)
    #define JACL_FAST_MATH 1
  #else /* no fast math */
    #define JACL_FAST_MATH 0
  #endif /* fast math check */
#endif /* !JACL_FAST_MATH */

// SIMD intrinsics
#ifndef JACL_HAS_IMMINTRIN
  #if JACL_HAS_C99 && (defined(__SSE2__) || defined(__ARM_NEON) || defined(__wasm_simd128__))
    #define JACL_HAS_IMMINTRIN 1
  #else /* no immintrin */
    #define JACL_HAS_IMMINTRIN 0
  #endif /* immintrin check */
#endif /* !JACL_HAS_IMMINTRIN */

// Safety check macro - compiles away in fast-math mode
#if JACL_FAST_MATH
  #define JACL_SAFETY(chk, rtn) do {} while(0)
#else /* safe but slower */
  #define JACL_SAFETY(chk, rtn) if (chk) return (rtn)
#endif /* JACL_FAST_MATH */

#endif // CONFIG_H
