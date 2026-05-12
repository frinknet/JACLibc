/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _TRANSIT_ROUTE_H
#define _TRANSIT_ROUTE_H
#pragma once

#include <config.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbit.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ROUTE_MATCH_MAX 16
#define ROUTE_FORWARD_MAX 16

/* ======================================================================== */
/* Opaque Types                                                             */
/* ======================================================================== */

typedef struct router *router_t;
typedef struct route_ctx *route_ctx_t;

/* ======================================================================== */
/* Handler Type                                                             */
/* ======================================================================== */

/* Handler - everything is a handler */
/* argv[0] = current path, argv[1..argc-1] = captures */
/* Returns: 0=continue chain, !=0=stop (status, redirect, etc.) */
typedef int (*route_handler_t)(route_ctx_t ctx, const char *argv[], int argc);

/* ======================================================================== */
/* Internal Structures (Hidden from users)                                  */
/* ======================================================================== */

/* Route entry (linked list) */
typedef struct route {
	const char *pattern;
	route_handler_t handler;
	struct route *next;
} route_t;

/* Router */
struct router {
	const char spec[3];
	route_t *routes;
	route_t *last;
	void *data;
};

/* Route context (created per-dispatch) */
struct route_ctx {
	const char *path;         /* Current path (may change on rewrite) */
	router_t router;          /* Parent router */
	void *conn;               /* Connection (sconn_t, client_t, etc.) */
	void *req;                /* Request data (http_req_t, smtp_cmd_t, etc.) */
	void *data;               /* Generic user data */
	int hops;
};

/* ======================================================================== */
/* Router Lifecycle                                                         */
/* ======================================================================== */

static inline router_t router_create(const char spec[3]) {
	if (!spec[0] || !spec[1] || !spec[2]) return NULL;

	router_t r = (router_t)calloc(1, sizeof(struct router));

	memcpy((void *)r->spec, spec, 3);

	return r;
}

static inline void router_free(router_t r) {
	if (!r) return;

	route_t *cur = r->routes;

	while (cur) {
		route_t *next = cur->next;

		free(cur);

		cur = next;
	}

	free(r);
}

/* ======================================================================== */
/* Route Pattern Matching                                                   */
/* ======================================================================== */

static inline int route_scan(const char *h, const char *p) {
	if (!p || *p != '[') return -1;

	BITS(cset, 256);
	p++;

	int m = 0;
	int min = 1;
	int max = INT_MAX;
  int neg = (*p == '^');

  if (neg) { ONES(cset); p++; }
  else ZERO(cset);

	while (*p && *p != ']') {
		if (p[1] == '-' && p[2] != ']') {
			for(int c = *p; c <= p[2]; c++) {
				if (neg) BITOFF(cset, c);
				else BITON(cset, c);
			}

			p += 3;
		} else {
			if (neg) BITOFF(cset, *p);
			else BITON(cset, *p);

			p++;
		}
	}

	if (*p == ']') p++;  // ]
	else return -1;

	if (*p == '{') {
		p++; min = 0; max = 0;

		while (*p >= '0' && *p <= '9') min = min * 10 + (*p++ - '0');

		if (*p == ',') {
			p++;

			if (*p != '}') max = 0;

			while (*p >= '0' && *p <= '9') max = max * 10 + (*p++ - '0');
		} else {
			max = min;
		}

		if (*p++ != '}') return -1;
	}

	while (*h) {
		int hit = BITCHECK(cset, *h);  // 1 cycle!

		if (hit) { h++; if (++m >= max) break; }

		else break;
	}

	return (m < min) ? -1 : m;
}

static inline int route_match(const char *path, const char *pattern, const char spec[3], const char *matches[], size_t msize) {
	if (!path || !pattern || !spec || !matches || !msize) return 0;

	char any = spec[0];
	char div = spec[1];
	char esc = spec[2];
	const char *p = pattern;
	const char *h = path;
	size_t cnt = 0;

	matches[cnt++] = path;

	while (*p) {
		if (*p == any) { // matches
			p++;

			const char *start = h;

			if (*p == '[') { // matches *[
				while (*p == '[') {
					int scan = route_scan(h, p);

					if (scan == -1) return 0;
					else h += scan;

					while (*p != ']') p++;

					p++;

					if (*p  == '{') {
						while (*p != '}') p++;

						p++;
					}
				}
			} else if (*p == any && *(p + 1) == '\0') { // matches **
				while (*h) h++;

				p++;
			} else { // matches *
				while (*h && *h != div) h++;
			}

			if (cnt < msize) matches[cnt++] = start;
		} else if (*p == esc) { // escapes \*
			p++;

			if (*p == *h) {
				p++;
				h++;
			}
			else return 0;
		} else if (*p == *h) { // same char
			p++;
			h++;
		} else {
			return 0;
		}
	}

	// end matches
	return (*p == '\0' && *h == '\0') ? cnt : 0;
}

/* ======================================================================== */
/* Route Registration                                                       */
/* ======================================================================== */

static inline int route_add(router_t r, const char *pattern, route_handler_t handler) {
	if (!r || !pattern || !handler) return -1;

	route_t *route = (route_t *)calloc(1, sizeof(route_t));
	if (!route) return -1;

	route->pattern = pattern;
	route->handler = handler;
	route->next = NULL;

	if (!r->routes) {
		r->routes = route;
		r->last = route;
	} else {
		r->last->next = route;
		r->last = route;
	}

	return 0;
}

static inline int route_remove(router_t r, const char *pattern, route_handler_t handler) {
	if (!r || !pattern || !handler) return -1;

	int deleted = 0;
	route_t *cur = r->routes;
	route_t *prev = NULL;

	while (cur) {
		route_t *next = cur->next;
		if (strcmp(cur->pattern, pattern) == 0 && cur->handler == handler) {
			if (prev) prev->next = next;
			else r->routes = next;
			if (r->last == cur) r->last = prev;
			free(cur);
			deleted++;
		} else {
			prev = cur;
		}
		cur = next;
	}

	return deleted;
}

/* ======================================================================== */
/* Route Dispatch                                                           */
/* ======================================================================== */

static inline int route_forward(route_ctx_t ctx, const char *path) {
	if (!ctx || !ctx->router || !path) return -1;

	int matched = 0, result = 0;
	route_t *cur = ctx->router->routes;

	if (ctx->hops++ > ROUTE_FORWARD_MAX) return 0;

	while (cur) {
		const char *argv[ROUTE_MATCH_MAX];
		int argc = route_match(path, cur->pattern, ctx->router->spec, argv, ROUTE_MATCH_MAX);

		if (argc > 0) {
			matched++;
			result = cur->handler(ctx, argv, argc);

			if (result) return result;
		}

		cur = cur->next;
	}

	return matched > 0 ? 0 : -1;
}

static inline int __route_dispatch(router_t router, const char *path, void *conn, void *req, void *data) {
	if (!router || !path) return -1;

	struct route_ctx ctx = { path, router, conn, req, data };

	return route_forward(&ctx, path);
}

#define route_dispatch(router, path, conn, req, data) __route_dispatch(router, path, (void *)conn, (void *)req, (void *)data)

/* ======================================================================== */
/* Route Accessors i                                                        */
/* ======================================================================== */

static inline const char *route_path(route_ctx_t ctx) { return ctx ? ctx->path : NULL; }
static inline void *route_conn(route_ctx_t ctx) { return ctx ? ctx->conn : NULL; }
static inline void *route_req(route_ctx_t ctx) { return ctx ? ctx->req : NULL; }
static inline void *route_data(route_ctx_t ctx) { return ctx ? ctx->data : NULL; }

/* ======================================================================== */
/* Route Accessor Macros                                                    */
/* ======================================================================== */

#define route_conn_t(ctx, T)  ((T)route_conn(ctx))
#define route_req_t(ctx,  T)  ((T)route_req(ctx))
#define route_data_t(ctx, T)  ((T)route_data(ctx))

#ifdef __cplusplus
}
#endif

#endif /* _TRANSIT_ROUTE_H */
