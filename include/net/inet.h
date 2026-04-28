/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _NET_INET_H
#define _NET_INET_H
#pragma once

/**
 * NOTE: This consolidates functionality traditionally found in:
 *
 *   - <netinet/in.h>
 *   - <arpa/inet.h>
 *
 * We unify these into a single header at <net/inet.h> for ease maintenance
 * and clarity with hopes that one day C Standards or POSIX may revamp layout
 * for sanity's sake so that we can remove the abstraction libraries that cause
 * so many security bugs because the coders don't see how things really work.
 *
 * There has to be a better way!!!
 */

#include <config.h>
#include <endian.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;

#define INET_ADDRSTRLEN		16
#define INET6_ADDRSTRLEN	46

#define INADDR_ANY		((in_addr_t) 0x00000000)
#define INADDR_BROADCAST	((in_addr_t) 0xffffffff)
#define INADDR_LOOPBACK		((in_addr_t) 0x7f000001)
#define INADDR_NONE		((in_addr_t) 0xffffffff)

#define X(ID, num, ...) ID = num,
enum {
#include <x/ip_protocols.h>
} __jacl_ipproto_t;
#undef X

#define IPPROTO_IP 0

#define X(ID, num, ...) ID = num,
enum {
#include <x/ip_ports.h>
} __jacl_ipport_t;
#undef X

#define IPPORT_RESERVED		1024
#define IPPORT_USERRESERVED	5000

#define IN_CLASSA(a)		(((in_addr_t)(a) & 0x80000000) == 0)
#define IN_CLASSA_NET		0xff000000
#define IN_CLASSB(a)		(((in_addr_t)(a) & 0xc0000000) == 0x80000000)
#define IN_CLASSB_NET		0xffff0000
#define IN_CLASSC(a)		(((in_addr_t)(a) & 0xe0000000) == 0xc0000000)
#define IN_CLASSC_NET		0xffffff00
#define IN_CLASSD(a)		(((in_addr_t)(a) & 0xf0000000) == 0xe0000000)
#define IN_CLASSE(a)		(((in_addr_t)(a) & 0xf8000000) == 0xf0000000)
#define IN_LOOPBACK(a)	(((in_addr_t)(a) & 0xff000000) == 0x7f000000)

struct in_addr {
    in_addr_t           s_addr;
};

struct in6_addr {
    union {
        uint8_t         u6_addr8[16];
        uint16_t        u6_addr16[8];
        uint32_t        u6_addr32[4];
    } in6_u;
};

struct sockaddr_in {
    sa_family_t         sin_family;
    in_port_t           sin_port;
    struct in_addr      sin_addr;
    unsigned char       sin_zero[8];
};

struct sockaddr_in6 {
    sa_family_t         sin6_family;
    in_port_t           sin6_port;
    uint32_t            sin6_flowinfo;
    struct in6_addr     sin6_addr;
    uint32_t            sin6_scope_id;
};

struct ip_mreq {
    struct in_addr      imr_multiaddr;
    struct in_addr      imr_interface;
};

struct ip_mreqn {
    struct in_addr      imr_multiaddr;
    struct in_addr      imr_address;
    int                 imr_ifindex;
};

struct ipv6_mreq {
    struct in6_addr     ipv6mr_multiaddr;
    unsigned int        ipv6mr_interface;
};

/* ======================================================================== */
/* IPv6 Address Test Macros                                                 */
/* ======================================================================== */

#define IN6_IS_ADDR_UNSPECIFIED(a) \
	(((const uint32_t *)(a))[0] == 0 && ((const uint32_t *)(a))[1] == 0 && \
	 ((const uint32_t *)(a))[2] == 0 && ((const uint32_t *)(a))[3] == 0)

#define IN6_IS_ADDR_LOOPBACK(a) \
	(((const uint8_t *)(a))[12] == 0 && ((const uint8_t *)(a))[13] == 0 && \
	 ((const uint8_t *)(a))[14] == 0 && ((const uint8_t *)(a))[15] == 1)

#define IN6_IS_ADDR_MULTICAST(a)	(((const uint8_t *)(a))[0] == 0xff)

#define IN6_IS_ADDR_LINKLOCAL(a) \
	(((const uint8_t *)(a))[0] == 0xfe && (((const uint8_t *)(a))[1] & 0xc0) == 0x80)

#define IN6_IS_ADDR_SITELOCAL(a) \
	(((const uint8_t *)(a))[0] == 0xfe && (((const uint8_t *)(a))[1] & 0xc0) == 0xc0)

#define IN6_IS_ADDR_V4MAPPED(a) \
	(((const uint8_t *)(a))[10] == 0xff && ((const uint8_t *)(a))[11] == 0xff && \
	 ((const uint32_t *)(a))[0] == 0 && ((const uint32_t *)(a))[1] == 0)

#define IN6_IS_ADDR_V4COMPAT(a) \
	(((const uint32_t *)(a))[0] == 0 && ((const uint32_t *)(a))[1] == 0 && \
	 ((const uint32_t *)(a))[2] == 0 && \
	 !(((const uint8_t *)(a))[12] == 0 && ((const uint8_t *)(a))[13] == 0 && \
	  ((const uint8_t *)(a))[14] == 0 && ((const uint8_t *)(a))[15] == 0) && \
	 !(((const uint8_t *)(a))[12] == 0 && ((const uint8_t *)(a))[13] == 0 && \
	 	((const uint8_t *)(a))[14] == 0 && ((const uint8_t *)(a))[15] == 1))

/* ======================================================================== */
/* Address Conversion Internals                                             */
/* ======================================================================== */

static inline int __jacl_pton_ip4(int af, const char *src, void *dst) {
	uint8_t *t = dst;

	for (int i = 0; i < 4; i++) {
		char *e;
		unsigned long v = strtoul(src, &e, 10);

		if (e == src || *e != (i < 3 ? '.' : '\0') || v > 255) return 0;

		t[i] = (uint8_t)v;
		src = e + 1;
	}

	return 1;
}

static inline int __jacl_pton_ip6(int af, const char *src, void *dst) {
	uint16_t w[8] = {0};
	int i = 0, comp = -1;
	const char *p = src;

	while (*p) {
		if (*p == ':') {
			if (comp >= 0) return 0;

			comp = i;

			if (*++p == ':') p++;

			continue;
		}

		if (!isxdigit((unsigned char)*p)) return 0;

		char *e;
		unsigned long v = strtoul(p, &e, 16);

		if (v > 0xffff || i >= 8) return 0;

		w[i++] = (uint16_t)v;
		p = e;

		if (*p == '.') {
			unsigned a,b,c,d;

			if (sscanf(p, "%u.%u.%u.%u", &a,&b,&c,&d) != 4 || a>255||b>255||c>255||d>255) return 0;
			if (comp < 0 && i != 6) return 0;

			w[i++] = (a << 8) | b;
			w[i++] = (c << 8) | d;

			break;
		}
	}

	if (comp >= 0) {
		int z = 8 - i + 1;

		if (i + z > 8) return 0;

		memmove(&w[comp + z], &w[comp], (i - comp) * 2);
		memset(&w[comp], 0, z * 2);
	} else if (i != 8) {
		return 0;
	}

	uint8_t *o = dst;

	for (int j = 0; j < 8; j++) { o[j*2] = w[j] >> 8; o[j*2+1] = w[j] & 0xff; }

	return 1;
}

static inline const char *__jacl_ntop_ip4(int af, const void *src, char *dst, socklen_t size) {
	const unsigned char *b = (const unsigned char *)src;

	if (size < INET_ADDRSTRLEN) return NULL;

	snprintf(dst, size, "%u.%u.%u.%u", b[0], b[1], b[2], b[3]);

	return dst;
}

static inline const char *__jacl_ntop_ip6(int af, const void *src, char *dst, socklen_t size) {
		const unsigned char *b = (const unsigned char *)src;
		uint16_t words[8];

		for (int i = 0; i < 8; i++) words[i] = (b[i*2] << 8) | b[i*2+1];

		int best_len = 0, best_idx = -1;
		int cur_len = 0, cur_idx = -1;

		for (int i = 0; i < 8; i++) {
			if (words[i] == 0) {
				if (cur_len == 0) cur_idx = i;

				cur_len++;

				if (cur_len > best_len) {
					best_len = cur_len;
					best_idx = cur_idx;
				}
			} else {
				cur_len = 0;
			}
		}

		if (best_len < 2) best_idx = -1;
		if (size < INET6_ADDRSTRLEN) return NULL;

		char *p = dst;

		for (int i = 0; i < 8; i++) {
			if (i == best_idx) {
				if (i == 0) *p++ = ':';

				*p++ = ':';
				i += best_len - 1;

				continue;
			}

			if (i > 0 && i != best_idx + 1) *p++ = ':';

			p += snprintf(p, (size_t)(dst + size - p), "%x", words[i]);
		}

		*p = '\0';

		return dst;
}

/* ======================================================================== */
/* Address Conversion Functions                                             */
/* ======================================================================== */

static inline int inet_pton(int af, const char *src, void *dst) {
	if (!src || !dst) return 0;
	if (af == AF_INET) return __jacl_pton_ip4(af, src, dst);
	if (af == AF_INET6) return __jacl_pton_ip6(af, src, dst);

	return -1;
}

static inline const char *inet_ntop(int af, const void *src, char *dst, socklen_t size) {
	if (!src || !dst) return NULL;
	if (af == AF_INET) return __jacl_ntop_ip4(af, src, dst, size);
	if (af == AF_INET6) return __jacl_ntop_ip6(af, src, dst, size);

	return NULL;
}

static inline char *inet_ntoa(struct in_addr in) {
	static char buf[INET_ADDRSTRLEN];
	unsigned char *b = (unsigned char *)&in.s_addr;

	snprintf(buf, INET_ADDRSTRLEN, "%u.%u.%u.%u", b[0], b[1], b[2], b[3]);

	return buf;
}

static inline in_addr_t inet_addr(const char *cp) {
	struct in_addr addr;

	if (inet_pton(AF_INET, cp, &addr) == 1) return addr.s_addr;

	return INADDR_NONE;
}

#ifdef __cplusplus
}
#endif

#endif /* _NET_INET_H */



