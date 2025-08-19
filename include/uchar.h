// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef UCHAR_H
#define UCHAR_H

#include <stddef.h>
#include <stdint.h>
#include <wchar.h>	// for mbstate_t

#ifdef __cplusplus
extern "C" {
#endif

/* C11 Unicode character types */
typedef uint_least16_t char16_t;
typedef uint_least32_t char32_t;

/* Limits */
#define CHAR16_MIN	0
#define CHAR16_MAX	0xFFFFu
#define CHAR32_MIN	0
#define CHAR32_MAX	0xFFFFFFFFu

/* Conversion functions */
size_t mbrtoc16(char16_t *pc16, const char *s, size_t n, mbstate_t *ps) { return __builtin_mbrtoc16(pc16, s, n, ps); }
size_t c16rtomb(char *s, char16_t c16, mbstate_t *ps) { return __builtin_c16rtomb(s, c16, ps); }
size_t mbrtoc32(char32_t *pc32, const char *s, size_t n, mbstate_t *ps) { return __builtin_mbrtoc32(pc32, s, n, ps); }
size_t c32rtomb(char *s, char32_t c32, mbstate_t *ps) { return __builtin_c32rtomb(s, c32, ps); }

#ifdef __cplusplus
}
#endif

#endif /* UCHAR_H */
