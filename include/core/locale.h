/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef CORE_LOCALE_H
#define CORE_LOCALE_H
#pragma once

#include <config.h>
#include <locale.h>

thread_local struct lconv __jacl_lconv_default = {
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

thread_local const char *__jacle_day_names[7] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

thread_local const char *__jacle_day_abbrs[7] = {
	"Sun", "Mon", "Tues", "Wed", "Thur", "Fri", "Sat"
};

thread_local const char *__jacle_month_names[12] = {
	"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"
};

thread_local const char *__jacle_month_abbrs[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"
};

#endif // CORE_LOCALE_H
