/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _NET_IF_ARP_H
#define _NET_IF_ARP_H
#pragma once

#include <config.h>
#include <stdint.h>
#include <net/inet.h>

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
	uint16_t ar_hrd; /* Hardware address format */
	uint16_t ar_pro; /* Protocol address format */
	uint8_t  ar_hln; /* Hardware address length */
	uint8_t  ar_pln; /* Protocol address length */
	uint16_t ar_op;  /* ARP opcode */
} JACL_PACK;

/* Ethernet-specific ARP packet (RFC 826 Section 3) */
JACL_LAYOUT
struct ether_arp {
	struct arphdr ea_hdr;
	uint8_t arp_sha[ETH_ALEN]; /* Sender hardware addr */
	uint8_t arp_spa[4];        /* Sender protocol addr */
	uint8_t arp_tha[ETH_ALEN]; /* Target hardware addr */
	uint8_t arp_tpa[4];        /* Target protocol addr */
} JACL_PACK;

/* Standard field aliases for ether_arp */
#define arp_hrd   ea_hdr.ar_hrd
#define arp_pro   ea_hdr.ar_pro
#define arp_hln   ea_hdr.ar_hln
#define arp_pln   ea_hdr.ar_pln
#define arp_op    ea_hdr.ar_op

/* ======================================================================== */
/* ARP Cache Entry Structure (POSIX/Linux ioctl)                            */
/* ======================================================================== */

JACL_LAYOUT
struct arpreq {
	struct sockaddr arp_pa;      /* Protocol address */
	struct sockaddr arp_ha;      /* Hardware address */
	int             arp_flags;   /* ATF_* flags */
	struct sockaddr arp_netmask; /* Netmask for proxy ARP */
	char            arp_dev[16]; /* Device name */
} JACL_PACK;

/* ======================================================================== */
/* ARP Entry Flags (ATF_*)                                                  */
/* ======================================================================== */

#define ATF_COM         0x02  /* Completed entry */
#define ATF_PERM        0x04  /* Permanent entry */
#define ATF_PUBL        0x08  /* Publish entry */
#define ATF_USETRAILERS 0x10  /* Trailer encapsulation */
#define ATF_NETMASK     0x20  /* Use netmask */
#define ATF_DONTPUB     0x40  /* Do not publish */

/* ======================================================================== */
/* Hardware Type Constants (ARPHRD_*)                                       */
/* ======================================================================== */

#define ARPHRD_NETROM     0
#define ARPHRD_ETHER      1
#define ARPHRD_EETHER     2
#define ARPHRD_AX25       3
#define ARPHRD_PRONET     4
#define ARPHRD_CHAOS      5
#define ARPHRD_IEEE802    6
#define ARPHRD_ARCNET     7
#define ARPHRD_ATALK      8
#define ARPHRD_ASH        9
#define ARPHRD_ECONET     10
#define ARPHRD_HDLC       17
#define ARPHRD_FRELAY     15
#define ARPHRD_LOOPBACK   772
#define ARPHRD_LOCALTLK   773
#define ARPHRD_FCFABRIC   784
#define ARPHRD_IEEE1394   24
#define ARPHRD_IPGRE      778
#define ARPHRD_IRDA       783
#define ARPHRD_INFINIBAND 32

/* ======================================================================== */
/* ARP Operation Codes (ARPOP_*)                                            */
/* ======================================================================== */

#define ARPOP_REQUEST    1 /* ARP request */
#define ARPOP_REPLY      2 /* ARP reply */
#define ARPOP_RREQUEST   3 /* RARP request */
#define ARPOP_RREPLY     4 /* RARP reply */
#define ARPOP_INVREQUEST 8 /* InARP request */
#define ARPOP_INVREPLY   9 /* InARP reply */

/* ======================================================================== */
/* Ioctl Command Constants (Linux/POSIX)                                    */
/* ======================================================================== */

#define SIOCSARP  0x8955  /* Set ARP entry */
#define SIOCGARP  0x8956  /* Get ARP entry */
#define SIOCDARP  0x8957  /* Delete ARP entry */

/* ======================================================================== */
/* Inline Helpers (Header-Only Utilities)                                   */
/* ======================================================================== */

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
