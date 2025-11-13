/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <stdbool.h>

TEST_TYPE(unit);
TEST_UNIT(stdbool.h);

/* ============================================================================
 * BOOL TYPE
 * ============================================================================ */
TEST_SUITE(bool_type);

TEST(bool_type_exists) {
	bool b = true;
	(void)b;
	ASSERT_TRUE(sizeof(bool) > 0);
}

TEST(bool_size) {
	ASSERT_TRUE(sizeof(bool) >= 1);
	ASSERT_TRUE(sizeof(bool) <= sizeof(int));
}

TEST(bool_is_unsigned) {
	bool b = true;
	ASSERT_TRUE(b > 0);
}

/* ============================================================================
 * TRUE CONSTANT
 * ============================================================================ */
TEST_SUITE(true_value);

TEST(true_is_nonzero) {
	ASSERT_TRUE(true);
	ASSERT_TRUE(true != 0);
}

TEST(true_equals_one) {
	ASSERT_EQ(1, true);
}

TEST(true_assignment) {
	bool b = true;
	ASSERT_TRUE(b);
	ASSERT_EQ(true, b);
}

/* ============================================================================
 * FALSE CONSTANT
 * ============================================================================ */
TEST_SUITE(false_value);

TEST(false_is_zero) {
	ASSERT_FALSE(false);
	ASSERT_EQ(0, false);
}

TEST(false_assignment) {
	bool b = false;
	ASSERT_FALSE(b);
	ASSERT_EQ(false, b);
}

/* ============================================================================
 * BOOL COMPARISONS
 * ============================================================================ */
TEST_SUITE(comparisons);

TEST(true_equals_true) {
	ASSERT_TRUE(true == true);
}

TEST(false_equals_false) {
	ASSERT_TRUE(false == false);
}

TEST(true_not_equal_false) {
	ASSERT_TRUE(true != false);
	ASSERT_FALSE(true == false);
}

TEST(bool_variable_comparisons) {
	bool t = true;
	bool f = false;

	ASSERT_TRUE(t == true);
	ASSERT_TRUE(f == false);
	ASSERT_TRUE(t != f);
}

/* ============================================================================
 * LOGICAL OPERATIONS
 * ============================================================================ */
TEST_SUITE(logical_ops);

TEST(and_operations) {
	ASSERT_TRUE(true && true);
	ASSERT_FALSE(true && false);
	ASSERT_FALSE(false && true);
	ASSERT_FALSE(false && false);
}

TEST(or_operations) {
	ASSERT_TRUE(true || true);
	ASSERT_TRUE(true || false);
	ASSERT_TRUE(false || true);
	ASSERT_FALSE(false || false);
}

TEST(not_operation) {
	ASSERT_TRUE(!false);
	ASSERT_FALSE(!true);
}

TEST(combined_logic) {
	ASSERT_TRUE((true && true) || false);
	ASSERT_FALSE((false || false) && true);
	ASSERT_TRUE(!(false && true));
}

/* ============================================================================
 * BOOL CONVERSIONS
 * ============================================================================ */
TEST_SUITE(conversions);

TEST(integer_to_bool_nonzero) {
	bool b1 = 1;
	bool b2 = 42;
	bool b3 = -1;

	ASSERT_TRUE(b1);
	ASSERT_TRUE(b2);
	ASSERT_TRUE(b3);
}

TEST(integer_to_bool_zero) {
	bool b = 0;
	ASSERT_FALSE(b);
}

TEST(comparison_to_bool) {
	bool b1 = (5 > 3);
	bool b2 = (10 < 5);

	ASSERT_TRUE(b1);
	ASSERT_FALSE(b2);
}

TEST(pointer_to_bool) {
	int x = 42;
	int *p1 = &x;
	int *p2 = NULL;

	bool b1 = (p1 != NULL);
	bool b2 = (p2 == NULL);

	ASSERT_TRUE(b1);
	ASSERT_TRUE(b2);
}

/* ============================================================================
 * CONTROL FLOW
 * ============================================================================ */
TEST_SUITE(control_flow);

TEST(bool_in_if_statement) {
	bool condition = true;
	int result = 0;

	if (condition) {
		result = 1;
	}

	ASSERT_EQ(1, result);
}

TEST(bool_in_while_loop) {
	bool running = true;
	int count = 0;

	while (running) {
		count++;
		if (count >= 5) {
			running = false;
		}
	}

	ASSERT_EQ(5, count);
}

TEST(bool_ternary_operator) {
	bool flag = true;
	int x = flag ? 10 : 20;

	ASSERT_EQ(10, x);
}

/* ============================================================================
 * ARRAYS AND STRUCTURES
 * ============================================================================ */
TEST_SUITE(composite_types);

TEST(bool_array) {
	bool flags[5] = {true, false, true, true, false};

	ASSERT_TRUE(flags[0]);
	ASSERT_FALSE(flags[1]);
	ASSERT_TRUE(flags[2]);
	ASSERT_TRUE(flags[3]);
	ASSERT_FALSE(flags[4]);
}

TEST(bool_in_struct) {
	struct state {
		bool active;
		bool valid;
		int value;
	};

	struct state s = {true, false, 42};

	ASSERT_TRUE(s.active);
	ASSERT_FALSE(s.valid);
	ASSERT_EQ(42, s.value);
}

TEST(bool_array_initialization) {
	bool all_false[3] = {false, false, false};
	bool mixed[4] = {true, false, true, false};

	ASSERT_FALSE(all_false[0]);
	ASSERT_FALSE(all_false[1]);
	ASSERT_FALSE(all_false[2]);

	ASSERT_TRUE(mixed[0]);
	ASSERT_FALSE(mixed[1]);
	ASSERT_TRUE(mixed[2]);
	ASSERT_FALSE(mixed[3]);
}

/* ============================================================================
 * STANDARD MACRO
 * ============================================================================ */
TEST_SUITE(standard_macro);

TEST(bool_true_false_are_defined) {
	ASSERT_EQ(1, __bool_true_false_are_defined);
}

TEST_MAIN()

