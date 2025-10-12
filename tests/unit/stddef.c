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
	bool b3 = 42;
	
	ASSERT_TRUE(b1);
	ASSERT_FALSE(b2);
	ASSERT_TRUE(b3);
}

TEST(bool_macro_defined) {
	ASSERT_EQ(1, __bool_true_false_are_defined);
}

TEST(bool_sizeof) {
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
 * STDDEF.H - BASIC TYPES
 * ============================================================================ */
TEST_SUITE(stddef_basic);

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
	size_t sz = -1;
	ASSERT_TRUE(sz > 0);
}

TEST(size_t_architecture_aware) {
	#if JACL_64BIT
		ASSERT_EQ(8, sizeof(size_t));
	#else
		ASSERT_EQ(4, sizeof(size_t));
	#endif
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

TEST(ptrdiff_t_architecture_aware) {
	#if JACL_64BIT
		ASSERT_EQ(8, sizeof(ptrdiff_t));
	#else
		ASSERT_EQ(4, sizeof(ptrdiff_t));
	#endif
}

TEST(wchar_t_defined) {
	wchar_t wc = L'A';
	ASSERT_TRUE(wc > 0);
}

/* ============================================================================
 * STDDEF.H - OFFSETOF
 * ============================================================================ */
TEST_SUITE(offsetof);

TEST(offsetof_basic) {
	struct test {
		char a;
		int b;
	};
	
	size_t offset = offsetof(struct test, b);
	ASSERT_TRUE(offset > 0);
}

TEST(offsetof_first_member) {
	struct test {
		int first;
		int second;
	};
	
	ASSERT_EQ(0, offsetof(struct test, first));
}

TEST(offsetof_multiple_members) {
	struct test {
		char a;
		int b;
		char c;
		long d;
	};
	
	size_t off_b = offsetof(struct test, b);
	size_t off_c = offsetof(struct test, c);
	size_t off_d = offsetof(struct test, d);
	
	ASSERT_TRUE(off_b > 0);
	ASSERT_TRUE(off_c > off_b);
	ASSERT_TRUE(off_d > off_c);
}

TEST(offsetof_array_member) {
	struct test {
		int x;
		char arr[10];
	};
	
	size_t offset = offsetof(struct test, arr);
	ASSERT_TRUE(offset >= sizeof(int));
}

/* ============================================================================
 * STDDEF.H - MAX_ALIGN_T (C11)
 * ============================================================================ */
#if JACL_HAS_C11
TEST_SUITE(max_align_t);

TEST(max_align_t_defined) {
	max_align_t m;
	(void)m;
	ASSERT_TRUE(sizeof(max_align_t) > 0);
}

TEST(max_align_t_alignment) {
	// max_align_t should have largest alignment
	ASSERT_TRUE(sizeof(max_align_t) >= sizeof(long long));
	ASSERT_TRUE(sizeof(max_align_t) >= sizeof(long double));
	ASSERT_TRUE(sizeof(max_align_t) >= sizeof(void*));
}
#endif

/* ============================================================================
 * STDDEF.H - KEYWORD MACROS
 * ============================================================================ */
TEST_SUITE(keyword_macros);

TEST(restrict_defined) {
	// __restrict should be defined
	int arr[10];
	int * __restrict ptr = arr;
	ptr[0] = 42;
	ASSERT_EQ(42, ptr[0]);
}

TEST(inline_defined) {
	// __inline should be defined
	__inline int test_func(void) { return 42; }
	ASSERT_EQ(42, test_func());
}

/* ============================================================================
 * COMBINED USAGE TESTS
 * ============================================================================ */
TEST_SUITE(combined_usage);

TEST(bool_array) {
	bool flags[5] = {true, false, true, true, false};
	
	ASSERT_TRUE(flags[0]);
	ASSERT_FALSE(flags[1]);
	ASSERT_TRUE(flags[2]);
}

TEST(bool_in_struct) {
	struct data {
		bool valid;
		size_t count;
	};
	
	struct data d = {true, 42};
	ASSERT_TRUE(d.valid);
	ASSERT_EQ(42, d.count);
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

TEST(pointer_difference_with_ptrdiff) {
	int arr[100];
	ptrdiff_t diff = &arr[99] - &arr[0];
	ASSERT_EQ(99, diff);
}

TEST(offsetof_with_sizeof) {
	struct test {
		int a;
		char b;
		long c;
	};
	
	size_t total = sizeof(struct test);
	size_t offset_c = offsetof(struct test, c);
	
	ASSERT_TRUE(total > offset_c);
}

TEST_MAIN()

#else

int main(void) {
	printf("Tests require C99 or later\n");
	return 0;
}

#endif
