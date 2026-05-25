/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

TEST_TYPE(unit);
TEST_UNIT(sys/sem.h);

/* ============================================================================
 * HELPERS
 * ============================================================================ */
static void *__poster_delayed(void *arg) {
	sem_t *s = (sem_t *)arg;
	usleep(50000);
	sem_post(s);
	return NULL;
}

static void *__waiter(void *arg) {
	sem_t *s = (sem_t *)arg;
	sem_wait(s);
	return NULL;
}

static void *__sysv_waiter(void *arg) {
	int id = (int)(intptr_t)arg;
	struct sembuf sb = {0, -1, 0};
	semop(id, &sb, 1);
	return NULL;
}

static void *__sysv_zcnt_waiter(void *arg) {
	int id = (int)(intptr_t)arg;
	struct sembuf sb = {0, 0, 0}; /* wait for zero */
	semop(id, &sb, 1);
	return NULL;
}

static _Atomic int __name_counter = 0;
static int __make_name(char *buf, size_t len, const char *tag) {
	int n = atomic_fetch_add(&__name_counter, 1);
	return snprintf(buf, len, "/t_%s_%d_%d", tag, (int)getpid(), n);
}

/* ============================================================================
 * CONSTANTS
 * ============================================================================ */
TEST_SUITE(constants_ipc);

TEST(constants_ipc_flags) {
	ASSERT_EQ(01000, IPC_CREAT);
	ASSERT_EQ(02000, IPC_EXCL);
	ASSERT_EQ(04000, IPC_NOWAIT);
}

TEST(constants_ipc_cmds) {
	ASSERT_EQ(0, IPC_RMID);
	ASSERT_EQ(1, IPC_SET);
	ASSERT_EQ(2, IPC_STAT);
}

TEST(constants_ipc_private) {
	ASSERT_EQ(0, IPC_PRIVATE);
}

TEST_SUITE(constants_sem);

TEST(constants_sem_failed) {
	ASSERT_EQ(-1, SEM_FAILED);
}

TEST(constants_sem_undo) {
	ASSERT_EQ(0x1000, SEM_UNDO);
}

TEST_SUITE(constants_getset);

TEST(constants_getset_vals) {
	ASSERT_EQ(12, GETVAL);
	ASSERT_EQ(16, SETVAL);
	ASSERT_EQ(13, GETALL);
	ASSERT_EQ(17, SETALL);
}

TEST(constants_getset_info) {
	ASSERT_EQ(11, GETPID);
	ASSERT_EQ(14, GETNCNT);
	ASSERT_EQ(15, GETZCNT);
}

/* ============================================================================
 * sem_init_struct
 * ============================================================================ */
TEST_SUITE(sem_init_struct);

TEST(sem_init_struct_null) {
	sem_init_struct(NULL); /* must not crash */
}

TEST(sem_init_struct_sets_failed) {
	sem_t s = 42;
	sem_init_struct(&s);
	ASSERT_EQ(SEM_FAILED, s);
}

/* ============================================================================
 * sem_is_valid
 * ============================================================================ */
TEST_SUITE(sem_is_valid);

#if SEM_POSIX

TEST(sem_is_valid_null) { ASSERT_FALSE(sem_is_valid(NULL)); }

TEST(sem_is_valid_bad_values) {
	sem_t a = SEM_FAILED, b = -5, c = 99999;
	ASSERT_FALSE(sem_is_valid(&a));
	ASSERT_FALSE(sem_is_valid(&b));
	ASSERT_FALSE(sem_is_valid(&c));
}

TEST(sem_is_valid_anon) {
	sem_t s; sem_init(&s, 0, 1);
	ASSERT_TRUE(sem_is_valid(&s));
	sem_destroy(&s);
	ASSERT_FALSE(sem_is_valid(&s));
}

TEST(sem_is_valid_named) {
	char n[64]; __make_name(n, sizeof(n), "iv");
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	ASSERT_TRUE(sem_is_valid(&s));
	sem_close(s); sem_unlink(n);
}

#endif

/* ============================================================================
 * sem_is_anonymous
 * ============================================================================ */
TEST_SUITE(sem_is_anonymous);

#if SEM_POSIX

TEST(sem_is_anonymous_invalid) {
	ASSERT_FALSE(sem_is_anonymous(NULL));
	sem_t s = SEM_FAILED;
	ASSERT_FALSE(sem_is_anonymous(&s));
}

TEST(sem_is_anonymous_true) {
	sem_t s; sem_init(&s, 0, 1);
	ASSERT_TRUE(sem_is_anonymous(&s));
	sem_destroy(&s);
}

TEST(sem_is_anonymous_false_named) {
	char n[64]; __make_name(n, sizeof(n), "ia");
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	ASSERT_FALSE(sem_is_anonymous(&s));
	sem_close(s); sem_unlink(n);
}

#endif

/* ============================================================================
 * sem_is_named
 * ============================================================================ */
TEST_SUITE(sem_is_named);

#if SEM_POSIX

TEST(sem_is_named_invalid) {
	ASSERT_FALSE(sem_is_named(NULL));
	sem_t s = SEM_FAILED;
	ASSERT_FALSE(sem_is_named(&s));
}

TEST(sem_is_named_true) {
	char n[64]; __make_name(n, sizeof(n), "in");
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	ASSERT_TRUE(sem_is_named(&s));
	sem_close(s); sem_unlink(n);
}

TEST(sem_is_named_false_anon) {
	sem_t s; sem_init(&s, 0, 1);
	ASSERT_FALSE(sem_is_named(&s));
	sem_destroy(&s);
}

#endif

/* ============================================================================
 * sem_is_unlinked
 * ============================================================================ */
TEST_SUITE(sem_is_unlinked);

#if SEM_POSIX

TEST(sem_is_unlinked_not_named) {
	ASSERT_EQ(0, sem_is_unlinked(NULL));
	sem_t a; sem_init(&a, 0, 1);
	ASSERT_EQ(0, sem_is_unlinked(&a));
	sem_destroy(&a);
}

TEST(sem_is_unlinked_lifecycle) {
	char n[64]; __make_name(n, sizeof(n), "iu");
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	ASSERT_EQ(0, sem_is_unlinked(&s));
	sem_unlink(n);
	ASSERT_EQ(1, sem_is_unlinked(&s));
	sem_close(s);
}

#endif

/* ============================================================================
 * sem_get_name
 * ============================================================================ */
TEST_SUITE(sem_get_name);

#if SEM_POSIX

TEST(sem_get_name_not_named) {
	ASSERT_NULL(sem_get_name(NULL));
	sem_t a; sem_init(&a, 0, 1);
	ASSERT_NULL(sem_get_name(&a));
	sem_destroy(&a);
}

TEST(sem_get_name_returns_name) {
	char n[64]; __make_name(n, sizeof(n), "gn");
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	const char *got = sem_get_name(&s);
	ASSERT_NOT_NULL(got);
	ASSERT_STR_EQ(n, got);
	sem_close(s); sem_unlink(n);
}

#endif

/* ============================================================================
 * sem_get_refcount
 * ============================================================================ */
TEST_SUITE(sem_get_refcount);

#if SEM_POSIX

TEST(sem_get_refcount_invalid) {
	ASSERT_EQ(-1, sem_get_refcount(NULL));
	sem_t s = SEM_FAILED;
	ASSERT_EQ(-1, sem_get_refcount(&s));
}

TEST(sem_get_refcount_anon) {
	sem_t s; sem_init(&s, 0, 1);
	ASSERT_EQ(1, sem_get_refcount(&s));
	sem_destroy(&s);
}

TEST(sem_get_refcount_named_increments) {
	char n[64]; __make_name(n, sizeof(n), "rc");
	sem_t s1 = sem_open(n, O_CREAT, 0644, 1);
	ASSERT_EQ(1, sem_get_refcount(&s1));
	sem_t s2 = sem_open(n, 0);
	ASSERT_EQ(2, sem_get_refcount(&s1));
	sem_close(s1);
	ASSERT_EQ(1, sem_get_refcount(&s2));
	sem_close(s2); sem_unlink(n);
}

#endif

/* ============================================================================
 * sem_open
 * ============================================================================ */
TEST_SUITE(sem_open);

#if SEM_POSIX

TEST(sem_open_bad_name) {
	errno = 0; ASSERT_EQ(SEM_FAILED, sem_open(NULL, O_CREAT, 0644, 1));  ASSERT_EQ(EINVAL, errno);
	errno = 0; ASSERT_EQ(SEM_FAILED, sem_open("x", O_CREAT, 0644, 1));   ASSERT_EQ(EINVAL, errno);
	errno = 0; ASSERT_EQ(SEM_FAILED, sem_open("/", O_CREAT, 0644, 1));   ASSERT_EQ(EINVAL, errno);
	errno = 0; ASSERT_EQ(SEM_FAILED, sem_open("/x", O_EXCL, 0644, 1));   ASSERT_EQ(EINVAL, errno);
}

TEST(sem_open_no_such) {
	errno = 0;
	sem_t s = sem_open("/nonexistent_99", 0);
	ASSERT_EQ(SEM_FAILED, s);
	ASSERT_EQ(ENOENT, errno);
}

TEST(sem_open_create) {
	char n[64]; __make_name(n, sizeof(n), "oc");
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	ASSERT_NE(SEM_FAILED, s);
	ASSERT_TRUE(sem_is_named(&s));
	ASSERT_STR_EQ(n, sem_get_name(&s));
	int v; sem_getvalue(&s, &v); ASSERT_EQ(1, v);
	sem_close(s); sem_unlink(n);
}

TEST(sem_open_create_value) {
	char n[64]; __make_name(n, sizeof(n), "ov");
	sem_t s = sem_open(n, O_CREAT, 0644, 0);
	int v; sem_getvalue(&s, &v); ASSERT_EQ(0, v);
	sem_close(s); sem_unlink(n);
}

TEST(sem_open_excl_existing) {
	char n[64]; __make_name(n, sizeof(n), "oe");
	sem_t s1 = sem_open(n, O_CREAT, 0644, 1);
	errno = 0;
	sem_t s2 = sem_open(n, O_CREAT | O_EXCL, 0644, 1);
	ASSERT_EQ(SEM_FAILED, s2);
	ASSERT_EQ(EEXIST, errno);
	sem_close(s1); sem_unlink(n);
}

TEST(sem_open_reopen) {
	char n[64]; __make_name(n, sizeof(n), "or");
	sem_t s1 = sem_open(n, O_CREAT, 0644, 1);
	sem_t s2 = sem_open(n, 0);
	ASSERT_EQ(2, sem_get_refcount(&s1));
	sem_close(s1);
	ASSERT_EQ(1, sem_get_refcount(&s2));
	sem_close(s2); sem_unlink(n);
}

#endif

/* ============================================================================
 * sem_close
 * ============================================================================ */
TEST_SUITE(sem_close);

#if SEM_POSIX

TEST(sem_close_bad_handle) {
	errno = 0; ASSERT_EQ(-1, sem_close(SEM_FAILED)); ASSERT_EQ(EINVAL, errno);
	errno = 0; ASSERT_EQ(-1, sem_close(-5));         ASSERT_EQ(EINVAL, errno);
	errno = 0; ASSERT_EQ(-1, sem_close(99999));      ASSERT_EQ(EINVAL, errno);
}

TEST(sem_close_anon_fails) {
	sem_t s; sem_init(&s, 0, 1);
	errno = 0;
	ASSERT_EQ(-1, sem_close(s));
	ASSERT_EQ(EINVAL, errno);
	sem_destroy(&s);
}

TEST(sem_close_double) {
	char n[64]; __make_name(n, sizeof(n), "cd");
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	ASSERT_EQ(0, sem_close(s));
	errno = 0;
	ASSERT_EQ(-1, sem_close(s));
	ASSERT_EQ(EINVAL, errno);
	sem_unlink(n);
}

TEST(sem_close_unlinked_destroys) {
	char n[64]; __make_name(n, sizeof(n), "cu");
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	sem_unlink(n);
	ASSERT_EQ(0, sem_close(s));
	errno = 0;
	ASSERT_EQ(SEM_FAILED, sem_open(n, 0));
	ASSERT_EQ(ENOENT, errno);
}

TEST(sem_close_keeps_if_not_unlinked) {
	char n[64]; __make_name(n, sizeof(n), "ck");
	sem_t s1 = sem_open(n, O_CREAT, 0644, 1);
	sem_t s2 = sem_open(n, 0);
	ASSERT_EQ(0, sem_close(s1));
	ASSERT_EQ(1, sem_get_refcount(&s2));
	sem_close(s2); sem_unlink(n);
}

#endif

/* ============================================================================
 * sem_unlink
 * ============================================================================ */
TEST_SUITE(sem_unlink);

#if SEM_POSIX

TEST(sem_unlink_bad) {
	errno = 0; ASSERT_EQ(-1, sem_unlink(NULL));          ASSERT_EQ(EINVAL, errno);
	errno = 0; ASSERT_EQ(-1, sem_unlink("/no_such_xyz")); ASSERT_EQ(ENOENT, errno);
}

TEST(sem_unlink_double) {
	char n[64]; __make_name(n, sizeof(n), "ud");
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	ASSERT_EQ(0, sem_unlink(n));
	errno = 0;
	ASSERT_EQ(-1, sem_unlink(n));
	ASSERT_EQ(ENOENT, errno);
	sem_close(s);
}

TEST(sem_unlink_no_refs_destroys_now) {
	char n[64]; __make_name(n, sizeof(n), "ui");
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	sem_close(s);
	ASSERT_EQ(0, sem_unlink(n));
	errno = 0;
	ASSERT_EQ(SEM_FAILED, sem_open(n, 0));
	ASSERT_EQ(ENOENT, errno);
}

#endif

/* ============================================================================
 * sem_init
 * ============================================================================ */
TEST_SUITE(sem_init);

#if SEM_POSIX

TEST(sem_init_null) {
	errno = 0;
	ASSERT_EQ(-1, sem_init(NULL, 0, 1));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sem_init_values) {
	sem_t s;
	ASSERT_EQ(0, sem_init(&s, 0, 5));
	ASSERT_TRUE(sem_is_anonymous(&s));
	int v; sem_getvalue(&s, &v); ASSERT_EQ(5, v);
	sem_destroy(&s);

	ASSERT_EQ(0, sem_init(&s, 0, 0));
	sem_getvalue(&s, &v); ASSERT_EQ(0, v);
	sem_destroy(&s);

	ASSERT_EQ(0, sem_init(&s, 0, 0x7FFFFFFF));
	sem_getvalue(&s, &v); ASSERT_EQ(0x7FFFFFFF, v);
	sem_destroy(&s);
}

TEST(sem_init_reinit_after_destroy) {
	sem_t s;
	sem_init(&s, 0, 1); sem_destroy(&s);
	sem_init(&s, 0, 7);
	int v; sem_getvalue(&s, &v); ASSERT_EQ(7, v);
	sem_destroy(&s);
}

#endif

/* ============================================================================
 * sem_destroy
 * ============================================================================ */
TEST_SUITE(sem_destroy);

#if SEM_POSIX

TEST(sem_destroy_bad) {
	errno = 0; ASSERT_EQ(-1, sem_destroy(NULL));  ASSERT_EQ(EINVAL, errno);
	sem_t s = SEM_FAILED;
	errno = 0; ASSERT_EQ(-1, sem_destroy(&s));    ASSERT_EQ(EINVAL, errno);
}

TEST(sem_destroy_named_fails) {
	char n[64]; __make_name(n, sizeof(n), "dn");
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	errno = 0;
	ASSERT_EQ(-1, sem_destroy(&s));
	ASSERT_EQ(EINVAL, errno);
	sem_close(s); sem_unlink(n);
}

TEST(sem_destroy_anon) {
	sem_t s; sem_init(&s, 0, 1);
	ASSERT_EQ(0, sem_destroy(&s));
	ASSERT_FALSE(sem_is_valid(&s));
	errno = 0;
	ASSERT_EQ(-1, sem_destroy(&s));
	ASSERT_EQ(EINVAL, errno);
}

#endif

/* ============================================================================
 * sem_wait
 * ============================================================================ */
TEST_SUITE(sem_wait);

#if SEM_POSIX

TEST(sem_wait_bad) {
	errno = 0; ASSERT_EQ(-1, sem_wait(NULL)); ASSERT_EQ(EINVAL, errno);
	sem_t s = SEM_FAILED;
	errno = 0; ASSERT_EQ(-1, sem_wait(&s));   ASSERT_EQ(EINVAL, errno);
}

TEST(sem_wait_decrements) {
	sem_t s; sem_init(&s, 0, 3);
	sem_wait(&s); sem_wait(&s); sem_wait(&s);
	int v; sem_getvalue(&s, &v); ASSERT_EQ(0, v);
	sem_destroy(&s);
}

TEST(sem_wait_blocks) {
	sem_t s; sem_init(&s, 0, 0);
	pthread_t t;
	pthread_create(&t, NULL, __poster_delayed, &s);
	ASSERT_EQ(0, sem_wait(&s));
	pthread_join(t, NULL);
	sem_destroy(&s);
}

TEST(sem_wait_multiple_waiters) {
	sem_t s; sem_init(&s, 0, 0);
	pthread_t t[4];
	for (int i = 0; i < 4; i++) pthread_create(&t[i], NULL, __waiter, &s);
	usleep(50000);
	for (int i = 0; i < 4; i++) sem_post(&s);
	for (int i = 0; i < 4; i++) pthread_join(t[i], NULL);
	int v; sem_getvalue(&s, &v); ASSERT_EQ(0, v);
	sem_destroy(&s);
}

#endif

/* ============================================================================
 * sem_trywait
 * ============================================================================ */
TEST_SUITE(sem_trywait);

#if SEM_POSIX

TEST(sem_trywait_bad) {
	errno = 0; ASSERT_EQ(-1, sem_trywait(NULL)); ASSERT_EQ(EINVAL, errno);
	sem_t s = SEM_FAILED;
	errno = 0; ASSERT_EQ(-1, sem_trywait(&s));   ASSERT_EQ(EINVAL, errno);
}

TEST(sem_trywait_ok) {
	sem_t s; sem_init(&s, 0, 2);
	ASSERT_EQ(0, sem_trywait(&s));
	int v; sem_getvalue(&s, &v); ASSERT_EQ(1, v);
	sem_destroy(&s);
}

TEST(sem_trywait_eagain) {
	sem_t s; sem_init(&s, 0, 1);
	sem_trywait(&s); /* drain */
	errno = 0;
	ASSERT_EQ(-1, sem_trywait(&s));
	ASSERT_EQ(EAGAIN, errno);
	sem_destroy(&s);
}

#endif

/* ============================================================================
 * sem_timedwait
 * ============================================================================ */
TEST_SUITE(sem_timedwait);

#if SEM_POSIX

TEST(sem_timedwait_bad) {
	struct timespec ts = {0, 0};
	errno = 0; ASSERT_EQ(-1, sem_timedwait(NULL, &ts)); ASSERT_EQ(EINVAL, errno);
	sem_t s; sem_init(&s, 0, 1);
	errno = 0; ASSERT_EQ(-1, sem_timedwait(&s, NULL));  ASSERT_EQ(EINVAL, errno);
	struct timespec bad1 = {1, -1};
	errno = 0; ASSERT_EQ(-1, sem_timedwait(&s, &bad1)); ASSERT_EQ(EINVAL, errno);
	struct timespec bad2 = {1, 1000000000L};
	errno = 0; ASSERT_EQ(-1, sem_timedwait(&s, &bad2)); ASSERT_EQ(EINVAL, errno);
	sem_destroy(&s);
}

TEST(sem_timedwait_timeout) {
	sem_t s; sem_init(&s, 0, 0);
	struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_nsec += 50000000;
	if (ts.tv_nsec >= 1000000000L) { ts.tv_sec++; ts.tv_nsec -= 1000000000L; }
	errno = 0;
	ASSERT_EQ(-1, sem_timedwait(&s, &ts));
	ASSERT_EQ(ETIMEDOUT, errno);
	sem_destroy(&s);
}

TEST(sem_timedwait_past) {
	sem_t s; sem_init(&s, 0, 0);
	struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec -= 10;
	errno = 0;
	ASSERT_EQ(-1, sem_timedwait(&s, &ts));
	ASSERT_EQ(ETIMEDOUT, errno);
	sem_destroy(&s);
}

TEST(sem_timedwait_wakes) {
	sem_t s; sem_init(&s, 0, 0);
	struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts); ts.tv_sec += 5;
	pthread_t t; pthread_create(&t, NULL, __poster_delayed, &s);
	ASSERT_EQ(0, sem_timedwait(&s, &ts));
	pthread_join(t, NULL);
	sem_destroy(&s);
}

TEST(sem_timedwait_immediate) {
	sem_t s; sem_init(&s, 0, 1);
	struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts);
	ASSERT_EQ(0, sem_timedwait(&s, &ts));
	sem_destroy(&s);
}

#endif

/* ============================================================================
 * sem_clockwait
 * ============================================================================ */
TEST_SUITE(sem_clockwait);

#if SEM_POSIX

TEST(sem_clockwait_timeout) {
	sem_t s; sem_init(&s, 0, 0);
	struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts); ts.tv_nsec += 10000000;
	if (ts.tv_nsec >= 1000000000L) { ts.tv_sec++; ts.tv_nsec -= 1000000000L; }
	errno = 0;
	ASSERT_EQ(-1, sem_clockwait(&s, CLOCK_REALTIME, &ts));
	ASSERT_EQ(ETIMEDOUT, errno);
	sem_destroy(&s);
}

TEST(sem_clockwait_ok) {
	sem_t s; sem_init(&s, 0, 1);
	struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts); ts.tv_sec += 1;
	ASSERT_EQ(0, sem_clockwait(&s, CLOCK_REALTIME, &ts));
	sem_destroy(&s);
}

#endif

/* ============================================================================
 * sem_post
 * ============================================================================ */
TEST_SUITE(sem_post);

#if SEM_POSIX

TEST(sem_post_bad) {
	errno = 0; ASSERT_EQ(-1, sem_post(NULL)); ASSERT_EQ(EINVAL, errno);
	sem_t s = SEM_FAILED;
	errno = 0; ASSERT_EQ(-1, sem_post(&s));   ASSERT_EQ(EINVAL, errno);
}

TEST(sem_post_increments) {
	sem_t s; sem_init(&s, 0, 0);
	sem_post(&s); sem_post(&s); sem_post(&s);
	int v; sem_getvalue(&s, &v); ASSERT_EQ(3, v);
	sem_destroy(&s);
}

TEST(sem_post_wakes_waiter) {
	sem_t s; sem_init(&s, 0, 0);
	pthread_t t; pthread_create(&t, NULL, __waiter, &s);
	usleep(50000);
	ASSERT_EQ(0, sem_post(&s));
	pthread_join(t, NULL);
	int v; sem_getvalue(&s, &v); ASSERT_EQ(0, v);
	sem_destroy(&s);
}

#endif

/* ============================================================================
 * sem_getvalue
 * ============================================================================ */
TEST_SUITE(sem_getvalue);

#if SEM_POSIX

TEST(sem_getvalue_bad) {
	int v;
	errno = 0; ASSERT_EQ(-1, sem_getvalue(NULL, &v)); ASSERT_EQ(EINVAL, errno);
	sem_t s; sem_init(&s, 0, 1);
	errno = 0; ASSERT_EQ(-1, sem_getvalue(&s, NULL)); ASSERT_EQ(EINVAL, errno);
	sem_t f = SEM_FAILED;
	errno = 0; ASSERT_EQ(-1, sem_getvalue(&f, &v));   ASSERT_EQ(EINVAL, errno);
	sem_destroy(&s);
}

TEST(sem_getvalue_reflects) {
	sem_t s; sem_init(&s, 0, 3);
	int v;
	sem_getvalue(&s, &v); ASSERT_EQ(3, v);
	sem_wait(&s);
	sem_getvalue(&s, &v); ASSERT_EQ(2, v);
	sem_post(&s); sem_post(&s);
	sem_getvalue(&s, &v); ASSERT_EQ(4, v);
	sem_destroy(&s);
}

#endif

/* ============================================================================
 * semget (System V)
 * ============================================================================ */
TEST_SUITE(semget);

#if SEM_POSIX

TEST(semget_bad_nsems) {
	errno = 0; ASSERT_EQ(-1, semget(1, -1, IPC_CREAT));  ASSERT_EQ(EINVAL, errno);
	errno = 0; ASSERT_EQ(-1, semget(1, 9999, IPC_CREAT)); ASSERT_EQ(EINVAL, errno);
}

TEST(semget_private) {
	int a = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);
	int b = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);
	ASSERT_TRUE(a >= 0 && b >= 0);
	ASSERT_NE(a, b);
	semctl(a, 0, IPC_RMID); semctl(b, 0, IPC_RMID);
}

TEST(semget_existing_key) {
	int a = semget(0xA001, 2, IPC_CREAT | 0666);
	int b = semget(0xA001, 2, IPC_CREAT | 0666);
	ASSERT_EQ(a, b);
	semctl(a, 0, IPC_RMID);
}

TEST(semget_excl) {
	int a = semget(0xA002, 2, IPC_CREAT | 0666);
	errno = 0;
	ASSERT_EQ(-1, semget(0xA002, 2, IPC_CREAT | IPC_EXCL | 0666));
	ASSERT_EQ(EEXIST, errno);
	semctl(a, 0, IPC_RMID);
}

TEST(semget_no_such) {
	errno = 0;
	ASSERT_EQ(-1, semget(0xF000, 1, 0666));
	ASSERT_EQ(ENOENT, errno);
}

TEST(semget_init_zero) {
	int id = semget(0xA003, 5, IPC_CREAT | 0666);
	for (int i = 0; i < 5; i++) ASSERT_EQ(0, semctl(id, i, GETVAL));
	semctl(id, 0, IPC_RMID);
}

#endif

/* ============================================================================
 * semop (System V)
 * ============================================================================ */
TEST_SUITE(semop);

#if SEM_POSIX

TEST(semop_bad) {
	struct sembuf sb = {0, 1, 0};
	errno = 0; ASSERT_EQ(-1, semop(-1, &sb, 1)); ASSERT_EQ(EINVAL, errno);
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	errno = 0; ASSERT_EQ(-1, semop(id, NULL, 1)); ASSERT_EQ(EINVAL, errno);
	errno = 0; ASSERT_EQ(-1, semop(id, &sb, 0));  ASSERT_EQ(EINVAL, errno);
	semctl(id, 0, IPC_RMID);
}

TEST(semop_post_wait) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	struct sembuf p = {0, 5, 0}; semop(id, &p, 1);
	ASSERT_EQ(5, semctl(id, 0, GETVAL));
	struct sembuf w = {0, -2, 0}; semop(id, &w, 1);
	ASSERT_EQ(3, semctl(id, 0, GETVAL));
	semctl(id, 0, IPC_RMID);
}

TEST(semop_nowait) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	struct sembuf sb = {0, -1, IPC_NOWAIT};
	errno = 0;
	ASSERT_EQ(-1, semop(id, &sb, 1));
	ASSERT_EQ(EAGAIN, errno);
	semctl(id, 0, IPC_RMID);
}

TEST(semop_wait_for_zero) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	struct sembuf ok = {0, 0, IPC_NOWAIT};
	ASSERT_EQ(0, semop(id, &ok, 1));
	struct sembuf p = {0, 3, 0}; semop(id, &p, 1);
	struct sembuf fail = {0, 0, IPC_NOWAIT};
	errno = 0;
	ASSERT_EQ(-1, semop(id, &fail, 1));
	ASSERT_EQ(EAGAIN, errno);
	semctl(id, 0, IPC_RMID);
}

TEST(semop_multi_atomic) {
	int id = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);
	unsigned short init[3] = {10, 10, 10};
	union semun u; u.array = init; semctl(id, 0, SETALL, u);
	struct sembuf sbs[3] = {{0, -3, 0}, {1, 5, 0}, {2, -1, 0}};
	ASSERT_EQ(0, semop(id, sbs, 3));
	ASSERT_EQ(7, semctl(id, 0, GETVAL));
	ASSERT_EQ(15, semctl(id, 1, GETVAL));
	ASSERT_EQ(9, semctl(id, 2, GETVAL));
	semctl(id, 0, IPC_RMID);
}

TEST(semop_undo_restores_on_exit) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	union semun u; u.val = 10; semctl(id, 0, SETVAL, u);

	pid_t pid = fork();
	if (pid == 0) {
		struct sembuf sb = {0, -3, SEM_UNDO};
		semop(id, &sb, 1);
		_exit(0);
	}
	int st; waitpid(pid, &st, 0);
	/* parent should see value restored to 10 */
	ASSERT_EQ(10, semctl(id, 0, GETVAL));
	semctl(id, 0, IPC_RMID);
}

#endif

/* ============================================================================
 * semctl (System V)
 * ============================================================================ */
TEST_SUITE(semctl);

#if SEM_POSIX

TEST(semctl_bad) {
	union semun u; u.val = 0;
	errno = 0; ASSERT_EQ(-1, semctl(-1, 0, GETVAL, u)); ASSERT_EQ(EINVAL, errno);
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	errno = 0; ASSERT_EQ(-1, semctl(id, 0, 9999, u));   ASSERT_EQ(EINVAL, errno);
	semctl(id, 0, IPC_RMID);
}

TEST(semctl_setval_getval) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	union semun u; u.val = 42;
	ASSERT_EQ(0, semctl(id, 0, SETVAL, u));
	ASSERT_EQ(42, semctl(id, 0, GETVAL));
	errno = 0;
	ASSERT_EQ(-1, semctl(id, 99, SETVAL, u));  ASSERT_EQ(EINVAL, errno);
	ASSERT_EQ(-1, semctl(id, 99, GETVAL));     ASSERT_EQ(EINVAL, errno);
	semctl(id, 0, IPC_RMID);
}

TEST(semctl_setall_getall) {
	int id = semget(IPC_PRIVATE, 4, IPC_CREAT | 0666);
	unsigned short in[4] = {10, 20, 30, 40};
	union semun u; u.array = in; semctl(id, 0, SETALL, u);
	unsigned short out[4] = {0}; u.array = out;
	ASSERT_EQ(0, semctl(id, 0, GETALL, u));
	ASSERT_EQ(10, out[0]); ASSERT_EQ(20, out[1]);
	ASSERT_EQ(30, out[2]); ASSERT_EQ(40, out[3]);
	union semun n; n.array = NULL;
	errno = 0; ASSERT_EQ(-1, semctl(id, 0, GETALL, n)); ASSERT_EQ(EFAULT, errno);
	errno = 0; ASSERT_EQ(-1, semctl(id, 0, SETALL, n)); ASSERT_EQ(EFAULT, errno);
	semctl(id, 0, IPC_RMID);
}

TEST(semctl_stat_set) {
	int id = semget(0xB001, 3, IPC_CREAT | 0600);
	struct semid_ds ds;
	union semun u; u.buf = &ds;
	ASSERT_EQ(0, semctl(id, 0, IPC_STAT, u));
	ASSERT_EQ(3, ds.sem_nsems);
	ASSERT_EQ(getuid(), ds.sem_perm.uid);
	ds.sem_perm.mode = 0644;
	ASSERT_EQ(0, semctl(id, 0, IPC_SET, u));
	union semun n; n.buf = NULL;
	errno = 0; ASSERT_EQ(-1, semctl(id, 0, IPC_STAT, n)); ASSERT_EQ(EFAULT, errno);
	errno = 0; ASSERT_EQ(-1, semctl(id, 0, IPC_SET, n));  ASSERT_EQ(EFAULT, errno);
	semctl(id, 0, IPC_RMID);
}

TEST(semctl_rmid) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	ASSERT_EQ(0, semctl(id, 0, IPC_RMID));
	union semun u; u.val = 0;
	errno = 0;
	ASSERT_EQ(-1, semctl(id, 0, GETVAL, u));
	ASSERT_EQ(EINVAL, errno);
}

TEST(semctl_getpid_after_op) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	ASSERT_EQ(0, semctl(id, 0, GETPID));
	struct sembuf sb = {0, 1, 0}; semop(id, &sb, 1);
	ASSERT_EQ((int)getpid(), semctl(id, 0, GETPID));
	errno = 0;
	ASSERT_EQ(-1, semctl(id, 99, GETPID));
	ASSERT_EQ(EINVAL, errno);
	semctl(id, 0, IPC_RMID);
}

TEST(semctl_getncnt_tracks_waiters) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	ASSERT_EQ(0, semctl(id, 0, GETNCNT));
	pthread_t t[3];
	for (int i = 0; i < 3; i++) pthread_create(&t[i], NULL, __sysv_waiter, (void*)(intptr_t)id);
	usleep(100000);
	ASSERT_EQ(3, semctl(id, 0, GETNCNT));
	struct sembuf sb = {0, 3, 0}; semop(id, &sb, 1);
	for (int i = 0; i < 3; i++) pthread_join(t[i], NULL);
	ASSERT_EQ(0, semctl(id, 0, GETNCNT));
	semctl(id, 0, IPC_RMID);
}

TEST(semctl_getzcnt_tracks_waiters) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	union semun u; u.val = 5; semctl(id, 0, SETVAL, u);
	ASSERT_EQ(0, semctl(id, 0, GETZCNT));
	pthread_t t[2];
	for (int i = 0; i < 2; i++) pthread_create(&t[i], NULL, __sysv_zcnt_waiter, (void*)(intptr_t)id);
	usleep(100000);
	ASSERT_EQ(2, semctl(id, 0, GETZCNT));
	struct sembuf sb = {0, -5, 0}; semop(id, &sb, 1);
	for (int i = 0; i < 2; i++) pthread_join(t[i], NULL);
	ASSERT_EQ(0, semctl(id, 0, GETZCNT));
	semctl(id, 0, IPC_RMID);
}

#endif

/* ============================================================================
 * NON-POSIX FALLBACK
 * ============================================================================ */
#if !SEM_POSIX

TEST(sem_enosys_all) {
	sem_t s;
	ASSERT_EQ(SEM_FAILED, sem_open("/t", O_CREAT, 0644, 1));
	ASSERT_EQ(-1, sem_close(s));
	ASSERT_EQ(-1, sem_unlink("/t"));
	ASSERT_EQ(-1, sem_init(&s, 0, 1));
	ASSERT_EQ(-1, sem_destroy(&s));
	ASSERT_EQ(-1, sem_wait(&s));
	ASSERT_EQ(-1, sem_trywait(&s));
	ASSERT_EQ(-1, sem_post(&s));
	int v;
	ASSERT_EQ(-1, sem_getvalue(&s, &v));
	ASSERT_EQ(-1, semget(0, 1, 0));
	ASSERT_EQ(-1, semop(0, NULL, 0));
	union semun u; u.val = 0;
	ASSERT_EQ(-1, semctl(0, 0, 0, u));
}

#endif

TEST_MAIN()
