/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef CORE_STACK_H
#define CORE_STACK_H

#include <config.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Stack canary guard value */
uintptr_t __stack_chk_guard = 0xdeadbeefcafebabeULL;

/* Called by compiler-inserted code when stack overflow is detected */
noreturn void __stack_chk_fail(void) {
	const char msg[] = "*** stack smashing detected ***\n";
	write(2, msg, sizeof(msg) - 1);
	abort();
}

#ifdef __cplusplus
}
#endif

#endif /* CORE_STACK_H */
