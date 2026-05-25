/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

TEST_TYPE(unit);
TEST_UNIT(setjmp.h);

static sigjmp_buf signal_env;
static volatile sig_atomic_t signal_caught = 0;

static void signal_jump_handler(int sig) {
	(void)sig;
	signal_caught = 1;
	siglongjmp(signal_env, 99);
}

/* ============================================================================ */

TEST_SUITE(jmp_buf);

TEST(jmp_buf_size) {
    size_t expected = sizeof(unsigned long[8]) +
                      sizeof(unsigned long) +
                      sizeof(sigset_t);
    ASSERT_EQ(sizeof(jmp_buf), expected);
}

TEST(jmp_buf_alignment) {
    union { jmp_buf x; char y; } u;
    uintptr_t addr = (uintptr_t)&u.y;
    ASSERT_EQ(addr % 16, 0);
}

TEST(jmp_buf_ptr_reuse) {
    jmp_buf *ptr = NULL;
    jmp_buf env = {0};

    ptr = &env;
    int r = setjmp(*ptr);

    ASSERT_EQ(r, 0);
}

/* ============================================================================ */

TEST_SUITE(sigjmp_buf);

TEST(sigjmp_buf_size) {
    ASSERT_EQ(sizeof(sigjmp_buf), sizeof(jmp_buf));
}

TEST(sigjmp_buf_fields) {
    sigjmp_buf env = {0};
    (void)env->buf;
    (void)env->saved;
    (void)env->sigset;
    ASSERT_TRUE(1);
}

TEST(sigjmp_buf_mask_offset) {
    sigjmp_buf env = {0};
    char *mask_addr = (char *)&env->sigset;
    char *jb_addr = (char *)&env->buf;

    ASSERT_GT((mask_addr - jb_addr), 0);
}

/* ============================================================================ */

TEST_SUITE(setjmp);

TEST(setjmp_initialized) {
    jmp_buf env = {0};
    int r = setjmp(env);
    ASSERT_EQ(r, 0);
}

TEST(setjmp_uninitialized) {
    jmp_buf env;
    int r = setjmp(env);
    ASSERT_EQ(r, 0);
}

/* ============================================================================ */

TEST_SUITE(longjmp);

TEST(longjmp_basic) {
    jmp_buf env = {0};
    volatile int val = setjmp(env);

    if (val == 0) longjmp(env, 42);
    ASSERT_EQ(val, 42);
}

TEST(longjmp_zero_to_one) {
    jmp_buf env = {0};
    volatile int val = setjmp(env);

    if (val == 0) longjmp(env, 0);
    ASSERT_EQ(val, 1);
}

TEST(longjmp_multiple_buffers) {
    jmp_buf buf1 = {0}, buf2 = {0};
    volatile int val = 0;

    val = setjmp(buf1);

    if (val == 0) val = setjmp(buf2);
    if (val == 0) longjmp(buf1, 1);
    if (val == 1) longjmp(buf2, 2);

    ASSERT_INT_EQ(val, 2);
}

TEST(longjmp_nested_calls) {
    jmp_buf outer = {0}, inner = {0};
    int outer_val = 0;
    int inner_val = 0;
    int path_taken = 0;  /* Which buffer returned? */

    outer_val = setjmp(outer);
    if (outer_val == 0) {
        inner_val = setjmp(inner);

        if (inner_val == 0) longjmp(outer, 77);

        path_taken = 2;
    } else {
        ASSERT_EQ(outer_val, 77);
        ASSERT_EQ(inner_val, 0);   /* Inner never saw this value */
        path_taken = 1;            /* Outer was the return target */
    }

    ASSERT_EQ(path_taken, 1);  /* Verify outer was the one that returned */
}

TEST(longjmp_resource_lock) {
    volatile int fake_mutex = 0;

    pid_t pid = fork();
    if (pid == 0) {
        jmp_buf env;
        if (setjmp(env) == 0) {
            fake_mutex = 1;
            longjmp(env, 1);
        }

        alarm(1);
        while (fake_mutex == 1) {}
        exit(0);
    }

    int status;
    waitpid(pid, &status, 0);

    ASSERT_TRUE(WIFSIGNALED(status));
    ASSERT_EQ(WTERMSIG(status), SIGALRM);
}


/* ============================================================================ */

TEST_SUITE(sigsetjmp);

TEST(sigsetjmp_no_mask) {
    sigjmp_buf env = {0};
    int r = sigsetjmp(env, 0);
    ASSERT_EQ(r, 0);
}

TEST(sigsetjmp_with_mask) {
    sigjmp_buf env = {0};
    int r = sigsetjmp(env, 1);
    ASSERT_EQ(r, 0);
}

TEST(sigsetjmp_save_disabled) {
    sigjmp_buf env = {0};
    int r = sigsetjmp(env, 0);

    ASSERT_EQ(r, 0);
    ASSERT_EQ(env->saved, 0);
}

TEST(sigsetjmp_context_calling) {
	signal_caught = 0;

	sig_t old = signal(SIGURG, signal_jump_handler);
	ASSERT_NE(old, SIG_ERR);

	int val = sigsetjmp(signal_env, 1);

	if (val == 0) {
		raise(SIGURG);
		ASSERT_TRUE(0);
	} else {
		ASSERT_EQ(val, 99);
		ASSERT_EQ(signal_caught, 1);
	}

	signal(SIGURG, old);
}

/* ============================================================================ */

TEST_SUITE(siglongjmp);

TEST(siglongjmp_no_mask) {
    sigjmp_buf env = {0};
    volatile int val = sigsetjmp(env, 0);

    if (val == 0) siglongjmp(env, 77);
    ASSERT_EQ(val, 77);
}

TEST(siglongjmp_with_mask) {
    sigjmp_buf env = {0};
    volatile int val = sigsetjmp(env, 1);

    if (val == 0) siglongjmp(env, 88);
    ASSERT_EQ(val, 88);
}

TEST(siglongjmp_save_signal) {
	sigjmp_buf env = {0};
	sigset_t before, after;

	sigemptyset(&before);
	sigemptyset(&after);

	sigprocmask(SIG_SETMASK, NULL, &before);

	if (sigsetjmp(env, 1) == 0) {
		sigset_t temp;
		sigemptyset(&temp);
		sigaddset(&temp, SIGTERM);
		sigprocmask(SIG_BLOCK, &temp, NULL);

		siglongjmp(env, 1);
	}

	sigprocmask(SIG_SETMASK, NULL, &after);

	ASSERT_EQ(before.__bits[0], after.__bits[0]);
	ASSERT_EQ(before.__bits[1], after.__bits[1]);
}

TEST(siglongjmp_zero_to_one) {
    sigjmp_buf env = {0};
    volatile int val = sigsetjmp(env, 0);

    if (val == 0) siglongjmp(env, 0);
    ASSERT_EQ(val, 1);
}

TEST(siglongjmp_nested_buffers) {
    sigjmp_buf outer = {0}, inner = {0};
    int result = 0;
    int jumped_from_inner = 0;

    /* First setjmp via outer */
    result = sigsetjmp(outer, 1);
    if (result == 0) {
        /* Set up flag that we entered inner scope */
        jumped_from_inner = 0;

        result = sigsetjmp(inner, 1);
        if (result == 0) {
            /* We've now entered inner - set the flag */
            jumped_from_inner = 1;

            /* Jump back to specifically outer */
            siglongjmp(outer, 42);
        }
        /* If we reach here, inner returned non-zero (unexpected in this path) */
        ASSERT_FALSE(jumped_from_inner);
    } else {
        /* Returned from outer after jumping from inner */
        ASSERT_EQ(result, 42);
        ASSERT_TRUE(jumped_from_inner);
    }
}

/* ============================================================================ */

TEST_MAIN()
