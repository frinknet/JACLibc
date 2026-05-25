/* (c) 2025 FRINKnet & Friends – MIT licence */

#include <endian.h>
#include <elf.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __BIN_INIT
#ifndef __ELF_INIT
#define __ELF_INIT

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

#endif /* __ELF_INIT */
#undef __BIN_INIT
#endif /* __BIN_INIT */

#ifdef __BIN_LINK
	typedef struct {
		ElfW(Sym) *symtab;
		const char *strtab;
		/* SYSV Hash */
		ElfW(Word) *sysv_buckets;
		ElfW(Word) *sysv_chains;
		ElfW(Word) sysv_nbucket;
		/* GNU Hash */
		ElfW(Word) *gnu_buckets;
		ElfW(Addr) *gnu_bloom;
		ElfW(Word) gnu_maskwords;
		ElfW(Word) gnu_shift2;
		ElfW(Word) gnu_symndx;
		ElfW(Word) gnu_nbucket;
	} __dl_elf_meta_t;
	static inline ElfW(Word) __dl_hash_sysv(const char *name) {
		ElfW(Word) h = 0, g;
		for (const char *p = name; *p; p++) {
			h = (h << 4) + (unsigned char)*p;
			if ((g = h & 0xF0000000)) h ^= g >> 24;
			h &= ~g;
		}
		return h;
	}

	static inline ElfW(Word) __dl_hash_gnu(const char *name) {
		ElfW(Word) h = 5381;
		for (const char *p = name; *p; p++) h = (h << 5) + h + *p;
		return h;
	}

	static inline void *__dl_find_elf(jacl_dl_handle_t *h, const char *sym) {
		__dl_elf_meta_t *m = (__dl_elf_meta_t *)h->meta;
		if (!m || !m->symtab || !m->strtab) return NULL;
	
		/* 1. GNU Hash */
		if (m->gnu_buckets) {
			ElfW(Word) gh = __dl_hash_gnu(sym);
			ElfW(Word) h1 = gh % m->gnu_nbucket;
			ElfW(Word) h2 = gh >> m->gnu_shift2;
			const uint64_t *bloom = (const uint64_t *)m->gnu_bloom;
			ElfW(Word) bits = sizeof(ElfW(Addr)) * 8;
			ElfW(Word) word = bloom[(gh / bits) % m->gnu_maskwords];
	
			if (word & (1ULL << (gh % bits)) & (1ULL << (h2 % bits))) {
				ElfW(Word) idx = m->gnu_buckets[h1];
				if (idx >= m->gnu_symndx) {
					const ElfW(Word) *chain = m->gnu_buckets + m->gnu_nbucket;
					while (1) {
						ElfW(Sym) *s = &m->symtab[idx];
						if (((__dl_hash_gnu(m->strtab + s->st_name) ^ gh) & ~1U) == 0 &&
							strcmp(m->strtab + s->st_name, sym) == 0)
							return (char *)h->base + s->st_value;
						if (chain[idx - m->gnu_symndx] & 1) break;
						idx++;
					}
				}
			}
		}
	
		/* 2. SYSV Hash */
		if (m->sysv_buckets) {
			ElfW(Word) sh = __dl_hash_sysv(sym);
			for (ElfW(Word) idx = m->sysv_buckets[sh % m->sysv_nbucket]; idx != 0; idx = m->sysv_chains[idx]) {
				ElfW(Sym) *s = &m->symtab[idx];
				if (s->st_name && strcmp(m->strtab + s->st_name, sym) == 0)
					return (char *)h->base + s->st_value;
			}
		}
	
		/* 3. Bounded Linear Fallback */
		size_t max = h->size / sizeof(ElfW(Sym));
		for (size_t i = 1; i < max; i++) {
			ElfW(Sym) *s = &m->symtab[i];
			if (s->st_name && strcmp(m->strtab + s->st_name, sym) == 0)
				return (char *)h->base + s->st_value;
		}
		return NULL;
	}

	static inline int __dl_load_elf(jacl_dl_handle_t *h, void *file_map, struct stat *st) {
		(void)st;
		ElfW(Ehdr) *eh = (ElfW(Ehdr) *)file_map;
		if (memcmp(eh->e_ident, ELFMAG, SELFMAG) != 0) return 0;
		if (eh->e_type != ET_EXEC && eh->e_type != ET_DYN) return 0;

		ElfW(Phdr) *ph = (ElfW(Phdr) *)((char *)file_map + eh->e_phoff);
		ElfW(Addr) min = (ElfW(Addr))-1, max = 0;

		for (ElfW(Half) i = 0; i < eh->e_phnum; i++) {
			if (ph[i].p_type == PT_LOAD) {
				if (ph[i].p_vaddr < min) min = ph[i].p_vaddr;
				ElfW(Addr) end = ph[i].p_vaddr + ph[i].p_memsz;
				if (end > max) max = end;
			}
		}
		if (min == (ElfW(Addr))-1) return 0;

		size_t alloc = (max - min + 4095) & ~4095;
		h->base = mmap(NULL, alloc, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (h->base == MAP_FAILED) return 0;
		h->size = alloc;

		for (ElfW(Half) i = 0; i < eh->e_phnum; i++) {
			if (ph[i].p_type == PT_LOAD) {
				void *dst = (char *)h->base + ph[i].p_vaddr - min;
				memcpy(dst, (char *)file_map + ph[i].p_offset, ph[i].p_filesz);
				memset((char *)dst + ph[i].p_filesz, 0, ph[i].p_memsz - ph[i].p_filesz);
			}
		}

		__dl_elf_meta_t *m = calloc(1, sizeof(__dl_elf_meta_t));
		if (!m) { munmap(h->base, h->size); h->base = MAP_FAILED; return 0; }
		h->meta = m;

		ElfW(Phdr) *pdyn = NULL;
		for (ElfW(Half) i = 0; i < eh->e_phnum; i++) if (ph[i].p_type == PT_DYNAMIC) { pdyn = &ph[i]; break; }
		if (!pdyn) { free(m); munmap(h->base, h->size); h->base = MAP_FAILED; return 0; }

		ElfW(Dyn) *dyn = (ElfW(Dyn) *)((char *)h->base + pdyn->p_vaddr - min);
		for (; dyn->d_tag != DT_NULL; dyn++) {
			switch (dyn->d_tag) {
				case DT_SYMTAB: m->symtab = (ElfW(Sym) *)((char *)h->base + dyn->d_un.d_ptr - min); break;
				case DT_STRTAB: m->strtab = (const char *)((char *)h->base + dyn->d_un.d_ptr - min); break;
				case DT_HASH: {
					ElfW(Word) *hash = (ElfW(Word) *)((char *)h->base + dyn->d_un.d_ptr - min);
					m->sysv_nbucket = hash[0];
					m->sysv_buckets = &hash[2];
					m->sysv_chains = &hash[2 + m->sysv_nbucket];
					break;
				}
				case DT_GNU_HASH: {
					ElfW(Word) *gh = (ElfW(Word) *)((char *)h->base + dyn->d_un.d_ptr - min);
					m->gnu_nbucket = gh[0];
					m->gnu_symndx = gh[1];
					m->gnu_maskwords = gh[2];
					m->gnu_shift2 = gh[3];
					m->gnu_bloom = (ElfW(Addr) *)(gh + 4);
					m->gnu_buckets = (ElfW(Word) *)((char *)m->gnu_bloom + m->gnu_maskwords * sizeof(ElfW(Addr)));
					break;
				}
			}
		}
		return (m->symtab && m->strtab && (m->sysv_buckets || m->gnu_buckets)) ? 1 : 0;
	}

	static inline void __dl_free_elf(jacl_dl_handle_t *h) {
		if (h->base != MAP_FAILED) munmap(h->base, h->size);
		free(h->meta);
	}

	static inline int __dl_addr_elf(jacl_dl_handle_t *h, const void *addr, Dl_info *info) {
		if (!info) return 0;
		uintptr_t base = (uintptr_t)h->base;
		uintptr_t target = (uintptr_t)addr;
		if (target < base || target >= base + h->size) return 0;

		__dl_elf_meta_t *m = (__dl_elf_meta_t *)h->meta;
		if (!m || !m->symtab || !m->strtab) return 0;

		ElfW(Sym) *best = NULL;
		size_t count = m->sysv_nbucket ? (m->sysv_nbucket * 2) : (m->gnu_nbucket ? m->gnu_nbucket * 4 : 0); /* Estimate if no explicit count */
		for (size_t i = 1; i < count && i < (h->size / sizeof(ElfW(Sym))); i++) {
			ElfW(Sym) *s = &m->symtab[i];
			if (s->st_value && s->st_value <= (target - base)) {
				if (!best || s->st_value > best->st_value) best = s;
			}
		}
		if (!best) return 0;

		info->dli_fname = h->path;
		info->dli_fbase = h->base;
		info->dli_sname = m->strtab + best->st_name;
		info->dli_saddr = (void *)(base + best->st_value);
		return 1;
	}
#undef __BIN_LINK
#endif /* __BIN_LINK */

#ifdef __cplusplus
}
#endif
