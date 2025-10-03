/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef ARCH_WASM_HELPERS_H
#define ARCH_WASM_HELPERS_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char	__jacl_aint_t;

/* C++ Static Initialization Guards */
#ifdef JACL_HAS_ATOMIC_BUILTINS
// Thread-safe implementation using atomics
typedef struct {
  _Atomic __jacl_aint_t initialized;
  _Atomic __jacl_aint_t in_progress;
} __guard_t;

static inline int __cxa_guard_acquire(void* g) {
  if (!g) return 0;
  __guard_t* guard = (__guard_t*)g;

  // Fast path: already initialized
  if (__atomic_load_n(&guard->initialized, __ATOMIC_ACQUIRE)) return 0;

  // Try to claim initialization
  __jacl_aint_t expected = 0;
  if (__atomic_compare_exchange_n(&guard->in_progress, &expected, 1, 0, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE))  return 1;

  // Another thread is initializing, spin wait
  while (!__atomic_load_n(&guard->initialized, __ATOMIC_ACQUIRE)) {
      // Busy wait
			//sleepish maybe
  }

  return 0;
}

static inline void __cxa_guard_release(void* g) {
  __guard_t* guard = (__guard_t*)g;
  __atomic_store_n(&guard->initialized, 1, __ATOMIC_RELEASE);
}

static inline void __cxa_guard_abort(void* g) {
  __guard_t* guard = (__guard_t*)g;
  __atomic_store_n(&guard->in_progress, 0, __ATOMIC_RELEASE);
}

#else

// Single-threaded implementation
static inline int __cxa_guard_acquire(void* g) { __jacl_aint_t* guard = (__jacl_aint_t*)g; if (*guard) return 0; *guard = 1; return 1; }
static inline void __cxa_guard_release(void* g) { __jacl_aint_t* guard = (__jacl_aint_t*)g; *guard = 0; }
static inline void __cxa_guard_abort(void* g) { (void)g; }

#endif /* JACL_HAS_ATOMIC_BUILTINS */

//TODO FIXME
static inline int __cxa_atexit(void (*f)(void *), void *p, void *d) { return 0; }

static inline double __trunctfdf2(long double x) { return (double)x; }
static inline long double __floatsitf(int i) { return (long double)i; }
static inline long double __extenddftf2(double d) { return (long double)d; }
static inline long double __extendsftf2(float f) { return (long double)f; }

#ifdef __cplusplus
}
#endif

#endif /* ARCH_WASM_HELPERS_H */
