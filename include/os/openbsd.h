/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __OS_CONFIG
	#undef openbsd
	#undef elf
	#define JACL_OS openbsd
	#define JACL_OS_BSD 1
	#define JACL_OS_OPENBSD 1
	#define JACL_FMT elf
	#define JACL_FMT_ELF
	#define __jacl_os_init      __openbsd_init
	#define __jacl_os_syscall   __openbsd_syscall
	#define __jacl_os_stat      __openbsd_stat
	#define __jacl_os_statnorm  __openbsd_statnorm
#undef __OS_CONFIG
#endif

#ifdef __OS_SYSCALL
	struct __openbsd_libc {
		#define X(SYS, num, fn, rettype, params, ...) rettype (*fn##_)params;
		#include JACL_X_SYSCALL
		#undef X
	};

	extern struct __openbsd_libc _openbsd;

	/* syscall backend */
	static inline long __openbsd_syscall(long n, long a0, long a1, long a2, long a3, long a4, long a5) {
		switch (n) {
		#define X(SYS, num, fn, rettype, params, ...) \
			case SYS: return (long)_openbsd.fn##_(__VA_ARGS__);
		#include JACL_X_SYSCALL
		#undef X
		default:
			errno = ENOSYS;
			return -1;
		}
	}
#undef __OS_SYSCALL
#endif

#ifdef __OS_INIT
	/* dispatch table */
	struct __openbsd_libc {
	#define X(SYS, num, fn, rettype, params, ...) \
		rettype (*fn##_)params;
	#include JACL_X_SYSCALL
	#undef X
	};

	struct __openbsd_libc _openbsd;

	/* init */
	static inline void __openbsd_init(void) {
		void *h = dlopen("libc.so", RTLD_LAZY);
		if (!h) h = dlopen(NULL, RTLD_LAZY);

	#define X(SYS, num, fn, rettype, params, ...) \
		_openbsd.fn##_ = (rettype(*)params)dlsym(h, #fn);
	#include JACL_X_SYSCALL
	#undef X
	}

#undef __OS_INIT
#endif

#ifdef __OS_STAT
/* OpenBSD kernel stat structure (POSIX-compliant naming) */
struct __openbsd_stat {
	dev_t     st_dev;
	ino_t     st_ino;
	mode_t    st_mode;
	nlink_t   st_nlink;
	uid_t     st_uid;
	gid_t     st_gid;
	dev_t     st_rdev;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	off_t     st_size;
	blkcnt_t  st_blocks;
	blksize_t st_blksize;
	uint32_t  st_flags;
	uint32_t  st_gen;
};

/* Convert OpenBSD kernel stat to POSIX stat */
static inline void __openbsd_statnorm(const struct __openbsd_stat *kbuf, struct stat *statbuf) {
	statbuf->st_dev     = kbuf->st_dev;
	statbuf->st_ino     = kbuf->st_ino;
	statbuf->st_mode    = kbuf->st_mode;
	statbuf->st_nlink   = kbuf->st_nlink;
	statbuf->st_uid     = kbuf->st_uid;
	statbuf->st_gid     = kbuf->st_gid;
	statbuf->st_rdev    = kbuf->st_rdev;
	statbuf->st_size    = kbuf->st_size;
	statbuf->st_blocks  = kbuf->st_blocks;
	statbuf->st_blksize = kbuf->st_blksize;
	statbuf->st_atim    = kbuf->st_atim;
	statbuf->st_mtim    = kbuf->st_mtim;
	statbuf->st_ctim    = kbuf->st_ctim;
}
#undef __OS_STAT
#endif
