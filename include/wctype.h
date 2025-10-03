/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef WCTYPE_H
#define WCTYPE_H
#pragma once

#include <wchar.h>		// wchar_t, wint_t, btowc(), wctomb()
#include <stdint.h>		// uint32_t

#ifdef __cplusplus
extern "C" {
#endif

/*――― Types & Constants ―――*/
	typedef int wctype_t;
	typedef int wctrans_t;

#ifndef WEOF
#define WEOF ((wint_t)-1)
#endif

/* C/POSIX Locale (ASCII-only) */
// TODO: Revisit for full Unicode support once locale infrastructure exists.
static inline int iswalpha(wint_t c)   { return (c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z'); }
static inline int iswcntrl(wint_t c)	{ return (c >= 0 && c <= 0x1F) || c == 0x7F; }
static inline int iswdigit(wint_t c)	{ return (c >= L'0' && c <= L'9'); }
static inline int iswalnum(wint_t c)	{ return iswalpha(c) || iswdigit(c); }
static inline int iswgraph(wint_t c)	{ return (c >  L' ' && c <= L'~'); }
static inline int iswlower(wint_t c)	{ return (c >= L'a' && c <= L'z'); }
static inline int iswprint(wint_t c)	{ return (c >= L' ' && c <= L'~'); }
static inline int iswpunct(wint_t c)	{ return iswgraph(c) && !iswalnum(c); }
static inline int iswspace(wint_t c)	{ return c == L' ' || (c >= L'\t' && c <= L'\r'); }
static inline int iswupper(wint_t c)	{ return (c >= L'A' && c <= L'Z'); }
static inline int iswxdigit(wint_t c) { return iswdigit(c) || (c >= L'A' && c <= L'F') || (c >= L'a' && c <= L'f'); }
static inline int iswblank(wint_t c)	{ return c == L' ' || c == L'\t'; }
static inline wint_t towlower(wint_t c) { return iswupper(c) ? c + 32 : c; }
static inline wint_t towupper(wint_t c) { return iswlower(c) ? c - 32 : c; }

/* POSIX Stubs */
//TODO: Flesh this out properly
static inline int iswctype(wint_t c, wctype_t desc) { (void)c; (void)desc; return 0; }
static inline wctype_t wctype(const char *name) { (void)name; return 0; }
static inline wint_t towctrans(wint_t c, wctrans_t desc) { (void)desc; return c; }
static inline wctrans_t wctrans(const char *name) { (void)name; return 0; }

#ifdef __cplusplus
}
#endif

#endif /* WCTYPE_H */
