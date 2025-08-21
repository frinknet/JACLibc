// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* C11 threading types */
typedef pthread_t				thrd_t;
typedef pthread_mutex_t mtx_t;
typedef pthread_cond_t	cnd_t;
typedef unsigned				tss_t;
typedef int (*thrd_start_t)(void*);

/* Thread functions */
static inline int thrd_create(thrd_t *thr, thrd_start_t func, void *arg) { return pthread_create(thr, NULL, (void*(*)(void*))func, arg); }
static inline _Noreturn void thrd_exit(int res) { pthread_exit((void*)(intptr_t)res); }
static inline int thrd_join(thrd_t thr, int *res) {
	void *r;

	int e = pthread_join(thr, &r);
	if (e == 0 && res) *res = (int)(intptr_t)r;

	return e;
}

/* Mutex */
static inline int mtx_init(mtx_t *m, int type) { return pthread_mutex_init(m, NULL); }
static inline int mtx_lock(mtx_t *m) { return pthread_mutex_lock(m); }
static inline int mtx_unlock(mtx_t *m) { return pthread_mutex_unlock(m); }
static inline void mtx_destroy(mtx_t *m) { pthread_mutex_destroy(m); }

/* Condition variable */
static inline int cnd_init(cnd_t *c) { return pthread_cond_init(c, NULL); }
static inline int cnd_signal(cnd_t *c) { return pthread_cond_signal(c); }
static inline int cnd_broadcast(cnd_t *c) { return pthread_cond_broadcast(c); }
static inline int cnd_wait(cnd_t *c, mtx_t *m) { return pthread_cond_wait(c, m); }
static inline void cnd_destroy(cnd_t *c) { pthread_cond_destroy(c); }

/* Thread local storage */
static inline int tss_create(tss_t *key, void (*dtor)(void*)) { return pthread_key_create(key, dtor); }
static inline int tss_delete(tss_t key) { return pthread_key_delete(key); }
static inline int tss_set(tss_t key, void *val) { return pthread_setspecific(key, val) ? errno : 0; }
static inline void *tss_get(tss_t key) { return pthread_getspecific(key); }

#ifdef __cplusplus
}
#endif

#endif /* THREADS_H */
