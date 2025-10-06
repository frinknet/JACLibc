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
  const char* name;
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
#define COLOR_RED    "\x1b[31m"
#define COLOR_GREEN  "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE   "\x1b[34m"
#define COLOR_CYAN   "\x1b[36m"
#define COLOR_RESET  "\x1b[0m"

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
/* Registration                                                  */
/* ============================================================= */
static inline const char* __jacl_test_type(const char* type) {
  static const char* current = "unit";
  if (type) current = type;
  return current;
}

static inline const char* __jacl_test_suite(const char* suite) {
  static const char* current = "default";
  if (suite) current = suite;
  return current;
}

static inline void __jacl_test_register(const char *test_name, test_func_t func) {
	// Build suite name from current type and suite
	char name_buf[256];

	snprintf(name_buf, sizeof(name_buf), "%s - %s tests", __jacl_test_suite(NULL), __jacl_test_type(NULL));

	// Find or create suite
	test_suite_t *suite = NULL;

	for (int i = 0; i < __jacl_test_suite_count; i++) {
		if (!strcmp(__jacl_test_suites[i].name, name_buf)) {
			suite = &__jacl_test_suites[i];
			break;
		}
	}

	if (!suite) {
		// Create new suite
		if (__jacl_test_suite_count >= JACL_MAX_SUITES) {
			fprintf(stderr, "Error: too many suites (>%d)\n", JACL_MAX_SUITES);
			exit(1);
		}

		suite = &__jacl_test_suites[__jacl_test_suite_count++];

		// Allocate permanent storage for suite name
		char *permanent_name = (char*)malloc(strlen(name_buf) + 1);

		strcpy(permanent_name, name_buf);

		suite->name = permanent_name;
		suite->test_count = 0;
		suite->stats = (test_stats_t){0, 0, 0, 0};
	}

	// Add test to suite (rest unchanged)
	if (suite->test_count >= JACL_MAX_TESTS) {
		fprintf(stderr, "Error: too many tests in suite '%s' (>%d)\n", suite->name, JACL_MAX_TESTS);
		exit(1);
	}

	suite->tests[suite->test_count].name = test_name;
	suite->tests[suite->test_count].func = func;
	suite->test_count++;
}

/* ============================================================= */
/* Test Utility Macros                                           */
/* ============================================================= */
#define TEST_FAIL(fmt,...) \
  do { __jacl_test_failed = __jacl_test_errored = 1; \
    snprintf(__jacl_test_error, sizeof(__jacl_test_error), fmt, \
      ##__VA_ARGS__); return; } while(0)

#define ASSERT_TRUE(cond) do{ if(!(cond)) TEST_FAIL("\tFAIL: %s:%d\n" \
  "\tTEST: %s == true", __FILE__,__LINE__,#cond);}while(0)

#define ASSERT_FALSE(cond) do{ if( (cond)) TEST_FAIL("\tFAIL: %s:%d\n" \
  "\tTEST: %s == false", __FILE__,__LINE__,#cond);}while(0)

#define ASSERT_EQ(e,a) do{ if((e)!=(a)) TEST_FAIL("\tFAIL: %s:%d\n" \
  "\tTEST: %ld == %ld", __FILE__,__LINE__,(long)(a),(long)(e));}while(0)

#define ASSERT_NE(e,a) do{ if((e)==(a)) TEST_FAIL("\tFAIL: %s:%d\n" \
  "\tTEST: values should differ", __FILE__,__LINE__);}while(0)

#define ASSERT_LT(a, b) do { \
  if (!((a) < (b))) { \
    TEST_FAIL("\tFAIL: %s:%d - Values should be less\n" \
      "\tTEST: %ld < %ld", __FILE__, __LINE__, (long)(a), (long)(b)); \
  } \
} while(0)

#define TEST_SKIP(reason) do { \
  printf("	SKIP: %s\n", reason); \
  __jacl_test_stats.skipped++; \
  return; \
} while(0)

#define TEST_INFO(format, ...) do { \
  printf("	INFO: " format "\n", ##__VA_ARGS__); \
} while(0)

#define TEST_RUN(test) __jacl_test_run_one(test)

#define TEST_RUN_SUITE(name) __jacl_test_run_suite(name)

#define TEST_RUN_ALL() __jacl_test_run_all()

/* ============================================================= */
/* Convenience MAIN Macro                                        */
/* ============================================================= */
#ifndef NO_TEST_MAIN
#include <static.h> // include everything

#define TEST_MAIN() \
  int main(int argc, char**argv) { \
    int r = 0; \
    if (argc > 2 && !strcmp(argv[1], "--suite")) r = __jacl_test_run_suite(argv[2]); \
    else if (argc > 2 && !strcmp(argv[1], "--test")) r = __jacl_test_run_one(argv[2]); \
    else if (argc == 1) r = __jacl_test_run_all(); \
    else { printf("Usage: %s [--suite S | --test T]\n", argv[0]); return 1; } \
    __jacl_test_summary(); \
    return r; \
  }
#endif

/* ============================================================= */
/* Assertion Macros (buffered)                                   */
/* ============================================================= */
#define ASSERT_GT(a, b) do { \
  if (!((a) > (b))) { \
    TEST_FAIL("\tFAIL: %s:%d - Values should be more\n" \
      "\tTEST: %ld > %ld", __FILE__, __LINE__, (long)(a), (long)(b)); \
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

#define ASSERT_FLOAT_EQ(expected, actual, epsilon) do { \
  double diff = (double)(expected) - (double)(actual); \
  if (diff < 0) diff = -diff; \
  if (diff > (epsilon)) { \
    TEST_FAIL("\tFAIL: %s:%d - Floats not equal within epsilon\n" \
      "\tTEST: %f != %f\n" \
      "\tWITH: ϵ = %f, Δ = %f", \
      __FILE__, __LINE__, (actual), (expected), (double)(epsilon), diff); \
  } \
} while(0)

/* ============================================================= */
/* Test Definition DSL                                           */
/* ============================================================= */
#define TEST_TYPE(name) __jacl_test_type(#name)

#define TEST_SUITE(name) __jacl_test_suite(#name)

#if __has_attribute(constructor)
  // Auto-registration
  #define TEST(name) \
    static void test_##name(void); \
    static void __attribute__((constructor)) \
    register_##name(void) { \
      __jacl_test_register(#name, test_##name); \
    } \
    static void test_##name(void)
#else
  // Static initializer fallback
  #define TEST(name) \
    static void test_##name(void); \
    static int register_##name##_init = \
      (__jacl_test_register(#name, test_##name), 0); \
    static void test_##name(void)
#endif

/* ============================================================= */
/* Runner Internals                                              */
/* ============================================================= */
static inline void __jacl_test_run_single(test_suite_t *suite, test_t *t) {
  __jacl_test_failed = __jacl_test_errored = 0;

  memset(__jacl_test_error, 0, sizeof(__jacl_test_error));

  __jacl_test_stats.total++;

  suite->stats.total++;

  printf("%s%-60s%s ", __jacl_test_color() ? COLOR_BLUE : "", t->name, __jacl_test_color() ? COLOR_RESET : "");

  fflush(stdout);

  t->func();

  if (__jacl_test_failed) {
    printf("%s[FAIL]%s\n", __jacl_test_color() ? COLOR_RED : "", __jacl_test_color() ? COLOR_RESET : "");

    if (__jacl_test_errored) puts(__jacl_test_error), putchar('\n');

    __jacl_test_stats.failed++;

    suite->stats.failed++;
  } else {
    printf("%s[PASS]%s\n", __jacl_test_color() ? COLOR_GREEN : "", __jacl_test_color() ? COLOR_RESET : "");

    __jacl_test_stats.passed++;

    suite->stats.passed++;
  }
}

/* ============================================================= */
/* Public Runner API                                             */
/* ============================================================= */
static inline int __jacl_test_run_all(void) {
  if (__jacl_test_suite_count == 0) {
    puts("No tests registered.");

    return 0;
  }

  // Count total tests
  int total = 0;

  for (int i = 0; i < __jacl_test_suite_count; i++) {
    total += __jacl_test_suites[i].test_count;
  }

  printf("Running %d test%s…\n\n", total, total == 1 ? "" : "s");

  // Run all suites
  for (int i = 0; i < __jacl_test_suite_count; i++) {
    test_suite_t *suite = &__jacl_test_suites[i];

    printf("\n%s=== %s ===%s\n", __jacl_test_color() ? COLOR_CYAN : "",
           suite->name, __jacl_test_color() ? COLOR_RESET : "");

    for (int j = 0; j < suite->test_count; j++) {
      __jacl_test_run_single(suite, &suite->tests[j]);
    }
  }

  return __jacl_test_stats.failed > 0;
}

static inline int __jacl_test_run_suite(const char *name) {
  test_suite_t *suite = NULL;

  // Find suite
  for (int i = 0; i < __jacl_test_suite_count; i++) {
    if (!strcmp(__jacl_test_suites[i].name, name)) {
      suite = &__jacl_test_suites[i];

      break;
    }
  }

  if (!suite) {
    printf("Suite '%s' not found\n", name);

    return 1;
  }

  printf("Suite '%s'\n\n", name);
  printf("%s=== %s ===%s\n", __jacl_test_color() ? COLOR_CYAN : "", name, __jacl_test_color() ? COLOR_RESET : "");

  for (int i = 0; i < suite->test_count; i++) {
    __jacl_test_run_single(suite, &suite->tests[i]);
  }

  return __jacl_test_stats.failed > 0;
}

static inline int __jacl_test_run_one(const char *test_name) {
  // Search all suites for test
  for (int i = 0; i < __jacl_test_suite_count; i++) {
    test_suite_t *suite = &__jacl_test_suites[i];

    for (int j = 0; j < suite->test_count; j++) {
      if (!strcmp(suite->tests[j].name, test_name)) {
        printf("Test '%s' (suite %s)\n\n", test_name, suite->name);
        printf("%s=== %s ===%s\n", __jacl_test_color() ? COLOR_CYAN : "", suite->name, __jacl_test_color() ? COLOR_RESET : "");
        __jacl_test_run_single(suite, &suite->tests[j]);

        return __jacl_test_stats.failed > 0;
      }
    }
  }

  printf("Test '%s' not found\n", test_name);

  return 1;
}

/* ============================================================= */
/* Summary                                                       */
/* ============================================================= */
static inline void __jacl_test_summary(void) {
  if (!__jacl_test_stats.total) {
    puts("No tests run.");

    return;
  }

  puts(
    __jacl_test_stats.failed
    ? (__jacl_test_color() ? COLOR_RED
      "❌ SOME CHECKS FAILED ❌" COLOR_RESET
      : "❌ SOME CHECKS FAILED ❌")
    : (__jacl_test_color() ? COLOR_GREEN
      "🎉 ALL CHECKS PASSED 🎉" COLOR_RESET
      : "🎉 ALL CHECKS PASSED 🎉")
  );

  printf("\n\t%d total   %d passed   %d failed   %d skipped\n",
    __jacl_test_stats.total, __jacl_test_stats.passed,
    __jacl_test_stats.failed, __jacl_test_stats.skipped
  );
}

#endif /* TESTING_H */
