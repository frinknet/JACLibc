/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef ARCH_X86_H
#define ARCH_X86_H
#pragma once

/* i386 uses same syscall numbers as x86_64 for most calls */
#define SYS_read      0
#define SYS_write     1
#define SYS_open      2
#define SYS_close     3
#define SYS_stat      4
#define SYS_fstat     5
#define SYS_lstat     6
#define SYS_lseek     8
#define SYS_mmap      9
#define SYS_mprotect  10
#define SYS_munmap    11
#define SYS_brk       12
#define SYS_nanosleep 35
#define SYS_getpid    39
#define SYS_clone     56
#define SYS_fork      57
#define SYS_execve    59
#define SYS_exit      60
#define SYS_kill      62
#define SYS_uname     63
#define SYS_fcntl     72
#define SYS_getcwd    79
#define SYS_chdir     80
#define SYS_rename    82
#define SYS_mkdir     83
#define SYS_rmdir     84
#define SYS_unlink    87
#define SYS_chmod     90
#define SYS_chown     92
#define SYS_getuid    102
#define SYS_getgid    104
#define SYS_setuid    105
#define SYS_setgid    106
#define SYS_gettid    186
#define SYS_futex     202

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
