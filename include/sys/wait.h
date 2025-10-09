// (c) 2025 FRINKnet & Friends - MIT licence
#ifndef SYS_WAIT_H
#define SYS_WAIT_H
#pragma once

#include <config.h>
#include <errno.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Status macros
#define WEXITSTATUS(s) (((s) & 0xff00) >> 8)
#define WTERMSIG(s) ((s) & 0x7f)
#define WSTOPSIG(s) WEXITSTATUS(s)
#define WIFEXITED(s) (!WTERMSIG(s))
#define WIFSIGNALED(s) (((s)&0xffff)-1U < 0xffu)
#define WIFSTOPPED(s) ((short)((((s)&0xffff)*0x10001)>>8) > 0x7f00)
#define WCOREDUMP(s) ((s) & 0x80)

#define WNOHANG    1
#define WUNTRACED  2

#if defined(JACL_OS_WINDOWS)
// Windows implementation
#include <windows.h>
#include <io.h>
static inline pid_t wait(int *status) {
	// No OS-wide process tracking, so just fail. Implement if you keep a process table.
	errno = ENOSYS;
	return -1;
}
static inline pid_t waitpid(pid_t pid, int *status, int options) {
	HANDLE h = (HANDLE)_get_osfhandle(pid);
	if (!h || h == INVALID_HANDLE_VALUE) {
		errno = EINVAL;
		return -1;
	}
	DWORD ms = (options & WNOHANG) ? 0 : INFINITE;
	DWORD r = WaitForSingleObject(h, ms);
	if (r == WAIT_OBJECT_0) {
		DWORD exitCode = 0;
		if (GetExitCodeProcess(h, &exitCode) && status)
			*status = (int)(exitCode << 8);
		return pid;
	}
	if (r == WAIT_TIMEOUT) return 0;
	errno = ECHILD;
	return -1;
}
#elif defined(JACL_ARCH_WASM)
// WASM/unsupported implementation---
static inline pid_t wait(int *status)         { (void)status; errno = ENOSYS; return -1; }
static inline pid_t waitpid(pid_t pid, int *status, int options)
                              { (void)pid; (void)status; (void)options; errno = ENOSYS; return -1; }
#else
// POSIX implementation
#include <sys/syscall.h>
#include <unistd.h>
static inline pid_t wait(int *status) {
	return (pid_t)syscall(SYS_wait4, -1, status, 0, NULL);
}
static inline pid_t waitpid(pid_t pid, int *status, int options) {
	return (pid_t)syscall(SYS_wait4, pid, status, options, NULL);
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYS_WAIT_H */
