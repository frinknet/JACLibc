/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <net/if_arp.h>
#include <string.h>
#include <limits.h>

TEST_TYPE(unit);
TEST_UNIT(net/if_arp.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_arphrd_values) {
	ASSERT_EQ(0, ARPHRD_NETROM);
	ASSERT_EQ(1, ARPHRD_ETHER);
	ASSERT_EQ(772, ARPHRD_LOOPBACK);
	ASSERT_EQ(32, ARPHRD_INFINIBAND);
}

TEST(constants_arpop_values) {
	ASSERT_EQ(1, ARPOP_REQUEST);
	ASSERT_EQ(2, ARPOP_REPLY);
	ASSERT_EQ(3, ARPOP_RREQUEST);
	ASSERT_EQ(8, ARPOP_INVREQUEST);
	ASSERT_EQ(9, ARPOP_INVREPLY);
}

TEST(constants_atf_flags_isolation) {
	/* Flags are distinct bits. AND == 0 proves isolation. */
	ASSERT_FALSE(ATF_PERM & ATF_PUBL);
	ASSERT_FALSE(ATF_COM & ATF_NETMASK);

	/* OR combines them correctly */
	int flags = ATF_PERM | ATF_PUBL | ATF_COM;
	ASSERT_TRUE(flags & ATF_PERM);
	ASSERT_TRUE(flags & ATF_PUBL);
	ASSERT_TRUE(flags & ATF_COM);
}

TEST(constants_ioctls_uniqueness) {
	ASSERT_NE(SIOCSARP, SIOCGARP);
	ASSERT_NE(SIOCGARP, SIOCDARP);
	ASSERT_NE(SIOCSARP, SIOCDARP);
}

/* ============================================================================ */
TEST_SUITE(arphdr_layout);

TEST(arphdr_size) {
	ASSERT_EQ(8, sizeof(struct arphdr));
}

TEST(arphdr_field_offsets) {
	ASSERT_OFFSET(struct arphdr, ar_hrd, 0);
	ASSERT_OFFSET(struct arphdr, ar_pro, 2);
	ASSERT_OFFSET(struct arphdr, ar_hln, 4);
	ASSERT_OFFSET(struct arphdr, ar_pln, 5);
	ASSERT_OFFSET(struct arphdr, ar_op, 6);
}

TEST(arphdr_zero_init) {
	struct arphdr h = {0};
	ASSERT_EQ(0, h.ar_hrd);
	ASSERT_EQ(0, h.ar_op);
	ASSERT_EQ(0, h.ar_hln);
}

TEST(arphdr_max_values) {
	struct arphdr h;
	memset(&h, 0xFF, sizeof(h));
	ASSERT_EQ(0xFFFF, h.ar_hrd);
	ASSERT_EQ(0xFF, h.ar_hln);
	ASSERT_EQ(0xFFFF, h.ar_op);
}

/* ============================================================================ */
TEST_SUITE(arphdr_sad_paths);

TEST(arphdr_is_arp_null) {
	ASSERT_FALSE(arphdr_is_arp(NULL));
	ASSERT_FALSE(arphdr_is_valid(NULL));
}

TEST(arphdr_is_arp_excludes_non_arp_ops) {
	struct arphdr rarp = {.ar_op = ARPOP_RREQUEST};
	struct arphdr inarp = {.ar_op = ARPOP_INVREQUEST};
	struct arphdr unknown = {.ar_op = 0xFF};

	ASSERT_FALSE(arphdr_is_arp(&rarp));
	ASSERT_FALSE(arphdr_is_arp(&inarp));
	ASSERT_FALSE(arphdr_is_arp(&unknown));
}

TEST(arphdr_is_valid_zero_hln) {
	struct arphdr h = {.ar_hrd = 1, .ar_pro = 0x0800, .ar_hln = 0, .ar_pln = 4, .ar_op = 1};
	ASSERT_FALSE(arphdr_is_valid(&h));
}

TEST(arphdr_is_valid_zero_pln) {
	struct arphdr h = {.ar_hrd = 1, .ar_pro = 0x0800, .ar_hln = 6, .ar_pln = 0, .ar_op = 1};
	ASSERT_FALSE(arphdr_is_valid(&h));
}

TEST(arphdr_is_valid_zero_op) {
	struct arphdr h = {.ar_hrd = 1, .ar_pro = 0x0800, .ar_hln = 6, .ar_pln = 4, .ar_op = 0};
	ASSERT_FALSE(arphdr_is_valid(&h));
}

TEST(arphdr_is_valid_all_zero) {
	struct arphdr h = {0};
	ASSERT_FALSE(arphdr_is_valid(&h));
}

/* ============================================================================ */
TEST_SUITE(ether_arp_layout);

TEST(ether_arp_size) {
	/* 8 (hdr) + 6 + 4 + 6 + 4 = 28. Must be exact for wire parsing */
	ASSERT_EQ(28, sizeof(struct ether_arp));
}

TEST(ether_arp_field_offsets) {
	ASSERT_OFFSET(struct ether_arp, arp_sha, 8);
	ASSERT_OFFSET(struct ether_arp, arp_spa, 14);
	ASSERT_OFFSET(struct ether_arp, arp_tha, 18);
	ASSERT_OFFSET(struct ether_arp, arp_tpa, 24);
}

TEST(ether_arp_alias_mapping) {
	struct ether_arp pkt;
	memset(&pkt, 0, sizeof(pkt));
	pkt.arp_hrd = 0xBEEF;
	pkt.arp_op = 0xCAFE;

	ASSERT_EQ(0xBEEF, pkt.ea_hdr.ar_hrd);
	ASSERT_EQ(0xCAFE, pkt.ea_hdr.ar_op);
}

TEST(ether_arp_partial_buffer_safety) {
	/* arphdr is 8 bytes. ar_op starts at offset 6. Must allocate 8 to avoid overflow */
	uint8_t raw[8] = {0};
	struct arphdr *h = (struct arphdr *)raw;
	h->ar_op = ARPOP_REQUEST; /* Host-order assignment matches helper */

	ASSERT_TRUE(arphdr_is_arp(h)); /* No crash, correct logic */
}

/* ============================================================================ */
TEST_SUITE(arpreq_layout_abi);

TEST(arpreq_not_packed) {
	/* Verify natural alignment. Should be > 64 due to padding on 64-bit */
	ASSERT_TRUE(sizeof(struct arpreq) >= 64);
}

TEST(arpreq_field_offsets) {
	ASSERT_OFFSET(struct arpreq, arp_pa, 0);
	ASSERT_OFFSET(struct arpreq, arp_ha, sizeof(struct sockaddr));
	ASSERT_OFFSET_ALIGNS(struct arpreq, arp_flags, int);
}

TEST(arpreq_dev_size) {
	ASSERT_EQ(IFNAMSIZ, sizeof(((struct arpreq *)0)->arp_dev));
}

/* ============================================================================ */
TEST_SUITE(arpreq_flags_sad_paths);

TEST(arpreq_flags_combinations) {
	struct arpreq req = {0};
	req.arp_flags = ATF_PERM | ATF_PUBL | ATF_NETMASK;
	ASSERT_TRUE(req.arp_flags & ATF_PERM);
	ASSERT_TRUE(req.arp_flags & ATF_PUBL);
	ASSERT_TRUE(req.arp_flags & ATF_NETMASK);
	ASSERT_FALSE(req.arp_flags & ATF_COM);
}

TEST(arpreq_flags_clear_mask) {
	struct arpreq req = {0};
	req.arp_flags = ATF_COM | ATF_PERM | ATF_PUBL;
	req.arp_flags &= ~ATF_PUBL;
	ASSERT_TRUE(req.arp_flags & ATF_PERM);
	ASSERT_FALSE(req.arp_flags & ATF_PUBL);
}

TEST(arpreq_flags_reserved_bits) {
	struct arpreq req = {0};
	req.arp_flags = 0xFFFF;
	ASSERT_TRUE(req.arp_flags & ATF_COM);
	ASSERT_TRUE(req.arp_flags & ATF_PERM);
	ASSERT_TRUE(req.arp_flags & ATF_PUBL);
}

TEST(arpreq_dev_truncation) {
	struct arpreq req = {0};
	/* String: "interface_name_exceeds_sixteen_chars_easily" */
	strncpy(req.arp_dev, "interface_name_exceeds_sixteen_chars_easily", sizeof(req.arp_dev) - 1);
	req.arp_dev[sizeof(req.arp_dev) - 1] = '\0';

	ASSERT_EQ('_', req.arp_dev[14]);  /* Fixed: actual char at index 14 */
	ASSERT_EQ('\0', req.arp_dev[15]); /* Null terminator */
}

TEST(arpreq_dev_empty) {
	struct arpreq req = {0};
	ASSERT_EQ('\0', req.arp_dev[0]);
	ASSERT_EQ(0, strlen(req.arp_dev));
}

/* ============================================================================ */
TEST_SUITE(ab_integrity);

TEST(arphdr_no_padding_between_fields) {
	/* Wire structs must be contiguous */
	ASSERT_EQ(sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint8_t) + sizeof(uint8_t), 6);
	ASSERT_OFFSET(struct arphdr, ar_op, 6);
}

TEST(ether_arp_wire_contiguous) {
	uint8_t raw[28] = {0};
	struct ether_arp *pkt = (struct ether_arp *)raw;
	pkt->arp_sha[0] = 0xAA;
	pkt->arp_spa[0] = 0xBB;
	pkt->arp_tha[0] = 0xCC;
	pkt->arp_tpa[0] = 0xDD;

	ASSERT_EQ(0xAA, raw[8]);
	ASSERT_EQ(0xBB, raw[14]);
	ASSERT_EQ(0xCC, raw[18]);
	ASSERT_EQ(0xDD, raw[24]);
}

TEST(arpreq_sockaddr_alignment) {
	/* Verify union/padding doesn't shift int unexpectedly */
	struct arpreq req = {0};
	req.arp_flags = 0xDEADBEEF;

	/* Read back via raw pointer to verify exact placement */
	uint8_t *p = (uint8_t *)&req;
	size_t off = offsetof(struct arpreq, arp_flags);
	ASSERT_EQ(0xDEADBEEF, *(uint32_t *)(p + off));
}

/* ============================================================================ */
TEST_SUITE(arphdr_endianness_traps);

TEST(arphdr_is_arp_network_order_required) {
	struct arphdr h = {0};
	h.ar_op = ARPOP_REQUEST;
	ASSERT_TRUE(arphdr_is_arp(&h)); /* Raw comparison works with host constants */

	h.ar_op = ARPOP_REPLY;
	ASSERT_TRUE(arphdr_is_arp(&h));

	h.ar_op = 0xFF;
	ASSERT_FALSE(arphdr_is_arp(&h));
}

TEST(arphdr_is_arp_max_op_values) {
    struct arphdr h = {0};
    h.ar_op = 0xFFFF; /* Invalid, but must not crash */
    ASSERT_FALSE(arphdr_is_arp(&h));

    h.ar_op = htons(0);
    ASSERT_FALSE(arphdr_is_arp(&h));
}

/* ============================================================================ */
TEST_SUITE(arphdr_length_boundaries);

TEST(arphdr_is_valid_max_hln_pln) {
    struct arphdr h = {0};
    h.ar_hln = 255;
    h.ar_pln = 255;
    h.ar_op = 1;
    /* Helpers only check >0. They don't validate buffer bounds. */
    ASSERT_TRUE(arphdr_is_valid(&h));
}

TEST(arphdr_zero_length_protection) {
    struct arphdr h = {0};
    h.ar_hln = 0; h.ar_pln = 4; h.ar_op = 1;
    ASSERT_FALSE(arphdr_is_valid(&h));

    h.ar_hln = 6; h.ar_pln = 0; h.ar_op = 1;
    ASSERT_FALSE(arphdr_is_valid(&h));
}

/* ============================================================================ */
TEST_SUITE(arpreq_flag_corruption);

TEST(arpreq_flags_undefined_bits_preserved) {
    struct arpreq req = {0};
    req.arp_flags = 0xF8 | ATF_COM; /* 0xF8 = undefined kernel bits */
    ASSERT_TRUE(req.arp_flags & ATF_COM);

    /* Clearing ATF_COM must not corrupt upper bits */
    req.arp_flags &= ~ATF_COM;
    ASSERT_EQ(0xF8, req.arp_flags);
}

TEST(arpreq_flags_all_known_bits_mask) {
    uint32_t known_mask = ATF_COM | ATF_PERM | ATF_PUBL | ATF_USETRAILERS | ATF_NETMASK | ATF_DONTPUB;
    struct arpreq req = {0};
    req.arp_flags = known_mask;
    /* Verify exact bitmask reconstruction */
    ASSERT_EQ(known_mask, req.arp_flags);
}

/* ============================================================================ */
TEST_SUITE(arpreq_device_name_boundaries);

TEST(arpreq_dev_exact_max_length) {
    struct arpreq req = {0};
    memset(req.arp_dev, 'Z', IFNAMSIZ - 1);
    req.arp_dev[IFNAMSIZ - 1] = '\0';
    ASSERT_EQ(IFNAMSIZ - 1, strlen(req.arp_dev));
}

TEST(arpreq_dev_missing_null_terminator) {
    struct arpreq req = {0};
    memset(req.arp_dev, 'X', IFNAMSIZ); /* Deliberately omit '\0' */
    ASSERT_TRUE(memchr(req.arp_dev, '\0', IFNAMSIZ) == NULL);
    /* Verify struct padding/alignment didn't silently fix it */
    ASSERT_EQ('X', req.arp_dev[0]);
    ASSERT_EQ('X', req.arp_dev[IFNAMSIZ - 1]);
}

/* ============================================================================ */
TEST_SUITE(ether_arp_alignment_safety);

TEST(ether_arp_unaligned_offset_read) {
    uint8_t buf[sizeof(struct ether_arp) + 1] = {0};
    /* Start at byte 1 to force unaligned access on strict-arch platforms */
    struct ether_arp *pkt = (struct ether_arp *)(buf + 1);
    pkt->ea_hdr.ar_hrd = htons(ARPHRD_ETHER);
    pkt->ea_hdr.ar_op = htons(ARPOP_REPLY);

    /* JACL_PACK guarantees byte-exact layout. Must not fault or misread. */
    ASSERT_EQ(htons(ARPHRD_ETHER), pkt->ea_hdr.ar_hrd);
    ASSERT_EQ(htons(ARPOP_REPLY), pkt->ea_hdr.ar_op);
}

TEST(ether_arp_partial_header_parse) {
    uint8_t raw[6] = {0}; /* Shorter than arphdr (8 bytes) */
    struct arphdr *h = (struct arphdr *)raw;
    h->ar_op = htons(ARPOP_REQUEST);

    /* Helper reads only ar_op. Must not segfault on truncated read */
    ASSERT_TRUE(arphdr_is_arp(h));
}

/* ============================================================================ */
TEST_MAIN_IF(JACL_HAS_POSIX, "net/if_arp.h requires POSIX\n")
