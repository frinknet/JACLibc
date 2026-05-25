/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _LIBGEN_H
#define _LIBGEN_H
#pragma once

#include <config.h>
#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int splitname(char *path, char **dir, char **base) {
	if (!path || !dir || !base) return -1;

	char *p = path;
	size_t len = strlen(p);

	/* Empty string */
	if (len == 0) {
		*dir = ".";
		*base = ".";

		return 0;
	}

	/* Strip trailing slashes */
	while (len > 1 && p[len - 1] == '/') len--;

	p[len] = '\0';

	/* All slashes -> root */
	if (len == 1 && p[0] == '/') {
		*dir = "/";
		*base = "/";

		return 0;
	}

	/* Find last slash */
	char *slash = p + len;

	while (slash > p && *(slash - 1) != '/') slash--;

	if (slash == p) {
		/* No slash found */
		*dir = ".";
		*base = p;
	} else {
		*base = slash;

		/* Walk backwards past consecutive slashes to find true dir end */
		char *dir_end = slash - 1;

		while (dir_end > p && *(dir_end - 1) == '/') dir_end--;

		/* If we walked all the way back to start, dirname is "/" */
		if (dir_end <= p) {
			*dir = "/";
		} else {
			*dir_end = '\0';
			*dir = p;
		}
	}

	return 0;
}

static inline char *basename(char *path) {
	char *dir, *base;
	if (splitname(path, &dir, &base) < 0) return ".";
	return base;
}

static inline char *dirname(char *path) {
	char *dir, *base;
	if (splitname(path, &dir, &base) < 0) return ".";
	return dir;
}

#ifdef __cplusplus
}
#endif

#endif /* _LIBGEN_H */
