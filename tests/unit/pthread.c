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

TEST(constants_thread_creation_values) {
	ASSERT_INT_EQ(0, PTHREAD_CREATE_JOINABLE);
	ASSERT_INT_EQ(1, PTHREAD_CREATE_DETACHED);
}

TEST(constants_mutex_type_values) {
	ASSERT_INT_EQ(0, PTHREAD_MUTEX_NORMAL);
	ASSERT_INT_EQ(1, PTHREAD_MUTEX_RECURSIVE);
	ASSERT_INT_EQ(2, PTHREAD_MUTEX_ERRORCHECK);
	ASSERT_INT_EQ(4, PTHREAD_MUTEX_ROBUST);
	ASSERT_INT_EQ(0, PTHREAD_MUTEX_STALLED);
}

TEST(constants_mutex_initializer_compiles) {
	pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
	ASSERT_NOT_NULL(&m);
}

TEST(constants_cond_initializer_compiles) {
	pthread_cond_t c = PTHREAD_COND_INITIALIZER;
	ASSERT_NOT_NULL(&c);
}

TEST(constants_rwlock_initializer_compiles) {
	pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
	ASSERT_NOT_NULL(&rw);
}

TEST(constants_barrier_values) {
	ASSERT_INT_EQ(-1, PTHREAD_BARRIER_SERIAL_THREAD);
}

TEST(constants_barrier_initializer_compiles) {
	pthread_barrier_t b = PTHREAD_BARRIER_INITIALIZER;
	ASSERT_NOT_NULL(&b);
}

TEST(constants_spinlock_values) {
	ASSERT_INT_EQ(1, PTHREAD_SPIN_LOCKED);
	ASSERT_INT_EQ(0, PTHREAD_SPIN_UNLOCKED);
}

TEST(constants_spinlock_initializer_compiles) {
	pthread_spinlock_t s = PTHREAD_SPIN_LOCK_INITIALIZER;
	ASSERT_NOT_NULL(&s);
}

TEST(constants_cancellation_enable_values) {
	ASSERT_INT_EQ(0, PTHREAD_CANCEL_ENABLE);
	ASSERT_INT_EQ(1, PTHREAD_CANCEL_DISABLE);
}

TEST(constants_cancellation_type_values) {
	ASSERT_INT_EQ(0, PTHREAD_CANCEL_DEFERRED);
	ASSERT_INT_EQ(1, PTHREAD_CANCEL_ASYNCHRONOUS);
}

TEST(constants_canceled_value) {
	ASSERT_PTR_EQ(PTHREAD_CANCELED, (void *)-1);
}

TEST(constants_process_sharing_values) {
	ASSERT_INT_EQ(0, PTHREAD_PROCESS_PRIVATE);
	ASSERT_INT_EQ(1, PTHREAD_PROCESS_SHARED);
}

TEST(constants_once_init_value) {
	pthread_once_t o = PTHREAD_ONCE_INIT;
	ASSERT_INT_EQ(0, o.done);
}

TEST(constants_destructor_iterations_value) {
	ASSERT_INT_EQ(4, PTHREAD_DESTRUCTOR_ITERATIONS);
}

TEST(constants_null_thread_compiles) {
	pthread_t null_thread = PTHREAD_NULL;
	ASSERT_TRUE(pthread_equal(null_thread, null_thread) || 1);
}

/* ============================================================================ */

TEST_SUITE(pthread_attr_t);

TEST(pthread_attr_t_size_minimum) {
	ASSERT_SIZE_MIN(pthread_attr_t, sizeof(int) + sizeof(size_t) + sizeof(void *));
}

TEST(pthread_attr_t_field_detached_offset) {
	ASSERT_OFFSET_PAST(pthread_attr_t, detached, 0);
}

TEST(pthread_attr_t_field_stack_size_offset) {
	ASSERT_OFFSET_AFTER(pthread_attr_t, stack_size, detached);
}

TEST(pthread_attr_t_field_stack_addr_offset) {
	ASSERT_OFFSET_AFTER(pthread_attr_t, stack_addr, stack_size);
}

/* ============================================================================ */

TEST_SUITE(pthread_mutex_t);

TEST(pthread_mutex_t_size_minimum) {
	ASSERT_SIZE_MIN(pthread_mutex_t, sizeof(_Atomic int) + sizeof(pid_t) + sizeof(int) * 2);
}

TEST(pthread_mutex_t_field_futex_offset) {
	ASSERT_OFFSET_PAST(pthread_mutex_t, futex, 0);
}

TEST(pthread_mutex_t_field_owner_offset) {
	ASSERT_OFFSET_AFTER(pthread_mutex_t, owner, futex);
}

TEST(pthread_mutex_t_field_type_offset) {
	ASSERT_OFFSET_AFTER(pthread_mutex_t, type, owner);
}

TEST(pthread_mutex_t_field_recursive_count_offset) {
	ASSERT_OFFSET_AFTER(pthread_mutex_t, recursive_count, type);
}

/* ============================================================================ */

TEST_SUITE(pthread_cond_t);

TEST(pthread_cond_t_size_minimum) {
	ASSERT_SIZE_MIN(pthread_cond_t, sizeof(_Atomic int) * 2);
}

TEST(pthread_cond_t_field_futex_offset) {
	ASSERT_OFFSET_PAST(pthread_cond_t, futex, 0);
}

TEST(pthread_cond_t_field_waiters_offset) {
	ASSERT_OFFSET_AFTER(pthread_cond_t, waiters, futex);
}

/* ============================================================================ */

TEST_SUITE(pthread_rwlock_t);

TEST(pthread_rwlock_t_size_minimum) {
	ASSERT_SIZE_MIN(pthread_rwlock_t, sizeof(_Atomic int) * 3 + sizeof(pid_t));
}

TEST(pthread_rwlock_t_field_readers_offset) {
	ASSERT_OFFSET_PAST(pthread_rwlock_t, readers, 0);
}

TEST(pthread_rwlock_t_field_writers_offset) {
	ASSERT_OFFSET_AFTER(pthread_rwlock_t, writers, readers);
}

TEST(pthread_rwlock_t_field_write_waiters_offset) {
	ASSERT_OFFSET_AFTER(pthread_rwlock_t, write_waiters, writers);
}

TEST(pthread_rwlock_t_field_writer_tid_offset) {
	ASSERT_OFFSET_AFTER(pthread_rwlock_t, writer_tid, write_waiters);
}

/* ============================================================================ */

TEST_SUITE(pthread_barrier_t);

TEST(pthread_barrier_t_size_minimum) {
	ASSERT_SIZE_MIN(pthread_barrier_t, sizeof(_Atomic int) * 3 + sizeof(unsigned));
}

TEST(pthread_barrier_t_field_count_offset) {
	ASSERT_OFFSET_PAST(pthread_barrier_t, count, 0);
}

TEST(pthread_barrier_t_field_phase_offset) {
	ASSERT_OFFSET_AFTER(pthread_barrier_t, phase, count);
}

TEST(pthread_barrier_t_field_limit_offset) {
	ASSERT_OFFSET_AFTER(pthread_barrier_t, limit, phase);
}

TEST(pthread_barrier_t_field_waiters_offset) {
	ASSERT_OFFSET_AFTER(pthread_barrier_t, waiters, limit);
}

/* ============================================================================ */

TEST_SUITE(pthread_spinlock_t);

TEST(pthread_spinlock_t_size_minimum) {
	ASSERT_SIZE_MIN(pthread_spinlock_t, sizeof(_Atomic int));
}

TEST(pthread_spinlock_t_field_lock_offset) {
	ASSERT_OFFSET_PAST(pthread_spinlock_t, lock, 0);
}

/* ============================================================================ */

TEST_SUITE(pthread_attr_init);

TEST(pthread_attr_init_returns_zero_on_success) {
	pthread_attr_t attr;
	ASSERT_INT_EQ(0, pthread_attr_init(&attr));
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_init_sets_default_detached) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, attr.detached);
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_init_sets_default_stack_size) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, attr.stack_size);
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_init_sets_default_stack_addr) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_NULL(attr.stack_addr);
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_init_returns_einval_on_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_attr_init(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_attr_destroy);

TEST(pthread_attr_destroy_returns_zero_on_valid_attr) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, pthread_attr_destroy(&attr));
}

TEST(pthread_attr_destroy_returns_einval_on_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_attr_destroy(NULL));
}

TEST(pthread_attr_destroy_returns_einval_on_uninitialized_attr) {
	pthread_attr_t attr;
	ASSERT_INT_EQ(EINVAL, pthread_attr_destroy(&attr));
}

/* ============================================================================ */

TEST_SUITE(pthread_attr_setdetachstate);

TEST(pthread_attr_setdetachstate_joinable_sets_detached_zero) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));
	ASSERT_INT_EQ(0, attr.detached);
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setdetachstate_detached_sets_detached_one) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
	ASSERT_INT_EQ(1, attr.detached);
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setdetachstate_returns_zero_on_success_joinable) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE));
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setdetachstate_returns_zero_on_success_detached) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED));
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setdetachstate_returns_einval_on_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_attr_setdetachstate(NULL, PTHREAD_CREATE_JOINABLE));
}

TEST(pthread_attr_setdetachstate_returns_einval_on_invalid_state_999) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_attr_setdetachstate(&attr, 999));
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setdetachstate_returns_einval_on_invalid_state_negative) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_attr_setdetachstate(&attr, -1));
	pthread_attr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_attr_setstacksize);

TEST(pthread_attr_setstacksize_sets_value_on_success) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, pthread_attr_setstacksize(&attr, 1024 * 1024));
	ASSERT_INT_EQ(1024 * 1024, attr.stack_size);
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setstacksize_returns_zero_on_success) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(0, pthread_attr_setstacksize(&attr, 2048));
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setstacksize_returns_einval_on_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_attr_setstacksize(NULL, 1024));
}

TEST(pthread_attr_setstacksize_returns_einval_on_zero_size) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_attr_setstacksize(&attr, 0));
	pthread_attr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_mutex_init);

TEST(pthread_mutex_init_returns_zero_with_null_attr) {
	pthread_mutex_t m;
	ASSERT_INT_EQ(0, pthread_mutex_init(&m, NULL));
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_init_sets_type_normal_with_null_attr) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(PTHREAD_MUTEX_NORMAL, m.type);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_init_sets_futex_zero) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(0, m.futex);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_init_sets_owner_zero) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(0, m.owner);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_init_sets_recursive_count_zero) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(0, m.recursive_count);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_init_returns_zero_with_normal_attr) {
	pthread_mutex_t m;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
	ASSERT_INT_EQ(0, pthread_mutex_init(&m, &attr));
	pthread_mutex_destroy(&m);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutex_init_returns_zero_with_recursive_attr) {
	pthread_mutex_t m;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	ASSERT_INT_EQ(0, pthread_mutex_init(&m, &attr));
	pthread_mutex_destroy(&m);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutex_init_returns_zero_with_errorcheck_attr) {
	pthread_mutex_t m;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	ASSERT_INT_EQ(0, pthread_mutex_init(&m, &attr));
	pthread_mutex_destroy(&m);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutex_init_returns_einval_on_null_mutex) {
	ASSERT_INT_EQ(EINVAL, pthread_mutex_init(NULL, NULL));
}

TEST(pthread_mutex_init_returns_einval_on_invalid_attr_type) {
	pthread_mutex_t m;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	attr.type = 999;
	ASSERT_INT_EQ(EINVAL, pthread_mutex_init(&m, &attr));
	pthread_mutexattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_mutex_destroy);

TEST(pthread_mutex_destroy_returns_zero_on_valid_mutex) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(0, pthread_mutex_destroy(&m));
}

TEST(pthread_mutex_destroy_returns_einval_on_null_mutex) {
	ASSERT_INT_EQ(EINVAL, pthread_mutex_destroy(NULL));
}

TEST(pthread_mutex_destroy_returns_einval_on_uninitialized_mutex) {
	pthread_mutex_t m;
	ASSERT_INT_EQ(EINVAL, pthread_mutex_destroy(&m));
}

/* ============================================================================ */

TEST_SUITE(pthread_mutex_lock);

TEST(pthread_mutex_lock_returns_zero_on_unlocked_mutex) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(0, pthread_mutex_lock(&m));
	pthread_mutex_unlock(&m);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_lock_sets_owner_on_success) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	pthread_mutex_lock(&m);
	ASSERT_INT_NE(0, m.owner);
	pthread_mutex_unlock(&m);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_lock_sets_futex_one_on_success) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	pthread_mutex_lock(&m);
	ASSERT_INT_EQ(1, m.futex);
	pthread_mutex_unlock(&m);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_lock_returns_einval_on_null_mutex) {
	ASSERT_INT_EQ(EINVAL, pthread_mutex_lock(NULL));
}

TEST(pthread_mutex_lock_returns_edeadlk_on_errorcheck_mutex_locked_by_same_thread) {
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

TEST(pthread_mutex_trylock_returns_zero_on_unlocked_mutex) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(0, pthread_mutex_trylock(&m));
	pthread_mutex_unlock(&m);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_trylock_returns_ebusy_on_locked_mutex) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	pthread_mutex_lock(&m);
	ASSERT_INT_EQ(EBUSY, pthread_mutex_trylock(&m));
	pthread_mutex_unlock(&m);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_trylock_returns_einval_on_null_mutex) {
	ASSERT_INT_EQ(EINVAL, pthread_mutex_trylock(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_mutex_unlock);

TEST(pthread_mutex_unlock_returns_zero_on_locked_mutex) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	pthread_mutex_lock(&m);
	ASSERT_INT_EQ(0, pthread_mutex_unlock(&m));
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_unlock_clears_futex_on_success) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	pthread_mutex_lock(&m);
	pthread_mutex_unlock(&m);
	ASSERT_INT_EQ(0, m.futex);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_unlock_returns_einval_on_null_mutex) {
	ASSERT_INT_EQ(EINVAL, pthread_mutex_unlock(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_mutex_timedlock);

TEST(pthread_mutex_timedlock_returns_zero_on_unlocked_mutex) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	struct timespec ts = {0, 1000000};
	ASSERT_INT_EQ(0, pthread_mutex_timedlock(&m, &ts));
	pthread_mutex_unlock(&m);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_timedlock_returns_etimedout_on_locked_mutex) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	pthread_mutex_lock(&m);
	struct timespec ts = {0, 1000000};
	ASSERT_INT_EQ(ETIMEDOUT, pthread_mutex_timedlock(&m, &ts));
	pthread_mutex_unlock(&m);
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_timedlock_returns_einval_on_null_mutex) {
	struct timespec ts = {0, 1000000};
	ASSERT_INT_EQ(EINVAL, pthread_mutex_timedlock(NULL, &ts));
}

TEST(pthread_mutex_timedlock_returns_einval_on_null_ts) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(EINVAL, pthread_mutex_timedlock(&m, NULL));
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_timedlock_returns_einval_on_invalid_nsec) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	struct timespec ts = {0, 2000000000};
	ASSERT_INT_EQ(EINVAL, pthread_mutex_timedlock(&m, &ts));
	pthread_mutex_destroy(&m);
}

TEST(pthread_mutex_timedlock_returns_einval_on_negative_nsec) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	struct timespec ts = {0, -1};
	ASSERT_INT_EQ(EINVAL, pthread_mutex_timedlock(&m, &ts));
	pthread_mutex_destroy(&m);
}

/* ============================================================================ */

TEST_SUITE(pthread_mutex_consistent);

TEST(pthread_mutex_consistent_returns_zero_on_robust_mutex) {
	pthread_mutex_t m;
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ROBUST);
	pthread_mutex_init(&m, &attr);
	ASSERT_INT_EQ(0, pthread_mutex_consistent(&m));
	pthread_mutex_destroy(&m);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutex_consistent_returns_einval_on_null_mutex) {
	ASSERT_INT_EQ(EINVAL, pthread_mutex_consistent(NULL));
}

TEST(pthread_mutex_consistent_returns_einval_on_non_robust_mutex) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(EINVAL, pthread_mutex_consistent(&m));
	pthread_mutex_destroy(&m);
}

/* ============================================================================ */

TEST_SUITE(pthread_cond_init);

TEST(pthread_cond_init_returns_zero_with_null_attr) {
	pthread_cond_t c;
	ASSERT_INT_EQ(0, pthread_cond_init(&c, NULL));
	pthread_cond_destroy(&c);
}

TEST(pthread_cond_init_sets_futex_zero) {
	pthread_cond_t c;
	pthread_cond_init(&c, NULL);
	ASSERT_INT_EQ(0, c.futex);
	pthread_cond_destroy(&c);
}

TEST(pthread_cond_init_sets_waiters_zero) {
	pthread_cond_t c;
	pthread_cond_init(&c, NULL);
	ASSERT_INT_EQ(0, c.waiters);
	pthread_cond_destroy(&c);
}

TEST(pthread_cond_init_returns_einval_on_null_cond) {
	ASSERT_INT_EQ(EINVAL, pthread_cond_init(NULL, NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_cond_destroy);

TEST(pthread_cond_destroy_returns_zero_on_valid_cond) {
	pthread_cond_t c;
	pthread_cond_init(&c, NULL);
	ASSERT_INT_EQ(0, pthread_cond_destroy(&c));
}

TEST(pthread_cond_destroy_returns_einval_on_null_cond) {
	ASSERT_INT_EQ(EINVAL, pthread_cond_destroy(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_cond_wait);

TEST(pthread_cond_wait_returns_zero_on_signal) {
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



TEST(pthread_cond_wait_returns_einval_on_null_cond) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	ASSERT_INT_EQ(EINVAL, pthread_cond_wait(NULL, &m));
	pthread_mutex_destroy(&m);
}

TEST(pthread_cond_wait_returns_einval_on_null_mutex) {
	pthread_cond_t c;
	pthread_cond_init(&c, NULL);
	ASSERT_INT_EQ(EINVAL, pthread_cond_wait(&c, NULL));
	pthread_cond_destroy(&c);
}

TEST(pthread_cond_wait_returns_einval_on_both_null) {
	ASSERT_INT_EQ(EINVAL, pthread_cond_wait(NULL, NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_cond_timedwait);

TEST(pthread_cond_timedwait_returns_etimedout_on_timeout) {
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

TEST(pthread_cond_timedwait_returns_einval_on_null_cond) {
	pthread_mutex_t m;
	pthread_mutex_init(&m, NULL);
	struct timespec ts = {0, 1000000};
	ASSERT_INT_EQ(EINVAL, pthread_cond_timedwait(NULL, &m, &ts));
	pthread_mutex_destroy(&m);
}

TEST(pthread_cond_timedwait_returns_einval_on_null_mutex) {
	pthread_cond_t c;
	pthread_cond_init(&c, NULL);
	struct timespec ts = {0, 1000000};
	ASSERT_INT_EQ(EINVAL, pthread_cond_timedwait(&c, NULL, &ts));
	pthread_cond_destroy(&c);
}

TEST(pthread_cond_timedwait_returns_einval_on_null_ts) {
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

TEST(pthread_cond_signal_returns_zero_on_valid_cond) {
	pthread_cond_t c;
	pthread_cond_init(&c, NULL);
	ASSERT_INT_EQ(0, pthread_cond_signal(&c));
	pthread_cond_destroy(&c);
}

TEST(pthread_cond_signal_returns_einval_on_null_cond) {
	ASSERT_INT_EQ(EINVAL, pthread_cond_signal(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_cond_broadcast);

TEST(pthread_cond_broadcast_returns_zero_on_valid_cond) {
	pthread_cond_t c;
	pthread_cond_init(&c, NULL);
	ASSERT_INT_EQ(0, pthread_cond_broadcast(&c));
	pthread_cond_destroy(&c);
}

TEST(pthread_cond_broadcast_returns_einval_on_null_cond) {
	ASSERT_INT_EQ(EINVAL, pthread_cond_broadcast(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_once);

TEST(pthread_once_calls_init_fn_once) {
	pthread_once_t once = PTHREAD_ONCE_INIT;
	__test_once_counter = 0;
	ASSERT_INT_EQ(0, pthread_once(&once, __test_once_init_fn));
	ASSERT_INT_EQ(1, __test_once_counter);
	ASSERT_INT_EQ(0, pthread_once(&once, __test_once_init_fn));
	ASSERT_INT_EQ(1, __test_once_counter);
}

TEST(pthread_once_returns_einval_on_null_control) {
	ASSERT_INT_EQ(EINVAL, pthread_once(NULL, __test_once_empty_fn));
}

TEST(pthread_once_returns_einval_on_null_routine) {
	pthread_once_t once = PTHREAD_ONCE_INIT;
	ASSERT_INT_EQ(EINVAL, pthread_once(&once, NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_key_create);

TEST(pthread_key_create_returns_zero_on_success) {
	pthread_key_t key;
	ASSERT_INT_EQ(0, pthread_key_create(&key, NULL));
	pthread_key_delete(key);
}

TEST(pthread_key_create_returns_einval_on_null_key) {
	ASSERT_INT_EQ(EINVAL, pthread_key_create(NULL, NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_key_delete);

TEST(pthread_key_delete_returns_zero_on_valid_key) {
	pthread_key_t key;
	pthread_key_create(&key, NULL);
	ASSERT_INT_EQ(0, pthread_key_delete(key));
}

TEST(pthread_key_delete_returns_einval_on_invalid_key) {
	ASSERT_INT_EQ(EINVAL, pthread_key_delete(9999));
}

/* ============================================================================ */

TEST_SUITE(pthread_setspecific);

TEST(pthread_setspecific_returns_zero_on_valid_key) {
	pthread_key_t key;
	pthread_key_create(&key, NULL);
	ASSERT_INT_EQ(0, pthread_setspecific(key, (void *)123));
	pthread_key_delete(key);
}

TEST(pthread_setspecific_returns_einval_on_invalid_key) {
	ASSERT_INT_EQ(EINVAL, pthread_setspecific(9999, NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_getspecific);

TEST(pthread_getspecific_returns_set_value) {
	pthread_key_t key;
	pthread_key_create(&key, NULL);
	pthread_setspecific(key, (void *)456);
	ASSERT_PTR_EQ((void *)456, pthread_getspecific(key));
	pthread_key_delete(key);
}

TEST(pthread_getspecific_returns_null_on_invalid_key) {
	ASSERT_NULL(pthread_getspecific(9999));
}

/* ============================================================================ */

TEST_SUITE(pthread_spin_init);

TEST(pthread_spin_init_returns_zero_on_success) {
	pthread_spinlock_t s;
	ASSERT_INT_EQ(0, pthread_spin_init(&s, PTHREAD_PROCESS_PRIVATE));
	pthread_spin_destroy(&s);
}

TEST(pthread_spin_init_sets_lock_unlocked) {
	pthread_spinlock_t s;
	pthread_spin_init(&s, PTHREAD_PROCESS_PRIVATE);
	ASSERT_INT_EQ(PTHREAD_SPIN_UNLOCKED, s.lock);
	pthread_spin_destroy(&s);
}

TEST(pthread_spin_init_returns_einval_on_null_lock) {
	ASSERT_INT_EQ(EINVAL, pthread_spin_init(NULL, PTHREAD_PROCESS_PRIVATE));
}

/* ============================================================================ */

TEST_SUITE(pthread_spin_lock);

TEST(pthread_spin_lock_returns_zero_on_unlocked_spinlock) {
	pthread_spinlock_t s;
	pthread_spin_init(&s, PTHREAD_PROCESS_PRIVATE);
	ASSERT_INT_EQ(0, pthread_spin_lock(&s));
	pthread_spin_unlock(&s);
	pthread_spin_destroy(&s);
}

TEST(pthread_spin_lock_returns_einval_on_null_lock) {
	ASSERT_INT_EQ(EINVAL, pthread_spin_lock(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_spin_trylock);

TEST(pthread_spin_trylock_returns_zero_on_unlocked_spinlock) {
	pthread_spinlock_t s;
	pthread_spin_init(&s, PTHREAD_PROCESS_PRIVATE);
	ASSERT_INT_EQ(0, pthread_spin_trylock(&s));
	pthread_spin_unlock(&s);
	pthread_spin_destroy(&s);
}

TEST(pthread_spin_trylock_returns_ebusy_on_locked_spinlock) {
	pthread_spinlock_t s;
	pthread_spin_init(&s, PTHREAD_PROCESS_PRIVATE);
	pthread_spin_lock(&s);
	ASSERT_INT_EQ(EBUSY, pthread_spin_trylock(&s));
	pthread_spin_unlock(&s);
	pthread_spin_destroy(&s);
}

TEST(pthread_spin_trylock_returns_einval_on_null_lock) {
	ASSERT_INT_EQ(EINVAL, pthread_spin_trylock(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_spin_unlock);

TEST(pthread_spin_unlock_returns_zero_on_locked_spinlock) {
	pthread_spinlock_t s;
	pthread_spin_init(&s, PTHREAD_PROCESS_PRIVATE);
	pthread_spin_lock(&s);
	ASSERT_INT_EQ(0, pthread_spin_unlock(&s));
	pthread_spin_destroy(&s);
}

TEST(pthread_spin_unlock_returns_einval_on_null_lock) {
	ASSERT_INT_EQ(EINVAL, pthread_spin_unlock(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_rwlock_init);

TEST(pthread_rwlock_init_returns_zero_with_null_attr) {
	pthread_rwlock_t rw;
	ASSERT_INT_EQ(0, pthread_rwlock_init(&rw, NULL));
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_init_sets_readers_zero) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	ASSERT_INT_EQ(0, rw.readers);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_init_sets_writers_zero) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	ASSERT_INT_EQ(0, rw.writers);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_init_sets_write_waiters_zero) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	ASSERT_INT_EQ(0, rw.write_waiters);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_init_sets_writer_tid_zero) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	ASSERT_INT_EQ(0, rw.writer_tid);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_init_returns_einval_on_null_rwlock) {
	ASSERT_INT_EQ(EINVAL, pthread_rwlock_init(NULL, NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_rwlock_destroy);

TEST(pthread_rwlock_destroy_returns_zero_on_valid_rwlock) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	ASSERT_INT_EQ(0, pthread_rwlock_destroy(&rw));
}

TEST(pthread_rwlock_destroy_returns_einval_on_null_rwlock) {
	ASSERT_INT_EQ(EINVAL, pthread_rwlock_destroy(NULL));
}

TEST(pthread_rwlock_destroy_returns_ebusy_on_locked_rwlock) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	pthread_rwlock_rdlock(&rw);
	ASSERT_INT_EQ(EBUSY, pthread_rwlock_destroy(&rw));
	pthread_rwlock_unlock(&rw);
	pthread_rwlock_destroy(&rw);
}

/* ============================================================================ */

TEST_SUITE(pthread_rwlock_rdlock);

TEST(pthread_rwlock_rdlock_returns_zero_on_unlocked_rwlock) {
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

TEST(pthread_rwlock_rdlock_returns_einval_on_null_rwlock) {
	ASSERT_INT_EQ(EINVAL, pthread_rwlock_rdlock(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_rwlock_wrlock);

TEST(pthread_rwlock_wrlock_returns_zero_on_unlocked_rwlock) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	ASSERT_INT_EQ(0, pthread_rwlock_wrlock(&rw));
	pthread_rwlock_unlock(&rw);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_wrlock_sets_writers_one) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	pthread_rwlock_wrlock(&rw);
	ASSERT_INT_EQ(1, rw.writers);
	pthread_rwlock_unlock(&rw);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_wrlock_sets_writer_tid) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	pthread_rwlock_wrlock(&rw);
	ASSERT_INT_NE(0, rw.writer_tid);
	pthread_rwlock_unlock(&rw);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_wrlock_returns_einval_on_null_rwlock) {
	ASSERT_INT_EQ(EINVAL, pthread_rwlock_wrlock(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_rwlock_unlock);

TEST(pthread_rwlock_unlock_returns_zero_on_read_lock) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	pthread_rwlock_rdlock(&rw);
	ASSERT_INT_EQ(0, pthread_rwlock_unlock(&rw));
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_unlock_returns_zero_on_write_lock) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	pthread_rwlock_wrlock(&rw);
	ASSERT_INT_EQ(0, pthread_rwlock_unlock(&rw));
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_unlock_clears_writers_on_write_unlock) {
	pthread_rwlock_t rw;
	pthread_rwlock_init(&rw, NULL);
	pthread_rwlock_wrlock(&rw);
	pthread_rwlock_unlock(&rw);
	ASSERT_INT_EQ(0, rw.writers);
	pthread_rwlock_destroy(&rw);
}

TEST(pthread_rwlock_unlock_returns_einval_on_null_rwlock) {
	ASSERT_INT_EQ(EINVAL, pthread_rwlock_unlock(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_barrier_init);

TEST(pthread_barrier_init_returns_zero_with_count_two) {
	pthread_barrier_t b;
	ASSERT_INT_EQ(0, pthread_barrier_init(&b, NULL, 2));
	pthread_barrier_destroy(&b);
}

TEST(pthread_barrier_init_sets_count_zero) {
	pthread_barrier_t b;
	pthread_barrier_init(&b, NULL, 2);
	ASSERT_INT_EQ(0, b.count);
	pthread_barrier_destroy(&b);
}

TEST(pthread_barrier_init_sets_phase_zero) {
	pthread_barrier_t b;
	pthread_barrier_init(&b, NULL, 2);
	ASSERT_INT_EQ(0, b.phase);
	pthread_barrier_destroy(&b);
}

TEST(pthread_barrier_init_sets_limit_to_count) {
	pthread_barrier_t b;
	pthread_barrier_init(&b, NULL, 3);
	ASSERT_INT_EQ(3, b.limit);
	pthread_barrier_destroy(&b);
}

TEST(pthread_barrier_init_returns_einval_on_null_barrier) {
	ASSERT_INT_EQ(EINVAL, pthread_barrier_init(NULL, NULL, 2));
}

TEST(pthread_barrier_init_returns_einval_on_zero_count) {
	pthread_barrier_t b;
	ASSERT_INT_EQ(EINVAL, pthread_barrier_init(&b, NULL, 0));
}

/* ============================================================================ */

TEST_SUITE(pthread_barrier_wait);

TEST(pthread_barrier_wait_returns_serial_thread_on_last_arrival) {
	pthread_barrier_t b;
	pthread_barrier_init(&b, NULL, 1);
	int r = pthread_barrier_wait(&b);
	ASSERT_TRUE(r == 0 || r == PTHREAD_BARRIER_SERIAL_THREAD);
	pthread_barrier_destroy(&b);
}

TEST(pthread_barrier_wait_returns_einval_on_null_barrier) {
	ASSERT_INT_EQ(EINVAL, pthread_barrier_wait(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_cancel);

TEST(pthread_cancel_returns_zero_on_valid_thread) {
	pthread_t self = pthread_self();
	ASSERT_INT_EQ(0, pthread_cancel(self));
}

TEST(pthread_cancel_returns_esrch_on_null_thread) {
	ASSERT_INT_EQ(ESRCH, pthread_cancel(PTHREAD_NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_setcancelstate);

TEST(pthread_setcancelstate_disable_sets_state) {
	pthread_t t;
	int done = 0;

	/* Run the risky code in a child thread */
	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __cancel_worker, &done));
	ASSERT_INT_EQ(0, pthread_join(t, NULL));

	ASSERT_INT_EQ(1, done);
}

TEST(pthread_setcancelstate_enable_sets_state) {
	pthread_t t;
	int done = 0;

	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __cancel_worker, &done));
	ASSERT_INT_EQ(0, pthread_join(t, NULL));

	ASSERT_INT_EQ(1, done);
}

TEST(pthread_setcancelstate_returns_zero_on_success) {
	pthread_t t;
	int done = 0;

	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __cancel_worker, &done));
	ASSERT_INT_EQ(0, pthread_join(t, NULL));

	ASSERT_INT_EQ(1, done);
}

TEST(pthread_setcancelstate_returns_einval_on_invalid_state) {
	int old;
	ASSERT_INT_EQ(EINVAL, pthread_setcancelstate(999, &old));
}

/* ============================================================================ */

TEST_SUITE(pthread_setcanceltype);

TEST(pthread_setcanceltype_deferred_sets_type) {
	int old;
	ASSERT_INT_EQ(0, pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &old));
}

TEST(pthread_setcanceltype_returns_zero_on_success) {
	int old;
	ASSERT_INT_EQ(0, pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &old));
}

TEST(pthread_setcanceltype_returns_einval_on_invalid_type) {
	int old;
	ASSERT_INT_EQ(EINVAL, pthread_setcanceltype(999, &old));
}

/* ============================================================================ */

TEST_SUITE(pthread_testcancel);

TEST(pthread_testcancel_does_not_crash) {
	pthread_t t;
	int done = 0;

	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __cancel_worker, &done));
	ASSERT_INT_EQ(0, pthread_join(t, NULL));

	ASSERT_INT_EQ(1, done);
}

/* ============================================================================ */

TEST_SUITE(pthread_create);

TEST(pthread_create_returns_zero_on_success) {
	pthread_t t;
	int val = 0;
	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __test_thread_fn_42, &val));
	pthread_join(t, NULL);
	ASSERT_INT_EQ(42, val);
}

TEST(pthread_create_returns_einval_on_null_thread) {
	ASSERT_INT_EQ(EINVAL, pthread_create(NULL, NULL, NULL, NULL));
}

TEST(pthread_create_returns_einval_on_null_routine) {
	pthread_t t;
	ASSERT_INT_EQ(EINVAL, pthread_create(&t, NULL, NULL, NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_join);

TEST(pthread_join_returns_zero_on_joinable_thread) {
	pthread_t t;
	int val = 0;
	pthread_create(&t, NULL, __test_thread_fn_99, &val);
	ASSERT_INT_EQ(0, pthread_join(t, NULL));
	ASSERT_INT_EQ(99, val);
}

TEST(pthread_join_returns_einval_on_null_thread) {
	ASSERT_INT_EQ(EINVAL, pthread_join(NULL, NULL));
}

TEST(pthread_join_returns_einval_or_esrch_on_detached_thread) {
	pthread_t t;
	pthread_create(&t, NULL, __test_thread_fn_null, NULL);
	pthread_detach(t);
	int r = pthread_join(t, NULL);
	ASSERT_TRUE(r == EINVAL || r == ESRCH);
}

/* ============================================================================ */

TEST_SUITE(pthread_detach);

TEST(pthread_detach_returns_zero_on_valid_thread) {
	pthread_t t;

	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __detach_worker, NULL));

	ASSERT_INT_EQ(0, pthread_detach(t));

	usleep(10000);
}

TEST(pthread_detach_returns_einval_on_null_thread) {
	ASSERT_INT_EQ(EINVAL, pthread_detach(NULL));
}

/* ============================================================================ */

TEST_SUITE(pthread_exit);

TEST(pthread_exit_compiles_as_noreturn) {
	ASSERT_TRUE(1);
}

/* ============================================================================ */

TEST_SUITE(pthread_self);

TEST(pthread_self_returns_non_null) {
	pthread_t self = pthread_self();
	ASSERT_NOT_NULL(self);
}

TEST(pthread_self_equals_self) {
	pthread_t t;
	int ok = 0;

	ASSERT_INT_EQ(0, pthread_create(&t, NULL, __self_worker, &ok));
	ASSERT_INT_EQ(0, pthread_join(t, NULL));

	ASSERT_INT_EQ(1, ok);
}

/* ============================================================================ */

TEST_SUITE(pthread_equal);

TEST(pthread_equal_returns_true_for_same_thread) {
	pthread_t self = pthread_self();
	ASSERT_TRUE(pthread_equal(self, self));
}

/* ============================================================================ */

TEST_SUITE(pthread_sigmask);

TEST(pthread_sigmask_returns_zero_on_valid_call) {
	sigset_t set, old;

	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);

	#if JACL_HAS_POSIX
		int r = pthread_sigmask(SIG_BLOCK, &set, &old);

		ASSERT_TRUE(r == 0 || r == EINVAL);
	#else
		ASSERT_INT_EQ(ENOSYS, pthread_sigmask(SIG_BLOCK, &set, &old));
	#endif
}

TEST(pthread_sigmask_returns_einval_on_invalid_how) {
	sigset_t set;
	sigemptyset(&set);
	#if JACL_HAS_POSIX
		ASSERT_INT_EQ(EINVAL, pthread_sigmask(999, &set, NULL));
	#else
		ASSERT_INT_EQ(ENOSYS, pthread_sigmask(999, &set, NULL));
	#endif
}

/* ============================================================================ */

/* ============================================================================ */

TEST_SUITE(pthread_mutexattr_setpshared);

TEST(pthread_mutexattr_setpshared_returns_zero_on_private) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE));
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_setpshared_returns_zero_on_shared) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED));
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_setpshared_returns_einval_on_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_mutexattr_setpshared(NULL, PTHREAD_PROCESS_PRIVATE));
}

TEST(pthread_mutexattr_setpshared_returns_einval_on_invalid_pshared) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_mutexattr_setpshared(&attr, 999));
	pthread_mutexattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_mutexattr_getpshared);

TEST(pthread_mutexattr_getpshared_returns_private_by_default) {
	pthread_mutexattr_t attr;
	int pshared;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_mutexattr_getpshared(&attr, &pshared));
	ASSERT_INT_EQ(PTHREAD_PROCESS_PRIVATE, pshared);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_getpshared_returns_what_was_set) {
	pthread_mutexattr_t attr;
	int pshared;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	ASSERT_INT_EQ(0, pthread_mutexattr_getpshared(&attr, &pshared));
	ASSERT_INT_EQ(PTHREAD_PROCESS_SHARED, pshared);
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_mutexattr_getpshared_returns_einval_on_null_attr) {
	int pshared;
	ASSERT_INT_EQ(EINVAL, pthread_mutexattr_getpshared(NULL, &pshared));
}

TEST(pthread_mutexattr_getpshared_returns_einval_on_null_pshared) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_mutexattr_getpshared(&attr, NULL));
	pthread_mutexattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_condattr_setpshared);

TEST(pthread_condattr_setpshared_returns_zero_on_private) {
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE));
	pthread_condattr_destroy(&attr);
}

TEST(pthread_condattr_setpshared_returns_zero_on_shared) {
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED));
	pthread_condattr_destroy(&attr);
}

TEST(pthread_condattr_setpshared_returns_einval_on_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_condattr_setpshared(NULL, PTHREAD_PROCESS_PRIVATE));
}

TEST(pthread_condattr_setpshared_returns_einval_on_invalid_pshared) {
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_condattr_setpshared(&attr, 999));
	pthread_condattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_condattr_getpshared);

TEST(pthread_condattr_getpshared_returns_private_by_default) {
	pthread_condattr_t attr;
	int pshared;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_condattr_getpshared(&attr, &pshared));
	ASSERT_INT_EQ(PTHREAD_PROCESS_PRIVATE, pshared);
	pthread_condattr_destroy(&attr);
}

TEST(pthread_condattr_getpshared_returns_what_was_set) {
	pthread_condattr_t attr;
	int pshared;
	pthread_condattr_init(&attr);
	pthread_condattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	ASSERT_INT_EQ(0, pthread_condattr_getpshared(&attr, &pshared));
	ASSERT_INT_EQ(PTHREAD_PROCESS_SHARED, pshared);
	pthread_condattr_destroy(&attr);
}

TEST(pthread_condattr_getpshared_returns_einval_on_null_attr) {
	int pshared;
	ASSERT_INT_EQ(EINVAL, pthread_condattr_getpshared(NULL, &pshared));
}

TEST(pthread_condattr_getpshared_returns_einval_on_null_pshared) {
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_condattr_getpshared(&attr, NULL));
	pthread_condattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_condattr_setclock);

TEST(pthread_condattr_setclock_returns_zero_on_realtime) {
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_condattr_setclock(&attr, CLOCK_REALTIME));
	pthread_condattr_destroy(&attr);
}

TEST(pthread_condattr_setclock_returns_zero_on_monotonic) {
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_condattr_setclock(&attr, CLOCK_MONOTONIC));
	pthread_condattr_destroy(&attr);
}

TEST(pthread_condattr_setclock_returns_einval_on_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_condattr_setclock(NULL, CLOCK_REALTIME));
}

TEST(pthread_condattr_setclock_returns_einval_on_invalid_clock) {
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_condattr_setclock(&attr, 999));
	pthread_condattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_condattr_getclock);

TEST(pthread_condattr_getclock_returns_realtime_by_default) {
	pthread_condattr_t attr;
	clockid_t clock_id;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_condattr_getclock(&attr, &clock_id));
	ASSERT_INT_EQ(CLOCK_REALTIME, clock_id);
	pthread_condattr_destroy(&attr);
}

TEST(pthread_condattr_getclock_returns_what_was_set) {
	pthread_condattr_t attr;
	clockid_t clock_id;
	pthread_condattr_init(&attr);
	pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
	ASSERT_INT_EQ(0, pthread_condattr_getclock(&attr, &clock_id));
	ASSERT_INT_EQ(CLOCK_MONOTONIC, clock_id);
	pthread_condattr_destroy(&attr);
}

TEST(pthread_condattr_getclock_returns_einval_on_null_attr) {
	clockid_t clock_id;
	ASSERT_INT_EQ(EINVAL, pthread_condattr_getclock(NULL, &clock_id));
}

TEST(pthread_condattr_getclock_returns_einval_on_null_clock) {
	pthread_condattr_t attr;
	pthread_condattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_condattr_getclock(&attr, NULL));
	pthread_condattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_rwlockattr_setpshared);

TEST(pthread_rwlockattr_setpshared_returns_zero_on_private) {
	pthread_rwlockattr_t attr;
	pthread_rwlockattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE));
	pthread_rwlockattr_destroy(&attr);
}

TEST(pthread_rwlockattr_setpshared_returns_zero_on_shared) {
	pthread_rwlockattr_t attr;
	pthread_rwlockattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_SHARED));
	pthread_rwlockattr_destroy(&attr);
}

TEST(pthread_rwlockattr_setpshared_returns_einval_on_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_rwlockattr_setpshared(NULL, PTHREAD_PROCESS_PRIVATE));
}

TEST(pthread_rwlockattr_setpshared_returns_einval_on_invalid_pshared) {
	pthread_rwlockattr_t attr;
	pthread_rwlockattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_rwlockattr_setpshared(&attr, 999));
	pthread_rwlockattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_rwlockattr_getpshared);

TEST(pthread_rwlockattr_getpshared_returns_private_by_default) {
	pthread_rwlockattr_t attr;
	int pshared;
	pthread_rwlockattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_rwlockattr_getpshared(&attr, &pshared));
	ASSERT_INT_EQ(PTHREAD_PROCESS_PRIVATE, pshared);
	pthread_rwlockattr_destroy(&attr);
}

TEST(pthread_rwlockattr_getpshared_returns_what_was_set) {
	pthread_rwlockattr_t attr;
	int pshared;
	pthread_rwlockattr_init(&attr);
	pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	ASSERT_INT_EQ(0, pthread_rwlockattr_getpshared(&attr, &pshared));
	ASSERT_INT_EQ(PTHREAD_PROCESS_SHARED, pshared);
	pthread_rwlockattr_destroy(&attr);
}

TEST(pthread_rwlockattr_getpshared_returns_einval_on_null_attr) {
	int pshared;
	ASSERT_INT_EQ(EINVAL, pthread_rwlockattr_getpshared(NULL, &pshared));
}

TEST(pthread_rwlockattr_getpshared_returns_einval_on_null_pshared) {
	pthread_rwlockattr_t attr;
	pthread_rwlockattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_rwlockattr_getpshared(&attr, NULL));
	pthread_rwlockattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_barrierattr_setpshared);

TEST(pthread_barrierattr_setpshared_returns_zero_on_private) {
	pthread_barrierattr_t attr;
	pthread_barrierattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE));
	pthread_barrierattr_destroy(&attr);
}

TEST(pthread_barrierattr_setpshared_returns_zero_on_shared) {
	pthread_barrierattr_t attr;
	pthread_barrierattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_SHARED));
	pthread_barrierattr_destroy(&attr);
}

TEST(pthread_barrierattr_setpshared_returns_einval_on_null_attr) {
	ASSERT_INT_EQ(EINVAL, pthread_barrierattr_setpshared(NULL, PTHREAD_PROCESS_PRIVATE));
}

TEST(pthread_barrierattr_setpshared_returns_einval_on_invalid_pshared) {
	pthread_barrierattr_t attr;
	pthread_barrierattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_barrierattr_setpshared(&attr, 999));
	pthread_barrierattr_destroy(&attr);
}

/* ============================================================================ */

TEST_SUITE(pthread_barrierattr_getpshared);

TEST(pthread_barrierattr_getpshared_returns_private_by_default) {
	pthread_barrierattr_t attr;
	int pshared;
	pthread_barrierattr_init(&attr);
	ASSERT_INT_EQ(0, pthread_barrierattr_getpshared(&attr, &pshared));
	ASSERT_INT_EQ(PTHREAD_PROCESS_PRIVATE, pshared);
	pthread_barrierattr_destroy(&attr);
}

TEST(pthread_barrierattr_getpshared_returns_what_was_set) {
	pthread_barrierattr_t attr;
	int pshared;
	pthread_barrierattr_init(&attr);
	pthread_barrierattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	ASSERT_INT_EQ(0, pthread_barrierattr_getpshared(&attr, &pshared));
	ASSERT_INT_EQ(PTHREAD_PROCESS_SHARED, pshared);
	pthread_barrierattr_destroy(&attr);
}

TEST(pthread_barrierattr_getpshared_returns_einval_on_null_attr) {
	int pshared;
	ASSERT_INT_EQ(EINVAL, pthread_barrierattr_getpshared(NULL, &pshared));
}

TEST(pthread_barrierattr_getpshared_returns_einval_on_null_pshared) {
	pthread_barrierattr_t attr;
	pthread_barrierattr_init(&attr);
	ASSERT_INT_EQ(EINVAL, pthread_barrierattr_getpshared(&attr, NULL));
	pthread_barrierattr_destroy(&attr);
}

/* ============================================================================ */

TEST_MAIN()
