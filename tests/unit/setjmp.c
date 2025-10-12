/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <setjmp.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(setjmp.h);

/* ============================================================================
 * BASIC SETJMP/LONGJMP TESTS
 * ============================================================================ */
TEST_SUITE(basic_jumps);

TEST(setjmp_returns_zero) {
	jmp_buf env;
	int result = setjmp(env);
	
	// First call should return 0
	ASSERT_EQ(0, result);
}

TEST(longjmp_simple) {
	jmp_buf env;
	int reached = 0;
	
	if (setjmp(env) == 0) {
		reached = 1;
		longjmp(env, 1);
		reached = 2;  // Should never reach here
	} else {
		reached = 3;
	}
	
	ASSERT_EQ(3, reached);
}

TEST(longjmp_returns_value) {
	jmp_buf env;
	int value = 0;
	
	value = setjmp(env);
	if (value == 0) {
		longjmp(env, 42);
	}
	
	ASSERT_EQ(42, value);
}

TEST(longjmp_zero_becomes_one) {
	jmp_buf env;
	int value = 0;
	
	value = setjmp(env);
	if (value == 0) {
		longjmp(env, 0);  // Pass 0
	}
	
	// Should return 1, not 0
	ASSERT_EQ(1, value);
}

/* ============================================================================
 * CONTROL FLOW TESTS
 * ============================================================================ */
TEST_SUITE(control_flow);

TEST(longjmp_skips_code) {
	jmp_buf env;
	int counter = 0;
	
	if (setjmp(env) == 0) {
		counter = 1;
		longjmp(env, 1);
		counter = 2;  // Should not execute
	}
	
	ASSERT_EQ(1, counter);
}

TEST(nested_function_longjmp) {
	jmp_buf env;
	int result = 0;
	
	void nested(void) {
		longjmp(env, 99);
	}
	
	if (setjmp(env) == 0) {
		nested();
		result = 1;  // Should not reach
	} else {
		result = 2;
	}
	
	ASSERT_EQ(2, result);
}

TEST(multiple_levels_deep) {
	jmp_buf env;
	int depth = 0;
	
	void level3(void) {
		depth = 3;
		longjmp(env, 3);
	}
	
	void level2(void) {
		depth = 2;
		level3();
	}
	
	void level1(void) {
		depth = 1;
		level2();
	}
	
	int val = setjmp(env);
	if (val == 0) {
		level1();
	}
	
	ASSERT_EQ(3, val);
	ASSERT_EQ(3, depth);
}

/* ============================================================================
 * VARIABLE PRESERVATION TESTS
 * ============================================================================ */
TEST_SUITE(variable_preservation);

TEST(local_vars_before_setjmp) {
	jmp_buf env;
	int x = 10;
	
	if (setjmp(env) == 0) {
		x = 20;
		longjmp(env, 1);
	}
	
	// Non-volatile local may have unpredictable value
	// (This is implementation-defined)
	ASSERT_TRUE(x == 10 || x == 20);
}

TEST(volatile_vars_preserved) {
	jmp_buf env;
	volatile int x = 10;
	
	if (setjmp(env) == 0) {
		x = 20;
		longjmp(env, 1);
	}
	
	// Volatile variables should retain modified value
	ASSERT_EQ(20, x);
}

TEST(static_vars_preserved) {
	jmp_buf env;
	static int counter = 0;
	
	if (setjmp(env) == 0) {
		counter++;
		if (counter < 3) {
			longjmp(env, counter);
		}
	}
	
	ASSERT_EQ(3, counter);
}

TEST(global_state_preserved) {
	static int global_counter = 0;
	jmp_buf env;
	
	global_counter = 0;
	
	if (setjmp(env) == 0) {
		global_counter = 99;
		longjmp(env, 1);
	}
	
	ASSERT_EQ(99, global_counter);
}

/* ============================================================================
 * MULTIPLE JUMP BUFFER TESTS
 * ============================================================================ */
TEST_SUITE(multiple_buffers);

TEST(two_jump_buffers) {
	jmp_buf env1, env2;
	int which = 0;
	
	if (setjmp(env1) == 0) {
		if (setjmp(env2) == 0) {
			which = 1;
			longjmp(env1, 1);
		} else {
			which = 2;
		}
	} else {
		if (which == 1) {
			longjmp(env2, 2);
		}
	}
	
	ASSERT_EQ(2, which);
}

TEST(selective_longjmp) {
	jmp_buf buf1, buf2;
	int result = 0;
	
	if (setjmp(buf1) == 0) {
		setjmp(buf2);
		result = 1;
		longjmp(buf1, 10);
	} else {
		result = 2;
	}
	
	ASSERT_EQ(2, result);
}

/* ============================================================================
 * LOOP TESTS
 * ============================================================================ */
TEST_SUITE(loops);

TEST(longjmp_in_loop) {
	jmp_buf env;
	int iterations = 0;
	
	if (setjmp(env) == 0) {
		for (int i = 0; i < 5; i++) {
			iterations++;
			if (i == 2) {
				longjmp(env, 1);
			}
		}
	}
	
	ASSERT_EQ(3, iterations);
}

TEST(retry_pattern) {
	jmp_buf env;
	static int attempts = 0;
	
	attempts = 0;
	
	int result = setjmp(env);
	attempts++;
	
	if (result == 0 && attempts < 3) {
		longjmp(env, attempts);
	}
	
	ASSERT_EQ(3, attempts);
}

/* ============================================================================
 * VALUE PASSING TESTS
 * ============================================================================ */
TEST_SUITE(value_passing);

TEST(longjmp_different_values) {
	jmp_buf env;
	static int count = 0;
	
	count = 0;
	int val = setjmp(env);
	
	if (val == 0) {
		longjmp(env, 1);
	} else if (val == 1) {
		count = 1;
		longjmp(env, 2);
	} else if (val == 2) {
		count = 2;
		longjmp(env, 3);
	} else {
		count = 3;
	}
	
	ASSERT_EQ(3, count);
}

TEST(longjmp_negative_values) {
	jmp_buf env;
	
	int val = setjmp(env);
	if (val == 0) {
		longjmp(env, -42);
	}
	
	ASSERT_EQ(-42, val);
}

TEST(longjmp_large_values) {
	jmp_buf env;
	
	int val = setjmp(env);
	if (val == 0) {
		longjmp(env, 999999);
	}
	
	ASSERT_EQ(999999, val);
}

/* ============================================================================
 * ERROR HANDLING PATTERN TESTS
 * ============================================================================ */
TEST_SUITE(error_handling);

TEST(exception_simulation) {
	jmp_buf error_handler;
	int error_code = 0;
	
	void might_fail(int should_fail) {
		if (should_fail) {
			longjmp(error_handler, 100);
		}
	}
	
	error_code = setjmp(error_handler);
	if (error_code == 0) {
		might_fail(0);  // Success
		might_fail(1);  // Fail
	}
	
	ASSERT_EQ(100, error_code);
}

TEST(cleanup_on_error) {
	jmp_buf env;
	int cleanup_done = 0;
	
	void cleanup(void) {
		cleanup_done = 1;
	}
	
	if (setjmp(env) == 0) {
		// Simulate error
		cleanup();
		longjmp(env, 1);
	}
	
	ASSERT_EQ(1, cleanup_done);
}

/* ============================================================================
 * STACK UNWINDING TESTS
 * ============================================================================ */
TEST_SUITE(stack_unwinding);

TEST(local_arrays_preserved) {
	jmp_buf env;
	volatile int arr[5] = {1, 2, 3, 4, 5};
	
	if (setjmp(env) == 0) {
		arr[2] = 99;
		longjmp(env, 1);
	}
	
	ASSERT_EQ(99, arr[2]);
}

TEST(struct_preservation) {
	jmp_buf env;
	
	struct {
		volatile int a;
		volatile int b;
	} data = {10, 20};
	
	if (setjmp(env) == 0) {
		data.a = 30;
		data.b = 40;
		longjmp(env, 1);
	}
	
	ASSERT_EQ(30, data.a);
	ASSERT_EQ(40, data.b);
}

/* ============================================================================
 * SIGNAL VERSIONS TESTS (POSIX only)
 * ============================================================================ */
#if JACL_HAS_POSIX && !defined(JACL_ARCH_WASM)
TEST_SUITE(signal_versions);

TEST(sigsetjmp_basic) {
	sigjmp_buf env;
	int result = sigsetjmp(env, 0);
	
	ASSERT_EQ(0, result);
}

TEST(siglongjmp_basic) {
	sigjmp_buf env;
	int value = 0;
	
	value = sigsetjmp(env, 0);
	if (value == 0) {
		siglongjmp(env, 42);
	}
	
	ASSERT_EQ(42, value);
}

TEST(sigsetjmp_with_mask) {
	sigjmp_buf env;
	int value = 0;
	
	value = sigsetjmp(env, 1);  // Save signal mask
	if (value == 0) {
		siglongjmp(env, 1);
	}
	
	ASSERT_EQ(1, value);
}
#endif

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(setjmp_multiple_calls) {
	jmp_buf env;
	
	// Can call setjmp multiple times
	ASSERT_EQ(0, setjmp(env));
	ASSERT_EQ(0, setjmp(env));
	ASSERT_EQ(0, setjmp(env));
}

TEST(jmp_buf_copy) {
	jmp_buf env1, env2;
	
	setjmp(env1);
	memcpy(env2, env1, sizeof(jmp_buf));
	
	// Copied buffer should work
	int val = setjmp(env2);
	if (val == 0) {
		longjmp(env2, 5);
	}
	
	ASSERT_EQ(5, val);
}

#ifndef JACL_ARCH_WASM
TEST(jmp_buf_size) {
	// jmp_buf should have reasonable size
	ASSERT_TRUE(sizeof(jmp_buf) > 0);
	ASSERT_TRUE(sizeof(jmp_buf) < 1024);  // Sanity check
}
#endif

/* ============================================================================
 * PERFORMANCE TESTS
 * ============================================================================ */
TEST_SUITE(performance);

TEST(repeated_setjmp) {
	jmp_buf env;
	
	for (int i = 0; i < 100; i++) {
		ASSERT_EQ(0, setjmp(env));
	}
}

TEST(repeated_longjmp) {
	jmp_buf env;
	static int counter = 0;
	
	counter = 0;
	
	if (setjmp(env) == 0) {
		while (counter < 100) {
			counter++;
			if (counter < 100) {
				longjmp(env, counter);
			}
		}
	}
	
	ASSERT_EQ(100, counter);
}

/* ============================================================================
 * DOCUMENTATION EXAMPLES
 * ============================================================================ */
TEST_SUITE(documentation);

TEST(typical_usage_pattern) {
	jmp_buf error_buf;
	int error = 0;
	
	error = setjmp(error_buf);
	if (error == 0) {
		// Normal execution
		longjmp(error_buf, 1);
	} else {
		// Error handling
		ASSERT_EQ(1, error);
	}
}

TEST(resource_cleanup_pattern) {
	jmp_buf cleanup_point;
	volatile int resource_allocated = 0;
	
	if (setjmp(cleanup_point) == 0) {
		resource_allocated = 1;
		// Simulate error
		longjmp(cleanup_point, 1);
	}
	
	// Cleanup
	if (resource_allocated) {
		resource_allocated = 0;
	}
	
	ASSERT_EQ(0, resource_allocated);
}

TEST_MAIN()
