/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef CORE_STDLIB_H
#define CORE_STDLIB_H

#include <config.h>
#include <stdlib.h>
#include <errno.h>
#include <core/format.h>
#include <core/memory.h>

#ifdef __cplusplus
extern {
#endif

/* ============================================================= */
/* Exit Handler Registry                                         */
/* ============================================================= */

#define __JACL_ATEXIT_MAX 32

static struct {
	void (*func)(void);
	int used;
} __jacl_exit_handlers[__JACL_ATEXIT_MAX];

static int __jacl_exit_count = 0;

#if JACL_HAS_C11
static struct {
	void (*func)(void);
	int used;
} __jacl_quick_exit_handlers[__JACL_ATEXIT_MAX];

static int __jacl_quick_exit_count = 0;
#endif

int atexit(void (*func)(void)) {
	if (!func) return -1;

	for (int i = 0; i < __JACL_ATEXIT_MAX; i++) {
		if (!__jacl_exit_handlers[i].used) {
			__jacl_exit_handlers[i].func = func;
			__jacl_exit_handlers[i].used = 1;
			__jacl_exit_count++;

			return 0;
		}
	}

	return -1;
}

static inline void __jacl_exit_run_handlers(void) {
	for (int i = __JACL_ATEXIT_MAX - 1; i >= 0; i--) {
		if (__jacl_exit_handlers[i].used && __jacl_exit_handlers[i].func) {
			__jacl_exit_handlers[i].func();
		}
	}
}

#if JACL_HAS_C11

int at_quick_exit(void (*func)(void)) {
	if (!func) return -1;

	for (int i = 0; i < __JACL_ATEXIT_MAX; i++) {
		if (!__jacl_quick_exit_handlers[i].used) {
			__jacl_quick_exit_handlers[i].func = func;
			__jacl_quick_exit_handlers[i].used = 1;
			__jacl_quick_exit_count++;

			return 0;
		}
	}

	return -1;
}

static inline void __jacl_exit_run_quick_handlers(void) {
	for (int i = __JACL_ATEXIT_MAX - 1; i >= 0; i--) {
		if (__jacl_quick_exit_handlers[i].used && __jacl_quick_exit_handlers[i].func) {
			__jacl_quick_exit_handlers[i].func();
		}
	}
}

noreturn void quick_exit(int status) {
	__jacl_exit_run_quick_handlers();
	_Exit(status);

	for(;;);
}

#endif /* JACL_HAS_C11 */

void exit(int status) {
	__jacl_exit_run_handlers();

	#if JACL_HAS_C99
		_Exit(status);
	#else
		(void)status;

		abort();
	#endif
}

/* ============================================================= */
/* Sorting & Searching                                           */
/* ============================================================= */

static inline void __jacl_sort_swap(unsigned char* a, unsigned char* b, size_t size) {
	while (size--) {
		unsigned char tmp = *a;

		*a++ = *b;
		*b++ = tmp;
	}
}

static void __jacl_sort_again(void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*)) {
	if (nmemb < 2) return;

	unsigned char* arr = (unsigned char*)base;

	// Median-of-three pivot selection
	size_t low = 0;
	size_t high = nmemb - 1;
	size_t mid = low + (high - low) / 2;

	// Sort low, mid, high
	if (compar(arr + low * size, arr + mid * size) > 0) __jacl_sort_swap(arr + low * size, arr + mid * size, size);
	if (compar(arr + mid * size, arr + high * size) > 0) __jacl_sort_swap(arr + mid * size, arr + high * size, size);
	if (compar(arr + low * size, arr + mid * size) > 0) __jacl_sort_swap(arr + low * size, arr + mid * size, size);

	// Allocate pivot storage
	void* pivot = malloc(size);

	if (!pivot) {
		// Fallback to bubble sort
		for (size_t i = 0; i < nmemb - 1; i++) {
			for (size_t j = 0; j < nmemb - i - 1; j++) {
				if (compar(arr + j * size, arr + (j + 1) * size) > 0) __jacl_sort_swap(arr + j * size, arr + (j + 1) * size, size);
			}
		}

		return;
	}

	memcpy(pivot, arr + mid * size, size);

	// Partition
	size_t i = 0;
	size_t j = nmemb - 1;

	while (1) {
		while (i < nmemb && compar(arr + i * size, pivot) < 0) i++;
		while (j > 0 && compar(arr + j * size, pivot) > 0) j--;

		if (i >= j) break;

		__jacl_sort_swap(arr + i * size, arr + j * size, size);

		i++;

		if (j > 0) j--;
	}

	free(pivot);

	// Recursively sort partitions
	if (j > 0) __jacl_sort_again(arr, j + 1, size, compar);
	if (i < nmemb - 1) __jacl_sort_again(arr + (i + 1) * size, nmemb - i - 1, size, compar);
}

void qsort(void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*)) {
	if (!base || !compar || size == 0 || nmemb < 2) return;

	__jacl_sort_again(base, nmemb, size, compar);
}

void* bsearch(const void* key, const void* base, size_t nmemb, size_t size, int (*compar)(const void*, const void*)) {
	if (!key || !base || !compar || size == 0 || nmemb == 0)
		return NULL;

	const unsigned char* arr = (const unsigned char*)base;
	size_t low = 0;
	size_t high = nmemb;

	while (low < high) {
		size_t mid = low + (high - low) / 2;
		const void* mid_elem = arr + mid * size;
		int cmp = compar(key, mid_elem);

		if (cmp == 0) return (void*)mid_elem;
		else if (cmp < 0) high = mid;
		else low = mid + 1;
	}

	return NULL;
}

/* ============================================================= */
/* String Conversion                                             */
/* ============================================================= */
#define __jacl_ato_int(name, type, len, usig) type ato##name(const char* str) { \
	const char* p = str; \
	int read = 0, ch = 0; \
	uintmax_t val = 0; \
	__jacl_fmt_t spec = usig? 10 : 10 | JACL_FMT_VAL(FLAG, sign); \
	__jacl_read_skip(ch, NULL, &p, &read); \
	if (__jacl_input_int(NULL, &p, &read, JACL_FMT_SET(LENGTH, spec, len), INT_MAX, &val)) return (type)val; \
	return 0; \
}
#define __jacl_strto_int(suf, type, len, usig) type strto##suf(const char* restrict str, char** restrict endptr, int base) { \
	if (base != 0 && (base < 2 || base > 36)) { \
		errno = EINVAL; \
		if (endptr) *endptr = (char*)str; \
		return 0; \
	} \
	const char* p = str; \
	int read = 0, ch = 0; \
	uintmax_t val = 0; \
	__jacl_fmt_t spec = usig? base : base | JACL_FMT_VAL(FLAG, sign); \
	__jacl_read_skip(ch, NULL, &p, &read); \
	if (__jacl_input_int(NULL, &p, &read, JACL_FMT_SET(LENGTH, spec, len), INT_MAX, &val)) { \
		if (endptr) *endptr = (char*)p; \
		return (type)val; \
	} \
	if (endptr) *endptr = (char*)str; \
	return 0; \
}
#define __jacl_strto_float(suf, type, len) type strto##suf(const char* restrict str, char** restrict endptr) { \
	const char* p = str; \
	int read = 0, ch = 0; \
	long double val = 0.0; \
	__jacl_read_skip(ch, NULL, &p, &read); \
	__jacl_fmt_t spec = 10; \
	JACL_FMT_SET(LENGTH, spec, len); \
	if (__jacl_input_special(NULL, &p, &read, spec, INT_MAX, &val) || __jacl_input_float(NULL, &p, &read, spec, INT_MAX, &val)) { \
		if (endptr) *endptr = (char*)p; \
		return (type)val; \
	} \
	if (endptr) *endptr = (char*)str; \
}

__jacl_ato_int(i, int, 0, 0)
__jacl_ato_int(l, long, l, 0)

__jacl_strto_int(l, long, l, 0)
__jacl_strto_int(ul, unsigned long, l, 1)

#if JACL_HAS_C99
__jacl_ato_int(ll, long long, ll, 0)

__jacl_strto_int(ll, long long, ll, 0)
__jacl_strto_int(ull, unsigned long long, ll, 1)

__jacl_strto_float(f, float, 0)
__jacl_strto_float(d, double, l)
__jacl_strto_float(ld, long double, L)
#endif

double atof(const char* str) {
	const char* p = str;
	int _read = 0;
	long double val = 0.0;

	if (__jacl_input_float(NULL, &p, &_read, JACL_FMT_BASE_exp, INT_MAX, &val)) return (double)val;

	return 0.0;
}

#ifdef __cplusplus
}
#endif

#endif /* CORE_STDLIB_H */
