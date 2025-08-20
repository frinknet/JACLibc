// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef UCHAR_H
#define UCHAR_H

#include <stddef.h>
#include <stdint.h>
#include <wchar.h>  // for mbstate_t

#ifdef __cplusplus
extern "C" {
#endif

/* C11 Unicode character types */
#ifndef __CHAR16_TYPE__
typedef uint_least16_t char16_t;
# define __CHAR16_TYPE__
#endif
#ifndef __CHAR32_TYPE__
typedef uint_least32_t char32_t;
# define __CHAR32_TYPE__
#endif

/* Limits */
#define CHAR16_MIN 0
#define CHAR16_MAX 0xFFFFu
#define CHAR32_MIN 0
#define CHAR32_MAX 0xFFFFFFFFu

/* Conversion functions */
size_t mbrtoc16(char16_t *pc16, const char *s, size_t n, mbstate_t *ps);
size_t c16rtomb(char *s, char16_t c16, mbstate_t *ps);
size_t mbrtoc32(char32_t *pc32, const char *s, size_t n, mbstate_t *ps);
size_t c32rtomb(char *s, char32_t c32, mbstate_t *ps);

#ifdef __cplusplus
}
#endif

#endif /* UCHAR_H */

