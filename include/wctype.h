/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef WCTYPE_H
#define WCTYPE_H
#pragma once

#include <locale.h>
#include <wchar.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int wctype_t;
typedef int wctrans_t;

extern thread_local __jacl_wctype_t __jacl_wctype;

/* Conversions */
static inline int iswdigit(wint_t c) {
	for (int i = 0; i < 10; i++) {
		if (__jacl_wctype.d[i] == c) return 1;
	}

	return 0;
}

static inline int iswcntrl(wint_t c) {
	return (c >= 0x0000 && c <= 0x001F)
		|| (c >= 0x007F && c <= 0x009F)
		|| c == 0x0085   // NEL
		|| c == 0x2028   // LS
		|| c == 0x2029;  // PS
}

static inline int iswspace(wint_t c) {
	return c == L' ' || c == L'\t' || c == L'\n' || c == L'\v' || c == L'\f' || c == L'\r';
}

/* Optimized iswlower/iswupper */
static inline int iswlower(wint_t c) {
	for (uint8_t i = 0; i < __jacl_wctype.l_count; i++) {
		if (__jacl_wctype.l[i] == c) return 1;
	}

	return 0;
}

static inline int iswupper(wint_t c) {
	for (uint8_t i = 0; i < __jacl_wctype.u_count; i++) {
		if (__jacl_wctype.u[i] == c) return 1;
	}

	return 0;
}

static inline int iswxdigit(wint_t c) {
	return (c >= L'0' && c <= L'9') || (c >= L'A' && c <= L'F') || (c >= L'a' && c <= L'f');
}

static inline int iswblank(wint_t c) {
	return c == L' ' || c == L'\t';
}

static inline int iswprint(wint_t c) {
	return !iswcntrl(c);
}

static inline int iswgraph(wint_t c) {
	return iswprint(c) && !iswspace(c);
}

static inline int iswpunct(wint_t c) {
	static const wchar_t core[] = L".,!?;:'\"()[]{}<>/\\-_=+@#$%^&*|`~–—…‘’“”·•°¶§†‡«»";

	for (uint8_t i = 0; i < 48; i++) {
		if (core[i] == c) return 1;
	}

	for (uint8_t i = 0; i < __jacl_wctype.e_count; i++) {
		if (__jacl_wctype.e[i] == c) return 1;
	}

	return 0;
}

static inline int iswalpha(wint_t c) {
	if(!__jacl_wctype.u_count && !__jacl_wctype.l_count) return (c >= __jacl_wctype.u[0] &&  c <= __jacl_wctype.l[0]);

	return iswlower(c) || iswupper(c);
}

static inline int iswalnum(wint_t c) {
	return iswalpha(c) || iswdigit(c);
}

/* Case Conversion */
static inline wint_t towupper(wint_t c) {
	if (__jacl_wctype.u_count != __jacl_wctype.l_count) return c;

	for (uint8_t i = 0; i < __jacl_wctype.l_count; i++) {
		if (__jacl_wctype.l[i] == c) return __jacl_wctype.u[i];
	}

	return c;
}

static inline wint_t towlower(wint_t c) {
	if (__jacl_wctype.u_count != __jacl_wctype.l_count) return c;

	for (uint8_t i = 0; i < __jacl_wctype.u_count; i++) {
		if (__jacl_wctype.u[i] == c) return __jacl_wctype.l[i];
	}

	return c;
}

/* Internal descriptors */
typedef enum {
	WCTYPE_LOWER = 0,
	WCTYPE_UPPER = 1,
	WCTYPE_DIGIT = 2,
	WCTYPE_ALPHA = 3,
	WCTYPE_ALNUM = 4,
	WCTYPE_SPACE = 5,
	WCTYPE_CNTRL = 6,
	WCTYPE_PRINT = 7,
	WCTYPE_GRAPH = 8,
	WCTYPE_PUNCT = 9,
	WCTYPE_BLANK = 10,
	WCTYPE_XDIGIT = 11
} __jacl_wctype_desc_t;

static inline wctype_t wctype(const char *name) {
	if (!name) return 0;

	/* Simple string comparison for common names */
	if (strcmp(name, "lower") == 0)  return WCTYPE_LOWER;
	if (strcmp(name, "upper") == 0)  return WCTYPE_UPPER;
	if (strcmp(name, "digit") == 0)  return WCTYPE_DIGIT;
	if (strcmp(name, "alpha") == 0)  return WCTYPE_ALPHA;
	if (strcmp(name, "alnum") == 0)  return WCTYPE_ALNUM;
	if (strcmp(name, "space") == 0)  return WCTYPE_SPACE;
	if (strcmp(name, "cntrl") == 0)  return WCTYPE_CNTRL;
	if (strcmp(name, "print") == 0)  return WCTYPE_PRINT;
	if (strcmp(name, "graph") == 0)  return WCTYPE_GRAPH;
	if (strcmp(name, "punct") == 0)  return WCTYPE_PUNCT;
	if (strcmp(name, "blank") == 0)  return WCTYPE_BLANK;
	if (strcmp(name, "xdigit") == 0) return WCTYPE_XDIGIT;

	return 0; /* Unknown */
}

static inline int iswctype(wint_t c, wctype_t desc) {
	if (!desc) return 0;

	switch ((__jacl_wctype_desc_t)desc) {
		case WCTYPE_LOWER:   return iswlower(c);
		case WCTYPE_UPPER:   return iswupper(c);
		case WCTYPE_DIGIT:   return iswdigit(c);
		case WCTYPE_ALPHA:   return iswalpha(c);
		case WCTYPE_ALNUM:   return iswalnum(c);
		case WCTYPE_SPACE:   return iswspace(c);
		case WCTYPE_CNTRL:   return iswcntrl(c);
		case WCTYPE_PRINT:   return iswprint(c);
		case WCTYPE_GRAPH:   return iswgraph(c);
		case WCTYPE_PUNCT:   return iswpunct(c);
		case WCTYPE_BLANK:   return iswblank(c);
		case WCTYPE_XDIGIT:  return iswxdigit(c);
		default:             return 0;
	}
}

/* Case Transformation Descriptors */
typedef enum {
	WCTRANS_TOLOWER = 0,
	WCTRANS_TOUPPER = 1
} __jacl_wctrans_desc_t;

static inline wctrans_t wctrans(const char *name) {
	if (!name) return 0;
	if (strcmp(name, "tolower") == 0) return WCTRANS_TOLOWER;
	if (strcmp(name, "toupper") == 0) return WCTRANS_TOUPPER;

	return 0;
}

static inline wint_t towctrans(wint_t c, wctrans_t desc) {
	if (!desc) return c;

	switch ((__jacl_wctrans_desc_t)desc) {
		case WCTRANS_TOLOWER:  return towlower(c);
		case WCTRANS_TOUPPER:  return towupper(c);
		default:               return c;
	}
}

#ifdef __cplusplus
}
#endif

#endif /* WCTYPE_H */
