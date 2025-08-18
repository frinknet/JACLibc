// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef STDNORETURN_H
#define STDNORETURN_H

/* C11 noreturn specifier */
#if defined(__GNUC__)
  #define noreturn  __attribute__((noreturn))
#elif defined(_MSC_VER)
  #define noreturn  __declspec(noreturn)
#else
  #define noreturn
#endif

#endif /* STDNORETURN_H */
