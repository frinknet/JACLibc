/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STDLIB_C
#define STDLIB_C

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

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


#ifdef __cplusplus
}
#endif

#endif /* STDLIB_C */
