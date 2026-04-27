/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __OS_CONFIG
	#undef darwin
	#undef macho
	#define JACL_OS darwin
	#define JACL_OS_DARWIN 1
	#define JACL_BIN macho
	#define JACL_BIN_MACHO
	#define __jacl_os_init      __darwin_init
	#define __jacl_os_syscall   __darwin_syscall
	#define __jacl_os_stat      __darwin_stat
	#define __jacl_os_statnorm  __darwin_statnorm
#undef __OS_CONFIG
#endif

#ifdef __OS_SYSCALL
	#define __ARCH_SYSCALL
	#include JACL_ARCH_FILE
	#define __darwin_syscall __jacl_arch_syscall
#undef __OS_SYSCALL
#endif

#ifdef __OS_INIT
static inline void __darwin_init(void) {
	/* noop */
}
#undef __OS_INIT
#endif

#ifdef __OS_STAT
/* Darwin kernel stat structure (matches macOS kernel ABI) */
struct __darwin_stat {
	dev_t     st_dev;
	ino_t     st_ino;
	mode_t    st_mode;
	nlink_t   st_nlink;
	uid_t     st_uid;
	gid_t     st_gid;
	dev_t     st_rdev;

	#if !defined(__DARWIN_64_BIT_INO_T) || __DARWIN_64_BIT_INO_T == 0
		/* 32-bit inode version */
		struct timespec st_atimespec;
		struct timespec st_mtimespec;
		struct timespec st_ctimespec;
	#else
		/* 64-bit inode version (modern macOS) */
		struct timespec st_atimespec;
		struct timespec st_mtimespec;
		struct timespec st_ctimespec;
		struct timespec st_birthtimespec;  /* Darwin extension */
	#endif

	off_t     st_size;
	blkcnt_t  st_blocks;
	blksize_t st_blksize;
	uint32_t  st_flags;
	uint32_t  st_gen;
};

/* Convert Darwin kernel stat to POSIX stat */
static inline void __darwin_statnorm(const struct __darwin_stat *kbuf, struct stat *statbuf) {
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

	/* Darwin uses *timespec naming, POSIX uses *tim */
	statbuf->st_atim = kbuf->st_atimespec;
	statbuf->st_mtim = kbuf->st_mtimespec;
	statbuf->st_ctim = kbuf->st_ctimespec;
}
#undef __OS_STAT
#endif
