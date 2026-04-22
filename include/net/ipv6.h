/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _NET_IPV6_H
#define _NET_IPV6_H
#pragma once

#include <config.h>
#include <stdint.h>
#include <net/inet.h> /* For struct in6_addr */

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Constants & Macros                                                       */
/* ======================================================================== */
#define IPV6_VERSION		6
#define IPV6_HDR_LEN		sizeof(struct ipv6_hdr)

#define IP6_VERS_SHIFT		28
#define IP6_VERS_MASK		0xf0000000
#define IP6_TC_MASK		0x0ff00000
#define IP6_FL_MASK		0x000fffff

#define IP6H_VERSION		0x60
#define IP6H_FL_MASK		0x000fffff

/* Bitfield access macros (C99/C11 fallback) */
#define IP6_VERSION(v_tc_fl)    (((v_tc_fl) >> 28) & 0x0F)
#define IP6_TRAFFIC_CLASS(v_tc_fl) (((v_tc_fl) >> 20) & 0xFF)
#define IP6_FLOW_LABEL(v_tc_fl) ((v_tc_fl) & 0x000FFFFF)

#define IP6_SET_VERSION(v)      (((uint32_t)(v) & 0x0F) << 28)
#define IP6_SET_TRAFFIC_CLASS(tc) (((uint32_t)(tc) & 0xFF) << 20)
#define IP6_SET_FLOW_LABEL(fl)  ((uint32_t)(fl) & 0x000FFFFF)

/* ======================================================================== */
/* Main IPv6 Header Structure                                               */
/* ======================================================================== */
JACL_LAYOUT
struct ipv6_hdr {
#if JACL_HAS_BE
	uint32_t version:4;
	uint32_t tc:8;
	uint32_t flow:20;
#else
	/* C99/C11 fallback: default to network-order (big-endian) bit layout */
	uint32_t version:4;
	uint32_t tc:8;
	uint32_t flow:20;
#endif
	uint16_t plen;
	uint8_t  nexthdr;
	uint8_t  hoplim;
	struct in6_addr saddr;
	struct in6_addr daddr;
} JACL_PACK;

/* BSD-style alias */
JACL_LAYOUT
struct ip6_hdr {
#if JACL_HAS_BE
	uint32_t ip6_v:4;
	uint32_t ip6_tc:8;
	uint32_t ip6_flow:20;
#else
	/* C99/C11 fallback: default to network-order (big-endian) bit layout */
	uint32_t ip6_v:4;
	uint32_t ip6_tc:8;
	uint32_t ip6_flow:20;
#endif
	uint16_t ip6_plen;
	uint8_t  ip6_nxt;
	uint8_t  ip6_hlim;
	struct in6_addr ip6_src;
	struct in6_addr ip6_dst;
} JACL_PACK;

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

/* Fragmentation bits */
#define IP6F_OFF_MASK		0xfff8
#define IP6F_RESERVED_MASK	0x0006
#define IP6F_MORE_FRAG		0x0001

/* ======================================================================== */
/* Protocol Numbers (Extension Headers)                                     */
/* ======================================================================== */
#define IPPROTO_HOPOPTS	0
#define IPPROTO_ROUTING	43
#define IPPROTO_FRAGMENT 44
#define IPPROTO_ESP		50
#define IPPROTO_AH		51
#define IPPROTO_ICMPV6	58
#define IPPROTO_NONE	59
#define IPPROTO_DSTOPTS	60

/* ======================================================================== */
/* Address Constants                                                        */
/* ======================================================================== */
#define IN6ADDR_ANY_INIT		{ { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } } }
#define IN6ADDR_LOOPBACK_INIT	{ { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 } } }

#define IPV6_ADDR_LOOPBACK	1
#define IPV6_ADDR_MULTICAST	2
#define IPV6_ADDR_LINKLOCAL	3
#define IPV6_ADDR_SITELOCAL	4
#define IPV6_ADDR_V4MAPPED	5
#define IPV6_ADDR_V4COMPAT	6

#ifdef __cplusplus
}
#endif
#endif /* _NET_IPV6_H */
