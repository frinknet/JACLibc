/* (c) 2026 FRINKnet & Friends – MIT licence */

#define _POSIX_VERSION      202405L
#define _POSIX2_VERSION     202405L
#define _XOPEN_VERSION      800

/* ============================================================= */
/* POSIX MINIMUM DEFINITIONS                                     */
/* ============================================================= */

/* Minimums for asynchronous I/O operations */
#define _POSIX_AIO_LISTIO_MAX                 2   /* number of I/O ops in a list I/O call */
#define _POSIX_AIO_MAX                        1   /* number of outstanding async I/O ops */
#define _POSIX_AIO_PRIO_DELTA_MAX             0   /* max decrease of async I/O priority */

/* arg max for exec / environment data length */
#define _POSIX_ARG_MAX                     4096   /* maximum length of exec args + env */

/* Processes and timers */
#define _POSIX_CHILD_MAX                     25   /* max simultaneous processes per real UID */
#define _POSIX_DELAYTIMER_MAX                32   /* timer expiration overruns */
#define _POSIX_TIMER_MAX                     32   /* per‑process number of timers */
#define _POSIX_CLOCKRES_MIN            20000000   /* CLOCK_REALTIME/MONOTONIC resolution (ns) */

/* Network and host‑name limits */
#define _POSIX_HOST_NAME_MAX                255   /* max bytes in host name (w/o null) */

/* File and link characteristics */
#define _POSIX_LINK_MAX                       8   /* max number of hard links to a file */
#define _POSIX_SYMLINK_MAX                  255   /* bytes in a symbolic link */
#define _POSIX_SYMLOOP_MAX                    8   /* max symlinks traversed (no loop) */

/* Terminal input limits */
#define _POSIX_MAX_CANON                    255   /* max bytes in a terminal canonical input queue */
#define _POSIX_MAX_INPUT                    255   /* max bytes in a terminal input queue */

/* File pathname / name limits */
#define _POSIX_NAME_MAX                      14   /* max bytes in a filename (w/o null) */
#define _POSIX_PATH_MAX                     256   /* min bytes the system accepts for pathname length */

/* POSIX‑style I/O properties */
#define _POSIX_OPEN_MAX                      20   /* one greater than max new file descriptor number */
#define _POSIX_PIPE_BUF                     512   /* max bytes guaranteed atomic when writing to a pipe */

	/* Supplementary groups */
#define _POSIX_NGROUPS_MAX                    8   /* max supplementary group IDs per process */

/* POSIX‑style software limits */
#define _POSIX_RE_DUP_MAX                   255   /* max repeated occurrences in a BRE {\(m,n\)} */
#define _POSIX2_EXPR_NEST_MAX                32   /* max nested expressions in an expr(1) parentheses chain */
#define _POSIX2_LINE_MAX                   2048   /* max bytes in a line for POSIX utilities (incl \n) */
#define _POSIX2_CHARCLASS_NAME_MAX           14   /* max bytes in a character class name */
#define _POSIX2_COLL_WEIGHTS_MAX              2   /* max weights per LC_COLLATE order entry */
#define _POSIX2_RE_DUP_MAX                  255   /* max repeated occurrences in a regex {\(m,n\)} */

/* Time‑zone and terminal name storage */
#define _POSIX_TZNAME_MAX                     6   /* max bytes for a timezone name (TZ var excluded) */
#define _POSIX_TTY_NAME_MAX                   9   /* bytes required to store a terminal device name (incl null) */

/* Threads and thread‑specific data */
#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS   4   /* attempts to destroy thread‑specific data on exit */
#define _POSIX_THREAD_KEYS_MAX              128   /* data keys per process (TSD keys) */
#define _POSIX_THREAD_THREADS_MAX            64   /* max threads per process */

/* POSIX‑style user‑visible SS/trace/string limits (rationale labels) */
#define _POSIX_SS_REPL_MAX                    4   /* replenishment ops pending for a sporadic server scheduler */
#define _POSIX_TRACE_EVENT_NAME_MAX          30   /* length of a trace event name (w/o null) */
#define _POSIX_TRACE_NAME_MAX                 8   /* length of a trace generation/stream name (w/o null) */
#define _POSIX_TRACE_SYS_MAX                  8   /* max simultaneously existing trace streams */
#define _POSIX_TRACE_USER_EVENT_MAX          32   /* max user trace event type IDs in a process */

/* POSIX system size constants (ssize_t) */
#define _POSIX_SSIZE_MAX                  32767   /* max value that can be stored in an ssize_t */

/* POSIX stream limits */
#define _POSIX_STREAM_MAX                     8   /* max streams one process can have open */

/* POSIX login‑name limit */
#define _POSIX_LOGIN_NAME_MAX                 9   /* bytes required for a login name (incl null) */

/* POSIX semaphore limits */
#define _POSIX_SEM_NSEMS_MAX                256   /* max semaphores a process may have */
#define _POSIX_SEM_VALUE_MAX              32767   /* max value a semaphore may have */

/* POSIX signal‑queue and realtime‑signal limits */
#define _POSIX_SIGQUEUE_MAX                  32   /* max queued signals pending at receiver(s) */
#define _POSIX_RTSIG_MAX                      8   /* realtime signal numbers reserved for application use */

/* POSIX message‑queue limits */
#define _POSIX_MQ_OPEN_MAX                    8   /* max message queues that can be open per process */
#define _POSIX_MQ_PRIO_MAX                   32   /* max number of message priorities supported */

/* POSIX page size and system limits */
#define _POSIX_PAGESIZE                    4096   /* memory management unit (MMU) page size in bytes */
#define _POSIX_CLK_TCK                      100   /* statistics clock ticks per second (for times/sysconf) */
#define _POSIX_ATEXIT_MAX                    32   /* maximum exit handlers registerable via atexit() */

/* I/O and Terminal minimums */
#define _POSIX_UIO_MAXIOV                    16   /* max iovec structures for readv/writev */
#define _POSIX_VDISABLE                       0    /* disable terminal special character handling */

/* POSIX‑2‑style mailbox / utility mailbox limits */
#define _POSIX2_RE_DUP_MAX                  255   /* repeated occurrences in a regex {\(m,n\)} (same as above) */
#define _POSIX2_LINE_MAX                   2048   /* input line length for POSIX utilities (same as above) */
#define _POSIX2_BC_DIM_MAX                 2048   /* array size for bc(1) (same as above) */
#define _POSIX2_BC_BASE_MAX                  99   /* max obase value for bc(1) */
#define _POSIX2_BC_SCALE_MAX                 99   /* scale for bc(1) (same as above) */
#define _POSIX2_BC_STRING_MAX              1000   /* string length for bc(1) (same as above) */
#define _POSIX2_EXPR_NEST_MAX                32   /* expressions nested in parentheses for expr(1) (same as above) */

/* X/Open / XSI‑style aliases (if you want to expose XSI) */
#define _XOPEN_IOV_MAX                       16   /* max iovec structures for readv/writev */
#define _XOPEN_NAME_MAX                     255   /* max bytes in a filename (XSI variant of NAME_MAX) */
#define _XOPEN_PATH_MAX                    1024   /* min bytes the system accepts for pathname length (XSI variant) */

/* ============================================================================ */
/* POSIX UNISTD DEFINITIONS                                                     */
/* ============================================================================ */

#define _POSIX_ADVISORY_INFO               202405L
#define _POSIX_ASYNCHRONOUS_IO             202405L
#define _POSIX_BARRIERS                    202405L
#define _POSIX_CLOCK_SELECTION             202405L
#define _POSIX_CPUTIME                     202405L
#define _POSIX_DEVICE_CONTROL              202405L
#define _POSIX_FSYNC                       202405L
#define _POSIX_IPV6                        202405L
#define _POSIX_MAPPED_FILES                202405L
#define _POSIX_MEMLOCK                     202405L
#define _POSIX_MEMLOCK_RANGE               202405L
#define _POSIX_MEMORY_PROTECTION           202405L
#define _POSIX_MESSAGE_PASSING             202405L
#define _POSIX_MONOTONIC_CLOCK             202405L
#define _POSIX_PRIORITIZED_IO              202405L
#define _POSIX_PRIORITY_SCHEDULING         202405L
#define _POSIX_RAW_SOCKETS                 202405L
#define _POSIX_READER_WRITER_LOCKS         202405L
#define _POSIX_REALTIME_SIGNALS            202405L
#define _POSIX_SEMAPHORES                  202405L
#define _POSIX_SHARED_MEMORY_OBJECTS       202405L
#define _POSIX_SPAWN                       202405L
#define _POSIX_SPIN_LOCKS                  202405L
#define _POSIX_SYNCHRONIZED_IO             202405L
#define _POSIX_THREAD_ATTR_STACKADDR       202405L
#define _POSIX_THREAD_ATTR_STACKSIZE       202405L
#define _POSIX_THREAD_CPUTIME              202405L
#define _POSIX_THREAD_PRIO_INHERIT         202405L
#define _POSIX_THREAD_PRIO_PROTECT         202405L
#define _POSIX_THREAD_PRIORITY_SCHEDULING  202405L
#define _POSIX_THREAD_PROCESS_SHARED       202405L
#define _POSIX_THREAD_ROBUST_PRIO_INHERIT  202405L
#define _POSIX_THREAD_ROBUST_PRIO_PROTECT  202405L
#define _POSIX_THREAD_SAFE_FUNCTIONS       202405L
#define _POSIX_THREADS                     202405L
#define _POSIX_TIMEOUTS                    202405L
#define _POSIX_TIMERS                      202405L

#define _POSIX2_C_BIND                     202405L
#define _POSIX2_C_DEV                      202405L
#define _POSIX2_LOCALEDEF                  202405L
#define _POSIX2_SW_DEV                     202405L
#define _POSIX2_UPE                        202405L
#define _POSIX2_SYMLINKS                   202405L

#define _POSIX_CHOWN_RESTRICTED            1
#define _POSIX_JOB_CONTROL                 1
#define _POSIX_NO_TRUNC                    1
#define _POSIX_REGEXP                      1
#define _POSIX_SAVED_IDS                   1
#define _POSIX_SHELL                       1

#define _POSIX_V7_ILP32_OFF32              1
#define _POSIX_V7_ILP32_OFFBIG             1
#define _POSIX_V7_LP64_OFF64               1
#define _POSIX_V7_LPBIG_OFFBIG             1

#define _POSIX_V8_ILP32_OFF32              1
#define _POSIX_V8_ILP32_OFFBIG             1
#define _POSIX_V8_LP64_OFF64               1
#define _POSIX_V8_LPBIG_OFFBIG             1

#define _POSIX2_CHAR_TERM                  1

#define _XOPEN_CRYPT                       1
#define _XOPEN_ENH_I18N                    1
#define _XOPEN_REALTIME                    1
#define _XOPEN_REALTIME_THREADS            1
#define _XOPEN_SHM                         1
#define _XOPEN_UNIX                        1

#define _POSIX_SPORADIC_SERVER             -1
#define _POSIX_THREAD_SPORADIC_SERVER      -1
#define _POSIX_TYPED_MEMORY_OBJECTS        -1

#define _POSIX2_FORT_RUN                   -1

#define _XOPEN_UUCP                        -1

/* ============================================================================ */
/* POSIX EXECUTION-TIME DEFINITION                                              */
/* ============================================================================ */

#define _POSIX_ASYNC_IO                    1
#define _POSIX_FALLOC                      1
#define _POSIX_PRIO_IO                     1
#define _POSIX_SYNC_IO                     1
#define _POSIX_TIMESTAMP_RESOLUTION        1

/* ============================================================================ */
/* UNISTD SYSCONF NAMES (_SC_*)                                                 */
/* ============================================================================ */

#define UNISTD_SYSCONF(X) \
	X(_SC_VERSION,                        1, _POSIX_VERSION) \
	X(_SC_PAGESIZE,                       2, _POSIX_PAGESIZE) \
	X(_SC_CLK_TCK,                        3, _POSIX_CLK_TCK) \
	X(_SC_ARG_MAX,                        4, _POSIX_ARG_MAX) \
	X(_SC_CHILD_MAX,                      5, _POSIX_CHILD_MAX) \
	X(_SC_OPEN_MAX,                       6, _POSIX_OPEN_MAX) \
	X(_SC_NGROUPS_MAX,                    7, _POSIX_NGROUPS_MAX) \
	X(_SC_JOB_CONTROL,                    8, _POSIX_JOB_CONTROL) \
	X(_SC_SAVED_IDS,                      9, _POSIX_SAVED_IDS) \
	X(_SC_STREAM_MAX,                    10, _POSIX_STREAM_MAX) \
	X(_SC_TZNAME_MAX,                    11, _POSIX_TZNAME_MAX) \
	X(_SC_HOST_NAME_MAX,                 12, _POSIX_HOST_NAME_MAX) \
	X(_SC_AIO_LISTIO_MAX,                23, _POSIX_AIO_LISTIO_MAX) \
	X(_SC_AIO_MAX,                       24, _POSIX_AIO_MAX) \
	X(_SC_AIO_PRIO_DELTA_MAX,            25, _POSIX_AIO_PRIO_DELTA_MAX) \
	X(_SC_ASYNCHRONOUS_IO,               26, _POSIX_ASYNCHRONOUS_IO) \
	X(_SC_COLL_WEIGHTS_MAX,              27, _POSIX2_COLL_WEIGHTS_MAX) \
	X(_SC_DELAYTIMER_MAX,                28, _POSIX_DELAYTIMER_MAX) \
	X(_SC_MQ_OPEN_MAX,                   29, _POSIX_MQ_OPEN_MAX) \
	X(_SC_MQ_PRIO_MAX,                   30, _POSIX_MQ_PRIO_MAX) \
	X(_SC_RTSIG_MAX,                     31, _POSIX_RTSIG_MAX) \
	X(_SC_SEM_NSEMS_MAX,                 32, _POSIX_SEM_NSEMS_MAX) \
	X(_SC_SEM_VALUE_MAX,                 33, _POSIX_SEM_VALUE_MAX) \
	X(_SC_SIGQUEUE_MAX,                  34, _POSIX_SIGQUEUE_MAX) \
	X(_SC_TIMER_MAX,                     35, _POSIX_TIMER_MAX) \
	X(_SC_BC_BASE_MAX,                   36, _POSIX2_BC_BASE_MAX) \
	X(_SC_BC_DIM_MAX,                    37, _POSIX2_BC_DIM_MAX) \
	X(_SC_BC_SCALE_MAX,                  38, _POSIX2_BC_SCALE_MAX) \
	X(_SC_BC_STRING_MAX,                 39, _POSIX2_BC_STRING_MAX) \
	X(_SC_EXPR_NEST_MAX,                 40, _POSIX2_EXPR_NEST_MAX) \
	X(_SC_LINE_MAX,                      41, _POSIX2_LINE_MAX) \
	X(_SC_RE_DUP_MAX,                    42, _POSIX2_RE_DUP_MAX) \
	X(_SC_2_C_BIND,                      45, _POSIX2_C_BIND) \
	X(_SC_2_C_DEV,                       46, _POSIX2_C_DEV) \
	X(_SC_2_FORT_RUN,                    47, _POSIX2_FORT_RUN) \
	X(_SC_2_LOCALEDEF,                   48, _POSIX2_LOCALEDEF) \
	X(_SC_2_SW_DEV,                      49, _POSIX2_SW_DEV) \
	X(_SC_2_UPE,                         50, _POSIX2_UPE) \
	X(_SC_2_VERSION,                     51, _POSIX2_VERSION) \
	X(_SC_GETGR_R_SIZE_MAX,              69, -1) \
	X(_SC_GETPW_R_SIZE_MAX,              70, -1) \
	X(_SC_IOV_MAX,                       71, _POSIX_UIO_MAXIOV) \
	X(_SC_LOGIN_NAME_MAX,                72, _POSIX_LOGIN_NAME_MAX) \
	X(_SC_TTY_NAME_MAX,                  73, _POSIX_TTY_NAME_MAX) \
	X(_SC_NPROCESSORS_CONF,              83, -1) \
	X(_SC_NPROCESSORS_ONLN,              84, -1) \
	X(_SC_ATEXIT_MAX,                    87, _POSIX_ATEXIT_MAX) \
	X(_SC_2_CHAR_TERM,                   95, _POSIX2_CHAR_TERM) \
	X(_SC_ADVISORY_INFO,                130, _POSIX_ADVISORY_INFO) \
	X(_SC_BARRIERS,                     131, _POSIX_BARRIERS) \
	X(_SC_CLOCK_SELECTION,              137, _POSIX_CLOCK_SELECTION) \
	X(_SC_CPUTIME,                      138, _POSIX_CPUTIME) \
	X(_SC_FSYNC,                        139, _POSIX_FSYNC) \
	X(_SC_MAPPED_FILES,                 140, _POSIX_MAPPED_FILES) \
	X(_SC_MEMLOCK,                      141, _POSIX_MEMLOCK) \
	X(_SC_MEMLOCK_RANGE,                142, _POSIX_MEMLOCK_RANGE) \
	X(_SC_MESSAGE_PASSING,              143, _POSIX_MESSAGE_PASSING) \
	X(_SC_PRIORITIZED_IO,               144, _POSIX_PRIORITIZED_IO) \
	X(_SC_PRIORITY_SCHEDULING,          145, _POSIX_PRIORITY_SCHEDULING) \
	X(_SC_REALTIME_SIGNALS,             146, _POSIX_REALTIME_SIGNALS) \
	X(_SC_SHARED_MEMORY_OBJECTS,        147, _POSIX_SHARED_MEMORY_OBJECTS) \
	X(_SC_SPAWN,                        148, _POSIX_SPAWN) \
	X(_SC_SPORADIC_SERVER,              149, _POSIX_SPORADIC_SERVER) \
	X(_SC_SYMLOOP_MAX,                  150, _POSIX_SYMLOOP_MAX) \
	X(_SC_SYNCHRONIZED_IO,              151, _POSIX_SYNCHRONIZED_IO) \
	X(_SC_THREAD_ATTR_STACKADDR,        152, _POSIX_THREAD_ATTR_STACKADDR) \
	X(_SC_THREAD_ATTR_STACKSIZE,        153, _POSIX_THREAD_ATTR_STACKSIZE) \
	X(_SC_THREAD_CPUTIME,               154, _POSIX_THREAD_CPUTIME) \
	X(_SC_THREAD_DESTRUCTOR_ITERATIONS, 155, _POSIX_THREAD_DESTRUCTOR_ITERATIONS) \
	X(_SC_THREAD_KEYS_MAX,              156, _POSIX_THREAD_KEYS_MAX) \
	X(_SC_THREAD_PRIO_INHERIT,          157, _POSIX_THREAD_PRIO_INHERIT) \
	X(_SC_THREAD_PRIO_PROTECT,          158, _POSIX_THREAD_PRIO_PROTECT) \
	X(_SC_THREAD_PRIORITY_SCHEDULING,   159, _POSIX_THREAD_PRIORITY_SCHEDULING) \
	X(_SC_THREAD_PROCESS_SHARED,        160, _POSIX_THREAD_PROCESS_SHARED) \
	X(_SC_THREAD_SPORADIC_SERVER,       161, _POSIX_THREAD_SPORADIC_SERVER) \
	X(_SC_THREAD_STACK_MIN,             162, -1) \
	X(_SC_THREAD_THREADS_MAX,           163, _POSIX_THREAD_THREADS_MAX) \
	X(_SC_TYPED_MEMORY_OBJECTS,         164, _POSIX_TYPED_MEMORY_OBJECTS) \
	X(_SC_MEMORY_PROTECTION,            165, _POSIX_MEMORY_PROTECTION) \
	X(_SC_MONOTONIC_CLOCK,              166, _POSIX_MONOTONIC_CLOCK) \
	X(_SC_READER_WRITER_LOCKS,          167, _POSIX_READER_WRITER_LOCKS) \
	X(_SC_REGEXP,                       168, _POSIX_REGEXP) \
	X(_SC_SEMAPHORES,                   169, _POSIX_SEMAPHORES) \
	X(_SC_V8_ILP32_OFF32,               170, _POSIX_V8_ILP32_OFF32) \
	X(_SC_V8_ILP32_OFFBIG,              171, _POSIX_V8_ILP32_OFFBIG) \
	X(_SC_V8_LP64_OFF64,                172, _POSIX_V8_LP64_OFF64) \
	X(_SC_V8_LPBIG_OFFBIG,              173, _POSIX_V8_LPBIG_OFFBIG) \
	X(_SC_XOPEN_CRYPT,                  174, _XOPEN_CRYPT) \
	X(_SC_XOPEN_ENH_I18N,               175, _XOPEN_ENH_I18N) \
	X(_SC_XOPEN_REALTIME,               176, _XOPEN_REALTIME) \
	X(_SC_XOPEN_REALTIME_THREADS,       177, _XOPEN_REALTIME_THREADS) \
	X(_SC_XOPEN_SHM,                    178, _XOPEN_SHM) \
	X(_SC_XOPEN_UNIX,                   179, _XOPEN_UNIX) \
	X(_SC_XOPEN_UUCP,                   180, _XOPEN_UUCP) \
	X(_SC_SHELL,                        181, _POSIX_SHELL) \
	X(_SC_SPIN_LOCKS,                   182, _POSIX_SPIN_LOCKS) \
	X(_SC_THREAD_SAFE_FUNCTIONS,        183, _POSIX_THREAD_SAFE_FUNCTIONS) \
	X(_SC_THREADS,                      184, _POSIX_THREADS) \
	X(_SC_TIMEOUTS,                     185, _POSIX_TIMEOUTS) \
	X(_SC_TIMERS,                       186, _POSIX_TIMERS) \
	X(_SC_XOPEN_VERSION,                187, _XOPEN_VERSION) \
	X(_SC_V7_ILP32_OFF32,               188, _POSIX_V7_ILP32_OFF32) \
	X(_SC_V7_ILP32_OFFBIG,              189, _POSIX_V7_ILP32_OFFBIG) \
	X(_SC_V7_LP64_OFF64,                190, _POSIX_V7_LP64_OFF64) \
	X(_SC_V7_LPBIG_OFFBIG,              191, _POSIX_V7_LPBIG_OFFBIG) \
	X(_SC_SS_REPL_MAX,                  241, _POSIX_SS_REPL_MAX) \
	X(_SC_DEVICE_CONTROL,               297, _POSIX_DEVICE_CONTROL) \
	X(_SC_IPV6,                         298, _POSIX_IPV6) \
	X(_SC_NSIG,                         299, -1) \
	X(_SC_RAW_SOCKETS,                  300, _POSIX_RAW_SOCKETS) \
	X(_SC_THREAD_ROBUST_PRIO_INHERIT,   301, _POSIX_THREAD_ROBUST_PRIO_INHERIT) \
	X(_SC_THREAD_ROBUST_PRIO_PROTECT,   302, _POSIX_THREAD_ROBUST_PRIO_PROTECT)

#define _SC_PAGE_SIZE _SC_PAGESIZE

/* ============================================================================ */
/* UNISTD PATHCONF NAMES (_PC_*)                                                */
/* ============================================================================ */

#define UNISTD_PATHCONF(X) \
	X(_PC_LINK_MAX,              0, _POSIX_LINK_MAX) \
	X(_PC_MAX_CANON,             1, _POSIX_MAX_CANON) \
	X(_PC_MAX_INPUT,             2, _POSIX_MAX_INPUT) \
	X(_PC_NAME_MAX,              3, _POSIX_NAME_MAX) \
	X(_PC_PATH_MAX,              4, _POSIX_PATH_MAX) \
	X(_PC_PIPE_BUF,              5, _POSIX_PIPE_BUF) \
	X(_PC_CHOWN_RESTRICTED,      6, _POSIX_CHOWN_RESTRICTED) \
	X(_PC_NO_TRUNC,              7, _POSIX_NO_TRUNC) \
	X(_PC_VDISABLE,              8, _POSIX_VDISABLE) \
	X(_PC_SYNC_IO,               9, _POSIX_SYNC_IO) \
	X(_PC_ASYNC_IO,             10, _POSIX_ASYNC_IO) \
	X(_PC_PRIO_IO,              11, _POSIX_PRIO_IO) \
	X(_PC_FILESIZEBITS,         13, -1) \
	X(_PC_REC_INCR_XFER_SIZE,   14, -1) \
	X(_PC_REC_MAX_XFER_SIZE,    15, -1) \
	X(_PC_REC_MIN_XFER_SIZE,    16, -1) \
	X(_PC_REC_XFER_ALIGN,       17, -1) \
	X(_PC_ALLOC_SIZE_MIN,       18, -1) \
	X(_PC_SYMLINK_MAX,          19, -1) \
	X(_PC_2_SYMLINKS,           20, _POSIX2_SYMLINKS) \
	X(_PC_FALLOC,               21, _POSIX_FALLOC) \
	X(_PC_TEXTDOMAIN_MAX,       22, -1) \
	X(_PC_TIMESTAMP_RESOLUTION, 23, _POSIX_TIMESTAMP_RESOLUTION)

/* ============================================================================ */
/* UNISTD CONFSTR NAMES (_CS_*)                                                 */
/* ============================================================================ */

#define UNISTD_CONFSTR(X) \
	X(_CS_PATH,                                1, "/bin:/usr/bin") \
	X(_CS_V8_ENV,                              2, "PATH=/bin:/usr/bin") \
	X(_CS_V7_ENV,                              3, "PATH=/bin:/usr/bin") \
	X(_CS_POSIX_V7_ILP32_OFF32_CFLAGS,      1100, "") \
	X(_CS_POSIX_V7_ILP32_OFF32_LDFLAGS,     1101, "") \
	X(_CS_POSIX_V7_ILP32_OFF32_LIBS,        1102, "") \
	X(_CS_POSIX_V7_ILP32_OFFBIG_CFLAGS,     1104, "") \
	X(_CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS,    1105, "") \
	X(_CS_POSIX_V7_ILP32_OFFBIG_LIBS,       1106, "") \
	X(_CS_POSIX_V7_LP64_OFF64_CFLAGS,       1108, "") \
	X(_CS_POSIX_V7_LP64_OFF64_LDFLAGS,      1109, "") \
	X(_CS_POSIX_V7_LP64_OFF64_LIBS,         1110, "") \
	X(_CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS,     1112, "") \
	X(_CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS,    1113, "") \
	X(_CS_POSIX_V7_LPBIG_OFFBIG_LIBS,       1114, "") \
	X(_CS_POSIX_V7_THREADS_CFLAGS,          1116, "-pthread") \
	X(_CS_POSIX_V7_THREADS_LDFLAGS,         1117, "-pthread") \
	X(_CS_POSIX_V7_WIDTH_RESTRICTED_ENVS,   1118, "") \
	X(_CS_POSIX_V8_ILP32_OFF32_CFLAGS,      1120, "") \
	X(_CS_POSIX_V8_ILP32_OFF32_LDFLAGS,     1121, "") \
	X(_CS_POSIX_V8_ILP32_OFF32_LIBS,        1122, "") \
	X(_CS_POSIX_V8_ILP32_OFFBIG_CFLAGS,     1124, "") \
	X(_CS_POSIX_V8_ILP32_OFFBIG_LDFLAGS,    1125, "") \
	X(_CS_POSIX_V8_ILP32_OFFBIG_LIBS,       1126, "") \
	X(_CS_POSIX_V8_LP64_OFF64_CFLAGS,       1128, "") \
	X(_CS_POSIX_V8_LP64_OFF64_LDFLAGS,      1129, "") \
	X(_CS_POSIX_V8_LP64_OFF64_LIBS,         1130, "") \
	X(_CS_POSIX_V8_LPBIG_OFFBIG_CFLAGS,     1132, "") \
	X(_CS_POSIX_V8_LPBIG_OFFBIG_LDFLAGS,    1133, "") \
	X(_CS_POSIX_V8_LPBIG_OFFBIG_LIBS,       1134, "") \
	X(_CS_POSIX_V8_THREADS_CFLAGS,          1136, "-pthread") \
	X(_CS_POSIX_V8_THREADS_LDFLAGS,         1137, "-pthread") \
	X(_CS_POSIX_V8_WIDTH_RESTRICTED_ENVS,   1138, "")
