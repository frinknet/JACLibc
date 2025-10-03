/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef SYS_TYPES_H
#define SYS_TYPES_H
#pragma once

/* Include basic types */
#include <config.h>
#include <stddef.h>  /* size_t */
#include <stdint.h>  /* fixed-width types */

#ifdef __cplusplus
extern "C" {
#endif

/* Essential system types - architecture aware */
#if JACL_64BIT
  /* 64-bit systems */
  typedef long ssize_t;           /* Signed size type */
  typedef long time_t;            /* Time type */
  typedef long clock_t;           /* Clock ticks type */
#else
  /* 32-bit systems */
  typedef int ssize_t;            /* Signed size type */
  typedef long time_t;            /* Time type */
  typedef long clock_t;           /* Clock ticks type */
#endif

/* File offset type - always 64-bit for large file support */
#if defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64
  typedef long long off_t;        /* Force 64-bit for large file support */
#elif JACL_64BIT
  typedef long off_t;             /* 64-bit systems use long */
#else
  typedef long long off_t;        /* 32-bit systems use long long for files */
#endif

/* Process and user types */
typedef int pid_t;                /* Process ID type */
typedef unsigned int uid_t;       /* User ID type */
typedef unsigned int gid_t;       /* Group ID type */
typedef unsigned int id_t;        /* General ID type */

/* File system types */
typedef unsigned int mode_t;      /* File mode/permissions */
typedef unsigned long ino_t;      /* Inode number */
typedef unsigned int dev_t;       /* Device ID */
typedef unsigned int nlink_t;     /* Link count */

/* Block and file system types */
typedef long blkcnt_t;            /* Block count */
typedef long blksize_t;           /* Block size */
typedef unsigned long fsblkcnt_t; /* File system block count */
typedef unsigned long fsfilcnt_t; /* File system file count */

/* Timing types */
typedef int clockid_t;            /* Clock identifier */

/* Microseconds types */
typedef unsigned int useconds_t;  /* Microseconds (0 to 1000000) */
typedef int suseconds_t;          /* Signed microseconds */

/* Large file support types */
#if JACL_LARGEFILE64
typedef long long off64_t;      /* 64-bit file offset */
typedef long long blkcnt64_t;   /* 64-bit block count */
#endif

/* main FILE */
typedef struct __jacl_file {
	int _flags;					 // File status flags
	char *_ptr;					 // Current position in buffer
	char *_base;				 // Base of buffer
	char *_end;					 // End of buffer
	size_t _bufsiz;			 // Buffer size
	int _fd;						 // File descriptor
	int _cnt;						 // Characters left in buffer
	int _orientation;		 // -1=byte, 0=unset, 1=wide
	unsigned char *_tmpfname; // Temp file name (if any)
} FILE;

#ifdef __cplusplus
}
#endif
#endif /* SYS_TYPES_H */
