/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

TEST_TYPE(unit);
TEST_UNIT(sys/wait.h);

/* ============================================================================
 * CONSTANTS
 * ============================================================================ */
TEST_SUITE(constants);

TEST(wnohang_constant) {
	ASSERT_EQ(1, WNOHANG);
}

TEST(wuntraced_constant) {
	ASSERT_EQ(2, WUNTRACED);
}

TEST(wait_constants_unique) {
	ASSERT_NE(WNOHANG, WUNTRACED);
}

/* ============================================================================
 * WEXITSTATUS MACRO
 * ============================================================================ */
TEST_SUITE(wexitstatus_macro);

TEST(wexitstatus_zero) {
	int status = 0 << 8;  // Exit code 0
	ASSERT_EQ(0, WEXITSTATUS(status));
}

TEST(wexitstatus_one) {
	int status = 1 << 8;  // Exit code 1
	ASSERT_EQ(1, WEXITSTATUS(status));
}

TEST(wexitstatus_max) {
	int status = 255 << 8;  // Exit code 255
	ASSERT_EQ(255, WEXITSTATUS(status));
}

TEST(wexitstatus_typical) {
	int status = 42 << 8;  // Exit code 42
	ASSERT_EQ(42, WEXITSTATUS(status));
}

/* ============================================================================
 * WTERMSIG MACRO
 * ============================================================================ */
TEST_SUITE(wtermsig_macro);

TEST(wtermsig_sigterm) {
	int status = SIGTERM;  // Terminated by SIGTERM
	ASSERT_EQ(SIGTERM, WTERMSIG(status));
}

TEST(wtermsig_sigkill) {
	int status = SIGKILL;  // Terminated by SIGKILL
	ASSERT_EQ(SIGKILL, WTERMSIG(status));
}

TEST(wtermsig_zero) {
	int status = 0;  // No signal
	ASSERT_EQ(0, WTERMSIG(status));
}

/* ============================================================================
 * WIFEXITED MACRO
 * ============================================================================ */
TEST_SUITE(wifexited_macro);

TEST(wifexited_normal_exit) {
	int status = 0 << 8;  // Normal exit
	ASSERT_TRUE(WIFEXITED(status));
}

TEST(wifexited_exit_code_one) {
	int status = 1 << 8;  // Exit with code 1
	ASSERT_TRUE(WIFEXITED(status));
}

TEST(wifexited_signaled) {
	int status = SIGTERM;  // Terminated by signal
	ASSERT_FALSE(WIFEXITED(status));
}

TEST(wifexited_various_codes) {
	for (int code = 0; code < 10; code++) {
		int status = code << 8;
		ASSERT_TRUE(WIFEXITED(status));
	}
}

/* ============================================================================
 * WIFSIGNALED MACRO
 * ============================================================================ */
TEST_SUITE(wifsignaled_macro);

TEST(wifsignaled_sigterm) {
	int status = SIGTERM;
	ASSERT_TRUE(WIFSIGNALED(status));
}

TEST(wifsignaled_sigkill) {
	int status = SIGKILL;
	ASSERT_TRUE(WIFSIGNALED(status));
}

TEST(wifsignaled_normal_exit) {
	int status = 0 << 8;  // Normal exit
	ASSERT_FALSE(WIFSIGNALED(status));
}

/* ============================================================================
 * WIFSTOPPED MACRO
 * ============================================================================ */
TEST_SUITE(wifstopped_macro);

TEST(wifstopped_basic) {
	int status = 0x7f;  // Stopped
	ASSERT_TRUE(WIFSTOPPED(status));
}

TEST(wifstopped_not_stopped) {
	int status = 0 << 8;  // Normal exit
	ASSERT_FALSE(WIFSTOPPED(status));
}

/* ============================================================================
 * WCOREDUMP MACRO
 * ============================================================================ */
TEST_SUITE(wcoredump_macro);

TEST(wcoredump_yes) {
	int status = 0x80;  // Core dumped
	ASSERT_TRUE(WCOREDUMP(status));
}

TEST(wcoredump_no) {
	int status = 0 << 8;  // No core dump
	ASSERT_FALSE(WCOREDUMP(status));
}

/* ============================================================================
 * MACRO COMBINATIONS
 * ============================================================================ */
TEST_SUITE(macro_combinations);

TEST(exit_code_extraction) {
	int exit_code = 42;
	int status = exit_code << 8;
	
	ASSERT_TRUE(WIFEXITED(status));
	ASSERT_EQ(exit_code, WEXITSTATUS(status));
}

TEST(signal_extraction) {
	int sig = SIGTERM;
	int status = sig;
	
	ASSERT_TRUE(WIFSIGNALED(status));
	ASSERT_EQ(sig, WTERMSIG(status));
}

TEST(exit_vs_signal) {
	int exit_status = 0 << 8;
	int signal_status = SIGTERM;
	
	ASSERT_TRUE(WIFEXITED(exit_status));
	ASSERT_FALSE(WIFEXITED(signal_status));
	ASSERT_FALSE(WIFSIGNALED(exit_status));
	ASSERT_TRUE(WIFSIGNALED(signal_status));
}

/* ============================================================================
 * WAIT / WAITPID (if supported)
 * ============================================================================ */
#if !defined(JACL_OS_WINDOWS) && !defined(JACL_ARCH_WASM)
TEST_SUITE(wait_functions);

TEST(waitpid_no_child) {
	int status;
	pid_t result = waitpid(-1, &status, WNOHANG);
	
	// Should return -1 (no children) or 0 (no child ready)
	ASSERT_TRUE(result <= 0);
}

TEST(waitpid_invalid_pid) {
	int status;
	pid_t result = waitpid(-999999, &status, WNOHANG);
	
	ASSERT_EQ(-1, result);
}

TEST(wait_no_child) {
	int status;
	pid_t result = wait(&status);
	
	// Should fail if no children
	ASSERT_TRUE(result == -1 || result > 0);
}
#endif

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(status_all_bits_set) {
	int status = 0xFFFF;
	
	// Should handle all bits set
	ASSERT_TRUE(WEXITSTATUS(status) >= 0);
	ASSERT_TRUE(WTERMSIG(status) >= 0);
}

TEST(status_zero) {
	int status = 0;
	
	ASSERT_EQ(0, WEXITSTATUS(status));
	ASSERT_EQ(0, WTERMSIG(status));
	ASSERT_TRUE(WIFEXITED(status));
}

TEST(exit_code_255) {
	int status = 255 << 8;
	
	ASSERT_EQ(255, WEXITSTATUS(status));
	ASSERT_TRUE(WIFEXITED(status));
}

/* ============================================================================
 * REALISTIC SCENARIOS
 * ============================================================================ */
TEST_SUITE(realistic_scenarios);

TEST(successful_exit) {
	int status = 0 << 8;  // exit(0)
	
	ASSERT_TRUE(WIFEXITED(status));
	ASSERT_FALSE(WIFSIGNALED(status));
	ASSERT_EQ(0, WEXITSTATUS(status));
}

TEST(error_exit) {
	int status = 1 << 8;  // exit(1)
	
	ASSERT_TRUE(WIFEXITED(status));
	ASSERT_FALSE(WIFSIGNALED(status));
	ASSERT_EQ(1, WEXITSTATUS(status));
}

TEST(killed_by_sigterm) {
	int status = SIGTERM;
	
	ASSERT_FALSE(WIFEXITED(status));
	ASSERT_TRUE(WIFSIGNALED(status));
	ASSERT_EQ(SIGTERM, WTERMSIG(status));
}

TEST(killed_by_sigkill) {
	int status = SIGKILL;
	
	ASSERT_FALSE(WIFEXITED(status));
	ASSERT_TRUE(WIFSIGNALED(status));
	ASSERT_EQ(SIGKILL, WTERMSIG(status));
}

/* ============================================================================
 * WSTOPSIG MACRO
 * ============================================================================ */
TEST_SUITE(wstopsig_macro);

TEST(wstopsig_basic) {
	int status = SIGSTOP << 8;
	ASSERT_EQ(SIGSTOP, WSTOPSIG(status));
}

TEST(wstopsig_equals_wexitstatus) {
	int status = 5 << 8;
	ASSERT_EQ(WEXITSTATUS(status), WSTOPSIG(status));
}

TEST_MAIN()
