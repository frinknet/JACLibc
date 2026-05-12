/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <string.h>
#include <net/icmp.h>

TEST_TYPE(unit);
TEST_UNIT(net/icmp.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_icmp_types) {
	ASSERT_EQ(0, ICMP_ECHOREPLY);
	ASSERT_EQ(3, ICMP_DEST_UNREACH);
	ASSERT_EQ(8, ICMP_ECHO);
	ASSERT_EQ(11, ICMP_TIME_EXCEEDED);
}

TEST(constants_icmp_unreach_codes) {
	ASSERT_EQ(0, ICMP_NET_UNREACH);
	ASSERT_EQ(1, ICMP_HOST_UNREACH);
	ASSERT_EQ(3, ICMP_PORT_UNREACH);
	ASSERT_EQ(4, ICMP_FRAG_NEEDED);
}

TEST(constants_icmp6_types) {
	ASSERT_EQ(1, ICMP6_DST_UNREACH);
	ASSERT_EQ(2, ICMP6_PACKET_TOO_BIG);
	ASSERT_EQ(128, ICMP6_ECHO_REQUEST);
	ASSERT_EQ(129, ICMP6_ECHO_REPLY);
	ASSERT_EQ(133, ICMP6_ND_ROUTER_SOLICIT);
	ASSERT_EQ(135, ICMP6_ND_NEIGHBOR_SOLICIT);
}

TEST(constants_nd_options) {
	ASSERT_EQ(1, ND_OPT_SOURCE_LINKADDR);
	ASSERT_EQ(2, ND_OPT_TARGET_LINKADDR);
	ASSERT_EQ(3, ND_OPT_PREFIX_INFORMATION);
	ASSERT_EQ(5, ND_OPT_MTU);
}

TEST(constants_nd_flags) {
	ASSERT_EQ(0x80, ND_RA_FLAG_MANAGED);
	ASSERT_EQ(0x40, ND_RA_FLAG_OTHER);
	ASSERT_EQ(0x80000000, ND_NA_FLAG_ROUTER);
	ASSERT_EQ(0x40000000, ND_NA_FLAG_SOLICITED);
}

TEST(constants_icmpv6_protocol) {
	ASSERT_EQ(58, IPPROTO_ICMPV6);
}

/* ============================================================================ */
TEST_SUITE(icmp_struct_sizes);

TEST(icmp_size) {
	ASSERT_SIZE(struct icmp, 8);
}

TEST(icmp6_hdr_size) {
	ASSERT_SIZE(struct icmp6_hdr, 8);
}

TEST(icmp_router_adv_size) {
	ASSERT_SIZE(struct icmp_router_adv, 8);
}

TEST(icmp_timestamp_size) {
	ASSERT_SIZE(struct icmp_timestamp, 20);
}

TEST(nd_neighbor_solicit_size) {
	ASSERT_SIZE(struct nd_neighbor_solicit, 24);
}

TEST(nd_neighbor_advert_size) {
	ASSERT_SIZE(struct nd_neighbor_advert, 28);  /* 8 + 4 + 16 */
}

TEST(nd_opt_prefix_info_size) {
	ASSERT_SIZE(struct nd_opt_prefix_info, 32);
}

TEST(nd_opt_mtu_size) {
	ASSERT_SIZE(struct nd_opt_mtu, 8);
}

/* ============================================================================ */
TEST_SUITE(icmp_layout);

TEST(icmp_field_offsets) {
	ASSERT_OFFSET(struct icmp, ic_type, 0);
	ASSERT_OFFSET(struct icmp, ic_code, 1);
	ASSERT_OFFSET(struct icmp, ic_cksum, 2);
}

TEST(icmp6_field_offsets) {
	ASSERT_OFFSET(struct icmp6_hdr, icmp6_type, 0);
	ASSERT_OFFSET(struct icmp6_hdr, icmp6_code, 1);
	ASSERT_OFFSET(struct icmp6_hdr, icmp6_cksum, 2);
}

TEST(icmp_id_macro_works) {
	struct icmp pkt;
	memset(&pkt, 0, sizeof(pkt));
	pkt.icmp_id = 0xBEEF;
	ASSERT_EQ(0xBEEF, pkt.icmp_id);
}

TEST(icmp6_id_macro_works) {
	struct icmp6_hdr pkt;
	memset(&pkt, 0, sizeof(pkt));
	pkt.icmp6_id = 0xDEAD;
	ASSERT_EQ(0xDEAD, pkt.icmp6_id);
}

/* ============================================================================ */
TEST_SUITE(icmp_checksum);

TEST(icmp_checksum_zero_buf) {
	uint8_t buf[8] = {0};
	ASSERT_EQ(0xFFFF, icmp_checksum(buf, 8));
}

TEST(icmp_checksum_echo_req) {
	uint8_t buf[8] = {8, 0, 0, 0, 0x12, 0x34, 0x56, 0x78};
	uint16_t csum = icmp_checksum(buf, 8);
	buf[2] = (csum >> 8) & 0xFF;
	buf[3] = csum & 0xFF;
	ASSERT_EQ(0x0000, icmp_checksum(buf, 8));
}

TEST(icmp_checksum_empty_buf) {
	ASSERT_EQ(0xFFFF, icmp_checksum(NULL, 0));
}

TEST(icmp_checksum_1_byte) {
	uint8_t buf[1] = {0xFF};
	ASSERT_EQ(0x00FF, icmp_checksum(buf, 1));
}

TEST(icmp_checksum_all_ones) {
	uint8_t buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	ASSERT_EQ(0x0000, icmp_checksum(buf, 4));
}

/* ============================================================================ */
TEST_SUITE(icmp6_checksum);

TEST(icmp6_checksum_basic) {
	struct in6_addr src, dst;
	memset(&src, 0, sizeof(src));
	memset(&dst, 0, sizeof(dst));
	src.s6_addr[15] = 1;
	dst.s6_addr[15] = 2;

	uint8_t buf[8] = {128, 0, 0, 0, 0x12, 0x34, 0x56, 0x78};
	uint16_t csum = icmp6_checksum(src, dst, buf, 8);

	ASSERT_TRUE(csum != 0);
	ASSERT_TRUE(csum != 0xFFFF);
}

TEST(icmp6_checksum_verify) {
	struct in6_addr src, dst;
	memset(&src, 0, sizeof(src));
	memset(&dst, 0, sizeof(dst));
	src.s6_addr[15] = 1;
	dst.s6_addr[15] = 2;

	uint8_t buf[8] = {128, 0, 0, 0, 0x12, 0x34, 0x56, 0x78};
	uint16_t csum = icmp6_checksum(src, dst, buf, 8);

	buf[2] = (csum >> 8) & 0xFF;
	buf[3] = csum & 0xFF;

	uint16_t verify = icmp6_checksum(src, dst, buf, 8);
	ASSERT_EQ(0, verify);  /* Verified checksum returns 0 */
}

TEST(icmp6_checksum_zero_length) {
	struct in6_addr src, dst;
	memset(&src, 0, sizeof(src));
	memset(&dst, 0, sizeof(dst));

	uint16_t csum = icmp6_checksum(src, dst, NULL, 0);
	/* Pseudo-header contributes, so NOT 0xFFFF */
	ASSERT_TRUE(csum != 0);  /* Just verify it doesn't crash */
}

TEST(icmp6_checksum_different_lengths) {
	struct in6_addr src, dst;
	memset(&src, 0, sizeof(src));
	memset(&dst, 0, sizeof(dst));

	uint8_t buf1[8] = {0};
	uint8_t buf2[16] = {0};

	uint16_t csum1 = icmp6_checksum(src, dst, buf1, 8);
	uint16_t csum2 = icmp6_checksum(src, dst, buf2, 16);

	ASSERT_TRUE(csum1 != csum2);
}

/* ============================================================================ */
TEST_SUITE(nd_messages);

TEST(nd_neighbor_solicit_layout) {
	struct nd_neighbor_solicit ns;
	memset(&ns, 0, sizeof(ns));
	ns.nd_ns_hdr.icmp6_type = ICMP6_ND_NEIGHBOR_SOLICIT;
	ns.nd_ns_target.s6_addr[15] = 0x42;

	ASSERT_EQ(ICMP6_ND_NEIGHBOR_SOLICIT, ns.nd_ns_hdr.icmp6_type);
	ASSERT_EQ(0x42, ns.nd_ns_target.s6_addr[15]);
	ASSERT_OFFSET(struct nd_neighbor_solicit, nd_ns_target, 8);
}

TEST(nd_neighbor_advert_flags) {
	struct nd_neighbor_advert na;
	memset(&na, 0, sizeof(na));
	na.nd_na_flags_reserved = htonl(ND_NA_FLAG_SOLICITED);

	ASSERT_EQ(ND_NA_FLAG_SOLICITED, ntohl(na.nd_na_flags_reserved));
}

TEST(nd_router_advert_layout) {
	struct nd_router_advert ra;
	memset(&ra, 0, sizeof(ra));
	ra.nd_ra_curhoplimit = 64;
	ra.nd_ra_router_lifetime = 300;

	ASSERT_EQ(64, ra.nd_ra_curhoplimit);
	ASSERT_EQ(300, ra.nd_ra_router_lifetime);
}

/* ============================================================================ */
TEST_SUITE(posix_compliance);

TEST(posix_struct_icmp_exists) {
	struct icmp pkt;
	(void)pkt;
	ASSERT_TRUE(1);
}

TEST(posix_struct_icmp6_hdr_exists) {
	struct icmp6_hdr pkt;
	(void)pkt;
	ASSERT_TRUE(1);
}

TEST(posix_icmp_type_field) {
	struct icmp pkt;
	pkt.ic_type = ICMP_ECHO;
	ASSERT_EQ(ICMP_ECHO, pkt.ic_type);
}

TEST(posix_icmp6_type_field) {
	struct icmp6_hdr pkt;
	pkt.icmp6_type = ICMP6_ECHO_REQUEST;
	ASSERT_EQ(ICMP6_ECHO_REQUEST, pkt.icmp6_type);
}

/* ============================================================================ */
/* SAD PATH / SECURITY TESTS */
/* ============================================================================ */

TEST_SUITE(icmp_checksum_sad_paths);

TEST(icmp_checksum_odd_byte_padding) {
	/* Odd byte IS included in checksum (padded with zero on right) */
	uint8_t buf1[3] = {0xFF, 0xFF, 0xFF};
	uint8_t buf2[3] = {0xFF, 0xFF, 0x00};

	/* These produce DIFFERENT checksums (odd byte matters) */
	uint16_t csum1 = icmp_checksum(buf1, 3);
	uint16_t csum2 = icmp_checksum(buf2, 3);

	ASSERT_TRUE(csum1 != csum2);  /* Fixed: they SHOULD differ */
	ASSERT_EQ(0x00FF, csum1);     /* Optional: verify exact values */
	ASSERT_EQ(0x0000, csum2);
}

TEST(icmp_checksum_carry_fold) {
	/* Verify carry bits are folded correctly */
	uint8_t buf[8];
	memset(buf, 0xFF, sizeof(buf));

	uint16_t csum = icmp_checksum(buf, 8);
	ASSERT_EQ(0x0000, csum);  /* All ones = 0 after complement */
}

TEST(icmp_checksum_large_packet) {
	/* Test with larger packet (simulating real ICMP payload) */
	uint8_t buf[64];
	memset(buf, 0xAA, sizeof(buf));

	uint16_t csum = icmp_checksum(buf, sizeof(buf));
	ASSERT_TRUE(csum != 0);
}

TEST(icmp_checksum_single_word) {
	uint8_t buf[2] = {0x12, 0x34};
	uint16_t csum = icmp_checksum(buf, 2);
	ASSERT_TRUE(csum != 0);
	ASSERT_TRUE(csum != 0xFFFF);
}

TEST(icmp_checksum_null_buffer_zero_len) {
	/* NULL buffer with zero length should not crash */
	ASSERT_EQ(0xFFFF, icmp_checksum(NULL, 0));
}

/* ============================================================================ */

TEST_SUITE(icmp6_checksum_sad_paths);

TEST(icmp6_checksum_null_buffer_zero_len) {
	struct in6_addr src, dst;
	memset(&src, 0, sizeof(src));
	memset(&dst, 0, sizeof(dst));

	/* Should not crash, but pseudo-header contributes */
	uint16_t csum = icmp6_checksum(src, dst, NULL, 0);
	ASSERT_TRUE(csum != 0);  /* Fixed expectation */
}

TEST(icmp6_checksum_same_address) {
	/* Source = Dest (loopback scenario) */
	struct in6_addr addr;
	memset(&addr, 0, sizeof(addr));
	addr.s6_addr[15] = 1;

	uint8_t buf[8] = {128, 0, 0, 0, 0, 0, 0, 1};
	uint16_t csum = icmp6_checksum(addr, addr, buf, 8);

	ASSERT_TRUE(csum != 0);
}

TEST(icmp6_checksum_all_zeros) {
	struct in6_addr src, dst;
	memset(&src, 0, sizeof(src));
	memset(&dst, 0, sizeof(dst));

	uint8_t buf[8] = {0};
	uint16_t csum = icmp6_checksum(src, dst, buf, 8);

	ASSERT_TRUE(csum != 0);  /* Pseudo-header contributes to sum */
}

TEST(icmp6_checksum_maximum_length) {
	struct in6_addr src, dst;
	memset(&src, 0, sizeof(src));
	memset(&dst, 0, sizeof(dst));

	uint8_t buf[1280];  /* IPv6 minimum MTU */
	memset(buf, 0, sizeof(buf));

	uint16_t csum = icmp6_checksum(src, dst, buf, sizeof(buf));
	ASSERT_TRUE(csum != 0);
}

TEST(icmp6_checksum_odd_length) {
	struct in6_addr src, dst;
	memset(&src, 0, sizeof(src));
	memset(&dst, 0, sizeof(dst));

	uint8_t buf[7] = {128, 0, 0, 0, 0x12, 0x34, 0x56};
	uint16_t csum = icmp6_checksum(src, dst, buf, 7);

	ASSERT_TRUE(csum != 0);
}

/* ============================================================================ */

TEST_SUITE(icmp_type_code_validation);

TEST(icmp_type_reserved_values) {
	/* Types 6, 7 are reserved in RFC 792 */
	struct icmp pkt;
	memset(&pkt, 0, sizeof(pkt));

	pkt.ic_type = 6;  /* Reserved */
	ASSERT_EQ(6, pkt.ic_type);  /* Header accepts any value */

	pkt.ic_type = 7;  /* Reserved */
	ASSERT_EQ(7, pkt.ic_type);
}

TEST(icmp_code_out_of_range) {
	/* Codes are type-dependent; header doesn't validate */
	struct icmp pkt;
	memset(&pkt, 0, sizeof(pkt));

	pkt.ic_type = ICMP_ECHO;
	pkt.ic_code = 255;  /* Invalid for ECHO (should be 0) */
	ASSERT_EQ(255, pkt.ic_code);  /* Header accepts any value */
}

TEST(icmp6_type_boundary) {
	struct icmp6_hdr pkt;
	memset(&pkt, 0, sizeof(pkt));

	/* Minimum ICMPv6 type (error messages) */
	pkt.icmp6_type = 0;
	ASSERT_EQ(0, pkt.icmp6_type);

	/* Maximum standard ICMPv6 type */
	pkt.icmp6_type = 255;
	ASSERT_EQ(255, pkt.icmp6_type);
}

/* ============================================================================ */

TEST_SUITE(nd_option_validation);

TEST(nd_opt_hdr_minimum_size) {
	/* ND options must be at least 2 bytes (type + length) */
	ASSERT_SIZE(struct nd_opt_hdr, 2);

	struct nd_opt_hdr opt;
	opt.nd_opt_type = ND_OPT_SOURCE_LINKADDR;
	opt.nd_opt_len = 1;  /* 8 bytes total */

	ASSERT_EQ(ND_OPT_SOURCE_LINKADDR, opt.nd_opt_type);
	ASSERT_EQ(1, opt.nd_opt_len);
}

TEST(nd_opt_prefix_info_flags) {
	struct nd_opt_prefix_info pi;
	memset(&pi, 0, sizeof(pi));

	pi.nd_opt_pi_flags_reserved = ND_OPT_PI_FLAG_ONLINK | ND_OPT_PI_FLAG_AUTO;

	ASSERT_EQ(ND_OPT_PI_FLAG_ONLINK | ND_OPT_PI_FLAG_AUTO,
	          pi.nd_opt_pi_flags_reserved);
}

TEST(nd_opt_mtu_minimum_value) {
	struct nd_opt_mtu mtu;
	memset(&mtu, 0, sizeof(mtu));

	mtu.nd_opt_mtu_mtu = htonl(1280);  /* IPv6 minimum MTU */

	ASSERT_EQ(1280, ntohl(mtu.nd_opt_mtu_mtu));
}

TEST(nd_ra_flags_combination) {
	struct nd_router_advert ra;
	memset(&ra, 0, sizeof(ra));

	/* Valid flag combinations */
	ra.nd_ra_flags_reserved = ND_RA_FLAG_MANAGED;
	ASSERT_EQ(ND_RA_FLAG_MANAGED, ra.nd_ra_flags_reserved);

	ra.nd_ra_flags_reserved = ND_RA_FLAG_OTHER;
	ASSERT_EQ(ND_RA_FLAG_OTHER, ra.nd_ra_flags_reserved);

	ra.nd_ra_flags_reserved = ND_RA_FLAG_MANAGED | ND_RA_FLAG_OTHER;
	ASSERT_EQ(ND_RA_FLAG_MANAGED | ND_RA_FLAG_OTHER, ra.nd_ra_flags_reserved);
}

/* ============================================================================ */

TEST_SUITE(struct_packing_verification);

TEST(icmp_no_padding_between_fields) {
	/* Verify no unexpected padding */
	ASSERT_OFFSET(struct icmp, ic_type, 0);
	ASSERT_OFFSET(struct icmp, ic_code, 1);
	ASSERT_OFFSET(struct icmp, ic_cksum, 2);
	ASSERT_OFFSET(struct icmp, icmp_hun, 4);
	ASSERT_SIZE(struct icmp, 8);  /* 1 + 1 + 2 + 4 = 8 */
}

TEST(icmp6_hdr_no_padding) {
	ASSERT_OFFSET(struct icmp6_hdr, icmp6_type, 0);
	ASSERT_OFFSET(struct icmp6_hdr, icmp6_code, 1);
	ASSERT_OFFSET(struct icmp6_hdr, icmp6_cksum, 2);
	ASSERT_OFFSET(struct icmp6_hdr, icmp6_dataun, 4);
	ASSERT_SIZE(struct icmp6_hdr, 8);  /* 1 + 1 + 2 + 4 = 8 */
}

TEST(nd_neighbor_solicit_packing) {
	/* 8 (hdr) + 16 (target) = 24 */
	ASSERT_SIZE(struct nd_neighbor_solicit, 24);
	ASSERT_OFFSET(struct nd_neighbor_solicit, nd_ns_target, 8);
}

TEST(nd_neighbor_advert_packing) {
	/* 8 (hdr) + 4 (flags) + 16 (target) = 28 */
	ASSERT_SIZE(struct nd_neighbor_advert, 28);
	ASSERT_OFFSET(struct nd_neighbor_advert, nd_na_flags_reserved, 8);
	ASSERT_OFFSET(struct nd_neighbor_advert, nd_na_target, 12);
}

TEST(nd_redirect_packing) {
	/* 8 (hdr) + 16 (target) + 16 (dst) = 40 */
	ASSERT_SIZE(struct nd_redirect, 40);
	ASSERT_OFFSET(struct nd_redirect, nd_rd_target, 8);
	ASSERT_OFFSET(struct nd_redirect, nd_rd_dst, 24);
}

/* ============================================================================ */

TEST_SUITE(buffer_boundary_tests);

TEST(icmp_echo_exact_buffer) {
	/* Exact-size buffer for echo request */
	uint8_t buf[8];
	struct icmp *pkt = (struct icmp *)buf;

	pkt->ic_type = ICMP_ECHO;
	pkt->icmp_id = 0x1234;
	pkt->icmp_seq = 0x5678;

	ASSERT_EQ(ICMP_ECHO, pkt->ic_type);
	ASSERT_EQ(0x1234, pkt->icmp_id);
}

TEST(icmp6_echo_exact_buffer) {
	uint8_t buf[8];
	struct icmp6_hdr *pkt = (struct icmp6_hdr *)buf;

	pkt->icmp6_type = ICMP6_ECHO_REQUEST;
	pkt->icmp6_id = 0xABCD;
	pkt->icmp6_seq = 0xEF01;

	ASSERT_EQ(ICMP6_ECHO_REQUEST, pkt->icmp6_type);
	ASSERT_EQ(0xABCD, pkt->icmp6_id);
}

TEST(nd_ns_exact_buffer) {
	uint8_t buf[24];
	struct nd_neighbor_solicit *ns = (struct nd_neighbor_solicit *)buf;

	ns->nd_ns_hdr.icmp6_type = ICMP6_ND_NEIGHBOR_SOLICIT;

	ASSERT_EQ(ICMP6_ND_NEIGHBOR_SOLICIT, ns->nd_ns_hdr.icmp6_type);
}

/* ============================================================================ */

TEST_SUITE(union_member_overlap);

TEST(icmp_union_echo_frag_overlap) {
	/* Verify union members overlap correctly */
	struct icmp pkt;
	memset(&pkt, 0, sizeof(pkt));

	/* Set echo fields */
	pkt.icmp_hun.ic_echo.icd_id = 0xBEEF;
	pkt.icmp_hun.ic_echo.icd_seq = 0xCAFE;

	/* Verify via frag view (same memory) */
	ASSERT_EQ(0xBEEF, pkt.icmp_hun.ic_frag.icd_pad);
	ASSERT_EQ(0xCAFE, pkt.icmp_hun.ic_frag.icd_mtu);
}

TEST(icmp6_union_pptr_mtu_echo_overlap) {
	struct icmp6_hdr pkt;
	memset(&pkt, 0, sizeof(pkt));

	/* Set via pptr */
	pkt.icmp6_pptr = 0xDEADBEEF;

	/* Verify via mtu (same memory) */
	ASSERT_EQ(0xDEADBEEF, pkt.icmp6_mtu);
}

/* ============================================================================ */

TEST_SUITE(endianness_verification);

TEST(icmp_checksum_network_order) {
	/* Checksum should be independent of host byte order */
	uint8_t buf[4] = {0x00, 0x01, 0x00, 0x02};
	uint16_t csum = icmp_checksum(buf, 4);

	ASSERT_TRUE(csum != 0);
}

TEST(icmp6_checksum_network_order) {
	struct in6_addr src, dst;
	memset(&src, 0, sizeof(src));
	memset(&dst, 0, sizeof(dst));
	src.s6_addr[15] = 1;
	dst.s6_addr[15] = 2;

	uint8_t buf[8] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01};
	uint16_t csum = icmp6_checksum(src, dst, buf, 8);

	ASSERT_TRUE(csum != 0);
}

TEST(nd_flags_network_order) {
	struct nd_neighbor_advert na;
	memset(&na, 0, sizeof(na));

	/* Flags are in network byte order */
	na.nd_na_flags_reserved = htonl(ND_NA_FLAG_SOLICITED);

	ASSERT_EQ(ND_NA_FLAG_SOLICITED, ntohl(na.nd_na_flags_reserved));
}

/* ============================================================================ */

TEST_MAIN_IF(JACL_HAS_POSIX, "net/icmp.h requires POSIX\n")
