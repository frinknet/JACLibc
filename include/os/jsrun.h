/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef ARCH_WASM_H
#define ARCH_WASM_H
#pragma once

/* Generic fallback syscall numbers */
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

#include <jsio.h>  // For js_read/js_write

#ifdef __cplusplus
extern "C" {
#endif

static inline long __jsrun_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
  switch ((int)num) {
    case SYS_read:
      return js_read((int)a1, (void*)a2, (size_t)a3);
    case SYS_write:
      return js_write((int)a1, (const void*)a2, (size_t)a3);

    // Add more cases as you implement them
    default:
      return 0; // Or return -1/ENOSYS for unsupported
  }
}

#ifdef __cplusplus
}
#endif

#endif // ARCH_WASM_H
