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
 * We unify these into a single header here at <net/igmp.h> for maintenance
 * and clarity with hopes that one day C Standards or POSIX may revamp layout
 * for sanity's sake so that we can remove the abstraction libraries that cause
 * so many security bugs because the codes don't see how things really work.
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

#define IGMP_MEMBERSHIP_QUERY     0x11  /* Host Membership Query */
#define IGMP_V1_MEMBERSHIP_REPORT 0x12 /* IGMPv1 Host Membership Report */
#define IGMP_V2_MEMBERSHIP_REPORT 0x16 /* IGMPv2 Host Membership Report */
#define IGMP_V3_MEMBERSHIP_REPORT 0x22 /* IGMPv3 Host Membership Report */
#define IGMP_LEAVE_GROUP          0x17  /* IGMPv2 Leave Group */

/* IGMPv3 Group Record Types */
#define IGMP_MODE_IS_INCLUDE     1  /* MODE {INCLUDE} */
#define IGMP_MODE_IS_EXCLUDE     2  /* MODE {EXCLUDE} */
#define IGMP_CHANGE_TO_INCLUDE   3  /* CHANGE_TO_INCLUDE */
#define IGMP_CHANGE_TO_EXCLUDE   4  /* CHANGE_TO_EXCLUDE */
#define IGMP_ALLOW_NEW_SOURCES   5  /* ALLOW_NEW_SOURCES */
#define IGMP_BLOCK_OLD_SOURCES   6  /* BLOCK_OLD_SOURCES */

/* ======================================================================== */
/* IGMP Constants                                                           */
/* ======================================================================== */

#define IGMP_MINLEN            8    /* Minimum IGMPv1/v2 message length */
#define IGMP_MAXLEN            576  /* Maximum IGMP message (IP MTU min) */
#define IGMP_TIMER_SCALE       10   /* 10ths of seconds for timer values */

/* Multicast addresses (RFC 2236) */
#define IGMP_ALL_HOSTS         htonl(0xE0000001) /* 224.0.0.1 */
#define IGMP_ALL_ROUTERS       htonl(0xE0000002) /* 224.0.0.2 */
#define IGMP_V3_ALL_ROUTERS    htonl(0xE0000016) /* 224.0.0.22 */

/* ======================================================================== */
/* IGMPv1/v2 Header Structure (RFC 2236)                                    */
/* ======================================================================== */

/* igmp main struct */
struct igmp {
	uint8_t  igmp_type;    /* Message type */
	uint8_t  igmp_code;    /* Max response time / unused */
	uint16_t igmp_cksum;   /* Checksum */
	struct in_addr igmp_group; /* Group address (network order) */
};

/* Modern alias with explicit naming */
struct igmp_hdr {
	uint8_t  type;
	uint8_t  max_resp_time;
	uint16_t checksum;
	struct in_addr group;
};

/* ======================================================================== */
/* IGMPv3 Structures (RFC 3376)                                             */
/* ======================================================================== */

/* IGMPv3 Query Message */
struct igmpv3_query {
	uint8_t  type;           /* 0x11 */
	uint8_t  max_resp_time;  /* Scaled by IGMP_TIMER_SCALE */
	uint16_t checksum;
	struct in_addr group;    /* 0 for general query */
#if defined(__STDC_ENDIAN_LITTLE__) && __STDC_ENDIAN_LITTLE__
	uint8_t  resv:4;
	uint8_t  s:1;            /* Suppress router-side processing */
	uint8_t  qrv:3;          /* Querier's Robustness Variable */
#elif defined(__STDC_ENDIAN_BIG__) && __STDC_ENDIAN_BIG__
	uint8_t  s:1;
	uint8_t  qrv:3;
	uint8_t  resv:4;
#else
	uint8_t  flags;
#endif
	uint8_t  qqic;           /* Querier's Query Interval Code */
	uint16_t num_src;        /* Number of source addresses */
	/* Followed by num_src source addresses (struct in_addr[]) */
};

/* IGMPv3 Group Record */
struct igmpv3_grec {
	uint8_t  grec_type;      /* Record type (MODE_*, CHANGE_*, etc.) */
	uint8_t  grec_auxlen;    /* Length of auxiliary data (in 32-bit words) */
	uint16_t grec_nsrcs;     /* Number of source addresses */
	struct in_addr grec_mca; /* Multicast address */
	/* Followed by grec_nsrcs source addresses, then aux data */
};

/* IGMPv3 Membership Report */
struct igmpv3_report {
	uint8_t  type;           /* 0x22 */
	uint8_t  resv1;
	uint16_t checksum;
	uint16_t resv2;
	uint16_t num_grps;       /* Number of group records */
	/* Followed by num_grps struct igmpv3_grec[] */
};

/* ======================================================================== */
/* Checksum Helper (Standard Internet Checksum)                             */
/* ======================================================================== */

static inline uint16_t igmp_checksum(const void *buf, int len) {
	uint32_t sum = 0;
	const uint16_t *p = (const uint16_t *)buf;
	while (len > 1) {
		sum += *p++;
		len -= 2;
	}
	if (len) sum += *(const uint8_t *)p;
	while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
	return (uint16_t)(~sum);
}

/* ======================================================================== */
/* Inline Helpers                                                           */
/* ======================================================================== */

/* Check if address is IGMP multicast range (224.0.0.0/4) */
static inline int igmp_is_multicast(struct in_addr addr) {
	return (ntohl(addr.s_addr) & 0xF0000000) == 0xE0000000;
}

/* Check if address is IGMP local network control block (224.0.0.0/24) */
static inline int igmp_is_local_control(struct in_addr addr) {
	return (ntohl(addr.s_addr) & 0xFFFFFF00) == 0xE0000000;
}

/* Validate IGMP message length */
static inline int igmp_valid_len(int type, int len) {
	switch (type) {
		case IGMP_MEMBERSHIP_QUERY:
		case IGMP_V1_MEMBERSHIP_REPORT:
		case IGMP_V2_MEMBERSHIP_REPORT:
		case IGMP_LEAVE_GROUP:
			return len >= IGMP_MINLEN;
		case IGMP_V3_MEMBERSHIP_REPORT:
			return len >= 8; /* type+resv+cksum+resv+num_grps */
		default:
			return 0;
	}
}

#ifdef __cplusplus
}
#endif
#endif /* _NET_IGMP_H */
