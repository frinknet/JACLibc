// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef STRING_H
#define STRING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// — Memory Operations —
static inline void*		memcpy(void *dest, const void *src, size_t n)				{ return __builtin_memcpy(dest, src, n); }
static inline void*		memset(void *s, int c, size_t n)										{ return __builtin_memset(s, c, n); }
static inline void*		memmove(void *dest, const void *src, size_t n)			{ return __builtin_memmove(dest, src, n); }
static inline int			memcmp(const void *a, const void *b, size_t n)			{ return __builtin_memcmp(a, b, n); }
static inline void*		memchr(const void *s, int c, size_t n)							{ return __builtin_memchr(s, c, n); }

#ifdef _GNU_SOURCE
static inline void*		memmem(const void *hay, size_t hl, const void *ndl, size_t nl){
		if (nl > hl) return NULL;

		const char *h = hay;

		for (size_t i = 0; i <= hl - nl; ++i)
				if (!memcmp(h + i, ndl, nl)) return (void*)(h + i);

		return NULL;
}
#endif

// — String Length & Copy —
static inline size_t	strlen(const char* s)																{ return __builtin_strlen(s); }
static inline char*		strdup(const char* s)																{ return __builtin_strdup(s); }
static inline char*		strndup(const char* s, size_t n)										{ return __builtin_strndup(s, n); }
static inline char*		strcpy(char* dest, const char* src)									{ return __builtin_strcpy(dest, src); }
static inline char*		strncpy(char* dest, const char* src, size_t n)			{ return __builtin_strncpy(dest, src, n); }
static inline char*		strcat(char* dest, const char* src)									{ return __builtin_strcat(dest, src); }
static inline char*		strncat(char* dest, const char* src, size_t n)			{ return __builtin_strncat(dest, src, n); }

// — Comparison & Collation —
static inline int			strcmp(const char* a, const char* b)								{ return __builtin_strcmp(a, b); }
static inline int			strncmp(const char* a, const char* b, size_t n)			{ return __builtin_strncmp(a, b, n); }
static inline int			strcasecmp(const char* a, const char* b)						{ return __builtin_strcasecmp(a, b); }
static inline int			strncasecmp(const char* a, const char* b, size_t n)	{ return __builtin_strncasecmp(a, b, n); }
static inline int			strcoll(const char* a, const char* b)								{ return __builtin_strcoll(a, b); }
static inline size_t	strxfrm(char* dest, const char* src, size_t n)			{ return __builtin_strxfrm(dest, src, n); }
static inline char*		strerror(int e) {
		static const char *msgs[] = { "Success", "Perm", "NoEnt", "Acc", "BadF" };

		unsigned u = (unsigned)e;

		return u < (sizeof msgs/sizeof *msgs) ? (char*)msgs[u] : "Unknown";
}

// — Search & Tokenization —
static inline char*		strchr(const char* s, int c)												{ return __builtin_strchr(s, c); }
static inline char*		strrchr(const char* s, int c)												{ return __builtin_strrchr(s, c); }
static inline void*		strstr(const char* h, const char* n)								{ return __builtin_strstr(h, n); }
static inline size_t	strspn(const char* s, const char* accept)						{ return __builtin_strspn(s, accept); }
static inline size_t	strcspn(const char* s, const char* reject)					{ return __builtin_strcspn(s, reject); }
static inline char*		strpbrk(const char* s, const char* accept)					{ return __builtin_strpbrk(s, accept); }
static inline char*		strtok(char *s,const char *delim) { static char *save; return strtok_r(s, delim, &save); }
static inline char*		strtok_r(char *s,const char *delim,char **save) {
	char *p = s ? s : *save;

	if (!p) return NULL;

	p += strspn(p,delim);

	if (!*p) { *save = NULL; return NULL; }

	char *q = p + strcspn(p,delim);

	if (*q) *q++ = '\0';

	*save = q;

	return p;
}

// — BSD-StyIle Safe Copy/Cat —
#if defined(_BSD_SOURCE) || defined(_SUSV2)
static inline size_t strlcpy(char* dest, const char* src, size_t n) {
	size_t len = strlen(src);
	size_t end = (len >= n) ? n - 1 : len;

	__builtin_strncpy(dest, src, end);

	dest[end] = '\0';

	return len;
}
static inline size_t strlcat(char* dest, const char* src, size_t n) {
	size_t dlen = strlen(dest);
	size_t slen = strlen(src);

	if (dlen >= n) return n + slen;

	size_t end = ((dlen + slen) >= n) ? (n - dlen - 1) : slen;

	memcpy(dest + dlen, src, end);

	dest[dlen + end] = '\0';

	return dlen + slen;
}
#endif

// — POSIX basename simple no alloc —
#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 200112L
static inline const char* basename(const char* path) { const char* p = strrchr(path, '/'); return p ? p + 1 : path; }
#endif

#ifdef __cplusplus
}

#include <type_traits>
template<typename T> T *memcpy(T*,const T*,size_t)=delete;
template<typename T> T *memmove(T*,const T*,size_t)=delete;
template<typename T> T *strcpy(T*,const char*)=delete;
template<typename T> T *strcat(T*,const char*)=delete;
static_assert(std::is_same<decltype(::memcpy((int*)0,(int*)0,0)),void*>::value, "Use C memcpy, not template");
#endif

#endif /* STRING_H */
