/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _PWD_H
#define _PWD_H

#pragma once

#include <config.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef JACL_PASSWD_PATH
#define JACL_PASSWD_PATH "/etc/passwd"
#endif

struct passwd {
	char   *pw_name;
	char   *pw_passwd;
	uid_t   pw_uid;
	gid_t   pw_gid;
	char   *pw_gecos;
	char   *pw_dir;
	char   *pw_shell;
};

/* ============================================================================ */
/* Internal Parser                                                              */
/* ============================================================================ */

static inline int __jacl_parse_passwd_line(char *line, struct passwd *pwd) {
	char *p = line;
	char *fields[7];
	int i;
	int count = 0;

	for (i = 0; i < 7; i++) {
		fields[i] = p;
		p = strchr(p, ':');
		if (!p) {
			count = i + 1;
			break;
		}
		*p = '\0';
		p++;
		count = i + 1;
	}

	if (count < 7) return -1;

	pwd->pw_name   = fields[0];
	pwd->pw_passwd = fields[1];

	char *endptr;
	long uid = strtol(fields[2], &endptr, 10);
	if (*endptr != '\0' || fields[2][0] == '\0') return -1;
	pwd->pw_uid = (uid_t)uid;

	long gid = strtol(fields[3], &endptr, 10);
	if (*endptr != '\0' || fields[3][0] == '\0') return -1;
	pwd->pw_gid = (gid_t)gid;

	pwd->pw_gecos = fields[4];
	pwd->pw_dir   = fields[5];
	pwd->pw_shell = fields[6];

	char *nl = strchr(pwd->pw_shell, '\n');
	if (nl) *nl = '\0';

	return 0;
}

/* ============================================================================ */
/* Core Search Logic                                                            */
/* ============================================================================ */

static inline int __jacl_search_passwd(int (*match)(const struct passwd *, const void *), const void *key, struct passwd *pwd, char *buf, size_t buflen) {
	FILE *f = fopen(JACL_PASSWD_PATH, "r");
	if (!f) return ENOENT;

	char line[1024];
	int found = 0;

	while (fgets(line, sizeof(line), f)) {
		if (line[0] == '#' || line[0] == '\n') continue;

		if (strlen(line) >= buflen) continue;
		strcpy(buf, line);

		if (__jacl_parse_passwd_line(buf, pwd) == 0) {
			if (match(pwd, key)) {
				found = 1;
				break;
			}
		}
	}

	fclose(f);
	return found ? 0 : ENOENT;
}

static inline int __jacl_match_uid(const struct passwd *pwd, const void *key) {
	return pwd->pw_uid == *(const uid_t *)key;
}

static inline int __jacl_match_name(const struct passwd *pwd, const void *key) {
	return strcmp(pwd->pw_name, (const char *)key) == 0;
}

/* ============================================================================ */
/* Reentrant API (_r)                                                           */
/* ============================================================================ */

static inline int getpwuid_r(uid_t uid, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result) {
	if (!pwd || !buf || !result) {
		if (result) *result = NULL;
		return EINVAL;
	}
	int r = __jacl_search_passwd(__jacl_match_uid, &uid, pwd, buf, buflen);
	if (r == 0) {
		*result = pwd;

		__errno_clr();
	} else {
		*result = NULL;

		__errno_set(r);
	}
	return r;
}

static inline int getpwnam_r(const char *name, struct passwd *pwd, char *buf, size_t buflen, struct passwd **result) {
	if (!name || !pwd || !buf || !result) {
		if (result) *result = NULL;
		return EINVAL;
	}
	int r = __jacl_search_passwd(__jacl_match_name, name, pwd, buf, buflen);
	if (r == 0) {
		*result = pwd;
		__errno_clr();
	} else {
		*result = NULL;
		__errno_set(r);
	}
	return r;
}

/* ============================================================================ */
/* Non-Reentrant API (Separate Static State)                                    */
/* ============================================================================ */

static inline struct passwd *getpwuid(uid_t uid) {
	static char buf_uid[1024];
	static struct passwd pwd_uid;
	struct passwd *result;
	if (getpwuid_r(uid, &pwd_uid, buf_uid, sizeof(buf_uid), &result) == 0) {
		return result;
	}
	return NULL;
}

static inline struct passwd *getpwnam(const char *name) {
	static char buf_name[1024];
	static struct passwd pwd_name;
	struct passwd *result;
	if (getpwnam_r(name, &pwd_name, buf_name, sizeof(buf_name), &result) == 0) {
		return result;
	}
	return NULL;
}

/* ============================================================================ */
/* Iteration API (Shared Static State)                                          */
/* ============================================================================ */

/* Shared static file pointer for iteration */
static FILE *__jacl_pw_file = NULL;

static inline void setpwent(void) {
	if (__jacl_pw_file) fclose(__jacl_pw_file);
	__jacl_pw_file = fopen(JACL_PASSWD_PATH, "r");
}

static inline void endpwent(void) {
	if (__jacl_pw_file) {
		fclose(__jacl_pw_file);
		__jacl_pw_file = NULL;
	}
}

static inline struct passwd *getpwent(void) {
	static char buf[1024];
	static struct passwd pwd;

	if (!__jacl_pw_file) {
		__jacl_pw_file = fopen(JACL_PASSWD_PATH, "r");
		if (!__jacl_pw_file) return NULL;
	}

	char line[1024];
	while (fgets(line, sizeof(line), __jacl_pw_file)) {
		if (line[0] == '#' || line[0] == '\n') continue;
		if (strlen(line) >= sizeof(buf)) continue;
		strcpy(buf, line);
		if (__jacl_parse_passwd_line(buf, &pwd) == 0) return &pwd;
	}

	fclose(__jacl_pw_file);
	__jacl_pw_file = NULL;
	return NULL;
}

#ifdef __cplusplus
}
#endif

#endif /* _PWD_H */
