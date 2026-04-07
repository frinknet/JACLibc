/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef CORE_UNISTD_H
#define CORE_UNISTD_H
#pragma once

#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void __jacl_memlock_block(void);
extern void __jacl_memlock_clear(void);
extern void __jacl_memlock_reset(void);

pid_t fork(void) {
	__jacl_memlock_block();

	pid_t pid = (pid_t)syscall(SYS_fork);

	if (pid == 0) __jacl_memlock_reset();
	else if (pid > 0) __jacl_memlock_clear();

	return pid;
}

#if JACL_OS_DARWIN
pid_t vfork(void) {
	return fork();
}
#else
pid_t vfork(void) {
	// NO LOCKS. NO TLS RESET.
	// Parent is suspended; child shares all memory.
	// Caller MUST exec() or _exit() immediately.
	// Any allocation corrupts parent heap—by design.
	return (pid_t)syscall(SYS_vfork);
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* CORE_UNISTD_H */
