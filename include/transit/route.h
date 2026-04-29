/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _TRANSIT_ROUTE_H
#define _TRANSIT_ROUTE_H
#pragma once

#include <config.h>
#include <transit/serve.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* Route Types                                                              */
/* ======================================================================== */

typedef enum {
    ROUTE_MATCH,
    ROUTE_HANDLE,
    ROUTE_PROXY,
    ROUTE_FILES,
    ROUTE_FALLBACK,
    ROUTE_GROUP
} route_type_t;

typedef struct route route_t;
typedef void (*route_handler_t)(serve_conn_t *sc);
typedef int (*route_middleware_t)(serve_conn_t *sc);

/* ======================================================================== */
/* Internal Structures                                                      */
/* ======================================================================== */

#define ROUTE_MAX_ENTRIES 64
#define ROUTE_MAX_PREFIX_LEN 256

typedef struct {
    route_type_t type;
    char pattern[ROUTE_MAX_PREFIX_LEN];
    const char *target;
    route_middleware_t mw;
    route_handler_t handler;
    route_t *group;
} route_entry_t;

struct route {
    route_entry_t entries[ROUTE_MAX_ENTRIES];
    int count;
    route_t *parent;
    char prefix[ROUTE_MAX_PREFIX_LEN];       /* Original prefix (for API) */
    char match_pattern[ROUTE_MAX_PREFIX_LEN]; /* Normalized pattern (for matching) */
    route_middleware_t global_mw[16];
    int global_mw_count;
    route_handler_t fallback;
};

/* ======================================================================== */
/* Path Matching Helpers                                                    */
/* ======================================================================== */

static inline int route_path_match(const char *pattern, const char *path) {
    if (!pattern || !path) return 0;
    
    if (strcmp(pattern, path) == 0) return 1;
    
    size_t plen = strlen(pattern);
    if (plen > 0 && pattern[plen-1] == '/') {
        return strncmp(path, pattern, plen) == 0;
    }
    
    if (plen >= 2 && pattern[plen-2] == '/' && pattern[plen-1] == '*') {
        size_t prefix_len = plen - 2;
        if (strncmp(path, pattern, prefix_len) != 0) return 0;
        return path[prefix_len] != '\0';
    }
    
    return 0;
}

static inline int route_path_capture(const char *pattern, const char *path, const char **capture_out) {
    if (!pattern || !path || !capture_out) return 0;
    
    size_t plen = strlen(pattern);
    if (plen < 2 || pattern[plen-2] != '/' || pattern[plen-1] != '*') {
        *capture_out = NULL;
        return 0;
    }
    
    if (strncmp(path, pattern, plen-2) != 0) {
        *capture_out = NULL;
        return 0;
    }
    
    *capture_out = path + (plen - 1);
    return 1;
}

/* ======================================================================== */
/* Route Creation                                                           */
/* ======================================================================== */

static inline route_t *route_new(void) {
    route_t *r = (route_t *)malloc(sizeof(route_t));
    if (!r) return NULL;
    memset(r, 0, sizeof(route_t));
    r->parent = NULL;
    return r;
}

static inline void route_free(route_t *r) {
    if (!r) return;
    free(r);
}

static inline route_t *route_use(route_t *r, route_middleware_t mw) {
    if (!r || !mw) return r;
    if (r->global_mw_count >= 16) return r;
    r->global_mw[r->global_mw_count++] = mw;
    return r;
}

static inline route_t *route_match(route_t *r, const char *pattern, route_middleware_t mw) {
    if (!r || !pattern || !mw || r->count >= ROUTE_MAX_ENTRIES) return r;
    route_entry_t *e = &r->entries[r->count++];
    e->type = ROUTE_MATCH;
    strncpy(e->pattern, pattern, ROUTE_MAX_PREFIX_LEN - 1);
    e->pattern[ROUTE_MAX_PREFIX_LEN - 1] = '\0';
    e->mw = mw;
    return r;
}

static inline route_t *route_handle(route_t *r, const char *path, route_handler_t handler) {
    if (!r || !path || !handler || r->count >= ROUTE_MAX_ENTRIES) return r;
    route_entry_t *e = &r->entries[r->count++];
    e->type = ROUTE_HANDLE;
    strncpy(e->pattern, path, ROUTE_MAX_PREFIX_LEN - 1);
    e->pattern[ROUTE_MAX_PREFIX_LEN - 1] = '\0';
    e->handler = handler;
    return r;
}

static inline route_t *route_proxy(route_t *r, const char *pattern, const char *upstream) {
    if (!r || !pattern || !upstream || r->count >= ROUTE_MAX_ENTRIES) return r;
    route_entry_t *e = &r->entries[r->count++];
    e->type = ROUTE_PROXY;
    strncpy(e->pattern, pattern, ROUTE_MAX_PREFIX_LEN - 1);
    e->pattern[ROUTE_MAX_PREFIX_LEN - 1] = '\0';
    e->target = upstream;
    return r;
}

static inline route_t *route_serve_files(route_t *r, const char *pattern, const char *root) {
    if (!r || !pattern || !root || r->count >= ROUTE_MAX_ENTRIES) return r;
    route_entry_t *e = &r->entries[r->count++];
    e->type = ROUTE_FILES;
    strncpy(e->pattern, pattern, ROUTE_MAX_PREFIX_LEN - 1);
    e->pattern[ROUTE_MAX_PREFIX_LEN - 1] = '\0';
    e->target = root;
    return r;
}

static inline route_t *route_fallback(route_t *r, route_handler_t handler) {
    if (!r) return r;
    r->fallback = handler;
    return r;
}

static inline route_t *route_group(route_t *r, const char *prefix) {
    if (!r || !prefix || r->count >= ROUTE_MAX_ENTRIES) return NULL;
    
    route_t *g = route_new();
    if (!g) return NULL;
    
    g->parent = r;
    
    /* Store original prefix (for API access) */
    strncpy(g->prefix, prefix, ROUTE_MAX_PREFIX_LEN - 1);
    g->prefix[ROUTE_MAX_PREFIX_LEN - 1] = '\0';
    
    /* Create normalized match pattern (ends with / for prefix matching) */
    strncpy(g->match_pattern, prefix, ROUTE_MAX_PREFIX_LEN - 1);
    g->match_pattern[ROUTE_MAX_PREFIX_LEN - 1] = '\0';
    size_t plen = strlen(g->match_pattern);
    if (plen > 0 && plen < ROUTE_MAX_PREFIX_LEN - 1 && g->match_pattern[plen-1] != '/') {
        g->match_pattern[plen] = '/';
        g->match_pattern[plen+1] = '\0';
    }
    
    /* Add group entry with normalized pattern */
    route_entry_t *e = &r->entries[r->count++];
    e->type = ROUTE_GROUP;
    strncpy(e->pattern, g->match_pattern, ROUTE_MAX_PREFIX_LEN - 1);
    e->pattern[ROUTE_MAX_PREFIX_LEN - 1] = '\0';
    e->group = g;
    
    return g;
}

/* ======================================================================== */
/* Internal Dispatch Helper                                                 */
/* ======================================================================== */

static inline void route_dispatch_internal(route_t *r, serve_conn_t *sc, const char *path) {
    if (!r || !sc || !path) return;
    
    for (int i = 0; i < r->global_mw_count; i++) {
        if (r->global_mw[i](sc) != 0) return;
    }
    
    int handled = 0;
    
    for (int i = 0; i < r->count && !handled; i++) {
        route_entry_t *e = &r->entries[i];
        
        if (!route_path_match(e->pattern, path)) continue;
        
        if (e->type == ROUTE_MATCH && e->mw) {
            if (e->mw(sc) != 0) return;
            continue;
        }
        
        switch (e->type) {
            case ROUTE_HANDLE:
                if (e->handler) e->handler(sc);
                handled = 1;
                break;
                
            case ROUTE_PROXY:
                sc->res.code = HTTP_NOT_IMPL;
                http_res_body(&sc->res, (uint8_t*)"Proxy not implemented", 21);
                handled = 1;
                break;
                
            case ROUTE_FILES:
                sc->res.code = HTTP_NOT_IMPL;
                http_res_body(&sc->res, (uint8_t*)"File serving not implemented", 28);
                handled = 1;
                break;
                
            case ROUTE_GROUP:
                if (e->group) {
                    size_t prefix_len = strlen(e->pattern);
                    const char *child_path = path + prefix_len;
                    /* Ensure child path starts with / */
                    if (*child_path == '\0') {
                        child_path = "/";
                    } else if (*child_path != '/') {
                        /* Need to prepend / - use a static buffer */
                        static char child_buf[ROUTE_MAX_PREFIX_LEN];
                        snprintf(child_buf, sizeof(child_buf), "/%s", child_path);
                        child_path = child_buf;
                    }
                    route_dispatch_internal(e->group, sc, child_path);
                    handled = 1;
                }
                break;
                
            default:
                break;
        }
    }
    
    if (!handled) {
        if (r->fallback) {
            r->fallback(sc);
        } else {
            sc->res.code = HTTP_NOT_FOUND;
            http_res_body(&sc->res, (uint8_t*)"Not Found", 9);
        }
    }
}

/* ======================================================================== */
/* Dispatch Logic                                                           */
/* ======================================================================== */

static inline void route_dispatch(route_t *r, serve_conn_t *sc) {
    if (!r || !sc) return;
    
    const char *path = sc->req.path;
    if (!path) path = "/";
    
    route_dispatch_internal(r, sc, path);
}

/* ======================================================================== */
/* Built-in Middleware                                                      */
/* ======================================================================== */

static inline int route_middleware_log(serve_conn_t *sc) {
    if (!sc) return -1;
    return 0;
}

static inline int route_middleware_cors(serve_conn_t *sc) {
    if (!sc) return -1;
    http_res_header(&sc->res, "Access-Control-Allow-Origin", "*");
    http_res_header(&sc->res, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    http_res_header(&sc->res, "Access-Control-Allow-Headers", "Content-Type, Authorization");
    
    if (sc->req.method == HTTP_OPTIONS) {
        sc->res.code = HTTP_NO_CONTENT;
        return 1;
    }
    return 0;
}

static inline int route_middleware_auth(serve_conn_t *sc) {
    if (!sc) return -1;
    const char *auth = http_get_header(&sc->req, "Authorization");
    if (!auth || strncmp(auth, "Bearer ", 7) != 0) {
        sc->res.code = HTTP_UNAUTHORIZED;
        http_res_body(&sc->res, (uint8_t*)"Unauthorized", 12);
        return 1;
    }
    return 0;
}

static inline int route_middleware_rate_limit(serve_conn_t *sc) {
    if (!sc) return -1;
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* _TRANSIT_ROUTE_H */
