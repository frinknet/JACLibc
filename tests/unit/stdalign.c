/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

TEST_TYPE(unit);
TEST_UNIT(stdalign.h);

#if JACL_HAS_C11
#include <stdalign.h>

/* ============================================================================
 * MACRO DEFINITIONS
 * ============================================================================ */
TEST_SUITE(macros);

TEST(macros_alignas_defined) {
	ASSERT_EQ(1, __alignas_is_defined);
}

TEST(macros_alignof_defined) {
	ASSERT_EQ(1, __alignof_is_defined);
}

/* ============================================================================
 * ALIGNOF BASIC TYPES
 * ============================================================================ */
TEST_SUITE(alignof_basic);

TEST(alignof_basic_char) {
	ASSERT_EQ(1, alignof(char));
}

TEST(alignof_basic_short) {
	ASSERT_TRUE(alignof(short) >= 2);
}

TEST(alignof_basic_int) {
	ASSERT_TRUE(alignof(int) >= 4);
}

TEST(alignof_basic_long) {
	ASSERT_TRUE(alignof(long) >= 4);
}

TEST(alignof_basic_long_long) {
	ASSERT_TRUE(alignof(long long) >= 8);
}

TEST(alignof_basic_float) {
	ASSERT_TRUE(alignof(float) >= 4);
}

TEST(alignof_basic_double) {
	ASSERT_TRUE(alignof(double) >= 8);
}

TEST(alignof_basic_pointer) {
	ASSERT_TRUE(alignof(void*) >= 4);
}

TEST(alignof_basic_max_align_t) {
	ASSERT_TRUE(alignof(max_align_t) >= 8);
}

/* ============================================================================
 * ALIGNAS VARIABLES
 * ============================================================================ */
TEST_SUITE(alignas_variables);

TEST(alignas_variables_stack_char) {
	alignas(64) char buffer[128];
	ASSERT_EQ(0, ((uintptr_t)buffer) % 64);
}

TEST(alignas_variables_int) {
	alignas(32) int value = 42;
	ASSERT_EQ(42, value);
	ASSERT_EQ(0, ((uintptr_t)&value) % 32);
}

TEST(alignas_variables_max_align) {
	alignas(max_align_t) char buffer[64];
	size_t align = alignof(max_align_t);
	ASSERT_EQ(0, ((uintptr_t)buffer) % align);
}

TEST(alignas_variables_cache_line) {
	alignas(64) int cache_line[16];
	ASSERT_EQ(0, ((uintptr_t)cache_line) % 64);
}

TEST(alignas_variables_simd) {
	alignas(16) float simd_data[4];
	ASSERT_EQ(0, ((uintptr_t)simd_data) % 16);
}

/* ============================================================================
 * ALIGNAS STRUCT MEMBERS
 * ============================================================================ */
TEST_SUITE(alignas_members);

TEST(alignas_members_single) {
	struct s {
		alignas(8) char c;
	};
	struct s obj;
	ASSERT_EQ(0, ((uintptr_t)&obj.c) % 8);
}

TEST(alignas_members_multiple) {
	struct s {
		alignas(16) int a;
		alignas(8) char b;
		int c;
	};
	ASSERT_TRUE(alignof(struct s) >= 16);
}

TEST(alignas_members_nested) {
	struct inner {
		alignas(16) int value;
	};
	struct outer {
		struct inner in;
	};
	ASSERT_TRUE(alignof(struct outer) >= 16);
}

TEST(alignas_members_array) {
	struct s {
		alignas(32) char data[64];
	};
	struct s obj;
	ASSERT_EQ(0, ((uintptr_t)obj.data) % 32);
}

/* ============================================================================
 * ALIGNOF STRUCTS
 * ============================================================================ */
TEST_SUITE(alignof_structs);

TEST(alignof_structs_packed) {
	struct s {
		char a;
		char b;
		char c;
	};
	ASSERT_TRUE(alignof(struct s) >= 1);
}

TEST(alignof_structs_mixed) {
	struct s {
		char c;
		int i;
		double d;
	};
	ASSERT_TRUE(alignof(struct s) >= alignof(double));
}

TEST(alignof_structs_with_alignas) {
	struct s {
		alignas(32) char c;
	};
	ASSERT_TRUE(alignof(struct s) >= 32);
}

TEST(alignof_structs_largest_member) {
	struct s {
		char c;
		int i;
		long long ll;
	};
	ASSERT_TRUE(alignof(struct s) >= alignof(long long));
}

/* ============================================================================
 * ALIGNMENT PROPERTIES
 * ============================================================================ */
TEST_SUITE(properties);

TEST(properties_power_of_two) {
	size_t align = alignof(double);
	ASSERT_EQ(0, align & (align - 1));
}

TEST(properties_monotonic) {
	ASSERT_TRUE(alignof(int) >= alignof(char));
	ASSERT_TRUE(alignof(double) >= alignof(char));
	ASSERT_TRUE(alignof(long long) >= alignof(int));
}

TEST(properties_alignas_increases) {
	struct normal { char c; };
	struct aligned { alignas(16) char c; };
	ASSERT_TRUE(alignof(struct aligned) > alignof(struct normal));
}

TEST(properties_sizeof_multiple) {
	struct s {
		char c;
		alignas(8) int i;
	};
	size_t sz = sizeof(struct s);
	size_t al = alignof(struct s);
	ASSERT_EQ(0, sz % al);
}

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(edge_cases_alignas_one) {
	alignas(1) char c = 'A';
	ASSERT_EQ('A', c);
}

TEST(edge_cases_alignas_large) {
	alignas(256) char buffer[512];
	ASSERT_EQ(0, ((uintptr_t)buffer) % 256);
}

TEST(edge_cases_max_in_struct) {
	struct s {
		alignas(8) char a;
		alignas(16) char b;
	};
	ASSERT_TRUE(alignof(struct s) >= 16);
}

TEST(edge_cases_array_alignment) {
	alignas(128) int array[10];
	ASSERT_EQ(0, ((uintptr_t)array) % 128);
}

#else

TEST_SUITE(stdalign_unavailable);

TEST(stdalign_requires_c11) {
	TEST_SKIP("stdalign.h requires C11");
}

#endif

TEST_MAIN()
