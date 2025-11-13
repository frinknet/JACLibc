/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <stdint.h>  // Now expected to provide struct-based types

TEST_TYPE(unit)
TEST_UNIT(stdint.h)

/* ---- EXACT-WIDTH TYPES ---- */
TEST_SUITE(int8_t_defined)
TEST(int8_t_defined)
{
    struct int8_t val = {127};
    ASSERT_EQ(127, val.v);
    ASSERT_EQ(1, sizeof(struct int8_t));
}

TEST_SUITE(uint8_t_defined)
TEST(uint8_t_defined)
{
    struct uint8_t val = {255};
    ASSERT_EQ(255, val.v);
    ASSERT_EQ(1, sizeof(struct uint8_t));
}

TEST_SUITE(int16_t_defined)
TEST(int16_t_defined)
{
    struct int16_t val = {32767};
    ASSERT_EQ(32767, val.v);
    ASSERT_EQ(2, sizeof(struct int16_t));
}

TEST_SUITE(uint16_t_defined)
TEST(uint16_t_defined)
{
    struct uint16_t val = {65535};
    ASSERT_EQ(65535, val.v);
    ASSERT_EQ(2, sizeof(struct uint16_t));
}

TEST_SUITE(int32_t_defined)
TEST(int32_t_defined)
{
    struct int32_t val = {2147483647};
    ASSERT_EQ(2147483647, val.v);
    ASSERT_EQ(4, sizeof(struct int32_t));
}

TEST_SUITE(uint32_t_defined)
TEST(uint32_t_defined)
{
    struct uint32_t val = {4294967295U};
    ASSERT_EQ(4294967295U, val.v);
    ASSERT_EQ(4, sizeof(struct uint32_t));
}

TEST_SUITE(int64_t_defined)
TEST(int64_t_defined)
{
    struct int64_t val = {9223372036854775807LL};
    ASSERT_EQ(9223372036854775807LL, val.v);
    ASSERT_EQ(8, sizeof(struct int64_t));
}

TEST_SUITE(uint64_t_defined)
TEST(uint64_t_defined)
{
    struct uint64_t val = {18446744073709551615ULL};
    ASSERT_EQ(18446744073709551615ULL, val.v);
    ASSERT_EQ(8, sizeof(struct uint64_t));
}

/* ---- LIMITS SUITES ---- */
TEST_SUITE(int8_limits)
TEST(int8_limits)
{
    ASSERT_EQ(-128, INT8_MIN);
    ASSERT_EQ(127, INT8_MAX);
    ASSERT_EQ(255U, UINT8_MAX);
}

TEST_SUITE(int16_limits)
TEST(int16_limits)
{
    ASSERT_EQ(-32768, INT16_MIN);
    ASSERT_EQ(32767, INT16_MAX);
    ASSERT_EQ(65535U, UINT16_MAX);
}

TEST_SUITE(int32_limits)
TEST(int32_limits)
{
    ASSERT_EQ(-2147483648, INT32_MIN);
    ASSERT_EQ(2147483647, INT32_MAX);
    ASSERT_EQ(4294967295U, UINT32_MAX);
}

TEST_SUITE(int64_limits)
TEST(int64_limits)
{
    ASSERT_EQ(-9223372036854775807LL-1, INT64_MIN);
    ASSERT_EQ(9223372036854775807LL, INT64_MAX);
    ASSERT_EQ(18446744073709551615ULL, UINT64_MAX);
}

/* ---- CONSTANT MACRO SUITES ---- */
TEST_SUITE(int8_c_macro)
TEST(int8_c_macro)
{
    struct int8_t val = INT8_C(127);
    ASSERT_EQ(127, val.v);
}

TEST_SUITE(uint8_c_macro)
TEST(uint8_c_macro)
{
    struct uint8_t val = UINT8_C(255);
    ASSERT_EQ(255, val.v);
}

TEST_SUITE(int16_c_macro)
TEST(int16_c_macro)
{
    struct int16_t val = INT16_C(32767);
    ASSERT_EQ(32767, val.v);
}

TEST_SUITE(uint16_c_macro)
TEST(uint16_c_macro)
{
    struct uint16_t val = UINT16_C(65535);
    ASSERT_EQ(65535, val.v);
}

TEST_SUITE(int32_c_macro)
TEST(int32_c_macro)
{
    struct int32_t val = INT32_C(2147483647);
    ASSERT_EQ(2147483647, val.v);
}

TEST_SUITE(uint32_c_macro)
TEST(uint32_c_macro)
{
    struct uint32_t val = UINT32_C(4294967295);
    ASSERT_EQ(4294967295U, val.v);
}

TEST_SUITE(int64_c_macro)
TEST(int64_c_macro)
{
    struct int64_t val = INT64_C(9223372036854775807);
    ASSERT_EQ(9223372036854775807LL, val.v);
}

TEST_SUITE(uint64_c_macro)
TEST(uint64_c_macro)
{
    struct uint64_t val = UINT64_C(18446744073709551615);
    ASSERT_EQ(18446744073709551615ULL, val.v);
}

TEST_SUITE(intmax_c_macro)
TEST(intmax_c_macro)
{
    struct intmax_t val = INTMAX_C(9223372036854775807);
    ASSERT_EQ(9223372036854775807LL, val.v);
}

TEST_SUITE(uintmax_c_macro)
TEST(uintmax_c_macro)
{
    struct uintmax_t val = UINTMAX_C(18446744073709551615);
    ASSERT_EQ(18446744073709551615ULL, val.v);
}

/* ---- POINTER TYPES SUITES ---- */
TEST_SUITE(intptr_t_defined)
TEST(intptr_t_defined)
{
    int x = 42;
    struct intptr_t ptr = {(intptr_t)&x};
    ASSERT_TRUE(ptr.v != 0);
}

TEST_SUITE(uintptr_t_defined)
TEST(uintptr_t_defined)
{
    int x = 42;
    struct uintptr_t ptr = {(uintptr_t)&x};
    ASSERT_TRUE(ptr.v != 0);
}

TEST_SUITE(intptr_size_matches_pointer)
TEST(intptr_size_matches_pointer)
{
    ASSERT_EQ(sizeof(void*), sizeof(struct intptr_t));
    ASSERT_EQ(sizeof(void*), sizeof(struct uintptr_t));
}

/* ---- ARITHMETIC SUITES ---- */
TEST_SUITE(uint8_overflow)
TEST(uint8_overflow)
{
    struct uint8_t val = {255};
    val.v++;
    ASSERT_EQ(0, val.v);
}

TEST_SUITE(int8_overflow)
TEST(int8_overflow)
{
    struct int8_t val = {127};
    val.v++;
    ASSERT_EQ(-128, val.v);
}

TEST_SUITE(uint64_arithmetic)
TEST(uint64_arithmetic)
{
    struct uint64_t a = {UINT64_C(10000000000)};
    struct uint64_t b = {UINT64_C(20000000000)};
    struct uint64_t sum = {a.v + b.v};
    ASSERT_EQ(UINT64_C(30000000000), sum.v);
}

/* ---- CONSISTENCY SUITES ---- */
TEST_SUITE(type_hierarchy)
TEST(type_hierarchy)
{
    ASSERT_TRUE(sizeof(struct int8_t) <= sizeof(struct int16_t));
    ASSERT_TRUE(sizeof(struct int16_t) <= sizeof(struct int32_t));
    ASSERT_TRUE(sizeof(struct int32_t) <= sizeof(struct int64_t));
}

TEST_SUITE(least_at_least_exact)
TEST(least_at_least_exact)
{
    ASSERT_TRUE(sizeof(struct int_least8_t) >= sizeof(struct int8_t));
    ASSERT_TRUE(sizeof(struct int_least16_t) >= sizeof(struct int16_t));
    ASSERT_TRUE(sizeof(struct int_least32_t) >= sizeof(struct int32_t));
    ASSERT_TRUE(sizeof(struct int_least64_t) >= sizeof(struct int64_t));
}

TEST_SUITE(fast_at_least_least)
TEST(fast_at_least_least)
{
    ASSERT_TRUE(sizeof(struct int_fast8_t) >= sizeof(struct int_least8_t));
    ASSERT_TRUE(sizeof(struct int_fast16_t) >= sizeof(struct int_least16_t));
    ASSERT_TRUE(sizeof(struct int_fast32_t) >= sizeof(struct int_least32_t));
    ASSERT_TRUE(sizeof(struct int_fast64_t) >= sizeof(struct int_least64_t));
}

TEST_SUITE(max_is_largest)
TEST(max_is_largest)
{
    ASSERT_TRUE(sizeof(struct intmax_t) >= sizeof(struct int64_t));
    ASSERT_TRUE(sizeof(struct uintmax_t) >= sizeof(struct uint64_t));
}

TEST_MAIN()

