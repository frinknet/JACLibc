/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STDNORETURN_H
#define STDNORETURN_H
#pragma once

#include <config.h>

/* C23 deprecated this header in favor of [[noreturn]] attribute */
#if JACL_HAS_C23
  #define __STDC_VERSION_STDNORETURN_H__ 202311L
  /* Header deprecated but still allowed - define noreturn as empty or [[noreturn]] */
  #define noreturn [[noreturn]]

#elif JACL_HAS_C11
  /* C11/C17: Use standard _Noreturn keyword */
  #define noreturn _Noreturn

#else
  /* Pre-C11: Fall back to compiler extensions */
  #if defined(__GNUC__) || defined(__clang__)
    #define noreturn __attribute__((noreturn))
  #elif defined(_MSC_VER)
    #define noreturn __declspec(noreturn)
  #else
    #define noreturn
  #endif
#endif

#define __noreturn_is_defined 1

#endif /* STDNORETURN_H */
