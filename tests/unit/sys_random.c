/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/random.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

TEST_TYPE(unit);
TEST_UNIT(sys/random.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_grnd_flags) {
    ASSERT_EQ(GRND_NONBLOCK, 0x0001);
    ASSERT_EQ(GRND_RANDOM, 0x0002);
    ASSERT_NE(GRND_NONBLOCK, GRND_RANDOM);
    ASSERT_EQ((GRND_NONBLOCK | GRND_RANDOM), 0x0003);
}

/* ============================================================================ */
TEST_SUITE(getrandom_happy);

TEST(getrandom_signature) {
    ssize_t (*fp)(void *, size_t, unsigned int) = getrandom;
    ASSERT_NOT_NULL(fp);
}

TEST(getrandom_zero_length) {
    ssize_t r = getrandom(NULL, 0, 0);
    ASSERT_EQ(r, 0);
}

TEST(getrandom_single_byte) {
    unsigned char buf[1] = {0};
    ssize_t r = getrandom(buf, 1, 0);
    ASSERT_EQ(r, 1);
}

TEST(getrandom_basic_fill) {
    unsigned char buf[32] = {0};
    ssize_t r = getrandom(buf, sizeof(buf), 0);
    ASSERT_EQ(r, sizeof(buf));
    
    /* Verify non-trivial entropy */
    int all_zero = 1;
    for (size_t i = 0; i < sizeof(buf); i++) if (buf[i] != 0) all_zero = 0;
    ASSERT_FALSE(all_zero);
}

TEST(getrandom_flags_random) {
    /* GRND_RANDOM requests /dev/random */
    unsigned char buf[32] = {0};
    ssize_t r = getrandom(buf, sizeof(buf), GRND_RANDOM);
    ASSERT_EQ(r, sizeof(buf));
}

TEST(getrandom_flags_nonblock) {
    /* GRND_NONBLOCK should not block */
    unsigned char buf[32] = {0};
    ssize_t r = getrandom(buf, sizeof(buf), GRND_NONBLOCK);
    ASSERT_EQ(r, sizeof(buf));
}

TEST(getrandom_flags_combined) {
    unsigned char buf[32] = {0};
    ssize_t r = getrandom(buf, sizeof(buf), GRND_RANDOM | GRND_NONBLOCK);
    ASSERT_EQ(r, sizeof(buf));
}

TEST(getrandom_large_buffer) {
    /* Test loop behavior with 1MB buffer */
    size_t size = 1024 * 1024;
    unsigned char *buf = malloc(size);
    if (!buf) {
        TEST_SKIP("malloc failed");
        return;
    }
    memset(buf, 0, size);
    ssize_t r = getrandom(buf, size, 0);
    ASSERT_EQ(r, size);
    free(buf);
}

TEST(getrandom_reproducibility_check) {
    /* Two calls should yield different buffers */
    unsigned char b1[16] = {0}, b2[16] = {0};
    getrandom(b1, sizeof(b1), 0);
    getrandom(b2, sizeof(b2), 0);
    ASSERT_MEM_NE(b1, b2, sizeof(b1));
}

/* ============================================================================ */
TEST_SUITE(getrandom_sad);

TEST(getrandom_null_buffer) {
    errno = 0;
    ssize_t r = getrandom(NULL, 1, 0);
    ASSERT_EQ(r, -1);
    ASSERT_ERRNO(EINVAL);
}

TEST(getrandom_null_buffer_zero_len) {
    /* NULL buffer with 0 len is valid */
    ssize_t r = getrandom(NULL, 0, 0);
    ASSERT_EQ(r, 0);
}

TEST(getrandom_garbage_flags) {
    unsigned char buf[16];
    /* Should not crash; behavior depends on impl (usually ignores unknown bits) */
    ssize_t r = getrandom(buf, sizeof(buf), 0xFF00);
    ASSERT_TRUE(r == sizeof(buf) || r == -1);
}

/* ============================================================================ */
TEST_SUITE(getrandom_concurrent);

static void *thread_random(void *arg) {
    unsigned char buf[32];
    ssize_t r = getrandom(buf, sizeof(buf), 0);
    return (r == sizeof(buf)) ? NULL : (void*)-1;
}

TEST(getrandom_thread_safety) {
    pthread_t t[10];
    for(int i=0; i<10; i++) {
        pthread_create(&t[i], NULL, thread_random, NULL);
    }
    for(int i=0; i<10; i++) {
        void *res;
        pthread_join(t[i], &res);
        ASSERT_PTR_EQ(res, NULL);
    }
}

/* ============================================================================ */
TEST_MAIN()
