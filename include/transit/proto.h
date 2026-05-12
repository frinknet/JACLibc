/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _TRANSIT_PROTO_H
#define _TRANSIT_PROTO_H
#pragma once

#include <config.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <transit/conn.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PROTO_LIST_MAX	32

/* ======================================================================== */
/* Protocol Types                                                           */
/* ======================================================================== */

typedef struct proto	*proto_t;
typedef struct proto_list	*proto_list_t;
typedef const proto_t (*proto_def)(void);

typedef struct serve_conn	*serve_conn_t;
typedef int (*serve_conn_fn)(serve_conn_t sc);

typedef struct client_conn	*client_conn_t;
typedef int (*client_conn_fn)(client_conn_t cc);

/* ======================================================================== */
/* Protocol Structures                                                      */
/* ======================================================================== */

struct proto {
	const char		*name;

	/* Self-Reference & Parent Link */
	proto_def		def;
	proto_def		from;

	/* Server Handlers */
	serve_conn_fn	serve_open;
	serve_conn_fn	serve_in;
	serve_conn_fn	serve_out;
	serve_conn_fn	serve_close;

	/* Client Handlers */
	client_conn_fn	client_open;
	client_conn_fn	client_in;
	client_conn_fn	client_out;
	client_conn_fn	client_close;

	/* Metadata */
	bool		persist;
	void		*data;
};

struct proto_list {
	proto_t	 proto[PROTO_LIST_MAX];
	int		   length;
	proto_t	 main;
};

/* ======================================================================== */
/* Protocol List Lifecycle                                                  */
/* ======================================================================== */

static inline proto_list_t proto_list(void) {
	return (proto_list_t)calloc(1, sizeof(struct proto_list));
}

static inline void proto_free(proto_list_t pl) {
	//TODO free children
	free(pl);
}

/* ======================================================================== */
/* Protocol Management                                                      */
/* ======================================================================== */

static inline const proto_t proto_main(proto_list_t pl) {
	return pl->main;
}

static inline const proto_t proto_find(proto_list_t pl, proto_def fn) {
	if (!pl || !fn) return NULL;

	for (int i = 0; i < pl->length; i++) {
		if (pl->proto[i]->def == fn) return pl->proto[i];
	}

	return NULL;
}

static inline const proto_t proto_add(proto_list_t pl, proto_def fn) {
	if (!pl || !fn || pl->length >= PROTO_LIST_MAX) return NULL;

	proto_t p = proto_find(pl, fn);

	if (!p) {
		p = fn();

		if (!p) return NULL;

		if (p->from) {
			proto_add(pl, p->from);
		} else {
			if (!pl->main) pl->main = p;
			else if(pl->main != p) return NULL;
		}

		pl->proto[pl->length++] = p;
	}

	return p;
}

static inline void proto_remove(proto_list_t pl, proto_def fn) {
	if (!pl || !fn) return;

	proto_t rem = NULL;

	for (int i = 0; i < pl->length; i++) {
		if (pl->proto[i]->def == fn) rem = pl->proto[i];
		else if (rem) pl->proto[i - 1] = pl->proto[i];
	}

	if (rem) {
		pl->proto[pl->length] = NULL;

		if (pl->length) pl->length--;
		if (pl->main == rem) pl->main = NULL;
	}
}

static inline const proto_t proto_upgrade(proto_list_t pl, proto_t restrict cur, const char *restrict nxt) {
	if (!pl || !cur || !nxt) return NULL;

	for (int i = 0; i < pl->length; i++) {
		const proto_t p = pl->proto[i];

		if (p->from == cur->def && strcmp(p->name, nxt) == 0) return p;
	}

	return NULL;
}

#ifdef __cplusplus
}
#endif

#endif /* _TRANSIT_PROTO_H */
