// (c) 2025 FRINKnet & Friends - MIT licence
#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H
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
#define WIFSTOPPED(s) ((short)((((s)&0xffff)*0x10001U)>>8) >= 0x7f00)
#define WCOREDUMP(s) ((s) & 0x80)

#define WNOHANG    1
#define WUNTRACED  2

#if defined(JACL_OS_WINDOWS)
// Windows implementation
#include <windows.h>
#include <io.h>
static inline pid_t wait(int *status) {
	return (__errno_set(ENOSYS), -1);
}
static inline pid_t waitpid(pid_t pid, int *status, int options) {
	HANDLE h = (HANDLE)_get_osfhandle(pid);
	if (!h || h == INVALID_HANDLE_VALUE) {
		return (__errno_set(EINVAL), -1);
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
	return (__errno_set(ECHILD), -1);
}
#elif defined(JACL_ARCH_WASM)
static inline pid_t wait(int *status)  { (void)status; return (__errno_set(ENOSYS), -1); }
static inline pid_t waitpid(pid_t pid, int *status, int options) { (void)pid; (void)status; (void)options; return (__errno_set(ENOSYS), -1); }
#else
#include <sys/syscall.h>
#include <unistd.h>
static inline pid_t wait(int *status) { return (pid_t)syscall(SYS_wait4, -1, status, 0, NULL); }
static inline pid_t waitpid(pid_t pid, int *status, int options) { return (pid_t)syscall(SYS_wait4, pid, status, options, NULL); }
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SYS_WAIT_H */
