/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _SYS_IPC_H
#define _SYS_IPC_H

#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================ */
/* Constants                                                        */
/* ================================================================ */

/* Mode bits */
#define IPC_CREAT   01000   /* Create entry if key does not exist */
#define IPC_EXCL    02000   /* Fail if key exists */
#define IPC_NOWAIT  04000   /* Error if request would need to wait */

/* Keys */
#define IPC_PRIVATE ((key_t)0) /* Private key */

/* Control commands */
#define IPC_RMID    0         /* Remove identifier */
#define IPC_SET     1         /* Set options */
#define IPC_STAT    2         /* Get options */

/* ================================================================ */
/* Structures                                                       */
/* ================================================================ */

/* IPC Permission Structure */
struct ipc_perm {
    uid_t  uid;   /* Owner's user ID */
    gid_t  gid;   /* Owner's group ID */
    uid_t  cuid;  /* Creator's user ID */
    gid_t  cgid;  /* Creator's group ID */
    mode_t mode;  /* Read/write permission */
};

/* ================================================================ */
/* Functions                                                        */
/* ================================================================ */

/**
 * ftok: Generate a System V IPC key from a pathname and project ID.
 * Uses stat() to get inode and device, then combines with id.
 */
static inline key_t ftok(const char *path, int id) {
    struct stat st;
    if (stat(path, &st) < 0) {
        return (key_t)-1;
    }

    /* Standard algorithm: lower 8 bits of id, lower 16 bits of inode, lower 8 bits of device */
    key_t key = (key_t)(
        ((id & 0xFF) << 24) |
        ((st.st_ino & 0xFFFF) << 8) |
        (st.st_dev & 0xFF)
    );

    return key;
}

#ifdef __cplusplus
}
#endif

#endif /* _SYS_IPC_H */
