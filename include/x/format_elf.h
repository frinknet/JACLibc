/**
 * LINUX ELF FORMAT IMPORTED  //  last updated: 2025-11-28 19:21:52 UTC
 * https://raw.githubusercontent.com/torvalds/linux/v6.17/include/uapi/linux/elf.h
 */

/* ELF Identification */
#define ELF_IDENT(X) \
	X(EI_NIDENT, 16) \
	X(EI_MAG0, 0 /* e_ident[] indexes */) \
	X(EI_MAG1, 1) \
	X(EI_MAG2, 2) \
	X(EI_MAG3, 3) \
	X(EI_CLASS, 4) \
	X(EI_DATA, 5) \
	X(EI_VERSION, 6) \
	X(EI_OSABI, 7) \
	X(EI_PAD, 8)

/* ELF Constants */
#define ELF_CONSTANTS(X) \
	X(ELFMAG0, 0x7f /* EI_MAG */) \
	X(ELFMAG1, 'E') \
	X(ELFMAG2, 'L') \
	X(ELFMAG3, 'F') \
	X(ELFCLASSNONE, 0 /* EI_CLASS */) \
	X(ELFCLASS32, 1) \
	X(ELFCLASS64, 2) \
	X(ELFCLASSNUM, 3) \
	X(ELFDATANONE, 0 /* e_ident[EI_DATA] */) \
	X(ELFDATA2LSB, 1) \
	X(ELFDATA2MSB, 2) \
	X(EV_NONE, 0 /* e_version, EI_VERSION */) \
	X(EV_CURRENT, 1) \
	X(EV_NUM, 2)

/* ELF File Types */
#define ELF_TYPES(X) \
	X(ET_NONE, 0) \
	X(ET_REL, 1) \
	X(ET_EXEC, 2) \
	X(ET_DYN, 3) \
	X(ET_CORE, 4) \
	X(ET_LOPROC, 0xff00) \
	X(ET_HIPROC, 0xffff)

/* ELF Program Header Types */
#define ELF_PHDRS(X) \
	X(PT_NULL, 0) \
	X(PT_LOAD, 1) \
	X(PT_DYNAMIC, 2) \
	X(PT_INTERP, 3) \
	X(PT_NOTE, 4) \
	X(PT_SHLIB, 5) \
	X(PT_PHDR, 6) \
	X(PT_TLS, 7 /* Thread local storage segment */) \
	X(PT_LOOS, 0x60000000 /* OS-specific */) \
	X(PT_HIOS, 0x6fffffff /* OS-specific */) \
	X(PT_LOPROC, 0x70000000) \
	X(PT_HIPROC, 0x7fffffff) \
	X(PT_GNU_EH_FRAME, (PT_LOOS + 0x474e550)) \
	X(PT_GNU_STACK, (PT_LOOS + 0x474e551)) \
	X(PT_GNU_RELRO, (PT_LOOS + 0x474e552)) \
	X(PT_GNU_PROPERTY, (PT_LOOS + 0x474e553)) \
	X(PT_AARCH64_MEMTAG_MTE, (PT_LOPROC + 0x2))

/* ELF Program Header Flags */
#define ELF_PHDR_FLAGS(X) \
	X(PF_R, 0x4) \
	X(PF_W, 0x2) \
	X(PF_X, 0x1)

/* ELF Section Types */
#define ELF_SECTIONS(X) \
	X(SHT_NULL, 0) \
	X(SHT_PROGBITS, 1) \
	X(SHT_SYMTAB, 2) \
	X(SHT_STRTAB, 3) \
	X(SHT_RELA, 4) \
	X(SHT_HASH, 5) \
	X(SHT_DYNAMIC, 6) \
	X(SHT_NOTE, 7) \
	X(SHT_NOBITS, 8) \
	X(SHT_REL, 9) \
	X(SHT_SHLIB, 10) \
	X(SHT_DYNSYM, 11) \
	X(SHT_NUM, 12) \
	X(SHT_LOPROC, 0x70000000) \
	X(SHT_HIPROC, 0x7fffffff) \
	X(SHT_LOUSER, 0x80000000) \
	X(SHT_HIUSER, 0xffffffff)

/* ELF Section Flags */
#define ELF_SECTION_FLAGS(X) \
	X(SHF_WRITE, 0x1) \
	X(SHF_ALLOC, 0x2) \
	X(SHF_EXECINSTR, 0x4) \
	X(SHF_MERGE, 0x10) \
	X(SHF_STRINGS, 0x20) \
	X(SHF_INFO_LINK, 0x40) \
	X(SHF_LINK_ORDER, 0x80) \
	X(SHF_OS_NONCONFORMING, 0x100) \
	X(SHF_GROUP, 0x200) \
	X(SHF_TLS, 0x400) \
	X(SHF_RELA_LIVEPATCH, 0x00100000) \
	X(SHF_RO_AFTER_INIT, 0x00200000) \
	X(SHF_ORDERED, 0x04000000) \
	X(SHF_EXCLUDE, 0x08000000) \
	X(SHF_MASKOS, 0x0ff00000) \
	X(SHF_MASKPROC, 0xf0000000)

/* ELF Symbol Info */
#define ELF_SYMBOLS(X) \
	X(STB_LOCAL, 0) \
	X(STB_GLOBAL, 1) \
	X(STB_WEAK, 2) \
	X(STT_NOTYPE, 0) \
	X(STT_OBJECT, 1) \
	X(STT_FUNC, 2) \
	X(STT_SECTION, 3) \
	X(STT_FILE, 4) \
	X(STT_COMMON, 5) \
	X(STT_TLS, 6)

/* ELF Dynamic Tags */
#define ELF_DYNAMIC(X) \
	X(DT_NULL, 0) \
	X(DT_NEEDED, 1) \
	X(DT_PLTRELSZ, 2) \
	X(DT_PLTGOT, 3) \
	X(DT_HASH, 4) \
	X(DT_STRTAB, 5) \
	X(DT_SYMTAB, 6) \
	X(DT_RELA, 7) \
	X(DT_RELASZ, 8) \
	X(DT_RELAENT, 9) \
	X(DT_STRSZ, 10) \
	X(DT_SYMENT, 11) \
	X(DT_INIT, 12) \
	X(DT_FINI, 13) \
	X(DT_SONAME, 14) \
	X(DT_RPATH, 15) \
	X(DT_SYMBOLIC, 16) \
	X(DT_REL, 17) \
	X(DT_RELSZ, 18) \
	X(DT_RELENT, 19) \
	X(DT_PLTREL, 20) \
	X(DT_DEBUG, 21) \
	X(DT_TEXTREL, 22) \
	X(DT_JMPREL, 23) \
	X(DT_ENCODING, 32) \
	X(DT_LOOS, 0x6000000d) \
	X(DT_HIOS, 0x6ffff000) \
	X(DT_VALRNGLO, 0x6ffffd00) \
	X(DT_VALRNGHI, 0x6ffffdff) \
	X(DT_ADDRRNGLO, 0x6ffffe00) \
	X(DT_GNU_HASH, 0x6ffffef5) \
	X(DT_ADDRRNGHI, 0x6ffffeff) \
	X(DT_VERSYM, 0x6ffffff0) \
	X(DT_RELACOUNT, 0x6ffffff9) \
	X(DT_RELCOUNT, 0x6ffffffa) \
	X(DT_FLAGS_1, 0x6ffffffb) \
	X(DT_VERDEF, 0x6ffffffc) \
	X(DT_VERDEFNUM, 0x6ffffffd) \
	X(DT_VERNEED, 0x6ffffffe) \
	X(DT_VERNEEDNUM, 0x6fffffff) \
	X(DT_LOPROC, 0x70000000) \
	X(DT_HIPROC, 0x7fffffff)

