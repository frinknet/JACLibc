/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_C99
#include <stdbool.h>
#include <stddef.h>

TEST_TYPE(unit);
TEST_UNIT(stdbool.h + stddef.h);

/* ============================================================================
 * STDBOOL.H TESTS
 * ============================================================================ */
TEST_SUITE(stdbool);

TEST(bool_type_exists) {
	bool b = true;
	ASSERT_TRUE(b);
}

TEST(true_value) {
	ASSERT_TRUE(true);
	ASSERT_EQ(1, true);
}

TEST(false_value) {
	ASSERT_FALSE(false);
	ASSERT_EQ(0, false);
}

TEST(bool_assignment) {
	bool b1 = true;
	bool b2 = false;
	
	ASSERT_TRUE(b1);
	ASSERT_FALSE(b2);
}

TEST(bool_comparison) {
	ASSERT_TRUE(true == true);
	ASSERT_TRUE(false == false);
	ASSERT_TRUE(true != false);
}

TEST(bool_logic) {
	ASSERT_TRUE(true && true);
	ASSERT_FALSE(true && false);
	ASSERT_TRUE(true || false);
	ASSERT_FALSE(false || false);
	ASSERT_TRUE(!false);
	ASSERT_FALSE(!true);
}

TEST(bool_conversion) {
	bool b1 = 1;
	bool b2 = 0;
	bool b3 = 42;  // Non-zero converts to true
	
	ASSERT_TRUE(b1);
	ASSERT_FALSE(b2);
	ASSERT_TRUE(b3);
}

TEST(bool_macro_defined) {
	ASSERT_EQ(1, __bool_true_false_are_defined);
}

TEST(bool_sizeof) {
	// bool should be 1 byte typically
	ASSERT_TRUE(sizeof(bool) >= 1);
	ASSERT_TRUE(sizeof(bool) <= sizeof(int));
}

TEST(bool_in_expressions) {
	bool condition = (5 > 3);
	ASSERT_TRUE(condition);
	
	bool result = (10 < 5);
	ASSERT_FALSE(result);
}

/* ============================================================================
 * STDDEF.H TESTS
 * ============================================================================ */
TEST_SUITE(stddef);

TEST(null_defined) {
	void *ptr = NULL;
	ASSERT_NULL(ptr);
}

TEST(null_value) {
	ASSERT_EQ(0, (intptr_t)NULL);
}

TEST(size_t_defined) {
	size_t sz = 42;
	ASSERT_EQ(42, sz);
}

TEST(size_t_unsigned) {
	size_t sz = -1;  // Wraps to max value
	ASSERT_TRUE(sz > 0);
}

TEST(size_t_sizeof) {
	// size_t should match pointer size
	ASSERT_EQ(sizeof(void*), sizeof(size_t));
}

TEST(ptrdiff_t_defined) {
	ptrdiff_t diff = 10 - 5;
	ASSERT_EQ(5, diff);
}

TEST(ptrdiff_t_signed) {
	ptrdiff_t positive = 10;
	ptrdiff_t negative = -10;
	
	ASSERT_TRUE(positive > 0);
	ASSERT_TRUE(negative < 0);
}

TEST(ptrdiff_t_pointer_arithmetic) {
	int arr[10];
	ptrdiff_t diff = &arr[5] - &arr[2];
	
	ASSERT_EQ(3, diff);
}

#if defined(offsetof)
TEST(offsetof_macro) {
	struct test_struct {
		char a;
		int b;
		char c;
	};
	
	// offsetof should give offset of member
	size_t offset_b = offsetof(struct test_struct, b);
	size_t offset_c = offsetof(struct test_struct, c);
	
	ASSERT_TRUE(offset_b > 0);
	ASSERT_TRUE(offset_c > offset_b);
}
#endif

TEST(sizeof_operator) {
	// Verify sizeof returns size_t
	size_t int_size = sizeof(int);
	ASSERT_TRUE(int_size >= 4);
}

/* ============================================================================
 * COMBINED TESTS
 * ============================================================================ */
TEST_SUITE(combined);

TEST(bool_array) {
	bool flags[5] = {true, false, true, true, false};
	
	ASSERT_TRUE(flags[0]);
	ASSERT_FALSE(flags[1]);
	ASSERT_TRUE(flags[2]);
}

TEST(bool_in_struct) {
	struct data {
		bool valid;
		int value;
	};
	
	struct data d = {true, 42};
	ASSERT_TRUE(d.valid);
	ASSERT_EQ(42, d.value);
}

TEST(size_t_loop) {
	size_t count = 0;
	for (size_t i = 0; i < 10; i++) {
		count++;
	}
	ASSERT_EQ(10, count);
}

TEST(null_pointer_check) {
	int *ptr = NULL;
	bool is_null = (ptr == NULL);
	
	ASSERT_TRUE(is_null);
}

TEST_MAIN()

#else

int main(void) {
	printf("stdbool.h requires C99 or later\n");
	return 0;
}

#endif
