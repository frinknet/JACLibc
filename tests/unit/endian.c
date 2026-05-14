/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <endian.h>
#include <stdint.h>

TEST_TYPE(unit);
TEST_UNIT(endian.h);

/* ============================================================================ */
TEST_SUITE(bswap_functions);

TEST(bswap_16_basic) {
	ASSERT_EQ(0x0001, __bswap_16(0x0100));
	ASSERT_EQ(0xABCD, __bswap_16(0xCDAB));
}

TEST(bswap_32_basic) {
	ASSERT_EQ(0x00000001, __bswap_32(0x01000000));
	ASSERT_EQ(0xDEADBEEF, __bswap_32(0xEFBEADDE));
}

TEST(bswap_64_basic) {
	ASSERT_EQ(0x0000000000000001ULL, __bswap_64(0x0100000000000000ULL));
	ASSERT_EQ(0x0102030405060708ULL, __bswap_64(0x0807060504030201ULL));
}

TEST(bswap_identity_16) {
	uint16_t x = 0x1234;
	ASSERT_EQ(x, __bswap_16(__bswap_16(x)));
}

TEST(bswap_identity_32) {
	uint32_t x = 0x12345678;
	ASSERT_EQ(x, __bswap_32(__bswap_32(x)));
}

TEST(bswap_identity_64) {
	uint64_t x = 0x123456789ABCDEF0ULL;
	ASSERT_EQ(x, __bswap_64(__bswap_64(x)));
}

/* ============================================================================ */
TEST_SUITE(pdp_swap_functions);

TEST(pdp_to_le32_basic) {
	/* 
	 * Input: 0x01020304
	 * Bytes: [01] [02] [03] [04] (MSB to LSB)
	 * Term 1: (0x00020000) >> 16 = 0x00000002
	 * Term 2: (0x00000300) << 16 = 0x03000000
	 * Term 3: (0x00000004) << 8  = 0x00000400
	 * Term 4: (0x01000000) >> 8  = 0x00010000
	 * Sum:   0x03010402
	 */
	ASSERT_EQ(0x03010402, __pdp_to_le32(0x01020304));
}

TEST(pdp_to_le64_basic) {
	/* 
	 * Input: 0x0102030405060708
	 * Lower 32: 0x05060708 -> pdp -> 0x07050806
	 * Upper 32: 0x01020304 -> pdp -> 0x03010402
	 * Result: (0x03010402 << 32) | 0x07050806
	 *       = 0x0301040207050806
	 */
	ASSERT_EQ(0x0301040207050806ULL, __pdp_to_le64(0x0102030405060708ULL));
}

/* Note: __pdp_to_le32 in your implementation is NOT an involution.
 * f(f(x)) != x. So no identity test.
 */

/* ============================================================================ */
TEST_SUITE(host_to_le);

TEST(htole16_identity_on_le) {
#if JACL_HAS_LE
	uint16_t x = 0x1234;
	ASSERT_EQ(x, htole16(x));
#else
	TEST_SKIP("Not LE architecture");
#endif
}

TEST(htole32_identity_on_le) {
#if JACL_HAS_LE
	uint32_t x = 0x12345678;
	ASSERT_EQ(x, htole32(x));
#else
	TEST_SKIP("Not LE architecture");
#endif
}

TEST(htole16_swap_on_be) {
#if JACL_HAS_BE
	uint16_t x = 0x1234;
	ASSERT_EQ(0x3412, htole16(x));
#else
	TEST_SKIP("Not BE architecture");
#endif
}

TEST(htole32_pdp_swap) {
#if JACL_HAS_PDP
	uint32_t x = 0x01020304;
	ASSERT_EQ(0x03010402, htole32(x));
#else
	TEST_SKIP("Not PDP architecture");
#endif
}

TEST(htole64_pdp_swap) {
#if JACL_HAS_PDP
	uint64_t x = 0x0102030405060708ULL;
	ASSERT_EQ(0x0301040207050806ULL, htole64(x));
#else
	TEST_SKIP("Not PDP architecture");
#endif
}

/* ============================================================================ */
TEST_SUITE(host_to_be);

TEST(htobe16_swap_on_le) {
#if JACL_HAS_LE
	uint16_t x = 0x1234;
	ASSERT_EQ(0x3412, htobe16(x));
#else
	TEST_SKIP("Not LE architecture");
#endif
}

TEST(htobe32_swap_on_le) {
#if JACL_HAS_LE
	uint32_t x = 0x12345678;
	ASSERT_EQ(0x78563412, htobe32(x));
#else
	TEST_SKIP("Not LE architecture");
#endif
}

TEST(htobe16_identity_on_be) {
#if JACL_HAS_BE
	uint16_t x = 0x1234;
	ASSERT_EQ(x, htobe16(x));
#else
	TEST_SKIP("Not BE architecture");
#endif
}

TEST(htobe32_pdp_swap) {
#if JACL_HAS_PDP
	/* htobe32(x) = __bswap_32(__pdp_to_le32(x))
	 * x = 0x01020304
	 * pdp = 0x03010402
	 * bswap(0x03010402) = 0x02040103
	 */
	uint32_t x = 0x01020304;
	ASSERT_EQ(0x02040103, htobe32(x));
#else
	TEST_SKIP("Not PDP architecture");
#endif
}

/* ============================================================================ */
TEST_SUITE(network_order);

TEST(htonl_consistency) {
	uint32_t x = 0x12345678;
	ASSERT_EQ(htobe32(x), htonl(x));
}

TEST(ntohl_consistency) {
	uint32_t x = 0x12345678;
	ASSERT_EQ(be32toh(x), ntohl(x));
}

TEST(htons_consistency) {
	ASSERT_EQ(htobe16(0x1234), htons(0x1234));
}

TEST(ntohs_consistency) {
	ASSERT_EQ(be16toh(0x1234), ntohs(0x1234));
}

/* ============================================================================ */
TEST_SUITE(round_trip);

TEST(round_trip_le16) {
	uint16_t x = 0x1234;
	uint16_t le = htole16(x);
	uint16_t host = le16toh(le);
	ASSERT_EQ(x, host);
}

TEST(round_trip_be32) {
	uint32_t x = 0x12345678;
	uint32_t be = htobe32(x);
	uint32_t host = be32toh(be);
	ASSERT_EQ(x, host);
}

TEST(round_trip_be64) {
	uint64_t x = 0x0102030405060708ULL;
	uint64_t be = htobe64(x);
	uint64_t host = be64toh(be);
	ASSERT_EQ(x, host);
}

TEST(round_trip_le32_pdp) {
#if JACL_HAS_PDP
	uint32_t x = 0x12345678;
	uint32_t le = htole32(x);
	uint32_t host = le32toh(le);
	ASSERT_EQ(x, host);
#else
	TEST_SKIP("Not PDP architecture");
#endif
}

TEST(round_trip_be32_pdp) {
#if JACL_HAS_PDP
	uint32_t x = 0x12345678;
	uint32_t be = htobe32(x);
	uint32_t host = be32toh(be);
	ASSERT_EQ(x, host);
#else
	TEST_SKIP("Not PDP architecture");
#endif
}

/* ============================================================================ */
TEST_SUITE(boundary_values);

TEST(bswap_16_zero_max) {
	ASSERT_EQ(0x0000, __bswap_16(0x0000));
	ASSERT_EQ(0xFFFF, __bswap_16(0xFFFF));
}

TEST(bswap_32_zero_max) {
	ASSERT_EQ(0x00000000, __bswap_32(0x00000000));
	ASSERT_EQ(0xFFFFFFFF, __bswap_32(0xFFFFFFFF));
}

TEST(bswap_64_zero_max) {
	ASSERT_EQ(0x0000000000000000ULL, __bswap_64(0x0000000000000000ULL));
	ASSERT_EQ(0xFFFFFFFFFFFFFFFFULL, __bswap_64(0xFFFFFFFFFFFFFFFFULL));
}

TEST(htole16_zero_max) {
	uint16_t z = htole16(0);
	uint16_t m = htole16(0xFFFF);
	ASSERT_EQ(0, le16toh(z));
	ASSERT_EQ(0xFFFF, le16toh(m));
}

/* ============================================================================ */
TEST_SUITE(cross_conversion);

TEST(le_to_be_is_bswap_16) {
	uint16_t x = 0x1234;
	ASSERT_EQ(htole16(x), htobe16(__bswap_16(x)));
}

TEST(le_to_be_is_bswap_32) {
	uint32_t x = 0x12345678;
	ASSERT_EQ(htole32(x), htobe32(__bswap_32(x)));
}

TEST(round_trip_le64_pdp) {
#if JACL_HAS_PDP
	uint64_t x = 0x0102030405060708ULL;
	uint64_t le = htole64(x);
	uint64_t host = le64toh(le);
	ASSERT_EQ(x, host);
#else
	TEST_SKIP("Not PDP architecture");
#endif
}

TEST(round_trip_be64_pdp) {
#if JACL_HAS_PDP
	uint64_t x = 0x0102030405060708ULL;
	uint64_t be = htobe64(x);
	uint64_t host = be64toh(be);
	ASSERT_EQ(x, host);
#else
	TEST_SKIP("Not PDP architecture");
#endif
}

/* ============================================================================ */
TEST_SUITE(type_safety);

TEST(htobe32_signed_input) {
	/* Ensure negative signed ints are handled as unsigned bit patterns */
	int32_t neg = -1; /* 0xFFFFFFFF */
	uint32_t res = htobe32(neg);
	/* On LE, bswap(0xFFFFFFFF) is 0xFFFFFFFF */
	ASSERT_EQ(0xFFFFFFFF, res);
}

TEST(htole16_char_input) {
	/* Ensure small types are promoted correctly */
	unsigned char c = 0xAB;
	uint16_t res = htole16(c);
#if JACL_HAS_LE
	ASSERT_EQ(0x00AB, res);
#else
	ASSERT_EQ(0xAB00, res);
#endif
}

/* ============================================================================ */
TEST_SUITE(volatile_access);

TEST(volatile_htobe32) {
	volatile uint32_t v = 0x12345678;
	uint32_t res = htobe32(v);
#if JACL_HAS_LE
	ASSERT_EQ(0x78563412, res);
#else
	ASSERT_EQ(0x12345678, res);
#endif
}

TEST(volatile_le32toh) {
	volatile uint32_t v = 0x12345678;
	uint32_t res = le32toh(v);
#if JACL_HAS_LE
	ASSERT_EQ(0x12345678, res);
#else
	ASSERT_EQ(0x78563412, res);
#endif
}

/* ============================================================================ */
TEST_MAIN()
