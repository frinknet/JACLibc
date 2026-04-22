/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _SYS_UN_H
#define _SYS_UN_H
#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 108
#endif

struct sockaddr_un {
	sa_family_t sun_family;
	char        sun_path[UNIX_PATH_MAX];
};

/* POSIX/BSD standard SUN_LEN calculation */
#define SUN_LEN(su) \
	((size_t)offsetof(struct sockaddr_un, sun_path) + \
	 ((su)->sun_path[0] ? strlen((su)->sun_path) : 0) + 1)

/* Safe initializer for AF_UNIX paths */
static inline void sockaddr_un_init(struct sockaddr_un *addr, const char *path) {
	if (!addr || !path) return;
	memset(addr, 0, sizeof(*addr));
	addr->sun_family = AF_UNIX;
	size_t len = strlen(path);
	if (len >= UNIX_PATH_MAX) len = UNIX_PATH_MAX - 1;
	memcpy(addr->sun_path, path, len);
	addr->sun_path[len] = '\0';
}

#ifdef __cplusplus
}
#endif
#endif /* _SYS_UN_H */
