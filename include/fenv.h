// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef FENV_H
#define FENV_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Exception flags
#define FE_INVALID		(1<<0)
#define FE_DIVBYZERO	(1<<1)
#define FE_OVERFLOW		(1<<2)
#define FE_UNDERFLOW	(1<<3)
#define FE_INEXACT		(1<<4)
#define FE_ALL_EXCEPT (FE_INVALID|FE_DIVBYZERO|FE_OVERFLOW|FE_UNDERFLOW|FE_INEXACT)

// Rounding modes
#define FE_TONEAREST	0
#define FE_DOWNWARD		1
#define FE_UPWARD			2
#define FE_TOWARDZERO 3

typedef unsigned fexcept_t;
typedef int fenv_t;

// Default environment
#define FE_DFL_ENV		((fenv_t)0)

#if defined(__wasm__)
/* WASM: no-op fenv */
static inline int fetestexcept(int e)									{ (void)e; return 0; }
static inline int feraiseexcept(int e)								{ (void)e; return 0; }
static inline int feclearexcept(int e)								{ (void)e; return 0; }
static inline int fegetexceptflag(fexcept_t* f,int e) { (void)f; (void)e; return 0; }
static inline int fesetexceptflag(const fexcept_t* f,int e) { (void)f; (void)e; return 0; }
static inline int fegetround(void)										{ return FE_TONEAREST; }
static inline int fesetround(int m)										{ (void)m; return 0; }
static inline int fegetenv(fenv_t* envp)							{ (void)envp; return 0; }
static inline int fesetenv(const fenv_t* envp)				{ (void)envp; return 0; }
static inline int feholdexcept(fenv_t* envp)					{ (void)envp; return 0; }
static inline int feupdateenv(const fenv_t* envp)			{ (void)envp; return 0; }
#else
/* Native: use builtins */
static inline int fetestexcept(int e)									{ return __builtin_fetestexcept(e); }
static inline int feraiseexcept(int e)								{ return __builtin_feraiseexcept(e); }
static inline int feclearexcept(int e)								{ return __builtin_feclearexcept(e); }
static inline int fegetexceptflag(fexcept_t* f,int e) { return __builtin_fegetexceptflag(f,e); }
static inline int fesetexceptflag(const fexcept_t* f,int e) { return __builtin_fesetexceptflag(f,e); }
static inline int fegetround(void)										{ return __builtin_fegetround(); }
static inline int fesetround(int m)										{ return __builtin_fesetround(m); }
static inline int fegetenv(fenv_t* envp)							{ return __builtin_fegetenv(envp); }
static inline int fesetenv(const fenv_t* envp)				{ return __builtin_fesetenv(envp); }
static inline int feholdexcept(fenv_t* envp)					{ return __builtin_feholdexcept(envp); }
static inline int feupdateenv(const fenv_t* envp)			{ return __builtin_feupdateenv(envp); }
#endif

#ifdef __cplusplus
}
#endif

#endif /* FENV_H */
