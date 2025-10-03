/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef TESTING_H
#define TESTING_H
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

/* ============================================================= */
/* Configuration                                                  */
/* ============================================================= */
#ifndef JACL_MAX_TESTS
#define JACL_MAX_TESTS 1000
#endif
#ifndef JACL_MAX_NAME_LEN
#define JACL_MAX_NAME_LEN 128
#endif

/* ============================================================= */
/* Result Counters                                               */
/* ============================================================= */
typedef struct {
  int total;
  int passed;
  int failed;
  int skipped;
} jacl_test_stats_t;

/* ------------- global state (header-only, so `static`) -------- */
static jacl_test_stats_t jacl_stats               = {0,0,0,0};
static int               jacl_current_test_failed = 0;
static int               jacl_has_error_message   = 0;
static char              jacl_error_buffer[2048]  = {0};

typedef void (*jacl_test_func_t)(void);
typedef struct {
  const char      *name;
  const char      *suite;
  jacl_test_func_t func;
} jacl_test_t;

static jacl_test_t jacl_tests[JACL_MAX_TESTS];
static int         jacl_test_count = 0;

/* ============================================================= */
/* ANSI Colour Helper                                            */
/* ============================================================= */
#define COLOR_RED   "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW"\x1b[33m"
#define COLOR_BLUE  "\x1b[34m"
#define COLOR_CYAN  "\x1b[36m"
#define COLOR_RESET "\x1b[0m"

static inline int jacl_use_colors(void) {
  static int checked = 0, has = 0;

  if (!checked) {
    const char *t = getenv("TERM");
    has = t && strcmp(t,"dumb");
    checked = 1;
  }

  return has;
}

/* ============================================================= */
/* Registration                                                  */
/* ============================================================= */
static inline void jacl_register_test(const char *suite, const char *name, jacl_test_func_t func) {
  if (jacl_test_count >= JACL_MAX_TESTS) {
    fprintf(stderr,"Error: too many tests (>%d)\n",JACL_MAX_TESTS);

    exit(1);
  }

  jacl_tests[jacl_test_count++] = (jacl_test_t){ name, suite, func };
}

/* ============================================================= */
/* Assertion Macros (buffered)                                   */
/* ============================================================= */
#define JACL_FAIL(fmt,...)                                    \
  do { jacl_current_test_failed = jacl_has_error_message = 1; \
  	snprintf(jacl_error_buffer,sizeof jacl_error_buffer, fmt, \
      ##__VA_ARGS__); return; } while(0)

#define ASSERT_TRUE(cond) do{ if(!(cond)) JACL_FAIL("\tFAIL: %s:%d\n" \
  "\tTEST: %s == true", __FILE__,__LINE__,#cond);}while(0)

#define ASSERT_FALSE(cond) do{ if( (cond)) JACL_FAIL("\tFAIL: %s:%d\n" \
  "\tTEST: %s == false", __FILE__,__LINE__,#cond);}while(0)

#define ASSERT_EQ(e,a) do{ if((e)!=(a)) JACL_FAIL("\tFAIL: %s:%d\n" \
  "\tTEST: %ld == %ld", __FILE__,__LINE__,(long)(a),(long)(e));}while(0)

#define ASSERT_NE(e,a)      do{ if((e)==(a)) JACL_FAIL("\tFAIL: %s:%d\n" \
  "\tTEST: values should differ", __FILE__,__LINE__);}while(0)

#define ASSERT_LT(a, b) \
  do { \
    if (!((a) < (b))) { \
      jacl_current_test_failed = 1; \
      jacl_has_error_message = 1; \
      snprintf(jacl_error_buffer, sizeof(jacl_error_buffer), \
        "\tFAIL: %s:%d - Values should be less\n" \
        "\tTEST: %ld < %ld", \
        __FILE__, __LINE__, (long)(a), (long)(b)); \
      return; \
    } \
  } while(0)

#define ASSERT_GT(a, b) \
		do { \
				if (!((a) > (b))) { \
						jacl_current_test_failed = 1; \
						jacl_has_error_message = 1; \
						snprintf(jacl_error_buffer, sizeof(jacl_error_buffer), \
								"\tFAIL: %s:%d - Values should be more\n" \
								"\tTEST: %ld > %ld", \
								__FILE__, __LINE__, (long)(a), (long)(b)); \
						return; \
				} \
		} while(0)

#define ASSERT_STR_EQ(expected, actual) \
		do { \
				if (!expected || !actual || strcmp((expected), (actual)) != 0) { \
						jacl_current_test_failed = 1; \
						jacl_has_error_message = 1; \
						snprintf(jacl_error_buffer, sizeof(jacl_error_buffer), \
								"\tFAIL: %s:%d - Strings not equal\n" \
								"\tTEST: \"%s\" == \"%s\"", \
								__FILE__, __LINE__, \
								(actual) ? (actual) : "(null)", \
								(expected) ? (expected) : "(null)" \
						); \
						return; \
				} \
		} while(0)

#define ASSERT_STR_NE(expected, actual) \
		do { \
				if (expected && actual && strcmp((expected), (actual)) == 0) { \
						jacl_current_test_failed = 1; \
						jacl_has_error_message = 1; \
						snprintf(jacl_error_buffer, sizeof(jacl_error_buffer), \
								"\tFAIL: %s:%d - Strings should not match\n" \
								"\tTEST: \"%s\" != \"%s\"", \
								__FILE__, __LINE__, \
								(actual) ? (actual) : "(null)", \
								(expected) ? (expected) : "(null)" \
							); \
						return; \
				} \
		} while(0)

#define ASSERT_NULL(ptr) \
		do { \
				if ((ptr) != NULL) { \
						jacl_current_test_failed = 1; \
						jacl_has_error_message = 1; \
						snprintf(jacl_error_buffer, sizeof(jacl_error_buffer), \
								"\tFAIL: %s:%d - Expected NULL pointer\n" \
								"\tTEST: %p != NULL", \
								__FILE__, __LINE__, (ptr) \
						); \
						return; \
				} \
		} while(0)

#define ASSERT_NOT_NULL(ptr) \
		do { \
				if ((ptr) == NULL) { \
						jacl_current_test_failed = 1; \
						jacl_has_error_message = 1; \
						snprintf(jacl_error_buffer, sizeof(jacl_error_buffer), \
								"\tFAIL: %s:%d - Expected non-NULL pointer\n" \
								"\tTEST: NULL != NULL", \
								__FILE__, __LINE__ \
						); \
						return; \
				} \
		} while(0)

#define ASSERT_FLOAT_EQ(expected, actual, epsilon) \
		do { \
				double diff = (double)(expected) - (double)(actual); \
				if (diff < 0) diff = -diff; \
				if (diff > (epsilon)) { \
						jacl_current_test_failed = 1; \
						jacl_has_error_message = 1; \
						snprintf(jacl_error_buffer, sizeof(jacl_error_buffer), \
								"\tFAIL: %s:%d - Floats not equal within epsilon\n" \
								"\tTEST: %f != %f\n" \
								"\tWITH: ϵ = %f, Δ = %f", \
								__FILE__, __LINE__, \
								(actual), \
								(expected), \
								(double)(epsilon), diff \
							); \
						return; \
				} \
		} while(0)

/* ============================================================= */
/* Test Definition DSL                                           */
/* ============================================================= */
#define TEST_SUITE(name) static const char *_current_suite = #name

#define TEST(name)                         \
  static void test_##name(void);           \
  static void __attribute__((constructor)) \
  register_##name(void) {                  \
    jacl_register_test(_current_suite ?    \
      _current_suite : "default",          \
      #name, test_##name);                 \
    }                                      \
    static void test_##name(void)

/* ============================================================= */
/* Runner Internals                                              */
/* ============================================================= */
static inline void jacl_run_single(const jacl_test_t *t) {
  jacl_current_test_failed = jacl_has_error_message = 0;

  memset(jacl_error_buffer,0,sizeof jacl_error_buffer);

  jacl_stats.total++;

  printf("%s%-60s%s ", jacl_use_colors()?COLOR_BLUE :"", t->name, jacl_use_colors()?COLOR_RESET:"");
  fflush(stdout);
  t->func();

  if (jacl_current_test_failed) {
    printf("%s[FAIL]%s\n",jacl_use_colors()?COLOR_RED:"",jacl_use_colors()?COLOR_RESET:"");

    if (jacl_has_error_message) puts(jacl_error_buffer), putchar('\n');

    jacl_stats.failed++;
  } else {
    printf("%s[PASS]%s\n",jacl_use_colors()?COLOR_GREEN:"",jacl_use_colors()?COLOR_RESET:"");

    jacl_stats.passed++;
  }
}

/* ============================================================= */
/* Public Runner API                                             */
/* ============================================================= */
static inline int jacl_run_all_tests(void) {
  if (!jacl_test_count) { puts("No tests registered."); return 0; }

  printf("Running %d test%s…\n\n", jacl_test_count, jacl_test_count==1?"":"s");

  const char *cur = NULL;

  for (int i=0;i<jacl_test_count;i++) {
    if (!cur || strcmp(cur,jacl_tests[i].suite)) {
      cur = jacl_tests[i].suite;

      printf("\n%s=== %s ===%s\n", jacl_use_colors()?COLOR_CYAN:"", cur, jacl_use_colors()?COLOR_RESET:"");
    }

    jacl_run_single(&jacl_tests[i]);
  }

  return jacl_stats.failed>0;
}

static inline int jacl_run_suite(const char *s) {
  int hit = 0;
  printf("Suite '%s'\n\n",s);
  printf("%s=== %s ===%s\n",  jacl_use_colors()?COLOR_CYAN:"", s, jacl_use_colors()?COLOR_RESET:"");

  for (int i=0;i<jacl_test_count;i++) {
    if (!strcmp(jacl_tests[i].suite,s)) {
			hit=1;
			jacl_run_single(&jacl_tests[i]);
		}
	}

  if (!hit) {
		printf("No tests in suite '%s'\n",s);

		return 1;
	}

  return jacl_stats.failed>0;
}

static inline int jacl_run_test(const char *n) {
  for (int i = 0; i < jacl_test_count; i++) {
    if (!strcmp(jacl_tests[i].name, n)) {
      printf("Test '%s' (suite %s)\n\n", n, jacl_tests[i].suite);
      printf("%s=== %s ===%s\n", jacl_use_colors()?COLOR_CYAN:"", jacl_tests[i].suite, jacl_use_colors()?COLOR_RESET:"");
      jacl_run_single(&jacl_tests[i]);

			return jacl_stats.failed > 0;
    }
	}

  printf("Test '%s' not found\n",n); return 1;
}

/* ============================================================= */
/* Summary                                                       */
/* ============================================================= */
static inline void jacl_print_summary(void) {
  if (!jacl_stats.total) {
		puts("No tests run.");

		return;
	}

  puts(
		jacl_stats.failed
		? (jacl_use_colors()?COLOR_RED
    "❌ SOME CHECKS FAILED ❌" COLOR_RESET
    : "❌ SOME CHECKS FAILED ❌")
    : (jacl_use_colors()?COLOR_GREEN
    "🎉 ALL CHECKS PASSED 🎉" COLOR_RESET
    : "🎉 ALL CHECKS PASSED 🎉")
	);

  printf("\n\t%d total   %d passed   %d failed   %d skipped\n",
    jacl_stats.total, jacl_stats.passed,
    jacl_stats.failed, jacl_stats.skipped
	);
}

/* ================================================================ */
/* Test Skip and Utility Macros																		 */
/* ================================================================ */
#define SKIP_TEST(reason) \
		do { \
				printf("	SKIP: %s\n", reason); \
				jacl_stats.skipped++; \
				return; \
		} while(0)

#define TEST_INFO(format, ...) \
		do { \
				printf("	INFO: " format "\n", ##__VA_ARGS__); \
		} while(0)

/* ============================================================= */
/* Convenience MAIN Macro                                        */
/* ============================================================= */
#define TEST_MAIN()                                                          \
  int main(int argc,char**argv) {                                            \
    int r=0;                                                                 \
    if(argc>2&&!strcmp(argv[1],"--suite")) r=jacl_run_suite(argv[2]);        \
    else if(argc>2&&!strcmp(argv[1],"--test")) r=jacl_run_test(argv[2]);     \
    else if(argc==1)                         r=jacl_run_all_tests();         \
    else { printf("Usage: %s [--suite S | --test T]\n",argv[0]); return 1; } \
    jacl_print_summary(); return r; }

#endif /* TESTING_H */
