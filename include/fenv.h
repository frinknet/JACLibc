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
int fetestexcept(int e)									{ (void)e; return 0; }
int feraiseexcept(int e)								{ (void)e; return 0; }
int feclearexcept(int e)								{ (void)e; return 0; }
int fegetexceptflag(fexcept_t* f,int e) { (void)f; (void)e; return 0; }
int fesetexceptflag(const fexcept_t* f,int e) { (void)f; (void)e; return 0; }
int fegetround(void)										{ return FE_TONEAREST; }
int fesetround(int m)										{ (void)m; return 0; }
int fegetenv(fenv_t* envp)							{ (void)envp; return 0; }
int fesetenv(const fenv_t* envp)				{ (void)envp; return 0; }
int feholdexcept(fenv_t* envp)					{ (void)envp; return 0; }
int feupdateenv(const fenv_t* envp)			{ (void)envp; return 0; }
#else
/* Native: use builtins */
int fetestexcept(int e)									{ return __builtin_fetestexcept(e); }
int feraiseexcept(int e)								{ return __builtin_feraiseexcept(e); }
int feclearexcept(int e)								{ return __builtin_feclearexcept(e); }
int fegetexceptflag(fexcept_t* f,int e) { return __builtin_fegetexceptflag(f,e); }
int fesetexceptflag(const fexcept_t* f,int e) { return __builtin_fesetexceptflag(f,e); }
int fegetround(void)										{ return __builtin_fegetround(); }
int fesetround(int m)										{ return __builtin_fesetround(m); }
int fegetenv(fenv_t* envp)							{ return __builtin_fegetenv(envp); }
int fesetenv(const fenv_t* envp)				{ return __builtin_fesetenv(envp); }
int feholdexcept(fenv_t* envp)					{ return __builtin_feholdexcept(envp); }
int feupdateenv(const fenv_t* envp)			{ return __builtin_feupdateenv(envp); }
#endif

#ifdef __cplusplus
}
#endif

#endif /* FENV_H */
