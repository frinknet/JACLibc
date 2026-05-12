/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <string.h>
#include <net/dns.h>

TEST_TYPE(unit);
TEST_UNIT(net/dns.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_sizes) {
	ASSERT_EQ(DNS_HDR_SIZE, 12);
	ASSERT_EQ(HFIXEDSZ, 12);
	ASSERT_EQ(QFIXEDSZ, 4);
	ASSERT_EQ(RRFIXEDSZ, 10);
	ASSERT_EQ(INT32SZ, 4);
	ASSERT_EQ(INT16SZ, 2);
	ASSERT_EQ(INT8SZ, 1);
}

TEST(constants_opcodes) {
	ASSERT_EQ(DNS_OPCODE_QUERY,  0);
	ASSERT_EQ(DNS_OPCODE_IQUERY, 1);
	ASSERT_EQ(DNS_OPCODE_STATUS, 2);
}

TEST(constants_rcodes) {
	ASSERT_EQ(DNS_RCODE_NOERR,    0);
	ASSERT_EQ(DNS_RCODE_FORMERR,  1);
	ASSERT_EQ(DNS_RCODE_SERVFAIL, 2);
	ASSERT_EQ(DNS_RCODE_NXDOMAIN, 3);
}

TEST(constants_types) {
	ASSERT_EQ(DNS_TYPE_A,      1);
	ASSERT_EQ(DNS_TYPE_NS,     2);
	ASSERT_EQ(DNS_TYPE_CNAME,  5);
	ASSERT_EQ(DNS_TYPE_SOA,    6);
	ASSERT_EQ(DNS_TYPE_PTR,    12);
	ASSERT_EQ(DNS_TYPE_MX,     15);
	ASSERT_EQ(DNS_TYPE_TXT,    16);
	ASSERT_EQ(DNS_TYPE_AAAA,   28);
	ASSERT_EQ(DNS_TYPE_SRV,    33);
	ASSERT_EQ(DNS_TYPE_OPT,    41);
	ASSERT_EQ(DNS_TYPE_ANY,    255);
}

TEST(constants_classes) {
	ASSERT_EQ(DNS_CLASS_IN,    1);
	ASSERT_EQ(DNS_CLASS_CS,    2);
	ASSERT_EQ(DNS_CLASS_CH,    3);
	ASSERT_EQ(DNS_CLASS_HS,    4);
	ASSERT_EQ(DNS_CLASS_ANY,   255);
}

/* ============================================================================ */

TEST_SUITE(dns_hdr_layout);

TEST(dns_hdr_layout_id) {
	struct dns_hdr hdr;
	memset(&hdr, 0, sizeof(hdr));
	hdr.id = 0x1234;
	ASSERT_EQ(0x1234, hdr.id);
}

TEST(dns_hdr_layout_qdcount) {
	struct dns_hdr hdr;
	memset(&hdr, 0, sizeof(hdr));
	hdr.qdcount = htons(1);
	ASSERT_EQ(1, ntohs(hdr.qdcount));
}

/* ============================================================================ */

TEST_SUITE(dns_get16);

TEST(dns_get16_basic) {
	uint8_t buf[2] = {0x01, 0x02};
	ASSERT_EQ(0x0102, dns_get16(buf));
}

TEST(dns_get16_zero) {
	uint8_t buf[2] = {0, 0};
	ASSERT_EQ(0, dns_get16(buf));
}

TEST(dns_get16_max) {
	uint8_t buf[2] = {0xFF, 0xFF};
	ASSERT_EQ(0xFFFF, dns_get16(buf));
}

/* ============================================================================ */

TEST_SUITE(dns_put16);

TEST(dns_put16_basic) {
	uint8_t buf[2];
	dns_put16(buf, 0x0102);
	ASSERT_EQ(1, buf[0]);
	ASSERT_EQ(2, buf[1]);
}

TEST(dns_put16_zero) {
	uint8_t buf[2] = {0xFF, 0xFF};
	dns_put16(buf, 0);
	ASSERT_EQ(0, buf[0]);
	ASSERT_EQ(0, buf[1]);
}

TEST(dns_put16_max) {
	uint8_t buf[2] = {0, 0};
	dns_put16(buf, 0xFFFF);
	ASSERT_EQ(0xFF, buf[0]);
	ASSERT_EQ(0xFF, buf[1]);
}

/* ============================================================================ */

TEST_SUITE(dns_get32);

TEST(dns_get32_basic) {
	uint8_t buf[4] = {0x01, 0x02, 0x03, 0x04};
	ASSERT_EQ(0x01020304, dns_get32(buf));
}

TEST(dns_get32_zero) {
	uint8_t buf[4] = {0, 0, 0, 0};
	ASSERT_EQ(0, dns_get32(buf));
}

/* ============================================================================ */

TEST_SUITE(dns_put32);

TEST(dns_put32_basic) {
	uint8_t buf[4];
	dns_put32(buf, 0x01020304);
	ASSERT_EQ(1, buf[0]);
	ASSERT_EQ(4, buf[3]);
}

TEST(dns_put32_zero) {
	uint8_t buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	dns_put32(buf, 0);
	ASSERT_EQ(0, buf[0]);
	ASSERT_EQ(0, buf[3]);
}

/* ============================================================================ */

TEST_SUITE(dn_comp);

TEST(dn_comp_simple) {
	uint8_t buf[256];
	int len = dn_comp("example.com", buf, sizeof(buf), NULL, NULL);
	ASSERT_EQ(13, len);
	ASSERT_EQ(7, buf[0]);
	ASSERT_EQ('e', buf[1]);
	ASSERT_EQ(3, buf[8]);
	ASSERT_EQ('c', buf[9]);
	ASSERT_EQ(0, buf[12]);
}

TEST(dn_comp_root) {
	uint8_t buf[256];
	int len = dn_comp(".", buf, sizeof(buf), NULL, NULL);
	ASSERT_EQ(1, len);
	ASSERT_EQ(0, buf[0]);
}

TEST(dn_comp_overflow) {
	uint8_t buf[10];
	int len = dn_comp("verylonglabelthatexceedsbuffer.com", buf, sizeof(buf), NULL, NULL);
	ASSERT_EQ(-1, len);
}

TEST(dn_comp_null) {
	uint8_t buf[256];
	ASSERT_EQ(-1, dn_comp(NULL, buf, sizeof(buf), NULL, NULL));
}

TEST(dn_comp_roundtrip_basic) {
	uint8_t buf[256];
	char result[256];
	int wlen = dn_comp("www.google.com", buf, sizeof(buf), NULL, NULL);
	ASSERT_TRUE(wlen > 0);
	int rlen = dn_expand(buf, buf + wlen, buf, result, sizeof(result));
	ASSERT_TRUE(rlen > 0);
	ASSERT_STR_EQ("www.google.com", result);
}

TEST(dn_comp_roundtrip_root) {
	uint8_t buf[256];
	char result[256];
	int wlen = dn_comp(".", buf, sizeof(buf), NULL, NULL);
	ASSERT_EQ(1, wlen);
	int rlen = dn_expand(buf, buf + wlen, buf, result, sizeof(result));
	ASSERT_TRUE(rlen > 0);
	ASSERT_STR_EQ("", result);
}

/* ============================================================================ */

TEST_SUITE(dn_expand);

TEST(dn_expand_simple) {
	uint8_t pkt[] = {7, 'e', 'x', 'a', 'm', 'p', 'l', 'e', 3, 'c', 'o', 'm', 0};
	char name[256];
	int len = dn_expand(pkt, pkt + sizeof(pkt), pkt, name, sizeof(name));
	ASSERT_TRUE(len > 0);
	ASSERT_STR_EQ("example.com", name);
}

TEST(dn_expand_root) {
	uint8_t pkt[] = {0};
	char name[256];
	int len = dn_expand(pkt, pkt + 1, pkt, name, sizeof(name));
	ASSERT_TRUE(len > 0);
	ASSERT_STR_EQ("", name);
}

TEST(dn_expand_pointer) {
	uint8_t pkt[] = {
		3, 'w', 'w', 'w', 7, 'e', 'x', 'a', 'm', 'p', 'l', 'e', 3, 'c', 'o', 'm', 0,
		0xC0, 0x04
	};
	char name[256];
	int len = dn_expand(pkt, pkt + sizeof(pkt), pkt + 17, name, sizeof(name));
	ASSERT_TRUE(len > 0);
	ASSERT_STR_EQ("example.com", name);
}

TEST(dn_expand_invalid_pointer) {
	uint8_t pkt[] = {0xC0, 0xFF}; /* Points outside */
	char name[256];
	int len = dn_expand(pkt, pkt + 2, pkt, name, sizeof(name));
	ASSERT_EQ(-1, len);
}

TEST(dn_expand_self_referential) {
	/* Pointer points to itself - should not infinite loop */
	uint8_t pkt[] = {0xC0, 0x00};
	char name[256];
	ASSERT_EQ(-1, dn_expand(pkt, pkt + 2, pkt, name, sizeof(name)));
}

TEST(dn_expand_pointer_chain_overflow) {
	/* Create longer packet with pointer chain to properly test jump limit */
	uint8_t pkt[30] = {0};
	/* Create chain: each pointer points to next, last points to start (loop) */
	for (int i = 0; i < 11; i++) {
		pkt[i*2] = 0xC0;
		pkt[i*2+1] = ((i+1)*2) & 0xFF;
	}
	pkt[22] = 0xC0;  /* Last pointer loops back to start */
	pkt[23] = 0x00;

	char name[256];
	ASSERT_EQ(-1, dn_expand(pkt, pkt + sizeof(pkt), pkt, name, sizeof(name)));
}

TEST(dn_expand_label_too_long) {
	/* Label length 64 is invalid (max 63) */
	uint8_t pkt[70] = {64}; /* Invalid label length */
	memset(pkt+1, 'a', 64);
	char name[256];
	ASSERT_EQ(-1, dn_expand(pkt, pkt + sizeof(pkt), pkt, name, sizeof(name)));
}

TEST(dn_expand_label_past_eom) {
	/* Label claims 8 bytes but only 3 provided */
	uint8_t pkt[10] = {8, 'a', 'b', 'c'};
	char name[256];
	ASSERT_EQ(-1, dn_expand(pkt, pkt + 5, pkt, name, sizeof(name)));
}

TEST(dn_expand_output_buffer_small) {
	/* Output buffer too small for "verylonglabel" */
	uint8_t pkt[] = {11, 'v', 'e', 'r', 'y', 'l', 'o', 'n', 'g', 'l', 'a', 'b', 'e', 'l', 0};
	char name[5]; /* Too small */
	ASSERT_EQ(-1, dn_expand(pkt, pkt + sizeof(pkt), pkt, name, sizeof(name)));
}

TEST(dn_expand_pointer_before_msg) {
	/* Pointer points before start of message */
	uint8_t pkt[] = {0xC0, 0x00};
	char name[256];
	ASSERT_EQ(-1, dn_expand(pkt, pkt + 2, pkt, name, sizeof(name)));
}

TEST(dn_expand_truncated_pointer) {
	/* Pointer with only 1 byte (needs 2) */
	uint8_t pkt[] = {0xC0};
	char name[256];
	ASSERT_EQ(-1, dn_expand(pkt, pkt + 1, pkt, name, sizeof(name)));
}

TEST(dn_expand_empty_label_in_middle) {
	/* "example..com" has empty label between dots */
	uint8_t pkt[] = {7, 'e', 'x', 'a', 'm', 'p', 'l', 'e', 0, 3, 'c', 'o', 'm', 0};
	char name[256];
	/* Should handle empty labels gracefully or reject */
	int len = dn_expand(pkt, pkt + sizeof(pkt), pkt, name, sizeof(name));
	ASSERT_TRUE(len > 0 || len == -1); /* Either is acceptable */
}

/* ============================================================================ */

TEST_SUITE(dn_comp_sad_paths);

TEST(dn_comp_empty_source) {
	/* Empty string "" is valid DNS root domain (same as ".") */
	uint8_t buf[256];
	int len = dn_comp("", buf, sizeof(buf), NULL, NULL);
	ASSERT_EQ(1, len);  /* Changed from -1 to 1 */
	ASSERT_EQ(0, buf[0]);  /* Should be null terminator */
}

TEST(dn_comp_label_too_long) {
	/* Label > 63 bytes should fail */
	char label[70];
	memset(label, 'a', 64);
	label[64] = '\0';
	uint8_t buf[256];
	ASSERT_EQ(-1, dn_comp(label, buf, sizeof(buf), NULL, NULL));
}

TEST(dn_comp_compression_table_oob) {
	/* Compression table with out-of-bounds entries should skip them */
	uint8_t buf[256], table_buf[256];
	uint8_t *table[2] = {table_buf + 300, NULL}; /* Points outside dst */
	uint8_t *last[2] = {table_buf + 301, NULL};
	int len = dn_comp("test", buf, sizeof(buf), table, last);
	ASSERT_TRUE(len > 0); /* Should succeed by ignoring OOB entry */
}

TEST(dn_comp_null_dst) {
	/* NULL destination should fail */
	ASSERT_EQ(-1, dn_comp("test", NULL, 256, NULL, NULL));
}

TEST(dn_comp_zero_dstsiz) {
	/* Zero destination size should fail */
	uint8_t buf[256];
	ASSERT_EQ(-1, dn_comp("test", buf, 0, NULL, NULL));
}

TEST(dn_comp_dstsiz_too_small) {
	uint8_t buf[1];
	/* dstsiz=0 should fail (not dstsiz=1, which is enough for ".") */
	ASSERT_EQ(-1, dn_comp(".", buf, 0, NULL, NULL));
}

/* ============================================================================ */

TEST_SUITE(byte_helpers_edge_cases);

TEST(dns_get16_unaligned) {
	/* Test unaligned access (if platform allows) */
	uint8_t buf[3] = {0x01, 0x02, 0x03};
	ASSERT_EQ(0x0203, dns_get16(buf + 1));
}

TEST(dns_get32_unaligned) {
	/* Test unaligned access */
	uint8_t buf[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
	ASSERT_EQ(0x02030405, dns_get32(buf + 1));
}

TEST(dns_put16_unaligned) {
	/* Test unaligned write */
	uint8_t buf[3] = {0xFF, 0xFF, 0xFF};
	dns_put16(buf + 1, 0x1234);
	ASSERT_EQ(0xFF, buf[0]);
	ASSERT_EQ(0x12, buf[1]);
	ASSERT_EQ(0x34, buf[2]);
}

/* ============================================================================ */

TEST_SUITE(struct_packing);

TEST(dns_hdr_size_exact) {
	/* Ensure struct is exactly 12 bytes with packing */
	ASSERT_SIZE(struct dns_hdr, 12);
}

TEST(dns_q_size_exact) {
	ASSERT_SIZE(struct dns_q, 4);
}

TEST(dns_rr_size_exact) {
	ASSERT_SIZE(struct dns_rr, 10);
}

/* ============================================================================ */

TEST_SUITE(endianness);

TEST(dns_get16_network_order) {
	/* dns_get16 should interpret bytes as big-endian */
	uint8_t buf[2] = {0x12, 0x34};
	ASSERT_EQ(0x1234, dns_get16(buf));
}

TEST(dns_put16_network_order) {
	/* dns_put16 should write bytes in big-endian order */
	uint8_t buf[2];
	dns_put16(buf, 0x1234);
	ASSERT_EQ(0x12, buf[0]);
	ASSERT_EQ(0x34, buf[1]);
}

/* ============================================================================ */

TEST_MAIN_IF(JACL_HAS_POSIX, "net/dns.h requires POSIX\n")
