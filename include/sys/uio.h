/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _SYS_UIO_H
#define _SYS_UIO_H
#pragma once

/**
 * NOTE: POSIX Scatter/Gather I/O
 *
 * Defines struct iovec and vectorized read/write functions.
 * Required by <sys/socket.h> for msghdr.msg_iov.
 */

#include <config.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

struct iovec {
	void	*iov_base;
	size_t	iov_len;
};

#if JACL_HAS_POSIX
static inline ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
	return syscall(SYS_readv, fd, iov, iovcnt);
}
static inline ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
	return syscall(SYS_writev, fd, iov, iovcnt);
}
static inline ssize_t preadv(int fd, const struct iovec *iov, int iovcnt, off_t offset) {
	return syscall(SYS_preadv, fd, iov, iovcnt, offset);
}
static inline ssize_t pwritev(int fd, const struct iovec *iov, int iovcnt, off_t offset) {
	return syscall(SYS_pwritev, fd, iov, iovcnt, offset);
}
#else
static inline ssize_t readv(int fd, const struct iovec *iov, int iovcnt) { (void)fd;(void)iov;(void)iovcnt; errno=ENOSYS; return -1; }
static inline ssize_t writev(int fd, const struct iovec *iov, int iovcnt) { (void)fd;(void)iov;(void)iovcnt; errno=ENOSYS; return -1; }
static inline ssize_t preadv(int fd, const struct iovec *iov, int iovcnt, off_t off) { (void)fd;(void)iov;(void)iovcnt;(void)off; errno=ENOSYS; return -1; }
static inline ssize_t pwritev(int fd, const struct iovec *iov, int iovcnt, off_t off) { (void)fd;(void)iov;(void)iovcnt;(void)off; errno=ENOSYS; return -1; }
#endif

#ifdef __cplusplus
}
#endif
#endif /* _SYS_UIO_H */
