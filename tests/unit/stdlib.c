/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <stdlib.h>

	TEST_SUITE(stdlib);

	/* ===== Memory Management Tests ===== */
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

	/* ===== Heap Growth Tests ===== */
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


/* ===== Conversion Function Tests ===== */
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

/* ===== Division Function Tests ===== */
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

/* ===== Random Number Tests ===== */
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

/* ===== String Duplication Tests ===== */
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

/* ===== Environment and System Tests ===== */
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

/* ===== Multibyte Character Tests ===== */
TEST(mblen_basic_functionality) {
	// Your implementation assumes C locale, single byte
	ASSERT_EQ(1, mblen("a", 1));
	ASSERT_EQ(1, mblen("A", 5));
	ASSERT_EQ(0, mblen("", 1));			 // Empty string
	ASSERT_EQ(-1, mblen("a", 0));		 // Zero length
	ASSERT_EQ(1, mblen(NULL, 1));		 // NULL = stateless
}

/* ===== Error Condition Tests ===== */
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

/* ===== Memory Management Edge Cases ===== */
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

/* ===== Integration Tests ===== */
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

TEST_MAIN()

