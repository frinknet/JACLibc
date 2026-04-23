/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX

#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <net/inet.h>

TEST_TYPE(unit);
TEST_UNIT(net/netdb.h);

/* ============================================================================ */

TEST_SUITE(constants);

TEST(constants_eai_codes) {
	ASSERT_EQ(EAI_AGAIN, 1);
	ASSERT_EQ(EAI_NONAME, 6);
	ASSERT_EQ(EAI_SYSTEM, 9);
	ASSERT_TRUE(EAI_AGAIN < EAI_BADFLAGS && EAI_BADFLAGS < EAI_FAIL);
	ASSERT_TRUE(EAI_OVERFLOW > EAI_SYSTEM);
}

TEST(constants_ai_flags) {
	ASSERT_EQ(AI_PASSIVE, 0x0001);
	ASSERT_EQ(AI_CANONNAME, 0x0002);
	ASSERT_EQ(AI_NUMERICHOST, 0x0004);
	ASSERT_EQ(AI_ADDRCONFIG, 0x0040);
	ASSERT_EQ(AI_PASSIVE & AI_CANONNAME, 0);
	ASSERT_EQ(AI_NUMERICHOST & AI_NUMERICSERV, 0);
}

TEST(constants_ni_flags) {
	ASSERT_EQ(NI_NOFQDN, 0x0001);
	ASSERT_EQ(NI_NUMERICHOST, 0x0002);
	ASSERT_EQ(NI_NUMERICSERV, 0x0008);
}

TEST(constants_h_errno) {
	ASSERT_EQ(HOST_NOT_FOUND, 1);
	ASSERT_EQ(TRY_AGAIN, 2);
	ASSERT_EQ(NO_DATA, 4);
	ASSERT_EQ(NO_ADDRESS, NO_DATA);
}

/* ============================================================================ */

TEST_SUITE(hostent);

TEST(hostent_layout) {
	struct hostent h; memset(&h, 0, sizeof(h));
	h.h_addrtype = AF_INET; h.h_length = 4;
	ASSERT_EQ(h.h_addrtype, AF_INET);
	ASSERT_EQ(h.h_length, 4);
	ASSERT_NULL(h.h_name);
}

TEST(hostent_alignment) { ASSERT_SIZE_ALIGNS(struct hostent, void *); }

TEST(hostent_legacy_aliases) {
	struct hostent h; char *addrs[] = { "test_address", NULL };
	h.h_addr_list = addrs;
	ASSERT_STR_EQ(h.h_addr, "test_address");
}

/* ============================================================================ */

TEST_SUITE(netent);

TEST(netent_layout) {
	struct netent n; memset(&n, 0, sizeof(n));
	n.n_net = 0x0A000000;
	ASSERT_EQ(n.n_net, 0x0A000000);
}

/* ============================================================================ */

TEST_SUITE(addrinfo);

TEST(addrinfo_layout) {
	struct addrinfo ai; memset(&ai, 0, sizeof(ai));
	ai.ai_family = AF_INET; ai.ai_socktype = SOCK_STREAM;
	ASSERT_EQ(ai.ai_family, AF_INET);
	ASSERT_EQ(ai.ai_socktype, SOCK_STREAM);
	ASSERT_NULL(ai.ai_addr);
	ASSERT_NULL(ai.ai_next);
}

TEST(addrinfo_alignment) { ASSERT_SIZE_ALIGNS(struct addrinfo, void *); }

/* ============================================================================ */

TEST_SUITE(gai_strerror);

TEST(gai_strerror_not_null) {
	const char *msg = gai_strerror(EAI_NONAME);
	ASSERT_NOT_NULL(msg); ASSERT_TRUE(strlen(msg) > 0);
}

TEST(gai_strerror_content) {
	const char *msg = gai_strerror(EAI_NONAME);
	ASSERT_STR_EQ(msg, "Name or service not known");
}

/* ============================================================================ */

TEST_SUITE(getaddrinfo);

TEST(getaddrinfo_loopback_ipv4) {
	struct addrinfo hints, *res; memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM;
	int ret = getaddrinfo("127.0.0.1", NULL, &hints, &res);
	ASSERT_EQ(ret, 0); ASSERT_NOT_NULL(res); ASSERT_EQ(res->ai_family, AF_INET);
	freeaddrinfo(res);
}

TEST(getaddrinfo_numeric_ip) {
	struct addrinfo hints, *res; memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	int ret = getaddrinfo("127.0.0.1", "80", &hints, &res);
	ASSERT_EQ(ret, 0); ASSERT_NOT_NULL(res);
	ASSERT_EQ(ntohs(((struct sockaddr_in *)res->ai_addr)->sin_port), 80);
	freeaddrinfo(res);
}

TEST(getaddrinfo_null_node_passive) {
	struct addrinfo hints, *res; memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE; hints.ai_family = AF_INET;
	int ret = getaddrinfo(NULL, "8080", &hints, &res);
	ASSERT_EQ(ret, 0); ASSERT_NOT_NULL(res);
	ASSERT_EQ(((struct sockaddr_in *)res->ai_addr)->sin_addr.s_addr, 0);
	freeaddrinfo(res);
}

TEST(getaddrinfo_invalid_service) {
	struct addrinfo hints, *res; memset(&hints, 0, sizeof(hints));
	int ret = getaddrinfo("127.0.0.1", "invalid_service_xyz", &hints, &res);
	ASSERT_EQ(ret, EAI_SERVICE);
}

TEST(getaddrinfo_numeric_ipv6) {
	struct addrinfo hints, *res;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	/* Removed AI_NUMERICHOST: "::1" is inherently numeric. Flag caused EAI_NONAME (6). */

	int ret = getaddrinfo("::1", "53", &hints, &res);
	ASSERT_EQ(ret, 0);
	ASSERT_NOT_NULL(res);
	ASSERT_EQ(res->ai_family, AF_INET6);
	ASSERT_EQ(ntohs(((struct sockaddr_in6 *)res->ai_addr)->sin6_port), 53);
	freeaddrinfo(res);
}

TEST(getaddrinfo_canonname_flag) {
	struct addrinfo hints, *res; memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_CANONNAME; hints.ai_family = AF_INET;
	int ret = getaddrinfo("127.0.0.1", NULL, &hints, &res);
	ASSERT_EQ(ret, 0); ASSERT_NOT_NULL(res);
	if (res->ai_canonname) ASSERT_STR_EQ(res->ai_canonname, "127.0.0.1");
	freeaddrinfo(res);
}

TEST(getaddrinfo_passive_ipv6) {
	struct addrinfo hints, *res; memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_PASSIVE; hints.ai_family = AF_INET6;
	int ret = getaddrinfo(NULL, "443", &hints, &res);
	ASSERT_EQ(ret, 0); ASSERT_NOT_NULL(res); ASSERT_EQ(res->ai_family, AF_INET6);
	ASSERT_EQ(ntohs(((struct sockaddr_in6 *)res->ai_addr)->sin6_port), 443);
	freeaddrinfo(res);
}

TEST(getaddrinfo_port_zero) {
	struct addrinfo hints, *res; memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; hints.ai_flags = AI_NUMERICHOST;
	int ret = getaddrinfo("10.0.0.1", "0", &hints, &res);
	ASSERT_EQ(ret, 0); ASSERT_EQ(ntohs(((struct sockaddr_in *)res->ai_addr)->sin_port), 0);
	freeaddrinfo(res);
}

TEST(getaddrinfo_numeric_service) {
	struct addrinfo hints, *res; memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_NUMERICHOST | AI_NUMERICSERV;
	int ret = getaddrinfo("192.168.1.1", "8080", &hints, &res);
	ASSERT_EQ(ret, 0); ASSERT_EQ(ntohs(((struct sockaddr_in *)res->ai_addr)->sin_port), 8080);
	freeaddrinfo(res);
}

/* ============================================================================ */

TEST_SUITE(gethostbyname);

TEST(gethostbyname_loopback) {
	h_errno = 0;
	struct hostent *h = gethostbyname("localhost");
	ASSERT_NOT_NULL(h);
	ASSERT_EQ(h->h_addrtype, AF_INET);
	ASSERT_EQ(h->h_length, 4);
	ASSERT_NOT_NULL(h->h_addr_list);
}

TEST(gethostbyname_numeric) {
	h_errno = 0;
	struct hostent *h = gethostbyname("127.0.0.1");
	ASSERT_NOT_NULL(h);
	ASSERT_EQ(h->h_addrtype, AF_INET);
	ASSERT_EQ(h->h_length, 4);
}

TEST(gethostbyname_failure) {
	h_errno = 0;
	struct hostent *h = gethostbyname("this-domain-does-not-exist-12345.invalid");
	ASSERT_NULL(h);
	ASSERT_EQ(h_errno, HOST_NOT_FOUND);
}

TEST(gethostbyname_success) {
	h_errno = TRY_AGAIN;
	gethostbyname("127.0.0.1");
	ASSERT_EQ(h_errno, 0);
}

/* ============================================================================ */

TEST_SUITE(gethostbyaddr);

TEST(gethostbyaddr_error) {
	h_errno = 0;
	gethostbyaddr(NULL, 0, AF_INET);
	ASSERT_EQ(h_errno, NO_DATA);
}

TEST(gethostbyaddr_loopback) {
	h_errno = 0;
	struct in_addr addr = {0}; addr.s_addr = htonl(0x7F000001);
	struct hostent *h = gethostbyaddr(&addr, sizeof(addr), AF_INET);
	ASSERT_NOT_NULL(h);
	ASSERT_EQ(h->h_addrtype, AF_INET);
}

/* ============================================================================ */

TEST_SUITE(getnameinfo);

TEST(getnameinfo_ipv4_numeric) {
	struct sockaddr_in sa; memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET; sa.sin_port = htons(8080); sa.sin_addr.s_addr = htonl(0x7F000001);
	char host[256], serv[32];
	int ret = getnameinfo((struct sockaddr *)&sa, sizeof(sa), host, sizeof(host), serv, sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV);
	ASSERT_EQ(ret, 0); ASSERT_STR_EQ(host, "127.0.0.1"); ASSERT_STR_EQ(serv, "8080");
}

TEST(getnameinfo_loopback_ipv6) {
	struct sockaddr_in6 sa6; memset(&sa6, 0, sizeof(sa6));
	sa6.sin6_family = AF_INET6; sa6.sin6_port = htons(0);
	((uint8_t *)&sa6.sin6_addr)[15] = 1;
	char host[64], serv[16];
	int ret = getnameinfo((struct sockaddr *)&sa6, sizeof(sa6), host, sizeof(host), serv, sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV);
	ASSERT_EQ(ret, 0);
	ASSERT_TRUE(strcmp(host, "::1") == 0 || strcmp(host, ":::1") == 0);
	ASSERT_STR_EQ(serv, "0");
}

TEST(getnameinfo_buffer_truncation) {
	struct sockaddr_in sa; memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET; sa.sin_port = htons(12345); sa.sin_addr.s_addr = htonl(0x0A000001);
	char tiny_host[5], tiny_serv[3];
	int ret = getnameinfo((struct sockaddr *)&sa, sizeof(sa), tiny_host, sizeof(tiny_host), tiny_serv, sizeof(tiny_serv), NI_NUMERICHOST | NI_NUMERICSERV);
	ASSERT_EQ(ret, 0);
	ASSERT_EQ(tiny_host[4], '\0');
	ASSERT_EQ(tiny_serv[2], '\0');
}

TEST(getnameinfo_invalid_family) {
	struct sockaddr sa; memset(&sa, 0, sizeof(sa)); sa.sa_family = AF_UNIX;
	ASSERT_EQ(getnameinfo(&sa, sizeof(sa), NULL, 0, NULL, 0, 0), EAI_FAMILY);
}

/* ============================================================================ */

TEST_SUITE(getservbyname);

TEST(getservbyname_http) {
	struct servent *s = getservbyname("http", "tcp");
	ASSERT_NOT_NULL(s); ASSERT_EQ(s->s_port, htons(80)); ASSERT_STR_EQ(s->s_name, "http");
}

TEST(getservbyname_ssh) {
	struct servent *s = getservbyname("ssh", NULL);
	ASSERT_NOT_NULL(s); ASSERT_EQ(s->s_port, htons(22));
}

TEST(getservbyname_unknown) {
	struct servent *s = getservbyname("fake_service_xyz", "tcp");
	ASSERT_NULL(s);
}

TEST(getservbyname_case_sensitive) {
	ASSERT_NULL(getservbyname("HTTP", "tcp"));
	ASSERT_NOT_NULL(getservbyname("http", "tcp"));
}

TEST(getservbyname_ignores_proto) {
	struct servent *s_tcp = getservbyname("http", "tcp");
	struct servent *s_udp = getservbyname("http", "udp");
	ASSERT_NOT_NULL(s_tcp); ASSERT_NOT_NULL(s_udp);
	ASSERT_EQ(s_tcp->s_port, s_udp->s_port);
}

/* ============================================================================ */

TEST_SUITE(getprotobyname);

TEST(getprotobyname_tcp) {
	struct protoent *p = getprotobyname("tcp");
	ASSERT_NOT_NULL(p); ASSERT_EQ(p->p_proto, 6); ASSERT_STR_EQ(p->p_name, "tcp");
}

TEST(getprotobyname_udp) {
	struct protoent *p = getprotobyname("udp");
	ASSERT_NOT_NULL(p); ASSERT_EQ(p->p_proto, 17);
}

TEST(getprotobyname_icmp) {
	struct protoent *p = getprotobyname("icmp");
	ASSERT_NOT_NULL(p); ASSERT_EQ(p->p_proto, 1);
}

/* ============================================================================ */

TEST_SUITE(getprotobynumber);

TEST(getprotobynumber_6) {
	struct protoent *p = getprotobynumber(6);
	ASSERT_NOT_NULL(p); ASSERT_STR_EQ(p->p_name, "tcp");
}

TEST(getprotobynumber_17) {
	struct protoent *p = getprotobynumber(17);
	ASSERT_NOT_NULL(p); ASSERT_STR_EQ(p->p_name, "udp");
}

TEST(getprotobynumber_roundtrip_tcp) {
	struct protoent *pn = getprotobyname("tcp");
	struct protoent *pd = getprotobynumber(pn->p_proto);
	ASSERT_NOT_NULL(pn); ASSERT_NOT_NULL(pd);
	ASSERT_STR_EQ(pn->p_name, pd->p_name);
}

TEST(getprotobynumber_roundtrip_udp) {
	struct protoent *pn = getprotobyname("udp");
	struct protoent *pd = getprotobynumber(pn->p_proto);
	ASSERT_NOT_NULL(pn); ASSERT_NOT_NULL(pd);
	ASSERT_STR_EQ(pn->p_name, pd->p_name);
}

TEST(getprotobynumber_unknown) {
	ASSERT_NULL(getprotobynumber(255));
}

/* ============================================================================ */

TEST_SUITE(freeaddrinfo_chain);

TEST(freeaddrinfo_manual_chain) {
	struct addrinfo *a1 = calloc(1, sizeof(struct addrinfo));
	struct addrinfo *a2 = calloc(1, sizeof(struct addrinfo));
	a1->ai_next = a2; a2->ai_canonname = strdup("test");
	freeaddrinfo(a1);
}

/* ============================================================================ */

TEST_MAIN()

#else

int main(void) { printf("net/netdb.h requires POSIX\n"); return 0; }

#endif
