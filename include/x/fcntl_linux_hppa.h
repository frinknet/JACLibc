/**
 * LINUX FCNTL CONSTANTS  //  last updated: 2026-03-04 00:30:01 UTC
 * https://raw.githubusercontent.com/torvalds/linux/v6.17/arch/parisc/include/uapi/asm/fcntl.h
 */

/* Open flags */
#define FCNTL_O_FLAGS(X) \
	X(O_RDONLY, 0) \
	X(O_TRUNC, 0) \
	X(O_WRONLY, 1) \
	X(O_RDWR, 2) \
	X(O_APPEND, 000000010) \
	X(O_NOFOLLOW, 000000200) \
	X(O_CREAT, 000000400) \
	X(O_EXCL, 000002000) \
	X(O_LARGEFILE, 000004000) \
	X(O_DIRECTORY, 000010000) \
	X(O_NONBLOCK, 000200000) \
	X(O_NOCTTY, 000400000) \
	X(O_DSYNC, 001000000) \
	X(O_RSYNC, 001000000) \
	X(O_SYNC, 01100000) \
	X(O_NOATIME, 004000000) \
	X(O_CLOEXEC, 010000000) \
	X(O_PATH, 020000000)

/* Fcntl commands */
#define FCNTL_F_COMMANDS(X) \
	X(F_DUPFD, 0) \
	X(F_GETFD, 1) \
	X(F_SETFD, 2) \
	X(F_GETFL, 3) \
	X(F_SETFL, 4) \
	X(F_GETLK64, 8) \
	X(F_SETLK64, 9) \
	X(F_SETLKW64, 10) \
	X(F_GETOWN, 11) \
	X(F_SETOWN, 12) \
	X(F_SETSIG, 13) \
	X(F_GETSIG, 14) \
	X(F_DUPFD_CLOEXEC, 1030)

/* File descriptor flags */
#define FCNTL_FD_FLAGS(X) \
	X(FD_CLOEXEC, 1)

/* Lock types */
#define FCNTL_LOCK_TYPES(X) \
	X(F_RDLCK, 01) \
	X(F_WRLCK, 02) \
	X(F_UNLCK, 03)

/* AT function flags */
#define FCNTL_AT_FLAGS(X) \
	X(AT_FDCWD, -100)

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

