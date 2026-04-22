/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _NET_IF_H
#define _NET_IF_H
#pragma once

/**
 * NOTE: This consolidates functionality traditionally found in:
 *
 *   - <net/if.h> (Interface definitions, ioctls, POSIX functions)
 *   - <linux/if.h> (Linux-specific flags and ioctls)
 *   - <bsd/net/if.h> (BSD interface data structures)
 *
 * We unify these into a single header here at <net/if.h> for maintenance
 * and clarity with hopes that one day C Standards or POSIX may revamp layout
 * for sanity's sake so that we can remove the abstraction libraries that cause
 * so many security bugs because the codes don't see how things really work.
 *
 * There has to be a better way!!!
 */

#include <config.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Core Constants                                                           */
/* ======================================================================== */

#define IFNAMSIZ	16
#define IFALIASZ	256

/* ======================================================================== */
/* Interface Flags (POSIX + Linux Standard Subset)                          */
/* ======================================================================== */

#define IFF_UP			0x1
#define IFF_BROADCAST		0x2
#define IFF_DEBUG		0x4
#define IFF_LOOPBACK		0x8
#define IFF_POINTOPOINT		0x10
#define IFF_NOTRAILERS		0x20
#define IFF_RUNNING		0x40
#define IFF_NOARP		0x80
#define IFF_PROMISC		0x100
#define IFF_ALLMULTI		0x200
#define IFF_MASTER		0x400
#define IFF_SLAVE		0x800
#define IFF_MULTICAST		0x1000
#define IFF_DYNAMIC		0x2000
#define IFF_LOWER_UP		0x10000
#define IFF_DORMANT		0x20000
#define IFF_ECHO		0x40000

/* ======================================================================== */
/* Hardware/Resource Map                                                    */
/* ======================================================================== */

struct ifmap {
	unsigned long	mem_start;
	unsigned long	mem_end;
	unsigned short	base_addr;
	unsigned char	irq;
	unsigned char	dma;
	unsigned char	port;
};

/* ======================================================================== */
/* Interface Request Structure (ifreq)                                      */
/* ======================================================================== */

struct ifreq {
	char	ifr_name[IFNAMSIZ];
	union {
		struct sockaddr ifru_addr;
		struct sockaddr ifru_dstaddr;
		struct sockaddr ifru_broadaddr;
		struct sockaddr ifru_netmask;
		struct sockaddr ifru_hwaddr;
		short		int ifru_flags;
		int		ifru_ifindex;
		int		ifru_metric;
		int		ifru_mtu;
		struct ifmap	ifru_map;
		char		ifru_slave[IFNAMSIZ];
		char		ifru_newname[IFNAMSIZ];
		void		*ifru_data;
	} ifr_ifru;
};

#define ifr_addr		ifr_ifru.ifru_addr
#define ifr_dstaddr		ifr_ifru.ifru_dstaddr
#define ifr_broadaddr		ifr_ifru.ifru_broadaddr
#define ifr_netmask		ifr_ifru.ifru_netmask
#define ifr_hwaddr		ifr_ifru.ifru_hwaddr
#define ifr_flags		ifr_ifru.ifru_flags
#define ifr_ifindex		ifr_ifru.ifru_ifindex
#define ifr_metric		ifr_ifru.ifru_metric
#define ifr_mtu			ifr_ifru.ifru_mtu
#define ifr_map			ifr_ifru.ifru_map
#define ifr_slave		ifr_ifru.ifru_slave
#define ifr_newname		ifr_ifru.ifru_newname
#define ifr_data		ifr_ifru.ifru_data

/* ======================================================================== */
/* Interface Configuration Buffer (ifconf)                                  */
/* ======================================================================== */

struct ifconf {
	int	ifc_len;
	union {
		char	*ifcu_buf;
		struct ifreq *ifcu_req;
	} ifc_ifcu;
};

#define ifc_buf		ifc_ifcu.ifcu_buf
#define ifc_req		ifc_ifcu.ifcu_req

/* ======================================================================== */
/* POSIX Interface Index Structure                                          */
/* ======================================================================== */

struct if_nameindex {
	unsigned int	if_index;
	char		*if_name;
};

/* ======================================================================== */
/* Ioctl Command Constants (Linux/POSIX Standard)                           */
/* ======================================================================== */

#define SIOCGIFNAME	0x8910
#define SIOCSIFLINK	0x8911
#define SIOCGIFCONF	0x8912
#define SIOCSIFFLAGS	0x8914
#define SIOCGIFFLAGS	0x8915
#define SIOCSIFADDR	0x8916
#define SIOCGIFADDR	0x8917
#define SIOCSIFNETMASK	0x8918
#define SIOCGIFNETMASK	0x8919
#define SIOCGIFMETRIC	0x891D
#define SIOCSIFMETRIC	0x891E
#define SIOCGIFMTU	0x8921
#define SIOCSIFMTU	0x8922
#define SIOCGIFHWADDR	0x8927
#define SIOCSIFHWADDR	0x8928
#define SIOCGIFINDEX	0x8933
#define SIOCSIFNAME	0x8923

/* ======================================================================== */
/* POSIX Interface Functions (Header-Only Stubs)                            */
/* ======================================================================== */

#if JACL_HAS_POSIX

static inline unsigned int if_nametoindex(const char *name) {
	if (!name) return 0;

	int fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (fd < 0) return 0;

	struct ifreq ifr = {0};

	strncpy(ifr.ifr_name, name, IFNAMSIZ - 1);

	int r = ioctl(fd, SIOCGIFINDEX, &ifr);

	close(fd);

	return r < 0 ? 0 : (unsigned int)ifr.ifr_ifindex;
}

static inline char *if_indextoname(unsigned int ifindex, char *ifname) {
	if (!ifname) return NULL;

	int fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (fd < 0) return NULL;

	char buf[4096];
	struct ifconf ifc = { .ifc_len = sizeof(buf), .ifc_buf = buf };

	if (ioctl(fd, SIOCGIFCONF, &ifc) < 0) { close(fd); return NULL; }

	close(fd);

	struct ifreq *ifr = ifc.ifc_req;
	int count = ifc.ifc_len / sizeof(struct ifreq);

	for (int i = 0; i < count; i++) {
		struct ifreq tmp = {0};

		strncpy(tmp.ifr_name, ifr[i].ifr_name, IFNAMSIZ - 1);

		int fd2 = socket(AF_INET, SOCK_DGRAM, 0);

		if (fd2 < 0) continue;

		if (ioctl(fd2, SIOCGIFINDEX, &tmp) == 0 && (unsigned int)tmp.ifr_ifindex == ifindex) {
			strncpy(ifname, tmp.ifr_name, IFNAMSIZ - 1);

			ifname[IFNAMSIZ - 1] = '\0';

			close(fd2);

			return ifname;
		}

		close(fd2);
	}

	errno = ENOENT;

	return NULL;
}

static inline void if_freenameindex(struct if_nameindex *ptr) {
	if (!ptr) return;

	for (int i = 0; ptr[i].if_index != 0 || ptr[i].if_name != NULL; i++) free(ptr[i].if_name);

	free(ptr);
}

static inline struct if_nameindex *if_nameindex(void) {
	int fd = socket(AF_INET, SOCK_DGRAM, 0);

	if (fd < 0) return NULL;

	char buf[8192];
	struct ifconf ifc = { .ifc_len = sizeof(buf), .ifc_buf = buf };

	if (ioctl(fd, SIOCGIFCONF, &ifc) < 0) { close(fd); return NULL; }

	close(fd);

	int count = ifc.ifc_len / sizeof(struct ifreq);
	struct if_nameindex *idx = calloc(count + 1, sizeof(struct if_nameindex));

	if (!idx) return NULL;

	for (int i = 0; i < count; i++) {
		idx[i].if_index = if_nametoindex(ifc.ifc_req[i].ifr_name);
		idx[i].if_name = strdup(ifc.ifc_req[i].ifr_name);

		if (!idx[i].if_name) { if_freenameindex(idx); return NULL; }
	}

	return idx;
}

#else

static inline unsigned int if_nametoindex(const char *name) { (void)name; return 0; }
static inline char *if_indextoname(unsigned int ifindex, char *ifname) { (void)ifindex; (void)ifname; return NULL; }
static inline struct if_nameindex *if_nameindex(void) { return NULL; }
static inline void if_freenameindex(struct if_nameindex *ptr) { (void)ptr; }

#endif

#ifdef __cplusplus
}
#endif
#endif /* _NET_IF_H */
