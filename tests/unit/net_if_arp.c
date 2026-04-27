/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#include <net/if_arp.h>

TEST_TYPE(unit);
TEST_UNIT(net/if_arp.h);

/* ============================================================================ */

TEST_SUITE(constants);

TEST(constants_arphrd) {
	ASSERT_EQ(1, ARPHRD_ETHER);
	ASSERT_EQ(772, ARPHRD_LOOPBACK);
	ASSERT_EQ(32, ARPHRD_INFINIBAND);
}

TEST(constants_arpop) {
	ASSERT_EQ(1, ARPOP_REQUEST);
	ASSERT_EQ(2, ARPOP_REPLY);
	ASSERT_EQ(8, ARPOP_INVREQUEST);
}

TEST(constants_atf_flags) {
	ASSERT_EQ(0x02, ATF_COM);
	ASSERT_EQ(0x04, ATF_PERM);
	ASSERT_EQ(0x08, ATF_PUBL);
}

TEST_SUITE(constants_ioctls);

TEST(constants_arp_ioctls) {
	ASSERT_EQ(0x8955, SIOCSARP);
	ASSERT_EQ(0x8956, SIOCGARP);
	ASSERT_EQ(0x8957, SIOCDARP);
}

/* ============================================================================ */

TEST_SUITE(arphdr);

TEST(arphdr_size) {
	ASSERT_SIZE(struct arphdr, 8);
}

TEST(arphdr_layout) {
	struct arphdr h;
	memset(&h, 0, sizeof(h));
	h.ar_hrd = htons(ARPHRD_ETHER);
	h.ar_pro = htons(0x0800);
	h.ar_hln = 6;
	h.ar_pln = 4;
	h.ar_op = htons(ARPOP_REQUEST);

	ASSERT_EQ(htons(ARPHRD_ETHER), h.ar_hrd);
	ASSERT_EQ(htons(ARPOP_REQUEST), h.ar_op);
}

TEST(arphdr_helpers_null) {
	ASSERT_FALSE(arphdr_is_arp(NULL));
	ASSERT_FALSE(arphdr_is_valid(NULL));
}

TEST(arphdr_is_arp_excludes_rarp_inarp) {
	struct arphdr rarp = {0, 0, 6, 4, ARPOP_RREQUEST};
	struct arphdr inarp = {0, 0, 6, 4, ARPOP_INVREPLY};
	ASSERT_FALSE(arphdr_is_arp(&rarp));
	ASSERT_FALSE(arphdr_is_arp(&inarp));
}

TEST(arphdr_is_valid_zero_fields) {
	struct arphdr zero_op = {1, 0x0800, 6, 4, 0};
	struct arphdr zero_hln = {1, 0x0800, 0, 4, 1};
	struct arphdr zero_pln = {1, 0x0800, 6, 0, 1};

	ASSERT_FALSE(arphdr_is_valid(&zero_op));
	ASSERT_FALSE(arphdr_is_valid(&zero_hln));
	ASSERT_FALSE(arphdr_is_valid(&zero_pln));
}

/* ============================================================================ */

TEST_SUITE(ether_arp);

TEST(ether_arp_size) {
	/* 8 (arphdr) + 6 + 4 + 6 + 4 = 28 */
	ASSERT_SIZE(struct ether_arp, 28);
}

TEST(ether_arp_aliases) {
	struct ether_arp pkt;
	memset(&pkt, 0, sizeof(pkt));
	pkt.arp_hrd = 1;
	pkt.arp_op = 2;

	ASSERT_EQ(1, pkt.ea_hdr.ar_hrd);
	ASSERT_EQ(2, pkt.ea_hdr.ar_op);
}

TEST(ether_arp_field_offsets) {
	ASSERT_OFFSET(struct ether_arp, arp_sha, 8);  /* 8 bytes after header */
	ASSERT_OFFSET(struct ether_arp, arp_spa, 14); /* 8 + 6 */
	ASSERT_OFFSET(struct ether_arp, arp_tha, 18); /* 8 + 6 + 4 */
	ASSERT_OFFSET(struct ether_arp, arp_tpa, 24); /* 8 + 6 + 4 + 6 */
}

TEST(ether_arp_partial_read_safety) {
	uint8_t buf[14] = {0}; /* Shorter than full ether_arp */
	struct arphdr *h = (struct arphdr *)buf;

	h->ar_hrd = 1; /* Just set valid ints */
	h->ar_op = ARPOP_REQUEST;
	h->ar_hln = 6;
	h->ar_pln = 4;

	ASSERT_TRUE(arphdr_is_arp(h));
}

/* ============================================================================ */

TEST_SUITE(arpreq_size);

TEST(arpreq_size) {
	/* struct sockaddr(16) + sockaddr(16) + int(4) + sockaddr(16) + char[16](16) = 68 on 32/64-bit usually */
	/* Exact size depends on alignment, just check it's not tiny */
	ASSERT_SIZE_MIN(struct arpreq, 68);
}

TEST(arpreq_flag_bitwise) {
	struct arpreq req = {0};
	req.arp_flags = ATF_PERM | ATF_PUBL | ATF_NETMASK;
	ASSERT_EQ(ATF_PERM | ATF_PUBL | ATF_NETMASK, req.arp_flags);
	ASSERT_TRUE(req.arp_flags & ATF_PUBL);
	ASSERT_FALSE(req.arp_flags & ATF_COM);
}

TEST(arpreq_dev_truncation) {
	struct arpreq req = {0};
	strncpy(req.arp_dev, "ethernet_interface_name_too_long", sizeof(req.arp_dev) - 1);
	ASSERT_EQ('\0', req.arp_dev[sizeof(req.arp_dev) - 1]);
}

/* ============================================================================ */

TEST_SUITE(arphdr_is_arp);

TEST(arphdr_is_arp) {
	struct arphdr req = {0, 0, 0, 0, ARPOP_REQUEST};
	struct arphdr rep = {0, 0, 0, 0, ARPOP_REPLY};
	struct arphdr inv = {0, 0, 0, 0, 0xFF};

	ASSERT_TRUE(arphdr_is_arp(&req));
	ASSERT_TRUE(arphdr_is_arp(&rep));
	ASSERT_FALSE(arphdr_is_arp(&inv));
}

TEST_SUITE(arphdr_is_valid);

TEST(arphdr_is_valid) {
	struct arphdr good = {1, 0x0800, 6, 4, 1};
	struct arphdr bad_hln = {1, 0x0800, 0, 4, 1};

	ASSERT_TRUE(arphdr_is_valid(&good));
	ASSERT_FALSE(arphdr_is_valid(&bad_hln));
	ASSERT_FALSE(arphdr_is_valid(NULL));
}

/* ============================================================================ */

TEST_MAIN_IF(JACL_HAS_POSIX, "net/if_arp.h requires POSIX\n")
