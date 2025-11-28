/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _ELF_H
#define _ELF_H
#pragma once

/**
 * ELF format header - Linux/LSB compatibility
 *
 * The Linux Standard Base (LSB) and most Linux systems expect <elf.h>
 * to provide ELF binary format structures. While not part of POSIX or
 * ISO C, this header is required for tools that parse executables,
 * debuggers, and low-level system utilities.
 *
 * The real definition is in fmt/elf.h
 */

#include <fmt/elf.h>

#endif /* _ELF_H */
