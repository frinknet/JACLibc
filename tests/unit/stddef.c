/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <stddef.h>

TEST_TYPE(unit);
TEST_UNIT(stddef.h);

/* ============================================================================
 * Helper functions - must be at file scope
 * ============================================================================ */
static __inline int helper_inline_test(void) { return 42; }

static int helper_restrict_test(int * __restrict ptr) {
	return ptr[0];
}

/* ============================================================================
 * NULL
 * ============================================================================ */
TEST_SUITE(null);

TEST(null_pointer_constant) {
	void *ptr = NULL;
	ASSERT_NULL(ptr);
}

TEST(null_value_is_zero) {
	ASSERT_EQ(0, (intptr_t)NULL);
}

TEST(null_equality) {
	void *p1 = NULL;
	void *p2 = NULL;
	ASSERT_TRUE(p1 == p2);
	ASSERT_TRUE(p1 == NULL);
}

TEST(null_pointer_checks) {
	int *p1 = NULL;
	int val = 5;
	int *p2 = &val;

	ASSERT_TRUE(p1 == NULL);
	ASSERT_TRUE(p2 != NULL);
}

/* ============================================================================
 * SIZE_T
 * ============================================================================ */
TEST_SUITE(size_t);

TEST(size_t_basic) {
	size_t sz = 42;
	ASSERT_EQ(42, sz);
	ASSERT_TRUE(sizeof(size_t) > 0);
}

TEST(size_t_is_unsigned) {
	size_t sz = (size_t)-1;
	ASSERT_TRUE(sz > 0);
}

TEST(size_t_max_value) {
	ASSERT_TRUE(SIZE_MAX > 0);
	ASSERT_EQ((size_t)-1, SIZE_MAX);
}

TEST(size_t_architecture_dependent) {
	#if JACL_64BIT
		ASSERT_EQ(8, sizeof(size_t));
	#else
		ASSERT_EQ(4, sizeof(size_t));
	#endif
}

TEST(size_t_sizeof_operator) {
	int arr[10];
	size_t sz = sizeof(arr);
	ASSERT_EQ(40, sz);
}

TEST(size_t_array_indexing) {
	int arr[10];
	size_t idx = 0;
	for (idx = 0; idx < 10; idx++) {
		arr[idx] = (int)idx;
	}
	ASSERT_EQ(9, arr[9]);
}

/* ============================================================================
 * PTRDIFF_T
 * ============================================================================ */
TEST_SUITE(ptrdiff_t);

TEST(ptrdiff_t_basic_arithmetic) {
	ptrdiff_t diff = 10 - 5;
	ASSERT_EQ(5, diff);
}

TEST(ptrdiff_t_is_signed) {
	ptrdiff_t pos = 10;
	ptrdiff_t neg = -10;
	ASSERT_TRUE(pos > 0);
	ASSERT_TRUE(neg < 0);
}

TEST(ptrdiff_t_pointer_subtraction) {
	int arr[10];
	ptrdiff_t diff = &arr[7] - &arr[2];
	ASSERT_EQ(5, diff);
}

TEST(ptrdiff_t_architecture_dependent) {
	#if JACL_64BIT
		ASSERT_EQ(8, sizeof(ptrdiff_t));
	#else
		ASSERT_EQ(4, sizeof(ptrdiff_t));
	#endif
}

TEST(ptrdiff_t_negative_difference) {
	int arr[10];
	ptrdiff_t diff = &arr[2] - &arr[7];
	ASSERT_EQ(-5, diff);
}

TEST(ptrdiff_t_pointer_distance) {
	int buf[100];
	int *start = &buf[10];
	int *end = &buf[90];
	ptrdiff_t dist = end - start;
	ASSERT_EQ(80, dist);
}

/* ============================================================================
 * WCHAR_T
 * ============================================================================ */
TEST_SUITE(wchar_t);

TEST(wchar_t_basic) {
	wchar_t wc = L'X';
	ASSERT_TRUE(sizeof(wchar_t) >= 1);
}

TEST(wchar_t_wide_literal) {
	wchar_t wc = L'A';
	ASSERT_TRUE(wc == L'A');
}

/* ============================================================================
 * OFFSETOF MACRO
 * ============================================================================ */
TEST_SUITE(offsetof);

TEST(offsetof_first_member_is_zero) {
	struct s { int a; int b; };
	ASSERT_EQ(0, offsetof(struct s, a));
}

TEST(offsetof_second_member_nonzero) {
	struct s { char a; int b; };
	ASSERT_TRUE(offsetof(struct s, b) > 0);
}

TEST(offsetof_respects_member_order) {
	struct s { char a; int b; char c; long d; };
	size_t off_a = offsetof(struct s, a);
	size_t off_b = offsetof(struct s, b);
	size_t off_c = offsetof(struct s, c);
	size_t off_d = offsetof(struct s, d);

	ASSERT_EQ(0, off_a);
	ASSERT_TRUE(off_b > off_a);
	ASSERT_TRUE(off_c > off_b);
	ASSERT_TRUE(off_d > off_c);
}

TEST(offsetof_array_member) {
	struct s { int x; char buf[16]; };
	size_t off = offsetof(struct s, buf);
	ASSERT_TRUE(off >= sizeof(int));
}

TEST(offsetof_nested_struct) {
	struct inner { int x; };
	struct outer { char a; struct inner b; };

	size_t off = offsetof(struct outer, b);
	ASSERT_TRUE(off > 0);
}

TEST(offsetof_packed_struct) {
	struct s { char a; char b; char c; };
	ASSERT_EQ(0, offsetof(struct s, a));
	ASSERT_EQ(1, offsetof(struct s, b));
	ASSERT_EQ(2, offsetof(struct s, c));
}

TEST(offsetof_struct_layout_analysis) {
	struct packet {
		char header[4];
		int length;
		char data[64];
	};

	ASSERT_EQ(0, offsetof(struct packet, header));
	ASSERT_TRUE(offsetof(struct packet, length) >= 4);
	ASSERT_TRUE(offsetof(struct packet, data) > offsetof(struct packet, length));
	ASSERT_TRUE(sizeof(struct packet) > offsetof(struct packet, data));
}

/* ============================================================================
 * MAX_ALIGN_T (C11)
 * ============================================================================ */
#if JACL_HAS_C11
TEST_SUITE(max_align_t);

TEST(max_align_t_exists) {
	max_align_t m;
	(void)m;
	ASSERT_TRUE(sizeof(max_align_t) > 0);
}

TEST(max_align_t_largest_alignment) {
	ASSERT_TRUE(sizeof(max_align_t) >= sizeof(long long));
	ASSERT_TRUE(sizeof(max_align_t) >= sizeof(long double));
	ASSERT_TRUE(sizeof(max_align_t) >= sizeof(void*));
}
#endif

/* ============================================================================
 * LANGUAGE COMPATIBILITY MACROS
 * ============================================================================ */
TEST_SUITE(compat_macros);

TEST(restrict_keyword_macro) {
	int arr[5] = {10, 20, 30, 40, 50};
	ASSERT_EQ(10, helper_restrict_test(arr));
}

TEST(inline_keyword_macro) {
	ASSERT_EQ(42, helper_inline_test());
}

TEST_MAIN()

