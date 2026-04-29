/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _TRANSIT_CONN_H
#define _TRANSIT_CONN_H
#pragma once

/**
 * Transport Connection (Protocol-Agnostic, Header-Only)
 *
 * DESIGN PHILOSOPHY:
 * - Single interface: One conn_t for TCP, UDP, Unix, TLS, QUIC.
 * - Header-only: All functions static inline. No .c file.
 * - Zero abstraction overhead: Direct syscalls inlined.
 * - Memory: Library allocates (malloc) in conn_create/from_fd, frees in conn_close.
 */

#include <config.h>
#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>      /* close(), read(), write() */
#include <fcntl.h>       /* fcntl(), O_NONBLOCK */
#include <sys/socket.h>  /* socket(), bind(), connect(), accept(), sockaddr_storage */
#include <sys/un.h>      /* struct sockaddr_un */
#include <sys/ioctl.h>   /* ioctl(), FIONREAD */
#include <netdb.h>       /* getaddrinfo() */
#include <string.h>      /* memset(), memcpy() */
#include <stdlib.h>      /* malloc(), free() */

/* JACL Network Headers */
#include <net/inet.h>
#include <net/tcp.h>
#include <net/udp.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Connection Types                                                         */
/* ======================================================================== */

typedef enum {
    CONN_STREAM,    /* TCP, Unix stream, TLS (ordered, reliable) */
    CONN_DGRAM,     /* UDP, Unix datagram (unordered, unreliable) */
    CONN_PACKET     /* Raw packet (Ethernet, IP) */
} conn_type_t;

/* ======================================================================== */
/* Connection Operations (Vtable)                                           */
/* ======================================================================== */

typedef struct conn_ops {
    ssize_t (*read)(void *ctx, void *buf, size_t len);
    ssize_t (*write)(void *ctx, const void *buf, size_t len);
    ssize_t (*recvfrom)(void *ctx, void *buf, size_t len, struct sockaddr *src, socklen_t *srclen);
    ssize_t (*sendto)(void *ctx, const void *buf, size_t len, const struct sockaddr *dst, socklen_t dstlen);
    int (*close)(void *ctx);
    int (*pending)(void *ctx);
    int (*set_timeout)(void *ctx, int timeout_ms);
    int (*set_option)(void *ctx, int level, int optname, const void *optval, socklen_t optlen);
} conn_ops_t;

/* ======================================================================== */
/* Connection Structure                                                     */
/* ======================================================================== */

/**
 * Connection handle
 *
 * MEMORY OWNERSHIP:
 * - conn_create(), conn_from_fd(): Library allocates (malloc).
 * - conn_close(): Library frees (free) after closing FD.
 * - User NEVER calls free() on conn_t directly.
 */
typedef struct conn {
    conn_type_t type;
    int domain;           /* AF_INET, AF_INET6, AF_UNIX, etc. */
    int socktype;         /* SOCK_STREAM, SOCK_DGRAM, etc. */
    int protocol;         /* IPPROTO_TCP, IPPROTO_UDP, etc. */
    int fd;               /* Underlying file descriptor */
    void *ctx;            /* Transport-specific context (usually &fd) */
    const conn_ops_t *ops;
    bool owns_fd;         /* True if conn_close() should close fd */
} conn_t;

/* ======================================================================== */
/* Internal Helpers (Static Inline)                                         */
/* ======================================================================== */

/* Default Stream Ops (TCP/Unix) */
static inline ssize_t _conn_std_read(void *ctx, void *buf, size_t len) {
    if (!ctx) { errno = EBADF; return -1; }
    return read(*(int *)ctx, buf, len);
}

static inline ssize_t _conn_std_write(void *ctx, const void *buf, size_t len) {
    if (!ctx) { errno = EBADF; return -1; }
    return write(*(int *)ctx, buf, len);
}

static inline int _conn_std_close(void *ctx) {
    if (!ctx) return 0;
    int fd = *(int *)ctx;
    return (fd >= 0) ? close(fd) : 0;
}

static inline int _conn_std_pending(void *ctx) {
    if (!ctx) { errno = EBADF; return -1; }
    int fd = *(int *)ctx;
    int val = 0;
    if (ioctl(fd, FIONREAD, &val) < 0) return -1;
    return (val > 0) ? 1 : 0;
}

static const conn_ops_t _conn_stream_ops = {
    .read = _conn_std_read,
    .write = _conn_std_write,
    .recvfrom = NULL,
    .sendto = NULL,
    .close = _conn_std_close,
    .pending = _conn_std_pending,
    .set_timeout = NULL,
    .set_option = NULL
};

/* Default Datagram Ops (UDP) */
static inline ssize_t _conn_dgram_recvfrom(void *ctx, void *buf, size_t len, struct sockaddr *src, socklen_t *srclen) {
    if (!ctx) { errno = EBADF; return -1; }
    return recvfrom(*(int *)ctx, buf, len, 0, src, srclen);
}

static inline ssize_t _conn_dgram_sendto(void *ctx, const void *buf, size_t len, const struct sockaddr *dst, socklen_t dstlen) {
    if (!ctx) { errno = EBADF; return -1; }
    return sendto(*(int *)ctx, buf, len, 0, dst, dstlen);
}

static const conn_ops_t _conn_dgram_ops = {
    .read = _conn_std_read,      /* Connected UDP can use read/write */
    .write = _conn_std_write,
    .recvfrom = _conn_dgram_recvfrom,
    .sendto = _conn_dgram_sendto,
    .close = _conn_std_close,
    .pending = _conn_std_pending,
    .set_timeout = NULL,
    .set_option = NULL
};

/* ======================================================================== */
/* Connection Creation                                                      */
/* ======================================================================== */

static inline conn_t *conn_create(int domain, int type, int protocol) {
    int fd = socket(domain, type, protocol);
    if (fd < 0) return NULL;

    conn_t *c = (conn_t *)malloc(sizeof(conn_t));
    if (!c) {
        close(fd);
        return NULL;
    }

    c->type = (type == SOCK_DGRAM) ? CONN_DGRAM : CONN_STREAM;
    c->domain = domain;
    c->socktype = type;
    c->protocol = protocol;
    c->fd = fd;
    c->ctx = &c->fd;
    c->owns_fd = true;
    c->ops = (type == SOCK_DGRAM) ? &_conn_dgram_ops : &_conn_stream_ops;

    return c;
}

static inline conn_t *conn_from_fd(int fd, int domain, int type, int protocol) {
    if (fd < 0) return NULL;

    conn_t *c = (conn_t *)malloc(sizeof(conn_t));
    if (!c) return NULL;

    c->type = (type == SOCK_DGRAM) ? CONN_DGRAM : CONN_STREAM;
    c->domain = domain;
    c->socktype = type;
    c->protocol = protocol;
    c->fd = fd;
    c->ctx = &c->fd;
    c->owns_fd = true;
    c->ops = (type == SOCK_DGRAM) ? &_conn_dgram_ops : &_conn_stream_ops;

    return c;
}

/* ======================================================================== */
/* Connection Management                                                    */
/* ======================================================================== */

static inline int conn_connect(conn_t *c, const char *host, const char *port) {
    if (!c || !host || !port) { errno = EINVAL; return -1; }

    struct addrinfo hints = {0}, *res;
    hints.ai_family = c->domain;
    hints.ai_socktype = c->socktype;
    hints.ai_protocol = c->protocol;

    if (getaddrinfo(host, port, &hints, &res) != 0) return -1;

    int r = connect(c->fd, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    return r;
}

static inline int conn_connect_path(conn_t *c, const char *path) {
    if (!c || !path || c->domain != AF_UNIX) { errno = EINVAL; return -1; }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    return connect(c->fd, (struct sockaddr *)&addr, sizeof(addr));
}

static inline int conn_bind(conn_t *c, const char *host, const char *port) {
    if (!c) { errno = EINVAL; return -1; }

    struct addrinfo hints = {0}, *res;
    hints.ai_family = c->domain;
    hints.ai_socktype = c->socktype;
    hints.ai_protocol = c->protocol;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(host, port, &hints, &res) != 0) return -1;

    int r = bind(c->fd, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    return r;
}

static inline int conn_bind_path(conn_t *c, const char *path) {
    if (!c || !path || c->domain != AF_UNIX) { errno = EINVAL; return -1; }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    
    unlink(path); /* Remove existing socket file */

    return bind(c->fd, (struct sockaddr *)&addr, sizeof(addr));
}

static inline int conn_listen(conn_t *c, int backlog) {
    if (!c) { errno = EINVAL; return -1; }
    return listen(c->fd, backlog);
}

static inline conn_t *conn_accept(conn_t *c) {
    if (!c) { errno = EINVAL; return NULL; }

    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);
    int fd = accept(c->fd, (struct sockaddr *)&addr, &addrlen);

    if (fd < 0) return NULL;

    return conn_from_fd(fd, c->domain, c->socktype, c->protocol);
}

/* ======================================================================== */
/* Generic Operations (Inline Wrappers)                                     */
/* ======================================================================== */

static inline ssize_t conn_read(conn_t *c, void *buf, size_t len) {
    if (!c || !c->ops) { errno = EINVAL; return -1; }
    return c->ops->read(c->ctx, buf, len);
}

static inline ssize_t conn_write(conn_t *c, const void *buf, size_t len) {
    if (!c || !c->ops) { errno = EINVAL; return -1; }
    return c->ops->write(c->ctx, buf, len);
}

static inline ssize_t conn_recvfrom(conn_t *c, void *buf, size_t len, struct sockaddr *src, socklen_t *srclen) {
    if (!c || !c->ops) { errno = EINVAL; return -1; }
    return c->ops->recvfrom ? c->ops->recvfrom(c->ctx, buf, len, src, srclen) : -1;
}

static inline ssize_t conn_sendto(conn_t *c, const void *buf, size_t len, const struct sockaddr *dst, socklen_t dstlen) {
    if (!c || !c->ops) { errno = EINVAL; return -1; }
    return c->ops->sendto ? c->ops->sendto(c->ctx, buf, len, dst, dstlen) : -1;
}

static inline int conn_close(conn_t *c) {
    if (!c) { errno = EINVAL; return -1; }
    int r = c->ops->close(c->ctx);
    free(c); /* Library allocated, library frees */
    return r;
}

static inline int conn_pending(conn_t *c) {
    if (!c || !c->ops) { errno = EINVAL; return -1; }
    return c->ops->pending(c->ctx);
}

static inline int conn_set_timeout(conn_t *c, int timeout_ms) {
    if (!c || !c->ops || !c->ops->set_timeout) { errno = EINVAL; return -1; }
    return c->ops->set_timeout(c->ctx, timeout_ms);
}

static inline int conn_set_option(conn_t *c, int level, int optname, const void *optval, socklen_t optlen) {
    if (!c || !c->ops || !c->ops->set_option) { errno = EINVAL; return -1; }
    return c->ops->set_option(c->ctx, level, optname, optval, optlen);
}

#ifdef __cplusplus
}
#endif

#endif /* _TRANSIT_CONN_H */
