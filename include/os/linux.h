/* (c) 2025 FRINKnet & Friends – MIT licence */

#ifdef __OS_CONFIG
	#undef elf
	#undef linux
	#define JACL_OS linux
	#define JACL_OS_LINUX 1
	#define JACL_BIN elf
	#define JACL_BIN_ELF 1
	#define __jacl_os_init      __linux_init
	#define __jacl_os_syscall   __linux_syscall
	#define __jacl_os_stat      __linux_stat
	#define __jacl_os_statnorm  __linux_statnorm
	#if defined(JACL_ARCH_X64) || defined(JACL_ARCH_X86)
		#undef __jacl_arch_tls_set
		#undef __jacl_arch_tls_get
		#define __jacl_arch_tls_set  __os_set_fs_register
		#define __jacl_arch_tls_get  __os_get_fs_register
	#endif
#undef __OS_CONFIG
#endif

#ifdef __OS_SYSCALL
	#define __ARCH_SYSCALL
	#include JACL_ARCH_FILE
	#define __linux_syscall __jacl_arch_syscall
#undef __OS_SYSCALL
#endif

#ifdef __OS_INIT
	/* Pull in format helpers for TLS metadata */
	#define __BIN_INIT
	#include JACL_BIN_FILE

	#if defined(JACL_ARCH_X64) || defined(JACL_ARCH_X86)
		#define ARCH_SET_FS 0x1002
		#define ARCH_GET_FS 0x1003

		static inline void __os_set_fs_register(void* addr) {
			__linux_syscall(SYS_arch_prctl, ARCH_SET_FS, (long)addr, 0, 0, 0, 0);
		}
		static inline void* __os_get_fs_register(void) {
			return __linux_syscall(SYS_arch_prctl, ARCH_GET_FS, 0, 0, 0, 0, 0);
		}
	#else
		/* Pull in arch TLS register ops */
		#define __ARCH_TLS
		#include JACL_ARCH_FILE
	#endif

	static inline void __linux_init(void) {
		size_t tls_size, tls_align, tls_init_size;
		void* tls_image;

		/* 1. Get TLS metadata from format (PT_TLS) */
		if (!__jacl_tls_get_info(&tls_size, &tls_align, &tls_image, &tls_init_size)) return;
		if (tls_size == 0) return;

		/* 2. Allocate TLS+TCB (Linux-specific) */
		size_t total = tls_size + tls_align + JACL_TCB_SIZE;
		void* mem = mmap(NULL, total, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

		if (mem == MAP_FAILED) return;

		/* 3. Compute aligned addresses */
		uintptr_t tls_base = JACL_ALIGN_UP((uintptr_t)mem, tls_align);
		uintptr_t tcb_addr = tls_base + JACL_ALIGN_UP(tls_size, tls_align);

		/* 4. Initialize TLS data */
		if (tls_image && tls_init_size > 0) memcpy((void*)tls_base, tls_image, tls_init_size);
		if (tls_size > tls_init_size) memset((void*)(tls_base + tls_init_size), 0, tls_size - tls_init_size);

		/* 5. Set TCB self-pointer */
		*(void**)tcb_addr = (void*)tcb_addr;

		/* 6. Install TLS pointer */
		__jacl_arch_tls_set((void*)tcb_addr);
	}
#undef __OS_INIT
#endif

#ifdef __OS_STAT
	/* Linux x86-64 kernel stat ABI */
	#if JACL_ARCH_X64
		struct __linux_stat {
			unsigned long st_dev;
			unsigned long st_ino;
			unsigned long st_nlink;
			unsigned int  st_mode;
			unsigned int  st_uid;
			unsigned int  st_gid;
			unsigned int  __pad0;
			unsigned long st_rdev;
			long          st_size;
			long          st_blksize;
			long          st_blocks;
			unsigned long st_atime;
			unsigned long st_atime_nsec;
			unsigned long st_mtime;
			unsigned long st_mtime_nsec;
			unsigned long st_ctime;
			unsigned long st_ctime_nsec;
			long          __unused[3];
		};
	#elif JACL_ARCH_ARM64
		/* ARM64 uses same layout as x64 */
		struct __linux_stat {  /* Fixed: was __kernel_stat */
			unsigned long st_dev;
			unsigned long st_ino;
			unsigned long st_nlink;
			unsigned int  st_mode;
			unsigned int  st_uid;
			unsigned int  st_gid;
			unsigned int  __pad0;
			unsigned long st_rdev;
			long          st_size;
			long          st_blksize;
			long          st_blocks;
			unsigned long st_atime;
			unsigned long st_atime_nsec;
			unsigned long st_mtime;
			unsigned long st_mtime_nsec;
			unsigned long st_ctime;
			unsigned long st_ctime_nsec;
			long          __unused[3];
		};
	#endif

	/* Normalize kernel struct to POSIX */
	static inline void __linux_statnorm(struct __linux_stat *ksrc, struct stat *dst) {
		dst->st_dev = ksrc->st_dev;
		dst->st_ino = ksrc->st_ino;
		dst->st_nlink = ksrc->st_nlink;
		dst->st_mode = ksrc->st_mode;
		dst->st_uid = ksrc->st_uid;
		dst->st_gid = ksrc->st_gid;
		dst->st_rdev = ksrc->st_rdev;
		dst->st_size = ksrc->st_size;
		dst->st_blksize = ksrc->st_blksize;
		dst->st_blocks = ksrc->st_blocks;

		/* Convert kernel sec/nsec to POSIX timespec */
		dst->st_atim.tv_sec = ksrc->st_atime;
		dst->st_atim.tv_nsec = ksrc->st_atime_nsec;
		dst->st_mtim.tv_sec = ksrc->st_mtime;
		dst->st_mtim.tv_nsec = ksrc->st_mtime_nsec;
		dst->st_ctim.tv_sec = ksrc->st_ctime;
		dst->st_ctim.tv_nsec = ksrc->st_ctime_nsec;
	}
#undef __OS_STAT
#endif
