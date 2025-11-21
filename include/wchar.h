/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _WCHAR_H
#define _WCHAR_H
#pragma once

#include <config.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

// Constants
#define MB_CUR_MAX 4
#define WEOF ((wint_t)-1)
#define WCHAR_MIN 0
#define WCHAR_MAX 0xFFFFFFFFu

#ifdef __cplusplus
extern "C" {
#endif

#if JACL_HAS_C99

#ifndef __WINT_TYPE__
typedef unsigned int wint_t;
#else
typedef __WINT_TYPE__ wint_t;
#endif

#ifndef WEOF
#define WEOF ((wint_t)-1)
#endif

#ifndef MB_CUR_MAX
#define MB_CUR_MAX 4
#endif

typedef struct {
	uint32_t wc;
	int bytes, want;
} mbstate_t;

// ===== Heper Function ======
static inline mbstate_t *__jacl_mbstate(mbstate_t *ps) {
	static mbstate_t zero_state;

	if (!ps) {
		memset(&zero_state, 0, sizeof(zero_state));

		return &zero_state;
	}

	return ps;
}

// ===== Core Converters ======
static inline size_t mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps) {
	mbstate_t *st = __jacl_mbstate(ps);
	unsigned char c;

	if (!s || n == 0) return 0;

	c = (unsigned char)s[0];

	if (c < 0x80) { if (pwc) *pwc = c; return 1; }
	if ((c & 0xE0) == 0xC0 && n >= 2) { if (pwc) *pwc = ((c & 0x1F) << 6) | ((unsigned char)s[1] & 0x3F); return 2; }
	if ((c & 0xF0) == 0xE0 && n >= 3) {
		if (pwc) *pwc = ((c & 0x0F) << 12)
		              | (((unsigned char)s[1] & 0x3F) << 6)
		              | ((unsigned char)s[2] & 0x3F);

		return 3;
	}

	if ((c & 0xF8) == 0xF0 && n >= 4) {
		if (pwc) *pwc = ((c & 0x07) << 18)
		              | (((unsigned char)s[1] & 0x3F) << 12)
		              | (((unsigned char)s[2] & 0x3F) << 6)
		              | ((unsigned char)s[3] & 0x3F);

		return 4;
	}

	st->wc    = c;
	st->bytes = 1;
	st->want  = (c & 0xE0) == 0xC0 ? 2 : (c & 0xF0) == 0xE0 ? 3 : (c & 0xF8) == 0xF0 ? 4 : 1;

	if (n < (size_t)st->want) return (size_t)-2;

	errno = EILSEQ;

	return (size_t)-1;
}
static inline size_t wcrtomb(char *s, wchar_t wc, mbstate_t *ps) {
	mbstate_t *st = __jacl_mbstate(ps);

	if (!s) { st->wc = 0; st->bytes = 0; st->want = 0; return 1; }
	if (wc > 0x10FFFF) wc = 0xFFFD;
	if (wc < 0x80) { s[0] = (char)wc; st->wc = 0; return 1; }
	if (wc < 0x800) { s[0] = 0xC0 | (wc >> 6); s[1] = 0x80 | (wc & 0x3F); st->wc = 0; return 2; }
	if (wc < 0x10000) {
		s[0] = 0xE0 | (wc >> 12);
		s[1] = 0x80 | ((wc >> 6) & 0x3F);
		s[2] = 0x80 | (wc & 0x3F);
		st->wc = 0;

		return 3;
	}

	if (wc <= 0x10FFFF) {
		s[0] = 0xF0 | (wc >> 18);
		s[1] = 0x80 | ((wc >> 12) & 0x3F);
		s[2] = 0x80 | ((wc >> 6) & 0x3F);
		s[3] = 0x80 | (wc & 0x3F);
		st->wc = 0;

		return 4;
	}

	errno = EILSEQ;

	return (size_t)-1;
}

// ===== Stateless Wrappers ======
static inline int mbtowc(wchar_t *pwc, const char *s, size_t n) { static mbstate_t internal_state = {0,0,0}; return (int)mbrtowc(pwc, s, n, &internal_state); }
static inline int wctomb(char *s, wchar_t wc) { return (int)wcrtomb(s, wc, NULL); }
static inline size_t mbrlen(const char *s, size_t n, mbstate_t *ps) {
		if (!s || n == 0) return 0;
		mbstate_t *st = __jacl_mbstate(ps);
		unsigned char c = (unsigned char)s[0];
		if (c < 0x80) return 1;
		if ((c & 0xE0) == 0xC0) { st->wc = c & 0x1F; st->want = 2; }
		else if ((c & 0xF0) == 0xE0) { st->wc = c & 0x0F; st->want = 3; }
		else if ((c & 0xF8) == 0xF0) { st->wc = c & 0x07; st->want = 4; }
		else return SIZE_MAX;
		st->bytes = 1;
		if (n < (size_t)st->want) return SIZE_MAX - 1;
		return st->want;
}

// ===== Stateless Buffer Conversion ======
static inline size_t mbstowcs(wchar_t *pw, const char *s, size_t n) {
	size_t i = 0, b;

	while (i < n && *s) {
		b = mbtowc(&pw[i], s, n - i);

		if (b == (size_t)-1) return (size_t)-1;

		s += b;
		i++;
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
static inline int mblen(const char *s, size_t n) { return mbtowc(NULL, s, n); }

// ===== Restartable Buffer Conversion ======
size_t mbsrtowcs(wchar_t * restrict dst, const char ** restrict src, size_t n, mbstate_t * restrict ps);
size_t wcsrtombs(char * restrict dst, const wchar_t ** restrict src, size_t n, mbstate_t * restrict ps);
size_t mbsnrtowcs(wchar_t * restrict dst, const char ** restrict src, size_t nms, size_t len, mbstate_t * restrict ps);
size_t wcsnrtombs(char * restrict dst, const wchar_t ** restrict src, size_t nms, size_t len, mbstate_t * restrict ps);

// ===== Wide Memory/String Operations =====
static inline wchar_t *wmemcpy(wchar_t *d, const wchar_t *s, size_t n) {
	for (size_t i=0; i<n; i++) d[i] = s[i];

	return d;
}
static inline wchar_t *wmemmove(wchar_t *d, const wchar_t *s, size_t n) {
	if (d < s) for (size_t i=0; i<n; i++) d[i]=s[i];
	else for (size_t i=n; i-->0;) d[i]=s[i];

	return d;
}
static inline wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t n) {
	for (size_t i=0; i<n; i++) if (s[i]==c) return (wchar_t*)&s[i];

	return NULL;
}
static inline int wmemcmp(const wchar_t *a, const wchar_t *b, size_t n) {
	for (size_t i=0; i<n; i++) if (a[i]!=b[i]) return a[i]<b[i]?-1:1;

	return 0;
}
static inline wchar_t *wmemset(wchar_t *s, wchar_t c, size_t n) {
	for (size_t i=0; i<n; i++) s[i] = c;

	return s;
}

static inline size_t wcslen(const wchar_t *s) {
	const wchar_t *p=s;

	while (*p) ++p;

	return (size_t)(p-s);
}
static inline wchar_t* wcscpy(wchar_t *d, const wchar_t *s) {
	wchar_t *p = d;

	while ((*p++ = *s++)) { }

	return d;
}
static inline wchar_t *wcsncpy(wchar_t *d, const wchar_t *s, size_t n) {
	size_t i=0;

	for (; i<n && s[i]; i++) d[i]=s[i];
	for (; i<n; i++) d[i]=0;

	return d;
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
static inline int wcscoll(const wchar_t *a, const wchar_t *b) {
	return wcscmp(a,b);
}
static inline size_t wcsxfrm(wchar_t *d, const wchar_t *s, size_t n) {
	size_t l = wcslen(s);

	if (n>l) wcscpy(d,s);

	return l;
}

// ===== Encoding/Decoding Utilities =====
static inline wint_t btowc(int c) { return c == EOF ? WEOF : (wint_t)(unsigned char)c; }
static inline int wctob(wint_t wc) { return wc == WEOF || wc > 0xFF ? EOF : (int)wc; }

// ========= Wide Character I/O ==========
static inline wint_t fputwc(wchar_t wc, FILE* stream) {
	if (JACL_UNLIKELY(!stream || !(stream->_flags & __SWR))) { errno = EBADF; return WEOF; }
	if (!stream->_orientation) stream->_orientation = 2;
	else if (stream->_orientation != 2) { errno = EINVAL; return WEOF; }
	if (stream->_last_op == 1) { stream->_cnt = 0; stream->_ptr = stream->_base + OVRSIZ; }

	char mb[MB_CUR_MAX];
	mbstate_t ps = {0};
	size_t len = wcrtomb(mb, wc, &ps);

	if (len == (size_t)-1) { stream->_flags |= __SERR; return WEOF; }
	for (size_t i = 0; i < len; ++i) if (fputc((unsigned char)mb[i], stream) == EOF) return WEOF;

	stream->_last_op = 2;

	return wc;
}
static inline wint_t fgetwc(FILE* stream) {
	if (JACL_UNLIKELY(!stream || !(stream->_flags & __SRD))) { errno = EBADF; return WEOF; }
	if (!stream->_orientation) stream->_orientation = 2;
	else if (stream->_orientation != 2) { errno = EINVAL; return WEOF; }

	char mb[MB_CUR_MAX];
	wchar_t wc;
	mbstate_t ps = {0};

	for (size_t i = 0; i < MB_CUR_MAX; i++) {
		int c = fgetc(stream);

		if (c == EOF) return WEOF;

		mb[i] = (char)c;

		size_t r = mbrtowc(&wc, mb, i+1, &ps);

		if (r == (size_t)-1) { stream->_flags |= __SERR; return WEOF; }
		if (r != (size_t)-2) return wc;
	}

	errno = EILSEQ; stream->_flags |= __SERR; return WEOF;
}
static inline wint_t putwc(wchar_t wc, FILE* stream) { return fputwc(wc, stream); }
static inline wint_t getwc(FILE* stream) { return fgetwc(stream); }
static inline wint_t ungetwc(wint_t wc, FILE* stream) {
	if (!stream || wc == WEOF) return WEOF;

	char mb[MB_CUR_MAX]; mbstate_t ps = {0};
	size_t len = wcrtomb(mb, (wchar_t)wc, &ps);

	if (len == (size_t)-1) return WEOF;
	for (size_t i = len; i > 0; i--) if (ungetc((unsigned char)mb[i-1], stream) == EOF) return WEOF;

	return wc;
}
static inline wint_t putwchar(wchar_t wc) { return fputwc(wc, stdout); }
static inline wint_t getwchar(void) { return fgetwc(stdin); }
static inline int fputws(const wchar_t* ws, FILE* stream) {
	if (!ws || !stream) { errno = EINVAL; return -1; }
	while (*ws) if (fputwc(*ws++, stream) == WEOF) return -1;

	return 0;
}
static inline wchar_t* fgetws(wchar_t* ws, int n, FILE* stream) {
	if (!ws || !stream || n <= 0) { errno = EINVAL; return NULL; }

	int i = 0; wint_t wc;

	while (i + 1 < n && (wc = fgetwc(stream)) != WEOF) {
		ws[i++] = (wchar_t)wc;

		if (wc == L'\n') break;
	}

	if (i == 0 && wc == WEOF) return NULL;

	ws[i] = L'\0'; return ws;
}
static inline int fwide(FILE *stream, int mode) {
    if (!stream) return 0;
    if (stream->_orientation == 0 && mode != 0) stream->_orientation = (mode > 0) ? 2 : 1;

    return stream->_orientation;
}

// ========= Wide Character Printf/Scanf ==========

// Wide printf family
int vwprintf(const wchar_t * restrict fmt, va_list ap);
int wprintf(const wchar_t * restrict fmt, ...);
int vfwprintf(FILE * restrict stream, const wchar_t * restrict fmt, va_list ap);
int fwprintf(FILE * restrict stream, const wchar_t * restrict fmt, ...);
int vswprintf(wchar_t * restrict s, size_t n, const wchar_t * restrict fmt, va_list ap);
int swprintf(wchar_t * restrict s, size_t n, const wchar_t * restrict fmt, ...);

// Wide scanf family
int vwscanf(const wchar_t * restrict fmt, va_list ap);
int wscanf(const wchar_t * restrict fmt, ...);
int vfwscanf(FILE * restrict stream, const wchar_t * restrict fmt, va_list ap);
int fwscanf(FILE * restrict stream, const wchar_t * restrict fmt, ...);
int vswscanf(const wchar_t * restrict s, const wchar_t * restrict fmt, va_list ap);
int swscanf(const wchar_t * restrict s, const wchar_t * restrict fmt, ...);

#endif /* JACL_HAS_C99 */

#ifdef __cplusplus
}
#endif

#endif /* _WCHAR_H */
