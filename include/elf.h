/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _ELF_H
#define _ELF_H

#pragma once

#include <config.h>
#include <stdint.h>
#include <endian.h>
#include <x/format_elf.h>
#include <x/format_em.h>
#include <x/format_auxv.h>

#define ELF_DATA(h) ((h).e_ident[EI_DATA])
#define ELF_RD16(h, f) (ELF_DATA(h) == ELFDATA2LSB ? le16toh((h).f) : be16toh((h).f))
#define ELF_RD32(h, f) (ELF_DATA(h) == ELFDATA2LSB ? le32toh((h).f) : be32toh((h).f))
#define ELF_RD64(h, f) (ELF_DATA(h) == ELFDATA2LSB ? le64toh((h).f) : be64toh((h).f))

#ifdef __cplusplus
extern "C" {
#endif

/* Import ELF constants from Torvalds */
#define X_ENUM(name, val) name = (val),
	enum { ELF_IDENT(X_ENUM) };
	enum { ELF_CONSTANTS(X_ENUM) };
	enum { ELF_TYPES(X_ENUM) };
	enum { ELF_PHDRS(X_ENUM) };
	enum { ELF_PHDR_FLAGS(X_ENUM) };
	enum { ELF_SECTIONS(X_ENUM) };
	enum { ELF_SECTION_FLAGS(X_ENUM) };
	enum { ELF_SYMBOLS(X_ENUM) };
	enum { ELF_DYNAMIC(X_ENUM) };
	enum { ELF_MACHINES(X_ENUM) };
	enum { ELF_AUXV(X_ENUM) };
#undef X_ENUM

/* ELF32 types */
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Word;
typedef int32_t  Elf32_Sword;
typedef uint16_t Elf32_Half;

/* ELF64 types */
typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint64_t Elf64_Xword;
typedef int64_t  Elf64_Sxword;
typedef uint32_t Elf64_Word;
typedef int32_t  Elf64_Sword;
typedef uint16_t Elf64_Half;

/* ELF32 structures */
JACL_LAYOUT
typedef struct {
	unsigned char e_ident[EI_NIDENT];
	Elf32_Half    e_type;
	Elf32_Half    e_machine;
	Elf32_Word    e_version;
	Elf32_Addr    e_entry;
	Elf32_Off     e_phoff;
	Elf32_Off     e_shoff;
	Elf32_Word    e_flags;
	Elf32_Half    e_ehsize;
	Elf32_Half    e_phentsize;
	Elf32_Half    e_phnum;
	Elf32_Half    e_shentsize;
	Elf32_Half    e_shnum;
	Elf32_Half    e_shstrndx;
} JACL_PACK Elf32_Ehdr;

JACL_LAYOUT
typedef struct {
	Elf32_Word p_type;
	Elf32_Off  p_offset;
	Elf32_Addr p_vaddr;
	Elf32_Addr p_paddr;
	Elf32_Word p_filesz;
	Elf32_Word p_memsz;
	Elf32_Word p_flags;
	Elf32_Word p_align;
} JACL_PACK Elf32_Phdr;

JACL_LAYOUT
typedef struct {
	Elf32_Word sh_name;
	Elf32_Word sh_type;
	Elf32_Word sh_flags;
	Elf32_Addr sh_addr;
	Elf32_Off  sh_offset;
	Elf32_Word sh_size;
	Elf32_Word sh_link;
	Elf32_Word sh_info;
	Elf32_Word sh_addralign;
	Elf32_Word sh_entsize;
} JACL_PACK Elf32_Shdr;

JACL_LAYOUT
typedef struct {
	Elf32_Word    st_name;
	Elf32_Addr    st_value;
	Elf32_Word    st_size;
	unsigned char st_info;
	unsigned char st_other;
	Elf32_Half    st_shndx;
} JACL_PACK Elf32_Sym;

JACL_LAYOUT
typedef struct {
	Elf32_Sword d_tag;
	union {
		Elf32_Word d_val;
		Elf32_Addr d_ptr;
	} d_un;
} JACL_PACK Elf32_Dyn;

JACL_LAYOUT
typedef struct {
	Elf32_Addr r_offset;
	Elf32_Word r_info;
} JACL_PACK Elf32_Rel;

JACL_LAYOUT
typedef struct {
	Elf32_Addr  r_offset;
	Elf32_Word  r_info;
	Elf32_Sword r_addend;
} JACL_PACK Elf32_Rela;

/* ELF64 structures */
JACL_LAYOUT
typedef struct {
	unsigned char e_ident[EI_NIDENT];
	Elf64_Half    e_type;
	Elf64_Half    e_machine;
	Elf64_Word    e_version;
	Elf64_Addr    e_entry;
	Elf64_Off     e_phoff;
	Elf64_Off     e_shoff;
	Elf64_Word    e_flags;
	Elf64_Half    e_ehsize;
	Elf64_Half    e_phentsize;
	Elf64_Half    e_phnum;
	Elf64_Half    e_shentsize;
	Elf64_Half    e_shnum;
	Elf64_Half    e_shstrndx;
} JACL_PACK Elf64_Ehdr;

JACL_LAYOUT
typedef struct {
	Elf64_Word  p_type;
	Elf64_Word  p_flags;
	Elf64_Off   p_offset;
	Elf64_Addr  p_vaddr;
	Elf64_Addr  p_paddr;
	Elf64_Xword p_filesz;
	Elf64_Xword p_memsz;
	Elf64_Xword p_align;
} JACL_PACK Elf64_Phdr;

JACL_LAYOUT
typedef struct {
	Elf64_Word  sh_name;
	Elf64_Word  sh_type;
	Elf64_Xword sh_flags;
	Elf64_Addr  sh_addr;
	Elf64_Off   sh_offset;
	Elf64_Xword sh_size;
	Elf64_Word  sh_link;
	Elf64_Word  sh_info;
	Elf64_Xword sh_addralign;
	Elf64_Xword sh_entsize;
} JACL_PACK Elf64_Shdr;

JACL_LAYOUT
typedef struct {
	Elf64_Word    st_name;
	unsigned char st_info;
	unsigned char st_other;
	Elf64_Half    st_shndx;
	Elf64_Addr    st_value;
	Elf64_Xword   st_size;
} JACL_PACK Elf64_Sym;

JACL_LAYOUT
typedef struct {
	Elf64_Sxword d_tag;
	union {
		Elf64_Xword d_val;
		Elf64_Addr  d_ptr;
	} d_un;
} JACL_PACK Elf64_Dyn;

JACL_LAYOUT
typedef struct {
	Elf64_Addr  r_offset;
	Elf64_Xword r_info;
} JACL_PACK Elf64_Rel;

JACL_LAYOUT
typedef struct {
	Elf64_Addr   r_offset;
	Elf64_Xword  r_info;
	Elf64_Sxword r_addend;
} JACL_PACK Elf64_Rela;

/* Bit-width dependent typedefs */
#if JACL_BITS == 64
	typedef Elf64_Ehdr Elf_Ehdr;
	typedef Elf64_Phdr Elf_Phdr;
	typedef Elf64_Shdr Elf_Shdr;
	typedef Elf64_Sym  Elf_Sym;
	typedef Elf64_Dyn  Elf_Dyn;
	typedef Elf64_Rel  Elf_Rel;
	typedef Elf64_Rela Elf_Rela;
	typedef Elf64_Addr Elf_Addr;
	typedef Elf64_Off  Elf_Off;
#else
	typedef Elf32_Ehdr Elf_Ehdr;
	typedef Elf32_Phdr Elf_Phdr;
	typedef Elf32_Shdr Elf_Shdr;
	typedef Elf32_Sym  Elf_Sym;
	typedef Elf32_Dyn  Elf_Dyn;
	typedef Elf32_Rel  Elf_Rel;
	typedef Elf32_Rela Elf_Rela;
	typedef Elf32_Addr Elf_Addr;
	typedef Elf32_Off  Elf_Off;
#endif

/* The magic string */
#ifndef ELFMAG
#define ELFMAG "\177ELF"
#endif

/* Section Header Special Indices */
#ifndef SHN_UNDEF
#define SHN_UNDEF     0
#define SHN_LORESERVE 0xff00
#define SHN_LOPROC    0xff00
#define SHN_HIPROC    0xff1f
#define SHN_LIVEPATCH 0xff20
#define SHN_ABS       0xfff1
#define SHN_COMMON    0xfff2
#define SHN_HIRESERVE 0xffff
#endif

/* Symbol Binding/Type/Visibility Macros */
#ifndef ELF32_ST_BIND
#define ELF32_ST_BIND(val)        (((unsigned char)(val)) >> 4)
#define ELF32_ST_TYPE(val)        ((val) & 0xf)
#define ELF32_ST_INFO(bind, type) (((bind) << 4) + ((type) & 0xf))
#define ELF32_ST_VISIBILITY(o)    ((o) & 0x3)

#define ELF64_ST_BIND(val)        ELF32_ST_BIND(val)
#define ELF64_ST_TYPE(val)        ELF32_ST_TYPE(val)
#define ELF64_ST_INFO(bind, type) ELF32_ST_INFO(bind, type)
#define ELF64_ST_VISIBILITY(o)    ELF32_ST_VISIBILITY(o)
#endif

/* Relocation Macros */
#ifndef ELF32_R_SYM
#define ELF32_R_SYM(info)         ((info) >> 8)
#define ELF32_R_TYPE(info)        ((unsigned char)(info))
#define ELF32_R_INFO(sym, type)   (((sym) << 8) + (unsigned char)(type))

#define ELF64_R_SYM(info)         ((info) >> 32)
#define ELF64_R_TYPE(info)        ((info) & 0xffffffffL)
#define ELF64_R_INFO(sym, type)   (((Elf64_Xword)(sym) << 32) + (type))
#endif

/* Dynamic Entry Macros */
#ifndef ELF32_D_TAG
#define ELF32_D_TAG(val)          ((val).d_tag)
#define ELF32_D_VAL(val)          ((val).d_un.d_val)
#define ELF32_D_PTR(val)          ((val).d_un.d_ptr)

#define ELF64_D_TAG(val)          ((val).d_tag)
#define ELF64_D_VAL(val)          ((val).d_un.d_val)
#define ELF64_D_PTR(val)          ((val).d_un.d_ptr)
#endif

/* Program Header Flags (if not defined in enum) */
#ifndef PF_X
#define PF_X 0x1
#define PF_W 0x2
#define PF_R 0x4
#endif

/* Section Header Flags (if not defined in enum) */
#ifndef SHF_WRITE
#define SHF_WRITE            0x1
#define SHF_ALLOC            0x2
#define SHF_EXECINSTR        0x4
#define SHF_MERGE            0x10
#define SHF_STRINGS          0x20
#define SHF_INFO_LINK        0x40
#define SHF_LINK_ORDER       0x80
#define SHF_OS_NONCONFORMING 0x100
#define SHF_GROUP            0x200
#define SHF_TLS              0x400
#endif

/* Symbol Bindings (if not defined in enum) */
#ifndef STB_LOCAL
#define STB_LOCAL   0
#define STB_GLOBAL  1
#define STB_WEAK    2
#define STB_LOOS    10
#define STB_HIOS    12
#define STB_LOPROC  13
#define STB_HIPROC  15
#endif

/* Symbol Types (if not defined in enum) */
#ifndef STT_NOTYPE
#define STT_NOTYPE   0
#define STT_OBJECT   1
#define STT_FUNC     2
#define STT_SECTION  3
#define STT_FILE     4
#define STT_COMMON   5
#define STT_TLS      6
#define STT_LOOS     10
#define STT_HIOS     12
#define STT_LOPROC   13
#define STT_HIPROC   15
#endif

/* Symbol Visibility */
#ifndef STV_DEFAULT
#define STV_DEFAULT   0
#define STV_INTERNAL  1
#define STV_HIDDEN    2
#define STV_PROTECTED 3
#endif

/* Dynamic Tags (Common ones if missing) */
#ifndef DT_NULL
#define DT_NULL           0
#define DT_NEEDED         1
#define DT_PLTRELSZ       2
#define DT_PLTGOT         3
#define DT_HASH           4
#define DT_STRTAB         5
#define DT_SYMTAB         6
#define DT_RELA           7
#define DT_RELASZ         8
#define DT_RELAENT        9
#define DT_STRSZ          10
#define DT_SYMENT         11
#define DT_INIT           12
#define DT_FINI           13
#define DT_SONAME         14
#define DT_RPATH          15
#define DT_SYMBOLIC       16
#define DT_REL            17
#define DT_RELSZ          18
#define DT_RELENT         19
#define DT_PLTREL         20
#define DT_DEBUG          21
#define DT_TEXTREL        22
#define DT_JMPREL         23
#define DT_BIND_NOW       24
#define DT_INIT_ARRAY     25
#define DT_FINI_ARRAY     26
#define DT_INIT_ARRAYSZ   27
#define DT_FINI_ARRAYSZ   28
#define DT_RUNPATH        29
#define DT_FLAGS          30
#define DT_ENCODING       32
#endif

/* ELF Version */
#ifndef EV_CURRENT
#define EV_NONE    0
#define EV_CURRENT 1
#define EV_NUM     2
#endif

#ifdef __cplusplus
}
#endif

#endif /* _ELF_H */
