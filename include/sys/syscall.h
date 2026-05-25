// (c) 2025 Frinknet & Friends – MIT licence
#ifndef _SYS_SYSCALL_H
#define _SYS_SYSCALL_H
#pragma once

#include <config.h>
#include <stdarg.h>

// include syscall numbers
typedef enum {
#define X(SYS, num, ...) SYS = (num),
#include JACL_X_SYSCALL
#undef X
	sys_max
} syscall_t;

// include os-specific syscall
#define __OS_SYSCALL
#include JACL_OS_FILE

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __jacl_file file;
extern thread_local int errno;
extern file* stderr;
int fprintf(file* restrict stream, const char* restrict fmt, ...);

static inline long __sys_error(long result) {
	if (result < 0 && result >= -4095) {
		errno = -result;

		return -1;
	}

	return result;
}

#define __sys_args1(num)                    __sys_error(__jacl_os_syscall(num, 0, 0, 0, 0, 0, 0))
#define __sys_args2(num, a)                 __sys_error(__jacl_os_syscall(num, (long)(a), 0, 0, 0, 0, 0))
#define __sys_args3(num, a, b)              __sys_error(__jacl_os_syscall(num, (long)(a), (long)(b), 0, 0, 0, 0))
#define __sys_args4(num, a, b, c)           __sys_error(__jacl_os_syscall(num, (long)(a), (long)(b), (long)(c), 0, 0, 0))
#define __sys_args5(num, a, b, c, d)        __sys_error(__jacl_os_syscall(num, (long)(a), (long)(b), (long)(c), (long)(d), 0, 0))
#define __sys_args6(num, a, b, c, d, e)     __sys_error(__jacl_os_syscall(num, (long)(a), (long)(b), (long)(c), (long)(d), (long)(e), 0))
#define __sys_args7(num, a, b, c, d, e, f)  __sys_error(__jacl_os_syscall(num, (long)(a), (long)(b), (long)(c), (long)(d), (long)(e), (long)(f)))

#define syscall(...) JACL_CONCAT_EXPAND(__sys_args, JACL_NARGS(__VA_ARGS__),)(__VA_ARGS__)
#define sysdebug(num, ...) ( \
	fprintf(stderr, "syscall(%s, %s)\n", #num, #__VA_ARGS__) \
	? __debug_syscall(num, __VA_ARGS__ +0, 0, 0, 0, 0, 0, 0) \
	: 0 \
)
static inline long __debug_syscall(long num, ...) {
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

	long result = __jacl_os_syscall(num, a1, a2, a3, a4, a5, a6);

	fprintf(stderr, "syscall(%d, %p, %p, %p, %p, %p, %p)\n",
		num,	(void*)a1, (void*)a2, (void*)a3, (void*)a4, (void*)a5, (void*)a6);

	// convert linux negative errno to posix -1 + errno
	if (result < 0 && result >= -4095) {
		errno = -result;

		fprintf(stderr, "error %d\n", errno);

		return -1;
	}

	fprintf(stderr, "returns %p\n", result);

	return result;
}

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SYSCALL_H */
