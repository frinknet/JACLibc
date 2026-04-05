/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef CTYPE_H
#define CTYPE_H
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * NOTE: These are all decidedly ASCII only rather than trying to be complex and ultimately wrong.
 *
 * The reason for this clear split is in the POSIX Standard which specifies the vialue of int as:
 *
 *    The c argument is an int, the value of which the application shall ensure is representable
 *    as an unsigned char or equal to the value of the macro EOF.
 *
 * They go on to say that any other value is undefined. In our case any other value does NOT PLAY.
 *
 * If you want UTF-8 you should be using wctype.h not this header. So this meanse that we have
 * chosen out of necessity to the obtuse view of the POSIX speification that all locals shall be
 * treated the same a C when it comes to ASCII and if you want international fun you should use
 * the UFT-8 variants.
 *
 * Hopefully, this simiplifies an otherwise esoteric and impossible standard. But it means that
 * locale specifics CAN NEVER apply properly here unless POSIX exbraces UNICODE as we have. So
 * as always we opt for pradictable simplicity and compliance over fancy footwork.
 */

/* ASCII table */
static inline int isdigit(int c) { return (c >= '0' && c <= '9'); }
static inline int iscntrl(int c) { return (c >= 0 && c < 32) || c == 127; }
static inline int isspace(int c) { return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; }
static inline int isprint(int c) { return (c >= 32 && c < 127); }
static inline int isgraph(int c) { return isprint(c) && c != ' '; }
static inline int isupper(int c) { return (c >= 'A' && c <= 'Z'); }
static inline int islower(int c) { return (c >= 'a' && c <= 'z'); }
static inline int isalpha(int c) { return isupper(c) || islower(c); }
static inline int isalnum(int c) { return isdigit(c) || isalpha(c); }
static inline int ispunct(int c) { return isgraph(c) && !isalnum(c); }
static inline int isxdigit(int c){ return isdigit(c) || ((c | 32) >= 'a' && (c | 32) <= 'f'); }

#if JACL_HAS_C99
static inline int isblank(int c) { return c==' ' || c=='\t'; }
#endif

/* Case conversion */
static inline int tolower(int c) { return (c >= 'A' && c <= 'Z') ? c + 32 : c; }
static inline int toupper(int c) { return (c >= 'a' && c <= 'z') ? c - 32 : c; }

#ifdef __cplusplus
}
#endif

#endif /* CTYPE_H */
