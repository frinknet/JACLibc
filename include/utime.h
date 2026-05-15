/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _UTIME_H
#define _UTIME_H
#pragma once

#include <config.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>

#ifdef __cplusplus
extern "C" {
#endif

struct utimbuf {
	time_t actime;
	time_t modtime;
};

static inline int utime(const char *path, const struct utimbuf *times) {
	struct timespec ts[2];

	if (times) {
		ts[0].tv_sec = times->actime;
		ts[0].tv_nsec = 0;
		ts[1].tv_sec = times->modtime;
		ts[1].tv_nsec = 0;
	} else {
		ts[0].tv_nsec = UTIME_NOW;
		ts[1].tv_nsec = UTIME_NOW;
	}

	return utimensat(AT_FDCWD, path, ts, 0);
}

#ifdef __cplusplus
}
#endif

#endif /* _UTIME_H */
