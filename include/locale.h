/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _LOCALE_H
#define _LOCALE_H
#pragma once

#include <config.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <x/locale_languages.h>
#include <x/locale_countries.h>

#define LC_CTYPE         0
#define LC_NUMERIC       1
#define LC_TIME	         2
#define LC_COLLATE       3
#define LC_MONETARY      4
#define LC_MESSAGES      5
#define LC_ALL           6

#define LC_CTYPE_MASK    (1 << LC_CTYPE)
#define LC_NUMERIC_MASK  (1 << LC_NUMERIC)
#define LC_TIME_MASK     (1 << LC_TIME)
#define LC_COLLATE_MASK  (1 << LC_COLLATE)
#define LC_MONETARY_MASK (1 << LC_MONETARY)
#define LC_MESSAGES_MASK (1 << LC_MESSAGES)
#define LC_ALL_MASK      ((1 << LC_ALL) - 1)

#define LC_GLOBAL_LOCALE ((locale_t)-1)
#define LC_THREAD_LOCALE ((locale_t)-2)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOCALE_MINIMUM
#define LOCALE_MINIMUM UINT_MAX
#endif

#define X(code, ...) code,
typedef enum locale_lang { X_LANGUAGES LANG_ERR } __jacl_locale_lang_t;
typedef enum locale_cc { X_COUNTRIES CC_ERR } __jacl_locale_cc_t;
#undef X

typedef struct {
	wchar_t d[10];   // Digits
	wchar_t u[256];  // Upper
	wchar_t l[256];  // Lower
	wchar_t e[256];  // Extra
	uint8_t u_count; // Upper Count
	uint8_t l_count; // Lower Count
	uint8_t e_count; // Extra Count
} __jacl_wctype_t;

typedef struct {
	__jacl_locale_lang_t  wctype;
	__jacl_locale_lang_t  time;
	__jacl_locale_lang_t  collate;
	__jacl_locale_lang_t  message;
	__jacl_locale_cc_t    numeric;
	__jacl_locale_cc_t    monetary;
} __jacl_lcats_t;

typedef struct {
	const char *days_full[7];
	const char *days_abbr[7];
	const char *months_full[12];
	const char *months_abbr[12];
	const char *am;
	const char *pm;
	const char *fmt_datetime;
	const char *fmt_date;
	const char *fmt_time_24;
	const char *fmt_time_12;
} __jacl_time_t;

typedef struct lconv {
	/* numeric */
	char	*decimal_point;
	char	*thousands_sep;
	char	*grouping;

	/* monetary */
	char	*int_curr_symbol;
	char	*currency_symbol;
	char	*mon_decimal_point;
	char	*mon_thousands_sep;
	char	*mon_grouping;
	char	*positive_sign;
	char	*negative_sign;

	char	frac_digits;
	char	p_cs_precedes;
	char	n_cs_precedes;
	char	p_sep_by_space;
	char	n_sep_by_space;
	char	p_sign_posn;
	char	n_sign_posn;

	char	int_frac_digits;
	char	int_p_cs_precedes;
	char	int_n_cs_precedes;
	char	int_p_sep_by_space;
	char	int_n_sep_by_space;
	char	int_p_sign_posn;
	char	int_n_sign_posn;
} __jacl_lconv_t;

typedef struct {
	__jacl_lcats_t  lcats;   // lang/cc category
	__jacl_wctype_t wctype;  // your tables
	__jacl_time_t   time;    // your time data
	__jacl_lconv_t  lconv;   // numeric/monetary
} __jacl_locale_t;

typedef __jacl_locale_t *locale_t;

extern locale_t __jacl_locale_global;
extern thread_local locale_t __jacl_locale_current;

static const char *__jacl_locale_env(int cat) {
	const char *env = getenv("LC_ALL");

	if (!env || !env[0]) {
		switch (cat) {
			case LC_CTYPE:	  env = getenv("LC_CTYPE"); break;
			case LC_NUMERIC:  env = getenv("LC_NUMERIC"); break;
			case LC_TIME:	    env = getenv("LC_TIME"); break;
			case LC_COLLATE:  env = getenv("LC_COLLATE"); break;
			case LC_MONETARY: env = getenv("LC_MONETARY"); break;
			case LC_MESSAGES: env = getenv("LC_MESSAGES"); break;
		}
	}

	if (!env || !env[0]) env = getenv("LANG");
	if (!env || !env[0]) env = "C";

	return env;
}

static __jacl_locale_lang_t __jacl_locale_lang(const char *lang) {
	size_t len = strlen(lang);

	if (len == 1) {
		return (*lang == 'C') ? LANG_C : LANG_ERR;
	} else if (len == 2) {
		uint16_t code = (toupper(lang[0]) << 8) | toupper(lang[1]);

		#define X(LANG, name, code2, code3) case code2: return LANG;
		switch (code) { X_LANGUAGES }
		#undef X
	} else if (len == 3) {
		uint32_t code = (toupper(lang[0]) << 16) | (toupper(lang[1]) << 8) | toupper(lang[2]);

		#define X(LANG, name, code2, code3) case code3: return LANG;
		switch (code) { X_LANGUAGES }
		#undef X
	}

	return LANG_ERR;
}

static __jacl_locale_cc_t __jacl_locale_cc(const char *cc) {
	if (*cc == '\0') return CC_NONE;

	uint16_t code = (toupper(cc[0]) << 8) | toupper(cc[1]);

	#define X(CC, name, code) case code: return CC;
	switch (code) { X_COUNTRIES }
	#undef X

	return CC_ERR;
}

static void __jacl_locale_split(const char *s, __jacl_locale_lang_t *out_lang, __jacl_locale_cc_t *out_cc) {
	const char *p = s;
	char lang[4];
	char cc[3];
	unsigned i = 0, j =  0;

	if (strcmp(s, "C")) {
		// lang: up to '_' or end, max 3
		while (islower(p[i]) && p[i] != '_' && i < 3) lang[i] = toupper(p[i]), i++;
	} else {
		lang[0] = s[0];
		i = 1;
	}

	lang[i] = '\0';
	j = i;
	i  = 0;

	// CC: if '_' present, copy next up-to-2 chars
	if (s[j] == '_') {
		p = s + j + 1;

		while (isupper(p[i]) && i < 2) cc[i] = toupper(p[i]), i++;

		j += i + 1;
	}

	cc[i] = '\0';

	// extra forces lang to fail which cascades
	if (isalpha(s[j])) lang[0] = '\0';

	*out_lang = __jacl_locale_lang(lang);
	*out_cc   = __jacl_locale_cc(cc);
}

static void __jacl_locale_wctype(__jacl_locale_lang_t lang, locale_t l) {
	l->lcats.wctype = lang;

	#define X(LANG, ...) case LANG: l->wctype = (__jacl_wctype_t){ __VA_ARGS__ }; break;
	switch(lang) {
	#include <x/locale_wctype.h>
	}
  #undef X
}

static void __jacl_locale_numeric(__jacl_locale_cc_t cc, locale_t l) {
	l->lcats.numeric = cc;

	#define X(CC, dec, sep, group) \
		case CC: \
			l->lconv.decimal_point = dec; \
			l->lconv.thousands_sep = sep; \
			l->lconv.grouping = group; \
			break;
	switch(cc) {
	#include <x/locale_numeric.h>
	}
	#undef X
}

static void __jacl_locale_time(__jacl_locale_lang_t lang, locale_t l) {
	l->lcats.time = lang;

	#define X(LANG, ...) case LANG: l->time = (__jacl_time_t){ __VA_ARGS__ }; break;
	switch(lang) {
	#include <x/locale_time.h>
	}
  #undef X
}

static void __jacl_locale_collate(__jacl_locale_lang_t lang, locale_t l) {
	l->lcats.collate = lang;
	// Update collation rules (affects strcoll, strxfrm)
}

static void __jacl_locale_message(__jacl_locale_lang_t lang, locale_t l) {
	l->lcats.message = lang;
	// Update message formatting (affects gettext, error messages)
}

static void __jacl_locale_monetary(__jacl_locale_cc_t cc, locale_t l) {
	l->lcats.monetary = cc;

	#define X(name, curr, mdec, mthou, mgrp, pos, neg, ifd, fd, pcp, psb, ncp, nsb, psp, nsp) \
		case name: \
			l->lconv.int_curr_symbol = curr; \
			l->lconv.currency_symbol = curr; \
			l->lconv.mon_decimal_point = mdec; \
			l->lconv.mon_thousands_sep = mthou; \
			l->lconv.mon_grouping = mgrp; \
			l->lconv.positive_sign = pos; \
			l->lconv.negative_sign = neg; \
			l->lconv.int_frac_digits = ifd; \
			l->lconv.frac_digits = fd; \
			l->lconv.int_p_cs_precedes = pcp; \
			l->lconv.p_cs_precedes = pcp; \
			l->lconv.int_p_sep_by_space = psb; \
			l->lconv.p_sep_by_space = psb; \
			l->lconv.int_n_cs_precedes = ncp; \
			l->lconv.n_cs_precedes = ncp; \
			l->lconv.int_n_sep_by_space = nsb; \
			l->lconv.n_sep_by_space = nsb; \
			l->lconv.int_p_sign_posn = psp; \
			l->lconv.p_sign_posn = psp; \
			l->lconv.int_n_sign_posn = nsp; \
			l->lconv.n_sign_posn = nsp; \
			break;
	switch (cc) {
	#include <x/locale_monetary.h>
	}
	#undef X
}

static void __jacl_locale_update(int cat, __jacl_locale_lang_t lang, __jacl_locale_cc_t cc, locale_t l) {
	switch (cat) {
	case LC_CTYPE:    __jacl_locale_wctype(lang, l);  break;
	case LC_TIME:  	  __jacl_locale_time(lang, l);    break;
	case LC_COLLATE:  __jacl_locale_collate(lang, l); break;
	case LC_MESSAGES: __jacl_locale_message(lang, l); break;
	case LC_NUMERIC:  __jacl_locale_numeric(cc, l);   break;
	case LC_MONETARY: __jacl_locale_monetary(cc, l);  break;
	case LC_ALL:
		__jacl_locale_wctype(lang, l);
		__jacl_locale_time(lang, l);
		__jacl_locale_collate(lang, l);
		__jacl_locale_message(lang, l);
		__jacl_locale_numeric(cc, l);
		__jacl_locale_monetary(cc, l);

		break;
	}
}

static char *__jacl_locale_get(int cat, locale_t l) {
	static thread_local char buf[32];
	__jacl_locale_lang_t lang = l->lcats.wctype;
	__jacl_locale_cc_t cc = l->lcats.numeric;
	char *lstr;
	char *cstr;

	switch (cat) {
		case LC_CTYPE:	  lang = l->lcats.wctype; break;
		case LC_TIME:	    lang = l->lcats.time; break;
		case LC_COLLATE:  lang = l->lcats.collate; break;
		case LC_MESSAGES: lang = l->lcats.message; break;
		case LC_NUMERIC:  cc   = l->lcats.numeric; break;
		case LC_MONETARY: cc   = l->lcats.monetary; break;
	}

	#define X(LANG, name, ...) case LANG: lstr = name; break;
	switch(lang) {
		X_LANGUAGES
		default: lstr = "C";
	}
	#undef X

	#define X(CC, name, ...) case CC: cstr = name; break;
	switch(cc) {
		X_COUNTRIES
		default: cstr = "";
	}
	#undef X

	snprintf(buf, sizeof(buf), "%s%s%s.UTF-8", lstr, (*cstr) ? "_" : "", cstr);

	return buf;
}

static inline void freelocale(locale_t l) {
	if (l != __jacl_locale_global && l != LC_GLOBAL_LOCALE) free(l);
}

static inline locale_t duplocale(locale_t l) {
	if (l == NULL) { errno = EINVAL; return NULL; }
	if (l == LC_GLOBAL_LOCALE) l = __jacl_locale_global;
	if (l == LC_THREAD_LOCALE) l = __jacl_locale_current;

	locale_t nl = malloc(sizeof(__jacl_locale_t));

	if (!nl) return NULL;

	memcpy(nl, l, sizeof(__jacl_locale_t));

	return nl;
}

static inline locale_t newlocale(int mask, const char *name, locale_t l) {
	if (l == (locale_t)0) l = __jacl_locale_global;
	if (l == LC_THREAD_LOCALE) l = __jacl_locale_current;
	if (l == LC_GLOBAL_LOCALE) l = __jacl_locale_global;

	locale_t nl = duplocale(l);

	if (!nl) return NULL;

	/* Parse once */
	__jacl_locale_lang_t L;
	__jacl_locale_cc_t C;

	__jacl_locale_split(name, &L, &C);

	/* Validate language parsing */
	if ((mask & (LC_CTYPE_MASK | LC_TIME_MASK | LC_COLLATE_MASK | LC_MESSAGES_MASK)) && L == LANG_ERR) {
		freelocale(nl);

		errno = EINVAL;

		return NULL;
	}

	/* Validate region parsing */
	if ((mask & (LC_NUMERIC_MASK | LC_MONETARY_MASK)) && C == CC_ERR) {
		freelocale(nl);

		errno = EINVAL;

		return NULL;
	}

	/* Apply to each set bit */
	if (mask & LC_CTYPE_MASK)    __jacl_locale_wctype(L, nl);
	if (mask & LC_NUMERIC_MASK)  __jacl_locale_numeric(C, nl);
	if (mask & LC_TIME_MASK)     __jacl_locale_time(L, nl);
	if (mask & LC_COLLATE_MASK)  __jacl_locale_collate(L, nl);
	if (mask & LC_MESSAGES_MASK) __jacl_locale_message(L, nl);
	if (mask & LC_MONETARY_MASK) __jacl_locale_monetary(C, nl);

	return nl;
}

static inline char *setlocale(int cat, const char *name) {
	if (name == NULL) return __jacl_locale_get(cat, __jacl_locale_current);
	if (name[0] == '\0') name = __jacl_locale_env(cat);
	if (strcmp(name, "POSIX") == 0) name = "C";
	if (strcmp(name, __jacl_locale_get(cat, __jacl_locale_current)) == 0) return __jacl_locale_get(cat, __jacl_locale_current);

	__jacl_locale_lang_t L;
	__jacl_locale_cc_t   C;

	__jacl_locale_split(name, &L, &C);

	if (L == LANG_ERR || C == CC_ERR) return NULL;

	__jacl_locale_update(cat, L, C, __jacl_locale_global);

	__jacl_locale_current = __jacl_locale_global;

	return __jacl_locale_get(cat, __jacl_locale_current);
}

static inline locale_t uselocale(locale_t l) {
	if (l == NULL) l = __jacl_locale_current;
	if (l == NULL) l = __jacl_locale_global;
	if (l == LC_GLOBAL_LOCALE) l = __jacl_locale_global;
	if (l == LC_THREAD_LOCALE) l = __jacl_locale_current;
	if (l == NULL) { errno = EINVAL; return __jacl_locale_current; }

	locale_t old = __jacl_locale_current;
	__jacl_locale_current = l;

	return old;
}

static inline struct lconv *localeconv(void) {
	return &__jacl_locale_current->lconv;
}

#ifdef __cplusplus
}
#endif

#endif /* _LOCALE_H */
