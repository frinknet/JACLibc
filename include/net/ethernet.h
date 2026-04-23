/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _NET_ETHERNET_H
#define _NET_ETHERNET_H
#pragma once

#include <config.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Core Constants & Sizes (IEEE 802.3, RFC 894)                             */
/* ======================================================================== */
#define ETH_ALEN		6
#define ETHER_ADDR_LEN		ETH_ALEN
#define ETH_HLEN		14
#define ETH_ZLEN		60
#define ETH_DATA_LEN		1500
#define ETH_FCS_LEN		4
#define ETH_FRAME_LEN		(ETH_DATA_LEN + ETH_HLEN + ETH_FCS_LEN)
#define ETH_MIN_MTU		68
#define ETH_MAX_MTU		0xFFFFU
#define ETH_P_802_3_MIN		0x0600 /* Type/Length boundary */

/* ======================================================================== */
/* MAC Address Types & Masks                                                */
/* ======================================================================== */
#define ETH_ADDR_UNICAST	0x00
#define ETH_ADDR_MULTICAST	0x01
#define ETH_ADDR_BROADCAST	0xFF
#define ETH_ADDR_LOCAL_MASK	0x02
#define ETH_ADDR_GLOBAL_MASK	(~ETH_ADDR_LOCAL_MASK)

/* ======================================================================== */
/* Ethernet Protocol Types (Linux ETH_P_* / BSD ETHERTYPE_*)                */
/* ======================================================================== */
/* Core Network */
#define ETH_P_IP		0x0800
#define ETH_P_IPV6		0x86DD
#define ETH_P_ARP		0x0806
#define ETH_P_RARP		0x8035
#define ETH_P_LOOP		0x9000

/* VLAN & QoS */
#define ETH_P_8021Q		0x8100
#define ETH_P_QINQ		0x9100
#define ETH_P_8021AD		0x88A8

/* Control & Management */
#define ETH_P_PAUSE		0x8808
#define ETH_P_SLOW		0x8809
#define ETH_P_LLDP		0x88CC
#define ETH_P_WCCP		0x883E

/* Routing & Encapsulation */
#define ETH_P_PPP_DISC		0x8863
#define ETH_P_PPP_SES		0x8864
#define ETH_P_MPLS_UC		0x8847
#define ETH_P_MPLS_MC		0x8848
#define ETH_P_TEB		0x6558 /* Transparent Ethernet Bridging */

/* Legacy / Specialized */
#define ETH_P_IPX		0x8137
#define ETH_P_ATALK		0x809B
#define ETH_P_DEC		0x6000
#define ETH_P_DNA_DL		0x6001
#define ETH_P_DNA_RC		0x6002
#define ETH_P_DNA_RT		0x6003
#define ETH_P_BATMAN		0x4305
#define ETH_P_CAN		0x000C
#define ETH_P_CANFD		0x000D
#define ETH_P_X25		0x0805
#define ETH_P_SNAP		0x0005
#define ETH_P_802_2		0x0004

/* Promiscuous Capture */
#define ETH_P_ALL		0x0003

/* BSD/POSIX Aliases */
#define ETHERTYPE_IP		ETH_P_IP
#define ETHERTYPE_ARP		ETH_P_ARP
#define ETHERTYPE_IPV6		ETH_P_IPV6
#define ETHERTYPE_VLAN		ETH_P_8021Q
#define ETHERTYPE_LOOP		ETH_P_LOOP
#define ETHERTYPE_IPX		ETH_P_IPX
#define ETHERTYPE_ATALK		ETH_P_ATALK
#define ETHERTYPE_PPP		ETH_P_PPP_SES
#define ETHERTYPE_SLOW		ETH_P_SLOW

/* ======================================================================== */
/* MAC Address Structure                                                    */
/* ======================================================================== */
JACL_LAYOUT
struct ether_addr {
	uint8_t ether_addr_octet[ETH_ALEN];
} JACL_PACK;

/* ======================================================================== */
/* Ethernet Header (Wire Format)                                            */
/* ======================================================================== */
JACL_LAYOUT
struct ethhdr {
	uint8_t	h_dest[ETH_ALEN];
	uint8_t	h_source[ETH_ALEN];
	uint16_t h_proto;
} JACL_PACK;

/* BSD/POSIX Alias */
JACL_LAYOUT
struct ether_header {
	uint8_t	ether_dhost[ETH_ALEN];
	uint8_t	ether_shost[ETH_ALEN];
	uint16_t ether_type;
} JACL_PACK;

/* ======================================================================== */
/* VLAN Header & Structures (IEEE 802.1Q)                                   */
/* ======================================================================== */
JACL_LAYOUT
struct vlan_hdr {
	uint16_t h_vlan_TCI;
	uint16_t h_vlan_encapsulated_proto;
} JACL_PACK;

JACL_LAYOUT
struct vlan_ethhdr {
	uint8_t	h_dest[ETH_ALEN];
	uint8_t	h_source[ETH_ALEN];
	uint16_t h_vlan_proto;
	uint16_t h_vlan_TCI;
	uint16_t h_vlan_encapsulated_proto;
} JACL_PACK;

#define VLAN_PRIO_MASK		0xe000
#define VLAN_PRIO_SHIFT		13
#define VLAN_CFI_MASK		0x1000
#define VLAN_VID_MASK		0x0fff

/* ======================================================================== */
/* Inline Helpers (Header-Only)                                             */
/* ======================================================================== */
static inline int ether_is_broadcast(const uint8_t *addr) {
	return addr[0] == 0xFF && addr[1] == 0xFF && addr[2] == 0xFF &&
	       addr[3] == 0xFF && addr[4] == 0xFF && addr[5] == 0xFF;
}

static inline int ether_is_multicast(const uint8_t *addr) {
	return (addr[0] & ETH_ADDR_MULTICAST) != 0;
}

static inline int ether_is_unicast(const uint8_t *addr) {
	return !ether_is_multicast(addr);
}

static inline int ether_is_local(const uint8_t *addr) {
	return (addr[0] & ETH_ADDR_LOCAL_MASK) != 0;
}

static inline int ether_is_global(const uint8_t *addr) {
	return !ether_is_local(addr);
}

static inline int ether_addr_equal(const uint8_t *a, const uint8_t *b) {
	return a[0] == b[0] && a[1] == b[1] && a[2] == b[2] &&
	       a[3] == b[3] && a[4] == b[4] && a[5] == b[5];
}

static inline int ether_is_valid(const uint8_t *addr) {
	return addr && !ether_is_broadcast(addr);
}

static inline uint16_t vlan_get_prio(uint16_t tci) { return (tci >> VLAN_PRIO_SHIFT) & 0x7; }
static inline int vlan_get_cfi(uint16_t tci) { return (tci >> 12) & 0x1; }
static inline uint16_t vlan_get_vid(uint16_t tci) { return tci & VLAN_VID_MASK; }

/* ======================================================================== */
/* Standard String Conversions (Declared, implemented in libc/libnet)       */
/* ======================================================================== */
char *ether_ntoa(const struct ether_addr *addr);
struct ether_addr *ether_aton(const char *ascii);
int ether_ntohost(char *hostname, const struct ether_addr *addr);
int ether_hostton(const char *hostname, struct ether_addr *addr);

#ifdef __cplusplus
}
#endif
#endif /* _NET_ETHERNET_H */
