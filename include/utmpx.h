/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _UTMPX_H
#define _UTMPX_H
#pragma once

#include <config.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================ */

#define EMPTY           0
#define BOOT_TIME       1
#define OLD_TIME        2
#define NEW_TIME        3
#define USER_PROCESS    4
#define INIT_PROCESS    5
#define LOGIN_PROCESS   6
#define DEAD_PROCESS    7

/* ============================================================================ */
/* Structure                                                                    */
/* ============================================================================ */

struct utmpx {
    char            ut_user[32];
    char            ut_id[4];
    char            ut_line[32];
    pid_t           ut_pid;
    short           ut_type;
    struct timeval  ut_tv;
};

/* ============================================================================ */
/* Internal State                                                               */
/* ============================================================================ */

static FILE *__utmpx_fp = NULL;
static struct utmpx __utmpx_buf;

/* ============================================================================ */
/* Path Resolution                                                              */
/* ============================================================================ */

static inline const char *__utmpx_path(void) {
#ifdef JACL_UTMPX_PATH
    return JACL_UTMPX_PATH;
#else
    const char *env = getenv("UTMPX_PATH");

    return (env && env[0]) ? env : "/var/run/utmp";
#endif
}

/* ============================================================================ */
/* Public API                                                                   */
/* ============================================================================ */

static inline void setutxent(void) {
	if (__utmpx_fp) {
		fflush(__utmpx_fp);
		clearerr(__utmpx_fp);
		rewind(__utmpx_fp);

		return;
	}

	const char *p = __utmpx_path();

	__utmpx_fp = fopen(p, "r+b");

	if (!__utmpx_fp) {
		FILE *tmp = fopen(p, "wb");

		if (tmp) fclose(tmp);

		__utmpx_fp = fopen(p, "r+b");
	}
}

static inline void endutxent(void) {
	if (__utmpx_fp) {
		fclose(__utmpx_fp);

		__utmpx_fp = NULL;
	}
}

static inline struct utmpx *getutxent(void) {
	if (!__utmpx_fp) setutxent();
	if (!__utmpx_fp || fread(&__utmpx_buf, sizeof(struct utmpx), 1, __utmpx_fp) != 1) return NULL;

	return &__utmpx_buf;
}

static inline struct utmpx *getutxid(const struct utmpx *id) {
	if (!id) return (__errno_set(EINVAL), NULL);

	setutxent();

	int id_is_empty = 1;

	for (int i = 0; i < (int)sizeof(id->ut_id); i++) {
		if (id->ut_id[i] != '\0') { id_is_empty = 0; break; }
	}

	struct utmpx *rec;

	while ((rec = getutxent())) {
		if (id->ut_type == INIT_PROCESS || id->ut_type == LOGIN_PROCESS ||
			id->ut_type == DEAD_PROCESS || id->ut_type == USER_PROCESS) {

			if (!id_is_empty && memcmp(rec->ut_id, id->ut_id, sizeof(rec->ut_id)) == 0) return rec;
			if (id_is_empty && id->ut_type == USER_PROCESS && rec->ut_pid == id->ut_pid) return rec;
		} else {
			if (rec->ut_type == id->ut_type && rec->ut_pid == id->ut_pid) return rec;
		}
	}

	return NULL;
}

static inline struct utmpx *getutxline(const struct utmpx *line) {
	if (!line) return (__errno_set(EINVAL), NULL);

	setutxent();

	struct utmpx *rec;

	while ((rec = getutxent())) {
		if ((rec->ut_type == USER_PROCESS || rec->ut_type == LOGIN_PROCESS) &&
			memcmp(rec->ut_line, line->ut_line, sizeof(rec->ut_line)) == 0) {

			return rec;
		}
	}

	return NULL;
}

static inline struct utmpx *pututxline(const struct utmpx *ut) {
	if (!ut) return (__errno_set(EINVAL), NULL);
	if (!__utmpx_fp) setutxent();
	if (!__utmpx_fp) return NULL;

	struct utmpx local_copy;
	memcpy(&local_copy, ut, sizeof(local_copy));
	const struct utmpx *src = &local_copy;

	rewind(__utmpx_fp);
	clearerr(__utmpx_fp);

	long idx = 0;
	struct utmpx rec;
	int fd = fileno(__utmpx_fp);

	while (fread(&rec, sizeof(struct utmpx), 1, __utmpx_fp) == 1) {
		int match = 0;

		if (src->ut_type == USER_PROCESS ||
			src->ut_type == INIT_PROCESS ||
			src->ut_type == LOGIN_PROCESS ||
			src->ut_type == DEAD_PROCESS) {
			match = (memcmp(rec.ut_id, src->ut_id, sizeof(rec.ut_id)) == 0);
		} else {
			match = (rec.ut_type == src->ut_type);
		}

		if (match) {
			off_t offset = idx * (off_t)sizeof(struct utmpx);

			if (lseek(fd, offset, SEEK_SET) == (off_t)-1) return NULL;
			if (write(fd, src, sizeof(struct utmpx)) != (ssize_t)sizeof(struct utmpx)) return NULL;

			__utmpx_fp->_cnt = 0;
			__utmpx_fp->_ptr = __utmpx_fp->_base + OVRSIZ;
			__utmpx_fp->_read_pos = offset + (off_t)sizeof(struct utmpx);
			__utmpx_fp->_write_pos = offset + (off_t)sizeof(struct utmpx);
			__utmpx_fp->_flags &= ~__SEOF;
			__utmpx_fp->_last_op = 0;

			memcpy(&__utmpx_buf, src, sizeof(__utmpx_buf));

			return &__utmpx_buf;
		}
		idx++;
	}

	if (ferror(__utmpx_fp)) return (__errno_set(EIO), NULL);
	if (fseek(__utmpx_fp, 0, SEEK_END) != 0) return NULL;

	off_t append_pos = lseek(fd, 0, SEEK_CUR);

	if (write(fd, src, sizeof(struct utmpx)) != (ssize_t)sizeof(struct utmpx)) return NULL;

	__utmpx_fp->_cnt = 0;
	__utmpx_fp->_ptr = __utmpx_fp->_base + OVRSIZ;
	__utmpx_fp->_read_pos = append_pos + (off_t)sizeof(struct utmpx);
	__utmpx_fp->_write_pos = append_pos + (off_t)sizeof(struct utmpx);
	__utmpx_fp->_flags &= ~__SEOF;
	__utmpx_fp->_last_op = 0;

	memcpy(&__utmpx_buf, src, sizeof(__utmpx_buf));

	return &__utmpx_buf;
}

#ifdef __cplusplus
}
#endif

#endif /* _UTMPX_H */
