// (c) 2025 FRINKnet & Friends - MIT licence

#ifndef UNISTD_H
#define UNISTD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Minimal type definitions */
typedef unsigned long size_t;
typedef long ssize_t;
typedef int pid_t;

/* Stub function declarations */
static inline void _exit(int status) { (void)status; while (1); }
static inline pid_t getpid(void) { return 1; }
static inline pid_t fork(void) { return -1; }
static inline int pipe(int pipefd[2]) { (void)pipefd; return -1; }
static inline ssize_t read(int fd, void *buf, size_t count) {
    (void)fd; (void)buf; (void)count; return -1;
}
static inline ssize_t write(int fd, const void *buf, size_t count) {
    (void)fd; (void)buf; (void)count; return -1;
}

/* Stub for execvp */
static inline int execvp(const char *file, char *const argv[]) {
    (void)file; (void)argv;
    return -1; /* Not implemented in WebAssembly */
}

#ifdef __cplusplus
}
#endif

#endif /* UNISTD_H */

