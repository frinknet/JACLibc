/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _TRANSIT_SSE_H
#define _TRANSIT_SSE_H
#pragma once

/**
 * Server-Sent Events (SSE) - Server Push Only
 *
 * DESIGN PHILOSOPHY:
 * - Upgrade from HTTP (one-way server→client)
 * - Text-based event stream (event:, data:, id:, retry:)
 * - Long-lived connection with auto-reconnect
 * - Header-only, integrates with proto.h / serve.h
 */

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <transit/conn.h>
#include <transit/proto.h>
#include <transit/serve.h>
#include <transit/http.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* SSE Event Structure                                                      */
/* ======================================================================== */

typedef struct {
    const char *event;      /* event: (optional) */
    const char *data;       /* data: (required) */
    const char *id;         /* id: (optional) */
    int retry_ms;           /* retry: (optional) */
} sse_event_t;

/* SSE Session State */
typedef struct {
    http_req_t req;
    bool headers_sent;
    size_t event_count;
    time_t last_ping;
    int ping_interval_ms;
} sse_state_t;

/* ======================================================================== */
/* SSE Event Writer                                                         */
/* ======================================================================== */

static inline size_t sse_event_build(uint8_t *buf, size_t buf_size, const sse_event_t *ev) {
    if (!buf || !ev || !ev->data) return 0;
    
    char *p = (char *)buf;
    size_t rem = buf_size;
    int n;
    
    /* event: (optional) */
    if (ev->event) {
        n = snprintf(p, rem, "event: %s\r\n", ev->event);
        if (n < 0 || (size_t)n >= rem) return 0;
        p += n; rem -= n;
    }
    
    /* id: (optional) */
    if (ev->id) {
        n = snprintf(p, rem, "id: %s\r\n", ev->id);
        if (n < 0 || (size_t)n >= rem) return 0;
        p += n; rem -= n;
    }
    
    /* retry: (optional) */
    if (ev->retry_ms > 0) {
        n = snprintf(p, rem, "retry: %d\r\n", ev->retry_ms);
        if (n < 0 || (size_t)n >= rem) return 0;
        p += n; rem -= n;
    }
    
    /* data: (required, may be multi-line) */
    const char *d = ev->data;
    while (*d) {
        const char *nl = strchr(d, '\n');
        size_t len = nl ? (size_t)(nl - d) : strlen(d);
        n = snprintf(p, rem, "data: %.*s\r\n", (int)len, d);
        if (n < 0 || (size_t)n >= rem) return 0;
        p += n; rem -= n;
        if (!nl) break;
        d = nl + 1;
    }
    
    /* Blank line to end event */
    if (rem < 2) return 0;
    memcpy(p, "\r\n", 2);
    p += 2;
    
    return (size_t)(p - (char *)buf);
}

static inline int sse_send(serve_conn_t sc, const sse_event_t *ev) {
    if (!sc || !ev || !ev->data) return SERVE_ERROR;
    
    uint8_t buf[4096];
    size_t len = sse_event_build(buf, sizeof(buf), ev);
    if (len == 0) return SERVE_ERROR;
    
    if (conn_write(sc->conn, buf, len) < 0) {
        return SERVE_CLOSE;
    }
    
    sse_state_t *state = (sse_state_t *)sc->data;
    if (state) state->event_count++;
    
    return SERVE_OK;
}

static inline int sse_ping(serve_conn_t sc) {
    if (!sc) return SERVE_ERROR;
    
    /* SSE ping is just a comment line */
    const char *ping = ": ping\r\n\r\n";
    if (conn_write(sc->conn, ping, strlen(ping)) < 0) {
        return SERVE_CLOSE;
    }
    
    sse_state_t *state = (sse_state_t *)sc->data;
    if (state) state->last_ping = time(NULL);
    
    return SERVE_OK;
}

/* ======================================================================== */
/* Protocol Handlers                                                        */
/* ======================================================================== */

static int sse_serve_open(serve_conn_t sc) {
    if (!sc) return SERVE_ERROR;
    
    sse_state_t *state = (sse_state_t *)calloc(1, sizeof(sse_state_t));
    if (!state) return SERVE_ERROR;
    
    http_init(&state->req, sc->buf, sc->buf_size);
    state->headers_sent = false;
    state->event_count = 0;
    state->last_ping = time(NULL);
    state->ping_interval_ms = 30000;  /* 30s default */
    
    sc->data = state;
    return SERVE_OK;
}

static int sse_serve_in(serve_conn_t sc) {
    if (!sc || !sc->data) return SERVE_ERROR;
    
    /* SSE is server-push only. Client sends nothing after upgrade. */
    /* Just check for disconnect */
    sse_state_t *state = (sse_state_t *)sc->data;
    
    /* Auto-ping if idle */
    time_t now = time(NULL);
    if (difftime(now, state->last_ping) * 1000 >= state->ping_interval_ms) {
        sse_ping(sc);
    }
    
    return SERVE_OK;  /* Keep connection open */
}

static int sse_serve_out(serve_conn_t sc) {
    if (!sc || !sc->data) return SERVE_ERROR;
    
    sse_state_t *state = (sse_state_t *)sc->data;
    
    /* Send SSE headers on first call */
    if (!state->headers_sent) {
        const char *headers = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/event-stream\r\n"
            "Cache-Control: no-cache\r\n"
            "Connection: keep-alive\r\n"
            "\r\n";
        
        if (conn_write(sc->conn, headers, strlen(headers)) < 0) {
            return SERVE_CLOSE;
        }
        
        state->headers_sent = true;
    }
    
    return SERVE_OK;  /* Keep connection open for more events */
}

static int sse_serve_close(serve_conn_t sc) {
    if (!sc || !sc->data) return SERVE_OK;
    
    free(sc->data);
    sc->data = NULL;
    return SERVE_OK;
}

/* ======================================================================== */
/* Protocol Definition                                                      */
/* ======================================================================== */

static const proto_t _sse_proto = {
    .name = "sse",
    .def = proto_sse_def,
    .from = proto_http_def,  /* Upgrades from HTTP */
    .serve_open = sse_serve_open,
    .serve_in = sse_serve_in,
    .serve_out = sse_serve_out,
    .serve_close = sse_serve_close,
    .persist = true,
    .data = NULL
};

static inline const proto_t proto_sse_def(void) {
    return &_sse_proto;
}

/* ======================================================================== */
/* HTTP → SSE Upgrade Helper                                                */
/* ======================================================================== */

static inline int http_upgrade_to_sse(serve_conn_t sc) {
    if (!sc) return SERVE_ERROR;
    return serve_upgrade(sc, proto_sse_def);
}

#ifdef __cplusplus
}
#endif
#endif /* _TRANSIT_SSE_H */
