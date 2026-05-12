/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <string.h>
#include <net/udp.h>

TEST_TYPE(unit);
TEST_UNIT(net/udp.h);

/* ============================================================================ */

TEST_SUITE(size);

TEST(size_hdr_len_value) { ASSERT_EQ(8, UDP_HDR_LEN); }
TEST(size_udp_hdr_check) { ASSERT_EQ(8, sizeof(struct udp_hdr)); }
TEST(size_udphdr_check) { ASSERT_EQ(8, sizeof(struct udphdr)); }
TEST(size_udphdr_typedef) { ASSERT_EQ(8, sizeof(udphdr)); }
TEST(size_udppseudo_check) { ASSERT_EQ(12, sizeof(struct udppseudo)); }
TEST(size_udppseudo6_check) { ASSERT_EQ(40, sizeof(struct udppseudo6)); }

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

TEST_SUITE(udphdr_fields);

TEST(udphdr_fields_layout) {
	struct udphdr h;
	memset(&h, 0, sizeof(h));
	h.uh_sport = htons(1234);
	h.uh_dport = htons(5678);
	h.uh_ulen = htons(20);
	h.uh_sum = 0;

	ASSERT_EQ(htons(1234), h.uh_sport);
	ASSERT_EQ(htons(5678), h.uh_dport);
	ASSERT_EQ(htons(20), h.uh_ulen);
}

TEST(udphdr_member_aliases) {
	/* Verify Linux and BSD structs have same layout */
	struct udp_hdr h1;
	struct udphdr h2;

	memset(&h1, 0, sizeof(h1));
	memset(&h2, 0, sizeof(h2));

	h1.source = htons(80);
	h2.uh_sport = htons(80);

	ASSERT_EQ(h1.source, h2.uh_sport);
	ASSERT_EQ(sizeof(h1), sizeof(h2));
}

/* ============================================================================ */

TEST_SUITE(udp_checksum_finish);

TEST(udp_checksum_finish_zero_data) {
	uint16_t buf[2] = { 0, 0 };
	ASSERT_EQ(0xFFFF, udp_checksum_finish(0, buf, 4));
}

TEST(udp_checksum_finish_verify) {
    uint8_t data[4] = {0x12, 0x34, 0x56, 0x78};
    uint16_t csum = udp_checksum_finish(0, data, 4);

    /* Append checksum in network byte order */
    uint8_t verify[6] = {0x12, 0x34, 0x56, 0x78, (csum >> 8) & 0xFF, csum & 0xFF};

    /* FIXED: Data + Checksum should verify to 0 (one's complement zero) */
    ASSERT_EQ(0, udp_checksum_finish(0, verify, 6));  /* Changed from 0xFFFF to 0 */
}

TEST(udp_checksum_finish_all_ones) {
	uint16_t buf[4] = { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };
	ASSERT_EQ(0x0000, udp_checksum_finish(0, buf, 8));
}

TEST(udp_checksum_finish_odd_byte) {
	uint8_t buf[1] = { 0xFF };
	uint16_t csum = udp_checksum_finish(0, buf, 1);
	ASSERT_TRUE(csum != 0);
	ASSERT_TRUE(csum != 0xFFFF);
}

TEST(udp_checksum_finish_single_word) {
	uint16_t buf[1] = { htons(0xABCD) };
	uint16_t csum = udp_checksum_finish(0, buf, 2);
	ASSERT_TRUE(csum != 0);
}

TEST(udp_checksum_finish_empty) {
	ASSERT_EQ(0xFFFF, udp_checksum_finish(0, NULL, 0));
}

/* ============================================================================ */

TEST_SUITE(udp_pseudo_sum_ipv4);

TEST(udp_pseudo_sum_ipv4_nonzero) {
	struct in_addr s = { .s_addr = htonl(0xC0A80001) };
	struct in_addr d = { .s_addr = htonl(0xC0A80002) };
	uint32_t sum = udp_pseudo_sum_ipv4(s, d, 20);
	ASSERT_TRUE(sum > 0);
}

TEST(udp_pseudo_sum_ipv4_zero_addrs) {
	struct in_addr z = { .s_addr = 0 };
	uint32_t sum = udp_pseudo_sum_ipv4(z, z, 0);
	ASSERT_TRUE(sum > 0);
}

TEST(udp_pseudo_sum_ipv4_consistency) {
	struct in_addr s = { .s_addr = htonl(0x0A000001) };
	struct in_addr d = { .s_addr = htonl(0x0A000002) };

	uint32_t sum1 = udp_pseudo_sum_ipv4(s, d, 100);
	uint32_t sum2 = udp_pseudo_sum_ipv4(s, d, 100);

	ASSERT_EQ(sum1, sum2);
}

/* ============================================================================ */

TEST_SUITE(udp_checksum_ipv4);

TEST(udp_checksum_ipv4_basic) {
	struct in_addr s = { .s_addr = htonl(0x0A000001) };
	struct in_addr d = { .s_addr = htonl(0x0A000002) };
	uint16_t data = 0;

	uint16_t csum = udp_checksum_ipv4(s, d, &data, 2);
	ASSERT_TRUE(csum != 0xFFFF);
}

TEST(udp_checksum_ipv4_roundtrip) {
    struct in_addr s = { .s_addr = htonl(0xC0A80001) };
    struct in_addr d = { .s_addr = htonl(0xC0A80002) };
    uint8_t data[4] = {0x12, 0x34, 0x56, 0x78};

    uint16_t csum = udp_checksum_ipv4(s, d, data, 4);
    ASSERT_TRUE(csum != 0);

    /* Verify: pseudo-header + data + checksum = 0xFFFF before complement */
    uint32_t sum = udp_pseudo_sum_ipv4(s, d, 4);
    sum += (data[0] << 8 | data[1]);
    sum += (data[2] << 8 | data[3]);
    sum += csum;
    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    ASSERT_EQ(0xFFFF, sum);  /* This is correct - sum BEFORE final complement */
}

/* ============================================================================ */

TEST_SUITE(udp_checksum_ipv6);

TEST(udp_checksum_ipv6_basic) {
	struct in6_addr s, d;
	memset(&s, 0, sizeof(s));
	memset(&d, 0, sizeof(d));
	s.s6_addr[15] = 1;
	d.s6_addr[15] = 2;

	uint8_t data[4] = {0x12, 0x34, 0x56, 0x78};
	uint16_t csum = udp_checksum_ipv6(s, d, data, 4);
	ASSERT_TRUE(csum != 0);
}

TEST(udp_checksum_ipv6_zero_length) {
	struct in6_addr s, d;
	memset(&s, 0, sizeof(s));
	memset(&d, 0, sizeof(d));

	uint16_t csum = udp_checksum_ipv6(s, d, NULL, 0);
	ASSERT_TRUE(csum != 0);
}

/* ============================================================================ */

TEST_SUITE(wire_format);

TEST(udp_hdr_wire_bytes) {
	struct udp_hdr h;
	memset(&h, 0, sizeof(h));
	h.source = htons(80);
	h.dest = htons(443);
	h.len = htons(8);
	h.check = 0;

	uint8_t *raw = (uint8_t *)&h;

	ASSERT_EQ(raw[0], 0x00);
	ASSERT_EQ(raw[1], 0x50);
	ASSERT_EQ(raw[2], 0x01);
	ASSERT_EQ(raw[3], 0xBB);
	ASSERT_EQ(raw[4], 0x00);
	ASSERT_EQ(raw[5], 0x08);
}

TEST(udphdr_wire_bytes) {
	struct udphdr h;
	memset(&h, 0, sizeof(h));
	h.uh_sport = htons(12345);
	h.uh_dport = htons(54321);
	h.uh_ulen = htons(20);
	h.uh_sum = 0;

	uint8_t *raw = (uint8_t *)&h;

	ASSERT_EQ(raw[0], 0x30);
	ASSERT_EQ(raw[1], 0x39);
	ASSERT_EQ(raw[2], 0xD4);
	ASSERT_EQ(raw[3], 0x31);
}

/* ============================================================================ */

TEST_SUITE(struct_compatibility);

TEST(udp_hdr_udphdr_size_match) {
	ASSERT_EQ(sizeof(struct udp_hdr), sizeof(struct udphdr));
}

TEST(udp_hdr_udphdr_offset_match) {
	ASSERT_EQ(offsetof(struct udp_hdr, source), offsetof(struct udphdr, uh_sport));
	ASSERT_EQ(offsetof(struct udp_hdr, dest), offsetof(struct udphdr, uh_dport));
	ASSERT_EQ(offsetof(struct udp_hdr, len), offsetof(struct udphdr, uh_ulen));
	ASSERT_EQ(offsetof(struct udp_hdr, check), offsetof(struct udphdr, uh_sum));
}

TEST(udp_hdr_udphdr_cast) {
	struct udp_hdr h1;
	memset(&h1, 0, sizeof(h1));
	h1.source = htons(8080);
	h1.dest = htons(9090);

	struct udphdr *h2 = (struct udphdr *)&h1;
	ASSERT_EQ(htons(8080), h2->uh_sport);
	ASSERT_EQ(htons(9090), h2->uh_dport);
}

/* ============================================================================ */

TEST_MAIN_IF(JACL_HAS_POSIX, "net/udp.h requires POSIX\n")
