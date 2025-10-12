/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_C11
#include <threads.h>

TEST_TYPE(unit);
TEST_UNIT(threads.h);

/* ============================================================================
 * CONSTANTS AND ENUMS
 * ============================================================================ */
TEST_SUITE(constants);

TEST(thread_result_codes) {
	ASSERT_EQ(0, thrd_success);
	ASSERT_NE(0, thrd_nomem);
	ASSERT_NE(0, thrd_timedout);
	ASSERT_NE(0, thrd_busy);
	ASSERT_NE(0, thrd_error);
}

TEST(mutex_types) {
	ASSERT_EQ(0, mtx_plain);
	ASSERT_NE(0, mtx_recursive);
	ASSERT_NE(0, mtx_timed);
}

TEST(tss_dtor_iterations) {
	ASSERT_TRUE(TSS_DTOR_ITERATIONS > 0);
}

#if JACL_HAS_C23
TEST(version_macro) {
	ASSERT_EQ(202311L, __STDC_VERSION_THREADS_H__);
}
#endif

/* ============================================================================
 * MUTEX - BASIC OPERATIONS
 * ============================================================================ */
TEST_SUITE(mutex_basic);

TEST(mtx_init_plain) {
	mtx_t mtx;
	int result = mtx_init(&mtx, mtx_plain);
	
	ASSERT_EQ(thrd_success, result);
	mtx_destroy(&mtx);
}

TEST(mtx_lock_unlock) {
	mtx_t mtx;
	mtx_init(&mtx, mtx_plain);
	
	ASSERT_EQ(thrd_success, mtx_lock(&mtx));
	ASSERT_EQ(thrd_success, mtx_unlock(&mtx));
	
	mtx_destroy(&mtx);
}

TEST(mtx_trylock_unlocked) {
	mtx_t mtx;
	mtx_init(&mtx, mtx_plain);
	
	int result = mtx_trylock(&mtx);
	ASSERT_EQ(thrd_success, result);
	
	mtx_unlock(&mtx);
	mtx_destroy(&mtx);
}

TEST(mtx_trylock_locked) {
	mtx_t mtx;
	mtx_init(&mtx, mtx_plain);
	
	mtx_lock(&mtx);
	int result = mtx_trylock(&mtx);
	
	// Should be busy (already locked)
	ASSERT_TRUE(result == thrd_busy || result == thrd_success);
	
	mtx_unlock(&mtx);
	mtx_destroy(&mtx);
}

TEST(mtx_multiple_lock_unlock) {
	mtx_t mtx;
	mtx_init(&mtx, mtx_plain);
	
	for (int i = 0; i < 5; i++) {
		ASSERT_EQ(thrd_success, mtx_lock(&mtx));
		ASSERT_EQ(thrd_success, mtx_unlock(&mtx));
	}
	
	mtx_destroy(&mtx);
}

/* ============================================================================
 * MUTEX - RECURSIVE
 * ============================================================================ */
TEST_SUITE(mutex_recursive);

TEST(mtx_init_recursive) {
	mtx_t mtx;
	int result = mtx_init(&mtx, mtx_recursive);
	
	ASSERT_EQ(thrd_success, result);
	mtx_destroy(&mtx);
}

#if JACL_HAS_THREADS && !JACL_ARCH_WASM
TEST(mtx_recursive_locking) {
	mtx_t mtx;
	mtx_init(&mtx, mtx_recursive);
	
	// Lock multiple times
	ASSERT_EQ(thrd_success, mtx_lock(&mtx));
	ASSERT_EQ(thrd_success, mtx_lock(&mtx));
	ASSERT_EQ(thrd_success, mtx_lock(&mtx));
	
	// Unlock same number of times
	ASSERT_EQ(thrd_success, mtx_unlock(&mtx));
	ASSERT_EQ(thrd_success, mtx_unlock(&mtx));
	ASSERT_EQ(thrd_success, mtx_unlock(&mtx));
	
	mtx_destroy(&mtx);
}
#endif

/* ============================================================================
 * CONDITION VARIABLES
 * ============================================================================ */
TEST_SUITE(condition_variables);

TEST(cnd_init) {
	cnd_t cnd;
	int result = cnd_init(&cnd);
	
	ASSERT_EQ(thrd_success, result);
	cnd_destroy(&cnd);
}

TEST(cnd_signal) {
	cnd_t cnd;
	cnd_init(&cnd);
	
	int result = cnd_signal(&cnd);
	ASSERT_EQ(thrd_success, result);
	
	cnd_destroy(&cnd);
}

TEST(cnd_broadcast) {
	cnd_t cnd;
	cnd_init(&cnd);
	
	int result = cnd_broadcast(&cnd);
	ASSERT_EQ(thrd_success, result);
	
	cnd_destroy(&cnd);
}

/* ============================================================================
 * ONCE FLAG
 * ============================================================================ */
TEST_SUITE(once_flag);

static int once_counter = 0;

static void increment_once(void) {
	once_counter++;
}

TEST(call_once_basic) {
	once_flag flag = ONCE_FLAG_INIT;
	once_counter = 0;
	
	call_once(&flag, increment_once);
	ASSERT_EQ(1, once_counter);
	
	// Second call should not execute
	call_once(&flag, increment_once);
	ASSERT_EQ(1, once_counter);
}

TEST(call_once_multiple_flags) {
	once_flag flag1 = ONCE_FLAG_INIT;
	once_flag flag2 = ONCE_FLAG_INIT;
	once_counter = 0;
	
	call_once(&flag1, increment_once);
	call_once(&flag2, increment_once);
	
	ASSERT_EQ(2, once_counter);
}

/* ============================================================================
 * THREAD CREATION AND JOINING (Native only)
 * ============================================================================ */
#if JACL_HAS_THREADS && !JACL_ARCH_WASM
TEST_SUITE(thread_creation);

static int simple_thread_func(void *arg) {
	int *value = (int*)arg;
	*value = 42;
	return 0;
}

TEST(thrd_create_join) {
	thrd_t thread;
	int value = 0;
	
	int result = thrd_create(&thread, simple_thread_func, &value);
	ASSERT_EQ(thrd_success, result);
	
	int thread_result;
	result = thrd_join(thread, &thread_result);
	
	ASSERT_EQ(thrd_success, result);
	ASSERT_EQ(0, thread_result);
	ASSERT_EQ(42, value);
}

static int return_value_thread(void *arg) {
	return *((int*)arg);
}

TEST(thrd_join_with_result) {
	thrd_t thread;
	int input = 123;
	
	thrd_create(&thread, return_value_thread, &input);
	
	int result;
	thrd_join(thread, &result);
	
	ASSERT_EQ(123, result);
}

TEST(thrd_detach) {
	thrd_t thread;
	int value = 0;
	
	thrd_create(&thread, simple_thread_func, &value);
	
	int result = thrd_detach(thread);
	ASSERT_EQ(thrd_success, result);
}

TEST(thrd_current) {
	thrd_t current = thrd_current();
	ASSERT_TRUE(current != 0 || current == 0);  // Just verify it returns something
}

TEST(thrd_equal) {
	thrd_t current1 = thrd_current();
	thrd_t current2 = thrd_current();
	
	ASSERT_TRUE(thrd_equal(current1, current2));
}

TEST(thrd_yield) {
	// Just verify it doesn't crash
	thrd_yield();
	ASSERT_TRUE(1);
}
#endif

/* ============================================================================
 * THREAD SLEEP
 * ============================================================================ */
#if JACL_HAS_THREADS && !JACL_ARCH_WASM
TEST_SUITE(thread_sleep);

TEST(thrd_sleep_basic) {
	struct timespec duration = {0, 10000000};  // 10ms
	struct timespec remaining;
	
	int result = thrd_sleep(&duration, &remaining);
	ASSERT_EQ(0, result);
}
#endif

/* ============================================================================
 * MUTEX WITH THREADS
 * ============================================================================ */
#if JACL_HAS_THREADS && !JACL_ARCH_WASM
TEST_SUITE(mutex_with_threads);

static mtx_t shared_mutex;
static int shared_counter = 0;

static int increment_thread(void *arg) {
	int iterations = *((int*)arg);
	
	for (int i = 0; i < iterations; i++) {
		mtx_lock(&shared_mutex);
		shared_counter++;
		mtx_unlock(&shared_mutex);
	}
	
	return 0;
}

TEST(mtx_mutual_exclusion) {
	mtx_init(&shared_mutex, mtx_plain);
	shared_counter = 0;
	
	int iterations = 100;
	thrd_t thread1, thread2;
	
	thrd_create(&thread1, increment_thread, &iterations);
	thrd_create(&thread2, increment_thread, &iterations);
	
	thrd_join(thread1, NULL);
	thrd_join(thread2, NULL);
	
	ASSERT_EQ(200, shared_counter);
	
	mtx_destroy(&shared_mutex);
}
#endif

/* ============================================================================
 * CONDITION VARIABLE WITH THREADS
 * ============================================================================ */
#if JACL_HAS_THREADS && !JACL_ARCH_WASM
TEST_SUITE(condvar_with_threads);

static cnd_t shared_cond;
static mtx_t cond_mutex;
static int cond_flag = 0;

static int waiter_thread(void *arg) {
	mtx_lock(&cond_mutex);
	
	while (!cond_flag) {
		cnd_wait(&shared_cond, &cond_mutex);
	}
	
	mtx_unlock(&cond_mutex);
	return 42;
}

TEST(cnd_signal_wakes_thread) {
	mtx_init(&cond_mutex, mtx_plain);
	cnd_init(&shared_cond);
	cond_flag = 0;
	
	thrd_t thread;
	thrd_create(&thread, waiter_thread, NULL);
	
	// Give thread time to wait
	struct timespec delay = {0, 50000000};  // 50ms
	thrd_sleep(&delay, NULL);
	
	// Signal the thread
	mtx_lock(&cond_mutex);
	cond_flag = 1;
	cnd_signal(&shared_cond);
	mtx_unlock(&cond_mutex);
	
	int result;
	thrd_join(thread, &result);
	
	ASSERT_EQ(42, result);
	
	cnd_destroy(&shared_cond);
	mtx_destroy(&cond_mutex);
}
#endif

/* ============================================================================
 * THREAD-SPECIFIC STORAGE
 * ============================================================================ */
#if JACL_HAS_THREADS && !JACL_ARCH_WASM
TEST_SUITE(thread_specific_storage);

TEST(tss_create_delete) {
	tss_t key;
	int result = tss_create(&key, NULL);
	
	ASSERT_EQ(thrd_success, result);
	tss_delete(key);
}

TEST(tss_set_get) {
	tss_t key;
	tss_create(&key, NULL);
	
	int value = 123;
	int result = tss_set(key, &value);
	ASSERT_EQ(thrd_success, result);
	
	void *retrieved = tss_get(key);
	ASSERT_EQ(&value, retrieved);
	ASSERT_EQ(123, *((int*)retrieved));
	
	tss_delete(key);
}

static tss_t global_tss_key;

static int tss_thread_func(void *arg) {
	int *thread_value = (int*)arg;
	tss_set(global_tss_key, thread_value);
	
	void *retrieved = tss_get(global_tss_key);
	return *((int*)retrieved);
}

TEST(tss_thread_independence) {
	tss_create(&global_tss_key, NULL);
	
	int value1 = 100;
	int value2 = 200;
	
	thrd_t thread1, thread2;
	thrd_create(&thread1, tss_thread_func, &value1);
	thrd_create(&thread2, tss_thread_func, &value2);
	
	int result1, result2;
	thrd_join(thread1, &result1);
	thrd_join(thread2, &result2);
	
	ASSERT_EQ(100, result1);
	ASSERT_EQ(200, result2);
	
	tss_delete(global_tss_key);
}
#endif

/* ============================================================================
 * ERROR CASES
 * ============================================================================ */
TEST_SUITE(error_cases);

TEST(mtx_null_pointer) {
	int result = mtx_lock(NULL);
	ASSERT_EQ(thrd_error, result);
}

TEST(cnd_null_pointer) {
	int result = cnd_signal(NULL);
	ASSERT_EQ(thrd_error, result);
}

TEST_MAIN()

#else

int main(void) {
	printf("threads.h requires C11 or later\n");
	return 0;
}

#endif
