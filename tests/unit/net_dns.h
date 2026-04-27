/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#include <net/dns.h>

TEST_TYPE(unit);
TEST_UNIT(net/dns.h);

/* ============================================================================ */

TEST_SUITE(constants);

TEST(constants_sizes) {
	ASSERT_EQ(12, DNS_HDR_SIZE);
	ASSERT_EQ(12, HFIXEDSZ);
	ASSERT_EQ(4, QFIXEDSZ);
	ASSERT_EQ(10, RRFIXEDSZ);
	ASSERT_EQ(4, INT32SZ);
	ASSERT_EQ(2, INT16SZ);
	ASSERT_EQ(1, INT8SZ);
}

TEST(constants_types) {
	ASSERT_EQ(1, DNS_TYPE_A);
	ASSERT_EQ(5, DNS_TYPE_CNAME);
	ASSERT_EQ(28, DNS_TYPE_AAAA);
	ASSERT_EQ(12, DNS_TYPE_PTR);
	ASSERT_EQ(255, DNS_TYPE_ANY);
}

TEST(constants_classes) {
	ASSERT_EQ(1, DNS_CLASS_IN);
	ASSERT_EQ(255, DNS_CLASS_ANY);
}

TEST/constants_opcodes_rcodes) {
	ASSERT_EQ(0, DNS_OPCODE_QUERY);
	ASSERT_EQ(0, DNS_RCODE_NOERR);
	ASSERT_EQ(3, DNS_RCODE_NXDOMAIN);
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

TEST_SUITE(dns_name_compress);

TEST(dns_name_compress_simple) {
	uint8_t buf[256];
	int len = dns_name_compress("example.com", buf, sizeof(buf));
	ASSERT_EQ(13, len);
	ASSERT_EQ(7, buf[0]);
	ASSERT_EQ('e', buf[1]);
	ASSERT_EQ(3, buf[8]);
	ASSERT_EQ('c', buf[9]);
	ASSERT_EQ(0, buf[12]);
}

TEST(dns_name_compress_root) {
	uint8_t buf[256];
	int len = dns_name_compress(".", buf, sizeof(buf));
	ASSERT_EQ(1, len);
	ASSERT_EQ(0, buf[0]);
}

TEST(dns_name_compress_overflow) {
	uint8_t buf[10];
	int len = dns_name_compress("verylonglabelthatexceedsbuffer.com", buf, sizeof(buf));
	ASSERT_EQ(-1, len);
}

TEST(dns_name_compress_null) {
	uint8_t buf[256];
	ASSERT_EQ(-1, dns_name_compress(NULL, buf, sizeof(buf)));
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

/* ============================================================================ */

TEST_SUITE(dns_name_compress_roundtrip);

TEST(dns_name_compress_roundtrip_basic) {
	uint8_t buf[256];
	char result[256];
	int wlen = dns_name_compress("www.google.com", buf, sizeof(buf));
	ASSERT_TRUE(wlen > 0);
	int rlen = dn_expand(buf, buf + wlen, buf, result, sizeof(result));
	ASSERT_TRUE(rlen > 0);
	ASSERT_STR_EQ("www.google.com", result);
}

TEST(dns_name_compress_roundtrip_root) {
	uint8_t buf[256];
	char result[256];
	int wlen = dns_name_compress(".", buf, sizeof(buf));
	ASSERT_EQ(1, wlen);
	int rlen = dn_expand(buf, buf + wlen, buf, result, sizeof(result));
	ASSERT_TRUE(rlen > 0);
	ASSERT_STR_EQ("", result);
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

TEST_MAIN._IF(JACL_HAS_POSIX, "net/dns.h requires POSIX\n")
