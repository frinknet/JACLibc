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
#include <sys/types.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t in_addr_t;
typedef uint16_t in_port_t;

#define AF_INET   2
#define AF_INET6 10
#define INET_ADDRSTRLEN		16
#define INET6_ADDRSTRLEN	46
#define INADDR_ANY		((in_addr_t) 0x00000000)
#define INADDR_BROADCAST	((in_addr_t) 0xffffffff)
#define INADDR_LOOPBACK		((in_addr_t) 0x7f000001)
#define INADDR_NONE		((in_addr_t) 0xffffffff)
#define IPPORT_RESERVED		1024
#define IPPORT_USERRESERVED	5000
#define IPPROTO_IP		0
#define IPPROTO_ICMP		1
#define IPPROTO_TCP		6
#define IPPROTO_UDP		17
#define IPPROTO_IPV6		41

#define IN_CLASSA(a)		(((in_addr_t)(a) & 0x80000000) == 0)
#define IN_CLASSA_NET		0xff000000
#define IN_CLASSB(a)		(((in_addr_t)(a) & 0xc0000000) == 0x80000000)
#define IN_CLASSB_NET		0xffff0000
#define IN_CLASSC(a)		(((in_addr_t)(a) & 0xe0000000) == 0xc0000000)
#define IN_CLASSC_NET		0xffffff00
#define IN_CLASSD(a)		(((in_addr_t)(a) & 0xf0000000) == 0xe0000000)
#define IN_CLASSE(a)		(((in_addr_t)(a) & 0xf8000000) == 0xf0000000)
#define IN_LOOPBACK(a)		(((in_addr_t)(a) & 0xff000000) == 0x7f000000)

struct in_addr { in_addr_t s_addr; };
struct in6_addr { union { uint8_t u6_addr8[16]; uint16_t u6_addr16[8]; uint32_t u6_addr32[4]; } in6_u; };
struct sockaddr_in { unsigned short sin_family; in_port_t sin_port; struct in_addr sin_addr; unsigned char sin_zero[8]; };
struct sockaddr_in6 { unsigned short sin6_family; in_port_t sin6_port; uint32_t sin6_flowinfo; struct in6_addr sin6_addr; uint32_t sin6_scope_id; };
struct ip_mreq { struct in_addr imr_multiaddr; struct in_addr imr_interface; };
struct ip_mreqn { struct in_addr imr_multiaddr; struct in_addr imr_address; int imr_ifindex; };
struct ipv6_mreq { struct in6_addr ipv6mr_multiaddr; unsigned int ipv6mr_interface; };

/* ======================================================================== */
/* IPv6 Address Test Macros                                                 */
/* ======================================================================== */

#define IN6_IS_ADDR_UNSPECIFIED(a) \
	(((const uint32_t *)(a))[0] == 0 && ((const uint32_t *)(a))[1] == 0 && \
	 ((const uint32_t *)(a))[2] == 0 && ((const uint32_t *)(a))[3] == 0)
#define IN6_IS_ADDR_LOOPBACK(a) \
	(((const uint32_t *)(a))[0] == 0 && ((const uint32_t *)(a))[1] == 0 && \
	 ((const uint32_t *)(a))[2] == 0 && ((const uint32_t *)(a))[3] == htonl(1))
#define IN6_IS_ADDR_MULTICAST(a)	(((const uint8_t *)(a))[0] == 0xff)
#define IN6_IS_ADDR_LINKLOCAL(a) \
	(((const uint8_t *)(a))[0] == 0xfe && (((const uint8_t *)(a))[1] & 0xc0) == 0x80)
#define IN6_IS_ADDR_SITELOCAL(a) \
	(((const uint8_t *)(a))[0] == 0xfe && (((const uint8_t *)(a))[1] & 0xc0) == 0xc0)
#define IN6_IS_ADDR_V4MAPPED(a) \
	(((const uint32_t *)(a))[0] == 0 && ((const uint32_t *)(a))[1] == 0 && \
	 ((const uint32_t *)(a))[2] == htonl(0xffff))
#define IN6_IS_ADDR_V4COMPAT(a) \
	(((const uint32_t *)(a))[0] == 0 && ((const uint32_t *)(a))[1] == 0 && \
	 ((const uint32_t *)(a))[2] == 0 && ((const uint32_t *)(a))[3] != 0 && \
	 ((const uint32_t *)(a))[3] != htonl(1))

/* ======================================================================== */
/* Address Conversion Functions                                             */
/* ======================================================================== */

static inline int inet_pton(int af, const char *src, void *dst) {
	if (!src || !dst) return 0;
	if (af == AF_INET) {
		unsigned char *t = (unsigned char *)dst;
		unsigned int a, b, c, d;
		int n = 0;
		if (sscanf(src, "%u.%u.%u.%u%n", &a, &b, &c, &d, &n) != 4) return 0;
		if (src[n] != '\0') return 0; /* Reject trailing garbage */
		if (a > 255 || b > 255 || c > 255 || d > 255) return 0;
		t[0]=a; t[1]=b; t[2]=c; t[3]=d;
		return 1;
	}
	if (af == AF_INET6) {
		uint16_t words[8] = {0};
		int idx = 0, compress = -1;
		const char *p = src;

		if (*p == ':') {
			if (*++p != ':') return 0;
			compress = 0;
			idx = 1;
			if (!*p) {
				memset(dst, 0, 16);
				return 1;
			}
		}

		while (*p) {
			if (*p == ':') {
				if (idx >= 8) return 0;
				if (*(p+1) == ':') {
					if (compress != -1) return 0;
					compress = idx;
					idx++;
					p++;
				} else {
					idx++;
				}
				p++;
				continue;
			}

			if (isxdigit((unsigned char)*p)) {
				char *end;
				unsigned long v = strtoul(p, &end, 16);
				if (v > 0xFFFF || idx >= 8) return 0;
				words[idx] = (uint16_t)v;
				idx++;
				p = end;
				continue;
			}

			if (*p == '.' && idx > 0 && idx <= 7) {
				unsigned a,b,c,d;
				if (sscanf(p, "%u.%u.%u.%u", &a,&b,&c,&d) != 4) return 0;
				if (a>255||b>255||c>255||d>255) return 0;
				if (idx > 6) return 0;
				words[idx]   = (uint16_t)((a<<8)|b);
				words[idx+1] = (uint16_t)((c<<8)|d);
				idx += 2;
				break;
			}

			return 0;
		}

		if (idx > 8) return 0;

		if (compress != -1) {
			int filled = idx - 1;
			if (filled < 0) filled = 0;
			int zeros = 8 - filled;
			if (zeros < 0) return 0;
			memmove(&words[compress + zeros], &words[compress], (filled - compress) * 2);
			memset(&words[compress], 0, zeros * 2);
		} else if (idx != 8) {
			return 0;
		}

		unsigned char *out = (unsigned char *)dst;
		for (int i = 0; i < 8; i++) {
			out[i*2]   = (words[i] >> 8) & 0xFF;
			out[i*2+1] = words[i] & 0xFF;
		}
		return 1;
	}
	return -1;
}

static inline const char *inet_ntop(int af, const void *src, char *dst, socklen_t size) {
	if (!src || !dst) return NULL;
	if (af == AF_INET) {
		const unsigned char *b = (const unsigned char *)src;
		if (size < INET_ADDRSTRLEN) return NULL;
		snprintf(dst, size, "%u.%u.%u.%u", b[0], b[1], b[2], b[3]);
		return dst;
	}
	if (af == AF_INET6) {
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
			p += sprintf(p, "%x", words[i]);
		}
		*p = '\0';
		return dst;
	}
	return NULL;
}

/* 2. Legacy Helpers (Depend on Core) */
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
