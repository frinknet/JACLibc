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
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>

#ifdef __cplusplus
extern "C" {
#endif

#define X(name, value, ...) name = value,
enum { UNISTD_SYSCONF(X) };
enum { UNISTD_PATHCONF(X) };
enum { UNISTD_CONFSTR(X) };
#undef X

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
		#define X(NAME, val, rtn) case val: return rtn;
		UNISTD_SYSCONF(X)
		#undef X
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

extern char *optarg;
extern int optind, opterr, optopt;

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
