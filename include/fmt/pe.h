/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef FMT_PE_H
#define FMT_PE_H

#include <stdint.h>
#include <x/format_pe.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __FMT_INIT

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
} IMAGE_DOS_HEADER;

/* Data directory */
typedef struct {
	DWORD VirtualAddress;
	DWORD Size;
} IMAGE_DATA_DIRECTORY;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

/* COFF file header */
typedef struct {
	WORD  Machine;
	WORD  NumberOfSections;
	DWORD TimeDateStamp;
	DWORD PointerToSymbolTable;
	DWORD NumberOfSymbols;
	WORD  SizeOfOptionalHeader;
	WORD  Characteristics;
} IMAGE_FILE_HEADER;

/* Optional header (32-bit) */
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
} IMAGE_OPTIONAL_HEADER32;

/* Optional header (64-bit) */
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
} IMAGE_OPTIONAL_HEADER64;

/* NT headers */
typedef struct {
	DWORD                 Signature;
	IMAGE_FILE_HEADER     FileHeader;
	IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32;

typedef struct {
	DWORD                 Signature;
	IMAGE_FILE_HEADER     FileHeader;
	IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64;

/* Section header */
#define IMAGE_SIZEOF_SHORT_NAME 8

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
} IMAGE_SECTION_HEADER;

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
extern init_func_t __start_.CRT$XCU[] __attribute__((weak));
extern init_func_t __stop_.CRT$XCU[] __attribute__((weak));
#define INIT_START __start_.CRT$XCU
#define INIT_END __stop_.CRT$XCU

#undef __FMT_INIT
#endif

#ifdef __cplusplus
}
#endif

#endif /* FMT_PE_H */

