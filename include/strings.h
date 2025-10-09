/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STRINGS_H
#define STRINGS_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <config.h>
#include <stdbit.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>

#if JACL_OS_WINDOWS
	#define STRINGS_WIN32
	#include <windows.h>
#elif JACL_ARCH_WASM
	#define STRINGS_WASM
#else
	#define STRINGS_POSIX
	#include <ctype.h>
#endif

/* Case-insensitive string comparison */
static inline int strcasecmp(const char *s1, const char *s2) {
	if (!s1 || !s2) return (s1 == s2) ? 0 : (s1 ? 1 : -1);

	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;

	while (*p1 && *p2) {
		int c1 = tolower(*p1);
		int c2 = tolower(*p2);

		if (c1 != c2) return c1 - c2;

		p1++;
		p2++;
	}

	return tolower(*p1) - tolower(*p2);
}

/* Case-insensitive string comparison (n characters) */
static inline int strncasecmp(const char *s1, const char *s2, size_t n) {
	// Handle zero-length comparison first
	if (n == 0) return 0;

	// Then handle NULL pointers
	if (!s1 || !s2) return (s1 == s2) ? 0 : (s1 ? 1 : -1);

	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;
	size_t i;

	for (i = 0; i < n && *p1 && *p2; i++) {
			int c1 = tolower(*p1);
			int c2 = tolower(*p2);

			if (c1 != c2) return c1 - c2;

			p1++;
			p2++;
	}

	// If we compared n characters successfully, they're equal
	if (i == n) return 0;

	// Otherwise, one string ended before n characters
	return tolower(*p1) - tolower(*p2);
}


/* Find first set bit (1-based indexing) */
static inline int ffs(int mask) {
  if (!mask) return 0;

  return __jacl_ctz32((uint32_t)mask) + 1;
}

/* Find first set bit in long */
static inline int ffsl(long mask) {
  if (!mask) return 0;

  #if JACL_64BIT
    uint32_t lo = (uint32_t)mask;

    return lo
			? (__jacl_ctz32(lo) + 1)
			: (__jacl_ctz32((uint32_t)(mask >> 32)) + 33);
  #else
      return __jacl_ctz32((uint32_t)mask) + 1;
  #endif
}

/* Find first set bit in long long */
static inline int ffsll(long long mask) {
    if (!mask) return 0;

    uint32_t lo = (uint32_t)mask;

    return lo
			? (__jacl_ctz32(lo) + 1)
			: (__jacl_ctz32((uint32_t)(mask >> 32)) + 33);
}

/* Find last set bit (1-based indexing) */
static inline int fls(int mask) {
  if (!mask) return 0;

  return 32 - __jacl_clz32((unsigned int)mask);
}

/* Find last set bit in long */
static inline int flsl(long mask) {
  if (!mask) return 0;

  #if JACL_64BIT
    uint32_t hi = (uint32_t)(mask >> 32);
    return hi ? (64 - __jacl_clz32(hi)) : (32 - __jacl_clz32((uint32_t)mask));
  #else
    return 32 - __jacl_clz32((unsigned long)mask);
  #endif
}

/* Find last set bit in long long */
static inline int flsll(long long mask) {
  if (!mask) return 0;

  uint32_t hi = (uint32_t)(mask >> 32);

  return hi ? (64 - __jacl_clz32(hi)) : (32 - __jacl_clz32((uint32_t)mask));
}

/* Legacy BSD functions (deprecated but sometimes needed) */
#ifndef JACL_OS_BSD

/* Zero memory (deprecated - use memset) */
static inline void bzero(void *s, size_t n) {
	memset(s, 0, n);
}

/* Copy memory (deprecated - use memmove) */
static inline void bcopy(const void *src, void *dest, size_t n) {
	memmove(dest, src, n);
}

/* Compare memory (deprecated - use memcmp) */
static inline int bcmp(const void *s1, const void *s2, size_t n) {
	return memcmp(s1, s2, n) != 0 ? 1 : 0;
}

/* Find character (deprecated - use strchr) */
static inline char *index(const char *s, int c) {
	return strchr(s, c);
}

/* Find last character (deprecated - use strrchr) */
static inline char *rindex(const char *s, int c) {
	return strrchr(s, c);
}

#endif /* JACL_OS_BSD */

#ifdef __cplusplus
}
#endif
#endif /* STRINGS_H */

