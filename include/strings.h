/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STRINGS_H
#define STRINGS_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <string.h>
#include <ctype.h>

#if defined(_WIN32)
	#define STRINGS_WIN32
	#include <windows.h>
#elif defined(__wasm__)
	#define STRINGS_WASM
#else
	#define STRINGS_POSIX
	#include <ctype.h>
#endif

void bzero(void *s, size_t n);

/* Case-insensitive string comparison */
static inline int strcasecmp(const char *s1, const char *s2) {
	if (!s1 || !s2) return (s1 == s2) ? 0 : (s1 ? 1 : -1);
	
#ifdef STRINGS_WIN32
	return _stricmp(s1, s2);
#else
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
#endif
}

/* Case-insensitive string comparison (n characters) */
static inline int strncasecmp(const char *s1, const char *s2, size_t n) {
		// Handle zero-length comparison first
		if (n == 0) return 0;
		
		// Then handle NULL pointers
		if (!s1 || !s2) return (s1 == s2) ? 0 : (s1 ? 1 : -1);
		
#ifdef STRINGS_WIN32
		return _strnicmp(s1, s2, n);
#else
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
#endif
}


/* Find first set bit (1-based indexing) */
static inline int ffs(int mask) {
	if (mask == 0) return 0;
	
#if defined(__GNUC__) || defined(__clang__)
	return __builtin_ffs(mask);
#elif defined(STRINGS_WIN32)
	unsigned long index;
	return _BitScanForward(&index, (unsigned long)mask) ? (int)(index + 1) : 0;
#else
	/* Portable implementation */
	int bit = 1;
	if ((mask & 0xFFFF) == 0) { mask >>= 16; bit += 16; }
	if ((mask & 0xFF) == 0) { mask >>= 8; bit += 8; }
	if ((mask & 0xF) == 0) { mask >>= 4; bit += 4; }
	if ((mask & 0x3) == 0) { mask >>= 2; bit += 2; }
	if ((mask & 0x1) == 0) bit++;
	return bit;
#endif
}

/* Find first set bit in long */
static inline int ffsl(long mask) {
	if (mask == 0) return 0;
	
#if defined(__GNUC__) || defined(__clang__)
	return __builtin_ffsl(mask);
#else
	return ffs((int)mask);
#endif
}

/* Find first set bit in long long */
static inline int ffsll(long long mask) {
	if (mask == 0) return 0;
	
#if defined(__GNUC__) || defined(__clang__)
	return __builtin_ffsll(mask);
#else
	/* Check lower 32 bits first */
	int result = ffs((int)(mask & 0xFFFFFFFF));
	if (result != 0) return result;
	/* Check upper 32 bits */
	result = ffs((int)(mask >> 32));
	return (result == 0) ? 0 : result + 32;
#endif
}

/* Find last set bit (1-based indexing) */
static inline int fls(int mask) {
	if (mask == 0) return 0;
	
#if defined(__GNUC__) || defined(__clang__)
	return 32 - __builtin_clz((unsigned int)mask);
#elif defined(STRINGS_WIN32)
	unsigned long index;
	return _BitScanReverse(&index, (unsigned long)mask) ? (int)(index + 1) : 0;
#else
	/* Portable implementation */
	int bit = 0;
	if (mask & 0xFFFF0000) { mask >>= 16; bit += 16; }
	if (mask & 0xFF00) { mask >>= 8; bit += 8; }
	if (mask & 0xF0) { mask >>= 4; bit += 4; }
	if (mask & 0xC) { mask >>= 2; bit += 2; }
	if (mask & 0x2) bit++;
	if (mask & 0x1) bit++;
	return bit;
#endif
}

/* Find last set bit in long */
static inline int flsl(long mask) {
	if (mask == 0) return 0;
	
#if defined(__GNUC__) || defined(__clang__)
	return (sizeof(long) == 8) ? 
		(64 - __builtin_clzl((unsigned long)mask)) :
		(32 - __builtin_clzl((unsigned long)mask));
#else
	return fls((int)mask);
#endif
}

/* Find last set bit in long long */
static inline int flsll(long long mask) {
	if (mask == 0) return 0;
	
#if defined(__GNUC__) || defined(__clang__)
	return 64 - __builtin_clzll((unsigned long long)mask);
#else
	/* Check upper 32 bits first */
	int result = fls((int)(mask >> 32));
	if (result != 0) return result + 32;
	/* Check lower 32 bits */
	return fls((int)(mask & 0xFFFFFFFF));
#endif
}

/* Legacy BSD functions (deprecated but sometimes needed) */
#ifndef __STRICT_ANSI__

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

#endif /* __STRICT_ANSI__ */

#ifdef __cplusplus
}
#endif
#endif /* STRINGS_H */

