// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* — Memory Operations — */
static inline void *memcpy(void *dest, const void *src, size_t n) {
		unsigned char *d = (unsigned char *)dest;
		const unsigned char *s = (const unsigned char *)src;
		for (size_t i = 0; i < n; i++) d[i] = s[i];
		return dest;
}

static inline void *memset(void *s, int c, size_t n) {
		unsigned char *p = (unsigned char *)s;
		for (size_t i = 0; i < n; i++) p[i] = (unsigned char)c;
		return s;
}

static inline void *memmove(void *dest, const void *src, size_t n) {
		unsigned char *d = (unsigned char *)dest;
		const unsigned char *s = (const unsigned char *)src;
		if (d < s) {
				for (size_t i = 0; i < n; i++) d[i] = s[i];
		} else {
				for (size_t i = n; i-- > 0;) d[i] = s[i];
		}
		return dest;
}

static inline int memcmp(const void *a, const void *b, size_t n) {
		const unsigned char *x = (const unsigned char *)a;
		const unsigned char *y = (const unsigned char *)b;
		for (size_t i = 0; i < n; i++)
				if (x[i] != y[i]) return x[i] < y[i] ? -1 : 1;
		return 0;
}

static inline void *memchr(const void *s, int c, size_t n) {
		const unsigned char *p = (const unsigned char *)s;
		for (size_t i = 0; i < n; i++)
				if (p[i] == (unsigned char)c) return (void *)&p[i];
		return NULL;
}

#ifdef _GNU_SOURCE
static inline void *memmem(const void *hay, size_t hl, const void *ndl, size_t nl) {
		if (nl > hl) return NULL;
		const unsigned char *h = (const unsigned char *)hay;
		for (size_t i = 0; i <= hl - nl; i++)
				if (!memcmp(h + i, ndl, nl)) return (void *)(h + i);
		return NULL;
}
#endif

/* — UTF-8–aware String Length & Copy — */
static inline size_t strlen(const char *s) {
		const char *p = s;
		while (*p) p++;
		return (size_t)(p - s);
}

static inline size_t strnlen(const char *s, size_t n) {
		size_t i = 0;
		while (i < n && s[i]) i++;
		return i;
}

static inline char *strcpy(char *dest, const char *src) {
		char *p = dest;
		while ((*p++ = *src++));
		return dest;
}

static inline char *strncpy(char *dest, const char *src, size_t n) {
		size_t i = 0;
		for (; i < n && src[i]; i++) dest[i] = src[i];
		for (; i < n; i++) dest[i] = '\0';
		return dest;
}

static inline char *strcat(char *dest, const char *src) {
		char *p = dest + strlen(dest);
		while ((*p++ = *src++));
		return dest;
}

static inline char *strncat(char *dest, const char *src, size_t n) {
		char *p = dest + strlen(dest);
		size_t i = 0;
		for (; i < n && src[i]; i++) p[i] = src[i];
		p[i] = '\0';
		return dest;
}

/* — Comparison & Collation (byte-safe, Unicode case-fold hooks) — */
static inline int strcmp(const char *a, const char *b) {
		while (*a && *a == *b) { a++; b++; }
		return (*a < *b) ? -1 : (*a > *b);
}

static inline int strncmp(const char *a, const char *b, size_t n) {
		size_t i = 0;
		for (; i < n && a[i] && a[i] == b[i]; i++);
		if (i == n) return 0;
		return (a[i] < b[i]) ? -1 : 1;
}

static inline char _jacl_tolower(char c) {
		return (c >= 'A' && c <= 'Z') ? c + 32 : c;
}

static inline int strcasecmp(const char *a, const char *b) {
		while (*a && *b) {
				char ca = _jacl_tolower(*a++);
				char cb = _jacl_tolower(*b++);
				if (ca != cb) return (ca < cb) ? -1 : 1;
		}
		return (*a < *b) ? -1 : (*a > *b);
}

static inline int strncasecmp(const char *a, const char *b, size_t n) {
		for (size_t i = 0; i < n && a[i] && b[i]; i++) {
				char ca = _jacl_tolower(a[i]);
				char cb = _jacl_tolower(b[i]);
				if (ca != cb) return (ca < cb) ? -1 : 1;
		}
		return 0;
}

static inline int strcoll(const char *a, const char *b) {
		return strcmp(a, b);
}

static inline size_t strxfrm(char *dest, const char *src, size_t n) {
		size_t l = strlen(src);
		if (n > l) strcpy(dest, src);
		return l;
}

/* — Thread-safe strerror & strerror_r — */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
static _Thread_local char _jacl_errbuf[64];
#else
static __thread char _jacl_errbuf[64];
#endif

static inline int strerror_r(int err, char *buf, size_t buflen) {
		static const char *msgs[] = { "Success", "Perm", "NoEnt", "Acc", "BadF" };	
		const char *m = (unsigned)err < (sizeof msgs/sizeof *msgs) ? msgs[err] : "Unknown";
		size_t l = strlen(m);
		if (l >= buflen) l = buflen - 1;
		memcpy(buf, m, l);
		buf[l] = '\0';
		return 0;
}

static inline char *strerror(int err) {
		strerror_r(err, _jacl_errbuf, sizeof _jacl_errbuf);
		return _jacl_errbuf;
}

/* — Search & Tokenization — */
static inline char *strchr(const char *s, int c) {
		for (; *s; s++) if (*s == (char)c) return (char *)s;
		return NULL;
}

static inline char *strrchr(const char *s, int c) {
		const char *r = NULL;
		for (; *s; s++) if (*s == (char)c) r = s;
		return (char *)r;
}

static inline char *strstr(const char *h, const char *n) {
		size_t nl = strlen(n);
		if (!nl) return (char *)h;
		for (; *h; h++)
				if (!strncmp(h, n, nl)) return (char *)h;
		return NULL;
}

static inline size_t strspn(const char *s, const char *accept) {
		size_t i = 0;
		while (s[i] && strchr(accept, s[i])) i++;
		return i;
}

static inline size_t strcspn(const char *s, const char *reject) {
		size_t i = 0;
		while (s[i] && !strchr(reject, s[i])) i++;
		return i;
}

static inline char *strpbrk(const char *s, const char *accept) {
		for (; *s; s++) if (strchr(accept, *s)) return (char *)s;
		return NULL;
}

static inline char *strtok_r(char *s, const char *delim, char **save) {
		char *p = s ? s : *save;
		if (!p) return NULL;
		p += strspn(p, delim);
		if (!*p) { *save = NULL; return NULL; }
		char *q = p + strcspn(p, delim);
		if (*q) *q++ = '\0';
		*save = q;
		return p;
}

static inline char *strtok(char *s, const char *delim) {
		static __thread char *tls_save;
		return strtok_r(s, delim, &tls_save);
}

/* — BSD-Style Safe Copy/Cat — */
#if defined(_BSD_SOURCE) || defined(_SUSV2)
static inline size_t strlcpy(char *dest, const char *src, size_t n) {
		size_t len = strlen(src);
		size_t end = (len >= n) ? n - 1 : len;
		memcpy(dest, src, end);
		dest[end] = '\0';
		return len;
}

static inline size_t strlcat(char *dest, const char *src, size_t n) {
		size_t dlen = strlen(dest);
		size_t slen = strlen(src);
		if (dlen >= n) return n + slen;
		size_t end = ((dlen + slen) >= n) ? (n - dlen - 1) : slen;
		memcpy(dest + dlen, src, end);
		dest[dlen + end] = '\0';
		return dlen + slen;
}
#endif

/* — POSIX basename simple no-alloc — */
#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L
static inline const char *basename(const char *path) {
		const char *p = strrchr(path, '/');
		return p ? p + 1 : path;
}
#endif

#ifdef __cplusplus
}
#if __has_include(<type_traits>)
#include <type_traits>
template<typename T> T *memcpy(T*, const T*, size_t) = delete;
template<typename T> T *memmove(T*, const T*, size_t) = delete;
template<typename T> T *strcpy(T*, const char*) = delete;
template<typename T> T *strcat(T*, const char*) = delete;
static_assert(std::is_same<decltype(::memcpy((int*)0, (int*)0, 0)), void*>::value,
							"Use C memcpy, not template");
#endif
#endif

#endif /* STRING_H */
