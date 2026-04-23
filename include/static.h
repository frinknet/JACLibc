/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _STATIC_H
#define _STATIC_H
#pragma once

/**
 * NOTE: This is the implementation section of the JACLibc. Since we have opted
 * for a HEADER-ONLY design pattern for this library it is important to include
 * the implementation ONLY ONCE in your project. You can do that by including
 * <static.h> alone or if you would rather you can include the implementations
 * separately... YOU HAVE A LOT OF FLEXIBILITY!!!
 */

#include <core/errno.h>
#include <core/stack.h>
#include <core/fenv.h>
#include <core/stdio.h>
#include <core/stdlib.h>
#include <core/pthread.h>
#include <core/complex.h>
#include <core/unistd.h>
#include <core/locale.h>
#include <core/jsio.h>
#include <core/start.h>

#endif /* _STATIC_H */
