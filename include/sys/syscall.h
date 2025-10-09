// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef SYS_SYSCALL_H
#define SYS_SYSCALL_H
#pragma once

#include <config.h>
#include <stdarg.h>

// Include OS-specific syscall numbers
#include JACL_HEADER(os, JACL_OS)

#ifdef __cplusplus
extern "C" {
#endif

static inline long syscall(long num, ...) {
	va_list args;
	long a1, a2, a3, a4, a5, a6;

	va_start(args, num);
	a1 = va_arg(args, long);
	a2 = va_arg(args, long);
	a3 = va_arg(args, long);
	a4 = va_arg(args, long);
	a5 = va_arg(args, long);
	a6 = va_arg(args, long);
	va_end(args);

	return __os_syscall(num, a1, a2, a3, a4, a5, a6);
}

#ifdef __cplusplus
}
#endif

#endif // SYS_SYSCALL_H
