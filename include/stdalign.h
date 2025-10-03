/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STDALIGN_H
#define STDALIGN_H
#pragma once

#include <config.h>

/* C23 makes alignas/alignof keywords, so this header is obsolete */
#if JACL_HAS_C23
	/* Keywords built into language - header not needed but allowed for compat */
	#define __alignas_is_defined 1
	#define __alignof_is_defined 1
#elif JACL_HAS_C11
	/* C11: map convenience macros to _Alignas/_Alignof */
	#define alignas _Alignas
	#define alignof _Alignof
	#define __alignas_is_defined 1
	#define __alignof_is_defined 1
#else
	/* Pre-C11: compiler-specific fallbacks */
	#if defined(__GNUC__) || defined(__clang__)
		#define alignas(x) __attribute__((aligned(x)))
		#define alignof __alignof__
		#define __alignas_is_defined 1
		#define __alignof_is_defined 1
	#else
		/* Unsupported - macros expand to nothing */
		#define alignas(x)
		#define alignof(x) 1  /* Return 1 instead of nothing to avoid syntax errors */
		/* Don't define __alignas_is_defined/__alignof_is_defined */
	#endif
#endif

#endif /* STDALIGN_H */
