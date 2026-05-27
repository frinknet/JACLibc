/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#ifndef _LIMITS_H
#define _LIMITS_H
#pragma once

#include <config.h>

/* ============================================================= */
/* Type Bit-Width Definitions                                    */
/* ============================================================= */

#ifndef CHAR_BIT
	#ifdef __CHAR_BIT__
		#define CHAR_BIT __CHAR_BIT__
	#else
		#define CHAR_BIT 8
	#endif
#endif

#ifndef SCHAR_BIT
	#define SCHAR_BIT CHAR_BIT
#endif

#ifndef SHRT_BIT
	#ifdef __SIZEOF_SHORT__
		#define SHRT_BIT (__SIZEOF_SHORT__ * CHAR_BIT)
	#elif defined(JACL_8BIT)
		#define SHRT_BIT (1 * CHAR_BIT)
	#else
		#define SHRT_BIT (2 * CHAR_BIT)
	#endif
#endif

#ifndef INT_BIT
	#ifdef __SIZEOF_INT__
		#define INT_BIT (__SIZEOF_INT__ * CHAR_BIT)
	#elif defined(JACL_16BIT)
		#define INT_BIT (2 * CHAR_BIT)
	#elif defined(JACL_8BIT)
		#define INT_BIT (1 * CHAR_BIT)
	#else
		#define INT_BIT (4 * CHAR_BIT)
	#endif
#endif

#ifndef LONG_BIT
	#ifdef __SIZEOF_LONG__
		#define LONG_BIT (__SIZEOF_LONG__ * CHAR_BIT)
	#elif defined(__LP64__) || defined(JACL_64BIT)
		#define LONG_BIT (8 * CHAR_BIT)
	#else
		#define LONG_BIT (4 * CHAR_BIT)
	#endif
#endif

#if JACL_HAS_C99
	#ifndef LLONG_BIT
		#ifdef __SIZEOF_LONG_LONG__
			#define LLONG_BIT (__SIZEOF_LONG_LONG__ * CHAR_BIT)
		#else
			#define LLONG_BIT (8 * CHAR_BIT)
		#endif
	#endif
#endif

#ifndef MB_LEN_MAX
	#define MB_LEN_MAX 16
#endif

/* ============================================================= */
/* Character Type Limits                                         */
/* ============================================================= */

#ifndef SCHAR_MAX
	#if defined(__SCHAR_MAX__)
		# define SCHAR_MAX __SCHAR_MAX__
	#else
		# define SCHAR_MAX 127
	#endif
#endif

#ifndef SCHAR_MIN
	#if defined(__SCHAR_MIN__)
		#define SCHAR_MIN __SCHAR_MIN__
	#else
	  #define SCHAR_MIN (-SCHAR_MAX - 1)
	#endif
#endif

#ifndef UCHAR_MAX
	#if defined(__UCHAR_MAX__)
		#define UCHAR_MAX __UCHAR_MAX__
	#else
		#define UCHAR_MAX (2U * SCHAR_MAX + 1U)
	#endif
#endif

#ifndef CHAR_MIN
	#if defined(__CHAR_UNSIGNED__) || defined(_CHAR_UNSIGNED)
		#define CHAR_MIN 0
	#else
		#define CHAR_MIN SCHAR_MIN
	#endif
#endif

#ifndef CHAR_MAX
	#if defined(__CHAR_UNSIGNED__) || defined(_CHAR_UNSIGNED)
		#define CHAR_MAX UCHAR_MAX
	#else
		#define CHAR_MAX SCHAR_MAX
	#endif
#endif

/* ============================================================= */
/* Short Integer Limits                                          */
/* ============================================================= */

#ifndef SHRT_MAX
	#if defined(__SHRT_MAX__)
		#define SHRT_MAX __SHRT_MAX__
	#else
		#define SHRT_MAX ((1U << (SHRT_BIT - 1)) - 1)
	#endif
#endif

#ifndef SHRT_MIN
	#if defined(__SHRT_MIN__)
		#define SHRT_MIN __SHRT_MIN__
	#else
		#define SHRT_MIN (-SHRT_MAX - 1)
	#endif
#endif

#ifndef USHRT_MAX
	#if defined(__USHRT_MAX__)
	  #define USHRT_MAX __USHRT_MAX__
	#else
		#define USHRT_MAX (2U * SHRT_MAX + 1U)
	#endif
#endif

/* ============================================================= */
/* Integer Limits                                                */
/* ============================================================= */

#ifndef INT_MAX
	#if defined(__INT_MAX__)
		#define INT_MAX __INT_MAX__
	#else
		#define INT_MAX ((1UL << (INT_BIT - 1)) - 1)
	#endif
#endif

#ifndef INT_MIN
	#if defined(__INT_MIN__)
		#define INT_MIN __INT_MIN__
	#else
		#define INT_MIN (-INT_MAX - 1)
	#endif
#endif

#ifndef UINT_MAX
	#if defined(__UINT_MAX__)
		#define UINT_MAX __UINT_MAX__
	#else
		#define UINT_MAX (2U * INT_MAX + 1U)
	#endif
#endif

/* ============================================================= */
/* Long Integer Limits                                           */
/* ============================================================= */

#ifndef LONG_MAX
	#if defined(__LONG_MAX__)
		#define LONG_MAX __LONG_MAX__
	#else
		#define LONG_MAX ((1ULL << (LONG_BIT - 1)) - 1)
	#endif
#endif

#ifndef LONG_MIN
	#if defined(__LONG_MIN__)
		#define LONG_MIN __LONG_MIN__
	#else
		#define LONG_MIN (-LONG_MAX - 1)
	#endif
#endif

#ifndef ULONG_MAX
	#if defined(__ULONG_MAX__)
		#define ULONG_MAX __ULONG_MAX__
	#else
		#define ULONG_MAX (2UL * LONG_MAX + 1UL)
	#endif
#endif

/* ============================================================= */
/* Long Long Integer Limits (C99)                                */
/* ============================================================= */


#if JACL_HAS_C99
	#ifndef LLONG_MAX
		#if defined(__LLONG_MAX__)
			#define LLONG_MAX __LLONG_MAX__
		#else
			#define LLONG_MAX ((1ULL << (LLONG_BIT - 1)) - 1)
		#endif
	#endif

	#ifndef LLONG_MIN
		#if defined(__LLONG_MIN__)
			#define LLONG_MIN __LLONG_MIN__
		#else
			#define LLONG_MIN (-LLONG_MAX - 1)
		#endif
	#endif

	#ifndef ULLONG_MAX
		#if defined(__ULLONG_MAX__)
			#define ULLONG_MAX __ULLONG_MAX__
		#else
			#define ULLONG_MAX (2ULL * LLONG_MAX + 1ULL)
		#endif
	#endif
#endif

/* ============================================================= */
/* Signed and Unsigned Size Type                                 */
/* ============================================================= */

#if JACL_64BIT
	#if defined(_WIN64) || defined(__LLP64__)
		typedef unsigned long long size_t;
		typedef long long          ssize_t;
	#else
		typedef unsigned long      size_t;
		typedef long               ssize_t;
	#endif
#else
	typedef unsigned int         size_t;
	typedef int                  ssize_t;
#endif

#ifndef SIZE_MAX
	#if defined(__SIZE_MAX__)
		#define SIZE_MAX __SIZE_MAX__
	#else
		#define SIZE_MAX ((size_t)-1)
	#endif
#endif

#ifndef SSIZE_MAX
	#if defined(__SSIZE_MAX__)
		#define SSIZE_MAX __SSIZE_MAX__
	#else
		#define SSIZE_MAX ((ssize_t)(SIZE_MAX / 2))
	#endif
#endif

#ifndef SSIZE_MIN
	#if defined(__SSIZE_MIN__)
		#define SSIZE_MIN __SSIZE_MIN__
	#else
			#define SSIZE_MIN (-SSIZE_MAX - 1)
	#endif
#endif

/* Missing minimums added by JACLibc for consistency */
#define _MINIMUM_PASS_MAX                    32
#define _MINIMUM_ATEXIT_MAX                  32

/* ============================================================= */
/* Resource Limits (POSIX‑enforced)                              */
/* ============================================================= */

#ifndef AIO_LISTIO_MAX
	#define AIO_LISTIO_MAX 64
#elif AIO_LISTIO_MAX < _POSIX_AIO_LISTIO_MAX
	#undef AIO_LISTIO_MAX
	#define AIO_LISTIO_MAX _POSIX_AIO_LISTIO_MAX
#endif

#ifndef AIO_MAX
	#define AIO_MAX 128
#elif AIO_MAX < _POSIX_AIO_MAX
	#undef AIO_MAX
	#define AIO_MAX _POSIX_AIO_MAX
#endif

#ifndef AIO_PRIO_DELTA_MAX
	#define AIO_PRIO_DELTA_MAX 0
#elif AIO_PRIO_DELTA_MAX < _POSIX_AIO_PRIO_DELTA_MAX
	#undef AIO_PRIO_DELTA_MAX
	#define AIO_PRIO_DELTA_MAX _POSIX_AIO_PRIO_DELTA_MAX
#endif

#ifndef ATEXIT_MAX
	#define ATEXIT_MAX 32
#elif ATEXIT_MAX < _MINIMUM_ATEXIT_MAX
	#undef ATEXIT_MAX
	#define ATEXIT_MAX 32
#endif

#ifndef CHILD_MAX
	#define CHILD_MAX 999
#elif CHILD_MAX < _POSIX_CHILD_MAX
	#undef CHILD_MAX
	#define CHILD_MAX _POSIX_CHILD_MAX
#endif

#ifndef DELAYTIMER_MAX
	#define DELAYTIMER_MAX 32
#elif DELAYTIMER_MAX < _POSIX_DELAYTIMER_MAX
	#undef DELAYTIMER_MAX
	#define DELAYTIMER_MAX _POSIX_DELAYTIMER_MAX
#endif

#ifndef NGROUPS_MAX
	#define NGROUPS_MAX 64
#elif NGROUPS_MAX < _POSIX_NGROUPS_MAX
	#undef NGROUPS_MAX
	#define NGROUPS_MAX _POSIX_NGROUPS_MAX
#endif

#ifndef STREAM_MAX
	#define STREAM_MAX 16
#elif STREAM_MAX < _POSIX_STREAM_MAX
	#undef STREAM_MAX
	#define STREAM_MAX _POSIX_STREAM_MAX
#endif

#ifndef LINE_MAX
	#define LINE_MAX 2048
#elif LINE_MAX < _POSIX2_LINE_MAX
	#undef LINE_MAX
	#define LINE_MAX _POSIX2_LINE_MAX
#endif

#ifndef TZNAME_MAX
	#define TZNAME_MAX 256
#elif TZNAME_MAX < _POSIX_TZNAME_MAX
	#undef TZNAME_MAX
	#define TZNAME_MAX _POSIX_TZNAME_MAX
#endif

#ifndef SEM_VALUE_MAX
	#define SEM_VALUE_MAX 32767
#elif SEM_VALUE_MAX < _POSIX_SEM_VALUE_MAX
	#undef SEM_VALUE_MAX
	#define SEM_VALUE_MAX _POSIX_SEM_VALUE_MAX
#endif

#ifndef SEM_NSEMS_MAX
	#define SEM_NSEMS_MAX 256
#elif SEM_NSEMS_MAX < _POSIX_SEM_NSEMS_MAX
	#undef SEM_NSEMS_MAX
	#define SEM_NSEMS_MAX _POSIX_SEM_NSEMS_MAX
#endif

#ifndef SIGQUEUE_MAX
	#define SIGQUEUE_MAX 32
#elif SIGQUEUE_MAX < _POSIX_SIGQUEUE_MAX
	#undef SIGQUEUE_MAX
	#define SIGQUEUE_MAX _POSIX_SIGQUEUE_MAX
#endif

#ifndef RTSIG_MAX
	#define RTSIG_MAX 8
#elif RTSIG_MAX < _POSIX_RTSIG_MAX
	#undef RTSIG_MAX
	#define RTSIG_MAX _POSIX_RTSIG_MAX
#endif

#ifndef HOST_NAME_MAX
		#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#elif HOST_NAME_MAX < _POSIX_HOST_NAME_MAX
		#undef HOST_NAME_MAX
		#define HOST_NAME_MAX _POSIX_HOST_NAME_MAX
#endif

#ifndef TIMER_MAX
		#define TIMER_MAX _POSIX_TIMER_MAX
#elif TIMER_MAX < _POSIX_TIMER_MAX
		#undef TIMER_MAX
		#define TIMER_MAX _POSIX_TIMER_MAX
#endif

#ifndef TTY_NAME_MAX
		#define TTY_NAME_MAX _POSIX_TTY_NAME_MAX
#elif TTY_NAME_MAX < _POSIX_TTY_NAME_MAX
		#undef TTY_NAME_MAX
		#define TTY_NAME_MAX _POSIX_TTY_NAME_MAX
#endif

#ifndef BC_BASE_MAX
		#define BC_BASE_MAX _POSIX2_BC_BASE_MAX
#endif

#ifndef BC_DIM_MAX
		#define BC_DIM_MAX _POSIX2_BC_DIM_MAX
#endif

#ifndef BC_SCALE_MAX
		#define BC_SCALE_MAX _POSIX2_BC_SCALE_MAX
#endif

#ifndef BC_STRING_MAX
		#define BC_STRING_MAX _POSIX2_BC_STRING_MAX
#endif

#ifndef CHARCLASS_NAME_MAX
		#define CHARCLASS_NAME_MAX _POSIX2_CHARCLASS_NAME_MAX
#endif

#ifndef COLL_WEIGHTS_MAX
		#define COLL_WEIGHTS_MAX _POSIX2_COLL_WEIGHTS_MAX
#endif

#ifndef EXPR_NEST_MAX
		#define EXPR_NEST_MAX _POSIX2_EXPR_NEST_MAX
#endif

#ifndef MQ_OPEN_MAX
	#define MQ_OPEN_MAX 128
#elif MQ_OPEN_MAX < _POSIX_MQ_OPEN_MAX
	#undef MQ_OPEN_MAX
	#define MQ_OPEN_MAX _POSIX_MQ_OPEN_MAX
#endif

#ifndef MQ_PRIO_MAX
	#define MQ_PRIO_MAX 32
#elif MQ_PRIO_MAX < _POSIX_MQ_PRIO_MAX
	#undef MQ_PRIO_MAX
	#define MQ_PRIO_MAX _POSIX_MQ_PRIO_MAX
#endif

#ifndef IOV_MAX
	#define IOV_MAX 1024
#elif IOV_MAX < _XOPEN_IOV_MAX
	#undef IOV_MAX
	#define IOV_MAX _XOPEN_IOV_MAX
#endif

/* ============================================================= */
/* System Limits (POSIX‑enforced)                                */
/* ============================================================= */

#ifndef NAME_MAX
	#define NAME_MAX 255
#elif NAME_MAX < _POSIX_NAME_MAX
	#undef NAME_MAX
	#define NAME_MAX _POSIX_NAME_MAX
#endif

#ifndef PATH_MAX
	#define PATH_MAX 4096
#elif PATH_MAX < _POSIX_PATH_MAX
	#undef PATH_MAX
	#define PATH_MAX _POSIX_PATH_MAX
#endif

#ifndef FILESIZEBITS
	#define FILESIZEBITS 32
#endif

#ifndef SYMLINK_MAX
	#define SYMLINK_MAX _POSIX_SYMLINK_MAX
#elif SYMLINK_MAX < _POSIX_SYMLINK_MAX
	#undef SYMLINK_MAX
	#define SYMLINK_MAX _POSIX_SYMLINK_MAX
#endif

#ifndef SYMLOOP_MAX
	#define SYMLOOP_MAX _POSIX_SYMLOOP_MAX
#elif SYMLOOP_MAX < _POSIX_SYMLOOP_MAX
	#undef SYMLOOP_MAX
	#define SYMLOOP_MAX _POSIX_SYMLOOP_MAX
#endif

#ifndef FILENAME_MAX
	#define FILENAME_MAX PATH_MAX
#endif

#ifndef ARG_MAX
	#define ARG_MAX 131072
#elif ARG_MAX < _POSIX_ARG_MAX
	#undef ARG_MAX
	#define ARG_MAX _POSIX_ARG_MAX
#endif

#ifndef OPEN_MAX
	#define OPEN_MAX 1024
#elif OPEN_MAX < _POSIX_OPEN_MAX
	#undef OPEN_MAX
	#define OPEN_MAX _POSIX_OPEN_MAX
#endif

#ifndef PIPE_BUF
	#define PIPE_BUF 4096
#elif PIPE_BUF < _POSIX_PIPE_BUF
	#undef PIPE_BUF
	#define PIPE_BUF _POSIX_PIPE_BUF
#endif

#ifndef LINK_MAX
	#define LINK_MAX 32
#elif LINK_MAX < _POSIX_LINK_MAX
	#undef LINK_MAX
	#define LINK_MAX _POSIX_LINK_MAX
#endif

#ifndef MAX_CANON
	#define MAX_CANON 255
#elif MAX_CANON < _POSIX_MAX_CANON
	#undef MAX_CANON
	#define MAX_CANON _POSIX_MAX_CANON
#endif

#ifndef MAX_INPUT
	#define MAX_INPUT 255
#elif MAX_INPUT < _POSIX_MAX_INPUT
	#undef MAX_INPUT
	#define MAX_INPUT _POSIX_MAX_INPUT
#endif

#ifndef LOGIN_NAME_MAX
	#define LOGIN_NAME_MAX 32
#elif LOGIN_NAME_MAX < _POSIX_LOGIN_NAME_MAX
	#undef LOGIN_NAME_MAX
	#define LOGIN_NAME_MAX _POSIX_LOGIN_NAME_MAX
#endif

#ifndef RE_DUP_MAX
	#define RE_DUP_MAX 255
#elif RE_DUP_MAX < _POSIX2_RE_DUP_MAX
	#undef RE_DUP_MAX
	#define RE_DUP_MAX _POSIX2_RE_DUP_MAX
#endif

#ifndef PASS_MAX
	#define PASS_MAX 256
#elif PASS_MAX < _MINIMUM_PASS_MAX
	#undef PASS_MAX
	#define PASS_MAX _MINIMUM_PASS_MAX
#endif

/* ============================================================= */
/* Other Invariant Limits (mostly XSI)                           */
/* ============================================================= */

#define NL_ARGMAX                   9   /* max n in "%n$" conversion specification */
#define NL_LANGMAX                 14   /* max bytes in a LANG name */
#define NL_MSGMAX               32767   /* max message number */
#define NL_SETMAX                 255   /* max set number */
#define NL_TEXTMAX   _POSIX2_LINE_MAX   /* max bytes in a message string */
#define NZERO                      20   /* default process priority (nice(2) baseline) */

/* Issue 8 invariant values (Austin Group Defects 741, 1122, 1134) */
#define GETENTROPY_MAX               256   /* max length arg to getentropy() */
#define TEXTDOMAIN_MAX  (_POSIX_NAME_MAX - 3)  /* max text domain name length (w/o null) */

/* Issue 8: NSIG_MAX (Defect 741) - signals representable by sigset_t */
#ifndef NSIG_MAX
	#define NSIG_MAX 64
#endif

/* Threading limits (POSIX.1-2024) */
#ifndef PTHREAD_DESTRUCTOR_ITERATIONS
	#define PTHREAD_DESTRUCTOR_ITERATIONS _POSIX_THREAD_DESTRUCTOR_ITERATIONS
#endif
#ifndef PTHREAD_KEYS_MAX
	#define PTHREAD_KEYS_MAX _POSIX_THREAD_KEYS_MAX
#endif
#ifndef PTHREAD_STACK_MIN
	#define PTHREAD_STACK_MIN 0
#endif
#ifndef PTHREAD_THREADS_MAX
	#define PTHREAD_THREADS_MAX _POSIX_THREAD_THREADS_MAX
#endif

/* Page size (XSI: PAGESIZE and PAGE_SIZE must be equivalent) */
#ifndef PAGESIZE
	#define PAGESIZE 4096
#endif

#ifndef PAGE_SIZE
	#define PAGE_SIZE PAGESIZE
#endif

#endif /* _LIMITS_H */
