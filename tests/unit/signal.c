/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <signal.h>

TEST_TYPE(unit);
TEST_UNIT(signal.h);

/* ============================================================================ */

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

TEST_SUITE(sigaction_struct);

TEST(sigaction_struct_basic) {
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

TEST(signal_queue_clean) {
	siginfo_t info; \
	struct timespec ts = { .tv_sec = 0, .tv_nsec = 0 }; \
	while (sigtimedwait(NULL, &info, &ts) >= 0) {} \
}

TEST(sigpending_null_pointer_error) {
    errno = 0;
    ASSERT_EQ(-1, sigpending(NULL));
    ASSERT_EQ(EINVAL, errno);
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

#endif /* JACL_HAS_POSIX */

/* ============================================================================ */

TEST_MAIN()
