/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STDLIB_C
#define STDLIB_C

#include <stdint.h>
#include <stddef.h>
#include <stdatomic.h>
#include <string.h>
#include <limits.h>
#include <stdbit.h>

#ifdef __cplusplus
extern "C" {
#endif

/* — Configuration — */
#ifndef JACL_HEAP_SIZE
#define JACL_HEAP_SIZE (1u<<20)
#endif
#ifndef JACL_SMALL_MAX	
#define JACL_SMALL_MAX 128u
#endif
#ifndef JACL_TLS_CHUNK
#define JACL_TLS_CHUNK 4096u
#endif
#ifndef JACL_ALIGNMENT
#define JACL_ALIGNMENT 8u
#endif

/* — Cross-platform compatibility — */
#if defined(_WIN32) || defined(_WIN64)
	#define JACL_THREAD_LOCAL __declspec(thread)
	#define JACL_ALIGNAS(x) __declspec(align(x))
#elif defined(__GNUC__) || defined(__clang__)
	#define JACL_THREAD_LOCAL __thread
	#define JACL_ALIGNAS(x) __attribute__((aligned(x)))
#else
	#define JACL_THREAD_LOCAL _Thread_local
	#define JACL_ALIGNAS(x) _Alignas(x)
#endif

#define JACL_ALIGN_UP(x,a) (((x)+((a)-1u))&~((a)-1u))
#define JACL_HDR_ALLOC 1u
#define JACL_HDR_ARENA 2u

/* — Internal State (prefixed __jacl_) — */
typedef struct { uint32_t size, prev_size, flags; } __jacl_hdr_t; /* Added prev_size for O(1) coalesce */
typedef struct { _Atomic uint32_t next, owner; } __jacl_lock_t;

static JACL_ALIGNAS(16) uint8_t __jacl_heap[JACL_HEAP_SIZE];
static _Atomic uint32_t __jacl_once = 0;
static _Atomic uint32_t __jacl_tls_cursor = 0;
static __jacl_lock_t __jacl_lock = {0,0};
static uint32_t __jacl_bitmap = 0;
static uint32_t __jacl_bins[16];

JACL_THREAD_LOCAL uint32_t __jacl_tls_off = 0;
JACL_THREAD_LOCAL uint32_t __jacl_tls_end = 0;

/* — Internal Functions — */
static inline void __jacl_lock_acquire(__jacl_lock_t* l) {
		uint32_t ticket = atomic_fetch_add_explicit(&l->next, 1, memory_order_relaxed);
		while (atomic_load_explicit(&l->owner, memory_order_acquire) != ticket) {
				/* Yield hint for better spinning on some platforms */
				#if defined(_WIN32)
						YieldProcessor();
				#elif defined(__linux__) || defined(__ANDROID__)
						sched_yield();
				#elif defined(__APPLE__)
						pthread_yield_np();
				#endif
		}
}

static inline void __jacl_lock_release(__jacl_lock_t* l) {
		atomic_fetch_add_explicit(&l->owner, 1, memory_order_release);
}

static inline int __jacl_bin_idx(uint32_t sz) {
		int i = 0; 
		uint32_t q = sz >> 4;
		while (q > 1 && i < 15) { q >>= 1; i++; }
		return i;
}

static inline void __jacl_bin_push(uint32_t off) {
		int i = __jacl_bin_idx(((__jacl_hdr_t*)(__jacl_heap + off))->size);
		*(uint32_t*)(__jacl_heap + off + sizeof(__jacl_hdr_t)) = __jacl_bins[i];
		__jacl_bins[i] = off;
		__jacl_bitmap |= 1u << i;
}

static inline void __jacl_bin_remove(uint32_t off) {
		int i = __jacl_bin_idx(((__jacl_hdr_t*)(__jacl_heap + off))->size);
		uint32_t cur = __jacl_bins[i];
		if (cur == off) {
				__jacl_bins[i] = *(uint32_t*)(__jacl_heap + off + sizeof(__jacl_hdr_t));
		} else {
				while (cur) {
						uint32_t next = *(uint32_t*)(__jacl_heap + cur + sizeof(__jacl_hdr_t));
						if (next == off) {
								*(uint32_t*)(__jacl_heap + cur + sizeof(__jacl_hdr_t)) = *(uint32_t*)(__jacl_heap + off + sizeof(__jacl_hdr_t));
								break;
						}
						cur = next;
				}
		}
		if (!__jacl_bins[i]) __jacl_bitmap &= ~(1u << i);
}

static inline uint32_t __jacl_bin_pop(uint32_t need) {
		int i = __jacl_bin_idx(need);
		uint32_t mask = (~0u) << i;
		uint32_t m = __jacl_bitmap & mask;
		if (!m) return 0;
		
		i = __jacl_ctz32(m);
		uint32_t off = __jacl_bins[i];
		if (!off) { 
				__jacl_bitmap &= ~(1u << i); 
				return 0; 
		}
		
		__jacl_bins[i] = *(uint32_t*)(__jacl_heap + off + sizeof(__jacl_hdr_t));
		if (!__jacl_bins[i]) __jacl_bitmap &= ~(1u << i);
		return off;
}

static void __jacl_init_once(void) {
		uint32_t expect = 0;
		if (!atomic_compare_exchange_strong(&__jacl_once, &expect, 1)) return;
		
		memset(__jacl_bins, 0, sizeof(__jacl_bins));
		__jacl_bitmap = 0;
		atomic_store_explicit(&__jacl_tls_cursor, 0, memory_order_relaxed);
		
		/* Reserve 25% for TLS arenas, rest for global bins */
		uint32_t base = JACL_ALIGN_UP(JACL_HEAP_SIZE/4, JACL_ALIGNMENT);
		if (base + sizeof(__jacl_hdr_t) < JACL_HEAP_SIZE) {
				__jacl_hdr_t* h = (__jacl_hdr_t*)(__jacl_heap + base);
				h->size = JACL_HEAP_SIZE - base;
				h->prev_size = 0;
				h->flags = 0;
				__jacl_bin_push(base);
		}
}

static int __jacl_tls_refill(void) {
		uint32_t cur = atomic_fetch_add_explicit(&__jacl_tls_cursor, JACL_TLS_CHUNK, memory_order_relaxed);
		if (cur + JACL_TLS_CHUNK >= JACL_HEAP_SIZE/4) return 0;
		
		__jacl_tls_off = JACL_ALIGN_UP(cur, JACL_ALIGNMENT);
		__jacl_tls_end = cur + JACL_TLS_CHUNK;
		return 1;
}

/* — Public API — */
void* malloc(size_t n) {
		if (n == 0) n = 1;
		if (n > UINT32_MAX - sizeof(__jacl_hdr_t)) return NULL;
		
		__jacl_init_once();
		uint32_t need = JACL_ALIGN_UP(n + sizeof(__jacl_hdr_t), JACL_ALIGNMENT);
		
		/* Fast TLS path for small allocations */
		if (n <= JACL_SMALL_MAX) {
				if (__jacl_tls_off == 0 || __jacl_tls_off + need > __jacl_tls_end) {
						if (!__jacl_tls_refill()) goto large_path;
				}
				
				__jacl_hdr_t* h = (__jacl_hdr_t*)(__jacl_heap + __jacl_tls_off);
				h->size = need;
				h->prev_size = (__jacl_tls_off > 0) ? (( __jacl_hdr_t*)(__jacl_heap + __jacl_tls_off - h->size))->size : 0;
				h->flags = JACL_HDR_ALLOC | JACL_HDR_ARENA;
				__jacl_tls_off += need;
				return (void*)(h + 1);
		}
		
large_path:
		/* Large allocations: global bins with lock */
		__jacl_lock_acquire(&__jacl_lock);
		uint32_t off = __jacl_bin_pop(need);
		if (off) {
				__jacl_hdr_t* h = (__jacl_hdr_t*)(__jacl_heap + off);
				
				/* Split if remainder is large enough */
				if (h->size >= need + sizeof(__jacl_hdr_t) + 8) {
						uint32_t rem_off = off + need;
						__jacl_hdr_t* rem = (__jacl_hdr_t*)(__jacl_heap + rem_off);
						rem->size = h->size - need;
						rem->prev_size = need;
						rem->flags = 0;
						__jacl_bin_push(rem_off);
						h->size = need;
				}
				
				/* Update next's prev_size if exists */
				uint32_t next_off = off + h->size;
				if (next_off < JACL_HEAP_SIZE) {
						(__jacl_hdr_t*)(__jacl_heap + next_off)->prev_size = h->size;
				}
				
				h->flags = JACL_HDR_ALLOC;
				__jacl_lock_release(&__jacl_lock);
				return (void*)(h + 1);
		}
		__jacl_lock_release(&__jacl_lock);
		return NULL;
}

void free(void* p) {
		if (!p) return;
		
		__jacl_hdr_t* h = (__jacl_hdr_t*)p - 1;
		if (h < (__jacl_hdr_t*)__jacl_heap || h >= (__jacl_hdr_t*)(__jacl_heap + JACL_HEAP_SIZE)) return;
		if (!(h->flags & JACL_HDR_ALLOC)) return; /* Double free guard */
		
		/* Arena blocks leak by design (thread-safe) */
		if (h->flags & JACL_HDR_ARENA) {
				h->flags = 0;
				return;
		}
		
		h->flags = 0;
		__jacl_lock_acquire(&__jacl_lock);
		
		/* Lazy coalesce: check next */
		uint32_t off = (uint32_t)((uint8_t*)h - __jacl_heap);
		uint32_t next_off = off + h->size;
		if (next_off < JACL_HEAP_SIZE) {
				__jacl_hdr_t* next = (__jacl_hdr_t*)(__jacl_heap + next_off);
				if (!(next->flags & JACL_HDR_ALLOC)) {
						__jacl_bin_remove(next_off);
						h->size += next->size;
						/* Update following's prev_size */
						uint32_t after_off = off + h->size;
						if (after_off < JACL_HEAP_SIZE) {
								(__jacl_hdr_t*)(__jacl_heap + after_off)->prev_size = h->size;
						}
				}
		}
		
		/* Check prev */
		if (off > 0 && h->prev_size > 0) {
				uint32_t prev_off = off - h->prev_size;
				__jacl_hdr_t* prev = (__jacl_hdr_t*)(__jacl_heap + prev_off);
				if (!(prev->flags & JACL_HDR_ALLOC)) {
						__jacl_bin_remove(prev_off);
						prev->size += h->size;
						/* Update following's prev_size */
						uint32_t after_off = prev_off + prev->size;
						if (after_off < JACL_HEAP_SIZE) {
								(__jacl_hdr_t*)(__jacl_heap + after_off)->prev_size = prev->size;
						}
						off = prev_off; /* Push merged block */
				}
		}
		
		__jacl_bin_push(off);
		__jacl_lock_release(&__jacl_lock);
}

void* calloc(size_t nmemb, size_t size) {
		if (nmemb && size > SIZE_MAX / nmemb) return NULL; /* Overflow check */
		size_t total = nmemb * size;
		void* p = malloc(total);
		if (p) memset(p, 0, total);
		return p;
}

void* realloc(void* ptr, size_t size) {
		if (!ptr) return malloc(size);
		if (size == 0) { free(ptr); return NULL; }
		
		__jacl_hdr_t* h = (__jacl_hdr_t*)ptr - 1;
		size_t old_size = h->size - sizeof(__jacl_hdr_t);
		
		void* new_ptr = malloc(size);
		if (!new_ptr) return NULL;
		
		size_t copy_size = old_size < size ? old_size : size;
		memcpy(new_ptr, ptr, copy_size);
		
		/* Only free non-arena blocks */
		if (!(h->flags & JACL_HDR_ARENA)) free(ptr);
		return new_ptr;
}

void* aligned_alloc(size_t alignment, size_t size) {
		if (alignment == 0 || (alignment & (alignment-1)) != 0) return NULL; /* Power of 2 */
		if (size % alignment != 0) return NULL; /* Size multiple of alignment */
		if (alignment <= JACL_ALIGNMENT) return malloc(size); /* Native alignment OK */
		
		/* Over-align: alloc extra, pad, stash original ptr */
		size_t extra = alignment + sizeof(void*);
		void* raw = malloc(size + extra);
		if (!raw) return NULL;
		
		void* aligned = (void*)JACL_ALIGN_UP((uintptr_t)raw + sizeof(void*), alignment);
		*(void**)((uint8_t*)aligned - sizeof(void*)) = raw; /* Stash original */
		return aligned;
}

#ifdef __cplusplus
}
#endif

#endif /* STDLIB_C */

