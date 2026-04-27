/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#include <net/udp.h>

TEST_TYPE(unit);
TEST_UNIT(net/udp.h);

/* ============================================================================ */

TEST_SUITE(size);

TEST(size_hdr_len_value) { ASSERT_EQ(8, UDP_HDR_LEN); }
TEST(size_udp_hdr_check) { ASSERT_EQ(8, sizeof(struct udp_hdr)); }
TEST(size_udphdr_check) { ASSERT_EQ(8, sizeof(struct udphdr)); }

/* ============================================================================ */

TEST_SUITE(macro);

TEST(macro_ipport_echo_value) { ASSERT_EQ(7, IPPORT_ECHO); }
TEST(macro_ipport_domain_value) { ASSERT_EQ(53, IPPORT_DOMAIN); }
TEST(macro_ipport_http_value) { ASSERT_EQ(80, IPPORT_HTTP); }
TEST(macro_ipport_ntp_value) { ASSERT_EQ(123, IPPORT_NTP); }
TEST(macro_ipport_bootps_value) { ASSERT_EQ(67, IPPORT_BOOTPS); }
TEST(macro_udp_cork_value) { ASSERT_EQ(1, UDP_CORK); }
TEST(macro_udp_encap_value) { ASSERT_EQ(100, UDP_ENCAP); }
TEST(macro_udp_segment_value) { ASSERT_EQ(103, UDP_SEGMENT); }

/* ============================================================================ */

TEST_SUITE(udp_hdr_fields);

TEST(udp_hdr_fields_layout) {
	struct udp_hdr h;
	memset(&h, 0, sizeof(h));
	h.source = htons(1234);
	h.dest = htons(5678);
	h.len = htons(20);
	h.check = 0;

	ASSERT_EQ(htons(1234), h.source);
	ASSERT_EQ(htons(5678), h.dest);
	ASSERT_EQ(htons(20), h.len);
}

/* ============================================================================ */

TEST_SUITE(udphdr_alias);

TEST(udphdr_alias_mapping) {
	struct udp_hdr h;
	memset(&h, 0, sizeof(h));
	h.source = htons(9999);

	struct udphdr *uh = (struct udphdr *)&h;
	ASSERT_EQ(htons(9999), uh->uh_sport);
}

/* ============================================================================ */

TEST_SUITE(udppseudo_size);

TEST(udppseudo_size_check) {
	/* 4+4+1+1+2 = 12 bytes */
	ASSERT_EQ(12, sizeof(struct udppseudo));
}

/* ============================================================================ */

TEST_SUITE(udppseudo6_size);

TEST(udppseudo6_size_check) {
	/* 16+16+4+3+1 = 40 bytes */
	ASSERT_EQ(40, sizeof(struct udppseudo6));
}

/* ============================================================================ */

TEST_SUITE(udp_pseudo_sum_ipv4);

TEST(udp_pseudo_sum_ipv4_nonzero) {
	struct in_addr s = { .s_addr = htonl(0xC0A80001) };
	struct in_addr d = { .s_addr = htonl(0xC0A80002) };
	uint32_t sum = udp_pseudo_sum_ipv4(s, d, 20);
	ASSERT_TRUE(sum > 0);
}

/* ============================================================================ */

TEST_SUITE(udp_checksum_finish);

TEST(udp_checksum_finish_zero_data) {
	uint16_t buf[2] = { 0, 0 };
	/* Sum of zeros is 0. ~0 is 0xFFFF */
	ASSERT_EQ(0xFFFF, udp_checksum_finish(0, buf, 4));
}

TEST(udp_checksum_finish_verify) {
	uint16_t buf[2] = { htons(0x1234), htons(0x5678) };
	uint16_t csum = udp_checksum_finish(0, buf, 4);

	/* Verify: Data + Checksum should result in 0x0000 */
	uint16_t verify[3] = { buf[0], buf[1], csum };
	ASSERT_EQ(0x0000, udp_checksum_finish(0, verify, 6));
}

/* ============================================================================ */

TEST_SUITE(udp_checksum_ipv4);

TEST(udp_checksum_ipv4_basic) {
	struct in_addr s = { .s_addr = htonl(0x0A000001) };
	struct in_addr d = { .s_addr = htonl(0x0A000002) };
	uint16_t data = 0; /* 2 bytes of zero */

	uint16_t csum = udp_checksum_ipv4(s, d, &data, 2);
	ASSERT_TRUE(csum != 0xFFFF); /* Should not be empty checksum */
}

/* ============================================================================ */

TEST_MAIN_IF(JACL_HAS_POSIX, "net/udp.h requires POSIX\n")
