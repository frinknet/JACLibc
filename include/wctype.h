// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef WCTYPE_H
#define WCTYPE_H

#include <wchar.h>

/* Wide-character classification */
int iswalnum(wint_t c)  { return __builtin_iswalnum(c); }
int iswalpha(wint_t c)  { return __builtin_iswalpha(c); }
int iswcntrl(wint_t c)  { return __builtin_iswcntrl(c); }
int iswdigit(wint_t c)  { return __builtin_iswdigit(c); }
int iswgraph(wint_t c)  { return __builtin_iswgraph(c); }
int iswlower(wint_t c)  { return __builtin_iswlower(c); }
int iswprint(wint_t c)  { return __builtin_iswprint(c); }
int iswpunct(wint_t c)  { return __builtin_iswpunct(c); }
int iswspace(wint_t c)  { return __builtin_iswspace(c); }
int iswupper(wint_t c)  { return __builtin_iswupper(c); }
int iswxdigit(wint_t c) { return __builtin_iswxdigit(c); }
int iswblank(wint_t c)  { return __builtin_iswblank(c); }

/* Wide-character case conversion */
wint_t towlower(wint_t c) { return __builtin_towlower(c); }
wint_t towupper(wint_t c) { return __builtin_towupper(c); }

#endif /* WCTYPE_H */

