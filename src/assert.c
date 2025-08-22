// (c) 2025 FRINKnet & Friends - MIT licence
#ifndef ASSERT_C
#define ASSERT_C

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

#ifdef __cplusplus
extern "C" {
#endif

noreturn void __assert_fail(const char *expr, const char *file, int line, const char *func) {
		fprintf(stderr, "Assertion failed: %s\nAt %s:%d, function: %s\n", expr, file, line, func);
		__builtin_trap();
}

#ifdef __cplusplus
}
#endif

#endif /* ASSERT_C */

