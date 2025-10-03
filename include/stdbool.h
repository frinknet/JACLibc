/* (c) 2025 FRINKnet & Friends - MIT licence */
#ifndef STDBOOL_H
#define STDBOOL_H
#pragma once

#include <config.h>

#ifndef __cplusplus

#if JACL_HAS_C23
    /* C23: bool, true, false are keywords - header is obsolete but allowed */
    /* No typedefs needed, but define the standard macro */
#elif JACL_HAS_C99
    /* C99-C17: _Bool exists, provide convenience macros */
    typedef _Bool bool;
		#define true  1
		#define false 0
#else
		/* Pre-C99: _Bool doesn't exist - cannot provide stdbool.h */
		#error "stdbool.h requires C99 or later (_Bool support needed)"
#endif

#endif /* !__cplusplus */

#define __bool_true_false_are_defined 1

#endif /* STDBOOL_H */
