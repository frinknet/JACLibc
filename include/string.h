/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _STRING_H
#define _STRING_H
#pragma once

#include <config.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <signal.h>
#include <sys/types.h>

#if JACL_HAS_C23
#define __STDC_VERSION_STRING_H__ 202311L
#endif

#ifdef __cplusplus
extern "C" {
#endif

void* malloc(size_t);

/* Memory Operations */
static inline void* memset(void* s, int c, size_t n) {
	if (JACL_UNLIKELY(!s)) return NULL;

	unsigned char* p = (unsigned char* )s;

	for (size_t i = 0; i < n; i++) p[i] = (unsigned char)c;

	return s;
}

static inline void* memmove(void* dest, const void* src, size_t n) {
	if (JACL_UNLIKELY(!dest || !src)) return NULL;

	unsigned char* d = (unsigned char* )dest;
	const unsigned char* s = (const unsigned char* )src;

	if (d < s) {
		for (size_t i = 0; i < n; i++) d[i] = s[i];
	} else {
		for (size_t i = n; i-- > 0;) d[i] = s[i];
	}

	return dest;
}

static inline int memcmp(const void* a, const void* b, size_t n) {
	if (!a || !b) return a == b ? 0 : (a ? 1 : -1);

	const unsigned char* x = (const unsigned char* )a;
	const unsigned char* y = (const unsigned char* )b;

	for (size_t i = 0; i < n; i++) {
		if (x[i] != y[i]) return x[i] < y[i] ? -1 : 1;
	}

	return 0;
}

static inline void* memchr(const void* s, int c, size_t n) {
	if (!s) return NULL;

	const unsigned char* p = (const unsigned char* )s;

	for (size_t i = 0; i < n; i++) {
		if (p[i] == (unsigned char)c) return (void* )&p[i];
	}

	return NULL;
}

static inline void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
	if (JACL_UNLIKELY(!dest || !src)) return NULL;

	unsigned char* d = (unsigned char* )dest;
	const unsigned char* s = (const unsigned char* )src;

	for (size_t i = 0; i < n; i++) d[i] = s[i];

	return dest;
}

static inline void* memmem(const void* hay, size_t hl, const void* ndl, size_t nl) {
	if (!hay || !ndl || nl > hl) return NULL;

	const unsigned char* h = (const unsigned char* )hay;

	for (size_t i = 0; i <= hl - nl; i++) {
		if (!memcmp(h + i, ndl, nl)) return (void* )(h + i);
	}

	return NULL;
}

static inline void* memccpy(void* restrict dest, const void* restrict src, int c, size_t n) {
	if (!dest || !src) return NULL;

	unsigned char* d = (unsigned char*)dest;
	const unsigned char* s = (const unsigned char*)src;

	for (size_t i = 0; i < n; i++) {
		d[i] = s[i];

		if (s[i] == (unsigned char)c) return d + i + 1;
	}

	return NULL;
}

/* UTF-8–aware String Length & Copy */
static inline size_t strlen(const char* s) {
	if (!s) return 0;

	size_t len = 0;

	while (s[len]) len++;

	return len;
}

static inline size_t strnlen(const char* s, size_t n) {
	if (!s) return 0;

	size_t i = 0;

	while (i < n && s[i]) i++;

	return i;
}

static inline char* strcpy(char* restrict dest, const char* src) {
	if (!dest || !src) return NULL;

	char* p = dest;

	while ((*p++ = *src++));

	return dest;
}

static inline char* strncpy(char* restrict dest, const char* src, size_t n) {
	if (!dest || !src) return NULL;

	size_t i = 0;

	for (; i < n && src[i]; i++) dest[i] = src[i];
	for (; i < n; i++) dest[i] = '\0';

	return dest;
}

static inline char* strcat(char* restrict dest, const char* src) {
	if (!dest || !src) return NULL;

	char* p = dest + strlen(dest);

	while ((*p++ = *src++));

	return dest;
}

static inline char* strncat(char* restrict dest, const char* src, size_t n) {
	if (!dest || !src) return NULL;

	char* p = dest + strlen(dest);
	size_t i = 0;

	for (; i < n && src[i]; i++) p[i] = src[i];

	p[i] = '\0';

	return dest;
}

static inline size_t strlcpy(char * restrict dest, const char * src, size_t n) {
	if (!src) return 0;

	size_t len = strlen(src);

	if (dest && n > 0) {
		size_t end = (len >= n) ? n - 1 : len;

		memcpy(dest, src, end);

		dest[end] = '\0';
	}

	return len;
}

static inline size_t strlcat(char * restrict dest, const char * src, size_t n) {
	if (!src) return dest ? strlen(dest) : 0;

	size_t slen = strlen(src);

	if (!dest || n == 0) return slen;

	size_t dlen = strlen(dest);

	if (dlen >= n) return n + slen;

	size_t end = ((dlen + slen) >= n) ? (n - dlen - 1) : slen;

	memcpy(dest + dlen, src, end);

	dest[dlen + end] = '\0';

	return dlen + slen;
}

static inline char* stpcpy(char* restrict dest, const char* restrict src) {
	if (!dest || !src) return NULL;

	while ((*dest = *src)) { dest++; src++; }

	return dest;
}

static inline char* stpncpy(char* restrict dest, const char* restrict src, size_t n) {
	if (!dest || !src) return NULL;

	size_t i;

	for (i = 0; i < n && src[i]; i++) dest[i] = src[i];

	char* ret = dest + i;

	for (; i < n; i++) dest[i] = '\0';

	return ret;
}

/* Comparison & Collation */
static inline int strcmp(const char* a, const char* b) {
	if (!a || !b) return a == b ? 0 : (a ? 1 : -1);

	while (*a && *a == *b) { a++; b++; }

	return (*a < *b) ? -1 : (*a > *b);
}

static inline int strncmp(const char* a, const char* b, size_t n) {
	if (!a || !b) return a == b ? 0 : (a ? 1 : -1);

	size_t i = 0;

	for (; i < n && a[i] && a[i] == b[i]; i++);

	if (i == n) return 0;

	return (a[i] < b[i]) ? -1 : 1;
}

static inline int strcoll_l(const char *a, const char *b, locale_t loc) {
	if (!a || !b) return a == b ? 0 : (a ? 1 : -1);

	const __jacl_collate_t *coll = &loc->collate;

	while (*a && *b) {
		int res = __jacl_compare_glyph(coll, __jacl_compare_decode(&a), __jacl_compare_decode(&b));

		if (res != 0) return res;
	}

	return (*a != '\0') - (*b != '\0');
}

/* TODO: make this use locale properly */
static inline size_t strxfrm_l(char * restrict dest, const char * restrict src, size_t n, locale_t loc) {
	if (!src) return 0;
	(void)loc;

	size_t l = strlen(src);

	if (dest && n > 0) {
		size_t copy_len = (l < n) ? l + 1 : n;

		memcpy(dest, src, copy_len);
	}

	return l;
}

static inline int strcoll(const char *a, const char *b) { return strcoll_l(a, b, __jacl_locale_current); }
static inline size_t strxfrm(char * restrict dest, const char * restrict src, size_t n) { return strxfrm_l(dest, src, n, __jacl_locale_current); }

/* Thread-safe strerror & strerror_r */
#if JACL_HAS_C11
static thread_local char _jacl_errbuf[64];
#else
static __thread char _jacl_errbuf[64];
#endif

static inline char* strerror(int err) {
	const char* msg;

	switch (err) {
		case 0:             msg = "Success"; break;
		case EPERM:         msg = "Operation not permitted"; break;
		case ENOENT:        msg = "No such file or directory"; break;
		case ESRCH:         msg = "No such process"; break;
		case EINTR:         msg = "Interrupted system call"; break;
		case EIO:           msg = "I/O error"; break;
		case ENXIO:         msg = "No such device or address"; break;
		case E2BIG:         msg = "Argument list too long"; break;
		case ENOEXEC:       msg = "Exec format error"; break;
		case EBADF:         msg = "Bad file descriptor"; break;
		case ECHILD:        msg = "No child processes"; break;
		case EAGAIN:        msg = "Try again"; break;
		case ENOMEM:        msg = "Out of memory"; break;
		case EACCES:        msg = "Permission denied"; break;
		case EFAULT:        msg = "Bad address"; break;
		case ENOTBLK:       msg = "Block device required"; break;
		case EBUSY:         msg = "Device or resource busy"; break;
		case EEXIST:        msg = "File exists"; break;
		case EXDEV:         msg = "Cross-device link"; break;
		case ENODEV:        msg = "No such device"; break;
		case ENOTDIR:       msg = "Not a directory"; break;
		case EISDIR:        msg = "Is a directory"; break;
		case EINVAL:        msg = "Invalid argument"; break;
		case ENFILE:        msg = "File table overflow"; break;
		case EMFILE:        msg = "Too many open files"; break;
		case ENOTTY:        msg = "Not a TTY"; break;
		case ETXTBSY:       msg = "Text file busy"; break;
		case EFBIG:         msg = "File too large"; break;
		case ENOSPC:        msg = "No space left on device"; break;
		case ESPIPE:        msg = "Illegal seek"; break;
		case EROFS:         msg = "Read-only file system"; break;
		case EMLINK:        msg = "Too many links"; break;
		case EPIPE:         msg = "Pipe broken"; break;
		case EDOM:          msg = "Outside of domain"; break;
		case ERANGE:        msg = "Outside of range"; break;
		case EDEADLK:       msg = "Resource deadlock avoided"; break;
		case ENAMETOOLONG:  msg = "File name too long"; break;
		case ENOLCK:        msg = "No locks available"; break;
		case ENOSYS:        msg = "Function not implemented"; break;
		case ENOTEMPTY:     msg = "Directory not empty"; break;
		default:            msg = "Unknown error"; break;
	}

	size_t len = strlen(msg);

	if (len >= sizeof(_jacl_errbuf)) len = sizeof(_jacl_errbuf) - 1;

	memcpy(_jacl_errbuf, msg, len);
	_jacl_errbuf[len] = '\0';

	return _jacl_errbuf;
}

static inline int strerror_r(int err, char* buf, size_t buflen) {
	if (!buf || buflen == 0) return EINVAL;

	// Reuse existing strerror() - it handles all the switch logic
	const char* msg = strerror(err);
	size_t len = strlen(msg);

	if (len >= buflen) {
		// Buffer too small - copy what fits and null terminate
		if (buflen > 0) {
			memcpy(buf, msg, buflen - 1);
			buf[buflen - 1] = '\0';
		}

		return ERANGE;
	}

	// Buffer is large enough - copy entire message including null terminator
	memcpy(buf, msg, len + 1);

	return 0;
}

/* Search & Tokenization */
static inline char* strchr(const char* s, int c) {
	if (!s) return NULL;
	for (;; s++) {
		if (*s == (char)c) return (char* )s;
		if (!*s) break;
	}
	return NULL;
}

static inline char* strrchr(const char* s, int c) {
	if (!s) return NULL;

	const char* r = NULL;

	for (;; s++) {
		if (*s == (char)c) r = s;
		if (!*s) break;
	}

	return (char* )r;
}

static inline char* strstr(const char* h, const char* n) {
	if (!h || !n) return NULL;

	size_t nl = strlen(n);

	if (!nl) return (char* )h;

	for (; *h; h++) {
		if (!strncmp(h, n, nl)) return (char* )h;
	}

	return NULL;
}

static inline size_t strspn(const char* s, const char* accept) {
	if (!s || !accept) return 0;

	size_t i = 0;

	while (s[i] && strchr(accept, s[i])) i++;

	return i;
}

static inline size_t strcspn(const char* s, const char* reject) {
	if (!s) return 0;

	size_t i = 0;

	while (s[i] && !strchr(reject, s[i])) i++;

	return i;
}

static inline char* strpbrk(const char* s, const char* accept) {
	if (!s || !accept) return NULL;

	for (; *s; s++) if (strchr(accept, *s)) return (char* )s;

	return NULL;
}

static inline char* strtok_r(char* s, const char* delim, char** save) {
	if (!delim || !save) return NULL;

	char* p = s ? s : *save;

	if (!p) return NULL;

	p += strspn(p, delim);

	if (!*p) { *save = NULL; return NULL; }

	char* q = p + strcspn(p, delim);

	if (*q) *q++ = '\0';

	*save = q;

	return p;
}

static inline char* strtok(char* s, const char* delim) {
	static thread_local char* tls_save;

	return strtok_r(s, delim, &tls_save);
}

#if JACL_HAS_C11
static thread_local char _jacl_sigbuf[64];
#else
static __thread char _jacl_sigbuf[64];
#endif

static inline char* strsignal(int sig) {
	const char* msg;

	switch (sig) {
		case SIGHUP:    msg = "Hangup"; break;
		case SIGINT:    msg = "Interrupt"; break;
		case SIGQUIT:   msg = "Quit"; break;
		case SIGILL:    msg = "Illegal instruction"; break;
		case SIGTRAP:   msg = "Trace/breakpoint trap"; break;
		case SIGABRT:   msg = "Aborted"; break;
		case SIGBUS:    msg = "Bus error"; break;
		case SIGFPE:    msg = "Floating point exception"; break;
		case SIGKILL:   msg = "Killed"; break;
		case SIGUSR1:   msg = "User defined signal 1"; break;
		case SIGSEGV:   msg = "Segmentation fault"; break;
		case SIGUSR2:   msg = "User defined signal 2"; break;
		case SIGPIPE:   msg = "Broken pipe"; break;
		case SIGALRM:   msg = "Alarm clock"; break;
		case SIGTERM:   msg = "Terminated"; break;
		default:        msg = "Unknown signal"; break;
	}

	size_t len = strlen(msg);

	if (len >= sizeof(_jacl_sigbuf)) len = sizeof(_jacl_sigbuf) - 1;

	memcpy(_jacl_sigbuf, msg, len);

	_jacl_sigbuf[len] = '\0';

	return _jacl_sigbuf;
}

static inline char *strdup(const char *s) {
	if (!s) { return (__errno_set(EINVAL), NULL); }
	size_t n = strlen(s) + 1;
	char *p = (char *)malloc(n);
	return p ? (char *)memcpy(p, s, n) : NULL;
}

#if JACL_HAS_C99
static inline char *strndup(const char *s, size_t n) {
	if (!s) { return (__errno_set(EINVAL), NULL); }

	size_t len = strnlen(s, n);
	char *p = (char *)malloc(len + 1);

	if (!p) return NULL;

	memcpy(p, s, len);

	p[len] = '\0';

	return p;
}
#endif

static inline char *strerror_l(int err, locale_t loc) {
	(void)loc;
	return strerror(err);
}

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include <type_traits>
template<typename T> T *memcpy(T*, const T*, size_t) = delete;
template<typename T> T *memmove(T*, const T*, size_t) = delete;
template<typename T> T *strcpy(T*, const char*) = delete;
template<typename T> T *strcat(T*, const char*) = delete;
static_assert(std::is_same<decltype(::memcpy((int*)0, (int*)0, 0)), void*>::value, "Use C memcpy, not template");
#endif

#endif /* _STRING_H */
