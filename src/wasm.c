// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef WASM_C
#define WASM_C

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __wasm__
int __cxa_guard_acquire(void* g) { return 1; }
void __cxa_guard_release(void* g) { }
void __cxa_guard_abort(void* g) { }
int __cxa_atexit(void (*f)(void *), void *p, void *d) { return 0; }

double __trunctfdf2(long double x) { return (double)x; }
long double __floatsitf(int32_t i) { return (long double)i; }
long double __extenddftf2(double d) { return (long double)d; }
long double __extendsftf2(float f) { return (long double)f; }
#endif

#ifdef __cplusplus
}
#endif

#endif /* WASM_C */
