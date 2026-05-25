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
    int saved_errno = errno;
    (void)nbyte;

    if (dev_info_ptr) *dev_info_ptr = 0;

    /* 1. Validate file descriptor */
    if (fildes < 0) { errno = saved_errno; return EBADF; }
    if (fcntl(fildes, F_GETFL) == -1) { errno = saved_errno; return EBADF; }

    /* 2. Verify character special device requirement */
    struct stat st;
    if (fstat(fildes, &st) == 0 && !S_ISCHR(st.st_mode)) {
        errno = saved_errno;
        return ENOTTY;
    }

    /* 3. Delegate to kernel or return ENOSYS */
    #ifdef SYS_ioctl
        long rc = syscall(SYS_ioctl, fildes, dcmd, dev_data_ptr);
        if (dev_info_ptr) *dev_info_ptr = (int)rc;
        errno = saved_errno;
        return (rc >= 0) ? 0 : -rc;
    #else
        errno = saved_errno;
        return ENOSYS;
    #endif
}

#ifdef __cplusplus
}
#endif
#endif /* _DEVCTL_H */
