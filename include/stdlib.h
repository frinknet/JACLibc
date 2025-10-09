/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STDLIB_H
#define STDLIB_H
#pragma once

#include <config.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbit.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>

#if JACL_HAS_C23
  #define __STDC_VERSION_STDLIB_H__ 202311L
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Memory API */
void* malloc(size_t n);
void free(void* ptr);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);

/* Builtin compatibility  */
#if defined(__GNUC__) || defined(__clang__)
	#define __jacl_trap() __builtin_trap()
#elif defined(_MSC_VER)
	#include <intrin.h>
	#define __jacl_trap() __debugbreak()
#else
	#define __jacl_trap() (*(volatile int*)0 = 0)
#endif

typedef struct {
	int quot, rem;
} div_t;
typedef struct {
	long quot, rem;
} ldiv_t;

#if JACL_HAS_C99
typedef struct { long long quot, rem;}	lldiv_t;
#endif

#if JACL_HAS_C11
void *aligned_alloc(size_t a, size_t s);
#endif

#if JACL_HAS_C99
static inline void _Exit(int status) {
	#if JACL_HAS_C99
			_exit(status);
	#else
		(void)status;

		abort();
	#endif
}
#endif

/* — Integer Conversion & Parsing — */
static inline int atoi(const char *nptr) { int v=0; sscanf(nptr, "%d", &v); return v; }
static inline long atol(const char *nptr) { long v=0; sscanf(nptr, "%ld", &v); return v; }
static inline double atof(const char *nptr) { double v=0; sscanf(nptr, "%lg", &v); return v; }
static inline long strtol(const char *nptr, char **endptr, int base) {
		if (!nptr) {
				if (endptr) *endptr = (char*)nptr;
				return 0;
		}

		const char *start = nptr;

		// Skip whitespace
		while (*nptr == ' ' || *nptr == '\t' || *nptr == '\n') nptr++;

		// Handle sign
		int sign = 1;
		if (*nptr == '-') {
				sign = -1;
				nptr++;
		} else if (*nptr == '+') {
				nptr++;
		}

		// Validate base
		if (base < 0 || base == 1 || base > 36) {
				if (endptr) *endptr = (char*)start;
				return 0;
		}

		// ONLY auto-detect if base == 0
		if (base == 0) {
				if (*nptr == '0') {
						if (*(nptr + 1) == 'x' || *(nptr + 1) == 'X') {
								base = 16;
								nptr += 2;
						} else {
								base = 8;
						}
				} else {
						base = 10;
				}
		} else if (base == 16) {
				// Handle 0x prefix for explicit base 16
				if (*nptr == '0' && (*(nptr + 1) == 'x' || *(nptr + 1) == 'X')) {
						nptr += 2;
				}
		}
		// For base 10, don't do any prefix detection!

		long result = 0;
		int digits_found = 0;

		while (*nptr) {
				int digit = -1;
				char c = *nptr;

				if (c >= '0' && c <= '9') {
						digit = c - '0';
				} else if (c >= 'A' && c <= 'Z') {
						digit = c - 'A' + 10;
				} else if (c >= 'a' && c <= 'z') {
						digit = c - 'a' + 10;
				}

				if (digit < 0 || digit >= base) break;

				result = result * base + digit;
				digits_found++;
				nptr++;
		}

		if (endptr) {
				*endptr = (char*)(digits_found > 0 ? nptr : start);
		}

		return digits_found > 0 ? sign * result : 0;
}

static inline unsigned long strtoul(const char *nptr, char **endptr, int base) {
		// Same logic as strtol but return unsigned and handle sign differently
		long result = strtol(nptr, endptr, base);
		return (unsigned long)result;
}
static inline double strtod(const char *nptr, char **endptr) {
	double v=0; int n=0;

	sscanf(nptr, "%lg%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
static inline int mblen(const char *s, size_t n) {
	if (!s) return 1; // C locale, stateless

	if (n == 0) return -1;

	return *s ? 1 : 0;
}

static inline div_t		div		(int n, int d)			 { return (div_t){ n/d, n%d }; }
static inline ldiv_t	ldiv	(long n, long d)		 { return (ldiv_t){ n/d, n%d }; }

/* — Pseudo-Random — */
#define RAND_MAX 0x7FFF  // Standard value (32767)

static unsigned __jacl_seed = 1;

static inline void srand(unsigned s) { __jacl_seed = s ? s : 1; }
static inline int rand(void) {
	__jacl_seed = __jacl_seed * 1103515245u + 12345u;
	return (int)((__jacl_seed >> 16) & RAND_MAX);
}

/* — Sorting & Searching — */
void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));
void* bsearch(const void* key, const void* base, size_t nmemb, size_t size, int (*compar)(const void*,const void*));

/* — Control & Env — */
static inline void abort(void) {
	raise(SIGABRT);  // Standard attempt

	#if JACL_OS_X64 || JACL_OS_X86
		__asm__ volatile ("ud2");  // Invalid opcode
	#elif JACL_OS_ARM64 || JACL_OS_ARM32
			__asm__ volatile ("brk #0");	// Breakpoint
	#elif JACL_OS_RISCV
			__asm__ volatile ("ebreak");	// Environment break
	#elif JACL_ARCH_WASM
			__asm__ volatile ("unreachable");  // WASM trap
	#else
			*(volatile int*)0 = 0;	// Universal fallback
	#endif

	for(;;) {}	// The heat death of the universe
}
static inline void exit(int status) {
	#if JACL_HAS_C99
			_Exit(status);
	#else
		(void)status;

		abort();
	#endif
}
static inline char* getenv(const char* name) {
	extern char **environ;
	size_t len = strlen(name);

	for (char **env = environ; *env; ++env) {
		if (!strncmp(*env, name, len) && (*env)[len] == '=') return &((*env)[len+1]);
	}

	return NULL;
}
static inline int system(const char* command) {
	#if JACL_OS_WINDOWS
		return _spawnlp(_P_WAIT, "cmd.exe", "cmd.exe", "/C", command, NULL);
	#elif JACL_OS_JSRUN
		(void)command;

		return -1;
	#else
		return fork_and_exec(command); // Use fork/exec, or just call `system(3)` from host.
	#endif
}

static inline char *strdup(const char *s) {
	size_t n = strlen(s) + 1;
	char *p = (char *)malloc(n);

	return p ? (char *)memcpy(p, s, n) : NULL;
}

#if JACL_HAS_C99

static inline long long atoll(const char *nptr) { long long v=0; sscanf(nptr, "%lld", &v); return v; }
static inline lldiv_t lldiv (long long n, long long d){ return (lldiv_t){ n/d, n%d }; }
static inline char *strndup(const char *s, size_t n) {
		size_t len = strnlen(s, n);
		char *p = (char *)malloc(len + 1);
		if (!p) return NULL;
		memcpy(p, s, len);
		p[len] = '\0';
		return p;
}
static inline float strtof(const char *nptr, char **endptr) {
	float v=0; int n=0;

	sscanf(nptr, "%g%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
static inline long double strtold(const char *nptr, char **endptr) {
	long double v=0; int n=0;

	sscanf(nptr, "%Lg%n", &v, &n);
	if (endptr) *endptr = (char*)&nptr[n];

	return v;
}
static inline long long strtoll(const char *nptr, char **endptr, int base) {
		// Same parsing logic, just return long long
		long result = strtol(nptr, endptr, base);
		return (long long)result;
}
static inline unsigned long long strtoull(const char *nptr, char **endptr, int base) {
		long result = strtol(nptr, endptr, base);
		return (unsigned long long)result;
}

#endif /* JACL_HAS_C99 */

#ifdef __cplusplus
}
#endif

#endif /* STDLIB_H */
