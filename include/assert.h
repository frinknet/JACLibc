/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef ASSERT_H
#define ASSERT_H
#pragma once

#include <stdnoreturn.h>
#include <stdio.h>
#include <stdlib.h>

#undef assert
#ifdef NDEBUG
#define assert(x) (void)0
#else
#define assert(x) ((void)((x) || (__assert_fail(#x, __FILE__, __LINE__, __func__),0)))
#endif

#ifdef __cplusplus
extern "C" {
#endif

noreturn static inline void __assert_fail(const char *expr, const char *file, int line, const char *func) {
	fprintf(stderr, "Assertion failed: %s\nAt %s:%d, function: %s\n", expr, file, line, func);
	__builtin_trap();
}

#ifdef __cplusplus
}
#endif

#endif /* ASSERT_H */
