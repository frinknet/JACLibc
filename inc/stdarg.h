// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef STDARG_H
#define STDARG_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
	/* Microsoft Visual C++ - uses system-provided vadefs.h */
	#include <vadefs.h>
#else
	/* Everyone else: GCC, Clang (including Xcode), XCC, TCC */
	typedef __builtin_va_list va_list;
	#define va_start(ap, last) __builtin_va_start(ap, last)
	#define va_arg(ap, type)	 __builtin_va_arg(ap, type)
	#define va_end(ap)				 __builtin_va_end(ap)
	#define va_copy(dest, src) __builtin_va_copy(dest, src)
#endif

#ifdef __cplusplus
}
#endif
#endif /* STDARG_H */
