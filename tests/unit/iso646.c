/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <iso646.h>
#include <stdbool.h>

TEST_TYPE(unit);
TEST_UNIT(iso646.h);

/* ============================================================================ */
TEST_SUITE(logical);

TEST(logical_and) {
	ASSERT_FALSE(1 and 0);
}

TEST(logical_or) {
	ASSERT_TRUE(1 or 0);
}

TEST(logical_not) {
	ASSERT_TRUE(not 0);
	ASSERT_FALSE(not 1);
}

/* ============================================================================ */
TEST_SUITE(bitwise);

TEST(bitwise_and) {
	int a = 0b1100;
	int b = 0b1010;
	ASSERT_EQ(0b1000, a bitand b);
}

TEST(bitwise_or) {
	int a = 0b1100;
	int b = 0b1010;
	ASSERT_EQ(0b1110, a bitor b);
}

TEST(bitwise_xor) {
	int a = 0b1100;
	int b = 0b1010;
	ASSERT_EQ(0b0110, a xor b);
}

TEST(bitwise_compl) {
	int a = 0;
	ASSERT_EQ(-1, compl a);
}

/* ============================================================================ */
TEST_SUITE(assign);

TEST(assign_and_eq) {
	int a = 0b1100;
	int b = 0b1010;
	a and_eq b;
	ASSERT_EQ(0b1000, a);
}

TEST(assign_or_eq) {
	int a = 0b1100;
	int b = 0b1010;
	a or_eq b;
	ASSERT_EQ(0b1110, a);
}

TEST(assign_xor_eq) {
	int a = 0b1100;
	int b = 0b1010;
	a xor_eq b;
	ASSERT_EQ(0b0110, a);
}

TEST(assign_not_eq) {
	int a = 5;
	int b = 5;
	int c = 6;
	ASSERT_FALSE(a not_eq b);
	ASSERT_TRUE(a not_eq c);
}

/* ============================================================================ */
TEST_MAIN()
