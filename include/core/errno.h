/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _CORE_ERRNO_H
#define _CORE_ERRNO_H
#pragma once

#include <config.h>

#ifdef __cplusplus
extern "C" {
#endif

thread_local int __jacl_errno = 0;

#ifdef __cplusplus
}
#endif

#endif /* _CORE_ERRNO_H */
