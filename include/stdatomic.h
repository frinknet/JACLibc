/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STDATOMIC_H
#define STDATOMIC_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Memory‐order constants */
typedef enum {
    memory_order_relaxed = __ATOMIC_RELAXED,
    memory_order_consume = __ATOMIC_CONSUME,
    memory_order_acquire = __ATOMIC_ACQUIRE,
    memory_order_release = __ATOMIC_RELEASE,
    memory_order_acq_rel = __ATOMIC_ACQ_REL,
    memory_order_seq_cst = __ATOMIC_SEQ_CST
} memory_order;

/* Detect WASM with real atomics support */
#if defined(__wasm__) && defined(__WASM_ATOMICS__)
    #define WASM_REAL_ATOMICS
#elif defined(__wasm__)
    #define WASM_FAKE_ATOMICS
#endif

#ifdef WASM_FAKE_ATOMICS  /* WASM single-threaded mock implementation */

typedef bool            atomic_bool;
typedef char            atomic_char;
typedef signed char     atomic_schar;
typedef unsigned char   atomic_uchar;
typedef short           atomic_short;
typedef unsigned short  atomic_ushort;
typedef int             atomic_int;
typedef unsigned int    atomic_uint;
typedef long            atomic_long;
typedef unsigned long   atomic_ulong;
typedef long long       atomic_llong;
typedef unsigned long long atomic_ullong;
typedef void*           atomic_voidptr;
typedef struct { bool _Value; } atomic_flag;

#define ATOMIC_FLAG_INIT         { false }
#define ATOMIC_VAR_INIT(x)       (x)

/* Generic atomic operations - WASM stubs */
#define atomic_init(obj, val)           (*(obj) = (val))
#define atomic_load(obj)                (*(obj))
#define atomic_store(obj, val)          (*(obj) = (val))
#define atomic_exchange(obj, val)       ({ typeof(*(obj)) _old = *(obj); *(obj) = (val); _old; })
#define atomic_fetch_add(obj, val)      ({ typeof(*(obj)) _old = *(obj); *(obj) += (val); _old; })
#define atomic_fetch_sub(obj, val)      ({ typeof(*(obj)) _old = *(obj); *(obj) -= (val); _old; })
#define atomic_fetch_and(obj, val)      ({ typeof(*(obj)) _old = *(obj); *(obj) &= (val); _old; })
#define atomic_fetch_or(obj, val)       ({ typeof(*(obj)) _old = *(obj); *(obj) |= (val); _old; })
#define atomic_fetch_xor(obj, val)      ({ typeof(*(obj)) _old = *(obj); *(obj) ^= (val); _old; })

#define atomic_compare_exchange_weak(obj, expected, desired) \
    ({ bool _ret = (*(obj) == *(expected)); if (_ret) *(obj) = (desired); else *(expected) = *(obj); _ret; })

#define atomic_compare_exchange_strong(obj, expected, desired) \
    atomic_compare_exchange_weak(obj, expected, desired)

/* All explicit versions just ignore memory order in fake mode */
#define atomic_load_explicit(obj, mo)              atomic_load(obj)
#define atomic_store_explicit(obj, val, mo)        atomic_store(obj, val)
#define atomic_exchange_explicit(obj, val, mo)     atomic_exchange(obj, val)
#define atomic_fetch_add_explicit(obj, val, mo)    atomic_fetch_add(obj, val)
#define atomic_fetch_sub_explicit(obj, val, mo)    atomic_fetch_sub(obj, val)
#define atomic_fetch_and_explicit(obj, val, mo)    atomic_fetch_and(obj, val)
#define atomic_fetch_or_explicit(obj, val, mo)     atomic_fetch_or(obj, val)
#define atomic_fetch_xor_explicit(obj, val, mo)    atomic_fetch_xor(obj, val)

#define atomic_compare_exchange_weak_explicit(obj, expected, desired, succ, fail) \
    atomic_compare_exchange_weak(obj, expected, desired)
#define atomic_compare_exchange_strong_explicit(obj, expected, desired, succ, fail) \
    atomic_compare_exchange_strong(obj, expected, desired)

static inline bool atomic_flag_test_and_set(atomic_flag *f) {
    bool old = f->_Value; f->_Value = true; return old;
}
static inline bool atomic_flag_test_and_set_explicit(atomic_flag *f, memory_order mo) {
    return atomic_flag_test_and_set(f);
}
static inline void atomic_flag_clear(atomic_flag *f) {
    f->_Value = false;
}
static inline void atomic_flag_clear_explicit(atomic_flag *f, memory_order mo) {
    atomic_flag_clear(f);
}
static inline void atomic_thread_fence(memory_order mo) { (void)mo; }
static inline void atomic_signal_fence(memory_order mo) { (void)mo; }

#elif defined(WASM_REAL_ATOMICS)  /* WASM with real atomic instructions */

typedef _Atomic(bool)            atomic_bool;
typedef _Atomic(char)            atomic_char;
typedef _Atomic(signed char)     atomic_schar;
typedef _Atomic(unsigned char)   atomic_uchar;
typedef _Atomic(short)           atomic_short;
typedef _Atomic(unsigned short)  atomic_ushort;
typedef _Atomic(int)             atomic_int;
typedef _Atomic(unsigned int)    atomic_uint;
typedef _Atomic(long)            atomic_long;
typedef _Atomic(unsigned long)   atomic_ulong;
typedef _Atomic(long long)       atomic_llong;
typedef _Atomic(unsigned long long) atomic_ullong;
typedef _Atomic(void*)           atomic_voidptr;
typedef struct { atomic_bool _Value; } atomic_flag;

#define ATOMIC_FLAG_INIT         { false }
#define ATOMIC_VAR_INIT(x)       (x)

/* WASM atomic instructions - currently only sequential consistency */
/* Note: WASM atomics are always sequentially consistent by design */
static inline void atomic_init_int(atomic_int *obj, int val) {
    /* Use i32.atomic.store for initialization */
    __asm__ volatile ("i32.atomic.store %0, %1" : "=m"(*obj) : "r"(val) : "memory");
}

static inline int atomic_load_int(const atomic_int *obj) {
    int result;
    __asm__ volatile ("i32.atomic.load %0, %1" : "=r"(result) : "m"(*obj) : "memory");
    return result;
}

static inline void atomic_store_int(atomic_int *obj, int val) {
    __asm__ volatile ("i32.atomic.store %0, %1" : "=m"(*obj) : "r"(val) : "memory");
}

static inline int atomic_exchange_int(atomic_int *obj, int val) {
    int result;
    __asm__ volatile ("i32.atomic.rmw.xchg %0, %1, %2" : "=r"(result), "=m"(*obj) : "r"(val) : "memory");
    return result;
}

static inline int atomic_fetch_add_int(atomic_int *obj, int val) {
    int result;
    __asm__ volatile ("i32.atomic.rmw.add %0, %1, %2" : "=r"(result), "=m"(*obj) : "r"(val) : "memory");
    return result;
}

static inline int atomic_fetch_sub_int(atomic_int *obj, int val) {
    int result;
    __asm__ volatile ("i32.atomic.rmw.sub %0, %1, %2" : "=r"(result), "=m"(*obj) : "r"(val) : "memory");
    return result;
}

static inline int atomic_fetch_and_int(atomic_int *obj, int val) {
    int result;
    __asm__ volatile ("i32.atomic.rmw.and %0, %1, %2" : "=r"(result), "=m"(*obj) : "r"(val) : "memory");
    return result;
}

static inline int atomic_fetch_or_int(atomic_int *obj, int val) {
    int result;
    __asm__ volatile ("i32.atomic.rmw.or %0, %1, %2" : "=r"(result), "=m"(*obj) : "r"(val) : "memory");
    return result;
}

static inline int atomic_fetch_xor_int(atomic_int *obj, int val) {
    int result;
    __asm__ volatile ("i32.atomic.rmw.xor %0, %1, %2" : "=r"(result), "=m"(*obj) : "r"(val) : "memory");
    return result;
}

static inline bool atomic_compare_exchange_weak_int(atomic_int *obj, int *expected, int desired) {
    int old_val = *expected;
    int result;
    __asm__ volatile ("i32.atomic.rmw.cmpxchg %0, %1, %2, %3" 
                      : "=r"(result), "=m"(*obj) 
                      : "r"(old_val), "r"(desired) 
                      : "memory");
    if (result == old_val) {
        return true;
    } else {
        *expected = result;
        return false;
    }
}

static inline bool atomic_compare_exchange_strong_int(atomic_int *obj, int *expected, int desired) {
    return atomic_compare_exchange_weak_int(obj, expected, desired);
}

/* Generic macros that dispatch to type-specific implementations */
#define atomic_init(obj, val) \
    _Generic((obj), \
        atomic_int*: atomic_init_int, \
        default: atomic_init_int \
    )(obj, val)

#define atomic_load(obj) \
    _Generic((obj), \
        const atomic_int*: atomic_load_int, \
        atomic_int*: atomic_load_int, \
        default: atomic_load_int \
    )(obj)

#define atomic_store(obj, val) \
    _Generic((obj), \
        atomic_int*: atomic_store_int, \
        default: atomic_store_int \
    )(obj, val)

#define atomic_exchange(obj, val) \
    _Generic((obj), \
        atomic_int*: atomic_exchange_int, \
        default: atomic_exchange_int \
    )(obj, val)

#define atomic_fetch_add(obj, val) \
    _Generic((obj), \
        atomic_int*: atomic_fetch_add_int, \
        default: atomic_fetch_add_int \
    )(obj, val)

#define atomic_fetch_sub(obj, val) \
    _Generic((obj), \
        atomic_int*: atomic_fetch_sub_int, \
        default: atomic_fetch_sub_int \
    )(obj, val)

#define atomic_fetch_and(obj, val) \
    _Generic((obj), \
        atomic_int*: atomic_fetch_and_int, \
        default: atomic_fetch_and_int \
    )(obj, val)

#define atomic_fetch_or(obj, val) \
    _Generic((obj), \
        atomic_int*: atomic_fetch_or_int, \
        default: atomic_fetch_or_int \
    )(obj, val)

#define atomic_fetch_xor(obj, val) \
    _Generic((obj), \
        atomic_int*: atomic_fetch_xor_int, \
        default: atomic_fetch_xor_int \
    )(obj, val)

#define atomic_compare_exchange_weak(obj, expected, desired) \
    _Generic((obj), \
        atomic_int*: atomic_compare_exchange_weak_int, \
        default: atomic_compare_exchange_weak_int \
    )(obj, expected, desired)

#define atomic_compare_exchange_strong(obj, expected, desired) \
    _Generic((obj), \
        atomic_int*: atomic_compare_exchange_strong_int, \
        default: atomic_compare_exchange_strong_int \
    )(obj, expected, desired)

/* Explicit memory order versions - WASM ignores ordering (always seq_cst) */
#define atomic_load_explicit(obj, mo)              atomic_load(obj)
#define atomic_store_explicit(obj, val, mo)        atomic_store(obj, val)
#define atomic_exchange_explicit(obj, val, mo)     atomic_exchange(obj, val)
#define atomic_fetch_add_explicit(obj, val, mo)    atomic_fetch_add(obj, val)
#define atomic_fetch_sub_explicit(obj, val, mo)    atomic_fetch_sub(obj, val)
#define atomic_fetch_and_explicit(obj, val, mo)    atomic_fetch_and(obj, val)
#define atomic_fetch_or_explicit(obj, val, mo)     atomic_fetch_or(obj, val)
#define atomic_fetch_xor_explicit(obj, val, mo)    atomic_fetch_xor(obj, val)

#define atomic_compare_exchange_weak_explicit(obj, expected, desired, succ, fail) \
    atomic_compare_exchange_weak(obj, expected, desired)
#define atomic_compare_exchange_strong_explicit(obj, expected, desired, succ, fail) \
    atomic_compare_exchange_strong(obj, expected, desired)

static inline bool atomic_flag_test_and_set(atomic_flag *f){
    return atomic_exchange(&f->_Value, true);
}
static inline bool atomic_flag_test_and_set_explicit(atomic_flag *f, memory_order mo){
    return atomic_flag_test_and_set(f);
}
static inline void atomic_flag_clear(atomic_flag *f){
    atomic_store(&f->_Value, false);
}
static inline void atomic_flag_clear_explicit(atomic_flag *f, memory_order mo){
    atomic_flag_clear(f);
}

/* WASM atomic fence - uses memory.atomic.fence */
static inline void atomic_thread_fence(memory_order mo) {
    (void)mo; /* WASM only has seq_cst fences */
    __asm__ volatile ("memory.atomic.fence" ::: "memory");
}
static inline void atomic_signal_fence(memory_order mo) {
    __asm__ volatile ("" ::: "memory"); /* Compiler barrier only */
}

#else /* Non-WASM: use native atomics */

#if !defined(__has_builtin)
# define __has_builtin(x) 0
#endif
#if !(__has_builtin(__atomic_load_n) && __has_builtin(__atomic_store_n))
# error "Compiler must support __atomic builtins"
#endif

typedef _Atomic(bool)            atomic_bool;
typedef _Atomic(char)            atomic_char;
typedef _Atomic(signed char)     atomic_schar;
typedef _Atomic(unsigned char)   atomic_uchar;
typedef _Atomic(short)           atomic_short;
typedef _Atomic(unsigned short)  atomic_ushort;
typedef _Atomic(int)             atomic_int;
typedef _Atomic(unsigned int)    atomic_uint;
typedef _Atomic(long)            atomic_long;
typedef _Atomic(unsigned long)   atomic_ulong;
typedef _Atomic(long long)       atomic_llong;
typedef _Atomic(unsigned long long) atomic_ullong;
typedef _Atomic(void*)           atomic_voidptr;
typedef struct { atomic_bool _Value; } atomic_flag;

#define ATOMIC_FLAG_INIT         { false }
#define ATOMIC_VAR_INIT(x)       (x)

/* Generic atomic operations using compiler builtins */
#define atomic_init(obj, val)       __atomic_store_n(obj, val, memory_order_relaxed)
#define atomic_load(obj)            __atomic_load_n(obj, memory_order_seq_cst)
#define atomic_store(obj, val)      __atomic_store_n(obj, val, memory_order_seq_cst)
#define atomic_exchange(obj, val)   __atomic_exchange_n(obj, val, memory_order_seq_cst)
#define atomic_fetch_add(obj, val)  __atomic_fetch_add(obj, val, memory_order_seq_cst)
#define atomic_fetch_sub(obj, val)  __atomic_fetch_sub(obj, val, memory_order_seq_cst)
#define atomic_fetch_and(obj, val)  __atomic_fetch_and(obj, val, memory_order_seq_cst)
#define atomic_fetch_or(obj, val)   __atomic_fetch_or(obj, val, memory_order_seq_cst)
#define atomic_fetch_xor(obj, val)  __atomic_fetch_xor(obj, val, memory_order_seq_cst)

#define atomic_compare_exchange_weak(obj, expected, desired) \
    __atomic_compare_exchange_n(obj, expected, desired, true, memory_order_seq_cst, memory_order_seq_cst)

#define atomic_compare_exchange_strong(obj, expected, desired) \
    __atomic_compare_exchange_n(obj, expected, desired, false, memory_order_seq_cst, memory_order_seq_cst)

/* Explicit memory order versions */
#define atomic_load_explicit(obj, mo)              __atomic_load_n(obj, mo)
#define atomic_store_explicit(obj, val, mo)        __atomic_store_n(obj, val, mo)
#define atomic_exchange_explicit(obj, val, mo)     __atomic_exchange_n(obj, val, mo)
#define atomic_fetch_add_explicit(obj, val, mo)    __atomic_fetch_add(obj, val, mo)
#define atomic_fetch_sub_explicit(obj, val, mo)    __atomic_fetch_sub(obj, val, mo)
#define atomic_fetch_and_explicit(obj, val, mo)    __atomic_fetch_and(obj, val, mo)
#define atomic_fetch_or_explicit(obj, val, mo)     __atomic_fetch_or(obj, val, mo)
#define atomic_fetch_xor_explicit(obj, val, mo)    __atomic_fetch_xor(obj, val, mo)

#define atomic_compare_exchange_weak_explicit(obj, expected, desired, succ, fail) \
    __atomic_compare_exchange_n(obj, expected, desired, true, succ, fail)

#define atomic_compare_exchange_strong_explicit(obj, expected, desired, succ, fail) \
    __atomic_compare_exchange_n(obj, expected, desired, false, succ, fail)

static inline bool atomic_flag_test_and_set(atomic_flag *f){
    return __atomic_test_and_set(&f->_Value, memory_order_seq_cst);
}
static inline bool atomic_flag_test_and_set_explicit(atomic_flag *f, memory_order mo){
    return __atomic_test_and_set(&f->_Value, mo);
}
static inline void atomic_flag_clear(atomic_flag *f){
    __atomic_clear(&f->_Value, memory_order_seq_cst);
}
static inline void atomic_flag_clear_explicit(atomic_flag *f, memory_order mo){
    __atomic_clear(&f->_Value, mo);
}

#define atomic_thread_fence(mo)      __atomic_thread_fence(mo)
#define atomic_signal_fence(mo)      __atomic_signal_fence(mo)

#endif /* Platform selection */

/* Lock-free macros (always claim lock-free for simplicity) */
#define ATOMIC_BOOL_LOCK_FREE       2
#define ATOMIC_CHAR_LOCK_FREE       2
#define ATOMIC_CHAR16_T_LOCK_FREE   2
#define ATOMIC_CHAR32_T_LOCK_FREE   2
#define ATOMIC_WCHAR_T_LOCK_FREE    2
#define ATOMIC_SHORT_LOCK_FREE      2
#define ATOMIC_INT_LOCK_FREE        2
#define ATOMIC_LONG_LOCK_FREE       2
#define ATOMIC_LLONG_LOCK_FREE      2
#define ATOMIC_POINTER_LOCK_FREE    2

#ifdef __cplusplus
}
#endif

#endif /* STDATOMIC_H */
