/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STDATOMIC_H
#define STDATOMIC_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Memory‐order constants */
#define memory_order_relaxed	 __ATOMIC_RELAXED
#define memory_order_consume	 __ATOMIC_CONSUME
#define memory_order_acquire	 __ATOMIC_ACQUIRE
#define memory_order_release	 __ATOMIC_RELEASE
#define memory_order_acq_rel	 __ATOMIC_ACQ_REL
#define memory_order_seq_cst	 __ATOMIC_SEQ_CST

#ifdef __wasm__  /* **WASM** single-threaded mock */

typedef bool					 atomic_bool;
typedef char					 atomic_char;
typedef signed char		 atomic_schar;
typedef unsigned char  atomic_uchar;
typedef short					 atomic_short;
typedef unsigned short atomic_ushort;
typedef int						 atomic_int;
typedef unsigned int	 atomic_uint;
typedef long					 atomic_long;
typedef unsigned long  atomic_ulong;
typedef long long			 atomic_llong;
typedef unsigned long long atomic_ullong;
typedef void*					 atomic_voidptr;
typedef struct { bool _Value; } atomic_flag;
#define ATOMIC_FLAG_INIT { false }
#define ATOMIC_VAR_INIT(x)		 (x)
static inline void atomic_init(int *obj, int v) {
		*obj = v;
}

static inline int atomic_load(const int *obj) {
		return *obj;
}

static inline void atomic_store(int *obj, int v) {
		*obj = v;
}

static inline int atomic_exchange(int *obj, int v) {
		int old = *obj;
		*obj = v;
		return old;
}

static inline bool atomic_compare_exchange_strong(int *obj, int *exp, int des) {
		bool ok = (*obj == *exp);
		if (ok) {
				*obj = des;
		} else {
				*exp = *obj;
		}
		return ok;
}

static inline bool atomic_compare_exchange_weak(int *obj, int *exp, int des) {
		return atomic_compare_exchange_strong(obj, exp, des);
}

static inline int atomic_fetch_add(int *obj, int v) {
		int old = *obj;
		*obj += v;
		return old;
}

static inline int atomic_fetch_sub(int *obj, int v) {
		int old = *obj;
		*obj -= v;
		return old;
}

static inline int atomic_fetch_and(int *obj, int v) {
		int old = *obj;
		*obj &= v;
		return old;
}

static inline int atomic_fetch_or(int *obj, int v) {
		int old = *obj;
		*obj |= v;
		return old;
}

static inline int atomic_fetch_xor(int *obj, int v) {
		int old = *obj;
		*obj ^= v;
		return old;
}

static inline bool atomic_flag_test_and_set(atomic_flag *f) {
		bool old = f->_Value; f->_Value = true; return old;
}
static inline void atomic_flag_clear(atomic_flag *f) {
		f->_Value = false;
}

static inline void atomic_thread_fence(int mo) { (void)mo; }
static inline void atomic_signal_fence(int mo) { (void)mo; }

#else  /* non-WASM: require real atomics */

#if !defined(__has_builtin)
# define __has_builtin(x) 0
#endif
#if !(__has_builtin(__atomic_load_n) && __has_builtin(__atomic_store_n))
# error "Compiler must support __atomic builtins"
#endif

typedef _Atomic(bool)						atomic_bool;
typedef _Atomic(char)						atomic_char;
typedef _Atomic(signed char)		atomic_schar;
typedef _Atomic(unsigned char)	atomic_uchar;
typedef _Atomic(short)					atomic_short;
typedef _Atomic(unsigned short) atomic_ushort;
typedef _Atomic(int)						atomic_int;
typedef _Atomic(unsigned int)		atomic_uint;
typedef _Atomic(long)						atomic_long;
typedef _Atomic(unsigned long)	atomic_ulong;
typedef _Atomic(long long)			atomic_llong;
typedef _Atomic(unsigned long long) atomic_ullong;
typedef _Atomic(void*)					atomic_voidptr;
typedef struct { atomic_bool _Value; } atomic_flag;

#define ATOMIC_FLAG_INIT { false }
#define ATOMIC_VAR_INIT(x)		 (x)

static inline void atomic_init(atomic_int *obj, int v) {
		__atomic_store_n(obj, v, memory_order_relaxed);
}

static inline int atomic_load(const atomic_int *obj) {
		return __atomic_load_n(obj, memory_order_seq_cst);
}

static inline void atomic_store(atomic_int *obj, int v) {
		__atomic_store_n(obj, v, memory_order_seq_cst);
}

static inline int atomic_exchange(atomic_int *obj, int v) {
		return __atomic_exchange_n(obj, v, memory_order_seq_cst);
}

static inline bool atomic_compare_exchange_strong(atomic_int *obj, int *exp, int des) {
		return __atomic_compare_exchange_n(obj, exp, &des, false, memory_order_seq_cst, memory_order_seq_cst);
}

static inline bool atomic_compare_exchange_weak(atomic_int *obj, int *exp, int des) {
		return __atomic_compare_exchange_n(obj, exp, &des, true, memory_order_seq_cst, memory_order_seq_cst);
}

static inline int atomic_fetch_add(atomic_int *obj, int v) {
		return __atomic_fetch_add(obj, v, memory_order_seq_cst);
}

static inline int atomic_fetch_sub(atomic_int *obj, int v) {
		return __atomic_fetch_sub(obj, v, memory_order_seq_cst);
}

static inline int atomic_fetch_and(atomic_int *obj, int v) {
		return __atomic_fetch_and(obj, v, memory_order_seq_cst);
}

static inline int atomic_fetch_or(atomic_int *obj, int v) {
		return __atomic_fetch_or(obj, v, memory_order_seq_cst);
}

static inline int atomic_fetch_xor(atomic_int *obj, int v) {
		return __atomic_fetch_xor(obj, v, memory_order_seq_cst);
}

static inline bool atomic_flag_test_and_set(atomic_flag *f){
		return __atomic_test_and_set(&f->_Value, memory_order_seq_cst);
}
static inline void atomic_flag_clear(atomic_flag *f){
		__atomic_clear(&f->_Value, memory_order_seq_cst);
}

#define atomic_thread_fence(mo)		__atomic_thread_fence(mo)
#define atomic_signal_fence(mo)		__atomic_signal_fence(mo)

#endif /* __wasm__ */

#ifdef __cplusplus
}
#endif

#endif /* STDATOMIC_H */

