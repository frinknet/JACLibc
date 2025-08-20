// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef STDARG_H
#define STDARG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef __builtin_va_list va_list;

static inline void va_start(va_list *ap, void *last) {
		__builtin_va_start(*ap, last);
}

static inline void *va_arg(va_list *ap, size_t type_size) {
		return __builtin_va_arg(*ap, void*);
}

static inline void va_end(va_list *ap) {
		__builtin_va_end(*ap);
}

static inline void va_copy(va_list *dest, const va_list *src) {
		__builtin_va_copy(*dest, *src);
}

#ifdef __cplusplus
}
#endif

#endif /* STDARG_H */

