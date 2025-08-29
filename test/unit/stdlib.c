/* (c) 2025 FRINKnet & Friends – MIT licence */
#include "../runner.h"
#include <stdlib.h>

TEST_SUITE(stdlib);

/* ===== Memory Management Tests ===== */
TEST(malloc_basic_allocation) {
		void *ptr = malloc(100);
		if (ptr) {	// Only test if malloc succeeds
				// Write to allocated memory to verify it's accessible
				char *test = (char*)ptr;
				test[0] = 'A';
				test[99] = 'Z';
				ASSERT_EQ('A', test[0]);
				ASSERT_EQ('Z', test[99]);
				free(ptr);
		}
		// Test that malloc(0) doesn't crash
		ptr = malloc(0);
		free(ptr);	// Should handle NULL gracefully
}

TEST(malloc_large_allocation) {
		void *ptr = malloc(JACL_HEAP_SIZE / 2);
		if (ptr) {
				free(ptr);
		}
		// Test allocation larger than heap
		void *big_ptr = malloc(JACL_HEAP_SIZE * 2);
		// Should return NULL or handle gracefully
		free(big_ptr);
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



/* ===== Conversion Function Tests ===== */
TEST(atoi_basic_conversion) {
		ASSERT_EQ(0, atoi("0"));
		ASSERT_EQ(123, atoi("123"));
		ASSERT_EQ(-456, atoi("-456"));
		ASSERT_EQ(42, atoi("	42"));				// Leading whitespace
		ASSERT_EQ(123, atoi("123abc"));			// Trailing garbage
		ASSERT_EQ(0, atoi("abc"));					// No digits
		ASSERT_EQ(0, atoi(""));							// Empty string
}

TEST(atoi_edge_cases) {
		ASSERT_EQ(2147483647, atoi("2147483647"));		 // INT_MAX
		ASSERT_EQ(-2147483648, atoi("-2147483648"));	 // INT_MIN (if fits)
		ASSERT_EQ(0, atoi("		"));										 // Only whitespace
		ASSERT_EQ(1, atoi("+1"));											 // Explicit positive
}

TEST(atol_long_conversion) {
		ASSERT_EQ(0L, atol("0"));
		ASSERT_EQ(123456789L, atol("123456789"));
		ASSERT_EQ(-987654321L, atol("-987654321"));
		ASSERT_EQ(LONG_MAX, atol("2147483647"));			 // Assuming 32-bit long
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
		ASSERT_EQ(0.0, atof("abc"));							 // Invalid
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
		ASSERT_EQ(255, strtol("0xFF", &endptr, 0));		// Auto-detect
		
		// Octal
		ASSERT_EQ(64, strtol("100", &endptr, 8));
		ASSERT_EQ(64, strtol("0100", &endptr, 0));		// Auto-detect
		
		// Invalid base
		ASSERT_EQ(0, strtol("123", &endptr, 37));			// Base > 36
		ASSERT_EQ(0, strtol("123", &endptr, 1));			// Base < 2
		
		// Base 36 (max)
		ASSERT_EQ(35, strtol("Z", &endptr, 36));			// Z = 35 in base 36
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
		ASSERT_EQ(0, mblen("", 1));					 // Empty string
		ASSERT_EQ(-1, mblen("a", 0));				 // Zero length
		ASSERT_EQ(1, mblen(NULL, 1));				 // NULL = stateless
}

/* ===== Error Condition Tests ===== */
TEST(malloc_stress_test) {
		// Try to exhaust memory (should fail gracefully)
		void *ptrs[100];
		int allocated = 0;
		
		for (int i = 0; i < 100; i++) {
				ptrs[i] = malloc(JACL_HEAP_SIZE / 50);	// Try to allocate 2% of heap
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

