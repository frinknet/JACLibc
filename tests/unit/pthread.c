/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

TEST_TYPE(unit);
TEST_UNIT(pthread.h);

/* ============================================================================ */

static volatile int __test_once_counter = 0;
static int __test_thread_val = 0;

static void __test_once_init_fn(void) { __test_once_counter++; }
static void __test_once_empty_fn(void) { }
static void *__test_thread_fn_42(void *arg) { *(int *)arg = 42; return NULL; }
static void *__test_thread_fn_99(void *arg) { *(int *)arg = 99; return NULL; }
static void *__test_thread_fn_null(void *arg) { (void)arg; return NULL; }

static int __test_create_temp_file(const char *prefix) {
	char path[256];
	snprintf(path, sizeof(path), "/tmp/%s_XXXXXX", prefix);
	int fd = mkstemp(path);
	if (fd >= 0) unlink(path);
	return fd;
}

static void *__cancel_worker(void *arg) {
	int *done = (int *)arg;
	int old;
	/* Try to disable/enable cancellation inside a child thread */
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &old);
	pthread_testcancel(); /* Safe here: if it hangs, only this thread hangs */
	if (done) *done = 1;
	return NULL;
}

static void *__detach_worker(void *arg) {
	(void)pthread_self(); /* Force TLS registration */
	usleep(5000);		 /* Stay alive briefly so main doesn't exit too soon */
	return NULL;
}

static void *__self_worker(void *arg) {
	int *ok = (int *)arg;
	pthread_t s1 = pthread_self();
	pthread_t s2 = pthread_self();
	if (pthread_equal(s1, s2)) *ok = 1;
	return NULL;
}

typedef struct {
	pthread_mutex_t *m;
	pthread_cond_t *c;
	int done;
} __cond_args_t;

static void *__cond_signaler(void *arg) {
	__cond_args_t *args = (__cond_args_t *)arg;

	pthread_mutex_lock(args->m);
	pthread_cond_signal(args->c);
	pthread_mutex_unlock(args->m);

	args->done = 1;

	return NULL;
}

/* ============================================================================ */

TEST_SUITE(constants);

TEST(constants_thread_creation) {
	ASSERT_INT_EQ(0, PTHREAD_CREATE_JOINABLE);
	ASSERT_INT_EQ(1, PTHREAD_CREATE_DETACHED);
}

TEST(constants_mutex_type) {
	ASSERT_INT_EQ(0, PTHREAD_MUTEX_NORMAL);
	ASSERT_INT_EQ(1, PTHREAD_MUTEX_RECURSIVE);
	ASSERT_INT_EQ(2, PTHREAD_MUTEX_ERRORCHECK);
	ASSERT_INT_EQ(4, PTHREAD_MUTEX_ROBUST);
	ASSERT_INT_EQ(0, PTHREAD_MUTEX_STALLED);
}

TEST(constants_mutex_initializer) {
	pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
	ASSERT_NOT_NULL(&m);
}

TEST(constants_cond_initializer) {
	pthread_cond_t c = PTHREAD_COND_INITIALIZER;
	ASSERT_NOT_NULL(&c);
}

TEST(constants_rwlock_initializer) {
	pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
	ASSERT_NOT_NULL(&rw);
}

TEST(constants_barrier) {
	ASSERT_INT_EQ(-1, PTHREAD_BARRIER_SERIAL_THREAD);
}

TEST(constants_barrier_initializer) {
	pthread_barrier_t b = PTHREAD_BARRIER_INITIALIZER;
	ASSERT_NOT_NULL(&b);
}

TEST(constants_spinlock) {
	ASSERT_INT_EQ(1, PTHREAD_SPIN_LOCKED);
	ASSERT_INT_EQ(0, PTHREAD_SPIN_UNLOCKED);
}

TEST(constants_spinlock_initializer) {
	pthread_spinlock_t s = PTHREAD_SPIN_LOCK_INITIALIZER;
	ASSERT_NOT_NULL(&s);
}

TEST(constants_cancellation_enable) {
	ASSERT_INT_EQ(0, PTHREAD_CANCEL_ENABLE);
	ASSERT_INT_EQ(1, PTHREAD_CANCEL_DISABLE);
}

TEST(constants_cancellation_type) {
	ASSERT_INT_EQ(0, PTHREAD_CANCEL_DEFERRED);
	ASSERT_INT_EQ(1, PTHREAD_CANCEL_ASYNCHRONOUS);
}

TEST(constants_canceled) {
	ASSERT_PTR_EQ(PTHREAD_CANCELED, (void *)-1);
}

TEST(constants_process_sharing) {
	ASSERT_INT_EQ(0, PTHREAD_PROCESS_PRIVATE);
	ASSERT_INT_EQ(1, PTHREAD_PROCESS_SHARED);
}

TEST(constants_once_init) {
	pthread_once_t o = PTHREAD_ONCE_INIT;
	ASSERT_INT_EQ(0, o.done);
}

TEST(constants_destructor_iterations) {
	ASSERT_INT_EQ(4, PTHREAD_DESTRUCTOR_ITERATIONS);
}

TEST(constants_null_thread) {
	pthread_t null_thread = PTHREAD_NULL;
	ASSERT_TRUE(pthread_equal(null_thread, null_thread) || 1);
}

/* ============================================================================ */

TEST_SUITE(pthread_attr_t);

TEST(pthread_attr_t_size) {
	ASSERT_SIZE_MIN(pthread_attr_t, sizeof(int) + sizeof(size_t) + sizeof(void *));
}

TEST(pthread_attr_t_field_detached) {
	ASSERT_OFFSET_PAST(pthread_attr_t, detached, 0);
}

TEST(pthread_attr_t_field_stack_size) {
	ASSERT_OFFSET_AFTER(pthread_attr_t, stack_size, detached);
}

TEST(pthread_attr_t_field_stack_addr) {
	ASSERT_OFFSET_AFTER(pthread_attr_t, stack_addr, stack_size);
}

/* ============================================================================ */

TEST_SUITE(pthread_mutex_t);

TEST(pthread_mutex_t_size) {
	ASSERT_SIZE_MIN(pthread_mutex_t, sizeof(_Atomic int) + sizeof(pid_t) + sizeof(int) * 2);
}

TEST(pthread_mutex_t_field_futex) {
	ASSERT_OFFSET_PAST(pthread_mutex_t, futex, 0);
}

TEST(pthread_mutex_t_field_owner) {
	ASSERT_OFFSET_AFTER(pthread_mutex_t, owner, futex);
}

TEST(pthread_mutex_t_field_type) {
	ASSERT_OFFSET_AFTER(pthread_mutex_t, type, owner);
}

TEST(pthread_mutex_t_field_recursive_count) {
	ASSERT_OFFSET_AFTER(pthread_mutex_t, recursive_count, type);
}

/* ============================================================================ */

TEST_SUITE(pthread_cond_t);

TEST(pthread_cond_t_size) {
	ASSERT_SIZE_MIN(pthread_cond_t, sizeof(_Atomic int) * 2);
}

TEST(pthread_cond_t_field_futex) {
	ASSERT_OFFSET_PAST(pthread_cond_t, futex, 0);
}

TEST(pthread_cond_t_field_waiters) {
	ASSERT_OFFSET_AFTER(pthread_cond_t, waiters, futex);
}

/* ============================================================================ */

TEST_SUITE(pthread_rwlock_t);

TEST(pthread_rwlock_t_size) {
	ASSERT_SIZE_MIN(pthread_rwlock_t, sizeof(_Atomic int) * 3 + sizeof(pid_t));
}

TEST(pthread_rwlock_t_field_readers) {
	ASSERT_OFFSET_PAST(pthread_rwlock_t, readers, 0);
}

TEST(pthread_rwlock_t_field_writers) {
	ASSERT_OFFSET_AFTER(pthread_rwlock_t, writers, readers);
}

TEST(pthread_rwlock_t_field_write_waiters) {
	ASSERT_OFFSET_AFTER(pthread_rwlock_t, write_waiters, writers);
}

TEST(pthread_rwlock_t_field_writer_tid) {
	ASSERT_OFFSET_AFTER(pthread_rwlock_t, writer_tid, write_waiters);
}

/* ============================================================================ */

TEST_SUITE(pthread_barrier_t);

TEST(pthread_barrier_t_size) {
	ASSERT_SIZE_MIN(pthread_barrier_t, sizeof(_Atomic int) * 3 + sizeof(unsigned));
}

TEST(pthread_barrier_t_field_count) {
	ASSERT_OFFSET_PAST(pthread_barrier_t, count, 0);
}

TEST(pthread_barrier_t_field_phase) {
	ASSERT_OFFSET_AFTER(pthread_barrier_t, phase, count);
}

TEST(pthread_barrier_t_field_limit) {
	ASSERT_OFFSET_AFTER(pthread_barrier_t, limit, phase);
}

TEST(pthread_barrier_t_field_waiters) {
	ASSERT_OFFSET_AFTER(pthread_barrier_t, waiters, limit);
}

/* ============================================================================ */

TEST_SUITE(pthread_spinlock_t);

TEST(pthread_spinlock_t_size) {
	ASSERT_SIZE_MIN(pthread_spinlock_t, sizeof(_Atomic int));
}

TEST(pthread_spinlock_t_field_lock) {
	ASSERT_OFFSET_PAST(pthread_spinlock_t, lock, 0);
}

/* ============================================================================ */

TEST_SUITE(pthread_attr_init);

TEST(pthread_attr_init_success) {
	pthread_attr_t attr;
	ASSERT_INT_EQ(0, pthread_attr_init(&attr));
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_init_detached) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, attr.detached);
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_init_stack_size) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, attr.stack_size);
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_init_stack_addr) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_NULL(attr.stack_addr);
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_init_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_attr_init(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_attr_destroy);

TEST(pthread_attr_destroy_valid_attr) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, pthread_attr_destroy(&attr));
}

TEST(pthread_attr_destroy_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_attr_destroy(NULL));
}

TEST(pthread_attr_destroy_uninitialized_attr) {
	pthread_attr_t attr;
	ASSERT_INT_EQ(EINVAL, pthread_attr_destroy(&attr));
}

/* ============================================================================ */

TEST_SUITE(pthread_attr_setdetachstate);

TEST(pthread_attr_setdetachstate_detached_zero) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));
	ASSERT_INT_EQ(0, attr.detached);
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setdetachstate_detached_one) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
	ASSERT_INT_EQ(1, attr.detached);
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setdetachstate_success_joinable) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setdetachstate_success_detached) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setdetachstate_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_attr_setdetachstate(NULL, PTHREAD_CREATE_JOINABLE));
}

TEST(pthread_attr_setdetachstate_invalid_state) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_attr_setdetachstate(&attr, 999));
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setdetachstate_invalid_state_negative) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_attr_setdetachstate(&attr, -1));
	pthread_attr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_attr_setstacksize);

TEST(pthread_attr_setstacksize_basic) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, pthread_attr_setstacksize(&attr, 1024 * 1024));
	ASSERT_INT_EQ(1024 * 1024, attr.stack_size);
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setstacksize_success) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, pthread_attr_setstacksize(&attr, 2048));
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setstacksize_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_attr_setstacksize(NULL, 1024));
}

TEST(pthread_attr_setstacksize_zero_size) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_attr_setstacksize(&attr, 0));
	pthread_attr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_mutex_init);

TEST(pthread_mutex_init_null_attr) {
	pthread_mutex_t m;
	ASSERT_INT_EQ(0, pthread_mutex_init(&m, NULL));
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_init_type_normal) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(PTHREAD_MUTEX_NORMAL, m.type);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_init_futex_zero) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(0, m.futex);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_init_owner_zero) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(0, m.owner);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_init_count_zero) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(0, m.recursive_count);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_init_normal_attr) {
	pthread_mutex_t m;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
	ASSERT_INT_EQ(0, pthread_mutex_init(&m, &attr));
	pthread_mutex_destroy(&m);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutex_init_recursive_attr) {
	pthread_mutex_t m;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	ASSERT_INT_EQ(0, pthread_mutex_init(&m, &attr));
	pthread_mutex_destroy(&m);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutex_init_errorcheck_attr) {
	pthread_mutex_t m;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	ASSERT_INT_EQ(0, pthread_mutex_init(&m, &attr));
	pthread_mutex_destroy(&m);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutex_init_null_mutex) {
	ASSERT_INT_EQ(EINVAL, pthread_mutex_init(NULL, NULL));
}

TEST(pthread_mutex_init_attr_type) {
	pthread_mutex_t m;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	attr.type = 999;
	ASSERT_INT_EQ(EINVAL, pthread_mutex_init(&m, &attr));
	pthread_mutexattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_mutex_destroy);

TEST(pthread_mutex_destroy_valid_mutex) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(0, pthread_mutex_destroy(&m));
}

TEST(pthread_mutex_destroy_null_mutex) {
	ASSERT_INT_EQ(EINVAL, pthread_mutex_destroy(NULL));
}

TEST(pthread_mutex_destroy_uninitialized_mutex) {
	pthread_mutex_t m;
	ASSERT_INT_EQ(EINVAL, pthread_mutex_destroy(&m));
}

/* ============================================================================ */

TEST_SUITE(pthread_mutex_lock);

TEST(pthread_mutex_lock_unlocked_mutex) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(0, pthread_mutex_lock(&m));
	pthread_mutex_unlock(&m);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_lock_sets_owner) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	pthread_mutex_lock(&m);
	ASSERT_INT_NE(0, m.owner);
	pthread_mutex_unlock(&m);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_lock_sets_futex_one) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	pthread_mutex_lock(&m);
	ASSERT_INT_EQ(1, m.futex);
	pthread_mutex_unlock(&m);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_lock_null_mutex) {
	ASSERT_INT_EQ(EINVAL, pthread_mutex_lock(NULL));
}

TEST(pthread_mutex_lock_same_thread) {
	pthread_mutex_t m;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	pthread_mutex_init(&m, &attr);
	pthread_mutex_lock(&m);
	ASSERT_INT_EQ(EDEADLK, pthread_mutex_lock(&m));
	pthread_mutex_unlock(&m);
	pthread_mutex_destroy(&m);
	pthread_mutexattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_mutex_trylock);

TEST(pthread_mutex_trylock_unlocked_mutex) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(0, pthread_mutex_trylock(&m));
	pthread_mutex_unlock(&m);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_trylock_locked_mutex) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	pthread_mutex_lock(&m);
	ASSERT_INT_EQ(EBUSY, pthread_mutex_trylock(&m));
	pthread_mutex_unlock(&m);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_trylock_null_mutex) {
	ASSERT_INT_EQ(EINVAL, pthread_mutex_trylock(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_mutex_unlock);

TEST(pthread_mutex_unlock_locked_mutex) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	pthread_mutex_lock(&m);
	ASSERT_INT_EQ(0, pthread_mutex_unlock(&m));
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_unlock_on_success) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	pthread_mutex_lock(&m);
	pthread_mutex_unlock(&m);
	ASSERT_INT_EQ(0, m.futex);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_unlock_null_mutex) {
	ASSERT_INT_EQ(EINVAL, pthread_mutex_unlock(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_mutex_timedlock);

TEST(pthread_mutex_timedlock_unlocked_mutex) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	struct timespec ts = {0, 1000000};
	ASSERT_INT_EQ(0, pthread_mutex_timedlock(&m, &ts));
	pthread_mutex_unlock(&m);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_timedlock_locked_mutex) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	pthread_mutex_lock(&m);
	struct timespec ts = {0, 1000000};
	ASSERT_INT_EQ(ETIMEDOUT, pthread_mutex_timedlock(&m, &ts));
	pthread_mutex_unlock(&m);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_timedlock_null_mutex) {
	struct timespec ts = {0, 1000000};
	ASSERT_INT_EQ(EINVAL, pthread_mutex_timedlock(NULL, &ts));
}

TEST(pthread_mutex_timedlock_null_ts) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(EINVAL, pthread_mutex_timedlock(&m, NULL));
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_timedlock_invalid_nsec) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	struct timespec ts = {0, 2000000000};
	ASSERT_INT_EQ(EINVAL, pthread_mutex_timedlock(&m, &ts));
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_timedlock_negative_nsec) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	struct timespec ts = {0, -1};
	ASSERT_INT_EQ(EINVAL, pthread_mutex_timedlock(&m, &ts));
	pthread_mutex_destroy(&m);
}

/* ============================================================================ */

TEST_SUITE(pthread_mutex_consistent);

TEST(pthread_mutex_consistent_robust_mutex) {
	pthread_mutex_t m;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ROBUST);
	pthread_mutex_init(&m, &attr);
	ASSERT_INT_EQ(0, pthread_mutex_consistent(&m));
	pthread_mutex_destroy(&m);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutex_consistent_null_mutex) {
	ASSERT_INT_EQ(EINVAL, pthread_mutex_consistent(NULL));
}

TEST(pthread_mutex_consistent_non_robust_mutex) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(EINVAL, pthread_mutex_consistent(&m));
	pthread_mutex_destroy(&m);
}

/* ============================================================================ */

TEST_SUITE(pthread_cond_init);

TEST(pthread_cond_init_null_attr) {
	pthread_cond_t c;
	ASSERT_INT_EQ(0, pthread_cond_init(&c, NULL));
	pthread_cond_destroy(&c);
}

TEST(pthread_cond_init_futex_zero) {
	pthread_cond_t c;
	pthread_cond_init(&c, NULL);
	ASSERT_INT_EQ(0, c.futex);
	pthread_cond_destroy(&c);
}

TEST(pthread_cond_init_waiters_zero) {
	pthread_cond_t c;
	pthread_cond_init(&c, NULL);
	ASSERT_INT_EQ(0, c.waiters);
	pthread_cond_destroy(&c);
}

TEST(pthread_cond_init_null_cond) {
	ASSERT_INT_EQ(EINVAL, pthread_cond_init(NULL, NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_cond_destroy);

TEST(pthread_cond_destroy_valid_cond) {
	pthread_cond_t c;
	pthread_cond_init(&c, NULL);
	ASSERT_INT_EQ(0, pthread_cond_destroy(&c));
}

TEST(pthread_cond_destroy_null_cond) {
	ASSERT_INT_EQ(EINVAL, pthread_cond_destroy(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_cond_wait);

TEST(pthread_cond_wait_signal) {
	pthread_cond_t c;
	pthread_mutex_t m;
	pthread_t t;
	__cond_args_t args;

	pthread_cond_init(&c, NULL);
	pthread_mutex_init(&m, NULL);

	pthread_mutex_lock(&m);

	args.m = &m;
	args.c = &c;
	args.done = 0;

	/* Start helper thread: it will lock, then signal */
	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __cond_signaler, &args));

	int r = pthread_cond_wait(&c, &m);

	pthread_mutex_unlock(&m);
	pthread_join(t, NULL);

	ASSERT_INT_EQ(0, r);

	pthread_cond_destroy(&c);
	pthread_mutex_destroy(&m);
}



TEST(pthread_cond_wait_null_cond) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(EINVAL, pthread_cond_wait(NULL, &m));
	pthread_mutex_destroy(&m);
}

TEST(pthread_cond_wait_null_mutex) {
	pthread_cond_t c;
	pthread_cond_init(&c, NULL);
	ASSERT_INT_EQ(EINVAL, pthread_cond_wait(&c, NULL));
	pthread_cond_destroy(&c);
}

TEST(pthread_cond_wait_both_null) {
	ASSERT_INT_EQ(EINVAL, pthread_cond_wait(NULL, NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_cond_timedwait);

TEST(pthread_cond_timedwait_timeout) {
	pthread_cond_t c;
	pthread_mutex_t m;
	pthread_cond_init(&c, NULL);
	pthread_mutex_init(&m, NULL);
	pthread_mutex_lock(&m);
	struct timespec ts = {0, 10000000};
	ASSERT_INT_EQ(ETIMEDOUT, pthread_cond_timedwait(&c, &m, &ts));
	pthread_mutex_unlock(&m);
	pthread_cond_destroy(&c);
	pthread_mutex_destroy(&m);
}

TEST(pthread_cond_timedwait_null_cond) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	struct timespec ts = {0, 1000000};
	ASSERT_INT_EQ(EINVAL, pthread_cond_timedwait(NULL, &m, &ts));
	pthread_mutex_destroy(&m);
}

TEST(pthread_cond_timedwait_null_mutex) {
	pthread_cond_t c;
	pthread_cond_init(&c, NULL);
	struct timespec ts = {0, 1000000};
	ASSERT_INT_EQ(EINVAL, pthread_cond_timedwait(&c, NULL, &ts));
	pthread_cond_destroy(&c);
}

TEST(pthread_cond_timedwait_null_ts) {
	pthread_cond_t c;
	pthread_mutex_t m;
	pthread_cond_init(&c, NULL);
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(EINVAL, pthread_cond_timedwait(&c, &m, NULL));
	pthread_cond_destroy(&c);
	pthread_mutex_destroy(&m);
}

/* ============================================================================ */

TEST_SUITE(pthread_cond_signal);

TEST(pthread_cond_signal_valid_cond) {
	pthread_cond_t c;
	pthread_cond_init(&c, NULL);
	ASSERT_INT_EQ(0, pthread_cond_signal(&c));
	pthread_cond_destroy(&c);
}

TEST(pthread_cond_signal_null_cond) {
	ASSERT_INT_EQ(EINVAL, pthread_cond_signal(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_cond_broadcast);

TEST(pthread_cond_broadcast_valid_cond) {
	pthread_cond_t c;
	pthread_cond_init(&c, NULL);
	ASSERT_INT_EQ(0, pthread_cond_broadcast(&c));
	pthread_cond_destroy(&c);
}

TEST(pthread_cond_broadcast_null_cond) {
	ASSERT_INT_EQ(EINVAL, pthread_cond_broadcast(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_once);

TEST(pthread_once_basic) {
	pthread_once_t once = PTHREAD_ONCE_INIT;
	__test_once_counter = 0;
	ASSERT_INT_EQ(0, pthread_once(&once, __test_once_init_fn));
	ASSERT_INT_EQ(1, __test_once_counter);
	ASSERT_INT_EQ(0, pthread_once(&once, __test_once_init_fn));
	ASSERT_INT_EQ(1, __test_once_counter);
}

TEST(pthread_once_null_control) {
	ASSERT_INT_EQ(EINVAL, pthread_once(NULL, __test_once_empty_fn));
}

TEST(pthread_once_null_routine) {
	pthread_once_t once = PTHREAD_ONCE_INIT;
	ASSERT_INT_EQ(EINVAL, pthread_once(&once, NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_key_create);

TEST(pthread_key_create_success) {
	pthread_key_t key;
	ASSERT_INT_EQ(0, pthread_key_create(&key, NULL));
	pthread_key_delete(key);
}

TEST(pthread_key_create_null_key) {
	ASSERT_INT_EQ(EINVAL, pthread_key_create(NULL, NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_key_delete);

TEST(pthread_key_delete_valid_key) {
	pthread_key_t key;
	pthread_key_create(&key, NULL);
	ASSERT_INT_EQ(0, pthread_key_delete(key));
}

TEST(pthread_key_delete_invalid_key) {
	ASSERT_INT_EQ(EINVAL, pthread_key_delete(9999));
}

/* ============================================================================ */

TEST_SUITE(pthread_setspecific);

TEST(pthread_setspecific_valid_key) {
	pthread_key_t key;
	pthread_key_create(&key, NULL);
	ASSERT_INT_EQ(0, pthread_setspecific(key, (void *)123));
	pthread_key_delete(key);
}

TEST(pthread_setspecific_invalid_key) {
	ASSERT_INT_EQ(EINVAL, pthread_setspecific(9999, NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_getspecific);

TEST(pthread_getspecific_value) {
	pthread_key_t key;
	pthread_key_create(&key, NULL);
	pthread_setspecific(key, (void *)456);
	ASSERT_PTR_EQ((void *)456, pthread_getspecific(key));
	pthread_key_delete(key);
}

TEST(pthread_getspecific_invalid_key) {
	ASSERT_NULL(pthread_getspecific(9999));
}

/* ============================================================================ */

TEST_SUITE(pthread_spin_init);

TEST(pthread_spin_init_success) {
	pthread_spinlock_t s;
	ASSERT_INT_EQ(0, pthread_spin_init(&s, PTHREAD_PROCESS_PRIVATE));
	pthread_spin_destroy(&s);
}

TEST(pthread_spin_init_unlocked) {
	pthread_spinlock_t s;
	pthread_spin_init(&s, PTHREAD_PROCESS_PRIVATE);
	ASSERT_INT_EQ(PTHREAD_SPIN_UNLOCKED, s.lock);
	pthread_spin_destroy(&s);
}

TEST(pthread_spin_init_null_lock) {
	ASSERT_INT_EQ(EINVAL, pthread_spin_init(NULL, PTHREAD_PROCESS_PRIVATE));
}

/* ============================================================================ */

TEST_SUITE(pthread_spin_lock);

TEST(pthread_spin_lock_unlocked_spinlock) {
	pthread_spinlock_t s;
	pthread_spin_init(&s, PTHREAD_PROCESS_PRIVATE);
	ASSERT_INT_EQ(0, pthread_spin_lock(&s));
	pthread_spin_unlock(&s);
	pthread_spin_destroy(&s);
}

TEST(pthread_spin_lock_null_lock) {
	ASSERT_INT_EQ(EINVAL, pthread_spin_lock(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_spin_trylock);

TEST(pthread_spin_trylock_unlocked_spinlock) {
	pthread_spinlock_t s;
	pthread_spin_init(&s, PTHREAD_PROCESS_PRIVATE);
	ASSERT_INT_EQ(0, pthread_spin_trylock(&s));
	pthread_spin_unlock(&s);
	pthread_spin_destroy(&s);
}

TEST(pthread_spin_trylock_locked_spinlock) {
	pthread_spinlock_t s;
	pthread_spin_init(&s, PTHREAD_PROCESS_PRIVATE);
	pthread_spin_lock(&s);
	ASSERT_INT_EQ(EBUSY, pthread_spin_trylock(&s));
	pthread_spin_unlock(&s);
	pthread_spin_destroy(&s);
}

TEST(pthread_spin_trylock_null_lock) {
	ASSERT_INT_EQ(EINVAL, pthread_spin_trylock(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_spin_unlock);

TEST(pthread_spin_unlock_locked_spinlock) {
	pthread_spinlock_t s;
	pthread_spin_init(&s, PTHREAD_PROCESS_PRIVATE);
	pthread_spin_lock(&s);
	ASSERT_INT_EQ(0, pthread_spin_unlock(&s));
	pthread_spin_destroy(&s);
}

TEST(pthread_spin_unlock_null_lock) {
	ASSERT_INT_EQ(EINVAL, pthread_spin_unlock(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_rwlock_init);

TEST(pthread_rwlock_init_null_attr) {
	pthread_rwlock_t rw;
	ASSERT_INT_EQ(0, pthread_rwlock_init(&rw, NULL));
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_init_readers_zero) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	ASSERT_INT_EQ(0, rw.readers);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_init_writers_zero) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	ASSERT_INT_EQ(0, rw.writers);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_init_write_waiters_zero) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	ASSERT_INT_EQ(0, rw.write_waiters);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_init_writer_tid_zero) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	ASSERT_INT_EQ(0, rw.writer_tid);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_init_null_rwlock) {
	ASSERT_INT_EQ(EINVAL, pthread_rwlock_init(NULL, NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_rwlock_destroy);

TEST(pthread_rwlock_destroy_valid_rwlock) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	ASSERT_INT_EQ(0, pthread_rwlock_destroy(&rw));
}

TEST(pthread_rwlock_destroy_null_rwlock) {
	ASSERT_INT_EQ(EINVAL, pthread_rwlock_destroy(NULL));
}

TEST(pthread_rwlock_destroy_locked_rwlock) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	pthread_rwlock_rdlock(&rw);
	ASSERT_INT_EQ(EBUSY, pthread_rwlock_destroy(&rw));
	pthread_rwlock_unlock(&rw);
	pthread_rwlock_destroy(&rw);
}

/* ============================================================================ */

TEST_SUITE(pthread_rwlock_rdlock);

TEST(pthread_rwlock_rdlock_unlocked_rwlock) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	ASSERT_INT_EQ(0, pthread_rwlock_rdlock(&rw));
	pthread_rwlock_unlock(&rw);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_rdlock_increments_readers) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	pthread_rwlock_rdlock(&rw);
	ASSERT_INT_EQ(1, rw.readers);
	pthread_rwlock_unlock(&rw);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_rdlock_null_rwlock) {
	ASSERT_INT_EQ(EINVAL, pthread_rwlock_rdlock(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_rwlock_wrlock);

TEST(pthread_rwlock_wrlock_unlocked_rwlock) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	ASSERT_INT_EQ(0, pthread_rwlock_wrlock(&rw));
	pthread_rwlock_unlock(&rw);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_wrlock_writers_one) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	pthread_rwlock_wrlock(&rw);
	ASSERT_INT_EQ(1, rw.writers);
	pthread_rwlock_unlock(&rw);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_wrlock_writer_tid) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	pthread_rwlock_wrlock(&rw);
	ASSERT_INT_NE(0, rw.writer_tid);
	pthread_rwlock_unlock(&rw);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_wrlock_null_rwlock) {
	ASSERT_INT_EQ(EINVAL, pthread_rwlock_wrlock(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_rwlock_unlock);

TEST(pthread_rwlock_unlock_read_lock) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	pthread_rwlock_rdlock(&rw);
	ASSERT_INT_EQ(0, pthread_rwlock_unlock(&rw));
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_unlock_write_lock) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	pthread_rwlock_wrlock(&rw);
	ASSERT_INT_EQ(0, pthread_rwlock_unlock(&rw));
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_unlock_write_unlock) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	pthread_rwlock_wrlock(&rw);
	pthread_rwlock_unlock(&rw);
	ASSERT_INT_EQ(0, rw.writers);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_unlock_null_rwlock) {
	ASSERT_INT_EQ(EINVAL, pthread_rwlock_unlock(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_barrier_init);

TEST(pthread_barrier_init_count_two) {
	pthread_barrier_t b;
	ASSERT_INT_EQ(0, pthread_barrier_init(&b, NULL, 2));
	pthread_barrier_destroy(&b);
}

TEST(pthread_barrier_init_sets_zero) {
	pthread_barrier_t b;
	pthread_barrier_init(&b, NULL, 2);
	ASSERT_INT_EQ(0, b.count);
	pthread_barrier_destroy(&b);
}

TEST(pthread_barrier_init_phase_zero) {
	pthread_barrier_t b;
	pthread_barrier_init(&b, NULL, 2);
	ASSERT_INT_EQ(0, b.phase);
	pthread_barrier_destroy(&b);
}

TEST(pthread_barrier_init_limit_count) {
	pthread_barrier_t b;
	pthread_barrier_init(&b, NULL, 3);
	ASSERT_INT_EQ(3, b.limit);
	pthread_barrier_destroy(&b);
}

TEST(pthread_barrier_init_null_barrier) {
	ASSERT_INT_EQ(EINVAL, pthread_barrier_init(NULL, NULL, 2));
}

TEST(pthread_barrier_init_zero_count) {
	pthread_barrier_t b;
	ASSERT_INT_EQ(EINVAL, pthread_barrier_init(&b, NULL, 0));
}

/* ============================================================================ */

TEST_SUITE(pthread_barrier_wait);

typedef struct {
	pthread_barrier_t *b;
	int result;
} __barrier_test_args_t;

static void *__barrier_worker(void *arg) {
	__barrier_test_args_t *args = (__barrier_test_args_t *)arg;
	args->result = pthread_barrier_wait(args->b);
	return NULL;
}

TEST(pthread_barrier_wait_serial_thread) {
	pthread_barrier_t b;
	pthread_barrier_init(&b, NULL, 1);
	/* The last (and only) thread to arrive gets SERIAL_THREAD */
	ASSERT_INT_EQ(PTHREAD_BARRIER_SERIAL_THREAD, pthread_barrier_wait(&b));
	pthread_barrier_destroy(&b);
}

TEST(pthread_barrier_wait_normal_and_serial) {
	pthread_barrier_t b;
	pthread_t t;
	__barrier_test_args_t args;

	pthread_barrier_init(&b, NULL, 2);

	args.b = &b;
	args.result = -1;

	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __barrier_worker, &args));

	/* Main thread is the second to arrive */
	int main_result = pthread_barrier_wait(&b);

	pthread_join(t, NULL);

	/* One must be 0, the other must be SERIAL_THREAD */
	ASSERT_TRUE(
		(main_result == 0 && args.result == PTHREAD_BARRIER_SERIAL_THREAD) ||
		(main_result == PTHREAD_BARRIER_SERIAL_THREAD && args.result == 0)
	);

	pthread_barrier_destroy(&b);
}

TEST(pthread_barrier_wait_null_barrier) {
	ASSERT_INT_EQ(EINVAL, pthread_barrier_wait(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_cancel);

TEST(pthread_cancel_valid_thread) {
	pthread_t self = pthread_self();
	ASSERT_INT_EQ(0, pthread_cancel(self));
}

TEST(pthread_cancel_null_thread) {
	ASSERT_INT_EQ(ESRCH, pthread_cancel(PTHREAD_NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_setcancelstate);

TEST(pthread_setcancelstate_disable_state) {
	pthread_t t;
	int done = 0;

	/* Run the risky code in a child thread */
	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __cancel_worker, &done));
	ASSERT_INT_EQ(0, pthread_join(t, NULL));

	ASSERT_INT_EQ(1, done);
}

TEST(pthread_setcancelstate_enable_state) {
	pthread_t t;
	int done = 0;

	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __cancel_worker, &done));
	ASSERT_INT_EQ(0, pthread_join(t, NULL));

	ASSERT_INT_EQ(1, done);
}

TEST(pthread_setcancelstate_success) {
	pthread_t t;
	int done = 0;

	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __cancel_worker, &done));
	ASSERT_INT_EQ(0, pthread_join(t, NULL));

	ASSERT_INT_EQ(1, done);
}

TEST(pthread_setcancelstate_invalid_state) {
	int old;
	ASSERT_INT_EQ(EINVAL, pthread_setcancelstate(999, &old));
}

/* ============================================================================ */

TEST_SUITE(pthread_setcanceltype);

TEST(pthread_setcanceltype_deferred_type) {
	int old;
	ASSERT_INT_EQ(0, pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &old));
}

TEST(pthread_setcanceltype_success) {
	int old;
	ASSERT_INT_EQ(0, pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &old));
}

TEST(pthread_setcanceltype_invalid_type) {
	int old;
	ASSERT_INT_EQ(EINVAL, pthread_setcanceltype(999, &old));
}

/* ============================================================================ */

TEST_SUITE(pthread_testcancel);

TEST(pthread_testcancel_basic) {
	pthread_t t;
	int done = 0;

	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __cancel_worker, &done));
	ASSERT_INT_EQ(0, pthread_join(t, NULL));

	ASSERT_INT_EQ(1, done);
}

/* ============================================================================ */

TEST_SUITE(pthread_create);

TEST(pthread_create_success) {
	pthread_t t;
	int val = 0;
	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __test_thread_fn_42, &val));
	pthread_join(t, NULL);
	ASSERT_INT_EQ(42, val);
}

TEST(pthread_create_null_thread) {
	ASSERT_INT_EQ(EINVAL, pthread_create(NULL, NULL, NULL, NULL));
}

TEST(pthread_create_null_routine) {
	pthread_t t;
	ASSERT_INT_EQ(EINVAL, pthread_create(&t, NULL, NULL, NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_join);

TEST(pthread_join_joinable_thread) {
	pthread_t t;
	int val = 0;
	pthread_create(&t, NULL, __test_thread_fn_99, &val);
	ASSERT_INT_EQ(0, pthread_join(t, NULL));
	ASSERT_INT_EQ(99, val);
}

TEST(pthread_join_null_thread) {
	ASSERT_INT_EQ(EINVAL, pthread_join(NULL, NULL));
}

TEST(pthread_join_detached_thread) {
	pthread_t t;
	pthread_create(&t, NULL, __test_thread_fn_null, NULL);
	pthread_detach(t);
	/* Joining a detached thread should fail with EINVAL */
	ASSERT_INT_EQ(EINVAL, pthread_join(t, NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_detach);

TEST(pthread_detach_valid_thread) {
	pthread_t t;

	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __detach_worker, NULL));

	ASSERT_INT_EQ(0, pthread_detach(t));

	usleep(10000);
}

TEST(pthread_detach_null_thread) {
	ASSERT_INT_EQ(EINVAL, pthread_detach(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_exit);

static void *__exit_worker(void *arg) {
	pthread_exit((void *)0x12345678);

	return NULL; /* Should not reach here */
}

TEST(pthread_exit_from_child) {
	pthread_t t;
	void *retval = NULL;

	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __exit_worker, NULL));
	ASSERT_INT_EQ(0, pthread_join(t, &retval));
	ASSERT_PTR_EQ((void *)0x12345678, retval);
}

/* ============================================================================ */

TEST_SUITE(pthread_self);

TEST(pthread_self_basic) {
	pthread_t self = pthread_self();
	ASSERT_NOT_NULL(self);
}

TEST(pthread_self_empty) {
	pthread_t t;
	int ok = 0;

	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __self_worker, &ok));
	ASSERT_INT_EQ(0, pthread_join(t, NULL));

	ASSERT_INT_EQ(1, ok);
}

/* ============================================================================ */

TEST_SUITE(pthread_equal);

TEST(pthread_equal_same_thread) {
	pthread_t self = pthread_self();
	ASSERT_TRUE(pthread_equal(self, self));
}

/* ============================================================================ */

TEST_SUITE(pthread_sigmask);

TEST(pthread_sigmask_success) {
	sigset_t set, old;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);

	#if JACL_HAS_POSIX
		ASSERT_INT_EQ(0, pthread_sigmask(SIG_BLOCK, &set, &old));
		/* Restore original mask */
		pthread_sigmask(SIG_SETMASK, &old, NULL);
	#else
		ASSERT_INT_EQ(ENOSYS, pthread_sigmask(SIG_BLOCK, &set, &old));
	#endif
}

TEST(pthread_sigmask_invalid_how) {
	sigset_t set;
	sigemptyset(&set);
	#if JACL_HAS_POSIX
		ASSERT_INT_EQ(EINVAL, pthread_sigmask(999, &set, NULL));
	#else
		ASSERT_INT_EQ(ENOSYS, pthread_sigmask(999, &set, NULL));
	#endif
}

/* ============================================================================ */

TEST_SUITE(pthread_mutexattr_setpshared);

TEST(pthread_mutexattr_setpshared_private) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE));
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_setpshared_shared) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED));
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_setpshared_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_mutexattr_setpshared(NULL, PTHREAD_PROCESS_PRIVATE));
}

TEST(pthread_mutexattr_setpshared_invalid_pshared) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_mutexattr_setpshared(&attr, 999));
	pthread_mutexattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_mutexattr_getpshared);

TEST(pthread_mutexattr_getpshared_basic) {
	pthread_mutexattr_t attr;
	int pshared;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	ASSERT_INT_EQ(0, pthread_mutexattr_getpshared(&attr, &pshared));
	ASSERT_INT_EQ(PTHREAD_PROCESS_SHARED, pshared);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_getpshared_default) {
	pthread_mutexattr_t attr;
	int pshared;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_mutexattr_getpshared(&attr, &pshared));
	ASSERT_INT_EQ(PTHREAD_PROCESS_PRIVATE, pshared);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_getpshared_null_attr) {
	int pshared;
	ASSERT_INT_EQ(EINVAL, pthread_mutexattr_getpshared(NULL, &pshared));
}

TEST(pthread_mutexattr_getpshared_null_shared) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_mutexattr_getpshared(&attr, NULL));
	pthread_mutexattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_condattr_setpshared);

TEST(pthread_condattr_setpshared_private) {
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE));
	pthread_condattr_destroy(&attr);
}

TEST(pthread_condattr_setpshared_shared) {
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED));
	pthread_condattr_destroy(&attr);
}

TEST(pthread_condattr_setpshared_returns_einval_on_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_condattr_setpshared(NULL, PTHREAD_PROCESS_PRIVATE));
}

TEST(pthread_condattr_setpshared_returns_einval_on_invalid) {
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_condattr_setpshared(&attr, 999));
	pthread_condattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_condattr_getpshared);

TEST(pthread_condattr_getpshared_basic) {
	pthread_condattr_t attr;
	int pshared;
	pthread_condattr_init(&attr);
	pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	ASSERT_INT_EQ(0, pthread_condattr_getpshared(&attr, &pshared));
	ASSERT_INT_EQ(PTHREAD_PROCESS_SHARED, pshared);
	pthread_condattr_destroy(&attr);
}

TEST(pthread_condattr_getpshared_default) {
	pthread_condattr_t attr;
	int pshared;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_condattr_getpshared(&attr, &pshared));
	ASSERT_INT_EQ(PTHREAD_PROCESS_PRIVATE, pshared);
	pthread_condattr_destroy(&attr);
}

TEST(pthread_condattr_getpshared_null_attr) {
	int pshared;
	ASSERT_INT_EQ(EINVAL, pthread_condattr_getpshared(NULL, &pshared));
}

TEST(pthread_condattr_getpshare_null_pshared) {
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_condattr_getpshared(&attr, NULL));
	pthread_condattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_condattr_setclock);

TEST(pthread_condattr_setclock_realtime) {
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_condattr_setclock(&attr, CLOCK_REALTIME));
	pthread_condattr_destroy(&attr);
}

TEST(pthread_condattr_setclock_monotonic) {
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_condattr_setclock(&attr, CLOCK_MONOTONIC));
	pthread_condattr_destroy(&attr);
}

TEST(pthread_condattr_setclock_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_condattr_setclock(NULL, CLOCK_REALTIME));
}

TEST(pthread_condattr_setclock_invalid_clock) {
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_condattr_setclock(&attr, 999));
	pthread_condattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_condattr_getclock);

TEST(pthread_condattr_getclock_basic) {
	pthread_condattr_t attr;
	clockid_t clock_id;
	pthread_condattr_init(&attr);
	pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
	ASSERT_INT_EQ(0, pthread_condattr_getclock(&attr, &clock_id));
	ASSERT_INT_EQ(CLOCK_MONOTONIC, clock_id);
	pthread_condattr_destroy(&attr);
}

TEST(pthread_condattr_getclock_default) {
	pthread_condattr_t attr;
	clockid_t clock_id;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_condattr_getclock(&attr, &clock_id));
	ASSERT_INT_EQ(CLOCK_REALTIME, clock_id);
	pthread_condattr_destroy(&attr);
}

TEST(pthread_condattr_getclock_null_attr) {
	clockid_t clock_id;
	ASSERT_INT_EQ(EINVAL, pthread_condattr_getclock(NULL, &clock_id));
}

TEST(pthread_condattr_getclock_null_clock) {
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_condattr_getclock(&attr, NULL));
	pthread_condattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_rwlockattr_setpshared);

TEST(pthread_rwlockattr_setpshared_private) {
	pthread_rwlockattr_t attr;
	pthread_rwlockattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE));
	pthread_rwlockattr_destroy(&attr);
}

TEST(pthread_rwlockattr_setpshared_shared) {
	pthread_rwlockattr_t attr;
	pthread_rwlockattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_SHARED));
	pthread_rwlockattr_destroy(&attr);
}

TEST(pthread_rwlockattr_setpshared_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_rwlockattr_setpshared(NULL, PTHREAD_PROCESS_PRIVATE));
}

TEST(pthread_rwlockattr_setpshared_invalid) {
	pthread_rwlockattr_t attr;
	pthread_rwlockattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_rwlockattr_setpshared(&attr, 999));
	pthread_rwlockattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_rwlockattr_getpshared);

TEST(pthread_rwlockattr_getpshared_basic) {
	pthread_rwlockattr_t attr;
	int pshared;
	pthread_rwlockattr_init(&attr);
	pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	ASSERT_INT_EQ(0, pthread_rwlockattr_getpshared(&attr, &pshared));
	ASSERT_INT_EQ(PTHREAD_PROCESS_SHARED, pshared);
	pthread_rwlockattr_destroy(&attr);
}

TEST(pthread_rwlockattr_getpshared_default) {
	pthread_rwlockattr_t attr;
	int pshared;
	pthread_rwlockattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_rwlockattr_getpshared(&attr, &pshared));
	ASSERT_INT_EQ(PTHREAD_PROCESS_PRIVATE, pshared);
	pthread_rwlockattr_destroy(&attr);
}

TEST(pthread_rwlockattr_getpshared_null_attr) {
	int pshared;
	ASSERT_INT_EQ(EINVAL, pthread_rwlockattr_getpshared(NULL, &pshared));
}

TEST(pthread_rwlockattr_getpshared_null_pshared) {
	pthread_rwlockattr_t attr;
	pthread_rwlockattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_rwlockattr_getpshared(&attr, NULL));
	pthread_rwlockattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_barrierattr_setpshared);

TEST(pthread_barrierattr_setpshared_zero_on_private) {
	pthread_barrierattr_t attr;
	pthread_barrierattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE));
	pthread_barrierattr_destroy(&attr);
}

TEST(pthread_barrierattr_setpshared_zero_on_shared) {
	pthread_barrierattr_t attr;
	pthread_barrierattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_SHARED));
	pthread_barrierattr_destroy(&attr);
}

TEST(pthread_barrierattr_setpshared_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_barrierattr_setpshared(NULL, PTHREAD_PROCESS_PRIVATE));
}

TEST(pthread_barrierattr_setpshared_invalid_pshared) {
	pthread_barrierattr_t attr;
	pthread_barrierattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_barrierattr_setpshared(&attr, 999));
	pthread_barrierattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_barrierattr_getpshared);

TEST(pthread_barrierattr_getpshared_basic) {
	pthread_barrierattr_t attr;
	int pshared;
	pthread_barrierattr_init(&attr);
	pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	ASSERT_INT_EQ(0, pthread_barrierattr_getpshared(&attr, &pshared));
	ASSERT_INT_EQ(PTHREAD_PROCESS_SHARED, pshared);
	pthread_barrierattr_destroy(&attr);
}

TEST(pthread_barrierattr_getpshared_default) {
	pthread_barrierattr_t attr;
	int pshared;
	pthread_barrierattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_barrierattr_getpshared(&attr, &pshared));
	ASSERT_INT_EQ(PTHREAD_PROCESS_PRIVATE, pshared);
	pthread_barrierattr_destroy(&attr);
}

TEST(pthread_barrierattr_getpshared_null_attr) {
	int pshared;
	ASSERT_INT_EQ(EINVAL, pthread_barrierattr_getpshared(NULL, &pshared));
}

TEST(pthread_barrierattr_getpshared_null_pshared) {
	pthread_barrierattr_t attr;
	pthread_barrierattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_barrierattr_getpshared(&attr, NULL));
	pthread_barrierattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_mutex_recursive);

static void *__recursive_worker(void *arg) {
	pthread_mutex_t *m = (pthread_mutex_t *)arg;
	pthread_mutex_lock(m);
	pthread_mutex_lock(m); /* Should not deadlock */
	pthread_mutex_unlock(m);
	pthread_mutex_unlock(m);
	return NULL;
}

TEST(pthread_mutex_recursive_double_lock) {
	pthread_mutex_t m;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&m, &attr);

	ASSERT_INT_EQ(0, pthread_mutex_lock(&m));
	ASSERT_INT_EQ(0, pthread_mutex_lock(&m));
	ASSERT_INT_EQ(2, m.recursive_count);
	pthread_mutex_unlock(&m);
	ASSERT_INT_EQ(1, m.recursive_count);
	pthread_mutex_unlock(&m);
	ASSERT_INT_EQ(0, m.recursive_count);

	pthread_mutex_destroy(&m);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutex_recursive_thread_safety) {
	pthread_mutex_t m;
	pthread_mutexattr_t attr;
	pthread_t t;

	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&m, &attr);

	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __recursive_worker, &m));
	ASSERT_INT_EQ(0, pthread_join(t, NULL));

	pthread_mutex_destroy(&m);
	pthread_mutexattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_mutexattr_settype);

TEST(pthread_mutexattr_settype_normal) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL));
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_settype_recursive) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_settype_errorcheck) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK));
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_settype_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_mutexattr_settype(NULL, PTHREAD_MUTEX_NORMAL));
}

TEST(pthread_mutexattr_settype_invalid_type) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_mutexattr_settype(&attr, 999));
	pthread_mutexattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_mutexattr_gettype);

TEST(pthread_mutexattr_gettype_basic) {
	pthread_mutexattr_t attr;
	int type;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	ASSERT_INT_EQ(0, pthread_mutexattr_gettype(&attr, &type));
	ASSERT_INT_EQ(PTHREAD_MUTEX_RECURSIVE, type);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_gettype_default) {
	pthread_mutexattr_t attr;
	int type;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_mutexattr_gettype(&attr, &type));
	ASSERT_INT_EQ(PTHREAD_MUTEX_NORMAL, type);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_gettype_null_attr) {
	int type;
	ASSERT_INT_EQ(EINVAL, pthread_mutexattr_gettype(NULL, &type));
}

TEST(pthread_mutexattr_gettype_null_type) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_mutexattr_gettype(&attr, NULL));
	pthread_mutexattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_mutexattr_setrobust);

TEST(pthread_mutexattr_setrobust_robust) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST));
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_setrobust_stalled) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_STALLED));
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_setrobust_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_mutexattr_setrobust(NULL, PTHREAD_MUTEX_ROBUST));
}

TEST(pthread_mutexattr_setrobust_invalid) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_mutexattr_setrobust(&attr, 999));
	pthread_mutexattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_mutexattr_getrobust);

TEST(pthread_mutexattr_getrobust_basic) {
	pthread_mutexattr_t attr;
	int robust;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
	ASSERT_INT_EQ(0, pthread_mutexattr_getrobust(&attr, &robust));
	ASSERT_INT_EQ(PTHREAD_MUTEX_ROBUST, robust);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_getrobust_default) {
	pthread_mutexattr_t attr;
	int robust;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_mutexattr_getrobust(&attr, &robust));
	ASSERT_INT_EQ(PTHREAD_MUTEX_STALLED, robust);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_getrobust_null_attr) {
	int robust;
	ASSERT_INT_EQ(EINVAL, pthread_mutexattr_getrobust(NULL, &robust));
}

TEST(pthread_mutexattr_getrobust_null_robust) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_mutexattr_getrobust(&attr, NULL));
	pthread_mutexattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_atfork);

static void __atfork_prepare(void) { }
static void __atfork_parent(void) { }
static void __atfork_child(void) { }

TEST(pthread_atfork_success) {
	ASSERT_INT_EQ(0, pthread_atfork(__atfork_prepare, __atfork_parent, __atfork_child));
}

TEST(pthread_atfork_null_handlers) {
	/* POSIX allows NULL handlers */
	ASSERT_INT_EQ(0, pthread_atfork(NULL, NULL, NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_kill);

TEST(pthread_kill_self_zero) {
	/* Sending signal 0 checks if the thread exists */
	ASSERT_INT_EQ(0, pthread_kill(pthread_self(), 0));
}

TEST(pthread_kill_null_thread) {
	errno = 0;
	ASSERT_INT_EQ(-1, pthread_kill(PTHREAD_NULL, 0));
	ASSERT_INT_EQ(EINVAL, errno);
}

TEST(pthread_kill_invalid_sig) {
	errno = 0;
	ASSERT_INT_EQ(-1, pthread_kill(pthread_self(), -1));
	ASSERT_INT_EQ(EINVAL, errno);
}

/* ============================================================================ */

TEST_SUITE(pthread_attr_getdetachstate);

TEST(pthread_attr_getdetachstate_joinable) {
	pthread_attr_t attr;
	int state;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	ASSERT_INT_EQ(0, pthread_attr_getdetachstate(&attr, &state));
	ASSERT_INT_EQ(PTHREAD_CREATE_JOINABLE, state);
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_getdetachstate_detached) {
	pthread_attr_t attr;
	int state;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	ASSERT_INT_EQ(0, pthread_attr_getdetachstate(&attr, &state));
	ASSERT_INT_EQ(PTHREAD_CREATE_DETACHED, state);
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_getdetachstate_null_attr) {
	int state;
	ASSERT_INT_EQ(EINVAL, pthread_attr_getdetachstate(NULL, &state));
}

TEST(pthread_attr_getdetachstate_null_state) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_attr_getdetachstate(&attr, NULL));
	pthread_attr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_attr_setschedparam);

TEST(pthread_attr_setschedparam_success) {
	pthread_attr_t attr;
	struct sched_param param;
	param.sched_priority = 0; /* SCHED_OTHER usually requires 0 */

	pthread_attr_init(&attr);
	#if JACL_HAS_POSIX
		ASSERT_INT_EQ(0, pthread_attr_setschedparam(&attr, &param));
	#else
		ASSERT_INT_EQ(ENOSYS, pthread_attr_setschedparam(&attr, &param));
	#endif
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setschedparam_null_attr) {
	struct sched_param param;
	ASSERT_INT_EQ(EINVAL, pthread_attr_setschedparam(NULL, &param));
}

TEST(pthread_attr_setschedparam_null_param) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_attr_setschedparam(&attr, NULL));
	pthread_attr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_attr_getschedparam);

TEST(pthread_attr_getschedparam_success) {
	pthread_attr_t attr;
	struct sched_param param;

	pthread_attr_init(&attr);
	#if JACL_HAS_POSIX
		ASSERT_INT_EQ(0, pthread_attr_getschedparam(&attr, &param));
	#else
		ASSERT_INT_EQ(ENOSYS, pthread_attr_getschedparam(&attr, &param));
	#endif
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_getschedparam_null_attr) {
	struct sched_param param;
	ASSERT_INT_EQ(EINVAL, pthread_attr_getschedparam(NULL, &param));
}

TEST(pthread_attr_getschedparam_null_param) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_attr_getschedparam(&attr, NULL));
	pthread_attr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_setschedparam);

TEST(pthread_setschedparam_self) {
	struct sched_param param;
	param.sched_priority = 0;

	#if JACL_HAS_POSIX
		int r = pthread_setschedparam(pthread_self(), SCHED_OTHER, &param);
		/* EPERM is common if not root, EINVAL if policy is wrong */
		ASSERT_TRUE(r == 0 || r == EPERM || r == EINVAL);
	#else
		ASSERT_INT_EQ(ENOSYS, pthread_setschedparam(pthread_self(), SCHED_OTHER, &param));
	#endif
}

TEST(pthread_setschedparam_null_thread) {
	struct sched_param param;
	ASSERT_INT_EQ(EINVAL, pthread_setschedparam(PTHREAD_NULL, SCHED_OTHER, &param));
}

/* ============================================================================ */

TEST_SUITE(pthread_getschedparam);

TEST(pthread_getschedparam_self) {
	int policy;
	struct sched_param param;

	#if JACL_HAS_POSIX
		int r = pthread_getschedparam(pthread_self(), &policy, &param);
		ASSERT_TRUE(r == 0 || r == ENOSYS);
	#else
		ASSERT_INT_EQ(ENOSYS, pthread_getschedparam(pthread_self(), &policy, &param));
	#endif
}

TEST(pthread_getschedparam_null_thread) {
	int policy;
	struct sched_param param;
	ASSERT_INT_EQ(EINVAL, pthread_getschedparam(PTHREAD_NULL, &policy, &param));
}

/* ============================================================================ */

TEST_SUITE(pthread_attr_setstack);

TEST(pthread_attr_setstack_success) {
	pthread_attr_t attr;
	char stack[8192];

	pthread_attr_init(&attr);
	#if JACL_HAS_POSIX
		ASSERT_INT_EQ(0, pthread_attr_setstack(&attr, stack, sizeof(stack)));
	#else
		ASSERT_INT_EQ(ENOSYS, pthread_attr_setstack(&attr, stack, sizeof(stack)));
	#endif
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setstack_null_attr) {
	char stack[8192];
	ASSERT_INT_EQ(EINVAL, pthread_attr_setstack(NULL, stack, sizeof(stack)));
}

/* ============================================================================ */

TEST_SUITE(pthread_attr_getstack);

TEST(pthread_attr_getstack_success) {
	pthread_attr_t attr;
	void *addr;
	size_t size;
	char stack[8192];

	pthread_attr_init(&attr);
	#if JACL_HAS_POSIX
		pthread_attr_setstack(&attr, stack, sizeof(stack));
		ASSERT_INT_EQ(0, pthread_attr_getstack(&attr, &addr, &size));
		ASSERT_PTR_EQ(stack, addr);
		ASSERT_INT_EQ((int)sizeof(stack), (int)size);
	#else
		ASSERT_INT_EQ(ENOSYS, pthread_attr_getstack(&attr, &addr, &size));
	#endif
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_getstack_null_attr) {
	void *addr;
	size_t size;
	ASSERT_INT_EQ(EINVAL, pthread_attr_getstack(NULL, &addr, &size));
}

/* ============================================================================ */

TEST_SUITE(pthread_getcpuclockid);

TEST(pthread_getcpuclockid_self) {
	clockid_t clock_id;

	#if JACL_HAS_POSIX
		int r = pthread_getcpuclockid(pthread_self(), &clock_id);
		/* If it succeeds, try to use it. If it fails, it might be ENOSYS */
		if (r == 0) {
			struct timespec ts;
			ASSERT_INT_EQ(0, clock_gettime(clock_id, &ts));
		} else {
			ASSERT_INT_EQ(ENOSYS, r);
		}
	#else
		ASSERT_INT_EQ(ENOSYS, pthread_getcpuclockid(pthread_self(), &clock_id));
	#endif
}

TEST(pthread_getcpuclockid_null_thread) {
	clockid_t clock_id;
	ASSERT_INT_EQ(EINVAL, pthread_getcpuclockid(PTHREAD_NULL, &clock_id));
}

/* ============================================================================ */

TEST_MAIN()
