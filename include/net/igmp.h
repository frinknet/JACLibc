/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _NET_IGMP_H
#define _NET_IGMP_H
#pragma once

/**
 * NOTE: This consolidates functionality traditionally found in:
 *
 *   - <netinet/igmp.h> (IGMPv2 structures and constants)
 *   - <linux/igmp.h> (IGMPv3 extensions)
 *
 * We unify these into a single header at <net/igmp.h> for ease of maintenance
 * and clarity with hopes that one day C Standards or POSIX may revamp layout
 * for sanity's sake so that we can remove the abstraction of legacy cruft that
 * cause so many security bugs because the coders don't see how things work.
 *
 * There has to be a better way!!!
 */

#include <config.h>
#include <stdint.h>
#include <net/inet.h> /* For struct in_addr, byte order macros */

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* IGMP Message Types (RFC 2236, RFC 3376)                                  */
/* ======================================================================== */

#define IGMP_MEMBERSHIP_QUERY     0x11
#define IGMP_V1_MEMBERSHIP_REPORT 0x12
#define IGMP_V2_MEMBERSHIP_REPORT 0x16
#define IGMP_V3_MEMBERSHIP_REPORT 0x22
#define IGMP_LEAVE_GROUP          0x17

/* IGMPv3 Group Record Types */
#define IGMP_MODE_IS_INCLUDE      1
#define IGMP_MODE_IS_EXCLUDE      2
#define IGMP_CHANGE_TO_INCLUDE    3
#define IGMP_CHANGE_TO_EXCLUDE    4
#define IGMP_ALLOW_NEW_SOURCES    5
#define IGMP_BLOCK_OLD_SOURCES    6

/* ======================================================================== */
/* IGMP Constants                                                           */
/* ======================================================================== */

#define IGMP_MINLEN               8
#define IGMP_MAXLEN               576
#define IGMP_TIMER_SCALE          10

/* Multicast addresses (Network Byte Order) */
#define IGMP_ALL_HOSTS            htonl(0xE0000001U) /* 224.0.0.1 */
#define IGMP_ALL_ROUTERS          htonl(0xE0000002U) /* 224.0.0.2 */
#define IGMP_V3_ALL_ROUTERS       htonl(0xE0000016U) /* 224.0.0.22 */

/* ======================================================================== */
/* IGMPv1/v2 Header Structure (RFC 2236)                                    */
/* ======================================================================== */

JACL_LAYOUT
struct igmp {
	uint8_t  igmp_type;
	uint8_t  igmp_code;
	uint16_t igmp_cksum;
	struct in_addr igmp_group;
} JACL_PACK;

/* Modern alias */
JACL_LAYOUT
struct igmp_hdr {
	uint8_t  type;
	uint8_t  max_resp_time;
	uint16_t checksum;
	struct in_addr group;
} JACL_PACK;

/* ======================================================================== */
/* IGMPv3 Structures (RFC 3376)                                             */
/* Zero bitfields. Scalar + macros guarantee wire layout across compilers.  */
/* ======================================================================== */

/* IGMPv3 Query Message */
JACL_LAYOUT
struct igmpv3_query {
	uint8_t  type;
	uint8_t  max_resp_time;
	uint16_t checksum;
	struct in_addr group;
	uint8_t  s_qrv;   /* Bit 7: S, Bits 4-6: QRV, Bits 0-3: Resv */
	uint8_t  qqic;
	uint16_t num_src;
} JACL_PACK;

#define IGMPV3_Q_S(q)      (((q)->s_qrv >> 7) & 1)
#define IGMPV3_Q_QRV(q)    (((q)->s_qrv >> 4) & 0x07)
#define IGMPV3_Q_RESV(q)   ((q)->s_qrv & 0x0F)

/* IGMPv3 Group Record */
JACL_LAYOUT
struct igmpv3_grec {
	uint8_t  grec_type;
	uint8_t  grec_auxlen;
	uint16_t grec_nsrcs;
	struct in_addr grec_mca;
	/* Followed by grec_nsrcs source addresses (struct in_addr[]) */
} JACL_PACK;

/* IGMPv3 Membership Report */
JACL_LAYOUT
struct igmpv3_report {
	uint8_t  type;
	uint8_t  resv1;
	uint16_t checksum;
	uint16_t resv2;
	uint16_t num_grps;
	/* Followed by num_grps struct igmpv3_grec[] */
} JACL_PACK;

/* ======================================================================== */
/* Checksum Helper (Standard Internet Checksum)                             */
/* Byte-wise access prevents unaligned faults on RISC/ARM.                  */
/* ======================================================================== */

static inline uint16_t igmp_checksum(const void *buf, int len) {
	uint32_t sum = 0;
	const uint8_t *ptr = (const uint8_t *)buf;

	while (len > 1) {
		sum += (uint16_t)(ptr[0] << 8 | ptr[1]);
		ptr += 2;
		len -= 2;
	}
	if (len) sum += (uint16_t)(ptr[0] << 8);
	while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
	return (uint16_t)(~sum);
}

/* ======================================================================== */
/* Inline Helpers                                                           */
/* ======================================================================== */

static inline int igmp_is_multicast(struct in_addr addr) {
	return (ntohl(addr.s_addr) & 0xF0000000) == 0xE0000000;
}

static inline int igmp_is_local_control(struct in_addr addr) {
	return (ntohl(addr.s_addr) & 0xFFFFFF00) == 0xE0000000;
}

static inline int igmp_valid_len(int type, int len) {
	switch (type) {
		case IGMP_MEMBERSHIP_QUERY:
		case IGMP_V1_MEMBERSHIP_REPORT:
		case IGMP_V2_MEMBERSHIP_REPORT:
		case IGMP_LEAVE_GROUP:
			return len >= IGMP_MINLEN;
		case IGMP_V3_MEMBERSHIP_REPORT:
			return len >= 8;
		default:
			return 0;
	}
}

#ifdef __cplusplus
}
#endif

#endif /* _NET_IGMP_H */
