// (c) 2025 FRINKnet & Friends - MIT licence
#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

#ifdef __cplusplus
extern "C" {
#endif

// Basic types
typedef int pid_t;

// Status macros (commonly used)
#define WEXITSTATUS(s) (((s) & 0xff00) >> 8)
#define WTERMSIG(s) ((s) & 0x7f)
#define WSTOPSIG(s) WEXITSTATUS(s)
#define WIFEXITED(s) (!WTERMSIG(s))
#define WIFSIGNALED(s) (((s)&0xffff)-1U < 0xffu)
#define WIFSTOPPED(s) ((short)((((s)&0xffff)*0x10001)>>8) > 0x7f00)
#define WCOREDUMP(s) ((s) & 0x80)

// Wait options
#define WNOHANG 1
#define WUNTRACED 2

// Stub function declarations (will not be implemented)
static inline pid_t wait(int *status) { 
    (void)status; // Suppress unused parameter warning
    return -1; // Return error - not implemented
}

static inline pid_t waitpid(pid_t pid, int *status, int options) {
    (void)pid; (void)status; (void)options;
    return -1; // Return error - not implemented
}

#ifdef __cplusplus
}
#endif

#endif /* _SYS_WAIT_H */

