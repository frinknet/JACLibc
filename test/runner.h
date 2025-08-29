
#ifndef JACL_TEST_RUNNER_H
#define JACL_TEST_RUNNER_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* ================================================================ */
/* Test Framework Configuration																			*/
/* ================================================================ */
#ifndef JACL_MAX_TESTS
#define JACL_MAX_TESTS 1000
#endif
#ifndef JACL_MAX_NAME_LEN
#define JACL_MAX_NAME_LEN 128
#endif

/* ================================================================ */
/* Test Result Counters																						 */
/* ================================================================ */
typedef struct {
		int total;
		int passed;
		int failed;
		int skipped;
} jacl_test_stats_t;

extern jacl_test_stats_t jacl_stats;
extern int jacl_current_test_failed;

// Error buffering for clean output
extern char jacl_error_buffer[2048];
extern int jacl_has_error_message;

/* ================================================================ */
/* Test Function Type																							 */
/* ================================================================ */
typedef void (*jacl_test_func_t)(void);
typedef struct {
		const char *name;
		const char *suite;
		jacl_test_func_t func;
} jacl_test_t;

/* ================================================================ */
/* Test Registration																							 */
/* ================================================================ */
void jacl_register_test(const char *suite, const char *name, jacl_test_func_t func);
int jacl_run_all_tests(void);
int jacl_run_suite(const char *suite_name);
int jacl_run_test(const char *test_name);
void jacl_print_summary(void);

/* ================================================================ */
/* Test Definition Macros																					 */
/* ================================================================ */
#define TEST_SUITE(suite_name) \
		static const char *_current_suite = #suite_name

#define TEST(test_name) \
		static void test_##test_name(void); \
		static void __attribute__((constructor)) register_test_##test_name(void) { \
				jacl_register_test(_current_suite ? _current_suite : "default", \
													#test_name, test_##test_name); \
		} \
		static void test_##test_name(void)

/* ================================================================ */
/* Assertion Macros (Buffered with 2-Space Indentation)						*/
/* ================================================================ */
#define ASSERT_TRUE(condition) \
		do { \
				if (!(condition)) { \
						jacl_current_test_failed = 1; \
						jacl_has_error_message = 1; \
						snprintf(jacl_error_buffer, sizeof(jacl_error_buffer), \
								"\tFAIL: %s:%d - Expected true, got false\n" \
								"\tTEST: %s == true", \
								__FILE__, __LINE__, #condition); \
						return; \
				} \
		} while(0)

#define ASSERT_FALSE(condition) \
		do { \
				if (condition) { \
						jacl_current_test_failed = 1; \
						jacl_has_error_message = 1; \
						snprintf(jacl_error_buffer, sizeof(jacl_error_buffer), \
								"\tFAIL: %s:%d - Expected false, got true\n" \
								"\tTEST: %s == false", \
								__FILE__, __LINE__, #condition); \
						return; \
				} \
		} while(0)

#define ASSERT_EQ(expected, actual) \
		do { \
				if ((expected) != (actual)) { \
						jacl_current_test_failed = 1; \
						jacl_has_error_message = 1; \
						snprintf(jacl_error_buffer, sizeof(jacl_error_buffer), \
								"\tFAIL: %s:%d - Values not equal\n" \
								"\tTEST: %ld == %ld", \
								__FILE__, __LINE__, \
								(long)(actual), \
								(long)(expected) \
							); \
						return; \
				} \
		} while(0)

#define ASSERT_NE(expected, actual) \
		do { \
				if ((expected) == (actual)) { \
						jacl_current_test_failed = 1; \
						jacl_has_error_message = 1; \
						snprintf(jacl_error_buffer, sizeof(jacl_error_buffer), \
								"\tFAIL: %s:%d - Values should not be equal\n" \
								"\tTEST: %ld != %ld", \
								__FILE__, __LINE__, \
								(long)(actual), \
								(long)(expected) \
							); \
						return; \
				} \
		} while(0)

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

/* ================================================================ */
/* Main Test Runner Macro																					 */
/* ================================================================ */
#define TEST_MAIN() \
		int main(int argc, char *argv[]) { \
				int result = 0; \
				if (argc > 1) { \
						if (strcmp(argv[1], "--suite") == 0 && argc > 2) { \
								result = jacl_run_suite(argv[2]); \
						} else if (strcmp(argv[1], "--test") == 0 && argc > 2) { \
								result = jacl_run_test(argv[2]); \
						} else { \
								printf("Usage: %s [--suite SUITE_NAME | --test TEST_NAME]\n", argv[0]); \
								return 1; \
						} \
				} else { \
						result = jacl_run_all_tests(); \
				} \
				jacl_print_summary(); \
				return result; \
		}

#endif /* JACL_TEST_RUNNER_H */
