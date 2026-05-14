/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _GRP_H
#define _GRP_H

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

#ifndef JACL_GROUP_PATH
#define JACL_GROUP_PATH "/etc/group"
#endif

struct group {
	char   *gr_name;
	char   *gr_passwd;
	gid_t   gr_gid;
	char  **gr_mem;
};

/* ============================================================================ */
/* Internal Parser                                                              */
/* ============================================================================ */

static inline int __jacl_parse_group_line(char *line, struct group *grp, char *mem_buf, size_t mem_buf_size) {
	char *p = line;
	char *fields[4];
	int i;
	int count = 0;

	/* Split into 4 fields */
	for (i = 0; i < 4; i++) {
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
	
	if (count < 4) return -1;

	grp->gr_name   = fields[0];
	grp->gr_passwd = fields[1];
	
	char *endptr;
	long gid = strtol(fields[2], &endptr, 10);
	if (*endptr != '\0' || fields[2][0] == '\0') return -1;
	grp->gr_gid = (gid_t)gid;

	/* Parse members */
	char *m = fields[3];
	char *nl = strchr(m, '\n');
	if (nl) *nl = '\0';

	/* mem_buf holds char* pointers. 
	   Max members = (mem_buf_size / sizeof(char*)) - 1 
	*/
	size_t max_members = (mem_buf_size / sizeof(char*)) - 1;
	if (max_members == 0) return -1; /* Buffer too small for even one pointer */

	char **mem_ptrs = (char **)mem_buf;
	int mem_count = 0;
	
	/* Manual tokenization to avoid strtok static state */
	char *token_start = m;
	while (*token_start != '\0' && mem_count < (int)max_members) {
		char *comma = strchr(token_start, ',');
		if (comma) {
			*comma = '\0';
			if (token_start != comma) { /* Skip empty tokens */
				mem_ptrs[mem_count++] = token_start;
			}
			token_start = comma + 1;
		} else {
			/* Last token */
			if (*token_start != '\0') {
				mem_ptrs[mem_count++] = token_start;
			}
			break;
		}
	}
	mem_ptrs[mem_count] = NULL;
	grp->gr_mem = mem_ptrs;

	return 0;
}

/* ============================================================================ */
/* Core Search Logic                                                            */
/* ============================================================================ */

static inline int __jacl_search_group(int (*match)(const struct group *, const void *), const void *key, struct group *grp, char *buf, size_t buflen, char *mem_buf, size_t mem_buf_size) {
	FILE *f = fopen(JACL_GROUP_PATH, "r");
	if (!f) return ENOENT;

	char line[1024];
	int found = 0;

	while (fgets(line, sizeof(line), f)) {
		if (line[0] == '#' || line[0] == '\n') continue;
		
		if (strlen(line) >= buflen) continue;
		strcpy(buf, line);

		if (__jacl_parse_group_line(buf, grp, mem_buf, mem_buf_size) == 0) {
			if (match(grp, key)) {
				found = 1;
				break;
			}
		}
	}

	fclose(f);
	return found ? 0 : ENOENT;
}

static inline int __jacl_match_gid(const struct group *grp, const void *key) {
	return grp->gr_gid == *(const gid_t *)key;
}

static inline int __jacl_match_name(const struct group *grp, const void *key) {
	return strcmp(grp->gr_name, (const char *)key) == 0;
}

/* ============================================================================ */
/* Reentrant API (_r)                                                           */
/* ============================================================================ */

static inline int getgrgid_r(gid_t gid, struct group *grp, char *buf, size_t buflen, struct group **result) {
	if (!grp || !buf || !result) {
		if (result) *result = NULL;
		return EINVAL;
	}
	
	/* Reserve space for member pointers at the end of buf */
	size_t ptr_space = 512; /* Enough for 64 pointers on 64-bit */
	if (buflen <= ptr_space) {
		if (result) *result = NULL;
		return ERANGE;
	}
	
	char *mem_ptr_area = buf + (buflen - ptr_space);
	
	int r = __jacl_search_group(__jacl_match_gid, &gid, grp, buf, buflen - ptr_space, mem_ptr_area, ptr_space);
	if (r == 0) {
		*result = grp;
		errno = 0;
	} else {
		*result = NULL;
		errno = r;
	}
	return r;
}

static inline int getgrnam_r(const char *name, struct group *grp, char *buf, size_t buflen, struct group **result) {
	if (!name || !grp || !buf || !result) {
		if (result) *result = NULL;
		return EINVAL;
	}

	size_t ptr_space = 512;
	if (buflen <= ptr_space) {
		if (result) *result = NULL;
		return ERANGE;
	}

	char *mem_ptr_area = buf + (buflen - ptr_space);

	int r = __jacl_search_group(__jacl_match_name, name, grp, buf, buflen - ptr_space, mem_ptr_area, ptr_space);
	if (r == 0) {
		*result = grp;
		errno = 0;
	} else {
		*result = NULL;
		errno = r;
	}
	return r;
}

/* ============================================================================ */
/* Non-Reentrant API (Separate Static State)                                    */
/* ============================================================================ */

static inline struct group *getgrgid(gid_t gid) {
	static char buf_gid[1024];
	static struct group grp_gid;
	static char mem_buf_gid[512]; /* 64 pointers */
	
	FILE *f = fopen(JACL_GROUP_PATH, "r");
	if (!f) return NULL;
	
	char line[1024];
	while (fgets(line, sizeof(line), f)) {
		if (line[0] == '#' || line[0] == '\n') continue;
		if (strlen(line) >= sizeof(buf_gid)) continue;
		strcpy(buf_gid, line);
		if (__jacl_parse_group_line(buf_gid, &grp_gid, mem_buf_gid, sizeof(mem_buf_gid)) == 0) {
			if (grp_gid.gr_gid == gid) {
				fclose(f);
				return &grp_gid;
			}
		}
	}
	fclose(f);
	return NULL;
}

static inline struct group *getgrnam(const char *name) {
	static char buf_name[1024];
	static struct group grp_name;
	static char mem_buf_name[512];
	
	FILE *f = fopen(JACL_GROUP_PATH, "r");
	if (!f) return NULL;
	
	char line[1024];
	while (fgets(line, sizeof(line), f)) {
		if (line[0] == '#' || line[0] == '\n') continue;
		if (strlen(line) >= sizeof(buf_name)) continue;
		strcpy(buf_name, line);
		if (__jacl_parse_group_line(buf_name, &grp_name, mem_buf_name, sizeof(mem_buf_name)) == 0) {
			if (strcmp(grp_name.gr_name, name) == 0) {
				fclose(f);
				return &grp_name;
			}
		}
	}
	fclose(f);
	return NULL;
}

/* ============================================================================ */
/* Iteration API (Shared Static State)                                          */
/* ============================================================================ */

static FILE *__jacl_gr_file = NULL;

static inline void setgrent(void) {
	if (__jacl_gr_file) fclose(__jacl_gr_file);
	__jacl_gr_file = fopen(JACL_GROUP_PATH, "r");
}

static inline void endgrent(void) {
	if (__jacl_gr_file) {
		fclose(__jacl_gr_file);
		__jacl_gr_file = NULL;
	}
}

static inline struct group *getgrent(void) {
	static char buf[1024];
	static struct group grp;
	static char mem_buf[512];

	if (!__jacl_gr_file) {
		__jacl_gr_file = fopen(JACL_GROUP_PATH, "r");
		if (!__jacl_gr_file) return NULL;
	}

	char line[1024];
	while (fgets(line, sizeof(line), __jacl_gr_file)) {
		if (line[0] == '#' || line[0] == '\n') continue;
		if (strlen(line) >= sizeof(buf)) continue;
		strcpy(buf, line);
		if (__jacl_parse_group_line(buf, &grp, mem_buf, sizeof(mem_buf)) == 0) return &grp;
	}

	fclose(__jacl_gr_file);
	__jacl_gr_file = NULL;
	return NULL;
}

#ifdef __cplusplus
}
#endif

#endif /* _GRP_H */
