/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _FNMATCH_H
#define _FNMATCH_H

#pragma once

#include <config.h>
#include <ctype.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================ */
/* Flags & Return Codes                                                         */
/* ============================================================================ */

#define FNM_PATHNAME 0x01
#define FNM_PERIOD   0x02
#define FNM_NOESCAPE 0x04
#define FNM_FILE_NAME FNM_PATHNAME
#define FNM_LEADING_DIR 0x08
#define FNM_CASEFOLD   0x10

#define FNM_MATCH   0
#define FNM_NOMATCH 1

/* ============================================================================ */
/* Forward Declarations                                                         */
/* ============================================================================ */

/* Forward declaration for recursive star handling */
static inline int __jacl_fnmatch(const char *pattern, const char *string, int flags);

/* ============================================================================ */
/* Internal Helpers                                                             */
/* ============================================================================ */

/* --- Question Mark Check --- */
static inline int __jacl_fnmatch_qmark(const char *s, int flags, const char *string_start) {
	if (!s || !*s) return FNM_NOMATCH;
	if ((flags & FNM_PATHNAME) && *s == '/') return FNM_NOMATCH;
	if ((flags & FNM_PERIOD) && *s == '.') {
		if (s == string_start) return FNM_NOMATCH;
		if (s > string_start && *(s-1) == '/') return FNM_NOMATCH;
	}
	return FNM_MATCH;
}

/* --- Star Handler --- */
static inline int __jacl_fnmatch_star(const char *pattern, const char *string, const char *orig_string, int flags) {
	const char *p = pattern;
	while (*p == '*') p++;

	if (!*p) {
		if (flags & FNM_PERIOD && *string == '.') {
			if (string == orig_string || *(string-1) == '/') return FNM_NOMATCH;
		}
		if (flags & FNM_PATHNAME) {
			return strchr(string, '/') ? FNM_NOMATCH : FNM_MATCH;
		}
		return FNM_MATCH;
	}

	while (*string) {
		if ((flags & FNM_PATHNAME) && *string == '/') break;
		if ((flags & FNM_PERIOD) && *string == '.') {
			if (string == orig_string || *(string-1) == '/') {
				string++;
				continue;
			}
		}
		if (__jacl_fnmatch(p, string, flags) == FNM_MATCH) return FNM_MATCH;
		string++;
	}
	return FNM_NOMATCH;
}

/* --- Bracket Class Handler --- */
/* 'first' tracks if we're at the start of the bracket expression.
 * This allows ']' to be a literal character if it appears first: [!]abc] matches ']' */
static inline int __jacl_fnmatch_bracket(const char **p_ptr, char c, int flags) {
	const char *p = *p_ptr;
	p++;

	int negate = 0;
	if (*p == '!' || *p == '^') {
		negate = 1;
		p++;
	}

	int matched = 0;
	char prev = 0;
	int first = 1;

	while (*p && (*p != ']' || first)) {
		first = 0;
		if (*p == '-' && p[1] && p[1] != ']') {
			p++;
			char c_l = flags & FNM_CASEFOLD ? tolower((unsigned char)c) : c;
			char prev_l = flags & FNM_CASEFOLD ? tolower((unsigned char)prev) : prev;
			char p_l = flags & FNM_CASEFOLD ? tolower((unsigned char)*p) : *p;
			if (c_l >= prev_l && c_l <= p_l) matched = 1;
			prev = 0;
		} else {
			char c_l = flags & FNM_CASEFOLD ? tolower((unsigned char)c) : c;
			char p_l = flags & FNM_CASEFOLD ? tolower((unsigned char)*p) : *p;
			if (c_l == p_l) matched = 1;
			prev = *p;
		}
		p++;
	}

	if (!*p) return -1;
	p++;
	*p_ptr = p;

	return (matched != negate) ? FNM_MATCH : FNM_NOMATCH;
}

/* --- Literal Character Match --- */
static inline int __jacl_fnmatch_literal(const char **p_ptr, char c, int flags) {
	const char *p = *p_ptr;
	char pc = *p;

	if (pc == '\\' && !(flags & FNM_NOESCAPE) && p[1]) {
		p++;
		pc = *p;
	}

	int match = (pc == c);
	if (!match && (flags & FNM_CASEFOLD)) {
		match = (tolower((unsigned char)pc) == tolower((unsigned char)c));
	}

	*p_ptr = p + 1;
	return match ? FNM_MATCH : FNM_NOMATCH;
}

/* ============================================================================ */
/* Main Recursive Engine                                                        */
/* ============================================================================ */

static inline int __jacl_fnmatch(const char *pattern, const char *string, int flags) {
	const char *p = pattern;
	const char *s = string;

	while (*p) {
		switch (*p) {
			case '?':
				if (__jacl_fnmatch_qmark(s, flags, string) != FNM_MATCH) return FNM_NOMATCH;
				p++; s++;
				break;

			case '*':
				return __jacl_fnmatch_star(p, s, string, flags);

			case '[':
				if (!*s) return FNM_NOMATCH;
				if ((flags & FNM_PATHNAME) && *s == '/') return FNM_NOMATCH;
				if ((flags & FNM_PERIOD) && *s == '.' && (s == string || *(s-1) == '/')) return FNM_NOMATCH;

				int bracket_res = __jacl_fnmatch_bracket(&p, *s, flags);
				if (bracket_res == -1) return FNM_NOMATCH;
				if (bracket_res != FNM_MATCH) return FNM_NOMATCH;
				s++;
				break;

			default:
				if (__jacl_fnmatch_literal(&p, *s, flags) != FNM_MATCH) return FNM_NOMATCH;
				s++;
				break;
		}
	}

	return *s ? FNM_NOMATCH : FNM_MATCH;
}

/* ============================================================================ */
/* Public API                                                                   */
/* ============================================================================ */

static inline int fnmatch(const char *pattern, const char *string, int flags) {
	if (!pattern || !string) return FNM_NOMATCH;
	return __jacl_fnmatch(pattern, string, flags);
}

#ifdef __cplusplus
}
#endif

#endif /* _FNMATCH_H */
