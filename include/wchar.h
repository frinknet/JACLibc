// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef WCHAR_H
#define WCHAR_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*–– Typedef guards ––*/
#if defined(__WCHAR_TYPE__)||defined(WCHAR_TYPE)
	/* use compiler’s wchar_t */
#elif !defined(_WCHAR_T_DEFINED_)
# define _WCHAR_T_DEFINED_
	typedef uint32_t wchar_t;
#endif
#ifndef _WINT_T_DEFINED_
# define _WINT_T_DEFINED_
	typedef uint32_t wint_t;
#endif
#ifndef _MBSTATE_T_DEFINED_
# define _MBSTATE_T_DEFINED_
	typedef int mbstate_t;
#endif

/*–– Constants ––*/
#ifndef MB_CUR_MAX
# define MB_CUR_MAX 1
#endif
#ifndef WEOF
# define WEOF ((wint_t)-1)
#endif
#ifndef WCHAR_MIN
# define WCHAR_MIN 0
#endif
#ifndef WCHAR_MAX
# define WCHAR_MAX 0xFFFFFFFFu
#endif

/*–– UTF-8 ⇄ wchar_t ––*/
static inline size_t mbrlen(const char *s, size_t n, mbstate_t *m) {
		(void)m;
		if (!s||n==0) return 0;
		unsigned char c = (unsigned char)*s;
		if (c < 0x80) return 1;
		if ((c & 0xE0) == 0xC0 && n >= 2) return 2;
		if ((c & 0xF0) == 0xE0 && n >= 3) return 3;
		if ((c & 0xF8) == 0xF0 && n >= 4) return 4;
		return (size_t)-1;
}

static inline int mbtowc(wchar_t *p, const char *s, size_t n) {
		if (!s||n==0) return 0;
		size_t l = mbrlen(s,n,NULL);
		if (l==(size_t)-1) { if (p) *p = 0xFFFD; return 1; }
		wchar_t wc = (unsigned char)s[0] & (0x7F >> l);
		for (size_t i = 1; i < l; i++)
				wc = (wc << 6) | ((unsigned char)s[i] & 0x3F);
		if (p) *p = wc;
		return (int)l;
}

static inline int wctomb(char *s, wchar_t wc) {
		if (wc > 0x10FFFF) wc = 0xFFFD;
		if (wc < 0x80) {
				s[0] = (char)wc; 
				return 1;
		} else if (wc < 0x800) {
				s[0] = 0xC0 | (wc >> 6);
				s[1] = 0x80 | (wc & 0x3F);
				return 2;
		} else if (wc < 0x10000) {
				s[0] = 0xE0 | (wc >> 12);
				s[1] = 0x80 | ((wc >> 6) & 0x3F);
				s[2] = 0x80 | (wc & 0x3F);
				return 3;
		} else {
				s[0] = 0xF0 | (wc >> 18);
				s[1] = 0x80 | ((wc >> 12) & 0x3F);
				s[2] = 0x80 | ((wc >> 6) & 0x3F);
				s[3] = 0x80 | (wc & 0x3F);
				return 4;
		}
}


/*–– String conversions ––*/
static inline size_t mbstowcs(wchar_t *pw, const char *s, size_t n) {
		size_t i=0, b;
		while (i<n && *s) {
				b = mbtowc(&pw[i], s, n - i);
				if (b==(size_t)-1) return (size_t)-1;
				s += b; i++;
		}
		return i;
}

static inline size_t wcstombs(char *s, const wchar_t *pw, size_t n) {
		size_t i=0, t=0, l;
		while (i<n && pw[i]) {
				l = wctomb(s + t, pw[i]);
				if (l==(size_t)-1) return (size_t)-1;
				t += l; i++;
		}
		return t;
}

/*–– Single-byte ↔ wide ––*/
static inline wint_t btowc(int c) {
		return c == EOF ? WEOF : (wint_t)(unsigned char)c;
}

static inline int wctob(wint_t wc) {
		return wc == WEOF || wc > 0xFF ? EOF : (int)wc;
}


/*–– Orientation ––*/
static inline int fwide(void *stream, int mode) { (void)stream; return mode; }

/*–– Memory ops ––*/
static inline wchar_t *wmemcpy(wchar_t *d, const wchar_t *s, size_t n) {
		for (size_t i=0; i<n; i++) d[i] = s[i]; return d;
}
static inline wchar_t *wmemmove(wchar_t *d, const wchar_t *s, size_t n) {
		if (d < s) for (size_t i=0; i<n; i++) d[i]=s[i];
		else				for (size_t i=n; i-->0;) d[i]=s[i];
		return d;
}
static inline wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t n) {
		for (size_t i=0; i<n; i++) if (s[i]==c) return (wchar_t*)&s[i];
		return NULL;
}
static inline int wmemcmp(const wchar_t *a, const wchar_t *b, size_t n) {
		for (size_t i=0; i<n; i++)
				if (a[i]!=b[i]) return a[i]<b[i]?-1:1;
		return 0;
}
static inline wchar_t *wmemset(wchar_t *s, wchar_t c, size_t n) {
		for (size_t i=0; i<n; i++) s[i] = c; return s;
}

/*–– String ops ––*/
static inline size_t wcslen(const wchar_t *s) {
		const wchar_t *p=s; while (*p) ++p; return (size_t)(p-s);
}
static inline wchar_t* wcscpy(wchar_t *d, const wchar_t *s) {
		wchar_t *p = d;
		while ((*p++ = *s++)) { }
		return d;
}

static inline wchar_t *wcsncpy(wchar_t *d, const wchar_t *s, size_t n) {
		size_t i=0; for (; i<n && s[i]; i++) d[i]=s[i];
		for (; i<n; i++) d[i]=0; return d;
}
static inline int wcscmp(const wchar_t *a, const wchar_t *b) {
		while (*a && *a==*b) { a++; b++; }
		return (*a<*b)? -1 : (*a>*b);
}
static inline int wcsncmp(const wchar_t *a, const wchar_t *b, size_t n) {
		size_t i=0;
		for (; i<n && a[i] && a[i]==b[i]; i++);
		if (i==n) return 0;
		return (a[i]<b[i])? -1 : 1;
}
static inline wchar_t *wcschr(const wchar_t *s, wchar_t c) {
		for (; *s; s++) if (*s==c) return (wchar_t*)s;
		return NULL;
}
static inline wchar_t *wcsrchr(const wchar_t *s, wchar_t c) {
		const wchar_t *r=NULL;
		for (; *s; s++) if (*s==c) r=s;
		return (wchar_t*)r;
}
static inline wchar_t *wcsstr(const wchar_t *h, const wchar_t *n) {
		size_t l = wcslen(n);
		if (!l) return (wchar_t*)h;
		for (; *h; h++) if (!wcsncmp(h,n,l)) return (wchar_t*)h;
		return NULL;
}

/*–– Collation & transform ––*/
static inline int		 strcoll(const wchar_t *a, const wchar_t *b) { return wcscmp(a,b); }
static inline size_t wcsxfrm(wchar_t *d, const wchar_t *s, size_t n) {
		size_t l = wcslen(s);
		if (n>l) wcscpy(d,s);
		return l;
}

#ifdef __cplusplus
}
#endif
#endif /* WCHAR_H */

