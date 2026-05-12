/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H
#pragma once

#include <config.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#if JACL_OS_WINDOWS
	#include <windows.h>
	#include <io.h>
#elif JACL_ARCH_WASM
/* WASM doesn't support memory mapping */
#else
	#include <unistd.h>
	#include <sys/syscall.h>
#endif

/* ================================================================ */
/* Memory protection flags                                          */
/* ================================================================ */
#define PROT_NONE  0x00
#define PROT_READ  0x01
#define PROT_WRITE 0x02
#define PROT_EXEC  0x04

/* ================================================================ */
/* Memory mapping flags                                             */
/* ================================================================ */
#define MAP_SHARED     0x01
#define MAP_PRIVATE    0x02
#define MAP_FIXED      0x10
#define MAP_ANONYMOUS  0x20
#define MAP_ANON       MAP_ANONYMOUS
#define MAP_SHARED_VALIDATE 0x03 /* Linux 4.15+ */

/* Linux-specific flags */
#if JACL_OS_LINUX
#define MAP_GROWSDOWN  0x00100
#define MAP_DENYWRITE  0x00800
#define MAP_EXECUTABLE 0x01000
#define MAP_LOCKED     0x02000
#define MAP_NORESERVE  0x04000
#define MAP_POPULATE   0x08000
#define MAP_NONBLOCK   0x10000
#define MAP_STACK      0x20000
#define MAP_HUGETLB    0x40000
#define MAP_SYNC       0x80000 /* DAX sync */
#define MAP_UNINITIALIZED 0x4000000 /* Kernel config dependent */
#endif

/* Error return value */
#define MAP_FAILED ((void*)-1)

/* ================================================================ */
/* Memory synchronization flags                                     */
/* ================================================================ */
#define MS_ASYNC      0x01
#define MS_SYNC       0x04
#define MS_INVALIDATE 0x02

/* ================================================================ */
/* Memory advisory flags                                            */
/* ================================================================ */
#define POSIX_MADV_NORMAL     0
#define POSIX_MADV_RANDOM     1
#define POSIX_MADV_SEQUENTIAL 2
#define POSIX_MADV_WILLNEED   3
#define POSIX_MADV_DONTNEED   4

#define MADV_NORMAL       0
#define MADV_RANDOM       1
#define MADV_SEQUENTIAL   2
#define MADV_WILLNEED     3
#define MADV_DONTNEED     4
#define MADV_FREE         8
#define MADV_REMOVE       9
#define MADV_DONTFORK     10
#define MADV_DOFORK       11
#define MADV_MERGEABLE    12
#define MADV_UNMERGEABLE  13
#define MADV_HUGEPAGE     14
#define MADV_NOHUGEPAGE   15
#define MADV_WIPEONFORK   18
#define MADV_KEEPONFORK   19

/* ================================================================ */
/* Memory locking flags                                             */
/* ================================================================ */
#define MCL_CURRENT 0x01
#define MCL_FUTURE  0x02
#define MCL_ONFAULT 0x04

/* ================================================================ */
/* Function implementations                                         */
/* ================================================================ */

#if JACL_OS_WINDOWS
/* ================================================================ */
/* Windows implementation using Win32 APIs                          */
/* ================================================================ */

static inline void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
	(void)addr; /* Windows chooses address unless MAP_FIXED is handled */

	/* Protection flags */
	DWORD win_prot = PAGE_NOACCESS;
	if (flags & MAP_ANONYMOUS) {
		if (prot & PROT_EXEC) win_prot = (prot & PROT_WRITE) ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ;
		else if (prot & PROT_WRITE) win_prot = PAGE_READWRITE;
		else if (prot & PROT_READ) win_prot = PAGE_READONLY;
	} else {
		if (prot & PROT_EXEC) win_prot = (prot & PROT_WRITE) ? PAGE_EXECUTE_WRITECOPY : PAGE_EXECUTE_READ;
		else if (prot & PROT_WRITE) win_prot = (prot & PROT_READ) ? PAGE_READWRITE : PAGE_WRITECOPY;
		else if (prot & PROT_READ) win_prot = PAGE_READONLY;
	}

	HANDLE hFile = INVALID_HANDLE_VALUE;
	HANDLE hMapping;
	void *result;

	if (flags & MAP_ANONYMOUS) {
		hMapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, win_prot,
		                              (DWORD)(length >> 32), (DWORD)length, NULL);
	} else {
		hFile = (HANDLE)_get_osfhandle(fd);
		if (hFile == INVALID_HANDLE_VALUE) return MAP_FAILED;
		/* Size is maximum mapping size, not offset+length */
		hMapping = CreateFileMappingA(hFile, NULL, win_prot,
		                              (DWORD)((offset + length) >> 32), (DWORD)(offset + length), NULL);
	}

	if (!hMapping) return MAP_FAILED;

	DWORD access = 0;
	if (prot & PROT_WRITE) access = FILE_MAP_WRITE;
	else if (prot & PROT_READ) access = FILE_MAP_READ;
	if (prot & PROT_EXEC) access |= FILE_MAP_EXECUTE;

	result = MapViewOfFile(hMapping, access, (DWORD)(offset >> 32), (DWORD)offset, length);
	CloseHandle(hMapping);

	return result ? result : MAP_FAILED;
}

static inline int munmap(void *addr, size_t length) {
	(void)length;
	return UnmapViewOfFile(addr) ? 0 : -1;
}

static inline int mprotect(void *addr, size_t len, int prot) {
	DWORD old_protect, new_protect = PAGE_NOACCESS;
	if (prot & PROT_EXEC) new_protect = (prot & PROT_WRITE) ? PAGE_EXECUTE_READWRITE : (prot & PROT_READ) ? PAGE_EXECUTE_READ : PAGE_EXECUTE;
	else if (prot & PROT_WRITE) new_protect = PAGE_READWRITE;
	else if (prot & PROT_READ) new_protect = PAGE_READONLY;

	return VirtualProtect(addr, len, new_protect, &old_protect) ? 0 : -1;
}

static inline int msync(void *addr, size_t length, int flags) {
	(void)flags;
	return FlushViewOfFile(addr, length) ? 0 : -1;
}

static inline int madvise(void *addr, size_t length, int advice) { (void)addr; (void)length; (void)advice; return 0; }
static inline int mlock(const void *addr, size_t len) { return VirtualLock((void*)addr, len) ? 0 : -1; }
static inline int munlock(const void *addr, size_t len) { return VirtualUnlock((void*)addr, len) ? 0 : -1; }
static inline int mlockall(int flags) { (void)flags; return -1; }
static inline int munlockall(void) { return -1; }
static inline int mincore(void *addr, size_t length, unsigned char *vec) { (void)addr; (void)length; (void)vec; return -1; }

#elif JACL_ARCH_WASM
/* ================================================================ */
/* WebAssembly - Memory mapping not supported                       */
/* ================================================================ */
static inline void *mmap(void *a, size_t l, int p, int f, int fd, off_t o) { (void)a;(void)l;(void)p;(void)f;(void)fd;(void)o; return MAP_FAILED; }
static inline int munmap(void *a, size_t l) { (void)a;(void)l; return -1; }
static inline int mprotect(void *a, size_t l, int p) { (void)a;(void)l;(void)p; return -1; }
static inline int msync(void *a, size_t l, int f) { (void)a;(void)l;(void)f; return -1; }
static inline int madvise(void *a, size_t l, int adv) { (void)a;(void)l;(void)adv; return -1; }
static inline int mlock(const void *a, size_t l) { (void)a;(void)l; return -1; }
static inline int munlock(const void *a, size_t l) { (void)a;(void)l; return -1; }
static inline int mlockall(int f) { (void)f; return -1; }
static inline int munlockall(void) { return -1; }
static inline int mincore(void *a, size_t l, unsigned char *v) { (void)a;(void)l;(void)v; return -1; }

#else
/* ================================================================ */
/* POSIX/Linux implementation using system calls                    */
/* ================================================================ */
static inline void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
	long res = syscall(SYS_mmap, (long)(uintptr_t)addr, (long)length, (long)prot, (long)flags, (long)fd, (long)offset);
	return (void*)res;
}
static inline int munmap(void *addr, size_t length) { return (int)syscall(SYS_munmap, (long)(uintptr_t)addr, (long)length); }
static inline int mprotect(void *addr, size_t len, int prot) { return (int)syscall(SYS_mprotect, (long)(uintptr_t)addr, (long)len, (long)prot); }
static inline int msync(void *addr, size_t length, int flags) { return (int)syscall(SYS_msync, (long)(uintptr_t)addr, (long)length, (long)flags); }
static inline int madvise(void *addr, size_t length, int advice) { return (int)syscall(SYS_madvise, (long)(uintptr_t)addr, (long)length, (long)advice); }
static inline int mlock(const void *addr, size_t len) { return (int)syscall(SYS_mlock, (long)(uintptr_t)addr, (long)len); }
static inline int munlock(const void *addr, size_t len) { return (int)syscall(SYS_munlock, (long)(uintptr_t)addr, (long)len); }
static inline int mlockall(int flags) { return (int)syscall(SYS_mlockall, (long)flags); }
static inline int munlockall(void) { return (int)syscall(SYS_munlockall); }
static inline int mincore(void *addr, size_t length, unsigned char *vec) { return (int)syscall(SYS_mincore, (long)(uintptr_t)addr, (long)length, (long)(uintptr_t)vec); }
#endif

#ifdef __cplusplus
}
#endif
#endif /* _SYS_MMAN_H */
