/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H
#pragma once

/* Include basic types */
#include <config.h>
#include <stdint.h>
#include <limits.h> /* Base limit macros */

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================= */
/* Essential system types - architecture aware                   */
/* ============================================================= */

#if JACL_HAS_C99
  typedef long long time_t;            /* Time type */
  typedef long long clock_t;           /* Clock ticks type */
#else
  typedef long time_t;                 /* Time type */
  typedef long clock_t;                /* Clock ticks type */
#endif

/* File offset type - always 64-bit for large file support */
#if defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64
  typedef long long off_t;             /* Force 64-bit for large file support */
#elif JACL_64BIT
  typedef long off_t;                  /* 64-bit systems use long */
#else
  typedef long long off_t;             /* 32-bit systems use long long for files */
#endif

/* Process and user types */
typedef int             pid_t;         /* Process ID type */
typedef unsigned int    uid_t;         /* User ID type */
typedef unsigned int    gid_t;         /* Group ID type */
typedef unsigned int    id_t;          /* General ID type */

/* File system types */
typedef unsigned int    mode_t;        /* File mode/permissions */
typedef unsigned long   ino_t;         /* Inode number */
typedef unsigned int    dev_t;         /* Device ID */
typedef unsigned int    nlink_t;       /* Link count */

/* Block and file system types */
typedef long            blkcnt_t;      /* Block count */
typedef long            blksize_t;     /* Block size */
typedef unsigned long   fsblkcnt_t;    /* File system block count */
typedef unsigned long   fsfilcnt_t;    /* File system file count */

/* Timing types */
typedef int             clockid_t;     /* Clock identifier */
typedef unsigned int    useconds_t;    /* Microseconds (0 to 1000000) */
typedef int             suseconds_t;   /* Signed microseconds */

/* Internet types */
typedef unsigned int    socklen_t;     /* Socket address length */
typedef unsigned short  sa_family_t;   /* Socket address family */

/* IPC types */
typedef int             key_t;         /* IPC key */

/* Large file support types */
#if JACL_HAS_LFS
typedef long long       off64_t;       /* 64-bit file offset */
typedef long long       blkcnt64_t;    /* 64-bit block count */
#endif

#if JACL_64BIT
typedef long            ssize_t;
#else
typedef int             ssize_t;
#endif

/* ============================================================= */
/* Type Limits (Zero-Overhead, Architecture-Aware)               */
/* ============================================================= */

#ifndef TIME_MAX
  #if JACL_HAS_C99
    #define TIME_MAX  ((time_t)((~0ULL) >> 1))
    #define TIME_MIN  ((time_t)(~TIME_MAX))
    #define CLOCK_MAX ((clock_t)((~0ULL) >> 1))
    #define CLOCK_MIN ((clock_t)(~CLOCK_MAX))
  #else
    #define TIME_MAX  ((time_t)((~0UL) >> 1))
    #define TIME_MIN  ((time_t)(~TIME_MAX))
    #define CLOCK_MAX ((clock_t)((~0UL) >> 1))
    #define CLOCK_MIN ((clock_t)(~CLOCK_MAX))
  #endif
#endif

#ifndef OFF_MAX
  #define OFF_MAX   ((off_t)((~0ULL) >> 1))
  #define OFF_MIN   ((off_t)(~OFF_MAX))
#endif

#ifndef PID_MAX
  #define PID_MAX   INT_MAX
  #define PID_MIN   INT_MIN
#endif

#ifndef UID_MAX
  #define UID_MAX   UINT_MAX
#endif
#ifndef GID_MAX
  #define GID_MAX   UINT_MAX
#endif

#ifndef MODE_MAX
  #define MODE_MAX  UINT_MAX
#endif
#ifndef INO_MAX
  #define INO_MAX   ULONG_MAX
#endif
#ifndef DEV_MAX
  #define DEV_MAX   UINT_MAX
#endif
#ifndef NLINK_MAX
  #define NLINK_MAX UINT_MAX
#endif

#ifndef BLKCNT_MAX
  #define BLKCNT_MAX ((blkcnt_t)((~0UL) >> 1))
  #define BLKCNT_MIN ((blkcnt_t)(~BLKCNT_MAX))
#endif
#ifndef BLKSIZE_MAX
  #define BLKSIZE_MAX ((blksize_t)((~0UL) >> 1))
#endif

#ifndef FSBLKCNT_MAX
  #define FSBLKCNT_MAX ULONG_MAX
#endif
#ifndef FSFILCNT_MAX
  #define FSFILCNT_MAX ULONG_MAX
#endif

#ifndef CLOCKID_MAX
  #define CLOCKID_MAX INT_MAX
#endif

#ifndef USECONDS_MAX
  #define USECONDS_MAX UINT_MAX
#endif
#ifndef SUSECONDS_MAX
  #define SUSECONDS_MAX INT_MAX
  #define SUSECONDS_MIN INT_MIN
#endif

#ifndef SOCKLEN_MAX
  #define SOCKLEN_MAX UINT_MAX
#endif
#ifndef SA_FAMILY_MAX
  #define SA_FAMILY_MAX USHRT_MAX
#endif

#ifndef KEY_MAX
  #define KEY_MAX   INT_MAX
  #define KEY_MIN   INT_MIN
#endif

#if JACL_HAS_LFS
  #ifndef OFF64_MAX
    #define OFF64_MAX    ((off64_t)((~0ULL) >> 1))
    #define OFF64_MIN    ((off64_t)(~OFF64_MAX))
  #endif
  #ifndef BLKCNT64_MAX
    #define BLKCNT64_MAX ((blkcnt64_t)((~0ULL) >> 1))
    #define BLKCNT64_MIN ((blkcnt64_t)(~BLKCNT64_MAX))
  #endif
#endif

#ifndef SSIZE_MAX
  #if JACL_64BIT
    #define SSIZE_MAX LONG_MAX
    #define SSIZE_MIN LONG_MIN
  #else
    #define SSIZE_MAX INT_MAX
    #define SSIZE_MIN INT_MIN
  #endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SYS_TYPES_H */
