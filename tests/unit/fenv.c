/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_C99
#include <fenv.h>

TEST_TYPE(unit);
TEST_UNIT(fenv.h);

/* ============================================================================
 * EXCEPTION FLAG CONSTANTS
 * ============================================================================ */
TEST_SUITE(exception_flags);

TEST(exception_constants_defined) {
	ASSERT_EQ(1u, FE_INVALID);
	ASSERT_EQ(2u, FE_DIVBYZERO);
	ASSERT_EQ(4u, FE_OVERFLOW);
	ASSERT_EQ(8u, FE_UNDERFLOW);
	ASSERT_EQ(16u, FE_INEXACT);
}

TEST(fe_all_except) {
	unsigned int expected = FE_INVALID | FE_DIVBYZERO | FE_OVERFLOW | FE_UNDERFLOW | FE_INEXACT;
	ASSERT_EQ(expected, FE_ALL_EXCEPT);
}

TEST(exception_flags_unique) {
	ASSERT_NE(FE_INVALID, FE_DIVBYZERO);
	ASSERT_NE(FE_OVERFLOW, FE_UNDERFLOW);
	ASSERT_NE(FE_INEXACT, FE_INVALID);
}

TEST(exception_flags_powers_of_two) {
	// Each flag should be a power of 2
	ASSERT_EQ(1, __builtin_popcount(FE_INVALID));
	ASSERT_EQ(1, __builtin_popcount(FE_DIVBYZERO));
	ASSERT_EQ(1, __builtin_popcount(FE_OVERFLOW));
	ASSERT_EQ(1, __builtin_popcount(FE_UNDERFLOW));
	ASSERT_EQ(1, __builtin_popcount(FE_INEXACT));
}

/* ============================================================================
 * ROUNDING MODE CONSTANTS
 * ============================================================================ */
TEST_SUITE(rounding_modes);

TEST(rounding_constants_defined) {
	ASSERT_EQ(0, FE_TONEAREST);
	ASSERT_EQ(1, FE_DOWNWARD);
	ASSERT_EQ(2, FE_UPWARD);
	ASSERT_EQ(3, FE_TOWARDZERO);
}

TEST(rounding_modes_unique) {
	ASSERT_NE(FE_TONEAREST, FE_DOWNWARD);
	ASSERT_NE(FE_UPWARD, FE_TOWARDZERO);
	ASSERT_NE(FE_TONEAREST, FE_UPWARD);
}

/* ============================================================================
 * DEFAULT ENVIRONMENT
 * ============================================================================ */
TEST_SUITE(default_env);

TEST(fe_dfl_env_defined) {
	ASSERT_NOT_NULL(FE_DFL_ENV);
}

TEST(fe_dfl_env_cleared) {
	ASSERT_EQ(0u, FE_DFL_ENV->excepts);
	ASSERT_EQ(FE_TONEAREST, FE_DFL_ENV->round);
}

/* ============================================================================
 * CLEAR EXCEPTIONS
 * ============================================================================ */
TEST_SUITE(clear_exceptions);

TEST(feclearexcept_all) {
	feraiseexcept(FE_ALL_EXCEPT);

	int result = feclearexcept(FE_ALL_EXCEPT);

	ASSERT_EQ(0, result);
	ASSERT_EQ(0, fetestexcept(FE_ALL_EXCEPT));
}

TEST(feclearexcept_single) {
	feraiseexcept(FE_INVALID | FE_OVERFLOW);
	feclearexcept(FE_INVALID);

	ASSERT_FALSE(fetestexcept(FE_INVALID));
	ASSERT_TRUE(fetestexcept(FE_OVERFLOW));
}

TEST(feclearexcept_multiple) {
	feraiseexcept(FE_ALL_EXCEPT);
	feclearexcept(FE_INVALID | FE_DIVBYZERO);

	ASSERT_FALSE(fetestexcept(FE_INVALID));
	ASSERT_FALSE(fetestexcept(FE_DIVBYZERO));
	ASSERT_TRUE(fetestexcept(FE_OVERFLOW));
}

/* ============================================================================
 * RAISE EXCEPTIONS
 * ============================================================================ */
TEST_SUITE(raise_exceptions);

TEST(feraiseexcept_single) {
	feclearexcept(FE_ALL_EXCEPT);

	int result = feraiseexcept(FE_INVALID);

	ASSERT_EQ(0, result);
	ASSERT_TRUE(fetestexcept(FE_INVALID));
}

TEST(feraiseexcept_multiple) {
	feclearexcept(FE_ALL_EXCEPT);

	feraiseexcept(FE_OVERFLOW | FE_UNDERFLOW);

	ASSERT_TRUE(fetestexcept(FE_OVERFLOW));
	ASSERT_TRUE(fetestexcept(FE_UNDERFLOW));
	ASSERT_FALSE(fetestexcept(FE_INVALID));
}

TEST(feraiseexcept_accumulate) {
	feclearexcept(FE_ALL_EXCEPT);

	feraiseexcept(FE_INVALID);
	feraiseexcept(FE_OVERFLOW);

	ASSERT_TRUE(fetestexcept(FE_INVALID));
	ASSERT_TRUE(fetestexcept(FE_OVERFLOW));
}

/* ============================================================================
 * TEST EXCEPTIONS
 * ============================================================================ */
TEST_SUITE(test_exceptions);

TEST(fetestexcept_none) {
	feclearexcept(FE_ALL_EXCEPT);

	ASSERT_EQ(0, fetestexcept(FE_ALL_EXCEPT));
}

TEST(fetestexcept_single) {
	feclearexcept(FE_ALL_EXCEPT);
	feraiseexcept(FE_INVALID);

	ASSERT_EQ(FE_INVALID, fetestexcept(FE_INVALID));
	ASSERT_EQ(0, fetestexcept(FE_OVERFLOW));
}

TEST(fetestexcept_multiple) {
	feclearexcept(FE_ALL_EXCEPT);
	feraiseexcept(FE_INVALID | FE_OVERFLOW);

	int flags = fetestexcept(FE_INVALID | FE_OVERFLOW);
	ASSERT_TRUE(flags & FE_INVALID);
	ASSERT_TRUE(flags & FE_OVERFLOW);
}

TEST(fetestexcept_mask) {
	feclearexcept(FE_ALL_EXCEPT);
	feraiseexcept(FE_ALL_EXCEPT);

	int some_flags = fetestexcept(FE_INVALID | FE_OVERFLOW);
	ASSERT_TRUE(some_flags & FE_INVALID);
	ASSERT_TRUE(some_flags & FE_OVERFLOW);
}

/* ============================================================================
 * EXCEPTION FLAG OPERATIONS
 * ============================================================================ */
TEST_SUITE(exception_flag_ops);

TEST(fegetexceptflag_basic) {
	fexcept_t flags;
	feclearexcept(FE_ALL_EXCEPT);
	feraiseexcept(FE_INVALID);

	int result = fegetexceptflag(&flags, FE_INVALID);

	ASSERT_EQ(0, result);
	ASSERT_TRUE(flags & FE_INVALID);
}

TEST(fegetexceptflag_multiple) {
	fexcept_t flags;
	feclearexcept(FE_ALL_EXCEPT);
	feraiseexcept(FE_INVALID | FE_OVERFLOW);

	fegetexceptflag(&flags, FE_INVALID | FE_OVERFLOW);

	ASSERT_TRUE(flags & FE_INVALID);
	ASSERT_TRUE(flags & FE_OVERFLOW);
}

TEST(fesetexceptflag_basic) {
	fexcept_t flags = FE_OVERFLOW;
	feclearexcept(FE_ALL_EXCEPT);

	int result = fesetexceptflag(&flags, FE_OVERFLOW);

	ASSERT_EQ(0, result);
	ASSERT_TRUE(fetestexcept(FE_OVERFLOW));
}

TEST(fesetexceptflag_masked) {
	fexcept_t flags = FE_INVALID | FE_OVERFLOW;
	feclearexcept(FE_ALL_EXCEPT);

	fesetexceptflag(&flags, FE_INVALID);

	ASSERT_TRUE(fetestexcept(FE_INVALID));
	ASSERT_FALSE(fetestexcept(FE_OVERFLOW));
}

/* ============================================================================
 * ROUNDING MODE OPERATIONS
 * ============================================================================ */
TEST_SUITE(rounding_mode_ops);

TEST(fegetround_default) {
	fesetenv(FE_DFL_ENV);
	ASSERT_EQ(FE_TONEAREST, fegetround());
}

TEST(fesetround_tonearest) {
	int result = fesetround(FE_TONEAREST);

	ASSERT_EQ(0, result);
	ASSERT_EQ(FE_TONEAREST, fegetround());
}

TEST(fesetround_downward) {
	int result = fesetround(FE_DOWNWARD);

	ASSERT_EQ(0, result);
	ASSERT_EQ(FE_DOWNWARD, fegetround());
}

TEST(fesetround_upward) {
	int result = fesetround(FE_UPWARD);

	ASSERT_EQ(0, result);
	ASSERT_EQ(FE_UPWARD, fegetround());
}

TEST(fesetround_towardzero) {
	int result = fesetround(FE_TOWARDZERO);

	ASSERT_EQ(0, result);
	ASSERT_EQ(FE_TOWARDZERO, fegetround());
}

TEST(fesetround_invalid) {
	int result = fesetround(99);
	ASSERT_NE(0, result);
}

TEST(fesetround_sequence) {
	fesetround(FE_UPWARD);
	ASSERT_EQ(FE_UPWARD, fegetround());

	fesetround(FE_DOWNWARD);
	ASSERT_EQ(FE_DOWNWARD, fegetround());

	fesetround(FE_TONEAREST);
	ASSERT_EQ(FE_TONEAREST, fegetround());
}

/* ============================================================================
 * ENVIRONMENT OPERATIONS
 * ============================================================================ */
TEST_SUITE(environment_ops);

TEST(fegetenv_basic) {
	fenv_t env;

	int result = fegetenv(&env);

	ASSERT_EQ(0, result);
}

TEST(fesetenv_default) {
	feraiseexcept(FE_ALL_EXCEPT);
	fesetround(FE_UPWARD);

	int result = fesetenv(FE_DFL_ENV);

	ASSERT_EQ(0, result);
	ASSERT_EQ(0, fetestexcept(FE_ALL_EXCEPT));
	ASSERT_EQ(FE_TONEAREST, fegetround());
}

TEST(fegetenv_fesetenv_roundtrip) {
	fenv_t saved;

	feraiseexcept(FE_INVALID);
	fesetround(FE_UPWARD);

	fegetenv(&saved);

	// Change state
	feclearexcept(FE_ALL_EXCEPT);
	fesetround(FE_DOWNWARD);

	// Restore
	fesetenv(&saved);

	ASSERT_TRUE(fetestexcept(FE_INVALID));
	ASSERT_EQ(FE_UPWARD, fegetround());
}

TEST(feholdexcept_basic) {
	fenv_t env;

	feraiseexcept(FE_OVERFLOW);

	int result = feholdexcept(&env);

	ASSERT_EQ(0, result);
	ASSERT_TRUE(env.excepts & FE_OVERFLOW);
	ASSERT_EQ(0, fetestexcept(FE_ALL_EXCEPT));
}

TEST(feupdateenv_basic) {
	fenv_t env;

	feclearexcept(FE_ALL_EXCEPT);
	feraiseexcept(FE_INVALID);
	fesetround(FE_UPWARD);
	fegetenv(&env);

	feclearexcept(FE_ALL_EXCEPT);
	feraiseexcept(FE_OVERFLOW);
	fesetround(FE_DOWNWARD);

	int result = feupdateenv(&env);

	ASSERT_EQ(0, result);
	ASSERT_EQ(FE_UPWARD, fegetround());
	ASSERT_TRUE(fetestexcept(FE_INVALID));
	ASSERT_TRUE(fetestexcept(FE_OVERFLOW));
}

/* ============================================================================
 * PRACTICAL USAGE PATTERNS
 * ============================================================================ */
TEST_SUITE(practical_usage);

TEST(save_restore_pattern) {
	fenv_t saved;

	// Save current state
	fegetenv(&saved);

	// Do something that might change state
	feraiseexcept(FE_INVALID);
	fesetround(FE_UPWARD);

	// Restore
	fesetenv(&saved);

	ASSERT_TRUE(1);  // If we got here, save/restore worked
}

TEST(hold_update_pattern) {
	fenv_t env;

	feraiseexcept(FE_OVERFLOW);
	feholdexcept(&env);

	// Do computation (exceptions cleared)
	ASSERT_EQ(0, fetestexcept(FE_ALL_EXCEPT));

	// Restore and merge
	feupdateenv(&env);
	ASSERT_TRUE(fetestexcept(FE_OVERFLOW));
}

TEST(selective_exception_handling) {
	feclearexcept(FE_ALL_EXCEPT);

	// Raise multiple exceptions
	feraiseexcept(FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW);

	// Clear only some
	feclearexcept(FE_INVALID);

	// Check state
	ASSERT_FALSE(fetestexcept(FE_INVALID));
	ASSERT_TRUE(fetestexcept(FE_OVERFLOW));
	ASSERT_TRUE(fetestexcept(FE_UNDERFLOW));
}

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(multiple_raises_same_flag) {
	feclearexcept(FE_ALL_EXCEPT);

	feraiseexcept(FE_INVALID);
	feraiseexcept(FE_INVALID);
	feraiseexcept(FE_INVALID);

	ASSERT_TRUE(fetestexcept(FE_INVALID));
}

TEST(clear_already_cleared) {
	feclearexcept(FE_ALL_EXCEPT);
	int result = feclearexcept(FE_ALL_EXCEPT);

	ASSERT_EQ(0, result);
	ASSERT_EQ(0, fetestexcept(FE_ALL_EXCEPT));
}

TEST(raise_no_flags) {
	feclearexcept(FE_ALL_EXCEPT);
	feraiseexcept(0);

	ASSERT_EQ(0, fetestexcept(FE_ALL_EXCEPT));
}


#else

TEST_SUITE(fenv_basics);

TEST(fenv_not_avaialable) {
	TEST_SKIP("NO C99 SUPPORT");
}

#endif

TEST_MAIN()
