/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <net/icmp.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(net/icmp.h);

/* ============================================================================ */

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_icmp_types) {
	ASSERT_EQ(0, ICMP_ECHOREPLY);
	ASSERT_EQ(3, ICMP_DEST_UNREACH);
	ASSERT_EQ(4, ICMP_SOURCE_QUENCH);
	ASSERT_EQ(5, ICMP_REDIRECT);
	ASSERT_EQ(8, ICMP_ECHO);
	ASSERT_EQ(9, ICMP_ROUTER_ADVERT);
	ASSERT_EQ(10, ICMP_ROUTER_SOLICIT);
	ASSERT_EQ(11, ICMP_TIME_EXCEEDED);
	ASSERT_EQ(12, ICMP_PARAMPROB);
	ASSERT_EQ(13, ICMP_TIMESTAMP);
	ASSERT_EQ(14, ICMP_TIMESTAMPREPLY);
}

TEST(constants_icmp_unreach_codes) {
	ASSERT_EQ(0, ICMP_NET_UNREACH);
	ASSERT_EQ(1, ICMP_HOST_UNREACH);
	ASSERT_EQ(2, ICMP_PROT_UNREACH);
	ASSERT_EQ(3, ICMP_PORT_UNREACH);
	ASSERT_EQ(4, ICMP_FRAG_NEEDED);
	ASSERT_EQ(5, ICMP_SR_FAILED);
	ASSERT_EQ(9, ICMP_NET_PROHIB);
	ASSERT_EQ(10, ICMP_HOST_PROHIB);
	ASSERT_EQ(13, ICMP_FILTER_PROHIB);
	ASSERT_EQ(14, ICMP_PREC_VIOLATION);
	ASSERT_EQ(15, ICMP_PREC_CUTOFF);
}

TEST(constants_icmp_redirect_codes) {
	ASSERT_EQ(0, ICMP_REDIRECT_NET);
	ASSERT_EQ(1, ICMP_REDIRECT_HOST);
}

TEST(constants_icmp_time_exceeded_codes) {
	ASSERT_EQ(0, ICMP_EXC_TTL);
	ASSERT_EQ(1, ICMP_EXC_FRAGTIME);
}

TEST(constants_icmp6_types) {
	ASSERT_EQ(1, ICMP6_DST_UNREACH);
	ASSERT_EQ(2, ICMP6_PACKET_TOO_BIG);
	ASSERT_EQ(3, ICMP6_TIME_EXCEEDED);
	ASSERT_EQ(4, ICMP6_PARAM_PROB);
	ASSERT_EQ(128, ICMP6_ECHO_REQUEST);
	ASSERT_EQ(129, ICMP6_ECHO_REPLY);
	ASSERT_EQ(133, ICMP6_ND_ROUTER_SOLICIT);
	ASSERT_EQ(134, ICMP6_ND_ROUTER_ADVERT);
	ASSERT_EQ(135, ICMP6_ND_NEIGHBOR_SOLICIT);
	ASSERT_EQ(136, ICMP6_ND_NEIGHBOR_ADVERT);
	ASSERT_EQ(137, ICMP6_ND_REDIRECT);
}

TEST(constants_icmp6_codes) {
	/* Destination Unreachable */
	ASSERT_EQ(0, ICMP6_DST_UNREACH_NOROUTE);
	ASSERT_EQ(1, ICMP6_DST_UNREACH_ADMIN);
	ASSERT_EQ(2, ICMP6_DST_UNREACH_BEYONDSCOPE);
	ASSERT_EQ(3, ICMP6_DST_UNREACH_ADDR);
	ASSERT_EQ(4, ICMP6_DST_UNREACH_NOPORT);

	/* Time Exceeded */
	ASSERT_EQ(0, ICMP6_TIME_EXCEED_TRANSIT);
	ASSERT_EQ(1, ICMP6_TIME_EXCEED_REASSEMBLY);

	/* Parameter Problem */
	ASSERT_EQ(0, ICMP6_PARAMPROB_HEADER);
	ASSERT_EQ(1, ICMP6_PARAMPROB_NEXTHEADER);
	ASSERT_EQ(2, ICMP6_PARAMPROB_OPTION);
}

TEST(constants_nd_options) {
	ASSERT_EQ(1, ND_OPT_SOURCE_LINKADDR);
	ASSERT_EQ(2, ND_OPT_TARGET_LINKADDR);
	ASSERT_EQ(3, ND_OPT_PREFIX_INFORMATION);
	ASSERT_EQ(4, ND_OPT_REDIRECTED_HEADER);
	ASSERT_EQ(5, ND_OPT_MTU);
}

/* ============================================================================ */

TEST_SUITE(icmp);

TEST(icmp_bsd_alias_size) {
	ASSERT_SIZE(struct icmp, 8);
}

TEST(icmp_bsd_alias_fields) {
	struct icmp h;
	memset(&h, 0, sizeof(h));
	h.ic_type = ICMP_ECHOREPLY;
	h.icmp_id = 0xABCD;
	h.icmp_seq = 0xEF01;
	ASSERT_EQ(ICMP_ECHOREPLY, h.ic_type);
	ASSERT_EQ(0xABCD, h.icmp_id);
	ASSERT_EQ(0xEF01, h.icmp_seq);
}

TEST(icmp_id_alias_matches_union) {
    struct icmp pkt;
    memset(&pkt, 0, sizeof(pkt));

    /* Write via union */
    pkt.icmp_hun.ic_echo.icd_id = 0xBEEF;

    /* Read via macro */
    ASSERT_EQ(0xBEEF, pkt.icmp_id);
}

TEST(icmp_seq_alias_matches_union) {
    struct icmp pkt;
    memset(&pkt, 0, sizeof(pkt));

    pkt.icmp_hun.ic_echo.icd_seq = 0xCAFE;
    ASSERT_EQ(0xCAFE, pkt.icmp_seq);
}

/* ============================================================================ */

TEST_SUITE(icmp_hdr);

TEST(icmp_hdr_size) {
	ASSERT_SIZE(struct icmp_hdr, 8);
}

TEST(icmp_hdr_layout) {
	struct icmp_hdr h;
	memset(&h, 0, sizeof(h));
	h.type = ICMP_ECHO;
	h.un.echo.id = 0x1234;
	h.un.echo.sequence = 0x5678;
	ASSERT_EQ(ICMP_ECHO, h.type);
	ASSERT_EQ(0x1234, h.un.echo.id);
	ASSERT_EQ(0x5678, h.un.echo.sequence);
}

TEST(icmp_hdr_is_packed) {
    ASSERT_SIZE(struct icmp_hdr, 8);
    ASSERT_OFFSET(struct icmp_hdr, un, 4);
}

TEST(icmp6_hdr_is_packed) {
    ASSERT_SIZE(struct icmp6_hdr, 8);
    ASSERT_OFFSET(struct icmp6_hdr, icmp6_dataun, 4);
}

/* ============================================================================ */

TEST_SUITE(icmp_router_entry_struct);
TEST(icmp_router_entry_size) {
    /* 4 (addr) + 4 (preference) = 8 */
    ASSERT_SIZE(struct icmp_router_entry, 8);
}

/* ============================================================================ */

TEST_SUITE(icmp_router_adv_struct);

TEST(icmp_router_adv_size) {
	ASSERT_SIZE(struct icmp_router_adv, 8);
}

TEST(icmp_router_adv_layout) {
	struct icmp_router_adv ra;
	memset(&ra, 0, sizeof(ra));
	ra.num_addrs = 1;
	ra.addr_size = 8;
	ra.lifetime = 300;
	ASSERT_EQ(1, ra.num_addrs);
	ASSERT_EQ(300, ra.lifetime);
}

/* ============================================================================ */

TEST_SUITE(icmp_timestamp_struct);

TEST(icmp_timestamp_size) {
	ASSERT_SIZE(struct icmp_timestamp, 20);
}

TEST(icmp_timestamp_layout) {
	struct icmp_timestamp ts;
	memset(&ts, 0, sizeof(ts));
	ts.originate = 1; ts.receive = 2; ts.transmit = 3;
	ASSERT_EQ(1, ts.originate);
	ASSERT_EQ(3, ts.transmit);
}

/* ============================================================================ */

TEST_SUITE(icmp6_hdr);

TEST(icmp6_hdr_size) {
	ASSERT_SIZE(struct icmp6_hdr, 8);
}

TEST(icmp6_hdr_layout) {
	struct icmp6_hdr h;
	memset(&h, 0, sizeof(h));
	h.type = ICMP6_ECHO_REQUEST;
	h.icmp6_id = 0x9999;
	h.icmp6_seq = 0xAAAA;
	ASSERT_EQ(ICMP6_ECHO_REQUEST, h.type);
	ASSERT_EQ(0x9999, h.icmp6_id);
}

/* ============================================================================ */

TEST_SUITE(nd_neighbor_solicit);

TEST(nd_neighbor_solicit_size) {
	ASSERT_SIZE(struct nd_neighbor_solicit, 24);
}

TEST(nd_neighbor_solicit_layout) {
	struct nd_neighbor_solicit ns;
	memset(&ns, 0, sizeof(ns));
	ns.nd_ns_hdr.type = ICMP6_ND_NEIGHBOR_SOLICIT;
	ASSERT_EQ(ICMP6_ND_NEIGHBOR_SOLICIT, ns.nd_ns_hdr.type);

	/* Target must start immediately after header (8 bytes) */
	ASSERT_OFFSET(struct nd_neighbor_solicit, nd_ns_target, 8);
}

/* ============================================================================ */

TEST_SUITE(nd_neighbor_advert_flags);

TEST(nd_neighbor_advert) {
    struct nd_neighbor_advert na;
    memset(&na, 0, sizeof(na));

    /* Set Solicited flag (bit 30) */
    na.nd_na_flags_reserved = htonl(0x40000000);

    ASSERT_EQ(0x40000000, ntohl(na.nd_na_flags_reserved));
}
/* ============================================================================ */

TEST_SUITE(nd_router);

TEST(nd_router_advert_size) {
	ASSERT_SIZE(struct nd_router_advert, 20);
}

TEST(nd_router_adv_entry_parsing) {
    /* Simulate a packet with 2 router entries */
    uint8_t buf[24];
    memset(buf, 0, sizeof(buf));

    /* Header */
    struct icmp_router_adv *ra = (struct icmp_router_adv *)buf;
    ra->num_addrs = 2;
    ra->addr_size = 8; /* 4 bytes addr + 4 bytes pref */

    /* Entries */
    struct icmp_router_entry *e1 = (struct icmp_router_entry *)(buf + 8);
    struct icmp_router_entry *e2 = (struct icmp_router_entry *)(buf + 16);

    e1->addr = 0x01010101;
    e1->preference = 100;
    e2->addr = 0x02020202;
    e2->preference = 200;

    ASSERT_EQ(2, ra->num_addrs);
    ASSERT_EQ(0x01010101, e1->addr);
    ASSERT_EQ(100, e1->preference);
    ASSERT_EQ(0x02020202, e2->addr);
}

/* ============================================================================ */

TEST_SUITE(icmp_checksum);

TEST(icmp_checksum_1_byte) {
    uint8_t buf[1] = {0xFF};
    /* Sum: 0xFF00 -> Fold: 0xFF00 -> Invert: 0x00FF */
    ASSERT_EQ(0x00FF, icmp_checksum(buf, 1));
}

TEST(icmp_checksum_zero_buf) {
	uint8_t buf[8] = {0};
	ASSERT_EQ(0xFFFF, icmp_checksum(buf, 8));
}

TEST(icmp_checksum_echo_req) {
	uint8_t buf[8] = {8, 0, 0, 0, 0x12, 0x34, 0x56, 0x78};
	uint16_t csum = icmp_checksum(buf, 8);
	buf[2] = (csum >> 8) & 0xFF; /* High byte */
	buf[3] = csum & 0xFF;        /* Low byte */
	ASSERT_EQ(0x0000, icmp_checksum(buf, 8));
}

TEST(icmp_checksum_odd_len) {
	uint8_t buf[3] = {0xFF, 0xFF, 0xFF};
	uint16_t csum = icmp_checksum(buf, 3);
	ASSERT_TRUE(csum != 0);
}

TEST(icmp_checksum_odd_byte_padding) {
	/* 0xFF 0xFF 0xFF
	 * Word 1: 0xFFFF
	 * Word 2: 0xFF00 (last byte padded with 0 on right)
	 * Sum: 0x1FEFF -> Fold: 0xFF00
	 * Invert: 0x00FF
	 */
	uint8_t buf[3] = {0xFF, 0xFF, 0xFF};
	ASSERT_EQ(0x00FF, icmp_checksum(buf, 3));
}

TEST(icmp_checksum_carry_fold) {
    /* Force multiple carries */
    uint8_t buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    /* Sum: 0xFFFF + 0xFFFF = 0x1FFFE
       Fold: 0x1 + 0xFFFE = 0xFFFF
       One's comp: ~0xFFFF = 0x0000 */
    ASSERT_EQ(0x0000, icmp_checksum(buf, 4));
}

TEST(icmp_checksum_empty_buf) {
    /* Empty buffer should return 0xFFFF (inverse of 0) */
    ASSERT_EQ(0xFFFF, icmp_checksum(NULL, 0));
}

TEST_MAIN()

#else

int main(void) { printf("net/icmp.h requires POSIX\n"); return 0; }

#endif
