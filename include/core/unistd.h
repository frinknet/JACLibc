/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef CORE_UNISTD_H
#define CORE_UNISTD_H
#pragma once

#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void __jacl_lock_acquire(__jacl_lock_t* l);
extern void __jacl_lock_release(__jacl_lock_t* l);
extern void __jacl_fork_reset(void);

pid_t fork(void) {
	__jacl_lock_acquire(&__jacl_lock);

	pid_t pid = (pid_t)syscall(SYS_fork);

	if (pid == 0) {
		__jacl_fork_reset();
		__jacl_lock_release(&__jacl_lock);
	} else if (pid > 0) {
		__jacl_lock_release(&__jacl_lock);
	}

	return pid;
}

#if JACL_OS_DARWIN
pid_t vfork(void) {
	return fork();
}
#else
pid_t vfork(void) {
	__jacl_lock_acquire(&__jacl_lock);

	pid_t pid = (pid_t)syscall(SYS_vfork);

	if (pid == 0) {
		// Child: minimal reset ONLY, then exec ASAP
		__jacl_tls_off = 0;
		__jacl_tls_end = 0;
		// Do NOT release lock - child will exec or _exit
	} else if (pid > 0) {
		// Parent: resume after child execs/exits
		__jacl_lock_release(&__jacl_lock);
	}

	return pid;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* CORE_UNISTD_H */
