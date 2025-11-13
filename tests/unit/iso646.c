/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

TEST_TYPE(unit)
TEST_UNIT(iso646.h)

#if JACL_HAS_C99
#include <iso646.h>

/* Helper for short-circuit tests */
static int called = 0;
static int side_effect(void) { called++; return 1; }

/* ============================================================================
 * and TESTS
 * ============================================================================ */
TEST_SUITE(and)

TEST(and_both_true) {
	ASSERT_TRUE(1 and 1);
	ASSERT_TRUE(5 and 10);
}

TEST(and_one_false) {
	ASSERT_FALSE(0 and 1);
	ASSERT_FALSE(1 and 0);
}

TEST(and_both_false) {
	ASSERT_FALSE(0 and 0);
}

TEST(and_expressions) {
	int a = 5, b = 10;
	ASSERT_TRUE((a < b) and (b > 0));
	ASSERT_FALSE((a > b) and (b > 0));
}

TEST(and_short_circuit) {
	called = 0;
	if (0 and side_effect()) { }
	ASSERT_EQ(0, called);
}

/* ============================================================================
 * or TESTS
 * ============================================================================ */
TEST_SUITE(or)

TEST(or_both_true) {
	ASSERT_TRUE(1 or 1);
	ASSERT_TRUE(5 or 10);
}

TEST(or_one_true) {
	ASSERT_TRUE(1 or 0);
	ASSERT_TRUE(0 or 1);
}

TEST(or_both_false) {
	ASSERT_FALSE(0 or 0);
}

TEST(or_expressions) {
	int a = 5, b = 10;
	ASSERT_TRUE((a > b) or (b > 0));
	ASSERT_TRUE((a < b) or (b < 0));
}

TEST(or_short_circuit) {
	called = 0;
	if (1 or side_effect()) { }
	ASSERT_EQ(0, called);
}

/* ============================================================================
 * not TESTS
 * ============================================================================ */
TEST_SUITE(not)

TEST(not_true) {
	ASSERT_FALSE(not 1);
	ASSERT_FALSE(not 5);
	ASSERT_FALSE(not -1);
}

TEST(not_false) {
	ASSERT_TRUE(not 0);
}

TEST(not_expressions) {
	ASSERT_TRUE(not (5 > 10));
	ASSERT_FALSE(not (5 < 10));
	ASSERT_TRUE(not (10 == 5));
}

TEST(not_double_negation) {
	ASSERT_TRUE(not not 1);      // !!1 = 1 (true)
	ASSERT_FALSE(not not 0);     // !!0 = 0 (false)
	ASSERT_FALSE(not not not 1); // !!!1 = !1 = 0 (false)
}

/* ============================================================================
 * not_eq TESTS
 * ============================================================================ */
TEST_SUITE(not_eq)

TEST(not_eq_different) {
	ASSERT_TRUE(5 not_eq 3);
	ASSERT_TRUE(100 not_eq 99);
	ASSERT_TRUE(-1 not_eq 1);
}

TEST(not_eq_equal) {
	ASSERT_FALSE(5 not_eq 5);
	ASSERT_FALSE(0 not_eq 0);
	ASSERT_FALSE(-1 not_eq -1);
}

/* ============================================================================
 * bitand TESTS
 * ============================================================================ */
TEST_SUITE(bitand)

TEST(bitand_basic) {
	ASSERT_EQ(0x0F, 0xFF bitand 0x0F);
	ASSERT_EQ(0xF0, 0xFF bitand 0xF0);
	ASSERT_EQ(0x00, 0xFF bitand 0x00);
}

TEST(bitand_identity) {
	ASSERT_EQ(0xFF, 0xFF bitand 0xFF);
	ASSERT_EQ(0x12, 0x12 bitand 0xFF);
}

TEST(bitand_zero) {
	ASSERT_EQ(0, 0x00 bitand 0xFF);
	ASSERT_EQ(0, 0x00 bitand 0x00);
}

TEST(bitand_masks) {
	int val = 0xABCD;
	ASSERT_EQ(0x00CD, val bitand 0x00FF);
	ASSERT_EQ(0xAB00, val bitand 0xFF00);
}

/* ============================================================================
 * bitor TESTS
 * ============================================================================ */
TEST_SUITE(bitor)

TEST(bitor_basic) {
	ASSERT_EQ(0xFF, 0xF0 bitor 0x0F);
	ASSERT_EQ(0xFF, 0xFF bitor 0x00);
	ASSERT_EQ(0xF0, 0xF0 bitor 0x00);
}

TEST(bitor_identity) {
	ASSERT_EQ(0xFF, 0xFF bitor 0xFF);
	ASSERT_EQ(0, 0x00 bitor 0x00);
}

TEST(bitor_combining) {
	int val = 0x00;
	val = val bitor 0x0F;
	val = val bitor 0xF0;
	ASSERT_EQ(0xFF, val);
}

/* ============================================================================
 * xor TESTS
 * ============================================================================ */
TEST_SUITE(xor)

TEST(xor_basic) {
	ASSERT_EQ(0xFF, 0xF0 xor 0x0F);
	ASSERT_EQ(0xFF, 0xFF xor 0x00);
	ASSERT_EQ(0x00, 0xFF xor 0xFF);
}

TEST(xor_toggle) {
	int val = 0xAA;
	val = val xor 0xFF;
	ASSERT_EQ(0x55, val);
	val = val xor 0xFF;
	ASSERT_EQ(0xAA, val);
}

TEST(xor_zero) {
	ASSERT_EQ(0xAB, 0xAB xor 0x00);
	ASSERT_EQ(0x00, 0x00 xor 0x00);
}

/* ============================================================================
 * compl TESTS
 * ============================================================================ */
TEST_SUITE(compl)

TEST(compl_basic) {
	unsigned char val = 0xAA;
	ASSERT_EQ(0x55, (unsigned char)(compl val));
}

TEST(compl_all_bits) {
	ASSERT_EQ(0x00, (unsigned char)(compl (unsigned char)0xFF));
	ASSERT_EQ(0xFF, (unsigned char)(compl (unsigned char)0x00));
}

TEST(compl_double) {
	unsigned char val = 0xAB;
	ASSERT_EQ(val, (unsigned char)(compl (unsigned char)(compl val)));
}

TEST(compl_patterns) {
	ASSERT_EQ(0x00, (unsigned char)(compl (unsigned char)0xFF));
	ASSERT_EQ(0x0F, (unsigned char)(compl (unsigned char)0xF0));
	ASSERT_EQ(0xF0, (unsigned char)(compl (unsigned char)0x0F));
}

/* ============================================================================
 * and_eq TESTS
 * ============================================================================ */
TEST_SUITE(and_eq)

TEST(and_eq_basic) {
	int val = 0xFF;
	val and_eq 0x0F;
	ASSERT_EQ(0x0F, val);
}

TEST(and_eq_mask) {
	int val = 0xABCD;
	val and_eq 0x00FF;
	ASSERT_EQ(0x00CD, val);
}

TEST(and_eq_zero) {
	int val = 0xFF;
	val and_eq 0x00;
	ASSERT_EQ(0x00, val);
}

/* ============================================================================
 * or_eq TESTS
 * ============================================================================ */
TEST_SUITE(or_eq)

TEST(or_eq_basic) {
	int val = 0xF0;
	val or_eq 0x0F;
	ASSERT_EQ(0xFF, val);
}

TEST(or_eq_accumulate) {
	int val = 0x00;
	val or_eq 0x01;
	val or_eq 0x04;
	val or_eq 0x10;
	ASSERT_EQ(0x15, val);
}

TEST(or_eq_identity) {
	int val = 0xAB;
	val or_eq 0x00;
	ASSERT_EQ(0xAB, val);
}

/* ============================================================================
 * xor_eq TESTS
 * ============================================================================ */
TEST_SUITE(xor_eq)

TEST(xor_eq_basic) {
	int val = 0xFF;
	val xor_eq 0x0F;
	ASSERT_EQ(0xF0, val);
}

TEST(xor_eq_toggle) {
	int val = 0xAA;
	val xor_eq 0xFF;
	ASSERT_EQ(0x55, val);
	val xor_eq 0xFF;
	ASSERT_EQ(0xAA, val);
}

TEST(xor_eq_cancel) {
	int val = 0xAB;
	val xor_eq 0xAB;
	ASSERT_EQ(0x00, val);
}

/* ============================================================================
 * COMBINED OPERATIONS TESTS
 * ============================================================================ */
TEST_SUITE(combined)

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

TEST(combined_with_compl) {
	unsigned char val = 0xAA;
	val = val bitand compl 0x0F;
	ASSERT_EQ(0xA0, val);
}

TEST(combined_assignments) {
	int val = 0xFF;
	val and_eq 0xF0;
	val or_eq 0x0A;
	val xor_eq 0x05;
	ASSERT_EQ(0xFF, val);
}

/* ============================================================================
 * PRACTICAL USE CASES TESTS
 * ============================================================================ */
TEST_SUITE(practical)

TEST(flag_checking) {
	int flags = 0x05;

	ASSERT_TRUE((flags bitand 0x01) not_eq 0);
	ASSERT_TRUE((flags bitand 0x04) not_eq 0);
	ASSERT_FALSE((flags bitand 0x02) not_eq 0);
}

TEST(flag_setting) {
	int flags = 0x00;

	flags or_eq 0x01;
	flags or_eq 0x04;

	ASSERT_EQ(0x05, flags);
}

TEST(flag_clearing) {
	int flags = 0xFF;

	flags and_eq compl 0x01;
	flags and_eq compl 0x02;

	ASSERT_EQ(0xFC, flags);
}

TEST(flag_toggling) {
	int flags = 0xAA;

	flags xor_eq 0x0F;
	ASSERT_EQ(0xA5, flags);

	flags xor_eq 0x0F;
	ASSERT_EQ(0xAA, flags);
}

TEST(condition_validation) {
	int value = 50;

	ASSERT_TRUE((value > 0) and (value < 100));
	ASSERT_FALSE((value < 0) or (value > 100));
}

TEST(range_checking) {
	int x = 25;
	int min = 0, max = 100;

	ASSERT_TRUE(not (x < min or x > max));
}

#else

TEST_SUITE(iso646_unavailable)

TEST(iso646_not_available) {
	TEST_SKIP("NO C99 SUPPORT");
}

#endif

TEST_MAIN()

