/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef SYS_RANDOM_H
#define SYS_RANDOM_H
#pragma once

#include <config.h>
#include <sys/types.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GRND_NONBLOCK 0x0001
#define GRND_RANDOM   0x0002

#if JACL_HAS_POSIX

#include <fcntl.h>
static inline ssize_t getrandom(void *buf, size_t buflen, unsigned int flags) {
	const char *dev = (flags & GRND_RANDOM) ? "/dev/random" : "/dev/urandom";
	int fd = syscall(SYS_open, dev, O_RDONLY, 0);
	if(fd < 0) return -1;
	
	ssize_t total = 0;
	while(total < (ssize_t)buflen) {
		ssize_t n = syscall(SYS_read, fd, (char*)buf + total, buflen - total);
		if(n <= 0) {
			syscall(SYS_close, fd);
			return -1;
		}
		total += n;
	}
	
	syscall(SYS_close, fd);
	return total;
}

#elif JACL_OS_WINDOWS

#include <sys/syscall.h>
static inline ssize_t getrandom(void *buf, size_t buflen, unsigned int flags) {
	(void)flags;
	return syscall(SYS_getrandom, buf, buflen, 0);
}

#elif JACL_OS_WASI

extern int __wasi_random_get(uint8_t *buf, size_t buflen);
static inline ssize_t getrandom(void *buf, size_t buflen, unsigned int flags) {
	(void)flags;
	return __wasi_random_get(buf, buflen) == 0 ? (ssize_t)buflen : -1;
}

#elif JACL_OS_JSRUN

static inline ssize_t getrandom(void *buf, size_t buflen, unsigned int flags) {
	(void)buf; (void)buflen; (void)flags;
	errno = ENOSYS;
	return -1;
}

#elif JACL_OS_NONE

static inline ssize_t getrandom(void *buf, size_t buflen, unsigned int flags) {
	(void)buf; (void)buflen; (void)flags;
	errno = ENOSYS;
	return -1;
}

#else
#error "Unsupported OS for getrandom()"
#endif

#ifdef __cplusplus
}
#endif
#endif /* SYS_RANDOM_H */

