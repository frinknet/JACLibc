/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _TRANSIT_SERVE_H
#define _TRANSIT_SERVE_H
#pragma once

#include <config.h>
#include <transit/http.h>
#include <transit/conn.h>
#include <aio.h>
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
    
    /* AIO state – embedded, no heap alloc per op */
    struct aiocb __aio_cb;
    enum { __AIO_IDLE = 0, __AIO_READ, __AIO_WRITE } __aio_state;
    uint8_t *__write_ptr;
    size_t __write_remaining;
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
    sc->__aio_state = __AIO_IDLE;
    sc->__write_ptr = NULL;
    sc->__write_remaining = 0;
    
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
    
    /* ==================== PHASE 1: REAP COMPLETIONS ==================== */
    __jacl_aio_reap_cq();
    
    /* Check read completion */
    if (sc->__aio_state == __AIO_READ) {
        int err = aio_error(&sc->__aio_cb);
        if (err != EINPROGRESS) {
            ssize_t n = aio_return(&sc->__aio_cb);
            sc->__aio_state = __AIO_IDLE;
            
            if (n > 0) {
                sc->buf_used += (size_t)n;
            } else if (n == 0) {
                /* EOF */
                if (sc->req.hcount == 0) {
                    sc->active = false;
                    return -1;
                }
            } else {
                /* Error */
                sc->active = false;
                return -1;
            }
        }
    }
    
    /* Check write completion */
    if (sc->__aio_state == __AIO_WRITE && sc->__write_ptr) {
        int err = aio_error(&sc->__aio_cb);
        if (err != EINPROGRESS) {
            ssize_t n = aio_return(&sc->__aio_cb);
            sc->__aio_state = __AIO_IDLE;
            
            if (n > 0 && (size_t)n < sc->__write_remaining) {
                /* Partial write – continue */
                sc->__write_ptr += (size_t)n;
                sc->__write_remaining -= (size_t)n;
            } else {
                /* Complete or error */
                sc->__write_ptr = NULL;
                sc->__write_remaining = 0;
                if (n < 0) {
                    sc->active = false;
                    return -1;
                }
            }
        }
    }
    
    /* ==================== PHASE 2: SUBMIT NEW OPS ==================== */
    
    /* Submit read if idle and buffer has space */
    if (sc->__aio_state == __AIO_IDLE && sc->buf_used < sc->buf_size && !sc->__write_ptr) {
        sc->__aio_cb = (struct aiocb){
            .aio_fildes = sc->conn->fd,
            .aio_buf = sc->buf + sc->buf_used,
            .aio_nbytes = sc->buf_size - sc->buf_used,
            .aio_offset = 0,
            .aio_sigevent.sigev_notify = SIGEV_NONE
        };
        if (aio_read(&sc->__aio_cb) == 0) {
            sc->__aio_state = __AIO_READ;
        } else {
            /* Fallback to blocking read */
            ssize_t n = read(sc->conn->fd, sc->buf + sc->buf_used, sc->buf_size - sc->buf_used);
            if (n > 0) sc->buf_used += (size_t)n;
            else if (n == 0 && sc->req.hcount == 0) { sc->active = false; return -1; }
            else if (n < 0) { sc->active = false; return -1; }
        }
    }
    
    /* Submit write if we have pending data */
    if (sc->__aio_state == __AIO_IDLE && sc->__write_ptr && sc->__write_remaining > 0) {
        sc->__aio_cb = (struct aiocb){
            .aio_fildes = sc->conn->fd,
            .aio_buf = sc->__write_ptr,
            .aio_nbytes = sc->__write_remaining,
            .aio_offset = 0,
            .aio_sigevent.sigev_notify = SIGEV_NONE
        };
        if (aio_write(&sc->__aio_cb) != 0) {
            /* Fallback to blocking write */
            ssize_t n = write(sc->conn->fd, sc->__write_ptr, sc->__write_remaining);
            if (n > 0) {
                sc->__write_ptr += (size_t)n;
                sc->__write_remaining -= (size_t)n;
            } else {
                sc->active = false;
                return -1;
            }
            sc->__write_ptr = NULL;
            sc->__write_remaining = 0;
        }
    }
    
    /* ==================== PHASE 3: PARSER DISPATCH ==================== */
    if (sc->req.hcount == 0 && sc->buf_used > 0) {
        size_t consumed = 0;
        http_stat_t st = http_feed(&sc->req, sc->buf, sc->buf_used, &consumed);
        
        if (st == HSTAT_MORE) return 1;  /* Need more data */
        
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
        
        /* Shift buffer */
        if (consumed < sc->buf_used) {
            memmove(sc->buf, sc->buf + consumed, sc->buf_used - consumed);
            sc->buf_used -= consumed;
        } else {
            sc->buf_used = 0;
        }
        
        http_res_init(&sc->res, HTTP_OK, "OK");
        sc->handler(sc);
        
    send_response:
        /* Build response into temp buffer */
        uint8_t out[4096];
        size_t len = http_res_build(out, sizeof(out), &sc->res);
        
        if (len > 0) {
            /* Queue write */
            sc->__write_ptr = out;
            sc->__write_remaining = len;
            
            /* Immediate flush if idle */
            if (sc->__aio_state == __AIO_IDLE) {
                ssize_t n = write(sc->conn->fd, sc->__write_ptr, sc->__write_remaining);
                if (n > 0) {
                    sc->__write_ptr += (size_t)n;
                    sc->__write_remaining -= (size_t)n;
                } else if (n < 0) {
                    sc->active = false;
                    return -1;
                }
                if (sc->__write_remaining == 0) sc->__write_ptr = NULL;
            }
        }
        
        /* Handle file sends */
        if (sc->res.file_fd >= 0) {
            char file_buf[8192];
            ssize_t n;
            while ((n = read(sc->res.file_fd, file_buf, sizeof(file_buf))) > 0) {
                if (write(sc->conn->fd, file_buf, (size_t)n) < 0) {
                    close(sc->res.file_fd);
                    sc->active = false;
                    return -1;
                }
            }
            close(sc->res.file_fd);
            sc->res.file_fd = -1;
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
