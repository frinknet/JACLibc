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
 * We unify these into a single header here at <net/icmp.h> for maintenance
 * and clarity with hopes that one day C Standards or POSIX may revamp layout
 * for sanity's sake so that we can remove the abstraction libraries that cause
 * so many security bugs because the coders don't see how things really work.
 *
 * There has to be a better way!!!
 *
 * COMPATIBILITY:
 * - struct icmp:    BSD/POSIX primary (ICMPv4)
 * - struct icmp6_hdr: BSD/POSIX primary (ICMPv6)
 * - Linux aliases provided via macros
 */

#include <config.h>
#include <stdint.h>
#include <net/inet.h> /* For struct in_addr, struct in6_addr, IPPROTO_ICMP */

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Protocol Numbers                                                         */
/* ======================================================================== */

#ifndef IPPROTO_ICMPV6
#define IPPROTO_ICMPV6  58
#endif

/* ======================================================================== */
/* ICMPv4 Types & Codes (RFC 792)                                           */
/* ======================================================================== */

#define ICMP_ECHOREPLY          0
#define ICMP_DEST_UNREACH       3
#define ICMP_SOURCE_QUENCH      4
#define ICMP_REDIRECT           5
#define ICMP_ECHO               8
#define ICMP_ROUTER_ADVERT      9
#define ICMP_ROUTER_SOLICIT     10
#define ICMP_TIME_EXCEEDED      11
#define ICMP_PARAMPROB          12
#define ICMP_TIMESTAMP          13
#define ICMP_TIMESTAMPREPLY     14
#define ICMP_INFO_REQUEST       15
#define ICMP_INFO_REPLY         16
#define ICMP_ADDRESS            17
#define ICMP_ADDRESSREPLY       18

/* Codes for DEST_UNREACH */
#define ICMP_NET_UNREACH        0
#define ICMP_HOST_UNREACH       1
#define ICMP_PROT_UNREACH       2
#define ICMP_PORT_UNREACH       3
#define ICMP_FRAG_NEEDED        4
#define ICMP_SR_FAILED          5
#define ICMP_NET_UNKNOWN        6
#define ICMP_HOST_UNKNOWN       7
#define ICMP_HOST_ISOLATED      8
#define ICMP_NET_PROHIB         9
#define ICMP_HOST_PROHIB        10
#define ICMP_TOS_NET            11
#define ICMP_TOS_HOST           12
#define ICMP_FILTER_PROHIB      13
#define ICMP_PREC_VIOLATION     14
#define ICMP_PREC_CUTOFF        15

/* Codes for REDIRECT */
#define ICMP_REDIRECT_NET       0
#define ICMP_REDIRECT_HOST      1
#define ICMP_REDIRECT_TOSNET    2
#define ICMP_REDIRECT_TOSHOST   3

/* Codes for TIME_EXCEEDED */
#define ICMP_EXC_TTL            0
#define ICMP_EXC_FRAGTIME       1

/* ======================================================================== */
/* ICMPv4 Header Structures (BSD/POSIX Primary)                             */
/* ======================================================================== */

JACL_LAYOUT
struct icmp {
	uint8_t   ic_type;
	uint8_t   ic_code;
	uint16_t  ic_cksum;
	union {
		struct { uint16_t icd_id; uint16_t icd_seq; } ic_echo;
		struct { uint16_t icd_pad; uint16_t icd_mtu; } ic_frag;
		uint32_t ic_gwaddr;
		struct { uint8_t icd_pptr; uint8_t icd_unused[3]; } ic_param;
		uint32_t ic_void;
	} icmp_hun;
} JACL_PACK;

/* Linux Aliases */
#define icmp_hdr        icmp
#define icmp_id         icmp_hun.ic_echo.icd_id
#define icmp_seq        icmp_hun.ic_echo.icd_seq
#define icmp_pptr       icmp_hun.ic_param.icd_pptr
#define icmp_gwaddr     icmp_hun.ic_gwaddr
#define icmp_mtu        icmp_hun.ic_frag.icd_mtu
#define icmp_void       icmp_hun.ic_void

/* Router Advertisement (RFC 1256) */
JACL_LAYOUT
struct icmp_router_adv {
	uint8_t   type;
	uint8_t   code;
	uint16_t  checksum;
	uint8_t   num_addrs;
	uint8_t   addr_size;
	uint16_t  lifetime;
} JACL_PACK;

JACL_LAYOUT
struct icmp_router_entry {
	uint32_t  addr;
	uint32_t  preference;
} JACL_PACK;

/* Timestamp Message (RFC 792) */
JACL_LAYOUT
struct icmp_timestamp {
	uint8_t   type;
	uint8_t   code;
	uint16_t  checksum;
	uint16_t  id;
	uint16_t  sequence;
	uint32_t  originate;
	uint32_t  receive;
	uint32_t  transmit;
} JACL_PACK;

/* ======================================================================== */
/* ICMPv6 Types & Codes (RFC 4443)                                          */
/* ======================================================================== */

#define ICMP6_DST_UNREACH           1
#define ICMP6_PACKET_TOO_BIG        2
#define ICMP6_TIME_EXCEEDED         3
#define ICMP6_PARAM_PROB            4
#define ICMP6_ECHO_REQUEST          128
#define ICMP6_ECHO_REPLY            129
#define ICMP6_MLD_LISTENER_QUERY    130
#define ICMP6_MLD_LISTENER_REPORT   131
#define ICMP6_MLD_LISTENER_DONE     132
#define ICMP6_ND_ROUTER_SOLICIT     133
#define ICMP6_ND_ROUTER_ADVERT      134
#define ICMP6_ND_NEIGHBOR_SOLICIT   135
#define ICMP6_ND_NEIGHBOR_ADVERT    136
#define ICMP6_ND_REDIRECT           137

/* Codes for DST_UNREACH */
#define ICMP6_DST_UNREACH_NOROUTE       0
#define ICMP6_DST_UNREACH_ADMIN         1
#define ICMP6_DST_UNREACH_BEYONDSCOPE   2
#define ICMP6_DST_UNREACH_ADDR          3
#define ICMP6_DST_UNREACH_NOPORT        4

/* Codes for TIME_EXCEEDED */
#define ICMP6_TIME_EXCEED_TRANSIT       0
#define ICMP6_TIME_EXCEED_REASSEMBLY    1

/* Codes for PARAM_PROB */
#define ICMP6_PARAMPROB_HEADER          0
#define ICMP6_PARAMPROB_NEXTHEADER      1
#define ICMP6_PARAMPROB_OPTION          2

/* ======================================================================== */
/* ICMPv6 Header Structure (BSD/POSIX Primary)                              */
/* ======================================================================== */

JACL_LAYOUT
struct icmp6_hdr {
	uint8_t   icmp6_type;
	uint8_t   icmp6_code;
	uint16_t  icmp6_cksum;
	union {
		uint32_t icmp6_pptr;
		uint32_t icmp6_mtu;
		struct { uint16_t icmp6_id; uint16_t icmp6_seq; } icmp6_echo;
	} icmp6_dataun;
} JACL_PACK;

/* Field Access Macros */
#define icmp6_pptr    icmp6_dataun.icmp6_pptr
#define icmp6_mtu     icmp6_dataun.icmp6_mtu
#define icmp6_id      icmp6_dataun.icmp6_echo.icmp6_id
#define icmp6_seq     icmp6_dataun.icmp6_echo.icmp6_seq

/* ======================================================================== */
/* Neighbor Discovery Messages (RFC 4861)                                   */
/* ======================================================================== */

JACL_LAYOUT
struct nd_router_solicit {
	struct icmp6_hdr  nd_rs_hdr;
} JACL_PACK;

JACL_LAYOUT
struct nd_router_advert {
	struct icmp6_hdr  nd_ra_hdr;
	uint8_t           nd_ra_curhoplimit;
	uint8_t           nd_ra_flags_reserved;
	uint16_t          nd_ra_router_lifetime;
	uint32_t          nd_ra_reachable;
	uint32_t          nd_ra_retransmit;
} JACL_PACK;

JACL_LAYOUT
struct nd_neighbor_solicit {
	struct icmp6_hdr  nd_ns_hdr;
	struct in6_addr   nd_ns_target;
} JACL_PACK;

JACL_LAYOUT
struct nd_neighbor_advert {
	struct icmp6_hdr  nd_na_hdr;
	uint32_t          nd_na_flags_reserved;
	struct in6_addr   nd_na_target;
} JACL_PACK;

JACL_LAYOUT
struct nd_redirect {
	struct icmp6_hdr  nd_rd_hdr;
	struct in6_addr   nd_rd_target;
	struct in6_addr   nd_rd_dst;
} JACL_PACK;

/* ======================================================================== */
/* ND Option Types (RFC 4861)                                               */
/* ======================================================================== */

#define ND_OPT_SOURCE_LINKADDR      1
#define ND_OPT_TARGET_LINKADDR      2
#define ND_OPT_PREFIX_INFORMATION   3
#define ND_OPT_REDIRECTED_HEADER    4
#define ND_OPT_MTU                  5
#define ND_OPT_SOURCE_LLADDR        ND_OPT_SOURCE_LINKADDR
#define ND_OPT_TARGET_LLADDR        ND_OPT_TARGET_LINKADDR

/* ND Option Header */
JACL_LAYOUT
struct nd_opt_hdr {
	uint8_t   nd_opt_type;
	uint8_t   nd_opt_len;
} JACL_PACK;

/* Prefix Information Option (RFC 4861) */
JACL_LAYOUT
struct nd_opt_prefix_info {
	uint8_t         nd_opt_pi_type;
	uint8_t         nd_opt_pi_len;
	uint8_t         nd_opt_pi_prefix_len;
	uint8_t         nd_opt_pi_flags_reserved;
	uint32_t        nd_opt_pi_valid_time;
	uint32_t        nd_opt_pi_preferred_time;
	uint32_t        nd_opt_pi_reserved2;
	struct in6_addr nd_opt_pi_prefix;
} JACL_PACK;

/* MTU Option (RFC 4861) */
JACL_LAYOUT
struct nd_opt_mtu {
	uint8_t   nd_opt_mtu_type;
	uint8_t   nd_opt_mtu_len;
	uint16_t  nd_opt_mtu_reserved;
	uint32_t  nd_opt_mtu_mtu;
} JACL_PACK;

/* ======================================================================== */
/* ND Flags (RFC 4861)                                                      */
/* ======================================================================== */

#define ND_RA_FLAG_MANAGED          0x80
#define ND_RA_FLAG_OTHER            0x40
#define ND_RA_FLAG_HOME_AGENT       0x20
#define ND_NA_FLAG_ROUTER           0x80000000
#define ND_NA_FLAG_SOLICITED        0x40000000
#define ND_NA_FLAG_OVERRIDE         0x20000000
#define ND_RS_FLAG_ROUTER           0x80000000

/* Prefix Flags */
#define ND_OPT_PI_FLAG_ONLINK       0x80
#define ND_OPT_PI_FLAG_AUTO         0x40
#define ND_OPT_PI_FLAG_RADDR        0x20

/* ======================================================================== */
/* ICMPv4 Pseudo-Header (for documentation - ICMPv4 has no pseudo-header)   */
/* ======================================================================== */

struct icmppseudo {
	struct in_addr  src;
	struct in_addr  dst;
	uint8_t         zero;
	uint8_t         proto;
	uint16_t        len;
};

/* ======================================================================== */
/* ICMPv6 Pseudo-Header (RFC 4443)                                          */
/* ======================================================================== */

struct icmp6_pseudo {
	struct in6_addr  src;
	struct in6_addr  dst;
	uint32_t         len;
	uint8_t          zero[3];
	uint8_t          next_header;
};

/* ======================================================================== */
/* Checksum Helpers (RFC 792 / RFC 4443)                                    */
/* ======================================================================== */

/* ICMPv4 checksum (no pseudo-header - RFC 792) */
static inline uint16_t icmp_checksum(const void *buf, int len) {
	uint32_t sum = 0;
	const uint8_t *ptr = (const uint8_t *)buf;

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

/* ICMPv6 checksum (includes pseudo-header - RFC 4443) */
static inline uint16_t icmp6_checksum(struct in6_addr src, struct in6_addr dst,
                                       const void *buf, uint32_t len) {
	uint32_t sum = 0;
	const uint16_t *s = (const uint16_t *)&src;
	const uint16_t *d = (const uint16_t *)&dst;
	const uint8_t *ptr = (const uint8_t *)buf;

	/* Source + Dest addresses (128 bits each = 8 x 16-bit words) */
	for (int i = 0; i < 8; i++) sum += s[i];
	for (int i = 0; i < 8; i++) sum += d[i];

	/* Upper-layer packet length */
	sum += (len >> 16) & 0xFFFF;
	sum += len & 0xFFFF;

	/* Next header (ICMPv6 = 58) */
	sum += htons(IPPROTO_ICMPV6);

	/* ICMPv6 payload */
	while (len > 1) {
		sum += (uint16_t)(ptr[0] << 8 | ptr[1]);
		ptr += 2;
		len -= 2;
	}
	if (len) sum += (uint16_t)(ptr[0] << 8);

	/* Fold and complement */
	while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
	return (uint16_t)(~sum);
}

/* ======================================================================== */
/* Size Constants                                                           */
/* ======================================================================== */

#define ICMP_HDR_LEN        sizeof(struct icmp)
#define ICMP6_HDR_LEN       sizeof(struct icmp6_hdr)
#define ICMP_ECHO_LEN       8
#define ICMP6_ECHO_LEN      8

#ifdef __cplusplus
}
#endif

#endif /* _NET_ICMP_H */
