/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _NET_IP6_H
#define _NET_IP6_H
#pragma once

/**
 * NOTE: This consolidates functionality traditionally found in:
 *
 *   - <netinet/ip6.h>
 *   - <netinet/ip6_var.h>
 *
 * We unify these into a single header at <net/ip6.h> for ease maintenance
 * and clarity with hopes that one day C Standards or POSIX may revamp layout
 * for sanity's sake so that we can remove the abstraction libraries that cause
 * so many security bugs because the coders don't see how things really work.
 *
 * There has to be a better way!!!
 */

#include <config.h>
#include <stdint.h>
#include <net/inet.h> /* For struct in6_addr, IPPROTO_*, IN6ADDR_*_INIT */

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Constants & Macros                                                       */
/* ======================================================================== */
#define IPV6_VERSION_NUM   6  /* Constant value (avoid macro collision) */
#define IPV6_HDR_LEN       40 /* Fixed IPv6 header length */
#define IP6_HDR_LEN        40 /* Alias for compatibility */
#define IPV6_MIN_MTU       1280 /* RFC 2460 minimum link MTU */

#define IP6_VERS_SHIFT     28
#define IP6_VERS_MASK      0xf0000000
#define IP6_TC_MASK        0x0ff00000
#define IP6_FL_MASK        0x000fffff

#define IP6H_VERSION       0x60
#define IP6H_FL_MASK       0x000fffff

/* Bitfield access macros (operate on network-byte-order 32-bit word) */
#define IP6_VERSION(v_tc_fl)    (((v_tc_fl) >> 28) & 0x0F)
#define IP6_TRAFFIC_CLASS(v_tc_fl) (((v_tc_fl) >> 20) & 0xFF)
#define IP6_FLOW_LABEL(v_tc_fl) ((v_tc_fl) & 0x000FFFFF)

#define IP6_SET_VERSION(v)      (((uint32_t)(v) & 0x0F) << 28)
#define IP6_SET_TRAFFIC_CLASS(tc) (((uint32_t)(tc) & 0xFF) << 20)
#define IP6_SET_FLOW_LABEL(fl)  ((uint32_t)(fl) & 0x000FFFFF)

/* ======================================================================== */
/* Main IPv6 Header Structure (Linux-style naming)                          */
/* ======================================================================== */
JACL_LAYOUT
struct ipv6_hdr {
#if JACL_HAS_BE || JACL_HAS_PDP
	/* Big-Endian: First field = MSB */
	uint32_t version:4;
	uint32_t tc:8;
	uint32_t flow:20;
#elif JACL_HAS_LE
	/* Little-Endian: First field = LSB. Reverse order to match MSB layout. */
	uint32_t flow:20;
	uint32_t tc:8;
	uint32_t version:4;
#else
#error "Unsupported endianness. Define JACL_HAS_BE or JACL_HAS_LE."
#endif
	uint16_t plen;
	uint8_t  nexthdr;
	uint8_t  hoplim;
	struct in6_addr saddr;
	struct in6_addr daddr;
} JACL_PACK;

/* ======================================================================== */
/* Main IPv6 Header Structure (BSD-style naming)                            */
/* ======================================================================== */
JACL_LAYOUT
struct ip6_hdr {
#if JACL_HAS_BE || JACL_HAS_PDP
	uint32_t ip6_v:4;
	uint32_t ip6_tc:8;
	uint32_t ip6_flow:20;
#elif JACL_HAS_LE
	uint32_t ip6_flow:20;
	uint32_t ip6_tc:8;
	uint32_t ip6_v:4;
#else
#error "Unsupported endianness. Define JACL_HAS_BE or JACL_HAS_LE."
#endif
	uint16_t ip6_plen;
	uint8_t  ip6_nxt;
	uint8_t  ip6_hlim;
	struct in6_addr ip6_src;
	struct in6_addr ip6_dst;
} JACL_PACK;

/* Linux compatibility shim */
typedef struct ipv6_hdr ipv6hdr;

/* ======================================================================== */
/* Extension Headers                                                        */
/* ======================================================================== */
JACL_LAYOUT
struct ip6_ext {
	uint8_t  ip6e_nxt;
	uint8_t  ip6e_len;
} JACL_PACK;

JACL_LAYOUT
struct ip6_frag {
	uint8_t  ip6f_nxt;
	uint8_t  ip6f_reserved;
	uint16_t ip6f_offlg;
	uint32_t ip6f_ident;
} JACL_PACK;

JACL_LAYOUT
struct ip6_rthdr {
	uint8_t  ip6r_nxt;
	uint8_t  ip6r_len;
	uint8_t  ip6r_type;
	uint8_t  ip6r_segleft;
} JACL_PACK;

/* Fragmentation bits (Network Byte Order) */
#define IP6F_OFF_MASK       0xfff8
#define IP6F_RESERVED_MASK  0x0006
#define IP6F_MORE_FRAG      0x0001

/* ======================================================================== */
/* Address Classification Constants                                         */
/* ======================================================================== */
#define IPV6_ADDR_LOOPBACK     1
#define IPV6_ADDR_MULTICAST    2
#define IPV6_ADDR_LINKLOCAL    3
#define IPV6_ADDR_SITELOCAL    4
#define IPV6_ADDR_V4MAPPED     5
#define IPV6_ADDR_V4COMPAT     6

_Static_assert(sizeof(struct ipv6_hdr) == 40, "ipv6_hdr size wrong");
_Static_assert(sizeof(struct in6_addr) == 16, "in6_addr size wrong");
_Static_assert(sizeof(struct ip6_frag) == 8, "ip6_frag size wrong");
_Static_assert(sizeof(struct ip6_rthdr) == 4, "ip6_rthdr size wrong");

#ifdef __cplusplus
}
#endif

#endif /* _NET_IP6_H */
