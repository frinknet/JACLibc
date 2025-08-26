/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef SYS_MMAN_H
#define SYS_MMAN_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <sys/types.h>

#if defined(_WIN32)
	#include <windows.h>
#elif defined(__wasm__)
	/* WASM doesn't support memory mapping */
#else
	#include <unistd.h>
	#ifdef __has_include
		#if __has_include(<sys/syscall.h>)
			#include <sys/syscall.h>
		#endif
	#endif
#endif

/* ================================================================ */
/* Memory protection flags                                          */
/* ================================================================ */

#define PROT_NONE  0x00  /* No access */
#define PROT_READ  0x01  /* Read access */
#define PROT_WRITE 0x02  /* Write access */  
#define PROT_EXEC  0x04  /* Execute access */

/* ================================================================ */
/* Memory mapping flags                                             */
/* ================================================================ */

#define MAP_SHARED     0x01  /* Share mapping with other processes */
#define MAP_PRIVATE    0x02  /* Private copy-on-write mapping */
#define MAP_FIXED      0x10  /* Map at exact address */
#define MAP_ANONYMOUS  0x20  /* Anonymous mapping (no file) */
#define MAP_ANON       MAP_ANONYMOUS  /* BSD compatibility */

/* Linux-specific flags */
#ifdef __linux__
#define MAP_GROWSDOWN  0x0100  /* Stack grows down */
#define MAP_DENYWRITE  0x0800  /* Do not permit writes to file */
#define MAP_EXECUTABLE 0x1000  /* Mark as executable */
#define MAP_LOCKED     0x2000  /* Lock pages in memory */
#define MAP_NORESERVE  0x4000  /* Don't reserve swap space */
#define MAP_POPULATE   0x8000  /* Populate (prefault) page tables */
#define MAP_NONBLOCK   0x10000 /* Don't block on IO */
#define MAP_STACK      0x20000 /* Allocation for stack */
#define MAP_HUGETLB    0x40000 /* Use huge pages */
#endif

/* Error return value */
#define MAP_FAILED ((void*)-1)

/* ================================================================ */
/* Memory synchronization flags                                     */
/* ================================================================ */

#define MS_ASYNC      0x01  /* Asynchronous sync */
#define MS_SYNC       0x04  /* Synchronous sync */
#define MS_INVALIDATE 0x02  /* Invalidate caches */

/* ================================================================ */
/* Memory advisory flags                                            */
/* ================================================================ */

#define MADV_NORMAL     0   /* No special treatment */
#define MADV_RANDOM     1   /* Expect random page access */
#define MADV_SEQUENTIAL 2   /* Expect sequential page access */
#define MADV_WILLNEED   3   /* Will need these pages */
#define MADV_DONTNEED   4   /* Don't need these pages */

/* Linux-specific advisory flags */
#ifdef __linux__
#define MADV_REMOVE     9   /* Remove pages from backing store */
#define MADV_DONTFORK   10  /* Don't inherit across fork */
#define MADV_DOFORK     11  /* Do inherit across fork */
#define MADV_MERGEABLE  12  /* KSM may merge identical pages */
#define MADV_UNMERGEABLE 13 /* KSM may not merge identical pages */
#define MADV_HUGEPAGE   14  /* Use transparent huge pages */
#define MADV_NOHUGEPAGE 15  /* Don't use transparent huge pages */
#endif

/* ================================================================ */
/* Memory locking flags                                             */
/* ================================================================ */

#define MCL_CURRENT 0x01  /* Lock currently mapped pages */
#define MCL_FUTURE  0x02  /* Lock pages mapped in future */

/* ================================================================ */
/* Function implementations                                         */
/* ================================================================ */

#if defined(_WIN32)
/* ================================================================ */
/* Windows implementation using Win32 APIs                         */
/* ================================================================ */

static inline void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
	(void)addr; /* Windows chooses the address */
	
	/* Convert protection flags */
	DWORD win_prot = PAGE_NOACCESS;
	if ((prot & (PROT_READ | PROT_WRITE)) == (PROT_READ | PROT_WRITE)) {
		win_prot = (prot & PROT_EXEC) ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE;
	} else if (prot & PROT_WRITE) {
		win_prot = (prot & PROT_EXEC) ? PAGE_EXECUTE_WRITECOPY : PAGE_WRITECOPY;
	} else if (prot & PROT_READ) {
		win_prot = (prot & PROT_EXEC) ? PAGE_EXECUTE_READ : PAGE_READONLY;
	} else if (prot & PROT_EXEC) {
		win_prot = PAGE_EXECUTE;
	}
	
	HANDLE hFile = INVALID_HANDLE_VALUE;
	HANDLE hMapping;
	void *result;
	
	if (flags & MAP_ANONYMOUS) {
		/* Anonymous mapping */
		hMapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, win_prot, 
		                             (DWORD)(length >> 32), (DWORD)length, NULL);
	} else {
		/* File mapping */
		hFile = (HANDLE)_get_osfhandle(fd);
		if (hFile == INVALID_HANDLE_VALUE) return MAP_FAILED;
		
		hMapping = CreateFileMappingA(hFile, NULL, win_prot,
		                             (DWORD)((offset + length) >> 32), 
		                             (DWORD)(offset + length), NULL);
	}
	
	if (!hMapping) return MAP_FAILED;
	
	DWORD access = 0;
	if (prot & PROT_WRITE) access = FILE_MAP_WRITE;
	else if (prot & PROT_READ) access = FILE_MAP_READ;
	if (prot & PROT_EXEC) access |= FILE_MAP_EXECUTE;
	
	result = MapViewOfFile(hMapping, access, (DWORD)(offset >> 32), 
	                       (DWORD)offset, length);
	
	CloseHandle(hMapping);
	
	return result ? result : MAP_FAILED;
}

static inline int munmap(void *addr, size_t length) {
	(void)length; /* Windows doesn't need length for unmapping */
	return UnmapViewOfFile(addr) ? 0 : -1;
}

static inline int mprotect(void *addr, size_t len, int prot) {
	DWORD old_protect;
	DWORD new_protect = PAGE_NOACCESS;
	
	if ((prot & (PROT_READ | PROT_WRITE)) == (PROT_READ | PROT_WRITE)) {
		new_protect = (prot & PROT_EXEC) ? PAGE_EXECUTE_READWRITE : PAGE_READWRITE;
	} else if (prot & PROT_WRITE) {
		new_protect = (prot & PROT_EXEC) ? PAGE_EXECUTE_WRITECOPY : PAGE_WRITECOPY;
	} else if (prot & PROT_READ) {
		new_protect = (prot & PROT_EXEC) ? PAGE_EXECUTE_READ : PAGE_READONLY;
	} else if (prot & PROT_EXEC) {
		new_protect = PAGE_EXECUTE;
	}
	
	return VirtualProtect(addr, len, new_protect, &old_protect) ? 0 : -1;
}

static inline int msync(void *addr, size_t length, int flags) {
	(void)flags; /* Windows always syncs synchronously */
	return FlushViewOfFile(addr, length) ? 0 : -1;
}

static inline int madvise(void *addr, size_t length, int advice) {
	(void)addr; (void)length; (void)advice;
	return 0; /* No-op on Windows */
}

static inline int mlock(const void *addr, size_t len) {
	return VirtualLock((void*)addr, len) ? 0 : -1;
}

static inline int munlock(const void *addr, size_t len) {
	return VirtualUnlock((void*)addr, len) ? 0 : -1;
}

static inline int mlockall(int flags) {
	(void)flags;
	/* Windows doesn't have equivalent - would need SetProcessWorkingSetSize */
	return -1;
}

static inline int munlockall(void) {
	/* Windows doesn't have equivalent */
	return -1;
}

static inline int mincore(void *addr, size_t length, unsigned char *vec) {
	(void)addr; (void)length; (void)vec;
	/* Windows has VirtualQuery but different interface */
	return -1;
}

#elif defined(__wasm__)
/* ================================================================ */
/* WebAssembly - Memory mapping not supported                      */
/* ================================================================ */

static inline void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
	(void)addr; (void)length; (void)prot; (void)flags; (void)fd; (void)offset;
	return MAP_FAILED; /* Not supported in WASM */
}

static inline int munmap(void *addr, size_t length) {
	(void)addr; (void)length;
	return -1; /* Not supported */
}

static inline int mprotect(void *addr, size_t len, int prot) {
	(void)addr; (void)len; (void)prot;
	return -1; /* Not supported */
}

static inline int msync(void *addr, size_t length, int flags) {
	(void)addr; (void)length; (void)flags;
	return -1; /* Not supported */
}

static inline int madvise(void *addr, size_t length, int advice) {
	(void)addr; (void)length; (void)advice;
	return -1; /* Not supported */
}

static inline int mlock(const void *addr, size_t len) {
	(void)addr; (void)len;
	return -1; /* Not supported */
}

static inline int munlock(const void *addr, size_t len) {
	(void)addr; (void)len;
	return -1; /* Not supported */
}

static inline int mlockall(int flags) {
	(void)flags;
	return -1; /* Not supported */
}

static inline int munlockall(void) {
	return -1; /* Not supported */
}

static inline int mincore(void *addr, size_t length, unsigned char *vec) {
	(void)addr; (void)length; (void)vec;
	return -1; /* Not supported */
}

#else
/* ================================================================ */
/* POSIX/Linux implementation using system calls                   */
/* ================================================================ */

static inline void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
#ifdef SYS_mmap
	long result = syscall(SYS_mmap, (long)addr, (long)length, (long)prot, 
	                      (long)flags, (long)fd, (long)offset);
	return (void*)result;
#else
	/* Fallback - try to use libc if available */
	extern void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
	return mmap(addr, length, prot, flags, fd, offset);
#endif
}

static inline int munmap(void *addr, size_t length) {
#ifdef SYS_munmap
	return (int)syscall(SYS_munmap, (long)addr, (long)length);
#else
	extern int munmap(void *addr, size_t length);
	return munmap(addr, length);
#endif
}

static inline int mprotect(void *addr, size_t len, int prot) {
#ifdef SYS_mprotect
	return (int)syscall(SYS_mprotect, (long)addr, (long)len, (long)prot);
#else
	extern int mprotect(void *addr, size_t len, int prot);
	return mprotect(addr, len, prot);
#endif
}

static inline int msync(void *addr, size_t length, int flags) {
#ifdef SYS_msync
	return (int)syscall(SYS_msync, (long)addr, (long)length, (long)flags);
#else
	extern int msync(void *addr, size_t length, int flags);
	return msync(addr, length, flags);
#endif
}

static inline int madvise(void *addr, size_t length, int advice) {
#ifdef SYS_madvise
	return (int)syscall(SYS_madvise, (long)addr, (long)length, (long)advice);
#else
	/* Best effort - often not critical if this fails */
	(void)addr; (void)length; (void)advice;
	return 0;
#endif
}

static inline int mlock(const void *addr, size_t len) {
#ifdef SYS_mlock
	return (int)syscall(SYS_mlock, (long)addr, (long)len);
#else
	extern int mlock(const void *addr, size_t len);
	return mlock(addr, len);
#endif
}

static inline int munlock(const void *addr, size_t len) {
#ifdef SYS_munlock
	return (int)syscall(SYS_munlock, (long)addr, (long)len);
#else
	extern int munlock(const void *addr, size_t len);
	return munlock(addr, len);
#endif
}

static inline int mlockall(int flags) {
#ifdef SYS_mlockall
	return (int)syscall(SYS_mlockall, (long)flags);
#else
	extern int mlockall(int flags);
	return mlockall(flags);
#endif
}

static inline int munlockall(void) {
#ifdef SYS_munlockall
	return (int)syscall(SYS_munlockall);
#else
	extern int munlockall(void);
	return munlockall();
#endif
}

static inline int mincore(void *addr, size_t length, unsigned char *vec) {
#ifdef SYS_mincore
	return (int)syscall(SYS_mincore, (long)addr, (long)length, (long)vec);
#else
	extern int mincore(void *addr, size_t length, unsigned char *vec);
	return mincore(addr, length, vec);
#endif
}

#endif

#ifdef __cplusplus
}
#endif
#endif /* SYS_MMAN_H */

