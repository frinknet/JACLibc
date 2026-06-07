/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#ifndef _WCHAR_H
#define _WCHAR_H
#pragma once

/**
 * Policy: wchar_t is wide‑enough for the target word model.
 *
 *  - 32‑bit or wider: full Unicode (0x0000..0x10FFFF).
 *  - 16‑bit or smaller: BMP‑only (0x0000..0xFFFF).
 *
 * This way even on constrained machines you still get basic UTF-8.
 */

#include <config.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <wctype.h>

// Constants
#define MB_CUR_MAX 4

#ifdef __cplusplus
extern "C" {
#endif

#if JACL_HAS_C99

#ifndef MB_CUR_MAX
#define MB_CUR_MAX 4
#endif

struct tm;

/* ===== Types ===== */
typedef struct {
	uint32_t wc;
	int bytes, want;
} mbstate_t;

/* ===== Helper Functions ===== */
static inline mbstate_t *__jacl_mbstate(mbstate_t *ps) {
	static thread_local mbstate_t zero_state;

	if (!ps) {
		memset(&zero_state, 0, sizeof(zero_state));
		return &zero_state;
	}

	return ps;
}

/* ===== Core Converters ===== */
static inline size_t mbrtowc(wchar_t *pwc, const char *s, size_t n, mbstate_t *ps) {
	if (!s) { if (ps) memset(ps, 0, sizeof(*ps)); return 0; }
	if (!n) return (size_t)-2;

	unsigned char c = s[0];

	if (c < 0x80) {
		if (pwc) *pwc = c;

		return c ? 1 : 0; // POSIX: Return 0 for the null character
	}

	if ((c & 0xE0) == 0xC0) {
		if (n < 2) return (size_t)-2;
		if ((s[1] & 0xC0) != 0x80) return (__errno_set(EILSEQ), (size_t)-1);
		if (pwc) *pwc = ((c & 0x1F) << 6) | (s[1] & 0x3F);
		if (pwc && *pwc < 0x80) return (__errno_set(EILSEQ), (size_t)-1);

		return 2;
	}

	if ((c & 0xF0) == 0xE0) {
		if (n < 3) return (size_t)-2;
		if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80) return (__errno_set(EILSEQ), (size_t)-1);
		if (pwc) *pwc = ((c & 0x0F) << 12) | ((s[1] & 0x3F) << 6) | (s[2] & 0x3F);
		if (pwc && (*pwc < 0x800 || (*pwc >= 0xD800 && *pwc <= 0xDFFF))) return (__errno_set(EILSEQ), (size_t)-1);

		return 3;
	}

	if ((c & 0xF8) == 0xF0) {
		if (n < 4) return (size_t)-2;
		if ((s[1] & 0xC0) != 0x80 || (s[2] & 0xC0) != 0x80 || (s[3] & 0xC0) != 0x80) return (__errno_set(EILSEQ), (size_t)-1);
		if (pwc) *pwc = ((c & 0x07) << 18) | ((s[1] & 0x3F) << 12) | ((s[2] & 0x3F) << 6) | (s[3] & 0x3F);
		if (pwc && (*pwc < 0x10000 || *pwc > 0x10FFFF)) return (__errno_set(EILSEQ), (size_t)-1);

		return 4;
	}

	return (__errno_set(EILSEQ), (size_t)-1);
}

static inline size_t wcrtomb(char *s, wchar_t wc, mbstate_t *ps) {
	mbstate_t *st = __jacl_mbstate(ps);

	if (!s) { st->wc = 0; st->bytes = 0; st->want = 0; return 1; }

	// Strict POSIX validation - REJECT surrogates and out-of-range
	if (wc > 0x10FFFF || (wc >= 0xD800 && wc <= 0xDFFF)) {
		return (__errno_set(EILSEQ), (size_t)-1);
	}

	if (wc < 0x80) { s[0] = (char)wc; st->wc = 0; return 1; }
	if (wc < 0x800) { s[0] = 0xC0 | (wc >> 6); s[1] = 0x80 | (wc & 0x3F); st->wc = 0; return 2; }

	if (wc < 0x10000) {
		s[0] = 0xE0 | (wc >> 12);
		s[1] = 0x80 | ((wc >> 6) & 0x3F);
		s[2] = 0x80 | (wc & 0x3F);
		st->wc = 0;

		return 3;
	}

	s[0] = 0xF0 | (wc >> 18);
	s[1] = 0x80 | ((wc >> 12) & 0x3F);
	s[2] = 0x80 | ((wc >> 6) & 0x3F);
	s[3] = 0x80 | (wc & 0x3F);
	st->wc = 0;

	return 4;
}

/* ===== Stateless Wrappers ===== */
static inline int mbtowc(wchar_t *pwc, const char *s, size_t n) {
	static thread_local mbstate_t internal_state = {0,0,0};
	return (int)mbrtowc(pwc, s, n, &internal_state);
}

static inline int wctomb(char *s, wchar_t wc) {
	if (!s) return 0;

	static thread_local mbstate_t internal_state = {0,0,0};

	return (int)wcrtomb(s, wc, &internal_state);
}

static inline size_t mbrlen(const char *s, size_t n, mbstate_t *ps) {
	if (!s) {
		mbstate_t *st = __jacl_mbstate(ps);

		st->wc = 0; st->bytes = 0; st->want = 0;

		return 0;
	}

	if (n == 0) return (size_t)-2;

	unsigned char c = (unsigned char)s[0];

	if (c < 0x80) return c ? 1 : 0; // POSIX: Return 0 for the null character

	if ((c & 0xE0) == 0xC0) {
		if (n < 2) return (size_t)-2;
		if (((unsigned char)s[1] & 0xC0) != 0x80) return (__errno_set(EILSEQ), (size_t)-1);
		if ((c & 0x1E) == 0) return (__errno_set(EILSEQ), (size_t)-1);

		return 2;
	}

	if ((c & 0xF0) == 0xE0) {
		if (n < 3) return (size_t)-2;
		if (((unsigned char)s[1] & 0xC0) != 0x80 || ((unsigned char)s[2] & 0xC0) != 0x80) return (__errno_set(EILSEQ), (size_t)-1);
		if (c == 0xE0 && ((unsigned char)s[1] & 0x20) == 0) return (__errno_set(EILSEQ), (size_t)-1);
		if (c == 0xED && ((unsigned char)s[1] & 0x20) != 0) return (__errno_set(EILSEQ), (size_t)-1);

		return 3;
	}

	if ((c & 0xF8) == 0xF0) {
		if (n < 4) return (size_t)-2;
		if (((unsigned char)s[1] & 0xC0) != 0x80 || ((unsigned char)s[2] & 0xC0) != 0x80 || ((unsigned char)s[3] & 0xC0) != 0x80) return (__errno_set(EILSEQ), (size_t)-1);
		if (c == 0xF0 && ((unsigned char)s[1] & 0x30) == 0) return (__errno_set(EILSEQ), (size_t)-1);
		if (c > 0xF4 || (c == 0xF4 && ((unsigned char)s[1] & 0x30) != 0)) return (__errno_set(EILSEQ), (size_t)-1);

		return 4;
	}

	return (__errno_set(EILSEQ), (size_t)-1);
}

static inline int mbsinit(const mbstate_t *ps) {
	if (!ps) return 1;

	static const mbstate_t zero_state = {0, 0, 0};

	return memcmp(ps, &zero_state, sizeof(mbstate_t)) == 0;
}

/* ===== Stateless Buffer Conversion ===== */
static inline size_t mbstowcs(wchar_t *pw, const char *s, size_t n) {
	if (!s) return 0;

	size_t i = 0;
	int b;

	while ((!pw || i < n) && *s) {
		b = mbtowc(pw ? &pw[i] : NULL, s, MB_CUR_MAX);

		if (b < 0) return (size_t)-1;
		if (b == 0) break;

		s += b;
		i++;
	}

	return i;
}

static inline size_t wcstombs(char *s, const wchar_t *pw, size_t n) {
	if (!pw) return 0;

	size_t i = 0, t = 0, l;
	char mb[MB_CUR_MAX];

	while (pw[i]) {
		l = wctomb(mb, pw[i]);
		if (l == (size_t)-1) return (size_t)-1;

		if (s && t + l > n) break;

		if (s) memcpy(s + t, mb, l);

		t += l;
		i++;
	}

	return t;
}

static inline int mblen(const char *s, size_t n) { return mbtowc(NULL, s, n); }

/* ===== Restartable Buffer Conversion ===== */
static inline size_t mbsrtowcs(wchar_t * restrict dst, const char ** restrict src, size_t len, mbstate_t * restrict ps) {
	size_t i = 0;
	const char *s = *src;

	while (i < len || !dst) {
		if (!*s) {
			if (dst) dst[i] = L'\0';

			*src = NULL;

			return i;
		}

		wchar_t wc;
		size_t b = mbrtowc(&wc, s, MB_CUR_MAX, ps);

		if (b == (size_t)-1 || b == (size_t)-2) { *src = s; return (size_t)-1; }
		if (dst) dst[i] = wc;

		s += b;
		i++;
	}

	*src = s;

	return i;
}

static inline size_t wcsrtombs(char * restrict dst, const wchar_t ** restrict src, size_t len, mbstate_t * restrict ps) {
	size_t i = 0;
	const wchar_t *s = *src;
	char mb[MB_CUR_MAX];

	while (*s) {
		size_t b = wcrtomb(mb, *s, ps);

		if (b == (size_t)-1) { *src = s; return (size_t)-1; }
		if (dst && i + b > len) break;
		if (dst) memcpy(dst + i, mb, b);

		i += b;
		s++;
	}

	if (dst && i < len) dst[i] = '\0';
	if (*s == L'\0') *src = NULL;
	else *src = s;

	return i;
}

static inline size_t mbsnrtowcs(wchar_t * restrict dst, const char ** restrict src, size_t nms, size_t len, mbstate_t * restrict ps) {
	size_t i = 0;
	const char *s = *src;
	const char *end = s + nms;

	while (i < len || !dst) {
		if (s >= end || !*s) {
			if (dst && i < len) dst[i] = L'\0';
			if (!*s) *src = NULL; else *src = s;

			return i;
		}
		wchar_t wc;
		size_t b = mbrtowc(&wc, s, end - s, ps);

		if (b == (size_t)-1 || b == (size_t)-2) { *src = s; return (size_t)-1; }
		if (dst) dst[i] = wc;

		s += b;
		i++;
	}

	*src = s;

	return i;
}

static inline size_t wcsnrtombs(char * restrict dst, const wchar_t ** restrict src, size_t nwc, size_t len, mbstate_t * restrict ps) {
	size_t i = 0;
	const wchar_t *s = *src;
	const wchar_t *end = s + nwc;
	char mb[MB_CUR_MAX];

	while (s < end && *s) {
		size_t b = wcrtomb(mb, *s, ps);

		if (b == (size_t)-1) { *src = s; return (size_t)-1; }
		if (dst && i + b > len) break;
		if (dst) memcpy(dst + i, mb, b);

		i += b;
		s++;
	}

	if (dst && i < len) dst[i] = '\0';

	// Only NULL the source pointer if we hit the string's null terminator
	if (*s == L'\0') *src = NULL;
	else *src = s;

	return i;
}

/* ===== Wide Memory/String Operations ===== */
static inline wchar_t *wmemcpy(wchar_t * restrict d, const wchar_t * restrict s, size_t n) {
	if (!d || !s) return NULL;

	for (size_t i = 0; i < n; i++) d[i] = s[i];

	return d;
}

static inline wchar_t *wmemmove(wchar_t *d, const wchar_t *s, size_t n) {
	if (!d || !s) return NULL;
	if (d < s) for (size_t i = 0; i < n; i++) d[i] = s[i];
	else for (size_t i = n; i-- > 0;) d[i] = s[i];

	return d;
}

static inline wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t n) {
	if (!s) return NULL;

	for (size_t i = 0; i < n; i++) if (s[i] == c) return (wchar_t *)&s[i];

	return NULL;
}

static inline int wmemcmp(const wchar_t *a, const wchar_t *b, size_t n) {
	if (!a || !b) return a == b ? 0 : (a ? 1 : -1);

	for (size_t i = 0; i < n; i++) if (a[i] != b[i]) return a[i] < b[i] ? -1 : 1;

	return 0;
}

static inline wchar_t *wmemset(wchar_t *s, wchar_t c, size_t n) {
	if (!s) return NULL;

	for (size_t i = 0; i < n; i++) s[i] = c;

	return s;
}

/* ===== Wide String Length & Copy ===== */
static inline size_t wcslen(const wchar_t *s) {
	if (!s) return 0;

	const wchar_t *p = s;

	while (*p) ++p;

	return (size_t)(p - s);
}

static inline size_t wcsnlen(const wchar_t *ws, size_t maxlen) {
	if (!ws) return 0;

	size_t i = 0;

	while (i < maxlen && ws[i]) i++;

	return i;
}

static inline wchar_t *wcscpy(wchar_t * restrict d, const wchar_t * restrict s) {
	if (!d || !s) return NULL;

	wchar_t *p = d;

	while ((*p++ = *s++));

	return d;
}

static inline wchar_t *wcsncpy(wchar_t * restrict d, const wchar_t * restrict s, size_t n) {
	if (!d || !s) return NULL;

	size_t i = 0;

	for (; i < n && s[i]; i++) d[i] = s[i];
	for (; i < n; i++) d[i] = 0;

	return d;
}

/* ===== Wide String Comparison ===== */
static inline int wcscmp(const wchar_t *a, const wchar_t *b) {
	if (!a || !b) return a == b ? 0 : (a ? 1 : -1);

	while (*a && *a == *b) { a++; b++; }

	return (*a < *b) ? -1 : (*a > *b);
}

static inline int wcsncmp(const wchar_t *a, const wchar_t *b, size_t n) {
	if (!a || !b) return a == b ? 0 : (a ? 1 : -1);

	size_t i = 0;

	for (; i < n && a[i] && a[i] == b[i]; i++);

	if (i == n) return 0;

	return (a[i] < b[i]) ? -1 : 1;
}

/* ===== Wide String Concatenation ===== */
static inline wchar_t *wcscat(wchar_t * restrict ws1, const wchar_t * restrict ws2) {
	if (!ws1 || !ws2) return NULL;

	wchar_t *d = ws1;

	while (*d) d++;
	while ((*d++ = *ws2++));

	return ws1;
}

static inline wchar_t *wcsncat(wchar_t * restrict ws1, const wchar_t * restrict ws2, size_t n) {
	if (!ws1 || !ws2) return NULL;

	wchar_t *d = ws1;

	while (*d) d++;

	size_t i;

	for (i = 0; i < n && ws2[i]; i++) d[i] = ws2[i];

	d[i] = L'\0';

	return ws1;
}

/* ===== Wide String Search ===== */
static inline wchar_t *wcschr(const wchar_t *s, wchar_t c) {
	if (!s) return NULL;

	for (;; s++) {
		if (*s == c) return (wchar_t *)s;
		if (!*s) break;
	}

	return NULL;
}

static inline wchar_t *wcsrchr(const wchar_t *s, wchar_t c) {
	if (!s) return NULL;

	const wchar_t *r = NULL;

	for (;; s++) {
		if (*s == c) r = s;
		if (!*s) break;
	}

	return (wchar_t *)r;
}

static inline size_t wcscspn(const wchar_t *ws1, const wchar_t *ws2) {
	if (!ws1) return 0;

	size_t i = 0;

	while (ws1[i]) {
		if (ws2 && wcschr(ws2, ws1[i])) return i;

		i++;
	}

	return i;
}

static inline size_t wcsspn(const wchar_t *ws1, const wchar_t *ws2) {
	if (!ws1 || !ws2) return 0;

	size_t i = 0;

	while (ws1[i] && wcschr(ws2, ws1[i])) i++;

	return i;
}

static inline wchar_t *wcspbrk(const wchar_t *ws1, const wchar_t *ws2) {
	if (!ws1 || !ws2) return NULL;

	while (*ws1) {
		if (wcschr(ws2, *ws1)) return (wchar_t *)ws1;
		else ws1++;
	}

	return NULL;
}

static inline wchar_t *wcsstr(const wchar_t *h, const wchar_t *n) {
	if (!h || !n) return NULL;

	size_t l = wcslen(n);

	if (!l) return (wchar_t *)h;

	for (; *h; h++) {
		if (!wcsncmp(h, n, l)) return (wchar_t *)h;
	}

	return NULL;
}

static inline wchar_t *wcpcpy(wchar_t * restrict ws1, const wchar_t * restrict ws2) {
	if (!ws1 || !ws2) return NULL;

	while ((*ws1 = *ws2)) { ws1++; ws2++; }

	return ws1;
}

static inline wchar_t *wcpncpy(wchar_t * restrict ws1, const wchar_t * restrict ws2, size_t n) {
	if (!ws1 || !ws2) return NULL;

	size_t i = 0;

	while (i < n && ws2[i]) { ws1[i] = ws2[i]; i++; }

	wchar_t *ret = ws1 + i;

	while (i < n) { ws1[i] = L'\0'; i++; }

	return ret;
}

static inline size_t wcslcpy(wchar_t * restrict dst, const wchar_t * restrict src, size_t dstsize) {
	size_t i = 0;

	if (dstsize > 0) {
		while (i < dstsize - 1 && src[i]) { dst[i] = src[i]; i++; }

		dst[i] = L'\0';
	}

	while (src[i]) i++;

	return i;
}

static inline size_t wcslcat(wchar_t * restrict dst, const wchar_t * restrict src, size_t dstsize) {
	size_t dstlen = 0;

	while (dstlen < dstsize && dst[dstlen]) dstlen++;

	if (dstlen == dstsize) return dstlen + wcslen(src);

	size_t i = 0;

	while (dstlen + i < dstsize - 1 && src[i]) { dst[dstlen + i] = src[i]; i++; }

	dst[dstlen + i] = L'\0';

	return dstlen + wcslen(src);
}

static inline wchar_t *wcsdup(const wchar_t *ws) {
	if (!ws) return (__errno_set(EINVAL), NULL);

	size_t len = wcslen(ws), bytes = (len + 1) * sizeof(wchar_t);
	wchar_t *p = (wchar_t *)malloc(bytes);

	return p ? (wchar_t *)wmemcpy(p, ws, len + 1) : NULL;
}

static inline wchar_t *wcstok(wchar_t * restrict ws1, const wchar_t * restrict ws2, wchar_t ** restrict ptr) {
	if (!ws2 || !ptr) return NULL;

	const wchar_t *s = ws1 ? ws1 : *ptr;

	if (!s) return NULL;

	// Skip leading delimiters (characters IN ws2)
	s += wcsspn(s, ws2);

	if (*s == L'\0') {
		*ptr = NULL;

		return NULL;
	}

	// Start of the token
	wchar_t *token = (wchar_t *)s;

	// Find the end of the token (characters NOT IN ws2)
	s += wcscspn(s, ws2);

	if (*s != L'\0') {
		*((wchar_t *)s) = L'\0';
		*ptr = (wchar_t *)(s + 1);
	} else {
		*ptr = NULL;
	}

	return token;
}

/* ===== Wide Collation & Transformation ===== */
static inline int wcscoll_l(const wchar_t *a, const wchar_t *b, locale_t loc) {
	if (!a || !b) return a == b ? 0 : (a ? 1 : -1);
	const __jacl_collate_t *coll = &loc->collate;

	while (*a && *b) {
		uint_least32_t wa = __jacl_compare_weight(coll, *a);
		uint_least32_t wb = __jacl_compare_weight(coll, *b);

		if (wa != wb) return (wa > wb) - (wa < wb);

		a++; b++;
	}

	return (*a > *b) - (*a < *b);
}
static inline int wcscasecmp_l(const wchar_t *ws1, const wchar_t *ws2, locale_t locale) {
	if (!ws1 || !ws2) return ws1 == ws2 ? 0 : (ws1 ? 1 : -1);

	while (*ws1 && *ws2) {
		wint_t c1 = towlower_l(*ws1, locale);
		wint_t c2 = towlower_l(*ws2, locale);

		if (c1 != c2) return (c1 > c2) - (c1 < c2);

		ws1++;
		ws2++;
	}

	wint_t c1 = *ws1 ? towlower_l(*ws1, locale) : 0;
	wint_t c2 = *ws2 ? towlower_l(*ws2, locale) : 0;

	return (c1 > c2) - (c1 < c2);
}
static inline int wcsncasecmp_l(const wchar_t *ws1, const wchar_t *ws2, size_t n, locale_t locale) {
	if (!ws1 || !ws2) return ws1 == ws2 ? 0 : (ws1 ? 1 : -1);
	if (n == 0) return 0;

	while (n > 1 && *ws1 && *ws2) {
		wint_t c1 = towlower_l(*ws1, locale);
		wint_t c2 = towlower_l(*ws2, locale);

		if (c1 != c2) return (c1 > c2) - (c1 < c2);

		ws1++;
		ws2++;
		n--;
	}

	wint_t c1 = *ws1 ? towlower_l(*ws1, locale) : 0;
	wint_t c2 = *ws2 ? towlower_l(*ws2, locale) : 0;

	return (c1 > c2) - (c1 < c2);
}
static inline size_t wcsxfrm_l(wchar_t * restrict ws1, const wchar_t * restrict ws2, size_t n, locale_t locale) {
	if (!ws2) return 0;

	const __jacl_collate_t *coll = &locale->collate;
	size_t i = 0;

	while (ws2[i]) {
		uint_least32_t w = __jacl_compare_weight(coll, ws2[i]);

		if (ws1 && i < n) ws1[i] = (wchar_t)w;

		i++;
	}

	if (ws1 && n > 0) ws1[(i < n) ? i : n - 1] = L'\0';

	return i;
}
static inline int wcscoll(const wchar_t *a, const wchar_t *b) { return wcscoll_l(a, b, __jacl_locale_current); }
static inline int wcscasecmp(const wchar_t *ws1, const wchar_t *ws2) { return wcscasecmp_l(ws1, ws2, __jacl_locale_current); }
static inline int wcsncasecmp(const wchar_t *ws1, const wchar_t *ws2, size_t n) { return wcsncasecmp_l(ws1, ws2, n, __jacl_locale_current); }
static inline size_t wcsxfrm(wchar_t * restrict ws1, const wchar_t * restrict ws2, size_t n) { return wcsxfrm_l(ws1, ws2, n, __jacl_locale_current); }

/* ===== Encoding/Decoding Utilities ===== */
static inline wint_t btowc(int c) { return (c == EOF || c < 0 || c > 0x7F) ? WEOF : (wint_t)c; }
static inline int wctob(wint_t wc) { return wc == WEOF || wc > 0xFF ? EOF : (int)wc; }

/* ===== Wide Character I/O ===== */
static inline int fwide(FILE *f, int mode) {
	if (!f) return 0;

	flockfile(f);

	if (f->_orientation == 0 && mode != 0) f->_orientation = (mode > 0) ? 2 : 1;

	int ret = f->_orientation;

	funlockfile(f);

	return ret;
}
static inline wint_t fputwc(wchar_t wc, FILE* f) {
	if (!f) return (__errno_set(EBADF), WEOF);

	flockfile(f);

	if (f->_orientation == 1) { funlockfile(f); return (__errno_set(EINVAL), WEOF); }
	if (f->_orientation == 0) f->_orientation = 2;

	char mb[MB_CUR_MAX];
	mbstate_t ps = {0};
	size_t len = wcrtomb(mb, wc, &ps);

	if (len == (size_t)-1) { f->_flags |= __SERR; funlockfile(f); return WEOF; }

	for (size_t i = 0; i < len; ++i) {
		if (fputc_unlocked((unsigned char)mb[i], f) == EOF) {
			funlockfile(f);

			return WEOF;
		}
	}

	funlockfile(f);

	return wc;
}
static inline wint_t fgetwc(FILE* f) {
	if (!f) return (__errno_set(EBADF), WEOF);
	flockfile(f);
	if (f->_orientation == 1) { funlockfile(f); return (__errno_set(EINVAL), WEOF); }
	if (f->_orientation == 0) f->_orientation = 2;

	char mb[MB_CUR_MAX];
	wchar_t wc;
	mbstate_t ps = {0};

	for (size_t i = 0; i < MB_CUR_MAX; i++) {
		int c = fgetc_unlocked(f);

		if (c == EOF) {
			if (i == 0) {
				funlockfile(f);

				return WEOF;
			}

			f->_flags |= __SERR;

			funlockfile(f);

			return (__errno_set(EILSEQ), WEOF);
		}

		mb[i] = (char)c;

		size_t r = mbrtowc(&wc, mb, i + 1, &ps);

		if (r == (size_t)-1) {
			f->_flags |= __SERR;

			funlockfile(f);

			return WEOF;
		}

		if (r != (size_t)-2) {
			funlockfile(f);

			return wc;
		}
	}

	f->_flags |= __SERR;

	funlockfile(f);

	return (__errno_set(EILSEQ), WEOF);
}
static inline wint_t ungetwc(wint_t wc, FILE* f) {
	if (!f || wc == WEOF) return WEOF;

	flockfile(f);

	if (f->_orientation == 1) { funlockfile(f); return (__errno_set(EINVAL), WEOF); }
	if (f->_orientation == 0) f->_orientation = 2;

	char mb[MB_CUR_MAX];
	mbstate_t ps = {0};
	size_t len = wcrtomb(mb, (wchar_t)wc, &ps);

	if (len == (size_t)-1) { funlockfile(f); return WEOF; }
	if (__jacl_switch_to_read(f) < 0) { funlockfile(f); return WEOF; }

	for (size_t i = len; i > 0; i--) {
		if (f->_ptr == f->_base) { funlockfile(f); return WEOF; }

		*(--f->_ptr) = (unsigned char)mb[i-1];
		f->_cnt++;
	}

	f->_flags &= ~__SEOF;

	funlockfile(f);

	return wc;
}
static inline int fputws(const wchar_t* ws, FILE* f) {
	if (!ws || !f) return (__errno_set(EINVAL), -1);

	while (*ws) {
		if (fputwc(*ws++, f) == WEOF) return -1;
	}

	return 0;
}
static inline wchar_t* fgetws(wchar_t* ws, int n, FILE* f) {
	if (!ws || !f || n <= 0) return (__errno_set(EINVAL), NULL);

	int i = 0;
	wint_t wc;

	while (i + 1 < n && (wc = fgetwc(f)) != WEOF) {
		ws[i++] = (wchar_t)wc;

		if (wc == L'\n') break;
	}

	if (i == 0 && wc == WEOF) return NULL;

	ws[i] = L'\0';

	return ws;
}
static inline wint_t putwc(wchar_t wc, FILE* f) { return fputwc(wc, f); }
static inline wint_t getwc(FILE* f) { return fgetwc(f); }
static inline wint_t putwchar(wchar_t wc) { return fputwc(wc, stdout); }
static inline wint_t getwchar(void) { return fgetwc(stdin); }

/* ===== Wide Character Terminal Ruler ===== */
static inline int wcwidth(wchar_t ucs) {
	if (ucs == 0) return 0;
	if (ucs < 0x20 || (ucs >= 0x7f && ucs < 0xa0)) return -1;

	// Simplified CJK double-width check
	if (ucs >= 0x1100 &&
		(ucs <= 0x115f || ucs == 0x2329 || ucs == 0x232a ||
		(ucs >= 0x2e80 && ucs <= 0xa4cf && ucs != 0x303f) ||
		(ucs >= 0xac00 && ucs <= 0xd7a3) ||
		(ucs >= 0xf900 && ucs <= 0xfaff) ||
		(ucs >= 0xfe10 && ucs <= 0xfe19) ||
		(ucs >= 0xfe30 && ucs <= 0xfe6f) ||
		(ucs >= 0xff00 && ucs <= 0xff60) ||
		(ucs >= 0xffe0 && ucs <= 0xffe6) ||
		(ucs >= 0x20000 && ucs <= 0x2fffd) ||
		(ucs >= 0x30000 && ucs <= 0x3fffd)))

		return 2;

	return 1;
}

static inline int wcswidth(const wchar_t *pwcs, size_t n) {
	if (!pwcs) return 0;

	int w = 0;

	for (size_t i = 0; i < n && pwcs[i]; i++) {
		int c = wcwidth(pwcs[i]);

		if (c < 0) return -1;

		w += c;
	}

	return w;
}

/* TODO: need to impliment the backing stuff

FILE *open_wmemstream(wchar_t **bufp, size_t *sizep);

// ===== Wide Character Printf/Scanf =====
int vwprintf(const wchar_t * restrict fmt, va_list ap);
int wprintf(const wchar_t * restrict fmt, ...);
int vfwprintf(FILE * restrict stream, const wchar_t * restrict fmt, va_list ap);
int fwprintf(FILE * restrict stream, const wchar_t * restrict fmt, ...);
int vswprintf(wchar_t * restrict s, size_t n, const wchar_t * restrict fmt, va_list ap);
int swprintf(wchar_t * restrict s, size_t n, const wchar_t * restrict fmt, ...);

int vwscanf(const wchar_t * restrict fmt, va_list ap);
int wscanf(const wchar_t * restrict fmt, ...);
int vfwscanf(FILE * restrict stream, const wchar_t * restrict fmt, va_list ap);
int fwscanf(FILE * restrict stream, const wchar_t * restrict fmt, ...);
int vswscanf(const wchar_t * restrict s, const wchar_t * restrict fmt, va_list ap);
int swscanf(const wchar_t * restrict s, const wchar_t * restrict fmt, ...);

// ===== Wide Character Number Parsing =====
long wcstol(const wchar_t * restrict nptr, wchar_t ** restrict endptr, int base);
long long wcstoll(const wchar_t * restrict nptr, wchar_t ** restrict endptr, int base);
unsigned long wcstoul(const wchar_t * restrict nptr, wchar_t ** restrict endptr, int base);
unsigned long long wcstoull(const wchar_t * restrict nptr, wchar_t ** restrict endptr, int base);
double wcstod(const wchar_t * restrict nptr, wchar_t ** restrict endptr);
float wcstof(const wchar_t * restrict nptr, wchar_t ** restrict endptr);
long double wcstold(const wchar_t * restrict nptr, wchar_t ** restrict endptr);

// ===== Wide Character Time =====
size_t wcsftime(wchar_t * restrict ws, size_t maxsize, const wchar_t * restrict format, const struct tm * restrict timeptr);

*/

#endif /* JACL_HAS_C99 */

#ifdef __cplusplus
}
#endif

#endif /* _WCHAR_H */
