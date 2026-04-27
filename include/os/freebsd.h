/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __OS_CONFIG
	#undef freebsd
	#undef elf
	#define JACL_OS freebsd
	#define JACL_OS_BSD 1
	#define JACL_OS_FREEBSD 1
	#define JACL_BIN elf
	#define JACL_BIN_ELF
	#define __jacl_os_init      __freebsd_init
	#define __jacl_os_syscall   __freebsd_syscall
	#define __jacl_os_stat      __freebsd_stat
	#define __jacl_os_statnorm  __freebsd_statnorm
#undef __OS_CONFIG
#endif

#ifdef __OS_SYSCALL
	#define __ARCH_SYSCALL
	#include JACL_ARCH_FILE
	#define __freebsd_syscall __jacl_arch_syscall
#undef __OS_SYSCALL
#endif

#ifdef __OS_INIT
	static inline void __freebsd_init(void) {
		/* noop */
	}
#undef __OS_INIT
#endif

#ifdef __OS_STAT
struct __freebsd_stat {
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
	struct timespec st_birthtim;  /* BSD extension */
	off_t     st_size;
	blkcnt_t  st_blocks;
	blksize_t st_blksize;
	uint32_t  st_flags;
	uint32_t  st_gen;
};

static inline void __freebsd_statnorm(const struct __freebsd_stat *kbuf, struct stat *statbuf) {
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
