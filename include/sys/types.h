/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H
#pragma once

#include <config.h>
#include <stdint.h>
#include <limits.h>
#include <x/locale_languages.h>
#include <x/locale_countries.h>

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

/* Dirent types */
typedef unsigned short  reclen_t;

/* Timing types */
typedef int timer_t;

#if JACL_BITS < 32
typedef long             clockid_t;     /* Clock identifier */
typedef unsigned long    useconds_t;    /* Microseconds (0 to 1000000) */
typedef long             suseconds_t;   /* Signed microseconds */
#else
typedef int             clockid_t;     /* Clock identifier */
typedef unsigned int    useconds_t;    /* Microseconds (0 to 1000000) */
typedef int             suseconds_t;   /* Signed microseconds */
#endif

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

/* size_t  in <limits.h> for SIZE_MAX */
/* ssize_t in <limits.h> to mirror size_t */

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

/* ============================================================= */
/* Pthread types - operating system aware                        */
/* ============================================================= */

struct __jacl_cleanup_node;
struct __jacl_pthread;

typedef struct __jacl_cleanup_node {
	void (*routine)(void *);
	void *arg;
	struct __jacl_cleanup_node *next;
} __jacl_cleanup_node_t;

/* Pthread types - Platform Specific */
#if JACL_OS_WINDOWS
	typedef struct { void *handle; unsigned int id; void *result; } pthread_t;
	typedef struct { void *cs; int type; } pthread_mutex_t;
	typedef struct { void *cv; } pthread_cond_t;
	typedef unsigned long pthread_key_t;
	typedef struct { int dummy; } pthread_spinlock_t;
	typedef struct { int dummy; } pthread_rwlock_t;
	typedef struct { int dummy; } pthread_barrier_t;
	typedef struct { int pshared; } pthread_barrierattr_t;
	typedef struct { int pshared; } pthread_rwlockattr_t;
#elif JACL_HAS_PTHREADS
	struct __jacl_pthread {
		pid_t tid;
		void *stack;
		size_t stack_size;
		int stack_is_mmap;
		void *result;
		_Atomic int finished;
		_Atomic int detached;
		_Atomic int joined;
		_Atomic int cancel_state;
		_Atomic int cancel_type;
		_Atomic int canceled;
		__jacl_cleanup_node_t *cleanup_stack;
	};

	typedef struct __jacl_pthread *pthread_t;

	typedef struct {
		_Atomic int futex;
		pid_t owner;
		int type;
		int recursive_count;
	} pthread_mutex_t;

	typedef struct {
		_Atomic int futex;
		_Atomic int waiters;
	} pthread_cond_t;

	typedef unsigned int pthread_key_t;

	typedef struct {
		_Atomic int lock;
	} pthread_spinlock_t;

	typedef struct {
		_Atomic int readers;
		_Atomic int writers;
		_Atomic int write_waiters;
		pid_t       writer_tid;
	} pthread_rwlock_t;

	typedef struct {
		_Atomic int count;
		_Atomic int phase;
		unsigned    limit;
		_Atomic int waiters;
	} pthread_barrier_t;

	typedef struct {
		int pshared;
	} pthread_barrierattr_t;

	typedef struct {
		int pshared;
	} pthread_rwlockattr_t;
#else /* dummy threads */
	typedef struct { int dummy; } pthread_t;
	typedef struct { int dummy; } pthread_mutex_t;
	typedef struct { int dummy; } pthread_cond_t;
	typedef struct { int dummy; } pthread_spinlock_t;
	typedef struct { int dummy; } pthread_rwlock_t;
	typedef struct { int dummy; } pthread_barrier_t;
	typedef struct { int dummy; } pthread_barrierattr_t;
	typedef struct { int dummy; } pthread_rwlockattr_t;
	typedef int pthread_key_t;
#endif

/* POSIX timespec structure */
	typedef struct timespec {
			time_t tv_sec;   /* Seconds */
				long  tv_nsec;   /* Nanoseconds */
	} timespec;

/* Standard tm structure */
typedef struct tm {
	int tm_sec;    /* Seconds (0-60) */
	int tm_min;    /* Minutes (0-59) */
	int tm_hour;   /* Hours (0-23) */
	int tm_mday;   /* Day of month (1-31) */
	int tm_mon;    /* Month (0-11) */
	int tm_year;   /* Years since 1900 */
	int tm_wday;   /* Day of week (0-6, Sunday = 0) */
	int tm_yday;   /* Day of year (0-365) */
	int tm_isdst;  /* Daylight saving time flag */
} tm;

/* POSIX timer structure */
typedef struct itimerspec {
	struct timespec it_interval;
	struct timespec it_value;
} itimerspec;

/* Scheduling parameter structure */
struct sched_param {
	int sched_priority;                   /* Process execution scheduling priority */
	int sched_ss_low_priority;            /* Low priority for sporadic server */
	struct timespec sched_ss_repl_period; /* Replenishment period */
	struct timespec sched_ss_init_budget; /* Initial budget */
	int sched_ss_max_repl;                /* Max pending replenishments */
};

/* Common Pthread Attributes */
typedef struct {
	int detached;
	size_t stack_size;
	void *stack_addr;
	struct sched_param sched_param;
	int policy;
} pthread_attr_t;

typedef struct {
	int type;
	int robust;
	int pshared;
} pthread_mutexattr_t;

typedef struct {
	int pshared;
	clockid_t clock_id;
} pthread_condattr_t;

typedef struct {
	_Atomic int done;
} pthread_once_t;

/* Window size structure for terminal ioctls */
struct winsize {
	unsigned short ws_row;      /* Rows, in characters */
	unsigned short ws_col;      /* Columns, in characters */
	unsigned short ws_xpixel;   /* Horizontal size, pixels */
	unsigned short ws_ypixel;   /* Vertical size, pixels */
};

/* ============================================================= */
/* Locale types - and comparison primitives                      */
/* ============================================================= */

#ifndef LOCALE_MINIMUM
#define LOCALE_MINIMUM UINT_MAX
#endif

#define X(code, ...) code,
typedef enum locale_lang { X_LANGUAGES LANG_ERR } __jacl_locale_lang_t;
typedef enum locale_cc { X_COUNTRIES CC_ERR } __jacl_locale_cc_t;
#undef X

typedef struct {
	wchar_t d[10];   // Digits
	wchar_t u[256];  // Upper
	wchar_t l[256];  // Lower
	wchar_t e[256];  // Extra
	uint8_t u_count; // Upper Count
	uint8_t l_count; // Lower Count
	uint8_t e_count; // Extra Count
} __jacl_wctype_t;

typedef struct {
	__jacl_locale_lang_t  wctype;
	__jacl_locale_lang_t  time;
	__jacl_locale_lang_t  collate;
	__jacl_locale_lang_t  message;
	__jacl_locale_cc_t    numeric;
	__jacl_locale_cc_t    monetary;
} __jacl_lcats_t;

typedef struct {
	const char *days_full[7];
	const char *days_abbr[7];
	const char *months_full[12];
	const char *months_abbr[12];
	const char *am;
	const char *pm;
	const char *fmt_datetime;
	const char *fmt_date;
	const char *fmt_time_24;
	const char *fmt_time_12;
} __jacl_time_t;

typedef struct lconv {
	/* numeric */
	char	*decimal_point;
	char	*thousands_sep;
	char	*grouping;

	/* monetary */
	char	*int_curr_symbol;
	char	*currency_symbol;
	char	*mon_decimal_point;
	char	*mon_thousands_sep;
	char	*mon_grouping;
	char	*positive_sign;
	char	*negative_sign;

	char	frac_digits;
	char	p_cs_precedes;
	char	n_cs_precedes;
	char	p_sep_by_space;
	char	n_sep_by_space;
	char	p_sign_posn;
	char	n_sign_posn;

	char	int_frac_digits;
	char	int_p_cs_precedes;
	char	int_n_cs_precedes;
	char	int_p_sep_by_space;
	char	int_n_sep_by_space;
	char	int_p_sign_posn;
	char	int_n_sign_posn;
} __jacl_lconv_t;

typedef struct {
	wchar_t sort[96];
	wchar_t fold_from[48];
	wchar_t fold_to[48];
	uint8_t sort_count;
	uint8_t fold_count;
} __jacl_collate_t;

typedef struct {
	__jacl_lcats_t   lcats;   // lang/cc category
	__jacl_wctype_t  wctype;  // your tables
	__jacl_time_t    time;    // your time data
	__jacl_lconv_t   lconv;   // numeric/monetary
	__jacl_collate_t collate; // collation
} __jacl_locale_t;

typedef __jacl_locale_t *locale_t;

extern locale_t __jacl_locale_global;
extern thread_local locale_t __jacl_locale_current;

static inline uint_least32_t __jacl_compare_weight(const __jacl_collate_t *coll, wchar_t c) {
	wchar_t fc = c;

	for (uint8_t i = 0; i < coll->fold_count; i++) {
		if (coll->fold_from[i] == c) {
			fc = coll->fold_to[i];

			break;
		}
	}

	if (coll->sort_count > 0) {
		for (uint8_t i = 0; i < coll->sort_count; i++) {
			if (coll->sort[i] == fc) return (uint_least32_t)i;
		}

		return (uint_least32_t)(uint32_t)fc + (uint_least32_t)coll->sort_count;
	}

	return (uint_least32_t)(uint32_t)fc;
}

static inline wchar_t __jacl_compare_decode(const char **s) {
	const unsigned char *p = (const unsigned char *)*s;
	wchar_t cp = *p++;

	if (cp < 0x80) {
		*s = (const char *)p;

		return cp;
	}

	if ((cp & 0xE0) == 0xC0) {
		cp = ((cp & 0x1F) << 6) | (*p++ & 0x3F);
	} else if ((cp & 0xF0) == 0xE0) {
		cp = ((cp & 0x0F) << 12) | ((*p++ & 0x3F) << 6) | (*p++ & 0x3F);
	} else if ((cp & 0xF8) == 0xF0) {
		cp = ((cp & 0x07) << 18) | ((*p++ & 0x3F) << 12) | ((*p++ & 0x3F) << 6) | (*p++ & 0x3F);
	} else {
		/* Invalid UTF-8: return raw byte and advance */
		*s = (const char *)p;

		return cp;
	}

	*s = (const char *)p;

	return cp;
}

static inline int __jacl_compare_glyph(const __jacl_collate_t *lang, const wchar_t a, const wchar_t b) {
	uint_least32_t wa = __jacl_compare_weight(lang, a);
	uint_least32_t wb = __jacl_compare_weight(lang, b);

	return (wa > wb) - (wa < wb);
}

#ifdef __cplusplus
}
#endif

#endif /* _SYS_TYPES_H */
