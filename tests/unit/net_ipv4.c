/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <net/ipv4.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(net/ipv4.h);

/* ============================================================================ */
/* CONSTANTS                                                                    */
/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_version) {
	ASSERT_EQ(IPV4_HDR_LEN, 20);
	ASSERT_EQ(IP_VERSION_4, 4);
	ASSERT_EQ(IPVERSION, 4);
}

TEST(constants_fragmentation) {
	ASSERT_EQ(IP_RF, 0x8000);
	ASSERT_EQ(IP_DF, 0x4000);
	ASSERT_EQ(IP_MF, 0x2000);
	ASSERT_EQ(IP_OFFMASK, 0x1fff);
}

TEST(constants_packet_limits) {
	ASSERT_EQ(MAXTTL, 255);
	ASSERT_EQ(IPDEFTTL, 64);
	ASSERT_EQ(IPFRAGTTL, 60);
	ASSERT_EQ(IPTTLDEC, 1);
	ASSERT_EQ(IP_MSS, 576);
	ASSERT_EQ(IP_MAXPACKET, 65535);
}

TEST(constants_protocols) {
	ASSERT_EQ(IPPROTO_ICMP, 1);
	ASSERT_EQ(IPPROTO_TCP, 6);
	ASSERT_EQ(IPPROTO_UDP, 17);
}

TEST(constants_iptos) {
	ASSERT_EQ(IPTOS_LOWDELAY, 0x10);
	ASSERT_EQ(IPTOS_THROUGHPUT, 0x08);
	ASSERT_EQ(IPTOS_RELIABILITY, 0x04);
	ASSERT_EQ(IPTOS_LOWCOST, 0x02);
	ASSERT_EQ(IPTOS_MINCOST, IPTOS_LOWCOST);
	ASSERT_EQ(IPTOS_PREC_NETCONTROL, 0xe0);
	ASSERT_EQ(IPTOS_PREC_INTERNETCONTROL, 0xc0);
	ASSERT_EQ(IPTOS_PREC_FLASHOVERRIDE, 0x80);
	ASSERT_EQ(IPTOS_PREC_ROUTINE, 0x00);
}

/* ============================================================================ */
/* MACRO: IPTOS_ECN                                                             */
/* ============================================================================ */
TEST_SUITE(iptos_ecn);
TEST(iptos_ecn_constants) {
	ASSERT_EQ(IPTOS_ECN_MASK, 0x03);
	ASSERT_EQ(IPTOS_ECN_NOT_ECT, 0x00);
	ASSERT_EQ(IPTOS_ECN_ECT1, 0x01);
	ASSERT_EQ(IPTOS_ECN_ECT0, 0x02);
	ASSERT_EQ(IPTOS_ECN_CE, 0x03);
}
TEST(iptos_ecn_mask_isolation) {
	uint8_t mixed = 0xFC | IPTOS_ECN_CE; // Upper 6 bits saturated
	ASSERT_EQ(IPTOS_ECN(mixed), IPTOS_ECN_CE);
}
TEST(iptos_ecn_extract_pure) {
	ASSERT_EQ(IPTOS_ECN(IPTOS_ECN_ECT1), IPTOS_ECN_ECT1);
}
TEST(iptos_ecn_boundary) {
	ASSERT_EQ(IPTOS_ECN(0xFF), IPTOS_ECN_CE); // Max byte truncates cleanly to mask
}

/* ============================================================================ */
/* MACRO: IPTOS_DSCP                                                            */
/* ============================================================================ */
TEST_SUITE(iptos_dscp);
TEST(iptos_dscp_constants) {
	ASSERT_EQ(IPTOS_DSCP_MASK, 0xfc);
	ASSERT_EQ(IPTOS_DSCP_AF11, 0x28); ASSERT_EQ(IPTOS_DSCP_AF12, 0x30);
	ASSERT_EQ(IPTOS_DSCP_AF13, 0x38); ASSERT_EQ(IPTOS_DSCP_AF21, 0x48);
	ASSERT_EQ(IPTOS_DSCP_AF22, 0x50); ASSERT_EQ(IPTOS_DSCP_AF23, 0x58);
	ASSERT_EQ(IPTOS_DSCP_AF31, 0x68); ASSERT_EQ(IPTOS_DSCP_AF32, 0x70);
	ASSERT_EQ(IPTOS_DSCP_AF33, 0x78); ASSERT_EQ(IPTOS_DSCP_AF41, 0x88);
	ASSERT_EQ(IPTOS_DSCP_AF42, 0x90); ASSERT_EQ(IPTOS_DSCP_AF43, 0x98);
	ASSERT_EQ(IPTOS_DSCP_EF, 0xb8);
}
TEST(iptos_dscp_mask_isolation) {
	uint8_t mixed = IPTOS_ECN_MASK | IPTOS_DSCP_EF; // ECN bits saturated
	ASSERT_EQ(IPTOS_DSCP(mixed), IPTOS_DSCP_EF);
}
TEST(iptos_dscp_extract_mixed) {
	uint8_t tos = IPTOS_DSCP_AF23 | IPTOS_ECN_CE;
	ASSERT_EQ(IPTOS_DSCP(tos), IPTOS_DSCP_AF23);
}
TEST(iptos_dscp_shift_verify) {
	/* DSCP occupies upper 6 bits. Verify alignment survives right-shift */
	ASSERT_EQ((IPTOS_DSCP_AF41 >> 2), 0x22);
}

/* ============================================================================ */
/* MACRO: IPTOS_CLASS                                                           */
/* ============================================================================ */
TEST_SUITE(iptos_class);
TEST(iptos_class_constants) {
	ASSERT_EQ(IPTOS_CLASS_MASK, 0xe0);
	ASSERT_EQ(IPTOS_CLASS_CS0, 0x00); ASSERT_EQ(IPTOS_CLASS_CS1, 0x20);
	ASSERT_EQ(IPTOS_CLASS_CS2, 0x40); ASSERT_EQ(IPTOS_CLASS_CS3, 0x60);
	ASSERT_EQ(IPTOS_CLASS_CS4, 0x80); ASSERT_EQ(IPTOS_CLASS_CS5, 0xa0);
	ASSERT_EQ(IPTOS_CLASS_CS6, 0xc0); ASSERT_EQ(IPTOS_CLASS_CS7, 0xe0);
}
TEST(iptos_class_mask_isolation) {
	uint8_t mixed = 0x1F | IPTOS_CLASS_CS6; // Lower 5 bits saturated
	ASSERT_EQ(IPTOS_CLASS(mixed), IPTOS_CLASS_CS6);
}
TEST(iptos_class_extract_mixed) {
	uint8_t tos = IPTOS_CLASS_CS4 | IPTOS_LOWDELAY | IPTOS_THROUGHPUT;
	ASSERT_EQ(IPTOS_CLASS(tos), IPTOS_CLASS_CS4);
}
TEST(iptos_class_dscp_overlap) {
	/* Class and DSCP share top 3 bits. Verify mapping holds across specs */
	ASSERT_EQ(IPTOS_CLASS(IPTOS_DSCP_AF41), IPTOS_CLASS_CS4);
}

/* ============================================================================ */
/* CONSTANTS: IP OPTIONS (Macro/Constraint Focus)                               */
/* ============================================================================ */
TEST_SUITE(ip_options);
TEST(ip_options_constants) {
	ASSERT_EQ(IPOPT_EOL, 0); ASSERT_EQ(IPOPT_NOP, 1);
	ASSERT_EQ(IPOPT_RR, 7); ASSERT_EQ(IPOPT_TS, 68);
	ASSERT_EQ(IPOPT_SECURITY, 130); ASSERT_EQ(IPOPT_LSRR, 131);
	ASSERT_EQ(IPOPT_SSRR, 137); ASSERT_EQ(IPOPT_RA, 148);
	ASSERT_EQ(MAX_IPOPTLEN, 40); ASSERT_EQ(IPOPT_MINOFF, 4);
}
TEST(ip_options_eol_nop_identity) {
	ASSERT_EQ(IPOPT_EOL, 0); ASSERT_EQ(IPOPT_NOP, 1);
	ASSERT_TRUE(IPOPT_EOL < IPOPT_NOP); // Termination precedes padding
}
TEST(ip_options_length_bounds) {
	/* Valid options (kind+length+data) must never exceed header limit */
	ASSERT_TRUE(MAX_IPOPTLEN >= IPOPT_MINOFF);
}
TEST(ip_options_kind_range) {
	/* Standard options live in low byte range; verify classification */
	ASSERT_TRUE(IPOPT_RR < 128);
	ASSERT_TRUE(IPOPT_TS > 60 && IPOPT_TS < 80); // Timestamp class
}
TEST(ip_options_wire_overlap_safety) {
	/* EOL and NOP occupy single bytes; others require >=2. Verify no collision */
	ASSERT_TRUE(IPOPT_EOL != IPOPT_NOP);
	ASSERT_TRUE(IPOPT_RR != IPOPT_TS);
}

/* ============================================================================ */
/* CONSTANTS: LEGACY TYPES                                                      */
/* ============================================================================ */
TEST_SUITE(ip_types);
TEST(ip_types_n_short) { ASSERT_SIZE_SAME(n_short, uint16_t); }
TEST(ip_types_n_long) { ASSERT_SIZE_SAME(n_long, uint32_t); }
TEST(ip_types_n_time) { ASSERT_SIZE_SAME(n_time, uint32_t); }

/* ============================================================================ */
/* MACRO: IPV4_VERS_IHL & HELPERS                                               */
/* ============================================================================ */
TEST_SUITE(IPV4_VERS_IHL);
TEST(IPV4_VERS_IHL_construct) { ASSERT_EQ(IPV4_VERS_IHL(4, 5), 0x45); }
TEST(IPV4_VERS_IHL_mask_hl) { ASSERT_EQ(IPV4_VERS_IHL(6, 15), 0x6F); }
TEST(IPV4_VERSION_extract) { ASSERT_EQ(IPV4_VERSION(0x45), 4); }
TEST(IPV4_IHL_extract) { ASSERT_EQ(IPV4_IHL(0x45), 5); }

/* ============================================================================ */
/* STRUCT: ipv4_hdr                                                             */
/* ============================================================================ */
TEST_SUITE(ipv4_hdr);
TEST(ipv4_hdr_size) { ASSERT_EQ(sizeof(struct ipv4_hdr), 20); }
TEST(ipv4_hdr_layout) {
	struct ipv4_hdr h; memset(&h, 0, sizeof(h));
	h.version = 4; h.ihl = 5; h.tos = 0x10;
	h.tot_len = htons(64); h.id = htons(0x1234);
	h.frag_off = htons(IP_DF); h.ttl = 64;
	h.protocol = IPPROTO_UDP; h.check = 0;
	h.saddr = htonl(0xC0A80001); h.daddr = htonl(0xC0A80002);
	ASSERT_EQ(h.version, 4); ASSERT_EQ(h.ihl, 5);
	ASSERT_EQ(h.tos, 0x10); ASSERT_EQ(ntohs(h.tot_len), 64);
	ASSERT_EQ(h.protocol, IPPROTO_UDP);
}
TEST(ipv4_hdr_offsets) {
	ASSERT_OFFSET(struct ipv4_hdr, tos, 1);
	ASSERT_OFFSET(struct ipv4_hdr, tot_len, 2);
	ASSERT_OFFSET(struct ipv4_hdr, id, 4);
	ASSERT_OFFSET(struct ipv4_hdr, frag_off, 6);
	ASSERT_OFFSET(struct ipv4_hdr, ttl, 8);
	ASSERT_OFFSET(struct ipv4_hdr, protocol, 9);
	ASSERT_OFFSET(struct ipv4_hdr, check, 10);
	ASSERT_OFFSET(struct ipv4_hdr, saddr, 12);
	ASSERT_OFFSET(struct ipv4_hdr, daddr, 16);
}
TEST(ipv4_hdr_wire_format) {
	struct ipv4_hdr h; memset(&h, 0, sizeof(h));
	h.version = 4; h.ihl = 5; h.tos = 0x10;
	h.tot_len = htons(64); h.id = htons(0x1234);
	h.frag_off = htons(IP_DF); h.ttl = 64;
	h.protocol = IPPROTO_UDP;
	h.saddr = htonl(0xC0A80001); h.daddr = htonl(0xC0A80002);
	uint8_t *w = (uint8_t *)&h;
	ASSERT_EQ(w[0], 0x45); ASSERT_EQ(w[1], 0x10);
	ASSERT_EQ(w[2], 0x00); ASSERT_EQ(w[3], 0x40);
	ASSERT_EQ(w[4], 0x12); ASSERT_EQ(w[5], 0x34);
	ASSERT_EQ(w[6], 0x40); ASSERT_EQ(w[7], 0x00);
	ASSERT_EQ(w[8], 64); ASSERT_EQ(w[9], IPPROTO_UDP);
	ASSERT_EQ(w[12], 0xC0); ASSERT_EQ(w[13], 0xA8);
}
TEST(ipv4_hdr_alignment) { ASSERT_SIZE_ALIGNS(struct ipv4_hdr, uint32_t); }

/* ============================================================================ */
/* STRUCT: ip                                                                   */
/* ============================================================================ */
TEST_SUITE(ip);
TEST(ip_size) { ASSERT_SIZE_SAME(struct ipv4_hdr, struct ip); }
TEST(ip_layout) {
	struct ip pkt; memset(&pkt, 0, sizeof(pkt));
	pkt.ip_v = 4; pkt.ip_hl = 5; pkt.ip_p = IPPROTO_UDP;
	ASSERT_EQ(pkt.ip_v, 4); ASSERT_EQ(pkt.ip_hl, 5);
	ASSERT_EQ(pkt.ip_p, IPPROTO_UDP);
}
TEST(ip_offsets) {
	ASSERT_OFFSET(struct ip, ip_tos, 1);
	ASSERT_OFFSET(struct ip, ip_len, 2);
	ASSERT_OFFSET(struct ip, ip_id, 4);
	ASSERT_OFFSET(struct ip, ip_off, 6);
	ASSERT_OFFSET(struct ip, ip_ttl, 8);
	ASSERT_OFFSET(struct ip, ip_p, 9);
	ASSERT_OFFSET(struct ip, ip_sum, 10);
}

/* ============================================================================ */
/* FUNCTION: ipv4_checksum                                                      */
/* ============================================================================ */
TEST_SUITE(ipv4_checksum);
TEST(ipv4_checksum_zero) {
	uint8_t buf[20] = {0};
	ASSERT_EQ(ipv4_checksum(buf, 20), 0xFFFF);
}
TEST(ipv4_checksum_basic) {
	uint8_t buf[20] = {
		0x45, 0x00, 0x00, 0x14, 0x00, 0x00, 0x40, 0x00,
		64, IPPROTO_UDP, 0x00, 0x00,
		0xc0, 0xa8, 0x01, 0x01, 0xc0, 0xa8, 0x01, 0x02
	};
	ASSERT_TRUE(ipv4_checksum(buf, 20) != 0);
}
TEST(ipv4_checksum_rfc1071) {
	uint8_t buf[20] = {
		0x45, 0x00, 0x00, 0x14, 0x00, 0x00, 0x40, 0x00,
		64, IPPROTO_UDP, 0x00, 0x00,
		0xc0, 0xa8, 0x01, 0x01, 0xc0, 0xa8, 0x01, 0x02
	};
	uint16_t csum = ipv4_checksum(buf, 20);
	buf[10] = (csum >> 8) & 0xFF;
	buf[11] = csum & 0xFF;
	ASSERT_EQ(ipv4_checksum(buf, 20), 0);
}
TEST(ipv4_checksum_odd_len) {
	uint8_t buf[21] = {0}; buf[20] = 0xFF;
	ASSERT_TRUE(ipv4_checksum(buf, 21) != ipv4_checksum(buf, 20));
}

TEST_MAIN()

#else
int main(void) { printf("net/ipv4.h requires POSIX\n"); return 0; }
#endif
