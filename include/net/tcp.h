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
 * so many security bugs because the codes don't see how things really work.
 *
 * There has to be a better way!!!
 */

#include <config.h>
#include <stdint.h>
#include <net/inet.h> /* For byte order macros, struct in_addr */

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Types & Sequence                                                         */
/* ======================================================================== */

typedef uint32_t tcp_seq;
typedef uint32_t tcp_cc; /* congestion control state */

/* ======================================================================== */
/* TCP States (from tcp_fsm.h)                                              */
/* ======================================================================== */

#define TCP_CLOSED		0
#define TCP_LISTEN		1
#define TCP_SYN_SENT		2
#define TCP_SYN_RECEIVED	3
#define TCP_ESTABLISHED		4
#define TCP_CLOSE_WAIT		5
#define TCP_FIN_WAIT_1		6
#define TCP_CLOSING		7
#define TCP_LAST_ACK		8
#define TCP_FIN_WAIT_2		9
#define TCP_TIME_WAIT		10

/* ======================================================================== */
/* TCP Flags (RFC 793, RFC 3168, RFC 3540)                                  */
/* ======================================================================== */

#define TH_FIN		0x01
#define TH_SYN		0x02
#define TH_RST		0x04
#define TH_PUSH		0x08
#define TH_ACK		0x10
#define TH_URG		0x20
#define TH_ECE		0x40 /* ECN-Echo */
#define TH_CWR		0x80 /* Congestion Window Reduced */
#define TH_NS		0x100 /* Nonce Sum (RFC 3540) */

#define TH_FLAGS	(TH_FIN|TH_SYN|TH_RST|TH_PUSH|TH_ACK|TH_URG|TH_ECE|TH_CWR)

/* ======================================================================== */
/* TCP Options (RFC 793, 1323, 2018, 7323, 6824)                            */
/* ======================================================================== */

#define TCPOPT_EOL		0
#define TCPOPT_NOP		1
#define TCPOPT_MAXSEG		2 /* MSS */
#define TCPOPT_WINDOW		3 /* Window Scale */
#define TCPOPT_SACK_PERMITTED	4
#define TCPOPT_SACK		5
#define TCPOPT_TIMESTAMP	8
#define TCPOPT_SIGNATURE	19 /* TCP MD5 Signature */
#define TCPOPT_MULTIPATH	30 /* MPTCP */
#define TCPOPT_EXP_FASTOPEN	34 /* TFO (Experimental) */

#define TCPOLEN_EOL		1
#define TCPOLEN_NOP		1
#define TCPOLEN_MAXSEG		4
#define TCPOLEN_WINDOW		3
#define TCPOLEN_SACK_PERMITTED	2
#define TCPOLEN_TIMESTAMP	10
#define TCPOLEN_SIGNATURE	18

/* ======================================================================== */
/* TCP Socket Options (Linux/BSD compatible subset)                         */
/* ======================================================================== */

#define TCP_NODELAY		1
#define TCP_MAXSEG		2
#define TCP_CORK		3
#define TCP_KEEPIDLE		4
#define TCP_KEEPINTVL		5
#define TCP_KEEPCNT		6
#define TCP_SYNCNT		7
#define TCP_LINGER2		8
#define TCP_DEFER_ACCEPT	9
#define TCP_WINDOW_CLAMP	10
#define TCP_INFO		11
#define TCP_QUICKACK		12
#define TCP_CONGESTION		13
#define TCP_MD5SIG		14
#define TCP_FASTOPEN		30
#define TCP_TIMESTAMP		31
#define TCP_NOTSENT_LOWAT	32

/* ======================================================================== */
/* Header Structures                                                        */
/* ======================================================================== */

JACL_LAYOUT
struct tcp_hdr {
	uint16_t source;
	uint16_t dest;
	tcp_seq  seq;
	tcp_seq  ack_seq;
#if JACL_HAS_LE
	uint16_t res1:4;
	uint16_t doff:4;
	uint16_t fin:1;
	uint16_t syn:1;
	uint16_t rst:1;
	uint16_t psh:1;
	uint16_t ack:1;
	uint16_t urg:1;
	uint16_t res2:2;
#elif JACL_HAS_BE
	uint16_t doff:4;
	uint16_t res1:4;
	uint16_t res2:2;
	uint16_t urg:1;
	uint16_t ack:1;
	uint16_t psh:1;
	uint16_t rst:1;
	uint16_t syn:1;
	uint16_t fin:1;
#else
	/* C99/C11 fallback: defaults to LE bit ordering.
	   Bitfield packing is implementation-defined; flatten to masks for strict cross-arch compliance. */
	uint16_t res1:4;
	uint16_t doff:4;
	uint16_t fin:1;
	uint16_t syn:1;
	uint16_t rst:1;
	uint16_t psh:1;
	uint16_t ack:1;
	uint16_t urg:1;
	uint16_t res2:2;
#endif
	uint16_t window;
	uint16_t check;
	uint16_t urg_ptr;
} JACL_PACK;

/* BSD/POSIX Alias */
JACL_LAYOUT
struct tcphdr {
	uint16_t th_sport;
	uint16_t th_dport;
	tcp_seq  th_seq;
	tcp_seq  th_ack;
#if JACL_HAS_LE
	uint8_t  th_x2:4;
	uint8_t  th_off:4;
#elif JACL_HAS_BE
	uint8_t  th_off:4;
	uint8_t  th_x2:4;
#else
	/* C99/C11 fallback: defaults to LE bit ordering */
	uint8_t  th_x2:4;
	uint8_t  th_off:4;
#endif
	uint8_t  th_flags;
	uint16_t th_win;
	uint16_t th_sum;
	uint16_t th_urp;
} JACL_PACK;

#define TCP_HDR_LEN	20
#define TCP_DATA_OFFSET(hdr) ((hdr)->doff << 2)

/* ======================================================================== */
/* Checksum Helpers (RFC 793 Pseudo-Header)                                 */
/* ======================================================================== */

static inline uint32_t tcp_pseudo_sum(struct in_addr src, struct in_addr dst,
                                      uint8_t proto, uint16_t len) {
	uint32_t sum = 0;
	sum += (src.s_addr >> 16) & 0xFFFF;
	sum += src.s_addr & 0xFFFF;
	sum += (dst.s_addr >> 16) & 0xFFFF;
	sum += dst.s_addr & 0xFFFF;
	sum += proto;
	sum += htons(len);
	return sum;
}

static inline uint16_t tcp_checksum_finish(uint32_t sum, const void *buf, int len) {
	const uint8_t *ptr = (const uint8_t *)buf;
	while (len > 1) {
		sum += (uint16_t)(ptr[0] << 8 | ptr[1]);
		ptr += 2;
		len -= 2;
	}
	if (len) sum += (uint16_t)(ptr[0] << 8); /* Odd byte: pad with zero */
	while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
	return (uint16_t)(~sum);
}

#ifdef __cplusplus
}
#endif
#endif /* _NET_TCP_H */
