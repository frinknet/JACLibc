/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _SYS_MSG_H
#define _SYS_MSG_H
#pragma once

#include <config.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Constants */
#ifndef MSG_NOERROR
#define MSG_NOERROR     010000
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

/* Types & Structures */
#ifndef __msgqnum_t_defined
typedef unsigned long msgqnum_t;
#define __msgqnum_t_defined
#endif

#ifndef __msglen_t_defined
typedef unsigned long msglen_t;
#define __msglen_t_defined
#endif

struct msqid_ds {
    struct ipc_perm msg_perm;
    msgqnum_t       msg_qnum;
    msglen_t        msg_qbytes;
    pid_t           msg_lspid;
    pid_t           msg_lrpid;
    time_t          msg_stime;
    time_t          msg_rtime;
    time_t          msg_ctime;
};

/* Internal State */
#define JACL_MSQ_MAX_QUEUES 64
#define JACL_MSQ_DEFAULT_QBYTES 16384

struct __jacl_msg_node {
    long mtype;
    size_t len;
    char *data;
    struct __jacl_msg_node *next;
};

typedef struct {
    int id;
    key_t key;
    int active;
    int deleted;
    struct msqid_ds meta;
    struct __jacl_msg_node *head;
    struct __jacl_msg_node *tail;
    size_t current_bytes;
} __jacl_msq;

static __jacl_msq __jacl_msq_registry[JACL_MSQ_MAX_QUEUES] = {0};
static int __jacl_msq_next_id = 1;

/* Helpers */
static inline __jacl_msq *__jacl_msq_find_by_id(int msqid) {
    for (int i = 0; i < JACL_MSQ_MAX_QUEUES; i++) {
        if (__jacl_msq_registry[i].active && __jacl_msq_registry[i].id == msqid)
            return &__jacl_msq_registry[i];
    }
    return NULL;
}

static inline __jacl_msq *__jacl_msq_find_by_key(key_t key) {
    for (int i = 0; i < JACL_MSQ_MAX_QUEUES; i++) {
        if (__jacl_msq_registry[i].active && __jacl_msq_registry[i].key == key)
            return &__jacl_msq_registry[i];
    }
    return NULL;
}

static inline void __jacl_msq_destroy(__jacl_msq *q) {
    struct __jacl_msg_node *curr = q->head;
    while (curr) {
        struct __jacl_msg_node *next = curr->next;
        free(curr->data);
        free(curr);
        curr = next;
    }
    q->head = NULL;
    q->tail = NULL;
    q->current_bytes = 0;
    q->meta.msg_qnum = 0;
    q->active = 0;
}

/* Public API */
static inline int msgget(key_t key, int msgflg) {
	if (key != IPC_PRIVATE) {
		__jacl_msq *existing = __jacl_msq_find_by_key(key);
		if (existing) {
			if ((msgflg & IPC_CREAT) && (msgflg & IPC_EXCL)) return (__errno_set(EEXIST), -1);
			return existing->id;
		}
	}

	if (!(msgflg & IPC_CREAT)) return (__errno_set(ENOENT), -1);

	int slot = -1;
	for (int i = 0; i < JACL_MSQ_MAX_QUEUES; i++) {
		if (!__jacl_msq_registry[i].active) { slot = i; break; }
	}
	if (slot == -1) return (__errno_set(ENOSPC), -1);

	__jacl_msq *q = &__jacl_msq_registry[slot];
	q->id = __jacl_msq_next_id++;
	q->key = key;
	q->active = 1;
	q->deleted = 0;
	q->head = NULL;
	q->tail = NULL;
	q->current_bytes = 0;

	memset(&q->meta, 0, sizeof(q->meta));
	q->meta.msg_perm.uid = getuid();
	q->meta.msg_perm.gid = getgid();
	q->meta.msg_perm.cuid = getuid();
	q->meta.msg_perm.cgid = getgid();
	q->meta.msg_perm.mode = msgflg & 0777;
	q->meta.msg_qbytes = JACL_MSQ_DEFAULT_QBYTES;
	q->meta.msg_ctime = time(NULL);

	return q->id;
}

static inline int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg) {
    if (!msgp) return (__errno_set(EINVAL), -1);

    long mtype = *(const long *)msgp;
    if (mtype <= 0) return (__errno_set(EINVAL), -1);

    __jacl_msq *q = __jacl_msq_find_by_id(msqid);
    if (!q) return (__errno_set(EINVAL), -1);
    if (q->deleted) return (__errno_set(EIDRM), -1);

    if (q->current_bytes + msgsz > q->meta.msg_qbytes) {
        return (__errno_set(EAGAIN), -1);
    }

    struct __jacl_msg_node *node = malloc(sizeof(*node));
    if (!node) return (__errno_set(ENOMEM), -1);

    node->data = malloc(msgsz > 0 ? msgsz : 1);
    if (!node->data) { free(node); return (__errno_set(ENOMEM), -1); }

    node->mtype = mtype;
    node->len = msgsz;
    if (msgsz > 0) memcpy(node->data, (const char *)msgp + sizeof(long), msgsz);
    node->next = NULL;

    if (q->tail) q->tail->next = node;
    else q->head = node;
    q->tail = node;

    q->current_bytes += msgsz;
    q->meta.msg_qnum++;
    q->meta.msg_lspid = getpid();
    q->meta.msg_stime = time(NULL);

    return 0;
}

static inline ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg) {
    if (!msgp) return (__errno_set(EINVAL), -1);

    __jacl_msq *q = __jacl_msq_find_by_id(msqid);
    if (!q) return (__errno_set(EINVAL), -1);
    if (q->deleted) return (__errno_set(EIDRM), -1);

    struct __jacl_msg_node *prev = NULL, *curr = q->head;
    struct __jacl_msg_node *found = NULL, *found_prev = NULL;

    while (curr) {
        int match = 0;
        if (msgtyp == 0) match = 1;
        else if (msgtyp > 0 && curr->mtype == msgtyp) match = 1;
        else if (msgtyp < 0 && curr->mtype <= -msgtyp) {
            if (!found || curr->mtype < found->mtype) {
                found = curr;
                found_prev = prev;
            }
        }

        if (match && msgtyp >= 0) {
            found = curr;
            found_prev = prev;
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    if (!found) {
        return (__errno_set(ENOMSG), -1);
    }

    if (found->len > msgsz) {
        if (!(msgflg & MSG_NOERROR)) return (__errno_set(E2BIG), -1);
    }

    size_t copy_len = (found->len > msgsz && (msgflg & MSG_NOERROR)) ? msgsz : found->len;

    *(long *)msgp = found->mtype;
    if (copy_len > 0) memcpy((char *)msgp + sizeof(long), found->data, copy_len);

    if (found_prev) found_prev->next = found->next;
    else q->head = found->next;
    if (found == q->tail) q->tail = found_prev;

    q->current_bytes -= found->len;
    q->meta.msg_qnum--;
    q->meta.msg_lrpid = getpid();
    q->meta.msg_rtime = time(NULL);

    free(found->data);
    free(found);

    return (ssize_t)copy_len;
}

static inline int msgctl(int msqid, int cmd, struct msqid_ds *buf) {
    if (cmd == IPC_INFO) {
        if (!buf) return (__errno_set(EINVAL), -1);
        memset(buf, 0, sizeof(*buf));
        buf->msg_qbytes = JACL_MSQ_MAX_QUEUES;
        return 0;
    }

    __jacl_msq *q = __jacl_msq_find_by_id(msqid);
    if (!q) return (__errno_set(EINVAL), -1);

    switch (cmd) {
        case IPC_STAT:
            if (!buf) return (__errno_set(EINVAL), -1);
            memcpy(buf, &q->meta, sizeof(*buf));
            return 0;
        case IPC_SET:
            if (!buf) return (__errno_set(EINVAL), -1);
            q->meta.msg_perm.uid = buf->msg_perm.uid;
            q->meta.msg_perm.gid = buf->msg_perm.gid;
            q->meta.msg_perm.mode = buf->msg_perm.mode & 0777;
            q->meta.msg_qbytes = buf->msg_qbytes;
            q->meta.msg_ctime = time(NULL);
            return 0;
        case IPC_RMID:
            __jacl_msq_destroy(q);
            return 0;
        default:
            return (__errno_set(EINVAL), -1);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* _SYS_MSG_H */
