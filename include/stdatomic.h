/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STDATOMIC_H
#define STDATOMIC_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Memory‐order constants */
#define memory_order_relaxed   __ATOMIC_RELAXED
#define memory_order_consume   __ATOMIC_CONSUME
#define memory_order_acquire   __ATOMIC_ACQUIRE
#define memory_order_release   __ATOMIC_RELEASE
#define memory_order_acq_rel   __ATOMIC_ACQ_REL
#define memory_order_seq_cst   __ATOMIC_SEQ_CST

#ifdef __wasm__  /* **WASM** single-threaded mock */

typedef bool           atomic_bool;
typedef char           atomic_char;
typedef signed char    atomic_schar;
typedef unsigned char  atomic_uchar;
typedef short          atomic_short;
typedef unsigned short atomic_ushort;
typedef int            atomic_int;
typedef unsigned int   atomic_uint;
typedef long           atomic_long;
typedef unsigned long  atomic_ulong;
typedef long long      atomic_llong;
typedef unsigned long long atomic_ullong;
typedef void*          atomic_voidptr;
typedef struct { bool _Value; } atomic_flag;
#define ATOMIC_FLAG_INIT { false }
#define ATOMIC_VAR_INIT(x)     (x)
#define atomic_init(obj,v)     (*(obj) = (v))

#define atomic_load(obj)       (*(obj))
#define atomic_store(obj,v)    (*(obj) = (v))
#define atomic_exchange(obj,v) ({ __auto_type _old = *(obj); *(obj) = (v); _old; })
#define atomic_compare_exchange_strong(obj,exp,des) \
    ({ bool _ok = (*(obj) == *(exp)); if (_ok) *(obj) = (des); else *(exp) = *(obj); _ok; })
#define atomic_compare_exchange_weak  atomic_compare_exchange_strong
#define atomic_fetch_add(obj,v) ({ __auto_type _old = *(obj); *(obj) += (v); _old; })
#define atomic_fetch_sub(obj,v) ({ __auto_type _old = *(obj); *(obj) -= (v); _old; })
#define atomic_fetch_and(obj,v) ({ __auto_type _old = *(obj); *(obj) &= (v); _old; })
#define atomic_fetch_or(obj,v)  ({ __auto_type _old = *(obj); *(obj) |= (v); _old; })
#define atomic_fetch_xor(obj,v) ({ __auto_type _old = *(obj); *(obj) ^= (v); _old; })

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

typedef _Atomic(bool)           atomic_bool;
typedef _Atomic(char)           atomic_char;
typedef _Atomic(signed char)    atomic_schar;
typedef _Atomic(unsigned char)  atomic_uchar;
typedef _Atomic(short)          atomic_short;
typedef _Atomic(unsigned short) atomic_ushort;
typedef _Atomic(int)            atomic_int;
typedef _Atomic(unsigned int)   atomic_uint;
typedef _Atomic(long)           atomic_long;
typedef _Atomic(unsigned long)  atomic_ulong;
typedef _Atomic(long long)      atomic_llong;
typedef _Atomic(unsigned long long) atomic_ullong;
typedef _Atomic(void*)          atomic_voidptr;
typedef struct { atomic_bool _Value; } atomic_flag;

#define ATOMIC_FLAG_INIT { false }
#define ATOMIC_VAR_INIT(x)     (x)
#define atomic_init(obj,v)     __atomic_store_n(&(obj), (v), memory_order_relaxed)

#define atomic_load(obj)       __atomic_load_n(&(obj), memory_order_seq_cst)
#define atomic_store(obj,v)    __atomic_store_n(&(obj), (v), memory_order_seq_cst)
#define atomic_exchange(obj,v) __atomic_exchange_n(&(obj), (v), memory_order_seq_cst)

#define atomic_compare_exchange_strong(obj,exp,des) \
    __atomic_compare_exchange_n(&(obj),(exp),(des),false, memory_order_seq_cst, memory_order_seq_cst)
#define atomic_compare_exchange_weak(obj,exp,des) \
    __atomic_compare_exchange_n(&(obj),(exp),(des),true,  memory_order_seq_cst, memory_order_seq_cst)

#define atomic_fetch_add(obj,v) __atomic_fetch_add(&(obj),(v), memory_order_seq_cst)
#define atomic_fetch_sub(obj,v) __atomic_fetch_sub(&(obj),(v), memory_order_seq_cst)
#define atomic_fetch_and(obj,v) __atomic_fetch_and(&(obj),(v), memory_order_seq_cst)
#define atomic_fetch_or(obj,v)  __atomic_fetch_or(&(obj),(v), memory_order_seq_cst)
#define atomic_fetch_xor(obj,v) __atomic_fetch_xor(&(obj),(v), memory_order_seq_cst)

static inline bool atomic_flag_test_and_set(atomic_flag *f){
    return __atomic_test_and_set(&f->_Value, memory_order_seq_cst);
}
static inline void atomic_flag_clear(atomic_flag *f){
    __atomic_clear(&f->_Value, memory_order_seq_cst);
}

#define atomic_thread_fence(mo)   __atomic_thread_fence(mo)
#define atomic_signal_fence(mo)   __atomic_signal_fence(mo)

#endif /* __wasm__ */

#ifdef __cplusplus
}
#endif

#endif /* STDATOMIC_H */

