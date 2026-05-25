/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _LANGINFO_H
#define _LANGINFO_H

#pragma once

#include <nl_types.h>
#include <locale.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    CODESET = 1,
    D_T_FMT, D_FMT, T_FMT, T_FMT_AMPM,
    AM_STR, PM_STR,
    DAY_1, DAY_2, DAY_3, DAY_4, DAY_5, DAY_6, DAY_7,
    ABDAY_1, ABDAY_2, ABDAY_3, ABDAY_4, ABDAY_5, ABDAY_6, ABDAY_7,
    MON_1, MON_2, MON_3, MON_4, MON_5, MON_6, MON_7, MON_8, MON_9, MON_10, MON_11, MON_12,
    ALTMON_1, ALTMON_2, ALTMON_3, ALTMON_4, ALTMON_5, ALTMON_6, ALTMON_7, ALTMON_8, ALTMON_9, ALTMON_10, ALTMON_11, ALTMON_12,
    ABMON_1, ABMON_2, ABMON_3, ABMON_4, ABMON_5, ABMON_6, ABMON_7, ABMON_8, ABMON_9, ABMON_10, ABMON_11, ABMON_12,
    ABALTMON_1, ABALTMON_2, ABALTMON_3, ABALTMON_4, ABALTMON_5, ABALTMON_6, ABALTMON_7, ABALTMON_8, ABALTMON_9, ABALTMON_10, ABALTMON_11, ABALTMON_12,
    ERA, ERA_D_FMT, ERA_D_T_FMT, ERA_T_FMT, ALT_DIGITS,
    RADIXCHAR, THOUSEP,
    YESEXPR, NOEXPR,
    CRNCYSTR
};

/* --- Locale-specific Variants (_l) - Primitives --- */

static inline char *nl_langinfo_l(nl_item item, locale_t loc) {
    if (!loc || loc == LC_GLOBAL_LOCALE) loc = __jacl_locale_current;
    if (!loc || loc == LC_THREAD_LOCALE) loc = __jacl_locale_global;
    if (!loc) return (char *)"";

    switch (item) {
        case CODESET:     return (char *)"UTF-8";
        case D_T_FMT:     return (char *)loc->time.fmt_datetime;
        case D_FMT:       return (char *)loc->time.fmt_date;
        case T_FMT:       return (char *)loc->time.fmt_time_24;
        case T_FMT_AMPM:  return (char *)loc->time.fmt_time_12;
        case AM_STR:      return (char *)loc->time.am;
        case PM_STR:      return (char *)loc->time.pm;
        case DAY_1:       return (char *)loc->time.days_full[0];
        case DAY_2:       return (char *)loc->time.days_full[1];
        case DAY_3:       return (char *)loc->time.days_full[2];
        case DAY_4:       return (char *)loc->time.days_full[3];
        case DAY_5:       return (char *)loc->time.days_full[4];
        case DAY_6:       return (char *)loc->time.days_full[5];
        case DAY_7:       return (char *)loc->time.days_full[6];
        case ABDAY_1:     return (char *)loc->time.days_abbr[0];
        case ABDAY_2:     return (char *)loc->time.days_abbr[1];
        case ABDAY_3:     return (char *)loc->time.days_abbr[2];
        case ABDAY_4:     return (char *)loc->time.days_abbr[3];
        case ABDAY_5:     return (char *)loc->time.days_abbr[4];
        case ABDAY_6:     return (char *)loc->time.days_abbr[5];
        case ABDAY_7:     return (char *)loc->time.days_abbr[6];
        case MON_1:       return (char *)loc->time.months_full[0];
        case MON_2:       return (char *)loc->time.months_full[1];
        case MON_3:       return (char *)loc->time.months_full[2];
        case MON_4:       return (char *)loc->time.months_full[3];
        case MON_5:       return (char *)loc->time.months_full[4];
        case MON_6:       return (char *)loc->time.months_full[5];
        case MON_7:       return (char *)loc->time.months_full[6];
        case MON_8:       return (char *)loc->time.months_full[7];
        case MON_9:       return (char *)loc->time.months_full[8];
        case MON_10:      return (char *)loc->time.months_full[9];
        case MON_11:      return (char *)loc->time.months_full[10];
        case MON_12:      return (char *)loc->time.months_full[11];
        case ALTMON_1:    return (char *)loc->time.months_full[0]; /* Stub for ALT */
        case ALTMON_2:    return (char *)loc->time.months_full[1];
        case ALTMON_3:    return (char *)loc->time.months_full[2];
        case ALTMON_4:    return (char *)loc->time.months_full[3];
        case ALTMON_5:    return (char *)loc->time.months_full[4];
        case ALTMON_6:    return (char *)loc->time.months_full[5];
        case ALTMON_7:    return (char *)loc->time.months_full[6];
        case ALTMON_8:    return (char *)loc->time.months_full[7];
        case ALTMON_9:    return (char *)loc->time.months_full[8];
        case ALTMON_10:   return (char *)loc->time.months_full[9];
        case ALTMON_11:   return (char *)loc->time.months_full[10];
        case ALTMON_12:   return (char *)loc->time.months_full[11];
        case ABMON_1:     return (char *)loc->time.months_abbr[0];
        case ABMON_2:     return (char *)loc->time.months_abbr[1];
        case ABMON_3:     return (char *)loc->time.months_abbr[2];
        case ABMON_4:     return (char *)loc->time.months_abbr[3];
        case ABMON_5:     return (char *)loc->time.months_abbr[4];
        case ABMON_6:     return (char *)loc->time.months_abbr[5];
        case ABMON_7:     return (char *)loc->time.months_abbr[6];
        case ABMON_8:     return (char *)loc->time.months_abbr[7];
        case ABMON_9:     return (char *)loc->time.months_abbr[8];
        case ABMON_10:    return (char *)loc->time.months_abbr[9];
        case ABMON_11:    return (char *)loc->time.months_abbr[10];
        case ABMON_12:    return (char *)loc->time.months_abbr[11];
        case ABALTMON_1:  return (char *)loc->time.months_abbr[0]; /* Stub */
        case ABALTMON_2:  return (char *)loc->time.months_abbr[1];
        case ABALTMON_3:  return (char *)loc->time.months_abbr[2];
        case ABALTMON_4:  return (char *)loc->time.months_abbr[3];
        case ABALTMON_5:  return (char *)loc->time.months_abbr[4];
        case ABALTMON_6:  return (char *)loc->time.months_abbr[5];
        case ABALTMON_7:  return (char *)loc->time.months_abbr[6];
        case ABALTMON_8:  return (char *)loc->time.months_abbr[7];
        case ABALTMON_9:  return (char *)loc->time.months_abbr[8];
        case ABALTMON_10: return (char *)loc->time.months_abbr[9];
        case ABALTMON_11: return (char *)loc->time.months_abbr[10];
        case ABALTMON_12: return (char *)loc->time.months_abbr[11];
        case ERA:         return (char *)"";
        case ERA_D_FMT:   return (char *)"";
        case ERA_D_T_FMT: return (char *)"";
        case ERA_T_FMT:   return (char *)"";
        case ALT_DIGITS:  return (char *)"";
        case RADIXCHAR:   return (char *)loc->lconv.decimal_point;
        case THOUSEP:     return (char *)loc->lconv.thousands_sep;
        case YESEXPR:     return (char *)"^[yY]";
        case NOEXPR:      return (char *)"^[nN]";
        case CRNCYSTR:    return (char *)loc->lconv.currency_symbol;
        default:          return (char *)"";
    }
}

/* --- Standard Variants - Wrappers --- */

static inline char *nl_langinfo(nl_item item) {
    return nl_langinfo_l(item, NULL);
}

#ifdef __cplusplus
}
#endif

#endif /* _LANGINFO_H */
