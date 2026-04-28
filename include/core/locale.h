/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _CORE_LOCALE_H
#define _CORE_LOCALE_H
#pragma once

#include <config.h>
#include <locale.h>

// Internal Locale Object for Process
static __jacl_locale_t __jacl_locale_storage = {
	{ /* lcats */
		LANG_C,
		LANG_C,
		LANG_C,
		LANG_C,
		CC_NONE,
		CC_NONE
	},
	{ /* wctype */
		L"0123456789",
		L"ABCDEFGHIJKLMNOPQRSTUVWXYZ",
		L"abcdefghijklmnopqrstuvwxyz",
		L"", 26, 26, 0
	},
	{ /* time */
		{ "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" },
		{ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" },
		{ "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" },
		{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" },
		"AM", "PM",
		"%a %b %e %H:%M:%S %Y",
		"%m/%d/%y",
		"%H:%M:%S",
		"%I:%M:%S %p"
	},
	{ /* lconv */
		".", "", "", "", "", "", "", "", "", "",
		CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX,
		CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX, CHAR_MAX
	}
};

// Global Locale Object for Process
locale_t __jacl_locale_global = &__jacl_locale_storage;

// Current Locale Object for Thread
thread_local locale_t __jacl_locale_current = &__jacl_locale_storage;

#endif /* _CORE_LOCALE_H */
