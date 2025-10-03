// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef FENV_C
#define FENV_C

#include <config.h>
#include <fenv.h>

_Thread_local fenv_t __jacl_fenv = { 0u, FE_TONEAREST };

#endif // FENV_C
