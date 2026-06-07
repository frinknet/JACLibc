/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _DLFCN_H
#define _DLFCN_H
#pragma once

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

/* POSIX Constants & Types */
#define RTLD_LAZY   0x00001
#define RTLD_NOW    0x00002
#define RTLD_GLOBAL 0x00100
#define RTLD_LOCAL  0x00000
#define RTLD_DEFAULT ((void *)0)
#define RTLD_NEXT    ((void *)-1)

typedef struct {
	const char *dli_fname;
	void       *dli_fbase;
	const char *dli_sname;
	void       *dli_saddr;
} Dl_info;

/* Internal State */
typedef struct jacl_dl_handle {
    char path[256];
    void *base;
    size_t size;
    void *meta;
    struct jacl_dl_handle *next;
} jacl_dl_handle_t;

static jacl_dl_handle_t *__dl_handles = NULL;
static char __dl_err[256] = {0};

/* Backend Injection */
#define __BIN_LINK 1
#include JACL_BIN_FILE

#define __dl_load JACL_CONCAT_EXPAND(__dl_load, JACL_BIN, _)
#define __dl_find JACL_CONCAT_EXPAND(__dl_find, JACL_BIN, _)
#define __dl_free JACL_CONCAT_EXPAND(__dl_free, JACL_BIN, _)
#define __dl_addr JACL_CONCAT_EXPAND(__dl_addr, JACL_BIN, _)

/* Public API */
static inline void *dlopen(const char *filename, int flag) {
	(void)flag;

	__dl_err[0] = '\0';

	if (!filename) return RTLD_DEFAULT;

	int fd = open(filename, O_RDONLY);

	if (fd < 0) { snprintf(__dl_err, sizeof(__dl_err), "open: %s", strerror(errno)); return NULL; }

	struct stat st;

	if (fstat(fd, &st) < 0) { close(fd); snprintf(__dl_err, sizeof(__dl_err), "fstat: %s", strerror(errno)); return NULL; }

	void *file_map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

	close(fd);

	if (file_map == MAP_FAILED) { snprintf(__dl_err, sizeof(__dl_err), "mmap: %s", strerror(errno)); return NULL; }

	jacl_dl_handle_t *h = calloc(1, sizeof(jacl_dl_handle_t));

	if (!h) { munmap(file_map, st.st_size); snprintf(__dl_err, sizeof(__dl_err), "alloc: %s", strerror(errno)); return NULL; }

	strncpy(h->path, filename, sizeof(h->path) - 1);

	if (!__dl_load(h, file_map, &st)) {
		munmap(file_map, st.st_size); free(h);
		snprintf(__dl_err, sizeof(__dl_err), "invalid binary or format not supported");

		return NULL;
	}

	munmap(file_map, st.st_size);

	h->next = __dl_handles;
	__dl_handles = h;

	return h;
}

static inline void *dlsym(void *handle, const char *symbol) {
	__dl_err[0] = '\0';
	if (!symbol) return (__errno_set(EINVAL), NULL);

	if (handle == RTLD_DEFAULT) {
		for (jacl_dl_handle_t *h = __dl_handles; h; h = h->next) {
			void *addr = __dl_find(h, symbol);

			if (addr) return addr;
		}

		strncpy(__dl_err, "not found", sizeof(__dl_err) - 1);

		return (__errno_set(EFAULT), NULL);
	}

	if (handle == RTLD_NEXT) {
		strncpy(__dl_err, "RTLD_NEXT unsupported", sizeof(__dl_err) - 1);

		return (__errno_set(ENOSYS), NULL);
	}

	void *addr = __dl_find((jacl_dl_handle_t *)handle, symbol);
	if (!addr) {
		strncpy(__dl_err, "not found in handle", sizeof(__dl_err) - 1);
		__errno_set(EFAULT);
	}

	return addr;
}

static inline char *dlerror(void) {
	return (__dl_err[0] == '\0') ? NULL : __dl_err;
}

static inline int dlclose(void *handle) {
	if (handle == RTLD_DEFAULT || handle == RTLD_NEXT) return 0;

	jacl_dl_handle_t *h = (jacl_dl_handle_t *)handle;
	jacl_dl_handle_t **prev = &__dl_handles;
	while (*prev) {
		if (*prev == h) {
			*prev = h->next;
			__dl_free(h);
			free(h);
			__dl_err[0] = '\0';
			return 0;
		}
		prev = &(*prev)->next;
	}
	snprintf(__dl_err, sizeof(__dl_err), "invalid handle");
	return -1;
}

static inline int dladdr(const void *addr, Dl_info *info) {
	__dl_err[0] = '\0';
	if (!info) return (__errno_set(EINVAL), 0);

	/* Initialize output */
	info->dli_fname = NULL;
	info->dli_fbase = NULL;
	info->dli_sname = NULL;
	info->dli_saddr = NULL;

	/* Scan loaded handles */
	for (jacl_dl_handle_t *h = __dl_handles; h; h = h->next) {
		if (__dl_addr(h, addr, info)) return 1;
	}

	return 0;
}

#ifdef __cplusplus
}
#endif
#endif /* _DLFCN_H */
