/**
 * LINUX AUXV FORMAT IMPORTED  //  last updated: 2025-11-28 19:21:55 UTC
 * https://raw.githubusercontent.com/torvalds/linux/v6.17/include/uapi/linux/auxvec.h
 */

/* ELF Auxiliary Vector */
#define ELF_AUXV(X) \
	X(AT_NULL, 0 /* end of vector */) \
	X(AT_IGNORE, 1 /* entry should be ignored */) \
	X(AT_EXECFD, 2 /* file descriptor of program */) \
	X(AT_PHDR, 3 /* program headers for program */) \
	X(AT_PHENT, 4 /* size of program header entry */) \
	X(AT_PHNUM, 5 /* number of program headers */) \
	X(AT_PAGESZ, 6 /* system page size */) \
	X(AT_BASE, 7 /* base address of interpreter */) \
	X(AT_FLAGS, 8 /* flags */) \
	X(AT_ENTRY, 9 /* entry point of program */) \
	X(AT_NOTELF, 10 /* program is not ELF */) \
	X(AT_UID, 11 /* real uid */) \
	X(AT_EUID, 12 /* effective uid */) \
	X(AT_GID, 13 /* real gid */) \
	X(AT_EGID, 14 /* effective gid */) \
	X(AT_PLATFORM, 15 /* string identifying CPU for optimizations */) \
	X(AT_HWCAP, 16 /* arch dependent hints at CPU capabilities */) \
	X(AT_CLKTCK, 17 /* frequency at which times() increments */) \
	X(AT_SECURE, 23 /* secure mode boolean */) \
	X(AT_BASE_PLATFORM, 24 /* string identifying real platform, may) \
	X(AT_RANDOM, 25 /* address of 16 random bytes */) \
	X(AT_HWCAP2, 26 /* extension of AT_HWCAP */) \
	X(AT_RSEQ_FEATURE_SIZE, 27 /* rseq supported feature size */) \
	X(AT_RSEQ_ALIGN, 28 /* rseq allocation alignment */) \
	X(AT_HWCAP3, 29 /* extension of AT_HWCAP */) \
	X(AT_HWCAP4, 30 /* extension of AT_HWCAP */) \
	X(AT_EXECFN, 31 /* filename of program */) \
	X(AT_MINSIGSTKSZ, 51 /* minimal stack size for signal delivery */)

