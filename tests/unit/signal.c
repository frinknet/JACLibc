/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <signal.h>

TEST_TYPE(unit);
TEST_UNIT(signal.h);

/* ============================================================================ */

static void suppress_stderr(void) {
	/* Redirect stderr to /dev/null */
	freopen("/dev/null", "w", stderr);
}

static void restore_stderr(void) {
	/* Restore stderr to the terminal.
	 * On Linux/ChromeOS, /proc/self/fd/2 is the safest bet if it exists,
	 * otherwise try /dev/tty. */
	FILE* tty = fopen("/dev/tty", "w");

	if (tty) {
		*stderr = *tty; /* Copy the struct content */
		fclose(tty);
	} else {
		/* Fallback: reopen /dev/null if tty fails (better than crashing) */
		freopen("/dev/null", "w", stderr);
	}
}

TEST_SUITE(constants);

TEST(constants_iso_signals_defined) {
	ASSERT_TRUE(SIGABRT > 0);
	ASSERT_TRUE(SIGFPE > 0);
	ASSERT_TRUE(SIGILL > 0);
	ASSERT_TRUE(SIGINT > 0);
	ASSERT_TRUE(SIGSEGV > 0);
	ASSERT_TRUE(SIGTERM > 0);
}

TEST(constants_iso_signals_unique) {
	ASSERT_NE(SIGABRT, SIGFPE);
	ASSERT_NE(SIGABRT, SIGILL);
	ASSERT_NE(SIGABRT, SIGINT);
	ASSERT_NE(SIGABRT, SIGSEGV);
	ASSERT_NE(SIGABRT, SIGTERM);
	ASSERT_NE(SIGFPE, SIGILL);
	ASSERT_NE(SIGFPE, SIGINT);
	ASSERT_NE(SIGFPE, SIGSEGV);
	ASSERT_NE(SIGFPE, SIGTERM);
	ASSERT_NE(SIGILL, SIGINT);
	ASSERT_NE(SIGILL, SIGSEGV);
	ASSERT_NE(SIGILL, SIGTERM);
	ASSERT_NE(SIGINT, SIGSEGV);
	ASSERT_NE(SIGINT, SIGTERM);
	ASSERT_NE(SIGSEGV, SIGTERM);
}

TEST(constants_handler_values) {
	ASSERT_EQ(0, (intptr_t)SIG_DFL);
	ASSERT_NE(0, (intptr_t)SIG_IGN);
	ASSERT_EQ(-1, (intptr_t)SIG_ERR);
	ASSERT_NE(SIG_DFL, SIG_IGN);
	ASSERT_NE(SIG_DFL, SIG_ERR);
	ASSERT_NE(SIG_IGN, SIG_ERR);
}

#if JACL_HAS_C23
TEST(constants_c23_version) {
	ASSERT_EQ(202311L, __STDC_VERSION_SIGNAL_H__);
}
#endif

#if JACL_HAS_POSIX
TEST(constants_posix_signals_defined) {
	ASSERT_TRUE(SIGHUP > 0);
	ASSERT_TRUE(SIGQUIT > 0);
	ASSERT_TRUE(SIGKILL > 0);
	ASSERT_TRUE(SIGUSR1 > 0);
	ASSERT_TRUE(SIGUSR2 > 0);
	ASSERT_TRUE(SIGPIPE > 0);
	ASSERT_TRUE(SIGALRM > 0);
	ASSERT_TRUE(SIGCHLD > 0);
	ASSERT_TRUE(SIGCONT > 0);
	ASSERT_TRUE(SIGSTOP > 0);
	ASSERT_TRUE(SIGTSTP > 0);
	ASSERT_TRUE(SIGTTIN > 0);
	ASSERT_TRUE(SIGTTOU > 0);
}

TEST(constants_realtime_range) {
	ASSERT_TRUE(SIGRTMIN > 0);
	ASSERT_TRUE(SIGRTMAX > 0);
	ASSERT_TRUE(SIGRTMAX > SIGRTMIN);
}

TEST(constants_mask_operations) {
	ASSERT_EQ(0, SIG_BLOCK);
	ASSERT_EQ(1, SIG_UNBLOCK);
	ASSERT_EQ(2, SIG_SETMASK);
}
#endif

/* ============================================================================ */

TEST_SUITE(sig_atomic_t);

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

#if JACL_HAS_C11 && !defined(__STDC_NO_THREADS__)
TEST(sig_atomic_t_store) {
	sig_atomic_t val = 0;
	signal_atomic_store(&val, 42);
	ASSERT_EQ(42, val);
}

TEST(sig_atomic_t_load) {
	sig_atomic_t val = 123;
	sig_atomic_t loaded = signal_atomic_load(&val);
	ASSERT_EQ(123, loaded);
}
#endif

/* ============================================================================ */

TEST_SUITE(sighandler_t);

TEST(sighandler_t_default_assignment) {
	__sighandler_t handler = SIG_DFL;
	ASSERT_EQ(SIG_DFL, handler);
}

TEST(sighandler_t_ignore_assignment) {
	__sighandler_t handler = SIG_IGN;
	ASSERT_NE(SIG_DFL, handler);
}

/* ============================================================================ */

TEST_SUITE(sig_t);

TEST(sig_t_default_assignment) {
	sig_t handler = SIG_DFL;
	ASSERT_EQ(SIG_DFL, handler);
}

TEST(sig_t_ignore_assignment) {
	sig_t handler = SIG_IGN;
	ASSERT_NE(SIG_DFL, handler);
}

/* ============================================================================ */

TEST_SUITE(sigset_t);

TEST(sigset_t_size_requirements) {
	/* Must be able to hold at least 64 bits (2 words) */
	ASSERT_TRUE(sizeof(sigset_t) >= 16);
}

TEST(sigset_t_manipulation) {
	sigset_t set;
	sigemptyset(&set);
	/* Check internal state */
	sigset_t copy = set;
	ASSERT_TRUE(copy.__bits[0] == 0 && copy.__bits[1] == 0);
}

/* ============================================================================ */

TEST_SUITE(sigval);

TEST(sigval_int_field) {
	union sigval val;
	val.sival_int = 123;
	ASSERT_EQ(123, val.sival_int);
}

TEST(sigval_ptr_field) {
	union sigval val;
	int dummy = 0;
	val.sival_ptr = &dummy;
	ASSERT_EQ(&dummy, val.sival_ptr);
}

/* ============================================================================ */

TEST_SUITE(siginfo_t);

TEST(siginfo_t_standard_fields) {
	siginfo_t info;
	info.si_signo = SIGINT;
	info.si_code = SI_USER;
	info.si_value.sival_int = 42;
	info.si_pid = 1234;
	info.si_uid = 5678;

	ASSERT_EQ(SIGINT, info.si_signo);
	ASSERT_EQ(SI_USER, info.si_code);
	ASSERT_EQ(42, info.si_value.sival_int);
	ASSERT_EQ(1234, info.si_pid);
	ASSERT_EQ(5678, info.si_uid);
}

/* ============================================================================ */

TEST_SUITE(struct_sigaction);

TEST(struct_sigaction_basic) {
	struct sigaction act;
	act.sa_handler = SIG_DFL;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);

	ASSERT_EQ(SIG_DFL, act.sa_handler);
	ASSERT_EQ(0, act.sa_flags);

	/* Verify mask is empty */
	sigset_t set = act.sa_mask;
	ASSERT_TRUE(set.__bits[0] == 0 && set.__bits[1] == 0);
}

/* ============================================================================ */

TEST_SUITE(raise);

static volatile sig_atomic_t raise_signal_received = 0;
static void raise_test_handler(int sig) { raise_signal_received = sig; }

TEST(raise_with_handler) {
	raise_signal_received = 0;
	sig_t old = signal(SIGUSR1, raise_test_handler);
	int result = raise(SIGUSR1);
	signal(SIGUSR1, old);
	ASSERT_EQ(0, result);
	ASSERT_EQ(SIGUSR1, raise_signal_received);
}

TEST(raise_with_ignore) {
	sig_t old = signal(SIGUSR2, SIG_IGN);
	int result = raise(SIGUSR2);
	signal(SIGUSR2, old);
	ASSERT_EQ(0, result);
}

TEST(raise_multiple_different) {
	raise_signal_received = 0;
	sig_t old1 = signal(SIGUSR1, raise_test_handler);
	raise(SIGUSR1);
	ASSERT_EQ(SIGUSR1, raise_signal_received);
	signal(SIGUSR1, old1);
	sig_t old2 = signal(SIGUSR2, raise_test_handler);
	raise(SIGUSR2);
	ASSERT_EQ(SIGUSR2, raise_signal_received);
	signal(SIGUSR2, old2);
}

/* ============================================================================ */

TEST_SUITE(signal);

static volatile sig_atomic_t signal_received = 0;
static void signal_test_handler(int sig) { signal_received = sig; }

TEST(signal_set_handler) {
	sig_t old = signal(SIGUSR1, signal_test_handler);
	ASSERT_NE(SIG_ERR, old);
	signal(SIGUSR1, old);
}

TEST(signal_set_default) {
	sig_t old = signal(SIGUSR1, SIG_DFL);
	ASSERT_NE(SIG_ERR, old);
	signal(SIGUSR1, old);
}

TEST(signal_set_ignore) {
	sig_t old = signal(SIGUSR1, SIG_IGN);
	ASSERT_NE(SIG_ERR, old);
	signal(SIGUSR1, old);
}

TEST(signal_restore_handler) {
	sig_t old = signal(SIGUSR1, signal_test_handler);
	sig_t restored = signal(SIGUSR1, old);
	ASSERT_EQ(signal_test_handler, restored);
	signal(SIGUSR1, old);
}

/* ============================================================================ */

TEST_SUITE(sigemptyset);

TEST(sigemptyset_basic) {
	sigset_t set;
	ASSERT_EQ(0, sigemptyset(&set));
	ASSERT_TRUE(set.__bits[0] == 0 && set.__bits[1] == 0);
}

TEST(sigemptyset_null_pointer) {
	errno = 0;
	ASSERT_EQ(-1, sigemptyset(NULL));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigemptyset_clears_all) {
	sigset_t set;
	sigfillset(&set);
	sigemptyset(&set);
	ASSERT_TRUE(set.__bits[0] == 0 && set.__bits[1] == 0);
	ASSERT_FALSE(sigismember(&set, SIGINT));
	ASSERT_FALSE(sigismember(&set, SIGTERM));
}

/* ============================================================================ */

TEST_SUITE(sigfillset);

TEST(sigfillset_basic) {
	sigset_t set;
	ASSERT_EQ(0, sigfillset(&set));
	ASSERT_FALSE(set.__bits[0] == 0 && set.__bits[1] == 0);
}

TEST(sigfillset_sets_all) {
	sigset_t set;
	sigemptyset(&set);
	sigfillset(&set);
	ASSERT_TRUE(sigismember(&set, SIGINT));
	ASSERT_TRUE(sigismember(&set, SIGTERM));
	ASSERT_TRUE(sigismember(&set, SIGHUP));
}

TEST(sigfillset_null_pointer) {
	errno = 0;
	ASSERT_EQ(-1, sigfillset(NULL));
	ASSERT_EQ(EINVAL, errno);
}

/* ============================================================================ */

TEST_SUITE(sigaddset);

TEST(sigaddset_basic) {
	sigset_t set;
	sigemptyset(&set);
	ASSERT_EQ(0, sigaddset(&set, SIGINT));
	ASSERT_TRUE(sigismember(&set, SIGINT));
}

#define TEST_SIGADDSET_FOR(sig) \
	TEST(sigaddset_##sig) { \
		sigset_t set; \
		sigemptyset(&set); \
		ASSERT_EQ(0, sigaddset(&set, sig)); \
		ASSERT_TRUE(sigismember(&set, sig)); \
	}

TEST_SIGADDSET_FOR(SIGINT)
TEST_SIGADDSET_FOR(SIGTERM)
TEST_SIGADDSET_FOR(SIGHUP)
TEST_SIGADDSET_FOR(SIGQUIT)
TEST_SIGADDSET_FOR(SIGKILL)
TEST_SIGADDSET_FOR(SIGUSR1)
TEST_SIGADDSET_FOR(SIGUSR2)
#undef TEST_SIGADDSET_FOR

TEST(sigaddset_multiple) {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGTERM);
	sigaddset(&set, SIGHUP);
	ASSERT_TRUE(sigismember(&set, SIGINT));
	ASSERT_TRUE(sigismember(&set, SIGTERM));
	ASSERT_TRUE(sigismember(&set, SIGHUP));
}

TEST(sigaddset_null_pointer) {
	errno = 0;
	ASSERT_EQ(-1, sigaddset(NULL, SIGINT));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigaddset_invalid_zero) {
	sigset_t set;
	sigemptyset(&set);
	errno = 0;
	ASSERT_EQ(-1, sigaddset(&set, 0));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigaddset_invalid_high) {
	sigset_t set;
	sigemptyset(&set);
	errno = 0;
	ASSERT_EQ(-1, sigaddset(&set, JACL_SIGSET_MAX + 1));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigaddset_boundary_first) {
	sigset_t set;
	sigemptyset(&set);
	ASSERT_EQ(0, sigaddset(&set, 1));
	ASSERT_TRUE(sigismember(&set, 1));
}

TEST(sigaddset_boundary_word) {
	sigset_t set;
	sigemptyset(&set);
	ASSERT_EQ(0, sigaddset(&set, 64));
	ASSERT_TRUE(sigismember(&set, 64));
	ASSERT_EQ(0, sigaddset(&set, 65));
	ASSERT_TRUE(sigismember(&set, 65));
}

TEST(sigaddset_bit_isolation) {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	ASSERT_TRUE(sigismember(&set, SIGINT));
	if (SIGINT > 1) ASSERT_FALSE(sigismember(&set, SIGINT - 1));
	if (SIGINT < JACL_SIGSET_MAX) ASSERT_FALSE(sigismember(&set, SIGINT + 1));
}

TEST(sigaddset_boundary_max) {
	sigset_t set;
	sigemptyset(&set);
	/* Test the exact boundary of JACL_SIGSET_MAX */
	ASSERT_EQ(0, sigaddset(&set, JACL_SIGSET_MAX));
	ASSERT_TRUE(sigismember(&set, JACL_SIGSET_MAX));
}

/* ============================================================================ */

TEST_SUITE(sigdelset);

TEST(sigdelset_basic) {
	sigset_t set;
	sigfillset(&set);
	ASSERT_EQ(0, sigdelset(&set, SIGINT));
	ASSERT_FALSE(sigismember(&set, SIGINT));
}

TEST(sigdelset_from_empty) {
	sigset_t set;
	sigemptyset(&set);
	ASSERT_EQ(0, sigdelset(&set, SIGINT));
	ASSERT_FALSE(sigismember(&set, SIGINT));
}

TEST(sigdelset_null_pointer) {
	errno = 0;
	ASSERT_EQ(-1, sigdelset(NULL, SIGINT));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigdelset_invalid_zero) {
	sigset_t set;
	sigfillset(&set);
	errno = 0;
	ASSERT_EQ(-1, sigdelset(&set, 0));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigdelset_invalid_high) {
	sigset_t set;
	sigfillset(&set);
	errno = 0;
	ASSERT_EQ(-1, sigdelset(&set, JACL_SIGSET_MAX + 1));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigdelset_boundary_max) {
	sigset_t set;
	sigfillset(&set);
	ASSERT_EQ(0, sigdelset(&set, JACL_SIGSET_MAX));
	ASSERT_FALSE(sigismember(&set, JACL_SIGSET_MAX));
}

TEST(sigdelset_signal_zero_rejected) {
	sigset_t set;
	sigfillset(&set);
	errno = 0;
	ASSERT_EQ(-1, sigdelset(&set, 0));
	ASSERT_EQ(EINVAL, errno);
}

/* ============================================================================ */

TEST_SUITE(sigismember);

TEST(sigismember_present) {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	ASSERT_TRUE(sigismember(&set, SIGINT));
}

TEST(sigismember_absent) {
	sigset_t set;
	sigemptyset(&set);
	ASSERT_FALSE(sigismember(&set, SIGINT));
}

TEST(sigismember_add_remove) {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	ASSERT_TRUE(sigismember(&set, SIGINT));
	sigdelset(&set, SIGINT);
	ASSERT_FALSE(sigismember(&set, SIGINT));
}

TEST(sigismember_null_pointer) {
	errno = 0;
	ASSERT_EQ(-1, sigismember(NULL, SIGINT));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigismember_invalid_zero) {
	sigset_t set;
	sigemptyset(&set);
	errno = 0;
	ASSERT_EQ(-1, sigismember(&set, 0));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigismember_invalid_high) {
	sigset_t set;
	sigemptyset(&set);
	errno = 0;
	ASSERT_EQ(-1, sigismember(&set, JACL_SIGSET_MAX + 1));
	ASSERT_EQ(EINVAL, errno);
}

/* ============================================================================ */

TEST_SUITE(sigaction);

TEST(sigaction_flags_restart) {
	struct sigaction act;
	act.sa_flags = SA_RESTART;
	ASSERT_EQ(SA_RESTART, act.sa_flags);
}

TEST(sigaction_flags_nocldstop) {
	struct sigaction act;
	act.sa_flags = SA_NOCLDSTOP;
	ASSERT_EQ(SA_NOCLDSTOP, act.sa_flags);
}

TEST(sigaction_flags_combined) {
	struct sigaction act;
	act.sa_flags = SA_RESTART | SA_NOCLDSTOP;
	ASSERT_TRUE(act.sa_flags & SA_RESTART);
	ASSERT_TRUE(act.sa_flags & SA_NOCLDSTOP);
}

TEST(sigaction_mask_operations) {
	struct sigaction act;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGINT);
	sigaddset(&act.sa_mask, SIGTERM);
	ASSERT_TRUE(sigismember(&act.sa_mask, SIGINT));
	ASSERT_TRUE(sigismember(&act.sa_mask, SIGTERM));
}

static volatile sig_atomic_t g_resethand_count = 0;
static void resethand_handler(int sig) {
		(void)sig;
			g_resethand_count++;
}

TEST(sigaction_resethand_oneshot) {
	struct sigaction act, old_act, query_act;

	g_resethand_count = 0;
	act.sa_handler = resethand_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESETHAND; /* Reset to SIG_DFL after first delivery */

	ASSERT_EQ(0, sigaction(SIGUSR1, &act, &old_act));

	raise(SIGUSR1);
	ASSERT_EQ(1, g_resethand_count);

	/* Verify the kernel reset the handler to SIG_DFL */
	ASSERT_EQ(0, sigaction(SIGUSR1, NULL, &query_act));
	ASSERT_EQ(SIG_DFL, query_act.sa_handler);

	sigaction(SIGUSR1, &old_act, NULL);
}

TEST(sigaction_null_act_retrieves_current) {
	struct sigaction old_act, query_act;

	/* Set a known handler first */
	struct sigaction set_act = { .sa_handler = SIG_IGN, .sa_flags = SA_RESTART };
	sigemptyset(&set_act.sa_mask);
	ASSERT_EQ(0, sigaction(SIGUSR2, &set_act, &old_act));

	/* Retrieve it using NULL act */
	ASSERT_EQ(0, sigaction(SIGUSR2, NULL, &query_act));
	ASSERT_EQ(SIG_IGN, query_act.sa_handler);

	/* Restore original */
	sigaction(SIGUSR2, &old_act, NULL);
}

/* ============================================================================ */

TEST_SUITE(sigset_integrity);

TEST(sigset_integrity_capacity) {
	ASSERT_TRUE(JACL_SIGSET_MAX >= 64);
}

TEST(sigset_integrity_independent_bits) {
	sigset_t set;
	sigemptyset(&set);
	for (int sig = 1; sig <= 64 && sig <= JACL_SIGSET_MAX; sig += 8) sigaddset(&set, sig);
	for (int sig = 1; sig <= 64 && sig <= JACL_SIGSET_MAX; sig++) {
		if (sig % 8 == 1) ASSERT_TRUE(sigismember(&set, sig));
		else ASSERT_FALSE(sigismember(&set, sig));
	}
}

TEST(sigset_integrity_word_boundary) {
	sigset_t set;
	sigemptyset(&set);
	if (JACL_SIGSET_MAX >= 65) {
		sigaddset(&set, 64);
		sigaddset(&set, 65);
		ASSERT_TRUE(sigismember(&set, 64));
		ASSERT_TRUE(sigismember(&set, 65));
		ASSERT_FALSE(sigismember(&set, 63));
		if (JACL_SIGSET_MAX >= 66) ASSERT_FALSE(sigismember(&set, 66));
	}
}

TEST(sigset_integrity_isempty_helper) {
	sigset_t set;
	sigemptyset(&set);
	ASSERT_TRUE(set.__bits[0] == 0 && set.__bits[1] == 0);
	sigaddset(&set, SIGINT);
	ASSERT_FALSE(set.__bits[0] == 0 && set.__bits[1] == 0);
}

/* ============================================================================ */

TEST_SUITE(sigisemptyset);

TEST(sigisemptyset_empty_returns_nonzero) {
	sigset_t set;
	sigemptyset(&set);
	ASSERT_TRUE(sigisemptyset(&set) != 0);
}

TEST(sigisemptyset_nonempty_returns_zero) {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	ASSERT_EQ(0, sigisemptyset(&set));
}

TEST(sigisemptyset_null_returns_error) {
	errno = 0;
	int r = sigisemptyset(NULL);
	ASSERT_EQ(-1, r);
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigisemptyset_after_fill_and_del_all) {
	sigset_t set;

	/* Only operate on kernel-supported signals (1-64) */
	sigemptyset(&set);
	for (int sig = 1; sig <= 64 && sig <= JACL_SIGSET_MAX; sig++) {
		sigaddset(&set, sig);
	}

	/* Now delete them */
	for (int sig = 1; sig <= 64 && sig <= JACL_SIGSET_MAX; sig++) {
		ASSERT_EQ(0, sigdelset(&set, sig));
	}

	ASSERT_TRUE(sigisemptyset(&set) != 0);
}

TEST(sigisemptyset_partial_set_returns_zero) {
	sigset_t set;
	sigfillset(&set);
	for (int sig = 2; sig <= JACL_SIGSET_MAX; sig++) sigdelset(&set, sig);
	ASSERT_EQ(0, sigisemptyset(&set));
}

TEST(sigisemptyset_does_not_modify_set) {
	sigset_t original;
	sigemptyset(&original);
	sigaddset(&original, SIGTERM);
	sigset_t copy = original;
	sigisemptyset(&copy);
	ASSERT_EQ(original.__bits[0], copy.__bits[0]);
	ASSERT_EQ(original.__bits[1], copy.__bits[1]);
}

/* ============================================================================ */

#if JACL_HAS_POSIX

TEST_SUITE(sigpending);

TEST(sigpending_basic) {
    sigset_t pending;
    ASSERT_EQ(0, sigpending(&pending));
}

TEST(sigpending_null_pointer_error) {
    errno = 0;
    ASSERT_EQ(-1, sigpending(NULL));
    ASSERT_EQ(EINVAL, errno);
}

TEST(sigpending_detects_blocked_signal) {
	sigset_t block_set, old_set, pending_set;
	sigemptyset(&block_set);
	sigaddset(&block_set, SIGUSR1);

	/* 1. Ignore SIGUSR1 so unblocking it later doesn't terminate the test runner */
	struct sigaction act, old_act;
	act.sa_handler = SIG_IGN;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGUSR1, &act, &old_act);

	/* 2. Block and send */
	ASSERT_EQ(0, sigprocmask(SIG_BLOCK, &block_set, &old_set));
	ASSERT_EQ(0, kill(getpid(), SIGUSR1));

	/* 3. Verify it is pending */
	ASSERT_EQ(0, sigpending(&pending_set));
	ASSERT_TRUE(sigismember(&pending_set, SIGUSR1));

	/* 4. Unblock it. Because it is SIG_IGN, the pending signal is safely discarded. */
	sigprocmask(SIG_SETMASK, &old_set, NULL);

	/* 5. Restore the original handler */
	sigaction(SIGUSR1, &old_act, NULL);
}

TEST(sigpending_no_pending_signals) {
	sigset_t pending;
	sigemptyset(&pending);

	/* Ensure no signals are pending right now */
	ASSERT_EQ(0, sigpending(&pending));
	ASSERT_TRUE(sigisemptyset(&pending));
}
#endif /* JACL_HAS_POSIX */

/* ============================================================================ */

#if JACL_HAS_POSIX

TEST_SUITE(sigtimedwait);

TEST(sigtimedwait_queue_drain) {
    sigset_t empty;
    sigemptyset(&empty);
    siginfo_t info;
    struct timespec ts = { .tv_sec = 0, .tv_nsec = 0 };

    /* Drain any pending signals with 0 timeout */
    while (sigtimedwait(&empty, &info, &ts) > 0) {}
}

TEST(sigtimedwait_timeout_returns_eagain) {
	sigset_t set, old_set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);

	ASSERT_EQ(0, sigprocmask(SIG_BLOCK, &set, &old_set));

	struct timespec ts = { .tv_sec = 0, .tv_nsec = 10000000 }; /* 10ms */
	siginfo_t info;

	errno = 0;
	int r = sigtimedwait(&set, &info, &ts);

	ASSERT_EQ(-1, r);
	ASSERT_EQ(EAGAIN, errno);

	sigprocmask(SIG_SETMASK, &old_set, NULL);
}

TEST(sigtimedwait_invalid_timeout) {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);
	sigprocmask(SIG_BLOCK, &set, NULL);

	struct timespec ts = { .tv_sec = 0, .tv_nsec = -1 }; /* Invalid */
	siginfo_t info;

	errno = 0;
	ASSERT_EQ(-1, sigtimedwait(&set, &info, &ts));
	ASSERT_EQ(EINVAL, errno);

	/* Unblock for cleanup */
	sigprocmask(SIG_UNBLOCK, &set, NULL);
}

#endif /* JACL_HAS_POSIX */

/* ============================================================================ */

#if JACL_HAS_POSIX

TEST_SUITE(sigsuspend);

TEST(sigsuspend_wakes_on_signal) {
	pid_t pid = fork();
	ASSERT_NE(pid, -1);

	if (pid == 0) {
		/* Child path */
		sigset_t empty_mask;
		sigemptyset(&empty_mask);

		int r = sigprocmask(SIG_SETMASK, &empty_mask, NULL);
		ASSERT_EQ(0, r);

		/* Block forever waiting for SIGUSR1 */
		sigsuspend((const sigset_t *)0);

		exit(0);  /* Only reached if signal arrives */
	}

	/* Parent path */
	usleep(100 * 1000);  /* Give child time to block */
	kill(pid, SIGUSR1);  /* Wake the child */

	int status;
	waitpid(pid, &status, 0);
	ASSERT_TRUE(WIFEXITED(status));
}

/* Timeout handler for sigsuspend tests */
static void alarm_handler(int sig) { exit(124); }

TEST(sigsuspend_timeout_guard) {
	struct sigaction old_action;
	struct sigaction new_action;

	/* Setup alarm handler */
	memset(&new_action, 0, sizeof(new_action));
	new_action.sa_handler = alarm_handler;
	sigaction(SIGALRM, &new_action, &old_action);

	alarm(5);  /* Kill test after 5 seconds */

	/* This should never return unless signal arrives */
	sigsuspend((const sigset_t *)0);

	alarm(0);  /* Cancel alarm if we somehow get here */
}

TEST(sigsuspend_null_mask_error) {
	errno = 0;
	ASSERT_EQ(-1, sigsuspend(NULL));
	ASSERT_EQ(EINVAL, errno);
}

#endif /* JACL_HAS_POSIX */

/* ============================================================================ */

#if JACL_HAS_POSIX

TEST_SUITE(sigaltstack);

TEST(sigaltstack_set_flags_invalid) {
	stack_t ss;
	ss.ss_sp = NULL;
	ss.ss_size = 0;
	ss.ss_flags = 1;
	errno = 0;
	int r = sigaltstack(&ss, NULL);
	ASSERT_EQ(-1, r);
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigaltstack_null_pointer_error) {
	errno = 0;
	ASSERT_EQ(-1, sigaltstack(NULL, NULL));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigaltstack_valid_set_and_get) {
	void *stack_mem = mmap(NULL, SIGSTKSZ, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	ASSERT_NE(MAP_FAILED, stack_mem);

	stack_t ss, query;
	ss.ss_sp = stack_mem;
	ss.ss_size = SIGSTKSZ;
	ss.ss_flags = 0;

	int r = sigaltstack(&ss, NULL);
	if (r != 0) {
		munmap(stack_mem, SIGSTKSZ);
		/* Only skip if it's a permission/environment restriction */
		if (errno == EPERM || errno == ENOMEM) {
			TEST_SKIP("sigaltstack restricted by environment");
		}
		/* Otherwise, let it fail so we know our syscall wrapper is broken */
	}

	ASSERT_EQ(0, sigaltstack(NULL, &query));
	ASSERT_EQ(stack_mem, query.ss_sp);

	stack_t disable_ss = { .ss_flags = SS_DISABLE };
	ASSERT_EQ(0, sigaltstack(&disable_ss, NULL));

	munmap(stack_mem, SIGSTKSZ);
}

#endif /* JACL_HAS_POSIX */

/* ============================================================================ */

#if JACL_HAS_POSIX
TEST_SUITE(kill);

TEST(kill_self_null_signal) {
	/* sig=0 is the null signal, used to check process validity */
	ASSERT_EQ(0, kill(getpid(), 0));
}

TEST(kill_invalid_pid) {
	errno = 0;
	ASSERT_EQ(-1, kill(-999999, SIGINT));
	ASSERT_EQ(ESRCH, errno);
}

TEST(kill_invalid_signal) {
	errno = 0;
	/* Signal 0 is valid (null signal), but -1 is not */
	ASSERT_EQ(-1, kill(getpid(), -1));
	ASSERT_EQ(EINVAL, errno);
}

TEST(kill_invalid_pid_range) {
	errno = 0;
	/* PID 0 is valid (self group), but extremely large PIDs usually fail */
	ASSERT_EQ(-1, kill(2147483647, SIGINT));
	ASSERT_EQ(ESRCH, errno);
}

TEST(kill_signal_out_of_range) {
	errno = 0;
	/* NSIG + 1 should be invalid */
	ASSERT_EQ(-1, kill(getpid(), NSIG + 1));
	ASSERT_EQ(EINVAL, errno);
}
#endif

/* ============================================================================ */

#if JACL_HAS_POSIX
TEST_SUITE(killpg);

TEST(killpg_self_group) {
	/* killpg with 0 targets the caller's process group */
	ASSERT_EQ(0, killpg(0, 0));
}

TEST(killpg_invalid_group) {
	errno = 0;
	/* Group 999999 likely doesn't exist */
	ASSERT_EQ(-1, killpg(999999, SIGINT));
	ASSERT_TRUE(errno == ESRCH || errno == EPERM);
}

TEST(killpg_null_signal) {
	/* Signal 0 is valid (checks existence/permissions) */
	ASSERT_EQ(0, killpg(0, 0));
}
#endif

/* ============================================================================ */

#if JACL_HAS_POSIX
TEST_SUITE(sigprocmask);

TEST(sigprocmask_block_and_unblock) {
	sigset_t block_set, old_set, unblock_set;
	sigemptyset(&block_set);
	sigaddset(&block_set, SIGUSR1);

	ASSERT_EQ(0, sigprocmask(SIG_BLOCK, &block_set, &old_set));

	sigemptyset(&unblock_set);
	sigaddset(&unblock_set, SIGUSR1);
	ASSERT_EQ(0, sigprocmask(SIG_UNBLOCK, &unblock_set, NULL));

	/* Restore original mask */
	sigprocmask(SIG_SETMASK, &old_set, NULL);
}

TEST(sigprocmask_setmask_replaces) {
	sigset_t set1, set2, old_set, query_set;
	sigemptyset(&set1);
	sigaddset(&set1, SIGINT);

	sigemptyset(&set2);
	sigaddset(&set2, SIGTERM);

	ASSERT_EQ(0, sigprocmask(SIG_SETMASK, &set1, &old_set));

	/* Replace entirely */
	ASSERT_EQ(0, sigprocmask(SIG_SETMASK, &set2, NULL));

	/* Verify ONLY SIGTERM is blocked now */
	ASSERT_EQ(0, sigprocmask(SIG_BLOCK, NULL, &query_set));
	ASSERT_TRUE(sigismember(&query_set, SIGTERM));
	ASSERT_FALSE(sigismember(&query_set, SIGINT));

	/* Restore */
	sigprocmask(SIG_SETMASK, &old_set, NULL);
}

TEST(sigprocmask_invalid_how) {
	errno = 0;
	sigset_t set;
	sigemptyset(&set);
	ASSERT_EQ(-1, sigprocmask(99, &set, NULL)); /* Invalid 'how' */
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigprocmask_null_set_noop) {
	sigset_t old_set;
	/* Blocking a null set should succeed and return the old mask */
	ASSERT_EQ(0, sigprocmask(SIG_BLOCK, NULL, &old_set));
}

TEST(sigprocmask_null_old_and_new) {
	/* Passing NULL for both set and oldset should be a valid no-op */
	ASSERT_EQ(0, sigprocmask(SIG_BLOCK, NULL, NULL));
}

TEST(sigprocmask_invalid_how_value) {
	sigset_t set;
	sigemptyset(&set);
	errno = 0;
	/* 3 is not a valid 'how' value */
	ASSERT_EQ(-1, sigprocmask(3, &set, NULL));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigprocmask_how_max_int) {
	sigset_t set;
	sigemptyset(&set);
	errno = 0;
	ASSERT_EQ(-1, sigprocmask(INT_MAX, &set, NULL));
	ASSERT_EQ(EINVAL, errno);
}
#endif

/* ============================================================================ */

#if JACL_HAS_POSIX
TEST_SUITE(sigqueue);

static volatile int g_queue_signo = 0;
static volatile int g_queue_code = 0;
static volatile int g_queue_value = 0;
static volatile int g_queue_received = 0;

static void queue_handler(int sig, siginfo_t *info, void *context) {
	(void)context;
	g_queue_signo = info->si_signo;
	g_queue_code = info->si_code;
	g_queue_value = info->si_value.sival_int;
	g_queue_received = 1;
}

TEST(sigqueue_delivers_value) {
	struct sigaction act, old_act;

	sigemptyset(&act.sa_mask);

	/* Clean POSIX: The sigaction wrapper will inject the restorer automatically */
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = queue_handler;

	ASSERT_EQ(0, sigaction(SIGUSR1, &act, &old_act));

	union sigval val;
	val.sival_int = 0xDEADBEEF;

	int sq_ret = sigqueue(getpid(), SIGUSR1, val);

	if (sq_ret != 0) {
		sigaction(SIGUSR1, &old_act, NULL);
		TEST_SKIP("sigqueue not supported or permitted");
	}

	int timeout = 1000000;
	while (!g_queue_received && timeout-- > 0) { }

	ASSERT_TRUE(g_queue_received);
	ASSERT_EQ(SIGUSR1, g_queue_signo);
	ASSERT_EQ(SI_QUEUE, g_queue_code);
	ASSERT_EQ((int)0xDEADBEEF, g_queue_value);

	sigaction(SIGUSR1, &old_act, NULL);

	g_queue_received = 0;
	g_queue_signo = 0;
	g_queue_code = 0;
	g_queue_value = 0;
}

TEST(sigqueue_invalid_pid) {
	union sigval val = { .sival_int = 0 };
	errno = 0;
	ASSERT_EQ(-1, sigqueue(-999999, SIGUSR1, val));
	ASSERT_EQ(ESRCH, errno);
}

TEST(sigqueue_invalid_signal) {
	union sigval val = { .sival_int = 0 };
	errno = 0;
	ASSERT_EQ(-1, sigqueue(getpid(), -1, val));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigqueue_nonexistent_pid) {
	union sigval val = { .sival_int = 42 };
	errno = 0;

	/* Send to a PID that definitely doesn't exist */
	ASSERT_EQ(-1, sigqueue(999999, SIGUSR1, val));
	ASSERT_EQ(ESRCH, errno);
}

#endif

/* ============================================================================ */

#if JACL_HAS_POSIX
TEST_SUITE(sigwait);

TEST(sigwait_captures_blocked_signal) {
	sigset_t set, old_set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);

	ASSERT_EQ(0, sigprocmask(SIG_BLOCK, &set, &old_set));
	ASSERT_EQ(0, kill(getpid(), SIGUSR1));

	int caught_sig = 0;
	ASSERT_EQ(0, sigwait(&set, &caught_sig));
	ASSERT_EQ(SIGUSR1, caught_sig);

	sigprocmask(SIG_SETMASK, &old_set, NULL);
}

TEST(sigwait_unblocked_signal_error) {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1); /* Don't block it! */

	int caught_sig = 0;
	errno = 0;

	/* Our sanitized wrapper should catch this UB and return EINVAL */
	ASSERT_EQ(EINVAL, sigwait(&set, &caught_sig));
}

TEST(sigwait_empty_set_error) {
	sigset_t set;
	sigemptyset(&set); /* No signals to wait for */

	int caught_sig = 0;
	errno = 0;
	ASSERT_EQ(EINVAL, sigwait(&set, &caught_sig));
}

TEST(sigwait_null_inputs) {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);

	errno = 0;
	ASSERT_EQ(EINVAL, sigwait(NULL, NULL));
	ASSERT_EQ(EINVAL, errno);
}

#endif

/* ============================================================================ */

#if JACL_HAS_POSIX
TEST_SUITE(sigwaitinfo);

TEST(sigwaitinfo_captures_blocked_signal) {
	sigset_t set, old_set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);

	ASSERT_EQ(0, sigprocmask(SIG_BLOCK, &set, &old_set));
	ASSERT_EQ(0, kill(getpid(), SIGUSR1));

	siginfo_t info;
	int r = sigwaitinfo(&set, &info);

	ASSERT_EQ(SIGUSR1, r);
	ASSERT_EQ(SIGUSR1, info.si_signo);

	sigprocmask(SIG_SETMASK, &old_set, NULL);
}

TEST(sigwaitinfo_null_pointer_error) {
	errno = 0;
	ASSERT_EQ(-1, sigwaitinfo(NULL, NULL));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigwaitinfo_invalid_set) {
	errno = 0;
	siginfo_t info;
	ASSERT_EQ(-1, sigwaitinfo(NULL, &info));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigwaitinfo_empty_set_returns_einval) {
	sigset_t set;
	sigemptyset(&set); /* No signals to wait for */

	siginfo_t info;
	errno = 0;
	/* Waiting on an empty set is logically invalid */
	ASSERT_EQ(-1, sigwaitinfo(&set, &info));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sigwaitinfo_with_timeout_eagain) {
	sigset_t set, old_set;
	sigemptyset(&set);
	sigaddset(&set, SIGUSR1);

	ASSERT_EQ(0, sigprocmask(SIG_BLOCK, &set, &old_set));

	struct timespec ts = { .tv_sec = 0, .tv_nsec = 1000000 }; /* 1ms */
	siginfo_t info;

	errno = 0;
	int r = sigtimedwait(&set, &info, &ts); /* Use sigtimedwait, not sigwaitinfo! */

	ASSERT_EQ(-1, r);
	ASSERT_EQ(EAGAIN, errno);

	sigprocmask(SIG_SETMASK, &old_set, NULL);
}
#endif

/* ============================================================================ */

#if JACL_HAS_POSIX
TEST_SUITE(sig2str);

TEST(sig2str_null_buffer) {
	errno = 0;
	ASSERT_EQ(-1, sig2str(SIGINT, NULL));
	ASSERT_EQ(EINVAL, errno); /* Or ENOSYS if you prefer, but NULL check usually comes first */
}

TEST(sig2str_converts_signal) {
	char buf[SIG2STR_MAX];
	ASSERT_EQ(0, sig2str(SIGINT, buf));
	ASSERT_STR_EQ("SIGINT", buf);
}
#endif

/* ============================================================================ */

#if JACL_HAS_POSIX
TEST_SUITE(str2sig);

TEST(str2sig_null_name) {
	int sig = 0;
	errno = 0;
	ASSERT_EQ(-1, str2sig(NULL, &sig));
	ASSERT_EQ(EINVAL, errno);
}

TEST(str2sig_null_outptr) {
	errno = 0;
	ASSERT_EQ(-1, str2sig("SIGINT", NULL));
	ASSERT_EQ(EINVAL, errno);
}

TEST(str2sig_converts_string) {
	int sig = 0;
	ASSERT_EQ(0, str2sig("SIGINT", &sig));
	ASSERT_EQ(SIGINT, sig);
}

TEST(str2sig_handles_no_prefix) {
	int sig = 0;
	ASSERT_EQ(0, str2sig("TERM", &sig));
	ASSERT_EQ(SIGTERM, sig);
}

TEST(str2sig_case_sensitive) {
	int sig = 0;
	errno = 0;
	/* POSIX requires uppercase */
	ASSERT_EQ(-1, str2sig("sigint", &sig));
	ASSERT_EQ(EINVAL, errno);
}

TEST(str2sig_unknown_string) {
	int sig = 0;
	errno = 0;
	ASSERT_EQ(-1, str2sig("SIGFAKE", &sig));
	ASSERT_EQ(EINVAL, errno);
}

#endif

/* ============================================================================ */

TEST_SUITE(sigevent);

TEST(sigevent_struct_fields) {
	struct sigevent ev;
	ev.sigev_notify = SIGEV_SIGNAL;
	ev.sigev_signo = SIGUSR1;
	ev.sigev_value.sival_int = 42;

	ASSERT_EQ(SIGEV_SIGNAL, ev.sigev_notify);
	ASSERT_EQ(SIGUSR1, ev.sigev_signo);
	ASSERT_EQ(42, ev.sigev_value.sival_int);
}

TEST(sigevent_thread_notification) {
	struct sigevent ev;
	ev.sigev_notify = SIGEV_THREAD;
	ev.sigev_notify_function = (void(*)(union sigval))0x12345678;
	ev.sigev_notify_attributes = NULL;

	ASSERT_EQ(SIGEV_THREAD, ev.sigev_notify);
	ASSERT_EQ((void*)0x12345678, (void*)ev.sigev_notify_function);
}

TEST(sigevent_none_notification) {
	struct sigevent ev;
	ev.sigev_notify = SIGEV_NONE;
	ev.sigev_signo = 0;

	ASSERT_EQ(SIGEV_NONE, ev.sigev_notify);
	ASSERT_EQ(0, ev.sigev_signo);
}

TEST(sigevent_zero_init) {
	struct sigevent ev = {0};
	ASSERT_EQ(0, ev.sigev_notify);
	ASSERT_EQ(0, ev.sigev_signo);
	ASSERT_EQ(0, ev.sigev_value.sival_int);
	ASSERT_EQ(NULL, ev.sigev_notify_function);
}

/* ============================================================================ */

TEST_SUITE(psignal);

TEST(psignal_basic_execution) {
	/* Verify it doesn't crash on valid input */
  suppress_stderr();
	psignal(SIGINT, "Test psignal");
  restore_stderr();
}

TEST(psignal_invalid_signal) {
	/* Verify it handles out-of-bounds gracefully */
	suppress_stderr();
	psignal(999, "Invalid signal test");
	restore_stderr();
}

TEST(psignal_empty_prefix) {
	/* Should print "Signal 2" without a leading colon */
	suppress_stderr();
	psignal(SIGINT, "");
	restore_stderr();
}

/* ============================================================================ */

TEST_SUITE(psiginfo);

TEST(psiginfo_basic_execution) {
	siginfo_t info;
	memset(&info, 0, sizeof(info));
	info.si_signo = SIGINT;
	info.si_code = SI_USER;

	/* Verify it doesn't crash on valid input */
	suppress_stderr();
	psiginfo(&info, "Test psiginfo");
	restore_stderr();
}

TEST(psiginfo_null_pointer) {
	/* Verify it handles null gracefully without segfaulting */
	suppress_stderr();
	psiginfo(NULL, "Null psiginfo test");
	restore_stderr();
}

TEST(psiginfo_empty_prefix) {
	siginfo_t info = {0};
	info.si_signo = SIGINT;
	
	/* Should print details without a leading colon */
	suppress_stderr();
	psiginfo(&info, "");
	restore_stderr();
}

/* ============================================================================ */

TEST_MAIN()
