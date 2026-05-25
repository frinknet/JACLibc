/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <limits.h>

TEST_TYPE(unit);
TEST_UNIT(limits.h);

/* ============================================================================
 * NUMERIC TYPE LIMITS
 * ============================================================================ */
TEST_SUITE(char_limits);

TEST(char_bit_size) { ASSERT_EQ(8, CHAR_BIT); }

TEST(schar_range) {
	ASSERT_EQ(-128, SCHAR_MIN);
	ASSERT_EQ(127, SCHAR_MAX);
}

TEST(uchar_range) { ASSERT_EQ(255, UCHAR_MAX); }

TEST(char_signedness) {
	ASSERT_TRUE(CHAR_MIN == 0 || CHAR_MIN == -128);
	ASSERT_TRUE(CHAR_MAX == 255 || CHAR_MAX == 127);
}

TEST(mb_len_max) {
	ASSERT_TRUE(MB_LEN_MAX >= 1);
	ASSERT_EQ(16, MB_LEN_MAX);
}

TEST_SUITE(short_limits);

TEST(shrt_range) {
	ASSERT_EQ(-32768, SHRT_MIN);
	ASSERT_EQ(32767, SHRT_MAX);
}

TEST(ushrt_range) { ASSERT_EQ(65535U, USHRT_MAX); }

TEST(short_size_consistency) {
	ASSERT_TRUE(sizeof(short) * CHAR_BIT >= 16);
}

TEST_SUITE(int_limits);

TEST(int_range) {
	ASSERT_EQ(-2147483648, INT_MIN);
	ASSERT_EQ(2147483647, INT_MAX);
}

TEST(uint_range) { ASSERT_EQ(4294967295U, UINT_MAX); }

TEST(int_size_consistency) {
	ASSERT_TRUE(sizeof(int) * CHAR_BIT >= 32);
}

TEST(int_boundary_values) {
	int min = INT_MIN;
	int max = INT_MAX;
	ASSERT_TRUE(min < 0);
	ASSERT_TRUE(max > 0);
	ASSERT_TRUE(max > min);
}

TEST_SUITE(long_limits);

TEST(long_range) {
	#ifdef __LP64__
		ASSERT_EQ(-9223372036854775807L-1, LONG_MIN);
		ASSERT_EQ(9223372036854775807L, LONG_MAX);
		ASSERT_EQ(18446744073709551615UL, ULONG_MAX);
	#else
		ASSERT_EQ(-2147483648L, LONG_MIN);
		ASSERT_EQ(2147483647L, LONG_MAX);
		ASSERT_EQ(4294967295UL, ULONG_MAX);
	#endif
}

TEST(long_size_consistency) {
	ASSERT_TRUE(sizeof(long) * CHAR_BIT >= 32);
}

TEST(long_vs_int) {
	ASSERT_TRUE(LONG_MAX >= INT_MAX);
	ASSERT_TRUE(ULONG_MAX >= UINT_MAX);
}

TEST_SUITE(llong_limits);

#if JACL_HAS_C99
TEST(llong_range) {
	ASSERT_EQ(-9223372036854775807LL-1, LLONG_MIN);
	ASSERT_EQ(9223372036854775807LL, LLONG_MAX);
	ASSERT_EQ(18446744073709551615ULL, ULLONG_MAX);
}

TEST(llong_size_consistency) {
	ASSERT_TRUE(sizeof(long long) * CHAR_BIT >= 64);
}

TEST(llong_vs_long) {
	ASSERT_TRUE(LLONG_MAX >= LONG_MAX);
	ASSERT_TRUE(ULLONG_MAX >= ULONG_MAX);
}
#endif

/* ============================================================================
 * TYPE HIERARCHY
 * ============================================================================ */
TEST_SUITE(type_hierarchy);

TEST(integer_size_progression) {
	ASSERT_TRUE(sizeof(char) <= sizeof(short));
	ASSERT_TRUE(sizeof(short) <= sizeof(int));
	ASSERT_TRUE(sizeof(int) <= sizeof(long));
	#if JACL_HAS_C99
	ASSERT_TRUE(sizeof(long) <= sizeof(long long));
	#endif
}

TEST(limit_consistency) {
	ASSERT_EQ(SCHAR_MAX, (UCHAR_MAX / 2));
	ASSERT_EQ(SHRT_MAX, (USHRT_MAX / 2));
	ASSERT_EQ(INT_MAX, (UINT_MAX / 2));
	ASSERT_EQ(LONG_MAX, (long)(ULONG_MAX / 2));
}

TEST(unsigned_max_values) {
	ASSERT_EQ(UCHAR_MAX, (unsigned char)~0);
	ASSERT_EQ(USHRT_MAX, (unsigned short)~0);
	ASSERT_EQ(UINT_MAX, (unsigned int)~0);
}

/* ============================================================================
 * ARITHMETIC BOUNDARIES
 * ============================================================================ */
TEST_SUITE(arithmetic_boundaries);

TEST(int_overflow_detection) {
	int max = INT_MAX;
	int min = INT_MIN;
	ASSERT_TRUE(max > 0);
	ASSERT_TRUE(min < 0);
}

TEST(unsigned_wraparound) {
	unsigned int max = UINT_MAX;
	ASSERT_EQ(0, max + 1);
}

/* ============================================================================
 * PRACTICAL USAGE
 * ============================================================================ */
TEST_SUITE(practical_usage);

TEST(safe_int_addition_check) {
	int a = 1000000;
	int b = 1000000;
	if (a > INT_MAX - b) {
		ASSERT_TRUE(0);
	} else {
		int sum = a + b;
		ASSERT_EQ(2000000, sum);
	}
}

TEST(type_range_validation) {
	char c = CHAR_MAX;
	short s = SHRT_MAX;
	int i = INT_MAX;
	long l = LONG_MAX;
	ASSERT_EQ(CHAR_MAX, c);
	ASSERT_EQ(SHRT_MAX, s);
	ASSERT_EQ(INT_MAX, i);
	ASSERT_EQ(LONG_MAX, l);
}

/* ============================================================================
 * COMPILE-TIME VALIDATION
 * ============================================================================ */
TEST_SUITE(compile_time);

TEST(static_assertions) {
	static_assert(CHAR_BIT == 8, "char must be 8 bits");
	static_assert(sizeof(short) >= 2, "short must be at least 16 bits");
	static_assert(sizeof(int) >= 4, "int must be at least 32 bits");
	static_assert(sizeof(long) >= 4, "long must be at least 32 bits");
	ASSERT_TRUE(1);
}

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(zero_handling) {
	int zero = 0;
	ASSERT_TRUE(zero < INT_MAX);
	ASSERT_TRUE(zero > INT_MIN);
}

/* ============================================================================
 * POSIX MINIMUM ACCEPTABLE VALUES
 * These *_MAX constants define MINIMUM acceptable values per POSIX spec
 * ============================================================================ */
TEST_SUITE(posix_minimums);

TEST(posix_minimums_io) {
	ASSERT_EQ(2, _POSIX_AIO_LISTIO_MAX);
	ASSERT_EQ(1, _POSIX_AIO_MAX);
	ASSERT_EQ(0, _POSIX_AIO_PRIO_DELTA_MAX);
	ASSERT_EQ(20, _POSIX_OPEN_MAX);
	ASSERT_EQ(512, _POSIX_PIPE_BUF);
}

TEST(posix_minimums_files) {
	ASSERT_EQ(8, _POSIX_LINK_MAX);
	ASSERT_EQ(14, _POSIX_NAME_MAX);
	ASSERT_EQ(256, _POSIX_PATH_MAX);
	ASSERT_EQ(255, _POSIX_SYMLINK_MAX);
	ASSERT_EQ(8, _POSIX_SYMLOOP_MAX);
}

TEST(posix_minimums_processes) {
	ASSERT_EQ(25, _POSIX_CHILD_MAX);
	ASSERT_EQ(32, _POSIX_DELAYTIMER_MAX);
	ASSERT_EQ(32, _POSIX_TIMER_MAX);
}

TEST(posix_minimums_terminal) {
	ASSERT_EQ(255, _POSIX_MAX_CANON);
	ASSERT_EQ(255, _POSIX_MAX_INPUT);
	ASSERT_EQ(9, _POSIX_TTY_NAME_MAX);
}

TEST(posix_minimums_exec) {
	ASSERT_EQ(4096, _POSIX_ARG_MAX);
}

TEST(posix_minimums_network) {
	ASSERT_EQ(255, _POSIX_HOST_NAME_MAX);
}

TEST(posix_minimums_groups) {
	ASSERT_EQ(8, _POSIX_NGROUPS_MAX);
}

TEST(posix_minimums_regex) {
	ASSERT_EQ(255, _POSIX_RE_DUP_MAX);
}

TEST(posix_minimums_utilities) {
	ASSERT_EQ(32, _POSIX2_EXPR_NEST_MAX);
	ASSERT_EQ(2048, _POSIX2_LINE_MAX);
	ASSERT_EQ(14, _POSIX2_CHARCLASS_NAME_MAX);
	ASSERT_EQ(2, _POSIX2_COLL_WEIGHTS_MAX);
}

TEST(posix_minimums_bc) {
	ASSERT_EQ(99, _POSIX2_BC_BASE_MAX);
	ASSERT_EQ(2048, _POSIX2_BC_DIM_MAX);
	ASSERT_EQ(99, _POSIX2_BC_SCALE_MAX);
	ASSERT_EQ(1000, _POSIX2_BC_STRING_MAX);
}

TEST(posix_minimums_time) {
	ASSERT_EQ(6, _POSIX_TZNAME_MAX);
	ASSERT_EQ(20000000, _POSIX_CLOCKRES_MIN);
}

TEST(posix_minimums_threads) {
	ASSERT_EQ(4, _POSIX_THREAD_DESTRUCTOR_ITERATIONS);
	ASSERT_EQ(128, _POSIX_THREAD_KEYS_MAX);
	ASSERT_EQ(64, _POSIX_THREAD_THREADS_MAX);
}

TEST(posix_minimums_semaphores) {
	ASSERT_EQ(256, _POSIX_SEM_NSEMS_MAX);
	ASSERT_EQ(32767, _POSIX_SEM_VALUE_MAX);
}

TEST(posix_minimums_signals) {
	ASSERT_EQ(32, _POSIX_SIGQUEUE_MAX);
	ASSERT_EQ(8, _POSIX_RTSIG_MAX);
}

TEST(posix_minimums_mq) {
	ASSERT_EQ(8, _POSIX_MQ_OPEN_MAX);
	ASSERT_EQ(32, _POSIX_MQ_PRIO_MAX);
}

TEST(posix_minimums_misc) {
	ASSERT_EQ(4, _POSIX_SS_REPL_MAX);
	ASSERT_EQ(32767, _POSIX_SSIZE_MAX);
	ASSERT_EQ(8, _POSIX_STREAM_MAX);
	ASSERT_EQ(9, _POSIX_LOGIN_NAME_MAX);
}

TEST(posix_minimums_xsi) {
	ASSERT_EQ(16, _XOPEN_IOV_MAX);
	ASSERT_EQ(255, _XOPEN_NAME_MAX);
	ASSERT_EQ(1024, _XOPEN_PATH_MAX);
}

/* ============================================================================
 * SIZE TYPES
 * ============================================================================ */
TEST_SUITE(size_t);

TEST(size_t_size) {
	ASSERT_TRUE(sizeof(size_t) > 0);
#if JACL_64BIT
	ASSERT_EQ(8, sizeof(size_t));
#else
	ASSERT_EQ(4, sizeof(size_t));
#endif
}

TEST(size_t_sign) {
	size_t neg = (size_t)-1;
	ASSERT_TRUE(neg > 0);
}

TEST(size_t_min) {
	size_t min = 0;
	ASSERT_EQ(0, min);
}

TEST(size_t_max) {
	ASSERT_TRUE(SIZE_MAX > 0);
	ASSERT_EQ(((size_t)-1), SIZE_MAX);
}

TEST_SUITE(ssize_t);

TEST(ssize_t_size) {
	ASSERT_EQ(sizeof(size_t), sizeof(ssize_t));
}

TEST(ssize_t_sign) {
	ssize_t neg = -1;
	ASSERT_TRUE(neg < 0);
}

TEST(ssize_t_min) {
	ASSERT_TRUE(SSIZE_MIN < 0);
}

TEST(ssize_t_max) {
	ASSERT_TRUE(SSIZE_MAX > 0);
}

/* ============================================================================
 * BIT WIDTHS
 * ============================================================================ */
TEST_SUITE(bit_widths);

TEST(bit_widths_CHAR_BIT) { ASSERT_EQ(8, CHAR_BIT); }
TEST(bit_widths_SCHAR_BIT) { ASSERT_EQ(CHAR_BIT, SCHAR_BIT); }
TEST(bit_widths_SHRT_BIT) { ASSERT_TRUE(SHRT_BIT >= 16); }
TEST(bit_widths_INT_BIT) { ASSERT_TRUE(INT_BIT >= 32); }
TEST(bit_widths_LONG_BIT) { ASSERT_TRUE(LONG_BIT >= 32); }
#if JACL_HAS_C99
TEST(bit_widths_LLONG_BIT) { ASSERT_TRUE(LLONG_BIT >= 64); }
#endif
TEST(bit_widths_MB_LEN_MAX) { ASSERT_TRUE(MB_LEN_MAX >= 1); }

/* ============================================================================
 * RESOURCE LIMITS (Runtime-enforceable)
 * ============================================================================ */
TEST_SUITE(resource_limits);

TEST(resource_limits_aio) {
	ASSERT_TRUE(AIO_LISTIO_MAX >= _POSIX_AIO_LISTIO_MAX);
	ASSERT_TRUE(AIO_MAX >= _POSIX_AIO_MAX);
	ASSERT_TRUE(AIO_PRIO_DELTA_MAX >= _POSIX_AIO_PRIO_DELTA_MAX);
}

TEST(resource_limits_processes) {
	ASSERT_TRUE(ATEXIT_MAX >= 32);
	ASSERT_TRUE(CHILD_MAX >= _POSIX_CHILD_MAX);
	ASSERT_TRUE(DELAYTIMER_MAX >= _POSIX_DELAYTIMER_MAX);
}

TEST(resource_limits_groups) {
	ASSERT_TRUE(NGROUPS_MAX >= _POSIX_NGROUPS_MAX);
}

TEST(resource_limits_streams) {
	ASSERT_TRUE(STREAM_MAX >= _POSIX_STREAM_MAX);
	ASSERT_TRUE(LINE_MAX >= _POSIX2_LINE_MAX);
	ASSERT_TRUE(TZNAME_MAX >= _POSIX_TZNAME_MAX);
}

TEST(resource_limits_semaphores) {
	ASSERT_TRUE(SEM_VALUE_MAX >= _POSIX_SEM_VALUE_MAX);
	ASSERT_TRUE(SEM_NSEMS_MAX >= _POSIX_SEM_NSEMS_MAX);
}

TEST(resource_limits_signals) {
	ASSERT_TRUE(SIGQUEUE_MAX >= _POSIX_SIGQUEUE_MAX);
	ASSERT_TRUE(RTSIG_MAX >= _POSIX_RTSIG_MAX);
}

TEST(resource_limits_mq) {
	ASSERT_TRUE(MQ_OPEN_MAX >= _POSIX_MQ_OPEN_MAX);
	ASSERT_TRUE(MQ_PRIO_MAX >= _POSIX_MQ_PRIO_MAX);
}

TEST(resource_limits_iov) {
	ASSERT_TRUE(IOV_MAX >= _XOPEN_IOV_MAX);
}

TEST(resource_limits_host_timer) {
	ASSERT_TRUE(HOST_NAME_MAX >= _POSIX_HOST_NAME_MAX);
	ASSERT_TRUE(TIMER_MAX >= _POSIX_TIMER_MAX);
	ASSERT_TRUE(TTY_NAME_MAX >= _POSIX_TTY_NAME_MAX);
}

TEST(resource_limits_bc) {
	ASSERT_TRUE(BC_BASE_MAX >= _POSIX2_BC_BASE_MAX);
	ASSERT_TRUE(BC_DIM_MAX >= _POSIX2_BC_DIM_MAX);
	ASSERT_TRUE(BC_SCALE_MAX >= _POSIX2_BC_SCALE_MAX);
	ASSERT_TRUE(BC_STRING_MAX >= _POSIX2_BC_STRING_MAX);
}

TEST(resource_limits_locale) {
	ASSERT_TRUE(CHARCLASS_NAME_MAX >= _POSIX2_CHARCLASS_NAME_MAX);
	ASSERT_TRUE(COLL_WEIGHTS_MAX >= _POSIX2_COLL_WEIGHTS_MAX);
	ASSERT_TRUE(EXPR_NEST_MAX >= _POSIX2_EXPR_NEST_MAX);
}

/* ============================================================================
 * SYSTEM / PATHNAME LIMITS
 * ============================================================================ */
TEST_SUITE(system_limits);

TEST(system_limits_names) {
	ASSERT_TRUE(NAME_MAX >= _POSIX_NAME_MAX);
	ASSERT_TRUE(PATH_MAX >= _POSIX_PATH_MAX);
	ASSERT_EQ(PATH_MAX, FILENAME_MAX);
}

TEST(system_limits_exec) {
	ASSERT_TRUE(ARG_MAX >= _POSIX_ARG_MAX);
	ASSERT_TRUE(OPEN_MAX >= _POSIX_OPEN_MAX);
}

TEST(system_limits_pipe) {
	ASSERT_TRUE(PIPE_BUF >= _POSIX_PIPE_BUF);
}

TEST(system_limits_links) {
	ASSERT_TRUE(LINK_MAX >= _POSIX_LINK_MAX);
	ASSERT_TRUE(SYMLINK_MAX >= _POSIX_SYMLINK_MAX);
	ASSERT_TRUE(SYMLOOP_MAX >= _POSIX_SYMLOOP_MAX);
}

TEST(system_limits_terminal) {
	ASSERT_TRUE(MAX_CANON >= _POSIX_MAX_CANON);
	ASSERT_TRUE(MAX_INPUT >= _POSIX_MAX_INPUT);
	ASSERT_TRUE(LOGIN_NAME_MAX >= _POSIX_LOGIN_NAME_MAX);
}

TEST(system_limits_regex) {
	ASSERT_TRUE(RE_DUP_MAX >= _POSIX2_RE_DUP_MAX);
}

TEST(system_limits_filesize) {
	ASSERT_TRUE(FILESIZEBITS >= 32);
}

/* ============================================================================
 * INVARIANT VALUES
 * ============================================================================ */
TEST_SUITE(invariant_values);

TEST(invariant_values_nl) {
	ASSERT_EQ(9, NL_ARGMAX);
	ASSERT_EQ(14, NL_LANGMAX);
	ASSERT_EQ(32767, NL_MSGMAX);
	ASSERT_EQ(255, NL_SETMAX);
	ASSERT_EQ(_POSIX2_LINE_MAX, NL_TEXTMAX);
}

TEST(invariant_values_nzero) {
	ASSERT_EQ(20, NZERO);
}

/* ============================================================================
 * ISSUE 8 ADDITIONS
 * ============================================================================ */
TEST_SUITE(issue8_additions);

TEST(issue8_getentropy_max) {
	ASSERT_EQ(256, GETENTROPY_MAX);
}

TEST(issue8_textdomain_max) {
	ASSERT_EQ(_POSIX_NAME_MAX - 3, TEXTDOMAIN_MAX);
}

TEST(issue8_nsig_max) {
	ASSERT_TRUE(NSIG_MAX > 0);
}

/* ============================================================================
 * THREADING LIMITS
 * ============================================================================ */
TEST_SUITE(threading_limits);

TEST(threading_limits_destructor) {
	ASSERT_EQ(_POSIX_THREAD_DESTRUCTOR_ITERATIONS, PTHREAD_DESTRUCTOR_ITERATIONS);
}

TEST(threading_limits_keys) {
	ASSERT_EQ(_POSIX_THREAD_KEYS_MAX, PTHREAD_KEYS_MAX);
}

TEST(threading_limits_stack_min) {
	ASSERT_EQ(0, PTHREAD_STACK_MIN);
}

TEST(threading_limits_threads_max) {
	ASSERT_EQ(_POSIX_THREAD_THREADS_MAX, PTHREAD_THREADS_MAX);
}

/* ============================================================================
 * PAGE SIZE (XSI)
 * ============================================================================ */
TEST_SUITE(page_size);

TEST(page_size_pagesize) {
	ASSERT_TRUE(PAGESIZE > 0);
}

TEST(page_size_page_size) {
	ASSERT_EQ(PAGESIZE, PAGE_SIZE);
}

TEST(page_size_power_of_two) {
	int p = PAGESIZE;
	ASSERT_TRUE(p > 0 && (p & (p - 1)) == 0);
}

TEST_MAIN()
