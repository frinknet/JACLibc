/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _NET_TELNET_H
#define _NET_TELNET_H
#pragma once

/**
 * NOTE: This consolidates functionality traditionally found in:
 *
 *   - <arpa/telnet.h> (Telnet command codes and options)
 *
 * We unify these into a single header here at <net/telnet.h> for maintenance
 * and clarity with hopes that one day C Standards or POSIX may revamp layout
 * for sanity's sake so that we can remove the abstraction libraries that cause
 * so many security bugs because the codes don't see how things really work.
 *
 * There has to be a better way!!!
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Protocol Commands (RFC 854)                                              */
/* ======================================================================== */

#define	IAC	255	/* Interpret As Command */
#define	DONT	254
#define	DO	253
#define	WONT	252
#define	WILL	251
#define	SB	250	/* Subnegotiation Begin */
#define	GA	249	/* Go Ahead */
#define	EL	248	/* Erase Line */
#define	EC	247	/* Erase Character */
#define	AYT	246	/* Are You There */
#define	AO	245	/* Abort Output */
#define	IP	244	/* Interrupt Process */
#define	BREAK	243
#define	DM	242	/* Data Mark */
#define	NOP	241	/* No Operation */
#define	SE	240	/* Subnegotiation End */

/* Legacy TELNET_* aliases for backwards compatibility */
#define	TELNET_IAC		IAC
#define	TELNET_DONT		DONT
#define	TELNET_DO		DO
#define	TELNET_WONT		WONT
#define	TELNET_WILL		WILL
#define	TELNET_SB		SB
#define	TELNET_GA		GA
#define	TELNET_EL		EL
#define	TELNET_EC		EC
#define	TELNET_AYT		AYT
#define	TELNET_AO		AO
#define	TELNET_IP		IP
#define	TELNET_BREAK	BREAK
#define	TELNET_DM		DM
#define	TELNET_NOP		NOP
#define	TELNET_SE		SE

/* ======================================================================== */
/* Subnegotiation Qualifiers (RFC 854)                                      */
/* ======================================================================== */

#define	TELQUAL_IS		0
#define	TELQUAL_SEND		1
#define	TELQUAL_INFO		2
#define	TELQUAL_REPLY		2
#define	TELQUAL_NAME		3

/* ======================================================================== */
/* Standard Options (RFC 855 + IANA Assigned)                               */
/* ======================================================================== */

#define	TELOPT_BINARY		0	/* Transmit Binary */
#define	TELOPT_ECHO		1	/* Echo */
#define	TELOPT_RECONNECT	2	/* Reconnection */
#define	TELOPT_SGA		3	/* Suppress Go Ahead */
#define	TELOPT_STATUS		4	/* Status */
#define	TELOPT_TM		5	/* Timing Mark */
#define	TELOPT_RCTE		6	/* Remote Controlled Trans and Echo */
#define	TELOPT_NAOL		7	/* Negotiate About Output Line Width */
#define	TELOPT_NAOP		8	/* Negotiate About Output Page Size */
#define	TELOPT_NAOCRD		9	/* Negotiate About CR Disposition */
#define	TELOPT_NAOHTS		10	/* Negotiate About HT Stops */
#define	TELOPT_NAOHTD		11	/* Negotiate About HT Disposition */
#define	TELOPT_NAOFFD		12	/* Negotiate About FF Disposition */
#define	TELOPT_NAOVTS		13	/* Negotiate About VT Stops */
#define	TELOPT_NAOVTD		14	/* Negotiate About VT Disposition */
#define	TELOPT_NAOLFD		15	/* Negotiate About LF Disposition */
#define	TELOPT_XASCII		16	/* Extended ASCII */
#define	TELOPT_LOGOUT		17	/* Logout */
#define	TELOPT_BM		18	/* Byte Macro */
#define	TELOPT_DET		19	/* Data Entry Terminal */
#define	TELOPT_SUPDUP		20	/* SUPDUP */
#define	TELOPT_SUPDUPOUTPUT	21	/* SUPDUP Output */
#define	TELOPT_SENDLOC		22	/* Send Location */
#define	TELOPT_TTYPE		23	/* Terminal Type */
#define	TELOPT_EOR		24	/* End of Record */
#define	TELOPT_TUID		25	/* TACACS UID */
#define	TELOPT_OUTMRK		26	/* Output Marking */
#define	TELOPT_TTYLOC		27	/* Terminal Location */
#define	TELOPT_3270REGIME	28	/* Telnet 3270 Regime */
#define	TELOPT_X3LOC		29	/* X.3 PAD */
#define	TELOPT_XDISPLOC		30	/* X Display Location */
#define	TELOPT_NAWS		31	/* Negotiate About Window Size */
#define	TELOPT_TSPEED		32	/* Terminal Speed */
#define	TELOPT_LFLOW		33	/* Remote Flow Control */
#define	TELOPT_LINEMODE		34	/* Linemode */
#define	TELOPT_XDISPLOC_OLD	35	/* Legacy X Display */
#define	TELOPT_ENVIRON		36	/* Environment */
#define	TELOPT_AUTHENTICATION	37	/* Authentication */
#define	TELOPT_ENCRYPT		38	/* Encryption */
#define	TELOPT_NEW_ENVIRON	39	/* New Environment */
#define	TELOPT_STARTTLS		40	/* STARTTLS */
#define	TELOPT_KERMIT		41	/* KERMIT */
#define	TELOPT_SEND_URL		42	/* SEND-URL */
#define	TELOPT_FORWARD_X	43	/* FORWARD-X */

/* Legacy TELNET_OPT_* aliases */
#define	TELNET_OPT_BINARY	TELOPT_BINARY
#define	TELNET_OPT_ECHO		TELOPT_ECHO
#define	TELNET_OPT_SUPPRESS_GA	TELOPT_SGA
#define	TELNET_OPT_TERM_TYPE	TELOPT_TTYPE
#define	TELNET_OPT_WINDOW_SIZE	TELOPT_NAWS

/* ======================================================================== */
/* Inline Helpers (Header-Only Utilities)                                   */
/* ======================================================================== */

static inline int telnet_is_cmd(uint8_t b) {
	return b >= SE && b <= IAC;
}

static inline int telnet_is_opt(uint8_t opt) {
	return opt <= TELOPT_FORWARD_X || opt == 255;
}

#ifdef __cplusplus
}
#endif
#endif /* _NET_TELNET_H */
