/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>

TEST_TYPE(unit);
TEST_UNIT(stdalign.h);

/* ============================================================================ */
TEST_SUITE(defined);

TEST(defined_alignas) {
	ASSERT_TRUE(__alignas_is_defined);
}

TEST(defined_alignof) {
	ASSERT_TRUE(__alignof_is_defined);
}

/* ============================================================================ */
TEST_SUITE(alignof);

TEST(alignof_basic_types) {
	ASSERT_EQ(1, alignof(char));
	ASSERT_GE(alignof(int), 2);
	ASSERT_GE(alignof(double), 4);
}

TEST(alignof_struct) {
	struct foo {
		char a;
		int b;
	};
	/* Struct alignment is usually aligned to its widest member */
	ASSERT_EQ(alignof(int), alignof(struct foo));
}

/* ============================================================================ */
TEST_SUITE(alignas);

TEST(alignas_variable_16) {
	alignas(16) char buf[16];
	ASSERT_TRUE(((uintptr_t)buf % 16) == 0);
}

TEST(alignas_variable_32) {
	alignas(32) char buf[32];
	ASSERT_TRUE(((uintptr_t)buf % 32) == 0);
}

TEST(alignas_struct_member) {
	struct aligned_struct {
		char a;
		alignas(8) int b;
	};
	/* The offset of b should be aligned to 8 */
	ASSERT_EQ(0, offsetof(struct aligned_struct, b) % 8);
}

/* ============================================================================ */
TEST_MAIN()
