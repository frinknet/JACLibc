/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef JACL_TEST_RUNNER_C
#define JACL_TEST_RUNNER_C
#include "runner.h"

/* ================================================================ */
/* Global State																										 */
/* ================================================================ */
jacl_test_stats_t jacl_stats = {0, 0, 0, 0};
int jacl_current_test_failed = 0;
char jacl_error_buffer[2048];
int jacl_has_error_message = 0;

static jacl_test_t jacl_tests[JACL_MAX_TESTS];
static int jacl_test_count = 0;

/* ================================================================ */
/* ANSI Color Codes																								 */
/* ================================================================ */
#define COLOR_RED			"\x1b[31m"
#define COLOR_GREEN		"\x1b[32m"
#define COLOR_YELLOW	"\x1b[33m"
#define COLOR_BLUE		"\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN		"\x1b[36m"
#define COLOR_RESET		"\x1b[0m"

/* Check if output supports colors */
static int use_colors(void) {
		static int checked = 0;
		static int has_colors = 0;
		
		if (!checked) {
				const char *term = getenv("TERM");
				has_colors = term && strcmp(term, "dumb") != 0;
				checked = 1;
		}
		
		return has_colors;
}

/* ================================================================ */
/* Test Registration																							 */
/* ================================================================ */
void jacl_register_test(const char *suite, const char *name, jacl_test_func_t func) {
		if (jacl_test_count >= JACL_MAX_TESTS) {
				fprintf(stderr, "Error: Too many tests registered (max: %d)\n", JACL_MAX_TESTS);
				exit(1);
		}
		
		jacl_tests[jacl_test_count].name = name;
		jacl_tests[jacl_test_count].suite = suite;
		jacl_tests[jacl_test_count].func = func;
		jacl_test_count++;
}

/* ================================================================ */
/* Test Execution																									 */
/* ================================================================ */
static void run_single_test(const jacl_test_t *test) {
		jacl_current_test_failed = 0;
		jacl_has_error_message = 0;
		memset(jacl_error_buffer, 0, sizeof(jacl_error_buffer));
		jacl_stats.total++;
		
		printf("%s%-60s%s ", 
					 use_colors() ? COLOR_BLUE : "", 
					 test->name,
					 use_colors() ? COLOR_RESET : "");
		fflush(stdout);
		
		/* Run the test */
		test->func();
		
		if (jacl_current_test_failed) {
				printf("%s[FAIL]%s\n", 
							 use_colors() ? COLOR_RED : "", 
							 use_colors() ? COLOR_RESET : "");
				if (jacl_has_error_message) {
						printf("\n%s\n\n", jacl_error_buffer);
				}
				jacl_stats.failed++;
		} else {
				printf("%s[PASS]%s\n", 
							 use_colors() ? COLOR_GREEN : "", 
							 use_colors() ? COLOR_RESET : "");
				jacl_stats.passed++;
		}
}

/* ================================================================ */
/* Test Runner Functions																					 */
/* ================================================================ */
int jacl_run_all_tests(void) {
		if (jacl_test_count == 0) {
				printf("No tests registered.\n");
				return 0;
		}
		
		printf("Running %d test%s...\n\n", 
					 jacl_test_count, jacl_test_count == 1 ? "" : "s");
		
		const char *current_suite = NULL;
		
		for (int i = 0; i < jacl_test_count; i++) {
				/* Print suite header if changed */
				if (!current_suite || strcmp(current_suite, jacl_tests[i].suite) != 0) {
						current_suite = jacl_tests[i].suite;
						printf("\n%s=== %s ===%s\n", 
									 use_colors() ? COLOR_CYAN : "", 
									 current_suite,
									 use_colors() ? COLOR_RESET : "");
				}
				
				run_single_test(&jacl_tests[i]);
		}
		
		return jacl_stats.failed > 0 ? 1 : 0;
}

int jacl_run_suite(const char *suite_name) {
		int found = 0;
		
		printf("Running tests in suite '%s'...\n\n", suite_name);
		printf("%s=== %s ===%s\n", 
					 use_colors() ? COLOR_CYAN : "", 
					 suite_name,
					 use_colors() ? COLOR_RESET : "");
		
		for (int i = 0; i < jacl_test_count; i++) {
				if (strcmp(jacl_tests[i].suite, suite_name) == 0) {
						found = 1;
						run_single_test(&jacl_tests[i]);
				}
		}
		
		if (!found) {
				printf("No tests found in suite '%s'\n", suite_name);
				return 1;
		}
		
		return jacl_stats.failed > 0 ? 1 : 0;
}

int jacl_run_test(const char *test_name) {
		for (int i = 0; i < jacl_test_count; i++) {
				if (strcmp(jacl_tests[i].name, test_name) == 0) {
						printf("Running test '%s' from suite '%s'...\n\n", 
									 test_name, jacl_tests[i].suite);
						printf("%s=== %s ===%s\n", 
									 use_colors() ? COLOR_CYAN : "", 
									 jacl_tests[i].suite,
									 use_colors() ? COLOR_RESET : "");
						
						run_single_test(&jacl_tests[i]);
						return jacl_stats.failed > 0 ? 1 : 0;
				}
		}
		
		printf("Test '%s' not found\n", test_name);
		return 1;
}

void jacl_print_summary(void) {
		if (jacl_stats.total == 0) {
				printf("No tests were run.\n");
				return;
		}

		if (jacl_stats.failed == 0 && jacl_stats.total > 0) {
				printf("%s - - - 🎉 ALL CHECKS PASSED! 🎉 - - - %s\n\n", 
							 use_colors() ? COLOR_GREEN : "", 
							 use_colors() ? COLOR_RESET : "");
		} else if (jacl_stats.failed > 0) {
				printf("%s - - - ❌ SOME CHECKS FAILED ❌ - - - %s\n\n", 
							 use_colors() ? COLOR_RED : "", 
							 use_colors() ? COLOR_RESET : "");
		}

		printf("\t%d Tests\n\n", jacl_stats.total);

		printf("\t%s%d Passed - %.1f%%%s\n", 
					 use_colors() ? COLOR_GREEN : "", 
					 jacl_stats.passed,
					(double)jacl_stats.passed / jacl_stats.total * 100.0,
					 use_colors() ? COLOR_RESET : "");
		
		if (jacl_stats.failed > 0) {
				printf("\t%s%d Failed - %.1f%%%s\n", 
							 use_colors() ? COLOR_RED : "", 
							 jacl_stats.failed,
							(double)jacl_stats.failed / jacl_stats.total * 100.0,
							 use_colors() ? COLOR_RESET : "");
		}
		
		if (jacl_stats.skipped > 0) {
						

				printf("\t%s%d Skiped - %.1f%%%s\n", 
							 use_colors() ? COLOR_YELLOW : "", 
							 jacl_stats.skipped,
							(double)jacl_stats.skipped / jacl_stats.total * 100.0,
							 use_colors() ? COLOR_RESET : "");
		}

		printf("\n -----------------------------------\n\n");
}

#endif /* JACL_TEST_RUNNER_C */
