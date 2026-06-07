/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _DEVCTL_H
#define _DEVCTL_H
#pragma once

#include <config.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * posix_devctl: Device control interface (POSIX.26).
 * Returns 0 on success, or POSIX error number on failure.
 * Preserves errno across internal validation steps.
 */
static inline int posix_devctl(int fildes, int dcmd, void *restrict dev_data_ptr, size_t nbyte, int *restrict dev_info_ptr) {
	int olderr = __errno_get();
	(void)nbyte;

	if (dev_info_ptr) *dev_info_ptr = 0;

	if (fildes < 0) { return (__errno_set(olderr), EBADF); }
	if (fcntl(fildes, F_GETFL) == -1) { return (__errno_set(olderr), EBADF); }

	struct stat st;

	if (fstat(fildes, &st) == 0 && !S_ISCHR(st.st_mode)) { return (__errno_set(olderr), ENOTTY); }

	#ifdef SYS_ioctl
		long rc = syscall(SYS_ioctl, fildes, dcmd, dev_data_ptr);

		if (dev_info_ptr) *dev_info_ptr = (int)rc;

		return (__errno_set(olderr), (rc >= 0) ? 0 : -rc);
	#else
		return (__errno_set(olderr), __errno_val(ENOSYS));
	#endif
}

#ifdef __cplusplus
}
#endif
#endif /* _DEVCTL_H */
