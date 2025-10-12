/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_C11
#include <stdalign.h>
#include <stddef.h>

TEST_TYPE(unit);
TEST_UNIT(stdalign.h);

/* ============================================================================
 * MACRO DEFINITIONS
 * ============================================================================ */
TEST_SUITE(macro_definitions);

TEST(alignas_is_defined) {
	ASSERT_EQ(1, __alignas_is_defined);
}

TEST(alignof_is_defined) {
	ASSERT_EQ(1, __alignof_is_defined);
}

/* ============================================================================
 * ALIGNOF - BASIC TYPES
 * ============================================================================ */
TEST_SUITE(alignof_basic_types);

TEST(alignof_char) {
	size_t align = alignof(char);
	ASSERT_EQ(1, align);
}

TEST(alignof_short) {
	size_t align = alignof(short);
	ASSERT_TRUE(align >= 2);
}

TEST(alignof_int) {
	size_t align = alignof(int);
	ASSERT_TRUE(align >= 4);
}

TEST(alignof_long) {
	size_t align = alignof(long);
	ASSERT_TRUE(align >= 4);
}

TEST(alignof_long_long) {
	size_t align = alignof(long long);
	ASSERT_TRUE(align >= 8);
}

TEST(alignof_float) {
	size_t align = alignof(float);
	ASSERT_TRUE(align >= 4);
}

TEST(alignof_double) {
	size_t align = alignof(double);
	ASSERT_TRUE(align >= 8);
}

TEST(alignof_pointer) {
	size_t align = alignof(void*);
	ASSERT_TRUE(align >= 4);
}

/* ============================================================================
 * ALIGNAS - STRUCT ALIGNMENT
 * ============================================================================ */
TEST_SUITE(alignas_struct);

TEST(alignas_basic) {
	struct alignas(16) aligned_struct {
		char c;
	};
	
	ASSERT_EQ(16, alignof(struct aligned_struct));
}

TEST(alignas_member) {
	struct test {
		alignas(8) char c;
	};
	
	// Member should be aligned
	struct test t;
	ASSERT_EQ(0, ((uintptr_t)&t.c) % 8);
}

TEST(alignas_multiple_members) {
	struct test {
		alignas(16) int a;
		alignas(8) char b;
		int c;
	};
	
	ASSERT_TRUE(alignof(struct test) >= 16);
}

/* ============================================================================
 * ALIGNAS - VARIABLE ALIGNMENT
 * ============================================================================ */
TEST_SUITE(alignas_variables);

TEST(alignas_stack_variable) {
	alignas(64) char buffer[128];
	
	ASSERT_EQ(0, ((uintptr_t)buffer) % 64);
}

TEST(alignas_int_alignment) {
	alignas(32) int value = 42;
	
	ASSERT_EQ(42, value);
	ASSERT_EQ(0, ((uintptr_t)&value) % 32);
}

TEST(alignas_max_align) {
	alignas(max_align_t) char buffer[64];
	
	size_t align = alignof(max_align_t);
	ASSERT_EQ(0, ((uintptr_t)buffer) % align);
}

/* ============================================================================
 * ALIGNOF - STRUCT TYPES
 * ============================================================================ */
TEST_SUITE(alignof_structs);

TEST(alignof_packed_struct) {
	struct packed {
		char a;
		char b;
		char c;
	};
	
	size_t align = alignof(struct packed);
	ASSERT_TRUE(align >= 1);
}

TEST(alignof_mixed_struct) {
	struct mixed {
		char c;
		int i;
		double d;
	};
	
	size_t align = alignof(struct mixed);
	ASSERT_TRUE(align >= alignof(double));
}

TEST(alignof_nested_struct) {
	struct inner {
		alignas(16) int value;
	};
	
	struct outer {
		struct inner in;
	};
	
	ASSERT_TRUE(alignof(struct outer) >= 16);
}

/* ============================================================================
 * ALIGNMENT PROPERTIES
 * ============================================================================ */
TEST_SUITE(alignment_properties);

TEST(alignment_power_of_two) {
	size_t align = alignof(double);
	
	// Alignment should be power of 2
	ASSERT_EQ(0, align & (align - 1));
}

TEST(larger_type_larger_alignment) {
	size_t char_align = alignof(char);
	size_t int_align = alignof(int);
	size_t double_align = alignof(double);
	
	ASSERT_TRUE(int_align >= char_align);
	ASSERT_TRUE(double_align >= char_align);
}

TEST(alignas_increases_alignment) {
	struct normal {
		char c;
	};
	
	struct aligned {
		alignas(16) char c;
	};
	
	ASSERT_TRUE(alignof(struct aligned) > alignof(struct normal));
}

/* ============================================================================
 * PRACTICAL USAGE
 * ============================================================================ */
TEST_SUITE(practical_usage);

TEST(cache_line_alignment) {
	alignas(64) struct cache_aligned {
		int data[16];
	} obj;
	
	ASSERT_EQ(0, ((uintptr_t)&obj) % 64);
}

TEST(simd_alignment) {
	alignas(16) float simd_data[4];
	
	ASSERT_EQ(0, ((uintptr_t)simd_data) % 16);
}

TEST(alignment_array) {
	alignas(32) int array[10];
	
	ASSERT_EQ(0, ((uintptr_t)array) % 32);
}

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(alignas_one) {
	alignas(1) char c = 'A';
	
	ASSERT_EQ('A', c);
}

TEST(alignas_large) {
	alignas(256) char buffer[512];
	
	ASSERT_EQ(0, ((uintptr_t)buffer) % 256);
}

TEST(multiple_alignas_same_struct) {
	struct test {
		alignas(8) char a;
		alignas(16) char b;
	};
	
	// Struct alignment should be max of member alignments
	ASSERT_TRUE(alignof(struct test) >= 16);
}

/* ============================================================================
 * SIZEOF VS ALIGNOF
 * ============================================================================ */
TEST_SUITE(sizeof_vs_alignof);

TEST(sizeof_includes_padding) {
	struct padded {
		char c;
		alignas(8) int i;
	};
	
	size_t size = sizeof(struct padded);
	size_t align = alignof(struct padded);
	
	// Size should be multiple of alignment
	ASSERT_EQ(0, size % align);
}

TEST(alignof_not_size) {
	struct test {
		char c;
	};
	
	ASSERT_NE(sizeof(struct test), alignof(struct test));
}

TEST_MAIN()

#else

int main(void) {
	printf("stdalign.h requires C11 or later\n");
	return 0;
}

#endif
