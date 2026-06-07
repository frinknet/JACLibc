/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _NL_TYPES_H
#define _NL_TYPES_H
#pragma once

#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Public Types & Constants */
typedef struct __nl_cat {
	char name[64];
	size_t count;
	const char *const *strings;
} __nl_cat_t;

typedef void *nl_catd;
typedef int nl_item;

#define NL_SETID	  1
#define NL_CAT_LOCALE 1

/* Catalog Manager */
static inline nl_catd catopen(const char *name, int oflag) {
	(void)oflag;
	if (!name) return (__errno_set(EINVAL), (nl_catd)-1);

	__nl_cat_t *cat = calloc(1, sizeof(__nl_cat_t));

	if (!cat) return (nl_catd)-1;

	strncpy(cat->name, name, sizeof(cat->name) - 1);

	cat->count = 0;
	cat->strings = NULL;

	return (nl_catd)cat;
}

static inline int catclose(nl_catd catd) {
	if (!catd || catd == (nl_catd)-1) return (__errno_set(EBADF), -1);

	free(((__nl_cat_t *)catd)->strings);
	free(catd);

	return 0;
}

static inline char *catgets(nl_catd catd, int set_id, int msg_id, const char *s) {
	if (!catd || catd == (nl_catd)-1) return (char *)s;

	__nl_cat_t *c = (__nl_cat_t *)catd;

	/* Bare-metal fallback: return default string if catalog empty/unloaded */
	if (!c->strings || set_id < 1 || msg_id < 1) return (char *)s;

	size_t idx = ((set_id - 1) * 100) + (msg_id - 1);

	if (idx >= c->count) return (char *)s;

	return (char *)c->strings[idx];
}

#ifdef __cplusplus
}
#endif

#endif /* _NL_TYPES_H */
