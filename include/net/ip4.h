/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _NET_IP4_H
#define _NET_IP4_H
#pragma once

/**
 * NOTE: This header consolidates functionality traditionally found in:
 *
 *   - <netinet/ip.h> (Raw IPv4 Headers - BSD & Linux)
 *   - <netinet/in_systm.h> (Legacy Types)
 *
 * We unify these into a single header at <net/ip6.h> for ease of maintenance
 * and clarity with hopes that one day C Standards or POSIX may revamp layout
 * for sanity's sake so that we can remove the abstraction of legacy cruft that
 * cause so many security bugs because the coders don't see how things work.
 *
 * There has to be a better way!!!
 *
 * COMPATIBILITY:
 * - struct ipv4_hdr: Linux-style naming (saddr, daddr)
 * - struct ip: BSD-style naming (ip_src, ip_dst)
 * Both have identical 20-byte memory layout.
 */

#include <config.h>
#include <stdint.h>
#include <net/inet.h> /* For struct in_addr, IPPROTO_*, etc. */

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Legacy Type Aliases (from in_systm.h)                                    */
/* ======================================================================== */
typedef uint16_t n_short;
typedef uint32_t n_long;
typedef uint32_t n_time;

/* ======================================================================== */
/* IPv4 Header Structure (Linux-style naming)                               */
/* ======================================================================== */
JACL_LAYOUT
struct ipv4_hdr {
#if JACL_HAS_BE
	uint8_t  version:4;
	uint8_t  ihl:4;
#elif JACL_HAS_LE
	uint8_t  ihl:4;
	uint8_t  version:4;
#else
#error "Unsupported endianness. Define JACL_HAS_BE or JACL_HAS_LE."
#endif
	uint8_t  tos;
	uint16_t tot_len;
	uint16_t id;
	uint16_t frag_off;
	uint8_t  ttl;
	uint8_t  protocol;
	uint16_t check;
	uint32_t saddr;
	uint32_t daddr;
} JACL_PACK;

/* ======================================================================== */
/* IPv4 Header Structure (BSD-style naming)                                 */
/* ======================================================================== */
JACL_LAYOUT
struct ip {
#if JACL_HAS_BE
	uint8_t  ip_v:4;
	uint8_t  ip_hl:4;
#elif JACL_HAS_LE
	uint8_t  ip_hl:4;
	uint8_t  ip_v:4;
#else
#error "Unsupported endianness. Define JACL_HAS_BE or JACL_HAS_LE."
#endif
	uint8_t  ip_tos;
	uint16_t ip_len;
	uint16_t ip_id;
	uint16_t ip_off;
	uint8_t  ip_ttl;
	uint8_t  ip_p;
	uint16_t ip_sum;
	struct in_addr ip_src;
	struct in_addr ip_dst;
} JACL_PACK;

/* Linux compatibility shim */
typedef struct ipv4_hdr iphdr;

#define IP4_HDR_LEN  sizeof(struct ipv4_hdr)
#define IPV4_HDR_LEN sizeof(struct ipv4_hdr)
#define IP_VERSION_4 4
#define IPVERSION    4

/* ======================================================================== */
/* Fragmentation Flags                                                      */
/* ======================================================================== */
#define IP_RF         0x8000 /* Reserved bit */
#define IP_DF         0x4000 /* Don't Fragment */
#define IP_MF         0x2000 /* More Fragments */
#define IP_OFFMASK    0x1fff /* Mask for fragment offset */

/* ======================================================================== */
/* Type of Service / DSCP Constants                                         */
/* ======================================================================== */
#define IPTOS_ECN_MASK		0x03
#define IPTOS_ECN(x)		((x) & IPTOS_ECN_MASK)
#define IPTOS_ECN_NOT_ECT	0x00
#define IPTOS_ECN_ECT1		0x01
#define IPTOS_ECN_ECT0		0x02
#define IPTOS_ECN_CE		0x03

#define IPTOS_DSCP_MASK		0xfc
#define IPTOS_DSCP(x)		((x) & IPTOS_DSCP_MASK)
#define IPTOS_DSCP_AF11		0x28
#define IPTOS_DSCP_AF12		0x30
#define IPTOS_DSCP_AF13		0x38
#define IPTOS_DSCP_AF21		0x48
#define IPTOS_DSCP_AF22		0x50
#define IPTOS_DSCP_AF23		0x58
#define IPTOS_DSCP_AF31		0x68
#define IPTOS_DSCP_AF32		0x70
#define IPTOS_DSCP_AF33		0x78
#define IPTOS_DSCP_AF41		0x88
#define IPTOS_DSCP_AF42		0x90
#define IPTOS_DSCP_AF43		0x98
#define IPTOS_DSCP_EF		0xb8

#define IPTOS_CLASS_MASK	0xe0
#define IPTOS_CLASS(x)		((x) & IPTOS_CLASS_MASK)
#define IPTOS_CLASS_CS0		0x00
#define IPTOS_CLASS_CS1		0x20
#define IPTOS_CLASS_CS2		0x40
#define IPTOS_CLASS_CS3		0x60
#define IPTOS_CLASS_CS4		0x80
#define IPTOS_CLASS_CS5		0xa0
#define IPTOS_CLASS_CS6		0xc0
#define IPTOS_CLASS_CS7		0xe0

#define IPTOS_LOWDELAY		0x10
#define IPTOS_THROUGHPUT	0x08
#define IPTOS_RELIABILITY	0x04
#define IPTOS_LOWCOST		0x02
#define IPTOS_MINCOST		IPTOS_LOWCOST

#define IPTOS_PREC_MASK		0xe0
#define IPTOS_PREC_NETCONTROL		0xe0
#define IPTOS_PREC_INTERNETCONTROL	0xc0
#define IPTOS_PREC_CRITIC_ECP		0xa0
#define IPTOS_PREC_FLASHOVERRIDE	0x80
#define IPTOS_PREC_FLASH		0x60
#define IPTOS_PREC_IMMEDIATE		0x40
#define IPTOS_PREC_PRIORITY		0x20
#define IPTOS_PREC_ROUTINE		0x00

/* ======================================================================== */
/* IP Option Constants                                                      */
/* ======================================================================== */
#define IPOPT_EOL		0
#define IPOPT_NOP		1
#define IPOPT_RR		7
#define IPOPT_TS		68
#define IPOPT_SECURITY		130
#define IPOPT_LSRR		131
#define IPOPT_SSRR		137
#define IPOPT_RA		148

#define MAX_IPOPTLEN		40
#define IPOPT_MINOFF		4

/* ======================================================================== */
/* TTL and MSS Constants                                                    */
/* ======================================================================== */
#define MAXTTL		255
#define IPDEFTTL	64
#define IPFRAGTTL	60
#define IPTTLDEC	1
#define IP_MSS		576
#define IP_MAXPACKET	65535

/* ======================================================================== */
/* Byte-Access Macros                                                       */
/* ======================================================================== */
#define IP_VERS_HL(v, hl)    (((uint8_t)(v) << 4) | ((uint8_t)(hl) & 0x0F))
#define IP_VERSION(b)        ((uint8_t)(b) >> 4)
#define IP_HL(b)             ((uint8_t)(b) & 0x0F)

/* IPv4-prefixed aliases for compatibility */
#define IPV4_VERS_IHL(v, hl) IP_VERS_HL(v, hl)
#define IPV4_VERSION(b)      IP_VERSION(b)
#define IPV4_IHL(b)          IP_HL(b)

/* ======================================================================== */
/* Helper: Calculate IPv4 Header Checksum                                   */
/* ======================================================================== */
static inline uint16_t ip_checksum(void *buf, int len) {
	uint32_t sum = 0;
	uint8_t *ptr = (uint8_t *)buf;

	while (len > 1) {
		sum += (uint16_t)(ptr[0] << 8 | ptr[1]);
		ptr += 2;
		len -= 2;
	}
	if (len) sum += (uint16_t)(ptr[0] << 8);
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return (uint16_t)(~sum);
}

/* Legacy alias */
#define ipv4_checksum ip_checksum

#ifdef __cplusplus
}
#endif

#endif /* _NET_IP4_H */
