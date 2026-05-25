/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _LIBINTL_H
#define _LIBINTL_H

#pragma once

#include <locale.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================ */
/* Functions                                                        */
/* ================================================================ */

/* --- Locale-specific Variants (_l) - Primitives --- */

static inline char *gettext_l(const char *msgid, locale_t locale) {
    (void)locale;
    return (char *)msgid;
}

static inline char *dgettext_l(const char *domainname, const char *msgid, locale_t locale) {
    (void)domainname;
    (void)locale;
    return (char *)msgid;
}

static inline char *dcgettext_l(const char *domainname, const char *msgid, int category, locale_t locale) {
    (void)domainname;
    (void)category;
    (void)locale;
    return (char *)msgid;
}

static inline char *ngettext_l(const char *msgid1, const char *msgid2, unsigned long int n, locale_t locale) {
    (void)locale;
    return (char *)((n == 1) ? msgid1 : msgid2);
}

static inline char *dngettext_l(const char *domainname, const char *msgid1, const char *msgid2, unsigned long int n, locale_t locale) {
    (void)domainname;
    (void)locale;
    return (char *)((n == 1) ? msgid1 : msgid2);
}

static inline char *dcngettext_l(const char *domainname, const char *msgid1, const char *msgid2, unsigned long int n, int category, locale_t locale) {
    (void)domainname;
    (void)category;
    (void)locale;
    return (char *)((n == 1) ? msgid1 : msgid2);
}

/* --- Standard Variants - Wrappers --- */

static inline char *gettext(const char *msgid) {
    return gettext_l(msgid, NULL);
}

static inline char *dgettext(const char *domainname, const char *msgid) {
    return dgettext_l(domainname, msgid, NULL);
}

static inline char *dcgettext(const char *domainname, const char *msgid, int category) {
    return dcgettext_l(domainname, msgid, category, NULL);
}

static inline char *ngettext(const char *msgid1, const char *msgid2, unsigned long int n) {
    return ngettext_l(msgid1, msgid2, n, NULL);
}

static inline char *dngettext(const char *domainname, const char *msgid1, const char *msgid2, unsigned long int n) {
    return dngettext_l(domainname, msgid1, msgid2, n, NULL);
}

static inline char *dcngettext(const char *domainname, const char *msgid1, const char *msgid2, unsigned long int n, int category) {
    return dcngettext_l(domainname, msgid1, msgid2, n, category, NULL);
}

/* --- Domain Management --- */

static inline char *textdomain(const char *domainname) {
    (void)domainname;
    return (char *)"messages";
}

static inline char *bindtextdomain(const char *domainname, const char *dirname) {
    (void)domainname;
    (void)dirname;
    return (char *)"/usr/share/locale";
}

static inline char *bind_textdomain_codeset(const char *domainname, const char *codeset) {
    (void)domainname;
    (void)codeset;
    return (char *)"UTF-8";
}

#ifdef __cplusplus
}
#endif

#endif /* _LIBINTL_H */
