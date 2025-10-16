/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef TESTING_H
#define TESTING_H
#pragma once

#include <config.h>
#include <stdio.h>   // fprintf(), printf(), puts()
#include <stdlib.h>  // exit()
#include <string.h>  // strcmp(), memset()
#include <stdarg.h>  // (if needed)
#include <math.h>

/* ============================================================= */
/* Configuration                                                 */
/* ============================================================= */
#ifndef JACL_MAX_TESTS
#define JACL_MAX_TESTS 1000
#endif
#ifndef JACL_MAX_SUITES
#define JACL_MAX_SUITES 100
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
static int          __jacl_test_errored       = 0;
static char         __jacl_test_error[2048]   = {0};

static test_suite_t __jacl_test_suites[JACL_MAX_SUITES];
static int          __jacl_test_suite_count   = 0;

/* ============================================================= */
/* ANSI Colour Helper                                            */
/* ============================================================= */
#define COLOR_HEAD   "\x1b[36m"
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
  __jacl_test_failed = __jacl_test_errored = 0;

  memset(__jacl_test_error, 0, sizeof(__jacl_test_error));

  __jacl_test_stats.total++;

  suite->stats.total++;

	printf("  %s%-60s%s", __jacl_test_color() ? COLOR_TEST : "", t->name, __jacl_test_color() ? COLOR_NORM : "");

  fflush(stdout);

  t->func();

  if (__jacl_test_failed) {
    printf("%s[FAIL]%s\n", __jacl_test_color() ? COLOR_FAIL : "", __jacl_test_color() ? COLOR_NORM : "");

    if (__jacl_test_errored) printf("\n%s%s%s\n\n", __jacl_test_color() ? COLOR_FAIL : "", __jacl_test_error, __jacl_test_color() ? COLOR_NORM : "");

    __jacl_test_stats.failed++;

    suite->stats.failed++;
  } else {
    printf("%s[PASS]%s\n", __jacl_test_color() ? COLOR_PASS : "", __jacl_test_color() ? COLOR_NORM : "");

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
		if (!strcmp(__jacl_test_suites[i].suite, suite_name)) {
			suite = &__jacl_test_suites[i];

			break;
		}
	}

	if (!suite) {
		printf("  Suite '%s' not found\n", suite_name);

		return 1;
	}

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
  if (__jacl_test_suite_count == 0) {
    puts("No tests registered.");

    return 0;
  }

  int total = 0;
  for (int i = 0; i < __jacl_test_suite_count; i++) {
    total += __jacl_test_suites[i].test_count;
  }

  // Add blank line before, color the "Running" message like test names (blue)
  printf("\n  %sRunning all %d test%s…%s\n",
         __jacl_test_color() ? COLOR_TEST : "",
         total,
         total == 1 ? "" : "s",
         __jacl_test_color() ? COLOR_NORM : "");

  for (int i = 0; i < __jacl_test_suite_count; i++) {
    test_suite_t *suite = &__jacl_test_suites[i];

    // Add blank line before header, keep header on its own line
    printf("\n  %s%s - %s: %s tests%s\n\n",
           __jacl_test_color() ? COLOR_HEAD : "",
           suite->suite, suite->unit, suite->type,
           __jacl_test_color() ? COLOR_NORM : "");

    for (int j = 0; j < suite->test_count; j++) {
      __jacl_test_run(suite, &suite->tests[j]);
    }
  }

  return __jacl_test_stats.failed > 0;
}

/* ============================================================= */
/* Print Summary                                                 */
/* ============================================================= */
static inline void __jacl_test_report(void) {
  if (!__jacl_test_stats.total) {
    puts("No tests run.");

    return;
  }

  printf("\n%s\n",
    __jacl_test_stats.failed
    ? (__jacl_test_color() ? COLOR_FAIL
      "  ❌ SOME CHECKS FAILED ❌" COLOR_NORM
      : "  ❌ SOME CHECKS FAILED ❌")
    : (__jacl_test_color() ? COLOR_PASS
      "  🎉 ALL CHECKS PASSED 🎉" COLOR_NORM
      : "  🎉 ALL CHECKS PASSED 🎉")
  );

  printf("\n  %s%d total   %s%d passed   %s%d failed   %s%d skipped%s\n\n",
    __jacl_test_color() ? COLOR_TEST : "",
    __jacl_test_stats.total,
    __jacl_test_color() ? COLOR_PASS : "",
		__jacl_test_stats.passed,
    __jacl_test_color() ? COLOR_FAIL : "",
    __jacl_test_stats.failed,
    __jacl_test_color() ? COLOR_SKIP : "",
		__jacl_test_stats.skipped,
    __jacl_test_color() ? COLOR_NORM : ""
  );
}

/* ============================================================= */
/* Test Assertions                                               */
/* ============================================================= */
#define ASSERT_TRUE(x) do { \
	if (!(x)) { \
		TEST_FAIL("\tFAIL: %s:%d - Value should be true\n" \
		          "\tTEST: %s", __FILE__, __LINE__, #x); \
	} \
} while(0)

#define ASSERT_FALSE(x) do { \
	if (x) { \
		TEST_FAIL("\tFAIL: %s:%d\n - Value should be false\n" \
		          "\tTEST: %s", __FILE__, __LINE__, #x); \
	} \
} while(0)

#define ASSERT_EQ(a, b) do { \
	if (!((a) == (b))) { \
		TEST_FAIL("\tFAIL: %s:%d - Values should be equal\n" \
		          "\tTEST: %ld == %ld", __FILE__, __LINE__, (long)(a), (long)(b)); \
	} \
} while(0)

#define ASSERT_GT(a, b) do { \
	if (!((a) > (b))) { \
		TEST_FAIL("\tFAIL: %s:%d - Values should be more\n" \
		          "\tTEST: %ld > %ld", __FILE__, __LINE__, (long)(a), (long)(b)); \
	} \
} while(0)

#define ASSERT_LT(a, b) do { \
	if (!((a) < (b))) { \
		TEST_FAIL("\tFAIL: %s:%d - Values should be less\n" \
		          "\tTEST: %ld < %ld", __FILE__, __LINE__, (long)(a), (long)(b)); \
	} \
} while(0)

#define ASSERT_GE(a, b) do { \
	if (!((a) >= (b))) { \
		TEST_FAIL("\tFAIL: %s:%d - Values shouldn't be less\n" \
		          "\tTEST: %ld >= %ld", __FILE__, __LINE__, (long)(a), (long)(b)); \
	} \
} while(0)

#define ASSERT_LE(a, b) do { \
	if (!((a) <= (b))) { \
		TEST_FAIL("\tFAIL: %s:%d - Values shouldn't be more\n" \
		          "\tTEST: %ld <= %ld", __FILE__, __LINE__, (long)(a), (long)(b)); \
	} \
} while(0)

#define ASSERT_NE(a, b) do { \
	if (!((a) != (b))) { \
		TEST_FAIL("\tFAIL: %s:%d - Values should be different\n" \
		          "\tTEST: %ld != %ld", __FILE__, __LINE__, (long)(a), (long)(b)); \
	} \
} while(0)

#define ASSERT_STR_EQ(expected, actual) do { \
	if (!expected || !actual || strcmp((expected), (actual)) != 0) { \
		TEST_FAIL("\tFAIL: %s:%d - Strings not equal\n" \
		          "\tTEST: \"%s\" == \"%s\"", \
		          __FILE__, __LINE__, \
		          (actual) ? (actual) : "(null)", \
		          (expected) ? (expected) : "(null)"); \
	} \
} while(0)

#define ASSERT_STR_NE(expected, actual) do { \
	if (expected && actual && strcmp((expected), (actual)) == 0) { \
		TEST_FAIL("\tFAIL: %s:%d - Strings should not match\n" \
		          "\tTEST: \"%s\" != \"%s\"", \
		          __FILE__, __LINE__, \
		          (actual) ? (actual) : "(null)", \
		          (expected) ? (expected) : "(null)"); \
	} \
} while(0)

#define ASSERT_NULL(ptr) do { \
	if ((ptr) != NULL) { \
		TEST_FAIL("\tFAIL: %s:%d - Expected NULL pointer\n" \
		          "\tTEST: %p != NULL", __FILE__, __LINE__, (ptr)); \
	} \
} while(0)

#define ASSERT_NOT_NULL(ptr) do { \
	if ((ptr) == NULL) { \
		TEST_FAIL("\tFAIL: %s:%d - Expected non-NULL pointer\n" \
		          "\tTEST: NULL != NULL", __FILE__, __LINE__); \
	} \
} while(0)

#define ASSERT_APPROX(expected, actual, tolerance) do { \
	double diff = (double)(expected) - (double)(actual); \
	if (diff < 0) diff = -diff; \
	if (diff > (tolerance)) { \
		TEST_FAIL("\tFAIL: %s:%d - not within tolerance\n" \
		          "\tTEST: %f ≈ %f\n" \
		          "\tWITH: ϵ = %f, Δ = %f", \
		          __FILE__, __LINE__, (actual), (expected), (double)(tolerance), diff); \
	} \
} while(0)

#define ASSERT_COMPLEX(expt_real, expt_imgn, actual, tolerance) do { \
	  double _r = creal(actual); \
	  double _i = cimag(actual); \
	  double diff_r = (double)(expt_real) - _r; \
	  double diff_i = (double)(expt_imgn) - _i; \
	  if (diff_r < 0) diff_r = -diff_r; \
	  if (diff_i < 0) diff_i = -diff_i; \
	  if (diff_r > (tolerance) || diff_i > (tolerance)) { \
			TEST_FAIL("\tFAIL: %s:%d - complex not in tolerance\n" \
			          "\tTEST: %.6fr%+.6fi ≈ %.6fr%+.6fi\n" \
			          "\tWITH: ϵ = %f, Δr = %f, Δi = %f", \
			          __FILE__, __LINE__, _r, _i, \
			          (double)(expt_real), (double)(expt_imgn), \
			          (double)(tolerance), diff_r, diff_i); \
		} \
} while(0)

/* ============================================================= */
/* Test Utilities                                                */
/* ============================================================= */
#define TEST_FAIL(fmt,...) \
  do { __jacl_test_failed = __jacl_test_errored = 1; \
    snprintf(__jacl_test_error, sizeof(__jacl_test_error), fmt, \
      ##__VA_ARGS__); return; } while(0)

#define TEST_SKIP(reason) do { \
  printf("\n	%sSKIP: %s%s\n\n", \
    __jacl_test_color() ? COLOR_SKIP : "", \
		reason \
    __jacl_test_color() ? COLOR_NORM : ""); \
  __jacl_test_stats.skipped++; \
  return; \
} while(0)

#define TEST_INFO(format, ...) do { \
  printf("	INFO: " format "\n", ##__VA_ARGS__); \
} while(0)

/* ============================================================= */
/* Test Definitions                                              */
/* ============================================================= */
#define TEST_TYPE(name) JACL_INIT(type) { __jacl_test_type = #name; }
#define TEST_UNIT(name) JACL_INIT(unit) { __jacl_test_unit = #name; }
#define TEST_SUITE(name) JACL_INIT(suite) { __jacl_test_suite = #name; }
#define TEST(name) \
	static void test_##name(void); \
	JACL_INIT(test) { __jacl_test_register(#name, test_##name); } \
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
#define TEST_MAIN() \
	int main(int argc, char**argv) { \
		int r = 0; \
		if (argc > 2 && !strcmp(argv[1], "--suite")) r = TEST_EACH(argv[2]); \
		else if (argc > 2 && !strcmp(argv[1], "--test")) r = TEST_ONLY(argv[2]); \
		else if (argc == 1) r = TEST_ALL(); \
		else { printf("Usage: %s [--suite S | --test T]\n", argv[0]); return 1; } \
		TEST_REPORT(); \
		fflush(stdout); \
		return r; \
	}
#else
#define TEST_MAIN() TEST_RUN()
#endif

#endif /* TESTING_H */
