/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <string.h>
#include <net/ip6.h>

TEST_TYPE(unit);
TEST_UNIT(net/ip6.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_version) {
	ASSERT_EQ(IPV6_VERSION_NUM, 6);  /* Fixed: use correct constant name */
}

TEST(constants_header_len) {
	ASSERT_EQ(IPV6_HDR_LEN, 40);
	ASSERT_EQ(IP6_HDR_LEN, 40);
}

TEST(constants_min_mtu) {
	ASSERT_EQ(IPV6_MIN_MTU, 1280);  /* Added: RFC 2460 mandate */
}

TEST(constants_ip6f_masks) {
	ASSERT_EQ(IP6F_OFF_MASK, 0xfff8);
	ASSERT_EQ(IP6F_RESERVED_MASK, 0x0006);
	ASSERT_EQ(IP6F_MORE_FRAG, 0x0001);
}

/* Guarded: x/ip_protocols.h is being updated by another dev */
#if defined(IPPROTO_HOPOPT) && defined(IPPROTO_ROUTING) && defined(IPPROTO_FRAGMENT)
TEST(constants_ipproto) {
	ASSERT_EQ(IPPROTO_HOPOPT, 0);
	ASSERT_EQ(IPPROTO_ROUTING, 43);
	ASSERT_EQ(IPPROTO_FRAGMENT, 44);
	ASSERT_EQ(IPPROTO_ESP, 50);
	ASSERT_EQ(IPPROTO_AH, 51);
	ASSERT_EQ(IPPROTO_ICMPV6, 58);
	ASSERT_EQ(IPPROTO_NONE, 59);
	ASSERT_EQ(IPPROTO_DSTOPTS, 60);
}
#endif

TEST(constants_ipv6_addr_classification) {
	ASSERT_EQ(IPV6_ADDR_LOOPBACK, 1);
	ASSERT_EQ(IPV6_ADDR_MULTICAST, 2);
	ASSERT_EQ(IPV6_ADDR_LINKLOCAL, 3);
	ASSERT_EQ(IPV6_ADDR_SITELOCAL, 4);
	ASSERT_EQ(IPV6_ADDR_V4MAPPED, 5);
	ASSERT_EQ(IPV6_ADDR_V4COMPAT, 6);
}

/* ============================================================================ */
TEST_SUITE(macros_bitwise);

TEST(ip6_version_extract) {
	ASSERT_EQ(IP6_VERSION(0x00000000), 0);
	ASSERT_EQ(IP6_VERSION(0x60000000), 6);
	ASSERT_EQ(IP6_VERSION(0xF0000000), 15);
	ASSERT_EQ(IP6_VERSION(0xFFFFFFFF), 15);
	ASSERT_EQ(IP6_VERSION(0x6ABCDEF0), 6);
}

TEST(ip6_traffic_class_extract) {
	ASSERT_EQ(IP6_TRAFFIC_CLASS(0x00000000), 0);
	ASSERT_EQ(IP6_TRAFFIC_CLASS(0x0FF00000), 0xFF);

	uint32_t mixed = 0xF0000000 | 0x000FFFFF;
	ASSERT_EQ(IP6_TRAFFIC_CLASS(mixed | (0xAB << 20)), 0xAB);

	uint32_t tc = (0x2E << 20);
	ASSERT_EQ(IP6_TRAFFIC_CLASS(tc), 0x2E);
}

TEST(ip6_flow_label_extract) {
	ASSERT_EQ(IP6_FLOW_LABEL(0x00000000), 0);
	ASSERT_EQ(IP6_FLOW_LABEL(0x000FFFFF), 0xFFFFF);
	ASSERT_EQ(IP6_FLOW_LABEL(0x00100000), 0);
	ASSERT_EQ(IP6_FLOW_LABEL(0x000FFFFF), 0xFFFFF);

	uint32_t mixed = 0xFFF00000;
	ASSERT_EQ(IP6_FLOW_LABEL(mixed | 0x12345), 0x12345);
}

TEST(ip6_set_macros) {
	uint32_t word = 0;
	word |= IP6_SET_VERSION(6);
	word |= IP6_SET_TRAFFIC_CLASS(0xAB);
	word |= IP6_SET_FLOW_LABEL(0x12345);

	ASSERT_EQ(IP6_VERSION(word), 6);
	ASSERT_EQ(IP6_TRAFFIC_CLASS(word), 0xAB);
	ASSERT_EQ(IP6_FLOW_LABEL(word), 0x12345);
}

/* ============================================================================ */
TEST_SUITE(struct_sizes);

TEST(ipv6_hdr_size) {
	ASSERT_EQ(sizeof(struct ipv6_hdr), 40);
}

TEST(ip6_hdr_size) {
	ASSERT_EQ(sizeof(struct ip6_hdr), 40);
}

TEST(ip6_ext_size) {
	ASSERT_EQ(sizeof(struct ip6_ext), 2);
}

TEST(ip6_frag_size) {
	ASSERT_EQ(sizeof(struct ip6_frag), 8);
}

TEST(ip6_rthdr_size) {
	ASSERT_EQ(sizeof(struct ip6_rthdr), 4);
}

TEST(sizes_match) {
	ASSERT_EQ(sizeof(struct ipv6_hdr), sizeof(struct ip6_hdr));
}

/* ============================================================================ */
TEST_SUITE(struct_layout_linux);

TEST(ipv6_hdr_offsets) {
	ASSERT_OFFSET(struct ipv6_hdr, plen, 4);
	ASSERT_OFFSET(struct ipv6_hdr, nexthdr, 6);
	ASSERT_OFFSET(struct ipv6_hdr, hoplim, 7);
	ASSERT_OFFSET(struct ipv6_hdr, saddr, 8);
	ASSERT_OFFSET(struct ipv6_hdr, daddr, 24);
}

TEST(ipv6_hdr_layout) {
	struct ipv6_hdr h;
	memset(&h, 0, sizeof(h));
	h.version = 6;
	h.plen = htons(128);
	h.nexthdr = IPPROTO_TCP;
	h.hoplim = 64;

	ASSERT_EQ(h.version, 6);
	ASSERT_EQ(ntohs(h.plen), 128);
	ASSERT_EQ(h.nexthdr, IPPROTO_TCP);
	ASSERT_EQ(h.hoplim, 64);
}

/* ============================================================================ */
TEST_SUITE(struct_layout_bsd);

TEST(ip6_hdr_offsets) {
	ASSERT_OFFSET(struct ip6_hdr, ip6_plen, 4);
	ASSERT_OFFSET(struct ip6_hdr, ip6_nxt, 6);
	ASSERT_OFFSET(struct ip6_hdr, ip6_hlim, 7);
	ASSERT_OFFSET(struct ip6_hdr, ip6_src, 8);
	ASSERT_OFFSET(struct ip6_hdr, ip6_dst, 24);
}

TEST(ip6_hdr_layout) {
	struct ip6_hdr p;
	memset(&p, 0, sizeof(p));
	p.ip6_v = 6;
	p.ip6_plen = htons(512);
	p.ip6_nxt = IPPROTO_TCP;  /* Changed: use defined constant */
	p.ip6_hlim = 255;

	ASSERT_EQ(p.ip6_v, 6);
	ASSERT_EQ(ntohs(p.ip6_plen), 512);
	ASSERT_EQ(p.ip6_nxt, IPPROTO_TCP);
	ASSERT_EQ(p.ip6_hlim, 255);
}

/* ============================================================================ */
TEST_SUITE(bitfield_packing);

TEST(ipv6_hdr_version_tc_flow_packing) {
	uint32_t raw = (6 << 28) | (0xAB << 20) | 0x12345;

	ASSERT_EQ(IP6_VERSION(raw), 6);
	ASSERT_EQ(IP6_TRAFFIC_CLASS(raw), 0xAB);
	ASSERT_EQ(IP6_FLOW_LABEL(raw), 0x12345);
}

TEST(ipv6_hdr_wire_format_first_word) {
	struct ipv6_hdr h;
	memset(&h, 0, sizeof(h));
	h.version = 6;
	h.plen = 0;

	uint8_t *raw = (uint8_t *)&h;

	#if JACL_HAS_LE
		ASSERT_EQ((raw[3] >> 4), 6);
	#else
		ASSERT_EQ((raw[0] >> 4), 6);
	#endif
}

/* ============================================================================ */
TEST_SUITE(extension_headers);

TEST(ip6_ext_layout) {
	struct ip6_ext e;
	memset(&e, 0, sizeof(e));
	e.ip6e_nxt = IPPROTO_TCP;  /* Changed: use defined constant */
	e.ip6e_len = 2;

	ASSERT_EQ(e.ip6e_nxt, IPPROTO_TCP);
	ASSERT_EQ(e.ip6e_len, 2);
}

TEST(ip6_frag_layout) {
	struct ip6_frag f;
	memset(&f, 0, sizeof(f));
	f.ip6f_nxt = IPPROTO_TCP;
	f.ip6f_offlg = htons(0x1238 | IP6F_MORE_FRAG);
	f.ip6f_ident = htonl(0xDEADBEEF);

	ASSERT_EQ(f.ip6f_nxt, IPPROTO_TCP);
	ASSERT_EQ(ntohs(f.ip6f_offlg) & IP6F_OFF_MASK, 0x1238);
	ASSERT_EQ(ntohs(f.ip6f_offlg) & IP6F_MORE_FRAG, IP6F_MORE_FRAG);
	ASSERT_EQ(ntohl(f.ip6f_ident), 0xDEADBEEF);
}

TEST(ip6_rthdr_layout) {
	struct ip6_rthdr r;
	memset(&r, 0, sizeof(r));
	r.ip6r_nxt = IPPROTO_TCP;
	r.ip6r_type = 2;
	r.ip6r_segleft = 3;

	ASSERT_EQ(r.ip6r_nxt, IPPROTO_TCP);
	ASSERT_EQ(r.ip6r_type, 2);
	ASSERT_EQ(r.ip6r_segleft, 3);
}

/* ============================================================================ */
TEST_SUITE(initializers);

/* Fixed: struct in6_addr is flat, use s6_addr */
TEST(in6addr_any_init_bytes) {
	struct in6_addr a = IN6ADDR_ANY_INIT;
	for (int i = 0; i < 16; i++) {
		ASSERT_EQ(a.s6_addr[i], 0);  /* Fixed: flat struct access */
	}
}

TEST(in6addr_loopback_init_bytes) {
	struct in6_addr a = IN6ADDR_LOOPBACK_INIT;
	for (int i = 0; i < 15; i++) {
		ASSERT_EQ(a.s6_addr[i], 0);  /* Fixed */
	}
	ASSERT_EQ(a.s6_addr[15], 1);     /* Fixed */
}

TEST(in6addr_static_vars) {
	ASSERT_EQ(in6addr_any.s6_addr[0], 0);        /* Fixed */
	ASSERT_EQ(in6addr_loopback.s6_addr[15], 1);  /* Fixed */
}

/* ============================================================================ */
TEST_SUITE(alignment);

TEST(ipv6_hdr_alignment) {
	ASSERT_SIZE_ALIGNS(struct ipv6_hdr, uint32_t);
	ASSERT_SIZE_ALIGNS(struct ip6_hdr, uint32_t);
}

TEST_MAIN_IF(JACL_HAS_POSIX, "net/ip6.h requires POSIX\n")
