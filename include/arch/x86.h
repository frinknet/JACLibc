/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef ARCH_X86_H
#define ARCH_X86_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* i386 syscall assembly */
static inline long __arch_syscall(long number, long a1, long a2, long a3, long a4, long a5, long a6) {
	long result;
	__asm__ volatile ("int $0x80"
		: "=a"(result)
		: "a"(number), "b"(a1), "c"(a2), "d"(a3), "S"(a4), "D"(a5)
		: "memory");

	(void)a6; /* i386 only supports 5 args */

	return result;
}

#ifdef __cplusplus
}
#endif

#endif // ARCH_X86_H
