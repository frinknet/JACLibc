/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef CORE_STDLIB_H
#define CORE_STDLIB_H

#include <config.h>
#include <stdlib.h>
#include <errno.h>
#include <core/format.h>
#include <core/memory.h>

#ifdef __cplusplus
extern {
#endif

/* ============================================================= */
/* Exit Handler Registry                                         */
/* ============================================================= */

#define __JACL_ATEXIT_MAX 32

static struct {
	void (*func)(void);
	int used;
} __jacl_exit_handlers[__JACL_ATEXIT_MAX];

unsigned 	     __jacl_rand_seed = RAND_MAX;
unsigned short __jacl_rand48_seed[3] = { 0x330E, 0xABCD, 0x1234 };
unsigned short __jacl_rand48_mult[3] = { 0xE66D, 0xDEEC, 0x0005 };
unsigned short __jacl_rand48_add = 0x000B;
unsigned short __jacl_rand48_oldseed[3] = { 0, 0, 0 };

uint_least32_t __jacl_random_state_default[31] = {0};
uint_least32_t *__jacl_random_state = __jacl_random_state_default;
uint_least32_t *__jacl_random_fptr = &__jacl_random_state_default[3];
uint_least32_t *__jacl_random_rptr = &__jacl_random_state_default[0];

static int __jacl_exit_count = 0;

#if JACL_HAS_C11
static struct {
	void (*func)(void);
	int used;
} __jacl_quick_exit_handlers[__JACL_ATEXIT_MAX];

static int __jacl_quick_exit_count = 0;
#endif

int atexit(void (*func)(void)) {
	if (!func) return -1;

	for (int i = 0; i < __JACL_ATEXIT_MAX; i++) {
		if (!__jacl_exit_handlers[i].used) {
			__jacl_exit_handlers[i].func = func;
			__jacl_exit_handlers[i].used = 1;
			__jacl_exit_count++;

			return 0;
		}
	}

	return -1;
}

static inline void __jacl_exit_run_handlers(void) {
	for (int i = __JACL_ATEXIT_MAX - 1; i >= 0; i--) {
		if (__jacl_exit_handlers[i].used && __jacl_exit_handlers[i].func) {
			__jacl_exit_handlers[i].func();
		}
	}
}

#if JACL_HAS_C11

int at_quick_exit(void (*func)(void)) {
	if (!func) return -1;

	for (int i = 0; i < __JACL_ATEXIT_MAX; i++) {
		if (!__jacl_quick_exit_handlers[i].used) {
			__jacl_quick_exit_handlers[i].func = func;
			__jacl_quick_exit_handlers[i].used = 1;
			__jacl_quick_exit_count++;

			return 0;
		}
	}

	return -1;
}

static inline void __jacl_exit_run_quick_handlers(void) {
	for (int i = __JACL_ATEXIT_MAX - 1; i >= 0; i--) {
		if (__jacl_quick_exit_handlers[i].used && __jacl_quick_exit_handlers[i].func) {
			__jacl_quick_exit_handlers[i].func();
		}
	}
}

noreturn void quick_exit(int status) {
	__jacl_exit_run_quick_handlers();
	_Exit(status);

	for(;;);
}

#endif /* JACL_HAS_C11 */


#if JACL_HAS_C99
noreturn void _Exit(int status) { _exit(status); }
#endif

void abort(void) {
	raise(SIGABRT);  // Standard attempt

	#if JACL_ARCH_X64 || JACL_ARCH_X86
		__asm__ volatile ("ud2");  // Invalid opcode
	#elif JACL_ARCH_ARM64 || JACL_ARCH_ARM32
			__asm__ volatile ("brk #0");	// Breakpoint
	#elif JACL_ARCH_RISCV
			__asm__ volatile ("ebreak");	// Environment break
	#elif JACL_ARCH_WASM
			__asm__ volatile ("unreachable");  // WASM trap
	#else
			*(volatile int*)0 = 0;	// Universal fallback
	#endif

	for(;;) {}	// The heat death of the universe
}

void exit(int status) {
	__jacl_exit_run_handlers();

	#if JACL_HAS_C99
		_Exit(status);
	#else
		(void)status;

		abort();
	#endif
}

/* ============================================================= */
/* String Conversion                                             */
/* ============================================================= */
#define __jacl_ato_int(name, type, len, usig) type ato##name(const char* str) { \
	const char* p = str; \
	int read = 0, ch = 0; \
	uintmax_t val = 0; \
	__jacl_fmt_t spec = usig? 10 : 10 | JACL_FMT_VAL(FLAG, sign); \
	__jacl_read_skip(ch, NULL, &p, &read); \
	if (__jacl_input_int(NULL, &p, &read, JACL_FMT_SET(LENGTH, spec, len), INT_MAX, &val)) return (type)val; \
	return 0; \
}
#define __jacl_strto_int(suf, type, len, usig) type strto##suf(const char* restrict str, char** restrict endptr, int base) { \
	if (base != 0 && (base < 2 || base > 36)) { \
		__errno_set(EINVAL); \
		if (endptr) *endptr = (char*)str; \
		return 0; \
	} \
	const char* p = str; \
	int read = 0, ch = 0; \
	uintmax_t val = 0; \
	__jacl_fmt_t spec = usig? base : base | JACL_FMT_VAL(FLAG, sign); \
	__jacl_read_skip(ch, NULL, &p, &read); \
	if (__jacl_input_int(NULL, &p, &read, JACL_FMT_SET(LENGTH, spec, len), INT_MAX, &val)) { \
		if (endptr) *endptr = (char*)p; \
		return (type)val; \
	} \
	if (endptr) *endptr = (char*)str; \
	return 0; \
}
#define __jacl_strto_float(suf, type, len) type strto##suf(const char* restrict str, char** restrict endptr) { \
	const char* p = str; \
	int read = 0, ch = 0; \
	long double val = 0.0; \
	__jacl_read_skip(ch, NULL, &p, &read); \
	__jacl_fmt_t spec = 10; \
	JACL_FMT_SET(LENGTH, spec, len); \
	if (__jacl_input_special(NULL, &p, &read, spec, INT_MAX, &val) || __jacl_input_float(NULL, &p, &read, spec, INT_MAX, &val)) { \
		if (endptr) *endptr = (char*)p; \
		return (type)val; \
	} \
	if (endptr) *endptr = (char*)str; \
}

__jacl_ato_int(i, int, 0, 0)
__jacl_ato_int(l, long, l, 0)

__jacl_strto_int(l, long, l, 0)
__jacl_strto_int(ul, unsigned long, l, 1)

#if JACL_HAS_C99
__jacl_ato_int(ll, long long, ll, 0)

__jacl_strto_int(ll, long long, ll, 0)
__jacl_strto_int(ull, unsigned long long, ll, 1)

__jacl_strto_float(f, float, 0)
__jacl_strto_float(d, double, l)
__jacl_strto_float(ld, long double, L)
#endif

double atof(const char* str) {
	const char* p = str;
	int _read = 0;
	long double val = 0.0;

	if (__jacl_input_float(NULL, &p, &_read, JACL_FMT_BASE_exp, INT_MAX, &val)) return (double)val;

	return 0.0;
}

#ifdef __cplusplus
}
#endif

#endif /* CORE_STDLIB_H */
