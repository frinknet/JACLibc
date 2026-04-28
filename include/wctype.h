/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _WCTYPE_H
#define _WCTYPE_H
#pragma once

#include <locale.h>
#include <wchar.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	WCTYPE_UNKNOWN  = 0,
	WCTYPE_LOWER    = 1,
	WCTYPE_UPPER    = 2,
	WCTYPE_DIGIT    = 3,
	WCTYPE_ALPHA    = 4,
	WCTYPE_ALNUM    = 5,
	WCTYPE_SPACE    = 6,
	WCTYPE_CNTRL    = 7,
	WCTYPE_PRINT    = 8,
	WCTYPE_GRAPH    = 9,
	WCTYPE_PUNCT    = 10,
	WCTYPE_BLANK    = 11,
	WCTYPE_XDIGIT   = 12,
} wctype_t;

typedef enum {
	WCTRANS_UNKNOWN = 0,
	WCTRANS_TOLOWER = 1,
	WCTRANS_TOUPPER = 2
} wctrans_t;

static inline int iswcntrl_l(wint_t c, locale_t l) {
	(void)l;

	return (c >= 0x0000 && c <= 0x001F)
		|| (c >= 0x007F && c <= 0x009F)
		|| c == 0x2028   // LS
		|| c == 0x2029;  // PS
}

static inline int iswspace_l(wint_t c,  locale_t l) {
	(void)l;

	return c == L' ' || c == L'\t' || c == L'\n' || c == L'\v' || c == L'\f' || c == L'\r';
}

static inline int iswblank_l(wint_t c,  locale_t l) {
	(void)l;

	return c == L' ' || c == L'\t';
}

static inline int iswprint_l(wint_t c,  locale_t l) {
	(void)l;

	return !iswcntrl_l(c, l);
}

static inline int iswgraph_l(wint_t c,  locale_t l) {
	(void)l;

	return iswprint_l(c, l) && !iswspace_l(c, l);
}

static inline int iswxdigit_l(wint_t c,  locale_t l) {
	(void)l;

	return (c >= L'0' && c <= L'9') || (c >= L'A' && c <= L'F') || (c >= L'a' && c <= L'f');
}

static inline int iswdigit_l(wint_t c, locale_t l) {
	for (int i = 0; i < 10; i++) {
		if (l->wctype.d[i] == c) return 1;
	}

	return 0;
}

static inline int iswpunct_l(wint_t c, locale_t l) {
	static const wchar_t core[] = L".,!?;:'\"()[]{}<>/\\-_=+@#$%^&*|`~–—…‘’“”·•°¶§†‡«»";

	for (uint8_t i = 0; i < 48; i++) {
		if (core[i] == c) return 1;
	}

	for (uint8_t i = 0; i < l->wctype.e_count; i++) {
		if (l->wctype.e[i] == c) return 1;
	}

	return 0;
}

static inline int iswlower_l(wint_t c, locale_t l) {
	for (uint8_t i = 0; i < l->wctype.l_count; i++) {
		if (l->wctype.l[i] == c) return 1;
	}

	return 0;
}

static inline int iswupper_l(wint_t c, locale_t l) {
	for (uint8_t i = 0; i < l->wctype.u_count; i++) {
		if (l->wctype.u[i] == c) return 1;
	}

	return 0;
}

static inline int iswalpha_l(wint_t c, locale_t l) {
	if(!l->wctype.u_count && !l->wctype.l_count) return (c >= l->wctype.u[0] &&  c <= l->wctype.l[0]);

	return iswlower_l(c, l) || iswupper_l(c, l);
}

static inline int iswalnum_l(wint_t c, locale_t l) {
	return iswalpha_l(c, l) || iswdigit_l(c, l);
}

static inline wint_t towupper_l(wint_t c, locale_t l) {
	if (l->wctype.u_count != l->wctype.l_count) return c;

	for (uint8_t i = 0; i < l->wctype.l_count; i++) {
		if (l->wctype.l[i] == c) return l->wctype.u[i];
	}

	return c;
}

static inline wint_t towlower_l(wint_t c,  locale_t l) {
	if (l->wctype.u_count != l->wctype.l_count) return c;

	for (uint8_t i = 0; i < l->wctype.u_count; i++) {
		if (l->wctype.u[i] == c) return l->wctype.l[i];
	}

	return c;
}

static inline wctype_t wctype_l(const char *name, locale_t l) {
	(void)l;

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

	return WCTYPE_UNKNOWN; /* Unknown */
}

static inline int iswctype_l(wint_t c, wctype_t t, locale_t l) {
	if (!t) return 0;

	switch (t) {
		case WCTYPE_LOWER:   return iswlower_l(c, l);
		case WCTYPE_UPPER:   return iswupper_l(c, l);
		case WCTYPE_DIGIT:   return iswdigit_l(c, l);
		case WCTYPE_ALPHA:   return iswalpha_l(c, l);
		case WCTYPE_ALNUM:   return iswalnum_l(c, l);
		case WCTYPE_SPACE:   return iswspace_l(c, l);
		case WCTYPE_CNTRL:   return iswcntrl_l(c, l);
		case WCTYPE_PRINT:   return iswprint_l(c, l);
		case WCTYPE_GRAPH:   return iswgraph_l(c, l);
		case WCTYPE_PUNCT:   return iswpunct_l(c, l);
		case WCTYPE_BLANK:   return iswblank_l(c, l);
		case WCTYPE_XDIGIT:  return iswxdigit_l(c, l);
		default:             return 0;
	}
}

static inline wctrans_t wctrans_l(const char *name, locale_t l) {
	if (!name) return 0;
	if (strcmp(name, "tolower") == 0) return WCTRANS_TOLOWER;
	if (strcmp(name, "toupper") == 0) return WCTRANS_TOUPPER;

	return 0;
}

static inline wint_t towctrans_l(wint_t c, wctrans_t t, locale_t l) {
	if (!t) return c;

	switch ((wctrans_t)t) {
		case WCTRANS_TOLOWER:  return towlower_l(c, l);
		case WCTRANS_TOUPPER:  return towupper_l(c, l);
		default:               return c;
	}
}

static inline int iswcntrl(wint_t c) { return iswcntrl_l(c,    __jacl_locale_current); }
static inline int iswspace(wint_t c) { return iswspace_l(c,    __jacl_locale_current); }
static inline int iswblank(wint_t c) { return iswblank_l(c,    __jacl_locale_current); }
static inline int iswprint(wint_t c) { return iswprint_l(c,    __jacl_locale_current); }
static inline int iswgraph(wint_t c) { return iswgraph_l(c,    __jacl_locale_current); }
static inline int iswxdigit(wint_t c) { return iswxdigit_l(c,  __jacl_locale_current); }

static inline int iswdigit(wint_t c) { return iswdigit_l(c,    __jacl_locale_current); }
static inline int iswpunct(wint_t c) { return iswpunct_l(c,    __jacl_locale_current); }
static inline int iswlower(wint_t c) { return iswlower_l(c,    __jacl_locale_current); }
static inline int iswupper(wint_t c) { return iswupper_l(c,    __jacl_locale_current); }
static inline int iswalpha(wint_t c) { return iswalpha_l(c,    __jacl_locale_current); }
static inline int iswalnum(wint_t c) { return iswalnum_l(c,    __jacl_locale_current); }

static inline wint_t towlower(wint_t c) { return towlower_l(c, __jacl_locale_current); }
static inline wint_t towupper(wint_t c) { return towupper_l(c, __jacl_locale_current); }

static inline wctrans_t wctrans(const char *c) { return wctrans_l(c, __jacl_locale_current); }
static inline wctrans_t towctrans(wint_t c, wctrans_t t) { return towctrans_l(c, t, __jacl_locale_current); }

static inline int iswctype(wint_t c, wctype_t t) { return iswctype_l(c, t,  __jacl_locale_current); }
static inline wctype_t wctype(const char *c) { return wctype_l(c, __jacl_locale_current); }

#ifdef __cplusplus
}
#endif

#endif /* _WCTYPE_H */
