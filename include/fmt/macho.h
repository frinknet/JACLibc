/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _FMT_MACH_O_H
#define _FMT_MACH_O_H

#include <config.h>
#include <stdint.h>
#include <x/format_macho.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __FMT_INIT

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

#undef __FMT_INIT
#endif

#ifdef __cplusplus
}
#endif

#endif /* _FMT_MACH_O_H */
