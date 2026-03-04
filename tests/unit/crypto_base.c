/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <crypto/base.h>

TEST_TYPE(unit);
TEST_UNIT(crypto/base.h);

/* ============================================================================
 * ROTATE TESTS
 * ============================================================================ */
TEST_SUITE(rotations);

TEST(rotl32_basic) {
	ASSERT_EQ(2U, rotl32(1U, 1));
	ASSERT_EQ(4U, rotl32(1U, 2));
	ASSERT_EQ(8U, rotl32(1U, 3));
}

TEST(rotl32_wrap) {
	ASSERT_EQ(1U, rotl32(0x80000000U, 1));
	ASSERT_EQ(1U, rotl32(1U, 32));
	ASSERT_EQ(2U, rotl32(1U, 33));
}

TEST(rotr32_basic) {
	ASSERT_EQ(1U, rotr32(2U, 1));
	ASSERT_EQ(1U, rotr32(4U, 2));
	ASSERT_EQ(1U, rotr32(8U, 3));
}

TEST(rotr32_wrap) {
	ASSERT_EQ(0x80000000U, rotr32(1U, 1));
	ASSERT_EQ(1U, rotr32(1U, 32));
	ASSERT_EQ(0x80000000U, rotr32(1U, 33));
}

TEST(rotate_inverse) {
	unsigned int val = 0x12345678U;

	ASSERT_EQ(val, rotr32(rotl32(val, 5), 5));
	ASSERT_EQ(val, rotl32(rotr32(val, 7), 7));
}

TEST(rotate_pattern) {
	unsigned int val = 0xAAAAAAAAU;

	unsigned int rotated = rotl32(val, 1);
	ASSERT_EQ(0x55555555U, rotated);
}

TEST_MAIN()
