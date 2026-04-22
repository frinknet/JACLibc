/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#include <net/inet.h>

TEST_TYPE(unit);
TEST_UNIT(net/inet.h);

TEST_SUITE(constants);

TEST(constants_restrictions) {
	ASSERT_EQ(INET_ADDRSTRLEN, 16);
	ASSERT_EQ(INET6_ADDRSTRLEN, 46);
	ASSERT_EQ(IPPORT_RESERVED, 1024);
}

TEST(constants_inaddr) {
	ASSERT_EQ(INADDR_ANY, 0x00000000);
	ASSERT_EQ(INADDR_NONE, 0xffffffff);
	ASSERT_EQ(INADDR_LOOPBACK, 0x7f000001);
}

TEST(constants_ipproto) {
	ASSERT_EQ(IPPROTO_IP ,0);
	ASSERT_EQ(IPPROTO_ICMP, 1);
	ASSERT_EQ(IPPROTO_TCP, 6);
	ASSERT_EQ(IPPROTO_UDP, 17);
	ASSERT_EQ(IPPROTO_IPV6, 41);
}

/* ============================================================================ */

TEST_SUITE(in_class);

TEST(macro_in_classa_match) { ASSERT_TRUE(IN_CLASSA(0x0A000001)); }
TEST(macro_in_classb_match) { ASSERT_TRUE(IN_CLASSB(0xAC100001)); }
TEST(macro_in_classc_match) { ASSERT_TRUE(IN_CLASSC(0xC0A80001)); }

/* ============================================================================ */

TEST_SUITE(in_loopback);

TEST(macro_in_loopback_match) { ASSERT_TRUE(IN_LOOPBACK(0x7F000001)); }

/* ============================================================================ */

TEST_SUITE(inet_ntoa);

TEST(inet_ntoa_zero) {
	struct in_addr a = {0};
	char *s = inet_ntoa(a);
	ASSERT_NOT_NULL(s); ASSERT_STR_EQ("0.0.0.0", s);
}

TEST(inet_ntoa_broadcast) {
	struct in_addr a; a.s_addr = 0xFFFFFFFF;
	char *s = inet_ntoa(a);
	ASSERT_NOT_NULL(s); ASSERT_STR_EQ("255.255.255.255", s);
}

TEST(inet_ntoa_localhost) {
	struct in_addr a; a.s_addr = htonl(0x7f000001);
	char *s = inet_ntoa(a);
	ASSERT_NOT_NULL(s); ASSERT_STR_EQ("127.0.0.1", s);
}

TEST(inet_ntoa_arbitrary) {
	struct in_addr a; a.s_addr = htonl(0xC0A80101);
	char *s = inet_ntoa(a);
	ASSERT_NOT_NULL(s); ASSERT_STR_EQ("192.168.1.1", s);
}

TEST(inet_ntoa_sequential) {
	struct in_addr a; a.s_addr = htonl(0x01020304);
	char *s = inet_ntoa(a);
	ASSERT_NOT_NULL(s); ASSERT_STR_EQ("1.2.3.4", s);
}

TEST(inet_ntoa_static_buffer) {
	struct in_addr a1; a1.s_addr = htonl(0x0A000001);
	struct in_addr a2; a2.s_addr = htonl(0x7F000001);
	char *s1 = inet_ntoa(a1);
	char *s2 = inet_ntoa(a2);
	/* POSIX: both must point to same static buffer */
	ASSERT_PTR_EQ(s1, s2);
	/* Second call overwrites first */
	ASSERT_STR_EQ("127.0.0.1", s1);
	ASSERT_STR_EQ("127.0.0.1", s2);
}

TEST(inet_ntoa_network_order) {
	/* Verify function expects network byte order, not host */
	struct in_addr a; a.s_addr = htonl(0x0A0B0C0D);
	char *s = inet_ntoa(a);
	ASSERT_NOT_NULL(s); ASSERT_STR_EQ("10.11.12.13", s);
}


/* ============================================================================ */
TEST_SUITE(inet_pton);

TEST(inet_pton_valid_std) {
	struct in_addr a; int r = inet_pton(AF_INET, "192.168.1.1", &a);
	ASSERT_EQ(1, r); ASSERT_EQ(htonl(0xC0A80101), a.s_addr);
}

TEST(inet_pton_boundary_zero) {
	struct in_addr a; ASSERT_EQ(1, inet_pton(AF_INET, "0.0.0.0", &a));
	ASSERT_EQ(0, a.s_addr);
}

TEST(inet_pton_boundary_max) {
	struct in_addr a; ASSERT_EQ(1, inet_pton(AF_INET, "255.255.255.255", &a));
	ASSERT_EQ(0xFFFFFFFF, a.s_addr);
}

TEST(inet_pton_leading_zeros) {
	struct in_addr a; ASSERT_EQ(1, inet_pton(AF_INET, "001.010.100.255", &a));
	ASSERT_EQ(htonl(0x010A64FF), a.s_addr);
}

TEST(inet_pton_too_few) {
	struct in_addr a; ASSERT_EQ(0, inet_pton(AF_INET, "10.0.1", &a));
}

TEST(inet_pton_too_many) {
	struct in_addr a; ASSERT_EQ(0, inet_pton(AF_INET, "1.2.3.4.5", &a));
}

TEST(inet_pton_null_src) {
	struct in_addr a; ASSERT_EQ(0, inet_pton(AF_INET, NULL, &a));
}

TEST(inet_pton_null_dst) {
	ASSERT_EQ(0, inet_pton(AF_INET, "1.1.1.1", NULL));
}

TEST(inet_pton_invalid_range) {
	struct in_addr a; ASSERT_EQ(0, inet_pton(AF_INET, "256.1.1.1", &a));
}

TEST(inet_pton_invalid_chars) {
	struct in_addr a; ASSERT_EQ(0, inet_pton(AF_INET, "1.2.3.X", &a));
}

TEST(inet_pton_invalid_af) {
	struct in_addr a; ASSERT_EQ(-1, inet_pton(99, "1.1.1.1", &a));
}

/* ============================================================================ */

TEST_SUITE(inet_addr);

TEST(inet_addr_localhost) {
	in_addr_t a = inet_addr("127.0.0.1");
	ASSERT_EQ(htonl(0x7f000001), a);
}

TEST(inet_addr_zero) {
	ASSERT_EQ(0, inet_addr("0.0.0.0"));
}

TEST(inet_addr_broadcast_legacy) {
	/* Legacy POSIX: 255.255.255.255 maps to INADDR_NONE */
	ASSERT_EQ(INADDR_NONE, inet_addr("255.255.255.255"));
}

TEST(inet_addr_invalid) {
	ASSERT_EQ(INADDR_NONE, inet_addr("invalid"));
}

TEST(inet_addr_empty) {
	ASSERT_EQ(INADDR_NONE, inet_addr(""));
}

TEST(inet_addr_null) {
	ASSERT_EQ(INADDR_NONE, inet_addr(NULL));
}

/* ============================================================================ */

TEST_SUITE(inet_ntop);

TEST(inet_ntop_valid_zero) {
	struct in_addr a = {0}; char buf[INET_ADDRSTRLEN];
	ASSERT_PTR_EQ(buf, inet_ntop(AF_INET, &a, buf, sizeof(buf)));
	ASSERT_STR_EQ("0.0.0.0", buf);
}

TEST(inet_ntop_valid_max) {
	struct in_addr a = {0xFFFFFFFF}; char buf[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &a, buf, sizeof(buf));
	ASSERT_STR_EQ("255.255.255.255", buf);
}

TEST(inet_ntop_exact_size) {
	struct in_addr a = {htonl(0x0A0B0C0D)}; char buf[16];
	ASSERT_PTR_EQ(buf, inet_ntop(AF_INET, &a, buf, 16));
}

TEST(inet_ntop_buffer_small) {
	struct in_addr a = {htonl(0x0A000001)}; char buf[5];
	ASSERT_NULL(inet_ntop(AF_INET, &a, buf, sizeof(buf)));
}

TEST(inet_ntop_null_src) {
	char buf[16]; ASSERT_NULL(inet_ntop(AF_INET, NULL, buf, sizeof(buf)));
}
TEST(inet_ntop_null_dst) {
	struct in_addr a = {0}; ASSERT_NULL(inet_ntop(AF_INET, &a, NULL, 16));
}

TEST(inet_ntop_invalid_af) {
	struct in_addr a = {0}; char buf[16];
	ASSERT_NULL(inet_ntop(99, &a, buf, sizeof(buf)));
}

/* ============================================================================ */
TEST_SUITE(sockaddr_in);

TEST(sockaddr_in_size) { ASSERT_EQ(16, sizeof(struct sockaddr_in)); }

/* ============================================================================ */

TEST_SUITE(in6_is_macros);

TEST(in6_is_addr_unspecified_match) {
	struct in6_addr a = {{{0}}}; ASSERT_TRUE(IN6_IS_ADDR_UNSPECIFIED(&a));
}

TEST(in6_is_addr_unspecified_mismatch) {
	struct in6_addr a = {{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}}};
	ASSERT_FALSE(IN6_IS_ADDR_UNSPECIFIED(&a));
}

TEST(in6_is_addr_loopback_match) {
	struct in6_addr a = {{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1}}};
	ASSERT_TRUE(IN6_IS_ADDR_LOOPBACK(&a));
}

TEST(in6_is_addr_loopback_mismatch) {
	struct in6_addr a = {{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2}}};
	ASSERT_FALSE(IN6_IS_ADDR_LOOPBACK(&a));
}

TEST(in6_is_addr_multicast_match) {
	struct in6_addr a = {{{0xff, 0x02, 0,0,0,0,0,0,0,0,0,0,0,0,0,1}}};
	ASSERT_TRUE(IN6_IS_ADDR_MULTICAST(&a));
}

TEST(in6_is_addr_multicast_mismatch) {
	struct in6_addr a = {{{0x20, 0x01, 0,0,0,0,0,0,0,0,0,0,0,0,0,1}}};
	ASSERT_FALSE(IN6_IS_ADDR_MULTICAST(&a));
}

TEST(in6_is_addr_linklocal_match) {
	struct in6_addr a = {{{0xfe, 0x80, 0,0,0,0,0,0,0,0,0,0,0,0,0,1}}};
	ASSERT_TRUE(IN6_IS_ADDR_LINKLOCAL(&a));
}

TEST(in6_is_addr_linklocal_mismatch) {
	struct in6_addr a = {{{0xfe, 0xc0, 0,0,0,0,0,0,0,0,0,0,0,0,0,1}}};
	ASSERT_FALSE(IN6_IS_ADDR_LINKLOCAL(&a));
}

TEST(in6_is_addr_sitelocal_match) {
	struct in6_addr a = {{{0xfe, 0xc0, 0,0,0,0,0,0,0,0,0,0,0,0,0,1}}};
	ASSERT_TRUE(IN6_IS_ADDR_SITELOCAL(&a));
}

TEST(in6_is_addr_v4mapped_match) {
	struct in6_addr a = {{{0,0,0,0,0,0,0,0,0,0,0xFF,0xFF,192,168,1,1}}};
	ASSERT_TRUE(IN6_IS_ADDR_V4MAPPED(&a));
}

TEST(in6_is_addr_v4mapped_mismatch) {
	struct in6_addr a = {{{0,0,0,0,0,0,0,0,0,0,0,0,192,168,1,1}}};
	ASSERT_FALSE(IN6_IS_ADDR_V4MAPPED(&a));
}

TEST(in6_is_addr_v4compat_match) {
	struct in6_addr a = {{{0,0,0,0,0,0,0,0,0,0,0,0,192,168,1,1}}};
	ASSERT_TRUE(IN6_IS_ADDR_V4COMPAT(&a));
}

TEST(in6_is_addr_v4compat_mismatch) {
	struct in6_addr a = {{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}}};
	ASSERT_FALSE(IN6_IS_ADDR_V4COMPAT(&a));
}

TEST_MAIN()
