/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _MQUEUE_H
#define _MQUEUE_H

#pragma once

#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pthread.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int mqd_t;

struct mq_attr {
    long mq_flags;
    long mq_maxmsg;
    long mq_msgsize;
    long mq_curmsgs;
};

typedef struct __jacl_mq_node {
    char *data;
    size_t len;
    unsigned prio;
    struct __jacl_mq_node *next;
} __jacl_mq_node_t;

typedef struct {
    char name[256];
    int active;
    struct mq_attr attr;
    __jacl_mq_node_t *head;
    int refcount;
    int unlinked;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} __jacl_mq_t;

#define __JACL_MQ_MAX 64
#define __JACL_MQ_DESC_MAX 256

static __jacl_mq_t __jacl_mq_pool[__JACL_MQ_MAX];
static pthread_mutex_t __jacl_mq_global = PTHREAD_MUTEX_INITIALIZER;

/* Per-descriptor state: each mq_open returns a unique descriptor index */
typedef struct {
    int active;     /* Descriptor slot in use */
    int pool_idx;   /* Index into __jacl_mq_pool */
    int oflags;     /* O_ACCMODE for this descriptor */
} __jacl_mq_desc_t;

static __jacl_mq_desc_t __jacl_mq_descs[__JACL_MQ_DESC_MAX];

/* Per-pool notification state */
typedef struct {
    int registered;           /* 1 if a process is registered */
    pid_t notify_pid;         /* PID of registered process */
    struct sigevent sev;      /* Copy of registration sigevent */
} __jacl_mq_notify_t;

static __jacl_mq_notify_t __jacl_mq_notify[__JACL_MQ_MAX];

static inline int __jacl_mq_name_valid(const char *name) {
    if (!name || name[0] != '/' || name[1] == '\0') return 0;
    if (strchr(name + 1, '/') != NULL) return 0;
    if (strlen(name) >= 256) return 0;
    return 1;
}

static inline __jacl_mq_t *__jacl_mq_find(const char *name) {
    for (int i = 0; i < __JACL_MQ_MAX; i++)
        if (__jacl_mq_pool[i].active && !__jacl_mq_pool[i].unlinked && strcmp(__jacl_mq_pool[i].name, name) == 0)
            return &__jacl_mq_pool[i];
    return NULL;
}

static inline int __jacl_mq_alloc(const char *name) {
    for (int i = 0; i < __JACL_MQ_MAX; i++) {
        if (!__jacl_mq_pool[i].active) {
            strncpy(__jacl_mq_pool[i].name, name, 255);
            __jacl_mq_pool[i].name[255] = '\0';
            __jacl_mq_pool[i].active = 1;
            __jacl_mq_pool[i].refcount = 0;
            __jacl_mq_pool[i].unlinked = 0;
            __jacl_mq_pool[i].head = NULL;
            __jacl_mq_pool[i].attr.mq_flags = 0;
            __jacl_mq_pool[i].attr.mq_curmsgs = 0;
            pthread_mutex_init(&__jacl_mq_pool[i].lock, NULL);
            pthread_cond_init(&__jacl_mq_pool[i].cond, NULL);
            memset(&__jacl_mq_notify[i], 0, sizeof(__jacl_mq_notify[i]));
            return i;
        }
    }
    return -1;
}

static inline int __jacl_mq_desc_alloc(int pool_idx, int oflags) {
    for (int i = 0; i < __JACL_MQ_DESC_MAX; i++) {
        if (!__jacl_mq_descs[i].active) {
            __jacl_mq_descs[i].active = 1;
            __jacl_mq_descs[i].pool_idx = pool_idx;
            __jacl_mq_descs[i].oflags = oflags & O_ACCMODE;
            return i;
        }
    }
    return -1;
}

static inline void __jacl_mq_destroy(__jacl_mq_t *mq) {
    while (mq->head) {
        __jacl_mq_node_t *tmp = mq->head;
        mq->head = tmp->next;
        free(tmp->data);
        free(tmp);
    }
    pthread_mutex_destroy(&mq->lock);
    pthread_cond_destroy(&mq->cond);
    mq->active = 0;
}

static inline void __jacl_mq_do_notify(int pool_idx) {
    __jacl_mq_notify_t *n = &__jacl_mq_notify[pool_idx];
    if (!n->registered) return;

    struct sigevent sev = n->sev;
    pid_t pid = n->notify_pid;

    /* Auto-deregister after firing (one-shot per empty-to-nonempty) */
    n->registered = 0;
    memset(&n->sev, 0, sizeof(n->sev));
    n->notify_pid = 0;

    switch (sev.sigev_notify) {
    case SIGEV_SIGNAL:
        kill(pid, sev.sigev_signo ? sev.sigev_signo : SIGUSR1);
        break;
    case SIGEV_THREAD:
        if (sev.sigev_notify_function) {
            pthread_t t;
            pthread_attr_t attr;
            if (sev.sigev_notify_attributes)
                memcpy(&attr, sev.sigev_notify_attributes, sizeof(attr));
            else
                pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            pthread_create(&t, &attr, (void *(*)(void *))sev.sigev_notify_function, sev.sigev_value.sival_ptr);
            pthread_attr_destroy(&attr);
        }
        break;
    case SIGEV_NONE:
    default:
        break;
    }
}

static inline mqd_t mq_open(const char *name, int oflag, ...) {
    if (!__jacl_mq_name_valid(name)) { errno = EINVAL; return -1; }
    if ((oflag & O_EXCL) && !(oflag & O_CREAT)) { errno = EINVAL; return -1; }

    pthread_mutex_lock(&__jacl_mq_global);
    __jacl_mq_t *mq = __jacl_mq_find(name);

    if (!mq) {
        if (!(oflag & O_CREAT)) { pthread_mutex_unlock(&__jacl_mq_global); errno = ENOENT; return -1; }
        int id = __jacl_mq_alloc(name);
        if (id < 0) { pthread_mutex_unlock(&__jacl_mq_global); errno = ENFILE; return -1; }
        mq = &__jacl_mq_pool[id];

        va_list ap; va_start(ap, oflag);
        (void)va_arg(ap, int); /* mode_t */
        struct mq_attr *attr = va_arg(ap, struct mq_attr *);
        va_end(ap);

        if (attr) {
            if (attr->mq_maxmsg <= 0 || attr->mq_msgsize <= 0) {
                pthread_mutex_unlock(&__jacl_mq_global); errno = EINVAL; return -1;
            }
            mq->attr.mq_flags = attr->mq_flags;
            mq->attr.mq_maxmsg = attr->mq_maxmsg;
            mq->attr.mq_msgsize = attr->mq_msgsize;
        } else {
            mq->attr.mq_maxmsg = 10;
            mq->attr.mq_msgsize = 8192;
        }
    } else if (oflag & O_CREAT) {
        pthread_mutex_unlock(&__jacl_mq_global); errno = EEXIST; return -1;
    }

    int pool_idx = (int)(mq - __jacl_mq_pool);
    int desc = __jacl_mq_desc_alloc(pool_idx, oflag);
    if (desc < 0) { pthread_mutex_unlock(&__jacl_mq_global); errno = EMFILE; return -1; }

    mq->refcount++;
    if (oflag & O_NONBLOCK) mq->attr.mq_flags |= O_NONBLOCK;
    pthread_mutex_unlock(&__jacl_mq_global);
    return (mqd_t)desc;
}

static inline int mq_close(mqd_t mqdes) {
    if (mqdes < 0 || mqdes >= __JACL_MQ_DESC_MAX) { errno = EBADF; return -1; }
    __jacl_mq_desc_t *desc = &__jacl_mq_descs[mqdes];
    if (!desc->active) { errno = EBADF; return -1; }

    pthread_mutex_lock(&__jacl_mq_global);
    __jacl_mq_t *mq = &__jacl_mq_pool[desc->pool_idx];

    /* Auto-deregister notification for this descriptor's pool entry */
    if (__jacl_mq_notify[desc->pool_idx].notify_pid == getpid()) {
        __jacl_mq_notify[desc->pool_idx].registered = 0;
        __jacl_mq_notify[desc->pool_idx].notify_pid = 0;
    }

    desc->active = 0;
    mq->refcount--;
    if (mq->refcount == 0 && mq->unlinked) __jacl_mq_destroy(mq);
    pthread_mutex_unlock(&__jacl_mq_global);
    return 0;
}

static inline int mq_unlink(const char *name) {
    if (!__jacl_mq_name_valid(name)) { errno = EINVAL; return -1; }
    pthread_mutex_lock(&__jacl_mq_global);
    __jacl_mq_t *mq = __jacl_mq_find(name);
    if (!mq) { pthread_mutex_unlock(&__jacl_mq_global); errno = ENOENT; return -1; }

    if (mq->unlinked) {
        pthread_mutex_unlock(&__jacl_mq_global); errno = ENOENT; return -1;
    }

    mq->unlinked = 1;
    if (mq->refcount == 0) __jacl_mq_destroy(mq);
    pthread_mutex_unlock(&__jacl_mq_global);
    return 0;
}

static inline int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio) {
    if (mqdes < 0 || mqdes >= __JACL_MQ_DESC_MAX) { errno = EBADF; return -1; }
    __jacl_mq_desc_t *desc = &__jacl_mq_descs[mqdes];
    if (!desc->active) { errno = EBADF; return -1; }
    __jacl_mq_t *mq = &__jacl_mq_pool[desc->pool_idx];
    if (!mq->active) { errno = EBADF; return -1; }
    if (!msg_ptr) { errno = EINVAL; return -1; }
    if (msg_len > (size_t)mq->attr.mq_msgsize) { errno = EMSGSIZE; return -1; }
    if (desc->oflags == O_RDONLY) { errno = EBADF; return -1; }

    int pool_idx = desc->pool_idx;

    pthread_mutex_lock(&mq->lock);
    while (mq->attr.mq_curmsgs >= mq->attr.mq_maxmsg) {
        if (mq->attr.mq_flags & O_NONBLOCK) { pthread_mutex_unlock(&mq->lock); errno = EAGAIN; return -1; }
        pthread_cond_wait(&mq->cond, &mq->lock);
    }

    __jacl_mq_node_t *node = malloc(sizeof(*node));
    node->data = malloc(msg_len);
    memcpy(node->data, msg_ptr, msg_len);
    node->len = msg_len;
    node->prio = msg_prio;
    node->next = NULL;

    __jacl_mq_node_t **pp = &mq->head;
    while (*pp && (*pp)->prio >= msg_prio) pp = &(*pp)->next;
    node->next = *pp;
    *pp = node;

    mq->attr.mq_curmsgs++;
    int was_empty = (mq->attr.mq_curmsgs == 1);
    pthread_cond_broadcast(&mq->cond);
    pthread_mutex_unlock(&mq->lock);

    if (was_empty) __jacl_mq_do_notify(pool_idx);
    return 0;
}

static inline ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio) {
    if (mqdes < 0 || mqdes >= __JACL_MQ_DESC_MAX) { errno = EBADF; return -1; }
    __jacl_mq_desc_t *desc = &__jacl_mq_descs[mqdes];
    if (!desc->active) { errno = EBADF; return -1; }
    __jacl_mq_t *mq = &__jacl_mq_pool[desc->pool_idx];
    if (!mq->active) { errno = EBADF; return -1; }
    if (!msg_ptr) { errno = EINVAL; return -1; }
    if (desc->oflags == O_WRONLY) { errno = EBADF; return -1; }

    pthread_mutex_lock(&mq->lock);

    /* Auto-deregister notification on receive per POSIX */
    __jacl_mq_notify[desc->pool_idx].registered = 0;
    __jacl_mq_notify[desc->pool_idx].notify_pid = 0;

    while (!mq->head) {
        if (mq->attr.mq_flags & O_NONBLOCK) { pthread_mutex_unlock(&mq->lock); errno = EAGAIN; return -1; }
        pthread_cond_wait(&mq->cond, &mq->lock);
    }

    __jacl_mq_node_t *node = mq->head;
    if (msg_len < node->len) { pthread_mutex_unlock(&mq->lock); errno = EMSGSIZE; return -1; }

    memcpy(msg_ptr, node->data, node->len);
    if (msg_prio) *msg_prio = node->prio;
    mq->head = node->next;
    mq->attr.mq_curmsgs--;
    ssize_t len = node->len;
    free(node->data);
    free(node);
    pthread_cond_broadcast(&mq->cond);
    pthread_mutex_unlock(&mq->lock);
    return len;
}

static inline int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio, const struct timespec *abs_timeout) {
    if (mqdes < 0 || mqdes >= __JACL_MQ_DESC_MAX) { errno = EBADF; return -1; }
    __jacl_mq_desc_t *desc = &__jacl_mq_descs[mqdes];
    if (!desc->active) { errno = EBADF; return -1; }
    __jacl_mq_t *mq = &__jacl_mq_pool[desc->pool_idx];
    if (!mq->active || !abs_timeout) { errno = mq->active ? EINVAL : EBADF; return -1; }
    if (abs_timeout->tv_nsec < 0 || abs_timeout->tv_nsec >= 1000000000L) { errno = EINVAL; return -1; }
    if (!msg_ptr) { errno = EINVAL; return -1; }
    if (msg_len > (size_t)mq->attr.mq_msgsize) { errno = EMSGSIZE; return -1; }
    if (desc->oflags == O_RDONLY) { errno = EBADF; return -1; }

    int pool_idx = desc->pool_idx;

    pthread_mutex_lock(&mq->lock);
    while (mq->attr.mq_curmsgs >= mq->attr.mq_maxmsg) {
        int rc = pthread_cond_timedwait(&mq->cond, &mq->lock, abs_timeout);
        if (rc == ETIMEDOUT) { pthread_mutex_unlock(&mq->lock); errno = ETIMEDOUT; return -1; }
    }

    __jacl_mq_node_t *node = malloc(sizeof(*node));
    node->data = malloc(msg_len);
    memcpy(node->data, msg_ptr, msg_len);
    node->len = msg_len;
    node->prio = msg_prio;
    node->next = NULL;

    __jacl_mq_node_t **pp = &mq->head;
    while (*pp && (*pp)->prio >= msg_prio) pp = &(*pp)->next;
    node->next = *pp;
    *pp = node;

    mq->attr.mq_curmsgs++;
    int was_empty = (mq->attr.mq_curmsgs == 1);
    pthread_cond_broadcast(&mq->cond);
    pthread_mutex_unlock(&mq->lock);

    if (was_empty) __jacl_mq_do_notify(pool_idx);
    return 0;
}

static inline ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio, const struct timespec *abs_timeout) {
    if (mqdes < 0 || mqdes >= __JACL_MQ_DESC_MAX) { errno = EBADF; return -1; }
    __jacl_mq_desc_t *desc = &__jacl_mq_descs[mqdes];
    if (!desc->active) { errno = EBADF; return -1; }
    __jacl_mq_t *mq = &__jacl_mq_pool[desc->pool_idx];
    if (!mq->active || !abs_timeout) { errno = mq->active ? EINVAL : EBADF; return -1; }
    if (abs_timeout->tv_nsec < 0 || abs_timeout->tv_nsec >= 1000000000L) { errno = EINVAL; return -1; }
    if (!msg_ptr) { errno = EINVAL; return -1; }
    if (desc->oflags == O_WRONLY) { errno = EBADF; return -1; }

    pthread_mutex_lock(&mq->lock);

    /* Auto-deregister notification on receive per POSIX */
    __jacl_mq_notify[desc->pool_idx].registered = 0;
    __jacl_mq_notify[desc->pool_idx].notify_pid = 0;

    while (!mq->head) {
        int rc = pthread_cond_timedwait(&mq->cond, &mq->lock, abs_timeout);
        if (rc == ETIMEDOUT) { pthread_mutex_unlock(&mq->lock); errno = ETIMEDOUT; return -1; }
    }

    __jacl_mq_node_t *node = mq->head;
    if (msg_len < node->len) { pthread_mutex_unlock(&mq->lock); errno = EMSGSIZE; return -1; }

    memcpy(msg_ptr, node->data, node->len);
    if (msg_prio) *msg_prio = node->prio;
    mq->head = node->next;
    mq->attr.mq_curmsgs--;
    ssize_t len = node->len;
    free(node->data);
    free(node);
    pthread_cond_broadcast(&mq->cond);
    pthread_mutex_unlock(&mq->lock);
    return len;
}

static inline int mq_notify(mqd_t mqdes, const struct sigevent *notification) {
    if (mqdes < 0 || mqdes >= __JACL_MQ_DESC_MAX) { errno = EBADF; return -1; }
    __jacl_mq_desc_t *desc = &__jacl_mq_descs[mqdes];
    if (!desc->active) { errno = EBADF; return -1; }
    __jacl_mq_t *mq = &__jacl_mq_pool[desc->pool_idx];
    if (!mq->active) { errno = EBADF; return -1; }

    pthread_mutex_lock(&__jacl_mq_global);
    __jacl_mq_notify_t *n = &__jacl_mq_notify[desc->pool_idx];

    if (notification == NULL) {
        /* Deregister: only the registering process may deregister */
        if (n->registered && n->notify_pid == getpid()) {
            n->registered = 0;
            memset(&n->sev, 0, sizeof(n->sev));
            n->notify_pid = 0;
        }
        pthread_mutex_unlock(&__jacl_mq_global);
        return 0;
    }

    /* Register */
    if (n->registered) {
        pthread_mutex_unlock(&__jacl_mq_global);
        errno = EBUSY;
        return -1;
    }

    n->registered = 1;
    n->notify_pid = getpid();
    memcpy(&n->sev, notification, sizeof(struct sigevent));
    pthread_mutex_unlock(&__jacl_mq_global);
    return 0;
}

static inline int mq_getattr(mqd_t mqdes, struct mq_attr *mqstat) {
    if (mqdes < 0 || mqdes >= __JACL_MQ_DESC_MAX || !mqstat) { errno = mqstat ? EBADF : EINVAL; return -1; }
    __jacl_mq_desc_t *desc = &__jacl_mq_descs[mqdes];
    if (!desc->active) { errno = EBADF; return -1; }
    __jacl_mq_t *mq = &__jacl_mq_pool[desc->pool_idx];
    if (!mq->active) { errno = EBADF; return -1; }

    pthread_mutex_lock(&mq->lock);
    *mqstat = mq->attr;
    pthread_mutex_unlock(&mq->lock);
    return 0;
}

static inline int mq_setattr(mqd_t mqdes, const struct mq_attr *mqstat, struct mq_attr *omqstat) {
    if (mqdes < 0 || mqdes >= __JACL_MQ_DESC_MAX || !mqstat) { errno = mqstat ? EBADF : EINVAL; return -1; }
    __jacl_mq_desc_t *desc = &__jacl_mq_descs[mqdes];
    if (!desc->active) { errno = EBADF; return -1; }
    __jacl_mq_t *mq = &__jacl_mq_pool[desc->pool_idx];
    if (!mq->active) { errno = EBADF; return -1; }

    pthread_mutex_lock(&mq->lock);
    if (omqstat) *omqstat = mq->attr;
    mq->attr.mq_flags = (mq->attr.mq_flags & ~O_NONBLOCK) | (mqstat->mq_flags & O_NONBLOCK);
    pthread_mutex_unlock(&mq->lock);
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* _MQUEUE_H */
