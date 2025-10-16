/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

// We need to test assert with NDEBUG both defined and undefined
// This requires careful test design since assert() terminates

#ifndef NDEBUG
// First test with assertions enabled
#include <assert.h>
#include <signal.h>
#include <setjmp.h>

TEST_TYPE(unit);
TEST_UNIT(assert.h);

/* ============================================================================
 * ASSERTION PASS TESTS (should not fail)
 * ============================================================================ */
TEST_SUITE(pass_conditions);

TEST(assert_true_literal) {
	// Should not fail
	assert(1);
	assert(2);
	assert(-1);
	ASSERT_TRUE(1);  // If we got here, assertion passed
}

TEST(assert_true_expression) {
	int x = 5;
	assert(x == 5);
	assert(x > 0);
	assert(x != 0);
	ASSERT_TRUE(1);
}

TEST(assert_pointer_non_null) {
	int value = 42;
	int *ptr = &value;
	assert(ptr != NULL);
	assert(ptr);
	ASSERT_TRUE(1);
}

TEST(assert_string_non_empty) {
	const char *str = "test";
	assert(str);
	assert(str[0]);
	ASSERT_TRUE(1);
}

TEST(assert_arithmetic) {
	assert(1 + 1 == 2);
	assert(5 * 5 == 25);
	assert(10 / 2 == 5);
	ASSERT_TRUE(1);
}

TEST(assert_logical) {
	assert(1 && 1);
	assert(1 || 0);
	assert(!(0));
	ASSERT_TRUE(1);
}

TEST(assert_comparison) {
	assert(5 > 3);
	assert(3 < 5);
	assert(5 >= 5);
	assert(5 <= 5);
	ASSERT_TRUE(1);
}

/* ============================================================================
 * ASSERTION EVALUATION TESTS
 * ============================================================================ */
TEST_SUITE(evaluation);

TEST(assert_evaluates_expression) {
	int counter = 0;

	// Expression should be evaluated
	assert((counter++, 1));
	ASSERT_EQ(1, counter);

	assert((counter++, 1));
	ASSERT_EQ(2, counter);
}

TEST(assert_short_circuit) {
	int called = 0;

	// Should short-circuit on true condition
	assert(1 || (called = 1, 0));
	ASSERT_EQ(0, called);
}

TEST(assert_with_side_effects) {
	int x = 0;

	// Side effect should occur
	assert((x = 5, 1));
	ASSERT_EQ(5, x);
}

/* ============================================================================
 * COMPLEX EXPRESSION TESTS
 * ============================================================================ */
TEST_SUITE(complex_expressions);

static int test_func(int x) {
	return x * 2;
}

TEST(assert_function_call) {
	assert(test_func(5) == 10);
	ASSERT_TRUE(1);
}

TEST(assert_nested_logic) {
	int a = 5, b = 3, c = 7;

	assert((a > b) && (c > a));
	assert((a > 0) || (b < 0));
	ASSERT_TRUE(1);
}

TEST(assert_ternary) {
	int x = 5;
	assert((x > 0) ? 1 : 0);
	ASSERT_TRUE(1);
}

TEST(assert_compound) {
	int arr[5] = {1, 2, 3, 4, 5};
	assert(arr[2] == 3);
	assert(sizeof(arr) == 5 * sizeof(int));
	ASSERT_TRUE(1);
}

/* ============================================================================
 * STATIC ASSERTION TESTS (compile-time)
 * ============================================================================ */
TEST_SUITE(static_assertions);

// These are compile-time checks
static_assert(1, "literal true");
static_assert(sizeof(int) >= 2, "int size");
static_assert(sizeof(char) == 1, "char size");

TEST(static_assert_compiles) {
	// If we compiled, static_assert passed
	ASSERT_TRUE(1);
}

TEST(static_assert_in_function) {
	static_assert(1 + 1 == 2, "math works");
	static_assert(sizeof(void*) >= 4, "pointer size");
	ASSERT_TRUE(1);
}

/* ============================================================================
 * EDGE CASE TESTS
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(assert_zero_vs_null) {
	int zero = 0;
	int *null_ptr = NULL;

	// These should pass (non-zero/non-null)
	assert(zero == 0);
	assert(!zero);
	assert(null_ptr == NULL);
	ASSERT_TRUE(1);
}

TEST(assert_with_casts) {
	float f = 5.5f;
	assert((int)f == 5);
	assert(f > 5.0f);
	ASSERT_TRUE(1);
}

TEST(assert_string_comparison) {
	const char *a = "test";
	const char *b = "test";

	// Pointer comparison (may be same in some implementations)
	assert(a || b);  // At least one is non-null
	ASSERT_TRUE(1);
}

/* ============================================================================
 * MACRO EXPANSION TESTS
 * ============================================================================ */
TEST_SUITE(macro_behavior);

TEST(assert_with_macros) {
	#define TRUE_VALUE 1
	#define FALSE_VALUE 0

	assert(TRUE_VALUE);
	assert(!FALSE_VALUE);

	#undef TRUE_VALUE
	#undef FALSE_VALUE

	ASSERT_TRUE(1);
}

TEST(assert_multiple_same_line) {
	// Multiple assertions can coexist
	assert(1); assert(2); assert(3);
	ASSERT_TRUE(1);
}

/* ============================================================================
 * TYPE TESTS
 * ============================================================================ */
TEST_SUITE(type_tests);

TEST(assert_with_different_types) {
	char c = 'A';
	short s = 100;
	long l = 1000L;
	unsigned u = 42U;

	assert(c == 'A');
	assert(s > 0);
	assert(l > 0);
	assert(u > 0);
	ASSERT_TRUE(1);
}

TEST(assert_with_float_double) {
	float f = 1.5f;
	double d = 2.5;

	assert(f > 1.0f);
	assert(d > 2.0);
	ASSERT_TRUE(1);
}

/* ============================================================================
 * DOCUMENTATION TESTS
 * ============================================================================ */
TEST_SUITE(usage_patterns);

void example_func(int *ptr, int size) {
	assert(ptr != NULL);
	assert(size > 0);
	// Function body would go here
}

TEST(assert_preconditions) {
	int arr[5];
	example_func(arr, 5);
	ASSERT_TRUE(1);
}

int compute(int x) {
	int result = x * 2;
	assert(result == x * 2);
	return result;
}

TEST(assert_postconditions) {
	ASSERT_EQ(10, compute(5));
}

TEST(assert_invariants) {
	int balance = 100;

	// Check invariant
	assert(balance >= 0);

	balance -= 50;
	assert(balance >= 0);

	ASSERT_EQ(50, balance);
}

TEST_MAIN()

#else
/* ============================================================================
 * NDEBUG DEFINED - Assertions disabled
 * ============================================================================ */
#include <assert.h>

TEST_TYPE(unit);
TEST_UNIT(assert.h);

TEST_SUITE(ndebug_mode);

TEST(assert_disabled_no_op) {
	// With NDEBUG, assert should be a no-op
	assert(0);  // Would fail if enabled
	assert(0 && "This won't trigger");
	ASSERT_TRUE(1);  // Test framework still works
}

TEST(assert_no_side_effects) {
	int counter = 0;

	// Side effects should NOT occur with NDEBUG
	assert((counter++, 0));
	ASSERT_EQ(0, counter);  // Counter should not have incremented
}

TEST(static_assert_still_works) {
	// static_assert should still work with NDEBUG
	static_assert(1, "compile time check");
	ASSERT_TRUE(1);
}

TEST_MAIN()

#endif
