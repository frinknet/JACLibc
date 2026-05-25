/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/futex.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>

TEST_TYPE(unit);
TEST_UNIT(sys/futex.h);

/* ============================================================================ */
/* Helpers                                                                      */
/* ============================================================================ */

typedef struct {
    uint32_t *uaddr;
    atomic_int *flag;
    int op;
    uint32_t val;
} futex_ctx_t;

static void* __thread_wait_op(void *p) {
    futex_ctx_t *ctx = (futex_ctx_t*)p;
    futex(ctx->uaddr, ctx->op, ctx->val, NULL, NULL, 0);
    atomic_store(ctx->flag, 1);
    return NULL;
}

static int __wait_for_flag(atomic_int *flag, int expected, int max_ms) {
    for (int i = 0; i < max_ms; i++) {
        if (atomic_load(flag) == expected) return 1;
        usleep(1000);
    }
    return 0;
}

/* ============================================================================ */
/* Suite: futex_constants                                                       */
/* ============================================================================ */

TEST_SUITE(futex_constants);

TEST(futex_constants_values) {
    ASSERT_EQ(0, FUTEX_WAIT);
    ASSERT_EQ(1, FUTEX_WAKE);
    ASSERT_EQ(2, FUTEX_FD);
    ASSERT_EQ(3, FUTEX_REQUEUE);
    ASSERT_EQ(4, FUTEX_CMP_REQUEUE);
    ASSERT_EQ(5, FUTEX_WAKE_OP);
    ASSERT_EQ(128, FUTEX_PRIVATE_FLAG);
    ASSERT_EQ(256, FUTEX_CLOCK_REALTIME);
}

TEST(futex_constants_bitset) {
    ASSERT_EQ(9, FUTEX_WAIT_BITSET);
    ASSERT_EQ(10, FUTEX_WAKE_BITSET);
}

/* ============================================================================ */
/* Suite: futex_wait                                                            */
/* ============================================================================ */

TEST_SUITE(futex_wait);

TEST(futex_wait_success) {
    uint32_t uaddr = 0;
    atomic_int done = 0;
    pthread_t t;
    futex_ctx_t ctx = { .uaddr = &uaddr, .flag = &done, .op = FUTEX_WAIT, .val = 0 };

    ASSERT_EQ(0, pthread_create(&t, NULL, __thread_wait_op, &ctx));
    usleep(2000);

    atomic_store(&uaddr, 1);
    int ret = futex(&uaddr, FUTEX_WAKE, 1, NULL, NULL, 0);
    ASSERT_TRUE(ret >= 0);
    ASSERT_TRUE(__wait_for_flag(&done, 1, 100));
    pthread_join(t, NULL);
}

TEST(futex_wait_eagain) {
    uint32_t uaddr = 42;
    int ret = futex(&uaddr, FUTEX_WAIT, 0, NULL, NULL, 0);
    ASSERT_EQ(-1, ret);
    ASSERT_ERRNO(EAGAIN);
}

TEST(futex_wait_timeout) {
    uint32_t uaddr = 0;
    struct timespec ts = { .tv_sec = 0, .tv_nsec = 10000000 }; /* 10ms */
    int ret = futex(&uaddr, FUTEX_WAIT, 0, &ts, NULL, 0);
    ASSERT_EQ(-1, ret);
    ASSERT_ERRNO(ETIMEDOUT);
}

TEST(futex_wait_null_ptr) {
    int ret = futex(NULL, FUTEX_WAIT, 0, NULL, NULL, 0);
    ASSERT_EQ(-1, ret);
    ASSERT_ERRNO(EFAULT);
}

TEST(futex_wait_unaligned) {
    char *buf = malloc(10);
    uint32_t *uaddr = (uint32_t*)(buf + 1); 
    *uaddr = 0;
    
    int ret = futex(uaddr, FUTEX_WAIT, 0, NULL, NULL, 0);
    ASSERT_TRUE(ret == -1 || ret == 0); 
    free(buf);
}

/* ============================================================================ */
/* Suite: futex_wake                                                            */
/* ============================================================================ */

TEST_SUITE(futex_wake);

TEST(futex_wake_no_waiters) {
    uint32_t uaddr = 0;
    int ret = futex(&uaddr, FUTEX_WAKE, 10, NULL, NULL, 0);
    ASSERT_EQ(0, ret);
}

TEST(futex_wake_zero_count) {
    uint32_t uaddr = 0;
    int ret = futex(&uaddr, FUTEX_WAKE, 0, NULL, NULL, 0);
    ASSERT_EQ(0, ret);
}

TEST(futex_wake_negative_count) {
    uint32_t uaddr = 0;
    int ret = futex(&uaddr, FUTEX_WAKE, -1, NULL, NULL, 0);
    ASSERT_TRUE(ret >= 0); 
}

TEST(futex_wake_null_ptr) {
    int ret = futex(NULL, FUTEX_WAKE, 1, NULL, NULL, 0);
    ASSERT_EQ(-1, ret);
    ASSERT_ERRNO(EFAULT);
}

/* ============================================================================ */
/* Suite: futex_requeue                                                         */
/* ============================================================================ */

TEST_SUITE(futex_requeue);

TEST(futex_requeue_basic) {
    uint32_t uaddr1 = 0, uaddr2 = 0;
    int ret = futex(&uaddr1, FUTEX_REQUEUE, 1, NULL, &uaddr2, 0);
    ASSERT_TRUE(ret >= 0);
}

TEST(futex_requeue_null_src) {
    uint32_t uaddr2 = 0;
    int ret = futex(NULL, FUTEX_REQUEUE, 1, NULL, &uaddr2, 0);
    ASSERT_EQ(-1, ret);
    ASSERT_ERRNO(EFAULT);
}

TEST(futex_requeue_null_dest) {
    uint32_t uaddr1 = 0;
    int ret = futex(&uaddr1, FUTEX_REQUEUE, 1, NULL, NULL, 0);
    ASSERT_EQ(-1, ret);
    ASSERT_ERRNO(EFAULT);
}

/* ============================================================================ */
/* Suite: futex_cmp_requeue                                                     */
/* ============================================================================ */

TEST_SUITE(futex_cmp_requeue);

TEST(futex_cmp_requeue_mismatch) {
    uint32_t uaddr1 = 5, uaddr2 = 0;
    int ret = futex(&uaddr1, FUTEX_CMP_REQUEUE, 1, NULL, &uaddr2, 0);
    ASSERT_EQ(-1, ret);
    ASSERT_ERRNO(EAGAIN);
}

TEST(futex_cmp_requeue_match) {
    uint32_t uaddr1 = 0, uaddr2 = 0;
    int ret = futex(&uaddr1, FUTEX_CMP_REQUEUE, 1, NULL, &uaddr2, 0);
    ASSERT_TRUE(ret >= 0);
}

/* ============================================================================ */
/* Suite: futex_invalid                                                         */
/* ============================================================================ */

TEST_SUITE(futex_invalid);

TEST(futex_invalid_op) {
    uint32_t uaddr = 0;
    int ret = futex(&uaddr, 0xFF, 0, NULL, NULL, 0);
    ASSERT_EQ(-1, ret);
    ASSERT_TRUE(errno == EINVAL || errno == ENOSYS);
}

TEST(futex_invalid_bad_nsec) {
    uint32_t uaddr = 0;
    struct timespec ts = { .tv_sec = 0, .tv_nsec = 2000000000 };
    int ret = futex(&uaddr, FUTEX_WAIT, 0, &ts, NULL, 0);
    ASSERT_EQ(-1, ret);
    ASSERT_TRUE(errno == EINVAL || errno == ENOSYS);
}

TEST(futex_invalid_negative_sec) {
    uint32_t uaddr = 0;
    struct timespec ts = { .tv_sec = -1, .tv_nsec = 0 };
    int ret = futex(&uaddr, FUTEX_WAIT, 0, &ts, NULL, 0);
    ASSERT_EQ(-1, ret);
    ASSERT_TRUE(errno == EINVAL || errno == ENOSYS);
}

TEST_MAIN()
