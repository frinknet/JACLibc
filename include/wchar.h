// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef WCHAR_H
#define WCHAR_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

/*–– Constants ––*/
#ifndef MB_CUR_MAX
# define MB_CUR_MAX 1
#endif
#ifndef EOF
#define EOF (-1)
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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
#ifdef __WCHAR_TYPE__
typedef __WCHAR_TYPE__ wchar_t;
#else
typedef long wchar_t;
#endif
#endif

#ifdef __WINT_TYPE__
typedef __WINT_TYPE__ wint_t;
#else
typedef unsigned int wint_t;
#endif

typedef struct { uint32_t wc; int bytes, want; } mbstate_t;

static mbstate_t *get_fallback_state(mbstate_t *ps) {
	static mbstate_t zero_state;

	if (!ps) {
		memset(&zero_state, 0, sizeof(zero_state));
		return &zero_state;
	}

	return ps;
}

static inline size_t mbrlen(const char *s, size_t n, mbstate_t *ps) {
	if (!s || n == 0) return 0;

	mbstate_t *st = get_fallback_state(ps);
	unsigned char c = (unsigned char)s[0];

	if (c < 0x80) return 1;
	if ((c & 0xE0) == 0xC0) { st->wc = c & 0x1F; st->want = 2; }
	else if ((c & 0xF0) == 0xE0) { st->wc = c & 0x0F; st->want = 3; }
	else if ((c & 0xF8) == 0xF0) { st->wc = c & 0x07; st->want = 4; }
	else return (size_t)-1;

	st->bytes = 1;

	if (n < (size_t)st->want) return (size_t)-2;

	return st->want;
}

static inline int mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps) {
	if (!s || n == 0) return 0;

	mbstate_t *st = get_fallback_state(ps);
	unsigned char c = (unsigned char)s[0];

	// Single-byte
	if (c < 0x80) { if (pwc) *pwc = c; return 1; }
	// Two-byte (110xxxxx 10xxxxxx)
	if ((c & 0xE0) == 0xC0 && n >= 2) {
			if (pwc) *pwc = ((c & 0x1F) << 6) | ((unsigned char)s[1] & 0x3F);

			return 2;
	}
	// Three-byte (1110xxxx 10xxxxxx 10xxxxxx)
	if ((c & 0xF0) == 0xE0 && n >= 3) {
			if (pwc) *pwc = ((c & 0x0F) << 12) | (((unsigned char)s[1] & 0x3F) << 6) | ((unsigned char)s[2] & 0x3F);

			return 3;
	}
	// Four-byte (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
	if ((c & 0xF8) == 0xF0 && n >= 4) {
			if (pwc) *pwc = ((c & 0x07) << 18) | (((unsigned char)s[1] & 0x3F) << 12) | (((unsigned char)s[2] & 0x3F) << 6) | ((unsigned char)s[3] & 0x3F);

			return 4;
	}
	// Incomplete sequence
	st->wc = c;
	st->bytes = 1;
	st->want = (c & 0xE0) == 0xC0 ? 2 : (c & 0xF0) == 0xE0 ? 3 : (c & 0xF8) == 0xF0 ? 4 : 1;

	if (n < (size_t)st->want) return (size_t)-2;

	return (size_t)-1;
}

static inline int mbtowc(wchar_t *pwc, const char *s, size_t n) {
		static mbstate_t internal_state = {0,0,0};
		return mbrtowc(pwc, s, n, &internal_state);
}

static inline int wcrtomb(char *s, wchar_t wc, mbstate_t *ps) {
		(void)ps;
		if (wc > 0x10FFFF) wc = 0xFFFD;
		if (wc < 0x80) {
				s[0] = (char)wc;
				return 1;
		}
		if (wc < 0x800) {
				s[0] = 0xC0 | (wc >> 6);
				s[1] = 0x80 | (wc & 0x3F);
				return 2;
		}
		if (wc < 0x10000) {
				s[0] = 0xE0 | (wc >> 12);
				s[1] = 0x80 | ((wc >> 6) & 0x3F);
				s[2] = 0x80 | (wc & 0x3F);
				return 3;
		}
		s[0] = 0xF0 | (wc >> 18);
		s[1] = 0x80 | ((wc >> 12) & 0x3F);
		s[2] = 0x80 | ((wc >> 6) & 0x3F);
		s[3] = 0x80 | (wc & 0x3F);
		return 4;
}

static inline int wctomb(char *s, wchar_t wc) {
		// No stateful logic needed unless you want to handle the -1 returns for partials
		return wcrtomb(s, wc, NULL);
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
static wint_t btowc(int c) {
		return c == EOF ? WEOF : (wint_t)(unsigned char)c;
}

static int wctob(wint_t wc) {
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
static inline int		 wstrcoll(const wchar_t *a, const wchar_t *b) { return wcscmp(a,b); }
static inline size_t wcsxfrm(wchar_t *d, const wchar_t *s, size_t n) {
		size_t l = wcslen(s);
		if (n>l) wcscpy(d,s);
		return l;
}

#ifdef __cplusplus
}
#endif
#endif /* WCHAR_H */

