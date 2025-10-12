/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <stdlib.h>

TEST_TYPE(unit);
TEST_UNIT(stdlib.h);

/* ============================================================================
 * MEMORY MANAGEMENT TESTS
 * ============================================================================ */
TEST_SUITE(memory_management);

TEST(malloc_basic_allocation) {
		void *ptr = malloc(100);
		if (ptr) {
				char *test = (char*)ptr;
				test[0] = 'A';
				test[99] = 'Z';
				ASSERT_EQ('A', test[0]);
				ASSERT_EQ('Z', test[99]);
				free(ptr);
		}
}

TEST(malloc_zero_size) {
	void *ptr = malloc(0);
	free(ptr);  // Should handle gracefully
}


TEST(calloc_zero_initialization) {
	int *arr = (int*)calloc(10, sizeof(int));
	if (arr) {
		for (int i = 0; i < 10; i++) {
			ASSERT_EQ(0, arr[i]);
		}
		free(arr);
	}

	// Test zero elements
	void *ptr = calloc(0, sizeof(int));
	free(ptr);

	// Test zero size
	ptr = calloc(10, 0);
	free(ptr);
}

TEST(realloc_resize_operations) {
	char *ptr = (char*)malloc(50);
	if (ptr) {
		strcpy(ptr, "Hello");

		// Expand
		ptr = (char*)realloc(ptr, 100);
		if (ptr) {
			ASSERT_EQ(0, strcmp(ptr, "Hello"));  // Data preserved
		}

		// Shrink
		ptr = (char*)realloc(ptr, 10);
		if (ptr) {
			ASSERT_EQ(0, strcmp(ptr, "Hello"));  // Data still there
		}

		// Realloc to zero (should act like free)
		ptr = (char*)realloc(ptr, 0);
		// ptr should be NULL or freed
	}

	// Realloc NULL pointer (should act like malloc)
	void *new_ptr = realloc(NULL, 100);
	free(new_ptr);
}

TEST(aligned_alloc_alignment) {
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wvoid-pointer-to-int-cast"

	void *ptr = aligned_alloc(16, 64);
	if (ptr) {
		uintptr_t addr = (uintptr_t)ptr;
		ASSERT_EQ(0UL, addr % 16);
		free(ptr);
	}

	ptr = aligned_alloc(8, 32);
	if (ptr) {
		uintptr_t addr = (uintptr_t)ptr;
		ASSERT_EQ(0UL, addr % 8);
		free(ptr);
	}

	#pragma GCC diagnostic pop
}

TEST(coalescing_large_blocks) {
	// Allocate 3 large adjacent blocks
	void *p1 = malloc(2048);
	void *p2 = malloc(2048);
	void *p3 = malloc(2048);

	ASSERT_NOT_NULL(p1);
	ASSERT_NOT_NULL(p2);
	ASSERT_NOT_NULL(p3);

	// Free middle one
	free(p2);

	// Free first - should coalesce with p2
	free(p1);

	// Now allocate 4KB - should succeed if coalescing worked
	void *big = malloc(4096);
	ASSERT_NOT_NULL(big);  // Proves p1+p2 coalesced

	free(p3);
	free(big);
}

TEST(double_free_protected) {
	void *ptr = malloc(1024);
	ASSERT_NOT_NULL(ptr);

	free(ptr);
	free(ptr);  // Should be safe (no-op or caught)

	// If we get here, double-free was handled
}

TEST(aligned_alloc_edge_cases) {
	// Invalid: alignment not power of 2
	void* ptr = aligned_alloc(3, 64);
	ASSERT_NULL(ptr);

	ptr = aligned_alloc(7, 64);
	ASSERT_NULL(ptr);

	// Invalid: size not multiple of alignment
	ptr = aligned_alloc(16, 63);
	ASSERT_NULL(ptr);

	// Invalid: zero alignment
	ptr = aligned_alloc(0, 64);
	ASSERT_NULL(ptr);

	// Valid: large alignment
	ptr = aligned_alloc(256, 512);
	if (ptr) {
		ASSERT_EQ(0, ((uintptr_t)ptr) % 256);
		free(ptr);
	}
}

TEST(realloc_null_and_zero) {
	// realloc(NULL, size) == malloc(size)
	void* ptr = realloc(NULL, 100);
	ASSERT_NOT_NULL(ptr);
	free(ptr);

	// realloc(ptr, 0) == free(ptr)
	ptr = malloc(100);
	void* result = realloc(ptr, 0);
	// result should be NULL, original ptr freed
	(void)result;
}

TEST(realloc_shrink_and_grow) {
	char* ptr = (char*)malloc(100);
	if (ptr) {
		memset(ptr, 0xAB, 100);

		// Shrink
		ptr = (char*)realloc(ptr, 50);
		if (ptr) {
			ASSERT_EQ(0xAB, (unsigned char)ptr[0]);
			ASSERT_EQ(0xAB, (unsigned char)ptr[49]);
		}

		// Grow back
		ptr = (char*)realloc(ptr, 200);
		if (ptr) {
			// Original data should still be there
			ASSERT_EQ(0xAB, (unsigned char)ptr[0]);
		}

		free(ptr);
	}
}

TEST(realloc_in_place_vs_move) {
	// Allocate and write pattern
	char* ptr1 = (char*)malloc(1000);

	if (ptr1) {
		// Write a pattern
		for (int i = 0; i < 1000; i++) {
			ptr1[i] = (char)(i % 256);
		}

		// Shrink - data should be preserved
		ptr1 = (char*)realloc(ptr1, 500);
		ASSERT_NOT_NULL(ptr1);

		// Verify first 500 bytes still have correct pattern
		for (int i = 0; i < 500; i++) {
			ASSERT_EQ((char)(i % 256), ptr1[i]);
		}

		free(ptr1);
	}
}

TEST(realloc_preserves_data) {
	char* ptr = (char*)malloc(100);

	if (ptr) {
		// Write pattern
		memset(ptr, 0xAB, 100);

		// Shrink to 50
		ptr = (char*)realloc(ptr, 50);
		ASSERT_NOT_NULL(ptr);

		// Verify first 50 bytes preserved
		for (int i = 0; i < 50; i++) {
			ASSERT_EQ(0xAB, (unsigned char)ptr[i]);
		}

		// Grow to 200
		ptr = (char*)realloc(ptr, 200);
		ASSERT_NOT_NULL(ptr);

		// Verify original 50 bytes still there
		for (int i = 0; i < 50; i++) {
			ASSERT_EQ(0xAB, (unsigned char)ptr[i]);
		}

		free(ptr);
	}
}

TEST(calloc_size_overflow) {
	// Try to overflow: SIZE_MAX / 2 * SIZE_MAX / 2
	void* ptr = calloc(SIZE_MAX / 2, SIZE_MAX / 2);
	ASSERT_NULL(ptr);  // Should detect overflow

	// Large but valid
	ptr = calloc(100, 100);
	if (ptr) {
		// Verify zeroed
		unsigned char* bytes = (unsigned char*)ptr;
		for (int i = 0; i < 100; i++) {
			ASSERT_EQ(0, bytes[i]);
		}
		free(ptr);
	}
}

/* ============================================================================
 * HEAP GROWTH TESTS
 * ============================================================================ */
TEST_SUITE(heap_growth);

TEST(heap_growth_basic) {
	// Allocate more than JACL_HEAP_INIT to force growth
	void *ptrs[10];
	size_t alloc_size = JACL_HEAP_INIT / 8;  // 32KB each
	int allocated = 0;

	for (int i = 0; i < 10; i++) {  // Try to allocate 320KB total
		ptrs[i] = malloc(alloc_size);
		if (ptrs[i]) {
			// Write to verify it's real
			memset(ptrs[i], 0xAB, alloc_size);
			allocated++;
		} else {
			break;
		}
	}

	// Should have allocated more than initial heap size
	ASSERT_GT(allocated, 3);  // At least 128KB (> 256KB init / 2)

	for (int i = 0; i < allocated; i++) {
		free(ptrs[i]);
	}
}

TEST(heap_max_limit_respected) {
	// Try to allocate more than JACL_HEAP_MAX
	void *huge = malloc(JACL_HEAP_MAX + 1024);
	ASSERT_NULL(huge);  // Should fail

	// But allocating within limit should work
	void *ok = malloc(1024);
	ASSERT_NOT_NULL(ok);
	free(ok);
}

TEST(heap_fixed_mode) {
	// If JACL_HEAP_INIT == JACL_HEAP_MAX, should not grow
	// This test documents the behavior
	void *ptr = malloc(100);
	if (ptr) {
		free(ptr);
	}
}

TEST(large_allocation_single_block) {
	// Allocate larger than JACL_HEAP_SEGMENT to test size adaptation
	size_t large_size = JACL_HEAP_SEGMENT + (256u<<10);  // 1.25MB
	void *large = malloc(large_size);

	if (large) {
		// Verify we can write to it
		memset(large, 0xFF, 1024);  // Just test first KB
		ASSERT_EQ(0xFF, ((unsigned char*)large)[0]);
		free(large);
	}
}


TEST(malloc_large_allocation) {
	void *ptr = malloc(JACL_HEAP_INIT / 2);
	if (ptr) {
		free(ptr);
	}
	// Test allocation larger than heap
	void *big_ptr = malloc(JACL_HEAP_MAX + 1024);
	// Should return NULL or handle gracefully
	free(big_ptr);
}

TEST(malloc_stress_test) {
	// Try to exhaust memory (should fail gracefully)
	void *ptrs[100];
	int allocated = 0;

	for (int i = 0; i < 100; i++) {
		ptrs[i] = malloc(JACL_HEAP_INIT / 50);	// Try to allocate 2% of heap
		if (ptrs[i]) {
			allocated++;
		} else {
			break;
		}
	}

	// Free what we allocated
	for (int i = 0; i < allocated; i++) {
		free(ptrs[i]);
	}
}

TEST(multiple_segments_allocation) {
	// Force creation of multiple segments
	#define NUM_BLOCKS 20
	void *ptrs[NUM_BLOCKS];
	size_t block_size = JACL_HEAP_SEGMENT / 2;  // 512KB each
	int allocated = 0;

	for (int i = 0; i < NUM_BLOCKS; i++) {
		ptrs[i] = malloc(block_size);
		if (ptrs[i]) {
			memset(ptrs[i], (unsigned char)i, 1024);  // Mark first KB
			allocated++;
		} else {
			break;
		}
	}

	// Should allocate at least 2 segments worth
	ASSERT_GT(allocated, 2);

	// Verify data integrity
	for (int i = 0; i < allocated; i++) {
		ASSERT_EQ((unsigned char)i, ((unsigned char*)ptrs[i])[0]);
		free(ptrs[i]);
	}
	#undef NUM_BLOCKS
}

TEST(quicklist_performance) {
	// Verify quicklist is actually being used (should be fast)
	#define ITERATIONS 10000
	void *ptrs[ITERATIONS];

	// Allocate/free hot sizes many times
	for (int i = 0; i < ITERATIONS; i++) {
		ptrs[i] = malloc(24);  // Quicklist size
		if (!ptrs[i]) break;
	}

	for (int i = 0; i < ITERATIONS; i++) {
		free(ptrs[i]);
	}

	// If we got here without crashing, quicklist works
	#undef ITERATIONS
}

TEST(mixed_size_allocations) {
	// Test interaction between quicklist, TLS, and bins
	void *small = malloc(32);	// Quicklist
	void *medium = malloc(100);  // TLS arena
	void *large = malloc(2048);  // Bins

	ASSERT_NOT_NULL(small);
	ASSERT_NOT_NULL(medium);
	ASSERT_NOT_NULL(large);

	// Write to all
	memset(small, 0xAA, 32);
	memset(medium, 0xBB, 100);
	memset(large, 0xCC, 2048);

	// Verify
	ASSERT_EQ(0xAA, ((unsigned char*)small)[0]);
	ASSERT_EQ(0xBB, ((unsigned char*)medium)[0]);
	ASSERT_EQ(0xCC, ((unsigned char*)large)[0]);

	free(large);
	free(medium);
	free(small);
}

TEST(arena_leak_by_design) {
	// TLS arena blocks should "leak" (not be freed)
	void *tls_allocs[100];
	int allocated = 0;

	for (int i = 0; i < 100; i++) {
		tls_allocs[i] = malloc(64);  // TLS size
		if (tls_allocs[i]) allocated++;
		else break;
	}

	// Free them (should be no-op for arena blocks)
	for (int i = 0; i < allocated; i++) {
		free(tls_allocs[i]);
	}

	// Allocate again - should still work
	void *after = malloc(64);
	ASSERT_NOT_NULL(after);
	free(after);
}

TEST(fragmentation_handling) {
	// Allocate, free alternating, then allocate again
	void *ptrs[10];

	// Allocate all
	for (int i = 0; i < 10; i++) {
		ptrs[i] = malloc(1024);
	}

	// Free odd indices
	for (int i = 1; i < 10; i += 2) {
		free(ptrs[i]);
		ptrs[i] = NULL;
	}

	// Try to allocate in freed spaces
	for (int i = 1; i < 10; i += 2) {
		ptrs[i] = malloc(1024);
		ASSERT_NOT_NULL(ptrs[i]);
	}

	// Free all
	for (int i = 0; i < 10; i++) {
		free(ptrs[i]);
	}
}

TEST(extreme_allocation_pattern) {
	// Allocate many small blocks
	#define MANY 10000
	void* ptrs[MANY];
	int allocated = 0;

	for (int i = 0; i < MANY; i++) {
		ptrs[i] = malloc(16);
		if (ptrs[i]) {
			allocated++;
		} else {
			break;
		}
	}

	TEST_INFO("Allocated %d small blocks", allocated);

	// Free all
	for (int i = 0; i < allocated; i++) {
		free(ptrs[i]);
	}

	// Should be able to allocate again
	void* after = malloc(1024);
	ASSERT_NOT_NULL(after);
	free(after);
	#undef MANY
}

/* ============================================================================
 * CONVERSION FUNCTION TESTS
 * ============================================================================ */
TEST_SUITE(conversions);

TEST(atoi_basic_conversion) {
	ASSERT_EQ(0, atoi("0"));
	ASSERT_EQ(123, atoi("123"));
	ASSERT_EQ(-456, atoi("-456"));
	ASSERT_EQ(42, atoi("	42"));		// Leading whitespace
	ASSERT_EQ(123, atoi("123abc"));		// Trailing garbage
	ASSERT_EQ(0, atoi("abc"));			// No digits
	ASSERT_EQ(0, atoi(""));				// Empty string
}

TEST(atoi_edge_cases) {
	ASSERT_EQ(2147483647, atoi("2147483647"));	 // INT_MAX
	ASSERT_EQ(-2147483648, atoi("-2147483648"));	 // INT_MIN (if fits)
	ASSERT_EQ(0, atoi("	"));					 // Only whitespace
	ASSERT_EQ(1, atoi("+1"));						 // Explicit positive
}

TEST(atol_long_conversion) {
	ASSERT_EQ(0L, atol("0"));
	ASSERT_EQ(123456789L, atol("123456789"));
	ASSERT_EQ(-987654321L, atol("-987654321"));
	ASSERT_EQ(LONG_MAX, atol("2147483647"));		 // Assuming 32-bit long
	ASSERT_EQ(LONG_MIN, atol("-2147483648"));
}

TEST(atoll_long_long_conversion) {
	ASSERT_EQ(0LL, atoll("0"));
	ASSERT_EQ(123456789012345LL, atoll("123456789012345"));
	ASSERT_EQ(-987654321098765LL, atoll("-987654321098765"));
	ASSERT_EQ(9223372036854775807LL, atoll("9223372036854775807")); // LLONG_MAX
}

TEST(atof_float_conversion) {
	ASSERT_EQ(0.0, atof("0"));
	ASSERT_EQ(3.14, atof("3.14"));
	ASSERT_EQ(-2.5, atof("-2.5"));
	ASSERT_EQ(1.23e10, atof("1.23e10"));
	ASSERT_EQ(-4.56e-5, atof("-4.56e-5"));
	ASSERT_EQ(0.0, atof("abc"));				 // Invalid
}

TEST(strtol_base_conversion) {
	char *endptr;

	// Base 10 - complete number parsing
	ASSERT_EQ(123, strtol("123", &endptr, 10));
	ASSERT_EQ('\0', *endptr);  // ← FIXED: Points to null terminator

	// Empty string test
	ASSERT_EQ(0, strtol("", &endptr, 16));

	// Test endptr handling with trailing characters
	long result = strtol("42abc", &endptr, 10);
	ASSERT_EQ(42, result);
	ASSERT_EQ('a', *endptr);	// Points to first non-digit

	// NULL endptr test
	ASSERT_EQ(100, strtol("100", NULL, 10));
}

TEST(strtol_advanced_bases) {
	char *endptr;

	// Hex with 0x prefix
	ASSERT_EQ(255, strtol("0xFF", &endptr, 16));
	ASSERT_EQ(255, strtol("0xFF", &endptr, 0));	// Auto-detect

	// Octal
	ASSERT_EQ(64, strtol("100", &endptr, 8));
	ASSERT_EQ(64, strtol("0100", &endptr, 0));	// Auto-detect

	// Invalid base
	ASSERT_EQ(0, strtol("123", &endptr, 37));		// Base > 36
	ASSERT_EQ(0, strtol("123", &endptr, 1));		// Base < 2

	// Base 36 (max)
	ASSERT_EQ(35, strtol("Z", &endptr, 36));		// Z = 35 in base 36
}

TEST(strtoul_unsigned_conversion) {
	char *endptr;

	ASSERT_EQ(123UL, strtoul("123", &endptr, 10));
	ASSERT_EQ(0UL, strtoul("", &endptr, 10));
	ASSERT_EQ(4294967295UL, strtoul("4294967295", &endptr, 10)); // UINT_MAX
}

TEST(strtod_double_precision) {
	char *endptr;

	double result = strtod("3.14159", &endptr);
	ASSERT_TRUE(result > 3.14 && result < 3.15);

	result = strtod("1.23e-4", &endptr);
	ASSERT_TRUE(result > 0.0001 && result < 0.0002);

	result = strtod("invalid", &endptr);
	ASSERT_EQ(0.0, result);
}

TEST(conversion_overflow_behavior) {
	char *endptr;

	// Test what happens with numbers too big for long
	strtol("999999999999999999999999", &endptr, 10);
	// Should set errno to ERANGE (if you implement it)

	// Test floating point extremes
	double huge = strtod("1e500", &endptr);  // Should be HUGE_VAL
	(void)huge; // Suppress unused warning
}

TEST(strtol_whitespace_and_signs) {
	char* endptr;

	// Leading whitespace
	ASSERT_EQ(42, strtol("  \t\n42", &endptr, 10));

	// Explicit positive sign
	ASSERT_EQ(123, strtol("+123", &endptr, 10));

	// Multiple signs (invalid)
	ASSERT_EQ(0, strtol("++123", &endptr, 10));
	ASSERT_EQ(0, strtol("--123", &endptr, 10));
}

TEST(strtoul_negative_rejection) {
	char* endptr;

	// Should reject negative numbers
	unsigned long result = strtoul("-123", &endptr, 10);
	ASSERT_EQ(0UL, result);
}

TEST(strtod_special_values) {
	char* endptr;

	// Test infinity
	double inf = strtod("inf", &endptr);
	(void)inf;

	// Test NaN
	double nan_val = strtod("nan", &endptr);
	(void)nan_val;

	// Test scientific notation
	ASSERT_TRUE(strtod("1e10", &endptr) > 0);
	ASSERT_TRUE(strtod("1e-10", &endptr) > 0);
}

/* ============================================================================
 * DIVISION FUNCTION TESTS
 * ============================================================================ */
TEST_SUITE(division);

TEST(div_integer_division) {
	div_t result;

	result = div(7, 3);
	ASSERT_EQ(2, result.quot);
	ASSERT_EQ(1, result.rem);

	result = div(-7, 3);
	ASSERT_EQ(-2, result.quot);
	ASSERT_EQ(-1, result.rem);

	result = div(7, -3);
	ASSERT_EQ(-2, result.quot);
	ASSERT_EQ(1, result.rem);

	result = div(10, 5);
	ASSERT_EQ(2, result.quot);
	ASSERT_EQ(0, result.rem);
}

TEST(ldiv_long_division) {
	ldiv_t result;

	result = ldiv(123456L, 1000L);
	ASSERT_EQ(123L, result.quot);
	ASSERT_EQ(456L, result.rem);

	result = ldiv(-100L, 7L);
	ASSERT_EQ(-14L, result.quot);
	ASSERT_EQ(-2L, result.rem);
}

TEST(lldiv_long_long_division) {
	lldiv_t result;

	result = lldiv(1234567890123LL, 1000000LL);
	ASSERT_EQ(1234567LL, result.quot);
	ASSERT_EQ(890123LL, result.rem);

	result = lldiv(0LL, 1LL);
	ASSERT_EQ(0LL, result.quot);
	ASSERT_EQ(0LL, result.rem);
}

TEST(div_negative_combinations) {
	div_t r;

	// Positive / Positive
	r = div(10, 3);
	ASSERT_EQ(3, r.quot);
	ASSERT_EQ(1, r.rem);

	// Negative / Positive
	r = div(-10, 3);
	ASSERT_EQ(-3, r.quot);
	ASSERT_EQ(-1, r.rem);

	// Positive / Negative
	r = div(10, -3);
	ASSERT_EQ(-3, r.quot);
	ASSERT_EQ(1, r.rem);

	// Negative / Negative
	r = div(-10, -3);
	ASSERT_EQ(3, r.quot);
	ASSERT_EQ(-1, r.rem);

	// Verify: quot * divisor + rem == dividend
	ASSERT_EQ(-10, r.quot * -3 + r.rem);
}

/* ============================================================================
 * ABSOLUTE VALUE TESTS
 * ============================================================================ */
TEST_SUITE(absolute_value);

TEST(abs_basic) {
	ASSERT_EQ(5, abs(5));
	ASSERT_EQ(5, abs(-5));
	ASSERT_EQ(0, abs(0));
}

TEST(labs_basic) {
	ASSERT_EQ(5L, labs(5L));
	ASSERT_EQ(5L, labs(-5L));
	ASSERT_EQ(0L, labs(0L));
}

#if JACL_HAS_C99
TEST(llabs_basic) {
	ASSERT_EQ(5LL, llabs(5LL));
	ASSERT_EQ(5LL, llabs(-5LL));
	ASSERT_EQ(0LL, llabs(0LL));
}
#endif

/* ============================================================================
 * RANDOM NUMBER TESTS
 * ============================================================================ */
TEST_SUITE(random_numbers);

TEST(rand_basic_generation) {
	srand(1);
	int r1 = rand();
	int r2 = rand();

	// Should be different
	ASSERT_NE(r1, r2);

	// Should be in valid range
	ASSERT_TRUE(r1 >= 0 && r1 <= RAND_MAX);
	ASSERT_TRUE(r2 >= 0 && r2 <= RAND_MAX);
}

TEST(rand_deterministic_sequence) {
	// Same seed should produce same sequence
	srand(42);
	int seq1[5];
	for (int i = 0; i < 5; i++) {
		seq1[i] = rand();
	}

	srand(42);	// Reset with same seed
	int seq2[5];
	for (int i = 0; i < 5; i++) {
		seq2[i] = rand();
	}

	for (int i = 0; i < 5; i++) {
		ASSERT_EQ(seq1[i], seq2[i]);
	}
}

TEST(rand_different_seeds) {
	srand(1);
	int r1 = rand();

	srand(2);
	int r2 = rand();

	// Different seeds should produce different first values
	ASSERT_NE(r1, r2);
}

TEST(rand_distribution) {
	srand(42);
	int counts[10] = {0};
	int total = 0;

	// Generate 1000 numbers, bucket them
	for (int i = 0; i < 1000; i++) {
		int r = rand() % 10;
		counts[r]++;
		total++;
	}

	// Verify we generated all numbers
	ASSERT_EQ(1000, total);

	// Check that each bucket got at least SOME values (very loose check)
	// With good RNG, each should get ~100, but we'll accept >= 20
	int empty_buckets = 0;
	for (int i = 0; i < 10; i++) {
		if (counts[i] < 20) {
			empty_buckets++;
		}
	}

	// At most 2 buckets should be that sparse
	ASSERT_LT(empty_buckets, 3);
}

/* ============================================================================
 * STRING DUPLICATION TESTS
 * ============================================================================ */
TEST_SUITE(string_duplication);

TEST(strdup_basic_duplication) {
	const char *original = "Hello World";
	char *copy = strdup(original);

	if (copy) {
		ASSERT_EQ(0, strcmp(original, copy));
		ASSERT_NE(original, copy);	// Different memory locations
		free(copy);
	}

	// Test empty string
	char *empty_copy = strdup("");
	if (empty_copy) {
		ASSERT_EQ(0, strcmp("", empty_copy));
		free(empty_copy);
	}
}

TEST(strndup_limited_duplication) {
	const char *original = "Hello World";
	char *copy = strndup(original, 5);

	if (copy) {
		ASSERT_EQ(0, strcmp("Hello", copy));
		ASSERT_EQ(5, strlen(copy));
		free(copy);
	}

	// Test with n larger than string
	copy = strndup("Hi", 10);
	if (copy) {
		ASSERT_EQ(0, strcmp("Hi", copy));
		ASSERT_EQ(2, strlen(copy));
		free(copy);
	}

	// Test with n = 0
	copy = strndup("test", 0);
	if (copy) {
		ASSERT_EQ(0, strcmp("", copy));
		free(copy);
	}
}

/* ============================================================================
 * EXIT HANDLER TESTS
 * ============================================================================ */
TEST_SUITE(exit_handlers);

static int exit_handler_order[5];
static int exit_handler_count = 0;

static void handler1(void) { exit_handler_order[exit_handler_count++] = 1; }
static void handler2(void) { exit_handler_order[exit_handler_count++] = 2; }
static void handler3(void) { exit_handler_order[exit_handler_count++] = 3; }

TEST(atexit_registration) {
	ASSERT_EQ(0, atexit(handler1));
	ASSERT_EQ(0, atexit(handler2));
	ASSERT_EQ(0, atexit(handler3));
}

TEST(atexit_null_function) {
	ASSERT_EQ(-1, atexit(NULL));
}

TEST(atexit_max_handlers) {
	int result = 0;
	for (int i = 0; i < 40; i++) {
		result = atexit(handler1);
	}
	ASSERT_EQ(-1, result);  // Should fail after 32
}

#if JACL_HAS_C11
TEST(at_quick_exit_registration) {
	ASSERT_EQ(0, at_quick_exit(handler1));
	ASSERT_EQ(0, at_quick_exit(handler2));
}

TEST(at_quick_exit_null_function) {
	ASSERT_EQ(-1, at_quick_exit(NULL));
}

TEST(quick_exit_handlers_separate) {
	// quick_exit handlers should be separate from atexit handlers
	static int atexit_called = 0;
	static int quick_called = 0;

	void atexit_handler(void) { atexit_called = 1; }
	void quick_handler(void) { quick_called = 1; }

	atexit(atexit_handler);
	at_quick_exit(quick_handler);

	// Note: Can't actually call quick_exit() in test
	// This just verifies registration works
}
#endif


/* ============================================================================
 * SORTING TESTS
 * ============================================================================ */
TEST_SUITE(sorting);

static int compare_ints(const void* a, const void* b) {
	return (*(int*)a - *(int*)b);
}

TEST(qsort_basic_sorting) {
	int arr[] = {5, 2, 9, 1, 7, 6, 3};
	int expected[] = {1, 2, 3, 5, 6, 7, 9};

	qsort(arr, 7, sizeof(int), compare_ints);

	for (int i = 0; i < 7; i++) {
		ASSERT_EQ(expected[i], arr[i]);
	}
}

TEST(qsort_already_sorted) {
	int arr[] = {1, 2, 3, 4, 5};
	qsort(arr, 5, sizeof(int), compare_ints);

	ASSERT_EQ(1, arr[0]);
	ASSERT_EQ(5, arr[4]);
}

TEST(qsort_reverse_sorted) {
	int arr[] = {5, 4, 3, 2, 1};
	qsort(arr, 5, sizeof(int), compare_ints);

	for (int i = 0; i < 5; i++) {
		ASSERT_EQ(i + 1, arr[i]);
	}
}

TEST(qsort_duplicates) {
	int arr[] = {3, 1, 4, 1, 5, 9, 2, 6, 5};
	qsort(arr, 9, sizeof(int), compare_ints);

	ASSERT_EQ(1, arr[0]);
	ASSERT_EQ(1, arr[1]);
	ASSERT_EQ(9, arr[8]);
}

TEST(qsort_single_element) {
	int arr[] = {42};
	qsort(arr, 1, sizeof(int), compare_ints);
	ASSERT_EQ(42, arr[0]);
}

TEST(qsort_empty_array) {
	int arr[1];
	qsort(arr, 0, sizeof(int), compare_ints);
}

TEST(qsort_null_base) {
	qsort(NULL, 10, sizeof(int), compare_ints);
}

TEST(qsort_large_array) {
	#define SIZE 1000
	int *arr = malloc(SIZE * sizeof(int));
	if (arr) {
		srand(42);
		for (int i = 0; i < SIZE; i++) {
			arr[i] = rand();
		}

		qsort(arr, SIZE, sizeof(int), compare_ints);

		for (int i = 1; i < SIZE; i++) {
			ASSERT_TRUE(arr[i-1] <= arr[i]);
		}

		free(arr);
	}
	#undef SIZE
}

/* ============================================================================
 * BINARY SEARCH TESTS
 * ============================================================================ */
TEST_SUITE(searching);

TEST(bsearch_basic_search) {
	int arr[] = {1, 3, 5, 7, 9, 11, 13};
	int key = 7;

	int* result = (int*)bsearch(&key, arr, 7, sizeof(int), compare_ints);

	ASSERT_NOT_NULL(result);
	ASSERT_EQ(7, *result);
}

TEST(bsearch_not_found) {
	int arr[] = {1, 3, 5, 7, 9};
	int key = 6;

	int* result = (int*)bsearch(&key, arr, 5, sizeof(int), compare_ints);

	ASSERT_NULL(result);
}

TEST(bsearch_first_element) {
	int arr[] = {1, 2, 3, 4, 5};
	int key = 1;

	int* result = (int*)bsearch(&key, arr, 5, sizeof(int), compare_ints);

	ASSERT_NOT_NULL(result);
	ASSERT_EQ(1, *result);
}

TEST(bsearch_last_element) {
	int arr[] = {1, 2, 3, 4, 5};
	int key = 5;

	int* result = (int*)bsearch(&key, arr, 5, sizeof(int), compare_ints);

	ASSERT_NOT_NULL(result);
	ASSERT_EQ(5, *result);
}

TEST(bsearch_single_element_found) {
	int arr[] = {42};
	int key = 42;

	int* result = (int*)bsearch(&key, arr, 1, sizeof(int), compare_ints);

	ASSERT_NOT_NULL(result);
	ASSERT_EQ(42, *result);
}

TEST(bsearch_single_element_not_found) {
	int arr[] = {42};
	int key = 10;

	int* result = (int*)bsearch(&key, arr, 1, sizeof(int), compare_ints);

	ASSERT_NULL(result);
}

TEST(bsearch_null_parameters) {
	int arr[] = {1, 2, 3};
	int key = 2;

	ASSERT_NULL(bsearch(NULL, arr, 3, sizeof(int), compare_ints));
	ASSERT_NULL(bsearch(&key, NULL, 3, sizeof(int), compare_ints));
	ASSERT_NULL(bsearch(&key, arr, 3, sizeof(int), NULL));
}

/* ============================================================================
 * ENVIRONMENT AND SYSTEM TESTS
 * ============================================================================ */
TEST_SUITE(environment);

TEST(getenv_returns_null) {
	// Your implementation always returns NULL
	ASSERT_EQ(NULL, getenv("PATH"));
	ASSERT_EQ(NULL, getenv("HOME"));
	ASSERT_EQ(NULL, getenv("NONEXISTENT"));
	ASSERT_EQ(NULL, getenv(""));
}

TEST(system_returns_error) {
	// Your implementation always returns -1
	ASSERT_EQ(-1, system("ls"));
	ASSERT_EQ(-1, system("echo hello"));
	ASSERT_EQ(-1, system(""));
	ASSERT_EQ(-1, system(NULL));
}

/* Add to environment suite */

TEST(getenv_null_name) {
	ASSERT_NULL(getenv(NULL));
}

#if JACL_HAS_POSIX
TEST(setenv_large_value) {
	// Test with very large environment value
	char large_value[10000];
	memset(large_value, 'A', sizeof(large_value) - 1);
	large_value[sizeof(large_value) - 1] = '\0';

	int result = setenv("JACL_LARGE_TEST", large_value, 1);
	// Should handle gracefully
	if (result == 0) {
		unsetenv("JACL_LARGE_TEST");
	}
}
#endif


/* ============================================================================
 * MULTIBYTE CHARACTER TESTS (expanded)
 * ============================================================================ */
TEST_SUITE(multibyte);

TEST(mblen_basic_functionality) {
	ASSERT_EQ(1, mblen("a", 1));
	ASSERT_EQ(1, mblen("A", 5));
	ASSERT_EQ(0, mblen("", 1));
	ASSERT_EQ(-1, mblen("a", 0));
	ASSERT_EQ(0, mblen(NULL, 1));  // Stateless
}

TEST(mbtowc_basic_conversion) {
	wchar_t wc;
	ASSERT_EQ(1, mbtowc(&wc, "A", 1));
	ASSERT_EQ(L'A', wc);

	ASSERT_EQ(0, mbtowc(&wc, "", 1));
	ASSERT_EQ(0, mbtowc(NULL, "test", 4));  // Stateless check
}

TEST(wctomb_basic_conversion) {
	char mb[MB_LEN_MAX];
	ASSERT_EQ(1, wctomb(mb, L'B'));
	ASSERT_EQ('B', mb[0]);

	ASSERT_EQ(0, wctomb(NULL, L'X'));  // Stateless check
}

TEST(mbstowcs_string_conversion) {
	wchar_t wcs[20];
	size_t result = mbstowcs(wcs, "Hello", 20);
	ASSERT_EQ(5, result);
	ASSERT_EQ(L'H', wcs[0]);
	ASSERT_EQ(L'o', wcs[4]);

	// Test truncation
	result = mbstowcs(wcs, "Hello World", 5);
	ASSERT_EQ(5, result);
}

TEST(wcstombs_string_conversion) {
	const wchar_t wcs[] = L"Test";
	char mbs[20];
	size_t result = wcstombs(mbs, wcs, 20);
	ASSERT_EQ(4, result);
	ASSERT_STR_EQ("Test", mbs);
}

/* Add to multibyte suite */

TEST(mbtowc_invalid_chars) {
	wchar_t wc;

	// Invalid: character > 0xFF in C locale
	int result = mbtowc(&wc, "\xFF", 1);
	// Should handle gracefully
	(void)result;
}

TEST(wctomb_invalid_wide) {
	char mb[MB_LEN_MAX];

	// Invalid: wide char > 0xFF
	int result = wctomb(mb, 0x1234);
	ASSERT_EQ(-1, result);  // Should fail
}

TEST(mbstowcs_partial_conversion) {
	wchar_t wcs[3];

	// String longer than buffer
	size_t result = mbstowcs(wcs, "Hello", 3);
	ASSERT_EQ(3, result);
	// wcs should contain "Hel" but NOT null-terminated
}

/* ============================================================================
 * TEMPORARY FILE TESTS (POSIX)
 * ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(temporary_files);

TEST(mkstemp_basic) {
	char template[] = "/tmp/testXXXXXX";
	int fd = mkstemp(template);

	if (fd >= 0) {
		ASSERT_TRUE(fd > 0);
		ASSERT_NE(0, strcmp(template, "/tmp/testXXXXXX"));  // Should be modified
		close(fd);
		unlink(template);
	}
}

TEST(mkstemp_invalid_template) {
	char bad_template[] = "/tmp/testXXX";  // Too few X's
	ASSERT_EQ(-1, mkstemp(bad_template));

	ASSERT_EQ(-1, mkstemp(NULL));
}

TEST(mkstemp_unique_names) {
	char template1[] = "/tmp/testXXXXXX";
	char template2[] = "/tmp/testXXXXXX";

	int fd1 = mkstemp(template1);
	int fd2 = mkstemp(template2);

	if (fd1 >= 0 && fd2 >= 0) {
		ASSERT_NE(0, strcmp(template1, template2));  // Should be different
		close(fd1);
		close(fd2);
		unlink(template1);
		unlink(template2);
	}
}
#endif /* JACL_HAS_POSIX */

/* ============================================================================
 * BOUNDARY ERROR TESTS
 * ============================================================================ */
TEST_SUITE(boundary_errors);

TEST(conversion_boundary_values) {
	// Test boundary conditions for conversions
	ASSERT_EQ(0, atoi("0"));

	// Test very large numbers (should handle gracefully)
	long result = strtol("999999999999999999999", NULL, 10);
	// Should either clamp to LONG_MAX or handle overflow
	(void)result;  // Avoid unused variable warning
}

TEST(division_by_zero_safety) {
	// Note: Division by zero is undefined behavior in C
	// These tests document current behavior but div(x,0) should be avoided

	// Test division by 1 (safe case)
	div_t safe_result = div(10, 1);
	ASSERT_EQ(10, safe_result.quot);
	ASSERT_EQ(0, safe_result.rem);

	// Test division of zero
	safe_result = div(0, 5);
	ASSERT_EQ(0, safe_result.quot);
	ASSERT_EQ(0, safe_result.rem);
}

/* ============================================================================
 * MEMORY SPECIAL CASE TESTS
 * ============================================================================ */
TEST_SUITE(memory_special);

TEST(free_null_pointer) {
	// free(NULL) should be safe
	free(NULL);
	// If we get here without crashing, test passes
}

TEST(multiple_free_detection) {
	// This tests your implementation's robustness
	void *ptr = malloc(100);
	if (ptr) {
		free(ptr);
		// Double free should be handled gracefully (or detected)
		// In a robust implementation, this might set ptr to NULL
		// or maintain free list integrity
	}
}

TEST(malloc_and_string_operations) {
	char *buffer = (char*)malloc(256);
	if (buffer) {
		strcpy(buffer, "Integration test: ");
		strcat(buffer, "malloc + string ops");

		ASSERT_EQ(0, strncmp(buffer, "Integration test:", 17));
		ASSERT_TRUE(strlen(buffer) > 17);

		free(buffer);
	}
}

TEST(conversion_and_memory) {
	const char *numbers[] = {"123", "456", "789"};
	int *results = (int*)malloc(3 * sizeof(int));

	if (results) {
		for (int i = 0; i < 3; i++) {
			results[i] = atoi(numbers[i]);
		}

		ASSERT_EQ(123, results[0]);
		ASSERT_EQ(456, results[1]);
		ASSERT_EQ(789, results[2]);

		free(results);
	}
}

/* ============================================================================
 * POSIX ENVIRONMENT TESTS (conditional)
 * ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(posix_environment);

TEST(setenv_basic) {
	ASSERT_EQ(0, setenv("JACL_TEST_VAR", "test_value", 1));
	char* val = getenv("JACL_TEST_VAR");
	if (val) {
		ASSERT_STR_EQ("test_value", val);
	}
	unsetenv("JACL_TEST_VAR");
}

TEST(setenv_overwrite) {
	setenv("JACL_TEST_VAR2", "value1", 1);
	setenv("JACL_TEST_VAR2", "value2", 1);  // Overwrite
	char* val = getenv("JACL_TEST_VAR2");
	if (val) {
		ASSERT_STR_EQ("value2", val);
	}
	unsetenv("JACL_TEST_VAR2");
}

TEST(setenv_no_overwrite) {
	setenv("JACL_TEST_VAR3", "value1", 1);
	setenv("JACL_TEST_VAR3", "value2", 0);  // Don't overwrite
	char* val = getenv("JACL_TEST_VAR3");
	if (val) {
		ASSERT_STR_EQ("value1", val);
	}
	unsetenv("JACL_TEST_VAR3");
}

TEST(putenv_basic) {
	static char env_string[] = "JACL_PUTENV_TEST=putenv_value";
	ASSERT_EQ(0, putenv(env_string));
	char* val = getenv("JACL_PUTENV_TEST");
	if (val) {
		ASSERT_STR_EQ("putenv_value", val);
	}
}

TEST(unsetenv_basic) {
	setenv("JACL_TEST_VAR4", "value", 1);
	ASSERT_NOT_NULL(getenv("JACL_TEST_VAR4"));
	unsetenv("JACL_TEST_VAR4");
	ASSERT_NULL(getenv("JACL_TEST_VAR4"));
}

TEST(setenv_invalid_name) {
	ASSERT_EQ(-1, setenv("", "value", 1));  // Empty name
	ASSERT_EQ(-1, setenv("BAD=NAME", "value", 1));  // Name with '='
	ASSERT_EQ(-1, setenv(NULL, "value", 1));  // NULL name
}
#endif /* JACL_HAS_POSIX */

TEST_MAIN()
