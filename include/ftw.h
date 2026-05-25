/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _FTW_H
#define _FTW_H
#pragma once

#include <config.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <libgen.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================ */
/* Public Types & Constants (POSIX.1-2024 Issue 8)                              */
/* ============================================================================ */

struct FTW {
    int base;   /* Offset of basename in path */
    int level;  /* Depth relative to root */
};

/* Type flags (3rd arg to callback) */
#define FTW_F    1  /* Non-directory file */
#define FTW_D    2  /* Directory */
#define FTW_DNR  3  /* Directory without read permission */
#define FTW_DP   4  /* Directory with subdirectories visited (post-order) */
#define FTW_NS   5  /* Unknown type; stat() failed */
#define FTW_SL   6  /* Symbolic link */
#define FTW_SLN  7  /* Symbolic link that names a nonexistent file */

/* Walk control flags (4th arg to nftw) */
#define FTW_PHYS   0x01  /* Physical walk, does not follow symbolic links */
#define FTW_MOUNT  0x02  /* Same device ID as start (FTW_XDEV synonym) */
#define FTW_XDEV   0x02  /* POSIX.1-2024: does not descend across devices */
#define FTW_DEPTH  0x04  /* Post-order traversal */
#define FTW_CHDIR  0x08  /* Change to directory before reading */

/* ============================================================================ */
/* Internal Recursive Walker                                                    */
/* ============================================================================ */

static int __nftw_walk(const char *path, int (*fn)(const char *, const struct stat *, int, struct FTW *), int nopenfd, int flags, int level, int base, dev_t root_dev) {
	struct stat st;
	int ret = 0;
	char cwd_buf[PATH_MAX];
	char *saved_cwd = NULL;
	struct FTW ftw = { .base = base, .level = level };

	/* lstat to get entity info */
	if (lstat(path, &st) != 0) return fn(path, &st, FTW_NS, &ftw);

	/* Handle symlinks */
	if (S_ISLNK(st.st_mode)) {
		if (flags & FTW_PHYS) return fn(path, &st, FTW_SL, &ftw);

		struct stat st_target;

		if (stat(path, &st_target) == 0) st = st_target;
		else return fn(path, &st, FTW_SLN, &ftw);
	}

	int type;

	if (S_ISDIR(st.st_mode)) type = (flags & FTW_DEPTH) ? FTW_DP : FTW_D;
	else type = FTW_F;

	if (S_ISDIR(st.st_mode) && (flags & FTW_CHDIR)) {
		saved_cwd = getcwd(cwd_buf, sizeof(cwd_buf));

		if (!saved_cwd || chdir(path) != 0) return fn(path, &st, FTW_DNR, &ftw);

		path = ".";
		ftw.base = 0;
	}

	/* Pre-order visit */
	if (!(flags & FTW_DEPTH)) ret = fn(path, &st, type, &ftw);

	/* Descend into directories */
	if (ret == 0 && S_ISDIR(st.st_mode)) {
		if ((flags & (FTW_MOUNT | FTW_XDEV)) && st.st_dev != root_dev) {
			if (flags & FTW_DEPTH) ret = fn(path, &st, FTW_DP, &ftw);
		} else {
			DIR *dir = opendir(path);

			if (!dir) {
				ret = fn(path, &st, FTW_DNR, &ftw);
			} else {
				char subpath[PATH_MAX];
				struct dirent *entry;

				while (ret == 0 && (entry = readdir(dir)) != NULL) {
					if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

					if (path[0] == '.' && path[1] == '\0') snprintf(subpath, sizeof(subpath), "./%s", entry->d_name);
					else snprintf(subpath, sizeof(subpath), "%s/%s", path, entry->d_name);

					/* Compute base using splitname; preserve errno across call */
					int saved_errno = errno;
					char sub_copy[PATH_MAX];
					strncpy(sub_copy, subpath, sizeof(sub_copy) - 1);
					sub_copy[sizeof(sub_copy) - 1] = '\0';
					char *dir_ptr, *base_ptr;
					splitname(sub_copy, &dir_ptr, &base_ptr);
					int child_base = base_ptr ? (int)(base_ptr - sub_copy) : 0;
					errno = saved_errno;

					ret = __nftw_walk(subpath, fn, nopenfd, flags, level + 1, child_base, root_dev);
				}

				closedir(dir);
			}
		}
	}

	/* Post-order visit */
	if (ret == 0 && (flags & FTW_DEPTH)) {
		ret = fn(path, &st, (S_ISDIR(st.st_mode) ? FTW_DP : type), &ftw);
	}

	/* Restore CWD if we changed it */
	if (saved_cwd) chdir(saved_cwd);

	return ret;
}

/* ============================================================================ */
/* Public API                                                                   */
/* ============================================================================ */

static inline int nftw(const char *path, int (*fn)(const char *, const struct stat *, int, struct FTW *), int nopenfd, int flags) {
	if (!path || !fn) { errno = EINVAL; return -1; }
	if (nopenfd < 1) { errno = EINVAL; return -1; }

	dev_t root_dev = 0;

	if (flags & (FTW_MOUNT | FTW_XDEV)) {
		struct stat root;

		if (stat(path, &root) == 0) root_dev = root.st_dev;
	}

	char path_copy[PATH_MAX];

	strncpy(path_copy, path, sizeof(path_copy) - 1);

	path_copy[sizeof(path_copy) - 1] = '\0';

	char *dir_ptr, *base_ptr;

	splitname(path_copy, &dir_ptr, &base_ptr);

	int base = base_ptr ? (int)(base_ptr - path_copy) : 0;

	return __nftw_walk(path, fn, nopenfd, flags, 0, base, root_dev);
}

#ifdef __cplusplus
}
#endif
#endif /* _FTW_H */
