/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H
#pragma once

#include <config.h>
#include <stddef.h>    /* for size_t */
#include <sys/uio.h>   /* for struct iovec */
#include <sys/types.h>
#include <sys/syscall.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Socket Address Families */
#define AF_UNSPEC    0
#define AF_UNIX      1
#define AF_LOCAL     1
#define AF_INET      2
#define AF_AX25      3
#define AF_IPX       4
#define AF_APPLETALK 5
#define AF_NETROM    6
#define AF_BRIDGE    7
#define AF_ATMPVC    8
#define AF_X25       9
#define AF_INET6     10
#define AF_ROSE      11
#define AF_DECnet    12
#define AF_NETBEUI   13
#define AF_SECURITY  14
#define AF_KEY       15
#define AF_NETLINK   16
#define AF_ROUTE     16
#define AF_PACKET    17
#define AF_ASH       18
#define AF_ECONET    19
#define AF_ATMSVC    20
#define AF_RDS       21
#define AF_SNA       22
#define AF_IRDA      23
#define AF_PPPOX     24
#define AF_WANPIPE   25
#define AF_LLC       26
#define AF_IB        27
#define AF_MPLS      28
#define AF_CAN       29
#define AF_TIPC      30
#define AF_BLUETOOTH 31
#define AF_IUCV      32
#define AF_RXRPC     33
#define AF_ISDN      34
#define AF_PHONET    35
#define AF_IEEE802154 36
#define AF_CAIF      37
#define AF_ALG       38
#define AF_NFC       39
#define AF_VSOCK     40
#define AF_KCM       41
#define AF_QIPCRTR   42
#define AF_SMC       43
#define AF_XDP       44

/* Socket Types */
#define SOCK_STREAM    1
#define SOCK_DGRAM     2
#define SOCK_RAW       3
#define SOCK_RDM       4
#define SOCK_SEQPACKET 5
#define SOCK_DCCP      6
#define SOCK_PACKET    10

/* Protocol Levels */
#define SOL_SOCKET     1

/* Socket Options */
#define SO_DEBUG       1
#define SO_REUSEADDR   2
#define SO_TYPE        3
#define SO_ERROR       4
#define SO_DONTROUTE   5
#define SO_BROADCAST   6
#define SO_SNDBUF      7
#define SO_RCVBUF      8
#define SO_KEEPALIVE   9
#define SO_OOBINLINE   10
#define SO_LINGER      13
#define SO_REUSEPORT   15

/* Message Flags */
#define MSG_OOB       0x01
#define MSG_PEEK      0x02
#define MSG_DONTROUTE 0x04
#define MSG_CTRUNC    0x08
#define MSG_TRUNC     0x20
#define MSG_DONTWAIT  0x40
#define MSG_EOR       0x80
#define MSG_WAITALL   0x100
#define MSG_NOSIGNAL  0x4000

/* Shutdown Commands */
#define SHUT_RD   0
#define SHUT_WR   1
#define SHUT_RDWR 2

/* Generic Socket Address */
struct sockaddr {
    unsigned short sa_family;
    char           sa_data[14];
};

/* Message Header for scatter/gather I/O */
struct msghdr {
    void         *msg_name;       /* Optional address */
    socklen_t     msg_namelen;    /* Size of address */
    struct iovec *msg_iov;        /* Scatter/gather array */
    size_t        msg_iovlen;     /* # elements in msg_iov */
    void         *msg_control;    /* Ancillary data */
    size_t        msg_controllen; /* Ancillary data buffer len */
    int           msg_flags;      /* Flags on received message */
};

/* Ancillary Data Object */
struct cmsghdr {
    socklen_t cmsg_len;   /* Data byte count, including header */
    int       cmsg_level; /* Originating protocol */
    int       cmsg_type;  /* Protocol-specific type */
};

/* Ancillary Data Macros */
#define CMSG_ALIGN(len) (((len) + sizeof(size_t) - 1) & ~(sizeof(size_t) - 1))
#define CMSG_SPACE(len) (CMSG_ALIGN(sizeof(struct cmsghdr)) + CMSG_ALIGN(len))
#define CMSG_LEN(len)   (CMSG_ALIGN(sizeof(struct cmsghdr)) + (len))
#define CMSG_FIRSTHDR(mhdr) \
    ((mhdr)->msg_controllen >= sizeof(struct cmsghdr) ? \
     (struct cmsghdr *)(mhdr)->msg_control : \
     (struct cmsghdr *)0)
#define CMSG_NXTHDR(mhdr, cmsg) \
    (((cmsg)->cmsg_len + CMSG_ALIGN(sizeof(struct cmsghdr)) > (mhdr)->msg_controllen) ? \
     (struct cmsghdr *)0 : \
     (struct cmsghdr *)((char *)(cmsg) + CMSG_ALIGN((cmsg)->cmsg_len)))
#define CMSG_DATA(cmsg) ((unsigned char *)((struct cmsghdr *)(cmsg) + 1))

#if JACL_HAS_POSIX

/* Core Socket Syscalls */
static inline int socket(int domain, int type, int protocol) { return (int)syscall(SYS_socket, domain, type, protocol); }
static inline int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { return (int)syscall(SYS_bind, sockfd, addr, addrlen); }
static inline int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { return (int)syscall(SYS_connect, sockfd, addr, addrlen); }
static inline int listen(int sockfd, int backlog) { return (int)syscall(SYS_listen, sockfd, backlog); }
static inline int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) { return (int)syscall(SYS_accept, sockfd, addr, addrlen); }
static inline int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen) { return (int)syscall(SYS_getsockname, sockfd, addr, addrlen); }
static inline int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen) { return (int)syscall(SYS_getpeername, sockfd, addr, addrlen); }
static inline ssize_t send(int sockfd, const void *buf, size_t len, int flags) { return syscall(SYS_sendto, sockfd, buf, len, flags, NULL, 0); }
static inline ssize_t recv(int sockfd, void *buf, size_t len, int flags) { return syscall(SYS_recvfrom, sockfd, buf, len, flags, NULL, NULL); }
static inline ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) { return syscall(SYS_sendto, sockfd, buf, len, flags, dest_addr, addrlen); }
static inline ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) { return syscall(SYS_recvfrom, sockfd, buf, len, flags, src_addr, addrlen); }
static inline int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) { return (int)syscall(SYS_setsockopt, sockfd, level, optname, optval, optlen); }
static inline int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) { return (int)syscall(SYS_getsockopt, sockfd, level, optname, optval, optlen); }
static inline int shutdown(int sockfd, int how) { return (int)syscall(SYS_shutdown, sockfd, how); }
static inline int accept4(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int flags) {
	#if JACL_HASSYS(accept4)
		return (int)syscall(SYS_accept4, sockfd, addr, addrlen, flags);
	#else
		(void)flags;
		return accept(sockfd, addr, addrlen);
	#endif
}

/* Scatter/Gather I/O */
static inline ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags) { return syscall(SYS_sendmsg, sockfd, msg, flags); }
static inline ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) { return syscall(SYS_recvmsg, sockfd, msg, flags); }

#else /* !JACL_HAS_POSIX */

static inline int socket(int domain, int type, int protocol) { (void)domain; (void)type; (void)protocol; errno = ENOSYS; return -1; }
static inline int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { (void)sockfd; (void)addr; (void)addrlen; errno = ENOSYS; return -1; }
static inline int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { (void)sockfd; (void)addr; (void)addrlen; errno = ENOSYS; return -1; }
static inline int listen(int sockfd, int backlog) { (void)sockfd; (void)backlog; errno = ENOSYS; return -1; }
static inline int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) { (void)sockfd; (void)addr; (void)addrlen; errno = ENOSYS; return -1; }
static inline int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen) { (void)sockfd; (void)addr; (void)addrlen; errno = ENOSYS; return -1; }
static inline int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen) { (void)sockfd; (void)addr; (void)addrlen; errno = ENOSYS; return -1; }
static inline ssize_t send(int sockfd, const void *buf, size_t len, int flags) { (void)sockfd; (void)buf; (void)len; (void)flags; errno = ENOSYS; return -1; }
static inline ssize_t recv(int sockfd, void *buf, size_t len, int flags) { (void)sockfd; (void)buf; (void)len; (void)flags; errno = ENOSYS; return -1; }
static inline ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) { (void)sockfd; (void)buf; (void)len; (void)flags; (void)dest_addr; (void)addrlen; errno = ENOSYS; return -1; }
static inline ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) { (void)sockfd; (void)buf; (void)len; (void)flags; (void)src_addr; (void)addrlen; errno = ENOSYS; return -1; }
static inline int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) { (void)sockfd; (void)level; (void)optname; (void)optval; (void)optlen; errno = ENOSYS; return -1; }
static inline int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) { (void)sockfd; (void)level; (void)optname; (void)optval; (void)optlen; errno = ENOSYS; return -1; }
static inline int shutdown(int sockfd, int how) { (void)sockfd; (void)how; errno = ENOSYS; return -1; }
static inline ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags) { (void)sockfd; (void)msg; (void)flags; errno = ENOSYS; return -1; }
static inline ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) { (void)sockfd; (void)msg; (void)flags; errno = ENOSYS; return -1; }

#endif /* JACL_HAS_POSIX */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_SOCKET_H */
