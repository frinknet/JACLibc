/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _TRANSIT_SERVE_H
#define _TRANSIT_SERVE_H
#pragma once

#include <config.h>
#include <transit/http.h>
#include <transit/conn.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Types                                                                    */
/* ======================================================================== */

typedef struct {
    bool keep_alive;
    int timeout_ms;
    int max_requests;
    size_t max_header_size;
    size_t max_body_size;
} serve_config_t;

typedef struct serve_conn {
    conn_t *conn;
    http_req_t req;
    http_res_t res;
    serve_config_t config;
    uint8_t *buf;
    size_t buf_size;
    size_t buf_used;
    void *parser_ctx;
    void (*handler)(struct serve_conn *sc);
    bool active;
    int request_count;
} serve_conn_t;

typedef void (*serve_handler_t)(serve_conn_t *sc);

/* ======================================================================== */
/* Advanced Event Loop (Defined First - Dependencies)                       */
/* ======================================================================== */

static inline serve_conn_t *serve_accept(conn_t *listen_c, serve_handler_t handler,
                                         uint8_t *buf, size_t buf_size) {
    if (!listen_c || !handler || !buf || buf_size < HTTP_HDR_BUFSZ) {
        errno = EINVAL;
        return NULL;
    }
    
    conn_t *client_c = conn_accept(listen_c);
    if (!client_c) return NULL;
    
    serve_conn_t *sc = (serve_conn_t *)malloc(sizeof(serve_conn_t));
    if (!sc) {
        conn_close(client_c);
        return NULL;
    }
    
    memset(sc, 0, sizeof(serve_conn_t));
    sc->conn = client_c;
    sc->handler = handler;
    sc->buf = buf;
    sc->buf_size = buf_size;
    sc->buf_used = 0;
    sc->active = true;
    sc->request_count = 0;
    
    sc->config.keep_alive = true;
    sc->config.timeout_ms = 5000;
    sc->config.max_requests = 100;
    sc->config.max_header_size = HTTP_HDR_MAX;
    sc->config.max_body_size = 65536;
    
    http_init(&sc->req, buf, buf_size);
    
    return sc;
}

static inline int serve_step(serve_conn_t *sc) {
    if (!sc || !sc->active || !sc->conn) {
        errno = EINVAL;
        return -1;
    }
    
    if (sc->buf_used < sc->buf_size) {
        ssize_t n = conn_read(sc->conn, sc->buf + sc->buf_used, sc->buf_size - sc->buf_used);
        if (n < 0) {
            sc->active = false;
            return -1;
        }
        if (n == 0) {
            if (sc->req.hcount == 0) {
                sc->active = false;
                return -1;
            }
        }
        sc->buf_used += (size_t)n;
    }
    
    if (sc->req.hcount == 0) {
        size_t consumed = 0;
        http_stat_t st = http_feed(&sc->req, sc->buf, sc->buf_used, &consumed);
        
        if (st == HSTAT_MORE) return 1;
        if (st == HSTAT_ERROR) {
            http_res_init(&sc->res, HTTP_ERROR, "Bad Request");
            goto send_response;
        }
        if (st == HSTAT_UPGRADE) {
            sc->handler(sc);
            return 0;
        }
        if (st != HSTAT_HEADERS) {
            sc->active = false;
            return -1;
        }
        
        if (consumed < sc->buf_used) {
            memmove(sc->buf, sc->buf + consumed, sc->buf_used - consumed);
            sc->buf_used -= consumed;
        } else {
            sc->buf_used = 0;
        }
        
        http_res_init(&sc->res, HTTP_OK, "OK");
        sc->handler(sc);
        
    send_response:
        uint8_t out[4096];
        size_t len = http_res_build(out, sizeof(out), &sc->res);
        if (len > 0) {
            conn_write(sc->conn, out, len);
            if (sc->res.file_fd >= 0) {
                char file_buf[8192];
                ssize_t n;
                while ((n = read(sc->res.file_fd, file_buf, sizeof(file_buf))) > 0) {
                    conn_write(sc->conn, file_buf, (size_t)n);
                }
                close(sc->res.file_fd);
                sc->res.file_fd = -1;
            }
        }
        
        sc->request_count++;
        
        if (!sc->req.keep_alive || sc->request_count >= sc->config.max_requests) {
            sc->active = false;
            return -1;
        }
        
        http_reset(&sc->req);
        return 0;
    }
    
    return 0;
}

static inline void serve_free(serve_conn_t *sc) {
    if (!sc) return;
    if (sc->conn) conn_close(sc->conn);
    free(sc);
}

static inline void serve_upgrade(serve_conn_t *sc, void *new_parser_ctx, serve_handler_t new_handler) {
    if (!sc || !new_handler) return;
    sc->parser_ctx = new_parser_ctx;
    sc->handler = new_handler;
}

static inline void serve_set_config(serve_conn_t *sc, serve_config_t config) {
    if (sc) sc->config = config;
}

static inline void serve_close(serve_conn_t *sc) {
    if (!sc) return;
    sc->active = false;
    if (sc->conn) conn_close(sc->conn);
    sc->conn = NULL;
}

/* ======================================================================== */
/* Simple Blocking Server (Defined Last - Uses Helpers Above)               */
/* ======================================================================== */

static inline void serve_loop(conn_t *listen_c, serve_handler_t handler) {
    if (!listen_c || !handler) return;
    uint8_t buf[HTTP_HDR_BUFSZ];
    
    while (1) {
        serve_conn_t *sc = serve_accept(listen_c, handler, buf, sizeof(buf));
        if (!sc) continue;
        while (sc->active) {
            int r = serve_step(sc);
            if (r < 0) break;
            if (r == 0 && !sc->req.keep_alive) break;
        }
        serve_free(sc);
    }
}

static inline void serve_loop_config(conn_t *listen_c, serve_handler_t handler, serve_config_t config) {
    if (!listen_c || !handler) return;
    uint8_t buf[HTTP_HDR_BUFSZ];
    
    while (1) {
        serve_conn_t *sc = serve_accept(listen_c, handler, buf, sizeof(buf));
        if (!sc) continue;
        sc->config = config;
        while (sc->active) {
            int r = serve_step(sc);
            if (r < 0) break;
            if (r == 0 && !sc->req.keep_alive) break;
        }
        serve_free(sc);
    }
}

#ifdef __cplusplus
}
#endif

#endif /* _TRANSIT_SERVE_H */
