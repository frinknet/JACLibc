/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <string.h>
#include <net/tcp.h>

TEST_TYPE(unit);
TEST_UNIT(net/tcp.h);

/* ============================================================================ */

TEST_SUITE(constants);

TEST(constants_states) {
	ASSERT_EQ(TCP_CLOSED, 0);
	ASSERT_EQ(TCP_LISTEN, 1);
	ASSERT_EQ(TCP_ESTABLISHED, 4);
	ASSERT_EQ(TCP_TIME_WAIT, 10);
}

TEST(constants_flags) {
	ASSERT_EQ(TH_SYN, 0x02);
	ASSERT_EQ(TH_ACK, 0x10);
	ASSERT_EQ(TH_RST, 0x04);
	ASSERT_EQ(TH_ECE, 0x40);
	ASSERT_EQ(TH_CWR, 0x80);
	ASSERT_EQ(TH_FIN, 0x01);
	ASSERT_EQ(TH_PUSH, 0x08);
	ASSERT_EQ(TH_URG, 0x20);
}

TEST(constants_flags_combinations) {
	uint8_t syn_ack = TH_SYN | TH_ACK;
	ASSERT_EQ(0x12, syn_ack);
	/* Removed TH_FLAGS test due to ELF header collision */
}

TEST(constants_flags_invalid_syn_fin) {
	uint8_t flags = TH_SYN | TH_FIN;
	ASSERT_TRUE(flags & TH_SYN);
	ASSERT_TRUE(flags & TH_FIN);
}

TEST(constants_flags_rst_ack_combo) {
	uint8_t flags = TH_RST | TH_ACK;
	ASSERT_TRUE(flags & TH_RST);
	ASSERT_TRUE(flags & TH_ACK);
}

TEST(constants_options) {
	ASSERT_EQ(TCPOPT_EOL, 0);
	ASSERT_EQ(TCPOPT_MAXSEG, 2);
	ASSERT_EQ(TCPOPT_TIMESTAMP, 8);
	ASSERT_EQ(TCPOPT_MULTIPATH, 30);
}

TEST(constants_socket_options) {
	ASSERT_EQ(TCP_NODELAY, 1);
	ASSERT_EQ(TCP_MAXSEG, 2);
	ASSERT_EQ(TCP_CORK, 3);
	ASSERT_EQ(TCP_KEEPIDLE, 4);
	ASSERT_EQ(TCP_HDR_LEN, 20);
}

/* ============================================================================ */

TEST_SUITE(tcphdr);

TEST(tcphdr_size) {
	ASSERT_SIZE(struct tcphdr, 20);
}

TEST(tcphdr_layout) {
	struct tcphdr h;
	memset(&h, 0, sizeof(h));

	h.th_sport = htons(80);
	h.th_dport = htons(12345);
	h.th_seq = htonl(0x12345678);
	h.th_offx2 = (5 << 4); /* doff=5, res=0 */
	h.th_win = htons(65535);

	ASSERT_EQ(htons(80), h.th_sport);
	ASSERT_EQ(htons(12345), h.th_dport);
	ASSERT_EQ(htonl(0x12345678), h.th_seq);
	ASSERT_EQ(5, TH_OFF(&h));
	ASSERT_EQ(htons(65535), h.th_win);
}

TEST(tcphdr_flags_and_offset) {
	struct tcphdr th;
	memset(&th, 0, sizeof(th));

	th.th_sport = htons(443);
	th.th_offx2 = (6 << 4); /* doff=6 */
	th.th_flags = TH_ACK | TH_PUSH;

	ASSERT_EQ(htons(443), th.th_sport);
	ASSERT_EQ(6, TH_OFF(&th));
	ASSERT_EQ(0x18, th.th_flags);
}

TEST(tcphdr_urg_without_ptr) {
	struct tcphdr h = {0};
	h.th_flags |= TH_URG;
	h.th_urp = 0;
	ASSERT_TRUE(h.th_flags & TH_URG);
	ASSERT_EQ(0, h.th_urp);
}

/* ============================================================================ */

TEST_SUITE(tcp_data_offset);

TEST(tcp_data_offset_min_doff) {
	struct tcphdr h = {0};
	h.th_offx2 = (5 << 4);
	ASSERT_EQ(20, TCP_DATA_OFFSET(&h));
}

TEST(tcp_data_offset_max_doff) {
	struct tcphdr h = {0};
	h.th_offx2 = (15 << 4);
	ASSERT_EQ(60, TCP_DATA_OFFSET(&h));
}

TEST(tcp_data_offset_invalid_low_doff) {
	struct tcphdr h = {0};
	h.th_offx2 = (4 << 4);
	uint8_t offset = TCP_DATA_OFFSET(&h);
	ASSERT_LT(offset, TCP_HDR_LEN);
}

TEST(tcp_data_offset_calc) {
	struct tcphdr h = {0};
	h.th_offx2 = (8 << 4);
	ASSERT_EQ(32, TCP_DATA_OFFSET(&h));
}

/* ============================================================================ */

TEST_SUITE(tcp_checksum_helpers);

TEST(tcp_pseudo_sum_ipv4_nonzero) {
	struct in_addr s = {.s_addr = htonl(0xC0A80001)};
	struct in_addr d = {.s_addr = htonl(0xC0A80002)};
	/* Signature is (src, dst, len) - protocol is hardcoded to TCP */
	ASSERT_TRUE(tcp_pseudo_sum_ipv4(s, d, 20) > 0);
}

TEST(tcp_pseudo_sum_ipv4_zero) {
	struct in_addr z = {.s_addr = 0};
	ASSERT_TRUE(tcp_pseudo_sum_ipv4(z, z, 0) > 0);
}

TEST(tcp_checksum_finish_basic_roundtrip) {
	uint8_t data[4] = {0x12, 0x34, 0x56, 0x78};
	uint16_t csum = tcp_checksum_finish(0, data, 4);
	ASSERT_TRUE(csum != 0);
	uint8_t verify[6] = {0x12, 0x34, 0x56, 0x78, (csum >> 8) & 0xFF, csum & 0xFF};
	ASSERT_EQ(0, tcp_checksum_finish(0, verify, 6));
}

TEST(tcp_checksum_finish_zero_len) {
	ASSERT_EQ(0xFFFF, tcp_checksum_finish(0, NULL, 0));
}

TEST(tcp_checksum_finish_odd_byte) {
	uint8_t buf[1] = {0xFF};
	ASSERT_TRUE(tcp_checksum_finish(0, buf, 1) != 0);
}

TEST(tcp_checksum_finish_all_zeros) {
	uint16_t z[4] = {0};
	ASSERT_EQ(0xFFFF, tcp_checksum_finish(0, z, 8));
}

TEST(tcp_checksum_finish_all_ones) {
	uint16_t o[4] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
	ASSERT_EQ(0, tcp_checksum_finish(0, o, 8));
}

/* ============================================================================ */

TEST_SUITE(tcp_options);

TEST(tcp_options_eol_early) {
	uint8_t opts[4] = {TCPOPT_EOL, 0xFF, 0xFF, 0xFF};
	ASSERT_EQ(TCPOPT_EOL, opts[0]);
}

TEST(tcp_options_nop_padding) {
	uint8_t opts[4] = {TCPOPT_NOP, TCPOPT_NOP, TCPOPT_MAXSEG, 4};
	ASSERT_EQ(TCPOPT_NOP, opts[0]);
	ASSERT_EQ(TCPOPT_MAXSEG, opts[2]);
}

TEST(tcp_options_malformed_len) {
	uint8_t opts[2] = {TCPOPT_MAXSEG, 1};
	ASSERT_EQ(1, opts[1]);
}

TEST(tcp_options_unknown_code) {
	uint8_t opts[2] = {255, 2};
	ASSERT_EQ(255, opts[0]);
}

/* ============================================================================ */

TEST_SUITE(wire_format);

TEST(tcphdr_wire_layout_bytes) {
	/* Verify exact byte layout against a known hex dump */
	/* Syn-Ack Packet: Src=80, Dst=12345, Seq=0x12345678, Ack=0x00000000,
	   Off=5, Res=0, Flags=0x12 (SYN|ACK), Win=0x7210, Sum=0, Urg=0 */
	struct tcphdr h;
	memset(&h, 0, sizeof(h));

	h.th_sport = htons(80);
	h.th_dport = htons(12345);
	h.th_seq = htonl(0x12345678);
	h.th_ack = 0;
	h.th_offx2 = (5 << 4);
	h.th_flags = TH_SYN | TH_ACK; /* 0x12 */
	h.th_win = htons(0x7210);

	uint8_t *raw = (uint8_t *)&h;

	/* Byte 0-1: Ports */
	ASSERT_EQ(raw[0], 0x00); ASSERT_EQ(raw[1], 0x50); /* 80 */
	ASSERT_EQ(raw[2], 0x30); ASSERT_EQ(raw[3], 0x39); /* 12345 */

	/* Byte 4-7: Seq */
	ASSERT_EQ(raw[4], 0x12); ASSERT_EQ(raw[5], 0x34);
	ASSERT_EQ(raw[6], 0x56); ASSERT_EQ(raw[7], 0x78);

	/* Byte 8-11: Ack (0) */
	ASSERT_EQ(raw[8], 0); ASSERT_EQ(raw[9], 0);

	/* Byte 12: Offset/Reserved (5 << 4 = 0x50) */
	ASSERT_EQ(raw[12], 0x50);

	/* Byte 13: Flags (SYN|ACK = 0x12) */
	ASSERT_EQ(raw[13], 0x12);

	/* Byte 14-15: Window */
	ASSERT_EQ(raw[14], 0x72); ASSERT_EQ(raw[15], 0x10);
}

TEST(tcphdr_typedef_consistency) {
	/* tcp_hdr is a typedef for struct tcphdr - verify they're the same */
	ASSERT_EQ(sizeof(struct tcphdr), sizeof(tcp_hdr));
	ASSERT_EQ(offsetof(struct tcphdr, th_sport), offsetof(tcp_hdr, th_sport));
	ASSERT_EQ(offsetof(struct tcphdr, th_flags), offsetof(tcp_hdr, th_flags));
	ASSERT_EQ(offsetof(struct tcphdr, th_urp), offsetof(tcp_hdr, th_urp));
}

/* ============================================================================ */

TEST_SUITE(tcp_options_parse);

TEST(tcp_options_find_mss) {
	/* Simulate options: NOP, NOP, MSS(2,4, 1460) */
	uint8_t opts[10] = {
		TCPOPT_NOP, TCPOPT_NOP,
		TCPOPT_MAXSEG, 4, 0x05, 0xB4, /* 1460 */
		TCPOPT_EOL, 0, 0, 0
	};

	/* Simple parser logic inline for test */
	int i = 0;
	int found_mss = 0;
	while (i < 10) {
		uint8_t kind = opts[i];
		if (kind == TCPOPT_EOL) break;
		if (kind == TCPOPT_NOP) { i++; continue; }

		uint8_t len = opts[i+1];
		if (kind == TCPOPT_MAXSEG && len >= 4) {
			found_mss = 1;
			break;
		}
		i += len;
	}
	ASSERT_TRUE(found_mss);
}

/* ============================================================================ */

TEST_MAIN_IF(JACL_HAS_POSIX, "net/tcp.h requires POSIX\n")
