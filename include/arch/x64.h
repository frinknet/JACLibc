/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef ARCH_X64_H
#define ARCH_X64_H
#pragma once

#ifdef __cplusplus
	extern "C" {
#endif

	/* x86_64 syscall assembly */
	static inline long __x64_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
		long result;
		register long rax __asm__("rax") = num;
		register long rdi __asm__("rdi") = a1;
		register long rsi __asm__("rsi") = a2;
		register long rdx __asm__("rdx") = a3;
		register long r10 __asm__("r10") = a4;
		register long r8 __asm__("r8") = a5;
		register long r9 __asm__("r9") = a6;

		__asm__ volatile ("syscall"
			: "=a"(result)
			: "r"(rax), "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10), "r"(r8), "r"(r9)
			: "memory", "rcx", "r11");

		return result;
	}

#ifdef __cplusplus
	}
#endif

#endif // ARCH_X64_H
