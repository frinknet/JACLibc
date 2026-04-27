/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#include <net/ethernet.h>

TEST_TYPE(unit);
TEST_UNIT(net/ethernet.h);

/* ============================================================================ */

TEST_SUITE(constants);

TEST(constants_eth_sizes) {
	ASSERT_EQ(ETH_ALEN, 6);
	ASSERT_EQ(ETHER_ADDR_LEN, 6);
	ASSERT_EQ(ETH_HLEN, 14);
	ASSERT_EQ(ETH_ZLEN, 60);
	ASSERT_EQ(ETH_DATA_LEN, 1500);
	ASSERT_EQ(ETH_FCS_LEN, 4);
	ASSERT_EQ(ETH_FRAME_LEN, 1518);
	ASSERT_EQ(ETH_P_802_3_MIN, 0x0600);
}

TEST(constants_eth_proto_linux) {
	ASSERT_EQ(ETH_P_IP, 0x0800);
	ASSERT_EQ(ETH_P_ARP, 0x0806);
	ASSERT_EQ(ETH_P_IPV6, 0x86DD);
	ASSERT_EQ(ETH_P_8021Q, 0x8100);
	ASSERT_EQ(ETH_P_PAUSE, 0x8808);
	ASSERT_EQ(ETH_P_LLDP, 0x88CC);
	ASSERT_EQ(ETH_P_ALL, 0x0003);
}

TEST(constants_eth_proto_bsd) {
	ASSERT_EQ(ETHERTYPE_IP, ETH_P_IP);
	ASSERT_EQ(ETHERTYPE_ARP, ETH_P_ARP);
	ASSERT_EQ(ETHERTYPE_IPV6, ETH_P_IPV6);
	ASSERT_EQ(ETHERTYPE_VLAN, ETH_P_8021Q);
	ASSERT_EQ(ETHERTYPE_SLOW, ETH_P_SLOW);
}

TEST(constants_eth_addr_masks) {
	ASSERT_EQ(ETH_ADDR_UNICAST, 0x00);
	ASSERT_EQ(ETH_ADDR_MULTICAST, 0x01);
	ASSERT_EQ(ETH_ADDR_BROADCAST, 0xFF);
	ASSERT_EQ(ETH_ADDR_LOCAL_MASK, 0x02);
}

/* ============================================================================ */

TEST_SUITE(ether_addr);

TEST(ether_addr_size) {
	ASSERT_SIZE(struct ether_addr, ETH_ALEN);
}

TEST(ether_addr_layout) {
	struct ether_addr addr = {{0x01, 0x23, 0x45, 0x67, 0x89, 0xAB}};
	ASSERT_EQ(addr.ether_addr_octet[0], 0x01);
	ASSERT_EQ(addr.ether_addr_octet[5], 0xAB);
}

/* ============================================================================ */

TEST_SUITE(ethhdr);

TEST(ethhdr_size) {
	ASSERT_SIZE(struct ethhdr, ETH_HLEN);
}

TEST(ethhdr_layout) {
	struct ethhdr hdr;
	memset(&hdr, 0, sizeof(hdr));
	memcpy(hdr.h_dest, "\x00\x11\x22\x33\x44\x55", ETH_ALEN);
	memcpy(hdr.h_source, "\x66\x77\x88\x99\xaa\xbb", ETH_ALEN);
	hdr.h_proto = htons(ETH_P_IP);

	ASSERT_EQ(memcmp(hdr.h_dest, "\x00\x11\x22\x33\x44\x55", ETH_ALEN), 0);
	ASSERT_EQ(memcmp(hdr.h_source, "\x66\x77\x88\x99\xaa\xbb", ETH_ALEN), 0);
	ASSERT_EQ(ntohs(hdr.h_proto), ETH_P_IP);
}

TEST(ethhdr_offsets) {
	ASSERT_OFFSET(struct ethhdr, h_dest, 0);
	ASSERT_OFFSET(struct ethhdr, h_source, 6);
	ASSERT_OFFSET(struct ethhdr, h_proto, 12);
}

TEST(ethhdr_alignment) {
	ASSERT_SIZE_ALIGNS(struct ethhdr, uint16_t);
}

/* ============================================================================ */

TEST_SUITE(ether_header);

TEST(ether_header_size) {
	ASSERT_SIZE(struct ether_header, ETH_HLEN);
}

TEST(ether_header_layout) {
	struct ether_header hdr;
	memset(&hdr, 0, sizeof(hdr));
	memcpy(hdr.ether_dhost, "\xaa\xbb\xcc\xdd\xee\xff", ETH_ALEN);
	hdr.ether_type = htons(ETH_P_ARP);

	ASSERT_EQ(memcmp(hdr.ether_dhost, "\xaa\xbb\xcc\xdd\xee\xff", ETH_ALEN), 0);
	ASSERT_EQ(ntohs(hdr.ether_type), ETH_P_ARP);
}

/* ============================================================================ */

TEST_SUITE(vlan_hdr);

TEST(vlan_hdr_size) {
	ASSERT_SIZE(struct vlan_hdr, 4);
}

TEST(vlan_hdr_layout) {
	struct vlan_hdr v;
	memset(&v, 0, sizeof(v));
	v.h_vlan_TCI = htons(0x8102);
	v.h_vlan_encapsulated_proto = htons(ETH_P_IP);

	ASSERT_EQ(ntohs(v.h_vlan_TCI), 0x8102);
	ASSERT_EQ(ntohs(v.h_vlan_encapsulated_proto), ETH_P_IP);
}

/* ============================================================================ */

TEST_SUITE(vlan_ethhdr);

TEST(vlan_ethhdr_size) {
	ASSERT_SIZE(struct vlan_ethhdr, 18);
}

TEST(vlan_ethhdr_layout) {
	struct vlan_ethhdr v;
	memset(&v, 0, sizeof(v));
	v.h_vlan_proto = htons(ETH_P_8021Q);
	v.h_vlan_TCI = htons(0x0001);
	v.h_vlan_encapsulated_proto = htons(ETH_P_IPV6);

	ASSERT_EQ(ntohs(v.h_vlan_proto), ETH_P_8021Q);
	ASSERT_EQ(ntohs(v.h_vlan_TCI), 0x0001);
	ASSERT_EQ(ntohs(v.h_vlan_encapsulated_proto), ETH_P_IPV6);
}

/* ============================================================================ */

TEST_SUITE(vlan_get_prio);

TEST(vlan_get_prio_zero) {
	struct vlan_hdr v = {0};
	v.h_vlan_TCI = htons(0x0000); /* Network-order: PCP=0 */
	ASSERT_EQ(vlan_get_prio(ntohs(v.h_vlan_TCI)), 0);
}

TEST(vlan_get_prio_max) {
	struct vlan_hdr v = {0};
	v.h_vlan_TCI = htons(0xE000); /* Network-order: PCP=7 */
	ASSERT_EQ(vlan_get_prio(ntohs(v.h_vlan_TCI)), 7);
}

/* ============================================================================ */

TEST_SUITE(vlan_get_cfi);

TEST(vlan_get_cfi_zero) {
	struct vlan_hdr v = {0};
	v.h_vlan_TCI = htons(0x0000); /* Network-order: DEI=0 */
	ASSERT_EQ(vlan_get_cfi(ntohs(v.h_vlan_TCI)), 0);
}

TEST(vlan_get_cfi_one) {
	struct vlan_hdr v = {0};
	v.h_vlan_TCI = htons(0x1000); /* Network-order: DEI=1 */
	ASSERT_EQ(vlan_get_cfi(ntohs(v.h_vlan_TCI)), 1);
}

/* ============================================================================ */

TEST_SUITE(vlan_get_vid);

TEST(vlan_get_vid_zero) {
	struct vlan_hdr v = {0};
	v.h_vlan_TCI = htons(0x0000); /* Network-order: VID=0 */
	ASSERT_EQ(vlan_get_vid(ntohs(v.h_vlan_TCI)), 0);
}

TEST(vlan_get_vid_max) {
	struct vlan_hdr v = {0};
	v.h_vlan_TCI = htons(0x0FFF); /* Network-order: VID=4095 */
	ASSERT_EQ(vlan_get_vid(ntohs(v.h_vlan_TCI)), 4095);
}

TEST(vlan_get_vid_masked) {
	struct vlan_hdr v = {0};
	v.h_vlan_TCI = htons(0xFFFF); /* Network-order: All bits set */
	ASSERT_EQ(vlan_get_vid(ntohs(v.h_vlan_TCI)), 4095); /* Verify mask ignores PCP/CFI */
}

/* ============================================================================ */

TEST_SUITE(ether_is_broadcast);

TEST(ether_is_broadcast_true) {
	uint8_t bc[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	ASSERT_TRUE(ether_is_broadcast(bc));
}

TEST(ether_is_broadcast_false) {
	uint8_t uc[6] = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E};
	ASSERT_FALSE(ether_is_broadcast(uc));
}

/* ============================================================================ */

TEST_SUITE(ether_is_multicast);

TEST(ether_is_multicast_true) {
	uint8_t mc[6] = {0x01, 0x00, 0x5E, 0x00, 0x00, 0x01};
	ASSERT_TRUE(ether_is_multicast(mc));
}

TEST(ether_is_multicast_false) {
	uint8_t uc[6] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x01};
	ASSERT_FALSE(ether_is_multicast(uc));
}

/* ============================================================================ */

TEST_SUITE(ether_is_unicast);

TEST(ether_is_unicast_true) {
	uint8_t uc[6] = {0x02, 0x11, 0x22, 0x33, 0x44, 0x55};
	ASSERT_TRUE(ether_is_unicast(uc));
}

TEST(ether_is_unicast_false) {
	uint8_t mc[6] = {0x01, 0x11, 0x22, 0x33, 0x44, 0x55};
	ASSERT_FALSE(ether_is_unicast(mc));
}

/* ============================================================================ */

TEST_SUITE(ether_is_local);

TEST(ether_is_local_true) {
	uint8_t local[6] = {0x02, 0x11, 0x22, 0x33, 0x44, 0x55};
	ASSERT_TRUE(ether_is_local(local));
}

TEST(ether_is_local_false) {
	uint8_t global[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
	ASSERT_FALSE(ether_is_local(global));
}

TEST(ether_is_local_multicast_combo) {
	/* Bit 0: Multicast, Bit 1: Locally Administered */
	uint8_t addr[6] = {0x03, 0x00, 0x00, 0x00, 0x00, 0x00};
	ASSERT_TRUE(ether_is_multicast(addr));
	ASSERT_TRUE(ether_is_local(addr));
	ASSERT_FALSE(ether_is_unicast(addr));
}

/* ============================================================================ */

TEST_SUITE(ether_is_global);

TEST(ether_is_global_true) {
	uint8_t global[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
	ASSERT_TRUE(ether_is_global(global));
}

TEST(ether_is_global_false) {
	uint8_t local[6] = {0x02, 0x11, 0x22, 0x33, 0x44, 0x55};
	ASSERT_FALSE(ether_is_global(local));
}

/* ============================================================================ */

TEST_SUITE(ether_addr_equal);

TEST(ether_addr_equal_match) {
	uint8_t a[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
	uint8_t b[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
	ASSERT_TRUE(ether_addr_equal(a, b));
}

TEST(ether_addr_equal_mismatch) {
	uint8_t a[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
	uint8_t b[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x77};
	ASSERT_FALSE(ether_addr_equal(a, b));
}

TEST(ether_addr_equal_identical_pointer) {
	uint8_t a[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
	ASSERT_TRUE(ether_addr_equal(a, a));
}

/* ============================================================================ */

TEST_SUITE(ether_is_valid);

TEST(ether_is_valid_null) {
	ASSERT_FALSE(ether_is_valid(NULL));
}

TEST(ether_is_valid_broadcast) {
	uint8_t bc[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	ASSERT_FALSE(ether_is_valid(bc));
}

TEST(ether_is_valid_unicast) {
	uint8_t uc[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
	ASSERT_TRUE(ether_is_valid(uc));
}

TEST(ether_is_valid_non_null_broadcast) {
	uint8_t bc[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	ASSERT_FALSE(ether_is_valid(bc));
}

/* ============================================================================ */

TEST_SUITE(ether_wire_integrity);

TEST(ether_wire_bytes_exact) {
	uint8_t raw[14] = {
		0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
		0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB,
		0x08, 0x00
	};
	struct ethhdr *h = (struct ethhdr *)raw;
	ASSERT_EQ(h->h_proto, htons(ETH_P_IP));
	ASSERT_EQ(h->h_dest[0], 0x00);
	ASSERT_EQ(h->h_source[5], 0xBB);
}

/* ============================================================================ */

TEST_SUITE(ether_addr_edge_cases);

TEST(ether_addr_null_mac_validity) {
	uint8_t null[6] = {0};
	/* Unspecified but technically valid per IEEE (not broadcast) */
	ASSERT_TRUE(ether_is_valid(null));
	ASSERT_TRUE(ether_is_unicast(null));
	ASSERT_FALSE(ether_is_multicast(null));
}

/* ============================================================================ */

TEST_SUITE(vlan_tci_bit_isolation);

TEST(vlan_vid_does_not_bleed_to_prio) {
	uint16_t tci = htons(0x0FFF); /* VID max, PCP/CFI zero */
	struct vlan_hdr v = {0}; v.h_vlan_TCI = tci;
	ASSERT_EQ(vlan_get_prio(ntohs(v.h_vlan_TCI)), 0);
}

TEST(vlan_vid_does_not_bleed_to_cfi) {
	uint16_t tci = htons(0x0FFF);
	struct vlan_hdr v = {0}; v.h_vlan_TCI = tci;
	ASSERT_EQ(vlan_get_cfi(ntohs(v.h_vlan_TCI)), 0);
}

TEST(vlan_tci_full_field_separation) {
	/* PCP=7, DEI=1, VID=4095 -> 0xFFFF */
	uint16_t tci = htons(0xFFFF);
	struct vlan_hdr v = {0}; v.h_vlan_TCI = tci;
	ASSERT_EQ(vlan_get_prio(ntohs(v.h_vlan_TCI)), 7);
	ASSERT_EQ(vlan_get_cfi(ntohs(v.h_vlan_TCI)), 1);
	ASSERT_EQ(vlan_get_vid(ntohs(v.h_vlan_TCI)), 4095);
}

TEST(vlan_vid_zero_reserved) {
	/* IEEE 802.1Q: VID 0 indicates priority-only tagging, not a valid VLAN */
	uint16_t tci = htons(0x0000);
	struct vlan_hdr v = {0}; v.h_vlan_TCI = tci;
	ASSERT_EQ(vlan_get_vid(ntohs(v.h_vlan_TCI)), 0);
}

TEST(vlan_vid_4095_reserved) {
	/* IEEE 802.1Q: VID 4095 is reserved, must not be used for data traffic */
	uint16_t tci = htons(0x0FFF);
	struct vlan_hdr v = {0}; v.h_vlan_TCI = tci;
	ASSERT_EQ(vlan_get_vid(ntohs(v.h_vlan_TCI)), 4095);
}


/* ============================================================================ */

TEST_SUITE(eth_proto_boundaries);

TEST(eth_proto_type_length_boundary) {
	/* >= 0x0600 = EtherType, < 0x0600 = 802.3 Length */
	ASSERT_TRUE(ETH_P_IP >= ETH_P_802_3_MIN);
	ASSERT_TRUE(ETH_P_8021Q >= ETH_P_802_3_MIN);
	ASSERT_FALSE(0x0500 >= ETH_P_802_3_MIN); /* Valid length, invalid type */
}

/* ============================================================================ */

TEST_SUITE(qinq_detection);

TEST(qinq_outer_tag_detection) {
	uint8_t raw[18] = {0};
	struct ethhdr *h = (struct ethhdr *)raw;
	h->h_proto = htons(ETH_P_8021Q);
	ASSERT_EQ(ntohs(h->h_proto), ETH_P_8021Q);
	/* Inner tag payload would start at offset 18 in wire */
}

/* ============================================================================ */

TEST_MAIN_IF(JACL_HAS_POSIX, "net/ethernet.h requires POSIX\n")
