/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _SYS_STAT_H
#define _SYS_STAT_H
#pragma once

#include <config.h>
#include <time.h>
#include <fcntl.h>

#ifdef __cplusplus
	extern "C" {
#endif

#if JACL_HAS_POSIX
	#include <unistd.h>
	#include <sys/syscall.h>
#endif

/* ================================================================ */
/* File type constants                                              */
/* ================================================================ */

#define S_IFMT    0170000
#define S_IFSOCK  0140000
#define S_IFLNK   0120000
#define S_IFREG   0100000
#define S_IFBLK   0060000
#define S_IFDIR   0040000
#define S_IFCHR   0020000
#define S_IFIFO   0010000

/* ================================================================ */
/* File permission constants                                        */
/* ================================================================ */

#define S_IRUSR   0000400
#define S_IWUSR   0000200
#define S_IXUSR   0000100
#define S_IRWXU   (S_IRUSR | S_IWUSR | S_IXUSR)

#define S_IRGRP   0000040
#define S_IWGRP   0000020
#define S_IXGRP   0000010
#define S_IRWXG   (S_IRGRP | S_IWGRP | S_IXGRP)

#define S_IROTH   0000004
#define S_IWOTH   0000002
#define S_IXOTH   0000001
#define S_IRWXO   (S_IROTH | S_IWOTH | S_IXOTH)

#define S_ISUID   0004000
#define S_ISGID   0002000
#define S_ISVTX   0001000

/* ================================================================ */
/* File type test macros                                            */
/* ================================================================ */

#define S_ISREG(m)   (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)   (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)   (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)   (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)  (((m) & S_IFMT) == S_IFIFO)
#define S_ISLNK(m)   (((m) & S_IFMT) == S_IFLNK)
#define S_ISSOCK(m)  (((m) & S_IFMT) == S_IFSOCK)

/* ================================================================ */
/* POSIX.1-2008 time constants                                      */
/* ================================================================ */

#define UTIME_NOW  ((1l << 30) - 1l)
#define UTIME_OMIT ((1l << 30) - 2l)

/* ================================================================ */
/* Structures                                                       */
/* ================================================================ */

struct stat {
	dev_t     st_dev;
	ino_t     st_ino;
	mode_t    st_mode;
	nlink_t   st_nlink;
	uid_t     st_uid;
	gid_t     st_gid;
	dev_t     st_rdev;
	off_t     st_size;
	blksize_t st_blksize;
	blkcnt_t  st_blocks;

	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
};

#if JACL_HAS_LFS

struct stat64 {
	dev_t     st_dev;
	ino_t     st_ino;
	mode_t    st_mode;
	nlink_t   st_nlink;
	uid_t     st_uid;
	gid_t     st_gid;
	dev_t     st_rdev;
	off64_t   st_size;

	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;

	blksize_t st_blksize;
	blkcnt64_t st_blocks;
};

#endif

struct statvfs {
	unsigned long  f_bsize;
	unsigned long  f_frsize;
	fsblkcnt_t     f_blocks;
	fsblkcnt_t     f_bfree;
	fsblkcnt_t     f_bavail;
	fsfilcnt_t     f_files;
	fsfilcnt_t     f_ffree;
	fsfilcnt_t     f_favail;
	unsigned long  f_fsid;
	unsigned long  f_flag;
	unsigned long  f_namemax;
};

#define __OS_STAT
#include JACL_OS_FILE

/* ================================================================ */
/* Implementations                                                  */
/* ================================================================ */

#if JACL_HAS_POSIX

static inline int stat(const char *pathname, struct stat *statbuf) {
	if (!pathname || !statbuf) return (__errno_set(EINVAL), -1);

	struct __jacl_os_stat kbuf;

	#if JACL_HASSYS(stat)
		if ((int)syscall(SYS_stat, pathname, &kbuf) < 0) return -1;
	#elif JACL_HASSYS(newfstatat)
		if ((int)syscall(SYS_newfstatat, AT_FDCWD, pathname, &kbuf, 0) < 0) return -1;
	#else
		return (__errno_set(ENOSYS), -1);
	#endif

	__jacl_os_statnorm(&kbuf, statbuf);

	return 0;
}

static inline int fstat(int fd, struct stat *statbuf) {
	if (!statbuf) return (__errno_set(EINVAL), -1);

	struct __jacl_os_stat kbuf;

	#if JACL_HASSYS(fstat)
		if ((int)syscall(SYS_fstat, fd, &kbuf) < 0) return -1;
	#elif JACL_HASSYS(newfstatat)
		if ((int)syscall(SYS_newfstatat, fd, "", &kbuf, AT_EMPTY_PATH) < 0) return -1;
	#else
		return (__errno_set(ENOSYS), -1);
	#endif

	__jacl_os_statnorm(&kbuf, statbuf);

	return 0;
}

static inline int lstat(const char *pathname, struct stat *statbuf) {
	if (!pathname || !statbuf) return (__errno_set(EINVAL), -1);

	struct __jacl_os_stat kbuf;

	#if JACL_HASSYS(lstat)
		if ((int)syscall(SYS_lstat, pathname, &kbuf) < 0) return -1;
	#elif JACL_HASSYS(newfstatat)
		if ((int)syscall(SYS_newfstatat, AT_FDCWD, pathname, &kbuf, AT_SYMLINK_NOFOLLOW) < 0) return -1;
	#else
		return (__errno_set(ENOSYS), -1);
	#endif

	__jacl_os_statnorm(&kbuf, statbuf);

	return 0;
}

static inline int chmod(const char *pathname, mode_t mode) {
	if (!pathname) return (__errno_set(EINVAL), -1);

	#if JACL_HASSYS(chmod)
		if ((int)syscall(SYS_chmod, pathname, mode) < 0) return -1;
	#elif JACL_HASSYS(fchmodat)
		if ((int)syscall(SYS_fchmodat, AT_FDCWD, pathname, mode) < 0) return -1;
	#else
		return (__errno_set(ENOSYS), -1);
	#endif

	return 0;
}

static inline int mkdir(const char *pathname, mode_t mode) {
	if (!pathname) return (__errno_set(EINVAL), -1);

	#if JACL_HASSYS(mkdir)
		if ((int)syscall(SYS_mkdir, pathname, mode) < 0) return -1;
	#elif JACL_HASSYS(mkdirat)
		if ((int)syscall(SYS_mkdirat, AT_FDCWD, pathname, mode) < 0) return -1;
	#else
		return (__errno_set(ENOSYS), -1);
	#endif

	return 0;
}

static inline int fchmod(int fd, mode_t mode) {
	#if JACL_HASSYS(fchmod)
		if ((int)syscall(SYS_fchmod, fd, mode) < 0) return -1;
		return 0;
	#else
		(void)fd; (void)mode;

		return (__errno_set(ENOSYS), -1);
	#endif
}

static inline mode_t umask(mode_t mask) {
	return (mode_t)syscall(SYS_umask, mask);
}

static inline int fstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags) {
	if (!pathname || !statbuf) return (__errno_set(EINVAL), -1);

	struct __jacl_os_stat kbuf;

	#if JACL_HASSYS(newfstatat)
		if ((int)syscall(SYS_newfstatat, dirfd, pathname, &kbuf, flags) < 0) return -1;
	#else
		return (__errno_set(ENOSYS), -1);
	#endif

	__jacl_os_statnorm(&kbuf, statbuf);

	return 0;
}

static inline int mknod(const char *pathname, mode_t mode, dev_t dev) {
	if (!pathname) return (__errno_set(EINVAL), -1);

	if ((int)syscall(SYS_mknod, pathname, mode, dev) < 0) return -1;
	return 0;
}

static inline int mkfifo(const char *pathname, mode_t mode) {
	return mknod(pathname, mode | S_IFIFO, 0);
}

#if JACL_HAS_LFS
static inline int stat64(const char *pathname, struct stat64 *statbuf) {
	if (!pathname || !statbuf) return (__errno_set(EINVAL), -1);

	#if JACL_HASSYS(stat64)
		if ((int)syscall(SYS_stat64, pathname, statbuf) < 0) return -1;
		return 0;
	#else
		return (__errno_set(ENOSYS), -1);
	#endif
}

static inline int fstat64(int fd, struct stat64 *statbuf) {
	if (!statbuf) return (__errno_set(EINVAL), -1);

	#if JACL_HASSYS(fstat64)
		if ((int)syscall(SYS_fstat64, fd, statbuf) < 0) return -1;
		return 0;
	#else
		return (__errno_set(ENOSYS), -1);
	#endif
}

static inline int lstat64(const char *pathname, struct stat64 *statbuf) {
	if (!pathname || !statbuf) return (__errno_set(EINVAL), -1);

	#if JACL_HASSYS(lstat64)
		if ((int)syscall(SYS_lstat64, pathname, statbuf) < 0) return -1;
		return 0;
	#else
		return (__errno_set(ENOSYS), -1);
	#endif
}
#endif /* JACL_HAS_LFS */

static inline int utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags) {
	if (!pathname) return (__errno_set(EINVAL), -1);

	#if JACL_HASSYS(utimensat)
		if ((int)syscall(SYS_utimensat, dirfd, pathname, times, flags) < 0) return -1;
		return 0;
	#else
		return (__errno_set(ENOSYS), -1);
	#endif
}

static inline int futimens(int fd, const struct timespec times[2]) {
	#if JACL_HASSYS(futimens)
		if ((int)syscall(SYS_futimens, fd, times) < 0) return -1;
		return 0;
	#else
		return (__errno_set(ENOSYS), -1);
	#endif
}

static inline int statvfs(const char *path, struct statvfs *buf) {
	if (!path || !buf) return (__errno_set(EINVAL), -1);

#if JACL_HASSYS(statvfs)
	if ((int)syscall(SYS_statvfs, path, buf) < 0) return -1;
	return 0;
#elif JACL_HASSYS(fstatvfs)
	int fd = open(path, O_RDONLY | O_DIRECTORY);

	if (fd < 0) return -1;

	int r = (int)syscall(SYS_fstatvfs, fd, buf);

	close(fd);

	if (r < 0) return -1;
	return 0;
#else
	return (__errno_set(ENOSYS), -1);
#endif
}

static inline int fstatvfs(int fd, struct statvfs *buf) {
	if (!buf) return (__errno_set(EINVAL), -1);

#if JACL_HASSYS(fstatvfs)
	if ((int)syscall(SYS_fstatvfs, fd, buf) < 0) return -1;
	return 0;
#else
	return (__errno_set(ENOSYS), -1);
#endif
}

#else /* !JACL_HAS_POSIX */

static inline int stat(const char *pathname, struct stat *statbuf) { (void)pathname; (void)statbuf; return (__errno_set(ENOSYS), -1); }
static inline int fstat(int fd, struct stat *statbuf) { (void)fd; (void)statbuf; return (__errno_set(ENOSYS), -1); }
static inline int lstat(const char *pathname, struct stat *statbuf) { (void)pathname; (void)statbuf; return (__errno_set(ENOSYS), -1); }
static inline int chmod(const char *pathname, mode_t mode) { (void)pathname; (void)mode; return (__errno_set(ENOSYS), -1); }
static inline int mkdir(const char *pathname, mode_t mode) { (void)pathname; (void)mode; return (__errno_set(ENOSYS), -1); }
static inline int fchmod(int fd, mode_t mode) { (void)fd; (void)mode; return (__errno_set(ENOSYS), -1); }
static inline mode_t umask(mode_t mask) { (void)mask; return 0; }
static inline int fstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags) { (void)dirfd; (void)pathname; (void)statbuf; (void)flags; return (__errno_set(ENOSYS), -1); }
static inline int mknod(const char *pathname, mode_t mode, dev_t dev) { (void)pathname; (void)mode; (void)dev; return (__errno_set(ENOSYS), -1); }
static inline int mkfifo(const char *pathname, mode_t mode) { (void)pathname; (void)mode; return (__errno_set(ENOSYS), -1); }

#if JACL_HAS_LFS
static inline int stat64(const char *pathname, struct stat64 *statbuf) { (void)pathname; (void)statbuf; return (__errno_set(ENOSYS), -1); }
static inline int fstat64(int fd, struct stat64 *statbuf) { (void)fd; (void)statbuf; return (__errno_set(ENOSYS), -1); }
static inline int lstat64(const char *pathname, struct stat64 *statbuf) { (void)pathname; (void)statbuf; return (__errno_set(ENOSYS), -1); }
#endif

static inline int utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags) { (void)dirfd; (void)pathname; (void)times; (void)flags; return (__errno_set(ENOSYS), -1); }
static inline int futimens(int fd, const struct timespec times[2]) { (void)fd; (void)times; return (__errno_set(ENOSYS), -1); }
static inline int statvfs(const char *path, struct statvfs *buf) { (void)path; (void)buf; return (__errno_set(ENOSYS), -1); }
static inline int fstatvfs(int fd, struct statvfs *buf) { (void)fd; (void)buf; return (__errno_set(ENOSYS), -1); }

#endif /* JACL_HAS_POSIX */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_STAT_H */
