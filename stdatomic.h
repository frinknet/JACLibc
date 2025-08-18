// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef STDATOMIC_H
#define STDATOMIC_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L && !defined(__STDC_NO_ATOMICS__)

/* C11 Atomic Types */
typedef _Bool													atomic_bool;
typedef _Atomic(char)									atomic_char;
typedef _Atomic(signed char)					atomic_schar;
typedef _Atomic(unsigned char)				atomic_uchar;
typedef _Atomic(short)								atomic_short;
typedef _Atomic(unsigned short)				atomic_ushort;
typedef _Atomic(int)									atomic_int;
typedef _Atomic(unsigned int)					atomic_uint;
typedef _Atomic(long)									atomic_long;
typedef _Atomic(unsigned long)				atomic_ulong;
typedef _Atomic(long long)						atomic_llong;
typedef _Atomic(unsigned long long)		atomic_ullong;
typedef _Atomic(void*)								atomic_voidptr;

/* Lock‐free flag type */
typedef struct { atomic_bool _Value; } atomic_flag;
#define ATOMIC_FLAG_INIT { false }

/* Memory Order */
#define memory_order_relaxed			__ATOMIC_RELAXED
#define memory_order_consume		 __ATOMIC_CONSUME
#define memory_order_acquire		 __ATOMIC_ACQUIRE
#define memory_order_release		 __ATOMIC_RELEASE
#define memory_order_acq_rel		 __ATOMIC_ACQ_REL
#define memory_order_seq_cst		 __ATOMIC_SEQ_CST

/* Atomic Initialization */
#define ATOMIC_VAR_INIT(x)				(x)
#define atomic_init(obj,v)				__atomic_init(&(obj),(v))

/* Atomic Load/Store */
#define atomic_load(obj)					__atomic_load_n(&(obj), memory_order_seq_cst)
#define atomic_store(obj,v)				__atomic_store_n(&(obj),(v), memory_order_seq_cst)

/* Atomic Exchange & Compare-Exchange */
#define atomic_exchange(obj,v)		__atomic_exchange_n(&(obj),(v), memory_order_seq_cst)
#define atomic_compare_exchange_strong(obj, expected, v) \
		__atomic_compare_exchange_n(&(obj),(expected),(v), false, memory_order_seq_cst, memory_order_seq_cst)
#define atomic_compare_exchange_weak(obj, expected, v) \
		__atomic_compare_exchange_n(&(obj),(expected),(v), true, memory_order_seq_cst, memory_order_seq_cst)

/* Atomic Fetch Operations */
#define atomic_fetch_add(obj,v)		__atomic_fetch_add(&(obj),(v), memory_order_seq_cst)
#define atomic_fetch_sub(obj,v)		__atomic_fetch_sub(&(obj),(v), memory_order_seq_cst)
#define atomic_fetch_and(obj,v)		__atomic_fetch_and(&(obj),(v), memory_order_seq_cst)
#define atomic_fetch_or(obj,v)		__atomic_fetch_or(&(obj),(v), memory_order_seq_cst)
#define atomic_fetch_xor(obj,v)		__atomic_fetch_xor(&(obj),(v), memory_order_seq_cst)

/* Atomic Flag Test-and-Set / Clear */
static inline bool atomic_flag_test_and_set(atomic_flag* f) {
		return __atomic_test_and_set(&f->_Value, memory_order_seq_cst);
}
static inline void atomic_flag_clear(atomic_flag* f) {
		__atomic_clear(&f->_Value, memory_order_seq_cst);
}

/* Atomic Thread Fences */
static inline void atomic_thread_fence(int mo) {
		__atomic_thread_fence(mo);
}
static inline void atomic_signal_fence(int mo) {
		__atomic_signal_fence(mo);
}

#else
	#error "C11 atomics not supported by this compiler/target"
#endif

#ifdef __cplusplus
}
#endif

#endif /* STDATOMIC_H */

