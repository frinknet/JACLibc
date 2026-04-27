/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#include <net/igmp.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(net/igmp.h);

/* ============================================================================ */

TEST_SUITE(macros);
TEST(igmp_membership_query) { ASSERT_EQ(0x11, IGMP_MEMBERSHIP_QUERY); }
TEST(igmp_v1_membership_report) { ASSERT_EQ(0x12, IGMP_V1_MEMBERSHIP_REPORT); }
TEST(igmp_v2_membership_report) { ASSERT_EQ(0x16, IGMP_V2_MEMBERSHIP_REPORT); }
TEST(igmp_v3_membership_report) { ASSERT_EQ(0x22, IGMP_V3_MEMBERSHIP_REPORT); }
TEST(igmp_leave_group) { ASSERT_EQ(0x17, IGMP_LEAVE_GROUP); }

TEST(igmp_mode_is_include) { ASSERT_EQ(1, IGMP_MODE_IS_INCLUDE); }
TEST(igmp_mode_is_exclude) { ASSERT_EQ(2, IGMP_MODE_IS_EXCLUDE); }
TEST(igmp_change_to_include) { ASSERT_EQ(3, IGMP_CHANGE_TO_INCLUDE); }
TEST(igmp_change_to_exclude) { ASSERT_EQ(4, IGMP_CHANGE_TO_EXCLUDE); }
TEST(igmp_allow_new_sources) { ASSERT_EQ(5, IGMP_ALLOW_NEW_SOURCES); }
TEST(igmp_block_old_sources) { ASSERT_EQ(6, IGMP_BLOCK_OLD_SOURCES); }

TEST(igmp_minlen) { ASSERT_EQ(8, IGMP_MINLEN); }
TEST(igmp_maxlen) { ASSERT_EQ(576, IGMP_MAXLEN); }
TEST(igmp_timer_scale) { ASSERT_EQ(10, IGMP_TIMER_SCALE); }
TEST(igmp_all_hosts) { ASSERT_EQ(htonl(0xE0000001), IGMP_ALL_HOSTS); }
TEST(igmp_all_routers) { ASSERT_EQ(htonl(0xE0000002), IGMP_ALL_ROUTERS); }
TEST(igmp_v3_all_routers) { ASSERT_EQ(htonl(0xE0000016), IGMP_V3_ALL_ROUTERS); }

/* ============================================================================ */

TEST_SUITE(struct_igmp);

TEST(struct_igmp_size) {
	ASSERT_EQ(8, sizeof(struct igmp));
}

TEST(struct_igmp_offsets) {
	struct igmp h;
	ASSERT_EQ(0, (size_t)&h.igmp_type - (size_t)&h);
	ASSERT_EQ(1, (size_t)&h.igmp_code - (size_t)&h);
	ASSERT_EQ(2, (size_t)&h.igmp_cksum - (size_t)&h);
	ASSERT_EQ(4, (size_t)&h.igmp_group - (size_t)&h);
}

TEST(struct_igmp_zero_init) {
	struct igmp h;
	memset(&h, 0, sizeof(h));
	ASSERT_EQ(0, h.igmp_type);
	ASSERT_EQ(0, h.igmp_code);
	ASSERT_EQ(0, h.igmp_cksum);
	ASSERT_EQ(0, h.igmp_group.s_addr);
}

TEST(struct_igmp_max_values) {
	struct igmp h;
	memset(&h, 0, sizeof(h));
	h.igmp_type = 0xFF;
	h.igmp_code = 0xFF;
	h.igmp_cksum = 0xFFFF;
	h.igmp_group.s_addr = 0xFFFFFFFF;

	ASSERT_EQ(0xFF, h.igmp_type);
	ASSERT_EQ(0xFF, h.igmp_code);
	ASSERT_EQ(0xFFFF, h.igmp_cksum);
	ASSERT_EQ(0xFFFFFFFF, h.igmp_group.s_addr);
}

TEST(struct_igmp_wire_format) {
	struct igmp h;
	memset(&h, 0, sizeof(h));
	h.igmp_type = 0x12;
	h.igmp_code = 0x34;
	h.igmp_cksum = htons(0x5678);
	h.igmp_group.s_addr = htonl(0xAABBCCDD);

	uint8_t *wire = (uint8_t *)&h;
	ASSERT_EQ(0x12, wire[0]);
	ASSERT_EQ(0x34, wire[1]);
	ASSERT_EQ(0x56, wire[2]);
	ASSERT_EQ(0x78, wire[3]);
	ASSERT_EQ(0xAA, wire[4]);
	ASSERT_EQ(0xBB, wire[5]);
	ASSERT_EQ(0xCC, wire[6]);
	ASSERT_EQ(0xDD, wire[7]);
}

TEST(struct_igmp_roundtrip) {
	uint8_t raw[8] = {0x11, 0x0A, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x01};
	struct igmp *h = (struct igmp *)raw;

	ASSERT_EQ(0x11, h->igmp_type);
	ASSERT_EQ(0x0A, h->igmp_code);
	ASSERT_EQ(0, h->igmp_cksum);
	ASSERT_EQ(htonl(0xE0000001), h->igmp_group.s_addr);
}

TEST(struct_igmp_alias_size) {
	ASSERT_EQ(sizeof(struct igmp), sizeof(struct igmp_hdr));
}

TEST(struct_igmp_alias_bidirectional) {
	struct igmp orig;
	memset(&orig, 0, sizeof(orig));
	orig.igmp_type = IGMP_LEAVE_GROUP;
	orig.igmp_code = 0x05;
	orig.igmp_cksum = 0xDEAD;
	orig.igmp_group.s_addr = htonl(0xE0000002);

	struct igmp_hdr *alias = (struct igmp_hdr *)&orig;

	ASSERT_EQ(IGMP_LEAVE_GROUP, alias->type);
	ASSERT_EQ(0x05, alias->max_resp_time);
	ASSERT_EQ(0xDEAD, alias->checksum);
	ASSERT_EQ(htonl(0xE0000002), alias->group.s_addr);
}

TEST(struct_igmp_alias_no_padding) {
	ASSERT_EQ(sizeof(struct igmp),
			  sizeof(uint8_t) + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(struct in_addr));
}

/* ============================================================================ */

TEST_SUITE(igmpv3_query);

TEST(igmpv3_query_size) {
	ASSERT_EQ(12, sizeof(struct igmpv3_query));
}

TEST(igmpv3_query_zero_init) {
	struct igmpv3_query q;
	memset(&q, 0, sizeof(q));
	ASSERT_EQ(0, q.type);
	ASSERT_EQ(0, q.max_resp_time);
	ASSERT_EQ(0, q.checksum);
	ASSERT_EQ(0, q.group.s_addr);
	ASSERT_EQ(0, q.qqic);
	ASSERT_EQ(0, ntohs(q.num_src));
}

TEST(igmpv3_query_max_bounds) {
	struct igmpv3_query q;
	memset(&q, 0, sizeof(q));
	q.type = 0xFF;
	q.max_resp_time = 0xFF;
	q.checksum = 0xFFFF;
	q.qqic = 0xFF;
	q.num_src = 0xFFFF;

	ASSERT_EQ(0xFF, q.type);
	ASSERT_EQ(0xFF, q.max_resp_time);
	ASSERT_EQ(0xFFFF, q.checksum);
	ASSERT_EQ(0xFF, q.qqic);
	ASSERT_EQ(0xFFFF, ntohs(q.num_src));
}

TEST(igmpv3_query_wire_format) {
	struct igmpv3_query q;
	memset(&q, 0, sizeof(q));
	q.type = 0x11;
	q.max_resp_time = 0x22;
	q.checksum = htons(0x3344);
	q.group.s_addr = htonl(0xAABBCCDD);
	q.qqic = 0x55;
	q.num_src = htons(0x6677);

	uint8_t *wire = (uint8_t *)&q;
	ASSERT_EQ(0x11, wire[0]);
	ASSERT_EQ(0x22, wire[1]);
	ASSERT_EQ(0x33, wire[2]);
	ASSERT_EQ(0x44, wire[3]);
	ASSERT_EQ(0xAA, wire[4]);
	ASSERT_EQ(0xBB, wire[5]);
	ASSERT_EQ(0xCC, wire[6]);
	ASSERT_EQ(0xDD, wire[7]);
	ASSERT_EQ(0x00, wire[8]);
	ASSERT_EQ(0x55, wire[9]);
	ASSERT_EQ(0x66, wire[10]);
	ASSERT_EQ(0x77, wire[11]);
}

TEST(igmpv3_query_bitfield_byte) {
	struct igmpv3_query q;
	memset(&q, 0, sizeof(q));
	uint8_t *flags = (uint8_t *)&q + 8;
	*flags = 0xB0;
	ASSERT_EQ(0xB0, *flags);
}

TEST(igmpv3_query_roundtrip) {
	uint8_t raw[12] = {
		0x11, 0x64, 0x00, 0x00,
		0xE0, 0x00, 0x00, 0x01,
		0x20, 0x1E, 0x00, 0x03
	};
	struct igmpv3_query *q = (struct igmpv3_query *)raw;

	ASSERT_EQ(0x11, q->type);
	ASSERT_EQ(0x64, q->max_resp_time);
	ASSERT_EQ(0, q->checksum);
	ASSERT_EQ(htonl(0xE0000001), q->group.s_addr);
	ASSERT_EQ(0x20, *(uint8_t*)(&q->qqic - 1));
	ASSERT_EQ(0x1E, q->qqic);
	ASSERT_EQ(3, ntohs(q->num_src));
}

TEST(igmpv3_query_total_size_calc) {
	uint16_t num_src = 5;
	size_t total = sizeof(struct igmpv3_query) + (num_src * sizeof(struct in_addr));
	ASSERT_EQ(12 + (5 * 4), total);
}

/* ============================================================================ */

TEST_SUITE(igmpv3_record);

TEST(igmpv3_record_size) { ASSERT_EQ(8, sizeof(struct igmpv3_grec)); }

TEST(igmpv3_record_offsets) {
	struct igmpv3_grec r;
	ASSERT_EQ(0, (size_t)&r.grec_type - (size_t)&r);
	ASSERT_EQ(1, (size_t)&r.grec_auxlen - (size_t)&r);
	ASSERT_EQ(2, (size_t)&r.grec_nsrcs - (size_t)&r);
	ASSERT_EQ(4, (size_t)&r.grec_mca - (size_t)&r);
}

TEST(igmpv3_record_wire_format) {
	struct igmpv3_grec r;
	memset(&r, 0, sizeof(r));
	r.grec_type = IGMP_MODE_IS_EXCLUDE;
	r.grec_auxlen = 2;
	r.grec_nsrcs = htons(3);
	r.grec_mca.s_addr = htonl(0xE0000002);

	uint8_t *wire = (uint8_t *)&r;
	ASSERT_EQ(IGMP_MODE_IS_EXCLUDE, wire[0]);
	ASSERT_EQ(2, wire[1]);
	ASSERT_EQ(0x00, wire[2]);
	ASSERT_EQ(0x03, wire[3]);
	ASSERT_EQ(0xE0, wire[4]);
	ASSERT_EQ(0x00, wire[5]);
	ASSERT_EQ(0x00, wire[6]);
	ASSERT_EQ(0x02, wire[7]);
}

TEST(igmpv3_record_aux_data_calc) {
	struct igmpv3_grec r;
	memset(&r, 0, sizeof(r));
	r.grec_nsrcs = htons(2);
	r.grec_auxlen = 1;

	size_t record_size = sizeof(struct igmpv3_grec) +
	                     (ntohs(r.grec_nsrcs) * sizeof(struct in_addr)) +
	                     (r.grec_auxlen * 4);
	ASSERT_EQ(8 + 8 + 4, record_size);
}

/* ============================================================================ */

TEST_SUITE(igmpv3_report);

TEST(igmpv3_report_size) { ASSERT_EQ(8, sizeof(struct igmpv3_report)); }

TEST(igmpv3_report_offsets) {
	struct igmpv3_report rpt;
	ASSERT_EQ(0, (size_t)&rpt.type - (size_t)&rpt);
	ASSERT_EQ(1, (size_t)&rpt.resv1 - (size_t)&rpt);
	ASSERT_EQ(2, (size_t)&rpt.checksum - (size_t)&rpt);
	ASSERT_EQ(4, (size_t)&rpt.resv2 - (size_t)&rpt);
	ASSERT_EQ(6, (size_t)&rpt.num_grps - (size_t)&rpt);
}

TEST(igmpv3_report_wire_format) {
	struct igmpv3_report rpt;
	memset(&rpt, 0, sizeof(rpt));
	rpt.type = 0x22;
	rpt.checksum = htons(0x1234);
	rpt.num_grps = htons(2);

	uint8_t *wire = (uint8_t *)&rpt;
	ASSERT_EQ(0x22, wire[0]);
	ASSERT_EQ(0x00, wire[1]);
	ASSERT_EQ(0x12, wire[2]);
	ASSERT_EQ(0x34, wire[3]);
	ASSERT_EQ(0x00, wire[4]);
	ASSERT_EQ(0x00, wire[5]);
	ASSERT_EQ(0x00, wire[6]);
	ASSERT_EQ(0x02, wire[7]);
}

TEST(igmpv3_report_roundtrip) {
	uint8_t raw[8] = {0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
	struct igmpv3_report *rpt = (struct igmpv3_report *)raw;

	ASSERT_EQ(0x22, rpt->type);
	ASSERT_EQ(0, rpt->checksum);
	ASSERT_EQ(0, rpt->resv1);
	ASSERT_EQ(0, rpt->resv2);
	ASSERT_EQ(1, ntohs(rpt->num_grps));
}

TEST(igmpv3_report_total_size_calc) {
	uint16_t num_grps = 3;
	uint16_t num_src_per_grp = 4;
	size_t total = sizeof(struct igmpv3_report) +
	               (num_grps * (sizeof(struct igmpv3_grec) + (num_src_per_grp * 4)));
	ASSERT_EQ(8 + (3 * (8 + 16)), total);
}

/* ============================================================================ */

TEST_SUITE(igmp_checksum);

TEST(igmp_checksum_zero) {
	uint8_t buf[8] = {0};
	ASSERT_EQ(0xFFFF, igmp_checksum(buf, 8));
}

TEST(igmp_checksum_verify) {
	uint8_t buf[8] = {0x11, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x01};
	uint16_t csum = igmp_checksum(buf, 8);
	buf[2] = csum & 0xFF;
	buf[3] = (csum >> 8) & 0xFF;
	ASSERT_EQ(0x0000, igmp_checksum(buf, 8));
}

TEST(igmp_checksum_all_ones) {
	uint8_t buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	ASSERT_EQ(0x0000, igmp_checksum(buf, 4));
}

TEST(igmp_checksum_odd_len) {
	uint8_t buf[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
	uint16_t csum = igmp_checksum(buf, 5);
	ASSERT_TRUE(csum != igmp_checksum(buf, 4));
}

TEST(igmp_checksum_carry_fold) {
	uint8_t buf[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x01};
	uint16_t csum = igmp_checksum(buf, 8);
	ASSERT_TRUE(csum != 0xFFFF);
}

/* ============================================================================ */

TEST_SUITE(igmp_is_multicast);

TEST(igmp_is_multicast_lower_boundary) {
	struct in_addr a = {.s_addr = htonl(0xE0000000)};
	ASSERT_TRUE(igmp_is_multicast(a));
}

TEST(igmp_is_multicast_upper_boundary) {
	struct in_addr a = {.s_addr = htonl(0xEFFFFFFF)};
	ASSERT_TRUE(igmp_is_multicast(a));
}

TEST(igmp_is_multicast_just_below) {
	struct in_addr a = {.s_addr = htonl(0xDFFFFFFF)};
	ASSERT_FALSE(igmp_is_multicast(a));
}

TEST(igmp_is_multicast_just_above) {
	struct in_addr a = {.s_addr = htonl(0xF0000000)};
	ASSERT_FALSE(igmp_is_multicast(a));
}

/* ============================================================================ */

TEST_SUITE(igmp_is_local_control);

TEST(igmp_is_local_control_lower_boundary) {
	struct in_addr a = {.s_addr = htonl(0xE0000000)};
	ASSERT_TRUE(igmp_is_local_control(a));
}

TEST(igmp_is_local_control_upper_boundary) {
	struct in_addr a = {.s_addr = htonl(0xE00000FF)};
	ASSERT_TRUE(igmp_is_local_control(a));
}

TEST(igmp_is_local_control_just_above) {
	struct in_addr a = {.s_addr = htonl(0xE0000100)};
	ASSERT_FALSE(igmp_is_local_control(a));
}

TEST(igmp_is_local_control_non_multicast) {
	struct in_addr a = {.s_addr = htonl(0x7F000001)};
	ASSERT_FALSE(igmp_is_local_control(a));
}

/* ============================================================================ */

TEST_SUITE(igmp_valid_len);

TEST(igmp_valid_len_query) {
	ASSERT_TRUE(igmp_valid_len(IGMP_MEMBERSHIP_QUERY, 8));
	ASSERT_FALSE(igmp_valid_len(IGMP_MEMBERSHIP_QUERY, 7));
}

TEST(igmp_valid_len_v1_report) {
	ASSERT_TRUE(igmp_valid_len(IGMP_V1_MEMBERSHIP_REPORT, 8));
}

TEST(igmp_valid_len_leave) {
	ASSERT_TRUE(igmp_valid_len(IGMP_LEAVE_GROUP, 8));
	ASSERT_FALSE(igmp_valid_len(IGMP_LEAVE_GROUP, 7));
}

TEST(igmp_valid_len_exact_min) {
	ASSERT_TRUE(igmp_valid_len(IGMP_V3_MEMBERSHIP_REPORT, IGMP_MINLEN));
}

TEST(igmp_valid_len_one_below_min) {
	ASSERT_FALSE(igmp_valid_len(IGMP_V3_MEMBERSHIP_REPORT, IGMP_MINLEN - 1));
}

TEST(igmp_valid_len_zero_len) {
	ASSERT_FALSE(igmp_valid_len(IGMP_MEMBERSHIP_QUERY, 0));
}

/* ============================================================================ */

TEST_MAIN_IF(JACL_HAS_POSIX, "net/igmp.h requires POSIX\n")
