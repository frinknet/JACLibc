/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _FCNTL_H
#define _FCNTL_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <config.h>
#include <errno.h>
#include <sys/types.h>
#include <stdarg.h>
#include JACL_X_FCNTL

#if JACL_OS_WINDOWS
	#define FCNTL_WIN32 1
	#include <windows.h>
	#include <io.h>
#else
	#define FCNTL_POSIX 1
	#include <sys/syscall.h>
#endif

#define X(name, value) name = value,
/* Open flags */
enum { FCNTL_O_FLAGS(X) };

/* Fcntl commands */
enum { FCNTL_F_COMMANDS(X) };

/* File descriptor flags */
enum { FCNTL_FD_FLAGS(X) };

/* Lock types */
enum { FCNTL_LOCK_TYPES(X) };

/* Owner types */
enum { FCNTL_OWNER_TYPES(X) };

/* AT function flags */
enum { FCNTL_AT_FLAGS(X) };

/* POSIX advisory flags */
enum { FCNTL_POSIX_FADV(X) };

/* Seek constants */
enum { FCNTL_SEEK(X) };
#undef X

/* ================================================================ */
/* Structures                                                       */
/* ================================================================ */

/* Standard file locking structure */
struct flock {
	short l_type;    /* Type of lock: F_RDLCK, F_WRLCK, F_UNLCK */
	short l_whence;  /* Where l_start is relative to: SEEK_SET, SEEK_CUR, SEEK_END */
	off_t l_start;   /* Starting offset for lock */
	off_t l_len;     /* Number of bytes to lock (0 means to EOF) */
	pid_t l_pid;     /* PID of process blocking our lock (F_GETLK only) */
};

/* Large file support */
#if FCNTL_HAS_LFS
struct flock64 {
	short l_type;
	short l_whence;
	off64_t l_start;
	off64_t l_len;
	pid_t l_pid;
};
#endif

/* Extended owner structure for F_*OWN_EX */
struct f_owner_ex {
	int type;    /* F_OWNER_TID, F_OWNER_PID, or F_OWNER_PGRP */
	pid_t pid;   /* Process/thread/group ID */
};

#if JACL_OS_DARWIN
struct radvisory {
	off_t ra_offset;
	int ra_count;
};

typedef struct fstore {
	unsigned int fst_flags;
	int fst_posmode;
	off_t fst_offset;
	off_t fst_length;
	off_t fst_bytesalloc;
} fstore_t;
#endif

/* ================================================================ */
/* Function implementations                                         */
/* ================================================================ */

#if FCNTL_WIN32
/* ================================================================ */
/* Windows implementation using Win32 APIs                         */
/* ================================================================ */

/* Convert POSIX open flags to Windows access/creation flags */
static inline void win32_convert_flags(int flags, DWORD *access, DWORD *creation, DWORD *attrs) {
	*access = 0;
	*creation = 0;
	*attrs = FILE_ATTRIBUTE_NORMAL;

	/* Access modes */
	switch (flags & O_ACCMODE) {
	case O_RDONLY:
	case O_SEARCH:
		*access = GENERIC_READ;
		break;
	case O_WRONLY:
		*access = GENERIC_WRITE;
		break;
	case O_RDWR:
		*access = GENERIC_READ | GENERIC_WRITE;
		break;
	case O_EXEC:
			*access = GENERIC_EXECUTE;
			break;
	}

	/* Creation flags */
	if (flags & O_CREAT) {
		if (flags & O_EXCL) {
			*creation = CREATE_NEW;      /* Fail if exists */
		} else if (flags & O_TRUNC) {
			*creation = CREATE_ALWAYS;   /* Create or truncate */
		} else {
			*creation = OPEN_ALWAYS;     /* Create or open */
		}
	} else {
		if (flags & O_TRUNC) {
			*creation = TRUNCATE_EXISTING; /* Truncate existing */
		} else {
			*creation = OPEN_EXISTING;     /* Open existing only */
		}
	}

	/* Directory flag */
	if (flags & O_DIRECTORY) {
		*attrs |= FILE_FLAG_BACKUP_SEMANTICS;
	}
}

static inline int open(const char *pathname, int flags, ...) {
	if (!pathname) return -1;

	DWORD access, creation, attrs;
	win32_convert_flags(flags, &access, &creation, &attrs);

	HANDLE hFile = CreateFileA(pathname, access,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		creation, attrs, NULL);

	if (hFile == INVALID_HANDLE_VALUE) return -1;

	/* Convert Windows HANDLE to integer file descriptor */
	int fd_flags = 0;
	if (flags & O_APPEND) fd_flags |= _O_APPEND;
	if (flags & O_RDONLY) fd_flags |= _O_RDONLY;

	int fd = _open_osfhandle((intptr_t)hFile, fd_flags);
	return fd;
}

static inline int openat(int dirfd, const char *pathname, int flags, ...) {
	/* Windows doesn't have openat, simulate if possible */
	(void)dirfd;
	if (!pathname) return -1;

	/* If pathname is absolute, ignore dirfd */
	if (pathname[0] == '/' || (pathname[0] && pathname[1] == ':')) {
		return open(pathname, flags);
	}

	/* Otherwise, not easily supported on Windows */
	return -1;
}

static inline int creat(const char *pathname, mode_t mode) {
	(void)mode; /* Windows doesn't use POSIX permissions */
	return open(pathname, O_CREAT | O_WRONLY | O_TRUNC, 0);
}

static inline int fcntl(int fd, int cmd, ...) {
	va_list args;
	va_start(args, cmd);

	switch (cmd) {
	case F_DUPFD: {
		int min_fd = va_arg(args, int);
		int new_fd = _dup(fd);

		va_end(args);

		return (new_fd >= min_fd) ? new_fd : -1;
	}

	case F_DUPFD_CLOEXEC: {
		int min_fd = va_arg(args, int);
		int new_fd = _dup(fd);

		va_end(args);

		/* Windows: Set inheritable to FALSE for CLOEXEC equivalent */
		if (new_fd >= min_fd) {
			HANDLE h = (HANDLE)_get_osfhandle(new_fd);
			SetHandleInformation(h, HANDLE_FLAG_INHERIT, 0);

			return new_fd;
		}

		return -1;
	}

	case F_GETFD:
		va_end(args);

		/* Check if handle is inheritable */
		{
			HANDLE h = (HANDLE)_get_osfhandle(fd);
			DWORD flags;

			if (GetHandleInformation(h, &flags)) return (flags & HANDLE_FLAG_INHERIT) ? 0 : FD_CLOEXEC;
		}

		return 0;

	case F_SETFD: {
		int flags = va_arg(args, int);

		va_end(args);

		HANDLE h = (HANDLE)_get_osfhandle(fd);
		DWORD win_flags = (flags & FD_CLOEXEC) ? 0 : HANDLE_FLAG_INHERIT;

		return SetHandleInformation(h, HANDLE_FLAG_INHERIT, win_flags) ? 0 : -1;
	}

	case F_GETFL:
		va_end(args);

		/* Cannot easily determine original open flags on Windows */
		return 0;

	case F_SETFL: {
		int flags = va_arg(args, int);

		va_end(args);

		/* Limited flag setting support on Windows */
		(void)flags;

		return 0;
	}

	default:
		va_end(args);

		return -1;
	}
}

/* POSIX advisory functions - limited Windows support */
static inline int posix_fadvise(int fd, off_t offset, off_t len, int advice) {
	(void)fd; (void)offset; (void)len; (void)advice;

	return 0; /* No-op on Windows */
}

static inline int posix_fallocate(int fd, off_t offset, off_t len) {
	(void)fd; (void)offset; (void)len;

	return -1; /* Not easily supported on Windows */
}

#else
/* ================================================================ */
/* POSIX/Linux implementation using system calls                   */
/* ================================================================ */

static inline int open(const char *pathname, int flags, ...) {
	if (!pathname) return -1;

	mode_t mode = 0;
	if (flags & O_CREAT) {
		va_list args;
		va_start(args, flags);
		mode = va_arg(args, mode_t);
		va_end(args);
	}

	#if JACL_HASSYS(openat)
		return (int)syscall(SYS_openat, AT_FDCWD, pathname, flags, mode);
	#elif JACL_HASSYS(open)
		return (int)syscall(SYS_open, pathname, flags, mode);
	#else
		errno = ENOSYS;
		return -1;
	#endif
}

static inline int openat(int dirfd, const char *pathname, int flags, ...) {
	if (!pathname) return -1;

	mode_t mode = 0;
	if (flags & O_CREAT) {
		va_list args;
		va_start(args, flags);
		mode = va_arg(args, mode_t);
		va_end(args);
	}

	return (int)syscall(SYS_openat, dirfd, pathname, flags, mode);
}

static inline int creat(const char *pathname, mode_t mode) {
	return open(pathname, O_CREAT | O_WRONLY | O_TRUNC, mode);
}

static inline int renameat(int olddirfd, const char *oldpath, int newdirfd, const char *newpath) {
	#if JACL_HASSYS(renameat2)
		return syscall(SYS_renameat2, olddirfd, oldpath, newdirfd, newpath, 0);
	#elif JACL_HASSYS(SYS_renameat)
		return syscall(SYS_renameat, olddirfd, oldpath, newdirfd, newpath);
	#else
		if (olddirfd != AT_FDCWD || newdirfd != AT_FDCWD) {
			errno = ENOSYS;

			return -1;
		}

		return syscall(SYS_rename, oldpath, newpath);
	#endif
}

static inline int fcntl(int fd, int cmd, ...) {
	va_list args;
	long arg = 0;
	struct flock *lock = NULL;
	struct f_owner_ex *owner = NULL;

	va_start(args, cmd);

	/* Get argument based on command type */
	switch (cmd) {
	case F_GETLK:
	case F_SETLK:
	case F_SETLKW:
	case F_OFD_GETLK:
	case F_OFD_SETLK:
	case F_OFD_SETLKW:
		lock = va_arg(args, struct flock *);
		arg = (long)lock;
		break;
	case F_GETOWN_EX:
	case F_SETOWN_EX:
		owner = va_arg(args, struct f_owner_ex *);
		arg = (long)owner;
		break;
	default:
		arg = va_arg(args, long);
		break;
	}

	va_end(args);

	return (int)syscall(SYS_fcntl, fd, cmd, arg);
}

/* POSIX advisory functions */
static inline int posix_fadvise(int fd, off_t offset, off_t len, int advice) {
	#if JACL_OS_LINUX
		return (int)syscall(SYS_fadvise64, fd, offset, len, advice);
	#elif JACL_OS_FREEBSD
		return (int)syscall(SYS_posix_fadvise, fd, offset, len, advice);
	#elif JACL_OS_NETBSD
		return (int)syscall(SYS_posix_fadvise, fd, 0, offset, 0, len, advice);
	#elif JACL_OS_DARWIN
		/* macOS: use fcntl F_RDADVISE for sequential/random hints */
		struct radvisory ra;
		ra.ra_offset = offset;
		ra.ra_count = (int)len;  // Cast needed here

		if (advice == POSIX_FADV_SEQUENTIAL || advice == POSIX_FADV_RANDOM) {
			return (int)syscall(SYS_fcntl, fd, F_RDADVISE, &ra);
		}

		return 0; /* Other hints ignored */
	#elif JACL_OS_OPENBSD
		/* OpenBSD doesn't support fadvise */
		(void)fd; (void)offset; (void)len; (void)advice;

		return 0;
	#elif JACL_OS_DRAGONFLY
		return (int)syscall(SYS_posix_fadvise, fd, offset, len, advice);
	#else
		(void)fd; (void)offset; (void)len; (void)advice;
		errno = ENOSYS;

		return -1;
	#endif
}

static inline int posix_fallocate(int fd, off_t offset, off_t len) {
	#if JACL_OS_LINUX
		return (int)syscall(SYS_fallocate, fd, 0, offset, len);
	#elif JACL_OS_FREEBSD
		return (int)syscall(SYS_posix_fallocate, fd, offset, len);
	#elif JACL_OS_NETBSD
		return (int)syscall(SYS_posix_fallocate, fd, 0, offset, 0, len);
	#elif JACL_OS_DARWIN
		/* macOS: use fcntl F_PREALLOCATE */
		fstore_t store = {F_ALLOCATECONTIG, F_PEOFPOSMODE, offset, len, 0};
		if (syscall(SYS_fcntl, fd, F_PREALLOCATE, &store) == -1) {
			/* Try non-contiguous */
			store.fst_flags = F_ALLOCATEALL;
			if (syscall(SYS_fcntl, fd, F_PREALLOCATE, &store) == -1) {
				return -1;
			}
		}
		return (int)syscall(SYS_ftruncate, fd, offset + len);
	#elif JACL_OS_OPENBSD
		/* OpenBSD doesn't support fallocate */
		(void)fd; (void)offset; (void)len;
		errno = EOPNOTSUPP;
		return -1;
	#elif JACL_OS_DRAGONFLY
		return (int)syscall(SYS_posix_fallocate, fd, offset, len);
	#else
		(void)fd; (void)offset; (void)len;
		errno = ENOSYS;
		return -1;
	#endif
}

/* Large file support */
#if JACL_HAS_LFS
static inline int fcntl64(int fd, int cmd, ...) {
	va_list args;
	long arg = 0;
	struct flock64 *lock = NULL;

	va_start(args, cmd);

	switch (cmd) {
	case F_GETLK:
	case F_SETLK:
	case F_SETLKW:
		lock = va_arg(args, struct flock64 *);
		arg = (long)lock;
		break;
	default:
		arg = va_arg(args, long);
		break;
	}

	va_end(args);

	#if JACL_HASSYS(fcntl64)
		return (int)syscall(SYS_fcntl64, fd, cmd, arg);
	#endif

	/* Fallback to regular fcntl */
	return fcntl(fd, cmd, arg);
}

static inline int posix_fadvise64(int fd, off64_t offset, off64_t len, int advice) {
	#if JACL_HASSYS(fadvise64_64)
		return (int)syscall(SYS_fadvise64_64, fd, offset, len, advice);
	#else
		errno = ENOSYS;

		return -1;
	#endif
}


static inline int posix_fallocate64(int fd, off64_t offset, off64_t len) {
	#if JACL_HASSYS(fallocate)
		return (int)syscall(SYS_fallocate, fd, 0, offset, len);
	#endif

	return posix_fallocate(fd, (off_t)offset, (off_t)len);
}
#endif /* JACL_HAS_LFS */

#endif

/* ================================================================ */
/* Helper functions                                                 */
/* ================================================================ */

/* Check if file descriptor is valid */
static inline int fcntl_valid_fd(int fd) {
	return fcntl(fd, F_GETFD) != -1;
}

/* Set close-on-exec flag */
static inline int fcntl_set_cloexec(int fd) {
	int flags = fcntl(fd, F_GETFD);
	if (flags == -1) return -1;
	return fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
}

/* Clear close-on-exec flag */
static inline int fcntl_clear_cloexec(int fd) {
	int flags = fcntl(fd, F_GETFD);
	if (flags == -1) return -1;
	return fcntl(fd, F_SETFD, flags & ~FD_CLOEXEC);
}

/* Set non-blocking mode */
static inline int fcntl_set_nonblock(int fd) {
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1) return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

/* Clear non-blocking mode */
static inline int fcntl_clear_nonblock(int fd) {
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1) return -1;
	return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
}

/* Duplicate file descriptor with CLOEXEC */
static inline int fcntl_dupfd_cloexec(int fd, int min_fd) {
	int new_fd = fcntl(fd, F_DUPFD_CLOEXEC, min_fd);
	if (new_fd != -1) return new_fd;

	/* Fallback for systems without F_DUPFD_CLOEXEC */
	new_fd = fcntl(fd, F_DUPFD, min_fd);
	if (new_fd == -1) return -1;

	if (fcntl_set_cloexec(new_fd) == -1) {
		syscall(SYS_close, new_fd);

		return -1;
	}

	return new_fd;
}

#ifdef __cplusplus
}
#endif

#endif /* _FCNTL_H */
