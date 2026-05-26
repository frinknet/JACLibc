/* (c) 2026 FRINKnet & Friends - MIT licence */
#ifndef _UNISTD_H
#define _UNISTD_H
#pragma once

#include <config.h>
#include <errno.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <time.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define _POSIX_VERSION                    202405L
#define _POSIX_ASYNCHRONOUS_IO            202405L
#define _POSIX_BARRIERS                   202405L
#define _POSIX_CLOCK_SELECTION            202405L
#define _POSIX_MAPPED_FILES               202405L
#define _POSIX_MEMORY_PROTECTION          202405L
#define _POSIX_MONOTONIC_CLOCK            202405L
#define _POSIX_READER_WRITER_LOCKS        202405L
#define _POSIX_REALTIME_SIGNALS           202405L
#define _POSIX_SEMAPHORES                 202405L
#define _POSIX_SPIN_LOCKS                 202405L
#define _POSIX_THREAD_SAFE_FUNCTIONS      202405L
#define _POSIX_THREADS                    202405L
#define _POSIX_TIMEOUTS                   202405L
#define _POSIX_TIMERS                     202405L
#define _POSIX2_SYMLINKS                  202405L
#define _POSIX_ADVISORY_INFO              202405L
#define _POSIX_CPUTIME                    202405L
#define _POSIX_DEVICE_CONTROL             202405L
#define _POSIX_FSYNC                      202405L
#define _POSIX_IPV6                       202405L
#define _POSIX_MEMLOCK                    202405L
#define _POSIX_MEMLOCK_RANGE              202405L
#define _POSIX_MESSAGE_PASSING            202405L
#define _POSIX_PRIORITIZED_IO             202405L
#define _POSIX_PRIORITY_SCHEDULING        202405L
#define _POSIX_RAW_SOCKETS                202405L
#define _POSIX_SHARED_MEMORY_OBJECTS      202405L
#define _POSIX_SPAWN                      202405L
#define _POSIX_SYNCHRONIZED_IO            202405L
#define _POSIX_THREAD_ATTR_STACKADDR      202405L
#define _POSIX_THREAD_ATTR_STACKSIZE      202405L
#define _POSIX_THREAD_CPUTIME             202405L
#define _POSIX_THREAD_PRIO_INHERIT        202405L
#define _POSIX_THREAD_PRIO_PROTECT        202405L
#define _POSIX_THREAD_PROCESS_SHARED      202405L
#define _POSIX_THREAD_PRIORITY_SCHEDULING 202405L
#define _POSIX_THREAD_ROBUST_PRIO_INHERIT 202405L
#define _POSIX_THREAD_ROBUST_PRIO_PROTECT 202405L
#define _POSIX2_C_DEV                     202405L
#define _POSIX2_CHAR_TERM                 202405L
#define _POSIX2_LOCALEDEF                 202405L
#define _POSIX2_SW_DEV                    202405L
#define _POSIX2_UPE                       202405L
#define _POSIX2_VERSION                   202405L
#define _POSIX2_C_BIND                    202405L

#define _POSIX_CHOWN_RESTRICTED         1
#define _POSIX_JOB_CONTROL              1
#define _POSIX_NO_TRUNC                 1
#define _POSIX_REGEXP                   1
#define _POSIX_SAVED_IDS                1
#define _POSIX_SHELL                    1
#define _POSIX_VDISABLE                 0
#define _POSIX_ASYNC_IO                 1
#define _POSIX_FALLOC                   1
#define _POSIX_PRIO_IO                  1
#define _POSIX_SYNC_IO                  1
#define _POSIX_TIMESTAMP_RESOLUTION     1
#define _POSIX_V7_ILP32_OFF32           1
#define _POSIX_V7_ILP32_OFFBIG          1
#define _POSIX_V7_LP64_OFF64            1
#define _POSIX_V7_LPBIG_OFFBIG          1
#define _POSIX_V8_ILP32_OFF32           1
#define _POSIX_V8_ILP32_OFFBIG          1
#define _POSIX_V8_LP64_OFF64            1
#define _POSIX_V8_LPBIG_OFFBIG          1
#define _POSIX_SPORADIC_SERVER         -1
#define _POSIX_THREAD_SPORADIC_SERVER  -1
#define _POSIX_TYPED_MEMORY_OBJECTS    -1
#define _POSIX2_FORT_RUN               -1

#define _XOPEN_VERSION      800
#define _XOPEN_CRYPT                    1
#define _XOPEN_ENH_I18N                 1
#define _XOPEN_REALTIME                 1
#define _XOPEN_REALTIME_THREADS         1
#define _XOPEN_SHM                      1
#define _XOPEN_UNIX                     1
#define _XOPEN_UUCP                    -1

#define _SC_VERSION                        1
#define _SC_PAGESIZE                       2
#define _SC_PAGE_SIZE                      _SC_PAGESIZE  /* Alias */
#define _SC_CLK_TCK                        3
#define _SC_ARG_MAX                        4
#define _SC_CHILD_MAX                      5
#define _SC_OPEN_MAX                       6
#define _SC_NGROUPS_MAX                    7
#define _SC_JOB_CONTROL                    8
#define _SC_SAVED_IDS                      9
#define _SC_STREAM_MAX                    10
#define _SC_TZNAME_MAX                    11
#define _SC_HOST_NAME_MAX                 12
#define _SC_AIO_LISTIO_MAX                23
#define _SC_AIO_MAX                       24
#define _SC_AIO_PRIO_DELTA_MAX            25
#define _SC_ASYNCHRONOUS_IO               26
#define _SC_COLL_WEIGHTS_MAX              27
#define _SC_DELAYTIMER_MAX                28
#define _SC_MQ_OPEN_MAX                   29
#define _SC_MQ_PRIO_MAX                   30
#define _SC_RTSIG_MAX                     31
#define _SC_SEM_NSEMS_MAX                 32
#define _SC_SEM_VALUE_MAX                 33
#define _SC_SIGQUEUE_MAX                  34
#define _SC_TIMER_MAX                     35
#define _SC_BC_BASE_MAX                   36
#define _SC_BC_DIM_MAX                    37
#define _SC_BC_SCALE_MAX                  38
#define _SC_BC_STRING_MAX                 39
#define _SC_EXPR_NEST_MAX                 40
#define _SC_LINE_MAX                      41
#define _SC_RE_DUP_MAX                    42
#define _SC_2_C_BIND                      45
#define _SC_2_C_DEV                       46
#define _SC_2_FORT_RUN                    47
#define _SC_2_LOCALEDEF                   48
#define _SC_2_SW_DEV                      49
#define _SC_2_UPE                         50
#define _SC_2_VERSION                     51
#define _SC_GETGR_R_SIZE_MAX              69
#define _SC_GETPW_R_SIZE_MAX              70
#define _SC_IOV_MAX                       71
#define _SC_LOGIN_NAME_MAX                72
#define _SC_TTY_NAME_MAX                  73
#define _SC_NPROCESSORS_CONF              83
#define _SC_NPROCESSORS_ONLN              84
#define _SC_ATEXIT_MAX                    87
#define _SC_2_CHAR_TERM                   95
#define _SC_ADVISORY_INFO                130
#define _SC_BARRIERS                     131
#define _SC_CLOCK_SELECTION              137
#define _SC_CPUTIME                      138
#define _SC_FSYNC                        139
#define _SC_MAPPED_FILES                 140
#define _SC_MEMLOCK                      141
#define _SC_MEMLOCK_RANGE                142
#define _SC_MESSAGE_PASSING              143
#define _SC_PRIORITIZED_IO               144
#define _SC_PRIORITY_SCHEDULING          145
#define _SC_REALTIME_SIGNALS             146
#define _SC_SHARED_MEMORY_OBJECTS        147
#define _SC_SPAWN                        148
#define _SC_SPORADIC_SERVER              149
#define _SC_SYMLOOP_MAX                  150
#define _SC_SYNCHRONIZED_IO              151
#define _SC_THREAD_ATTR_STACKADDR        152
#define _SC_THREAD_ATTR_STACKSIZE        153
#define _SC_THREAD_CPUTIME               154
#define _SC_THREAD_DESTRUCTOR_ITERATIONS 155
#define _SC_THREAD_KEYS_MAX              156
#define _SC_THREAD_PRIO_INHERIT          157
#define _SC_THREAD_PRIO_PROTECT          158
#define _SC_THREAD_PRIORITY_SCHEDULING   159
#define _SC_THREAD_PROCESS_SHARED        160
#define _SC_THREAD_SPORADIC_SERVER       161
#define _SC_THREAD_STACK_MIN             162
#define _SC_THREAD_THREADS_MAX           163
#define _SC_TYPED_MEMORY_OBJECTS         164
#define _SC_MEMORY_PROTECTION            165
#define _SC_MONOTONIC_CLOCK              166
#define _SC_READER_WRITER_LOCKS          167
#define _SC_REGEXP                       168
#define _SC_SEMAPHORES                   169
#define _SC_V8_ILP32_OFF32               170
#define _SC_V8_ILP32_OFFBIG              171
#define _SC_V8_LP64_OFF64                172
#define _SC_V8_LPBIG_OFFBIG              173
#define _SC_XOPEN_CRYPT                  174
#define _SC_XOPEN_ENH_I18N               175
#define _SC_XOPEN_REALTIME               176
#define _SC_XOPEN_REALTIME_THREADS       177
#define _SC_XOPEN_SHM                    178
#define _SC_XOPEN_UNIX                   179
#define _SC_XOPEN_UUCP                   180
#define _SC_SHELL                        181
#define _SC_SPIN_LOCKS                   182
#define _SC_THREAD_SAFE_FUNCTIONS        183
#define _SC_THREADS                      184
#define _SC_TIMEOUTS                     185
#define _SC_TIMERS                       186
#define _SC_XOPEN_VERSION                187
#define _SC_V7_ILP32_OFF32               188
#define _SC_V7_ILP32_OFFBIG              189
#define _SC_V7_LP64_OFF64                190
#define _SC_V7_LPBIG_OFFBIG              191
#define _SC_SS_REPL_MAX                  241
#define _SC_DEVICE_CONTROL               297
#define _SC_IPV6                         298
#define _SC_NSIG                         299
#define _SC_RAW_SOCKETS                  300
#define _SC_THREAD_ROBUST_PRIO_INHERIT   301
#define _SC_THREAD_ROBUST_PRIO_PROTECT   302

#define _PC_LINK_MAX                     0
#define _PC_MAX_CANON                    1
#define _PC_MAX_INPUT                    2
#define _PC_NAME_MAX                     3
#define _PC_PATH_MAX                     4
#define _PC_PIPE_BUF                     5
#define _PC_CHOWN_RESTRICTED             6
#define _PC_NO_TRUNC                     7
#define _PC_VDISABLE                     8
#define _PC_SYNC_IO                      9
#define _PC_ASYNC_IO                    10
#define _PC_PRIO_IO                     11
#define _PC_FILESIZEBITS                13
#define _PC_REC_INCR_XFER_SIZE          14
#define _PC_REC_MAX_XFER_SIZE           15
#define _PC_REC_MIN_XFER_SIZE           16
#define _PC_REC_XFER_ALIGN              17
#define _PC_ALLOC_SIZE_MIN              18
#define _PC_SYMLINK_MAX                 19
#define _PC_2_SYMLINKS                  20
#define _PC_FALLOC                      21
#define _PC_TEXTDOMAIN_MAX              22
#define _PC_TIMESTAMP_RESOLUTION        23

#define _CS_PATH                        1
#define _CS_POSIX_V7_ILP32_OFF32_CFLAGS      1100
#define _CS_POSIX_V7_ILP32_OFF32_LDFLAGS     1101
#define _CS_POSIX_V7_ILP32_OFF32_LIBS        1102
#define _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS     1104
#define _CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS    1105
#define _CS_POSIX_V7_ILP32_OFFBIG_LIBS       1106
#define _CS_POSIX_V7_LP64_OFF64_CFLAGS       1108
#define _CS_POSIX_V7_LP64_OFF64_LDFLAGS      1109
#define _CS_POSIX_V7_LP64_OFF64_LIBS         1110
#define _CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS     1112
#define _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS    1113
#define _CS_POSIX_V7_LPBIG_OFFBIG_LIBS       1114
#define _CS_POSIX_V7_THREADS_CFLAGS          1124
#define _CS_POSIX_V7_THREADS_LDFLAGS         1125
#define _CS_POSIX_V7_WIDTH_RESTRICTED_ENVS   1138
#define _CS_V7_ENV                           1139
#define _CS_POSIX_V8_ILP32_OFF32_CFLAGS      1116
#define _CS_POSIX_V8_ILP32_OFF32_LDFLAGS     1117
#define _CS_POSIX_V8_ILP32_OFF32_LIBS        1118
#define _CS_POSIX_V8_ILP32_OFFBIG_CFLAGS     1120
#define _CS_POSIX_V8_ILP32_OFFBIG_LDFLAGS    1121
#define _CS_POSIX_V8_ILP32_OFFBIG_LIBS       1122
#define _CS_POSIX_V8_LP64_OFF64_CFLAGS       1124
#define _CS_POSIX_V8_LP64_OFF64_LDFLAGS      1125
#define _CS_POSIX_V8_LP64_OFF64_LIBS         1126
#define _CS_POSIX_V8_LPBIG_OFFBIG_CFLAGS     1128
#define _CS_POSIX_V8_LPBIG_OFFBIG_LDFLAGS    1129
#define _CS_POSIX_V8_LPBIG_OFFBIG_LIBS       1130
#define _CS_POSIX_V8_THREADS_CFLAGS          1132
#define _CS_POSIX_V8_THREADS_LDFLAGS         1133
#define _CS_POSIX_V8_WIDTH_RESTRICTED_ENVS   1140
#define _CS_V8_ENV                      2

#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2
#define SEEK_DATA   3
#define SEEK_HOLE   4

#define STDIN_FILENO    0
#define STDOUT_FILENO   1
#define STDERR_FILENO   2

#define F_OK    0
#define R_OK    4
#define W_OK    2
#define X_OK    1

#define F_LOCK  1
#define F_TEST  3
#define F_TLOCK 2
#define F_ULOCK 0

#define POSIX_CLOSE_RESTART  0x0001
#define HOST_NAME_MAX        256

extern char **environ;

static inline char *getenv(const char *name) {
	if (name == NULL) return NULL;

	size_t len = strlen(name);

	for (char **env = environ; *env; ++env) {
		if (!strncmp(*env, name, len) && (*env)[len] == '=') return &((*env)[len+1]);
	}

	return NULL;
}

int snprintf(char * restrict s, size_t n, const char * restrict fmt, ...);

#if JACL_HAS_POSIX

#if JACL_HASSYS(exit_group)
noreturn static inline void _exit(int status) { syscall(SYS_exit_group, status); while(1); }
#else
noreturn static inline void _exit(int status) { syscall(SYS_exit, status); while(1); }
#endif
static inline pid_t getpid(void) { return (pid_t)syscall(SYS_getpid); }
static inline pid_t getppid(void) { return (pid_t)syscall(SYS_getppid); }
pid_t fork(void);
pid_t vfork(void);

static inline pid_t _Fork(void) {
	#if JACL_HASSYS(fork)
		return (pid_t)syscall(SYS_fork);
	#else
		errno = ENOSYS; return -1;
	#endif
}
static inline ssize_t read(int fd, void *buf, size_t count) { return syscall(SYS_read, fd, buf, count); }
static inline ssize_t write(int fd, const void *buf, size_t count) { return syscall(SYS_write, fd, buf, count); }
static inline int close(int fd) { return (int)syscall(SYS_close, fd); }
static inline int posix_close(int fd, int flags) {
	if (flags & ~POSIX_CLOSE_RESTART) { errno = EINVAL; return -1; }

	int r;

	do {
		r = close(fd);
	} while (r == -1 && errno == EINTR && (flags & POSIX_CLOSE_RESTART));

	if (r == -1 && errno == EBADF && (flags & POSIX_CLOSE_RESTART)) return 0;

	return r;
}
static inline off_t lseek(int fd, off_t offset, int whence) { return (off_t)syscall(SYS_lseek, fd, offset, whence); }
static inline int dup(int fd) { return (int)syscall(SYS_dup, fd); }
static inline int dup2(int oldfd, int newfd) { return (int)syscall(SYS_dup2, oldfd, newfd); }
static inline int dup3(int oldfd, int newfd, int flags) {
	if (oldfd == newfd) { errno = EINVAL; return -1; }
	#if JACL_HASSYS(dup3)
		return (int)syscall(SYS_dup3, oldfd, newfd, flags);
	#else
		int fd = dup2(oldfd, newfd);
		if (fd < 0) return -1;
		if (flags & O_CLOEXEC) fcntl(newfd, F_SETFD, FD_CLOEXEC);
		return fd;
	#endif
}
static inline int pipe(int pipefd[2]) {
	#if JACL_HASSYS(pipe2)
		return (int)syscall(SYS_pipe2, pipefd, 0);
	#elif JACL_HASSYS(pipe)
		return (int)syscall(SYS_pipe, pipefd);
	#else
		errno = ENOSYS; return -1;
	#endif
}
static inline int pipe2(int pipefd[2], int flags) {
	#if JACL_HASSYS(pipe2)
			return (int)syscall(SYS_pipe2, pipefd, flags);
	#else
		int r = pipe(pipefd);
		if (r < 0) return -1;
		if (flags & O_CLOEXEC) {
			fcntl(pipefd[0], F_SETFD, FD_CLOEXEC);
			fcntl(pipefd[1], F_SETFD, FD_CLOEXEC);
		}
		if (flags & O_NONBLOCK) {
			fcntl(pipefd[0], F_SETFL, O_NONBLOCK);
			fcntl(pipefd[1], F_SETFL, O_NONBLOCK);
		}
		return 0;
	#endif
}
static inline int access(const char *pathname, int mode) { return (int)syscall(SYS_access, pathname, mode); }
static inline int unlink(const char *pathname) { return (int)syscall(SYS_unlink, pathname); }
static inline int rmdir(const char *pathname) { return (int)syscall(SYS_rmdir, pathname); }
static inline int link(const char *oldpath, const char *newpath) { return (int)syscall(SYS_link, oldpath, newpath); }
static inline int symlink(const char *target, const char *linkpath) { return (int)syscall(SYS_symlink, target, linkpath); }
static inline ssize_t readlink(const char *pathname, char *buf, size_t bufsiz) { return syscall(SYS_readlink, pathname, buf, bufsiz); }
static inline int ftruncate(int fd, off_t length) { return (int)syscall(SYS_ftruncate, fd, length); }
static inline int truncate(const char *path, off_t length) { return (int)syscall(SYS_truncate, path, length); }
static inline int faccessat(int dirfd, const char *pathname, int mode, int flags) { return (int)syscall(SYS_faccessat, dirfd, pathname, mode, flags); }
static inline int fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags) { return (int)syscall(SYS_fchownat, dirfd, pathname, owner, group, flags); }
static inline int linkat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath, int flags) { return (int)syscall(SYS_linkat, olddirfd, oldpath, newdirfd, newpath, flags); }
static inline ssize_t readlinkat(int dirfd, const char *pathname, char *buf, size_t bufsiz) { return syscall(SYS_readlinkat, dirfd, pathname, buf, bufsiz); }
static inline int symlinkat(const char *target, int newdirfd, const char *linkpath) { return (int)syscall(SYS_symlinkat, target, newdirfd, linkpath); }
static inline int unlinkat(int dirfd, const char *pathname, int flags) { return (int)syscall(SYS_unlinkat, dirfd, pathname, flags); }
static inline int chdir(const char *path) { return (int)syscall(SYS_chdir, path); }
static inline char *getcwd(char *buf, size_t size) {
	if (size == 0) { errno = EINVAL; return NULL; }
	if (buf == NULL) { errno = EINVAL; return NULL; }

	long res = syscall(SYS_getcwd, buf, size);

	if (res == -1) return NULL;

	return buf;
}
static inline int getentropy(void *buffer, size_t length) {
	if (length > 256) { errno = EIO; return -1; }
	if (buffer == NULL && length > 0) { errno = EFAULT; return -1; }

	#if JACL_HASSYS(getentropy)
		return (int)syscall(SYS_getentropy, buffer, length);
	#elif JACL_HASSYS(getrandom)
		ssize_t r = syscall(SYS_getrandom, buffer, length, 0);
		if (r < 0 || (size_t)r != length) { errno = EIO; return -1; }
		return 0;
	#else
		int fd = open("/dev/urandom", O_RDONLY);
		if (fd < 0) return -1;
		ssize_t r = read(fd, buffer, length);
		close(fd);
		if (r < 0 || (size_t)r != length) { errno = EIO; return -1; }
		return 0;
	#endif
}
static inline int getlogin_r(char *buf, size_t size) {
	if (!buf || size == 0) return EINVAL;

	#if JACL_HASSYS(getlogin_r)
		/* OpenBSD: native getlogin_r syscall */
		return (int)syscall(SYS_getlogin_r, buf, size);
	#elif JACL_HASSYS(getlogin)
		/* Darwin, FreeBSD, DragonFly: use getlogin syscall */
		char *name = (char *)syscall(SYS_getlogin);
		if (!name) return errno ? errno : ENOTTY;
		size_t n = strlen(name) + 1;
		if (n > size) return ERANGE;
		memcpy(buf, name, n);
		return 0;
	#else
		/* Linux, NetBSD, others: fallback to environment */
		char *l = getenv("LOGNAME");
		if (!l) return ENOTTY;
		size_t n = strlen(l) + 1;
		if (n > size) return ERANGE;
		memcpy(buf, l, n);
		return 0;
	#endif
}
static inline char *getlogin(void) {
	#if JACL_HASSYS(getlogin)
		/* Darwin, FreeBSD, DragonFly: native syscall returns static buffer */
		return (char *)syscall(SYS_getlogin);
	#else
		/* OpenBSD, Linux, NetBSD: use getlogin_r with static buffer */
		static char name[256];
		if (getlogin_r(name, sizeof(name)) != 0) return NULL;
		return name;
	#endif
}
static inline int execve(const char *pathname, char *const argv[], char *const envp[]) { return (int)syscall(SYS_execve, pathname, argv, envp ? envp : environ); }
static inline int execv(const char *pathname, char *const argv[]) { return execve(pathname, argv, NULL); }
static inline int execvp(const char *file, char *const argv[]) {
	const char* p = file;
	const char* env_path = NULL;
	size_t path_len = 4;
	while (*p && *p != '/') p++;
	if (*p) return execve(file, argv, environ);
	for (char **env = environ; *env; ++env) {
		if (!strncmp(*env, "PATH=", path_len + 1)) { env_path = &((*env)[path_len + 1]); break; }
	}
	if (!env_path) env_path = "/bin:/usr/bin";
	char path_copy[4096];
	size_t env_len = 0;
	while (env_path[env_len] && env_len < sizeof(path_copy) - 1) { path_copy[env_len] = env_path[env_len]; env_len++; }
	path_copy[env_len] = '\0';
	const char* start = path_copy;
	while (1) {
		const char* end = start;
		while (*end && *end != ':') end++;
		if (end > start) {
			size_t dir_len = end - start;
			size_t file_len = 0;
			size_t i = 0;
			const char* temp = file;
			char full_path[4096];
			while (temp[file_len]) file_len++;
			if (dir_len + 1 + file_len + 1 > sizeof(full_path)) { errno = ENAMETOOLONG; continue; }
			for (size_t j = 0; j < dir_len; j++) full_path[i++] = start[j];
			full_path[i++] = '/';
			for (size_t j = 0; j < file_len; j++) full_path[i++] = file[j];
			full_path[i] = '\0';
			int res = execve(full_path, argv, environ);
			if (res == 0 || errno != ENOENT) return res;
		}
		if (!*end) break;
		start = end + 1;
	}
	errno = ENOENT;
	return -1;
}
static inline int execl(const char *pathname, const char *arg, ...) {
	const char *argv[256];
	va_list ap;
	int i = 0;
	argv[i++] = arg;
	va_start(ap, arg);
	while (i < 255 && (argv[i] = va_arg(ap, const char*)) != NULL) i++;
	va_end(ap);
	argv[i] = NULL;
	return execve(pathname, (char *const*)argv, environ);
}
static inline int execlp(const char *file, const char *arg, ...) {
	const char *argv[256];
	va_list ap;
	int i = 0;
	argv[i++] = arg;
	va_start(ap, arg);
	while (i < 255 && (argv[i] = va_arg(ap, const char*)) != NULL) i++;
	va_end(ap);
	argv[i] = NULL;
	return execvp(file, (char *const*)argv);
}
static inline int execle(const char *pathname, const char *arg, ...) {
	const char *argv[256];
	char *const *envp;
	va_list ap;
	int i = 0;
	argv[i++] = arg;
	va_start(ap, arg);
	while (i < 255 && (argv[i] = va_arg(ap, const char*)) != NULL) i++;
	envp = va_arg(ap, char *const*);
	va_end(ap);
	argv[i] = NULL;
	return execve(pathname, (char *const*)argv, envp);
}
static inline int fexecve(int fd, char *const argv[], char *const envp[]) {
	if (argv == NULL || envp == NULL) { errno = EFAULT; return -1; }
	if (fcntl(fd, F_GETFD) == -1) return -1; /* errno set to EBADF by fcntl */

	#if JACL_HASSYS(fexecve)
		return (int)syscall(SYS_fexecve, fd, argv, envp);
	#elif JACL_OS_DARWIN || JACL_OS_OPENBSD
		char path[32];
		snprintf(path, sizeof(path), "/dev/fd/%d", fd);
		return execve(path, argv, envp);
	#else
		char path[64];
		snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);
		return execve(path, argv, envp);
	#endif
}
static inline uid_t getuid(void) { return (uid_t)syscall(SYS_getuid); }
static inline int setuid(uid_t uid) { return (int)syscall(SYS_setuid, uid); }
static inline gid_t getgid(void) { return (gid_t)syscall(SYS_getgid); }
static inline int setgid(gid_t gid) { return (int)syscall(SYS_setgid, gid); }
static inline uid_t geteuid(void) { return (uid_t)syscall(SYS_geteuid); }
static inline gid_t getegid(void) { return (gid_t)syscall(SYS_getegid); }
static inline int setegid(gid_t egid) {
	#if JACL_HASSYS(setresgid)
		return (int)syscall(SYS_setresgid, (gid_t)-1, egid, (gid_t)-1);
	#elif JACL_HASSYS(setegid)
		return (int)syscall(SYS_setegid, egid);
	#else
		errno = ENOSYS; return -1;
	#endif
}
static inline int seteuid(uid_t euid) {
	#if JACL_HASSYS(setresuid)
		return (int)syscall(SYS_setresuid, (uid_t)-1, euid, (uid_t)-1);
	#elif JACL_HASSYS(seteuid)
		return (int)syscall(SYS_seteuid, euid);
	#else
		errno = ENOSYS; return -1;
	#endif
}
static inline int getgroups(int size, gid_t list[]) { return (int)syscall(SYS_getgroups, size, list); }
static inline pid_t getsid(pid_t pid) {
	#if JACL_HASSYS(getsid)
		return (pid_t)syscall(SYS_getsid, pid);
	#else
		errno = ENOSYS; return (pid_t)-1;
	#endif
}
static inline int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid) {
	#if JACL_HASSYS(getresuid)
		uid_t tmp_r, tmp_e, tmp_s;
		int r = (int)syscall(SYS_getresuid,
		ruid ? ruid : &tmp_r,
		euid ? euid : &tmp_e,
		suid ? suid : &tmp_s);
		return r;
	#else
		errno = ENOSYS;
		return -1;
	#endif
}
static inline int getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid) {
	#if JACL_HASSYS(getresgid)
		gid_t tmp_r, tmp_e, tmp_s;
		int r = (int)syscall(SYS_getresgid,
		rgid ? rgid : &tmp_r,
		egid ? egid : &tmp_e,
		sgid ? sgid : &tmp_s);
		return r;
	#else
		errno = ENOSYS;
		return -1;
	#endif
}
static inline int setresuid(uid_t ruid, uid_t euid, uid_t suid) {
	#if JACL_HASSYS(setresuid)
		return (int)syscall(SYS_setresuid, ruid, euid, suid);
	#else
		errno = ENOSYS; return -1;
	#endif
}
static inline int setresgid(gid_t rgid, gid_t egid, gid_t sgid) {
	#if JACL_HASSYS(setresgid)
		return (int)syscall(SYS_setresgid, rgid, egid, sgid);
	#else
		errno = ENOSYS; return -1;
	#endif
}
static inline pid_t getpgrp(void) { return (pid_t)syscall(SYS_getpgrp); }
static inline pid_t getpgid(pid_t pid) { return (pid_t)syscall(SYS_getpgid, pid); }
static inline int setpgid(pid_t pid, pid_t pgid) { return (int)syscall(SYS_setpgid, pid, pgid); }
static inline pid_t setsid(void) { return (pid_t)syscall(SYS_setsid); }
static inline int gethostname(char *name, size_t len) {
	if (name == NULL) { errno = EFAULT; return -1; }

	#if JACL_HASSYS(gethostname)
	  int r = (int)syscall(SYS_gethostname, name, len);

		if (r == 0) return 0;
	#else
		if (len > 9) { strcpy(name, "localhost"); return 0; }

		errno = EINVAL;
	#endif

	return -1;
}
static inline int sethostname(const char *name, size_t len) {
	#if JACL_HASSYS(sethostname)
		return (int)syscall(SYS_sethostname, name, len);
	#else
		errno = ENOSYS; return -1;
	#endif
}
static inline int isatty(int fd) {
	#if JACL_HASSYS(ioctl) && defined(TIOCGWINSZ)
		struct winsize ws;
		return syscall(SYS_ioctl, fd, TIOCGWINSZ, &ws) + 1;
	#else
		errno = ENOSYS; return 0;
	#endif
}

#ifndef __JACL_TIOCGPGRP
#if JACL_OS_LINUX
#define __JACL_TIOCGPGRP  0x540F
#define __JACL_TIOCSPGRP  0x5410
#elif JACL_OS_DARWIN || JACL_OS_BSD
#define __JACL_TIOCGPGRP  0x40047477
#define __JACL_TIOCSPGRP  0x80047476
#endif
#endif

static inline pid_t tcgetpgrp(int fd) {
	/* Validate fd first (EBADF), then check tty (ENOTTY) */
	if (fcntl(fd, F_GETFD) == -1) return (pid_t)-1;
	if (!isatty(fd)) { errno = ENOTTY; return (pid_t)-1; }

	pid_t pgrp = -1;
	if (ioctl(fd, __JACL_TIOCGPGRP, &pgrp) < 0) return (pid_t)-1;
	return pgrp;
}
static inline int tcsetpgrp(int fd, pid_t pgrp) {
	/* Validate fd first (EBADF), then check tty (ENOTTY) */
	if (fcntl(fd, F_GETFD) == -1) return -1;
	if (!isatty(fd)) { errno = ENOTTY; return -1; }
	return ioctl(fd, __JACL_TIOCSPGRP, &pgrp);
}
static inline int ttyname_r(int fd, char *buf, size_t buflen) {
	if (buf == NULL || buflen == 0) return EINVAL;
	if (fcntl(fd, F_GETFD) == -1) return EBADF;
	if (!isatty(fd)) return ENOTTY;

	char path[64];
	#if JACL_OS_DARWIN || JACL_OS_BSD
		snprintf(path, sizeof(path), "/dev/fd/%d", fd);
	#else
		snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);
	#endif

	/* Read symlink into a temp buffer to get full length */
	char tmp[256];
	ssize_t n = readlink(path, tmp, sizeof(tmp) - 1);
	if (n < 0) return errno;
	tmp[n] = '\0';

	/* POSIX: return ERANGE if buffer can't hold path + null terminator */
	if ((size_t)n + 1 > buflen) return ERANGE;

	/* Verify it actually points to a tty device */
	if (strncmp(tmp, "/dev/", 5) != 0) return ENOTTY;

	memcpy(buf, tmp, n + 1);

	return 0;
}
static inline char *ttyname(int fd) {
	static char buf[256];
	if (ttyname_r(fd, buf, sizeof(buf)) != 0) return NULL;
	return buf;
}
static inline int chown(const char *pathname, uid_t owner, gid_t group) { return (int)syscall(SYS_chown, pathname, owner, group); }
static inline int fchown(int fd, uid_t owner, gid_t group) { return (int)syscall(SYS_fchown, fd, owner, group); }
static inline int lchown(const char *pathname, uid_t owner, gid_t group) { return (int)syscall(SYS_lchown, pathname, owner, group); }
static inline int fchdir(int fd) { return (int)syscall(SYS_fchdir, fd); }
static inline void sync(void) { syscall(SYS_sync); }
static inline int fsync(int fd) { return (int)syscall(SYS_fsync, fd); }
static inline int fdatasync(int fd) {
	#if JACL_HASSYS(fdatasync)
		return (int)syscall(SYS_fdatasync, fd);
	#else
		return fsync(fd);
	#endif
}
static inline unsigned int sleep(unsigned int seconds) {
	struct timespec req = {seconds, 0}, rem = {0, 0};
	if (nanosleep(&req, &rem) == -1) return (unsigned int)rem.tv_sec + (rem.tv_nsec > 0 ? 1 : 0);
	return 0;
}
static inline int usleep(useconds_t usec) {
	struct timespec req = {usec / 1000000, (usec % 1000000) * 1000};
	return (int)nanosleep(&req, NULL);
}
static inline unsigned int alarm(unsigned int seconds) {
	#if JACL_HASSYS(alarm)
		return (unsigned int)syscall(SYS_alarm, seconds);
	#else
		errno = ENOSYS; return 0;
	#endif
}
static inline int pause(void) {
	#if JACL_HASSYS(pause)
		return (int)syscall(SYS_pause);
	#else
		errno = ENOSYS; return -1;
	#endif
}

#if JACL_OS_LINUX
static inline ssize_t pread(int fd, void *buf, size_t count, off_t offset) { return syscall(SYS_pread64, fd, buf, count, offset); }
static inline ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) { return syscall(SYS_pwrite64, fd, buf, count, offset); }
#elif JACL_OS_DRAGONFLY
static inline ssize_t pread(int fd, void *buf, size_t count, off_t offset) { return syscall(SYS_extpread, fd, buf, count, 0, offset); }
static inline ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) { return syscall(SYS_extpwrite, fd, buf, count, 0, offset); }
#elif JACL_OS_FREEBSD || JACL_OS_NETBSD || JACL_OS_OPENBSD
static inline ssize_t pread(int fd, void *buf, size_t count, off_t offset) { return syscall(SYS_pread, fd, buf, count, offset); }
static inline ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) { return syscall(SYS_pwrite, fd, buf, count, offset); }
#elif JACL_OS_DARWIN
static inline ssize_t pread(int fd, void *buf, size_t count, off_t offset) { return syscall(SYS_pread, fd, buf, count, offset); }
static inline ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) { return syscall(SYS_pwrite, fd, buf, count, offset); }
#endif

static inline long sysconf(int name) {
    switch (name) {
        case _SC_PAGESIZE:
            /* _SC_PAGE_SIZE is an alias, no separate case needed */
            return 4096;
        case _SC_CLK_TCK: return 100;
        case _SC_ARG_MAX: return 2097152;
        case _SC_CHILD_MAX: return 25;
        case _SC_OPEN_MAX: return 256;
        case _SC_VERSION: return _POSIX_VERSION;
        default: errno = EINVAL; return -1;
    }
}

static inline long fpathconf(int fd, int name) {
	/* Validate fd first per POSIX [EBADF] requirement */
	if (fcntl(fd, F_GETFD) == -1) return -1; /* errno = EBADF */

	switch (name) {
		case _PC_NO_TRUNC:
		case _PC_SYNC_IO:
		case _PC_2_SYMLINKS:
		case _PC_TIMESTAMP_RESOLUTION:
		case _PC_CHOWN_RESTRICTED:
			return 1;
		case _PC_VDISABLE:
		case _PC_ASYNC_IO:
		case _PC_PRIO_IO:
			return 0;
		case _PC_FILESIZEBITS:
			return 64;
		case _PC_LINK_MAX:
			return 127;
		case _PC_MAX_CANON:
		case _PC_MAX_INPUT:
		case _PC_NAME_MAX:
		case _PC_SYMLINK_MAX:
			return 255;
		case _PC_PATH_MAX:
		case _PC_PIPE_BUF:
		case _PC_REC_INCR_XFER_SIZE:
		case _PC_REC_MAX_XFER_SIZE:
		case _PC_REC_MIN_XFER_SIZE:
		case _PC_REC_XFER_ALIGN:
		case _PC_ALLOC_SIZE_MIN:
			return 4096;
		default:
			errno = EINVAL;
			return -1;
	}
}

static inline long pathconf(const char *path, int name) {
	if (path == NULL) { errno = EFAULT; return -1; }
	if (path[0] == '\0') { errno = ENOENT; return -1; }

	int fd = open(path, O_RDONLY | O_CLOEXEC);
	if (fd < 0) return -1;

	long result = fpathconf(fd, name);
	close(fd);
	return result;
}

extern char *getenv(const char *name);

static inline size_t confstr(int name, char *buf, size_t len) {
	const char *val = getenv("PATH");
	if (val == NULL) val = "/bin:/usr/bin";
	if (name != _CS_PATH) { errno = EINVAL; return 0; }

	size_t required = strlen(val) + 1;

	if (buf != NULL && len > 0) {
		size_t copy = (len < required) ? len : required;

		memcpy(buf, val, copy);

		buf[copy - 1] = '\0';
	}
	return required;
}

extern char *optarg;
extern int optind, opterr, optopt;

static inline int getopt(int argc, char *const argv[], const char *optstring) {
	static int optpos = 1;
	const char *opt;
	if (optind == 0) optind = 1, optpos = 1;
	if (optind >= argc || argv[optind][0] != '-') return -1;
	if (argv[optind][1] == '\0' || (argv[optind][1] == '-' && argv[optind][2] == '\0')) {
		optind++; return -1; /* -- ends options per POSIX */
	}
	opt = strchr(optstring, argv[optind][optpos]);
	if (!opt || *opt == ':') {
		if (opterr) {
			write(STDERR_FILENO, argv[0], strlen(argv[0]));
			write(STDERR_FILENO, ": illegal option -- ", 20);
			write(STDERR_FILENO, &argv[optind][optpos], 1);
			write(STDERR_FILENO, "\n", 1);
		}
		optpos++;
		if (argv[optind][optpos] == '\0') optind++, optpos = 1;
		return '?';
	}
	optarg = NULL;
	if (opt[1] == ':') {
		if (argv[optind][optpos+1]) {
			optarg = &argv[optind][optpos+1]; optind++; optpos = 1;
		} else if (++optind < argc) {
			optarg = argv[optind++]; optpos = 1;
		} else {
			if (opterr) {
				write(STDERR_FILENO, argv[0], strlen(argv[0]));
				write(STDERR_FILENO, ": option requires an argument -- ", 33);
				write(STDERR_FILENO, &argv[optind][optpos], 1);
				write(STDERR_FILENO, "\n", 1);
			}
			return optstring[0] == ':' ? ':' : '?';
		}
	} else {
		optpos++;
		if (argv[optind][optpos] == '\0') { optind++; optpos = 1; }
	}
	return *opt;
}

#else /* !JACL_HAS_POSIX */

static inline noreturn void _exit(int status) { (void)status; while(1); }
static inline pid_t getpid(void) { return 1; }
static inline pid_t getppid(void) { return 1; }
static inline pid_t fork(void) { errno = ENOSYS; return -1; }
static inline pid_t vfork(void) { return fork(); }
static inline pid_t _Fork(void) { errno = ENOSYS; return -1; }
static inline ssize_t read(int fd, void *buf, size_t count) { (void)fd; (void)buf; (void)count; errno = ENOSYS; return -1; }
static inline ssize_t write(int fd, const void *buf, size_t count) { (void)fd; (void)buf; (void)count; errno = ENOSYS; return -1; }
static inline int close(int fd) { (void)fd; errno = ENOSYS; return -1; }
static inline off_t lseek(int fd, off_t offset, int whence) { (void)fd; (void)offset; (void)whence; errno = ENOSYS; return (off_t)-1; }
static inline int dup(int fd) { (void)fd; errno = ENOSYS; return -1; }
static inline int dup2(int oldfd, int newfd) { (void)oldfd; (void)newfd; errno = ENOSYS; return -1; }
static inline int dup3(int oldfd, int newfd, int flags) { (void)oldfd; (void)newfd; (void)flags; errno = ENOSYS; return -1; }
static inline int pipe(int pipefd[2]) { (void)pipefd; errno = ENOSYS; return -1; }
static inline int pipe2(int pipefd[2], int flags) { (void)pipefd; (void)flags; errno = ENOSYS; return -1; }
static inline int access(const char *pathname, int mode) { (void)pathname; (void)mode; errno = ENOSYS; return -1; }
static inline int unlink(const char *pathname) { (void)pathname; errno = ENOSYS; return -1; }
static inline int rmdir(const char *pathname) { (void)pathname; errno = ENOSYS; return -1; }
static inline int link(const char *oldpath, const char *newpath) { (void)oldpath; (void)newpath; errno = ENOSYS; return -1; }
static inline int symlink(const char *target, const char *linkpath) { (void)target; (void)linkpath; errno = ENOSYS; return -1; }
static inline ssize_t readlink(const char *pathname, char *buf, size_t bufsiz) { (void)pathname; (void)buf; (void)bufsiz; errno = ENOSYS; return -1; }
static inline int ftruncate(int fd, off_t length) { (void)fd; (void)length; errno = ENOSYS; return -1; }
static inline int truncate(const char *path, off_t length) { (void)path; (void)length; errno = ENOSYS; return -1; }
static inline int chdir(const char *path) { (void)path; errno = ENOSYS; return -1; }
static inline char *getcwd(char *buf, size_t size) { (void)buf; (void)size; errno = ENOSYS; return NULL; }
static inline int execve(const char *pathname, char *const argv[], char *const envp[]) { (void)pathname; (void)argv; (void)envp; errno = ENOSYS; return -1; }
static inline int execv(const char *pathname, char *const argv[]) { (void)pathname; (void)argv; errno = ENOSYS; return -1; }
static inline int execvp(const char *file, char *const argv[]) { (void)file; (void)argv; errno = ENOSYS; return -1; }
static inline int execl(const char *pathname, const char *arg, ...) { (void)pathname; (void)arg; errno = ENOSYS; return -1; }
static inline int execlp(const char *file, const char *arg, ...) { (void)file; (void)arg; errno = ENOSYS; return -1; }
static inline int execle(const char *pathname, const char *arg, ...) { (void)pathname; (void)arg; errno = ENOSYS; return -1; }
static inline int fexecve(int fd, char *const argv[], char *const envp[]) { (void)fd; (void)argv; (void)envp; errno = ENOSYS; return -1; }
static inline uid_t getuid(void) { return 0; }
static inline gid_t getgid(void) { return 0; }
static inline uid_t geteuid(void) { return 0; }
static inline gid_t getegid(void) { return 0; }
static inline int setuid(uid_t uid) { (void)uid; errno = ENOSYS; return -1; }
static inline int setgid(gid_t gid) { (void)gid; errno = ENOSYS; return -1; }
static inline int getgroups(int size, gid_t list[]) { (void)size; (void)list; errno = ENOSYS; return -1; }
static inline pid_t getpgrp(void) { return 1; }
static inline int setpgid(pid_t pid, pid_t pgid) { (void)pid; (void)pgid; errno = ENOSYS; return -1; }
static inline pid_t setsid(void) { errno = ENOSYS; return -1; }
static inline int gethostname(char *name, size_t len) { (void)name; (void)len; errno = ENOSYS; return -1; }
static inline int sethostname(const char *name, size_t len) { (void)name; (void)len; errno = ENOSYS; return -1; }
static inline int isatty(int fd) { (void)fd; errno = ENOSYS; return 0; }
static inline int chown(const char *pathname, uid_t owner, gid_t group) { (void)pathname; (void)owner; (void)group; errno = ENOSYS; return -1; }
static inline int fchown(int fd, uid_t owner, gid_t group) { (void)fd; (void)owner; (void)group; errno = ENOSYS; return -1; }
static inline int lchown(const char *pathname, uid_t owner, gid_t group) { (void)pathname; (void)owner; (void)group; errno = ENOSYS; return -1; }
static inline int fchdir(int fd) { (void)fd; errno = ENOSYS; return -1; }
static inline void sync(void) { /* noop */ }
static inline int fsync(int fd) { (void)fd; errno = ENOSYS; return -1; }
static inline int fdatasync(int fd) { (void)fd; errno = ENOSYS; return -1; }
static inline unsigned int sleep(unsigned int seconds) { return seconds; }
static inline int usleep(useconds_t usec) { (void)usec; errno = ENOSYS; return -1; }
static inline unsigned int alarm(unsigned int seconds) { (void)seconds; errno = ENOSYS; return 0; }
static inline int pause(void) { errno = ENOSYS; return -1; }
static inline ssize_t pread(int fd, void *buf, size_t count, off_t offset) { (void)fd; (void)buf; (void)count; (void)offset; errno = ENOSYS; return -1; }
static inline ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset) { (void)fd; (void)buf; (void)count; (void)offset; errno = ENOSYS; return -1; }
static inline long sysconf(int name) { (void)name; errno = ENOSYS; return -1; }
static inline long pathconf(const char *path, int name) { (void)path; (void)name; errno = ENOSYS; return -1; }
static inline size_t confstr(int name, char *buf, size_t len) { (void)name; (void)buf; (void)len; errno = ENOSYS; return 0; }
static inline int getopt(int argc, char *const argv[], const char *optstring) { (void)argc; (void)argv; (void)optstring; errno = ENOSYS; return -1; }
static inline int posix_close(int fd, int f) { (void)fd; (void)f; errno = ENOSYS; return -1; }
static inline int faccessat(int d, const char *p, int m, int f) { (void)d; (void)p; (void)m; (void)f; errno = ENOSYS; return -1; }
static inline int fchownat(int d, const char *p, uid_t o, gid_t g, int f) { (void)d; (void)p; (void)o; (void)g; (void)f; errno = ENOSYS; return -1; }
static inline int linkat(int od, const char *op, int nd, const char *np, int f) { (void)od; (void)op; (void)nd; (void)np; (void)f; errno = ENOSYS; return -1; }
static inline ssize_t readlinkat(int d, const char *p, char *b, size_t s) { (void)d; (void)p; (void)b; (void)s; errno = ENOSYS; return -1; }
static inline int symlinkat(const char *t, int d, const char *l) { (void)t; (void)d; (void)l; errno = ENOSYS; return -1; }
static inline int unlinkat(int d, const char *p, int f) { (void)d; (void)p; (void)f; errno = ENOSYS; return -1; }
static inline int getentropy(void *b, size_t l) { (void)b; (void)l; errno = ENOSYS; return -1; }
static inline char *getlogin(void) { errno = ENOSYS; return NULL; }
static inline int getlogin_r(char *n, size_t s) { (void)n; (void)s; errno = ENOSYS; return -1; }
static inline pid_t getpgid(pid_t p) { (void)p; errno = ENOSYS; return -1; }
static inline pid_t getsid(pid_t p) { (void)p; errno = ENOSYS; return -1; }
static inline int getresuid(uid_t *r, uid_t *e, uid_t *s) { (void)r; (void)e; (void)s; errno = ENOSYS; return -1; }
static inline int getresgid(gid_t *r, gid_t *e, gid_t *s) { (void)r; (void)e; (void)s; errno = ENOSYS; return -1; }
static inline int setegid(gid_t g) { (void)g; errno = ENOSYS; return -1; }
static inline int seteuid(uid_t u) { (void)u; errno = ENOSYS; return -1; }
static inline int setresuid(uid_t r, uid_t e, uid_t s) { (void)r; (void)e; (void)s; errno = ENOSYS; return -1; }
static inline int setresgid(gid_t r, gid_t e, gid_t s) { (void)r; (void)e; (void)s; errno = ENOSYS; return -1; }
static inline pid_t tcgetpgrp(int fd) { (void)fd; errno = ENOSYS; return -1; }
static inline int tcsetpgrp(int fd, pid_t p) { (void)fd; (void)p; errno = ENOSYS; return -1; }
static inline char *ttyname(int fd) { (void)fd; errno = ENOSYS; return NULL; }
static inline int ttyname_r(int fd, char *b, size_t s) { (void)fd; (void)b; (void)s; errno = ENOSYS; return -1; }
static inline long fpathconf(int fd, int n) { (void)fd; (void)n; errno = ENOSYS; return -1; }

#endif /* JACL_HAS_POSIX */

#ifdef __cplusplus
}
#endif

#endif /* _UNISTD_H */
