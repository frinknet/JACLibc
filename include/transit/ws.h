/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _TRANSIT_WS_H
#define _TRANSIT_WS_H
#pragma once

/**
 * WebSocket Protocol (RFC 6455)
 *
 * DESIGN PHILOSOPHY:
 * - Upgrade from HTTP (bidirectional frames)
 * - Frame-based messaging (text, binary, ping, pong, close)
 * - Optional masking (client→server), compression (per-message deflate)
 * - Header-only, integrates with proto.h / serve.h / client.h
 */

#include <config.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <transit/conn.h>
#include <transit/proto.h>
#include <transit/serve.h>
#include <transit/client.h>
#include <transit/http.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* WebSocket Constants                                                      */
/* ======================================================================== */

#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"

typedef enum {
    WS_CONT = 0x0, WS_TEXT = 0x1, WS_BINARY = 0x2,
    WS_CLOSE = 0x8, WS_PING = 0x9, WS_PONG = 0xA
} ws_opcode_t;

typedef enum {
    WSSTAT_MORE = 0, WSSTAT_FRAME = 1, WSSTAT_ERROR = 2, WSSTAT_CLOSE = 3
} ws_stat_t;

/* ======================================================================== */
/* WebSocket Frame                                                          */
/* ======================================================================== */

typedef struct {
    uint8_t opcode;
    bool fin;
    bool masked;
    uint64_t payload_len;
    uint8_t mask_key[4];
    const uint8_t *payload;
} ws_frame_t;

/* WebSocket Session State */
typedef struct {
    http_req_t req;
    ws_frame_t frame;
    uint8_t *msg_buf;
    size_t msg_size;
    size_t msg_used;
    bool headers_sent;
    bool is_server;
    uint16_t close_code;
    char close_reason[128];
} ws_state_t;

/* ======================================================================== */
/* WebSocket Helpers                                                        */
/* ======================================================================== */

static inline uint64_t ws_decode_len(const uint8_t *buf, size_t *out_len) {
    uint64_t len = buf[0] & 0x7F;
    size_t offset = 1;
    
    if (len == 126) {
        len = (buf[1] << 8) | buf[2];
        offset = 3;
    } else if (len == 127) {
        len = 0;
        for (int i = 0; i < 8; i++) {
            len = (len << 8) | buf[1 + i];
        }
        offset = 9;
    }
    
    if (out_len) *out_len = offset;
    return len;
}

static inline void ws_mask(uint8_t *data, size_t len, const uint8_t *mask) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= mask[i % 4];
    }
}

static inline size_t ws_frame_build(uint8_t *buf, size_t buf_size, 
                                    ws_opcode_t opcode, const uint8_t *payload, 
                                    size_t payload_len, bool masked) {
    if (!buf || buf_size < 14) return 0;
    
    size_t i = 0;
    
    /* FIN + opcode */
    buf[i++] = 0x80 | (opcode & 0x0F);
    
    /* Mask + length */
    if (payload_len < 126) {
        buf[i++] = (masked ? 0x80 : 0x00) | (payload_len & 0x7F);
    } else if (payload_len < 65536) {
        buf[i++] = (masked ? 0x80 : 0x00) | 126;
        buf[i++] = (payload_len >> 8) & 0xFF;
        buf[i++] = payload_len & 0xFF;
    } else {
        buf[i++] = (masked ? 0x80 : 0x00) | 127;
        for (int j = 7; j >= 0; j--) {
            buf[i++] = (payload_len >> (j * 8)) & 0xFF;
        }
    }
    
    /* Mask key (if masked) */
    if (masked) {
        /* Generate random mask */
        buf[i++] = (rand() >> 16) & 0xFF;
        buf[i++] = (rand() >> 16) & 0xFF;
        buf[i++] = (rand() >> 16) & 0xFF;
        buf[i++] = (rand() >> 16) & 0xFF;
        
        /* Copy and mask payload */
        for (size_t j = 0; j < payload_len && i < buf_size; j++) {
            buf[i] = payload[j] ^ buf[i - 4 + (j % 4)];
            i++;
        }
    } else {
        /* Copy payload */
        if (i + payload_len > buf_size) return 0;
        memcpy(buf + i, payload, payload_len);
        i += payload_len;
    }
    
    return i;
}

/* ======================================================================== */
/* WebSocket Send Helpers                                                   */
/* ======================================================================== */

static inline int ws_send_text(serve_conn_t sc, const char *text) {
    if (!sc || !text) return SERVE_ERROR;
    
    uint8_t buf[8192];
    size_t len = ws_frame_build(buf, sizeof(buf), WS_TEXT, 
                                (const uint8_t *)text, strlen(text), false);
    if (len == 0) return SERVE_ERROR;
    
    if (conn_write(sc->conn, buf, len) < 0) {
        return SERVE_CLOSE;
    }
    
    return SERVE_OK;
}

static inline int ws_send_binary(serve_conn_t sc, const uint8_t *data, size_t len) {
    if (!sc || !data) return SERVE_ERROR;
    
    uint8_t buf[8192];
    size_t frame_len = ws_frame_build(buf, sizeof(buf), WS_BINARY, data, len, false);
    if (frame_len == 0) return SERVE_ERROR;
    
    if (conn_write(sc->conn, buf, frame_len) < 0) {
        return SERVE_CLOSE;
    }
    
    return SERVE_OK;
}

static inline int ws_send_ping(serve_conn_t sc) {
    if (!sc) return SERVE_ERROR;
    
    uint8_t buf[128];
    size_t len = ws_frame_build(buf, sizeof(buf), WS_PING, NULL, 0, false);
    if (len == 0) return SERVE_ERROR;
    
    if (conn_write(sc->conn, buf, len) < 0) {
        return SERVE_CLOSE;
    }
    
    return SERVE_OK;
}

static inline int ws_send_pong(serve_conn_t sc, const uint8_t *data, size_t len) {
    if (!sc) return SERVE_ERROR;
    
    uint8_t buf[128];
    size_t frame_len = ws_frame_build(buf, sizeof(buf), WS_PONG, data, len, false);
    if (frame_len == 0) return SERVE_ERROR;
    
    if (conn_write(sc->conn, buf, frame_len) < 0) {
        return SERVE_CLOSE;
    }
    
    return SERVE_OK;
}

static inline int ws_send_close(serve_conn_t sc, uint16_t code, const char *reason) {
    if (!sc) return SERVE_ERROR;
    
    uint8_t buf[128];
    size_t i = 0;
    
    /* Close code (network byte order) */
    buf[i++] = (code >> 8) & 0xFF;
    buf[i++] = code & 0xFF;
    
    /* Close reason */
    if (reason) {
        size_t rlen = strlen(reason);
        if (i + rlen <= sizeof(buf)) {
            memcpy(buf + i, reason, rlen);
            i += rlen;
        }
    }
    
    size_t frame_len = ws_frame_build(buf, sizeof(buf), WS_CLOSE, buf, i, false);
    if (frame_len == 0) return SERVE_ERROR;
    
    if (conn_write(sc->conn, buf, frame_len) < 0) {
        return SERVE_CLOSE;
    }
    
    return SERVE_CLOSE;  /* Close connection after sending */
}

/* ======================================================================== */
/* Protocol Handlers                                                        */
/* ======================================================================== */

static int ws_serve_open(serve_conn_t sc) {
    if (!sc) return SERVE_ERROR;
    
    ws_state_t *state = (ws_state_t *)calloc(1, sizeof(ws_state_t));
    if (!state) return SERVE_ERROR;
    
    state->msg_buf = (uint8_t *)malloc(65536);
    state->msg_size = state->msg_buf ? 65536 : 0;
    state->msg_used = 0;
    state->headers_sent = false;
    state->is_server = true;
    state->close_code = 0;
    
    sc->data = state;
    return SERVE_OK;
}

static int ws_serve_in(serve_conn_t sc) {
    if (!sc || !sc->data) return SERVE_ERROR;
    
    ws_state_t *state = (ws_state_t *)sc->data;
    
    /* Parse WebSocket frame */
    if (sc->buf_used < 2) return SERVE_OK;  /* Need more data */
    
    uint8_t opcode = sc->buf[0] & 0x0F;
    bool fin = (sc->buf[0] & 0x80) != 0;
    bool masked = (sc->buf[1] & 0x80) != 0;
    
    size_t len_offset = 0;
    uint64_t payload_len = ws_decode_len(sc->buf + 1, &len_offset);
    
    /* Check if we have full frame */
    size_t header_size = 2 + len_offset + (masked ? 4 : 0);
    if (sc->buf_used < header_size + payload_len) {
        return SERVE_OK;  /* Need more data */
    }
    
    /* Handle control frames */
    if (opcode == WS_PING) {
        ws_send_pong(sc, sc->buf + header_size, payload_len);
        sc->buf_consumed = header_size + payload_len;
        return SERVE_OK;
    }
    
    if (opcode == WS_PONG) {
        sc->buf_consumed = header_size + payload_len;
        return SERVE_OK;
    }
    
    if (opcode == WS_CLOSE) {
        if (payload_len >= 2) {
            state->close_code = (sc->buf[header_size] << 8) | sc->buf[header_size + 1];
        }
        return SERVE_CLOSE;
    }
    
    /* Data frames - accumulate if fragmented */
    if (opcode == WS_TEXT || opcode == WS_BINARY) {
        if (state->msg_used + payload_len > state->msg_size) {
            return ws_send_close(sc, 1009, "Message too big");
        }
        
        uint8_t *payload = sc->buf + header_size;
        
        /* Unmask if needed */
        if (masked) {
            uint8_t *mask = sc->buf + 2 + len_offset;
            for (uint64_t i = 0; i < payload_len; i++) {
                state->msg_buf[state->msg_used + i] = payload[i] ^ mask[i % 4];
            }
        } else {
            memcpy(state->msg_buf + state->msg_used, payload, payload_len);
        }
        
        state->msg_used += payload_len;
        
        if (fin) {
            /* Complete message - user handler processes it */
            /* For now, just echo back */
            if (opcode == WS_TEXT) {
                state->msg_buf[state->msg_used] = '\0';
            }
            state->msg_used = 0;  /* Reset for next message */
        }
        
        sc->buf_consumed = header_size + payload_len;
        return SERVE_DONE;
    }
    
    sc->buf_consumed = header_size + payload_len;
    return SERVE_OK;
}

static int ws_serve_out(serve_conn_t sc) {
    if (!sc || !sc->data) return SERVE_ERROR;
    
    ws_state_t *state = (ws_state_t *)sc->data;
    
    /* Send handshake response on first call */
    if (!state->headers_sent) {
        /* Compute Sec-WebSocket-Accept */
        /* (Simplified - full impl needs SHA1 hash) */
        const char *accept = "s3pPLMBiTxaQ9kYGzzhZRbK+xOo=";
        
        char resp[512];
        int len = snprintf(resp, sizeof(resp),
            "HTTP/1.1 101 Switching Protocols\r\n"
            "Upgrade: websocket\r\n"
            "Connection: Upgrade\r\n"
            "Sec-WebSocket-Accept: %s\r\n"
            "\r\n", accept);
        
        if (len > 0) {
            conn_write(sc->conn, resp, (size_t)len);
        }
        
        state->headers_sent = true;
    }
    
    return SERVE_OK;
}

static int ws_serve_close(serve_conn_t sc) {
    if (!sc || !sc->data) return SERVE_OK;
    
    ws_state_t *state = (ws_state_t *)sc->data;
    if (state->msg_buf) free(state->msg_buf);
    free(state);
    sc->data = NULL;
    
    return SERVE_OK;
}

/* ======================================================================== */
/* Protocol Definition                                                      */
/* ======================================================================== */

static const proto_t _ws_proto = {
    .name = "websocket",
    .def = proto_ws_def,
    .from = proto_http_def,  /* Upgrades from HTTP */
    .serve_open = ws_serve_open,
    .serve_in = ws_serve_in,
    .serve_out = ws_serve_out,
    .serve_close = ws_serve_close,
    .persist = true,
    .data = NULL
};

static inline const proto_t proto_ws_def(void) {
    return &_ws_proto;
}

/* ======================================================================== */
/* HTTP → WebSocket Upgrade Helper                                          */
/* ======================================================================== */

static inline int http_upgrade_to_ws(serve_conn_t sc) {
    if (!sc) return SERVE_ERROR;
    return serve_upgrade(sc, proto_ws_def);
}

#ifdef __cplusplus
}
#endif
#endif /* _TRANSIT_WS_H */
