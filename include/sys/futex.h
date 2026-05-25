/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _SYS_FUTEX_H
#define _SYS_FUTEX_H
#pragma once

#include <config.h>
#include <sys/syscall.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================ */
/* Futex Operations (Linux-compatible ABI)                                      */
/* ============================================================================ */
#define FUTEX_WAIT            0
#define FUTEX_WAKE            1
#define FUTEX_FD              2
#define FUTEX_REQUEUE         3
#define FUTEX_CMP_REQUEUE     4
#define FUTEX_WAKE_OP         5
#define FUTEX_LOCK_PI         6
#define FUTEX_UNLOCK_PI       7
#define FUTEX_TRYLOCK_PI      8
#define FUTEX_WAIT_BITSET     9
#define FUTEX_WAKE_BITSET    10
#define FUTEX_WAIT_REQUEUE_PI 11
#define FUTEX_CMP_REQUEUE_PI 12

#define FUTEX_PRIVATE_FLAG   128
#define FUTEX_CLOCK_REALTIME 256
#define __FUTEX_BUCKETS      64

static inline size_t __futex_hash(uint32_t *uaddr) {
    return ((uintptr_t)uaddr >> 2) & (__FUTEX_BUCKETS - 1);
}

static inline int futex(uint32_t *uaddr, int futex_op, uint32_t val,  const struct timespec *timeout, uint32_t *uaddr2, uint32_t val3);

#if JACL_OS_LINUX || JACL_OS_NETBSD
/* Native syscall */
static inline int futex(uint32_t *uaddr, int futex_op, uint32_t val,
                        const struct timespec *timeout, uint32_t *uaddr2, uint32_t val3) {
    return (int)syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

#elif JACL_OS_WINDOWS
/* Win8+ WaitOnAddress/WakeByAddressAll */
#include <windows.h>
#include <synchapi.h>

static inline int futex(uint32_t *uaddr, int futex_op, uint32_t val,
                        const struct timespec *timeout, uint32_t *uaddr2, uint32_t val3) {
    (void)uaddr2; (void)val3;
    if (futex_op == FUTEX_WAIT || futex_op == FUTEX_WAIT_BITSET) {
        if (*uaddr != val) { errno = EAGAIN; return -1; }
        DWORD ms = timeout ? (DWORD)(timeout->tv_sec * 1000ULL + timeout->tv_nsec / 1000000ULL) : INFINITE;
        if (!WaitOnAddress(uaddr, &val, sizeof(uint32_t), ms)) {
            errno = (GetLastError() == 0x102) ? ETIMEDOUT : EINTR;
            return -1;
        }
        return 0;
    }
    if (futex_op == FUTEX_WAKE || futex_op == FUTEX_WAKE_BITSET) {
        WakeByAddressAll(uaddr);
        return 0;
    }
    errno = ENOSYS;
    return -1;
}

#else
/* POSIX Fallback: Darwin, OpenBSD, FreeBSD, DragonFly, WASM, etc. */
#include <pthread.h>

extern pthread_mutex_t __futex_locks[__FUTEX_BUCKETS];
extern pthread_cond_t  __futex_conds[__FUTEX_BUCKETS];

static inline int futex(uint32_t *uaddr, int futex_op, uint32_t val,
                        const struct timespec *timeout, uint32_t *uaddr2, uint32_t val3) {
    (void)uaddr2; (void)val3;
    size_t idx = __futex_hash(uaddr);
    pthread_mutex_t *lock = &__futex_locks[idx];
    pthread_cond_t  *cond = &__futex_conds[idx];
    int ret = 0;

    if (futex_op == FUTEX_WAIT || futex_op == FUTEX_WAIT_BITSET) {
        if (*uaddr != val) { errno = EAGAIN; return -1; }
        pthread_mutex_lock(lock);
        if (*uaddr != val) { pthread_mutex_unlock(lock); errno = EAGAIN; return -1; }
        ret = timeout ? pthread_cond_timedwait(cond, lock, timeout) : pthread_cond_wait(cond, lock);
        pthread_mutex_unlock(lock);
        if (ret == ETIMEDOUT) errno = ETIMEDOUT;
        return ret ? -1 : 0;
    }
    if (futex_op == FUTEX_WAKE || futex_op == FUTEX_WAKE_BITSET) {
        pthread_mutex_lock(lock);
        for (uint32_t i = 0; i < val; i++) pthread_cond_signal(cond);
        pthread_mutex_unlock(lock);
        return 0;
    }
    errno = ENOSYS;
    return -1;
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SYS_FUTEX_H */
