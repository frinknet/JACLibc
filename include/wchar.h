// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef WCHAR_H
#define WCHAR_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t wchar_t;
typedef uint32_t wint_t;
typedef int mbstate_t;

/* Multibyte <-> wide conversions */
static inline size_t mbrlen(const char *s, size_t n, mbstate_t *ps) { return __builtin_mbrlen(s, n, ps); }
static inline int mbtowc(wchar_t *pwc, const char *s, size_t n) { return __builtin_mbtowc(pwc, s, n); }
static inline int wctomb(char *s, wchar_t wc) { return __builtin_wctomb(s, wc); }
static inline size_t mbstowcs(wchar_t *pwcs, const char *s, size_t n) { return __builtin_mbstowcs(pwcs, s, n); }
static inline size_t wcstombs(char *s, const wchar_t *pwcs, size_t n) { return __builtin_wcstombs(s, pwcs, n); }

/* Single-byte <-> wide conversions */
static inline wint_t btowc(int c)			 { return __builtin_btowc(c); }
static inline int		wctob(wint_t wc)	 { return __builtin_wctob(wc); }

/* Orientation */
static inline int		fwide(void *stream, int mode) { return __builtin_fwide(stream, mode); }

/* Wide-character string operations */
static inline wchar_t* wmemcpy(wchar_t* d, const wchar_t* s, size_t n) { return __builtin_wmemcpy(d, s, n); }
static inline wchar_t* wmemmove(wchar_t* d, const wchar_t* s, size_t n) { return __builtin_wmemmove(d, s, n); }
static inline wchar_t* wmemchr(const wchar_t* s, wchar_t c, size_t n) { return __builtin_wmemchr(s, c, n); }
static inline int			 wmemcmp(const wchar_t* s1, const wchar_t* s2, size_t n) { return __builtin_wmemcmp(s1, s2, n); }
static inline wchar_t* wmemset(wchar_t* s, wchar_t c, size_t n) { return __builtin_wmemset(s, c, n); }

/* State and limits */
#define MB_CUR_MAX 1
#define WCHAR_MIN  0
#define WCHAR_MAX  0xFFFFFFFFu

#ifdef __cplusplus
}
#endif

#endif /* WCHAR_H */
