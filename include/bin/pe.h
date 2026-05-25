/* (c) 2025 FRINKnet & Friends – MIT licence */

#include <endian.h>
#include <x/format_pe.h>

/* PE is strictly Little-Endian. Always convert from file (LE) to host. */
#define PE_RD16(h, f) le16toh((h).f)
#define PE_RD32(h, f) le32toh((h).f)
#define PE_RD64(h, f) le64toh((h).f)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __BIN_INIT

#define X_ENUM(name, val) name = (val),
	enum { PE_MAGICS(X_ENUM) };
	enum { PE_MACHINES(X_ENUM) };
	enum { PE_SECTION_FLAGS(X_ENUM) };
#undef X_ENUM

/* PE base types */
typedef uint8_t   BYTE;
typedef uint16_t  WORD;
typedef uint32_t  DWORD;
typedef uint64_t  ULONGLONG;

/* DOS header */
JACL_LAYOUT
typedef struct {
	WORD  e_magic;      /* Magic number (IMAGE_DOS_SIGNATURE) */
	WORD  e_cblp;
	WORD  e_cp;
	WORD  e_crlc;
	WORD  e_cparhdr;
	WORD  e_minalloc;
	WORD  e_maxalloc;
	WORD  e_ss;
	WORD  e_sp;
	WORD  e_csum;
	WORD  e_ip;
	WORD  e_cs;
	WORD  e_lfarlc;
	WORD  e_ovno;
	WORD  e_res[4];
	WORD  e_oemid;
	WORD  e_oeminfo;
	WORD  e_res2[10];
	DWORD e_lfanew;     /* File offset of PE header */
} JACL_PACK IMAGE_DOS_HEADER;

/* Data directory */
JACL_LAYOUT
typedef struct {
	DWORD VirtualAddress;
	DWORD Size;
} JACL_PACK IMAGE_DATA_DIRECTORY;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

/* COFF file header */
JACL_LAYOUT
typedef struct {
	WORD  Machine;
	WORD  NumberOfSections;
	DWORD TimeDateStamp;
	DWORD PointerToSymbolTable;
	DWORD NumberOfSymbols;
	WORD  SizeOfOptionalHeader;
	WORD  Characteristics;
} JACL_PACK IMAGE_FILE_HEADER;

/* Optional header (32-bit) */
JACL_LAYOUT
typedef struct {
	WORD  Magic;
	BYTE  MajorLinkerVersion;
	BYTE  MinorLinkerVersion;
	DWORD SizeOfCode;
	DWORD SizeOfInitializedData;
	DWORD SizeOfUninitializedData;
	DWORD AddressOfEntryPoint;
	DWORD BaseOfCode;
	DWORD BaseOfData;
	DWORD ImageBase;
	DWORD SectionAlignment;
	DWORD FileAlignment;
	WORD  MajorOperatingSystemVersion;
	WORD  MinorOperatingSystemVersion;
	WORD  MajorImageVersion;
	WORD  MinorImageVersion;
	WORD  MajorSubsystemVersion;
	WORD  MinorSubsystemVersion;
	DWORD Win32VersionValue;
	DWORD SizeOfImage;
	DWORD SizeOfHeaders;
	DWORD CheckSum;
	WORD  Subsystem;
	WORD  DllCharacteristics;
	DWORD SizeOfStackReserve;
	DWORD SizeOfStackCommit;
	DWORD SizeOfHeapReserve;
	DWORD SizeOfHeapCommit;
	DWORD LoaderFlags;
	DWORD NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} JACL_PACK IMAGE_OPTIONAL_HEADER32;

/* Optional header (64-bit) */
JACL_LAYOUT
typedef struct {
	WORD      Magic;
	BYTE      MajorLinkerVersion;
	BYTE      MinorLinkerVersion;
	DWORD     SizeOfCode;
	DWORD     SizeOfInitializedData;
	DWORD     SizeOfUninitializedData;
	DWORD     AddressOfEntryPoint;
	DWORD     BaseOfCode;
	ULONGLONG ImageBase;
	DWORD     SectionAlignment;
	DWORD     FileAlignment;
	WORD      MajorOperatingSystemVersion;
	WORD      MinorOperatingSystemVersion;
	WORD      MajorImageVersion;
	WORD      MinorImageVersion;
	WORD      MajorSubsystemVersion;
	WORD      MinorSubsystemVersion;
	DWORD     Win32VersionValue;
	DWORD     SizeOfImage;
	DWORD     SizeOfHeaders;
	DWORD     CheckSum;
	WORD      Subsystem;
	WORD      DllCharacteristics;
	ULONGLONG SizeOfStackReserve;
	ULONGLONG SizeOfStackCommit;
	ULONGLONG SizeOfHeapReserve;
	ULONGLONG SizeOfHeapCommit;
	DWORD     LoaderFlags;
	DWORD     NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} JACL_PACK IMAGE_OPTIONAL_HEADER64;

/* NT headers */
JACL_LAYOUT
typedef struct {
	DWORD                 Signature;
	IMAGE_FILE_HEADER     FileHeader;
	IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} JACL_PACK IMAGE_NT_HEADERS32;

JACL_LAYOUT
typedef struct {
	DWORD                 Signature;
	IMAGE_FILE_HEADER     FileHeader;
	IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} JACL_PACK IMAGE_NT_HEADERS64;

/* Section header */
#define IMAGE_SIZEOF_SHORT_NAME 8

JACL_LAYOUT
typedef struct {
	BYTE  Name[IMAGE_SIZEOF_SHORT_NAME];
	union {
		DWORD PhysicalAddress;
		DWORD VirtualSize;
	} Misc;
	DWORD VirtualAddress;
	DWORD SizeOfRawData;
	DWORD PointerToRawData;
	DWORD PointerToRelocations;
	DWORD PointerToLinenumbers;
	WORD  NumberOfRelocations;
	WORD  NumberOfLinenumbers;
	DWORD Characteristics;
} JACL_PACK IMAGE_SECTION_HEADER;

/* Bit-width dependent typedefs */
#if JACL_BITS == 64
	typedef IMAGE_NT_HEADERS64       IMAGE_NT_HEADERS;
	typedef IMAGE_OPTIONAL_HEADER64  IMAGE_OPTIONAL_HEADER;
#else
	typedef IMAGE_NT_HEADERS32       IMAGE_NT_HEADERS;
	typedef IMAGE_OPTIONAL_HEADER32  IMAGE_OPTIONAL_HEADER;
#endif

/* .CRT$XCU section */
typedef void (*init_func_t)(void);
extern init_func_t __start_.CRT$XCU[] JACL_WEAK;
extern init_func_t __stop_.CRT$XCU[] JACL_WEAK;
#define INIT_START __start_.CRT$XCU
#define INIT_END __stop_.CRT$XCU

#undef __BIN_INIT
#endif

#ifdef __BIN_LINK

/* Minimal PE types if <windows.h> is excluded */
#ifndef IMAGE_DOS_SIGNATURE
#define IMAGE_DOS_SIGNATURE 0x5A4D
#define IMAGE_NT_SIGNATURE  0x00004550
#endif
#ifndef IMAGE_DIRECTORY_ENTRY_EXPORT
#define IMAGE_DIRECTORY_ENTRY_EXPORT 0
#endif

typedef struct {
	uint32_t *names;
	uint16_t *ordinals;
	uint32_t *addresses;
	uint32_t count;
} __dl_pe_meta_t;

static inline int __dl_load_pe(jacl_dl_handle_t *h, void *file_map, struct stat *st) {
	uint16_t *dos_magic = (uint16_t *)file_map;

	if (*dos_magic != IMAGE_DOS_SIGNATURE) return 0;

	uint32_t *pe_off = (uint32_t *)((char *)file_map + 0x3C);
	uint32_t *pe_sig  = (uint32_t *)((char *)file_map + *pe_off);

	if (*pe_sig != IMAGE_NT_SIGNATURE) return 0;

	/* Parse PE headers manually to avoid windows.h bloat */
	uint16_t *machine	 = (uint16_t *)((char *)pe_sig + 4);
	uint16_t *num_sections= (uint16_t *)((char *)pe_sig + 6);
	uint16_t *opt_size	= (uint16_t *)((char *)pe_sig + 20);

	/* Data Directory starts after Optional Header */
	uint32_t *data_dir = (uint32_t *)((char *)pe_sig + 24 + *opt_size);
	uint32_t exp_rva = data_dir[IMAGE_DIRECTORY_ENTRY_EXPORT * 2];

	if (!exp_rva) return 0;

	h->base = VirtualAlloc(NULL, st->st_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	if (!h->base) return 0;

	h->size = st->st_size;

	memcpy(h->base, file_map, st->st_size);

	__dl_pe_meta_t *m = calloc(1, sizeof(__dl_pe_meta_t));

	if (!m) { VirtualFree(h->base, 0, MEM_RELEASE); return 0; }

	h->meta = m;

	uint8_t *img = (uint8_t *)h->base;
	uint32_t *exp_dir = (uint32_t *)(img + exp_rva);

	m->names	 = (uint32_t *)(img + exp_dir[2]);   /* AddressOfNames */
	m->ordinals  = (uint16_t *)(img + exp_dir[3]);   /* AddressOfNameOrdinals */
	m->addresses = (uint32_t *)(img + exp_dir[1]);   /* AddressOfFunctions */
	m->count	 = exp_dir[5];					   /* NumberOfNames */

	return 1;
}

static inline void *__dl_find_pe(jacl_dl_handle_t *h, const char *sym) {
	__dl_pe_meta_t *m = (__dl_pe_meta_t *)h->meta;

	if (!m || !m->names || !m->addresses) return NULL;

	for (uint32_t i = 0; i < m->count; i++) {
		const char *name = (const char *)((uint8_t *)h->base + m->names[i]);

		if (strcmp(name, sym) == 0) {
			uint32_t rva = m->addresses[m->ordinals[i]];

			return (void *)((uint8_t *)h->base + rva);
		}
	}

	return NULL;
}

static inline void __dl_free_pe(jacl_dl_handle_t *h) {
	if (h->base) VirtualFree(h->base, 0, MEM_RELEASE);

	free(h->meta);
}

static inline int __dl_addr_pe(jacl_dl_handle_t *h, const void *addr, Dl_info *info) {
	if (!info) return 0;

	uintptr_t base = (uintptr_t)h->base;
	uintptr_t target = (uintptr_t)addr;

	if (target < base || target >= base + h->size) return 0;

	__dl_pe_meta_t *m = (__dl_pe_meta_t *)h->meta;

	if (!m || !m->names || !m->addresses) return 0;

	uintptr_t offset = target - base;
	const char *best_name = NULL;
	uint32_t best_rva = 0;

	for (uint32_t i = 0; i < m->count; i++) {
		uint32_t rva = m->addresses[m->ordinals[i]];

		if (rva <= offset && rva > best_rva) {
			best_rva = rva;
			best_name = (const char *)((uint8_t *)h->base + m->names[i]);
		}
	}

	if (!best_name) return 0;

	info->dli_fname = h->path;
	info->dli_fbase = h->base;
	info->dli_sname = best_name;
	info->dli_saddr = (void *)(base + best_rva);

	return 1;
}

#undef __BIN_LINK
#endif

#ifdef __cplusplus
}
#endif
