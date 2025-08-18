// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef WCTYPE_H
#define WCTYPE_H

#include <wchar.h>

/* Wide-character classification */
static inline int iswalnum(wint_t c)  { return __builtin_iswalnum(c); }
static inline int iswalpha(wint_t c)  { return __builtin_iswalpha(c); }
static inline int iswcntrl(wint_t c)  { return __builtin_iswcntrl(c); }
static inline int iswdigit(wint_t c)  { return __builtin_iswdigit(c); }
static inline int iswgraph(wint_t c)  { return __builtin_iswgraph(c); }
static inline int iswlower(wint_t c)  { return __builtin_iswlower(c); }
static inline int iswprint(wint_t c)  { return __builtin_iswprint(c); }
static inline int iswpunct(wint_t c)  { return __builtin_iswpunct(c); }
static inline int iswspace(wint_t c)  { return __builtin_iswspace(c); }
static inline int iswupper(wint_t c)  { return __builtin_iswupper(c); }
static inline int iswxdigit(wint_t c) { return __builtin_iswxdigit(c); }
static inline int iswblank(wint_t c)  { return __builtin_iswblank(c); }

/* Wide-character case conversion */
static inline wint_t towlower(wint_t c) { return __builtin_towlower(c); }
static inline wint_t towupper(wint_t c) { return __builtin_towupper(c); }

#endif /* WCTYPE_H */

