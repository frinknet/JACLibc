/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
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

/* — Internal State (prefixed JACL_) — */
static uint8_t	 __jacl_heap[JACL_DEFAULT_HEAP_SIZE];
static size_t		 __jacl_arena_offset;
static uint16_t  __jacl_free_bitmap;
static uint16_t  __jacl_free_list[JACL_FL_INDEX_COUNT];
static int			 __jacl_initialized;

/* — One-time guarded init — */
#define JACL_INIT_MEM()																											 \
	do {																																					\
		if (!__jacl_initialized) {																													\
			__jacl_arena_offset = 0;																											 \
			__jacl_free_bitmap	 = 0;																												\
			for (int i = 0; i < JACL_FL_INDEX_COUNT; i++)																			\
				__jacl_free_list[i] = 0xFFFF;																									\
			size_t st = JACL_ALIGN_UP(0);																								 \
			int c = 31 - __builtin_clz((unsigned)(JACL_DEFAULT_HEAP_SIZE - st));							\
			if (c >= JACL_FL_INDEX_COUNT) c = JACL_FL_INDEX_COUNT - 1;															 \
			__jacl_free_list[c] = (uint16_t)st;																							\
			__jacl_free_bitmap	|= 1u << c;																									\
			__jacl_initialized		 = 1;																												\
		}																																						\
	} while (0)

void *memset(void *s, int c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t maxlen);

/* — malloc: bump-arena for small, segregated free-list for large — */
void* malloc(size_t n) {
	JACL_INIT_MEM();
	if (n <= JACL_FASTBIN_MAX) {
		size_t sz = JACL_ALIGN_UP(n);
		if (__jacl_arena_offset + sz <= JACL_DEFAULT_HEAP_SIZE) {
			void* p = __jacl_heap + __jacl_arena_offset;
			__jacl_arena_offset += sz;
			return p;
		}
	}
	size_t sz = n < JACL_ALIGNMENT ? JACL_ALIGNMENT : n;
	int c = 31 - __builtin_clz((unsigned)sz);
	if (c >= JACL_FL_INDEX_COUNT) c = JACL_FL_INDEX_COUNT - 1;
	uint32_t m = __jacl_free_bitmap & (~0u << c);
	if (!m) return NULL;
	c = __builtin_ctz(m);
	uint16_t idx = __jacl_free_list[c];
	__jacl_free_list[c] = *(uint16_t*)(__jacl_heap + idx);
	if (!__jacl_free_list[c]) __jacl_free_bitmap &= ~(1u << c);
	return __jacl_heap + idx;
}

/* — free: return to free-list (arena leaks) — */
void free(void* ptr) {
	JACL_INIT_MEM();
	if (!ptr) return;
	uintptr_t off = (uint8_t*)ptr - __jacl_heap;
	if (off < __jacl_arena_offset) return;
	int c = 31 - __builtin_clz((unsigned)(JACL_DEFAULT_HEAP_SIZE - off));
	if (c >= JACL_FL_INDEX_COUNT) c = JACL_FL_INDEX_COUNT - 1;
	*(uint16_t*)(__jacl_heap + off) = __jacl_free_list[c];
	__jacl_free_list[c] = (uint16_t)off;
	__jacl_free_bitmap |= 1u << c;
}

/* — calloc & realloc — */
void* calloc(size_t nmemb, size_t size) {
	JACL_INIT_MEM();
	size_t tot = nmemb * size;
	void* p = malloc(tot);
	if (p) memset(p, 0, tot);
	return p;
}
void* realloc(void* ptr, size_t size) {
	JACL_INIT_MEM();
	if (!ptr) return malloc(size);
	void* q = malloc(size);
	if (q) memcpy(q, ptr, size);
	return q;
}

/* — Integer Conversion & Parsing — */
int atoi(const char *nptr) { int v=0; sscanf(nptr, "%d", &v); return v; }
long atol(const char *nptr) { long v=0; sscanf(nptr, "%ld", &v); return v; }
long long atoll(const char *nptr) { long long v=0; sscanf(nptr, "%lld", &v); return v; }
double atof(const char *nptr) { double v=0; sscanf(nptr, "%lg", &v); return v; }
long strtol(const char *nptr, char **endptr, int base) {
	long v = 0; int n = 0;

	if (base == 10) sscanf(nptr, "%ld%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
unsigned long strtoul(const char *nptr, char **endptr, int base) {
	unsigned long v = 0; int n = 0;

	if (base == 10) sscanf(nptr, "%lu%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
long long strtoll(const char *nptr, char **endptr, int base) {
	long long v = 0; int n = 0;

	if (base == 10) sscanf(nptr, "%lld%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
unsigned long long strtoull(const char *nptr, char **endptr, int base) {
	unsigned long long v = 0; int n = 0;

	if (base == 10) sscanf(nptr, "%llu%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
float strtof(const char *nptr, char **endptr) {
	float v=0; int n=0;

	sscanf(nptr, "%g%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
double strtod(const char *nptr, char **endptr) {
	double v=0; int n=0;

	sscanf(nptr, "%lg%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
long double strtold(const char *nptr, char **endptr) {
	long double v=0; int n=0;

	sscanf(nptr, "%Lg%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
int mblen(const char *s, size_t n) {
	if (!s) return 1; // C locale, stateless

	if (n == 0) return -1;

	return *s ? 1 : 0;
}

typedef struct { int quot, rem; }				div_t;
typedef struct { long quot, rem; }			ldiv_t;
typedef struct { long long quot, rem;}	lldiv_t;

div_t		div		(int n, int d)			 { return (div_t){ n/d, n%d }; }
ldiv_t	ldiv	(long n, long d)		 { return (ldiv_t){ n/d, n%d }; }
lldiv_t lldiv (long long n, long long d){ return (lldiv_t){ n/d, n%d }; }

/* — Pseudo-Random — */
static unsigned jacl_seed = 1;

void srand(unsigned s) { jacl_seed = s ? s : 1; }
int rand(void) { jacl_seed = jacl_seed * 1103515245u + 12345u; return (int)((jacl_seed >> 16) & 0x7FFF); }

/* — Sorting & Searching — */
void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *)) {
	char *arr = (char *)base, tmp;
	size_t total = nmemb * size;
	size_t gap = nmemb;

	if (nmemb < 2 || size == 0) return;

	while (gap > 1) {
		gap = (5 * gap - 1) / 11;
		if (gap < 1) gap = 1;
		size_t wgap = gap * size;

		for (size_t i = wgap; i < total; i += size) {
			for (size_t j = i; j >= wgap; j -= wgap) {
				char *p = arr + j;
				char *q = p - wgap;
				if (compar(q, p) <= 0) break;
				for (size_t k = 0; k < size; ++k) {
					tmp = p[k];
					p[k] = q[k];
					q[k] = tmp;
				}
			}
		}
	}
}
void* bsearch(const void* key, const void* base, size_t nmemb, size_t size, int (*compar)(const void*,const void*)) {
	const char* arr = (const char*)base;
	size_t low = 0, high = nmemb;

	while (low < high) {
		size_t mid = (low + high) >> 1;
		const void* elem = arr + mid * size;
		int cmp = compar(key, elem);

		if (cmp < 0) high = mid;
		else if (cmp > 0) low = mid + 1;
		else return (void*)elem;
	}

	return NULL;
}

/* — Aligned Alloc (C11) — */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
/* C11 guarantee of aligned_alloc() */
void *aligned_alloc(size_t a, size_t s) {
	(void)a; (void)s;
	return NULL;
}
#elif defined(__has_builtin)
#if __has_builtin(__builtin_aligned_alloc)
void *aligned_alloc(size_t a, size_t s) { return __builtin_aligned_alloc(a, s); }
#endif
#endif

/* — Control & Env — */
void abort(void) { __builtin_trap(); }
void exit(int st)	{ (void)st; abort(); }
char* getenv(const char* n)	{ return NULL; }
int		system(const char* c)	{ return -1; }

char *strdup(const char *s) {
		size_t n = strlen(s) + 1;
		char *p = (char *)malloc(n);
		return p ? (char *)memcpy(p, s, n) : NULL;
}

char *strndup(const char *s, size_t n) {
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
