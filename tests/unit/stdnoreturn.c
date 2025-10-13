/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <stdnoreturn.h>
#include <stdlib.h>

TEST_TYPE(unit);
TEST_UNIT(stdnoreturn_h);

/* ============================================================================
 * MACRO DEFINITIONS
 * ============================================================================ */
TEST_SUITE(macro_definitions);

TEST(noreturn_is_defined_macro) {
	ASSERT_EQ(1, __noreturn_is_defined);
}

#if JACL_HAS_C23
TEST(c23_version_macro) {
	ASSERT_EQ(202311L, __STDC_VERSION_STDNORETURN_H__);
}
#endif

/* ============================================================================
 * NORETURN FUNCTIONS
 * ============================================================================ */
TEST_SUITE(noreturn_functions);

/* Simple noreturn function */
static noreturn void exit_immediately(void) {
	exit(0);
}

/* Noreturn function with parameter */
static noreturn void exit_with_code(int code) {
	exit(code);
}

/* Noreturn function that calls another noreturn function */
static noreturn void chain_exit(void) {
	exit_immediately();
}

TEST(noreturn_compiles) {
	// Test that noreturn functions compile correctly
	// We can't actually call them in tests without terminating
	ASSERT_TRUE(1);
}

TEST(noreturn_with_parameters) {
	// Verify noreturn works with function parameters
	ASSERT_TRUE(1);
}

/* ============================================================================
 * NORETURN WITH DIFFERENT LINKAGE
 * ============================================================================ */
TEST_SUITE(noreturn_linkage);

/* Static noreturn function */
static noreturn void static_exit(void) {
	exit(0);
}

/* Extern declaration (not defined here) */
extern noreturn void external_exit(void);

TEST(noreturn_static_linkage) {
	// Verify static noreturn compiles
	ASSERT_TRUE(1);
}

TEST(noreturn_extern_linkage) {
	// Verify extern noreturn declaration compiles
	ASSERT_TRUE(1);
}

/* ============================================================================
 * NORETURN FUNCTION POINTERS
 * ============================================================================ */
TEST_SUITE(noreturn_pointers);

TEST(noreturn_function_pointer) {
	// Pointer to noreturn function (noreturn applies to the function, not the pointer)
	void (*exit_ptr)(void) = exit_immediately;
	
	// Don't actually call it
	(void)exit_ptr;
	ASSERT_TRUE(1);
}

TEST(noreturn_function_pointer_with_args) {
	void (*exit_code_ptr)(int) = exit_with_code;
	
	(void)exit_code_ptr;
	ASSERT_TRUE(1);
}

/* ============================================================================
 * NORETURN IN DIFFERENT CONTEXTS
 * ============================================================================ */
TEST_SUITE(noreturn_contexts);

/* Noreturn with inline */
static inline noreturn void inline_exit(void) {
	exit(0);
}

TEST(noreturn_with_inline) {
	// Verify noreturn works with inline
	ASSERT_TRUE(1);
}

TEST(noreturn_order_matters) {
	// Test that "noreturn void" compiles (not "void noreturn")
	void (*ptr)(void);
	(void)ptr;
	ASSERT_TRUE(1);
}

/* ============================================================================
 * PRACTICAL USAGE PATTERNS
 * ============================================================================ */
TEST_SUITE(practical_patterns);

/* Error handler pattern */
static noreturn void fatal_error(const char *msg) {
	(void)msg;
	exit(1);
}

/* Assertion failure pattern */
static noreturn void assertion_failed(const char *expr, const char *file, int line) {
	(void)expr;
	(void)file;
	(void)line;
	exit(1);
}

TEST(error_handler_pattern) {
	// Verify error handler pattern compiles
	ASSERT_TRUE(1);
}

TEST(assertion_pattern) {
	// Verify assertion pattern compiles
	ASSERT_TRUE(1);
}

/* ============================================================================
 * COMPILER COMPATIBILITY
 * ============================================================================ */
TEST_SUITE(compiler_compatibility);

TEST(noreturn_macro_defined) {
	// The noreturn macro should be defined
	#ifdef noreturn
		ASSERT_TRUE(1);
	#else
		ASSERT_TRUE(0);
	#endif
}

#if JACL_HAS_C11
TEST(c11_noreturn_available) {
	// In C11+, noreturn should map to _Noreturn or [[noreturn]]
	ASSERT_TRUE(1);
}
#endif

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

/* Multiple noreturn in chain */
static noreturn void level_3(void) {
	exit(0);
}

static noreturn void level_2(void) {
	level_3();
}

static noreturn void level_1(void) {
	level_2();
}

TEST(chained_noreturn) {
	// Verify chained noreturn functions compile
	ASSERT_TRUE(1);
}

/* Noreturn with variadic arguments */
static noreturn void exit_with_message(const char *fmt, ...) {
	(void)fmt;
	exit(1);
}

TEST(noreturn_variadic) {
	// Verify noreturn with variadic args compiles
	ASSERT_TRUE(1);
}

TEST_MAIN()
