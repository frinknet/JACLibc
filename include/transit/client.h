/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _TRANSIT_CLIENT_H
#define _TRANSIT_CLIENT_H
#pragma once

/**
 * HTTP Client (Header-Only)
 *
 * DESIGN PHILOSOPHY:
 * - Simple HTTP: One-liners for GET/POST (90% of cases)
 * - Pluggable transport: TCP, UDP, Unix, TLS, QUIC (10% of cases)
 * - Reusable connections: client_t for keep-alive, connection pools
 * - Header-only: All functions static inline. No .c file.
 * - Memory: Library allocates (malloc) in client_create, frees in client_free.
 * - Lazy Connection: conn_t created in client_connect(), not client_create()
 */

#include <config.h>
#include <transit/conn.h>
#include <transit/http.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Client Structure                                                         */
/* ======================================================================== */

typedef struct client {
    conn_t *conn;
    int domain;       /* Store for later conn_create */
    int type;         /* Store for later conn_create */
    int protocol;     /* Store for later conn_create */
    char *host;
    char *port;
    int timeout_ms;
    bool connected;
    bool keep_alive;
} client_t;

/* ======================================================================== */
/* Simple HTTP One-Liners (90% of Cases)                                    */
/* ======================================================================== */

static inline int http_get(const char *host, const char *port, const char *path,
                           http_res_t *res_out, uint8_t *body_buf, size_t body_buf_size) {
    if (!host || !port || !path || !res_out || !body_buf || body_buf_size == 0) {
        errno = EINVAL;
        return -1;
    }

    conn_t *c = conn_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (!c) return -1;

    if (conn_connect(c, host, port) < 0) {
        conn_close(c);
        return -1;
    }

    char req[1024];
    int len = snprintf(req, sizeof(req),
                       "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
                       path, host);
    if (len < 0 || (size_t)len >= sizeof(req)) {
        conn_close(c);
        return -1;
    }

    if (conn_write(c, req, (size_t)len) < 0) {
        conn_close(c);
        return -1;
    }

    ssize_t n = conn_read(c, body_buf, body_buf_size - 1);
    conn_close(c);

    if (n <= 0) return -1;
    body_buf[n] = '\0';

    char *sp = strchr((char *)body_buf, ' ');
    if (!sp) return -1;
    res_out->code = (http_code_t)atoi(sp + 1);

    char *body_start = strstr((char *)body_buf, "\r\n\r\n");
    if (!body_start) return -1;
    body_start += 4;

    size_t body_len = (size_t)(n - (body_start - (char *)body_buf));
    if (body_len >= body_buf_size) body_len = body_buf_size - 1;
    memmove(body_buf, body_start, body_len);
    body_buf[body_len] = '\0';

    return 0;
}

static inline int http_post(const char *host, const char *port, const char *path,
                            const uint8_t *body, size_t body_len,
                            http_res_t *res_out, uint8_t *resp_body_buf, size_t resp_body_buf_size) {
    if (!host || !port || !path || !body || !res_out || !resp_body_buf || resp_body_buf_size == 0) {
        errno = EINVAL;
        return -1;
    }

    conn_t *c = conn_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (!c) return -1;

    if (conn_connect(c, host, port) < 0) {
        conn_close(c);
        return -1;
    }

    char req[2048];
    int len = snprintf(req, sizeof(req),
                       "POST %s HTTP/1.1\r\nHost: %s\r\nContent-Length: %zu\r\nConnection: close\r\n\r\n",
                       path, host, body_len);
    if (len < 0 || (size_t)len >= sizeof(req)) {
        conn_close(c);
        return -1;
    }

    if (conn_write(c, req, (size_t)len) < 0 || conn_write(c, body, body_len) < 0) {
        conn_close(c);
        return -1;
    }

    ssize_t n = conn_read(c, resp_body_buf, resp_body_buf_size - 1);
    conn_close(c);

    if (n <= 0) return -1;
    resp_body_buf[n] = '\0';

    char *sp = strchr((char *)resp_body_buf, ' ');
    if (!sp) return -1;
    res_out->code = (http_code_t)atoi(sp + 1);

    char *body_start = strstr((char *)resp_body_buf, "\r\n\r\n");
    if (!body_start) return -1;
    body_start += 4;

    size_t resp_len = (size_t)(n - (body_start - (char *)resp_body_buf));
    if (resp_len >= resp_body_buf_size) resp_len = resp_body_buf_size - 1;
    memmove(resp_body_buf, body_start, resp_len);
    resp_body_buf[resp_len] = '\0';

    return 0;
}

static inline int http_put(const char *host, const char *port, const char *path,
                           const uint8_t *body, size_t body_len,
                           http_res_t *res_out, uint8_t *resp_body_buf, size_t resp_body_buf_size) {
    if (!host || !port || !path || !body || !res_out || !resp_body_buf || resp_body_buf_size == 0) {
        errno = EINVAL;
        return -1;
    }

    conn_t *c = conn_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (!c) return -1;

    if (conn_connect(c, host, port) < 0) {
        conn_close(c);
        return -1;
    }

    char req[2048];
    int len = snprintf(req, sizeof(req),
                       "PUT %s HTTP/1.1\r\nHost: %s\r\nContent-Length: %zu\r\nConnection: close\r\n\r\n",
                       path, host, body_len);
    if (len < 0 || (size_t)len >= sizeof(req)) {
        conn_close(c);
        return -1;
    }

    if (conn_write(c, req, (size_t)len) < 0 || conn_write(c, body, body_len) < 0) {
        conn_close(c);
        return -1;
    }

    ssize_t n = conn_read(c, resp_body_buf, resp_body_buf_size - 1);
    conn_close(c);
    if (n <= 0) return -1;
    resp_body_buf[n] = '\0';

    char *sp = strchr((char *)resp_body_buf, ' ');
    if (!sp) return -1;
    res_out->code = (http_code_t)atoi(sp + 1);

    char *body_start = strstr((char *)resp_body_buf, "\r\n\r\n");
    if (!body_start) return -1;
    body_start += 4;
    size_t resp_len = (size_t)(n - (body_start - (char *)resp_body_buf));
    if (resp_len >= resp_body_buf_size) resp_len = resp_body_buf_size - 1;
    memmove(resp_body_buf, body_start, resp_len);
    resp_body_buf[resp_len] = '\0';

    return 0;
}

static inline int http_delete(const char *host, const char *port, const char *path,
                              http_res_t *res_out, uint8_t *resp_body_buf, size_t resp_body_buf_size) {
    if (!host || !port || !path || !res_out || !resp_body_buf || resp_body_buf_size == 0) {
        errno = EINVAL;
        return -1;
    }

    conn_t *c = conn_create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (!c) return -1;

    if (conn_connect(c, host, port) < 0) {
        conn_close(c);
        return -1;
    }

    char req[1024];
    int len = snprintf(req, sizeof(req),
                       "DELETE %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
                       path, host);
    if (len < 0 || (size_t)len >= sizeof(req)) {
        conn_close(c);
        return -1;
    }

    if (conn_write(c, req, (size_t)len) < 0) {
        conn_close(c);
        return -1;
    }

    ssize_t n = conn_read(c, resp_body_buf, resp_body_buf_size - 1);
    conn_close(c);
    if (n <= 0) return -1;
    resp_body_buf[n] = '\0';

    char *sp = strchr((char *)resp_body_buf, ' ');
    if (!sp) return -1;
    res_out->code = (http_code_t)atoi(sp + 1);

    char *body_start = strstr((char *)resp_body_buf, "\r\n\r\n");
    if (!body_start) return -1;
    body_start += 4;
    size_t resp_len = (size_t)(n - (body_start - (char *)resp_body_buf));
    if (resp_len >= resp_body_buf_size) resp_len = resp_body_buf_size - 1;
    memmove(resp_body_buf, body_start, resp_len);
    resp_body_buf[resp_len] = '\0';

    return 0;
}

/* ======================================================================== */
/* Reusable Client (10% of Cases)                                           */
/* ======================================================================== */

static inline client_t *client_create(int domain, int type, int protocol) {
    client_t *c = (client_t *)malloc(sizeof(client_t));
    if (!c) return NULL;

    /* Don't create conn yet - defer to client_connect() */
    c->conn = NULL;
    c->domain = domain;
    c->type = type;
    c->protocol = protocol;
    c->host = NULL;
    c->port = NULL;
    c->timeout_ms = -1;
    c->connected = false;
    c->keep_alive = false;

    return c;
}

static inline client_t *client_from_conn(conn_t *conn) {
    if (!conn) return NULL;

    client_t *c = (client_t *)malloc(sizeof(client_t));
    if (!c) return NULL;

    c->conn = conn;
    c->domain = conn->domain;
    c->type = conn->socktype;
    c->protocol = conn->protocol;
    c->host = NULL;
    c->port = NULL;
    c->timeout_ms = -1;
    c->connected = true;
    c->keep_alive = false;

    return c;
}

static inline int client_connect(client_t *c, const char *host, const char *port) {
    if (!c || !host || !port) {
        errno = EINVAL;
        return -1;
    }

    /* Create connection now (lazy initialization) */
    if (!c->conn) {
        c->conn = conn_create(c->domain, c->type, c->protocol);
        if (!c->conn) return -1;
    }

    int r = conn_connect(c->conn, host, port);
    if (r == 0) {
        c->connected = true;
        c->host = strdup(host);
        c->port = strdup(port);
    }
    return r;
}

static inline int client_connect_path(client_t *c, const char *path) {
    if (!c || !path) {
        errno = EINVAL;
        return -1;
    }

    /* Create connection now (lazy initialization) */
    if (!c->conn) {
        c->conn = conn_create(c->domain, c->type, c->protocol);
        if (!c->conn) return -1;
    }

    if (c->conn->domain != AF_UNIX) {
        errno = EINVAL;
        return -1;
    }

    int r = conn_connect_path(c->conn, path);
    if (r == 0) c->connected = true;
    return r;
}

static inline void client_disconnect(client_t *c) {
    if (!c) return;
    if (c->conn) {
        conn_close(c->conn);
        c->conn = NULL;
    }
    c->connected = false;
    free(c->host);
    free(c->port);
    c->host = c->port = NULL;
}

static inline void client_free(client_t *c) {
    if (!c) return;
    if (c->conn) conn_close(c->conn);
    free(c->host);
    free(c->port);
    free(c);
}

static inline int client_set_timeout(client_t *c, int timeout_ms) {
    if (!c || !c->conn) {
        errno = EINVAL;
        return -1;
    }
    c->timeout_ms = timeout_ms;
    return conn_set_timeout(c->conn, timeout_ms);
}

static inline void client_set_keepalive(client_t *c, bool keep_alive) {
    if (c) c->keep_alive = keep_alive;
}

static inline ssize_t client_send(client_t *c, const void *buf, size_t len) {
    if (!c || !c->conn) { errno = EBADF; return -1; }
    return conn_write(c->conn, buf, len);
}

static inline ssize_t client_recv(client_t *c, void *buf, size_t len) {
    if (!c || !c->conn) { errno = EBADF; return -1; }
    return conn_read(c->conn, buf, len);
}

/* ======================================================================== */
/* HTTP Methods (Reusable Connection)                                       */
/* ======================================================================== */

static inline int client_get(client_t *c, const char *path,
                             http_res_t *res_out, uint8_t *body_buf, size_t body_buf_size) {
    if (!c || !c->conn || !c->connected || !path || !res_out || !body_buf || body_buf_size == 0) {
        errno = EINVAL;
        return -1;
    }

    char req[1024];
    int len = snprintf(req, sizeof(req),
                       "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: %s\r\n\r\n",
                       path, c->host ? c->host : "localhost",
                       c->keep_alive ? "keep-alive" : "close");
    if (len < 0 || (size_t)len >= sizeof(req)) return -1;

    if (conn_write(c->conn, req, (size_t)len) < 0) return -1;

    ssize_t n = conn_read(c->conn, body_buf, body_buf_size - 1);
    if (n <= 0) {
        if (!c->keep_alive) client_disconnect(c);
        return -1;
    }
    body_buf[n] = '\0';

    char *sp = strchr((char *)body_buf, ' ');
    if (!sp) {
        if (!c->keep_alive) client_disconnect(c);
        return -1;
    }
    res_out->code = (http_code_t)atoi(sp + 1);

    char *body_start = strstr((char *)body_buf, "\r\n\r\n");
    if (!body_start) {
        if (!c->keep_alive) client_disconnect(c);
        return -1;
    }
    body_start += 4;

    size_t body_len = (size_t)(n - (body_start - (char *)body_buf));
    if (body_len >= body_buf_size) body_len = body_buf_size - 1;
    memmove(body_buf, body_start, body_len);
    body_buf[body_len] = '\0';

    if (!c->keep_alive) client_disconnect(c);
    return 0;
}

static inline int client_post(client_t *c, const char *path,
                              const uint8_t *body, size_t body_len,
                              http_res_t *res_out, uint8_t *resp_body_buf, size_t resp_body_buf_size) {
    if (!c || !c->conn || !c->connected || !path || !body || !res_out || !resp_body_buf || resp_body_buf_size == 0) {
        errno = EINVAL;
        return -1;
    }

    char req[2048];
    int len = snprintf(req, sizeof(req),
                       "POST %s HTTP/1.1\r\nHost: %s\r\nContent-Length: %zu\r\nConnection: %s\r\n\r\n",
                       path, c->host ? c->host : "localhost", body_len,
                       c->keep_alive ? "keep-alive" : "close");
    if (len < 0 || (size_t)len >= sizeof(req)) return -1;

    if (conn_write(c->conn, req, (size_t)len) < 0 || conn_write(c->conn, body, body_len) < 0) {
        if (!c->keep_alive) client_disconnect(c);
        return -1;
    }

    ssize_t n = conn_read(c->conn, resp_body_buf, resp_body_buf_size - 1);
    if (n <= 0) {
        if (!c->keep_alive) client_disconnect(c);
        return -1;
    }
    resp_body_buf[n] = '\0';

    char *sp = strchr((char *)resp_body_buf, ' ');
    if (!sp) {
        if (!c->keep_alive) client_disconnect(c);
        return -1;
    }
    res_out->code = (http_code_t)atoi(sp + 1);

    char *body_start = strstr((char *)resp_body_buf, "\r\n\r\n");
    if (!body_start) {
        if (!c->keep_alive) client_disconnect(c);
        return -1;
    }
    body_start += 4;
    size_t resp_len = (size_t)(n - (body_start - (char *)resp_body_buf));
    if (resp_len >= resp_body_buf_size) resp_len = resp_body_buf_size - 1;
    memmove(resp_body_buf, body_start, resp_len);
    resp_body_buf[resp_len] = '\0';

    if (!c->keep_alive) client_disconnect(c);
    return 0;
}

static inline int client_put(client_t *c, const char *path,
                             const uint8_t *body, size_t body_len,
                             http_res_t *res_out, uint8_t *resp_body_buf, size_t resp_body_buf_size) {
    if (!c || !c->conn || !c->connected || !path || !body || !res_out || !resp_body_buf || resp_body_buf_size == 0) {
        errno = EINVAL;
        return -1;
    }

    char req[2048];
    int len = snprintf(req, sizeof(req),
                       "PUT %s HTTP/1.1\r\nHost: %s\r\nContent-Length: %zu\r\nConnection: %s\r\n\r\n",
                       path, c->host ? c->host : "localhost", body_len,
                       c->keep_alive ? "keep-alive" : "close");
    if (len < 0 || (size_t)len >= sizeof(req)) return -1;

    if (conn_write(c->conn, req, (size_t)len) < 0 || conn_write(c->conn, body, body_len) < 0) {
        if (!c->keep_alive) client_disconnect(c);
        return -1;
    }

    ssize_t n = conn_read(c->conn, resp_body_buf, resp_body_buf_size - 1);
    if (n <= 0) {
        if (!c->keep_alive) client_disconnect(c);
        return -1;
    }
    resp_body_buf[n] = '\0';

    char *sp = strchr((char *)resp_body_buf, ' ');
    if (!sp) {
        if (!c->keep_alive) client_disconnect(c);
        return -1;
    }
    res_out->code = (http_code_t)atoi(sp + 1);

    char *body_start = strstr((char *)resp_body_buf, "\r\n\r\n");
    if (!body_start) {
        if (!c->keep_alive) client_disconnect(c);
        return -1;
    }
    body_start += 4;
    size_t resp_len = (size_t)(n - (body_start - (char *)resp_body_buf));
    if (resp_len >= resp_body_buf_size) resp_len = resp_body_buf_size - 1;
    memmove(resp_body_buf, body_start, resp_len);
    resp_body_buf[resp_len] = '\0';

    if (!c->keep_alive) client_disconnect(c);
    return 0;
}

static inline int client_delete(client_t *c, const char *path,
                                http_res_t *res_out, uint8_t *resp_body_buf, size_t resp_body_buf_size) {
    if (!c || !c->conn || !c->connected || !path || !res_out || !resp_body_buf || resp_body_buf_size == 0) {
        errno = EINVAL;
        return -1;
    }

    char req[1024];
    int len = snprintf(req, sizeof(req),
                       "DELETE %s HTTP/1.1\r\nHost: %s\r\nConnection: %s\r\n\r\n",
                       path, c->host ? c->host : "localhost",
                       c->keep_alive ? "keep-alive" : "close");
    if (len < 0 || (size_t)len >= sizeof(req)) return -1;

    if (conn_write(c->conn, req, (size_t)len) < 0) {
        if (!c->keep_alive) client_disconnect(c);
        return -1;
    }

    ssize_t n = conn_read(c->conn, resp_body_buf, resp_body_buf_size - 1);
    if (n <= 0) {
        if (!c->keep_alive) client_disconnect(c);
        return -1;
    }
    resp_body_buf[n] = '\0';

    char *sp = strchr((char *)resp_body_buf, ' ');
    if (!sp) {
        if (!c->keep_alive) client_disconnect(c);
        return -1;
    }
    res_out->code = (http_code_t)atoi(sp + 1);

    char *body_start = strstr((char *)resp_body_buf, "\r\n\r\n");
    if (!body_start) {
        if (!c->keep_alive) client_disconnect(c);
        return -1;
    }
    body_start += 4;
    size_t resp_len = (size_t)(n - (body_start - (char *)resp_body_buf));
    if (resp_len >= resp_body_buf_size) resp_len = resp_body_buf_size - 1;
    memmove(resp_body_buf, body_start, resp_len);
    resp_body_buf[resp_len] = '\0';

    if (!c->keep_alive) client_disconnect(c);
    return 0;
}

static inline int client_request(client_t *c, const char *method, const char *path,
                                 const char **headers,
                                 const uint8_t *body, size_t body_len,
                                 http_res_t *res_out, uint8_t *resp_body_buf, size_t resp_body_buf_size) {
    if (!c || !c->conn || !c->connected || !method || !path || !res_out || !resp_body_buf || resp_body_buf_size == 0) {
        errno = EINVAL;
        return -1;
    }

    char req[4096];
    int len = snprintf(req, sizeof(req), "%s %s HTTP/1.1\r\nHost: %s\r\n",
                       method, path, c->host ? c->host : "localhost");
    if (len < 0 || (size_t)len >= sizeof(req)) return -1;

    if (headers) {
        for (int i = 0; headers[i]; i++) {
            int hlen = snprintf(req + len, sizeof(req) - (size_t)len, "%s\r\n", headers[i]);
            if (hlen < 0 || (size_t)(len + hlen) >= sizeof(req)) return -1;
            len += hlen;
        }
    }

    if (body && body_len > 0) {
        int hlen = snprintf(req + len, sizeof(req) - (size_t)len, "Content-Length: %zu\r\n\r\n", body_len);
        if (hlen < 0 || (size_t)(len + hlen) >= sizeof(req)) return -1;
        len += hlen;
    } else {
        int hlen = snprintf(req + len, sizeof(req) - (size_t)len, "\r\n");
        if (hlen < 0 || (size_t)(len + hlen) >= sizeof(req)) return -1;
        len += hlen;
    }

    if (conn_write(c->conn, req, (size_t)len) < 0) return -1;
    if (body && body_len > 0 && conn_write(c->conn, body, body_len) < 0) return -1;

    ssize_t n = conn_read(c->conn, resp_body_buf, resp_body_buf_size - 1);
    if (n <= 0) return -1;
    resp_body_buf[n] = '\0';

    char *sp = strchr((char *)resp_body_buf, ' ');
    if (!sp) return -1;
    res_out->code = (http_code_t)atoi(sp + 1);

    char *body_start = strstr((char *)resp_body_buf, "\r\n\r\n");
    if (!body_start) return -1;
    body_start += 4;
    size_t resp_len = (size_t)(n - (body_start - (char *)resp_body_buf));
    if (resp_len >= resp_body_buf_size) resp_len = resp_body_buf_size - 1;
    memmove(resp_body_buf, body_start, resp_len);
    resp_body_buf[resp_len] = '\0';

    return 0;
}

/* ======================================================================== */
/* Response Parsing                                                         */
/* ======================================================================== */

static inline int http_response_parse(http_res_t *res, uint8_t *buf, size_t len, size_t *consumed) {
    if (!res || !buf || !consumed || len == 0) {
        errno = EINVAL;
        return -1;
    }

    char *headers_end = strstr((char *)buf, "\r\n\r\n");
    if (!headers_end) return -1;

    char *sp = strchr((char *)buf, ' ');
    if (!sp) return -1;
    res->code = (http_code_t)atoi(sp + 1);

    char *rp = strchr(sp + 1, ' ');
    if (rp) {
        char *eol = strchr(rp, '\r');
        if (eol) {
            *eol = '\0';
            res->reason = rp + 1;
        }
    }

    *consumed = (size_t)((headers_end + 4) - (char *)buf);
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* _TRANSIT_CLIENT_H */
