/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef PTHREAD_H
#define PTHREAD_H
#pragma once

#include <config.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>

#if JACL_HAS_C11
	#include <stdatomics.h>
#else
	#define ATOMIC_VAR_INIT(x) (x)
	#define atomic_load(ptr) (*(ptr))
	#define atomic_store(ptr, val) (*(ptr) = (val))
	#define atomic_fetch_add(ptr, val) ((*(ptr))++)
	#define atomic_fetch_sub(ptr, val) ((*(ptr))--)
	#define atomic_compare_exchange_strong(ptr, expected, desired) \
		((*(ptr) == *(expected)) ? (*(ptr) = (desired), 1) : (*(expected) = *(ptr), 0))
	#define atomic_compare_exchange_weak atomic_compare_exchange_strong
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Platform detection and includes */
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
	#define pthread_syscall syscall
#else
	#define PTHREAD_DUMMY 1
#endif

#if JACL_OS_LINUX
	#include <sched.h>
	#include <sys/futex.h>
#endif

/* ================================================================ */
/* COMMON TYPES AND CONSTANTS                                       */
/* ================================================================ */

/* Thread attributes */
typedef struct {
	int detached;
	size_t stack_size;
	void *stack_addr;
} pthread_attr_t;

/* Mutex attributes */
typedef struct {
	int type;
} pthread_mutexattr_t;

/* Condition attributes */
typedef struct {
	int dummy;
} pthread_condattr_t;

/* Once flag */
typedef struct {
	_Atomic int done;
} pthread_once_t;


/* platform dependent types */
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

typedef struct {
	CONDITION_VARIABLE cv;
} pthread_cond_t;

typedef DWORD pthread_key_t;

#elif PTHREAD_POSIX

typedef struct {
	pid_t tid;
	void *stack;
	size_t stack_size;
	void *result;
	_Atomic int finished;
	_Atomic int detached;
	_Atomic int joined;
} pthread_t;

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

#else /* PTHREAD_DUMMY */

typedef struct { int dummy; } pthread_t;
typedef struct { int dummy; } pthread_mutex_t;
typedef struct { int dummy; } pthread_cond_t;
typedef int pthread_key_t;

#endif /* platform dependent types */

/* Constants */
#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1
#define PTHREAD_MUTEX_NORMAL    0
#define PTHREAD_MUTEX_RECURSIVE 1
#define PTHREAD_MUTEX_ERRORCHECK 2

#define PTHREAD_ONCE_INIT {ATOMIC_VAR_INIT(0)}

/* ================================================================ */
/* WINDOWS IMPLEMENTATION                                           */
/* ================================================================ */

#if PTHREAD_WIN32

/* Thread functions */
struct win_thread_arg {
	void *(*start_routine)(void *);
	void *arg;
	pthread_t *thread_ptr;
};

static unsigned __stdcall win_thread_start(void *arg) {
	struct win_thread_arg *ta = (struct win_thread_arg *)arg;
	void *result = ta->start_routine(ta->arg);
	ta->thread_ptr->result = result;
	free(ta);
	return (unsigned)(uintptr_t)result;
}

static inline int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                                void *(*start_routine)(void *), void *arg) {
	if (!thread || !start_routine) return EINVAL;

	struct win_thread_arg *ta = malloc(sizeof(*ta));
	if (!ta) return ENOMEM;

	ta->start_routine = start_routine;
	ta->arg = arg;
	ta->thread_ptr = thread;
	thread->result = NULL;

	thread->handle = (HANDLE)_beginthreadex(NULL,
		attr ? (unsigned)attr->stack_size : 0,
		win_thread_start, ta, 0, &thread->id);

	if (!thread->handle) {
		free(ta);
		return EAGAIN;
	}
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

static inline void pthread_exit(void *retval) {
	_endthreadex((unsigned)(uintptr_t)retval);
}

static inline pthread_t pthread_self(void) {
	pthread_t self = {0};
	self.handle = GetCurrentThread();
	self.id = GetCurrentThreadId();
	return self;
}

static inline int pthread_equal(pthread_t t1, pthread_t t2) {
	return t1.id == t2.id;
}

static inline int pthread_detach(pthread_t thread) {
	return CloseHandle(thread.handle) ? 0 : EINVAL;
}

/* Mutex functions */
static inline int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
	if (!mutex) return EINVAL;
	InitializeCriticalSection(&mutex->cs);
	mutex->type = (attr && attr->type) ? attr->type : PTHREAD_MUTEX_NORMAL;
	return 0;
}

static inline int pthread_mutex_lock(pthread_mutex_t *mutex) {
	if (!mutex) return EINVAL;
	EnterCriticalSection(&mutex->cs);
	return 0;
}

static inline int pthread_mutex_trylock(pthread_mutex_t *mutex) {
	if (!mutex) return EINVAL;
	return TryEnterCriticalSection(&mutex->cs) ? 0 : EBUSY;
}

static inline int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	if (!mutex) return EINVAL;
	LeaveCriticalSection(&mutex->cs);
	return 0;
}

static inline int pthread_mutex_destroy(pthread_mutex_t *mutex) {
	if (!mutex) return EINVAL;
	DeleteCriticalSection(&mutex->cs);
	return 0;
}

/* Condition variables */
static inline int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
	(void)attr;
	if (!cond) return EINVAL;
	InitializeConditionVariable(&cond->cv);
	return 0;
}

static inline int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
	if (!cond || !mutex) return EINVAL;
	return SleepConditionVariableCS(&cond->cv, &mutex->cs, INFINITE) ? 0 : EINVAL;
}

static inline int pthread_cond_signal(pthread_cond_t *cond) {
	if (!cond) return EINVAL;
	WakeConditionVariable(&cond->cv);
	return 0;
}

static inline int pthread_cond_broadcast(pthread_cond_t *cond) {
	if (!cond) return EINVAL;
	WakeAllConditionVariable(&cond->cv);
	return 0;
}

static inline int pthread_cond_destroy(pthread_cond_t *cond) {
	(void)cond;
	return 0;
}

/* Thread-local storage */
static inline int pthread_key_create(pthread_key_t *key, void (*destructor)(void*)) {
	(void)destructor;
	if (!key) return EINVAL;
	*key = TlsAlloc();
	return (*key != TLS_OUT_OF_INDEXES) ? 0 : EAGAIN;
}

static inline int pthread_key_delete(pthread_key_t key) {
	return TlsFree(key) ? 0 : EINVAL;
}

static inline int pthread_setspecific(pthread_key_t key, const void *value) {
	return TlsSetValue(key, (void*)value) ? 0 : EINVAL;
}

static inline void *pthread_getspecific(pthread_key_t key) {
	return TlsGetValue(key);
}

/* ================================================================ */
/* POSIX IMPLEMENTATION                                             */
/* ================================================================ */

#elif PTHREAD_POSIX

/* Helper functions */
static inline pid_t pthread_gettid(void) {
#if defined(SYS_gettid)
	return (pid_t)pthread_syscall(SYS_gettid);
#elif defined(__APPLE__)
	uint64_t tid;
	pthread_threadid_np(NULL, &tid);
	return (pid_t)tid;
#else
	return getpid();
#endif
}

static inline int pthread_futex_wait(void *addr, int val) {
#if defined(SYS_futex)
	return pthread_syscall(SYS_futex, addr, FUTEX_WAIT, val, NULL, NULL, 0);
#else
	(void)addr; (void)val;
	struct timespec ts = {0, 1000000}; /* 1ms */
	nanosleep(&ts, NULL);
	return 0;
#endif
}

static inline int pthread_futex_wake(void *addr, int num) {
#if defined(SYS_futex)
	return pthread_syscall(SYS_futex, addr, FUTEX_WAKE, num, NULL, NULL, 0);
#else
	(void)addr; (void)num;
	return 0;
#endif
}

static inline pid_t pthread_clone_thread(void *stack, size_t stack_size, int (*fn)(void *), void *arg) {
#if defined(SYS_clone)
	return (pid_t)pthread_syscall(SYS_clone,
		CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD,
		(char*)stack + stack_size, NULL, NULL, 0);
#else
	(void)stack; (void)stack_size;
	pid_t pid = fork();
	if (pid == 0) {
		fn(arg);
		exit(0);
	}
	return pid;
#endif
}

/* Thread functions */
struct posix_thread_arg {
	void *(*start_routine)(void *);
	void *arg;
	pthread_t *thread_ptr;
};

static int thread_entry(void *arg) {
	struct posix_thread_arg *ta = (struct posix_thread_arg *)arg;
	void *result = ta->start_routine(ta->arg);

	ta->thread_ptr->result = result;
	atomic_store(&ta->thread_ptr->finished, 1);
	free(ta);

#if defined(SYS_exit)
	pthread_syscall(SYS_exit, (long)(uintptr_t)result);
#else
	exit((int)(uintptr_t)result);
#endif
	return 0;
}

static inline int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                                void *(*start_routine)(void *), void *arg) {
	if (!thread || !start_routine) return EINVAL;

	struct posix_thread_arg *ta = malloc(sizeof(*ta));
	if (!ta) return ENOMEM;

	size_t stack_size = (attr && attr->stack_size) ? attr->stack_size : 1024*1024;
	void *stack = NULL;

#if JACL_OS_LINUX
	stack = mmap(NULL, stack_size, PROT_READ | PROT_WRITE,
	             MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
	if (stack == MAP_FAILED) stack = NULL;
#endif

	if (!stack) {
		stack = malloc(stack_size);
		if (!stack) {
			free(ta);
			return ENOMEM;
		}
	}

	ta->start_routine = start_routine;
	ta->arg = arg;
	ta->thread_ptr = thread;

	thread->stack = stack;
	thread->stack_size = stack_size;
	thread->result = NULL;
	atomic_store(&thread->finished, 0);
	atomic_store(&thread->detached, (attr && attr->detached) ? 1 : 0);
	atomic_store(&thread->joined, 0);

	pid_t tid = pthread_clone_thread(stack, stack_size, thread_entry, ta);

	if (tid > 0) {
		thread->tid = tid;
		return 0;
	} else {
#if JACL_OS_LINUX
		if (stack != malloc(stack_size)) {
			munmap(stack, stack_size);
		} else
#endif
		free(stack);
		free(ta);
		return EAGAIN;
	}
}

static inline int pthread_join(pthread_t thread, void **retval) {
	if (atomic_load(&thread.detached)) return EINVAL;

	int expected = 0;
	if (!atomic_compare_exchange_strong(&thread.joined, &expected, 1)) {
		return EINVAL;
	}

	while (!atomic_load(&thread.finished)) {
		struct timespec ts = {0, 1000000}; /* 1ms */
		nanosleep(&ts, NULL);
	}

	if (retval) *retval = thread.result;

#if JACL_OS_LINUX
	if (munmap(thread.stack, thread.stack_size) != 0) {
		free(thread.stack);
	}
#else
	free(thread.stack);
#endif

	return 0;
}

static inline void pthread_exit(void *retval) {
#if defined(SYS_exit)
	pthread_syscall(SYS_exit, (long)(uintptr_t)retval);
#else
	exit((int)(uintptr_t)retval);
#endif
}

static inline pthread_t pthread_self(void) {
	pthread_t self = {0};
	self.tid = pthread_gettid();
	return self;
}

static inline int pthread_equal(pthread_t t1, pthread_t t2) {
	return t1.tid == t2.tid;
}

static inline int pthread_detach(pthread_t thread) {
	atomic_store(&thread.detached, 1);
	return 0;
}

/* Mutex functions */
static inline int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
	if (!mutex) return EINVAL;
	atomic_store(&mutex->futex, 0);
	mutex->owner = 0;
	mutex->type = (attr && attr->type) ? attr->type : PTHREAD_MUTEX_NORMAL;
	mutex->recursive_count = 0;
	return 0;
}

static inline int pthread_mutex_lock(pthread_mutex_t *mutex) {
	if (!mutex) return EINVAL;

	if (mutex->type == PTHREAD_MUTEX_RECURSIVE) {
		pid_t current_tid = pthread_gettid();
		if (mutex->owner == current_tid) {
			mutex->recursive_count++;
			return 0;
		}
	}

	int expected = 0;
	int backoff = 1000;

	while (!atomic_compare_exchange_weak(&mutex->futex, &expected, 1)) {
		expected = 0;

		if (pthread_futex_wait(&mutex->futex, 1) != 0) {
			struct timespec ts = {0, backoff};
			nanosleep(&ts, NULL);
			if (backoff < 1000000) backoff *= 2;
		}
	}

	if (mutex->type == PTHREAD_MUTEX_RECURSIVE) {
		mutex->owner = pthread_gettid();
		mutex->recursive_count = 1;
	}

	return 0;
}

static inline int pthread_mutex_trylock(pthread_mutex_t *mutex) {
	if (!mutex) return EINVAL;
	int expected = 0;
	if (atomic_compare_exchange_strong(&mutex->futex, &expected, 1)) {
		if (mutex->type == PTHREAD_MUTEX_RECURSIVE) {
			mutex->owner = pthread_gettid();
			mutex->recursive_count = 1;
		}
		return 0;
	}
	return EBUSY;
}

static inline int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	if (!mutex) return EINVAL;

	if (mutex->type == PTHREAD_MUTEX_RECURSIVE) {
		if (mutex->recursive_count > 1) {
			mutex->recursive_count--;
			return 0;
		}
		mutex->owner = 0;
		mutex->recursive_count = 0;
	}

	atomic_store(&mutex->futex, 0);
	pthread_futex_wake(&mutex->futex, 1);
	return 0;
}

static inline int pthread_mutex_destroy(pthread_mutex_t *mutex) {
	(void)mutex;
	return 0;
}

/* Condition variables */
static inline int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
	(void)attr;
	if (!cond) return EINVAL;
	atomic_store(&cond->futex, 0);
	atomic_store(&cond->waiters, 0);
	return 0;
}

static inline int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
	if (!cond || !mutex) return EINVAL;

	atomic_fetch_add(&cond->waiters, 1);
	int old_futex = atomic_load(&cond->futex);

	pthread_mutex_unlock(mutex);

	if (pthread_futex_wait(&cond->futex, old_futex) != 0) {
		while (atomic_load(&cond->futex) == old_futex) {
			struct timespec ts = {0, 1000000}; /* 1ms */
			nanosleep(&ts, NULL);
		}
	}

	pthread_mutex_lock(mutex);
	atomic_fetch_sub(&cond->waiters, 1);
	return 0;
}

static inline int pthread_cond_signal(pthread_cond_t *cond) {
	if (!cond) return EINVAL;
	if (atomic_load(&cond->waiters) > 0) {
		atomic_fetch_add(&cond->futex, 1);
		pthread_futex_wake(&cond->futex, 1);
	}
	return 0;
}

static inline int pthread_cond_broadcast(pthread_cond_t *cond) {
	if (!cond) return EINVAL;
	int waiters = atomic_load(&cond->waiters);
	if (waiters > 0) {
		atomic_fetch_add(&cond->futex, 1);
		pthread_futex_wake(&cond->futex, waiters);
	}
	return 0;
}

static inline int pthread_cond_destroy(pthread_cond_t *cond) {
	(void)cond;
	return 0;
}

/* Thread-local storage */
#define MAX_TLS_KEYS 64
static _Atomic unsigned int tls_key_counter = ATOMIC_VAR_INIT(0);
static thread_local void *tls_values[MAX_TLS_KEYS];

static inline int pthread_key_create(pthread_key_t *key, void (*destructor)(void*)) {
	(void)destructor;
	if (!key) return EINVAL;
	*key = atomic_fetch_add(&tls_key_counter, 1);
	return (*key < MAX_TLS_KEYS) ? 0 : EAGAIN;
}

static inline int pthread_key_delete(pthread_key_t key) {
	(void)key;
	return 0;
}

static inline int pthread_setspecific(pthread_key_t key, const void *value) {
	if (key >= MAX_TLS_KEYS) return EINVAL;
	tls_values[key] = (void*)value;
	return 0;
}

static inline void *pthread_getspecific(pthread_key_t key) {
	return (key < MAX_TLS_KEYS) ? tls_values[key] : NULL;
}

/* ================================================================ */
/* DUMMY IMPLEMENTATION (Single-threaded stubs)                    */
/* ================================================================ */

#else /* PTHREAD_DUMMY */

/* Thread functions - all return ENOSYS */
static inline int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                                void *(*start_routine)(void *), void *arg) {
	(void)thread; (void)attr; (void)start_routine; (void)arg;
	return ENOSYS;
}

static inline int pthread_join(pthread_t thread, void **retval) {
	(void)thread; (void)retval;
	return ENOSYS;
}

static inline void pthread_exit(void *retval) {
	(void)retval;
	exit(0);
}

static inline pthread_t pthread_self(void) {
	pthread_t self = {0};
	return self;
}

static inline int pthread_equal(pthread_t t1, pthread_t t2) {
	(void)t1; (void)t2;
	return 1;
}

static inline int pthread_detach(pthread_t thread) {
	(void)thread;
	return ENOSYS;
}

/* Mutex functions - no-ops (no contention in single-threaded) */
static inline int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
	(void)mutex; (void)attr;
	return 0;
}

static inline int pthread_mutex_lock(pthread_mutex_t *mutex) {
	(void)mutex;
	return 0;
}

static inline int pthread_mutex_trylock(pthread_mutex_t *mutex) {
	(void)mutex;
	return 0;
}

static inline int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	(void)mutex;
	return 0;
}

static inline int pthread_mutex_destroy(pthread_mutex_t *mutex) {
	(void)mutex;
	return 0;
}

/* Condition variables - return ENOSYS */
static inline int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr) {
	(void)cond; (void)attr;
	return 0;
}

static inline int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex) {
	(void)cond; (void)mutex;
	return ENOSYS;
}

static inline int pthread_cond_signal(pthread_cond_t *cond) {
	(void)cond;
	return 0;
}

static inline int pthread_cond_broadcast(pthread_cond_t *cond) {
	(void)cond;
	return 0;
}

static inline int pthread_cond_destroy(pthread_cond_t *cond) {
	(void)cond;
	return 0;
}

/* Thread-local storage - return ENOSYS */
static inline int pthread_key_create(pthread_key_t *key, void (*destructor)(void*)) {
	(void)key; (void)destructor;
	return ENOSYS;
}

static inline int pthread_key_delete(pthread_key_t key) {
	(void)key;
	return 0;
}

static inline int pthread_setspecific(pthread_key_t key, const void *value) {
	(void)key; (void)value;
	return ENOSYS;
}

static inline void *pthread_getspecific(pthread_key_t key) {
	(void)key;
	return NULL;
}

#endif /* Platform implementations */

/* ================================================================ */
/* COMMON FUNCTIONS - Same across all platforms                    */
/* ================================================================ */

static inline int pthread_once(pthread_once_t *once_control, void (*init_routine)(void)) {
	if (!once_control || !init_routine) return EINVAL;
	int expected = 0;
	if (atomic_compare_exchange_strong(&once_control->done, &expected, 1)) {
		init_routine();
	}
	return 0;
}

/* Attribute functions */
static inline int pthread_attr_init(pthread_attr_t *attr) {
	if (!attr) return EINVAL;
	attr->detached = 0;
	attr->stack_size = 0;
	attr->stack_addr = NULL;
	return 0;
}

static inline int pthread_attr_destroy(pthread_attr_t *attr) {
	(void)attr;
	return 0;
}

static inline int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
	if (!attr) return EINVAL;
	attr->detached = (detachstate == PTHREAD_CREATE_DETACHED);
	return 0;
}

static inline int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
	if (!attr) return EINVAL;
	attr->stack_size = stacksize;
	return 0;
}

static inline int pthread_mutexattr_init(pthread_mutexattr_t *attr) {
	if (!attr) return EINVAL;
	attr->type = PTHREAD_MUTEX_NORMAL;
	return 0;
}

static inline int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
	(void)attr;
	return 0;
}

static inline int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
	if (!attr) return EINVAL;
	attr->type = type;
	return 0;
}

static inline int pthread_condattr_init(pthread_condattr_t *attr) {
	if (!attr) return EINVAL;
	attr->dummy = 0;
	return 0;
}

static inline int pthread_condattr_destroy(pthread_condattr_t *attr) {
	(void)attr;
	return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* PTHREAD_H */
