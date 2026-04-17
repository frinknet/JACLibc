/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _STDALIGN_H
#define _STDALIGN_H
#pragma once

#include <config.h>

/* C23 makes alignas/alignof keywords, so this header is obsolete */
#if JACL_HAS_C23
	#define __alignas_is_defined 1
	#define __alignof_is_defined 1
#elif JACL_HAS_C11
	#define alignas _Alignas
	#define alignof _Alignof
	#define __alignas_is_defined 1
	#define __alignof_is_defined 1
#else
	#error "stdalign.h reqires C11 or later"
#endif

#endif /* _STDALIGN_H */
