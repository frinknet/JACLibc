/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_C99
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(inttypes.h);

/* ============================================================================
 * TYPE DEFINITIONS
 * ============================================================================ */
TEST_SUITE(type_definitions);

TEST(imax_t_defined) {
	imax_t val = 42;
	ASSERT_EQ(42, val);
}

TEST(umax_t_defined) {
	umax_t val = 42U;
	ASSERT_EQ(42U, val);
}

TEST(imaxdiv_t_defined) {
	imaxdiv_t result;
	result.quot = 10;
	result.rem = 3;
	
	ASSERT_EQ(10, result.quot);
	ASSERT_EQ(3, result.rem);
}

/* ============================================================================
 * IMAXDIV TESTS
 * ============================================================================ */
TEST_SUITE(imaxdiv);

TEST(imaxdiv_basic) {
	imaxdiv_t result = imaxdiv(10, 3);
	
	ASSERT_EQ(3, result.quot);
	ASSERT_EQ(1, result.rem);
}

TEST(imaxdiv_exact) {
	imaxdiv_t result = imaxdiv(10, 5);
	
	ASSERT_EQ(2, result.quot);
	ASSERT_EQ(0, result.rem);
}

TEST(imaxdiv_negative_numerator) {
	imaxdiv_t result = imaxdiv(-10, 3);
	
	ASSERT_EQ(-3, result.quot);
	ASSERT_EQ(-1, result.rem);
}

TEST(imaxdiv_negative_denominator) {
	imaxdiv_t result = imaxdiv(10, -3);
	
	ASSERT_EQ(-3, result.quot);
	ASSERT_EQ(1, result.rem);
}

TEST(imaxdiv_both_negative) {
	imaxdiv_t result = imaxdiv(-10, -3);
	
	ASSERT_EQ(3, result.quot);
	ASSERT_EQ(-1, result.rem);
}

/* ============================================================================
 * STRTOIMAX TESTS
 * ============================================================================ */
TEST_SUITE(strtoimax);

TEST(strtoimax_decimal) {
	char *endptr;
	intmax_t val = strtoimax("12345", &endptr, 10);
	
	ASSERT_EQ(12345, val);
	ASSERT_EQ('\0', *endptr);
}

TEST(strtoimax_negative) {
	char *endptr;
	intmax_t val = strtoimax("-12345", &endptr, 10);
	
	ASSERT_EQ(-12345, val);
}

TEST(strtoimax_hex) {
	char *endptr;
	intmax_t val = strtoimax("0x1234", &endptr, 16);
	
	ASSERT_EQ(0x1234, val);
}

TEST(strtoimax_octal) {
	char *endptr;
	intmax_t val = strtoimax("0755", &endptr, 8);
	
	ASSERT_EQ(0755, val);
}

TEST(strtoimax_auto_base) {
	char *endptr;
	
	intmax_t dec = strtoimax("123", &endptr, 0);
	ASSERT_EQ(123, dec);
	
	intmax_t hex = strtoimax("0x123", &endptr, 0);
	ASSERT_EQ(0x123, hex);
	
	intmax_t oct = strtoimax("0123", &endptr, 0);
	ASSERT_EQ(0123, oct);
}

TEST(strtoimax_partial) {
	char *endptr;
	intmax_t val = strtoimax("123abc", &endptr, 10);
	
	ASSERT_EQ(123, val);
	ASSERT_STR_EQ("abc", endptr);
}

/* ============================================================================
 * STRTOUMAX TESTS
 * ============================================================================ */
TEST_SUITE(strtoumax);

TEST(strtoumax_decimal) {
	char *endptr;
	uintmax_t val = strtoumax("12345", &endptr, 10);
	
	ASSERT_EQ(12345U, val);
}

TEST(strtoumax_hex) {
	char *endptr;
	uintmax_t val = strtoumax("ABCDEF", &endptr, 16);
	
	ASSERT_EQ(0xABCDEF, val);
}

TEST(strtoumax_large) {
	char *endptr;
	uintmax_t val = strtoumax("18446744073709551615", &endptr, 10);
	
	ASSERT_EQ(UINT64_MAX, val);
}

/* ============================================================================
 * PRINTF FORMAT MACROS - 8-bit
 * ============================================================================ */
TEST_SUITE(printf_8bit);

TEST(pri_8bit_macros_defined) {
	ASSERT_STR_EQ("hhd", PRId8);
	ASSERT_STR_EQ("hhi", PRIi8);
	ASSERT_STR_EQ("hho", PRIo8);
	ASSERT_STR_EQ("hhu", PRIu8);
	ASSERT_STR_EQ("hhx", PRIx8);
	ASSERT_STR_EQ("hhX", PRIX8);
}

TEST(printf_8bit_usage) {
	char buf[64];
	int8_t val = 127;
	
	snprintf(buf, sizeof(buf), "%" PRId8, val);
	ASSERT_STR_EQ("127", buf);
}

TEST(printf_u8_usage) {
	char buf[64];
	uint8_t val = 255;
	
	snprintf(buf, sizeof(buf), "%" PRIu8, val);
	ASSERT_STR_EQ("255", buf);
}

/* ============================================================================
 * PRINTF FORMAT MACROS - 16-bit
 * ============================================================================ */
TEST_SUITE(printf_16bit);

TEST(pri_16bit_macros_defined) {
	ASSERT_STR_EQ("hd", PRId16);
	ASSERT_STR_EQ("hi", PRIi16);
	ASSERT_STR_EQ("ho", PRIo16);
	ASSERT_STR_EQ("hu", PRIu16);
	ASSERT_STR_EQ("hx", PRIx16);
	ASSERT_STR_EQ("hX", PRIX16);
}

TEST(printf_16bit_usage) {
	char buf[64];
	int16_t val = 32767;
	
	snprintf(buf, sizeof(buf), "%" PRId16, val);
	ASSERT_STR_EQ("32767", buf);
}

TEST(printf_u16_usage) {
	char buf[64];
	uint16_t val = 65535;
	
	snprintf(buf, sizeof(buf), "%" PRIu16, val);
	ASSERT_STR_EQ("65535", buf);
}

/* ============================================================================
 * PRINTF FORMAT MACROS - 32-bit
 * ============================================================================ */
TEST_SUITE(printf_32bit);

TEST(pri_32bit_macros_defined) {
	ASSERT_STR_EQ("d", PRId32);
	ASSERT_STR_EQ("i", PRIi32);
	ASSERT_STR_EQ("o", PRIo32);
	ASSERT_STR_EQ("u", PRIu32);
	ASSERT_STR_EQ("x", PRIx32);
	ASSERT_STR_EQ("X", PRIX32);
}

TEST(printf_32bit_usage) {
	char buf[64];
	int32_t val = 2147483647;
	
	snprintf(buf, sizeof(buf), "%" PRId32, val);
	ASSERT_STR_EQ("2147483647", buf);
}

TEST(printf_u32_usage) {
	char buf[64];
	uint32_t val = 4294967295U;
	
	snprintf(buf, sizeof(buf), "%" PRIu32, val);
	ASSERT_STR_EQ("4294967295", buf);
}

TEST(printf_32bit_hex) {
	char buf[64];
	uint32_t val = 0xDEADBEEF;
	
	snprintf(buf, sizeof(buf), "0x%" PRIx32, val);
	ASSERT_STR_EQ("0xdeadbeef", buf);
}

/* ============================================================================
 * PRINTF FORMAT MACROS - 64-bit
 * ============================================================================ */
TEST_SUITE(printf_64bit);

TEST(pri_64bit_macros_defined) {
	ASSERT_NOT_NULL(PRId64);
	ASSERT_NOT_NULL(PRIi64);
	ASSERT_NOT_NULL(PRIo64);
	ASSERT_NOT_NULL(PRIu64);
	ASSERT_NOT_NULL(PRIx64);
	ASSERT_NOT_NULL(PRIX64);
}

TEST(printf_64bit_usage) {
	char buf[64];
	int64_t val = 9223372036854775807LL;
	
	snprintf(buf, sizeof(buf), "%" PRId64, val);
	ASSERT_STR_EQ("9223372036854775807", buf);
}

TEST(printf_u64_usage) {
	char buf[64];
	uint64_t val = 18446744073709551615ULL;
	
	snprintf(buf, sizeof(buf), "%" PRIu64, val);
	ASSERT_STR_EQ("18446744073709551615", buf);
}

/* ============================================================================
 * PRINTF FORMAT MACROS - MAX/PTR
 * ============================================================================ */
TEST_SUITE(printf_max_ptr);

TEST(pri_max_macros_defined) {
	ASSERT_NOT_NULL(PRIdMAX);
	ASSERT_NOT_NULL(PRIiMAX);
	ASSERT_NOT_NULL(PRIoMAX);
	ASSERT_NOT_NULL(PRIuMAX);
	ASSERT_NOT_NULL(PRIxMAX);
	ASSERT_NOT_NULL(PRIXMAX);
}

TEST(pri_ptr_macros_defined) {
	ASSERT_NOT_NULL(PRIdPTR);
	ASSERT_NOT_NULL(PRIiPTR);
	ASSERT_NOT_NULL(PRIoPTR);
	ASSERT_NOT_NULL(PRIuPTR);
	ASSERT_NOT_NULL(PRIxPTR);
	ASSERT_NOT_NULL(PRIXPTR);
}

TEST(printf_max_usage) {
	char buf[64];
	intmax_t val = 123456789;
	
	snprintf(buf, sizeof(buf), "%" PRIdMAX, val);
	ASSERT_STR_EQ("123456789", buf);
}

TEST(printf_ptr_usage) {
	char buf[64];
	uintptr_t val = 0x12345678;
	
	snprintf(buf, sizeof(buf), "0x%" PRIxPTR, val);
	ASSERT_STR_EQ("0x12345678", buf);
}

/* ============================================================================
 * SCANF FORMAT MACROS - 8-bit
 * ============================================================================ */
TEST_SUITE(scanf_8bit);

TEST(scn_8bit_macros_defined) {
	ASSERT_STR_EQ("hhd", SCNd8);
	ASSERT_STR_EQ("hhi", SCNi8);
	ASSERT_STR_EQ("hho", SCNo8);
	ASSERT_STR_EQ("hhu", SCNu8);
	ASSERT_STR_EQ("hhx", SCNx8);
}

TEST(scanf_8bit_usage) {
	int8_t val;
	sscanf("127", "%" SCNd8, &val);
	ASSERT_EQ(127, val);
}

TEST(scanf_u8_usage) {
	uint8_t val;
	sscanf("255", "%" SCNu8, &val);
	ASSERT_EQ(255, val);
}

/* ============================================================================
 * SCANF FORMAT MACROS - 16-bit
 * ============================================================================ */
TEST_SUITE(scanf_16bit);

TEST(scn_16bit_macros_defined) {
	ASSERT_STR_EQ("hd", SCNd16);
	ASSERT_STR_EQ("hi", SCNi16);
	ASSERT_STR_EQ("ho", SCNo16);
	ASSERT_STR_EQ("hu", SCNu16);
	ASSERT_STR_EQ("hx", SCNx16);
}

TEST(scanf_16bit_usage) {
	int16_t val;
	sscanf("32767", "%" SCNd16, &val);
	ASSERT_EQ(32767, val);
}

/* ============================================================================
 * SCANF FORMAT MACROS - 32-bit
 * ============================================================================ */
TEST_SUITE(scanf_32bit);

TEST(scn_32bit_macros_defined) {
	ASSERT_STR_EQ("d", SCNd32);
	ASSERT_STR_EQ("i", SCNi32);
	ASSERT_STR_EQ("o", SCNo32);
	ASSERT_STR_EQ("u", SCNu32);
	ASSERT_STR_EQ("x", SCNx32);
}

TEST(scanf_32bit_usage) {
	int32_t val;
	sscanf("2147483647", "%" SCNd32, &val);
	ASSERT_EQ(2147483647, val);
}

TEST(scanf_32bit_hex) {
	uint32_t val;
	sscanf("deadbeef", "%" SCNx32, &val);
	ASSERT_EQ(0xDEADBEEF, val);
}

/* ============================================================================
 * SCANF FORMAT MACROS - 64-bit
 * ============================================================================ */
TEST_SUITE(scanf_64bit);

TEST(scn_64bit_macros_defined) {
	ASSERT_NOT_NULL(SCNd64);
	ASSERT_NOT_NULL(SCNi64);
	ASSERT_NOT_NULL(SCNo64);
	ASSERT_NOT_NULL(SCNu64);
	ASSERT_NOT_NULL(SCNx64);
}

TEST(scanf_64bit_usage) {
	int64_t val;
	sscanf("9223372036854775807", "%" SCNd64, &val);
	ASSERT_EQ(9223372036854775807LL, val);
}

/* ============================================================================
 * SCANF FORMAT MACROS - MAX/PTR
 * ============================================================================ */
TEST_SUITE(scanf_max_ptr);

TEST(scn_max_macros_defined) {
	ASSERT_NOT_NULL(SCNdMAX);
	ASSERT_NOT_NULL(SCNiMAX);
	ASSERT_NOT_NULL(SCNoMAX);
	ASSERT_NOT_NULL(SCNuMAX);
	ASSERT_NOT_NULL(SCNxMAX);
}

TEST(scn_ptr_macros_defined) {
	ASSERT_NOT_NULL(SCNdPTR);
	ASSERT_NOT_NULL(SCNiPTR);
	ASSERT_NOT_NULL(SCNoPTR);
	ASSERT_NOT_NULL(SCNuPTR);
	ASSERT_NOT_NULL(SCNxPTR);
}

TEST(scanf_max_usage) {
	intmax_t val;
	sscanf("123456789", "%" SCNdMAX, &val);
	ASSERT_EQ(123456789, val);
}

/* ============================================================================
 * ROUNDTRIP TESTS
 * ============================================================================ */
TEST_SUITE(roundtrip);

TEST(printf_scanf_8bit_roundtrip) {
	char buf[64];
	int8_t original = 123;
	int8_t parsed;
	
	snprintf(buf, sizeof(buf), "%" PRId8, original);
	sscanf(buf, "%" SCNd8, &parsed);
	
	ASSERT_EQ(original, parsed);
}

TEST(printf_scanf_16bit_roundtrip) {
	char buf[64];
	int16_t original = 12345;
	int16_t parsed;
	
	snprintf(buf, sizeof(buf), "%" PRId16, original);
	sscanf(buf, "%" SCNd16, &parsed);
	
	ASSERT_EQ(original, parsed);
}

TEST(printf_scanf_32bit_roundtrip) {
	char buf[64];
	int32_t original = 1234567890;
	int32_t parsed;
	
	snprintf(buf, sizeof(buf), "%" PRId32, original);
	sscanf(buf, "%" SCNd32, &parsed);
	
	ASSERT_EQ(original, parsed);
}

TEST(printf_scanf_64bit_roundtrip) {
	char buf[64];
	int64_t original = 9223372036854775807LL;
	int64_t parsed;
	
	snprintf(buf, sizeof(buf), "%" PRId64, original);
	sscanf(buf, "%" SCNd64, &parsed);
	
	ASSERT_EQ(original, parsed);
}

TEST(printf_scanf_hex_roundtrip) {
	char buf[64];
	uint32_t original = 0xDEADBEEF;
	uint32_t parsed;
	
	snprintf(buf, sizeof(buf), "%" PRIx32, original);
	sscanf(buf, "%" SCNx32, &parsed);
	
	ASSERT_EQ(original, parsed);
}

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(imaxdiv_by_one) {
	imaxdiv_t result = imaxdiv(42, 1);
	
	ASSERT_EQ(42, result.quot);
	ASSERT_EQ(0, result.rem);
}

TEST(imaxdiv_zero_numerator) {
	imaxdiv_t result = imaxdiv(0, 5);
	
	ASSERT_EQ(0, result.quot);
	ASSERT_EQ(0, result.rem);
}

TEST(strtoimax_whitespace) {
	char *endptr;
	intmax_t val = strtoimax("  \t123", &endptr, 10);
	
	ASSERT_EQ(123, val);
}

TEST(strtoimax_plus_sign) {
	char *endptr;
	intmax_t val = strtoimax("+123", &endptr, 10);
	
	ASSERT_EQ(123, val);
}

TEST_MAIN()

#else

// Stub for pre-C99
int main(void) {
	printf("inttypes.h requires C99 or later\n");
	return 0;
}

#endif

