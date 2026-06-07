/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#ifndef _SYS_SEM_H
#define _SYS_SEM_H
#pragma once

/**
 * NOTE: This consolidates functionality traditionally found in:
 *
 *   - <semaphore.h>
 *   - <sys/sem.h>
 *
 * We unify these into a single header at <sys/sem.h> for ease of maintenance
 * and clarity with hopes that one day C Standards or POSIX may revamp layout
 * for sanity's sake so that we can remove the abstraction of legacy cruft that
 * cause so many security bugs because the coders don't see how things work.
 *
 * There has to be a better way!!!
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <config.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdatomic.h>
#include <limits.h>

#if JACL_OS_WINDOWS
	#define SEM_WIN32 1
	#include <windows.h>
#elif JACL_ARCH_WASM
	#define SEM_DUMMY 1
#else
	#define SEM_POSIX 1
#endif

/* ============================================================================ */
/* Internal Core Primitive                                                      */
/* ============================================================================ */

typedef struct {
	unsigned int value;
	pid_t		last_pid;
	unsigned int ncnt;
	unsigned int zcnt;
	int		  pshared;
	int		  anonymous;
	clockid_t	clock_id;
	pthread_mutex_t lock;
	pthread_cond_t  cond;
} __jacl_sem_primitive_t;

typedef struct {
	int active;
	__jacl_sem_primitive_t p;
} __jacl_sem_slot_t;

#define __JACL_SEM_SLOT_MAX 1024
static __jacl_sem_slot_t __jacl_sem_slots[__JACL_SEM_SLOT_MAX];
static pthread_mutex_t __jacl_sem_slot_lock = PTHREAD_MUTEX_INITIALIZER;

/* Initialize the mutex + condvar inside a primitive */
static inline void __jacl_sem_primitive_init(__jacl_sem_primitive_t *p, unsigned int val,
											 int pshared, int anonymous, clockid_t cid) {
	p->value = val;
	p->last_pid = 0;
	p->ncnt = 0;
	p->zcnt = 0;
	p->pshared = pshared;
	p->anonymous = anonymous;
	p->clock_id = cid;

	pthread_mutexattr_t mattr;
	pthread_mutexattr_init(&mattr);
	if (pshared) pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&p->lock, &mattr);
	pthread_mutexattr_destroy(&mattr);

	pthread_condattr_t cattr;
	pthread_condattr_init(&cattr);
	if (pshared) pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);
	pthread_condattr_setclock(&cattr, cid);
	pthread_cond_init(&p->cond, &cattr);
	pthread_condattr_destroy(&cattr);
}

/* Destroy the mutex + condvar inside a primitive */
static inline void __jacl_sem_primitive_destroy(__jacl_sem_primitive_t *p) {
	pthread_mutex_destroy(&p->lock);
	pthread_cond_destroy(&p->cond);
}

static inline int __jacl_sem_slot_alloc(unsigned int val, int pshared, int anonymous, clockid_t cid) {
	pthread_mutex_lock(&__jacl_sem_slot_lock);
	for (int i = 0; i < __JACL_SEM_SLOT_MAX; i++) {
		if (!__jacl_sem_slots[i].active) {
			__jacl_sem_slots[i].active = 1;
			__jacl_sem_primitive_init(&__jacl_sem_slots[i].p, val, pshared, anonymous, cid);
			pthread_mutex_unlock(&__jacl_sem_slot_lock);
			return i;
		}
	}
	pthread_mutex_unlock(&__jacl_sem_slot_lock);
	return -1;
}

static inline void __jacl_sem_slot_free(int idx) {
	if (idx < 0 || idx >= __JACL_SEM_SLOT_MAX) return;
	pthread_mutex_lock(&__jacl_sem_slot_lock);
	if (__jacl_sem_slots[idx].active) {
		__jacl_sem_primitive_destroy(&__jacl_sem_slots[idx].p);
		__jacl_sem_slots[idx].active = 0;
	}
	pthread_mutex_unlock(&__jacl_sem_slot_lock);
}

/* ============================================================================ */
/* Internal Named Semaphore Registry                                            */
/* ============================================================================ */

#define __JACL_SEM_NAMED_MAX 64

typedef struct {
	int active;
	char name[256];
	int unlinked;
	int refcount;
	int slot_idx;
} __jacl_sem_named_t;

static __jacl_sem_named_t __jacl_sem_named[__JACL_SEM_NAMED_MAX];
static pthread_mutex_t __jacl_sem_named_lock = PTHREAD_MUTEX_INITIALIZER;

/* Look up a named entry by name. Caller must hold __jacl_sem_named_lock. */
static inline int __jacl_sem_named_find_by_name(const char *name) {
	for (int i = 0; i < __JACL_SEM_NAMED_MAX; i++) {
		if (__jacl_sem_named[i].active && !__jacl_sem_named[i].unlinked && strcmp(__jacl_sem_named[i].name, name) == 0) return i;
	}

	return -1;
}

/* Look up a named entry by slot_idx. Caller must hold __jacl_sem_named_lock. */
static inline int __jacl_sem_named_find_by_slot(int slot_idx) {
	for (int i = 0; i < __JACL_SEM_NAMED_MAX; i++) {
		if (__jacl_sem_named[i].active && __jacl_sem_named[i].slot_idx == slot_idx)
			return i;
	}
	return -1;
}

/* Find first free named entry. Caller must hold __jacl_sem_named_lock. */
static inline int __jacl_sem_named_find_free(void) {
	for (int i = 0; i < __JACL_SEM_NAMED_MAX; i++) {
		if (!__jacl_sem_named[i].active) return i;
	}
	return -1;
}

/* ============================================================================ */
/* Internal Undo Log (System V SEM_UNDO)                                        */
/* ============================================================================ */

#define __JACL_SEM_UNDO_MAX 4096

typedef struct {
	pid_t pid;
	int slot_idx;
	int adjust;
} __jacl_sem_undo_entry_t;

static __jacl_sem_undo_entry_t __jacl_sem_undo_entries[__JACL_SEM_UNDO_MAX];
static pthread_mutex_t __jacl_sem_undo_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_once_t __jacl_sem_undo_once = PTHREAD_ONCE_INIT;

static inline void __jacl_sem_undo_cleanup(void) {
	pid_t me = getpid();
	pthread_mutex_lock(&__jacl_sem_undo_lock);
	for (int i = 0; i < __JACL_SEM_UNDO_MAX; i++) {
		if (__jacl_sem_undo_entries[i].pid == me && __jacl_sem_undo_entries[i].slot_idx >= 0) {
			int idx = __jacl_sem_undo_entries[i].slot_idx;
			if (__jacl_sem_slots[idx].active) {
				__jacl_sem_primitive_t *p = &__jacl_sem_slots[idx].p;
				pthread_mutex_lock(&p->lock);
				p->value += (unsigned int)(-__jacl_sem_undo_entries[i].adjust);
				pthread_cond_broadcast(&p->cond);
				pthread_mutex_unlock(&p->lock);
			}
			__jacl_sem_undo_entries[i].pid = 0;
			__jacl_sem_undo_entries[i].slot_idx = -1;
			__jacl_sem_undo_entries[i].adjust = 0;
		}
	}
	pthread_mutex_unlock(&__jacl_sem_undo_lock);
}

static inline void __jacl_sem_undo_init(void) {
	for (int i = 0; i < __JACL_SEM_UNDO_MAX; i++) {
		__jacl_sem_undo_entries[i].pid = 0;
		__jacl_sem_undo_entries[i].slot_idx = -1;
		__jacl_sem_undo_entries[i].adjust = 0;
	}
	atexit(__jacl_sem_undo_cleanup);
}

static inline void __jacl_sem_undo_add(int slot_idx, int op) {
	if (op == 0) return;
	pid_t me = getpid();
	pthread_mutex_lock(&__jacl_sem_undo_lock);
	int found = -1;
	for (int i = 0; i < __JACL_SEM_UNDO_MAX; i++) {
		if (__jacl_sem_undo_entries[i].pid == me && __jacl_sem_undo_entries[i].slot_idx == slot_idx) {
			found = i;
			break;
		}
		if (found == -1 && __jacl_sem_undo_entries[i].pid == 0) found = i;
	}
	if (found >= 0) {
		__jacl_sem_undo_entries[found].pid = me;
		__jacl_sem_undo_entries[found].slot_idx = slot_idx;
		__jacl_sem_undo_entries[found].adjust += op;
	}
	pthread_mutex_unlock(&__jacl_sem_undo_lock);
}

/* ============================================================================ */
/* POSIX Public ABI (<semaphore.h>)                                             */
/* ============================================================================ */

typedef int sem_t;
#define SEM_FAILED ((sem_t)-1)

static inline __jacl_sem_primitive_t *__jacl_sem_resolve(sem_t *sem) {
	if (!sem || *sem < 0 || *sem >= __JACL_SEM_SLOT_MAX) return NULL;
	if (!__jacl_sem_slots[*sem].active) return NULL;
	return &__jacl_sem_slots[*sem].p;
}

/* Validate sem_open name per POSIX: must be "/<at-least-one-char>" */
static inline int __jacl_sem_name_valid(const char *name) {
	return name && name[0] == '/' && name[1] != '\0';
}

/* Parse the va_args for O_CREAT (mode + value). */
static inline unsigned int __jacl_sem_parse_create_args(int oflag, va_list *ap) {
	if (!(oflag & O_CREAT)) return 0;
	(void)va_arg(*ap, int);			  /* mode - ignored */
	return (unsigned int)va_arg(*ap, unsigned int);
}

#if SEM_POSIX

static inline int sem_init(sem_t *sem, int pshared, unsigned int value) {
	if (!sem) return (__errno_set(EINVAL), -1);

	int idx = __jacl_sem_slot_alloc(value, pshared, 1, CLOCK_REALTIME);

	if (idx < 0) return (__errno_set(ENOSPC), -1);

	*sem = (sem_t)idx;

	return 0;
}

static inline int sem_destroy(sem_t *sem) {
	__jacl_sem_primitive_t *p = __jacl_sem_resolve(sem);
	if (!p) return (__errno_set(EINVAL), -1);
	if (!p->anonymous) return (__errno_set(EINVAL), -1);

	__jacl_sem_slot_free(*sem);

	return 0;
}

static inline sem_t sem_open(const char *name, int oflag, ...) {
	if (!__jacl_sem_name_valid(name)) return (__errno_set(EINVAL), SEM_FAILED);
	if ((oflag & O_EXCL) && !(oflag & O_CREAT)) return (__errno_set(EINVAL), SEM_FAILED);

	va_list ap;
	va_start(ap, oflag);
	unsigned int value = __jacl_sem_parse_create_args(oflag, &ap);
	va_end(ap);

	pthread_mutex_lock(&__jacl_sem_named_lock);

	int found = __jacl_sem_named_find_by_name(name);
	if (found >= 0) {
		if ((oflag & O_CREAT) && (oflag & O_EXCL)) {
			pthread_mutex_unlock(&__jacl_sem_named_lock);

			return (__errno_set(EEXIST), SEM_FAILED);
		}
		__jacl_sem_named[found].refcount++;
		sem_t result = (sem_t)__jacl_sem_named[found].slot_idx;
		pthread_mutex_unlock(&__jacl_sem_named_lock);
		return result;
	}

	if (!(oflag & O_CREAT)) {
		pthread_mutex_unlock(&__jacl_sem_named_lock);

		return (__errno_set(ENOENT), SEM_FAILED);
	}

	int free_name = __jacl_sem_named_find_free();
	if (free_name < 0) {
		pthread_mutex_unlock(&__jacl_sem_named_lock);

		return (__errno_set(ENFILE), SEM_FAILED);
	}

	int prim_idx = __jacl_sem_slot_alloc(value, 0, 0, CLOCK_REALTIME);
	if (prim_idx < 0) {
		pthread_mutex_unlock(&__jacl_sem_named_lock);

		return (__errno_set(ENOSPC), SEM_FAILED);
	}

	__jacl_sem_named[free_name].active = 1;
	strncpy(__jacl_sem_named[free_name].name, name, 255);
	__jacl_sem_named[free_name].name[255] = '\0';
	__jacl_sem_named[free_name].unlinked = 0;
	__jacl_sem_named[free_name].refcount = 1;
	__jacl_sem_named[free_name].slot_idx = prim_idx;

	pthread_mutex_unlock(&__jacl_sem_named_lock);
	return (sem_t)prim_idx;
}

static inline int sem_close(sem_t sem) {
	if (sem < 0 || sem >= __JACL_SEM_SLOT_MAX) return (__errno_set(EINVAL), -1);

	pthread_mutex_lock(&__jacl_sem_named_lock);
	int found = __jacl_sem_named_find_by_slot((int)sem);

	if (found < 0) {
		pthread_mutex_unlock(&__jacl_sem_named_lock);

		return (__errno_set(EINVAL), -1);
	}

	if (__jacl_sem_named[found].refcount <= 0) {
		pthread_mutex_unlock(&__jacl_sem_named_lock);

		return (__errno_set(EINVAL), -1);
	}

	__jacl_sem_named[found].refcount--;
	int should_destroy = (__jacl_sem_named[found].refcount == 0 && __jacl_sem_named[found].unlinked);
	int prim_idx = __jacl_sem_named[found].slot_idx;
	if (should_destroy) __jacl_sem_named[found].active = 0;

	pthread_mutex_unlock(&__jacl_sem_named_lock);

	if (should_destroy) __jacl_sem_slot_free(prim_idx);
	return 0;
}

static inline int sem_unlink(const char *name) {
	if (!name) return (__errno_set(EINVAL), -1);

	pthread_mutex_lock(&__jacl_sem_named_lock);
	int found = __jacl_sem_named_find_by_name(name);
	if (found < 0) {
		pthread_mutex_unlock(&__jacl_sem_named_lock);
		return (__errno_set(ENOENT), -1);
	}

	if (__jacl_sem_named[found].unlinked) {
		pthread_mutex_unlock(&__jacl_sem_named_lock);
		return (__errno_set(ENOENT), -1);
	}

	__jacl_sem_named[found].unlinked = 1;
	int should_destroy = (__jacl_sem_named[found].refcount == 0);
	int prim_idx = __jacl_sem_named[found].slot_idx;
	if (should_destroy) __jacl_sem_named[found].active = 0;

	pthread_mutex_unlock(&__jacl_sem_named_lock);

	if (should_destroy) __jacl_sem_slot_free(prim_idx);
	return 0;
}

/* Shared wait core: pass timeout==NULL for unbounded wait, or absolute timespec */
static inline int __jacl_sem_wait_core(sem_t *sem, const struct timespec *abs_timeout) {
	__jacl_sem_primitive_t *p = __jacl_sem_resolve(sem);

	if (!p) return (__errno_set(EINVAL), -1);

	pthread_mutex_lock(&p->lock);
	p->last_pid = getpid();
	while (p->value == 0) {
		p->ncnt++;
		int rc;
		if (abs_timeout) {
			rc = pthread_cond_timedwait(&p->cond, &p->lock, abs_timeout);
		} else {
			pthread_cond_wait(&p->cond, &p->lock);

			rc = 0;
		}
		p->ncnt--;
		if (rc == __errno_val(ETIMEDOUT)) {
			pthread_mutex_unlock(&p->lock);

			return (__errno_set(ETIMEDOUT), -1);
		}
	}

	p->value--;

	pthread_mutex_unlock(&p->lock);

	return 0;
}

static inline int sem_wait(sem_t *sem) {
	return __jacl_sem_wait_core(sem, NULL);
}

static inline int sem_trywait(sem_t *sem) {
	__jacl_sem_primitive_t *p = __jacl_sem_resolve(sem);

	if (!p) return (__errno_set(EINVAL), -1);

	pthread_mutex_lock(&p->lock);

	p->last_pid = getpid();

	if (p->value == 0) {
		pthread_mutex_unlock(&p->lock);

		return (__errno_set(EAGAIN), -1);
	}

	p->value--;

	pthread_mutex_unlock(&p->lock);

	return 0;
}

static inline int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout) {
	if (!abs_timeout) return (__errno_set(EINVAL), -1);
	if (abs_timeout->tv_nsec < 0 || abs_timeout->tv_nsec >= 1000000000L) return (__errno_set(EINVAL), -1);

	return __jacl_sem_wait_core(sem, abs_timeout);
}

static inline int sem_clockwait(sem_t *sem, clockid_t clock_id, const struct timespec *abs_timeout) {
	(void)clock_id;
	return sem_timedwait(sem, abs_timeout);
}

static inline int sem_post(sem_t *sem) {
	__jacl_sem_primitive_t *p = __jacl_sem_resolve(sem);
	if (!p) return (__errno_set(EINVAL), -1);

	pthread_mutex_lock(&p->lock);
	p->last_pid = getpid();
	p->value++;
	pthread_cond_broadcast(&p->cond);
	pthread_mutex_unlock(&p->lock);
	return 0;
}

static inline int sem_getvalue(sem_t *sem, int *sval) {
	__jacl_sem_primitive_t *p = __jacl_sem_resolve(sem);
	if (!p || !sval) return (__errno_set(EINVAL), -1);

	pthread_mutex_lock(&p->lock);
	*sval = (int)p->value;
	pthread_mutex_unlock(&p->lock);
	return 0;
}

#else /* !SEM_POSIX (Windows/WASM stubs) */

static inline int sem_init(sem_t *sem, int pshared, unsigned int value) {
	(void)sem; (void)pshared; (void)value;
	return (__errno_set(ENOSYS), -1);
}
static inline int sem_destroy(sem_t *sem) {
	(void)sem; return (__errno_set(ENOSYS), -1);
}
static inline sem_t sem_open(const char *name, int oflag, ...) {
	(void)name; (void)oflag; return (__errno_set(ENOSYS), SEM_FAILED);
}
static inline int sem_close(sem_t sem) {
	(void)sem; return (__errno_set(ENOSYS), -1);
}
static inline int sem_unlink(const char *name) {
	(void)name; return (__errno_set(ENOSYS), -1);
}
static inline int sem_wait(sem_t *sem) {
	(void)sem; return (__errno_set(ENOSYS), -1);
}
static inline int sem_trywait(sem_t *sem) {
	(void)sem; return (__errno_set(ENOSYS), -1);
}
static inline int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout) {
	(void)sem; (void)abs_timeout; return (__errno_set(ENOSYS), -1);
}
static inline int sem_clockwait(sem_t *sem, clockid_t clock_id, const struct timespec *abs_timeout) {
	(void)sem; (void)clock_id; (void)abs_timeout; return (__errno_set(ENOSYS), -1);
}
static inline int sem_post(sem_t *sem) {
	(void)sem; return (__errno_set(ENOSYS), -1);
}
static inline int sem_getvalue(sem_t *sem, int *sval) {
	(void)sem; (void)sval; return (__errno_set(ENOSYS), -1);
}

#endif

/* ============================================================================ */
/* System V Public ABI (<sys/sem.h>)                                            */
/* ============================================================================ */

#ifndef IPC_CREAT
#define IPC_CREAT  01000
#define IPC_EXCL   02000
#define IPC_NOWAIT 04000
#define IPC_RMID   0
#define IPC_SET	1
#define IPC_STAT   2
#define IPC_PRIVATE ((key_t)0)
#endif

#ifndef SEM_UNDO
#define SEM_UNDO 0x1000
#endif

#define GETVAL   12
#define SETVAL   16
#define GETALL   13
#define SETALL   17
#define GETPID   11
#define GETNCNT  14
#define GETZCNT  15

#ifndef _IPC_PERM_DEFINED
#define _IPC_PERM_DEFINED
struct ipc_perm {
	uid_t  uid;
	gid_t  gid;
	uid_t  cuid;
	gid_t  cgid;
	mode_t mode;
};
#endif

struct semid_ds {
	struct ipc_perm sem_perm;
	unsigned short  sem_nsems;
	time_t		  sem_otime;
	time_t		  sem_ctime;
};

struct sembuf {
	unsigned short sem_num;
	short		  sem_op;
	short		  sem_flg;
};

union semun {
	int			  val;
	struct semid_ds *buf;
	unsigned short  *array;
};

#define __JACL_SYSV_SEMSET_MAX 64
#define __JACL_SYSV_MAX_SEMS   256

typedef struct {
	int active;
	key_t key;
	struct semid_ds ds;
	int slot_indices[__JACL_SYSV_MAX_SEMS];
} __jacl_sysv_semset_t;

static __jacl_sysv_semset_t __jacl_sysv_semsets[__JACL_SYSV_SEMSET_MAX];
static pthread_mutex_t __jacl_sysv_lock = PTHREAD_MUTEX_INITIALIZER;

/* Look up active System V set by key. Caller must hold __jacl_sysv_lock. */
static inline int __jacl_sysv_find_by_key(key_t key) {
	for (int i = 0; i < __JACL_SYSV_SEMSET_MAX; i++) {
		if (__jacl_sysv_semsets[i].active && __jacl_sysv_semsets[i].key == key)
			return i;
	}
	return -1;
}

/* Find first free System V set slot. Caller must hold __jacl_sysv_lock. */
static inline int __jacl_sysv_find_free(void) {
	for (int i = 0; i < __JACL_SYSV_SEMSET_MAX; i++) {
		if (!__jacl_sysv_semsets[i].active) return i;
	}
	return -1;
}

#if SEM_POSIX

static inline int semget(key_t key, int nsems, int semflg) {
	if (nsems < 0 || nsems > __JACL_SYSV_MAX_SEMS) return (__errno_set(EINVAL), -1);

	pthread_once(&__jacl_sem_undo_once, __jacl_sem_undo_init);

	pthread_mutex_lock(&__jacl_sysv_lock);

	int found = -1;
	if (key != IPC_PRIVATE) found = __jacl_sysv_find_by_key(key);

	if (found >= 0) {
		if ((semflg & IPC_CREAT) && (semflg & IPC_EXCL)) {
			pthread_mutex_unlock(&__jacl_sysv_lock);

			return (__errno_set(EEXIST), -1);
		}

		pthread_mutex_unlock(&__jacl_sysv_lock);

		return found;
	}

	if (!(semflg & IPC_CREAT) && key != IPC_PRIVATE) {
		pthread_mutex_unlock(&__jacl_sysv_lock);

		return (__errno_set(ENOENT), -1);
	}

	int free_set = __jacl_sysv_find_free();
	if (free_set < 0) {
		pthread_mutex_unlock(&__jacl_sysv_lock);

		return (__errno_set(ENOSPC), -1);
	}

	__jacl_sysv_semsets[free_set].active = 1;
	__jacl_sysv_semsets[free_set].key = key;
	__jacl_sysv_semsets[free_set].ds.sem_nsems = (unsigned short)nsems;
	__jacl_sysv_semsets[free_set].ds.sem_otime = 0;
	__jacl_sysv_semsets[free_set].ds.sem_ctime = time(NULL);
	__jacl_sysv_semsets[free_set].ds.sem_perm.uid = getuid();
	__jacl_sysv_semsets[free_set].ds.sem_perm.gid = getgid();
	__jacl_sysv_semsets[free_set].ds.sem_perm.cuid = getuid();
	__jacl_sysv_semsets[free_set].ds.sem_perm.cgid = getgid();
	__jacl_sysv_semsets[free_set].ds.sem_perm.mode = semflg & 0777;

	for (int i = 0; i < nsems; i++) {
		int prim_idx = __jacl_sem_slot_alloc(0, 0, 0, CLOCK_REALTIME);

		if (prim_idx < 0) {
			for (int j = 0; j < i; j++) __jacl_sem_slot_free(__jacl_sysv_semsets[free_set].slot_indices[j]);

			__jacl_sysv_semsets[free_set].active = 0;

			pthread_mutex_unlock(&__jacl_sysv_lock);

			return (__errno_set(ENOSPC), -1);
		}
		__jacl_sysv_semsets[free_set].slot_indices[i] = prim_idx;
	}

	pthread_mutex_unlock(&__jacl_sysv_lock);
	return free_set;
}

/* Build sorted array of primitive slot indices for this op vector. */
static inline void __jacl_sem_build_sorted_indices(int semid, struct sembuf *sops, size_t nsops, int *sorted) {
	for (size_t i = 0; i < nsops; i++)
		sorted[i] = __jacl_sysv_semsets[semid].slot_indices[sops[i].sem_num];
	for (size_t i = 0; i < nsops; i++) {
		for (size_t j = i + 1; j < nsops; j++) {
			if (sorted[j] < sorted[i]) {
				int tmp = sorted[i]; sorted[i] = sorted[j]; sorted[j] = tmp;
			}
		}
	}
}

/* Lock a set of primitive slots (deduped). Marks locked[idx]=1. */
static inline void __jacl_sem_lock_slots(const int *sorted, size_t nsops, int *locked) {
	for (size_t i = 0; i < nsops; i++) {
		int idx = sorted[i];
		if (!locked[idx]) {
			pthread_mutex_lock(&__jacl_sem_slots[idx].p.lock);
			locked[idx] = 1;
		}
	}
}

/* Unlock every slot marked in locked[]. */
static inline void __jacl_sem_unlock_all(int *locked) {
	for (int i = 0; i < __JACL_SEM_SLOT_MAX; i++) {
		if (locked[i]) pthread_mutex_unlock(&__jacl_sem_slots[i].p.lock);
	}
}

/* Check if any op in the vector would block. */
struct __jacl_sem_block_info {
	int would_block;
	int block_idx;
	int block_op_positive;	/* 1 if blocking op was sem_op==0 (wait-for-zero) */
};

static inline struct __jacl_sem_block_info __jacl_sem_check_block(int semid, struct sembuf *sops, size_t nsops) {
	struct __jacl_sem_block_info bi = {0, -1, 0};
	for (size_t i = 0; i < nsops; i++) {
		int idx = __jacl_sysv_semsets[semid].slot_indices[sops[i].sem_num];
		__jacl_sem_primitive_t *p = &__jacl_sem_slots[idx].p;
		if (sops[i].sem_op < 0 && p->value < (unsigned int)(-sops[i].sem_op)) {
			bi.would_block = 1;
			bi.block_idx = idx;
			bi.block_op_positive = 0;
			break;
		}
		if (sops[i].sem_op == 0 && p->value != 0) {
			bi.would_block = 1;
			bi.block_idx = idx;
			bi.block_op_positive = 1;
			break;
		}
	}
	return bi;
}

static inline int __jacl_sem_has_nowait(struct sembuf *sops, size_t nsops) {
	for (size_t i = 0; i < nsops; i++) {
		if (sops[i].sem_flg & IPC_NOWAIT) return 1;
	}
	return 0;
}

/* Wait on the blocking primitive until signalled. */
static inline void __jacl_sem_wait_blocker(struct __jacl_sem_block_info bi) {
	__jacl_sem_primitive_t *bp = &__jacl_sem_slots[bi.block_idx].p;
	pthread_mutex_lock(&bp->lock);
	if (bi.block_op_positive) bp->zcnt++; else bp->ncnt++;
	pthread_cond_wait(&bp->cond, &bp->lock);
	if (bi.block_op_positive) bp->zcnt--; else bp->ncnt--;
	pthread_mutex_unlock(&bp->lock);
}

/* Apply the op vector (caller holds all locks). Broadcasts wake zcnt/ncnt waiters. */
static inline void __jacl_sem_apply_ops(int semid, struct sembuf *sops, size_t nsops) {
	for (size_t i = 0; i < nsops; i++) {
		int idx = __jacl_sysv_semsets[semid].slot_indices[sops[i].sem_num];
		__jacl_sem_primitive_t *p = &__jacl_sem_slots[idx].p;
		if (sops[i].sem_op < 0) {
			p->value -= (unsigned int)(-sops[i].sem_op);
			if (sops[i].sem_flg & SEM_UNDO) __jacl_sem_undo_add(idx, sops[i].sem_op);
			pthread_cond_broadcast(&p->cond);
		} else if (sops[i].sem_op > 0) {
			p->value += (unsigned int)sops[i].sem_op;
			if (sops[i].sem_flg & SEM_UNDO) __jacl_sem_undo_add(idx, sops[i].sem_op);
			pthread_cond_broadcast(&p->cond);
		}
		p->last_pid = getpid();
	}
	__jacl_sysv_semsets[semid].ds.sem_otime = time(NULL);
}

static inline int semop(int semid, struct sembuf *sops, size_t nsops) {
	if (semid < 0 || semid >= __JACL_SYSV_SEMSET_MAX || !__jacl_sysv_semsets[semid].active || !sops || nsops == 0) return (__errno_set(EINVAL), -1);

	/* Bounds-check every sem_num up front. */
	unsigned short nsems = __jacl_sysv_semsets[semid].ds.sem_nsems;
	for (size_t i = 0; i < nsops; i++) {
		if (sops[i].sem_num >= nsems) return (__errno_set(EFBIG), -1);
	}

	pthread_once(&__jacl_sem_undo_once, __jacl_sem_undo_init);

	while (1) {
		int locked[__JACL_SEM_SLOT_MAX];

		memset(locked, 0, sizeof(locked));

		int sorted_indices[__JACL_SYSV_MAX_SEMS];

		__jacl_sem_build_sorted_indices(semid, sops, nsops, sorted_indices);
		__jacl_sem_lock_slots(sorted_indices, nsops, locked);

		struct __jacl_sem_block_info bi = __jacl_sem_check_block(semid, sops, nsops);

		if (bi.would_block) {
			__jacl_sem_unlock_all(locked);

			if (__jacl_sem_has_nowait(sops, nsops)) return (__errno_set(EAGAIN), -1);

			__jacl_sem_wait_blocker(bi);

			continue;
		}

		__jacl_sem_apply_ops(semid, sops, nsops);
		__jacl_sem_unlock_all(locked);

		return 0;
	}
}

/* semctl handlers - each does one cmd's work, pre-validated. */
static inline int __jacl_semctl_rmid(__jacl_sysv_semset_t *set) {
	pthread_mutex_lock(&__jacl_sysv_lock);

	for (int i = 0; i < set->ds.sem_nsems; i++) __jacl_sem_slot_free(set->slot_indices[i]);

	set->active = 0;

	pthread_mutex_unlock(&__jacl_sysv_lock);

	return 0;
}

static inline int __jacl_semctl_stat(__jacl_sysv_semset_t *set, union semun arg) {
	if (!arg.buf) return (__errno_set(EFAULT), -1);

	pthread_mutex_lock(&__jacl_sysv_lock);

	*arg.buf = set->ds;

	pthread_mutex_unlock(&__jacl_sysv_lock);

	return 0;
}

static inline int __jacl_semctl_set(__jacl_sysv_semset_t *set, union semun arg) {
	if (!arg.buf) return (__errno_set(EFAULT), -1);

	pthread_mutex_lock(&__jacl_sysv_lock);

	set->ds.sem_perm.uid = arg.buf->sem_perm.uid;
	set->ds.sem_perm.gid = arg.buf->sem_perm.gid;
	set->ds.sem_perm.mode = arg.buf->sem_perm.mode;
	set->ds.sem_ctime = time(NULL);

	pthread_mutex_unlock(&__jacl_sysv_lock);

	return 0;
}

static inline int __jacl_semctl_getval(__jacl_sysv_semset_t *set, int semnum) {
	if (semnum < 0 || semnum >= set->ds.sem_nsems) return (__errno_set(EINVAL), -1);

	return (int)__jacl_sem_slots[set->slot_indices[semnum]].p.value;
}

static inline int __jacl_semctl_setval(__jacl_sysv_semset_t *set, int semnum, union semun arg) {
	if (semnum < 0 || semnum >= set->ds.sem_nsems) return (__errno_set(EINVAL), -1);

	int idx = set->slot_indices[semnum];

	pthread_mutex_lock(&__jacl_sem_slots[idx].p.lock);

	__jacl_sem_slots[idx].p.value = (unsigned int)arg.val;

	pthread_cond_broadcast(&__jacl_sem_slots[idx].p.cond);
	pthread_mutex_unlock(&__jacl_sem_slots[idx].p.lock);

	return 0;
}

static inline int __jacl_semctl_getall(__jacl_sysv_semset_t *set, union semun arg) {
	if (!arg.array) return (__errno_set(EFAULT), -1);

	for (int i = 0; i < set->ds.sem_nsems; i++) arg.array[i] = (unsigned short)__jacl_sem_slots[set->slot_indices[i]].p.value;

	return 0;
}

static inline int __jacl_semctl_setall(__jacl_sysv_semset_t *set, union semun arg) {
	if (!arg.array) return (__errno_set(EFAULT), -1);

	for (int i = 0; i < set->ds.sem_nsems; i++) {
		int idx = set->slot_indices[i];

		pthread_mutex_lock(&__jacl_sem_slots[idx].p.lock);

		__jacl_sem_slots[idx].p.value = arg.array[i];

		pthread_cond_broadcast(&__jacl_sem_slots[idx].p.cond);
		pthread_mutex_unlock(&__jacl_sem_slots[idx].p.lock);
	}

	return 0;
}

static inline int __jacl_semctl_getinfo(__jacl_sysv_semset_t *set, int semnum, int cmd) {
	if (semnum < 0 || semnum >= set->ds.sem_nsems) return (__errno_set(EINVAL), -1);

	__jacl_sem_primitive_t *p = &__jacl_sem_slots[set->slot_indices[semnum]].p;

	switch (cmd) {
		case GETPID:  return (int)p->last_pid;
		case GETNCNT: return (int)p->ncnt;
		case GETZCNT: return (int)p->zcnt;
		default:      return (__errno_set(EINVAL), -1);
	}
}

static inline int semctl(int semid, int semnum, int cmd, ...) {
	if (semid < 0 || semid >= __JACL_SYSV_SEMSET_MAX || !__jacl_sysv_semsets[semid].active) return (__errno_set(EINVAL), -1);

	va_list ap;

	va_start(ap, cmd);

	union semun arg = va_arg(ap, union semun);

	va_end(ap);

	__jacl_sysv_semset_t *set = &__jacl_sysv_semsets[semid];

	switch (cmd) {
		case IPC_RMID: return __jacl_semctl_rmid(set);
		case IPC_STAT: return __jacl_semctl_stat(set, arg);
		case IPC_SET:  return __jacl_semctl_set(set, arg);
		case GETVAL:   return __jacl_semctl_getval(set, semnum);
		case SETVAL:   return __jacl_semctl_setval(set, semnum, arg);
		case GETALL:   return __jacl_semctl_getall(set, arg);
		case SETALL:   return __jacl_semctl_setall(set, arg);
		case GETPID:
		case GETNCNT:
		case GETZCNT:  return __jacl_semctl_getinfo(set, semnum, cmd);
		default:       return (__errno_set(EINVAL), -1);
	}
}

#else /* !SEM_POSIX (Windows/WASM stubs) */

static inline int semget(key_t key, int nsems, int semflg) {
	(void)key; (void)nsems; (void)semflg; return (__errno_set(ENOSYS), -1);
}
static inline int semop(int semid, struct sembuf *sops, size_t nsops) {
	(void)semid; (void)sops; (void)nsops; return (__errno_set(ENOSYS), -1);
}
static inline int semctl(int semid, int semnum, int cmd, ...) {
	(void)semid; (void)semnum; (void)cmd; return (__errno_set(ENOSYS), -1);
}

#endif

/* ============================================================================ */
/* Utility Functions                                                            */
/* ============================================================================ */

static inline void sem_init_struct(sem_t *sem) {
	if (sem) *sem = SEM_FAILED;
}

static inline int sem_is_valid(sem_t *sem) {
	return __jacl_sem_resolve(sem) != NULL;
}

static inline int sem_is_anonymous(sem_t *sem) {
	__jacl_sem_primitive_t *p = __jacl_sem_resolve(sem);
	return p && p->anonymous;
}

static inline int sem_is_named(sem_t *sem) {
	__jacl_sem_primitive_t *p = __jacl_sem_resolve(sem);
	return p && !p->anonymous;
}

static inline int sem_is_unlinked(sem_t *sem) {
	if (!sem_is_named(sem)) return 0;
	pthread_mutex_lock(&__jacl_sem_named_lock);
	int found = __jacl_sem_named_find_by_slot((int)*sem);
	int result = (found >= 0) ? __jacl_sem_named[found].unlinked : 0;
	pthread_mutex_unlock(&__jacl_sem_named_lock);
	return result;
}

static inline const char *sem_get_name(sem_t *sem) {
	if (!sem_is_named(sem)) return NULL;
	pthread_mutex_lock(&__jacl_sem_named_lock);
	int found = __jacl_sem_named_find_by_slot((int)*sem);
	const char *result = (found >= 0) ? __jacl_sem_named[found].name : NULL;
	pthread_mutex_unlock(&__jacl_sem_named_lock);
	return result;
}

static inline int sem_get_refcount(sem_t *sem) {
	if (!sem || *sem < 0 || *sem >= __JACL_SEM_SLOT_MAX) return -1;
	if (!__jacl_sem_slots[*sem].active) return -1;
	if (__jacl_sem_slots[*sem].p.anonymous) return 1;
	pthread_mutex_lock(&__jacl_sem_named_lock);
	int found = __jacl_sem_named_find_by_slot((int)*sem);
	int result = (found >= 0) ? __jacl_sem_named[found].refcount : -1;
	pthread_mutex_unlock(&__jacl_sem_named_lock);
	return result;
}

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SEM_H */
