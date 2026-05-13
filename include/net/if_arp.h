/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _NET_IF_ARP_H
#define _NET_IF_ARP_H
#pragma once

#include <config.h>
#include <stdint.h>
#include <sys/socket.h>
#include <net/if.h>    /* For IFNAMSIZ */
#include <net/inet.h>  /* For struct in_addr */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ETH_ALEN
#define ETH_ALEN 6
#endif

/* ======================================================================== */
/* ARP Header Structure (RFC 826)                                           */
/* ======================================================================== */

JACL_LAYOUT
struct arphdr {
	uint16_t ar_hrd;
	uint16_t ar_pro;
	uint8_t  ar_hln;
	uint8_t  ar_pln;
	uint16_t ar_op;
} JACL_PACK;

JACL_LAYOUT
struct ether_arp {
	struct arphdr ea_hdr;
	uint8_t arp_sha[ETH_ALEN];
	uint8_t arp_spa[4];
	uint8_t arp_tha[ETH_ALEN];
	uint8_t arp_tpa[4];
} JACL_PACK;

/* Legacy aliases (requires struct field named 'ea_hdr') */
#define arp_hrd   ea_hdr.ar_hrd
#define arp_pro   ea_hdr.ar_pro
#define arp_hln   ea_hdr.ar_hln
#define arp_pln   ea_hdr.ar_pln
#define arp_op    ea_hdr.ar_op

/* ======================================================================== */
/* ARP Cache Entry (POSIX/Linux ioctl)                                      */
/* NOTE: DO NOT PACK. Kernel ABI expects natural padding/alignment.         */
/* ======================================================================== */

struct arpreq {
	struct sockaddr arp_pa;
	struct sockaddr arp_ha;
	int             arp_flags;
	struct sockaddr arp_netmask;
	char            arp_dev[IFNAMSIZ];
};

/* ======================================================================== */
/* Constants & Flags                                                        */
/* ======================================================================== */

#define ATF_COM         0x02
#define ATF_PERM        0x04
#define ATF_PUBL        0x08
#define ATF_USETRAILERS 0x10
#define ATF_NETMASK     0x20
#define ATF_DONTPUB     0x40

#define ARPHRD_NETROM     0
#define ARPHRD_ETHER      1
#define ARPHRD_EETHER     2
#define ARPHRD_AX25       3
#define ARPHRD_PRONET     4
#define ARPHRD_CHAOS      5
#define ARPHRD_IEEE802    6
#define ARPHRD_ARCNET     7
#define ARPHRD_ATALK      8
#define ARPHRD_HDLC       17
#define ARPHRD_FRELAY     15
#define ARPHRD_LOOPBACK   772
#define ARPHRD_IEEE1394   24
#define ARPHRD_IPGRE      778
#define ARPHRD_IRDA       783
#define ARPHRD_INFINIBAND 32

#define ARPOP_REQUEST    1
#define ARPOP_REPLY      2
#define ARPOP_RREQUEST   3
#define ARPOP_RREPLY     4
#define ARPOP_INVREQUEST 8
#define ARPOP_INVREPLY   9

#define SIOCSARP  0x8955
#define SIOCGARP  0x8956
#define SIOCDARP  0x8957

/* ======================================================================== */
/* Inline Helpers                                                           */
/* ======================================================================== */

/**
 * NOTE: These helpers operate on in-memory structs in HOST byte order.
 * For wire parsing (raw sockets), convert with ntohs() first.
 * For ioctl results (SIOCGARP), use directly (kernel converts).
 */

static inline int arphdr_is_arp(const struct arphdr *h) {
	return h && (h->ar_op == ARPOP_REQUEST || h->ar_op == ARPOP_REPLY);
}

static inline int arphdr_is_valid(const struct arphdr *h) {
	return h && h->ar_hln > 0 && h->ar_pln > 0 && h->ar_op > 0;
}

#ifdef __cplusplus
}
#endif

#endif /* _NET_IF_ARP_H */
