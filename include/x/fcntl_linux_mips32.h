/**
 * LINUX FCNTL CONSTANTS  //  last updated: 2026-03-04 00:30:03 UTC
 * https://raw.githubusercontent.com/torvalds/linux/v6.17/arch/mips/include/uapi/asm/fcntl.h
 */

/* Open flags */
#define FCNTL_O_FLAGS(X) \
	X(O_CLOEXEC, 0) \
	X(O_DIRECTORY, 0) \
	X(O_RDONLY, 0) \
	X(O_WRONLY, 1) \
	X(O_RDWR, 2) \
	X(O_APPEND, 0x0008) \
	X(O_DSYNC, 0x0010) \
	X(O_RSYNC, 0x0010) \
	X(O_SYNC, 0x0010) \
	X(O_NONBLOCK, 0x0080) \
	X(O_CREAT, 0x0100) \
	X(O_TRUNC, 0x0200) \
	X(O_EXCL, 0x0400) \
	X(O_NOCTTY, 0x0800) \
	X(O_LARGEFILE, 0x2000) \
	X(O_DIRECT, 0x8000)

/* Fcntl commands */
#define FCNTL_F_COMMANDS(X) \
	X(F_DUPFD, 0) \
	X(F_GETFD, 1) \
	X(F_SETFD, 2) \
	X(F_GETFL, 3) \
	X(F_SETFL, 4) \
	X(F_SETLK, 6) \
	X(F_SETLKW, 7) \
	X(F_GETLK, 14) \
	X(F_GETOWN, 23) \
	X(F_SETOWN, 24) \
	X(F_GETLK64, 33) \
	X(F_SETLK64, 34) \
	X(F_SETLKW64, 35) \
	X(F_DUPFD_CLOEXEC, 1030)

/* File descriptor flags */
#define FCNTL_FD_FLAGS(X) \
	X(FD_CLOEXEC, 1)

/* Lock types */
#define FCNTL_LOCK_TYPES(X) \
	X(F_RDLCK, 0) \
	X(F_WRLCK, 1) \
	X(F_UNLCK, 2)

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

