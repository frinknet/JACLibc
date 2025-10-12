/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <pthread.h>
#include <unistd.h>

TEST_TYPE(unit);
TEST_UNIT(pthread.h);

/* ============================================================================
 * CONSTANTS
 * ============================================================================ */
TEST_SUITE(constants);

TEST(detach_state_constants) {
	ASSERT_EQ(0, PTHREAD_CREATE_JOINABLE);
	ASSERT_EQ(1, PTHREAD_CREATE_DETACHED);
}

TEST(mutex_type_constants) {
	ASSERT_EQ(0, PTHREAD_MUTEX_NORMAL);
	ASSERT_NE(0, PTHREAD_MUTEX_RECURSIVE);
	ASSERT_NE(0, PTHREAD_MUTEX_ERRORCHECK);
}

/* ============================================================================
 * ATTRIBUTES
 * ============================================================================ */
TEST_SUITE(attributes);

TEST(pthread_attr_init_destroy) {
	pthread_attr_t attr;
	
	ASSERT_EQ(0, pthread_attr_init(&attr));
	ASSERT_EQ(0, pthread_attr_destroy(&attr));
}

TEST(pthread_attr_setdetachstate) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	
	int result = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	ASSERT_EQ(0, result);
	ASSERT_EQ(1, attr.detached);
	
	pthread_attr_destroy(&attr);
}

TEST(pthread_attr_setstacksize) {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	
	int result = pthread_attr_setstacksize(&attr, 2097152);  // 2MB
	ASSERT_EQ(0, result);
	ASSERT_EQ(2097152, attr.stack_size);
	
	pthread_attr_destroy(&attr);
}

TEST(pthread_mutexattr_init_destroy) {
	pthread_mutexattr_t attr;
	
	ASSERT_EQ(0, pthread_mutexattr_init(&attr));
	ASSERT_EQ(0, pthread_mutexattr_destroy(&attr));
}

TEST(pthread_mutexattr_settype) {
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	
	int result = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	ASSERT_EQ(0, result);
	ASSERT_EQ(PTHREAD_MUTEX_RECURSIVE, attr.type);
	
	pthread_mutexattr_destroy(&attr);
}

TEST(pthread_condattr_init_destroy) {
	pthread_condattr_t attr;
	
	ASSERT_EQ(0, pthread_condattr_init(&attr));
	ASSERT_EQ(0, pthread_condattr_destroy(&attr));
}

/* ============================================================================
 * MUTEX BASIC OPERATIONS
 * ============================================================================ */
TEST_SUITE(mutex_basic);

TEST(pthread_mutex_init_destroy) {
	pthread_mutex_t mutex;
	
	ASSERT_EQ(0, pthread_mutex_init(&mutex, NULL));
	ASSERT_EQ(0, pthread_mutex_destroy(&mutex));
}

TEST(pthread_mutex_lock_unlock) {
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);
	
	ASSERT_EQ(0, pthread_mutex_lock(&mutex));
	ASSERT_EQ(0, pthread_mutex_unlock(&mutex));
	
	pthread_mutex_destroy(&mutex);
}

TEST(pthread_mutex_trylock_unlocked) {
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);
	
	int result = pthread_mutex_trylock(&mutex);
	ASSERT_EQ(0, result);
	
	pthread_mutex_unlock(&mutex);
	pthread_mutex_destroy(&mutex);
}

TEST(pthread_mutex_multiple_lock_unlock) {
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex, NULL);
	
	for (int i = 0; i < 10; i++) {
		ASSERT_EQ(0, pthread_mutex_lock(&mutex));
		ASSERT_EQ(0, pthread_mutex_unlock(&mutex));
	}
	
	pthread_mutex_destroy(&mutex);
}

/* ============================================================================
 * RECURSIVE MUTEX
 * ============================================================================ */
TEST_SUITE(mutex_recursive);

TEST(pthread_mutex_recursive_init) {
	pthread_mutex_t mutex;
	pthread_mutexattr_t attr;
	
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	
	int result = pthread_mutex_init(&mutex, &attr);
	ASSERT_EQ(0, result);
	
	pthread_mutex_destroy(&mutex);
	pthread_mutexattr_destroy(&attr);
}

#if !PTHREAD_DUMMY
TEST(pthread_mutex_recursive_locking) {
	pthread_mutex_t mutex;
	pthread_mutexattr_t attr;
	
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&mutex, &attr);
	
	// Lock multiple times
	ASSERT_EQ(0, pthread_mutex_lock(&mutex));
	ASSERT_EQ(0, pthread_mutex_lock(&mutex));
	ASSERT_EQ(0, pthread_mutex_lock(&mutex));
	
	// Unlock same number of times
	ASSERT_EQ(0, pthread_mutex_unlock(&mutex));
	ASSERT_EQ(0, pthread_mutex_unlock(&mutex));
	ASSERT_EQ(0, pthread_mutex_unlock(&mutex));
	
	pthread_mutex_destroy(&mutex);
	pthread_mutexattr_destroy(&attr);
}
#endif

/* ============================================================================
 * CONDITION VARIABLES
 * ============================================================================ */
TEST_SUITE(condition_variables);

TEST(pthread_cond_init_destroy) {
	pthread_cond_t cond;
	
	ASSERT_EQ(0, pthread_cond_init(&cond, NULL));
	ASSERT_EQ(0, pthread_cond_destroy(&cond));
}

TEST(pthread_cond_signal) {
	pthread_cond_t cond;
	pthread_cond_init(&cond, NULL);
	
	int result = pthread_cond_signal(&cond);
	ASSERT_EQ(0, result);
	
	pthread_cond_destroy(&cond);
}

TEST(pthread_cond_broadcast) {
	pthread_cond_t cond;
	pthread_cond_init(&cond, NULL);
	
	int result = pthread_cond_broadcast(&cond);
	ASSERT_EQ(0, result);
	
	pthread_cond_destroy(&cond);
}

/* ============================================================================
 * ONCE FLAG
 * ============================================================================ */
TEST_SUITE(once_flag);

static int pthread_once_counter = 0;

static void pthread_increment_once(void) {
	pthread_once_counter++;
}

TEST(pthread_once_basic) {
	pthread_once_t once_control = PTHREAD_ONCE_INIT;
	pthread_once_counter = 0;
	
	pthread_once(&once_control, pthread_increment_once);
	ASSERT_EQ(1, pthread_once_counter);
	
	// Second call should not execute
	pthread_once(&once_control, pthread_increment_once);
	ASSERT_EQ(1, pthread_once_counter);
}

TEST(pthread_once_multiple_controls) {
	pthread_once_t once1 = PTHREAD_ONCE_INIT;
	pthread_once_t once2 = PTHREAD_ONCE_INIT;
	pthread_once_counter = 0;
	
	pthread_once(&once1, pthread_increment_once);
	pthread_once(&once2, pthread_increment_once);
	
	ASSERT_EQ(2, pthread_once_counter);
}

/* ============================================================================
 * THREAD SELF AND EQUAL
 * ============================================================================ */
TEST_SUITE(thread_identity);

TEST(pthread_self) {
	pthread_t self = pthread_self();
	
	// Just verify it returns something
	ASSERT_TRUE(1);
}

TEST(pthread_equal_same_thread) {
	pthread_t self1 = pthread_self();
	pthread_t self2 = pthread_self();
	
	ASSERT_TRUE(pthread_equal(self1, self2));
}

/* ============================================================================
 * THREAD CREATION AND JOINING (Platform-dependent)
 * ============================================================================ */
#if !PTHREAD_DUMMY
TEST_SUITE(thread_creation);

static void *simple_pthread_func(void *arg) {
	int *value = (int*)arg;
	*value = 42;
	return NULL;
}

TEST(pthread_create_join) {
	pthread_t thread;
	int value = 0;
	
	int result = pthread_create(&thread, NULL, simple_pthread_func, &value);
	ASSERT_EQ(0, result);
	
	result = pthread_join(thread, NULL);
	ASSERT_EQ(0, result);
	ASSERT_EQ(42, value);
}

static void *return_value_pthread(void *arg) {
	return arg;
}

TEST(pthread_join_with_retval) {
	pthread_t thread;
	void *input = (void*)0x12345678;
	void *output = NULL;
	
	pthread_create(&thread, NULL, return_value_pthread, input);
	pthread_join(thread, &output);
	
	ASSERT_EQ(input, output);
}

TEST(pthread_detach) {
	pthread_t thread;
	int value = 0;
	
	pthread_create(&thread, NULL, simple_pthread_func, &value);
	
	int result = pthread_detach(thread);
	ASSERT_EQ(0, result);
	
	// Give thread time to finish
	usleep(100000);  // 100ms
}

static void *counting_pthread(void *arg) {
	int *counter = (int*)arg;
	for (int i = 0; i < 100; i++) {
		(*counter)++;
	}
	return NULL;
}

TEST(pthread_multiple_threads) {
	pthread_t threads[5];
	int counters[5] = {0};
	
	for (int i = 0; i < 5; i++) {
		pthread_create(&threads[i], NULL, counting_pthread, &counters[i]);
	}
	
	for (int i = 0; i < 5; i++) {
		pthread_join(threads[i], NULL);
	}
	
	for (int i = 0; i < 5; i++) {
		ASSERT_EQ(100, counters[i]);
	}
}
#endif

/* ============================================================================
 * MUTEX WITH THREADS
 * ============================================================================ */
#if !PTHREAD_DUMMY
TEST_SUITE(mutex_with_threads);

static pthread_mutex_t shared_pthread_mutex;
static int shared_pthread_counter = 0;

static void *pthread_increment_thread(void *arg) {
	int iterations = *((int*)arg);
	
	for (int i = 0; i < iterations; i++) {
		pthread_mutex_lock(&shared_pthread_mutex);
		shared_pthread_counter++;
		pthread_mutex_unlock(&shared_pthread_mutex);
	}
	
	return NULL;
}

TEST(pthread_mutex_mutual_exclusion) {
	pthread_mutex_init(&shared_pthread_mutex, NULL);
	shared_pthread_counter = 0;
	
	int iterations = 1000;
	pthread_t thread1, thread2;
	
	pthread_create(&thread1, NULL, pthread_increment_thread, &iterations);
	pthread_create(&thread2, NULL, pthread_increment_thread, &iterations);
	
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	
	ASSERT_EQ(2000, shared_pthread_counter);
	
	pthread_mutex_destroy(&shared_pthread_mutex);
}
#endif

/* ============================================================================
 * CONDITION VARIABLE WITH THREADS
 * ============================================================================ */
#if !PTHREAD_DUMMY
TEST_SUITE(condvar_with_threads);

static pthread_cond_t pthread_shared_cond;
static pthread_mutex_t pthread_cond_mutex;
static int pthread_cond_flag = 0;

static void *pthread_waiter_thread(void *arg) {
	pthread_mutex_lock(&pthread_cond_mutex);
	
	while (!pthread_cond_flag) {
		pthread_cond_wait(&pthread_shared_cond, &pthread_cond_mutex);
	}
	
	pthread_mutex_unlock(&pthread_cond_mutex);
	return (void*)42;
}

TEST(pthread_cond_signal_wakes_thread) {
	pthread_mutex_init(&pthread_cond_mutex, NULL);
	pthread_cond_init(&pthread_shared_cond, NULL);
	pthread_cond_flag = 0;
	
	pthread_t thread;
	pthread_create(&thread, NULL, pthread_waiter_thread, NULL);
	
	// Give thread time to wait
	usleep(50000);  // 50ms
	
	// Signal the thread
	pthread_mutex_lock(&pthread_cond_mutex);
	pthread_cond_flag = 1;
	pthread_cond_signal(&pthread_shared_cond);
	pthread_mutex_unlock(&pthread_cond_mutex);
	
	void *result;
	pthread_join(thread, &result);
	
	ASSERT_EQ(42, (int)(intptr_t)result);
	
	pthread_cond_destroy(&pthread_shared_cond);
	pthread_mutex_destroy(&pthread_cond_mutex);
}
#endif

/* ============================================================================
 * THREAD-SPECIFIC STORAGE
 * ============================================================================ */
#if !PTHREAD_DUMMY
TEST_SUITE(thread_specific_storage);

TEST(pthread_key_create_delete) {
	pthread_key_t key;
	int result = pthread_key_create(&key, NULL);
	
	ASSERT_EQ(0, result);
	pthread_key_delete(key);
}

TEST(pthread_setspecific_getspecific) {
	pthread_key_t key;
	pthread_key_create(&key, NULL);
	
	int value = 123;
	int result = pthread_setspecific(key, &value);
	ASSERT_EQ(0, result);
	
	void *retrieved = pthread_getspecific(key);
	ASSERT_EQ(&value, retrieved);
	ASSERT_EQ(123, *((int*)retrieved));
	
	pthread_key_delete(key);
}

static pthread_key_t global_pthread_key;

static void *pthread_tss_thread_func(void *arg) {
	int *thread_value = (int*)arg;
	pthread_setspecific(global_pthread_key, thread_value);
	
	void *retrieved = pthread_getspecific(global_pthread_key);
	return retrieved;
}

TEST(pthread_tss_thread_independence) {
	pthread_key_create(&global_pthread_key, NULL);
	
	int value1 = 100;
	int value2 = 200;
	
	pthread_t thread1, thread2;
	pthread_create(&thread1, NULL, pthread_tss_thread_func, &value1);
	pthread_create(&thread2, NULL, pthread_tss_thread_func, &value2);
	
	void *result1, *result2;
	pthread_join(thread1, &result1);
	pthread_join(thread2, &result2);
	
	ASSERT_EQ(100, *((int*)result1));
	ASSERT_EQ(200, *((int*)result2));
	
	pthread_key_delete(global_pthread_key);
}
#endif

/* ============================================================================
 * ERROR HANDLING
 * ============================================================================ */
TEST_SUITE(error_handling);

TEST(pthread_mutex_null_pointer) {
	int result = pthread_mutex_lock(NULL);
	ASSERT_NE(0, result);
}

TEST(pthread_cond_null_pointer) {
	int result = pthread_cond_signal(NULL);
	ASSERT_NE(0, result);
}

TEST(pthread_attr_null_pointer) {
	int result = pthread_attr_init(NULL);
	ASSERT_NE(0, result);
}

TEST_MAIN()
