/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _NET_DNS_H
#define _NET_DNS_H
#pragma once

/**
 * NOTE: This consolidates functionality traditionally found in:
 *
 *   - <arpa/nameser.h> (Packet structures and macros)
 *   - <resolv.h> (Resolution helpers like dn_expand)
 *
 * We unify these into a single header at <net/dns.h> for easy maintenance
 * and clarity with hopes that one day C Standards or POSIX may revamp layout
 * for sanity's sake so that we can remove the abstraction libraries that cause
 * so many security bugs because the coders don't see how things really work.
 *
 * There has to be a better way!!!
 */

#include <config.h>
#include <stdint.h>
#include <string.h>
#include <net/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Header Structures (from arpa/nameser.h)                                  */
/* ======================================================================== */

JACL_LAYOUT
struct dns_hdr {
	uint16_t id;
#if JACL_HAS_LE
	uint16_t rd:1;
	uint16_t tc:1;
	uint16_t aa:1;
	uint16_t opcode:4;
	uint16_t qr:1;
	uint16_t rcode:4;
	uint16_t z:3;
	uint16_t ra:1;
#elif JACL_HAS_BE
	uint16_t qr:1;
	uint16_t opcode:4;
	uint16_t aa:1;
	uint16_t tc:1;
	uint16_t rd:1;
	uint16_t ra:1;
	uint16_t z:3;
	uint16_t rcode:4;
#else
	uint16_t flags;
#endif
	uint16_t qdcount;
	uint16_t ancount;
	uint16_t nscount;
	uint16_t arcount;
} JACL_PACK;

JACL_LAYOUT
struct dns_q {
	uint16_t qtype;
	uint16_t qclass;
} JACL_PACK;

JACL_LAYOUT
struct dns_rr {
	uint16_t type;
	uint16_t class;
	uint32_t ttl;
	uint16_t rdlength;
} JACL_PACK;

/* Legacy BSD Alias */
typedef struct dns_hdr HEADER;

/* ======================================================================== */
/* Constants (from arpa/nameser.h)                                          */
/* ======================================================================== */

#define DNS_HDR_SIZE sizeof(struct dns_hdr)
#define DNS_MAX_NAME 253
#define DNS_MAX_PACKET 512

#define HFIXEDSZ   sizeof(struct dns_hdr)
#define QFIXEDSZ   sizeof(struct dns_q)
#define RRFIXEDSZ  sizeof(struct dns_rr)
#define INT32SZ    4
#define INT16SZ    2
#define INT8SZ     1

#define DNS_OPCODE_QUERY  0
#define DNS_OPCODE_IQUERY 1
#define DNS_OPCODE_STATUS 2

#define DNS_RCODE_NOERR    0
#define DNS_RCODE_FORMERR  1
#define DNS_RCODE_SERVFAIL 2
#define DNS_RCODE_NXDOMAIN 3

#define DNS_TYPE_A      1
#define DNS_TYPE_NS     2
#define DNS_TYPE_CNAME  5
#define DNS_TYPE_SOA    6
#define DNS_TYPE_PTR    12
#define DNS_TYPE_MX     15
#define DNS_TYPE_TXT    16
#define DNS_TYPE_AAAA   28
#define DNS_TYPE_SRV    33
#define DNS_TYPE_OPT    41
#define DNS_TYPE_ANY    255

#define DNS_CLASS_IN    1
#define DNS_CLASS_CS    2
#define DNS_CLASS_CH    3
#define DNS_CLASS_HS    4
#define DNS_CLASS_ANY   255

/* ======================================================================== */
/* Byte Access Helpers                                                      */
/* ======================================================================== */

static inline uint16_t dns_get16(const uint8_t *ptr) {
	return ((uint16_t)ptr[0] << 8) | ptr[1];
}

static inline void dns_put16(uint8_t *ptr, uint16_t val) {
	ptr[0] = (val >> 8) & 0xff;
	ptr[1] = val & 0xff;
}

static inline uint32_t dns_get32(const uint8_t *ptr) {
	return ((uint32_t)ptr[0] << 24) | ((uint32_t)ptr[1] << 16) |
	       ((uint32_t)ptr[2] << 8) | ptr[3];
}

static inline void dns_put32(uint8_t *ptr, uint32_t val) {
	ptr[0] = (val >> 24) & 0xff;
	ptr[1] = (val >> 16) & 0xff;
	ptr[2] = (val >> 8) & 0xff;
	ptr[3] = val & 0xff;
}

/* Legacy Aliases */
#define ns_get16(p) dns_get16((const uint8_t *)(p))
#define ns_put16(s, p) dns_put16((uint8_t *)(p), (s))
#define ns_get32(p) dns_get32((const uint8_t *)(p))
#define ns_put32(s, p) dns_put32((uint8_t *)(p), (s))

/* ======================================================================== */
/* Name Compression & Expansion (from resolv.h)                             */
/* ======================================================================== */

static inline int dn_expand(const uint8_t *msg, const uint8_t *eom, const uint8_t *src, char *dst, int dstsiz) {
	int pos = 0;
	int consumed = 0;
	const uint8_t *p = src;

	while (p < eom) {
		uint8_t len = *p;

		if (len == 0) {
			if (pos > 0 && pos < dstsiz) dst[pos++] = '\0';
			if (consumed == 0) consumed = (int)(p - src + 1);
			break;
		}

		if ((len & 0xC0) == 0xC0) {
			if (p + 1 >= eom) return -1; /* Not enough bytes for pointer */

			int offset = ((len & 0x3F) << 8) | p[1];

			/* SAFETY CHECK: Ensure target is within bounds */
			if (offset >= (eom - msg)) return -1;

			if (consumed == 0) consumed = (int)(p - src + 2);
			p = msg + offset;
			continue;
		}

		if (len > 63) return -1; /* Invalid label length */
		if (p + 1 + len > eom) return -1; /* Label extends past end */

		if (pos > 0 && pos < dstsiz) dst[pos++] = '.';
		if (pos + len >= dstsiz) return -1; /* Output buffer full */

		memcpy(dst + pos, p + 1, len);
		pos += len;
		p += 1 + len;
	}

	if (pos >= dstsiz) return -1;
	dst[pos] = '\0';

	return consumed ? consumed : (int)(p - src + 1);
}

static inline int dn_comp(const char *src, uint8_t *dst, int dstsiz, uint8_t **dnptrs, uint8_t **lastdnptr) {
	const char *label = src;
	int pos = 0;

	if (!src || !dst || dstsiz <= 0) return -1;

	/* Special case: Root domain "." */
	if (strcmp(src, ".") == 0) {
		if (dstsiz < 1) return -1;
		dst[0] = 0;
		return 1;
	}

	/* Compression table lookup (if provided) */
	if (dnptrs && *dnptrs && lastdnptr && *lastdnptr) {
		const char *check = src;
		while (*check) {
			const char *dot = strchr(check, '.');
			int len = dot ? (int)(dot - check) : (int)strlen(check);
            if (len == 0) { /* Skip empty labels caused by trailing dots or ".." */
                if (!dot) break;
                check = dot + 1;
                continue;
            }
			for (uint8_t **pp = dnptrs; pp < lastdnptr && *pp; pp++) {
				const uint8_t *entry = *pp;
				if (entry[0] == len && memcmp(entry + 1, check, len) == 0) {
					if (pos + 2 > dstsiz) return -1;
					int offset = (int)(entry - dst);
					dst[pos++] = 0xC0 | ((offset >> 8) & 0x3F);
					dst[pos++] = offset & 0xFF;
					return pos;
				}
			}
			if (!dot) break;
			check = dot + 1;
		}
	}

	/* Emit literal labels */
	label = src;
	while (*label) {
		const char *dot = strchr(label, '.');
		int len = dot ? (int)(dot - label) : (int)strlen(label);

        /* Skip empty labels (e.g., trailing dots) */
        if (len == 0) {
            if (!dot) break;
            label = dot + 1;
            continue;
        }

		if (len > 63 || pos + len + 1 >= dstsiz) return -1;

		if (dnptrs && lastdnptr && *dnptrs < *lastdnptr) {
			*(*dnptrs)++ = dst + pos;
		}

		dst[pos++] = (uint8_t)len;
		memcpy(dst + pos, label, len);
		pos += len;

		if (!dot) break;
		label = dot + 1;
	}

	if (pos + 1 >= dstsiz) return -1;
	dst[pos++] = 0;

	return pos;
}

/* ======================================================================== */
/* Resolver Stubs (from resolv.h)                                           */
/* ======================================================================== */

static inline int res_init(void) { return 0; }
static inline int res_query(const char *name, int class, int type, uint8_t *answer, int anslen) {
	(void)name; (void)class; (void)type; (void)answer; (void)anslen;
	return -1;
}

#ifdef __cplusplus
}
#endif
#endif /* _NET_DNS_H */
