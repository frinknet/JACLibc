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
 */

#include <config.h>
#include <transit/conn.h>
#include <transit/proto.h>
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

/**
 * Client session wrapper
 *
 * MEMORY OWNERSHIP:
 * - client_conn_create(): Allocates session + buffer
 * - client_conn_free(): Frees session + buffer + closes conn
 * - User NEVER calls free() on client_conn_t directly
 *
 * BUFFER: Owned by session (not conn_t). Protocol decides how to parse.
 * - buf, buf_size, buf_used: Read buffer
 * - buf_consumed: Bytes consumed by proto handler (for shifting)
 *
 * STATE: Bidirectional tracking (read/write independent)
 * - read_eof, read_blocked: Read side state
 * - write_blocked: Write side state
 *
 * ERROR: Per-session (not per-FD) for scalability
 * - last_error, error_op: Last error details
 */
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
    
    /* Read/Write state (bidirectional) */
    bool read_eof;
    bool read_blocked;
    bool write_blocked;
    
    /* Session state */
    bool active;
    int hops;                     /* Upgrade hop counter */
    int request_count;            /* For max_requests limit */
    int last_error;
    char error_op[16];
    
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
    cc->read_blocked = false;
    cc->write_blocked = false;
    
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
    
    /* ========== WRITE SIDE (Client usually writes first) ========== */
    if (!cc->write_blocked && cc->write_used > cc->write_sent) {
        ssize_t n = conn_write(cc->conn, 
                               cc->write_buf + cc->write_sent, 
                               cc->write_used - cc->write_sent);
        
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                cc->write_blocked = true;
                return CLIENT_OK;
            }
            cc->last_error = errno;
            strncpy(cc->error_op, "write", sizeof(cc->error_op) - 1);
            return CLIENT_CLOSE;
        }
        
        cc->write_sent += (size_t)n;
        cc->write_blocked = false;
        
        /* Write complete? */
        if (cc->write_sent >= cc->write_used) {
            cc->write_used = 0;
            cc->write_sent = 0;
        }
    }
    
    /* ========== READ SIDE ========== */
    if (!cc->read_eof && !cc->read_blocked && cc->buf_used < cc->buf_size) {
        ssize_t n = conn_read(cc->conn, cc->buf + cc->buf_used, 
                              cc->buf_size - cc->buf_used);
        
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                cc->read_blocked = true;
                return CLIENT_OK;
            }
            cc->last_error = errno;
            strncpy(cc->error_op, "read", sizeof(cc->error_op) - 1);
            return CLIENT_CLOSE;
        }
        
        if (n == 0) {
            cc->read_eof = true;  /* EOF */
        } else {
            cc->buf_used += (size_t)n;
            cc->read_blocked = false;
        }
    }
    
    /* ========== PROTOCOL INBOUND ========== */
    if (cc->proto->client_in && cc->buf_used > cc->buf_consumed) {
        int r = cc->proto->client_in(cc);
        
        if (r == CLIENT_UPGRADE) {
            return CLIENT_UPGRADE;
        }
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
    
    /* ========== PROTOCOL OUTBOUND ========== */
    if (cc->proto->client_out && !cc->write_blocked) {
        int r = cc->proto->client_out(cc);
        
        if (r == CLIENT_CLOSE || r == CLIENT_ERROR) {
            cc->last_error = -r;
            strncpy(cc->error_op, "proto_out", sizeof(cc->error_op) - 1);
            return r;
        }
        
        if (r == CLIENT_UPGRADE) {
            return CLIENT_UPGRADE;
        }
    }
    
    /* ========== EOF HANDLING ========== */
    if (cc->read_eof && cc->buf_used == 0 && cc->write_used == 0) {
        return CLIENT_CLOSE;  /* Done */
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
