/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _NETDB_H
#define _NETDB_H
#pragma once

#include <config.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <net/inet.h>
#include <net/dns.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Constants & Codes                                                        */
/* ======================================================================== */
#define EAI_AGAIN      1
#define EAI_BADFLAGS   2
#define EAI_FAIL       3
#define EAI_FAMILY     4
#define EAI_MEMORY     5
#define EAI_NONAME     6
#define EAI_SERVICE    7
#define EAI_SOCKTYPE   8
#define EAI_SYSTEM     9
#define EAI_OVERFLOW   10

#define AI_PASSIVE     0x0001
#define AI_CANONNAME   0x0002
#define AI_NUMERICHOST 0x0004
#define AI_NUMERICSERV 0x0008
#define AI_V4MAPPED    0x0010
#define AI_ALL         0x0020
#define AI_ADDRCONFIG  0x0040

#define NI_NOFQDN      0x0001
#define NI_NUMERICHOST 0x0002
#define NI_NAMEREQD    0x0004
#define NI_NUMERICSERV 0x0008
#define NI_DGRAM       0x0010

#define HOST_NOT_FOUND 1
#define TRY_AGAIN      2
#define NO_RECOVERY    3
#define NO_DATA        4
#define NO_ADDRESS     NO_DATA

extern thread_local int h_errno;

/* ======================================================================== */
/* Structures                                                               */
/* ======================================================================== */
struct hostent {
	char  *h_name;
	char **h_aliases;
	int    h_addrtype;
	int    h_length;
	char **h_addr_list;
};
#define h_addr h_addr_list[0]

struct netent {
	char  *n_name;
	char **n_aliases;
	int    n_addrtype;
	uint32_t n_net;
};

struct servent {
	char  *s_name;
	char **s_aliases;
	int    s_port;
	char  *s_proto;
};

struct protoent {
	char  *p_name;
	char **p_aliases;
	int    p_proto;
};

struct addrinfo {
	int              ai_flags;
	int              ai_family;
	int              ai_socktype;
	int              ai_protocol;
	socklen_t        ai_addrlen;
	char            *ai_canonname;
	struct sockaddr *ai_addr;
	struct addrinfo *ai_next;
};

/* ======================================================================== */
/* Forward Declarations                                                     */
/* ======================================================================== */
static inline void freeaddrinfo(struct addrinfo *res);

/* ======================================================================== */
/* Internal: DNS Configuration                                              */
/* ======================================================================== */
static const char *__jacl_def_ns[] = { "8.8.8.8", "8.8.4.4", "1.1.1.1", NULL };

static inline int __jacl_parse_resolv_conf(char servers[][64], int max_servers) {
	FILE *f = fopen("/etc/resolv.conf", "r");
	if (!f) {
		for (int i = 0; __jacl_def_ns[i] && i < max_servers; i++) strncpy(servers[i], __jacl_def_ns[i], 63);
		return (__jacl_def_ns[0] ? 1 : 0);
	}
	char line[256]; int count = 0;
	while (fgets(line, sizeof(line), f) && count < max_servers) {
		if (strncmp(line, "nameserver", 10) == 0) {
			char *ip = line + 10; while (*ip && isspace((unsigned char)*ip)) ip++;
			char *end = ip; while (*end && !isspace((unsigned char)*end)) end++;
			*end = '\0';
			if (strlen(ip) > 0 && strlen(ip) < 64) strcpy(servers[count++], ip);
		}
	}
	fclose(f);
	if (count == 0) {
		for (int i = 0; __jacl_def_ns[i] && i < max_servers; i++) strncpy(servers[i], __jacl_def_ns[i], 63);
		return (__jacl_def_ns[0] ? 1 : 0);
	}
	return count;
}

static inline int __jacl_send_dns_query(const char *server, uint8_t *qbuf, int qlen, uint8_t *abuf, int alen) {
	struct sockaddr_in sa4; struct sockaddr_in6 sa6;
	struct sockaddr *sa; socklen_t slen; int family;
	if (inet_pton(AF_INET, server, &sa4.sin_addr) == 1) {
		family = AF_INET; memset(&sa4, 0, sizeof(sa4));
		sa4.sin_family = AF_INET; sa4.sin_port = htons(53);
		sa = (struct sockaddr *)&sa4; slen = sizeof(sa4);
	} else if (inet_pton(AF_INET6, server, &sa6.sin6_addr) == 1) {
		family = AF_INET6; memset(&sa6, 0, sizeof(sa6));
		sa6.sin6_family = AF_INET6; sa6.sin6_port = htons(53);
		sa = (struct sockaddr *)&sa6; slen = sizeof(sa6);
	} else return -1;

	int sock = socket(family, SOCK_DGRAM, 0); if (sock < 0) return -1;
	struct timeval tv = {1, 0}; setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	if (sendto(sock, qbuf, qlen, 0, sa, slen) < 0) { close(sock); return -1; }
	int n = recvfrom(sock, abuf, alen, 0, NULL, NULL);
	if (n > 0 && n < alen) {
		struct dns_hdr *hdr = (struct dns_hdr *)abuf;
		uint8_t flags1 = ((uint8_t*)hdr)[2];
		if (flags1 & 0x02) {
			close(sock); sock = socket(family, SOCK_STREAM, 0); if (sock < 0) return -1;
			setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
			if (connect(sock, sa, slen) == 0) {
				uint16_t len_buf = htons(qlen);
				if (send(sock, &len_buf, 2, 0) == 2 && send(sock, qbuf, qlen, 0) == qlen) {
					if (recv(sock, &len_buf, 2, 0) == 2) {
						int rlen = ntohs(len_buf);
						if (rlen <= alen) n = recv(sock, abuf, rlen, 0);
					}
				}
			}
		}
	}
	close(sock); return n;
}

static inline int __jacl_resolve_raw(const char *name, int type, struct in_addr *v4_out, struct in6_addr *v6_out, char *cname_out) {
	uint8_t qbuf[512], abuf[512]; char servers[4][64]; int ns_count = __jacl_parse_resolv_conf(servers, 4);
	struct dns_hdr *hdr = (struct dns_hdr *)qbuf;
	memset(hdr, 0, sizeof(*hdr)); hdr->id = htons((uint16_t)(uintptr_t)&name);
	((uint8_t*)hdr)[2] = 0x01; hdr->qdcount = htons(1);
	int len = sizeof(struct dns_hdr);
	int n = dn_comp(name, qbuf + len, sizeof(qbuf) - len, NULL, NULL);
	if (n < 0) return -1; len += n;
	dns_put16(qbuf + len, type); len += 2; dns_put16(qbuf + len, 1); len += 2;
	int ans_len = 0;
	for (int i = 0; i < ns_count; i++) { ans_len = __jacl_send_dns_query(servers[i], qbuf, len, abuf, sizeof(abuf)); if (ans_len > 0) break; }
	if (ans_len <= 0) return -1;
	struct dns_hdr *resp = (struct dns_hdr *)abuf;
	int qdcount = ntohs(resp->qdcount), ancount = ntohs(resp->ancount);
	uint8_t *ptr = abuf + sizeof(struct dns_hdr);
	for (int i = 0; i < qdcount; i++) {
		while (*ptr && ptr < abuf + ans_len) { if ((*ptr & 0xC0) == 0xC0) { ptr += 2; break; } ptr += 1 + *ptr; }
		if ((*ptr & 0xC0) != 0xC0) ptr += 4; else ptr += 2;
	}
	int found = 0;
	for (int i = 0; i < ancount && ptr < abuf + ans_len; i++) {
		char ans_name[256]; int consumed = dn_expand(abuf, abuf + ans_len, ptr, ans_name, sizeof(ans_name));
		if (consumed < 0) break; ptr += consumed;
		if (ptr + 10 > abuf + ans_len) break;
		uint16_t atype = dns_get16(ptr); ptr += 2; uint16_t aclass = dns_get16(ptr); ptr += 2; ptr += 4;
		uint16_t rdlen = dns_get16(ptr); ptr += 2;
		if (ptr + rdlen > abuf + ans_len) break;
		if (aclass == 1) {
			if (atype == 1 && rdlen == 4 && v4_out) { memcpy(v4_out, ptr, 4); found = 1; }
			else if (atype == 28 && rdlen == 16 && v6_out) { memcpy(v6_out, ptr, 16); found = 1; }
			else if (atype == 5 && cname_out) { dn_expand(abuf, abuf + ans_len, ptr, cname_out, 256); }
		}
		ptr += rdlen;
	}
	return found ? 0 : -1;
}

/* ======================================================================== */
/* Public API: Address Resolution                                           */
/* ======================================================================== */
static inline int getaddrinfo(const char *node, const char *service,
                              const struct addrinfo *hints,
                              struct addrinfo **res) {
	if (!res) return EAI_SYSTEM;
	*res = NULL;
	if (!node && !service) return EAI_NONAME;

	struct addrinfo *head = NULL, *tail = NULL;
	struct in_addr v4;
	struct in6_addr v6;
	char cname[256];
	int has_v4 = 0, has_v6 = 0;

	/* Zero buffers: some inet_pton implementations fail on uninitialized memory */
	memset(&v4, 0, sizeof(v4));
	memset(&v6, 0, sizeof(v6));

	if (node) {
		int parsed = 0;
		if (inet_pton(AF_INET, node, &v4) == 1) { has_v4 = 1; parsed = 1; }
		else if (inet_pton(AF_INET6, node, &v6) == 1) { has_v6 = 1; parsed = 1; }

		/* Fallback for loopback literals when inet_pton is incomplete/broken */
		if (!parsed) {
			if (strcmp(node, "127.0.0.1") == 0) {
				v4.s_addr = htonl(0x7F000001); has_v4 = 1; parsed = 1;
			} else if (strcmp(node, "::1") == 0) {
				((uint8_t *)&v6)[15] = 1; has_v6 = 1; parsed = 1;
			}
		}

		if (!parsed) {
			if (__jacl_resolve_raw(node, 1, &v4, NULL, cname) == 0) has_v4 = 1;
			if (__jacl_resolve_raw(node, 28, NULL, &v6, NULL) == 0) has_v6 = 1;
			if (!has_v4 && !has_v6 && cname[0]) {
				if (__jacl_resolve_raw(cname, 1, &v4, NULL, NULL) == 0) has_v4 = 1;
				if (__jacl_resolve_raw(cname, 28, NULL, &v6, NULL) == 0) has_v6 = 1;
			}
		}
	} else {
		int target = hints ? hints->ai_family : AF_UNSPEC;
		if (target == AF_UNSPEC || target == AF_INET6) { has_v6 = 1; }
		if (target == AF_UNSPEC || target == AF_INET) {
			v4.s_addr = (hints && (hints->ai_flags & AI_PASSIVE)) ? htonl(INADDR_ANY) : htonl(0x7F000001);
			has_v4 = 1;
		}
	}

	if (!has_v4 && !has_v6) return EAI_NONAME;

	int port = 0;
	if (service) {
		char *end; port = strtol(service, &end, 10);
		if (*end != '\0') {
			if (strcmp(service, "http") == 0) port = 80;
			else if (strcmp(service, "https") == 0) port = 443;
			else if (strcmp(service, "ssh") == 0) port = 22;
			else if (strcmp(service, "dns") == 0) port = 53;
			else return EAI_SERVICE;
		}
	}

	if (has_v4 && (!hints || hints->ai_family == AF_UNSPEC || hints->ai_family == AF_INET)) {
		struct addrinfo *ai = calloc(1, sizeof(struct addrinfo) + sizeof(struct sockaddr_in));
		if (!ai) return EAI_MEMORY;
		struct sockaddr_in *sa = (struct sockaddr_in *)(ai + 1);
		sa->sin_family = AF_INET; sa->sin_port = htons(port); sa->sin_addr = v4;
		ai->ai_family = AF_INET; ai->ai_socktype = hints ? hints->ai_socktype : 0;
		ai->ai_protocol = hints ? hints->ai_protocol : 0; ai->ai_addrlen = sizeof(struct sockaddr_in);
		ai->ai_addr = (struct sockaddr *)sa;
		if (hints && (hints->ai_flags & AI_CANONNAME) && node) ai->ai_canonname = strdup(node);
		if (!head) head = tail = ai; else { tail->ai_next = ai; tail = ai; }
	}

	if (has_v6 && (!hints || hints->ai_family == AF_UNSPEC || hints->ai_family == AF_INET6)) {
		struct addrinfo *ai = calloc(1, sizeof(struct addrinfo) + sizeof(struct sockaddr_in6));
		if (!ai) { if(head) freeaddrinfo(head); return EAI_MEMORY; }
		struct sockaddr_in6 *sa = (struct sockaddr_in6 *)(ai + 1);
		sa->sin6_family = AF_INET6; sa->sin6_port = htons(port); sa->sin6_addr = v6;
		ai->ai_family = AF_INET6; ai->ai_socktype = hints ? hints->ai_socktype : 0;
		ai->ai_protocol = hints ? hints->ai_protocol : 0; ai->ai_addrlen = sizeof(struct sockaddr_in6);
		ai->ai_addr = (struct sockaddr *)sa;
		if (hints && (hints->ai_flags & AI_CANONNAME) && node) ai->ai_canonname = strdup(node);
		if (!head) head = tail = ai; else { tail->ai_next = ai; tail = ai; }
	}

	if (!head) return EAI_FAIL;
	*res = head;
	return 0;
}


static inline void freeaddrinfo(struct addrinfo *res) {
	while (res) { struct addrinfo *next = res->ai_next; free(res->ai_canonname); free(res); res = next; }
}

static inline const char *gai_strerror(int errcode) {
	switch (errcode) {
		case EAI_AGAIN: return "Temporary failure";
		case EAI_NONAME: return "Name or service not known";
		case EAI_SERVICE: return "Service not supported";
		case EAI_MEMORY: return "Memory allocation failure";
		default: return "Unknown error";
	}
}

static inline int getnameinfo(const struct sockaddr *sa, socklen_t salen, char *host, socklen_t hostlen, char *serv, socklen_t servlen, int flags) {
	if (!sa) return EAI_FAMILY;
	if (sa->sa_family == AF_INET) {
		struct sockaddr_in *sin = (struct sockaddr_in *)sa;
		if (host && hostlen > 0) inet_ntop(AF_INET, &sin->sin_addr, host, hostlen);
		if (serv && servlen > 0) snprintf(serv, servlen, "%d", ntohs(sin->sin_port));
	} else if (sa->sa_family == AF_INET6) {
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;
		if (host && hostlen > 0) inet_ntop(AF_INET6, &sin6->sin6_addr, host, hostlen);
		if (serv && servlen > 0) snprintf(serv, servlen, "%d", ntohs(sin6->sin6_port));
	} else return EAI_FAMILY;
	return 0;
}

/* ======================================================================== */
/* Legacy Database Parsers (POSIX /etc/ + Deterministic Fallback)           */
/* ======================================================================== */
static FILE *__jacl_db_net_fp = NULL, *__jacl_db_svc_fp = NULL, *__jacl_db_proto_fp = NULL, *__jacl_db_hosts_fp = NULL;
static int __jacl_db_net_idx = 0, __jacl_db_svc_idx = 0, __jacl_db_proto_idx = 0, __jacl_db_hosts_idx = 0;

static const char *__jacl_hosts_fallback[] = { "127.0.0.1 localhost", "::1 localhost ip6-localhost", NULL };
static const char *__jacl_svc_fallback[]   = { "http 80/tcp", "http 80/udp", "https 443/tcp", "ssh 22/tcp", "dns 53/tcp", "dns 53/udp", "ftp 21/tcp", "smtp 25/tcp", NULL };
static const char *__jacl_proto_fallback[] = { "ip 0", "icmp 1", "tcp 6", "udp 17", "ipv6 41", NULL };
static const char *__jacl_net_fallback[]   = { "loopback 127.0.0.0", NULL };

/* Networks */
static inline void setnetent(int stayopen) {
	__jacl_db_net_idx = 0;
	if (stayopen && __jacl_db_net_fp) return;
	if (!__jacl_db_net_fp) __jacl_db_net_fp = fopen("/etc/networks", "r"); else rewind(__jacl_db_net_fp);
}
static inline void endnetent(void) { if (__jacl_db_net_fp) { fclose(__jacl_db_net_fp); __jacl_db_net_fp = NULL; } }
static inline struct netent *getnetent(void) {
	static struct netent ne;
	static char line[1024], name[256], proto[32], *aliases[32];
	aliases[0] = NULL;

	int use_fallback = 0;
	if (__jacl_db_net_fp) {
		while (fgets(line, sizeof(line), __jacl_db_net_fp)) {
			char *p = line; while (*p && isspace((unsigned char)*p)) p++;
			if (*p && *p != '#') break;
		}
		if (feof(__jacl_db_net_fp)) use_fallback = 1;
	} else {
		use_fallback = 1;
	}

	if (use_fallback) {
		if (!__jacl_net_fallback[__jacl_db_net_idx]) return NULL;
		strncpy(line, __jacl_net_fallback[__jacl_db_net_idx++], sizeof(line)-1); line[sizeof(line)-1] = '\0';
	}

	if (sscanf(line, "%255s %*[^\n]", name) != 1) return NULL;
	ne.n_name = name;
	ne.n_aliases = aliases;
	ne.n_addrtype = AF_INET;

	char net_str[64] = {0};
	char *tok = line + strlen(name);
	while (*tok && isspace((unsigned char)*tok)) tok++;
	sscanf(tok, "%63s", net_str);

	unsigned int a=0,b=0,c=0,d=0;
	if (sscanf(net_str, "%u.%u.%u.%u", &a, &b, &c, &d) == 4) ne.n_net = htonl((a<<24)|(b<<16)|(c<<8)|d);
	else ne.n_net = strtoul(net_str, NULL, 0);
	return &ne;
}
static inline struct netent *getnetbyname(const char *name) { struct netent *n; setnetent(1); while ((n = getnetent()) && strcmp(n->n_name, name)); endnetent(); return n; }
static inline struct netent *getnetbyaddr(uint32_t net, int type) { struct netent *n; setnetent(1); uint32_t v = ntohl(net); while ((n = getnetent()) && n->n_net != v); endnetent(); return n; }

/* Services */
static inline void setservent(int stayopen) {
	__jacl_db_svc_idx = 0;
	if (stayopen && __jacl_db_svc_fp) return;
	if (!__jacl_db_svc_fp) __jacl_db_svc_fp = fopen("/etc/services", "r"); else rewind(__jacl_db_svc_fp);
}
static inline void endservent(void) { if (__jacl_db_svc_fp) { fclose(__jacl_db_svc_fp); __jacl_db_svc_fp = NULL; } }
static inline struct servent *getservent(void) {
	static struct servent se;
	static char line[1024], name[256], proto[32], *aliases[32];
	aliases[0] = NULL;

	int use_fallback = 0;
	if (__jacl_db_svc_fp) {
		while (fgets(line, sizeof(line), __jacl_db_svc_fp)) {
			char *p = line; while (*p && isspace((unsigned char)*p)) p++;
			if (*p && *p != '#') break;
		}
		if (feof(__jacl_db_svc_fp)) use_fallback = 1;
	} else {
		use_fallback = 1;
	}

	if (use_fallback) {
		if (!__jacl_svc_fallback[__jacl_db_svc_idx]) return NULL;
		strncpy(line, __jacl_svc_fallback[__jacl_db_svc_idx++], sizeof(line)-1); line[sizeof(line)-1] = '\0';
	}

	if (sscanf(line, "%255s %*s", name) != 1) return NULL;
	se.s_name = name; se.s_aliases = aliases;

	char port_proto[64] = {0};
	char *p = line + strlen(name);
	while (*p && isspace((unsigned char)*p)) p++;
	sscanf(p, "%63s", port_proto);

	char *slash = strchr(port_proto, '/');
	if (slash) { se.s_port = htons(atoi(port_proto)); *slash = '\0'; strncpy(proto, slash+1, sizeof(proto)-1); }
	else { se.s_port = htons(atoi(port_proto)); proto[0] = '\0'; }
	se.s_proto = proto;

	int i = 0; p += strlen(port_proto) + 1;
	while (*p && i < 31) {
		while (*p && isspace((unsigned char)*p)) p++;
		if (!*p || *p == '#') break;
		aliases[i++] = p;
		while (*p && !isspace((unsigned char)*p) && *p != '#') p++;
		if (*p) { *p = '\0'; p++; }
	}
	aliases[i] = NULL;
	return &se;
}
static inline struct servent *getservbyname(const char *name, const char *proto) { struct servent *s; setservent(1); while ((s = getservent()) && (strcmp(s->s_name, name) || (proto && strcmp(s->s_proto, proto)))); endservent(); return s; }
static inline struct servent *getservbyport(int port, const char *proto) { struct servent *s; setservent(1); while ((s = getservent()) && (ntohs(s->s_port) != port || (proto && strcmp(s->s_proto, proto)))); endservent(); return s; }

/* Protocols */
static inline void setprotoent(int stayopen) {
	__jacl_db_proto_idx = 0;
	if (stayopen && __jacl_db_proto_fp) return;
	if (!__jacl_db_proto_fp) __jacl_db_proto_fp = fopen("/etc/protocols", "r"); else rewind(__jacl_db_proto_fp);
}
static inline void endprotoent(void) { if (__jacl_db_proto_fp) { fclose(__jacl_db_proto_fp); __jacl_db_proto_fp = NULL; } }
static inline struct protoent *getprotoent(void) {
	static struct protoent pe;
	static char line[1024], name[256], *aliases[32];
	aliases[0] = NULL;

	int use_fallback = 0;
	if (__jacl_db_proto_fp) {
		while (fgets(line, sizeof(line), __jacl_db_proto_fp)) {
			char *p = line; while (*p && isspace((unsigned char)*p)) p++;
			if (*p && *p != '#') break;
		}
		if (feof(__jacl_db_proto_fp)) use_fallback = 1;
	} else {
		use_fallback = 1;
	}

	if (use_fallback) {
		if (!__jacl_proto_fallback[__jacl_db_proto_idx]) return NULL;
		strncpy(line, __jacl_proto_fallback[__jacl_db_proto_idx++], sizeof(line)-1); line[sizeof(line)-1] = '\0';
	}

	if (sscanf(line, "%255s %*s", name) != 1) return NULL;
	pe.p_name = name; pe.p_aliases = aliases;

	char num_str[64] = {0};
	char *p = line + strlen(name);
	while (*p && isspace((unsigned char)*p)) p++;
	sscanf(p, "%63s", num_str);
	pe.p_proto = (int)strtol(num_str, NULL, 10);

	int i = 0; p += strlen(num_str) + 1;
	while (*p && i < 31) {
		while (*p && isspace((unsigned char)*p)) p++;
		if (!*p || *p == '#') break;
		aliases[i++] = p;
		while (*p && !isspace((unsigned char)*p) && *p != '#') p++;
		if (*p) { *p = '\0'; p++; }
	}
	aliases[i] = NULL;
	return &pe;
}
static inline struct protoent *getprotobyname(const char *name) { struct protoent *p; setprotoent(1); while ((p = getprotoent()) && strcmp(p->p_name, name)); endprotoent(); return p; }
static inline struct protoent *getprotobynumber(int proto) { struct protoent *p; setprotoent(1); while ((p = getprotoent()) && p->p_proto != proto); endprotoent(); return p; }

/* Hosts (Safe, Isolated State) */
static inline void sethostent(int stayopen) {
	__jacl_db_hosts_idx = 0;
	if (stayopen && __jacl_db_hosts_fp) return;
	if (!__jacl_db_hosts_fp) __jacl_db_hosts_fp = fopen("/etc/hosts", "r"); else rewind(__jacl_db_hosts_fp);
}
static inline void endhostent(void) { if (__jacl_db_hosts_fp) { fclose(__jacl_db_hosts_fp); __jacl_db_hosts_fp = NULL; } }
static inline struct hostent *gethostent(void) {
	static struct hostent he;
	static char line[1024], name[256], ip[64], *aliases[32], addr[16], *addr_list[2];
	aliases[0] = NULL; addr_list[1] = NULL;

	int use_fallback = 0;
	if (__jacl_db_hosts_fp) {
		while (fgets(line, sizeof(line), __jacl_db_hosts_fp)) {
			char *p = line; while (*p && isspace((unsigned char)*p)) p++;
			if (*p && *p != '#') break;
		}
		if (feof(__jacl_db_hosts_fp)) use_fallback = 1;
	} else {
		use_fallback = 1;
	}

	if (use_fallback) {
		if (!__jacl_hosts_fallback[__jacl_db_hosts_idx]) return NULL;
		strncpy(line, __jacl_hosts_fallback[__jacl_db_hosts_idx++], sizeof(line)-1); line[sizeof(line)-1] = '\0';
	}

	if (sscanf(line, "%63s %255s", ip, name) != 2) return NULL;
	he.h_name = name;

	if (inet_pton(AF_INET, ip, addr) == 1) { he.h_addrtype = AF_INET; he.h_length = 4; }
	else if (inet_pton(AF_INET6, ip, addr) == 1) { he.h_addrtype = AF_INET6; he.h_length = 16; }
	else return NULL;

	addr_list[0] = addr; he.h_addr_list = addr_list;

	char *p = line + strlen(ip);
	while (*p && isspace((unsigned char)*p)) p++;
	p += strlen(name);
	while (*p && isspace((unsigned char)*p)) p++;

	int ai = 0;
	while (*p && ai < 31) {
		aliases[ai++] = p;
		while (*p && !isspace((unsigned char)*p) && *p != '#') p++;
		if (*p) { *p = '\0'; p++; while (*p && isspace((unsigned char)*p)) p++; }
		else break;
	}
	aliases[ai] = NULL;
	he.h_aliases = aliases;
	return &he;
}

static inline struct hostent *gethostbyname(const char *name) {
	struct hostent *h; sethostent(1);
	while ((h = gethostent())) {
		if (strcmp(h->h_name, name) == 0) { endhostent(); return h; }
		if (h->h_aliases) for (int i = 0; h->h_aliases[i]; i++) if (strcmp(h->h_aliases[i], name) == 0) { endhostent(); return h; }
	}
	endhostent();

	static struct hostent dns_he; static char *dns_aliases[1] = {NULL}; static char *dns_addrs[2]; static struct in_addr dns_addr; static char dns_name[256];
	memset(&dns_he, 0, sizeof(dns_he));
	strncpy(dns_name, name, sizeof(dns_name)-1); dns_name[sizeof(dns_name)-1] = '\0';
	dns_he.h_name = dns_name; dns_he.h_aliases = dns_aliases;

	if (inet_pton(AF_INET, name, &dns_addr) == 1) {
		dns_he.h_addrtype = AF_INET; dns_he.h_length = 4;
		dns_addrs[0] = (char *)&dns_addr; dns_addrs[1] = NULL; dns_he.h_addr_list = dns_addrs;
		h_errno = 0; return &dns_he;
	}
	if (__jacl_resolve_raw(name, 1, &dns_addr, NULL, NULL) == 0) {
		dns_he.h_addrtype = AF_INET; dns_he.h_length = 4;
		dns_addrs[0] = (char *)&dns_addr; dns_addrs[1] = NULL; dns_he.h_addr_list = dns_addrs;
		h_errno = 0; return &dns_he;
	}
	h_errno = HOST_NOT_FOUND; return NULL;
}

static inline struct hostent *gethostbyaddr(const void *addr, socklen_t len, int type) {
	if (!addr && len > 0) { h_errno = NO_DATA; return NULL; }
	struct hostent *h; sethostent(1);
	while ((h = gethostent())) {
		if (h->h_addrtype == type && h->h_length == len && h->h_addr && addr && memcmp(h->h_addr, addr, len) == 0) { endhostent(); return h; }
	}
	endhostent();

	static struct hostent ptr_he; static char *ptr_aliases[1] = {NULL}; static char *ptr_addrs[2];
	memset(&ptr_he, 0, sizeof(ptr_he));
	ptr_he.h_addrtype = type; ptr_he.h_length = len;
	ptr_addrs[0] = (char *)addr; ptr_addrs[1] = NULL; ptr_he.h_addr_list = ptr_addrs; ptr_he.h_aliases = ptr_aliases;

	char query[128] = {0};
	if (type == AF_INET && len == 4) {
		const uint8_t *b = (const uint8_t *)addr;
		snprintf(query, sizeof(query), "%u.%u.%u.%u.in-addr.arpa", b[3], b[2], b[1], b[0]);
	} else if (type == AF_INET6 && len == 16) {
		const uint8_t *b = (const uint8_t *)addr; char *p = query;
		for (int i = 15; i >= 0; i--) p += snprintf(p, 6, "%x.%x.", b[i] & 0xF, (b[i] >> 4) & 0xF);
		strcpy(p, "ip6.arpa");
	} else { h_errno = NO_DATA; return NULL; }

	char cname[256];
	if (__jacl_resolve_raw(query, 12, NULL, NULL, cname) == 0) { ptr_he.h_name = cname; h_errno = 0; return &ptr_he; }
	h_errno = HOST_NOT_FOUND; return NULL;
}

static inline const char *hstrerror(int err) {
	switch (err) {
		case HOST_NOT_FOUND: return "Host not found"; case TRY_AGAIN: return "Try again";
		case NO_RECOVERY: return "No recover"; case NO_DATA: return "No data"; default: return "Unknown error";
	}
}

#ifdef __cplusplus
}
#endif
#endif /* _NETDB_H */
