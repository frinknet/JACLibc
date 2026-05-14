/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _TRANSIT_CLIENT_H
#define _TRANSIT_CLIENT_H
#pragma once

/**
 * Client Session Layer (Protocol-Agnostic, Header-Only)
 *
 * DESIGN PHILOSOPHY:
 * - Mirror of serve.h for outbound connections
 * - Buffer owned by session, not transport
 * - Proto handlers drive lifecycle (client_open, client_in, client_out, client_close)
 * - Return codes control flow (OK, DONE, CLOSE, ERROR, UPGRADE)
 * - Bidirectional error tracking (read/write independent)
 * - Zero HTTP assumptions - works for SMTP, WebSocket, MQTT, etc.
 * - ASYNC RETROFIT: Driven by AIO completions, not blocking syscalls
 */

#include <config.h>
#include <transit/conn.h>
#include <transit/proto.h>
#include <aio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Configuration                                                            */
/* ======================================================================== */

#define CLIENT_BUF_SIZE     8192
#define CLIENT_MAX_HOPS     16
#define CLIENT_MAX_REQUESTS 1000

/* ======================================================================== */
/* Return Codes (Proto handlers return these)                               */
/* ======================================================================== */

#define CLIENT_OK        0   /* Continue, more data expected */
#define CLIENT_DONE      1   /* Message handled, ready for next */
#define CLIENT_UPGRADE   2   /* Protocol switch requested */
#define CLIENT_CLOSE    -1   /* Close connection */
#define CLIENT_ERROR    -2   /* Error occurred */

/* ======================================================================== */
/* Client Connection (Per-Session State)                                    */
/* ======================================================================== */

typedef struct client_conn {
    conn_t *conn;                 /* Transport (from conn.h) */
    const proto_t *proto;         /* Current protocol (from proto.h) */
    
    /* Buffer (session-owned) */
    uint8_t *buf;
    size_t buf_size;
    size_t buf_used;
    size_t buf_consumed;          /* Bytes consumed by proto handler */
    
    /* Write buffer (for outbound data) */
    uint8_t *write_buf;
    size_t write_size;
    size_t write_used;
    size_t write_sent;
    
    /* Session state */
    bool active;
    int hops;                     /* Upgrade hop counter */
    int request_count;            /* For max_requests limit */
    int last_error;
    char error_op[16];
    bool read_eof;
    
    /* AIO state – embedded, no heap alloc per op */
    struct aiocb __aio_cb;
    enum { __AIO_IDLE = 0, __AIO_READ, __AIO_WRITE } __aio_state;
    size_t __write_remaining;     /* Track remaining bytes for current write op */
    
    /* User data */
    void *data;
} client_conn_t;

/* ======================================================================== */
/* Client                                                                   */
/* ======================================================================== */

typedef struct client {
    proto_list_t protos;
    void *data;                   /* Shared across all connections */
} client_t;

/* ======================================================================== */
/* Client Lifecycle                                                         */
/* ======================================================================== */

static inline client_t *client_create(proto_list_t protos) {
    if (!protos) return NULL;
    
    client_t *c = (client_t *)calloc(1, sizeof(client_t));
    if (!c) return NULL;
    
    c->protos = protos;
    c->data = NULL;
    
    return c;
}

static inline void client_free(client_t *c) {
    if (!c) return;
    free(c);
}

/* ======================================================================== */
/* Connection Lifecycle                                                     */
/* ======================================================================== */

static inline client_conn_t *client_conn_create(conn_t *c, client_t *client) {
    if (!c || !client) return NULL;
    
    client_conn_t *cc = (client_conn_t *)calloc(1, sizeof(client_conn_t));
    if (!cc) return NULL;
    
    cc->conn = c;
    cc->proto = proto_main(client->protos);
    cc->buf = (uint8_t *)malloc(CLIENT_BUF_SIZE);
    cc->buf_size = cc->buf ? CLIENT_BUF_SIZE : 0;
    cc->buf_used = 0;
    cc->buf_consumed = 0;
    cc->write_buf = (uint8_t *)malloc(CLIENT_BUF_SIZE);
    cc->write_size = cc->write_buf ? CLIENT_BUF_SIZE : 0;
    cc->write_used = 0;
    cc->write_sent = 0;
    cc->active = true;
    cc->hops = 0;
    cc->request_count = 0;
    cc->last_error = 0;
    cc->read_eof = false;
    cc->__aio_state = __AIO_IDLE;
    cc->__write_remaining = 0;
    
    /* Call protocol open hook */
    if (cc->proto && cc->proto->client_open) {
        int r = cc->proto->client_open(cc);
        if (r == CLIENT_CLOSE || r == CLIENT_ERROR) {
            client_conn_free(cc);
            return NULL;
        }
    }
    
    return cc;
}

static inline void client_conn_free(client_conn_t *cc) {
    if (!cc) return;
    
    /* Call protocol close hook */
    if (cc->proto && cc->proto->client_close) {
        cc->proto->client_close(cc);
    }
    
    if (cc->conn) conn_close(cc->conn);
    if (cc->buf) free(cc->buf);
    if (cc->write_buf) free(cc->write_buf);
    free(cc);
}

/* ======================================================================== */
/* Upgrade (Protocol Switch)                                                */
/* ======================================================================== */

static inline int client_upgrade(client_conn_t *cc, proto_def next_fn) {
    if (!cc || !next_fn || !cc->proto) return CLIENT_ERROR;
    if (cc->hops++ > CLIENT_MAX_HOPS) return CLIENT_ERROR;
    
    /* Get new proto */
    const proto_t *next = next_fn();
    if (!next) return CLIENT_ERROR;
    
    /* Verify upgrade path exists */
    proto_list_t list = proto_list();
    const proto_t *valid = proto_upgrade(list, (proto_t)cc->proto, next->name);
    if (!valid) return CLIENT_ERROR;
    
    /* Close old protocol */
    if (cc->proto->client_close) {
        cc->proto->client_close(cc);
    }
    
    /* Switch to new protocol */
    cc->proto = next;
    
    /* Open new protocol */
    if (cc->proto->client_open) {
        int r = cc->proto->client_open(cc);
        if (r == CLIENT_CLOSE || r == CLIENT_ERROR) return CLIENT_ERROR;
    }
    
    return CLIENT_UPGRADE;
}

/* ======================================================================== */
/* Connection Step (Process One Read/Write Cycle)                           */
/* ======================================================================== */

static inline int client_step(client_conn_t *cc) {
    if (!cc || !cc->active || !cc->conn || !cc->proto) {
        return CLIENT_ERROR;
    }
    
    /* ==================== PHASE 1: REAP COMPLETIONS ==================== */
    __jacl_aio_reap_cq();
    
    /* Check read completion */
    if (cc->__aio_state == __AIO_READ) {
        int err = aio_error(&cc->__aio_cb);
        if (err != EINPROGRESS) {
            ssize_t n = aio_return(&cc->__aio_cb);
            cc->__aio_state = __AIO_IDLE;
            
            if (n > 0) {
                cc->buf_used += (size_t)n;
            } else if (n == 0) {
                cc->read_eof = true;
            } else {
                cc->last_error = errno;
                strncpy(cc->error_op, "aio_read", sizeof(cc->error_op) - 1);
                return CLIENT_CLOSE;
            }
        }
    }
    
    /* Check write completion */
    if (cc->__aio_state == __AIO_WRITE) {
        int err = aio_error(&cc->__aio_cb);
        if (err != EINPROGRESS) {
            ssize_t n = aio_return(&cc->__aio_cb);
            cc->__aio_state = __AIO_IDLE;
            
            if (n > 0 && (size_t)n < cc->__write_remaining) {
                /* Partial write – advance cursor */
                cc->write_sent += (size_t)n;
                cc->__write_remaining -= (size_t)n;
            } else {
                /* Complete or error */
                if (n > 0) cc->write_sent += (size_t)n;
                
                if (cc->write_sent >= cc->write_used) {
                    cc->write_used = 0;
                    cc->write_sent = 0;
                }
                cc->__write_remaining = 0;
                
                if (n < 0) {
                    cc->last_error = errno;
                    strncpy(cc->error_op, "aio_write", sizeof(cc->error_op) - 1);
                    return CLIENT_CLOSE;
                }
            }
        }
    }
    
    /* ==================== PHASE 2: SUBMIT NEW OPS ==================== */
    
    /* Submit write first (client usually speaks first) */
    if (cc->__aio_state == __AIO_IDLE && cc->write_used > cc->write_sent) {
        size_t len = cc->write_used - cc->write_sent;
        cc->__aio_cb = (struct aiocb){
            .aio_fildes = cc->conn->fd,
            .aio_buf = cc->write_buf + cc->write_sent,
            .aio_nbytes = len,
            .aio_offset = 0,
            .aio_sigevent.sigev_notify = SIGEV_NONE
        };
        cc->__write_remaining = len;
        
        if (aio_write(&cc->__aio_cb) != 0) {
            /* Fallback to blocking write */
            ssize_t n = write(cc->conn->fd, cc->write_buf + cc->write_sent, len);
            if (n > 0) {
                cc->write_sent += (size_t)n;
                if (cc->write_sent >= cc->write_used) {
                    cc->write_used = 0;
                    cc->write_sent = 0;
                }
            } else {
                cc->last_error = errno;
                strncpy(cc->error_op, "write", sizeof(cc->error_op) - 1);
                return CLIENT_CLOSE;
            }
        } else {
            cc->__aio_state = __AIO_WRITE;
        }
    }
    
    /* Submit read if idle and buffer has space */
    if (cc->__aio_state == __AIO_IDLE && !cc->read_eof && cc->buf_used < cc->buf_size) {
        cc->__aio_cb = (struct aiocb){
            .aio_fildes = cc->conn->fd,
            .aio_buf = cc->buf + cc->buf_used,
            .aio_nbytes = cc->buf_size - cc->buf_used,
            .aio_offset = 0,
            .aio_sigevent.sigev_notify = SIGEV_NONE
        };
        if (aio_read(&cc->__aio_cb) != 0) {
            /* Fallback to blocking read */
            ssize_t n = read(cc->conn->fd, cc->buf + cc->buf_used, cc->buf_size - cc->buf_used);
            if (n > 0) {
                cc->buf_used += (size_t)n;
            } else if (n == 0) {
                cc->read_eof = true;
            } else {
                cc->last_error = errno;
                strncpy(cc->error_op, "read", sizeof(cc->error_op) - 1);
                return CLIENT_CLOSE;
            }
        } else {
            cc->__aio_state = __AIO_READ;
        }
    }
    
    /* ==================== PHASE 3: PROTOCOL DISPATCH ==================== */
    
    /* Inbound */
    if (cc->proto->client_in && cc->buf_used > cc->buf_consumed) {
        int r = cc->proto->client_in(cc);
        
        if (r == CLIENT_UPGRADE) return CLIENT_UPGRADE;
        if (r == CLIENT_CLOSE || r == CLIENT_ERROR) {
            cc->last_error = -r;
            strncpy(cc->error_op, "proto_in", sizeof(cc->error_op) - 1);
            return r;
        }
        
        /* Shift buffer if proto consumed bytes */
        if (cc->buf_consumed > 0 && cc->buf_consumed < cc->buf_used) {
            memmove(cc->buf, cc->buf + cc->buf_consumed, 
                    cc->buf_used - cc->buf_consumed);
            cc->buf_used -= cc->buf_consumed;
            cc->buf_consumed = 0;
        } else if (cc->buf_consumed >= cc->buf_used) {
            cc->buf_used = 0;
            cc->buf_consumed = 0;
        }
        
        if (r == CLIENT_DONE) {
            cc->request_count++;
            if (cc->request_count >= CLIENT_MAX_REQUESTS) {
                return CLIENT_CLOSE;
            }
        }
    }
    
    /* Outbound */
    if (cc->proto->client_out && cc->__aio_state == __AIO_IDLE) {
        int r = cc->proto->client_out(cc);
        
        if (r == CLIENT_CLOSE || r == CLIENT_ERROR) {
            cc->last_error = -r;
            strncpy(cc->error_op, "proto_out", sizeof(cc->error_op) - 1);
            return r;
        }
        
        if (r == CLIENT_UPGRADE) return CLIENT_UPGRADE;
    }
    
    /* EOF Handling */
    if (cc->read_eof && cc->buf_used == 0 && cc->write_used == 0 && cc->__aio_state == __AIO_IDLE) {
        return CLIENT_CLOSE;
    }
    
    return CLIENT_OK;
}

/* ======================================================================== */
/* Client Connect & Run (Blocking)                                          */
/* ======================================================================== */

static inline client_conn_t *client_connect(client_t *c, const char *host, const char *port) {
    if (!c || !host || !port) return NULL;
    
    /* Create TCP connection */
    conn_t *conn = conn_create(AF_INET, SOCK_STREAM, 0);
    if (!conn) return NULL;
    
    if (conn_connect(conn, host, port) < 0) {
        conn_close(conn);
        return NULL;
    }
    
    /* Wrap in client session */
    client_conn_t *cc = client_conn_create(conn, c);
    if (!cc) {
        conn_close(conn);
        return NULL;
    }
    
    return cc;
}

static inline int client_run(client_conn_t *cc) {
    if (!cc) return CLIENT_ERROR;
    
    while (cc->active) {
        int r = client_step(cc);
        if (r == CLIENT_CLOSE || r == CLIENT_ERROR) {
            break;
        }
        /* CLIENT_UPGRADE continues loop with new proto */
    }
    
    return (cc->last_error == 0) ? CLIENT_OK : CLIENT_ERROR;
}

static inline void client_close(client_conn_t *cc) {
    if (cc) cc->active = false;
}

/* ======================================================================== */
/* Write Helper (Queue data for outbound)                                   */
/* ======================================================================== */

static inline int client_write(client_conn_t *cc, const uint8_t *data, size_t len) {
    if (!cc || !data || !len) return CLIENT_ERROR;
    
    /* Check buffer space */
    if (cc->write_used + len > cc->write_size) {
        /* Resize write buffer */
        size_t new_size = cc->write_size * 2;
        while (new_size < cc->write_used + len) {
            new_size *= 2;
        }
        
        uint8_t *new_buf = (uint8_t *)realloc(cc->write_buf, new_size);
        if (!new_buf) return CLIENT_ERROR;
        
        cc->write_buf = new_buf;
        cc->write_size = new_size;
    }
    
    /* Queue data */
    memcpy(cc->write_buf + cc->write_used, data, len);
    cc->write_used += len;
    
    return CLIENT_OK;
}

/* ======================================================================== */
/* Accessors                                                                */
/* ======================================================================== */

static inline conn_t *client_conn_conn(client_conn_t *cc) 
    { return cc ? cc->conn : NULL; }

static inline const proto_t *client_conn_proto(client_conn_t *cc) 
    { return cc ? cc->proto : NULL; }

static inline void *client_conn_data(client_conn_t *cc) 
    { return cc ? cc->data : NULL; }

static inline void client_conn_set_data(client_conn_t *cc, void *data) 
    { if (cc) cc->data = data; }

static inline void *client_data(client_t *c) 
    { return c ? c->data : NULL; }

static inline void client_set_data(client_t *c, void *data) 
    { if (c) c->data = data; }

static inline proto_list_t client_protos(client_t *c) 
    { return c ? c->protos : NULL; }

#ifdef __cplusplus
}
#endif
#endif /* _TRANSIT_CLIENT_H */
