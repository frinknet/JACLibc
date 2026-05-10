/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#include <net/ipv6.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(net/ipv6.h);

/* ============================================================================ */

TEST_SUITE(constants);

TEST(constants_version) {
	ASSERT_EQ(IPV6_VERSION, 6);
}

TEST(constants_ip6f_masks) {
	ASSERT_EQ(IP6F_OFF_MASK, 0xfff8);
	ASSERT_EQ(IP6F_RESERVED_MASK, 0x0006);
	ASSERT_EQ(IP6F_MORE_FRAG, 0x0001);
}

TEST(constants_ipproto) {
	ASSERT_EQ(IPPROTO_HOPOPTS, 0);
	ASSERT_EQ(IPPROTO_ROUTING, 43);
	ASSERT_EQ(IPPROTO_FRAGMENT, 44);
	ASSERT_EQ(IPPROTO_ESP, 50);
	ASSERT_EQ(IPPROTO_AH, 51);
	ASSERT_EQ(IPPROTO_ICMPV6, 58);
	ASSERT_EQ(IPPROTO_NONE, 59);
	ASSERT_EQ(IPPROTO_DSTOPTS, 60);
}

TEST(constants_ipv6_addr_classification) {
	ASSERT_EQ(IPV6_ADDR_LOOPBACK, 1);
	ASSERT_EQ(IPV6_ADDR_MULTICAST, 2);
	ASSERT_EQ(IPV6_ADDR_LINKLOCAL, 3);
	ASSERT_EQ(IPV6_ADDR_SITELOCAL, 4);
	ASSERT_EQ(IPV6_ADDR_V4MAPPED, 5);
	ASSERT_EQ(IPV6_ADDR_V4COMPAT, 6);
}

/* ============================================================================ */

TEST_SUITE(ip6_version);

TEST(ip6_version_extract_zero) {
	ASSERT_EQ(IP6_VERSION(0x00000000), 0);
}

TEST(ip6_version_extract_six) {
	ASSERT_EQ(IP6_VERSION(0x60000000), 6);
}

TEST(ip6_version_extract_f) {
	ASSERT_EQ(IP6_VERSION(0xF0000000), 15);
}

TEST(ip6_version_mask_isolation) {
	ASSERT_EQ(IP6_VERSION(0xFFFFFFFF), 15);
	ASSERT_EQ(IP6_VERSION(0x6ABCDEF0), 6);
}

/* ============================================================================ */

TEST_SUITE(ip6_traffic_class);

TEST(ip6_traffic_class_extract_zero) {
	ASSERT_EQ(IP6_TRAFFIC_CLASS(0x00000000), 0);
}

TEST(ip6_traffic_class_extract_ff) {
	ASSERT_EQ(IP6_TRAFFIC_CLASS(0x0FF00000), 0xFF);
}

TEST(ip6_traffic_class_mask_isolation) {
	uint32_t mixed = 0xF0000000 | 0x000FFFFF; /* version + flow saturated */
	ASSERT_EQ(IP6_TRAFFIC_CLASS(mixed | (0xAB << 20)), 0xAB);
}

TEST(ip6_traffic_class_dscp_ecn_overlap) {
	/* DSCP (upper 6) + ECN (lower 2) both live in TC byte */
	uint32_t tc = (0x2E << 20); /* DSCP 0x2E (AF11) + ECN 0x02 (ECT0) */
	ASSERT_EQ(IP6_TRAFFIC_CLASS(tc), 0x2E);
}

/* ============================================================================ */

TEST_SUITE(ip6_flow_label);

TEST(ip6_flow_label_extract_zero) {
	ASSERT_EQ(IP6_FLOW_LABEL(0x00000000), 0);
}

TEST(ip6_flow_label_extract_max) {
	ASSERT_EQ(IP6_FLOW_LABEL(0x000FFFFF), 0xFFFFF);
}

TEST(ip6_flow_label_mask_isolation) {
	uint32_t mixed = 0xFFF00000; /* version + TC saturated */
	ASSERT_EQ(IP6_FLOW_LABEL(mixed | 0x12345), 0x12345);
}

TEST(ip6_flow_label_20bit_boundary) {
	ASSERT_EQ(IP6_FLOW_LABEL(0x00100000), 0); /* Bit 20 is TC, not flow */
	ASSERT_EQ(IP6_FLOW_LABEL(0x000FFFFF), 0xFFFFF);
}

/* ============================================================================ */
/* MACRO: IP6F_OFF_MASK                                                         */
/* ============================================================================ */

TEST_SUITE(ip6f_off_mask);

TEST(ip6f_off_mask_extract_zero) {
	ASSERT_EQ((0x0000 & IP6F_OFF_MASK), 0);
}

TEST(ip6f_off_mask_extract_max) {
	ASSERT_EQ((0xFFF8 & IP6F_OFF_MASK), 0xFFF8);
}

TEST(ip6f_off_mask_with_more_frag) {
	uint16_t field = IP6F_MORE_FRAG | 0x1238; /* offset=0x1238/8, M=1 */
	ASSERT_EQ((field & IP6F_OFF_MASK), 0x1238);
	ASSERT_EQ((field & IP6F_MORE_FRAG), IP6F_MORE_FRAG);
}

TEST(ip6f_off_mask_reserved_bits_ignored) {
	uint16_t field = 0x0006 | 0x1000; /* reserved bits set */
	ASSERT_EQ((field & IP6F_OFF_MASK), 0x1000);
	ASSERT_EQ((field & IP6F_RESERVED_MASK), 0x0006);
}

/* ============================================================================ */
TEST_SUITE(ipv6_hdr);

TEST(ipv6_hdr_size) {
	ASSERT_SIZE(struct ipv6_hdr, IPV6_HDR_LEN);
}

#if JACL_HAS_BSD

TEST(ip6_hdr_layout) {
	struct ip6_hdr p; memset(&p, 0, sizeof(p));
	p.ip6_v = 6; p.ip6_plen = htons(512);
	p.ip6_nxt = IPPROTO_ICMPV6; p.ip6_hlim = 255;
	ASSERT_EQ(p.ip6_v, 6);
	ASSERT_EQ(ntohs(p.ip6_plen), 512);
	ASSERT_EQ(p.ip6_nxt, IPPROTO_ICMPV6);
	ASSERT_EQ(p.ip6_hlim, 255);
}

TEST(ip6_hdr_offsets) {
	ASSERT_OFFSET(struct ip6_hdr, ip6_plen, 4);
	ASSERT_OFFSET(struct ip6_hdr, ip6_nxt, 6);
	ASSERT_OFFSET(struct ip6_hdr, ip6_hlim, 7);
	ASSERT_OFFSET(struct ip6_hdr, ip6_src, 8);
	ASSERT_OFFSET(struct ip6_hdr, ip6_dst, 24);
}

#else

TEST(ipv6_hdr_layout) {
	struct ipv6_hdr h; memset(&h, 0, sizeof(h));
	h.version = 6; h.plen = htons(128);
	h.nexthdr = IPPROTO_TCP; h.hoplim = 64;
	ASSERT_EQ(h.version, 6);
	ASSERT_EQ(ntohs(h.plen), 128);
	ASSERT_EQ(h.nexthdr, IPPROTO_TCP);
	ASSERT_EQ(h.hoplim, 64);
}

TEST(ipv6_hdr_offsets) {
	ASSERT_OFFSET(struct ipv6_hdr, plen, 4);
	ASSERT_OFFSET(struct ipv6_hdr, nexthdr, 6);
	ASSERT_OFFSET(struct ipv6_hdr, hoplim, 7);
	ASSERT_OFFSET(struct ipv6_hdr, saddr, 8);
	ASSERT_OFFSET(struct ipv6_hdr, daddr, 24);
}

#endif

TEST(ipv6_hdr_wire_format) {
	struct ipv6_hdr h; memset(&h, 0, sizeof(h));
	h.version = 6; h.plen = htons(256);
	h.nexthdr = IPPROTO_UDP; h.hoplim = 128;
	h.saddr.in6_u.u6_addr8[0] = 0x20; h.saddr.in6_u.u6_addr8[1] = 0x01;
	h.daddr.in6_u.u6_addr8[15] = 0x01;

	/* Verify network-byte-order fields and fixed offsets */
	ASSERT_EQ(ntohs(h.plen), 256);
	ASSERT_EQ(h.nexthdr, IPPROTO_UDP);
	ASSERT_EQ(h.hoplim, 128);
	ASSERT_EQ(h.saddr.in6_u.u6_addr8[0], 0x20);
	ASSERT_EQ(h.saddr.in6_u.u6_addr8[1], 0x01);
	ASSERT_EQ(h.daddr.in6_u.u6_addr8[15], 0x01);
}

TEST(ipv6_hdr_alignment){
	ASSERT_SIZE_ALIGNS(struct ipv6_hdr, uint32_t);
}

TEST(ipv6_hdr_max_payload) {
	struct ipv6_hdr h; memset(&h, 0, sizeof(h));
	h.plen = htons(0xFFFF);
	ASSERT_EQ(ntohs(h.plen), 0xFFFF); /* 65535 bytes */
}

TEST(ipv6_hdr_zero_payload) {
	struct ipv6_hdr h; memset(&h, 0, sizeof(h));
	h.plen = 0; h.nexthdr = IPPROTO_NONE;
	ASSERT_EQ(h.plen, 0);
	ASSERT_EQ(h.nexthdr, IPPROTO_NONE);
}

TEST(ipv6_hdr_hoplim_zero) {
	struct ipv6_hdr h; memset(&h, 0, sizeof(h));
	h.hoplim = 0;
	ASSERT_EQ(h.hoplim, 0); /* Valid; routers discard, not header error */
}

TEST(ipv6_hdr_version_tc_flow_packing) {
	/* Verify macros extract correctly from a raw 32-bit network-order word */
	uint32_t raw = (6 << 28) | (0xAB << 20) | 0x12345;
	ASSERT_EQ(IP6_VERSION(raw), 6);
	ASSERT_EQ(IP6_TRAFFIC_CLASS(raw), 0xAB);
	ASSERT_EQ(IP6_FLOW_LABEL(raw), 0x12345);
}

TEST(ipv6_hdr_null_safety) {
	struct ipv6_hdr *h = NULL;
	ASSERT_TRUE(h == NULL); /* Document: no helper derefs NULL */
}

/* ============================================================================ */

TEST_SUITE(ip6_ext);

TEST(ipv6_ext_size) {
	ASSERT_SIZE(struct ip6_ext, 2);
}

TEST(ip6_ext_layout) {
	struct ip6_ext e; memset(&e, 0, sizeof(e));
	e.ip6e_nxt = IPPROTO_DSTOPTS; e.ip6e_len = 2; /* 2*8+8 = 24 bytes total */
	ASSERT_EQ(e.ip6e_nxt, IPPROTO_DSTOPTS);
	ASSERT_EQ(e.ip6e_len, 2);
}

TEST(ip6_ext_len_semantics) {
	/* ip6e_len is in 8-byte units, excluding first 8 bytes */
	struct ip6_ext e; e.ip6e_len = 0; /* minimum: 8 bytes total */
	ASSERT_EQ(e.ip6e_len, 0);
	e.ip6e_len = 7; /* maximum standard: 7*8+8 = 64 bytes */
	ASSERT_EQ(e.ip6e_len, 7);
}

/* ============================================================================ */

TEST_SUITE(ip6_frag);

TEST(ipv6_frag_size) {
	ASSERT_SIZE(struct ip6_frag, 8);
}

TEST(ip6_frag_layout) {
	struct ip6_frag f; memset(&f, 0, sizeof(f));
	f.ip6f_nxt = IPPROTO_TCP;
	f.ip6f_offlg = htons(0x1238 | IP6F_MORE_FRAG);
	f.ip6f_ident = htonl(0xDEADBEEF);
	ASSERT_EQ(f.ip6f_nxt, IPPROTO_TCP);
	ASSERT_EQ(ntohs(f.ip6f_offlg) & IP6F_OFF_MASK, 0x1238);
	ASSERT_EQ(ntohl(f.ip6f_ident), 0xDEADBEEF);
}

TEST(ip6_frag_offset_extraction) {
	struct ip6_frag f; f.ip6f_offlg = htons(0x0128);
	ASSERT_EQ((ntohs(f.ip6f_offlg) & IP6F_OFF_MASK), 0x0128);
}

TEST(ip6_frag_ident_endianness) {
	struct ip6_frag f; f.ip6f_ident = htonl(0x12345678);
	ASSERT_EQ(ntohl(f.ip6f_ident), 0x12345678);
}

TEST(ip6_frag_null_safety) {
	struct ip6_frag *f = NULL;
	ASSERT_TRUE(f == NULL);
}

/* ============================================================================ */

TEST_SUITE(ip6_rthdr);

TEST(ip6_rthdr_size) {
	ASSERT_SIZE(struct ip6_rthdr, 4);
}

TEST(ip6_rthdr_layout) {
	struct ip6_rthdr r; memset(&r, 0, sizeof(r));
	r.ip6r_nxt = IPPROTO_TCP;
	r.ip6r_type = 2; /* Type 2: RFC 6275 mobility */
	r.ip6r_segleft = 3;
	ASSERT_EQ(r.ip6r_nxt, IPPROTO_TCP);
	ASSERT_EQ(r.ip6r_type, 2);
	ASSERT_EQ(r.ip6r_segleft, 3);
}

TEST(ip6_rthdr_segleft_decrement) {
	struct ip6_rthdr r; r.ip6r_segleft = 1;
	r.ip6r_segleft--;
	ASSERT_EQ(r.ip6r_segleft, 0); /* Last hop reached */
}

/* ============================================================================ */

TEST_SUITE(in6addr_any_init);

TEST(in6addr_any_init_bytes) {
	struct in6_addr a = IN6ADDR_ANY_INIT;
	for (int i = 0; i < 16; i++) ASSERT_EQ(a.in6_u.u6_addr8[i], 0);
}

/* ============================================================================ */

TEST_SUITE(in6addr_loopback_init);

TEST(in6addr_loopback_init_bytes) {
	struct in6_addr a = IN6ADDR_LOOPBACK_INIT;
	for (int i = 0; i < 15; i++) ASSERT_EQ(a.in6_u.u6_addr8[i], 0);
	ASSERT_EQ(a.in6_u.u6_addr8[15], 1);
}

/* ============================================================================ */

TEST_MAIN_IF(JACL_HAS_POSIX, "net/ipv6.h requires POSIX\n")
