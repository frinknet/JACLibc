// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef WCTYPE_H
#define WCTYPE_H

#include <wchar.h>		/* for wchar_t, wint_t, btowc(), wctomb(), MBCS support */
#include <stdint.h>		/* for uint32_t if needed */

#ifdef __cplusplus
extern "C" {
#endif

/*―― Definitions ――*/
static inline int iswalpha(wint_t c)	{
		/* ASCII letters */
		if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z'))
				return 1;
		/* Arabic block U+0600–U+06FF */
		if (c >= 0x0600 && c <= 0x06FF)
				return 1;
		return 0;
}

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

#ifdef __cplusplus
}
#endif

#endif /* WCTYPE_H */
