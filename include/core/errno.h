/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef CORE_ERRNO_H
#define CORE_ERRNO_H
#pragma once

#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif

_Thread_local int __jacl_errno = 0;

#ifdef __cplusplus
}
#endif

#endif /* CORE_ERRNO_H */
