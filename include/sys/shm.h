/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _SYS_SHM_H
#define _SYS_SHM_H
#pragma once

#include <config.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================ */
/* Constants                                                                    */
/* ============================================================================ */

#ifndef SHM_RDONLY
#define SHM_RDONLY      010000
#endif
#ifndef SHM_RND
#define SHM_RND         020000
#endif
#ifndef SHM_REMAP
#define SHM_REMAP       040000
#endif
#ifndef SHM_EXEC
#define SHM_EXEC        0100000
#endif
#ifndef SHM_LOCK
#define SHM_LOCK        11
#endif
#ifndef SHM_UNLOCK
#define SHM_UNLOCK      12
#endif
#ifndef SHM_STAT
#define SHM_STAT        13
#endif
#ifndef SHM_INFO
#define SHM_INFO        14
#endif
#ifndef SHM_HUGETLB
#define SHM_HUGETLB     04000
#endif
#ifndef SHM_NORESERVE
#define SHM_NORESERVE   0100000
#endif
#ifndef SHMLBA
#define SHMLBA          4096
#endif
#ifndef SHM_FAILED
#define SHM_FAILED      ((void *)(intptr_t)-1)
#endif

#ifndef IPC_CREAT
#define IPC_CREAT       01000
#endif
#ifndef IPC_EXCL
#define IPC_EXCL        02000
#endif
#ifndef IPC_NOWAIT
#define IPC_NOWAIT      04000
#endif
#ifndef IPC_PRIVATE
#define IPC_PRIVATE     0
#endif
#ifndef IPC_RMID
#define IPC_RMID        0
#endif
#ifndef IPC_SET
#define IPC_SET         1
#endif
#ifndef IPC_STAT
#define IPC_STAT        2
#endif
#ifndef IPC_INFO
#define IPC_INFO        3
#endif

/* ============================================================================ */
/* Structures                                                                   */
/* ============================================================================ */

#ifndef __shmatt_t_defined
typedef unsigned long shmatt_t;
#define __shmatt_t_defined
#endif

struct shmid_ds {
    struct ipc_perm shm_perm;
    size_t          shm_segsz;
    time_t          shm_atime;
    time_t          shm_dtime;
    time_t          shm_ctime;
    pid_t           shm_cpid;
    pid_t           shm_lpid;
    shmatt_t        shm_nattch;
};

/* ============================================================================ */
/* Internal State                                                               */
/* ============================================================================ */

#define JACL_SHM_MAX_SEGMENTS 256

typedef struct {
    int             id;
    key_t           key;
    void           *addr;
    size_t          size;
    int             flags;
    int             deleted;
    shmatt_t        nattch;
    struct shmid_ds meta;
    int             active;
} __jacl_shm_seg;

static __jacl_shm_seg __jacl_shm_registry[JACL_SHM_MAX_SEGMENTS] = {0};
static int __jacl_shm_next_id = 1;

/* ============================================================================ */
/* Helpers: Lookups                                                             */
/* ============================================================================ */

static inline __jacl_shm_seg *__jacl_shm_find_by_id(int shmid) {
    for (int i = 0; i < JACL_SHM_MAX_SEGMENTS; i++) {
        if (__jacl_shm_registry[i].active && __jacl_shm_registry[i].id == shmid)
            return &__jacl_shm_registry[i];
    }
    return NULL;
}

static inline __jacl_shm_seg *__jacl_shm_find_by_key(key_t key) {
    for (int i = 0; i < JACL_SHM_MAX_SEGMENTS; i++) {
        if (__jacl_shm_registry[i].active && __jacl_shm_registry[i].key == key)
            return &__jacl_shm_registry[i];
    }
    return NULL;
}

static inline __jacl_shm_seg *__jacl_shm_find_by_addr(const void *addr) {
    for (int i = 0; i < JACL_SHM_MAX_SEGMENTS; i++) {
        if (__jacl_shm_registry[i].active && __jacl_shm_registry[i].addr == addr)
            return &__jacl_shm_registry[i];
    }
    return NULL;
}

static inline __jacl_shm_seg *__jacl_shm_alloc_slot(void) {
    for (int i = 0; i < JACL_SHM_MAX_SEGMENTS; i++) {
        if (!__jacl_shm_registry[i].active) return &__jacl_shm_registry[i];
    }
    return NULL;
}

/* ============================================================================ */
/* Helpers: Mapping & Teardown                                                  */
/* ============================================================================ */

static inline void *__jacl_shm_map_file(key_t key, size_t *size, int shmflg) {
    char path[256];
    snprintf(path, sizeof(path), "/tmp/jacl_sysv_shm_%d", (int)key);

    int fd = open(path, O_RDWR);
    if (fd >= 0 && (shmflg & IPC_EXCL)) {
        close(fd);
        errno = EEXIST;
        return MAP_FAILED;
    }

    if (fd < 0) {
        fd = open(path, O_RDWR | O_CREAT | O_EXCL, (shmflg & 0777));
        if (fd < 0) {
            fd = open(path, O_RDWR);
            if (fd >= 0 && (shmflg & IPC_EXCL)) { close(fd); errno = EEXIST; return MAP_FAILED; }
            if (fd < 0) return MAP_FAILED;
        }
        if (ftruncate(fd, *size) < 0) {
            close(fd);
            unlink(path);
            return MAP_FAILED;
        }
    } else {
        struct stat st;
        if (fstat(fd, &st) == 0) {
            if (*size > 0 && *size > (size_t)st.st_size) { close(fd); errno = EINVAL; return MAP_FAILED; }
            if (*size == 0) *size = st.st_size;
        }
    }

    void *addr = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    return addr;
}

static inline void __jacl_shm_init_meta(__jacl_shm_seg *seg, key_t key, size_t size, int shmflg, void *addr) {
    seg->id = __jacl_shm_next_id++;
    seg->key = key;
    seg->addr = addr;
    seg->size = size;
    seg->flags = shmflg;
    seg->deleted = 0;
    seg->nattch = 0;
    seg->active = 1;

    struct shmid_ds *m = &seg->meta;
    memset(m, 0, sizeof(*m));
    m->shm_perm.uid = getuid();
    m->shm_perm.gid = getgid();
    m->shm_perm.cuid = getuid();
    m->shm_perm.cgid = getgid();
    m->shm_perm.mode = (shmflg & 0777);
    m->shm_segsz = size;
    m->shm_ctime = time(NULL);
    m->shm_cpid = getpid();
}

static inline void __jacl_shm_destroy(__jacl_shm_seg *seg) {
    munmap(seg->addr, seg->size);
    if (seg->key != IPC_PRIVATE) {
        char path[256];
        snprintf(path, sizeof(path), "/tmp/jacl_sysv_shm_%d", (int)seg->key);
        unlink(path);
    }
    seg->active = 0;
}

/* ============================================================================ */
/* Public API                                                                   */
/* ============================================================================ */

static inline int shmget(key_t key, size_t size, int shmflg) {
    if (size == 0 && (shmflg & IPC_CREAT)) { errno = EINVAL; return -1; }

    if (key != IPC_PRIVATE) {
        __jacl_shm_seg *existing = __jacl_shm_find_by_key(key);
        if (existing) {
            if ((shmflg & IPC_CREAT) && (shmflg & IPC_EXCL)) { errno = EEXIST; return -1; }
            if (size > 0 && size > existing->size) { errno = EINVAL; return -1; }
            return existing->id;
        }
    }

    if (!(shmflg & IPC_CREAT)) { errno = ENOENT; return -1; }

    __jacl_shm_seg *seg = __jacl_shm_alloc_slot();
    if (!seg) { errno = ENOSPC; return -1; }

    void *addr;
    if (key == IPC_PRIVATE) {
        addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    } else {
        addr = __jacl_shm_map_file(key, &size, shmflg);
    }

    if (addr == MAP_FAILED) return -1;

    __jacl_shm_init_meta(seg, key, size, shmflg, addr);
    return seg->id;
}

static inline void *shmat(int shmid, const void *shmaddr, int shmflg) {
    __jacl_shm_seg *seg = __jacl_shm_find_by_id(shmid);
    if (!seg) { errno = EINVAL; return (void *)-1; }
    if (seg->deleted) { errno = EIDRM; return (void *)-1; }

    if (shmaddr) {
        if (shmflg & SHM_RND) {
            uintptr_t a = (uintptr_t)shmaddr & ~(SHMLBA - 1);
            shmaddr = (const void *)a;
        }
        if (shmaddr != seg->addr && !(shmflg & SHM_REMAP)) {
            errno = EINVAL;
            return (void *)-1;
        }
    }

    int prot = (shmflg & SHM_RDONLY) ? PROT_READ : (PROT_READ | PROT_WRITE);
    if (mprotect(seg->addr, seg->size, prot) == -1) return (void *)-1;

    seg->nattch++;
    seg->meta.shm_nattch++;
    seg->meta.shm_atime = time(NULL);
    seg->meta.shm_lpid = getpid();

    return seg->addr;
}

static inline int shmdt(const void *shmaddr) {
    __jacl_shm_seg *seg = __jacl_shm_find_by_addr(shmaddr);
    if (!seg) { errno = EINVAL; return -1; }
    if (seg->nattch == 0) { errno = EINVAL; return -1; }

    seg->nattch--;
    seg->meta.shm_nattch--;
    seg->meta.shm_dtime = time(NULL);
    seg->meta.shm_lpid = getpid();

    if (seg->nattch == 0 && seg->deleted) {
        __jacl_shm_destroy(seg);
    }
    return 0;
}

static inline int shmctl(int shmid, int cmd, struct shmid_ds *buf) {
	if (cmd == IPC_INFO || cmd == SHM_INFO) {
		if (!buf) { errno = EINVAL; return -1; }

		memset(buf, 0, sizeof(*buf));

		buf->shm_segsz = JACL_SHM_MAX_SEGMENTS;

		return 0;
	}

	__jacl_shm_seg *seg = __jacl_shm_find_by_id(shmid);

	if (!seg) { errno = EINVAL; return -1; }

	switch (cmd) {
		case IPC_STAT:
			if (!buf) { errno = EINVAL; return -1; }

			memcpy(buf, &seg->meta, sizeof(struct shmid_ds));

			return 0;
		case IPC_SET:
			if (!buf) { errno = EINVAL; return -1; }

			seg->meta.shm_perm.uid = buf->shm_perm.uid;
			seg->meta.shm_perm.gid = buf->shm_perm.gid;
			seg->meta.shm_perm.mode = buf->shm_perm.mode & 0777;
			seg->meta.shm_ctime = time(NULL);

			return 0;
		case IPC_RMID:
			seg->deleted = 1;

			if (seg->nattch == 0) __jacl_shm_destroy(seg);

			return 0;
		case SHM_STAT:
			if (!buf) { errno = EINVAL; return -1; }

			memcpy(buf, &seg->meta, sizeof(struct shmid_ds));

			return seg->id;
		case SHM_LOCK:
			return mlock(seg->addr, seg->size);
		case SHM_UNLOCK:
			return munlock(seg->addr, seg->size);
		default:
			errno = EINVAL;

			return -1;
	}
}

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SHM_H */
