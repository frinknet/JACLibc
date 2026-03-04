/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef LOCALE_H
#define LOCALE_H
#pragma once

#include <config.h>
#include <limits.h>
#include <stddef.h>
#include <string.h>

#include <x/locale_languages.h>
#include <x/locale_countries.h>

#define LC_CTYPE    0
#define LC_NUMERIC  1
#define LC_TIME     2
#define LC_COLLATE  3
#define LC_MONETARY 4
#define LC_MESSAGES 5
#define LC_ALL      6

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LOCALE_MINIMUM
#define LOCALE_MINIMUM UINT_MAX
#endif

#define X(name, ...) name,
typedef enum locale_lang { X_LANGUAGES } __jacl_locale_lang_t;
typedef enum locale_cc { X_COUNTRIES } __jacl_locale_cc_t;
#undef X

typedef struct {
	__jacl_locale_lang_t ctype;
	__jacl_locale_lang_t numeric;
	__jacl_locale_lang_t time;
	__jacl_locale_cc_t   monetary;
	__jacl_locale_lang_t collate;
	__jacl_locale_lang_t message;
} __jacl_locale_t;

/* C99 localization */
struct lconv {
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
};

extern thread_local __jacl_locale_t __jacl_locale;
extern thread_local struct lconv    __jacl_lconv;

// Parse language code
static __jacl_locale_lang_t __jacl_locale_lang(const char *lang) {
	size_t len = strlen(lang);

	if (len == 2) {
		uint16_t code = (lang[0] << 8) | lang[1];

		#define X(LANG, code2, code3) case code2: return LANG;
		switch (code) { X_LANGUAGES }
		#undef X
	} else if (len == 3) {
		uint32_t code = (lang[0] << 16) | (lang[1] << 8) | lang[2];

		#define X(LANG, code2, code3) case code3: return LANG;
		switch (code) { X_LANGUAGES }
		#undef X
	}

	return LANG_C;
}

// Parse country code
static __jacl_locale_cc_t __jacl_locale_cc(const char *cc) {
	uint16_t code = (cc[0] << 8) | cc[1];

	#define X(CC, code) case code: return CC;
	switch (code) { X_COUNTRIES }
	#undef X

	return CC_NONE;
}

// Parse locale
static void __jacl_locale_split(const char *s, __jacl_locale_lang_t *out_lang, __jacl_locale_cc_t *out_cc) {
	const char *p = s;
	char lang[4];
	char cc[3];
	unsigned i = 0, j = 0;

	/* lang: up to '_' or end, max 3 */
	while (p[i] && p[i] != '_' && i < 3)
		lang[i] = p[i], i++;
	lang[i] = '\0';

	/* CC: if '_' present, copy next up-to-2 chars */
	if (p[i] == '_') {
		const char *q = p + i + 1;
		while (q[j] && j < 2) cc[j] = q[j], j++;
	}
	cc[j] = '\0';

	*out_lang = __jacl_locale_lang(lang);
	*out_cc   = __jacl_locale_cc(cc);
}

static void __jacl_locale_update(int category, __jacl_locale_lang_t lang, __jacl_locale_cc_t   cc) {
	switch (category) {
	case LC_CTYPE:    __jacl_locale.ctype    = lang; break;
	case LC_NUMERIC:  __jacl_locale.numeric  = lang; break;
	case LC_TIME:     __jacl_locale.time     = lang; break;
	case LC_COLLATE:  __jacl_locale.collate  = lang; break;
	case LC_MESSAGES: __jacl_locale.message  = lang; break;
	case LC_MONETARY: __jacl_locale.monetary = cc;   break;
	case LC_ALL:
		__jacl_locale.ctype    = lang;
		__jacl_locale.numeric  = lang;
		__jacl_locale.time     = lang;
		__jacl_locale.collate  = lang;
		__jacl_locale.message  = lang;
		__jacl_locale.monetary = cc;
		break;
	}
}

static inline char *setlocale(int category, const char *locale) {
	if (locale == NULL) return (char *)"C";

	__jacl_locale_lang_t L;
	__jacl_locale_cc_t   C;

	__jacl_locale_split(locale, &L, &C);
	__jacl_locale_update(category, L, C);

	return (char *)locale;
}

static inline struct lconv *localeconv(void) { return &__jacl_lconv; }

#ifdef __cplusplus
}
#endif

#endif // LOCALE_H
