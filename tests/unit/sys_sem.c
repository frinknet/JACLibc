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
	struct sembuf sb = {0, 0, 0};
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
TEST_SUITE(constants);

TEST(constants_ipc) {
	ASSERT_EQ(01000, IPC_CREAT);
	ASSERT_EQ(02000, IPC_EXCL);
	ASSERT_EQ(04000, IPC_NOWAIT);
	ASSERT_EQ(0, IPC_RMID);
	ASSERT_EQ(1, IPC_SET);
	ASSERT_EQ(2, IPC_STAT);
	ASSERT_EQ(0, IPC_PRIVATE);
}

TEST(constants_sem) {
	ASSERT_EQ(-1, SEM_FAILED);
	ASSERT_EQ(0x1000, SEM_UNDO);
}

TEST(constants_getset) {
	ASSERT_EQ(12, GETVAL);
	ASSERT_EQ(16, SETVAL);
	ASSERT_EQ(13, GETALL);
	ASSERT_EQ(17, SETALL);
	ASSERT_EQ(11, GETPID);
	ASSERT_EQ(14, GETNCNT);
	ASSERT_EQ(15, GETZCNT);
}

/* ============================================================================
 * sem_init_struct
 * ============================================================================ */
TEST_SUITE(sem_init_struct);

TEST(sem_init_struct_null) { sem_init_struct(NULL); }

TEST(sem_init_struct_sets_failed) {
	sem_t s = 42;
	sem_init_struct(&s);
	ASSERT_EQ(SEM_FAILED, s);
}

TEST(sem_init_struct_idempotent) {
	sem_t s = 999;
	sem_init_struct(&s);
	ASSERT_EQ(SEM_FAILED, s);
	sem_init_struct(&s);
	ASSERT_EQ(SEM_FAILED, s);
}

/* ============================================================================
 * sem_predicates
 * ============================================================================ */
TEST_SUITE(sem_predicates);

#if SEM_POSIX

TEST(sem_predicates_invalid_inputs) {
	ASSERT_FALSE(sem_is_valid(NULL));
	ASSERT_FALSE(sem_is_anonymous(NULL));
	ASSERT_FALSE(sem_is_named(NULL));
	ASSERT_EQ(0, sem_is_unlinked(NULL));
	sem_t f = SEM_FAILED;
	ASSERT_FALSE(sem_is_valid(&f));
	ASSERT_FALSE(sem_is_anonymous(&f));
	ASSERT_FALSE(sem_is_named(&f));
}

TEST(sem_predicates_anonymous) {
	sem_t s; sem_init(&s, 0, 1);
	ASSERT_TRUE(sem_is_valid(&s));
	ASSERT_TRUE(sem_is_anonymous(&s));
	ASSERT_FALSE(sem_is_named(&s));
	ASSERT_EQ(0, sem_is_unlinked(&s));
	sem_destroy(&s);
	ASSERT_FALSE(sem_is_valid(&s));
}

TEST(sem_predicates_named) {
	char n[64]; __make_name(n, sizeof(n), "pr");
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	ASSERT_TRUE(sem_is_valid(&s));
	ASSERT_TRUE(sem_is_named(&s));
	ASSERT_FALSE(sem_is_anonymous(&s));
	ASSERT_EQ(0, sem_is_unlinked(&s));
	sem_unlink(n);
	ASSERT_EQ(1, sem_is_unlinked(&s));
	sem_close(s);
}

TEST(sem_predicates_after_destroy) {
	sem_t s; sem_init(&s, 0, 1);
	sem_destroy(&s);
	ASSERT_FALSE(sem_is_valid(&s));
	ASSERT_FALSE(sem_is_anonymous(&s));
	ASSERT_FALSE(sem_is_named(&s));
	ASSERT_EQ(0, sem_is_unlinked(&s));
}

#endif

/* ============================================================================
 * sem_inspectors
 * ============================================================================ */
TEST_SUITE(sem_inspectors);

#if SEM_POSIX

TEST(sem_inspectors_invalid) {
	ASSERT_NULL(sem_get_name(NULL));
	ASSERT_EQ(-1, sem_get_refcount(NULL));
	sem_t f = SEM_FAILED;
	ASSERT_NULL(sem_get_name(&f));
	ASSERT_EQ(-1, sem_get_refcount(&f));
	sem_t a; sem_init(&a, 0, 1);
	ASSERT_NULL(sem_get_name(&a));
	ASSERT_EQ(1, sem_get_refcount(&a));
	sem_destroy(&a);
}

TEST(sem_inspectors_named) {
	char n[64]; __make_name(n, sizeof(n), "gi");
	sem_t s1 = sem_open(n, O_CREAT, 0644, 1);
	ASSERT_STR_EQ(n, sem_get_name(&s1));
	ASSERT_EQ(1, sem_get_refcount(&s1));
	sem_t s2 = sem_open(n, 0);
	ASSERT_EQ(2, sem_get_refcount(&s1));
	ASSERT_EQ(2, sem_get_refcount(&s2));
	sem_close(s1);
	ASSERT_EQ(1, sem_get_refcount(&s2));
	sem_close(s2); sem_unlink(n);
}

TEST(sem_inspectors_anon_always_one) {
	sem_t s; sem_init(&s, 0, 1);
	ASSERT_EQ(1, sem_get_refcount(&s));
	sem_destroy(&s);
}

TEST(sem_inspectors_name_after_unlink) {
	char n[64]; __make_name(n, sizeof(n), "gnu");
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	sem_unlink(n);
	const char *got = sem_get_name(&s);
	ASSERT_NOT_NULL(got);
	ASSERT_STR_EQ(n, got);
	sem_close(s);
}

#endif

/* ============================================================================
 * sem_open
 * ============================================================================ */
TEST_SUITE(sem_open);

#if SEM_POSIX

TEST(sem_open_bad_name) {
	errno = 0; ASSERT_EQ(SEM_FAILED, sem_open(NULL, O_CREAT, 0644, 1)); ASSERT_EQ(EINVAL, errno);
	errno = 0; ASSERT_EQ(SEM_FAILED, sem_open("x", O_CREAT, 0644, 1));  ASSERT_EQ(EINVAL, errno);
	errno = 0; ASSERT_EQ(SEM_FAILED, sem_open("/", O_CREAT, 0644, 1));  ASSERT_EQ(EINVAL, errno);
	errno = 0; ASSERT_EQ(SEM_FAILED, sem_open("/x", O_EXCL, 0644, 1));  ASSERT_EQ(EINVAL, errno);
}

TEST(sem_open_no_such) {
	errno = 0;
	ASSERT_EQ(SEM_FAILED, sem_open("/nonexistent_99", 0));
	ASSERT_EQ(ENOENT, errno);
}

TEST(sem_open_create_basic) {
	char n[64]; __make_name(n, sizeof(n), "oc");
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	ASSERT_NE(SEM_FAILED, s);
	ASSERT_TRUE(sem_is_named(&s));
	ASSERT_STR_EQ(n, sem_get_name(&s));
	int v; sem_getvalue(&s, &v); ASSERT_EQ(1, v);
	sem_close(s); sem_unlink(n);
}

TEST(sem_open_create_zero) {
	char n[64]; __make_name(n, sizeof(n), "oz");
	sem_t s = sem_open(n, O_CREAT, 0644, 0);
	int v; sem_getvalue(&s, &v); ASSERT_EQ(0, v);
	sem_close(s); sem_unlink(n);
}

TEST(sem_open_excl_existing) {
	char n[64]; __make_name(n, sizeof(n), "oe");
	sem_t s1 = sem_open(n, O_CREAT, 0644, 1);
	errno = 0;
	ASSERT_EQ(SEM_FAILED, sem_open(n, O_CREAT | O_EXCL, 0644, 1));
	ASSERT_EQ(EEXIST, errno);
	sem_close(s1); sem_unlink(n);
}

TEST(sem_open_reopen_shares_state) {
	char n[64]; __make_name(n, sizeof(n), "or");
	sem_t s1 = sem_open(n, O_CREAT, 0644, 5);
	sem_wait(&s1);
	sem_t s2 = sem_open(n, 0);
	int v; sem_getvalue(&s2, &v);
	ASSERT_EQ(4, v);
	ASSERT_EQ(2, sem_get_refcount(&s1));
	sem_close(s1);
	ASSERT_EQ(1, sem_get_refcount(&s2));
	sem_close(s2); sem_unlink(n);
}

TEST(sem_open_reopen_after_close) {
	char n[64]; __make_name(n, sizeof(n), "ora");
	sem_t s1 = sem_open(n, O_CREAT, 0644, 1);
	sem_close(s1);
	sem_t s2 = sem_open(n, 0);
	ASSERT_NE(SEM_FAILED, s2);
	int v; sem_getvalue(&s2, &v); ASSERT_EQ(1, v);
	sem_close(s2); sem_unlink(n);
	errno = 0;
	ASSERT_EQ(SEM_FAILED, sem_open(n, 0));
	ASSERT_EQ(ENOENT, errno);
}

TEST(sem_open_after_unlink_creates_new) {
	char n[64]; __make_name(n, sizeof(n), "oun");
	sem_t s1 = sem_open(n, O_CREAT, 0644, 5);
	sem_t s2 = sem_open(n, 0);
	sem_unlink(n);
	sem_t s3 = sem_open(n, O_CREAT, 0644, 99);
	ASSERT_NE(SEM_FAILED, s3);
	ASSERT_NE(s1, s3);
	int v;
	sem_getvalue(&s1, &v); ASSERT_EQ(5, v);
	sem_getvalue(&s3, &v); ASSERT_EQ(99, v);
	sem_close(s1);
	sem_close(s2);
	sem_wait(&s3);
	sem_getvalue(&s3, &v); ASSERT_EQ(98, v);
	sem_close(s3);
	sem_unlink(n);
}

TEST(sem_open_long_name) {
	char n[200];
	snprintf(n, sizeof(n), "/long_name_%d", (int)getpid());
	for (size_t i = strlen(n); i < 150; i++) n[i] = 'x';
	n[150] = '\0';
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	ASSERT_NE(SEM_FAILED, s);
	sem_close(s);
	sem_unlink(n);
}

TEST(sem_open_max_length_name) {
	char n[256];
	n[0] = '/';
	for (int i = 1; i < 255; i++) n[i] = 'a';
	n[255] = '\0';
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	ASSERT_NE(SEM_FAILED, s);
	sem_close(s);
	sem_unlink(n);
}

TEST(sem_open_value_independent_of_opens) {
	char n[64]; __make_name(n, sizeof(n), "vind");
	sem_t handles[10];
	for (int i = 0; i < 10; i++) {
		handles[i] = sem_open(n, O_CREAT, 0644, 100);
		ASSERT_NE(SEM_FAILED, handles[i]);
	}
	sem_wait(&handles[0]);
	sem_wait(&handles[5]);
	int v;
	sem_getvalue(&handles[9], &v);
	ASSERT_EQ(98, v);
	for (int i = 0; i < 10; i++) sem_close(handles[i]);
	sem_unlink(n);
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
	errno = 0; ASSERT_EQ(-1, sem_unlink(NULL));           ASSERT_EQ(EINVAL, errno);
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

TEST(sem_unlink_while_two_refs) {
	char n[64]; __make_name(n, sizeof(n), "utr");
	sem_t s1 = sem_open(n, O_CREAT, 0644, 1);
	sem_t s2 = sem_open(n, 0);
	ASSERT_EQ(0, sem_unlink(n));
	sem_post(&s1);
	int v; sem_getvalue(&s2, &v);
	ASSERT_EQ(2, v);
	errno = 0;
	ASSERT_EQ(SEM_FAILED, sem_open(n, 0));
	ASSERT_EQ(ENOENT, errno);
	sem_close(s1);
	sem_getvalue(&s2, &v);
	ASSERT_EQ(2, v);
	sem_close(s2);
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
	sem_t s; int v;
	ASSERT_EQ(0, sem_init(&s, 0, 5));
	ASSERT_TRUE(sem_is_anonymous(&s));
	sem_getvalue(&s, &v); ASSERT_EQ(5, v);
	sem_destroy(&s);

	ASSERT_EQ(0, sem_init(&s, 0, 0));
	sem_getvalue(&s, &v); ASSERT_EQ(0, v);
	sem_destroy(&s);

	ASSERT_EQ(0, sem_init(&s, 0, 0x7FFFFFFF));
	sem_getvalue(&s, &v); ASSERT_EQ(0x7FFFFFFF, v);
	sem_destroy(&s);
}

TEST(sem_init_pshared) {
	sem_t s;
	ASSERT_EQ(0, sem_init(&s, 0, 1));
	sem_destroy(&s);
	ASSERT_EQ(0, sem_init(&s, 1, 1));
	sem_destroy(&s);
}

TEST(sem_init_reinit_after_destroy) {
	sem_t s; int v;
	sem_init(&s, 0, 1); sem_destroy(&s);
	sem_init(&s, 0, 7);
	sem_getvalue(&s, &v); ASSERT_EQ(7, v);
	sem_destroy(&s);
}

TEST(sem_init_many_slots) {
	sem_t arr[50];
	for (int i = 0; i < 50; i++) {
		ASSERT_EQ(0, sem_init(&arr[i], 0, i));
		int v; sem_getvalue(&arr[i], &v);
		ASSERT_EQ(i, v);
	}
	for (int i = 0; i < 50; i++) {
		ASSERT_EQ(0, sem_destroy(&arr[i]));
	}
}

TEST(sem_init_max_value) {
	sem_t s;
	ASSERT_EQ(0, sem_init(&s, 0, UINT_MAX));
	sem_destroy(&s);
}

TEST(sem_init_resource_limit) {
	sem_t slots[1024];
	int ok = 0;
	for (int i = 0; i < 1024; i++) {
		if (sem_init(&slots[i], 0, 1) == 0) ok++;
		else break;
	}
	ASSERT_EQ(1024, ok);
	sem_t extra;
	errno = 0;
	ASSERT_EQ(-1, sem_init(&extra, 0, 1));
	ASSERT_EQ(ENOSPC, errno);
	for (int i = 0; i < ok; i++) sem_destroy(&slots[i]);
	ASSERT_EQ(0, sem_init(&extra, 0, 1));
	sem_destroy(&extra);
}

#endif

/* ============================================================================
 * sem_destroy
 * ============================================================================ */
TEST_SUITE(sem_destroy);

#if SEM_POSIX

TEST(sem_destroy_bad) {
	errno = 0; ASSERT_EQ(-1, sem_destroy(NULL)); ASSERT_EQ(EINVAL, errno);
	sem_t s = SEM_FAILED;
	errno = 0; ASSERT_EQ(-1, sem_destroy(&s));   ASSERT_EQ(EINVAL, errno);
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

TEST(sem_destroy_double) {
	sem_t s; sem_init(&s, 0, 1);
	ASSERT_EQ(0, sem_destroy(&s));
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

TEST(sem_wait_post_race) {
	sem_t s; sem_init(&s, 0, 0);
	pthread_t w[8], p[8];
	for (int i = 0; i < 8; i++) pthread_create(&w[i], NULL, __waiter, &s);
	usleep(30000);
	for (int i = 0; i < 8; i++) pthread_create(&p[i], NULL, __poster_delayed, &s);
	for (int i = 0; i < 8; i++) pthread_join(w[i], NULL);
	for (int i = 0; i < 8; i++) pthread_join(p[i], NULL);
	int v; sem_getvalue(&s, &v);
	ASSERT_EQ(0, v);
	sem_destroy(&s);
}

TEST(sem_wait_named) {
	char n[64]; __make_name(n, sizeof(n), "wn");
	sem_t s = sem_open(n, O_CREAT, 0644, 1);
	ASSERT_EQ(0, sem_wait(&s));
	int v; sem_getvalue(&s, &v);
	ASSERT_EQ(0, v);
	sem_close(s); sem_unlink(n);
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
	sem_trywait(&s);
	errno = 0;
	ASSERT_EQ(-1, sem_trywait(&s));
	ASSERT_EQ(EAGAIN, errno);
	sem_destroy(&s);
}

TEST(sem_trywait_then_post_then_trywait) {
	sem_t s; sem_init(&s, 0, 0);
	errno = 0;
	ASSERT_EQ(-1, sem_trywait(&s));
	ASSERT_EQ(EAGAIN, errno);
	sem_post(&s);
	ASSERT_EQ(0, sem_trywait(&s));
	sem_destroy(&s);
}

TEST(sem_trywait_storm) {
	sem_t s; sem_init(&s, 0, 4);
	_Atomic int success = 0;
	for (int i = 0; i < 8; i++) {
		if (sem_trywait(&s) == 0) atomic_fetch_add(&success, 1);
	}
	ASSERT_EQ(4, atomic_load(&success));
	int v; sem_getvalue(&s, &v);
	ASSERT_EQ(0, v);
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

TEST(sem_timedwait_nsec_zero_valid) {
	sem_t s; sem_init(&s, 0, 1);
	struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_nsec = 0;
	ASSERT_EQ(0, sem_timedwait(&s, &ts));
	sem_destroy(&s);
}

TEST(sem_timedwait_nsec_max_valid) {
	sem_t s; sem_init(&s, 0, 1);
	struct timespec ts; clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_nsec = 999999999L;
	ASSERT_EQ(0, sem_timedwait(&s, &ts));
	sem_destroy(&s);
}

TEST(sem_timedwait_wakes_at_exact_deadline) {
	sem_t s; sem_init(&s, 0, 0);
	struct timespec past; clock_gettime(CLOCK_REALTIME, &past); past.tv_sec -= 1;
	errno = 0;
	ASSERT_EQ(-1, sem_timedwait(&s, &past));
	ASSERT_EQ(ETIMEDOUT, errno);
	sem_post(&s);
	ASSERT_EQ(0, sem_timedwait(&s, &past));
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

TEST(sem_post_many) {
	sem_t s; sem_init(&s, 0, 0);
	for (int i = 0; i < 100; i++) sem_post(&s);
	int v; sem_getvalue(&s, &v);
	ASSERT_EQ(100, v);
	sem_destroy(&s);
}

TEST(sem_post_overflow_wraps) {
	sem_t s; sem_init(&s, 0, UINT_MAX - 1);
	sem_post(&s);
	sem_post(&s);
	errno = 0;
	ASSERT_EQ(-1, sem_trywait(&s));
	ASSERT_EQ(EAGAIN, errno);
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
	sem_t s; sem_init(&s, 0, 3); int v;
	sem_getvalue(&s, &v); ASSERT_EQ(3, v);
	sem_wait(&s);
	sem_getvalue(&s, &v); ASSERT_EQ(2, v);
	sem_post(&s); sem_post(&s);
	sem_getvalue(&s, &v); ASSERT_EQ(4, v);
	sem_destroy(&s);
}

TEST(sem_getvalue_after_destroy) {
	sem_t s; sem_init(&s, 0, 7);
	sem_destroy(&s);
	int v;
	errno = 0;
	ASSERT_EQ(-1, sem_getvalue(&s, &v));
	ASSERT_EQ(EINVAL, errno);
}

TEST(sem_getvalue_interleaved) {
	sem_t s; sem_init(&s, 0, 100);
	int v;
	for (int i = 0; i < 100; i++) {
		sem_getvalue(&s, &v);
		ASSERT_EQ(100 - i, v);
		sem_wait(&s);
	}
	sem_getvalue(&s, &v);
	ASSERT_EQ(0, v);
	sem_destroy(&s);
}

#endif

/* ============================================================================
 * semget
 * ============================================================================ */
TEST_SUITE(semget);

#if SEM_POSIX

TEST(semget_bad_nsems) {
	errno = 0; ASSERT_EQ(-1, semget(1, -1, IPC_CREAT));    ASSERT_EQ(EINVAL, errno);
	errno = 0; ASSERT_EQ(-1, semget(1, 9999, IPC_CREAT));  ASSERT_EQ(EINVAL, errno);
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

TEST(semget_init_zero_and_perms) {
	int id = semget(0xA003, 5, IPC_CREAT | 0640);
	struct semid_ds ds;
	union semun u; u.buf = &ds;
	ASSERT_EQ(0, semctl(id, 0, IPC_STAT, u));
	ASSERT_EQ(5, ds.sem_nsems);
	ASSERT_EQ(0640, (int)(ds.sem_perm.mode & 0777));
	for (int i = 0; i < 5; i++) ASSERT_EQ(0, semctl(id, i, GETVAL));
	semctl(id, 0, IPC_RMID);
}

TEST(semget_nsems_zero_existing) {
	int a = semget(0xC001, 3, IPC_CREAT | 0666);
	ASSERT_TRUE(a >= 0);
	int b = semget(0xC001, 0, 0);
	ASSERT_EQ(a, b);
	semctl(a, 0, IPC_RMID);
}

TEST(semget_key_reuse_after_rmid) {
	int a = semget(0xC002, 2, IPC_CREAT | 0666);
	semctl(a, 0, IPC_RMID);
	int b = semget(0xC002, 2, IPC_CREAT | 0666);
	ASSERT_TRUE(b >= 0);
	semctl(b, 0, IPC_RMID);
}

TEST(semget_open_no_creat_existing) {
	int a = semget(0xC003, 2, IPC_CREAT | 0666);
	int b = semget(0xC003, 0, 0);
	ASSERT_EQ(a, b);
	semctl(a, 0, IPC_RMID);
}

TEST(semget_set_limit) {
	int ids[64];
	int ok = 0;
	for (int i = 0; i < 64; i++) {
		ids[i] = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
		if (ids[i] >= 0) ok++;
		else break;
	}
	ASSERT_EQ(64, ok);
	errno = 0;
	ASSERT_EQ(-1, semget(IPC_PRIVATE, 1, IPC_CREAT | 0666));
	ASSERT_EQ(ENOSPC, errno);
	for (int i = 0; i < ok; i++) semctl(ids[i], 0, IPC_RMID);
	int fresh = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	ASSERT_TRUE(fresh >= 0);
	semctl(fresh, 0, IPC_RMID);
}

TEST(semget_sem_per_set_limit) {
	errno = 0;
	ASSERT_EQ(-1, semget(IPC_PRIVATE, 257, IPC_CREAT | 0666));
	ASSERT_EQ(EINVAL, errno);
	int id = semget(IPC_PRIVATE, 256, IPC_CREAT | 0666);
	ASSERT_TRUE(id >= 0);
	semctl(id, 0, IPC_RMID);
}

#endif

/* ============================================================================
 * semop
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

TEST(semop_invalid_sem_num) {
	int id = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
	struct sembuf sb = {5, 1, 0};
	errno = 0;
	ASSERT_EQ(-1, semop(id, &sb, 1));
	ASSERT_EQ(EFBIG, errno);
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

TEST(semop_atomic_no_partial_on_block) {
	int id = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
	union semun u; u.val = 5;
	semctl(id, 0, SETVAL, u);
	semctl(id, 1, SETVAL, u);
	struct sembuf sbs[2] = {
		{0, -3, 0},
		{1, -10, IPC_NOWAIT}
	};
	errno = 0;
	ASSERT_EQ(-1, semop(id, sbs, 2));
	ASSERT_EQ(EAGAIN, errno);
	ASSERT_EQ(5, semctl(id, 0, GETVAL));
	ASSERT_EQ(5, semctl(id, 1, GETVAL));
	semctl(id, 0, IPC_RMID);
}

TEST(semop_undo_accumulates) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	union semun u; u.val = 20; semctl(id, 0, SETVAL, u);
	pid_t pid = fork();
	if (pid == 0) {
		struct sembuf a = {0, -5, SEM_UNDO};
		struct sembuf b = {0, -3, SEM_UNDO};
		semop(id, &a, 1);
		semop(id, &b, 1);
		_exit(0);
	}
	int st; waitpid(pid, &st, 0);
	ASSERT_EQ(20, semctl(id, 0, GETVAL));
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
	ASSERT_EQ(10, semctl(id, 0, GETVAL));
	semctl(id, 0, IPC_RMID);
}

TEST(semop_decrement_too_much_nowait) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	union semun u; u.val = 5; semctl(id, 0, SETVAL, u);
	struct sembuf sb = {0, -100, IPC_NOWAIT};
	errno = 0;
	ASSERT_EQ(-1, semop(id, &sb, 1));
	ASSERT_EQ(EAGAIN, errno);
	ASSERT_EQ(5, semctl(id, 0, GETVAL));
	semctl(id, 0, IPC_RMID);
}

TEST(semop_two_ops_same_sem) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	union semun u; u.val = 10; semctl(id, 0, SETVAL, u);
	struct sembuf sbs[2] = {{0, 5, 0}, {0, -3, 0}};
	ASSERT_EQ(0, semop(id, sbs, 2));
	ASSERT_EQ(12, semctl(id, 0, GETVAL));
	semctl(id, 0, IPC_RMID);
}

TEST(semop_large_value) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	struct sembuf p = {0, 30000, 0};
	ASSERT_EQ(0, semop(id, &p, 1));
	ASSERT_EQ(30000, semctl(id, 0, GETVAL));
	struct sembuf w = {0, -29000, 0};
	ASSERT_EQ(0, semop(id, &w, 1));
	ASSERT_EQ(1000, semctl(id, 0, GETVAL));
	semctl(id, 0, IPC_RMID);
}

TEST(semop_wait_for_zero_wakes) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	union semun u; u.val = 5; semctl(id, 0, SETVAL, u);
	pthread_t t1, t2;
	pthread_create(&t1, NULL, __sysv_zcnt_waiter, (void*)(intptr_t)id);
	pthread_create(&t2, NULL, __sysv_zcnt_waiter, (void*)(intptr_t)id);
	usleep(50000);
	ASSERT_EQ(2, semctl(id, 0, GETZCNT));
	struct sembuf drain = {0, -5, 0};
	ASSERT_EQ(0, semop(id, &drain, 1));
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	ASSERT_EQ(0, semctl(id, 0, GETZCNT));
	semctl(id, 0, IPC_RMID);
}

TEST(semop_nowait_on_zero_op) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	struct sembuf sb = {0, 0, IPC_NOWAIT};
	ASSERT_EQ(0, semop(id, &sb, 1));
	semctl(id, 0, IPC_RMID);
}

TEST(semop_undo_across_fork) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	union semun u; u.val = 10; semctl(id, 0, SETVAL, u);
	pid_t pid = fork();
	if (pid == 0) {
		struct sembuf sb = {0, -3, SEM_UNDO};
		semop(id, &sb, 1);
		_exit(0);
	}
	int st; waitpid(pid, &st, 0);
	/* Child's UNDO cleanup runs when child exits, restoring value from 7 to 10 */
	ASSERT_EQ(10, semctl(id, 0, GETVAL));
	semctl(id, 0, IPC_RMID);
}

#endif

/* ============================================================================
 * semctl
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
	ASSERT_EQ(-1, semctl(id, 99, SETVAL, u)); ASSERT_EQ(EINVAL, errno);
	ASSERT_EQ(-1, semctl(id, 99, GETVAL));    ASSERT_EQ(EINVAL, errno);
	semctl(id, 0, IPC_RMID);
}

TEST(semctl_setval_max) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	union semun u; u.val = 0x7FFFFFFF;
	ASSERT_EQ(0, semctl(id, 0, SETVAL, u));
	ASSERT_EQ(0x7FFFFFFF, semctl(id, 0, GETVAL));
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
	ASSERT_EQ(0600, (int)(ds.sem_perm.mode & 0777));
	ds.sem_perm.mode = 0644;
	ASSERT_EQ(0, semctl(id, 0, IPC_SET, u));
	struct semid_ds ds2;
	u.buf = &ds2;
	semctl(id, 0, IPC_STAT, u);
	ASSERT_EQ(0644, (int)(ds2.sem_perm.mode & 0777));
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

TEST(semctl_setval_only_one_index) {
	int id = semget(IPC_PRIVATE, 3, IPC_CREAT | 0666);
	union semun u; u.val = 42;
	ASSERT_EQ(0, semctl(id, 1, SETVAL, u));
	ASSERT_EQ(0, semctl(id, 0, GETVAL));
	ASSERT_EQ(42, semctl(id, 1, GETVAL));
	ASSERT_EQ(0, semctl(id, 2, GETVAL));
	semctl(id, 0, IPC_RMID);
}

TEST(semctl_negative_semnum) {
	int id = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);
	union semun u; u.val = 0;
	errno = 0; ASSERT_EQ(-1, semctl(id, -1, GETVAL, u)); ASSERT_EQ(EINVAL, errno);
	errno = 0; ASSERT_EQ(-1, semctl(id, -1, SETVAL, u)); ASSERT_EQ(EINVAL, errno);
	errno = 0; ASSERT_EQ(-1, semctl(id, -1, GETPID));     ASSERT_EQ(EINVAL, errno);
	semctl(id, 0, IPC_RMID);
}

TEST(semctl_getval_all_indices) {
	int id = semget(IPC_PRIVATE, 5, IPC_CREAT | 0666);
	union semun u;
	for (int i = 0; i < 5; i++) {
		u.val = (i + 1) * 10;
		ASSERT_EQ(0, semctl(id, i, SETVAL, u));
	}
	for (int i = 0; i < 5; i++) {
		ASSERT_EQ((i + 1) * 10, semctl(id, i, GETVAL));
	}
	semctl(id, 0, IPC_RMID);
}

TEST(semctl_set_updates_ctime) {
	int id = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
	struct semid_ds ds1, ds2;
	union semun u; u.buf = &ds1;
	semctl(id, 0, IPC_STAT, u);
	sleep(1);
	struct semid_ds mod = ds1;
	mod.sem_perm.mode = 0644;
	u.buf = &mod;
	ASSERT_EQ(0, semctl(id, 0, IPC_SET, u));
	u.buf = &ds2;
	semctl(id, 0, IPC_STAT, u);
	ASSERT_TRUE(ds2.sem_ctime >= ds1.sem_ctime);
	semctl(id, 0, IPC_RMID);
}

TEST(semctl_setall_large) {
	int id = semget(IPC_PRIVATE, 256, IPC_CREAT | 0666);
	unsigned short vals[256];
	for (int i = 0; i < 256; i++) vals[i] = (unsigned short)(i * 100);
	union semun u; u.array = vals;
	ASSERT_EQ(0, semctl(id, 0, SETALL, u));
	unsigned short out[256];
	u.array = out;
	ASSERT_EQ(0, semctl(id, 0, GETALL, u));
	for (int i = 0; i < 256; i++) ASSERT_EQ((unsigned short)(i * 100), out[i]);
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
