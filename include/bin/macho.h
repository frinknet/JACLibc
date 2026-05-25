/* (c) 2025-2026 FRINKnet & Friends – MIT licence */

#include <endian.h>

/* Magic dictates byte order */
#define MACH_IS_LE(m) ((m) == 0xfeedface || (m) == 0xfeedfacf)
#define MACH_RD32(h, f) ((MACH_IS_LE((h).magic)) ? le32toh((h).f) : be32toh((h).f))

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MACHO_TYPES
#define __MACHO_TYPES

#include <x/format_macho.h>

/* Expand to enums */
#define X_ENUM(name, val) name = val,
enum { MACH_MAGICS(X_ENUM) };
enum { MACH_LOAD_COMMANDS(X_ENUM) };
enum { MACH_HEADER_FLAGS(X_ENUM) };
enum { MACH_CPU_TYPES(X_ENUM) };
#undef X_ENUM

/* Minimal Mach-O structures - define only what you need */
JACL_LAYOUT
typedef struct {
	uint32_t magic;
	uint32_t cputype;
	uint32_t cpusubtype;
	uint32_t filetype;
	uint32_t ncmds;
	uint32_t sizeofcmds;
	uint32_t flags;
	uint32_t reserved;
} JACL_PACK Mach_Header_64;

JACL_LAYOUT
typedef struct {
	uint32_t cmd;
	uint32_t cmdsize;
} JACL_PACK Mach_Load_Command;

#endif /* __MACHO_TYPES */

#ifdef __BIN_INIT

/* Get TLS functions from arch */
#define __ARCH_TLS
#include JACL_ARCH_FILE

static inline void __jacl_init_fmt(void) {
	/* Mach-O / dyld init - currently no-op */
	(void)__jacl_arch_tls_get;
	(void)__jacl_arch_tls_set;
}

/* macOS: __mod_init_func section */
typedef void (*init_func_t)(void);
extern init_func_t __start___mod_init_func[] JACL_WEAK;
extern init_func_t __stop___mod_init_func[] JACL_WEAK;
#define INIT_START __start___mod_init_func
#define INIT_END __stop___mod_init_func

#undef __BIN_INIT
#endif

#ifdef __BIN_LINK

/* Minimal Mach-O definitions (64-bit) */
#define __MH_MAGIC_64  0xfeedfacf
#define __MH_CIGAM_64  0xcffaedfe
#define __LC_SYMTAB	0x02

typedef struct {
	uint32_t magic, cputype, cpusubtype, filetype;
	uint32_t ncmds, sizeofcmds, flags, reserved;
} __macho_hdr_t;

typedef struct {
	uint32_t cmd, cmdsize;
} __macho_lc_t;

typedef struct {
	uint32_t cmd, cmdsize, symoff, nsyms, stroff, strsize;
} __macho_symtab_t;

typedef struct {
	union { uint32_t n_strx; } n_un;
	uint8_t  n_type, n_sect;
	uint16_t n_desc;
	uint64_t n_value;
} __macho_nlist_t;

typedef struct {
	__macho_nlist_t *symtab;
	const char *strtab;
	uint32_t nsyms;
} __dl_macho_meta_t;

static inline int __dl_load_macho(jacl_dl_handle_t *h, void *file_map, struct stat *st) {
	__macho_hdr_t *mh = (__macho_hdr_t *)file_map;

	if (mh->magic != __MH_MAGIC_64 && mh->magic != __MH_CIGAM_64) return 0;

	__macho_lc_t *lc = (__macho_lc_t *)((char *)mh + sizeof(__macho_hdr_t));
	__macho_symtab_t *sym_cmd = NULL;

	for (uint32_t i = 0; i < mh->ncmds; i++) {
		if (lc->cmd == __LC_SYMTAB) sym_cmd = (__macho_symtab_t *)lc;

		lc = (__macho_lc_t *)((char *)lc + lc->cmdsize);
	}

	if (!sym_cmd) return 0;

	size_t alloc = (st->st_size + 4095) & ~4095;

	h->base = mmap(NULL, alloc, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (h->base == MAP_FAILED) return 0;

	h->size = alloc;

	memcpy(h->base, file_map, st->st_size);

	__dl_macho_meta_t *m = calloc(1, sizeof(__dl_macho_meta_t));

	if (!m) { munmap(h->base, h->size); return 0; }

	h->meta = m;
	m->symtab = (__macho_nlist_t *)((char *)h->base + sym_cmd->symoff);
	m->strtab = (const char *)((char *)h->base + sym_cmd->stroff);
	m->nsyms = sym_cmd->nsyms;

	return 1;
}

static inline void *__dl_find_macho(jacl_dl_handle_t *h, const char *sym) {
	__dl_macho_meta_t *m = (__dl_macho_meta_t *)h->meta;

	if (!m || !m->symtab || !m->strtab) return NULL;

	for (uint32_t i = 0; i < m->nsyms; i++) {
		if (m->symtab[i].n_un.n_strx && m->symtab[i].n_value) {
			const char *name = m->strtab + m->symtab[i].n_un.n_strx;

			if (name[0] == '_') name++;
			if (strcmp(name, sym) == 0) return (void *)((char *)h->base + m->symtab[i].n_value);
		}
	}

	return NULL;
}

static inline void __dl_free_macho(jacl_dl_handle_t *h) {
	if (h->base != MAP_FAILED) munmap(h->base, h->size);

	free(h->meta);
}

static inline int __dl_addr_macho(jacl_dl_handle_t *h, const void *addr, Dl_info *info) {
	if (!info) return 0;

	uintptr_t base = (uintptr_t)h->base;
	uintptr_t target = (uintptr_t)addr;

	if (target < base || target >= base + h->size) return 0;

	__dl_macho_meta_t *m = (__dl_macho_meta_t *)h->meta;

	if (!m || !m->symtab || !m->strtab) return 0;

	uintptr_t offset = target - base;

	__macho_nlist_t *best = NULL;

	for (uint32_t i = 0; i < m->nsyms; i++) {
		__macho_nlist_t *sym = &m->symtab[i];

		if (sym->n_un.n_strx && sym->n_value && sym->n_value <= offset) {
			if (!best || sym->n_value > best->n_value) best = sym;
		}
	}

	if (!best) return 0;

	const char *name = m->strtab + best->n_un.n_strx;

	if (name[0] == '_') name++;

	info->dli_fname = h->path;
	info->dli_fbase = h->base;
	info->dli_sname = name;
	info->dli_saddr = (void *)(base + best->n_value);

	return 1;
}

#undef __BIN_LINK
#endif

#ifdef __cplusplus
}
#endif
