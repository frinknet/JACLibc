/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _AIO_H
#define _AIO_H
#pragma once

#include <config.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <stdatomic.h>
#include <pthread.h>
#include <fcntl.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================== */
/* POSIX Surface (Frozen ABI)                                               */
/* ======================================================================== */

struct aiocb {
	int             aio_fildes;
	int             aio_lio_opcode;
	int             aio_reqprio;
	volatile void  *aio_buf;
	size_t          aio_nbytes;
	off_t           aio_offset;
	struct sigevent aio_sigevent;

	/* Implementation-reserved fields */
	int             __jacl_state;
	ssize_t         __jacl_result;
	int             __jacl_errno_val;
	void           *__jacl_private;
};

typedef struct aiocb aiocb_t;

struct aioinit {
	int aio_threads;
	int aio_num;
	int aio_locks;
	int aio_usedba;
	int aio_debug;
	int aio_numusers;
	int aio_idle_time;
};

#define AIO_CANCELED    1
#define AIO_NOTCANCELED 2
#define AIO_ALLDONE     3

#define LIO_READ        0
#define LIO_WRITE       1
#define LIO_NOP         2

#define LIO_WAIT        0
#define LIO_NOWAIT      1

#define __JACL_OP_READ  0
#define __JACL_OP_WRITE 1
#define __JACL_OP_FSYNC 2

/* ======================================================================== */
/* Linux Backend: io_uring with full POSIX support                          */
/* ======================================================================== */
#if JACL_OS_LINUX

#define JACL_IORING_SETUP_CLAMP      (1U << 4)
#define JACL_IORING_ENTER_GETEVENTS  (1U << 0)
#define JACL_IORING_OP_READ          0
#define JACL_IORING_OP_WRITE         1
#define JACL_IORING_OP_FSYNC         2
#define JACL_IORING_OP_ASYNC_CANCEL  15

static inline int __jacl_check_fd_access(int fd, int want_write) {
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1) return EBADF;
	int accmode = flags & O_ACCMODE;
	if (want_write && accmode == O_RDONLY) return EBADF;
	if (!want_write && accmode == O_WRONLY) return EBADF;
	return 0;
}

struct __jacl_uring_sqe {
	uint8_t  opcode; uint8_t  flags; uint16_t ioprio; int32_t  fd;
	uint64_t off; uint64_t addr; uint32_t len; uint32_t rw_flags;
	uint64_t user_data; uint16_t buf_index; uint16_t personality;
	int32_t  splice_fd_in; uint32_t file_index; uint64_t addr2; uint8_t  __pad[8];
};

struct __jacl_uring_cqe {
	uint64_t user_data; int32_t  res; uint32_t flags;
};

struct __jacl_uring_params {
	uint32_t sq_entries; uint32_t cq_entries; uint32_t flags;
	uint32_t sq_thread_cpu; uint32_t sq_thread_idle; uint32_t features;
	uint32_t reserved[4];
	struct { uint32_t head, tail, ring_mask, ring_entries; uint32_t flags; uint32_t dropped; uint32_t array; uint32_t resv1; uint64_t resv2; uint64_t resv3[3]; } sq_off;
	struct { uint32_t head, tail, ring_mask, ring_entries; uint32_t overflow; uint32_t cqes; uint64_t resv[2]; } cq_off;
};

#define PROT_READ  0x1
#define PROT_WRITE 0x2
#define MAP_SHARED 0x01
#define MAP_POPULATE 0x8000
#define MAP_FAILED ((void *)-1)

struct __jacl_aio_thread_pool {
	pthread_t       *threads;
	int             num_threads;
	_Atomic int     shutdown;
	pthread_mutex_t lock;
	pthread_cond_t  cond;
	struct aiocb    **queue;
	int             queue_head;
	int             queue_tail;
	int             queue_size;
};

struct __jacl_aio_ring {
	int          fd;
	_Atomic int  initialized;
	_Atomic int  use_fallback;
	unsigned     sq_mask, cq_mask, sq_entries, cq_entries;

	_Atomic unsigned *sq_head;
	_Atomic unsigned *sq_tail;
	_Atomic unsigned *cq_head;
	_Atomic unsigned *cq_tail;

	struct __jacl_uring_sqe *sqes;
	struct __jacl_uring_cqe *cqes;
	unsigned    *sq_array;
};

extern void *mmap(void *, size_t, int, int, int, off_t);
extern int munmap(void *, size_t);
extern int close(int);

static inline struct __jacl_aio_ring* __jacl_ring_instance(void) {
	static struct __jacl_aio_ring _ring;
	static _Atomic int _init = 0;

	int state = atomic_load_explicit(&_init, memory_order_acquire);
	if (state != 2) {
		int expected = 0;

		if (atomic_compare_exchange_strong_explicit(&_init, &expected, 1, memory_order_acq_rel, memory_order_acquire)) {
			_ring.fd = -1;
			_ring.initialized = 0;
			_ring.use_fallback = 0;

			atomic_store_explicit(&_init, 2, memory_order_release);
		} else {
			while (atomic_load_explicit(&_init, memory_order_acquire) != 2) sched_yield();
		}
	}
	return &_ring;
}

static inline struct __jacl_aio_thread_pool* __jacl_pool_instance(void) {
	static struct __jacl_aio_thread_pool _pool;
	static _Atomic int _init = 0;

	int state = atomic_load_explicit(&_init, memory_order_acquire);
	if (state != 2) {
		int expected = 0;

		if (atomic_compare_exchange_strong_explicit(&_init, &expected, 1, memory_order_acq_rel, memory_order_acquire)) {
			_pool.threads = NULL;
			_pool.num_threads = 0;
			_pool.shutdown = 0;
			_pool.queue = NULL;
			_pool.queue_head = 0;
			_pool.queue_tail = 0;
			_pool.queue_size = 0;

			pthread_mutex_init(&_pool.lock, NULL);
			pthread_cond_init(&_pool.cond, NULL);
			atomic_store_explicit(&_init, 2, memory_order_release);
		} else {
			while (atomic_load_explicit(&_init, memory_order_acquire) != 2) sched_yield();
		}
	}
	return &_pool;
}

static inline int __jacl_validate_sigevent(const struct aiocb *cb) {
	if (!cb) return EINVAL;
	if (cb->aio_sigevent.sigev_notify != SIGEV_NONE && cb->aio_sigevent.sigev_notify != SIGEV_SIGNAL && cb->aio_sigevent.sigev_notify != SIGEV_THREAD) return EINVAL;
	if (cb->aio_sigevent.sigev_notify == SIGEV_THREAD && cb->aio_sigevent.sigev_notify_function == NULL) return EINVAL;
	if (cb->aio_sigevent.sigev_notify == SIGEV_SIGNAL && (cb->aio_sigevent.sigev_signo <= 0 || cb->aio_sigevent.sigev_signo >= NSIG)) return EINVAL;
	return 0;
}

static inline int __jacl_validate_args(struct aiocb *cb) {
	if (!cb) return EINVAL;
	if (cb->aio_fildes < 0) return EBADF;
	if (fcntl(cb->aio_fildes, F_GETFD) == -1) return EBADF;
	if (cb->aio_offset < 0) return EINVAL;
	if (cb->aio_nbytes > 0 && cb->aio_buf == NULL) return EFAULT;
	if (cb->aio_nbytes > (size_t)SSIZE_MAX) return EINVAL;
	return 0;
}

#define __jacl_ring (*__jacl_ring_instance())
#define __jacl_thread_pool (*__jacl_pool_instance())

static inline void aio_destroy(void) {
	pthread_mutex_lock(&__jacl_thread_pool.lock);

	/* Teardown Ring */
	if (__jacl_ring.fd >= 0) {
		close(__jacl_ring.fd);
		__jacl_ring.fd = -1;

		/* Unmap rings to prevent memory leaks and pointer corruption on re-init */
		if (__jacl_ring.sqes) {
			munmap(__jacl_ring.sqes, __jacl_ring.sq_entries * sizeof(struct __jacl_uring_sqe));
			__jacl_ring.sqes = NULL;
		}
		if (__jacl_ring.sq_head) {
			/* sq_head and sq_array are in the same mmap region */
			size_t sq_ring_sz = __jacl_ring.sq_entries * sizeof(unsigned) + 64; /* approx */
			munmap((void *)__jacl_ring.sq_head, sq_ring_sz);
			__jacl_ring.sq_head = NULL;
		}
		if (__jacl_ring.cq_head) {
			size_t cq_ring_sz = __jacl_ring.cq_entries * sizeof(struct __jacl_uring_cqe) + 64;
			munmap((void *)__jacl_ring.cq_head, cq_ring_sz);
			__jacl_ring.cq_head = NULL;
		}

		atomic_store_explicit(&__jacl_ring.use_fallback, 0, memory_order_release);
		atomic_store_explicit(&__jacl_ring.initialized, 0, memory_order_release);
	}

	/* Teardown Threads */
	if (__jacl_thread_pool.num_threads == 0) {
		pthread_mutex_unlock(&__jacl_thread_pool.lock);
		return;
	}

	atomic_store_explicit(&__jacl_thread_pool.shutdown, 1, memory_order_release);
	pthread_cond_broadcast(&__jacl_thread_pool.cond);
	pthread_mutex_unlock(&__jacl_thread_pool.lock);

	for (int i = 0; i < __jacl_thread_pool.num_threads; i++) {
		pthread_join(__jacl_thread_pool.threads[i], NULL);
	}

	free(__jacl_thread_pool.threads);
	free(__jacl_thread_pool.queue);

	/* Reset state. DO NOT destroy mutex/cond. */
	__jacl_thread_pool.threads = NULL;
	__jacl_thread_pool.queue = NULL;
	__jacl_thread_pool.num_threads = 0;
	__jacl_thread_pool.queue_head = 0;
	__jacl_thread_pool.queue_tail = 0;
	__jacl_thread_pool.queue_size = 0;
	atomic_store_explicit(&__jacl_thread_pool.shutdown, 0, memory_order_release);
}

static void __jacl_aio_thread_worker(struct __jacl_aio_thread_pool *pool) {
	while (!atomic_load_explicit(&pool->shutdown, memory_order_acquire)) {
		pthread_mutex_lock(&pool->lock);

		while (pool->queue_head == pool->queue_tail && !atomic_load_explicit(&pool->shutdown, memory_order_acquire)) {
			pthread_cond_wait(&pool->cond, &pool->lock);
		}

		if (atomic_load_explicit(&pool->shutdown, memory_order_acquire)) {
			pthread_mutex_unlock(&pool->lock);

			break;
		}

		struct aiocb *cb = pool->queue[pool->queue_head];
		pool->queue_head = (pool->queue_head + 1) % pool->queue_size;

		pthread_mutex_unlock(&pool->lock);

		if (cb && cb->__jacl_state == 0) {
			uintptr_t op = (uintptr_t)cb->__jacl_private;
			ssize_t r = -1;

			if (op == __JACL_OP_WRITE) {
				r = pwrite(cb->aio_fildes, (const void *)cb->aio_buf, cb->aio_nbytes, cb->aio_offset);
			} else if (op == __JACL_OP_FSYNC) {
				r = fsync(cb->aio_fildes);
			} else {
				r = pread(cb->aio_fildes, (void *)cb->aio_buf, cb->aio_nbytes, cb->aio_offset);
			}

			cb->__jacl_result = r;
			cb->__jacl_errno_val = (r < 0) ? errno : 0;

			int notify = cb->aio_sigevent.sigev_notify;
			int signo = cb->aio_sigevent.sigev_signo;
			void (*notify_func)(union sigval) = cb->aio_sigevent.sigev_notify_function;

			atomic_store_explicit((_Atomic int *)&cb->__jacl_state, (r < 0) ? 2 : 1, memory_order_release);

			if (notify == SIGEV_SIGNAL) {
				if (signo > 0 && signo < NSIG) {
					union sigval val = {.sival_ptr = (void *)cb};

					if (sigqueue(getpid(), signo, val) < 0 && __errno_chk(EAGAIN)) kill(getpid(), signo);
				}
			} else if (notify == SIGEV_THREAD && notify_func) {
				union sigval val = {.sival_ptr = (void *)cb};
				notify_func(val);
			}
		}
	}
}

static void *__jacl_aio_thread_trampoline(void *arg) {
	__jacl_aio_thread_worker((struct __jacl_aio_thread_pool *)arg);
	return NULL;
}

static inline int __jacl_aio_init_thread_pool(int num_threads) {
	pthread_mutex_lock(&__jacl_thread_pool.lock);
	if (__jacl_thread_pool.num_threads > 0) {
		pthread_mutex_unlock(&__jacl_thread_pool.lock);
		return 0;
	}

	num_threads = (num_threads > 0) ? num_threads : 4;

	__jacl_thread_pool.threads = calloc(num_threads, sizeof(pthread_t));
	__jacl_thread_pool.queue = calloc(1024, sizeof(struct aiocb *));
	__jacl_thread_pool.queue_size = 1024;
	atomic_store_explicit(&__jacl_thread_pool.shutdown, 0, memory_order_release);

	int created = 0;
	for (int i = 0; i < num_threads; i++) {
		if (pthread_create(&__jacl_thread_pool.threads[i], NULL, __jacl_aio_thread_trampoline, &__jacl_thread_pool) == 0) {
			created++;
		} else {
			break;
		}
	}

	if (created == 0) {
		free(__jacl_thread_pool.threads);
		free(__jacl_thread_pool.queue);
		__jacl_thread_pool.threads = NULL;
		__jacl_thread_pool.queue = NULL;
		__jacl_thread_pool.num_threads = 0;
		pthread_mutex_unlock(&__jacl_thread_pool.lock);
		return -1;
	}

	__jacl_thread_pool.num_threads = created;
	pthread_mutex_unlock(&__jacl_thread_pool.lock);
	return 0;
}

static inline int __jacl_aio_queue_fallback(struct aiocb *cb) {
	__jacl_aio_init_thread_pool(4);

	pthread_mutex_lock(&__jacl_thread_pool.lock);

	int next_tail = (__jacl_thread_pool.queue_tail + 1) % __jacl_thread_pool.queue_size;
	if (next_tail == __jacl_thread_pool.queue_head) {
		pthread_mutex_unlock(&__jacl_thread_pool.lock);
		return (__errno_set(EAGAIN), -1);
	}

	__jacl_thread_pool.queue[__jacl_thread_pool.queue_tail] = cb;
	__jacl_thread_pool.queue_tail = next_tail;

	pthread_cond_signal(&__jacl_thread_pool.cond);
	pthread_mutex_unlock(&__jacl_thread_pool.lock);

	return 0;
}

static inline int __jacl_aio_kick(int to_submit) {
	if (__jacl_ring.fd < 0) return -1;
	long r = syscall(SYS_io_uring_enter, __jacl_ring.fd, to_submit, 0,
					JACL_IORING_ENTER_GETEVENTS, NULL, 0);
	return (r < 0) ? -1 : 0;
}

static inline int __jacl_aio_init_ring(unsigned entries) {
	if (atomic_load_explicit(&__jacl_ring.initialized, memory_order_acquire)) {
		return 0;
	}

	/* Serialize initialization to prevent threads from using half-built rings */
	pthread_mutex_lock(&__jacl_thread_pool.lock);

	if (atomic_load_explicit(&__jacl_ring.initialized, memory_order_acquire)) {
		pthread_mutex_unlock(&__jacl_thread_pool.lock);

		return 0;
	}

	struct __jacl_uring_params p = {0};
	p.flags = JACL_IORING_SETUP_CLAMP;
	p.sq_entries = entries;
	p.cq_entries = entries * 2;

	int fd = (int)syscall(SYS_io_uring_setup, entries, &p);

	if (fd < 0) {
		if (__errno_chk(EPERM) || __errno_chk(ENOSYS) || __errno_chk(EOPNOTSUPP)) atomic_store_explicit(&__jacl_ring.use_fallback, 1, memory_order_release);

		atomic_store_explicit(&__jacl_ring.initialized, 1, memory_order_release);
		pthread_mutex_unlock(&__jacl_thread_pool.lock);

		return -1;
	}

	size_t sq_ring_sz = p.sq_off.array + p.sq_entries * sizeof(unsigned);
	size_t cq_ring_sz = p.cq_off.cqes + p.cq_entries * sizeof(struct __jacl_uring_cqe);
	size_t sqe_sz     = p.sq_entries * sizeof(struct __jacl_uring_sqe);

	void *sq_ptr = mmap(NULL, sq_ring_sz, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, 0);
	void *cq_ptr = mmap(NULL, cq_ring_sz, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, 0x40000000ULL);
	__jacl_ring.sqes = (struct __jacl_uring_sqe *)mmap(NULL, sqe_sz, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_POPULATE, fd, 0x80000000ULL);

	if (sq_ptr == MAP_FAILED || cq_ptr == MAP_FAILED || __jacl_ring.sqes == MAP_FAILED) {
		if (sq_ptr != MAP_FAILED) munmap(sq_ptr, sq_ring_sz);
		if (cq_ptr != MAP_FAILED) munmap(cq_ptr, cq_ring_sz);
		if (__jacl_ring.sqes != MAP_FAILED) munmap(__jacl_ring.sqes, sqe_sz);

		close(fd);

		__jacl_ring.fd = -1;

		atomic_store_explicit(&__jacl_ring.initialized, 1, memory_order_release);
		pthread_mutex_unlock(&__jacl_thread_pool.lock);

		return -1;
	}

	__jacl_ring.fd = fd;
	__jacl_ring.sq_entries = p.sq_entries;
	__jacl_ring.cq_entries = p.cq_entries;

	__jacl_ring.sq_head = (_Atomic unsigned *)((char *)sq_ptr + p.sq_off.head);
	__jacl_ring.sq_tail = (_Atomic unsigned *)((char *)sq_ptr + p.sq_off.tail);
	__jacl_ring.sq_mask = *(unsigned *)((char *)sq_ptr + p.sq_off.ring_mask);
	__jacl_ring.sq_array = (unsigned *)((char *)sq_ptr + p.sq_off.array);

	__jacl_ring.cq_head = (_Atomic unsigned *)((char *)cq_ptr + p.cq_off.head);
	__jacl_ring.cq_tail = (_Atomic unsigned *)((char *)cq_ptr + p.cq_off.tail);
	__jacl_ring.cq_mask = *(unsigned *)((char *)cq_ptr + p.cq_off.ring_mask);
	__jacl_ring.cqes = (struct __jacl_uring_cqe *)((char *)cq_ptr + p.cq_off.cqes);

	/* Mark as initialized ONLY after all pointers are valid */
	atomic_store_explicit(&__jacl_ring.initialized, 1, memory_order_release);
	pthread_mutex_unlock(&__jacl_thread_pool.lock);

	return 0;
}

static inline struct __jacl_uring_sqe *__jacl_aio_get_sqe(void) {
	if (atomic_load_explicit(&__jacl_ring.use_fallback, memory_order_acquire) ||
	    __jacl_ring.fd < 0) return NULL;

	unsigned tail = atomic_load_explicit(__jacl_ring.sq_tail, memory_order_acquire);
	unsigned head = atomic_load_explicit(__jacl_ring.sq_head, memory_order_acquire);

	if (tail - head >= __jacl_ring.sq_entries) return NULL;

	unsigned idx = tail & __jacl_ring.sq_mask;
	__jacl_ring.sq_array[idx] = idx;

	atomic_store_explicit(__jacl_ring.sq_tail, tail + 1, memory_order_release);

	return &__jacl_ring.sqes[idx];
}

static inline void __jacl_aio_reap_cq(void) {
	if (atomic_load_explicit(&__jacl_ring.use_fallback, memory_order_acquire) ||
	    __jacl_ring.fd < 0) return;

	unsigned head = atomic_load_explicit(__jacl_ring.cq_head, memory_order_acquire);
	unsigned tail = atomic_load_explicit(__jacl_ring.cq_tail, memory_order_acquire);

	while (head != tail) {
		unsigned idx = head & __jacl_ring.cq_mask;
		struct __jacl_uring_cqe *cqe = &__jacl_ring.cqes[idx];
		struct aiocb *cb = (struct aiocb *)(uintptr_t)cqe->user_data;
		if (cb && cb->__jacl_state == 0) {
			if (cqe->res >= 0) {
				cb->__jacl_result = cqe->res;
				cb->__jacl_errno_val = 0;
			} else if (cqe->res == -125) {
				cb->__jacl_result = -1;
				cb->__jacl_errno_val = 125;
			} else {
				cb->__jacl_result = -1;
				cb->__jacl_errno_val = -cqe->res;
			}

			/* CACHE signal data BEFORE flipping state to prevent Use-After-Free */
			int notify = cb->aio_sigevent.sigev_notify;
			int signo = cb->aio_sigevent.sigev_signo;
			void (*notify_func)(union sigval) = cb->aio_sigevent.sigev_notify_function;

			/* Flip state last with release semantics */
			atomic_store_explicit((_Atomic int *)&cb->__jacl_state, (cqe->res >= 0) ? 1 : ((cqe->res == -125) ? 3 : 2), memory_order_release);

			/* Use cached data so we don't touch the potentially dead cb */
			if (notify == SIGEV_SIGNAL) {
				if (signo > 0 && signo < NSIG) {
					union sigval val = {.sival_ptr = (void *)cb};
					if (sigqueue(getpid(), signo, val) < 0 && __errno_chk(EAGAIN)) {
						kill(getpid(), signo);
					}
				}
			} else if (notify == SIGEV_THREAD && notify_func) {
				union sigval val = {.sival_ptr = (void *)cb};
				notify_func(val);
			}
		}
		head++;
	}

	atomic_store_explicit(__jacl_ring.cq_head, head, memory_order_release);
}

static inline int __jacl_aio_submit_sqe(struct __jacl_uring_sqe *sqe, struct aiocb *cb) {
	if (!cb || !sqe) return -1;
	sqe->user_data = (uint64_t)(uintptr_t)cb;
	sqe->flags = 0;
	cb->__jacl_state = 0;
	cb->__jacl_result = -1;
	cb->__jacl_errno_val = 0;
	return 0;
}

static inline void aio_init(const struct aioinit *init) {
	__jacl_aio_init_ring(1024);
	if (init && init->aio_threads > 0) {
		__jacl_aio_init_thread_pool(init->aio_threads);
	} else {
		__jacl_aio_init_thread_pool(4);
	}
}

static inline int aio_read(struct aiocb *cb) {
	int err = __jacl_validate_sigevent(cb);
	if (err != 0) return (__errno_set(err), -1);

	err = __jacl_validate_args(cb);
	if (err != 0) return (__errno_set(err), -1);

	err = __jacl_check_fd_access(cb->aio_fildes, 0);
	if (err != 0) return (__errno_set(err), -1);

	if (__jacl_aio_init_ring(1024) == 0) {
		struct __jacl_uring_sqe *sqe = __jacl_aio_get_sqe();
		if (sqe) {
			sqe->opcode = JACL_IORING_OP_READ;
			sqe->fd = cb->aio_fildes;
			sqe->addr = (uint64_t)(uintptr_t)cb->aio_buf;
			sqe->len = cb->aio_nbytes;
			sqe->off = cb->aio_offset;
			__jacl_aio_submit_sqe(sqe, cb);
			return __jacl_aio_kick(1);
		}
	}

	cb->__jacl_private = (void *)(uintptr_t)__JACL_OP_READ;
	return __jacl_aio_queue_fallback(cb);
}

static inline int aio_write(struct aiocb *cb) {
	int err = __jacl_validate_sigevent(cb);
	if (err != 0) return (__errno_set(err), -1);

	err = __jacl_validate_args(cb);
	if (err != 0) return (__errno_set(err), -1);

	err = __jacl_check_fd_access(cb->aio_fildes, 1);
	if (err != 0) return (__errno_set(err), -1);

	if (__jacl_aio_init_ring(1024) == 0) {
		struct __jacl_uring_sqe *sqe = __jacl_aio_get_sqe();
		if (sqe) {
			sqe->opcode = JACL_IORING_OP_WRITE;
			sqe->fd = cb->aio_fildes;
			sqe->addr = (uint64_t)(uintptr_t)cb->aio_buf;
			sqe->len = cb->aio_nbytes;
			sqe->off = cb->aio_offset;
			__jacl_aio_submit_sqe(sqe, cb);
			return __jacl_aio_kick(1);
		}
	}

	cb->__jacl_private = (void *)(uintptr_t)__JACL_OP_WRITE;
	return __jacl_aio_queue_fallback(cb);
}

static inline int aio_fsync(int op, struct aiocb *cb) {
	(void)op;
	int err = __jacl_validate_sigevent(cb);
	if (err != 0) return (__errno_set(err), -1);

	err = __jacl_validate_args(cb);
	if (err != 0) return (__errno_set(err), -1);

	if (__jacl_aio_init_ring(1024) == 0) {
		struct __jacl_uring_sqe *sqe = __jacl_aio_get_sqe();
		if (sqe) {
			sqe->opcode = JACL_IORING_OP_FSYNC;
			sqe->fd = cb->aio_fildes;
			__jacl_aio_submit_sqe(sqe, cb);
			return __jacl_aio_kick(1);
		}
	}

	cb->__jacl_private = (void *)(uintptr_t)__JACL_OP_FSYNC;
	return __jacl_aio_queue_fallback(cb);
}

static inline int aio_mlock(struct aiocb *cb) { (void)cb; return (__errno_set(ENOSYS), -1); }

static inline int aio_error(const struct aiocb *cb) {
	if (!cb) return EINVAL;
	__jacl_aio_reap_cq();

	int state = atomic_load_explicit((_Atomic int *)&cb->__jacl_state, memory_order_acquire);
	if (state == 0) return EINPROGRESS;
	if (state >= 1 && state <= 3) {
		return (cb->__jacl_errno_val != 0) ? cb->__jacl_errno_val : 0;
	}
	return EINVAL;
}

static inline ssize_t aio_return(struct aiocb *cb) {
	if (!cb) return (__errno_set(EINVAL), -1);
	int err = aio_error(cb);
	if (err == EINPROGRESS) return (__errno_set(err), -1);
	if (cb->__jacl_state == 2 || cb->__jacl_state == 3) return (__errno_set(cb->__jacl_errno_val), -1);
	return cb->__jacl_result;
}

static inline int aio_suspend(const struct aiocb *const cbs[], int n, const struct timespec *timeout) {
	if (!cbs || n <= 0) return (__errno_set(EINVAL), -1);
	__jacl_aio_reap_cq();

	for (int i = 0; i < n; i++) if (cbs[i] && cbs[i]->__jacl_state != 0) return 0;

	if (__jacl_ring.fd < 0) return (__errno_set(EINVAL), -1);

	long r = syscall(SYS_io_uring_enter, __jacl_ring.fd, 0, 1, JACL_IORING_ENTER_GETEVENTS,
					timeout ? (void *)timeout : NULL, sizeof(struct timespec));
	if (r < 0 && __errno_chk(EINTR)) return -1;

	__jacl_aio_reap_cq();
	for (int i = 0; i < n; i++) if (cbs[i] && cbs[i]->__jacl_state != 0) return 0;
	return (timeout && r == 0) ? -1 : 0;
}

static inline int aio_cancel(int fd, struct aiocb *cb) {
	(void)fd;
	if (!cb) return AIO_NOTCANCELED;

	__jacl_aio_reap_cq();
	if (cb->__jacl_state != 0) {
		return (cb->__jacl_state == 3) ? AIO_CANCELED : AIO_ALLDONE;
	}

	if (__jacl_ring.fd < 0) return AIO_NOTCANCELED;

	struct __jacl_uring_sqe *sqe = __jacl_aio_get_sqe();
	if (!sqe) return AIO_NOTCANCELED;

	sqe->opcode = JACL_IORING_OP_ASYNC_CANCEL;
	sqe->addr = (uint64_t)(uintptr_t)cb;

	if (__jacl_aio_kick(1) < 0) return AIO_NOTCANCELED;

	for (int i = 0; i < 100; i++) {
		__jacl_aio_reap_cq();
		if (cb->__jacl_state != 0) {
			return (cb->__jacl_state == 3) ? AIO_CANCELED : AIO_ALLDONE;
		}
		usleep(100);
	}

	return AIO_NOTCANCELED;
}

static inline int lio_listio(int mode, struct aiocb *const cb[], int nent, struct sigevent *sig) {
	if (nent <= 0 || cb == NULL) return (__errno_set(EINVAL), -1);
	if (sig != NULL) return (__errno_set(EINVAL), -1);

	for (int i = 0; i < nent; i++) {
		if (cb[i] == NULL) continue;

		if (cb[i]->aio_lio_opcode != LIO_READ &&
		    cb[i]->aio_lio_opcode != LIO_WRITE &&
		    cb[i]->aio_lio_opcode != LIO_NOP) {
			return (__errno_set(EINVAL), -1);
		}

		if (cb[i]->aio_lio_opcode != LIO_NOP && cb[i]->aio_fildes < 0) {
			return (__errno_set(EBADF), -1);
		}

		if (cb[i]->aio_sigevent.sigev_notify != SIGEV_NONE &&
		    cb[i]->aio_sigevent.sigev_notify != SIGEV_SIGNAL &&
		    cb[i]->aio_sigevent.sigev_notify != SIGEV_THREAD) {
			return (__errno_set(EINVAL), -1);
		}
	}

	for (int i = 0; i < nent; i++) {
		if (cb[i] == NULL) continue;

		if (cb[i]->aio_lio_opcode == LIO_NOP) {
			cb[i]->__jacl_state = 1;
			cb[i]->__jacl_result = 0;
			cb[i]->__jacl_errno_val = 0;
			continue;
		}

		if (cb[i]->aio_lio_opcode == LIO_READ) {
			aio_read(cb[i]);
		} else if (cb[i]->aio_lio_opcode == LIO_WRITE) {
			aio_write(cb[i]);
		}
	}

	if (mode == LIO_WAIT) {
		for (int i = 0; i < nent; i++) {
			if (cb[i] == NULL) continue;
			struct timespec start, now;
			clock_gettime(CLOCK_MONOTONIC, &start);
			while (cb[i]->__jacl_state == 0) {
				clock_gettime(CLOCK_MONOTONIC, &now);
				long elapsed_ms = (now.tv_sec - start.tv_sec) * 1000 +
				                  (now.tv_nsec - start.tv_nsec) / 1000000;
				if (elapsed_ms > 30000) {
					cb[i]->__jacl_state = 2;
					cb[i]->__jacl_errno_val = ETIME;
					cb[i]->__jacl_result = -1;
					break;
				}
				sched_yield();
			}
		}
	}

	return 0;
}

static inline int aio_submit_batch(struct aiocb *cbs[], int n) {
	if (!cbs || n <= 0) return (__errno_set(EINVAL), -1);

	int submitted = 0;
	for (int i = 0; i < n; i++) {
		if (cbs[i] == NULL) continue;
		if (cbs[i]->aio_lio_opcode == LIO_WRITE) {
			if (aio_write(cbs[i]) == 0) submitted++;
		} else {
			if (aio_read(cbs[i]) == 0) submitted++;
		}
	}

	return submitted;
}

#elif JACL_OS_DARWIN || JACL_OS_BSD

#include <sys/event.h>

static inline int *__jacl_kq_instance(void) {
	static int _kq = -1;
	return &_kq;
}
#define __jacl_kq (*__jacl_kq_instance())

static inline int __jacl_kqueue_init(void) {
	if (__jacl_kq < 0) {
		__jacl_kq = kqueue();
		if (__jacl_kq < 0) return -1;
	}
	return 0;
}
static inline void aio_init(const struct aioinit *init) {
	(void)init;
	__jacl_kqueue_init();
}
static inline void aio_destroy(void) {
	if (__jacl_kq >= 0) {
		close(__jacl_kq);
		__jacl_kq = -1;
	}
}
static inline int aio_read(struct aiocb *cb) {
	if (!cb) return (__errno_set(EINVAL), -1);

	int ret = syscall(SYS_aio_read, cb);
	if (ret < 0) return -1;

	if (cb->aio_sigevent.sigev_notify != SIGEV_NONE && __jacl_kq >= 0) {
		struct kevent ev;
		EV_SET(&ev, (uintptr_t)cb, EVFILT_AIO, EV_ADD, 0, 0, cb);
		kevent(__jacl_kq, &ev, 1, NULL, 0, NULL);
	}

	return 0;
}
static inline int aio_write(struct aiocb *cb) {
	if (!cb) return (__errno_set(EINVAL), -1);

	int ret = syscall(SYS_aio_write, cb);
	if (ret < 0) return -1;

	if (cb->aio_sigevent.sigev_notify != SIGEV_NONE && __jacl_kq >= 0) {
		struct kevent ev;
		EV_SET(&ev, (uintptr_t)cb, EVFILT_AIO, EV_ADD, 0, 0, cb);
		kevent(__jacl_kq, &ev, 1, NULL, 0, NULL);
	}

	return 0;
}
static inline int aio_fsync(int op, struct aiocb *cb) {
	(void)op;
	if (!cb) return (__errno_set(EINVAL), -1);
	return syscall(SYS_aio_fsync, cb);
}
static inline int aio_mlock(struct aiocb *cb) { (void)cb; return (__errno_set(ENOSYS), -1); }
static inline int aio_error(const struct aiocb *cb) {
	if (!cb) return EINVAL;
	return syscall(SYS_aio_error, cb);
}
static inline ssize_t aio_return(struct aiocb *cb) {
	if (!cb) return (__errno_set(EINVAL), -1);
	return syscall(SYS_aio_return, cb);
}
static inline int aio_suspend(const struct aiocb *const cbs[], int n, const struct timespec *timeout) {
	if (!cbs || n <= 0) return (__errno_set(EINVAL), -1);

	if (__jacl_kq >= 0) {
		struct kevent ev;
		int ret = kevent(__jacl_kq, NULL, 0, &ev, 1, timeout);
		if (ret > 0) return 0;
		if (__errno_chk(EINTR)) return -1;
		if (timeout && ret == 0) return (__errno_set(EAGAIN), -1);
	}

	return syscall(SYS_aio_suspend, cbs, n, timeout);
}
static inline int aio_cancel(int fd, struct aiocb *cb) {
	if (!cb) return AIO_NOTCANCELED;
	return syscall(SYS_aio_cancel, fd, cb);
}
static inline int lio_listio(int mode, struct aiocb *const cbs[], int n, struct sigevent *sig) {
	if (!cbs || n <= 0) return (__errno_set(EINVAL), -1);
	return syscall(SYS_lio_listio, mode, cbs, n, sig);
}
static inline int aio_submit_batch(struct aiocb *cbs[], int n) {
	return lio_listio(LIO_NOWAIT, cbs, n, NULL);
}

#elif JACL_OS_WINDOWS

typedef void *HANDLE;
typedef unsigned long DWORD;
typedef int BOOL;

typedef struct _OVERLAPPED {
	unsigned long Internal, InternalHigh, Offset, OffsetHigh;
	void *hEvent;
} OVERLAPPED;

#define ERROR_IO_PENDING 997L

extern BOOL ReadFile(HANDLE, void *, DWORD, DWORD *, OVERLAPPED *);
extern BOOL WriteFile(HANDLE, const void *, DWORD, DWORD *, OVERLAPPED *);
extern BOOL FlushFileBuffers(HANDLE);
extern unsigned long GetLastError(void);
extern void *calloc(size_t, size_t);
extern void free(void *);
extern intptr_t _get_osfhandle(int fd);

static inline HANDLE __jacl_win_get_handle(int fd) {
	return (fd >= 0) ? (HANDLE)_get_osfhandle(fd) : INVALID_HANDLE_VALUE;
}
static inline void aio_init(const struct aioinit *init) { (void)init; }
static inline void aio_destroy(void) { }

static inline int aio_read(struct aiocb *cb) {
	if (!cb) return (__errno_set(EINVAL), -1);

	HANDLE h = __jacl_win_get_handle(cb->aio_fildes);
	if (h == INVALID_HANDLE_VALUE) return (__errno_set(EBADF), -1);

	OVERLAPPED *ol = (OVERLAPPED *)calloc(1, sizeof(OVERLAPPED));
	if (!ol) return (__errno_set(ENOMEM), -1);
	ol->Offset = cb->aio_offset & 0xFFFFFFFF;
	ol->OffsetHigh = (cb->aio_offset >> 32) & 0xFFFFFFFF;

	BOOL r = ReadFile(h, (void *)cb->aio_buf, (DWORD)cb->aio_nbytes, NULL, ol);
	if (r) {
		free(ol);
		cb->__jacl_state = 1;
		cb->__jacl_result = cb->aio_nbytes;
		return 0;
	}
	if (GetLastError() == ERROR_IO_PENDING) {
		cb->__jacl_private = ol;
		cb->__jacl_state = 0;
		return 0;
	}
	free(ol);
	cb->__jacl_state = 2;
	cb->__jacl_errno_val = (int)GetLastError();
	return -1;
}

static inline int aio_write(struct aiocb *cb) {
	if (!cb) return (__errno_set(EINVAL), -1);

	HANDLE h = __jacl_win_get_handle(cb->aio_fildes);
	if (h == INVALID_HANDLE_VALUE) return (__errno_set(EBADF), -1);

	OVERLAPPED *ol = (OVERLAPPED *)calloc(1, sizeof(OVERLAPPED));
	if (!ol) return (__errno_set(ENOMEM), -1);
	ol->Offset = cb->aio_offset & 0xFFFFFFFF;
	ol->OffsetHigh = (cb->aio_offset >> 32) & 0xFFFFFFFF;

	BOOL r = WriteFile(h, (const void *)cb->aio_buf, (DWORD)cb->aio_nbytes, NULL, ol);
	if (r) {
		free(ol);
		cb->__jacl_state = 1;
		cb->__jacl_result = cb->aio_nbytes;
		return 0;
	}
	if (GetLastError() == ERROR_IO_PENDING) {
		cb->__jacl_private = ol;
		cb->__jacl_state = 0;
		return 0;
	}
	free(ol);
	cb->__jacl_state = 2;
	cb->__jacl_errno_val = (int)GetLastError();
	return -1;
}

static inline int aio_fsync(int op, struct aiocb *cb) {
	(void)op;
	if (!cb) return (__errno_set(EINVAL), -1);
	HANDLE h = __jacl_win_get_handle(cb->aio_fildes);
	if (h == INVALID_HANDLE_VALUE) return (__errno_set(EBADF), -1);
	return FlushFileBuffers(h) ? 0 : -1;
}

static inline int aio_mlock(struct aiocb *cb) { (void)cb; return (__errno_set(ENOSYS), -1); }

static inline int aio_error(const struct aiocb *cb) {
	if (!cb) return EINVAL;
	return cb->__jacl_state == 0 ? EINPROGRESS : cb->__jacl_errno_val;
}

static inline ssize_t aio_return(struct aiocb *cb) {
	if (!cb) return (__errno_set(EINVAL), -1);
	if (cb->__jacl_state == 0) return (__errno_set(EINPROGRESS), -1);

	if (cb->__jacl_private) {
		free(cb->__jacl_private);
		cb->__jacl_private = NULL;
	}

	return cb->__jacl_result;
}

static inline int aio_suspend(const struct aiocb *const cbs[], int n, const struct timespec *timeout) {  (void)cbs; (void)n; (void)timeout; return (__errno_set(ENOSYS), -1); }
static inline int aio_cancel(int fd, struct aiocb *cb) { (void)fd; (void)cb; return AIO_NOTCANCELED; }
static inline int lio_listio(int mode, struct aiocb *const cbs[], int n, struct sigevent *sig) { (void)mode; (void)cbs; (void)n; (void)sig; return (__errno_set(ENOSYS), -1); }
static inline int aio_submit_batch(struct aiocb *cbs[], int n) { (void)cbs; (void)n; return (__errno_set(ENOSYS), -1); }

#else

static inline void aio_init(const struct aioinit *init) { (void)init; }
static inline void aio_destroy(void) { }
static inline int aio_read(struct aiocb *cb) { (void)cb; return (__errno_set(ENOSYS), -1); }
static inline int aio_write(struct aiocb *cb) { (void)cb; return (__errno_set(ENOSYS), -1); }
static inline int aio_fsync(int op, struct aiocb *cb) { (void)op; (void)cb; return (__errno_set(ENOSYS), -1); }
static inline int aio_mlock(struct aiocb *cb) { (void)cb; return (__errno_set(ENOSYS), -1); }
static inline int aio_error(const struct aiocb *cb) { (void)cb; return ENOSYS; }
static inline ssize_t aio_return(struct aiocb *cb) { (void)cb; return (__errno_set(ENOSYS), -1); }
static inline int aio_suspend(const struct aiocb *const cbs[], int n, const struct timespec *timeout) { (void)cbs; (void)n; (void)timeout; return (__errno_set(ENOSYS), -1); }
static inline int aio_cancel(int fd, struct aiocb *cb) { (void)fd; (void)cb; return AIO_NOTCANCELED; }
static inline int lio_listio(int mode, struct aiocb *const cbs[], int n, struct sigevent *sig) { (void)mode; (void)cbs; (void)n; (void)sig; return (__errno_set(ENOSYS), -1); }
static inline int aio_submit_batch(struct aiocb *cbs[], int n) { (void)cbs; (void)n; return (__errno_set(ENOSYS), -1); }

#endif /* JACL_OS_* */

/* ======================================================================== */
/* LFS Aliases                                                              */
/* ======================================================================== */
#if JACL_HAS_LFS

static inline int aio_read64(struct aiocb *cb) { return aio_read(cb); }
static inline int aio_write64(struct aiocb *cb) { return aio_write(cb); }
static inline int aio_fsync64(int op, struct aiocb *cb) { return aio_fsync(op, cb); }
static inline int aio_mlock64(struct aiocb *cb) { return aio_mlock(cb); }
static inline int aio_error64(const struct aiocb *cb) { return aio_error(cb); }
static inline ssize_t aio_return64(struct aiocb *cb) { return aio_return(cb); }
static inline int aio_suspend64(const struct aiocb *const cbs[], int n, const struct timespec *timeout) { return aio_suspend(cbs, n, timeout); }
static inline int aio_cancel64(int fd, struct aiocb *cb) { return aio_cancel(fd, cb); }
static inline int lio_listio64(int mode, struct aiocb *const cbs[], int n, struct sigevent *sig) { return lio_listio(mode, cbs, n, sig); }

#endif /* JACL_HAS_LFS */

#ifdef __cplusplus
}
#endif

#endif /* _AIO_H */
