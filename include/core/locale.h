/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef CORE_LOCALE_H
#define CORE_LOCALE_H
#pragma once

#include <config.h>
#include <locale.h>

thread_local struct lconv __jacl_lconv = {
	/* numeric */
	".",           // decimal_point
	"",            // thousands_sep
	"",            // grouping

	/* monetary */
	"",            // int_curr_symbol
	"",            // currency_symbol
	"",            // mon_decimal_point
	"",            // mon_thousands_sep
	"",            // mon_grouping
	"",            // positive_sign
	"",            // negative_sign

	CHAR_MAX,      // frac_digits
	CHAR_MAX,      // p_cs_precedes
	CHAR_MAX,      // n_cs_precedes
	CHAR_MAX,      // p_sep_by_space
	CHAR_MAX,      // n_sep_by_space
	CHAR_MAX,      // p_sign_posn
	CHAR_MAX,      // n_sign_posn

	CHAR_MAX,      // int_frac_digits
	CHAR_MAX,      // int_p_cs_precedes
	CHAR_MAX,      // int_n_cs_precedes
	CHAR_MAX,      // int_p_sep_by_space
	CHAR_MAX,      // int_n_sep_by_space
	CHAR_MAX,      // int_p_sign_posn
	CHAR_MAX,      // int_n_sign_posn
};

typedef struct {
	LC_LANG_C;
			__jacl_locale_lang_t  time;
				__jacl_locale_lang_t  collate;
					__jacl_locale_lang_t  message;
						__jacl_locale_cc_t    numeric;
							__jacl_locale_cc_t    monetary;
} __jacl_locale_t;



thread_local __jacl_wctype_t __jacl_wctype = {
	L'0123456789',
	L'ABCDEFGHIJKLMNOPQRSTUVWXYZ',
	L'abcdefghijklmnopqrstuvwxyz'
};

thread_local __jacl_calendar_t __jacl_calendar = {
	"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday",
	"Sun","Mon","Tue","Wed","Thu","Fri","Sat",
	"January","February","March","April","May","June","July","August",
	"September","October","November","December",
	"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec",
	"AM","PM",
	"%a %b %e %H:%M:%S %Y",
	"%m/%d/%y",
	"%H:%M:%S",
	"%I:%M:%S %p"
};

#endif // CORE_LOCALE_H
