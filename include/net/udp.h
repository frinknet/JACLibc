/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _NET_UDP_H
#define _NET_UDP_H
#pragma once

/**
 * NOTE: This consolidates functionality traditionally found in:
 *
 *   - <netinet/udp.h> (UDP header structure)
 *   - <netinet/udp_var.h> (UDP variables, PCB helpers)
 *   - <netinet/udpip.h> (UDP-in-IP encapsulation helpers)
 *
 * We unify these into a single header here at <net/udp.h> for maintenance
 * and clarity with hopes that one day C Standards or POSIX may revamp layout
 * for sanity's sake so that we can remove the abstraction libraries that cause
 * so many security bugs because the codes don't see how things really work.
 *
 * There has to be a better way!!!
 */

#include <stdint.h>
#include <net/inet.h> /* For byte order macros, struct in_addr, struct in6_addr */

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Header Structures                                                        */
/* ======================================================================== */

struct udp_hdr {
	uint16_t source;
	uint16_t dest;
	uint16_t len;
	uint16_t check;
};

/* BSD/POSIX Alias */
struct udphdr {
	uint16_t uh_sport;
	uint16_t uh_dport;
	uint16_t uh_ulen;
	uint16_t uh_sum;
};

#define UDP_HDR_LEN	8

/* ======================================================================== */
/* Pseudo-Header Structure (RFC 768 / RFC 2460)                             */
/* Used for checksum calculation                                            */
/* ======================================================================== */

struct udppseudo {
	struct in_addr src;
	struct in_addr dst;
	uint8_t  zero;
	uint8_t  proto;
	uint16_t len;
};

struct udppseudo6 {
	struct in6_addr src;
	struct in6_addr dst;
	uint32_t len;
	uint8_t  zero[3];
	uint8_t  next_header;
};

/* ======================================================================== */
/* UDP Socket Options (Linux/BSD compatible subset)                         */
/* ======================================================================== */

#define UDP_CORK		        1
#define UDP_ENCAP	        	100
#define UDP_NO_CHECK6_TX	  100
#define UDP_NO_CHECK6_RX   	101
#define UDP_NO_CHECK_TX     100 /* Deprecated alias for NO_CHECK6_TX */
#define UDP_NO_CHECK_RX     101 /* Deprecated alias for NO_CHECK6_RX */
#define UDP_SEGMENT     		103 /* GSO/UDP segmentation */


/* ======================================================================== */
/* Checksum Helpers (RFC 768 Pseudo-Header)                                 */
/* ======================================================================== */

/* Calculate initial sum of pseudo-header fields */
static inline uint32_t udp_pseudo_sum_ipv4(struct in_addr src, struct in_addr dst,
                                           uint16_t len) {
	uint32_t sum = 0;
	sum += (src.s_addr >> 16) & 0xFFFF;
	sum += src.s_addr & 0xFFFF;
	sum += (dst.s_addr >> 16) & 0xFFFF;
	sum += dst.s_addr & 0xFFFF;
	sum += htons(IPPROTO_UDP);
	sum += htons(len);
	return sum;
}

/* Finalize checksum from initial sum and data buffer */
static inline uint16_t udp_checksum_finish(uint32_t sum, const void *buf, int len) {
	const uint16_t *p = (const uint16_t *)buf;
	while (len > 1) {
		sum += *p++;
		len -= 2;
	}
	if (len) sum += *(const uint8_t *)p;
	while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
	return (uint16_t)(~sum);
}

/* One-shot helper for IPv4 UDP checksum */
static inline uint16_t udp_checksum_ipv4(struct in_addr src, struct in_addr dst,
                                         const void *data, uint16_t len) {
	uint32_t sum = udp_pseudo_sum_ipv4(src, dst, len);
	return udp_checksum_finish(sum, data, len);
}

/* One-shot helper for IPv6 UDP checksum */
static inline uint16_t udp_checksum_ipv6(struct in6_addr src, struct in6_addr dst, const void *data, uint32_t len) {
	uint32_t sum = 0;
	const uint16_t *s = (const uint16_t *)&src;
	const uint16_t *d = (const uint16_t *)&dst;

	for (int i = 0; i < 8; i++) sum += s[i];
	for (int i = 0; i < 8; i++) sum += d[i];

	sum += htonl(len);
	sum += htons(IPPROTO_UDP);

	return udp_checksum_finish(sum, data, len);
}

#ifdef __cplusplus
}
#endif
#endif /* _NET_UDP_H */
