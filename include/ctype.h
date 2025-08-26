/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef CTYPE_H
#define CTYPE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ASCII table */
static inline int isalnum(int c) { return ((c >= '0' && c <= '9') || ((c|32) >= 'a' && (c|32) <= 'z')); }
static inline int isalpha(int c) { return ((c|32) >= 'a' && (c|32) <= 'z'); }
static inline int isdigit(int c) { return (c >= '0' && c <= '9'); }
static inline int iscntrl(int c) { return (c >= 0 && c < 32) || c == 127; }
static inline int isspace(int c) { return c==' '||c=='\f'||c=='\n'||c=='\r'||c=='\t'||c=='\v'; }
static inline int isupper(int c) { return (c >= 'A' && c <= 'Z'); }
static inline int islower(int c) { return (c >= 'a' && c <= 'z'); }
static inline int isprint(int c) { return (c >= 32 && c < 127); }
static inline int isgraph(int c) { return isprint(c) && c != ' '; }
static inline int ispunct(int c) { return isgraph(c) && !isalnum(c); }
static inline int isxdigit(int c){ return isdigit(c) || ((c|32)>='a'&&(c|32)<='f'); }
static inline int isblank(int c) { return c==' '||c=='\t'; }

/* Case conversion */
static inline int tolower(int c) { return (c >= 'A' && c <= 'Z') ? c + 32 : c; }
static inline int toupper(int c) { return (c >= 'a' && c <= 'z') ? c - 32 : c; }

#ifdef __cplusplus
}
#endif

#endif /* CTYPE_H */
