/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _TESTING_H
#define _TESTING_H
#pragma once

/**
 * INSTRUCTIONS:
 *
 *   TEST_TYPE(name)              an identifier displayed in the front end
 *   TEST_UNIT(filename.h)        filename of the unit you are testing
 *   TEST_SUITE(function)         name of the specific function you are testing
 *   TEST(function_specific_test) {  name of specific thing you are checking
 *     TEST_SKIP("reason");       skip tests that segfault until you can fix
 *     TEST_INFO("essay");        add as much explanation as you want
 *     ASSERT...                  run whatever assertions you need
 *   }
 *
 *   TEST_EACH(suite)             runs a suite of tests
 *   TEST_ONLY(name)              runs one test
 *   TEST_ALL()                   runs all tests
 *   TEST_REPORT()                sprints test report
 *   TEST_MAIN()                  runs all tests and prints report (plus more: see --help)
 *
 * ASSERTIONS:
 *
 *   Boolean:
 *     ASSERT_TRUE(x)                                      - value is true
 *     ASSERT_FALSE(x)                                     - value is false
 *
 *   Comparisons:
 *     ASSERT_EQ(a, b)                                     - equal (alias for ASSERT_INT_EQ)
 *     ASSERT_NE(a, b)                                     - not equal
 *     ASSERT_GT(a, b)                                     - greater than
 *     ASSERT_LT(a, b)                                     - less than
 *     ASSERT_GE(a, b)                                     - greater or equal
 *     ASSERT_LE(a, b)                                     - less or equal
 *
 *   Strings Comparisons:
 *     ASSERT_STR_EQ(actual, expected)                     - strings equal
 *     ASSERT_STR_NE(actual, expected)                     - strings not equal
 *     ASSERT_STR_PRE(prefix, string)                      - string starts with prefix
 *     ASSERT_STR_SUF(suffix, string)                      - string ends with suffix
 *     ASSERT_STR_HAS(needle, string)                      - string contains substring
 *
 *   Numeric Comparisons:
 *     ASSERT_INT_EQ/NE/GT/LT/GE/LE(a, b)                  - explicit integer comparison
 *     ASSERT_FLT_EQ/NE/GT/LT/GE/LE(a, b)                  - float comparisons
 *     ASSERT_DBL_EQ/NE/GT/LT/GE/LE(a, b)                  - double comparisons
 *     ASSERT_LDBL_EQ/NE/GT/LT/GE/LE(a, b)                 - long double comparisons
 *
 *   Tolerant Comparisons:
 *     ASSERT_FLT_NEAR(actual, expected, tolerance)        - float within tolerance
 *     ASSERT_DBL_NEAR(actual, expected, tolerance)        - double within tolerance
 *     ASSERT_LDBL_NEAR(actual, expected, tolerance)       - long double within tolerance
 *
 *   Complex Comparisons:
 *     ASSERT_FLT_COMPLEX(actual, real, imag, tolerance    - float complex numbers
 *     ASSERT_DBL_COMPLEX(actual, real, imag, tolerance)   - double complex numbers
 *     ASSERT_LDBL_COMPLEX(actual, real, imag, tolerance)  - long complex numbers
 *
 *   Special Values:
 *     ASSERT_NAN(x)                                       - value is NaN
 *     ASSERT_INF(x)                                       - value is infinity
 *     ASSERT_FINITE(x)                                    - value is finite
 *
 *   Null Comparisons:
 *     ASSERT_NULL(ptr)                                    - pointer is NULL
 *     ASSERT_NOT_NULL(ptr)                                - pointer is not NULL
 *
 *   Pointer Comparisons:
 *     ASSERT_PTR_EQ(a, b)                                 - pointers are equal
 *     ASSERT_PTR_NE(a, b)                                 - pointers are different
 *
 *   Memory Comparisons:
 *     ASSERT_MEM_EQ(actual,expected, size)               - memory regions equal
 *     ASSERT_MEM_NE(actual,expected, size)               - memory regions different
 *
 *   Size Comparisons:
 *     ASSERT_SIZE(object, size)                           - memory region is exactly size
 *     ASSERT_SIZE_MIN(object, size)                       - memory region is at least size
 *     ASSERT_SIZE_MAX(object, size)                       - memory region is at most size
 *     ASSERT_SIZE_FITS(object, thing)                     - memory region fits the thing
 *     ASSERT_SIZE_SAME(object, thing)                     - memory region is the same size
 *     ASSERT_SIZE_ALIGNS(object, thing)                   - memory region aligns to size
 *
 *   Offset Comparisons:
 *     ASSERT_OFFSET(object, property, offset)             - memory offset is exactly
 *     ASSERT_OFFSET_PAST(object, property, offset)        - memory offset is past offset
 *     ASSERT_OFFSET_AFTER(object, prop_a, prop_b)         - memory offset is past offset
 *     ASSERT_OFFSET_ALIGNS(object, property, thing)       - memory offset aligns to size
 *
 *   Errno Checks:
 *     ASSERT_ERRNO(err)                                   - errno equals expected
 */

#include <config.h>
#include <stdarg.h>  // va_arg()
#include <stddef.h>  // offsetof
#include <stdio.h>   // fprintf(), printf(), puts()
#include <stdlib.h>  // exit()
#include <string.h>  // strcmp(), memset()
#include <inttypes.h>  // PRIdMAX, PRIuMAX
#include <errno.h>
#include <math.h>

/* ============================================================= */
/* Configuration                                                 */
/* ============================================================= */
#ifndef JACL_MAX_TESTS
#define JACL_MAX_TESTS 1000
#endif
#ifndef JACL_MAX_SUITES
#define JACL_MAX_SUITES 1000
#endif

/* ============================================================= */
/* Data Structures                                               */
/* ============================================================= */
typedef void (*test_func_t)(void);

typedef struct {
  int total;
  int passed;
  int failed;
  int skipped;
} test_stats_t;

typedef struct {
  const char* name;
  test_func_t func;
} test_t;

typedef struct {
	const char* type;
	const char* unit;
	const char* suite;
	test_t tests[JACL_MAX_TESTS];
	int test_count;
	test_stats_t stats;
} test_suite_t;

static test_stats_t __jacl_test_stats         = {0,0,0,0};
static int          __jacl_test_failed        = 0;
static int          __jacl_test_skipped       = 0;
static int          __jacl_test_errored       = 0;
static char         __jacl_test_error[2048]   = {0};
static int          __jacl_show_only_failed   = 0;
static int          __jacl_show_only_skipped  = 0;
static int          __jacl_test_suite_count   = 0;
static test_suite_t __jacl_test_suites[JACL_MAX_SUITES];

/* ============================================================= */
/* ANSI Colour Helper                                            */
/* ============================================================= */
#define COLOR_HEAD   "\x1b[39m"
#define COLOR_TEST   "\x1b[93m"
#define COLOR_PASS   "\x1b[92m"
#define COLOR_FAIL   "\x1b[31m"
#define COLOR_SKIP   "\x1b[36m"
#define COLOR_NORM   "\x1b[0m"

static inline int __jacl_test_color(void) {
  static int checked = 0, has = 0;

  if (!checked) {
    const char *t = getenv("TERM");

    has = t && strcmp(t, "dumb");
    checked = 1;
  }

  return has;
}

/* ============================================================= */
/* Registration Internals                                        */
/* ============================================================= */
static char *__jacl_test_type = "unit";
static char *__jacl_test_unit = "test";
static char *__jacl_test_suite = "default";

static inline void __jacl_test_register(const char *test_name, test_func_t func) {
	test_suite_t *suite = NULL;

	for (int i = 0; i < __jacl_test_suite_count; i++) {
		if (!strcmp(__jacl_test_suites[i].type, __jacl_test_type) &&
		    !strcmp(__jacl_test_suites[i].unit, __jacl_test_unit) &&
		    !strcmp(__jacl_test_suites[i].suite, __jacl_test_suite)) {
			suite = &__jacl_test_suites[i];

			break;
		}
	}

	if (!suite) {
		if (__jacl_test_suite_count >= JACL_MAX_SUITES) {
			fprintf(stderr, "Error: too many suites (>%d)\n", JACL_MAX_SUITES);

			exit(1);
		}

		suite = &__jacl_test_suites[__jacl_test_suite_count++];
		suite->type = __jacl_test_type;
		suite->unit = __jacl_test_unit;
		suite->suite = __jacl_test_suite;
		suite->test_count = 0;
		suite->stats = (test_stats_t){0, 0, 0, 0};
	}

	if (suite->test_count >= JACL_MAX_TESTS) {
		fprintf(stderr, "Error: too many tests in suite '%s: %s' (>%d)\n", suite->unit, suite->suite, JACL_MAX_TESTS);
		exit(1);
	}

	suite->tests[suite->test_count].name = test_name;
	suite->tests[suite->test_count].func = func;
	suite->test_count++;
}

/* ============================================================= */
/* Runner Internals                                              */
/* ============================================================= */
static inline void __jacl_test_run(test_suite_t *suite, test_t *t) {
	int print_this = 1;

	__jacl_test_failed  = 0;
	__jacl_test_errored = 0;
	__jacl_test_skipped = 0;
	__jacl_test_stats.total++;
	suite->stats.total++;
	errno = 0;

	memset(__jacl_test_error, 0, sizeof(__jacl_test_error));

	// Filter output: only print matching tests if filter is set
	if (__jacl_show_only_failed || __jacl_show_only_skipped) print_this = 0;

	t->func();

	// Evaluate result for filtered output
	if (__jacl_show_only_failed && __jacl_test_failed) print_this = 1;
	if (__jacl_show_only_skipped && __jacl_test_skipped) print_this = 1;
	if (print_this) printf("  %s%-60s%s", __jacl_test_color() ? COLOR_TEST : "", t->name, __jacl_test_color() ? COLOR_NORM : "");

	// Output and stats
	if (__jacl_test_skipped) {
		if (print_this) printf("%s[SKIP]%s\n", __jacl_test_color() ? COLOR_SKIP : "", __jacl_test_color() ? COLOR_NORM : "");
		if (print_this) printf("\n%s%s%s\n\n", __jacl_test_color() ? COLOR_SKIP : "", __jacl_test_error, __jacl_test_color() ? COLOR_NORM : "");

		__jacl_test_stats.skipped++;
		suite->stats.skipped++;
	} else if (__jacl_test_failed) {
		if (print_this) printf("%s[FAIL]%s\n", __jacl_test_color() ? COLOR_FAIL : "", __jacl_test_color() ? COLOR_NORM : "");
		if (__jacl_test_errored && print_this) printf("\n%s%s%s\n\n", __jacl_test_color() ? COLOR_FAIL : "", __jacl_test_error, __jacl_test_color() ? COLOR_NORM : "");

		__jacl_test_stats.failed++;
		suite->stats.failed++;
	} else {
		if (print_this) printf("%s[PASS]%s\n", __jacl_test_color() ? COLOR_PASS : "", __jacl_test_color() ? COLOR_NORM : "");

		__jacl_test_stats.passed++;
		suite->stats.passed++;
	}
}
static inline int __jacl_test_only(const char *test_name) {
	for (int i = 0; i < __jacl_test_suite_count; i++) {
		test_suite_t *suite = &__jacl_test_suites[i];

		for (int j = 0; j < suite->test_count; j++) {
			if (!strcmp(suite->tests[j].name, test_name)) {
  			printf("\n  %sRunning only '%s' test…%s\n",
				       __jacl_test_color() ? COLOR_TEST : "",
							 test_name,
				       __jacl_test_color() ? COLOR_NORM : "");
				printf("  %s%s - %s: %s test%s\n\n",
				       __jacl_test_color() ? COLOR_HEAD : "",
				       suite->suite, suite->unit, suite->type,
				       __jacl_test_color() ? COLOR_NORM : "");
				__jacl_test_run(suite, &suite->tests[j]);

				return __jacl_test_stats.failed > 0;
			}
		}
	}

  printf("Test '%s' not found\n", test_name);

  return 1;
}
static inline int __jacl_test_each(const char *suite_name) {
	test_suite_t *suite = NULL;

	for (int i = 0; i < __jacl_test_suite_count; i++) {
		if (!strcmp(__jacl_test_suites[i].suite, suite_name)) { suite = &__jacl_test_suites[i]; break; }
	}

	if (!suite) { printf("  Suite '%s' not found\n", suite_name); return 1; }

  printf("\n  %sRunning '%s' suite…%s\n",
	       __jacl_test_color() ? COLOR_TEST : "",
				 suite_name,
	       __jacl_test_color() ? COLOR_NORM : "");

	printf("  %s%s - %s: %s tests%s\n\n",
	       __jacl_test_color() ? COLOR_HEAD : "",
	       suite->suite, suite->unit, suite->type,
	       __jacl_test_color() ? COLOR_NORM : "");

	for (int i = 0; i < suite->test_count; i++) __jacl_test_run(suite, &suite->tests[i]);

	return __jacl_test_stats.failed > 0;
}
static inline int __jacl_test_all(void) {
  if (__jacl_test_suite_count == 0) { puts("No tests registered."); return 0; }

  int total = 0;

  for (int i = 0; i < __jacl_test_suite_count; i++) total += __jacl_test_suites[i].test_count;

  // Add blank line before, color the "Running" message like test names (blue)
  printf("\n  %sRunning all %d test%s…%s\n",
         __jacl_test_color() ? COLOR_TEST : "",
         total,
         total == 1 ? "" : "s",
         __jacl_test_color() ? COLOR_NORM : "");

	if (__jacl_show_only_failed || __jacl_show_only_skipped) printf("\n");

  for (int i = 0; i < __jacl_test_suite_count; i++) {
    test_suite_t *suite = &__jacl_test_suites[i];

    // Add blank line before header, keep header on its own line
    if (!__jacl_show_only_failed && !__jacl_show_only_skipped) printf("\n  %s%s - %s: %s tests%s\n\n",
           __jacl_test_color() ? COLOR_HEAD : "",
           suite->suite, suite->unit, suite->type,
           __jacl_test_color() ? COLOR_NORM : "");

    for (int j = 0; j < suite->test_count; j++) __jacl_test_run(suite, &suite->tests[j]);
  }

  return __jacl_test_stats.failed > 0;
}

/* ============================================================= */
/* Print Summary                                                 */
/* ============================================================= */
static inline void __jacl_test_report(void) {
  if (!__jacl_test_stats.total) { puts("No tests run."); return; }

  printf("\n%s\n",
    __jacl_test_stats.failed
    ? (__jacl_test_color() ? COLOR_FAIL
      "  ❌ SOME CHECKS FAILED ❌" COLOR_NORM
      : "  ❌ SOME CHECKS FAILED ❌")
    : (__jacl_test_color() ? COLOR_PASS
      "  🎉 ALL CHECKS PASSED 🎉" COLOR_NORM
      : "  🎉 ALL CHECKS PASSED 🎉")
  );

  printf("\n  %s%d total   %s%d passed   %s%d skipped   %s%d failed%s\n\n",
    __jacl_test_color() ? COLOR_TEST : "",
    __jacl_test_stats.total,
    __jacl_test_color() ? COLOR_PASS : "",
		__jacl_test_stats.passed,
    __jacl_test_color() ? COLOR_SKIP : "",
		__jacl_test_stats.skipped,
    __jacl_test_color() ? COLOR_FAIL : "",
    __jacl_test_stats.failed,
    __jacl_test_color() ? COLOR_NORM : ""
  );
}

/* ============================================================= */
/* Test Assertions                                               */
/* ============================================================= */
#define ASSERT_TRUE(x) do { \
	if (!(x)) { \
		TEST_FAIL("Value should be true\n" \
		          "\tTEST: %s", #x); \
	} \
} while(0)
#define ASSERT_FALSE(x) do { \
	if (x) { \
		TEST_FAIL("Value should be false\n" \
		          "\tTEST: %s", #x); \
	} \
} while(0)

#define ASSERT_EQ(a, b)  ASSERT_INT_EQ(a, b)
#define ASSERT_NE(a, b)  ASSERT_INT_NE(a, b)
#define ASSERT_GT(a, b)  ASSERT_INT_GT(a, b)
#define ASSERT_LT(a, b)  ASSERT_INT_LT(a, b)
#define ASSERT_GE(a, b)  ASSERT_INT_GE(a, b)
#define ASSERT_LE(a, b)  ASSERT_INT_LE(a, b)

#define ASSERT_STR_EQ(actual, expected) do { \
	if (!expected || !actual || strcmp((expected), (actual)) != 0) { \
		TEST_FAIL("Strings not equal\n\tINPUT: (%s) is (%s)\n\tVALUE: \"%s\" is \"%s\"", \
		          #actual, #expected, (actual) ? (actual) : "(null)", (expected) ? (expected) : "(null)"); \
	} \
} while(0)
#define ASSERT_STR_NE(actual, expected) do { \
	if (expected && actual && strcmp((expected), (actual)) == 0) { \
		TEST_FAIL("Strings should not match\n\tINPUT: (%s) is not (%s)\n\tVALUE: \"%s\" is not \"%s\"", \
		          #actual, #expected, (actual) ? (actual) : "(null)", (expected) ? (expected) : "(null)"); \
	} \
} while(0)
#define ASSERT_STR_PRE(prefix, string) do { \
	if (!prefix || !string || strncmp((prefix), (string), strlen(prefix)) != 0) { \
		TEST_FAIL("String should have prefix\n\tINPUT: (%s) starts with (%s)\n\tVALUE: \"%s\" starts with \"%s\"", \
		          #string, #prefix, (string) ? (string) : "(null)", (prefix) ? (prefix) : "(null)"); \
	} \
} while(0)
#define ASSERT_STR_SUF(suffix, string) do { \
	size_t __test_suf_len = suffix ? strlen(suffix) : 0; \
	size_t __test_act_len = string ? strlen(string) : 0; \
	if (!suffix || !string || __test_act_len < __test_suf_len || \
	    strcmp((string) + __test_act_len - __test_suf_len, (suffix)) != 0) { \
		TEST_FAIL("String should have suffix\n\tINPUT: (%s) ends with (%s)\n\tVALUE: \"%s\" ends with \"%s\"", \
		          #string, #suffix, (string) ? (string) : "(null)", (suffix) ? (suffix) : "(null)"); \
	} \
} while(0)
#define ASSERT_STR_HAS(needle, string) do { \
	if (!needle || !string || strstr((string), (needle)) == NULL) { \
		TEST_FAIL("String should contain substring\n\tINPUT: (%s) includes (%s)\n\tVALUE: \"%s\" includes \"%s\"", \
		          #string, #needle, (string) ? (string) : "(null)", (needle) ? (needle) : "(null)"); \
	} \
} while(0)
#define ASSERT_INT_EQ(a, b) do { \
	if (!((intmax_t)(a) == (intmax_t)(b))) { \
		TEST_FAIL("Values should be equal\n\tINPUT: %s == %s\n\tVALUE: %" PRIdMAX " == %" PRIdMAX, \
		          #a, #b, (intmax_t)(a), (intmax_t)(b)); \
	} \
} while(0)
#define ASSERT_INT_GT(a, b) do { \
	if (!((intmax_t)(a) > (intmax_t)(b))) { \
		TEST_FAIL("Values should be more\n\tINPUT: %s > %s\n\tVALUE: %" PRIdMAX " > %" PRIdMAX, \
		          #a, #b, (intmax_t)(a), (intmax_t)(b)); \
	} \
} while(0)
#define ASSERT_INT_LT(a, b) do { \
	if (!((intmax_t)(a) < (intmax_t)(b))) { \
		TEST_FAIL(Values should be less\n\tINPUT: %s < %s\n\tVALUE: %" PRIdMAX " < %" PRIdMAX, \
		          #a, #b, (intmax_t)(a), (intmax_t)(b)); \
	} \
} while(0)
#define ASSERT_INT_GE(a, b) do { \
	if (!((intmax_t)(a) >= (intmax_t)(b))) { \
		TEST_FAIL("Values shouldn't be less\n\tINPUT: %s >= %s\n\tVALUE: %" PRIdMAX " >= %" PRIdMAX, \
		          #a, #b, (intmax_t)(a), (intmax_t)(b)); \
	} \
} while(0)
#define ASSERT_INT_LE(a, b) do { \
	if (!((intmax_t)(a) <= (intmax_t)(b))) { \
		TEST_FAIL("Values shouldn't be more\n\tINPUT: %s <= %s\n\tVALUE: %" PRIdMAX " <= %" PRIdMAX, \
		          #a, #b, (intmax_t)(a), (intmax_t)(b)); \
	} \
} while(0)
#define ASSERT_INT_NE(a, b) do { \
	if (!((intmax_t)(a) != (intmax_t)(b))) { \
		TEST_FAIL("Values should be different\n\tINPUT: %s != %s\n\tVALUE: %" PRIdMAX " != %" PRIdMAX, \
		          #a, #b, (intmax_t)(a), (intmax_t)(b)); \
	} \
} while(0)

#define ASSERT_NAN(x) do { \
	if (!isnan(x)) { \
		TEST_FAIL("Expected NaN\n\tINPUT: isnan(%s)\n\tVALUE: isnan(%Lf)", \
		          #x, (long double)(x)); \
	} \
} while(0)
#define ASSERT_INF(x) do { \
	if (!isinf(x)) { \
		TEST_FAIL("Expected Infinity\n\tINPUT: isinf(%s)\n\tVALUE: isinf(%Lg)", \
		          #x, (long double)(x)); \
	} \
} while(0)
#define ASSERT_FINITE(x) do { \
	if (!isfinite(x)) { \
		TEST_FAIL("Expected finite value\n\tINPUT: isfinite(%s)\n\tVALUE: isfinite(%Lg)", \
		          #x, (long double)(x)); \
	} \
} while(0)

#define ASSERT_FLT_EQ(a, b) do { \
	if (!((float)(a) == (float)(b))) { \
		TEST_FAIL("Values should be equal\n\tINPUT: %s == %s\n\tVALUE: %g == %g", \
		          #a, #b, (float)(a), (float)(b)); \
	} \
} while(0)
#define ASSERT_FLT_GT(a, b) do { \
	if (!((float)(a) > (float)(b))) { \
		TEST_FAIL("Values should be more\n\tINPUT: %s > %s\n\tVALUE: %g > %g", \
		          #a, #b, (float)(a), (float)(b)); \
	} \
} while(0)
#define ASSERT_FLT_LT(a, b) do { \
	if (!((float)(a) < (float)(b))) { \
		TEST_FAIL("Values should be less\n\tINPUT: %s < %s\n\tVALUE: %g < %g", \
		          #a, #b, (float)(a), (float)(b)); \
	} \
} while(0)
#define ASSERT_FLT_GE(a, b) do { \
	if (!((float)(a) >= (float)(b))) { \
		TEST_FAIL("Values should be more\n\tINPUT: %s >= %s\n\tVALUE: %g >= %g", \
		          #a, #b, (float)(a), (float)(b)); \
	} \
} while(0)
#define ASSERT_FLT_LE(a, b) do { \
	if (!((float)(a) <= (float)(b))) { \
		TEST_FAIL("Values should be less\n\tINPUT: %s <= %s\n\tVALUE: %g <= %g", \
		          #a, #b, (float)(a), (float)(b)); \
	} \
} while(0)
#define ASSERT_FLT_NE(a, b) do { \
	if (!((float)(a) != (float)(b))) { \
		TEST_FAIL("Values should be different\n\tINPUT: %s != %s\n\tVALUE: %g != %g", \
		          #a, #b, (float)(a), (float)(b)); \
	} \
} while(0)
#define ASSERT_FLT_NEAR(actual, expected, tolerance) do { \
	float __test_diff = (float)(expected) - (float)(actual); \
	if (__test_diff < 0) __test_diff = -__test_diff; \
	if (__test_diff > (tolerance)) { \
		TEST_FAIL("Not within tolerance\n\tINPUT: %s ≈ %s\n\tVALUE: %e ≈ %e\n\tWITH: ϵ = %e, Δ = %e", \
		          #actual, #expected, (actual), (expected), (double)(tolerance), __test_diff); \
	} \
} while(0)
#define ASSERT_FLT_COMPLEX(actual, expt_real, expt_imgn, tolerance) do { \
	float __test_r = creal(actual); \
	float __test_i = cimag(actual); \
	float __test_diff_r = (float)(expt_real) - __test_r; \
	float __test_diff_i = (float)(expt_imgn) - __test_i; \
	if (__test_diff_r < 0) __test_diff_r = -__test_diff_r; \
	if (__test_diff_i < 0) __test_diff_i = -__test_diff_i; \
	if (__test_diff_r > (tolerance) || __test_diff_i > (tolerance)) { \
		TEST_FAIL("Complex not in tolerance\n\tTEST: %.6fr%+.6fi ≈ %.6fr%+.6fi\n\tWITH: ϵ = %e, Δr = %e, Δi = %e", \
		          __test_r, __test_i, (float)(expt_real), (float)(expt_imgn), (float)(tolerance), __test_diff_r, __test_diff_i); \
	} \
} while(0)

#define ASSERT_DBL_EQ(a, b) do { \
	if (!((double)(a) == (double)(b))) { \
		TEST_FAIL("Values should be equal\n\tINPUT: %s == %s\n\tVALUE: %lg == %lg", \
		          #a, #b, (double)(a), (double)(b)); \
	} \
} while(0)
#define ASSERT_DBL_GT(a, b) do { \
	if (!((double)(a) > (double)(b))) { \
		TEST_FAIL("Values should be more\n\tINPUT: %s > %s\n\tVALUE: %lg > %lg", \
		          #a, #b, (double)(a), (double)(b)); \
	} \
} while(0)
#define ASSERT_DBL_LT(a, b) do { \
	if (!((double)(a) < (double)(b))) { \
		TEST_FAIL("Values should be less\n\tINPUT: %s < %s\n\tTEST: %lg < %lg", \
		          #a, #b, (double)(a), (double)(b)); \
	} \
} while(0)
#define ASSERT_DBL_GE(a, b) do { \
	if (!((double)(a) >= (double)(b))) { \
		TEST_FAIL("Values should be more\n\tINPUT: %s >= %s\n\tVALUE: %lg >= %lg", \
		          #a, #b, (double)(a), (double)(b)); \
	} \
} while(0)
#define ASSERT_DBL_LE(a, b) do { \
	if (!((double)(a) <= (double)(b))) { \
		TEST_FAIL("Values should be less\n\tINPUT: %s <= %s\n\tVALUE: %lg <= %lg", \
		          #a, #b, (double)(a), (double)(b)); \
	} \
} while(0)
#define ASSERT_DBL_NE(a, b) do { \
	if (!((double)(a) != (double)(b))) { \
		TEST_FAIL("Values should be different\n\tINPUT: %s != %s\n\tVALUE: %lg != %lg", \
		          #a, #b, (double)(a), (double)(b)); \
	} \
} while(0)
#define ASSERT_DBL_NEAR(actual, expected, tolerance) do { \
	double __test_diff = (double)(expected) - (double)(actual); \
	if (__test_diff < 0) __test_diff = -__test_diff; \
	if (__test_diff > (tolerance)) { \
		TEST_FAIL("Not within tolerance\n\tINPUT: %s ≈ %s\n\tVALUE: %le ≈ %le\n\tWITH: ϵ = %le, Δ = %le", \
		          #actual, #expected, (actual), (expected), (double)(tolerance), __test_diff); \
	} \
} while(0)
#define ASSERT_DBL_COMPLEX(expt_real, expt_imgn, actual, tolerance) do { \
	double __test_r = creal(actual); \
	double __test_i = cimag(actual); \
	double __test_diff_r = (double)(expt_real) - __test_r; \
	double __test_diff_i = (double)(expt_imgn) - __test_i; \
	if (__test_diff_r < 0) __test_diff_r = -__test_diff_r; \
	if (__test_diff_i < 0) __test_diff_i = -__test_diff_i; \
	if (__test_diff_r > (tolerance) || __test_diff_i > (tolerance)) { \
		TEST_FAIL("Complex not in tolerance\n\tTEST: %.6lfr%+.6lfi ≈ %.6lfr%+.6lfi\n\tWITH: ϵ = %le, Δr = %le, Δi = %le", \
		          __test_r, __test_i, (double)(expt_real), (double)(expt_imgn), (double)(tolerance), __test_diff_r, __test_diff_i); \
	} \
} while(0)

#define ASSERT_LDBL_EQ(a, b) do { \
	if (!((long double)(a) == (long double)(b))) { \
		TEST_FAIL("Values should be equal\n\tINPUT: %s == %s\n\tVALUE: %Lf == %Lf", \
		          #a, #b, (long double)(a), (long double)(b)); \
	} \
} while(0)
#define ASSERT_LDBL_GT(a, b) do { \
	if (!((long double)(a) > (long double)(b))) { \
		TEST_FAIL("Values should be more\n\tINPUT: %s > %s\n\tVALUE: %Lg > %Lg", \
		          #a, #b, (long double)(a), (long double)(b)); \
	} \
} while(0)
#define ASSERT_LDBL_LT(a, b) do { \
	if (!((long double)(a) < (long double)(b))) { \
		TEST_FAIL("Values should be less\n\tINPUT: %s < %s\n\tVALUE: %Lf < %Lf", \
		          #a, #b, (long double)(a), (long double)(b)); \
	} \
} while(0)
#define ASSERT_LDBL_GE(a, b) do { \
	if (!((long double)(a) >= (long double)(b))) { \
		TEST_FAIL("Values should be more\n\tINPUT: %s >= %s\n\tVALUE: %Lf >= %Lf", \
		          #a, #b, (long double)(a), (long double)(b)); \
	} \
} while(0)
#define ASSERT_LDBL_LE(a, b) do { \
	if (!((long double)(a) <= (long double)(b))) { \
		TEST_FAIL("Values should be less\n\tINPUT: %s <= %s\n\tVALUE: %Lf <= %Lf", \
		          #a, #b, (long double)(a), (long double)(b)); \
	} \
} while(0)
#define ASSERT_LDBL_NE(a, b) do { \
	if (!((long double)(a) != (long double)(b))) { \
		TEST_FAIL("Values should be different\n\tINPUT: %s != %s\n\tVALUE: %Lf != %Lf", \
		          #a, #b, (long double)(a), (long double)(b)); \
	} \
} while(0)
#define ASSERT_LDBL_NEAR(actual, expected, tolerance) do { \
	long double __test_diff = (long double)(expected) - (long double)(actual); \
	if (__test_diff < 0) __test_diff = -__test_diff; \
	if (__test_diff > (long double)(tolerance)) { \
		TEST_FAIL("Not within tolerance\n\tINPUT: %s ≈ %s\n\tVALUE: %Lg ≈ %Lg\n\tWITH: ϵ = %Le, Δ = %Le", \
		          #actual, #expected, (long double)(actual), (long double)(expected), (long double)(tolerance), __test_diff); \
	} \
} while(0)
#define ASSERT_LDBL_COMPLEX(expt_real, expt_imgn, actual, tolerance) do { \
	long double __test_r = creal(actual); \
	long double __test_i = cimag(actual); \
	long double __test_diff_r = (long double)(expt_real) - __test_r; \
	long double __test_diff_i = (long double)(expt_imgn) - __test_i; \
	if (__test_diff_r < 0) __test_diff_r = -__test_diff_r; \
	if (__test_diff_i < 0) __test_diff_i = -__test_diff_i; \
	if (__test_diff_r > (tolerance) || __test_diff_i > (tolerance)) { \
		TEST_FAIL("Complex not in tolerance\n\tTEST: %.6Lgfr%+.6Lgfi ≈ %.6Lgfr%+.6Lgfi\n\tWITH: ϵ = %Lf, Δr = %Lf, Δi = %Lf", \
		          __test_r, __test_i, (long double)(expt_real), (long double)(expt_imgn), (long double)(tolerance), __test_diff_r, __test_diff_i); \
	} \
} while(0)

#define ASSERT_NULL(ptr) do { \
	if (!((ptr) == NULL)) { \
		TEST_FAIL("Expected NULL pointer\n\tINPUT: %s == NULL\n\tVALUE: %p == NULL", \
		          #ptr, (ptr)); \
	} \
} while(0)
#define ASSERT_NOT_NULL(ptr) do { \
	if (!((ptr) != NULL)) { \
		TEST_FAIL("Expected non-NULL pointer\n\tINPUT: %s == NULL\n\tVALUE: NULL != NULL", \
		          #ptr); \
	} \
} while(0)

#define ASSERT_PTR_EQ(a, b) do { \
	if (!((void*)(a) == (void*)(b))) { \
		TEST_FAIL("Pointers should be equal\n\tINPUT: %s == %s\n\tVALUE: %p == %p", \
		          #a, #b, (void*)(a), (void*)(b)); \
		} \
} while(0)
#define ASSERT_PTR_NE(a, b) do { \
	if (!((void*)(a) != (void*)(b))) { \
		TEST_FAIL("Pointers should be different\n\tINPUT: %s != %s\n\tVALUE: %p != %p", \
		          #a, #b, (void*)(a), (void*)(b)); \
		} \
} while(0)

#define ASSERT_MEM_EQ(actual, expected, size) do { \
	if (!(memcmp((expected), (actual), (size_t)(size)) == 0)) { \
		TEST_FAIL("Memory regions should be the same\n\tINPUT: %s == %s\n", "\tSIZE: %zu bytes", \
		          #actual, #expected, (size_t)(size)); \
	} \
} while(0)
#define ASSERT_MEM_NE(actual, expected, size) do { \
	if (!(memcmp((expected), (actual), (size_t)(size)) != 0)) { \
		TEST_FAIL("Memory regions should be different\n\tINPUT: %s != %s\n\tSIZE: %zu bytes", \
		          #actual, #expected, (size_t)(size)); \
	} \
} while(0)

#define ASSERT_SIZE(object, size) do { \
	if (!(sizeof(object) == (size_t)(size))) { \
		TEST_FAIL("Memory size should be exact\n\tTEST: sizeof(%s) == %zu", \
		          #object, (size_t)(size)); \
	} \
} while(0)
#define ASSERT_SIZE_MIN(object, size) do { \
	if (!(sizeof(object) >= (size_t)(size))) { \
		TEST_FAIL("Memory size should be more\n\tTEST: sizeof(%s) >= %zu", \
		          #object, (size_t)(size)); \
	} \
} while(0)
#define ASSERT_SIZE_MAX(object, size) do { \
	if (!(sizeof(object) <= (size_t)(size))) { \
		TEST_FAIL("Memory size should be less\n\tTEST: sizeof(%s) <= %zu", \
		          #object, (size_t)(size)); \
	} \
} while(0)
#define ASSERT_SIZE_FITS(object, thing) do { \
	if (!(sizeof(object) >= sizeof(thing))) { \
		TEST_FAIL("Memory size should fit\n\tTEST: sizeof(%s) >= sizeof(%s)", \
		          #object, #thing); \
	} \
} while(0)
#define ASSERT_SIZE_SAME(object, thing) do { \
	if (!(sizeof(object) == sizeof(thing))) { \
		TEST_FAIL("Memory size should be the same\n\tTEST: sizeof(%s) == sizeof(%s)", \
		          #object, #thing); \
	} \
} while(0)
#define ASSERT_SIZE_ALIGNS(object, thing) do { \
	if (!(sizeof(object) % sizeof(thing) == 0)) { \
		TEST_FAIL("Memory size should be aligned\n\tTEST: sizeof(%s) %% sizeof(%s) == 0", \
		          #object, #thing); \
	} \
} while(0)

#define ASSERT_OFFSET(object, property, offset) do { \
	if (!(offsetof(object, property) == (size_t)(offset))) { \
		TEST_FAIL("Memory offset should be exact\n\tTEST: offsetof(%s, %s) == %zu", \
		          #object, #property, (size_t)(offset)); \
	} \
} while(0)
#define ASSERT_OFFSET_PAST(object, property, offset) do { \
	if (!(offsetof(object, property) >= (size_t)(offset))) { \
		TEST_FAIL("Memory offset should be past\n\tTEST: offsetof(%s, %s) >= %zu", \
		          #object, #property, (size_t)(offset)); \
	} \
} while(0)
#define ASSERT_OFFSET_AFTER(object, prop_a, prop_b) do { \
	if (!(offsetof(object, prop_a) >= offsetof(object, prop_b))) { \
		TEST_FAIL("Memory offset should be after \n\tTEST: offsetof(%s, %s) >= offsetof(%s, %s)", \
		          #object, #prop_a, #object, #prop_b); \
	} \
} while(0)
#define ASSERT_OFFSET_ALIGNS(object, property, thing) do { \
	if (!(offsetof(object, property) % sizeof(thing) == 0)) { \
		TEST_FAIL("Memory offset should align\n\tTEST: offsetof(%s, %s) %% sizeof(%s) == 0", \
		          #object, #property, #thing); \
	} \
} while(0)

#define ASSERT_ERRNO(err) do { \
	if (!((errno) == (err))) { \
		TEST_FAIL("Error should be set\n\tTEST: error %d is %d", \
		          (errno), (err)); \
	} \
} while(0)

/* ============================================================= */
/* Test Utilities                                                */
/* ============================================================= */
#define TEST_FAIL(fmt,...) do { __jacl_test_failed = __jacl_test_errored = 1; \
	snprintf(__jacl_test_error, sizeof(__jacl_test_error), "\tFAILS: %s:%d -  " fmt, __FILE__, __LINE__, __VA_ARGS__); \
	return; \
} while(0)
#define TEST_SKIP(reason) do { \
	__jacl_test_skipped = 1; \
	snprintf(__jacl_test_error, sizeof(__jacl_test_error), "\t%s", reason); \
	return; \
} while(0)
#define TEST_INFO(format, ...) do { \
  printf("	INFO: " format "\n", __VA_ARGS__); \
} while(0)

/* ============================================================= */
/* Test Definitions                                              */
/* ============================================================= */
#define TEST_TYPE(name) JACL_INIT(type) { __jacl_test_type = #name; }
#define TEST_UNIT(name) JACL_INIT(unit) { __jacl_test_unit = #name; }
#define TEST_SUITE(name) JACL_INIT(suite_##name) { __jacl_test_suite = #name; }
#define TEST(name) \
	static void test_##name(void); \
	JACL_INIT(test_##name) { __jacl_test_register(#name, test_##name); } \
	static void test_##name(void)

/* ============================================================= */
/* Test Running                                                  */
/* ============================================================= */
#define  TEST_REPORT() __jacl_test_report()
#define  TEST_EACH(suite) __jacl_test_each(suite)
#define  TEST_ONLY(name) __jacl_test_only(name)
#define  TEST_ALL() __jacl_test_all()
#define  TEST_RUN() JACL_INIT(run) { __jacl_test_all(); }

#ifndef NO_TEST_MAIN
#define TEST_MAIN() TEST_MAIN_IF(1, "")
#define TEST_MAIN_IF(CASE, MESSAGE) int main(int argc, char**argv) { \
	fflush(stdout); fflush(stderr); (void)stdin; \
	if (CASE -1) { \
		printf("\n  %sSKIPPED: %s%s\n", __jacl_test_color() ? COLOR_SKIP : "", MESSAGE, __jacl_test_color() ? COLOR_NORM : ""); \
		return 0; \
	} \
	int r = 0; \
	if (argc > 1) { \
		if (strncmp(argv[1], "--suite=", 8) == 0) { r = TEST_EACH(argv[1] + 8); } \
		else if (strncmp(argv[1], "--test=", 7) == 0) { r = TEST_ONLY(argv[1] + 7); } \
		else if (!strcmp(argv[1], "--failed")) { __jacl_show_only_failed = 1; r = TEST_ALL(); } \
		else if (!strcmp(argv[1], "--skipped")) { __jacl_show_only_skipped = 1; r = TEST_ALL(); } \
		else { printf("Usage: %s [--suite=NAME | --test=NAME | --failed | --skipped]\n", argv[0]); return 1; } \
	} else { r = TEST_ALL(); } \
	TEST_REPORT(); \
	fflush(stdout); \
	return r; \
}
#define TEST_MAIN__(N, ...) JACL_CONCAT(TEST_MAIN, N, _)(__VA_ARGS__)
#endif

#endif /* _TESTING_H */
