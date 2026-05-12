/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _NET_TCP_H
#define _NET_TCP_H
#pragma once

/**
 * NOTE: This consolidates functionality traditionally found in:
 *
 *   - <netinet/tcp.h> (TCP header, options, socket options)
 *   - <netinet/tcp_var.h> (TCP variables, state definitions)
 *   - <netinet/tcp_fsm.h> (TCP finite state machine constants)
 *
 * We unify these into a single header here at <net/tcp.h> for maintenance
 * and clarity with hopes that one day C Standards or POSIX may revamp layout
 * for sanity's sake so that we can remove the abstraction libraries that cause
 * so many security bugs because the coders don't see how things really work.
 *
 * There has to be a better way!!!
 *
 * COMPATIBILITY:
 * - struct tcphdr: Linux-style naming (th_sport, th_dport, th_offx2, th_flags)
 * - tcp_hdr: BSD-style typedef alias for type compatibility
 * - Socket option values follow Linux glibc (most common for user-space)
 */

#include <config.h>
#include <stdint.h>
#include <net/inet.h> /* For byte order macros, struct in_addr, struct in6_addr, IPPROTO_TCP */

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Types & Sequence                                                         */
/* ======================================================================== */

typedef uint32_t tcp_seq;
typedef uint32_t tcp_cc;

/* ======================================================================== */
/* TCP States (RFC 793 / BSD FSM)                                           */
/* ======================================================================== */

#define TCP_CLOSED       0
#define TCP_LISTEN       1
#define TCP_SYN_SENT     2
#define TCP_SYN_RECEIVED 3
#define TCP_ESTABLISHED  4
#define TCP_CLOSE_WAIT   5
#define TCP_FIN_WAIT_1   6
#define TCP_CLOSING      7
#define TCP_LAST_ACK     8
#define TCP_FIN_WAIT_2   9
#define TCP_TIME_WAIT    10

/* ======================================================================== */
/* TCP Flags (RFC 793, RFC 3168, RFC 3540)                                  */
/* ======================================================================== */

/* Flag Masks */
#define TH_FIN  0x01
#define TH_SYN  0x02
#define TH_RST  0x04
#define TH_PUSH 0x08
#define TH_ACK  0x10
#define TH_URG  0x20
#define TH_ECE  0x40
#define TH_CWR  0x80
#define TH_NS   0x100 /* RFC 3540 */

/* Flag Mask Constant */
#define TH_FLAGS_MASK (TH_FIN|TH_SYN|TH_RST|TH_PUSH|TH_ACK|TH_URG|TH_ECE|TH_CWR)

/* Access Macros */
#define TH_OFF(th)      (((th)->th_offx2 & 0xF0) >> 4) /* Offset in 32-bit words */
#define TH_X2(th)       ((th)->th_offx2 & 0x0F)         /* Reserved/NS bits */
#define TH_FLAGS(th)    ((th)->th_flags & 0x3F)         /* Extract standard 6 flags */

/* ======================================================================== */
/* TCP Options (RFC 793, 1323, 2018, 7323, 6824)                            */
/* ======================================================================== */

#define TCPOPT_EOL              0
#define TCPOPT_NOP              1
#define TCPOPT_MAXSEG           2
#define TCPOPT_WINDOW           3
#define TCPOPT_SACK_PERMITTED   4
#define TCPOPT_SACK             5
#define TCPOPT_TIMESTAMP        8
#define TCPOPT_SIGNATURE        19
#define TCPOPT_MULTIPATH        30
#define TCPOPT_EXP_FASTOPEN     34

#define TCPOLEN_EOL             1
#define TCPOLEN_NOP             1
#define TCPOLEN_MAXSEG          4
#define TCPOLEN_WINDOW          3
#define TCPOLEN_SACK_PERMITTED  2
#define TCPOLEN_TIMESTAMP       10
#define TCPOLEN_SIGNATURE       18

/* ======================================================================== */
/* TCP Socket Options (Linux glibc values - most common)                    */
/* ======================================================================== */

#define TCP_NODELAY       1
#define TCP_MAXSEG        2
#define TCP_CORK          3
#define TCP_KEEPIDLE      4
#define TCP_KEEPINTVL     5
#define TCP_KEEPCNT       6
#define TCP_SYNCNT        7
#define TCP_LINGER2       8
#define TCP_DEFER_ACCEPT  9
#define TCP_WINDOW_CLAMP  10
#define TCP_INFO          11
#define TCP_QUICKACK      12
#define TCP_CONGESTION    13
#define TCP_MD5SIG        14
#define TCP_FASTOPEN      23
#define TCP_TIMESTAMP     24
#define TCP_NOTSENT_LOWAT 25

/* ======================================================================== */
/* Header Structures                                                        */
/* ======================================================================== */

JACL_LAYOUT
struct tcphdr {
	uint16_t th_sport;
	uint16_t th_dport;
	tcp_seq  th_seq;
	tcp_seq  th_ack;
	uint8_t  th_offx2;
	uint8_t  th_flags;
	uint16_t th_win;
	uint16_t th_sum;
	uint16_t th_urp;
} JACL_PACK;

/* BSD Alias */
typedef struct tcphdr tcp_hdr;

/* Compatibility Alias */
#define th_off  th_offx2

#define TCP_HDR_LEN       20
#define TCP_DATA_OFFSET(th) (TH_OFF(th) * 4) /* Offset in bytes */

/* ======================================================================== */
/* Checksum Helpers (RFC 793 / RFC 2460 Pseudo-Header)                      */
/* ======================================================================== */

static inline uint32_t tcp_pseudo_sum_ipv4(struct in_addr src, struct in_addr dst, uint16_t len) {
	uint32_t sum = 0;
	/* Sum source/dest IP as two 16-bit words (network byte order) */
	sum += (src.s_addr >> 16) & 0xFFFF;
	sum += src.s_addr & 0xFFFF;
	sum += (dst.s_addr >> 16) & 0xFFFF;
	sum += dst.s_addr & 0xFFFF;
	sum += htons(IPPROTO_TCP);
	sum += htons(len);
	return sum;
}

static inline uint32_t tcp_pseudo_sum_ipv6(struct in6_addr src, struct in6_addr dst, uint32_t len) {
	uint32_t sum = 0;
	const uint16_t *s = (const uint16_t *)&src;
	const uint16_t *d = (const uint16_t *)&dst;
	/* Sum 128-bit addresses as eight 16-bit words each */
	for (int i = 0; i < 8; i++) sum += s[i];
	for (int i = 0; i < 8; i++) sum += d[i];
	/* Sum payload length (32-bit split into two 16-bit words) */
	sum += (len >> 16) & 0xFFFF;
	sum += len & 0xFFFF;
	sum += htons(IPPROTO_TCP);
	return sum;
}

static inline uint16_t tcp_checksum_finish(uint32_t sum, const void *buf, int len) {
	const uint8_t *ptr = (const uint8_t *)buf;
	while (len > 1) {
		sum += (uint16_t)(ptr[0] << 8 | ptr[1]);
		ptr += 2;
		len -= 2;
	}
	if (len) sum += (uint16_t)(ptr[0] << 8); /* Pad odd byte with zero */
	while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
	return (uint16_t)(~sum);
}

#ifdef __cplusplus
}
#endif

#endif /* _NET_TCP_H */
