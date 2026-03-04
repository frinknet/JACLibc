/**
 * OPENBSD FCNTL CONSTANTS  //  last updated: 2026-03-04 00:30:10 UTC
 * https://cvsweb.openbsd.org/checkout/src/sys/sys/fcntl.h
 */

/* Open flags */
#define FCNTL_O_FLAGS(X) \
	X(O_RDONLY, 0x0000) \
	X(O_WRONLY, 0x0001) \
	X(O_RDWR, 0x0002) \
	X(O_ACCMODE, 0x0003) \
	X(O_NDELAY, 0x0004) \
	X(O_NONBLOCK, 0x0004) \
	X(O_APPEND, 0x0008) \
	X(O_SHLOCK, 0x0010) \
	X(O_EXLOCK, 0x0020) \
	X(O_ASYNC, 0x0040) \
	X(O_DSYNC, 0x0080) \
	X(O_FSYNC, 0x0080) \
	X(O_RSYNC, 0x0080) \
	X(O_SYNC, 0x0080) \
	X(O_NOFOLLOW, 0x0100) \
	X(O_CREAT, 0x0200) \
	X(O_TRUNC, 0x0400) \
	X(O_EXCL, 0x0800) \
	X(O_NOCTTY, 0x8000) \
	X(O_CLOEXEC, 0x10000) \
	X(O_DIRECTORY, 0x20000) \
	X(O_CLOFORK, 0x40000)

/* Fcntl commands */
#define FCNTL_F_COMMANDS(X) \
	X(F_DUPFD, 0) \
	X(F_GETFD, 1) \
	X(F_SETFD, 2) \
	X(F_GETFL, 3) \
	X(F_SETFL, 4) \
	X(F_GETOWN, 5) \
	X(F_SETOWN, 6) \
	X(F_GETLK, 7) \
	X(F_SETLK, 8) \
	X(F_SETLKW, 9) \
	X(F_DUPFD_CLOEXEC, 10) \
	X(F_ISATTY, 11) \
	X(F_DUPFD_CLOFORK, 12) \
	X(F_WAIT, 0x010) \
	X(F_FLOCK, 0x020) \
	X(F_POSIX, 0x040) \
	X(F_INTR, 0x080)

/* File descriptor flags */
#define FCNTL_FD_FLAGS(X) \
	X(FD_CLOEXEC, 1) \
	X(FD_CLOFORK, 4)

/* Lock types */
#define FCNTL_LOCK_TYPES(X) \
	X(F_RDLCK, 1) \
	X(F_UNLCK, 2) \
	X(F_WRLCK, 3)

/* AT function flags */
#define FCNTL_AT_FLAGS(X) \
	X(AT_FDCWD, -100) \
	X(AT_EACCESS, 0x01) \
	X(AT_SYMLINK_NOFOLLOW, 0x02) \
	X(AT_SYMLINK_FOLLOW, 0x04) \
	X(AT_REMOVEDIR, 0x08)

/* POSIX advisory flags */
#define FCNTL_POSIX_FADV(X) \
	X(POSIX_FADV_NORMAL, 0) \
	X(POSIX_FADV_RANDOM, 1) \
	X(POSIX_FADV_SEQUENTIAL, 2) \
	X(POSIX_FADV_WILLNEED, 3) \
	X(POSIX_FADV_DONTNEED, 4) \
	X(POSIX_FADV_NOREUSE, 5)

/* Seek constants */
#define FCNTL_SEEK(X) \
	X(SEEK_SET, 0) \
	X(SEEK_CUR, 1) \
	X(SEEK_END, 2)

