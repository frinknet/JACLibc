/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef JACLIBC_H
#define JACLIBC_H
#pragma once

/**
 * NOTE: This is the canonical entrypoint for the library for beginners and
 * those who refuse to be bothered by traditional include hell that libc can
 * bring. Simply include this header and it will include the entire libc.
 *
 *   #include <jaclibc.h>   ← ONE AND DONE!!!
 *
 * In your main.c file you need to first define JACL_MAIN so that the global
 * static pieces also get included. You can clearly see how this works below...
 *
 * While legacy libc makes this apporach less useful due to few inlines and the
 * need for dynamic linking, we use the anti-thetical approach opting for tiny
 * binaries with miniscule attack surface and maximum portablity. This is one of
 * three usage patterns supported by JACLibc.
 *
 * The second way to do this is compiling the included jaclib.c in your final
 * compilation so that you get the necessary static globals. Or if you prefer
 * maximum control you can include <static.h> in your main translation unit.
 *
 * EITHER WAY THIS SHOULD "JUST WORK" - FILE AN ISSUE ON GITHUB IF IT DOESN'T...
 *
 * NOTE: You can also use this if you are confused over inclusion order.
 */

// Platform config
#include <config.h>       // Build configuration and feature detection

// Error Handling
#include <errno.h>        // System error reporting mechanism

// Numeric Limits
#include <float.h>        // Floating-point implementation characteristics
#include <limits.h>       // Integer type ranges and boundaries

// Core Definitions
#include <stdarg.h>       // Variable-length argument list support
#include <stddef.h>       // Fundamental type definitions and NULL

// Core Types
#include <stdint.h>       // Exact-width integer types
#include <stdbit.h>       // Bit manipulation utilities
#include <stdbool.h>      // Boolean type and values
#include <stdalign.h>     // Alignment control
#include <stdnoreturn.h>  // Non-returning function annotation

// Character Strings
#include <ctype.h>        // ASCII character testing and conversion
#include <string.h>       // String manipulation and memory operations
#include <wctype.h>       // Wide character classification

// System Types and Interfaces
#include <sys/types.h>    // Primitive system data types
#include <sys/syscall.h>  // System call numbers and interface
#include <sys/ioctl.h>    // Device control operations
#include <sys/wait.h>     // Process waiting and status

// POSIX Environment
#include <unistd.h>       // POSIX operating system API
#include <fenv.h>         // Floating-point environment control
#include <fcntl.h>        // File control and descriptor operations
#include <time.h>         // Time types and manipulation
#include <math.h>         // Mathematical functions

// C99 Features
#if JACL_HAS_C99
#include <complex.h>      // Complex number arithmetic
#include <inttypes.h>     // Extended integer formatting and parsing
#include <locale.h>       // Localization and culture settings
#include <wchar.h>        // Wide character string operations
#endif

// C11 Features
#if JACL_HAS_C11
#include <stdatomic.h>    // Atomic operations and memory ordering
#include <tgmath.h>       // Type-generic mathematical macros
#include <uchar.h>        // Unicode character utilities
#endif

// System IO Extensions
#include <sys/mman.h>     // Memory mapping and shared memory
#include <sys/time.h>     // Time structures with microsecond precision
#include <sys/stat.h>     // File status and attributes

// File IO Extensions
#include <poll.h>         // Multiplexed I/O event notification
#include <stdio.h>        // Standard input/output streams
#include <stdlib.h>       // Memory allocation and program utilities
#include <strings.h>      // BSD string functions
#include <dirent.h>       // Directory traversal and reading

// Process Control
#include <signal.h>       // Signal handling and generation

// Concurrency
#include <sched.h>        // CPU scheduling policies
#include <pthread.h>      // POSIX threading API
#include <threads.h>      // C11 threading primitives
#include <assert.h>       // Diagnostic assertions

// JACLibc Extensions
#include <testing.h>      // Testing Harness
#include <jsio.h>         // JS Type Bridge

// STATIC GLOBALS
#ifdef JACL_MAIN
#include <static.h>      // Global initialization
#endif

#endif /* JACLIBC_H */
