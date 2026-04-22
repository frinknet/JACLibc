/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _NET_ICMP_H
#define _NET_ICMP_H
#pragma once

/**
 * NOTE: This consolidates functionality traditionally found in:
 *
 *   - <netinet/ip_icmp.h> (ICMPv4 structures and constants)
 *   - <netinet/icmp6.h> (ICMPv6 and Neighbor Discovery definitions)
 *
 * We unify these into a single header here at <net/igmp.h> for maintenance
 * and clarity with hopes that one day C Standards or POSIX may revamp layout
 * for sanity's sake so that we can remove the abstraction libraries that cause
 * so many security bugs because the codes don't see how things really work.
 *
 * There has to be a better way!!!
 */

#include <config.h>
#include <stdint.h>
#include <net/inet.h> /* For struct in_addr, struct in6_addr */

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* ICMPv4 Types                                                             */
/* ======================================================================== */

#define ICMP_ECHOREPLY		0
#define ICMP_DEST_UNREACH	3
#define ICMP_SOURCE_QUENCH	4
#define ICMP_REDIRECT		5
#define ICMP_ECHO		8
#define ICMP_ROUTER_ADVERT	9
#define ICMP_ROUTER_SOLICIT	10
#define ICMP_TIME_EXCEEDED	11
#define ICMP_PARAMPROB		12
#define ICMP_TIMESTAMP		13
#define ICMP_TIMESTAMPREPLY	14
#define ICMP_INFO_REQUEST	15
#define ICMP_INFO_REPLY		16
#define ICMP_ADDRESS		17
#define ICMP_ADDRESSREPLY	18
#define NR_ICMP_TYPES		18

/* Codes for DEST_UNREACH */
#define ICMP_NET_UNREACH	0
#define ICMP_HOST_UNREACH	1
#define ICMP_PROT_UNREACH	2
#define ICMP_PORT_UNREACH	3
#define ICMP_FRAG_NEEDED	4
#define ICMP_SR_FAILED		5
#define ICMP_NET_UNKNOWN	6
#define ICMP_HOST_UNKNOWN	7
#define ICMP_HOST_ISOLATED	8
#define ICMP_NET_PROHIB		9
#define ICMP_HOST_PROHIB	10
#define ICMP_TOS_NET		11
#define ICMP_TOS_HOST		12
#define ICMP_FILTER_PROHIB	13
#define ICMP_PREC_VIOLATION	14
#define ICMP_PREC_CUTOFF	15

/* Codes for REDIRECT */
#define ICMP_REDIRECT_NET	0
#define ICMP_REDIRECT_HOST	1
#define ICMP_REDIRECT_TOSNET	2
#define ICMP_REDIRECT_TOSHOST	3

/* Codes for TIME_EXCEEDED */
#define ICMP_EXC_TTL		0
#define ICMP_EXC_FRAGTIME	1

/* ======================================================================== */
/* ICMPv4 Header Structure (Standard 8-byte base)                           */
/* ======================================================================== */

JACL_LAYOUT
struct icmp_hdr {
	uint8_t  type;
	uint8_t  code;
	uint16_t checksum;
	union {
		struct {
			uint16_t id;
			uint16_t sequence;
		} echo;
		struct {
			uint16_t pad;
			uint16_t mtu;
		} frag;
		uint32_t gateway;
		struct {
			uint8_t  pointer;
			uint8_t  unused[3];
		} param;
	} un;
} JACL_PACK;

/* BSD-style alias */
JACL_LAYOUT
struct icmp {
	uint8_t  ic_type;
	uint8_t  ic_code;
	uint16_t ic_cksum;
	union {
		struct {
			uint16_t icd_id;
			uint16_t icd_seq;
		} ic_echo;
		struct {
			uint16_t icd_pad;
			uint16_t icd_mtu;
		} ic_frag;
		uint32_t ic_gwaddr;
		struct {
			uint8_t  icd_pptr;
			uint8_t  icd_unused[3];
		} ic_param;
	} icmp_hun;
} JACL_PACK;

#define icmp_id		icmp_hun.ic_echo.icd_id
#define icmp_seq	icmp_hun.ic_echo.icd_seq
#define icmp_pptr	icmp_hun.ic_param.icd_pptr
#define icmp_gwaddr	icmp_hun.ic_gwaddr
#define icmp_mtu	icmp_hun.ic_frag.icd_mtu

/* ======================================================================== */
/* Router Advertisement Structures (Separate to keep base struct small)     */
/* ======================================================================== */

JACL_LAYOUT
struct icmp_router_adv {
	uint8_t  type;
	uint8_t  code;
	uint16_t checksum;
	uint8_t  num_addrs;
	uint8_t  addr_size;
	uint16_t lifetime;
} JACL_PACK;

JACL_LAYOUT
struct icmp_router_entry {
	uint32_t addr;
	uint32_t preference;
} JACL_PACK;

/* ======================================================================== */
/* Timestamp Message Structures                                             */
/* ======================================================================== */

JACL_LAYOUT
struct icmp_timestamp {
	uint8_t  type;
	uint8_t  code;
	uint16_t checksum;
	uint16_t id;
	uint16_t sequence;
	uint32_t originate;
	uint32_t receive;
	uint32_t transmit;
} JACL_PACK;

/* ======================================================================== */
/* ICMPv6 Types                                                             */
/* ======================================================================== */

#define ICMP6_DST_UNREACH	1
#define ICMP6_PACKET_TOO_BIG	2
#define ICMP6_TIME_EXCEEDED	3
#define ICMP6_PARAM_PROB	4
#define ICMP6_ECHO_REQUEST	128
#define ICMP6_ECHO_REPLY	129
#define ICMP6_MLD_LISTENER_QUERY	130
#define ICMP6_MLD_LISTENER_REPORT	131
#define ICMP6_MLD_LISTENER_DONE		132
#define ICMP6_ND_ROUTER_SOLICIT		133
#define ICMP6_ND_ROUTER_ADVERT		134
#define ICMP6_ND_NEIGHBOR_SOLICIT	135
#define ICMP6_ND_NEIGHBOR_ADVERT	136
#define ICMP6_ND_REDIRECT		137

#define ICMP6_DST_UNREACH_NOROUTE	0
#define ICMP6_DST_UNREACH_ADMIN		1
#define ICMP6_DST_UNREACH_BEYONDSCOPE	2
#define ICMP6_DST_UNREACH_ADDR		3
#define ICMP6_DST_UNREACH_NOPORT	4

#define ICMP6_TIME_EXCEED_TRANSIT	0
#define ICMP6_TIME_EXCEED_REASSEMBLY	1

#define ICMP6_PARAMPROB_HEADER		0
#define ICMP6_PARAMPROB_NEXTHEADER	1
#define ICMP6_PARAMPROB_OPTION		2

/* ======================================================================== */
/* ICMPv6 Header Structure                                                  */
/* ======================================================================== */

JACL_LAYOUT
struct icmp6_hdr {
	uint8_t  type;
	uint8_t  code;
	uint16_t checksum;
	union {
		uint32_t pptr;
		uint32_t mtu;
		struct {
			uint16_t id;
			uint16_t sequence;
		} echo;
	} icmp6_dataun;
} JACL_PACK;

#define icmp6_pptr	icmp6_dataun.pptr
#define icmp6_mtu	icmp6_dataun.mtu
#define icmp6_id	icmp6_dataun.echo.id
#define icmp6_seq	icmp6_dataun.echo.sequence

/* ======================================================================== */
/* Neighbor Discovery Structures                                            */
/* ======================================================================== */

JACL_LAYOUT
struct nd_router_solicit {
	struct icmp6_hdr nd_rs_hdr;
} JACL_PACK;

JACL_LAYOUT
struct nd_router_advert {
	struct icmp6_hdr nd_ra_hdr;
	uint8_t  nd_ra_curhoplimit;
	uint8_t  nd_ra_flags_reserved;
	uint16_t nd_ra_router_lifetime;
	uint32_t nd_ra_reachable;
	uint32_t nd_ra_retransmit;
} JACL_PACK;

JACL_LAYOUT
struct nd_neighbor_solicit {
	struct icmp6_hdr nd_ns_hdr;
	struct in6_addr nd_ns_target;
} JACL_PACK;

JACL_LAYOUT
struct nd_neighbor_advert {
	struct icmp6_hdr nd_na_hdr;
	uint32_t nd_na_flags_reserved;
	struct in6_addr nd_na_target;
} JACL_PACK;

JACL_LAYOUT
struct nd_redirect {
	struct icmp6_hdr nd_rd_hdr;
	struct in6_addr nd_rd_target;
	struct in6_addr nd_rd_dst;
} JACL_PACK;

#define ND_OPT_SOURCE_LINKADDR		1
#define ND_OPT_TARGET_LINKADDR		2
#define ND_OPT_PREFIX_INFORMATION	3
#define ND_OPT_REDIRECTED_HEADER	4
#define ND_OPT_MTU			5

/* ======================================================================== */
/* Checksum Helper                                                          */
/* ======================================================================== */

static inline uint16_t icmp_checksum(const void *buf, size_t len) {
	uint32_t sum = 0;
	const uint8_t *ptr = (const uint8_t *)buf;

	while (len > 1) {
		sum += (uint16_t)(ptr[0] << 8 | ptr[1]);
		ptr += 2;
		len -= 2;
	}
	if (len) sum += (uint16_t)(ptr[0] << 8);  /* Odd byte: pad with zero */

	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return (uint16_t)(~sum);
}

#ifdef __cplusplus
}
#endif
#endif /* _NET_ICMP_H */
