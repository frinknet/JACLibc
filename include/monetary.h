/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _MONETARY_H
#define _MONETARY_H

#pragma once

#include <locale.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ================================================================ */
/* Functions                                                        */
/* ================================================================ */

static inline ssize_t __jacl_strfmon_impl(char *restrict s, size_t maxsize, locale_t loc, const char *restrict format, va_list ap) {
    if (!s || !format || maxsize == 0) return -1;
    if (!loc || loc == LC_GLOBAL_LOCALE) loc = __jacl_locale_current;
    if (!loc || loc == LC_THREAD_LOCALE) loc = __jacl_locale_global;
    if (!loc) return -1;

    struct lconv *lc = &loc->lconv;
    size_t len = 0;
    const char *p = format;

    while (*p && len < maxsize - 1) {
        if (*p != '%') { s[len++] = *p++; continue; }
        p++;
        if (*p == '%') { s[len++] = '%'; p++; continue; }

        /* Skip flags, width, precision for now (POSIX subset) */
        int prec = -1;
        while (*p == '-' || *p == '+' || *p == '^' || *p == '!' || *p == '(' || *p == '=' || *p == '0') p++;
        while (*p >= '0' && *p <= '9') p++;
        if (*p == '.') {
            p++;
            prec = 0;
            while (*p >= '0' && *p <= '9') { prec = prec * 10 + (*p - '0'); p++; }
        }
        if (*p == '#') { p++; prec = -2; }

        char conv = *p;
        if (conv != 'n' && conv != 'i') { s[len++] = '%'; continue; }
        p++;

        double val = va_arg(ap, double);
        int is_neg = val < 0.0;
        if (is_neg) val = -val;

        /* Resolve precision safely */
        int frac = (prec == -1) ? lc->frac_digits : (prec == -2) ? lc->int_frac_digits : prec;
        if (frac == CHAR_MAX || frac < 0 || frac > 20) frac = 2;

        /* Safe number formatting via snprintf */
        char num[64];
        int nlen = snprintf(num, sizeof(num), "%.*f", frac, val);
        if (nlen < 0 || nlen >= (int)sizeof(num)) return -1;

        /* Inject locale decimal point */
        for (int i = 0; i < nlen; i++) {
            if (num[i] == '.' && lc->mon_decimal_point[0]) {
                num[i] = lc->mon_decimal_point[0];
                break;
            }
        }

        /* Sign & Symbol */
        const char *sym = (conv == 'i') ? lc->int_curr_symbol : lc->currency_symbol;
        if (!sym) sym = "";
        const char *sign = is_neg ? (lc->negative_sign ? lc->negative_sign : "-") : (lc->positive_sign ? lc->positive_sign : "");
        if (!sign) sign = "";

        /* Assemble chunk */
        char chunk[128];
        int clen = snprintf(chunk, sizeof(chunk), "%s%s%s", sign, sym, num);
        if (clen < 0 || clen >= (int)sizeof(chunk)) return -1;

        /* Copy to output */
        size_t avail = maxsize - len - 1;
        size_t to_copy = (size_t)clen < avail ? (size_t)clen : avail;
        memcpy(s + len, chunk, to_copy);
        len += to_copy;
    }
    s[len] = '\0';
    return (ssize_t)len;
}

/* --- Locale-specific Variants (_l) - Primitives --- */
static inline ssize_t strfmon_l(char *restrict s, size_t maxsize, locale_t locale, const char *restrict format, ...) {
    va_list ap;
    va_start(ap, format);
    ssize_t r = __jacl_strfmon_impl(s, maxsize, locale, format, ap);
    va_end(ap);
    return r;
}

/* --- Standard Variants - Wrappers --- */
static inline ssize_t strfmon(char *restrict s, size_t maxsize, const char *restrict format, ...) {
    va_list ap;
    va_start(ap, format);
    ssize_t r = __jacl_strfmon_impl(s, maxsize, NULL, format, ap);
    va_end(ap);
    return r;
}

#ifdef __cplusplus
}
#endif

#endif /* _MONETARY_H */
