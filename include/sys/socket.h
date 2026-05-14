/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H
#pragma once

#include <config.h>
#include <errno.h>
#include <stddef.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/syscall.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Socket Address Families                                                  */
/* ======================================================================== */

#define AF_UNSPEC       0
#define AF_UNIX         1
#define AF_LOCAL        1
#define AF_INET         2
#define AF_AX25         3
#define AF_IPX          4
#define AF_APPLETALK    5
#define AF_NETROM       6
#define AF_BRIDGE       7
#define AF_ATMPVC       8
#define AF_X25          9
#define AF_INET6        10
#define AF_ROSE         11
#define AF_DECnet       12
#define AF_NETBEUI      13
#define AF_SECURITY     14
#define AF_KEY          15
#define AF_NETLINK      16
#define AF_ROUTE        16
#define AF_PACKET       17
#define AF_ASH          18
#define AF_ECONET       19
#define AF_ATMSVC       20
#define AF_RDS          21
#define AF_SNA          22
#define AF_IRDA         23
#define AF_PPPOX        24
#define AF_WANPIPE      25
#define AF_LLC          26
#define AF_IB           27
#define AF_MPLS         28
#define AF_CAN          29
#define AF_TIPC         30
#define AF_BLUETOOTH    31
#define AF_IUCV         32
#define AF_RXRPC        33
#define AF_ISDN         34
#define AF_PHONET       35
#define AF_IEEE802154   36
#define AF_CAIF         37
#define AF_ALG          38
#define AF_NFC          39
#define AF_VSOCK        40
#define AF_KCM          41
#define AF_QIPCRTR      42
#define AF_SMC          43
#define AF_XDP          44

/* ======================================================================== */
/* Socket Types                                                             */
/* ======================================================================== */

#define SOCK_STREAM     1
#define SOCK_DGRAM      2
#define SOCK_RAW        3
#define SOCK_RDM        4
#define SOCK_SEQPACKET  5
#define SOCK_DCCP       6
#define SOCK_PACKET     10

/* Socket Creation Flags (Linux) */
#define SOCK_NONBLOCK   04000
#define SOCK_CLOEXEC    02000000

/* ======================================================================== */
/* Protocol Levels                                                          */
/* ======================================================================== */

#define SOL_SOCKET      1
#define SOL_IP          0
#define SOL_IPV6        41
#define SOL_TCP         6
#define SOL_UDP         17

/* ======================================================================== */
/* Socket Options                                                           */
/* ======================================================================== */

#define SO_DEBUG                    1
#define SO_REUSEADDR                2
#define SO_TYPE                     3
#define SO_ERROR                    4
#define SO_DONTROUTE                5
#define SO_BROADCAST                6
#define SO_SNDBUF                   7
#define SO_RCVBUF                   8
#define SO_KEEPALIVE                9
#define SO_OOBINLINE               10
#define SO_NO_CHECK                11
#define SO_PRIORITY                12
#define SO_LINGER                  13
#define SO_BSDCOMPAT               14
#define SO_REUSEPORT               15
#define SO_PASSCRED                16
#define SO_PEERCRED                17
#define SO_RCVLOWAT                18
#define SO_SNDLOWAT                19

#if JACL_HAS_BSD
#define SO_RCVTIMEO                0x1006
#define SO_SNDTIMEO                0x1005
#else
#define SO_RCVTIMEO                20
#define SO_SNDTIMEO                21
#endif

#define SO_SECURITY_AUTHENTICATION       22
#define SO_SECURITY_ENCRYPTION_TRANSPORT 23
#define SO_SECURITY_ENCRYPTION_NETWORK   24
#define SO_BINDTODEVICE                  25
#define SO_ATTACH_FILTER                 26
#define SO_DETACH_FILTER                 27
#define SO_GET_FILTER                    SO_ATTACH_FILTER
#define SO_PEERNAME                      28
#define SO_TIMESTAMP                     29
#define SO_ACCEPTCONN                    30

/* ======================================================================== */
/* Message Flags                                                            */
/* ======================================================================== */

#define MSG_OOB          0x01
#define MSG_PEEK         0x02
#define MSG_DONTROUTE    0x04
#define MSG_CTRUNC       0x08
#define MSG_PROBE        0x10
#define MSG_TRUNC        0x20
#define MSG_DONTWAIT     0x40
#define MSG_EOR          0x80
#define MSG_WAITALL      0x100
#define MSG_FIN          0x200
#define MSG_SYN          0x400
#define MSG_CONFIRM      0x800
#define MSG_RST          0x1000
#define MSG_NOSIGNAL     0x4000
#define MSG_MORE         0x8000
#define MSG_WAITFORONE   0x10000

/* ======================================================================== */
/* Shutdown Commands                                                        */
/* ======================================================================== */

#define SHUT_RD    0
#define SHUT_WR    1
#define SHUT_RDWR  2

/* ======================================================================== */
/* Structures                                                               */
/* ======================================================================== */

/* Generic Socket Address */
struct sockaddr {
	unsigned short sa_family;
	char           sa_data[14];
};

/* Message Header */
struct msghdr {
	void         *msg_name;
	socklen_t     msg_namelen;
	struct iovec *msg_iov;
	size_t        msg_iovlen;
	void         *msg_control;
	socklen_t     msg_controllen;
	int           msg_flags;
};

/* Ancillary Data Header */
struct cmsghdr {
	socklen_t cmsg_len;
	int       cmsg_level;
	int       cmsg_type;
};

/* SO_LINGER */
struct linger {
	int l_onoff;
	int l_linger;
};

/* Socket Address Storage (POSIX: 128 bytes minimum) */
struct sockaddr_storage {
	unsigned short ss_family;

#if defined(_LP64) || defined(__x86_64__) || defined(__aarch64__)
	unsigned short __ss_pad1[3];
	unsigned long  __ss_pad2[15];
#else
	unsigned char  __ss_pad1[26];
	unsigned long  __ss_pad2[12];
#endif
};

#define _SS_MAXSIZE   128
#define _SS_ALIGNSIZE sizeof(long)

/* ======================================================================== */
/* Ancillary Data Macros                                                    */
/* ======================================================================== */

#define CMSG_ALIGN(len) (((size_t)(len) + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1))
#define CMSG_SPACE(len) (CMSG_ALIGN(sizeof(struct cmsghdr)) + CMSG_ALIGN(len))
#define CMSG_LEN(len)   (CMSG_ALIGN(sizeof(struct cmsghdr)) + (len))

#define CMSG_FIRSTHDR(mhdr) \
	((mhdr)->msg_controllen >= CMSG_LEN(0) ? \
	 (struct cmsghdr *)(mhdr)->msg_control : \
	 (struct cmsghdr *)0)

#define CMSG_NXTHDR(mhdr, cmsg) \
	(((cmsg) == NULL) || \
	 ((cmsg)->cmsg_len < sizeof(struct cmsghdr)) || \
	 ((unsigned char *)(cmsg) + CMSG_ALIGN((cmsg)->cmsg_len) >= \
	  (unsigned char *)(mhdr)->msg_control + (mhdr)->msg_controllen) \
	 ? (struct cmsghdr *)NULL \
	 : (struct cmsghdr *)((unsigned char *)(cmsg) + CMSG_ALIGN((cmsg)->cmsg_len)))

#define CMSG_DATA(cmsg) ((unsigned char *)(cmsg) + CMSG_ALIGN(sizeof(struct cmsghdr)))

/* ======================================================================== */
/* Syscall Wrappers                                                         */
/* ======================================================================== */

#if JACL_HAS_POSIX

static inline int socket(int domain, int type, int protocol) {
	return (int)syscall(SYS_socket, domain, type, protocol);
}

static inline int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	return (int)syscall(SYS_bind, sockfd, addr, addrlen);
}

static inline int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	return (int)syscall(SYS_connect, sockfd, addr, addrlen);
}

static inline int listen(int sockfd, int backlog) {
	return (int)syscall(SYS_listen, sockfd, backlog);
}

static inline int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	return (int)syscall(SYS_accept, sockfd, addr, addrlen);
}

static inline int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	return (int)syscall(SYS_getsockname, sockfd, addr, addrlen);
}

static inline int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	return (int)syscall(SYS_getpeername, sockfd, addr, addrlen);
}

static inline ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) {
	if (len == 0) return 0;

	return syscall(SYS_sendto, sockfd, buf, len, flags, dest_addr, addrlen);
}

static inline ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) {
	if (len == 0) return 0;

	return syscall(SYS_recvfrom, sockfd, buf, len, flags, src_addr, addrlen);
}

static inline int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
	return (int)syscall(SYS_setsockopt, sockfd, level, optname, optval, optlen);
}

static inline int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) {
	return (int)syscall(SYS_getsockopt, sockfd, level, optname, optval, optlen);
}

static inline int shutdown(int sockfd, int how) {
	return (int)syscall(SYS_shutdown, sockfd, how);
}

static inline ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags) {
	return syscall(SYS_sendmsg, sockfd, msg, flags);
}

static inline ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
	return syscall(SYS_recvmsg, sockfd, msg, flags);
}

static inline int socketpair(int domain, int type, int protocol, int sv[]) {
	return (int)syscall(SYS_socketpair, domain, type, protocol, sv);
}

#else /* !JACL_HAS_POSIX */

static inline int socket(int d, int t, int p) { (void)d;(void)t;(void)p; errno=ENOSYS; return -1; }
static inline int bind(int s, const struct sockaddr *a, socklen_t l) { (void)s;(void)a;(void)l; errno=ENOSYS; return -1; }
static inline int connect(int s, const struct sockaddr *a, socklen_t l) { (void)s;(void)a;(void)l; errno=ENOSYS; return -1; }
static inline int listen(int s, int b) { (void)s;(void)b; errno=ENOSYS; return -1; }
static inline int accept(int s, struct sockaddr *a, socklen_t *l) { (void)s;(void)a;(void)l; errno=ENOSYS; return -1; }
static inline int getsockname(int s, struct sockaddr *a, socklen_t *l) { (void)s;(void)a;(void)l; errno=ENOSYS; return -1; }
static inline int getpeername(int s, struct sockaddr *a, socklen_t *l) { (void)s;(void)a;(void)l; errno=ENOSYS; return -1; }
static inline ssize_t sendto(int s, const void *b, size_t l, int f, const struct sockaddr *d, socklen_t a) { (void)s;(void)b;(void)l;(void)f;(void)d;(void)a; errno=ENOSYS; return -1; }
static inline ssize_t recvfrom(int s, void *b, size_t l, int f, struct sockaddr *src, socklen_t *a) { (void)s;(void)b;(void)l;(void)f;(void)src;(void)a; errno=ENOSYS; return -1; }
static inline int setsockopt(int s, int l, int o, const void *v, socklen_t n) { (void)s;(void)l;(void)o;(void)v;(void)n; errno=ENOSYS; return -1; }
static inline int getsockopt(int s, int l, int o, void *v, socklen_t *n) { (void)s;(void)l;(void)o;(void)v;(void)n; errno=ENOSYS; return -1; }
static inline int shutdown(int s, int h) { (void)s;(void)h; errno=ENOSYS; return -1; }
static inline ssize_t sendmsg(int s, const struct msghdr *m, int f) { (void)s;(void)m;(void)f; errno=ENOSYS; return -1; }
static inline ssize_t recvmsg(int s, struct msghdr *m, int f) { (void)s;(void)m;(void)f; errno=ENOSYS; return -1; }
static inline int socketpair(int d, int t, int p, int sv[]) { (void)d;(void)t;(void)p;(void)sv; errno=ENOSYS; return -1; }

#endif /* JACL_HAS_POSIX */

static inline ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
	return sendto(sockfd, buf, len, flags, NULL, 0);
}

static inline ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
	return recvfrom(sockfd, buf, len, flags, NULL, NULL);
}

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SOCKET_H */
