/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _BIN_ELF_H
#define _BIN_ELF_H
#pragma once

#include <config.h>
#include <endian.h>
#include <stdint.h>
#include <elf.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __BIN_INIT

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

#undef __BIN_INIT
#endif

#ifdef __cplusplus
}
#endif

#endif /* _BIN_ELF_H */
