/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <signal.h>

TEST_TYPE(unit);
TEST_UNIT(signal.h);

/* ============================================================================
 * SIGNAL CONSTANTS - ISO C
 * ============================================================================ */
TEST_SUITE(iso_signals);

TEST(standard_signals_defined) {
	ASSERT_EQ(6, SIGABRT);
	ASSERT_EQ(8, SIGFPE);
	ASSERT_EQ(4, SIGILL);
	ASSERT_EQ(2, SIGINT);
	ASSERT_EQ(11, SIGSEGV);
	ASSERT_EQ(15, SIGTERM);
}

TEST(signal_constants_unique) {
	ASSERT_NE(SIGABRT, SIGFPE);
	ASSERT_NE(SIGINT, SIGTERM);
	ASSERT_NE(SIGILL, SIGSEGV);
}

TEST(signal_handler_constants) {
	ASSERT_NOT_NULL(SIG_DFL);
	ASSERT_NOT_NULL(SIG_IGN);
	ASSERT_EQ(-1, (intptr_t)SIG_ERR);
}

/* ============================================================================
 * POSIX SIGNALS
 * ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(posix_signals);

TEST(posix_signals_defined) {
	ASSERT_EQ(1, SIGHUP);
	ASSERT_EQ(3, SIGQUIT);
	ASSERT_EQ(9, SIGKILL);
	ASSERT_EQ(10, SIGUSR1);
	ASSERT_EQ(12, SIGUSR2);
	ASSERT_EQ(13, SIGPIPE);
	ASSERT_EQ(14, SIGALRM);
}

TEST(process_control_signals) {
	ASSERT_EQ(17, SIGCHLD);
	ASSERT_EQ(18, SIGCONT);
	ASSERT_EQ(19, SIGSTOP);
	ASSERT_EQ(20, SIGTSTP);
}

TEST(terminal_signals) {
	ASSERT_EQ(21, SIGTTIN);
	ASSERT_EQ(22, SIGTTOU);
}

TEST(realtime_signals) {
	ASSERT_EQ(34, SIGRTMIN);
	ASSERT_EQ(64, SIGRTMAX);
	ASSERT_TRUE(SIGRTMAX > SIGRTMIN);
}
#endif

/* ============================================================================
 * SIG_ATOMIC_T TYPE
 * ============================================================================ */
TEST_SUITE(sig_atomic);

TEST(sig_atomic_t_defined) {
	sig_atomic_t val = 42;
	ASSERT_EQ(42, val);
}

TEST(sig_atomic_t_assignment) {
	sig_atomic_t val = 0;
	val = 1;
	ASSERT_EQ(1, val);
	val = -1;
	ASSERT_EQ(-1, val);
}

TEST(sig_atomic_t_volatile) {
	volatile sig_atomic_t flag = 0;
	flag = 1;
	ASSERT_EQ(1, flag);
}

/* ============================================================================
 * SIGNAL HANDLER TYPES
 * ============================================================================ */
TEST_SUITE(handler_types);

TEST(sighandler_t_typedef) {
	__sighandler_t handler = SIG_DFL;
	ASSERT_EQ(SIG_DFL, handler);
}

TEST(sig_t_typedef) {
	sig_t handler = SIG_IGN;
	ASSERT_EQ(SIG_IGN, handler);
}

/* ============================================================================
 * RAISE FUNCTION
 * ============================================================================ */
TEST_SUITE(raise_function);

static volatile sig_atomic_t signal_received = 0;

static void test_handler(int sig) {
	signal_received = sig;
}

TEST(raise_basic) {
	signal_received = 0;
	signal(SIGTERM, test_handler);
	
	int result = raise(SIGTERM);
	
	ASSERT_EQ(0, result);
	ASSERT_EQ(SIGTERM, signal_received);
}

TEST(raise_different_signals) {
	signal_received = 0;
	
	signal(SIGINT, test_handler);
	raise(SIGINT);
	ASSERT_EQ(SIGINT, signal_received);
	
	signal(SIGABRT, test_handler);
	raise(SIGABRT);
	ASSERT_EQ(SIGABRT, signal_received);
}

/* ============================================================================
 * SIGNAL FUNCTION
 * ============================================================================ */
TEST_SUITE(signal_function);

TEST(signal_set_handler) {
	sig_t old = signal(SIGTERM, test_handler);
	ASSERT_NE(SIG_ERR, old);
}

TEST(signal_default) {
	sig_t old = signal(SIGINT, SIG_DFL);
	ASSERT_NE(SIG_ERR, old);
}

TEST(signal_ignore) {
	sig_t old = signal(SIGTERM, SIG_IGN);
	ASSERT_NE(SIG_ERR, old);
}

TEST(signal_restore) {
	sig_t old = signal(SIGINT, test_handler);
	sig_t restored = signal(SIGINT, old);
	
	ASSERT_EQ(test_handler, restored);
}

/* ============================================================================
 * POSIX SIGSET OPERATIONS
 * ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(sigset_operations);

TEST(sigemptyset_basic) {
	sigset_t set;
	int result = sigemptyset(&set);
	
	ASSERT_EQ(0, result);
	ASSERT_EQ(0, set);
}

TEST(sigfillset_basic) {
	sigset_t set;
	int result = sigfillset(&set);
	
	ASSERT_EQ(0, result);
	ASSERT_NE(0, set);
}

TEST(sigaddset_basic) {
	sigset_t set;
	sigemptyset(&set);
	
	int result = sigaddset(&set, SIGINT);
	ASSERT_EQ(0, result);
	ASSERT_NE(0, set);
}

TEST(sigdelset_basic) {
	sigset_t set;
	sigfillset(&set);
	
	int result = sigdelset(&set, SIGINT);
	ASSERT_EQ(0, result);
}

TEST(sigismember_basic) {
	sigset_t set;
	sigemptyset(&set);
	
	ASSERT_FALSE(sigismember(&set, SIGINT));
	
	sigaddset(&set, SIGINT);
	ASSERT_TRUE(sigismember(&set, SIGINT));
}

TEST(sigset_multiple_signals) {
	sigset_t set;
	sigemptyset(&set);
	
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGTERM);
	sigaddset(&set, SIGHUP);
	
	ASSERT_TRUE(sigismember(&set, SIGINT));
	ASSERT_TRUE(sigismember(&set, SIGTERM));
	ASSERT_TRUE(sigismember(&set, SIGHUP));
	ASSERT_FALSE(sigismember(&set, SIGQUIT));
}

TEST(sigset_add_remove) {
	sigset_t set;
	sigemptyset(&set);
	
	sigaddset(&set, SIGINT);
	ASSERT_TRUE(sigismember(&set, SIGINT));
	
	sigdelset(&set, SIGINT);
	ASSERT_FALSE(sigismember(&set, SIGINT));
}
#endif

/* ============================================================================
 * SIGACTION STRUCTURE
 * ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(sigaction_struct);

TEST(sigaction_structure_defined) {
	struct sigaction act;
	
	act.sa_handler = SIG_DFL;
	act.sa_flags = 0;
	
	ASSERT_EQ(SIG_DFL, act.sa_handler);
}

TEST(sigaction_flags) {
	struct sigaction act;
	
	act.sa_flags = SA_RESTART;
	ASSERT_EQ(SA_RESTART, act.sa_flags);
	
	act.sa_flags = SA_NOCLDSTOP;
	ASSERT_EQ(SA_NOCLDSTOP, act.sa_flags);
}

TEST(sigaction_multiple_flags) {
	struct sigaction act;
	
	act.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	ASSERT_TRUE(act.sa_flags & SA_RESTART);
	ASSERT_TRUE(act.sa_flags & SA_NOCLDSTOP);
}
#endif

/* ============================================================================
 * SIGINFO STRUCTURE
 * ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(siginfo_struct);

TEST(siginfo_t_structure) {
	siginfo_t info;
	
	info.si_signo = SIGINT;
	info.si_code = SI_USER;
	info.si_value.sival_int = 42;
	
	ASSERT_EQ(SIGINT, info.si_signo);
	ASSERT_EQ(SI_USER, info.si_code);
	ASSERT_EQ(42, info.si_value.sival_int);
}

TEST(sigval_union) {
	union sigval val;
	
	val.sival_int = 123;
	ASSERT_EQ(123, val.sival_int);
	
	int dummy = 0;
	val.sival_ptr = &dummy;
	ASSERT_EQ(&dummy, val.sival_ptr);
}
#endif

/* ============================================================================
 * SIGNAL MASK OPERATIONS
 * ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(signal_mask);

TEST(sig_block_constant) {
	ASSERT_EQ(0, SIG_BLOCK);
}

TEST(sig_unblock_constant) {
	ASSERT_EQ(1, SIG_UNBLOCK);
}

TEST(sig_setmask_constant) {
	ASSERT_EQ(2, SIG_SETMASK);
}
#endif

/* ============================================================================
 * INLINE OPTIMIZATIONS
 * ============================================================================ */
#if JACL_HAS_C99 && JACL_HAS_POSIX
TEST_SUITE(inline_optimizations);

TEST(sigisemptyset_fast) {
	sigset_t set;
	sigemptyset(&set);
	
	ASSERT_TRUE(__sigisemptyset(&set));
	
	sigaddset(&set, SIGINT);
	ASSERT_FALSE(__sigisemptyset(&set));
}

TEST(sigemptyset_fast) {
	sigset_t set;
	__sigemptyset_fast(&set);
	
	ASSERT_EQ(0, set);
}

TEST(sigfillset_fast) {
	sigset_t set;
	__sigfillset_fast(&set);
	
	ASSERT_NE(0, set);
}

TEST(sigaddset_fast) {
	sigset_t set = 0;
	
	int result = __sigaddset_fast(&set, SIGINT);
	ASSERT_EQ(0, result);
	ASSERT_NE(0, set);
}

TEST(sigdelset_fast) {
	sigset_t set = ~0UL;
	
	int result = __sigdelset_fast(&set, SIGINT);
	ASSERT_EQ(0, result);
}
#endif

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(signal_zero) {
	// Signal 0 is special (null signal)
	ASSERT_TRUE(SIGINT > 0);
	ASSERT_TRUE(SIGTERM > 0);
}

TEST(signal_range) {
	// All standard signals should be positive
	ASSERT_TRUE(SIGABRT > 0);
	ASSERT_TRUE(SIGFPE > 0);
	ASSERT_TRUE(SIGILL > 0);
	ASSERT_TRUE(SIGINT > 0);
	ASSERT_TRUE(SIGSEGV > 0);
	ASSERT_TRUE(SIGTERM > 0);
}

#if JACL_HAS_POSIX
TEST(sigset_invalid_signal) {
	sigset_t set;
	sigemptyset(&set);
	
	// Invalid signal numbers should fail
	ASSERT_EQ(-1, sigaddset(&set, 0));
	ASSERT_EQ(-1, sigaddset(&set, 100));
}
#endif

/* ============================================================================
 * C11 ATOMIC OPERATIONS
 * ============================================================================ */
#if JACL_HAS_C11 && !defined(__STDC_NO_THREADS__)
TEST_SUITE(c11_atomic);

TEST(signal_atomic_store) {
	sig_atomic_t val = 0;
	signal_atomic_store(&val, 42);
	ASSERT_EQ(42, val);
}

TEST(signal_atomic_load) {
	sig_atomic_t val = 123;
	sig_atomic_t loaded = signal_atomic_load(&val);
	ASSERT_EQ(123, loaded);
}
#endif

/* ============================================================================
 * C23 VERSION MACRO
 * ============================================================================ */
#if JACL_HAS_C23
TEST_SUITE(c23_features);

TEST(version_macro_defined) {
	ASSERT_EQ(202311L, __STDC_VERSION_SIGNAL_H__);
}
#endif

TEST_MAIN()
