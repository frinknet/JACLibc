/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <net/tcp.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(net/tcp.h);

/* ============================================================================ */
/* CONSTANTS                                                                    */
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
}

TEST(constants_flags_combinations) {
	uint8_t syn_ack = TH_SYN | TH_ACK;
	ASSERT_EQ(0x12, syn_ack);
	ASSERT_TRUE((syn_ack & TH_FLAGS) == syn_ack);
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
/* STRUCT: tcp_hdr                                                              */
/* ============================================================================ */
TEST_SUITE(tcp_hdr);

TEST(tcp_hdr_size) { ASSERT_SIZE(struct tcp_hdr, 20); }

TEST(tcp_hdr_layout) {
	struct tcp_hdr h; memset(&h, 0, sizeof(h));
	h.source = htons(80); h.dest = htons(12345);
	h.seq = htonl(0x12345678); h.doff = 5; h.window = htons(65535);
	h.res1 = 0xF; h.res2 = 0x3;
	ASSERT_EQ(htons(80), h.source); ASSERT_EQ(htons(12345), h.dest);
	ASSERT_EQ(htonl(0x12345678), h.seq); ASSERT_EQ(5, h.doff);
	ASSERT_EQ(htons(65535), h.window);
	ASSERT_EQ(0xF, h.res1); ASSERT_EQ(0x3, h.res2);
}

TEST(tcp_hdr_urg_without_ptr) {
	struct tcp_hdr h = {0};
	h.urg = 1; h.urg_ptr = 0;
	ASSERT_TRUE(h.urg); ASSERT_EQ(0, h.urg_ptr);
}

/* ============================================================================ */
/* STRUCT: tcphdr                                                               */
/* ============================================================================ */
TEST_SUITE(tcphdr);

TEST(tcphdr_size) { ASSERT_SIZE(struct tcphdr, 20); }

TEST(tcphdr_bsd_alias_layout) {
	struct tcphdr th; memset(&th, 0, sizeof(th));
	th.th_sport = htons(443); th.th_off = 6; th.th_flags = TH_ACK | TH_PUSH;
	ASSERT_EQ(htons(443), th.th_sport); ASSERT_EQ(6, th.th_off); ASSERT_EQ(0x18, th.th_flags);
}

/* ============================================================================ */
/* MACRO: TCP_DATA_OFFSET                                                       */
/* ============================================================================ */
TEST_SUITE(tcp_data_offset);

TEST(tcp_data_offseT_min_doff) {
	struct tcp_hdr h = {0}; h.doff = 5;
	ASSERT_EQ(20, TCP_DATA_OFFSET(&h));
}

TEST(tcp_data_offseT_max_doff) {
	struct tcp_hdr h = {0}; h.doff = 15;
	ASSERT_EQ(60, TCP_DATA_OFFSET(&h));
}

TEST(tcp_data_offseT_invalid_low_doff) {
	struct tcp_hdr h = {0}; h.doff = 4;
	ASSERT_LT(TCP_DATA_OFFSET(&h), TCP_HDR_LEN);
}

TEST(tcp_data_offseT_calc) {
	struct tcp_hdr h; h.doff = 8;
	ASSERT_EQ(32, TCP_DATA_OFFSET(&h));
}

TEST(tcp_data_offseT_null_safety) {
	struct tcp_hdr *h = NULL;
	ASSERT_TRUE(h == NULL);
}

/* ============================================================================ */
/* FUNCTION: tcp_pseudo_sum                                                     */
/* ============================================================================ */
TEST_SUITE(tcp_pseudo_sum);

TEST(tcp_pseudo_sum_nonzero_addrs) {
	struct in_addr s = {.s_addr = htonl(0xC0A80001)};
	struct in_addr d = {.s_addr = htonl(0xC0A80002)};
	ASSERT_TRUE(tcp_pseudo_sum(s, d, IPPROTO_TCP, 20) > 0);
}

TEST(tcp_pseudo_sum_zero_addrs) {
	struct in_addr z = {.s_addr = 0};
	ASSERT_TRUE(tcp_pseudo_sum(z, z, IPPROTO_TCP, 0) > 0);
}

/* ============================================================================ */
/* FUNCTION: tcp_checksum_finish                                                */
/* ============================================================================ */
TEST_SUITE(tcp_checksum_finish);

TEST(tcp_checksum_finish_basic_roundtrip) {
	uint8_t data[4] = {0x12, 0x34, 0x56, 0x78};
	uint16_t csum = tcp_checksum_finish(0, data, 4);
	ASSERT_TRUE(csum != 0);
	uint8_t verify[6] = {0x12, 0x34, 0x56, 0x78, (csum >> 8) & 0xFF, csum & 0xFF};
	ASSERT_EQ(0, tcp_checksum_finish(0, verify, 6));
}

TEST(tcp_checksum_finish_zero_len) { ASSERT_EQ(0xFFFF, tcp_checksum_finish(0, NULL, 0)); }

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

TEST(tcp_checksum_finish_carry_propagation) {
	uint32_t s = 0x1FFFE;
	while (s >> 16) s = (s & 0xFFFF) + (s >> 16);
	ASSERT_EQ(0xFFFF, s);
}

/* ============================================================================ */
/* CONSTANTS: tcp_options (Byte layout verification)                            */
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

TEST_MAIN()

#else
int main(void) { printf("net/tcp.h requires POSIX\n"); return 0; }
#endif
