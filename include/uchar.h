/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef UCHAR_H
#define UCHAR_H
#pragma once

#include <config.h>
#include <uchar.h>
#include <wchar.h>
#include <errno.h>

#if !JACL_HAS_C11
#error "uchar.h requires C11 or later"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* C11 Unicode character types */
#ifndef __cplusplus
typedef uint_least16_t char16_t;
typedef uint_least32_t char32_t;
#endif

/* Limits */
#define CHAR16_MIN 0
#define CHAR16_MAX 0xFFFFu
#define CHAR32_MIN 0
#define CHAR32_MAX 0xFFFFFFFFu

// UTF-8/UTF-16 conversion with proper surrogate pair handling
static inline size_t mbrtoc16(char16_t *pc16, const char *s, size_t n, mbstate_t *ps) {
		static unsigned internal_state;
		if (!ps) ps = (mbstate_t *)&internal_state;

		unsigned *pending = (unsigned *)ps;

		if (!s) return mbrtoc16(0, "", 1, ps);

		// Handle pending surrogate from previous call
		if ((int)*pending > 0) {
				if (pc16) *pc16 = *pending;
				*pending = 0;
				return (size_t)-3;	// Surrogate continuation
		}

		wchar_t wc;
		size_t ret = mbrtowc(&wc, s, n, ps);

		if (ret <= 4) {  // Valid multibyte conversion
				if (wc >= 0x10000) {
						// Surrogate pair needed
						*pending = (wc & 0x3ff) + 0xdc00;  // Low surrogate
						wc = 0xd7c0 + (wc >> 10);					 // High surrogate
				}
				if (pc16) *pc16 = wc;
		}

		return ret;
}

static inline size_t c16rtomb(char *s, char16_t c16, mbstate_t *ps) {
		static unsigned internal_state;
		if (!ps) ps = (mbstate_t *)&internal_state;

		unsigned *x = (unsigned *)ps;
		wchar_t wc;

		if (!s) {
				*x = 0;
				return c16rtomb((char[MB_CUR_MAX]){0}, 0, ps);
		}

		if (!c16) {
				if (*x) return (size_t)-1;	// Incomplete surrogate
				return wcrtomb(s, 0, ps);
		}

		if (*x) {
				// Low surrogate expected
				if (c16 < 0xdc00 || c16 >= 0xe000) {
						*x = 0;
						errno = EILSEQ;
						return (size_t)-1;
				}
				wc = 0x10000 + ((*x - 0xd800) << 10) + (c16 - 0xdc00);
				*x = 0;
		} else if (c16 >= 0xd800 && c16 < 0xdc00) {
				// High surrogate - store and wait for low
				*x = c16;
				return 0;
		} else if (c16 >= 0xdc00 && c16 < 0xe000) {
				// Unexpected low surrogate
				errno = EILSEQ;
				return (size_t)-1;
		} else {
				// BMP character
				wc = c16;
		}

		return wcrtomb(s, wc, ps);
}

// UTF-32 versions are simpler - no surrogate pairs
static inline size_t mbrtoc32(char32_t *pc32, const char *s, size_t n, mbstate_t *ps) {
		static unsigned internal_state;
		if (!ps) ps = (mbstate_t *)&internal_state;

		if (!s) return mbrtoc32(0, "", 1, ps);

		wchar_t wc;
		size_t ret = mbrtowc(&wc, s, n, ps);

		if (ret <= 4 && pc32) *pc32 = wc;
		return ret;
}

static inline size_t c32rtomb(char *s, char32_t c32, mbstate_t *ps) {
		static unsigned internal_state;
		if (!ps) ps = (mbstate_t *)&internal_state;

		if (!s) return c32rtomb((char[MB_CUR_MAX]){0}, 0, ps);

		return wcrtomb(s, c32, ps);
}

#ifdef __cplusplus
}
#endif

#endif /* UCHAR_H */
