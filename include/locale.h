// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef LOCALE_H
#define LOCALE_H

#include <stddef.h>

/* C99 localization */
typedef struct {
		char *decimal_point;
		char *thousands_sep;
		char *grouping;
		char *int_curr_symbol;
		char *currency_symbol;
		char *mon_decimal_point;
		char *mon_thousands_sep;
		char *mon_grouping;
		char *positive_sign;
		char *negative_sign;
		char int_frac_digits;
		char frac_digits;
		char p_cs_precedes;
		char p_sep_by_space;
		char n_cs_precedes;
		char n_sep_by_space;
		char p_sign_posn;
		char n_sign_posn;
} lconv;

static inline char *setlocale(int category, const char *locale) {
	//TODO: Just return "C" for minimal support
	static char c_locale[] = "C";

	return (locale == NULL) ? c_locale : (strcmp(locale, "") == 0 || strcmp(locale, "C") == 0) ? c_locale : NULL;
}

static inline static lconv __jacl_lconv_default = {
	.decimal_point			= ".",
	.thousands_sep			= "",
	.grouping						= "",
	.int_curr_symbol		= "",
	.currency_symbol		= "",
	.mon_decimal_point	= "",
	.mon_thousands_sep	= "",
	.mon_grouping				= "",
	.positive_sign			= "",
	.negative_sign			= "",
	.int_frac_digits		= CHAR_MAX,
	.frac_digits				= CHAR_MAX,
	.p_cs_precedes			= CHAR_MAX,
	.p_sep_by_space			= CHAR_MAX,
	.n_cs_precedes			= CHAR_MAX,
	.n_sep_by_space			= CHAR_MAX,
	.p_sign_posn				= CHAR_MAX,
	.n_sign_posn				= CHAR_MAX
};

static inline lconv *localeconv(void) { return &__jacl_lconv_default; }

#define LC_ALL			 0
#define LC_COLLATE	 1
#define LC_CTYPE		 2
#define LC_MONETARY  3
#define LC_NUMERIC	 4
#define LC_TIME			 5

#endif /* LOCALE_H */
