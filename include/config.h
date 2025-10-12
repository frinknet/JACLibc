/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef CONFIG_H
#define CONFIG_H
#pragma once

#define JACL_VERSION 1.34
#define JACL_RELEASE 202510L

// avoid compile screams
#if defined(__GNUC__) || defined(__clang__)
  #pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
  #pragma GCC diagnostic ignored "-Wincompatible-library-redeclaration"
#endif

// useful macros
#define JACL_QUOTE(x) #x
#define JACL_CONCAT(a,b,space) a##space##b
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
	#else
		#define JACL_STDC_VERSION 1989
	#endif
#else
	#define JACL_STDC_VERSION 1989
#endif

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
	#else
    #define restrict
	#endif
#endif

// C11 features polyfill
#if !JACL_HAS_C11
	#define _Static_assert(cond, msg) typedef char JACL_CONCAT(static_assertion_,__LINE__)[(cond) ? 1 : -1]
	#define _Atomic volatile

  #if defined(__GNUC__) || defined(__clang__) || defined(__SUNPRO_C) || defined(__TINYC__)
    #define _Thread_local __thread
  #elif defined(_MSC_VER)
    #define _Thread_local __declspec(thread)
  #else
    #define _Thread_local
  #endif

	#if defined(_WIN32) || defined(_WIN64)
		#define _Alignas(x) __declspec(align(x))
		#define _Alignof(t) __alignof(t)
	#elif defined(__GNUC__) || defined(__clang__)
		#define _Alignas(x) __attribute__((aligned(x)))
		#define _Alignof(t) __alignof__(t)
	#else
		#define aoffset(type, member) ((unsigned long)&(((type*)0)->member))
		#define _Alignas(x)
		#define _Alignof(t) aoffset(struct { char c; t m; }, m)
	#endif
#endif

// C23 features
#if !JACL_HAS_C23
	#if !defined(__cplusplus)
  	#define thread_local _Thread_local
	#endif
#endif

// thread support detection
#if defined(_POSIX_THREADS) || defined(__pthreads__)
  #define JACL_HAS_PTHREADS 1
#else
  #define JACL_HAS_PTHREADS 0
#endif

// Feature attribute detection
#ifndef __has_attribute
  #define __has_attribute(x) 0
#endif

// Feature attribute detection
#ifndef __has_builtin
  #define __has_builtin(x) 0
#endif

// Check for atomic builtin support
#if __has_builtin(__atomic_load_n) && __has_builtin(__atomic_compare_exchange_n) && __has_builtin(__atomic_store_n)
	#define JACL_HAS_ATOMIC_BUILTINS 1
#else
	#define JACL_HAS_ATOMIC_BUILTINS 0
#endif

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
	#else
		#define JACL_BITS 32
	#endif
#elif defined(__wasm__)
	#undef wasm
	#define JACL_ARCH wasm
	#define JACL_ARCH_WASM 1
	#define __jacl_arch_syscall __wasm_syscall
	#define JACL_BITS 32
	#include <arch/wasm_helpers.h>
#else
	#error "JACLibc - Unsupported Architecture"
#endif

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
#else
	#error "JACLibc - Unsupported Operating System"
#endif

// bit depth
#define JACL_16BIT (JACL_BITS == 16)
#define JACL_32BIT (JACL_BITS == 32)
#define JACL_64BIT (JACL_BITS == 64)

// large files
#if JACL_64BIT
  #define JACL_HAS_LFS 1
#elif defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)
  #define JACL_HAS_LFS 1
#else
  #define JACL_HAS_LFS 0
#endif

// Has POSIX
#ifdef JACL_OS_WINDOWS
	#define JACL_HAS_POSIX 0
#else
	#define JACL_HAS_POSIX 1
#endif

//Compiler Optimization
#if __has_builtin(__builtin_expect)
		#define JACL_LIKELY(x)   __builtin_expect(!!(x), 1)
		#define JACL_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
		#define JACL_LIKELY(x)   (x)
		#define JACL_UNLIKELY(x) (x)
#endif

// Builtin Polyfills
#if !__has_builtin(__builtin_unreachable)
	#define __builtin_unreachable()
#endif

#if !__has_builtin(__builtin_prefetch)
	#define __builtin_prefetch(addr, rw, locality) ((void)(addr))
#endif

#if !__has_builtin(__builtin_trap)
	#define __builtin_trap() do { *(volatile int*)0 = 0; } while(0)
#endif

#endif // CONFIG_H
