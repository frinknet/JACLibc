/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STDLIB_H
#define STDLIB_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/* — Configuration — */
#define JACL_DEFAULT_HEAP_SIZE		(64 * 1024)
#define JACL_FASTBIN_MAX	64
#define JACL_ALIGNMENT		8
#define JACL_FL_INDEX_BITS			4
#define JACL_FL_INDEX_COUNT			 (1u << JACL_FL_INDEX_BITS)
#define JACL_ALIGN_UP(sz)		 (((sz) + JACL_ALIGNMENT - 1) & ~(JACL_ALIGNMENT - 1))

void* malloc(size_t n);
void free(void* ptr);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);

/* — Integer Conversion & Parsing — */
static inline int atoi(const char *nptr) { int v=0; sscanf(nptr, "%d", &v); return v; }
static inline long atol(const char *nptr) { long v=0; sscanf(nptr, "%ld", &v); return v; }
static inline long long atoll(const char *nptr) { long long v=0; sscanf(nptr, "%lld", &v); return v; }
static inline double atof(const char *nptr) { double v=0; sscanf(nptr, "%lg", &v); return v; }
static inline long strtol(const char *nptr, char **endptr, int base) {
	long v = 0; int n = 0;

	if (base == 10) sscanf(nptr, "%ld%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
static inline unsigned long strtoul(const char *nptr, char **endptr, int base) {
	unsigned long v = 0; int n = 0;

	if (base == 10) sscanf(nptr, "%lu%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
static inline long long strtoll(const char *nptr, char **endptr, int base) {
	long long v = 0; int n = 0;

	if (base == 10) sscanf(nptr, "%lld%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
static inline unsigned long long strtoull(const char *nptr, char **endptr, int base) {
	unsigned long long v = 0; int n = 0;

	if (base == 10) sscanf(nptr, "%llu%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
static inline float strtof(const char *nptr, char **endptr) {
	float v=0; int n=0;

	sscanf(nptr, "%g%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
static inline double strtod(const char *nptr, char **endptr) {
	double v=0; int n=0;

	sscanf(nptr, "%lg%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
static inline long double strtold(const char *nptr, char **endptr) {
	long double v=0; int n=0;

	sscanf(nptr, "%Lg%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
static inline int mblen(const char *s, size_t n) {
	if (!s) return 1; // C locale, stateless

	if (n == 0) return -1;

	return *s ? 1 : 0;
}

typedef struct { int quot, rem; }				div_t;
typedef struct { long quot, rem; }			ldiv_t;
typedef struct { long long quot, rem;}	lldiv_t;

static inline div_t		div		(int n, int d)			 { return (div_t){ n/d, n%d }; }
static inline ldiv_t	ldiv	(long n, long d)		 { return (ldiv_t){ n/d, n%d }; }
static inline lldiv_t lldiv (long long n, long long d){ return (lldiv_t){ n/d, n%d }; }

/* — Pseudo-Random — */
static unsigned jacl_seed = 1;

static inline void srand(unsigned s) { jacl_seed = s ? s : 1; }
static inline int rand(void) { jacl_seed = jacl_seed * 1103515245u + 12345u; return (int)((jacl_seed >> 16) & 0x7FFF); }

/* — Sorting & Searching — */
void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
void* bsearch(const void* key, const void* base, size_t nmemb, size_t size, int (*compar)(const void*,const void*));

/* — Aligned Alloc (C11) — */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
/* C11 guarantee of aligned_alloc() */
static inline void *aligned_alloc(size_t a, size_t s) {
	(void)a; (void)s;
	return NULL;
}
#elif defined(__has_builtin)
#if __has_builtin(__builtin_aligned_alloc)
static inline void *aligned_alloc(size_t a, size_t s) { return __builtin_aligned_alloc(a, s); }
#endif
#endif

/* — Control & Env — */
static inline void abort(void) { __builtin_trap(); }
static inline void exit(int st)	{ (void)st; abort(); }
static inline char* getenv(const char* n)	{ return NULL; }
static inline int		system(const char* c)	{ return -1; }

static inline char *strdup(const char *s) {
	size_t n = strlen(s) + 1;
	char *p = (char *)malloc(n);

	return p ? (char *)memcpy(p, s, n) : NULL;
}

static inline char *strndup(const char *s, size_t n) {
		size_t len = strnlen(s, n);
		char *p = (char *)malloc(len + 1);
		if (!p) return NULL;
		memcpy(p, s, len);
		p[len] = '\0';
		return p;
}

#ifdef __cplusplus
}
#endif

#endif /* STDLIB_H */
