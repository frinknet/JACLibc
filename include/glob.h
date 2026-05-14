/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _GLOB_H
#define _GLOB_H

#pragma once

#include <config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fnmatch.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	size_t gl_pathc;
	char **gl_pathv;
	size_t gl_offs;
} glob_t;

#define GLOB_ERR      0x01
#define GLOB_MARK     0x02
#define GLOB_NOSORT   0x04
#define GLOB_DOOFFS   0x08
#define GLOB_NOESCAPE 0x10
#define GLOB_PERIOD   0x20

#define GLOB_ABORTED  1
#define GLOB_NOMATCH  2
#define GLOB_NOSPACE  3

static inline void globfree(glob_t *pglob) {
	if (!pglob || !pglob->gl_pathv) return;
	for (size_t i = 0; i < pglob->gl_pathc; i++) free(pglob->gl_pathv[i]);
	free(pglob->gl_pathv);
	pglob->gl_pathv = NULL;
	pglob->gl_pathc = 0;
}

static inline int __jacl_glob_add(glob_t *pglob, const char *path, int flags) {
	size_t new_count = pglob->gl_pathc + 1;
	size_t alloc_size = (new_count + pglob->gl_offs) * sizeof(char*);
	char **new_pathv = realloc(pglob->gl_pathv, alloc_size);
	if (!new_pathv) { globfree(pglob); return GLOB_NOSPACE; }

	if (!pglob->gl_pathv && (flags & GLOB_DOOFFS)) {
		for (size_t i = 0; i < pglob->gl_offs; i++) new_pathv[i] = NULL;
	}
	pglob->gl_pathv = new_pathv;

	char *copy = strdup(path);
	if (!copy) { globfree(pglob); return GLOB_NOSPACE; }

	pglob->gl_pathv[pglob->gl_offs + pglob->gl_pathc] = copy;
	pglob->gl_pathc = new_count;
	return 0;
}

static inline int glob(const char *pattern, int flags, int (*errfunc)(const char *, int), glob_t *pglob) {
	if (!pattern || !pglob) return GLOB_NOMATCH;

	const char *slash = strrchr(pattern, '/');
	const char *base = slash ? slash + 1 : pattern;

	char dir_buf[512] = {0};
	const char *dir = ".";
	if (slash) {
		size_t len = (slash == pattern) ? 1 : (size_t)(slash - pattern);
		if (len >= sizeof(dir_buf)) len = sizeof(dir_buf) - 1;
		memcpy(dir_buf, pattern, len);
		dir = dir_buf;
	}

	int has_wild = 0;
	for (const char *p = base; *p; p++) {
		if (*p == '*' || *p == '?' || *p == '[') { has_wild = 1; break; }
	}

	/* Literal path handling */
	if (!has_wild) {
		struct stat st;
		if (stat(pattern, &st) == 0) {
			int r = __jacl_glob_add(pglob, pattern, flags);
			if (r) return r;
			if ((flags & GLOB_MARK) && S_ISDIR(st.st_mode)) {
				size_t idx = pglob->gl_offs + pglob->gl_pathc - 1;
				size_t len = strlen(pglob->gl_pathv[idx]);
				char *new = realloc(pglob->gl_pathv[idx], len + 2);
				if (new) { new[len] = '/'; new[len+1] = '\0'; pglob->gl_pathv[idx] = new; }
			}
			return 0;
		}
		return GLOB_NOMATCH;
	}

	DIR *d = opendir(dir);
	if (!d) {
		if (errfunc && errfunc(dir, errno)) return GLOB_ABORTED;
		return GLOB_NOMATCH;
	}

	struct dirent *ent;
	int found = 0;

	/* FIX: Default to FNM_PERIOD (skip dotfiles) unless GLOB_PERIOD is set */
	int fnm_flags = FNM_PERIOD;
	if (flags & GLOB_NOESCAPE) fnm_flags |= FNM_NOESCAPE;
	if (flags & GLOB_PERIOD) fnm_flags &= ~FNM_PERIOD;

	while ((ent = readdir(d)) != NULL) {
		/* POSIX: glob never matches . or .. */
		if (ent->d_name[0] == '.') {
			if (ent->d_name[1] == '\0' || (ent->d_name[1] == '.' && ent->d_name[2] == '\0')) continue;
		}

		if (fnmatch(base, ent->d_name, fnm_flags) == 0) {
			char path[1024];
			if (slash) {
				size_t dlen = (slash == pattern) ? 0 : (size_t)(slash - pattern);
				if (dlen + strlen(ent->d_name) + 2 > sizeof(path)) continue;
				snprintf(path, sizeof(path), "%.*s/%s", (int)dlen, pattern, ent->d_name);
			} else {
				snprintf(path, sizeof(path), "%s", ent->d_name);
			}

			if (flags & GLOB_MARK) {
				struct stat st;
				if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
					size_t len = strlen(path);
					if (len + 2 < sizeof(path)) { path[len] = '/'; path[len+1] = '\0'; }
				}
			}

			int r = __jacl_glob_add(pglob, path, flags);
			if (r) { closedir(d); return r; }
			found = 1;
		}
	}
	closedir(d);
	return found ? 0 : GLOB_NOMATCH;
}

#ifdef __cplusplus
}
#endif
#endif /* _GLOB_H */
