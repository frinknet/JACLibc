/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _PROTO_HTTP_H
#define _PROTO_HTTP_H
#pragma once

#include <config.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <net/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Constants                                                                */
/* ======================================================================== */

#ifndef HTTP_MAXHDR
#define HTTP_MAXHDR 32
#endif

#ifndef HTTP_HDR_BUFSZ
#define HTTP_HDR_BUFSZ 2048
#endif

#ifndef HTTP_HDR_MAX
#define HTTP_HDR_MAX (16<<10)
#endif

#define HTTP_VERSION_10  0x0100
#define HTTP_VERSION_11  0x0101
#define HTTP_VERSION_20  0x0200
#define HTTP_VERSION_30  0x0300

#define HTTP_ALPN_H1     "http/1.1"
#define HTTP_ALPN_H2     "h2"
#define HTTP_ALPN_H3     "h3"

#define HTTP2_PREFACE    "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n"
#define HTTP2_PREFACE_LEN 24

/* ======================================================================== */
/* Enums                                                                    */
/* ======================================================================== */

typedef enum {
    HTTP_GET = 0, HTTP_POST = 1, HTTP_PUT = 2, HTTP_DELETE = 3,
    HTTP_HEAD = 4, HTTP_OPTIONS = 5, HTTP_PATCH = 6, HTTP_CONNECT = 7,
    HTTP_UNKNOWN = 99
} http_method_t;

typedef enum {
    HTTP_CONT = 100, HTTP_SWITCH = 101, HTTP_OK = 200,
    HTTP_CREATED = 201, HTTP_NO_CONTENT = 204,
    HTTP_MOVED = 301, HTTP_FOUND = 302, HTTP_NOT_MODIFIED = 304,
    HTTP_UNAUTHORIZED = 401, HTTP_FORBIDDEN = 403, HTTP_NOT_FOUND = 404,
    HTTP_METHOD = 405, HTTP_ERROR = 400, HTTP_NOT_IMPL = 501,
    HTTP_BAD_GATE = 502, HTTP_UNAVAILABLE = 503
} http_code_t;

typedef enum {
    HSTAT_HEADERS = 0, HSTAT_VERSION = 1, HSTAT_MORE = 2,
    HSTAT_ERROR = 3, HSTAT_UPGRADE = 4
} http_stat_t;

/* ======================================================================== */
/* Structs                                                                  */
/* ======================================================================== */

typedef struct { const char *key; const char *val; } http_hdr_t;

typedef struct {
    http_method_t method;
    const char *path;
    const char *query;
    const char *authority;
    uint16_t version;
    http_hdr_t hdrs[HTTP_MAXHDR];
    uint8_t hcount;
    size_t content_len;
    bool chunked;
    bool keep_alive;
    bool upgrade;
    const char *upgrade_proto;
    const char *alpn;
    uint8_t _state;
    size_t _offset;
    char *_line_start;
} http_req_t;

typedef struct {
    http_code_t code;
    const char *reason;
    uint16_t version;
    char hdr_buf[1024];
    size_t hdr_len;
    size_t hdr_cap;
    http_hdr_t trailers[16];
    uint8_t tcount;
    const uint8_t *body;
    size_t body_len;
    const char *file_path;
    int file_fd;
    size_t file_len;
    bool chunked;
    bool push_promise;
    uint8_t _built_len;
} http_res_t;

/* ======================================================================== */
/* Forward Declarations                                                     */
/* ======================================================================== */

static inline const char *http_code_reason(http_code_t code);
static inline bool http2_is_preface(const uint8_t *buf, size_t len);
static inline http_method_t http_parse_method(const char *str);
static inline int http_parse_header(http_req_t *req, char *line);

/* ======================================================================== */
/* Version Detection                                                        */
/* ======================================================================== */

static inline uint16_t http_detect_version(const char *alpn, const uint8_t *first_bytes, size_t first_len) {
    if (alpn) {
        if (strcmp(alpn, HTTP_ALPN_H2) == 0) return HTTP_VERSION_20;
        if (strcmp(alpn, HTTP_ALPN_H3) == 0) return HTTP_VERSION_30;
        if (strcmp(alpn, HTTP_ALPN_H1) == 0) return HTTP_VERSION_11;
    }
    if (first_bytes && first_len >= HTTP2_PREFACE_LEN)
        return http2_is_preface(first_bytes, first_len) ? HTTP_VERSION_20 : HTTP_VERSION_11;
    return HTTP_VERSION_11;
}

static inline bool http2_is_preface(const uint8_t *buf, size_t len) {
    return len >= HTTP2_PREFACE_LEN && memcmp(buf, HTTP2_PREFACE, HTTP2_PREFACE_LEN) == 0;
}

static inline const char *http_version_str(uint16_t version) {
    switch (version) {
        case HTTP_VERSION_10: return "HTTP/1.0";
        case HTTP_VERSION_11: return "HTTP/1.1";
        case HTTP_VERSION_20: return "HTTP/2";
        case HTTP_VERSION_30: return "HTTP/3";
        default: return "HTTP/unknown";
    }
}

/* ======================================================================== */
/* Parser Helpers                                                           */
/* ======================================================================== */

static inline http_method_t http_parse_method(const char *str) {
    if (!str) return HTTP_UNKNOWN;
    if (strcmp(str, "GET") == 0) return HTTP_GET;
    if (strcmp(str, "POST") == 0) return HTTP_POST;
    if (strcmp(str, "PUT") == 0) return HTTP_PUT;
    if (strcmp(str, "DELETE") == 0) return HTTP_DELETE;
    if (strcmp(str, "HEAD") == 0) return HTTP_HEAD;
    if (strcmp(str, "OPTIONS") == 0) return HTTP_OPTIONS;
    if (strcmp(str, "PATCH") == 0) return HTTP_PATCH;
    if (strcmp(str, "CONNECT") == 0) return HTTP_CONNECT;
    return HTTP_UNKNOWN;
}

static inline int http_parse_header(http_req_t *req, char *line) {
    if (!req || !line || req->hcount >= HTTP_MAXHDR) return -1;
    char *col = strchr(line, ':');
    if (!col) return -1;
    *col = '\0';
    char *val = col + 1;
    while (*val == ' ' || *val == '\t') val++;
    req->hdrs[req->hcount].key = line;
    req->hdrs[req->hcount].val = val;
    if (strcasecmp(line, "Content-Length") == 0)
        req->content_len = strtoul(val, NULL, 10);
    else if (strcasecmp(line, "Transfer-Encoding") == 0 && strcasecmp(val, "chunked") == 0)
        req->chunked = true;
    else if (strcasecmp(line, "Connection") == 0)
        req->keep_alive = strcasecmp(val, "close") != 0;
    else if (strcasecmp(line, "Upgrade") == 0) {
        req->upgrade = true;
        req->upgrade_proto = val;
    }
    else if (strcasecmp(line, "Host") == 0)
        req->authority = val;
    req->hcount++;
    return 0;
}

static inline http_stat_t http_parse_request_line(http_req_t *req, char *line) {
    if (!req || !line) return HSTAT_ERROR;
    char *sp1 = strchr(line, ' ');
    if (!sp1) return HSTAT_ERROR;
    *sp1 = '\0';
    req->method = http_parse_method(line);
    char *p2 = sp1 + 1;
    char *sp2 = strchr(p2, ' ');
    if (!sp2) return HSTAT_ERROR;
    *sp2 = '\0';
    req->path = p2;
    char *q = strchr(p2, '?');
    req->query = q ? (q + 1) : NULL;
    if (q) *q = '\0';
    req->version = HTTP_VERSION_11;
    if (strcmp(sp2 + 1, "HTTP/1.0") == 0) {
        req->version = HTTP_VERSION_10;
        req->keep_alive = false;
    }
    return HSTAT_MORE;
}

/* ======================================================================== */
/* Parser Functions                                                         */
/* ======================================================================== */

static inline void http_init(http_req_t *req, uint8_t *buf, size_t buf_size) {
    if (!req || !buf) return;
    memset(req, 0, sizeof(http_req_t));
    req->method = HTTP_UNKNOWN;
    req->version = HTTP_VERSION_11;
    req->keep_alive = true;
    req->_state = 0;
    req->_offset = 0;
    req->_line_start = (char *)buf;
    (void)buf_size;
}

static inline http_stat_t http_feed(http_req_t *req, uint8_t *buf, size_t len, size_t *consumed) {
    if (!req || !buf || !consumed) return HSTAT_ERROR;
    size_t i = req->_offset;
    while (i < len) {
        if (req->_state == 0) {
            if (i + 1 >= len) break;
            if (buf[i] == '\r' && buf[i+1] == '\n') {
                buf[i] = '\0'; buf[i+1] = '\0';
                i += 2;
                http_stat_t st = http_parse_request_line(req, req->_line_start);
                if (st != HSTAT_MORE) { *consumed = i; return st; }
                req->_state = 1;
                req->_line_start = (char *)(buf + i);
            } else { i++; }
        } else if (req->_state == 1) {
            if (i + 1 >= len) break;
            if (buf[i] == '\r' && buf[i+1] == '\n') {
                if (req->_line_start == (char *)(buf + i)) {
                    buf[i] = '\0'; buf[i+1] = '\0';
                    i += 2;
                    *consumed = i;
                    return req->upgrade ? HSTAT_UPGRADE : HSTAT_HEADERS;
                }
                buf[i] = '\0'; buf[i+1] = '\0';
                http_parse_header(req, req->_line_start);
                i += 2;
                req->_line_start = (char *)(buf + i);
            } else { i++; }
        }
    }
    req->_offset = i;
    *consumed = i;
    return HSTAT_MORE;
}

static inline void http_reset(http_req_t *req) {
    if (!req) return;
    req->method = HTTP_UNKNOWN;
    req->path = req->query = req->authority = NULL;
    req->hcount = 0;
    req->content_len = 0;
    req->chunked = req->upgrade = false;
    req->upgrade_proto = NULL;
    req->_state = req->_offset = 0;
}

static inline const char *http_get_header(const http_req_t *req, const char *key) {
    if (!req || !key) return NULL;
    for (uint8_t i = 0; i < req->hcount; i++)
        if (strcasecmp(req->hdrs[i].key, key) == 0)
            return req->hdrs[i].val;
    return NULL;
}

static inline bool http_is_upgrade(const http_req_t *req, const char *proto) {
    return req && req->upgrade && req->upgrade_proto && strcasecmp(req->upgrade_proto, proto) == 0;
}

/* ======================================================================== */
/* Body Reading Helpers (Stubs)                                             */
/* ======================================================================== */

int http_read_body(void *conn, uint8_t **body_out, size_t max_len) { (void)conn; (void)body_out; (void)max_len; return -1; }
int http_read_file(void *conn, int out_fd, size_t len) { (void)conn; (void)out_fd; (void)len; return -1; }
ssize_t http_read_chunk(void *conn, uint8_t *buf, size_t buf_size) { (void)conn; (void)buf; (void)buf_size; return -1; }
int http_skip_body(void *conn, size_t len) { (void)conn; (void)len; return -1; }

/* ======================================================================== */
/* Response Builders                                                        */
/* ======================================================================== */

static inline void http_res_init(http_res_t *res, http_code_t code, const char *reason) {
    if (!res) return;
    memset(res, 0, sizeof(http_res_t));
    res->code = code;
    res->reason = reason ? reason : http_code_reason(code);
    res->version = HTTP_VERSION_11;
    res->hdr_cap = sizeof(res->hdr_buf);
    res->file_fd = -1;
}

static inline int http_res_header(http_res_t *res, const char *key, const char *val) {
    if (!res || !key || !val) return -1;
    size_t rem = res->hdr_cap - res->hdr_len;
    if (rem < 32) return -1;
    int n = snprintf(res->hdr_buf + res->hdr_len, rem, "%s: %s\r\n", key, val);
    if (n < 0 || (size_t)n >= rem) return -1;
    res->hdr_len += n;
    return 0;
}

static inline int http_res_trailer(http_res_t *res, const char *key, const char *val) {
    if (!res || !key || !val || res->tcount >= 16) return -1;
    res->trailers[res->tcount].key = key;
    res->trailers[res->tcount].val = val;
    res->tcount++;
    return 0;
}

static inline void http_res_body(http_res_t *res, const uint8_t *data, size_t len) {
    if (!res) return;
    res->body = data;
    res->body_len = len;
    res->file_path = NULL;
    res->file_fd = -1;
    res->chunked = false;
}

int http_res_file(http_res_t *res, const char *path) {
    if (!res || !path) return -1;
    res->file_path = path;
    res->body = NULL;
    res->file_fd = -1;
    return 0;
}

void http_res_file_fd(http_res_t *res, int fd, size_t len) {
    if (!res || fd < 0) return;
    res->file_fd = fd;
    res->file_len = len;
    res->body = NULL;
    res->file_path = NULL;
}

static inline size_t http_res_build(uint8_t *buf, size_t buf_size, http_res_t *res) {
    if (!buf || !res || buf_size < 64) return 0;
    char *p = (char *)buf;
    size_t rem = buf_size;
    int n = snprintf(p, rem, "HTTP/1.1 %d %s\r\n", res->code, res->reason);
    if (n < 0 || (size_t)n >= rem) return 0;
    p += n; rem -= n;
    if (res->hdr_len > 0) {
        if (res->hdr_len >= rem) return 0;
        memcpy(p, res->hdr_buf, res->hdr_len);
        p += res->hdr_len; rem -= res->hdr_len;
    }
    if (res->body && res->body_len > 0 && !res->chunked && !res->file_path && res->file_fd < 0) {
        n = snprintf(p, rem, "Content-Length: %zu\r\n", res->body_len);
        if (n < 0 || (size_t)n >= rem) return 0;
        p += n; rem -= n;
    }
    if (rem < 2) return 0;
    memcpy(p, "\r\n", 2);
    p += 2; rem -= 2;
    size_t hl = p - (char *)buf;
    if (res->body && res->body_len > 0 && !res->chunked && !res->file_path && res->file_fd < 0) {
        if (res->body_len > rem) return 0;
        memcpy(p, res->body, res->body_len);
        return hl + res->body_len;
    }
    return hl;
}

/* ======================================================================== */
/* Utility Functions                                                        */
/* ======================================================================== */

static inline const char *http_method_str(http_method_t method) {
    switch (method) {
        case HTTP_GET: return "GET"; case HTTP_POST: return "POST";
        case HTTP_PUT: return "PUT"; case HTTP_DELETE: return "DELETE";
        case HTTP_HEAD: return "HEAD"; case HTTP_OPTIONS: return "OPTIONS";
        case HTTP_PATCH: return "PATCH"; case HTTP_CONNECT: return "CONNECT";
        default: return "UNKNOWN";
    }
}

static inline http_method_t http_method_from_str(const char *str) {
    if (!str) return HTTP_UNKNOWN;
    if (strcmp(str, "GET") == 0) return HTTP_GET;
    if (strcmp(str, "POST") == 0) return HTTP_POST;
    if (strcmp(str, "PUT") == 0) return HTTP_PUT;
    if (strcmp(str, "DELETE") == 0) return HTTP_DELETE;
    if (strcmp(str, "HEAD") == 0) return HTTP_HEAD;
    if (strcmp(str, "OPTIONS") == 0) return HTTP_OPTIONS;
    if (strcmp(str, "PATCH") == 0) return HTTP_PATCH;
    if (strcmp(str, "CONNECT") == 0) return HTTP_CONNECT;
    return HTTP_UNKNOWN;
}

static inline const char *http_code_reason(http_code_t code) {
    switch (code) {
        case HTTP_OK: return "OK";
        case HTTP_CREATED: return "Created";
        case HTTP_NO_CONTENT: return "No Content";
        case HTTP_MOVED: return "Moved Permanently";
        case HTTP_FOUND: return "Found";
        case HTTP_NOT_MODIFIED: return "Not Modified";
        case HTTP_UNAUTHORIZED: return "Unauthorized";
        case HTTP_FORBIDDEN: return "Forbidden";
        case HTTP_NOT_FOUND: return "Not Found";
        case HTTP_METHOD: return "Method Not Allowed";
        case HTTP_ERROR: return "Bad Request";
        case HTTP_NOT_IMPL: return "Not Implemented";
        case HTTP_BAD_GATE: return "Bad Gateway";
        case HTTP_UNAVAILABLE: return "Service Unavailable";
        default: return "Unknown";
    }
}

#ifdef __cplusplus
}
#endif

#endif /* _PROTO_HTTP_H */
