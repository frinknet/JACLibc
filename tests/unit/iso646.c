/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_C99
#include <iso646.h>

TEST_TYPE(unit);
TEST_UNIT(iso646.h);

/* ============================================================================
 * LOGICAL OPERATORS
 * ============================================================================ */
TEST_SUITE(logical_operators);

TEST(and_operator) {
	int a = 1, b = 1;
	ASSERT_TRUE(a and b);
	ASSERT_FALSE(0 and 1);
	ASSERT_FALSE(1 and 0);
	ASSERT_FALSE(0 and 0);
}

TEST(or_operator) {
	ASSERT_TRUE(1 or 0);
	ASSERT_TRUE(0 or 1);
	ASSERT_TRUE(1 or 1);
	ASSERT_FALSE(0 or 0);
}

TEST(not_operator) {
	ASSERT_TRUE(not 0);
	ASSERT_FALSE(not 1);
	ASSERT_TRUE(not (5 > 10));
	ASSERT_FALSE(not (5 < 10));
}

TEST(not_eq_operator) {
	ASSERT_TRUE(5 not_eq 3);
	ASSERT_FALSE(5 not_eq 5);
	ASSERT_TRUE(100 not_eq 99);
}

/* ============================================================================
 * BITWISE OPERATORS
 * ============================================================================ */
TEST_SUITE(bitwise_operators);

TEST(bitand_operator) {
	ASSERT_EQ(0x0F, 0xFF bitand 0x0F);
	ASSERT_EQ(0, 0xFF bitand 0x00);
	ASSERT_EQ(0x12, 0x12 bitand 0xFF);
}

TEST(bitor_operator) {
	ASSERT_EQ(0xFF, 0xF0 bitor 0x0F);
	ASSERT_EQ(0xFF, 0xFF bitor 0x00);
	ASSERT_EQ(0x00, 0x00 bitor 0x00);
}

TEST(xor_operator) {
	ASSERT_EQ(0xFF, 0xF0 xor 0x0F);
	ASSERT_EQ(0xFF, 0xFF xor 0x00);
	ASSERT_EQ(0x00, 0xFF xor 0xFF);
}

TEST(compl_operator) {
	unsigned char val = 0xAA;
	ASSERT_EQ(0x55, compl val);
	ASSERT_EQ(0, compl (unsigned char)0xFF);
}

/* ============================================================================
 * COMPOUND ASSIGNMENT OPERATORS
 * ============================================================================ */
TEST_SUITE(compound_assignment);

TEST(and_eq_operator) {
	int val = 0xFF;
	val and_eq 0x0F;
	ASSERT_EQ(0x0F, val);
}

TEST(or_eq_operator) {
	int val = 0xF0;
	val or_eq 0x0F;
	ASSERT_EQ(0xFF, val);
}

TEST(xor_eq_operator) {
	int val = 0xFF;
	val xor_eq 0x0F;
	ASSERT_EQ(0xF0, val);
}

/* ============================================================================
 * COMPLEX EXPRESSIONS
 * ============================================================================ */
TEST_SUITE(complex_expressions);

TEST(combined_logical) {
	int a = 5, b = 10, c = 15;
	ASSERT_TRUE((a < b) and (b < c));
	ASSERT_TRUE((a > b) or (b < c));
	ASSERT_FALSE((a > b) and (b > c));
}

TEST(combined_bitwise) {
	int val = 0xAB;
	val = (val bitand 0xF0) bitor 0x05;
	ASSERT_EQ(0xA5, val);
}

TEST(not_with_comparison) {
	ASSERT_TRUE(not (10 < 5));
	ASSERT_FALSE(not (10 > 5));
}

TEST(bitwise_with_compl) {
	unsigned char val = 0xAA;
	val = val bitand compl 0x0F;
	ASSERT_EQ(0xA0, val);
}

/* ============================================================================
 * PRECEDENCE TESTS
 * ============================================================================ */
TEST_SUITE(operator_precedence);

TEST(and_or_precedence) {
	// and has higher precedence than or
	ASSERT_TRUE(0 or 1 and 1);
	ASSERT_TRUE(1 and 1 or 0);
}

TEST(bitwise_precedence) {
	int result = 0x0F bitor 0xF0 bitand 0xFF;
	// bitand has higher precedence
	ASSERT_EQ(0xFF, result);
}

TEST(not_precedence) {
	int a = 0;
	ASSERT_TRUE(not a or 1);
}

/* ============================================================================
 * PRACTICAL USAGE
 * ============================================================================ */
TEST_SUITE(practical_usage);

TEST(flag_checking) {
	int flags = 0x05;  // bits 0 and 2 set
	
	ASSERT_TRUE((flags bitand 0x01) not_eq 0);
	ASSERT_TRUE((flags bitand 0x04) not_eq 0);
	ASSERT_FALSE((flags bitand 0x02) not_eq 0);
}

TEST(flag_setting) {
	int flags = 0x00;
	
	flags or_eq 0x01;  // Set bit 0
	flags or_eq 0x04;  // Set bit 2
	
	ASSERT_EQ(0x05, flags);
}

TEST(flag_clearing) {
	int flags = 0xFF;
	
	flags and_eq compl 0x01;  // Clear bit 0
	flags and_eq compl 0x02;  // Clear bit 1
	
	ASSERT_EQ(0xFC, flags);
}

TEST(condition_validation) {
	int value = 50;
	
	if ((value > 0) and (value < 100)) {
		ASSERT_TRUE(1);
	} else {
		ASSERT_TRUE(0);
	}
}

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(zero_values) {
	ASSERT_FALSE(0 and 0);
	ASSERT_FALSE(0 or 0);
	ASSERT_EQ(0, 0 bitand 0);
	ASSERT_EQ(0, 0 bitor 0);
	ASSERT_EQ(0, 0 xor 0);
}

TEST(all_ones) {
	ASSERT_TRUE(1 and 1);
	ASSERT_TRUE(1 or 1);
	ASSERT_EQ(0xFF, 0xFF bitand 0xFF);
	ASSERT_EQ(0xFF, 0xFF bitor 0xFF);
	ASSERT_EQ(0, 0xFF xor 0xFF);
}

TEST(short_circuit_and) {
	int called = 0;
	#define side_effect() (called++, 1)
	
	// Short-circuit should prevent second evaluation
	if (0 and side_effect()) { }
	ASSERT_EQ(0, called);
	
	#undef side_effect
}

TEST(short_circuit_or) {
	int called = 0;
	#define side_effect() (called++, 1)
	
	// Short-circuit should prevent second evaluation
	if (1 or side_effect()) { }
	ASSERT_EQ(0, called);
	
	#undef side_effect
}

TEST_MAIN()

#else

int main(void) {
	printf("iso646.h requires C99 or later\n");
	return 0;
}

#endif
