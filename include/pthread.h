/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _PTHREAD_H
#define _PTHREAD_H
#pragma once

#include <config.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>

#if JACL_HAS_C11
	#include <stdatomic.h>
#else
	#define atomic_load(ptr) (*(ptr))
	#define atomic_store(ptr, val) (*(ptr) = (val))
	#define atomic_fetch_add(ptr, val) ((*(ptr))++)
	#define atomic_fetch_sub(ptr, val) ((*(ptr))--)
	#define atomic_compare_exchange_strong(ptr, expected, desired) \
		((*(ptr) == *(expected)) ? (*(ptr) = (desired), 1) : (*(expected) = *(ptr), 0))
	#define atomic_compare_exchange_weak atomic_compare_exchange_strong
	#warning "WARNING: Before C11 threading happens without atomics"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* PLATFORM */
#if JACL_OS_WINDOWS
	#define PTHREAD_WIN32 1
	#include <windows.h>
	#include <process.h>
#elif JACL_HAS_PTHREADS
	#define PTHREAD_POSIX 1
	#include <unistd.h>
	#include <signal.h>
	#include <sys/wait.h>
	#include <sys/mman.h>
	#include <sys/syscall.h>
#else
	#define PTHREAD_DUMMY 1
#endif

#if JACL_OS_LINUX
	#include <sched.h>
	#include <sys/futex.h>
	#define __ARCH_CLONE
	#include JACL_ARCH_FILE
#endif

/* CONSTANTS */
#define PTHREAD_CREATE_JOINABLE         0
#define PTHREAD_CREATE_DETACHED         1
#define PTHREAD_DESTRUCTOR_ITERATIONS   4
#define PTHREAD_MUTEX_NORMAL            0
#define PTHREAD_MUTEX_RECURSIVE         1
#define PTHREAD_MUTEX_ERRORCHECK        2
#define PTHREAD_MUTEX_DEFAULT           0
#define PTHREAD_MUTEX_ROBUST            4
#define PTHREAD_MUTEX_STALLED           0
#define PTHREAD_PROCESS_PRIVATE         0
#define PTHREAD_PROCESS_SHARED          1
#define MAX_TLS_KEYS                    64

#define PTHREAD_SPIN_LOCKED             1
#define PTHREAD_SPIN_UNLOCKED           0

#define PTHREAD_CANCEL_ENABLE           0
#define PTHREAD_CANCEL_DISABLE          1
#define PTHREAD_CANCEL_DEFERRED         0
#define PTHREAD_CANCEL_ASYNCHRONOUS     1
#define PTHREAD_CANCELED                ((void *)-1)

#define PTHREAD_BARRIER_SERIAL_THREAD   -1

#define PTHREAD_NULL                    ((pthread_t)NULL)

#ifndef PTHREAD_DETACHED_WAIT
#define PTHREAD_DETACHED_WAIT       5000
#endif

/* INITIALIZERS */
#define PTHREAD_ONCE_INIT               {0}
#define PTHREAD_MUTEX_INITIALIZER       { 0, 0, PTHREAD_MUTEX_NORMAL, 0 }
#define PTHREAD_COND_INITIALIZER        { 0, 0 }
#define PTHREAD_RWLOCK_INITIALIZER      { 0, 0, 0, 0 }
#define PTHREAD_BARRIER_INITIALIZER     { 0, 0, 0, 0 }
#define PTHREAD_SPIN_LOCK_INITIALIZER   { 0 }
#define PTHREAD_MUTEXATTR_INITIALIZER   { PTHREAD_MUTEX_NORMAL }
#define PTHREAD_CONDATTR_INITIALIZER    { 0 }
#define PTHREAD_ATTR_INITIALIZER        { 0, 0, NULL }
#define PTHREAD_BARRIERATTR_INITIALIZER { PTHREAD_PROCESS_PRIVATE }
#define PTHREAD_RWLOCKATTR_INITIALIZER  { PTHREAD_PROCESS_PRIVATE }

/* TYPES */
struct __jacl_cleanup_node;
struct __jacl_pthread;

typedef struct {
	int detached;
	size_t stack_size;
	void *stack_addr;
} pthread_attr_t;

typedef struct {
	int type;
	int robust;
} pthread_mutexattr_t;

typedef struct {
	int dummy;
} pthread_condattr_t;

typedef struct {
	_Atomic int done;
} pthread_once_t;

typedef struct __jacl_cleanup_node {
	void (*routine)(void *);
	void *arg;
	struct __jacl_cleanup_node *next;
} __jacl_cleanup_node_t;

#if PTHREAD_WIN32

typedef struct {
	HANDLE handle;
	DWORD id;
	void *result;
} pthread_t;

typedef struct {
	CRITICAL_SECTION cs;
	int type;
} pthread_mutex_t;

typedef struct { CONDITION_VARIABLE cv; } pthread_cond_t;
typedef DWORD pthread_key_t;
typedef struct { int dummy; } pthread_spinlock_t;
typedef struct { int dummy; } pthread_rwlock_t;
typedef struct { int dummy; } pthread_barrier_t;
typedef struct { int pshared; } pthread_barrierattr_t;
typedef struct { int pshared; } pthread_rwlockattr_t;

#elif PTHREAD_POSIX

struct __jacl_pthread {
	pid_t tid;
	void *stack;
	size_t stack_size;
	int stack_is_mmap;
	void *result;
	_Atomic int finished;
	_Atomic int detached;
	_Atomic int joined;
	_Atomic int cancel_state;
	_Atomic int cancel_type;
	_Atomic int canceled;
	__jacl_cleanup_node_t *cleanup_stack;
};

typedef struct __jacl_pthread *pthread_t;

typedef struct {
	_Atomic int futex;
	pid_t owner;
	int type;
	int recursive_count;
} pthread_mutex_t;

typedef struct {
	_Atomic int futex;
	_Atomic int waiters;
} pthread_cond_t;

typedef unsigned int pthread_key_t;
typedef struct { _Atomic int lock; } pthread_spinlock_t;

typedef struct {
	_Atomic int readers;
	_Atomic int writers;
	_Atomic int write_waiters;
	pid_t     writer_tid;
} pthread_rwlock_t;

typedef struct {
	_Atomic int count;
	_Atomic int phase;
	unsigned  limit;
	_Atomic int waiters;
} pthread_barrier_t;

typedef struct { int pshared; } pthread_barrierattr_t;
typedef struct { int pshared; } pthread_rwlockattr_t;

extern _Atomic unsigned int __jacl_pthread_tls_key_counter;
extern thread_local void *__jacl_pthread_tls_values[MAX_TLS_KEYS];
extern pthread_key_t __jacl_pthread_self_key;
extern void (*__jacl_pthread_key_destructors[MAX_TLS_KEYS])(void *);
extern _Atomic int __jacl_pthread_inited;

#else

typedef struct { int dummy; } pthread_t;
typedef struct { int dummy; } pthread_mutex_t;
typedef struct { int dummy; } pthread_cond_t;
typedef struct { int dummy; } pthread_spinlock_t;
typedef struct { int dummy; } pthread_rwlock_t;
typedef struct { int dummy; } pthread_barrier_t;
typedef struct { int dummy; } pthread_barrierattr_t;
typedef struct { int dummy; } pthread_rwlockattr_t;
typedef int pthread_key_t;

#endif

typedef struct {
	void *(*start_routine)(void *);
	void *arg;
	pthread_t thread_ptr;
} __jacl_thread_arg_t;

/* WINDOWS */
#if PTHREAD_WIN32

static unsigned __stdcall __jacl_win_thread_start(void *arg) {
	__jacl_thread_arg_t *ta = (__jacl_thread_arg_t *)arg;
	void *result = ta->start_routine(ta->arg);
	ta->thread_ptr->result = result;
	free(ta);
	return (unsigned)(uintptr_t)result;
}
static inline int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg) {
	if (!thread || !start_routine) return EINVAL;
	__jacl_thread_arg_t *ta = malloc(sizeof(*ta));
	if (!ta) return ENOMEM;
	ta->start_routine = start_routine; ta->arg = arg; ta->thread_ptr = thread; thread->result = NULL;
	thread->handle = (HANDLE)_beginthreadex(NULL, attr ? (unsigned)attr->stack_size : 0, __jacl_win_thread_start, ta, 0, &thread->id);
	if (!thread->handle) { free(ta); return EAGAIN; }
	return 0;
}
static inline int pthread_join(pthread_t thread, void **retval) {
	DWORD result = WaitForSingleObject(thread.handle, INFINITE);
	if (result == WAIT_OBJECT_0) {
		if (retval) *retval = thread.result;
		CloseHandle(thread.handle);
		return 0;
	}
	return EINVAL;
}
static inline void pthread_exit(void *retval) { _endthreadex((unsigned)(uintptr_t)retval); }
static inline pthread_t pthread_self(void) {
	pthread_t self = {0}; self.handle = GetCurrentThread(); self.id = GetCurrentThreadId();
	return self;
}
static inline int pthread_equal(pthread_t t1, pthread_t t2) { return t1.id == t2.id; }
static inline int pthread_detach(pthread_t thread) { return CloseHandle(thread.handle) ? 0 : EINVAL; }
static inline int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
	if (!mutex) return EINVAL;
	InitializeCriticalSection(&mutex->cs);
	mutex->type = (attr && attr->type) ? attr->type : PTHREAD_MUTEX_NORMAL;
	return 0;
}
static inline int pthread_mutex_lock(pthread_mutex_t *mutex) { if (!mutex) return EINVAL; EnterCriticalSection(&mutex->cs); return 0; }
static inline int pthread_mutex_trylock(pthread_mutex_t *mutex) { if (!mutex) return EINVAL; return TryEnterCriticalSection(&mutex->cs) ? 0 : EBUSY; }
static inline int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *ts) {
	if (!mutex || !ts) return EINVAL;
	DWORD timeout = (ts->tv_sec * 1000) + (ts->tv_nsec / 1000000), result = WaitForSingleObject(mutex->cs, timeout);
	if (result == WAIT_OBJECT_0) return 0;
	if (result == WAIT_TIMEOUT) return ETIMEDOUT;
	return EINVAL;
}
static inline int pthread_mutex_unlock(pthread_mutex_t *mutex) { if (!mutex) return EINVAL; LeaveCriticalSection(&mutex->cs); return 0; }
static inline int pthread_mutex_destroy(pthread_mutex_t *mutex) { if (!mutex) return EINVAL; DeleteCriticalSection(&mutex->cs); return 0; }
static inline int pthread_mutex_consistent(pthread_mutex_t *mutex) { (void)mutex; return ENOSYS; }
static inline int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) { (void)attr; if (!cond) return EINVAL; InitializeConditionVariable(&cond->cv); return 0; }
static inline int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) { if (!cond || !mutex) return EINVAL; return SleepConditionVariableCS(&cond->cv, &mutex->cs, INFINITE) ? 0 : EINVAL; }
static inline int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime) { (void)abstime; if (!cond || !mutex) return EINVAL; return SleepConditionVariableCS(&cond->cv, &mutex->cs, INFINITE) ? 0 : EINVAL; }
static inline int pthread_cond_clockwait(pthread_cond_t *cond, pthread_mutex_t *mutex, clockid_t clock_id, const struct timespec *abstime) { (void)clock_id; (void)abstime; return pthread_cond_timedwait(cond, mutex, abstime); }
static inline int pthread_cond_signal(pthread_cond_t *cond) { if (!cond) return EINVAL; WakeConditionVariable(&cond->cv); return 0; }
static inline int pthread_cond_broadcast(pthread_cond_t *cond) { if (!cond) return EINVAL; WakeAllConditionVariable(&cond->cv); return 0; }
static inline int pthread_cond_destroy(pthread_cond_t *cond) { if (!cond) return EINVAL; return 0; }
static inline int pthread_key_create(pthread_key_t *key, void (*destructor)(void*)) { (void)destructor; if (!key) return EINVAL; *key = TlsAlloc(); return (*key != TLS_OUT_OF_INDEXES) ? 0 : EAGAIN; }
static inline int pthread_key_delete(pthread_key_t key) { return TlsFree(key) ? 0 : EINVAL; }
static inline int pthread_setspecific(pthread_key_t key, const void *value) { return TlsSetValue(key, (void*)value) ? 0 : EINVAL; }
static inline void *pthread_getspecific(pthread_key_t key) { return TlsGetValue(key); }
static inline int pthread_once(pthread_once_t *once_control, void (*init_routine)(void)) {
	if (!once_control || !init_routine) return EINVAL;
	int expected = 0;
	if (atomic_compare_exchange_strong(&once_control->done, &expected, 1)) init_routine();
	return 0;
}
static inline int pthread_attr_init(pthread_attr_t *attr) { if (!attr) return EINVAL; attr->detached = 0; attr->stack_size = 0; attr->stack_addr = NULL; return 0; }
static inline int pthread_attr_destroy(pthread_attr_t *attr) { if (!attr) return EINVAL; return 0; }
static inline int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) { if (!attr) return EINVAL; attr->detached = (detachstate == PTHREAD_CREATE_DETACHED); return 0; }
static inline int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) { if (!attr) return EINVAL; attr->stack_size = stacksize; return 0; }
static inline int pthread_mutexattr_init(pthread_mutexattr_t *attr) { if (!attr) return EINVAL; attr->type = PTHREAD_MUTEX_NORMAL; attr->robust = 0; return 0; }
static inline int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) { if (!attr) return EINVAL; return 0; }
static inline int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) { if (!attr) return EINVAL; attr->type = type; return 0; }
static inline int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type) { if (!attr || !type) return EINVAL; *type = attr->type; return 0; }
static inline int pthread_mutexattr_setrobust(pthread_mutexattr_t *attr, int robust) { if (!attr) return EINVAL; attr->robust = robust; return 0; }
static inline int pthread_mutexattr_getrobust(const pthread_mutexattr_t *attr, int *robust) { if (!attr || !robust) return EINVAL; *robust = attr->robust; return 0; }
static inline int pthread_condattr_init(pthread_condattr_t *attr) { if (!attr) return EINVAL; attr->dummy = 0; return 0; }
static inline int pthread_condattr_destroy(pthread_condattr_t *attr) { if (!attr) return EINVAL; return 0; }
static inline int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset) { (void)how; (void)set; (void)oldset; return ENOSYS; }
static inline int pthread_spin_init(pthread_spinlock_t *lock, int pshared) { (void)lock; (void)pshared; return ENOSYS; }
static inline int pthread_spin_destroy(pthread_spinlock_t *lock) { (void)lock; return ENOSYS; }
static inline int pthread_spin_lock(pthread_spinlock_t *lock) { (void)lock; return ENOSYS; }
static inline int pthread_spin_trylock(pthread_spinlock_t *lock) { (void)lock; return ENOSYS; }
static inline int pthread_spin_unlock(pthread_spinlock_t *lock) { (void)lock; return ENOSYS; }
static inline int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr) { (void)rwlock; (void)attr; return ENOSYS; }
static inline int pthread_rwlock_destroy(pthread_rwlock_t *rwlock) { (void)rwlock; return ENOSYS; }
static inline int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) { (void)rwlock; return ENOSYS; }
static inline int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) { (void)rwlock; return ENOSYS; }
static inline int pthread_rwlock_unlock(pthread_rwlock_t *rwlock) { (void)rwlock; return ENOSYS; }
static inline int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock) { (void)rwlock; return ENOSYS; }
static inline int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock) { (void)rwlock; return ENOSYS; }
static inline int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abstime) { (void)rwlock; (void)abstime; return ENOSYS; }
static inline int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abstime) { (void)rwlock; (void)abstime; return ENOSYS; }
static inline int pthread_rwlockattr_init(pthread_rwlockattr_t *attr) { (void)attr; return ENOSYS; }
static inline int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr) { (void)attr; return ENOSYS; }
static inline int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned count) { (void)barrier; (void)attr; (void)count; return ENOSYS; }
static inline int pthread_barrier_destroy(pthread_barrier_t *barrier) { (void)barrier; return ENOSYS; }
static inline int pthread_barrier_wait(pthread_barrier_t *barrier) { (void)barrier; return ENOSYS; }
static inline int pthread_barrierattr_init(pthread_barrierattr_t *attr) { (void)attr; return ENOSYS; }
static inline int pthread_barrierattr_destroy(pthread_barrierattr_t *attr) { (void)attr; return ENOSYS; }
static inline int pthread_cancel(pthread_t thread) { (void)thread; return ENOSYS; }
static inline int pthread_setcancelstate(int state, int *oldstate) { (void)state; (void)oldstate; return ENOSYS; }
static inline int pthread_setcanceltype(int type, int *oldtype) { (void)type; (void)oldtype; return ENOSYS; }
static inline void pthread_testcancel(void) { }
static inline int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) { (void)attr; (void)detachstate; return ENOSYS; }
#define pthread_cleanup_push(routine, arg) do{
#define pthread_cleanup_pop(execute) }while(0)

/* ======================================================================== */
/* POSIX IMPLEMENTATION (PURE C - NO ASM)                                   */
/* ======================================================================== */
#elif PTHREAD_POSIX

#if JACL_OS_LINUX
	static inline pid_t __jacl_pthread_gettid(void) { return (pid_t)syscall(SYS_gettid); }
	static inline int __jacl_pthread_futex_wait(void *addr, int val) { return syscall(SYS_futex, addr, FUTEX_WAIT, val, NULL, NULL, 0); }
	static inline int __jacl_pthread_futex_wake(void *addr, int num) { return syscall(SYS_futex, addr, FUTEX_WAKE, num, NULL, NULL, 0); }
#elif JACL_OS_DARWIN
	static inline pid_t __jacl_pthread_gettid(void) { return (pid_t)syscall(SYS_thread_selfid); }
	extern int __ulock_wait(uint32_t, void *, uint64_t, uint32_t);
	extern int __ulock_wake(uint32_t, void *);
	static inline int __jacl_pthread_futex_wait(void *addr, int val) { return syscall(SYS_ulock_wait, 1, addr, val, 0); }
	static inline int __jacl_pthread_futex_wake(void *addr, int num) { uint32_t op = (num == 1) ? 0x101 : 0x100; return syscall(SYS_ulock_wake, op, addr); }
#elif JACL_OS_FREEBSD
	static inline pid_t __jacl_pthread_gettid(void) { long tid; syscall(SYS_thr_self, &tid); return (pid_t)tid; }
	static inline int __jacl_pthread_futex_wait(void *addr, int val) { return syscall(SYS__umtx_op, addr, 3, val, NULL, NULL); }
	static inline int __jacl_pthread_futex_wake(void *addr, int num) { return syscall(SYS__umtx_op, addr, 4, num, NULL, NULL); }
#else
	static inline pid_t __jacl_pthread_gettid(void) { return getpid(); }
	static inline int __jacl_pthread_futex_wait(void *addr, int val) { return -1; }
	static inline int __jacl_pthread_futex_wake(void *addr, int num) { return -1; }
#endif

static inline void __jacl_pthread_init_keys(void) {
	int expected = 0;
	if (!atomic_compare_exchange_strong(&__jacl_pthread_inited, &expected, 1)) return;
	while (1) {
		unsigned int k = atomic_fetch_add_explicit(&__jacl_pthread_tls_key_counter, 1, memory_order_acq_rel);
		if (k >= MAX_TLS_KEYS) {
			atomic_store_explicit(&__jacl_pthread_tls_key_counter, 0, memory_order_release);
			k = 0;
		}
		__jacl_pthread_self_key = k;
		break;
	}
}
static inline void __jacl_pthread_register_self(void) {
	__jacl_pthread_init_keys();
	if (__jacl_pthread_self_key >= MAX_TLS_KEYS) return;
	pthread_t self = (pthread_t)__jacl_pthread_tls_values[__jacl_pthread_self_key];
	if (self && self->tid == __jacl_pthread_gettid()) return;
	pthread_t t = calloc(1, sizeof *t);
	if (!t) return;
	t->tid = __jacl_pthread_gettid(); t->stack = NULL; t->stack_size = 0; t->stack_is_mmap = 0; t->result = NULL;
	atomic_store(&t->finished, 0); atomic_store(&t->detached, 0); atomic_store(&t->joined, 0); atomic_store(&t->cancel_state, PTHREAD_CANCEL_ENABLE);
	atomic_store(&t->cancel_type, PTHREAD_CANCEL_DEFERRED); atomic_store(&t->canceled, 0);
	t->cleanup_stack = NULL;
	__jacl_pthread_tls_values[__jacl_pthread_self_key] = t;
}
static inline int __jacl_pthread_attr_is_valid(const pthread_attr_t *attr) { if (!attr) return 0; if (attr->detached != 0 && attr->detached != 1) return 0; return 1; }
static inline int __jacl_pthread_mutex_is_valid(const pthread_mutex_t *mutex) {
	if (!mutex) return 0;
	int f = atomic_load(&mutex->futex);
	if (f != 0 && f != 1) return 0;
	if (mutex->type != PTHREAD_MUTEX_NORMAL && mutex->type != PTHREAD_MUTEX_RECURSIVE && mutex->type != PTHREAD_MUTEX_ERRORCHECK && mutex->type != PTHREAD_MUTEX_ROBUST) return 0;
	return 1;
}
static inline void __jacl_pthread_destroy(void) {
	int iterations = PTHREAD_DESTRUCTOR_ITERATIONS, had_values;
	do {
		had_values = 0;
		for (unsigned int i = 0; i < atomic_load(&__jacl_pthread_tls_key_counter) && i < MAX_TLS_KEYS; i++) {
			void *value = __jacl_pthread_tls_values[i];
			void (*destructor)(void *) = __jacl_pthread_key_destructors[i];
			if (value && destructor) { __jacl_pthread_tls_values[i] = NULL; destructor(value); had_values = 1; }
		}
	} while (had_values && --iterations > 0);
}
static inline void __jacl_pthread_set_self(pthread_t thread_ptr) { if (__jacl_pthread_self_key < MAX_TLS_KEYS) __jacl_pthread_tls_values[__jacl_pthread_self_key] = thread_ptr; }
static inline int __jacl_pthread_entry(void *arg) {
	__jacl_thread_arg_t *ta = (__jacl_thread_arg_t *)arg;
	pthread_t t = ta->thread_ptr;
	__jacl_pthread_set_self(t);
	void *result = NULL;
	int err = EINVAL;
	if (t != NULL) { result = ta->start_routine(ta->arg); t->result = result; err = 0; }
	atomic_store(&t->finished, 1);
	free(ta);
	return err;
}
static inline pid_t __jacl_pthread_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) { return __jacl_arch_clone_thread(stack, stack_size, fn, arg); }
static inline int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg) {
	if (!thread || !start_routine) return EINVAL;
	__jacl_pthread_init_keys();
	pthread_t t = calloc(1, sizeof *t);
	if (!t) return ENOMEM;
	__jacl_thread_arg_t *ta = malloc(sizeof *ta);
	if (!ta) { free(t); return ENOMEM; }
	size_t stack_size = (attr && attr->stack_size) ? attr->stack_size : 1024 * 1024;
	void *stack = NULL;
	int stack_is_mmap = 0;
	stack = mmap(NULL, stack_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_STACK, -1, 0);
	if (stack != MAP_FAILED) stack_is_mmap = 1;
	else stack = NULL;
	if (!stack) {
		stack = malloc(stack_size);
		if (!stack) { free(ta); free(t); return ENOMEM; }
		stack_is_mmap = 0;
	}
	ta->start_routine = start_routine; ta->arg = arg; ta->thread_ptr = t;
	t->stack = stack; t->stack_size = stack_size; t->stack_is_mmap = stack_is_mmap;
	t->result = NULL;
	atomic_store(&t->finished, 0);
	atomic_store(&t->detached, (attr && attr->detached) ? 1 : 0);
	atomic_store(&t->joined, 0);
	pid_t tid = __jacl_pthread_clone_thread(stack, stack_size, __jacl_pthread_entry, ta);
	if (tid > 0) { t->tid = tid; *thread = t; return 0; }
	if (stack) {
		if (stack_is_mmap) munmap(stack, stack_size);
		else free(stack);
	}
	free(ta); free(t); return EAGAIN;
}
static inline int pthread_join(pthread_t thread, void **retval) {
	if (!thread) return EINVAL;
	if (atomic_load(&thread->detached)) return EINVAL;
	__jacl_pthread_init_keys();
	int expected = 0;
	if (!atomic_compare_exchange_strong(&thread->joined, &expected, 1)) return EINVAL;
	struct timespec ts = {0, 100000};
	while (!atomic_load(&thread->finished)) {
		if (atomic_load(&thread->detached)) return ESRCH;
		nanosleep(&ts, NULL);
	}
	if (retval) *retval = thread->result;
	return 0;
}
static inline pthread_t pthread_self(void) {
	static struct __jacl_pthread fallback = {0};
	__jacl_pthread_register_self();
	if (__jacl_pthread_self_key >= MAX_TLS_KEYS) return &fallback;
	pthread_t self = (pthread_t)__jacl_pthread_tls_values[__jacl_pthread_self_key];
	return self ? self : &fallback;
}
static inline void pthread_exit(void *retval) {
	pthread_t self = pthread_self();
	int is_detached = 0;
	volatile int elapsed_ms = 0;
	struct timespec ts = {0, 100000};
	if (self) {
		self->result = retval;
		atomic_store(&self->finished, 1);
		is_detached = atomic_load_explicit(&self->detached, memory_order_relaxed);
	}
	while (is_detached && elapsed_ms < PTHREAD_DETACHED_WAIT) {
		nanosleep(&ts, NULL);
		elapsed_ms += 100;
	}
}
static inline int pthread_equal(pthread_t t1, pthread_t t2) { return t1 == t2; }
static inline int pthread_detach(pthread_t thread) {
	if (!thread) return EINVAL;
	__jacl_pthread_register_self();
	int expected = 0;
	if (!atomic_compare_exchange_strong(&thread->detached, &expected, 1)) return EINVAL;
	return 0;
}
static inline int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
	if (!mutex) return EINVAL;
	int type = (attr && attr->type) ? attr->type : PTHREAD_MUTEX_NORMAL;
	if (type != PTHREAD_MUTEX_NORMAL && type != PTHREAD_MUTEX_RECURSIVE && type != PTHREAD_MUTEX_ERRORCHECK && type != PTHREAD_MUTEX_ROBUST) return EINVAL;
	atomic_store(&mutex->futex, 0);
	mutex->owner = 0; mutex->type = type; mutex->recursive_count = 0;
	return 0;
}
static inline int pthread_mutex_lock(pthread_mutex_t *mutex) {
	if (!mutex) return EINVAL;
	if (mutex->type == PTHREAD_MUTEX_ERRORCHECK) {
		pid_t tid = __jacl_pthread_gettid();
		if (mutex->owner == tid) return EDEADLK;
	}
	if (mutex->type == PTHREAD_MUTEX_RECURSIVE) {
		pid_t tid = __jacl_pthread_gettid();
		if (mutex->owner == tid) { mutex->recursive_count++; return 0; }
	}
	int expected = 0, backoff = 1000;
	while (!atomic_compare_exchange_weak(&mutex->futex, &expected, 1)) {
		expected = 0;
		if (__jacl_pthread_futex_wait(&mutex->futex, 1) != 0) {
			struct timespec ts = {0, backoff}; nanosleep(&ts, NULL);
			if (backoff < 1000000) backoff *= 2;
		}
	}
	mutex->owner = __jacl_pthread_gettid();
	if (mutex->type == PTHREAD_MUTEX_RECURSIVE) mutex->recursive_count = 1;
	return 0;
}
static inline int pthread_mutex_trylock(pthread_mutex_t *mutex) {
	if (!mutex) return EINVAL;
	if (mutex->type == PTHREAD_MUTEX_ERRORCHECK) { pid_t tid = __jacl_pthread_gettid(); if (mutex->owner == tid) return EDEADLK; }
	if (mutex->type == PTHREAD_MUTEX_RECURSIVE) { pid_t tid = __jacl_pthread_gettid(); if (mutex->owner == tid) { mutex->recursive_count++; return 0; } }
	int expected = 0;
	if (atomic_compare_exchange_strong(&mutex->futex, &expected, 1)) {
		mutex->owner = __jacl_pthread_gettid();
		if (mutex->type == PTHREAD_MUTEX_RECURSIVE) mutex->recursive_count = 1;
		return 0;
	}
	return EBUSY;
}
static inline int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *ts) {
	if (!mutex || !ts) return EINVAL;
	if (ts->tv_nsec < 0 || ts->tv_nsec >= 1000000000) return EINVAL;
	if (mutex->type == PTHREAD_MUTEX_ERRORCHECK) { pid_t tid = __jacl_pthread_gettid(); if (mutex->owner == tid) return EDEADLK; }
	if (mutex->type == PTHREAD_MUTEX_RECURSIVE) { pid_t tid = __jacl_pthread_gettid(); if (mutex->owner == tid) { mutex->recursive_count++; return 0; } }
	struct timespec end, now;
	clock_gettime(CLOCK_REALTIME, &end);
	end.tv_sec += ts->tv_sec; end.tv_nsec += ts->tv_nsec;
	if (end.tv_nsec >= 1000000000) { end.tv_sec++; end.tv_nsec -= 1000000000; }
	int expected = 0, backoff = 1000;
	while (!atomic_compare_exchange_weak(&mutex->futex, &expected, 1)) {
		expected = 0;
		clock_gettime(CLOCK_REALTIME, &now);
		if (now.tv_sec > end.tv_sec || (now.tv_sec == end.tv_sec && now.tv_nsec >= end.tv_nsec)) return ETIMEDOUT;
		struct timespec ts_wait = {0, backoff}; nanosleep(&ts_wait, NULL);
		if (backoff < 1000000) backoff *= 2;
	}
	mutex->owner = __jacl_pthread_gettid();
	if (mutex->type == PTHREAD_MUTEX_RECURSIVE) mutex->recursive_count = 1;
	return 0;
}
static inline int pthread_mutex_clocklock(pthread_mutex_t *mutex, clockid_t clock_id, const struct timespec *abstime) {
	(void)clock_id;
	return pthread_mutex_timedlock(mutex, abstime);
}
static inline int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	if (!mutex) return EINVAL;
	if (mutex->type == PTHREAD_MUTEX_RECURSIVE) {
		if (mutex->recursive_count > 1) { mutex->recursive_count--; return 0; }
		mutex->owner = 0; mutex->recursive_count = 0;
	} else {
		mutex->owner = 0;
	}
	atomic_store(&mutex->futex, 0);
	__jacl_pthread_futex_wake(&mutex->futex, 1);
	return 0;
}
static inline int pthread_mutex_destroy(pthread_mutex_t *mutex) {
	if (!mutex) return EINVAL;
	if (!__jacl_pthread_mutex_is_valid(mutex)) return EINVAL;
	if (atomic_load(&mutex->futex) != 0) return EBUSY;
	return 0;
}
static inline int pthread_mutex_consistent(pthread_mutex_t *mutex) {
	if (!mutex || mutex->type != PTHREAD_MUTEX_ROBUST) return EINVAL;
	return 0;
}
static inline int pthread_mutexattr_init(pthread_mutexattr_t *attr) {
	if (!attr) return EINVAL;
	attr->type = PTHREAD_MUTEX_NORMAL;
	attr->robust = 0;
	return 0;
}
static inline int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
	if (!attr) return EINVAL;
	return 0;
}
static inline int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
	if (!attr) return EINVAL;
	attr->type = type;
	return 0;
}
static inline int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *type) {
	if (!attr || !type) return EINVAL;
	*type = attr->type;
	return 0;
}
static inline int pthread_mutexattr_setrobust(pthread_mutexattr_t *attr, int robust) {
	if (!attr) return EINVAL;
	attr->robust = robust;
	return 0;
}
static inline int pthread_mutexattr_getrobust(const pthread_mutexattr_t *attr, int *robust) {
	if (!attr || !robust) return EINVAL;
	*robust = attr->robust;
	return 0;
}
static inline int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
	(void)attr; if (!cond) return EINVAL;
	atomic_store(&cond->futex, 0); atomic_store(&cond->waiters, 0); return 0;
}
static inline int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
	if (!cond || !mutex) return EINVAL;
	atomic_fetch_add(&cond->waiters, 1);
	int old = atomic_load(&cond->futex);
	pthread_mutex_unlock(mutex);
	if (__jacl_pthread_futex_wait(&cond->futex, old) != 0) {
		while (atomic_load(&cond->futex) == old) {
			struct timespec ts = {0, 1000000};
			nanosleep(&ts, NULL);
		}
	}
	pthread_mutex_lock(mutex);
	atomic_fetch_sub(&cond->waiters, 1);
	return 0;
}
static inline int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime) {
	if (!cond || !mutex || !abstime) return EINVAL;
	atomic_fetch_add(&cond->waiters, 1);
	int old = atomic_load(&cond->futex);
	pthread_mutex_unlock(mutex);
	struct timespec now;
	while (atomic_load(&cond->futex) == old) {
		clock_gettime(CLOCK_REALTIME, &now);
		if (now.tv_sec > abstime->tv_sec || (now.tv_sec == abstime->tv_sec && now.tv_nsec >= abstime->tv_nsec)) {
			pthread_mutex_lock(mutex);
			atomic_fetch_sub(&cond->waiters, 1);
			return ETIMEDOUT;
		}
		struct timespec ts = {0, 1000000}; nanosleep(&ts, NULL);
	}
	pthread_mutex_lock(mutex);
	atomic_fetch_sub(&cond->waiters, 1);
	return 0;
}
static inline int pthread_cond_clockwait(pthread_cond_t *cond, pthread_mutex_t *mutex, clockid_t clock_id, const struct timespec *abstime) {
	(void)clock_id;
	return pthread_cond_timedwait(cond, mutex, abstime);
}
static inline int pthread_cond_signal(pthread_cond_t *cond) {
	if (!cond) return EINVAL;
	if (atomic_load(&cond->waiters) > 0) {
		atomic_fetch_add(&cond->futex, 1);
		__jacl_pthread_futex_wake(&cond->futex, 1);
	}
	return 0;
}
static inline int pthread_cond_broadcast(pthread_cond_t *cond) {
	if (!cond) return EINVAL;
	int w = atomic_load(&cond->waiters);
	if (w > 0) { atomic_fetch_add(&cond->futex, 1); __jacl_pthread_futex_wake(&cond->futex, w); }
	return 0;
}
static inline int pthread_cond_destroy(pthread_cond_t *cond) { if (!cond) return EINVAL; return 0; }
static inline int pthread_key_create(pthread_key_t *key, void (*destructor)(void*)) {
	if (!key) return EINVAL;
	__jacl_pthread_init_keys();
	unsigned int k = atomic_fetch_add(&__jacl_pthread_tls_key_counter, 1);
	if (k >= MAX_TLS_KEYS) return EAGAIN;
	*key = k;
	__jacl_pthread_key_destructors[k] = destructor;
	return 0;
}
static inline int pthread_key_delete(pthread_key_t key) {
	if (key >= MAX_TLS_KEYS) return EINVAL;
	__jacl_pthread_key_destructors[key] = NULL;
	return 0;
}
static inline int pthread_setspecific(pthread_key_t key, const void *value) {
	if (key >= MAX_TLS_KEYS || !__jacl_pthread_tls_values) return EINVAL;
	__jacl_pthread_tls_values[key] = (void *)value;
	return 0;
}
static inline void *pthread_getspecific(pthread_key_t key) {
	if (key >= MAX_TLS_KEYS || !__jacl_pthread_tls_values) return NULL;
	return __jacl_pthread_tls_values[key];
}
static inline int pthread_spin_init(pthread_spinlock_t *lock, int pshared) {
	(void)pshared;
	if (!lock) return EINVAL;
	atomic_store(&lock->lock, PTHREAD_SPIN_UNLOCKED);
	return 0;
}
static inline int pthread_spin_destroy(pthread_spinlock_t *lock) { if (!lock) return EINVAL; return 0; }
static inline int pthread_spin_lock(pthread_spinlock_t *lock) {
	if (!lock) return EINVAL;
	while (!atomic_compare_exchange_weak(&lock->lock, &(int){PTHREAD_SPIN_UNLOCKED}, PTHREAD_SPIN_LOCKED)) sched_yield();
	return 0;
}
static inline int pthread_spin_trylock(pthread_spinlock_t *lock) {
	if (!lock) return EINVAL;
	int expected = PTHREAD_SPIN_UNLOCKED;
	return atomic_compare_exchange_strong(&lock->lock, &expected, PTHREAD_SPIN_LOCKED) ? 0 : EBUSY;
}
static inline int pthread_spin_unlock(pthread_spinlock_t *lock) {
	if (!lock) return EINVAL;
	atomic_store(&lock->lock, PTHREAD_SPIN_UNLOCKED); return 0;
}
static inline int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr) {
	(void)attr; if (!rwlock) return EINVAL;
	atomic_store(&rwlock->readers, 0); atomic_store(&rwlock->writers, 0);
	atomic_store(&rwlock->write_waiters, 0); rwlock->writer_tid = 0; return 0;
}
static inline int pthread_rwlock_destroy(pthread_rwlock_t *rwlock) {
	if (!rwlock) return EINVAL;
	if (atomic_load(&rwlock->readers) != 0 || atomic_load(&rwlock->writers) != 0) return EBUSY;
	return 0;
}
static inline int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock) {
	if (!rwlock) return EINVAL;
	while (atomic_load(&rwlock->writers) != 0 || atomic_load(&rwlock->write_waiters) > 0) __jacl_pthread_futex_wait(&rwlock->writers, 1);
	atomic_fetch_add(&rwlock->readers, 1); return 0;
}
static inline int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock) {
	if (!rwlock) return EINVAL;
	atomic_fetch_add(&rwlock->write_waiters, 1);
	while (atomic_load(&rwlock->readers) != 0 || atomic_load(&rwlock->writers) != 0) __jacl_pthread_futex_wait(&rwlock->readers, 0);
	atomic_fetch_sub(&rwlock->write_waiters, 1);
	atomic_store(&rwlock->writers, 1);
	rwlock->writer_tid = __jacl_pthread_gettid(); return 0;
}
static inline int pthread_rwlock_unlock(pthread_rwlock_t *rwlock) {
	if (!rwlock) return EINVAL;
	if (atomic_load(&rwlock->writers) == 1 && rwlock->writer_tid == __jacl_pthread_gettid()) {
		atomic_store(&rwlock->writers, 0); rwlock->writer_tid = 0;
		__jacl_pthread_futex_wake(&rwlock->readers, 1);
		__jacl_pthread_futex_wake(&rwlock->writers, 1);
	} else {
		if (atomic_fetch_sub(&rwlock->readers, 1) == 1) __jacl_pthread_futex_wake(&rwlock->readers, 1);
	}
	return 0;
}
static inline int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock) {
	if (!rwlock) return EINVAL;
	if (atomic_load(&rwlock->writers) != 0 || atomic_load(&rwlock->write_waiters) > 0) return EBUSY;
	atomic_fetch_add(&rwlock->readers, 1);
	return 0;
}
static inline int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock) {
	if (!rwlock) return EINVAL;
	if (atomic_load(&rwlock->readers) != 0 || atomic_load(&rwlock->writers) != 0) return EBUSY;
	int expected = 0;
	if (!atomic_compare_exchange_strong(&rwlock->writers, &expected, 1)) return EBUSY;
	rwlock->writer_tid = __jacl_pthread_gettid();
	return 0;
}
static inline int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abstime) {
	if (!rwlock || !abstime) return EINVAL;
	struct timespec now;
	while (atomic_load(&rwlock->writers) != 0 || atomic_load(&rwlock->write_waiters) > 0) {
		clock_gettime(CLOCK_REALTIME, &now);
		if (now.tv_sec > abstime->tv_sec || (now.tv_sec == abstime->tv_sec && now.tv_nsec >= abstime->tv_nsec))
			return ETIMEDOUT;
		__jacl_pthread_futex_wait(&rwlock->writers, 1);
	}
	atomic_fetch_add(&rwlock->readers, 1);
	return 0;
}
static inline int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abstime) {
	if (!rwlock || !abstime) return EINVAL;
	struct timespec now;
	atomic_fetch_add(&rwlock->write_waiters, 1);
	while (atomic_load(&rwlock->readers) != 0 || atomic_load(&rwlock->writers) != 0) {
		clock_gettime(CLOCK_REALTIME, &now);
		if (now.tv_sec > abstime->tv_sec || (now.tv_sec == abstime->tv_sec && now.tv_nsec >= abstime->tv_nsec)) {
			atomic_fetch_sub(&rwlock->write_waiters, 1);
			return ETIMEDOUT;
		}
		__jacl_pthread_futex_wait(&rwlock->readers, 0);
	}
	atomic_fetch_sub(&rwlock->write_waiters, 1);
	atomic_store(&rwlock->writers, 1);
	rwlock->writer_tid = __jacl_pthread_gettid();
	return 0;
}
static inline int pthread_rwlockattr_init(pthread_rwlockattr_t *attr) { if (!attr) return EINVAL; attr->pshared = PTHREAD_PROCESS_PRIVATE; return 0; }
static inline int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr) { if (!attr) return EINVAL; return 0; }
static inline int pthread_barrier_init(pthread_barrier_t *barrier, const pthread_barrierattr_t *attr, unsigned count) {
	(void)attr;
	if (!barrier || count == 0) return EINVAL;
	atomic_store(&barrier->count, 0); atomic_store(&barrier->phase, 0);
	atomic_store(&barrier->waiters, 0); barrier->limit = count;
	return 0;
}
static inline int pthread_barrier_destroy(pthread_barrier_t *barrier) {
	if (!barrier) return EINVAL;
	if (atomic_load(&barrier->count) < barrier->limit) return EBUSY;
	return 0;
}
static inline int pthread_barrier_wait(pthread_barrier_t *barrier) {
	if (!barrier) return EINVAL;
	int phase = atomic_load(&barrier->phase), my_count = atomic_fetch_add(&barrier->count, 1) + 1;
	if (my_count < barrier->limit) {
		atomic_fetch_add(&barrier->waiters, 1);
		while (atomic_load(&barrier->phase) == phase) __jacl_pthread_futex_wait(&barrier->phase, phase);
		return 0;
	} else {
		atomic_store(&barrier->count, 0);
		atomic_fetch_add(&barrier->phase, 1);
		__jacl_pthread_futex_wake(&barrier->phase, barrier->limit - 1);
		return PTHREAD_BARRIER_SERIAL_THREAD;
	}
}
static inline int pthread_barrierattr_init(pthread_barrierattr_t *attr) { if (!attr) return EINVAL; attr->pshared = PTHREAD_PROCESS_PRIVATE; return 0; }
static inline int pthread_barrierattr_destroy(pthread_barrierattr_t *attr) { if (!attr) return EINVAL; return 0; }
static inline void pthread_testcancel(void) {
	pthread_t self = pthread_self();
	if (!self) return;
	int was_enabled = atomic_load(&self->cancel_state);
	if (was_enabled != PTHREAD_CANCEL_ENABLE) return;
	__jacl_cleanup_node_t *item = self->cleanup_stack;
	while (item) {
		__jacl_cleanup_node_t *next = item->next;
		item->routine(item->arg);
		item->routine = NULL; item->arg = NULL;
		free(item); item = next;
	}
	self->cleanup_stack = NULL;
	atomic_store(&self->canceled, 0);
	pthread_exit(PTHREAD_CANCELED);
}
static inline int pthread_cancel(pthread_t thread) {
	if (!thread) return ESRCH;
	atomic_store(&thread->canceled, 1);
	__jacl_pthread_futex_wake(&thread->finished, 1);
	return 0;
}
static inline int pthread_setcancelstate(int state, int *oldstate) {
	pthread_t self = pthread_self();
	if (!self || (state != PTHREAD_CANCEL_ENABLE && state != PTHREAD_CANCEL_DISABLE)) return EINVAL;
	if (oldstate) *oldstate = atomic_load(&self->cancel_state);
	atomic_store(&self->cancel_state, state);
	if (state == PTHREAD_CANCEL_ENABLE) pthread_testcancel();
	return 0;
}
static inline int pthread_setcanceltype(int type, int *oldtype) {
	pthread_t self = pthread_self();
	if (!self || (type != PTHREAD_CANCEL_DEFERRED && type != PTHREAD_CANCEL_ASYNCHRONOUS)) return EINVAL;
	if (oldtype) *oldtype = atomic_load(&self->cancel_type);
	atomic_store(&self->cancel_type, type);
	return 0;
}

#define pthread_cleanup_push(routine, arg) \
	do { \
		pthread_t _self = pthread_self(); \
		if (_self) { \
			__jacl_cleanup_node_t _cb_item; \
			_cb_item.routine = (routine); \
			_cb_item.arg = (arg); \
			_cb_item.next = _self->cleanup_stack; \
			_self->cleanup_stack = &_cb_item;

#define pthread_cleanup_pop(execute) \
			_self->cleanup_stack = _cb_item.next; \
			if ((execute) && _cb_item.routine) _cb_item.routine(_cb_item.arg); \
		} \
	} while (0)

static inline int pthread_once(pthread_once_t *once_control, void (*init_routine)(void)) {
	if (!once_control || !init_routine) return EINVAL;
	int expected = 0;
	if (atomic_compare_exchange_strong(&once_control->done, &expected, 1)) init_routine();
	return 0;
}
static inline int pthread_attr_init(pthread_attr_t *attr) { if (!attr) return EINVAL; attr->detached = 0; attr->stack_size = 0; attr->stack_addr = NULL; return 0; }
static inline int pthread_attr_destroy(pthread_attr_t *attr) { if (!attr) return EINVAL; if (!__jacl_pthread_attr_is_valid(attr)) return EINVAL; attr->detached = -1; return 0; }
static inline int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
	if (!attr) return EINVAL;
	if (!__jacl_pthread_attr_is_valid(attr)) return EINVAL;
	if (detachstate != PTHREAD_CREATE_JOINABLE && detachstate != PTHREAD_CREATE_DETACHED) return EINVAL;
	attr->detached = (detachstate == PTHREAD_CREATE_DETACHED);
	return 0;
}
static inline int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
	if (!attr) return EINVAL;
	if (!__jacl_pthread_attr_is_valid(attr)) return EINVAL;
	if (stacksize == 0) return EINVAL;
	attr->stack_size = stacksize;
	return 0;
}
static inline int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) {
	if (!attr || !detachstate) return EINVAL;
	*detachstate = attr->detached;
	return 0;
}
static inline int pthread_condattr_init(pthread_condattr_t *attr) { if (!attr) return EINVAL; attr->dummy = 0; return 0; }
static inline int pthread_condattr_destroy(pthread_condattr_t *attr) { if (!attr) return EINVAL; return 0; }
static inline int pthread_sigmask(int how, const sigset_t *set, sigset_t *oldset) {
	#if JACL_HAS_POSIX
		if (sigprocmask(how, set, oldset) == 0) return 0;
		return (errno != 0) ? errno : EINVAL;
	#else
		(void)how; (void)set; (void)oldset; return ENOSYS;
	#endif
}

#else /* PTHREAD_DUMMY */

#define pthread_create(t,a,s,arg) (ENOSYS)
#define pthread_join(t,r) (ENOSYS)
#define pthread_detach(t) (ENOSYS)
#define pthread_exit(r) do { exit(0); } while(0)
#define pthread_self() ((pthread_t){0})
#define pthread_equal(t1,t2) (1)
#define pthread_mutex_init(m,a) (ENOSYS)
#define pthread_mutex_lock(m) (ENOSYS)
#define pthread_mutex_trylock(m) (ENOSYS)
#define pthread_mutex_timedlock(m,t) (ENOSYS)
#define pthread_mutex_clocklock(m,k,t) (ENOSYS)
#define pthread_mutex_unlock(m) (ENOSYS)
#define pthread_mutex_destroy(m) (ENOSYS)
#define pthread_mutex_consistent(m) (ENOSYS)
#define pthread_mutexattr_init(a) (ENOSYS)
#define pthread_mutexattr_destroy(a) (ENOSYS)
#define pthread_mutexattr_settype(a,t) (ENOSYS)
#define pthread_mutexattr_gettype(a,t) (ENOSYS)
#define pthread_mutexattr_setrobust(a,r) (ENOSYS)
#define pthread_mutexattr_getrobust(a,r) (ENOSYS)
#define pthread_cond_init(c,a) (ENOSYS)
#define pthread_cond_wait(c,m) (ENOSYS)
#define pthread_cond_timedwait(c,m,t) (ENOSYS)
#define pthread_cond_clockwait(c,m,k,t) (ENOSYS)
#define pthread_cond_signal(c) (ENOSYS)
#define pthread_cond_broadcast(c) (ENOSYS)
#define pthread_cond_destroy(c) (ENOSYS)
#define pthread_once(o,i) (ENOSYS)
#define pthread_key_create(k,d) (ENOSYS)
#define pthread_key_delete(k) (ENOSYS)
#define pthread_setspecific(k,v) (ENOSYS)
#define pthread_getspecific(k) (NULL)
#define pthread_sigmask(h,s,o) (ENOSYS)
#define pthread_attr_init(a) (ENOSYS)
#define pthread_attr_destroy(a) (ENOSYS)
#define pthread_attr_setdetachstate(a,s) (ENOSYS)
#define pthread_attr_setstacksize(a,s) (ENOSYS)
#define pthread_attr_getdetachstate(a,s) (ENOSYS)
#define pthread_condattr_init(a) (ENOSYS)
#define pthread_condattr_destroy(a) (ENOSYS)
#define pthread_spin_init(l,p) (ENOSYS)
#define pthread_spin_destroy(l) (ENOSYS)
#define pthread_spin_lock(l) (ENOSYS)
#define pthread_spin_trylock(l) (ENOSYS)
#define pthread_spin_unlock(l) (ENOSYS)
#define pthread_rwlock_init(r,a) (ENOSYS)
#define pthread_rwlock_destroy(r) (ENOSYS)
#define pthread_rwlock_rdlock(r) (ENOSYS)
#define pthread_rwlock_wrlock(r) (ENOSYS)
#define pthread_rwlock_unlock(r) (ENOSYS)
#define pthread_rwlock_tryrdlock(r) (ENOSYS)
#define pthread_rwlock_trywrlock(r) (ENOSYS)
#define pthread_rwlock_timedrdlock(r,t) (ENOSYS)
#define pthread_rwlock_timedwrlock(r,t) (ENOSYS)
#define pthread_rwlockattr_init(a) (ENOSYS)
#define pthread_rwlockattr_destroy(a) (ENOSYS)
#define pthread_barrier_init(b,a,c) (ENOSYS)
#define pthread_barrier_destroy(b) (ENOSYS)
#define pthread_barrier_wait(b) (ENOSYS)
#define pthread_barrierattr_init(a) (ENOSYS)
#define pthread_barrierattr_destroy(a) (ENOSYS)
#define pthread_cancel(t) (ENOSYS)
#define pthread_setcancelstate(s,o) (ENOSYS)
#define pthread_setcanceltype(t,o) (ENOSYS)
#define pthread_testcancel() do{}while(0)
#define pthread_cleanup_push(r,a) do{
#define pthread_cleanup_pop(e) }while(0)

#endif /* PTHREADS_? */

#ifdef __cplusplus
}
#endif

#endif /* _PTHREAD_H */
