/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_C11
#include <stdatomic.h>

TEST_TYPE(unit);
TEST_UNIT(stdatomic.h);

/* ============================================================================
 * ATOMIC TYPE TESTS
 * ============================================================================ */
TEST_SUITE(atomic_types);

TEST(atomic_types_defined) {
	// Verify atomic types exist
	atomic_int ai;
	atomic_uint au;
	atomic_long al;
	atomic_ulong aul;
	atomic_llong all;
	atomic_ullong aull;
	
	(void)ai; (void)au; (void)al; (void)aul; (void)all; (void)aull;
	ASSERT_TRUE(1);
}

TEST(atomic_bool_type) {
	atomic_bool ab;
	(void)ab;
	ASSERT_TRUE(1);
}

TEST(atomic_char_types) {
	atomic_char ac;
	atomic_schar asc;
	atomic_uchar auc;
	
	(void)ac; (void)asc; (void)auc;
	ASSERT_TRUE(1);
}

TEST(atomic_short_types) {
	atomic_short as;
	atomic_ushort aus;
	
	(void)as; (void)aus;
	ASSERT_TRUE(1);
}

/* ============================================================================
 * ATOMIC INIT TESTS
 * ============================================================================ */
TEST_SUITE(atomic_init);

TEST(atomic_init_int) {
	atomic_int ai;
	atomic_init(&ai, 42);
	
	int val = atomic_load(&ai);
	ASSERT_EQ(42, val);
}

TEST(atomic_init_zero) {
	atomic_int ai;
	atomic_init(&ai, 0);
	
	ASSERT_EQ(0, atomic_load(&ai));
}

TEST(atomic_init_negative) {
	atomic_int ai;
	atomic_init(&ai, -100);
	
	ASSERT_EQ(-100, atomic_load(&ai));
}

/* ============================================================================
 * ATOMIC LOAD/STORE TESTS
 * ============================================================================ */
TEST_SUITE(load_store);

TEST(atomic_store_load) {
	atomic_int ai;
	atomic_init(&ai, 0);
	
	atomic_store(&ai, 123);
	ASSERT_EQ(123, atomic_load(&ai));
	
	atomic_store(&ai, 456);
	ASSERT_EQ(456, atomic_load(&ai));
}

TEST(atomic_store_explicit_relaxed) {
	atomic_int ai;
	atomic_init(&ai, 0);
	
	atomic_store_explicit(&ai, 789, memory_order_relaxed);
	int val = atomic_load_explicit(&ai, memory_order_relaxed);
	
	ASSERT_EQ(789, val);
}

TEST(atomic_store_explicit_seq_cst) {
	atomic_int ai;
	atomic_init(&ai, 0);
	
	atomic_store_explicit(&ai, 999, memory_order_seq_cst);
	int val = atomic_load_explicit(&ai, memory_order_seq_cst);
	
	ASSERT_EQ(999, val);
}

TEST(atomic_load_after_init) {
	atomic_int ai;
	atomic_init(&ai, 42);
	
	// Multiple loads should return same value
	ASSERT_EQ(42, atomic_load(&ai));
	ASSERT_EQ(42, atomic_load(&ai));
	ASSERT_EQ(42, atomic_load(&ai));
}

/* ============================================================================
 * ATOMIC EXCHANGE TESTS
 * ============================================================================ */
TEST_SUITE(exchange);

TEST(atomic_exchange_basic) {
	atomic_int ai;
	atomic_init(&ai, 10);
	
	int old = atomic_exchange(&ai, 20);
	
	ASSERT_EQ(10, old);
	ASSERT_EQ(20, atomic_load(&ai));
}

TEST(atomic_exchange_chain) {
	atomic_int ai;
	atomic_init(&ai, 1);
	
	int old = atomic_exchange(&ai, 2);
	ASSERT_EQ(1, old);
	
	old = atomic_exchange(&ai, 3);
	ASSERT_EQ(2, old);
	
	old = atomic_exchange(&ai, 4);
	ASSERT_EQ(3, old);
	
	ASSERT_EQ(4, atomic_load(&ai));
}

TEST(atomic_exchange_explicit) {
	atomic_int ai;
	atomic_init(&ai, 100);
	
	int old = atomic_exchange_explicit(&ai, 200, memory_order_relaxed);
	
	ASSERT_EQ(100, old);
	ASSERT_EQ(200, atomic_load(&ai));
}

/* ============================================================================
 * ATOMIC ARITHMETIC TESTS
 * ============================================================================ */
TEST_SUITE(arithmetic);

TEST(atomic_fetch_add_basic) {
	atomic_int ai;
	atomic_init(&ai, 10);
	
	int old = atomic_fetch_add(&ai, 5);
	
	ASSERT_EQ(10, old);
	ASSERT_EQ(15, atomic_load(&ai));
}

TEST(atomic_fetch_add_chain) {
	atomic_int ai;
	atomic_init(&ai, 0);
	
	atomic_fetch_add(&ai, 1);
	atomic_fetch_add(&ai, 2);
	atomic_fetch_add(&ai, 3);
	
	ASSERT_EQ(6, atomic_load(&ai));
}

TEST(atomic_fetch_add_negative) {
	atomic_int ai;
	atomic_init(&ai, 100);
	
	int old = atomic_fetch_add(&ai, -50);
	
	ASSERT_EQ(100, old);
	ASSERT_EQ(50, atomic_load(&ai));
}

TEST(atomic_fetch_sub_basic) {
	atomic_int ai;
	atomic_init(&ai, 100);
	
	int old = atomic_fetch_sub(&ai, 30);
	
	ASSERT_EQ(100, old);
	ASSERT_EQ(70, atomic_load(&ai));
}

TEST(atomic_fetch_sub_chain) {
	atomic_int ai;
	atomic_init(&ai, 100);
	
	atomic_fetch_sub(&ai, 10);
	atomic_fetch_sub(&ai, 20);
	atomic_fetch_sub(&ai, 30);
	
	ASSERT_EQ(40, atomic_load(&ai));
}

TEST(atomic_increment_pattern) {
	atomic_int counter;
	atomic_init(&counter, 0);
	
	for (int i = 0; i < 100; i++) {
		atomic_fetch_add(&counter, 1);
	}
	
	ASSERT_EQ(100, atomic_load(&counter));
}

/* ============================================================================
 * COMPARE-EXCHANGE TESTS
 * ============================================================================ */
TEST_SUITE(compare_exchange);

TEST(atomic_cmpxchg_success) {
	atomic_int ai;
	atomic_init(&ai, 42);
	
	int expected = 42;
	bool result = atomic_compare_exchange_strong(&ai, &expected, 99);
	
	ASSERT_TRUE(result);
	ASSERT_EQ(42, expected);  // Unchanged on success
	ASSERT_EQ(99, atomic_load(&ai));
}

TEST(atomic_cmpxchg_failure) {
	atomic_int ai;
	atomic_init(&ai, 42);
	
	int expected = 10;  // Wrong value
	bool result = atomic_compare_exchange_strong(&ai, &expected, 99);
	
	ASSERT_FALSE(result);
	ASSERT_EQ(42, expected);  // Updated with actual value
	ASSERT_EQ(42, atomic_load(&ai));  // Unchanged
}

TEST(atomic_cmpxchg_loop) {
	atomic_int ai;
	atomic_init(&ai, 0);
	
	// Increment using CAS loop
	for (int i = 0; i < 10; i++) {
		int expected = atomic_load(&ai);
		while (!atomic_compare_exchange_strong(&ai, &expected, expected + 1)) {
			// Retry with updated expected value
		}
	}
	
	ASSERT_EQ(10, atomic_load(&ai));
}

TEST(atomic_cmpxchg_weak) {
	atomic_int ai;
	atomic_init(&ai, 50);
	
	int expected = 50;
	bool result = atomic_compare_exchange_weak(&ai, &expected, 100);
	
	// Weak may spuriously fail, but should eventually succeed
	while (!result) {
		expected = atomic_load(&ai);
		result = atomic_compare_exchange_weak(&ai, &expected, 100);
	}
	
	ASSERT_EQ(100, atomic_load(&ai));
}

/* ============================================================================
 * BITWISE OPERATION TESTS
 * ============================================================================ */
TEST_SUITE(bitwise);

TEST(atomic_fetch_and_basic) {
	atomic_uint au;
	atomic_init(&au, 0xFF);
	
	unsigned int old = atomic_fetch_and(&au, 0x0F);
	
	ASSERT_EQ(0xFF, old);
	ASSERT_EQ(0x0F, atomic_load(&au));
}

TEST(atomic_fetch_or_basic) {
	atomic_uint au;
	atomic_init(&au, 0x0F);
	
	unsigned int old = atomic_fetch_or(&au, 0xF0);
	
	ASSERT_EQ(0x0F, old);
	ASSERT_EQ(0xFF, atomic_load(&au));
}

TEST(atomic_fetch_xor_basic) {
	atomic_uint au;
	atomic_init(&au, 0xAA);
	
	unsigned int old = atomic_fetch_xor(&au, 0xFF);
	
	ASSERT_EQ(0xAA, old);
	ASSERT_EQ(0x55, atomic_load(&au));
}

TEST(atomic_fetch_and_chain) {
	atomic_uint au;
	atomic_init(&au, 0xFFFF);
	
	atomic_fetch_and(&au, 0x0FFF);
	atomic_fetch_and(&au, 0x00FF);
	atomic_fetch_and(&au, 0x000F);
	
	ASSERT_EQ(0x000F, atomic_load(&au));
}

/* ============================================================================
 * ATOMIC FLAG TESTS
 * ============================================================================ */
TEST_SUITE(atomic_flag);

TEST(atomic_flag_init) {
	atomic_flag flag = ATOMIC_FLAG_INIT;
	
	// Initially clear
	bool was_set = atomic_flag_test_and_set(&flag);
	ASSERT_FALSE(was_set);
}

TEST(atomic_flag_test_and_set) {
	atomic_flag flag = ATOMIC_FLAG_INIT;
	
	// First test_and_set should return false (was clear)
	ASSERT_FALSE(atomic_flag_test_and_set(&flag));
	
	// Second test_and_set should return true (was set)
	ASSERT_TRUE(atomic_flag_test_and_set(&flag));
}

TEST(atomic_flag_clear) {
	atomic_flag flag = ATOMIC_FLAG_INIT;
	
	atomic_flag_test_and_set(&flag);
	atomic_flag_clear(&flag);
	
	// After clear, should be false again
	ASSERT_FALSE(atomic_flag_test_and_set(&flag));
}

TEST(atomic_flag_explicit) {
	atomic_flag flag = ATOMIC_FLAG_INIT;
	
	bool was_set = atomic_flag_test_and_set_explicit(&flag, memory_order_relaxed);
	ASSERT_FALSE(was_set);
	
	atomic_flag_clear_explicit(&flag, memory_order_relaxed);
	was_set = atomic_flag_test_and_set_explicit(&flag, memory_order_relaxed);
	ASSERT_FALSE(was_set);
}

TEST(atomic_flag_spinlock_pattern) {
	atomic_flag lock = ATOMIC_FLAG_INIT;
	int shared_resource = 0;
	
	// Acquire lock
	while (atomic_flag_test_and_set(&lock)) {
		// Spin
	}
	
	// Critical section
	shared_resource = 42;
	
	// Release lock
	atomic_flag_clear(&lock);
	
	ASSERT_EQ(42, shared_resource);
}

/* ============================================================================
 * MEMORY ORDER TESTS
 * ============================================================================ */
TEST_SUITE(memory_order);

TEST(memory_order_relaxed) {
	atomic_int ai;
	atomic_init(&ai, 0);
	
	atomic_store_explicit(&ai, 10, memory_order_relaxed);
	int val = atomic_load_explicit(&ai, memory_order_relaxed);
	
	ASSERT_EQ(10, val);
}

TEST(memory_order_acquire_release) {
	atomic_int ai;
	atomic_init(&ai, 0);
	
	atomic_store_explicit(&ai, 20, memory_order_release);
	int val = atomic_load_explicit(&ai, memory_order_acquire);
	
	ASSERT_EQ(20, val);
}

TEST(memory_order_seq_cst) {
	atomic_int ai;
	atomic_init(&ai, 0);
	
	atomic_store_explicit(&ai, 30, memory_order_seq_cst);
	int val = atomic_load_explicit(&ai, memory_order_seq_cst);
	
	ASSERT_EQ(30, val);
}

/* ============================================================================
 * FENCE TESTS
 * ============================================================================ */
TEST_SUITE(fences);

TEST(atomic_thread_fence_seq_cst) {
	atomic_thread_fence(memory_order_seq_cst);
	ASSERT_TRUE(1);  // Just verify it doesn't crash
}

TEST(atomic_thread_fence_acquire) {
	atomic_thread_fence(memory_order_acquire);
	ASSERT_TRUE(1);
}

TEST(atomic_thread_fence_release) {
	atomic_thread_fence(memory_order_release);
	ASSERT_TRUE(1);
}

TEST(atomic_signal_fence) {
	atomic_signal_fence(memory_order_seq_cst);
	ASSERT_TRUE(1);
}

/* ============================================================================
 * LOCK-FREE MACROS
 * ============================================================================ */
TEST_SUITE(lock_free);

TEST(lock_free_macros_defined) {
	ASSERT_EQ(2, ATOMIC_BOOL_LOCK_FREE);
	ASSERT_EQ(2, ATOMIC_CHAR_LOCK_FREE);
	ASSERT_EQ(2, ATOMIC_SHORT_LOCK_FREE);
	ASSERT_EQ(2, ATOMIC_INT_LOCK_FREE);
	ASSERT_EQ(2, ATOMIC_LONG_LOCK_FREE);
	ASSERT_EQ(2, ATOMIC_LLONG_LOCK_FREE);
	ASSERT_EQ(2, ATOMIC_POINTER_LOCK_FREE);
}

/* ============================================================================
 * DIFFERENT TYPE TESTS
 * ============================================================================ */
TEST_SUITE(different_types);

TEST(atomic_long_operations) {
	atomic_long al;
	atomic_init(&al, 1000L);
	
	atomic_fetch_add(&al, 500L);
	ASSERT_EQ(1500L, atomic_load(&al));
}

TEST(atomic_llong_operations) {
	atomic_llong all;
	atomic_init(&all, 1000000LL);
	
	atomic_fetch_add(&all, 500000LL);
	ASSERT_EQ(1500000LL, atomic_load(&all));
}

TEST(atomic_uchar_operations) {
	atomic_uchar auc;
	atomic_init(&auc, 100);
	
	atomic_fetch_add(&auc, 50);
	ASSERT_EQ(150, atomic_load(&auc));
}

TEST(atomic_ushort_operations) {
	atomic_ushort aus;
	atomic_init(&aus, 1000);
	
	atomic_fetch_add(&aus, 234);
	ASSERT_EQ(1234, atomic_load(&aus));
}

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(atomic_zero_values) {
	atomic_int ai;
	atomic_init(&ai, 0);
	
	ASSERT_EQ(0, atomic_load(&ai));
	atomic_fetch_add(&ai, 0);
	ASSERT_EQ(0, atomic_load(&ai));
}

TEST(atomic_max_values) {
	atomic_uint au;
	atomic_init(&au, UINT_MAX);
	
	ASSERT_EQ(UINT_MAX, atomic_load(&au));
}

TEST(atomic_overflow) {
	atomic_uchar auc;
	atomic_init(&auc, 255);
	
	atomic_fetch_add(&auc, 1);
	ASSERT_EQ(0, atomic_load(&auc));  // Wraps around
}

/* ============================================================================
 * USAGE PATTERNS
 * ============================================================================ */
TEST_SUITE(usage_patterns);

TEST(counter_pattern) {
	atomic_int counter;
	atomic_init(&counter, 0);
	
	for (int i = 0; i < 1000; i++) {
		atomic_fetch_add(&counter, 1);
	}
	
	ASSERT_EQ(1000, atomic_load(&counter));
}

TEST(flag_pattern) {
	atomic_bool flag;
	atomic_init(&flag, false);
	
	atomic_store(&flag, true);
	ASSERT_TRUE(atomic_load(&flag));
	
	atomic_store(&flag, false);
	ASSERT_FALSE(atomic_load(&flag));
}

TEST(cas_retry_pattern) {
	atomic_int shared;
	atomic_init(&shared, 0);
	
	// Increment using CAS
	int expected = atomic_load(&shared);
	while (!atomic_compare_exchange_strong(&shared, &expected, expected + 1)) {
		// expected is automatically updated on failure
	}
	
	ASSERT_EQ(1, atomic_load(&shared));
}

TEST_MAIN()

#else

// Stub for pre-C11
int main(void) {
	printf("stdatomic.h requires C11 or later\n");
	return 0;
}

#endif
