/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _FMT_ELF_H
#define _FMT_ELF_H
#pragma once

#include <config.h>
#include <endian.h>
#include <stdint.h>
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

#ifdef __FMT_INIT

/* TLS internals */
#define JACL_TCB_SIZE 128

extern char **environ;

static inline unsigned long __jacl_getauxval(unsigned long type) {
	char **envp = environ;

	if (!envp) return 0;
	while (*envp) envp++;

	unsigned long *auxv = (unsigned long *)(envp + 1);

	while (*auxv) {
		if (auxv[0] == type) return auxv[1];

		auxv += 2;
	}

	return 0;
}

static inline int __jacl_tls_get_info(size_t* size, size_t* align, void** image, size_t* init_size) {
	unsigned long phdr = __jacl_getauxval(AT_PHDR);
	unsigned long phnum = __jacl_getauxval(AT_PHNUM);

	if (!phdr || !phnum) return 0;

	Elf_Phdr* phdrs = (Elf_Phdr*)phdr;

	for (size_t i = 0; i < phnum; i++) {
		if (phdrs[i].p_type == PT_TLS) {
			*size = phdrs[i].p_memsz;
			*align = phdrs[i].p_align ? phdrs[i].p_align : 1;
			*image = (void*)phdrs[i].p_vaddr;
			*init_size = phdrs[i].p_filesz;

			return 1;
		}
	}

	return 0;
}

/* ELF: .init_array section */
typedef void (*init_func_t)(void);
extern init_func_t __init_array_start[] JACL_WEAK;
extern init_func_t __init_array_end[] JACL_WEAK;
#define INIT_START __init_array_start
#define INIT_END __init_array_end

#undef __FMT_INIT
#endif

#ifdef __cplusplus
}
#endif

#endif /* _FMT_ELF_H */
