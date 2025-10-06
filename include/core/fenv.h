/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef CORE_FENV_H
#define CORE_FENV_H

#include <config.h>
#include <fenv.h>

_Thread_local fenv_t __jacl_fenv = { 0u, FE_TONEAREST };

#endif /* CORE_FENV_H */
