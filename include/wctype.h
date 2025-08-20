// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef WCTYPE_H
#define WCTYPE_H

#include <wchar.h>    /* for wchar_t, wint_t, btowc(), wctomb(), MBCS support */
#include <stdint.h>   /* for uint32_t if needed */

#ifdef __cplusplus
extern "C" {
#endif

/*―― Classification API ――*/
int iswalnum(wint_t c);
int iswalpha(wint_t c);
int iswcntrl(wint_t c);
int iswdigit(wint_t c);
int iswgraph(wint_t c);
int iswlower(wint_t c);
int iswprint(wint_t c);
int iswpunct(wint_t c);
int iswspace(wint_t c);
int iswupper(wint_t c);
int iswxdigit(wint_t c);
int iswblank(wint_t c);

/*―― Case conversion API ――*/
wint_t towlower(wint_t c);
wint_t towupper(wint_t c);

#ifdef __cplusplus
}
#endif

/*―― Definitions ――*/
int iswalnum(wint_t c)  { return iswalpha(c) || iswdigit(c); }

int iswalpha(wint_t c)  {
    /* ASCII letters */
    if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z'))
        return 1;
    /* Arabic block U+0600–U+06FF */
    if (c >= 0x0600 && c <= 0x06FF)
        return 1;
    return 0;
}

int iswcntrl(wint_t c)  {
    return (c >= 0 && c <= 0x1F) || c == 0x7F;
}

int iswdigit(wint_t c)  {
    return (c >= L'0' && c <= L'9');
}

int iswgraph(wint_t c)  {
    return (c >  L' ' && c <= L'~');
}

int iswlower(wint_t c)  {
    return (c >= L'a' && c <= L'z');
}

int iswprint(wint_t c)  {
    return (c >= L' ' && c <= L'~');
}

int iswpunct(wint_t c)  {
    return iswgraph(c) && !iswalnum(c);
}

int iswspace(wint_t c)  {
    /* space, tab, newline, vertical tab, form feed, carriage return */
    return c == L' ' || (c >= L'\t' && c <= L'\r');
}

int iswupper(wint_t c)  {
    return (c >= L'A' && c <= L'Z');
}

int iswxdigit(wint_t c) {
    return iswdigit(c)
        || (c >= L'A' && c <= L'F')
        || (c >= L'a' && c <= L'f');
}

int iswblank(wint_t c)  {
    return c == L' ' || c == L'\t';
}

wint_t towlower(wint_t c) {
    return iswupper(c) ? c + 32 : c;
}

wint_t towupper(wint_t c) {
    return iswlower(c) ? c - 32 : c;
}

#endif /* WCTYPE_H */

