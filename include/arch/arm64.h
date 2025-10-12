/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef ARCH_ARM64_H
#define ARCH_ARM64_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* ARM64 syscall assembly */
static inline long __arm64_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
	long result;
	register long x8 __asm__("x8") = num;
	register long x0 __asm__("x0") = a1;
	register long x1 __asm__("x1") = a2;
	register long x2 __asm__("x2") = a3;
	register long x3 __asm__("x3") = a4;
	register long x4 __asm__("x4") = a5;
	register long x5 __asm__("x5") = a6;

	__asm__ volatile ("svc #0"
		: "=r"(x0)
		: "r"(x8), "r"(x0), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5)
		: "memory");

	result = x0;

	return result;
}

#ifdef __cplusplus
}
#endif

#endif // ARCH_ARM64_H
