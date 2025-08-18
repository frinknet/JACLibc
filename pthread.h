// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef PTHREAD_H
#define PTHREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdatomic.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#if defined(_WIN32)
	#define PTHREAD_WIN32
	#include <windows.h>
	#include <process.h>
#elif defined(__wasm__) || defined(__EMSCRIPTEN__)
	#define PTHREAD_WASM
#else
	#define PTHREAD_POSIX
	#include <unistd.h>
	#include <sys/syscall.h>
	#include <linux/futex.h>
	#include <sys/time.h>
#endif

/*―― Types ――*/
typedef struct { void *handle; uintptr_t id; void *stack; } pthread_t;
typedef struct { atomic_int state; } pthread_mutex_t;
typedef struct { atomic_int waiters; atomic_int futex; } pthread_cond_t;
typedef uintptr_t pthread_key_t;

/*―― Threads ――*/
static inline int pthread_create(pthread_t *t, const void *attr,
																 void *(*start)(void*), void *arg) {
	(void)attr;
#ifdef PTHREAD_WIN32
	t->handle = (void*)_beginthreadex(NULL,0,
			(unsigned(__stdcall*)(void*))start,arg,0,
			(unsigned*)&t->id);
	return t->handle?0:EAGAIN;
#elif defined(PTHREAD_WASM)
	return ENOSYS;
#else
	const int SS = 1024*1024;
	void *stk = malloc(SS);
	if (!stk) return ENOMEM;
	long tid = syscall(SYS_clone,
		CLONE_VM|CLONE_FS|CLONE_FILES|CLONE_SIGHAND|CLONE_THREAD,
		(char*)stk+SS, NULL, NULL, 0);
	if (tid>0) { t->id=tid; t->handle=NULL; t->stack=stk; return 0; }
	free(stk); return EAGAIN;
#endif
}

static inline int pthread_join(pthread_t t, void **res) {
#ifdef PTHREAD_WIN32
	WaitForSingleObject((HANDLE)t.handle,INFINITE);
	if(res)*res=NULL; CloseHandle((HANDLE)t.handle);
	return 0;
#elif defined(PTHREAD_WASM)
	return ENOSYS;
#else
	// busy‐poll until thread exits
	while(syscall(SYS_tgkill, getpid(), t.id, 0)==0)
		usleep(1000);
	free(t.stack);
	if(res)*res=NULL;
	return 0;
#endif
}

static inline void pthread_exit(void *res) {
#ifdef PTHREAD_WIN32
	_endthreadex((uintptr_t)res);
#elif defined(PTHREAD_WASM)
	(void)res;
#else
	syscall(SYS_exit, (uintptr_t)res);
#endif
}

/*―― Mutex ――*/
#define PTHREAD_MUTEX_UNLOCKED 0
#define PTHREAD_MUTEX_LOCKED	 1

static inline int pthread_mutex_init(pthread_mutex_t *m,const void*_) {
	(void)_;
	atomic_store(&m->state,PTHREAD_MUTEX_UNLOCKED);
	return 0;
}
static inline int pthread_mutex_lock(pthread_mutex_t *m) {
	int e=PTHREAD_MUTEX_UNLOCKED;
	while(!atomic_compare_exchange_weak(&m->state,&e,PTHREAD_MUTEX_LOCKED))
		{ e=PTHREAD_MUTEX_UNLOCKED; 
#ifdef PTHREAD_WIN32 Sleep(0);
#else usleep(100);
#endif }
	return 0;
}
static inline int pthread_mutex_unlock(pthread_mutex_t *m) {
	atomic_store(&m->state,PTHREAD_MUTEX_UNLOCKED);
	return 0;
}
static inline int pthread_mutex_destroy(pthread_mutex_t*m){(void)m;return 0;}

/*―― Condition ――*/
static inline int pthread_cond_init(pthread_cond_t *c,const void*_) {
	(void)_; atomic_store(&c->waiters,0); atomic_store(&c->futex,0);
	return 0;
}

static inline int pthread_cond_wait(pthread_cond_t *c,pthread_mutex_t *m){
#ifdef PTHREAD_WIN32
	SleepConditionVariableCS((PCONDITION_VARIABLE)c,(LPCRITICAL_SECTION)m,INFINITE);
	return 0;
#elif defined(PTHREAD_WASM)
	return 0;
#else
	atomic_fetch_add(&c->waiters,1);
	pthread_mutex_unlock(m);
	// futex wait
	syscall(SYS_futex,&c->futex, FUTEX_WAIT, 0, NULL, NULL, 0);
	pthread_mutex_lock(m);
	atomic_fetch_sub(&c->waiters,1);
	return 0;
#endif
}

static inline int pthread_cond_signal(pthread_cond_t *c){
#ifdef PTHREAD_WIN32
	WakeConditionVariable((PCONDITION_VARIABLE)c);
	return 0;
#elif defined(PTHREAD_WASM)
	return 0;
#else
	if(atomic_load(&c->waiters)>0){
		atomic_store(&c->futex,1);
		syscall(SYS_futex,&c->futex,FUTEX_WAKE,1,NULL,NULL,0);
	}
	return 0;
#endif
}

static inline int pthread_cond_broadcast(pthread_cond_t*c){
#ifdef PTHREAD_WIN32
	WakeAllConditionVariable((PCONDITION_VARIABLE)c);
	return 0;
#elif defined(PTHREAD_WASM)
	return 0;
#else
	atomic_store(&c->futex,1);
	syscall(SYS_futex,&c->futex,FUTEX_WAKE,INT_MAX,NULL,NULL,0);
	return 0;
#endif
}

static inline int pthread_cond_destroy(pthread_cond_t*c){
	(void)c; return 0;
}

/*―― TLS ――*/
#define MAX_TLS 64
static atomic_uint tls_ctr=0;
static void *tls_arr[MAX_TLS];

static inline int pthread_key_create(pthread_key_t*k,void(*_)(void*)){
	*k = atomic_fetch_add(&tls_ctr,1);
	return (*k<MAX_TLS)?0:EAGAIN;
}
static inline int pthread_key_delete(pthread_key_t k){
	(void)k; return 0;
}
static inline int pthread_setspecific(pthread_key_t k,const void*v){
	if(k<MAX_TLS){tls_arr[k]=(void*)v;return 0;}return EINVAL;
}
static inline void* pthread_getspecific(pthread_key_t k){
	return (k<MAX_TLS)?tls_arr[k]:NULL;
}

#ifdef __cplusplus
}
#endif
#endif /* PTHREAD_H */
