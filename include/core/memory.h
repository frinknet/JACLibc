/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef CORE_MEMORY_H
#define CORE_MEMORY_H

#include <config.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#if JACL_OS_WINDOWS
  #include <windows.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Heap Growth Configuration */
#ifndef JACL_HEAP_INIT
#define JACL_HEAP_INIT (256u<<10)
#endif

#ifndef JACL_HEAP_MAX
#define JACL_HEAP_MAX (1u<<30)
#endif

#ifndef JACL_HEAP_SEG
#define JACL_HEAP_SEG (1u<<20)
#endif

/* Configuration */
#ifndef JACL_SMALL_MAX
#define JACL_SMALL_MAX 128u
#endif

#ifndef JACL_TLS_CHUNK
#define JACL_TLS_CHUNK 4096u
#endif

#ifndef JACL_HDR_ALLOC
#define JACL_HDR_ALLOC 1u
#endif

#ifndef JACL_HDR_ARENA
#define JACL_HDR_ARENA 2u
#endif

#ifndef JACL_ALIGNMENT
#define JACL_ALIGNMENT 8u
#endif

#ifndef JACL_CONCURENCY
#define JACL_CONCURENCY 32
#endif

#ifndef JACL_RECYCLING
#define JACL_RECYCLING 12
#endif

#ifndef JACL_COALESCING
#define JACL_COALESCING 1024u
#endif

#if JACL_CONCURENCY > 32
#undef JACL_CONCURENCY
#define JACL_CONCURENCY 32
#endif

#if JACL_HAS_C11
	#include <stdatomic.h>
#else
	#define memory_order_relaxed 0
	#define memory_order_acquire 0
	#define memory_order_release 0
	#define memory_order_seq_cst 0

	#define atomic_load(ptr) (*(ptr))
	#define atomic_load_explicit(ptr, order) (*(ptr))
	#define atomic_store(ptr, val) (*(ptr) = (val))
	#define atomic_store_explicit(ptr, val, order) (*(ptr) = (val))
	#define atomic_fetch_add(ptr, val) ((*(ptr))++)
	#define atomic_fetch_add_explicit(ptr, val, order) ((*(ptr)) += (val), (*(ptr)) - (val))
	#define atomic_fetch_and(ptr, val) ((*(ptr)) &= (val))
	#define atomic_fetch_and_explicit(ptr, val, order) ((*(ptr)) &= (val))
	#define atomic_fetch_or(ptr, val) ((*(ptr)) |= (val))
	#define atomic_fetch_or_explicit(ptr, val, order) ((*(ptr)) |= (val))
	#define atomic_compare_exchange_strong(ptr, expected, desired) \
		((*(ptr) == *(expected)) ? (*(ptr) = (desired), 1) : (*(expected) = *(ptr), 0))

	#warning "WARNING: Before C11 your memory is single-threaded mode only"
#endif

/* Internal State */
typedef struct {
	size_t size, prev_size, flags;
} __jacl_memhdr_t;

typedef struct __jacl_memseg {
	uint8_t *base;
	size_t size, bins[JACL_CONCURENCY];
	uint_least32_t bitmap;
	struct __jacl_memseg *next;
} __jacl_memseg_t;

typedef struct {
	_Atomic uint_fast32_t next, owner;
} __jacl_memlock_t;

// Initial static heap
static _Alignas(16) uint8_t __jacl_static_heap[JACL_HEAP_INIT];

// Segment tracking
static __jacl_memseg_t __jacl_memseg_init;
static __jacl_memseg_t* __jacl_memseg_head = NULL;
static size_t __jacl_heap_current = JACL_HEAP_INIT;
static size_t __jacl_heap_max = JACL_HEAP_MAX;
static _Atomic uint32_t __jacl_once = 0;
static __jacl_memlock_t __jacl_locks[JACL_CONCURENCY];
static __jacl_memlock_t __jacl_growth;

// TLS state
_Atomic uint32_t __jacl_tls_cursor = 0;
thread_local uint32_t __jacl_tls_off = 0;
thread_local uint32_t __jacl_tls_end = 0;

/* Recycling */
thread_local _Alignas(64) struct {
	void* slots[JACL_RECYCLING];
	int count;
} __jacl_recycling[4];

/* Internal Functions */
void __jacl_memlock_acquire(__jacl_memlock_t* l) {
	uint32_t ticket = atomic_fetch_add_explicit(&l->next, 1, memory_order_relaxed);

	while (atomic_load_explicit(&l->owner, memory_order_acquire) != ticket) {
		#if JACL_OS_WINDOWS
			YieldProcessor();
		#elif JACL_OS_DARWIN
			pthread_yield_np();
		#else
			sched_yield();
		#endif
	}
}

void __jacl_memlock_release(__jacl_memlock_t* l) {
	atomic_fetch_add_explicit(&l->owner, 1, memory_order_release);
}

void __jacl_memlock_blank(__jacl_memlock_t* l) {
	l->next = 0;
	l->owner = 0;
}

void __jacl_memlock_block(void) {
	for (int i = JACL_CONCURENCY; i--;) __jacl_memlock_acquire(&__jacl_locks[i]);

	__jacl_memlock_acquire(&__jacl_growth);
}

void __jacl_memlock_clear(void) {
	for (int i = JACL_CONCURENCY; i--;) __jacl_memlock_release(&__jacl_locks[i]);

	__jacl_memlock_release(&__jacl_growth);
}

void __jacl_memlock_reset(void) {
	__jacl_tls_off = JACL_HEAP_INIT / 4;
	__jacl_tls_end = JACL_HEAP_INIT / 4;  // Force refill or skip TLS
	__jacl_memseg_head = &__jacl_memseg_init;
	__jacl_once = 1;

	__jacl_memlock_clear();
}

#define __jacl_membin_safety(seg, off, where) do { \
	if ((off) >= (seg)->size || (off) + sizeof(__jacl_memhdr_t) > (seg)->size) { \
		return 0; \
	} \
} while (0)

static inline int __jacl_membin_get(size_t sz) {
	int i;

	if (sz <= 128) i = (sz + 7) >> 3;
	else {
		i = 16;

		size_t q = sz >> 7;

		while (q > 1 && i < 31) { q >>= 1; i++; }
	}

	/* Clamp to JACL_CONCURENCY - 1 */
	return (i >= JACL_CONCURENCY) ? JACL_CONCURENCY - 1 : i;
}

static inline size_t __jacl_membin_pop(__jacl_memseg_t* seg, size_t need) {
	int i = __jacl_membin_get(need);
	uint32_t mask = (~0u) << i;
	uint32_t m = seg->bitmap & mask;

	if (!m) return 0;

	i = __jacl_ctz32(m);

	size_t off = seg->bins[i];

	if (!off) {
		atomic_fetch_and_explicit((_Atomic uint_least32_t*)&seg->bitmap, ~(1u << i), memory_order_relaxed);

		return 0;
	}

	__jacl_membin_safety(seg, off, "pop");

	uint8_t *ptr = seg->base + off + sizeof(__jacl_memhdr_t);

	if (ptr < seg->base + sizeof(__jacl_memhdr_t) || ptr > seg->base + seg->size - sizeof(size_t)) {
			seg->bins[i] = 0;

			atomic_fetch_and_explicit((_Atomic uint_least32_t*)&seg->bitmap, ~(1u << i), memory_order_relaxed);

			return 0;
		}

	seg->bins[i] = *(size_t*)ptr;

	if (!seg->bins[i]) {
		atomic_fetch_and_explicit((_Atomic uint_least32_t*)&seg->bitmap, ~(1u << i), memory_order_relaxed);
	}

	return off;
}

static inline void __jacl_membin_push(__jacl_memseg_t* seg, size_t off) {
	int i = __jacl_membin_get(((__jacl_memhdr_t*)(seg->base + off))->size);
	__jacl_membin_safety(seg, off, "push");

	*(size_t*)(seg->base + off + sizeof(__jacl_memhdr_t)) = seg->bins[i];
	seg->bins[i] = off;

	atomic_fetch_or_explicit((_Atomic uint_least32_t*)&seg->bitmap, 1u << i, memory_order_relaxed);
}

static inline void __jacl_membin_remove(__jacl_memseg_t* seg, size_t off) {
	int i = __jacl_membin_get(((__jacl_memhdr_t*)(seg->base + off))->size);
	size_t cur = seg->bins[i];

	__jacl_membin_safety(seg, off, "remove");

	if (cur == off) {
		seg->bins[i] = *(size_t*)(seg->base + off + sizeof(__jacl_memhdr_t));
	} else {
		while (cur) {
			size_t next = *(size_t*)(seg->base + cur + sizeof(__jacl_memhdr_t));

			if (next) __builtin_prefetch(seg->base + next, 0, 1);

			if (next == off) {
				*(size_t*)(seg->base + cur + sizeof(__jacl_memhdr_t)) = *(size_t*)(seg->base + off + sizeof(__jacl_memhdr_t));
				break;
			}
			cur = next;
		}
	}

	if (!seg->bins[i]) {
		atomic_fetch_and_explicit((_Atomic uint_least32_t*)&seg->bitmap, ~(1u << i), memory_order_relaxed);
	}
}

static inline void __jacl_membin_init(void) {
	static int canary = 0;
	canary++;

	if (atomic_load_explicit(&__jacl_once, memory_order_acquire)) return;

	for (int i = JACL_CONCURENCY; i--;) __jacl_memlock_blank(&__jacl_locks[i]);

	uint32_t expect = 0;

	if (!atomic_compare_exchange_strong(&__jacl_once, &expect, 1)) return;

	/* Init Recycling */
	for (int i = 0; i < 4; i++) {
		__jacl_recycling[i].count = 0;

		for (int j = 0; j < JACL_RECYCLING; j++) __jacl_recycling[i].slots[j] = NULL;
	}

	__jacl_memseg_init.base = __jacl_static_heap;
	__jacl_memseg_init.size = JACL_HEAP_INIT;
	__jacl_memseg_init.next = NULL;

	memset(__jacl_memseg_init.bins, 0, sizeof(__jacl_memseg_init.bins));

	__jacl_memseg_init.bitmap = 0;
	__jacl_memseg_head = &__jacl_memseg_init;

	atomic_store_explicit(&__jacl_tls_cursor, 0, memory_order_relaxed);

	size_t base = JACL_ALIGN_UP(JACL_HEAP_INIT/4, JACL_ALIGNMENT);

	if (base + sizeof(__jacl_memhdr_t) < JACL_HEAP_INIT) {
		__jacl_memhdr_t* h = (__jacl_memhdr_t*)(__jacl_static_heap + base);

		h->size = JACL_HEAP_INIT - base;
		h->prev_size = 0;
		h->flags = 0;

		__jacl_membin_push(&__jacl_memseg_init, base);
	}
}

static inline __jacl_memseg_t* __jacl_memseg_find(void* ptr) {
	if (!ptr) return NULL;

	__jacl_memseg_t* seg = __jacl_memseg_head;

	while (seg) {
		if (ptr >= (void*)seg->base && ptr < (void*)(seg->base + seg->size)) return seg;

		seg = seg->next;
	}

	return NULL;
}

static inline int __jacl_tls_refill(void) {
	uint32_t cur = atomic_fetch_add_explicit(&__jacl_tls_cursor, JACL_TLS_CHUNK, memory_order_relaxed);
	uint32_t limit = JACL_HEAP_INIT / 4;

	if (cur >= limit) return 0;

	uint32_t available = (cur + JACL_TLS_CHUNK > limit) ? (limit - cur) : JACL_TLS_CHUNK;

	if (available < 64) return 0;

	__jacl_tls_off = JACL_ALIGN_UP(cur, JACL_ALIGNMENT);
	__jacl_tls_end = cur + available;

	return 1;
}

static inline size_t __jacl_round_size(size_t sz) {
	if (sz <= 128) return (sz + 7) & ~7u;
	if (sz <= 512) return (sz + 15) & ~15u;

	return JACL_ALIGN_UP(sz, 32);
}

static inline void __jacl_segment_init(__jacl_memseg_t* seg) {
	size_t off = sizeof(__jacl_memhdr_t);

	__jacl_memhdr_t* h = (__jacl_memhdr_t*)(seg->base + off);
	h->size = seg->size - off;
	h->prev_size = 0;
	h->flags = 0;

	__jacl_membin_push(seg, off);
}

static inline void __jacl_segment_link(__jacl_memseg_t* seg) {
	__jacl_memlock_acquire(&__jacl_growth);

	__jacl_memseg_t* tail = __jacl_memseg_head;

	while (tail->next) tail = tail->next;

	tail->next = seg;

	__jacl_memlock_release(&__jacl_growth);
}

static inline __jacl_memseg_t* __jacl_segment(__jacl_memseg_t* seg, size_t seg_size) {
	if (!seg) return NULL;

	seg->base = (uint8_t*)seg + sizeof(__jacl_memseg_t);
	seg->size = seg_size - sizeof(__jacl_memseg_t);
	seg->next = NULL;
	seg->bitmap = 0;

	memset(seg->bins, 0, sizeof(seg->bins));
	__jacl_segment_init(seg);
	__jacl_segment_link(seg);

	__jacl_heap_current += seg_size;

	return seg;
}

static inline int __jacl_grow_check(void) {
	return (__jacl_heap_max != JACL_HEAP_INIT && __jacl_heap_current < __jacl_heap_max);
}

static inline size_t __jacl_grow_size(size_t need) {
	size_t seg_size = (need > JACL_HEAP_SEG)
		? JACL_ALIGN_UP(need, 4096)
		: JACL_HEAP_SEG;

	if (__jacl_heap_current + seg_size > __jacl_heap_max) {
		seg_size = __jacl_heap_max - __jacl_heap_current;

		if (seg_size < need) return 0;
	}

	return seg_size;
}

static inline void* __jacl_grow_acquire(size_t seg_size) {
	#if JACL_OS_WINDOWS
		return VirtualAlloc(NULL, seg_size, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	#elif JACL_ARCH_WASM
		size_t pages = (seg_size + 65535) / 65536;
		int old = __builtin_wasm_memory_grow(0, pages);

		return (old < 0) ? NULL : (void*)(old * 65536);
	#elif JACL_HAS_POSIX
		void* p = mmap(NULL, seg_size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

		return (p == MAP_FAILED) ? NULL : p;
	#else
		return NULL;
	#endif
}

static inline __jacl_memseg_t* __jacl_grow_heap(size_t need) {
	if (!__jacl_grow_check()) return NULL;

	size_t seg_size = __jacl_grow_size(need);

	if (!seg_size) return NULL;

	void* mem = __jacl_grow_acquire(seg_size);

	return __jacl_segment((__jacl_memseg_t*)mem, seg_size);
}

static inline size_t __jacl_alloc_need(size_t n) {
	if (n == 0) n = 1;
	if (JACL_UNLIKELY(n > SIZE_MAX - sizeof(__jacl_memhdr_t))) {
		errno = ENOMEM;

		return 0;
	}

	return __jacl_round_size(n + sizeof(__jacl_memhdr_t));
}

static inline void* __jacl_alloc_recycle(size_t n) {
	if (n < 16 || n > 48) return NULL;

	int rbin = (n >> 3) - 2;

	if (rbin >= 0 && rbin < 4 && __jacl_recycling[rbin].count > 0) {
		void* p = __jacl_recycling[rbin].slots[--__jacl_recycling[rbin].count];

		((__jacl_memhdr_t*)p - 1)->flags = JACL_HDR_ALLOC;

		return p;
	}

	return NULL;
}

static inline void* __jacl_alloc_arena(size_t n, size_t need) {
	if (JACL_UNLIKELY(n > JACL_SMALL_MAX)) return NULL;

	int can_alloc = 0;

	if (__jacl_tls_off + need <= __jacl_tls_end || __jacl_tls_refill()) can_alloc = 1;
	if (__jacl_tls_off + need > (JACL_HEAP_INIT / 4)) can_alloc = 0;
	if (!can_alloc) return NULL;

	__jacl_memhdr_t* h = (__jacl_memhdr_t*)(__jacl_static_heap + __jacl_tls_off);
	h->size = need;
	h->prev_size = 0;
	h->flags = JACL_HDR_ALLOC | JACL_HDR_ARENA;
	__jacl_tls_off += need;

	return (void*)(h + 1);
}

static inline void* __jacl_alloc_bins(__jacl_memseg_t* seg, size_t need) {
	while (seg) {
		size_t off = __jacl_membin_pop(seg, need);

		if (!off) { seg = seg->next; continue; }

		__jacl_memhdr_t* h = (__jacl_memhdr_t*)(seg->base + off);

		if (h->size < need) {
			__jacl_membin_push(seg, off);

			seg = seg->next;

			continue;
		}

		if (h->size >= need + sizeof(__jacl_memhdr_t) + 8) {
			size_t rem_off = off + need;
			__jacl_memhdr_t* rem = (__jacl_memhdr_t*)(seg->base + rem_off);

			rem->size = h->size - need;
			rem->prev_size = need;
			rem->flags = 0;
			h->size = need;

			__jacl_membin_push(seg, rem_off);
		}

		size_t next_off = off + h->size;

		if (next_off < seg->size) ((__jacl_memhdr_t*)(seg->base + next_off))->prev_size = h->size;

		h->flags = JACL_HDR_ALLOC;

		return (void*)(h + 1);
	}

	return NULL;
}

static inline int __jacl_free_arena(void* p) {
	__jacl_memhdr_t* h = (__jacl_memhdr_t*)p - 1;

	if (h->flags & JACL_HDR_ARENA) {
		if (h->flags & JACL_HDR_ALLOC) h->flags = 0;

		return 1;
	}

	return 0;
}

static inline void* __jacl_free_aligned(void* p, __jacl_memhdr_t** h, __jacl_memseg_t** seg) {
	void** stored = (void**)((uint8_t*)p - sizeof(void*));
	void* orig = *stored;

	if (!orig || ((uintptr_t)orig & (JACL_ALIGNMENT - 1))) return NULL;

	*h = (__jacl_memhdr_t*)orig - 1;
	*seg = __jacl_memseg_find((void*)*h);

	if (!*seg) return NULL;

	return (void*)(*h + 1);
}

static inline int __jacl_free_recycle(void* p, size_t size) {
	if (size < 16 || size > 48) return 0;

	int rbin = (size >> 3) - 2;

	if (rbin >= 0 && rbin < 4 && __jacl_recycling[rbin].count < JACL_RECYCLING) {
		((__jacl_memhdr_t*)p - 1)->flags = 0;
		__jacl_recycling[rbin].slots[__jacl_recycling[rbin].count++] = p;

		return 1;
	}

	return 0;
}

static inline void __jacl_free_coalesce(__jacl_memseg_t* seg, size_t off) {
	__jacl_memhdr_t* h = (__jacl_memhdr_t*)(seg->base + off);

	if (h->size >= JACL_COALESCING) {
		size_t next_off = off + h->size;

		if (next_off < seg->size) {
			__jacl_memhdr_t* next = (__jacl_memhdr_t*)(seg->base + next_off);

			if (!(next->flags & JACL_HDR_ALLOC)) {
				__jacl_membin_remove(seg, next_off);

				h->size += next->size;

				uint32_t after_off = off + h->size;

				if (after_off < seg->size) ((__jacl_memhdr_t*)(seg->base + after_off))->prev_size = h->size;
			}
		}

		if (off > 0 && h->prev_size > 0 && h->prev_size <= off) {
			size_t prev_off = off - h->prev_size;

			__jacl_memhdr_t* prev = (__jacl_memhdr_t*)(seg->base + prev_off);

			if ((uint8_t*)prev >= seg->base
				&& (uint8_t*)prev + sizeof(__jacl_memhdr_t) <= seg->base + seg->size
				&& !(prev->flags & JACL_HDR_ALLOC)
			) {
				__jacl_membin_remove(seg, prev_off);

				prev->size += h->size;

				size_t after_off = prev_off + prev->size;

				if (after_off < seg->size) ((__jacl_memhdr_t*)(seg->base + after_off))->prev_size = prev->size;

				off = prev_off;
			}
		}
	}

	__jacl_membin_push(seg, off);
}

/* Public API */
void* malloc(size_t n) {
	size_t need = __jacl_alloc_need(n);

	if (JACL_UNLIKELY(!need)) return NULL;

	__jacl_membin_init();

	void* p;

	if ((p = __jacl_alloc_recycle(n))) return p;
	if ((p = __jacl_alloc_arena(n, need))) return p;

	assert(__jacl_tls_off <= JACL_HEAP_INIT / 4);
	assert(__jacl_tls_end <= JACL_HEAP_INIT / 4);
	assert(__jacl_tls_off <= __jacl_tls_end);

	int bin = __jacl_membin_get(need);
	__jacl_memseg_t* seg = __jacl_memseg_head;

	__jacl_memlock_acquire(&__jacl_locks[bin]);

	p = __jacl_alloc_bins(__jacl_memseg_head, need);

	if (!p) {
		seg = __jacl_grow_heap(need);
		p = __jacl_alloc_bins(seg, need);
	}

	__jacl_memlock_release(&__jacl_locks[bin]);

	if (p) return p;

	errno = ENOMEM;

	return NULL;
}

void free(void* p) {
	if (JACL_UNLIKELY(!p)) return;

	if (__jacl_free_arena(p)) return;

	__jacl_memhdr_t* h = (__jacl_memhdr_t*)p - 1;
	__jacl_memseg_t* seg = __jacl_memseg_find((void*)h);

	if (!seg) {
		void* resolved = __jacl_free_aligned(p, &h, &seg);

		if (!resolved) return;

		p = resolved;
	}

	if (JACL_UNLIKELY(!(h->flags & JACL_HDR_ALLOC))) return;

	size_t size = h->size - sizeof(__jacl_memhdr_t);

	if (__jacl_free_recycle(p, size)) return;

	h->flags = 0;

	size_t off = (size_t)((uint8_t*)h - seg->base);
	int bin = __jacl_membin_get(h->size);

	__jacl_memlock_acquire(&__jacl_locks[bin]);
	__jacl_free_coalesce(seg, off);
	__jacl_memlock_release(&__jacl_locks[bin]);
}

void* calloc(size_t nmemb, size_t size) {
	if (nmemb && size > SIZE_MAX / nmemb) {
		errno = ENOMEM;

		return NULL;
	}

	size_t total = nmemb * size;
	void* p = malloc(total);

	if (p) memset(p, 0, total);

	return p;
}

void* realloc(void* ptr, size_t size) {
	if (!ptr) return malloc(size);
	if (size == 0) {
		free(ptr);

		return NULL;
	}

	__jacl_memhdr_t* h = (__jacl_memhdr_t*)ptr - 1;
	size_t old_size = h->size - sizeof(__jacl_memhdr_t);

	if (size <= old_size) return ptr;

	void* new_ptr = malloc(size);

	if (!new_ptr) return NULL;

	memcpy(new_ptr, ptr, old_size);

	if (!(h->flags & JACL_HDR_ARENA)) free(ptr);

	return new_ptr;
}

void* aligned_alloc(size_t alignment, size_t size) {
	if (alignment == 0 || (alignment & (alignment-1)) != 0 || size % alignment != 0) {
		errno = EINVAL;

		return NULL;
	}
	if (alignment <= JACL_ALIGNMENT) return malloc(size);

	size_t extra = alignment + sizeof(void*);
	void* raw = malloc(size + extra);

	if (!raw) return NULL;

	union { void* ptr; uintptr_t addr; } raw_conv, aligned_conv;

	raw_conv.ptr = raw;
	aligned_conv.addr = JACL_ALIGN_UP(raw_conv.addr + sizeof(void*), alignment);
	*(void**)((uint8_t*)aligned_conv.ptr - sizeof(void*)) = raw;

	return aligned_conv.ptr;
}

int posix_memalign(void** memptr, size_t alignment, size_t size) {
	if (!memptr) return EINVAL;
	if (alignment < sizeof(void*) || (alignment & (alignment - 1))) return EINVAL;
	if (size % alignment != 0) return EINVAL;

	void* p = aligned_alloc(alignment, size);

	if (!p) return ENOMEM;

	*memptr = p;

	return 0;
}

void* reallocarray(void* ptr, size_t nmemb, size_t size) {
	if (nmemb && size > SIZE_MAX / nmemb) {
		errno = ENOMEM;

		return NULL;
	}

	return realloc(ptr, nmemb * size);
}

#ifdef __cplusplus
}
#endif

#endif /* CORE_STDLIB_H */
