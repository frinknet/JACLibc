/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef ARCH_WASM_H
#define ARCH_WASM_H
#pragma once

#include <jsio.h>  // For js_read/js_write

#ifdef __cplusplus
extern "C" {
#endif

static inline long __arch_syscall(long num, long a1, long a2, long a3, long a4, long a5, long a6) {
  switch ((int)num) {
    case SYS_read:
      return js_read((int)a1, (void*)a2, (size_t)a3);
    case SYS_write:
      return js_write((int)a1, (const void*)a2, (size_t)a3);

    // Add more cases as you implement them
    default:
      return 0; // Or return -1/ENOSYS for unsupported
  }
}

#ifdef __cplusplus
}
#endif

#endif // ARCH_WASM_H
