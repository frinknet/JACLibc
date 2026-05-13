/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <pthread.h>
#include <signal.h>

TEST_TYPE(unit);
TEST_UNIT(aio.h);

#if JACL_HAS_POSIX

static int __test_create_temp_file(const char *prefix, const char *data, size_t len) {
	char path[256];
	snprintf(path, sizeof(path), "/tmp/%s_XXXXXX", prefix);
	int fd = mkstemp(path);
	if (fd < 0) return -1;
	if (data && len > 0) {
		ssize_t w = write(fd, data, len);
		if (w < 0) { close(fd); unlink(path); return -1; }
	}
	unlink(path);
	return fd;
}

static int __test_create_persistent_file(const char *prefix, const char *data, size_t len) {
	char path[256];
	snprintf(path, sizeof(path), "/tmp/%s_XXXXXX", prefix);
	int fd = mkstemp(path);
	if (fd < 0) return -1;
	if (data && len > 0) {
		write(fd, data, len);
	}
	/* Don't unlink - keep for duration of test */
	return fd;
}

/* Thread helper that tracks results */
struct __test_thread_arg {
	int fd;
	int offset;
	int *success_count;
	pthread_mutex_t *lock;
};

static void *__test_thread_aio_write(void *arg) {
	struct __test_thread_arg *targ = (struct __test_thread_arg *)arg;
	struct aiocb cb = {0};
	cb.aio_fildes = targ->fd;
	cb.aio_buf = (void *)"X";
	cb.aio_nbytes = 1;
	cb.aio_offset = targ->offset;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;

	int r = aio_write(&cb);
	if (r == 0) {
		int tries = 0;
		while (aio_error(&cb) == EINPROGRESS && tries++ < 100) usleep(1000);
		if (aio_error(&cb) == 0 && aio_return(&cb) == 1) {
			pthread_mutex_lock(targ->lock);
			(*targ->success_count)++;
			pthread_mutex_unlock(targ->lock);
		}
	}
	return NULL;
}

/* Signal handler for SIGEV_SIGNAL tests */
static volatile sig_atomic_t __test_signal_received = 0;
static void __test_signal_handler(int sig) {
	(void)sig;
	__test_signal_received = 1;
}

/* ============================================================================ */

TEST_SUITE(constants);

TEST(constants_aio_canceled) {
	ASSERT_EQ(1, AIO_CANCELED);
	ASSERT_EQ(2, AIO_NOTCANCELED);
	ASSERT_EQ(3, AIO_ALLDONE);
}

TEST(constants_lio_opcode) {
	ASSERT_EQ(0, LIO_READ);
	ASSERT_EQ(1, LIO_WRITE);
	ASSERT_EQ(2, LIO_NOP);
}

TEST(constants_lio_mode) {
	ASSERT_EQ(0, LIO_WAIT);
	ASSERT_EQ(1, LIO_NOWAIT);
}

/* ============================================================================ */

TEST_SUITE(aioinit);

TEST(aioinit_size_min) {
	ASSERT_SIZE_MIN(struct aioinit, sizeof(int) * 7);
}

TEST(aioinit_field_names) {
	struct aioinit init = {0};
	init.aio_threads = 4;
	init.aio_num = 8;
	init.aio_locks = 2;
	ASSERT_EQ(4, init.aio_threads);
	ASSERT_EQ(8, init.aio_num);
	ASSERT_EQ(2, init.aio_locks);
}

TEST(aioinit_zero_values) {
	struct aioinit init = {0};
	aio_init(&init);
	ASSERT_TRUE(1);
}

TEST(aioinit_negative_threads) {
	struct aioinit init = {0};
	init.aio_threads = -1;
	aio_init(&init);
	ASSERT_TRUE(1);
}

TEST(aioinit_large_threads) {
	struct aioinit init = {0};
	init.aio_threads = 10000;
	aio_init(&init);
	ASSERT_TRUE(1);
}

/* ============================================================================ */

TEST_SUITE(aiocb);

TEST(aiocb_size_min) {
	ASSERT_SIZE_MIN(struct aiocb, sizeof(int) * 3 + sizeof(void *) * 3 + sizeof(size_t) + sizeof(off_t));
}

TEST(aiocb_field_offsets) {
	ASSERT_OFFSET_PAST(struct aiocb, aio_fildes, 0);
	ASSERT_OFFSET_PAST(struct aiocb, aio_buf, offsetof(struct aiocb, aio_fildes));
	ASSERT_OFFSET_PAST(struct aiocb, aio_sigevent, offsetof(struct aiocb, aio_buf));
}

TEST(aiocb_reserved_fields) {
	struct aiocb cb = {0};
	cb.aio_fildes = 42;
	cb.__jacl_state = 99;
	ASSERT_EQ(42, cb.aio_fildes);
	ASSERT_EQ(99, cb.__jacl_state);
}

TEST(aiocb_null_buf_with_nbytes) {
	struct aiocb cb = {0};
	cb.aio_fildes = 1;
	cb.aio_buf = NULL;
	cb.aio_nbytes = 10;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_read(&cb);
	ASSERT_EQ(-1, r);
}

TEST(aiocb_zero_nbytes) {
	int fd = __test_create_temp_file("zero", "x", 1);
	ASSERT_GE(fd, 0);
	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = (void *)"test";
	cb.aio_nbytes = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_read(&cb);
	ASSERT_EQ(0, r);
	close(fd);
}

TEST(aiocb_negative_fd) {
	struct aiocb cb = {0};
	cb.aio_fildes = -1;
	cb.aio_buf = (void *)"test";
	cb.aio_nbytes = 4;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_read(&cb);
	ASSERT_EQ(-1, r);
}

TEST(aiocb_large_offset) {
	struct aiocb cb = {0};
	cb.aio_fildes = 1;
	cb.aio_buf = (void *)"test";
	cb.aio_nbytes = 4;
	cb.aio_offset = (off_t)SIZE_MAX;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_read(&cb);
	ASSERT_EQ(-1, r);
}

/* ============================================================================ */

TEST_SUITE(aio_init);

TEST(aio_init_null_ok) {
	aio_init(NULL);
	ASSERT_TRUE(1);
}

TEST(aio_init_empty_struct) {
	struct aioinit init = {0};
	aio_init(&init);
	ASSERT_TRUE(1);
}

TEST(aio_init_multiple_safe) {
	aio_init(NULL);
	aio_init(NULL);
	aio_init(NULL);
	ASSERT_TRUE(1);
}

TEST(aio_init_concurrent_threads) {
	/* Test race condition: multiple threads calling aio_init simultaneously */
	pthread_t threads[4];
	for (int i = 0; i < 4; i++) {
		pthread_create(&threads[i], NULL, (void *(*)(void *))aio_init, NULL);
	}
	for (int i = 0; i < 4; i++) {
		pthread_join(threads[i], NULL);
	}
	// this is a poor way to write a test
	ASSERT_TRUE(1);
}

TEST(aio_init_repeated) {
	for (int i = 0; i < 10; i++) {
		aio_init(NULL);
	}
	ASSERT_TRUE(1);
}

TEST(aio_init_extreme_thread_count) {
	struct aioinit init = {0};
	init.aio_threads = INT_MAX;  /* Extreme value */

	aio_init(&init);
	/* Should handle gracefully (clamp or ignore) */
	ASSERT_TRUE(1);
}

/* ============================================================================ */

TEST_SUITE(aio_destroy);

TEST(aio_destroy_no_crash) {
	aio_destroy();
	ASSERT_TRUE(1);
}

TEST(aio_destroy_multiple_safe) {
	aio_destroy();
	aio_destroy();
	aio_destroy();
	ASSERT_TRUE(1);
}

TEST(aio_destroy_after_init) {
	aio_init(NULL);
	aio_destroy();
	ASSERT_TRUE(1);
}

TEST(aio_destroy_before_init) {
	aio_destroy();
	ASSERT_TRUE(1);
}

TEST(aio_destroy_with_pending_ops) {
	int fd = __test_create_persistent_file("destroy_pending", "test", 4);
	ASSERT_GE(fd, 0);

	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = (void *)"test";
	cb.aio_nbytes = 4;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;

	aio_write(&cb);
	aio_destroy();
	close(fd);
	ASSERT_TRUE(1);
}

TEST(aio_destroy_cleanup_on_failure) {
	int fd = __test_create_temp_file("memory_fail", NULL, 0);
	ASSERT_GE(fd, 0);

	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = (void *)"";
	cb.aio_nbytes = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;

	aio_write(&cb);
	aio_destroy();
	close(fd);
	ASSERT_TRUE(1);
}

TEST(aio_destroy_during_suspend) {
	struct aiocb cb = {0};
	cb.__jacl_state = 0;  /* Pending */
	const struct aiocb *cbs[] = {&cb};

	/* Start suspend in background */
	pthread_t t;
	pthread_create(&t, NULL, (void *(*)(void *))aio_suspend, (void *)cbs);

	/* Destroy immediately */
	usleep(1000);
	aio_destroy();

	pthread_join(t, NULL);
	/* Should not crash */
	ASSERT_TRUE(1);
}

/* ============================================================================ */

TEST_SUITE(aio_read);

TEST(aio_read_valid_file) {
	const char *data = "Hello, AIO!";
	int fd = __test_create_temp_file("aio_read", data, strlen(data));
	ASSERT_GE(fd, 0);

	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = malloc(64);
	cb.aio_nbytes = strlen(data);
	cb.aio_offset = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;

	int r = aio_read(&cb);
	int tries = 0;
	while (aio_error(&cb) == EINPROGRESS && tries++ < 100) usleep(1000);

	ASSERT_EQ((ssize_t)strlen(data), aio_return(&cb));
	ASSERT_EQ(0, memcmp((const void *)cb.aio_buf, data, strlen(data)));

	free((void *)cb.aio_buf);
	close(fd);
}

TEST(aio_read_zero_bytes) {
	int fd = __test_create_temp_file("zero", "x", 1);
	ASSERT_GE(fd, 0);
	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = malloc(1);
	cb.aio_nbytes = 0;
	cb.aio_offset = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_read(&cb);
	ASSERT_EQ(0, r);
	free((void *)cb.aio_buf);
	close(fd);
}

TEST(aio_read_null_cb) {
	ASSERT_EQ(-1, aio_read(NULL));
}

TEST(aio_read_invalid_sigev) {
	struct aiocb cb = {0};
	cb.aio_sigevent.sigev_notify = 999;
	ASSERT_EQ(-1, aio_read(&cb));
}

TEST(aio_read_bad_fd) {
	struct aiocb cb = {0};
	cb.aio_fildes = -99;
	cb.aio_buf = "test";
	cb.aio_nbytes = 4;
	cb.aio_offset = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_read(&cb);
	ASSERT_EQ(-1, r);
}

TEST(aio_read_negative_offset) {
	int fd = __test_create_temp_file("negoff", "test", 4);
	ASSERT_GE(fd, 0);
	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = malloc(4);
	cb.aio_nbytes = 4;
	cb.aio_offset = -1;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_read(&cb);
	ASSERT_EQ(-1, r);
	free((void *)cb.aio_buf);
	close(fd);
}

TEST(aio_read_closed_fd) {
	int fd = __test_create_temp_file("closed", "test", 4);
	ASSERT_GE(fd, 0);
	close(fd);
	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = malloc(4);
	cb.aio_nbytes = 4;
	cb.aio_offset = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_read(&cb);
	ASSERT_EQ(-1, r);
	free((void *)cb.aio_buf);
}

TEST(aio_read_large_nbytes) {
	int fd = __test_create_temp_file("large", "test", 4);
	ASSERT_GE(fd, 0);
	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = malloc(4);
	cb.aio_nbytes = SIZE_MAX;
	cb.aio_offset = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_read(&cb);
	ASSERT_EQ(-1, r);
	free((void *)cb.aio_buf);
	close(fd);
}

TEST(aio_read_null_buf) {
	int fd = __test_create_temp_file("nullbuf", "test", 4);
	ASSERT_GE(fd, 0);
	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = NULL;
	cb.aio_nbytes = 4;
	cb.aio_offset = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_read(&cb);
	ASSERT_EQ(-1, r);
	close(fd);
}

TEST(aio_read_nested_io) {
	int fd = __test_create_temp_file("reenter", "XX", 2);

	ASSERT_GE(fd, 0);

	struct aiocb cb1 = {0}, cb2 = {0};

	cb1.aio_fildes = fd;
	cb1.aio_buf = malloc(1);
	cb1.aio_nbytes = 1;
	cb1.aio_offset = 0;
	cb1.aio_sigevent.sigev_notify = SIGEV_NONE;

	cb2.aio_fildes = fd;
	cb2.aio_buf = malloc(1);
	cb2.aio_nbytes = 1;
	cb2.aio_offset = 1;
	cb2.aio_sigevent.sigev_notify = SIGEV_NONE;

	int r1 = aio_read(&cb1);
	int r2 = aio_read(&cb2);
	int tries = 0;

	ASSERT_TRUE(r1 == 0 || r1 == -1);
	ASSERT_TRUE(r2 == 0 || r2 == -1);

	while ((aio_error(&cb1) == EINPROGRESS || aio_error(&cb2) == EINPROGRESS) && tries++ < 100) usleep(1000);

	if (aio_error(&cb1) == 0 && aio_error(&cb2) == 0) {
		ASSERT_EQ('X', *(char*)cb1.aio_buf);
		ASSERT_EQ('X', *(char*)cb2.aio_buf);
	}

	free((void*)cb1.aio_buf);
	free((void*)cb2.aio_buf);
	close(fd);
}

TEST(aio_read_eintr_handling) {
	/* Hard to force EINTR reliably, but verify error path doesn't crash */
	int fd = __test_create_temp_file("eintr", "test", 4);
	ASSERT_GE(fd, 0);

	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = malloc(4);
	cb.aio_nbytes = 4;
	cb.aio_offset = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;

	/* Submit, then immediately destroy ring to simulate interruption */
	int r = aio_read(&cb);
	aio_destroy();  /* Force fallback/error path */

	/* Should not crash; may return error */
	ASSERT_TRUE(r == 0 || r == -1);

	free((void*)cb.aio_buf);
	close(fd);
}

TEST(aio_read_beyond_eof) {
	int fd = __test_create_temp_file("eof", "AB", 2);
	ASSERT_GE(fd, 0);

	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = malloc(10);
	cb.aio_nbytes = 10;  /* Request more than file has */
	cb.aio_offset = 1;   /* Start at byte 1, only 1 byte available */
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;

	int r = aio_read(&cb);
	int tries = 0;
	while (aio_error(&cb) == EINPROGRESS && tries++ < 100) usleep(1000);

	if (aio_error(&cb) == 0) {
		/* Should return bytes actually read (1), not error */
		ASSERT_EQ(1, aio_return(&cb));
	}

	free((void*)cb.aio_buf);
	close(fd);
}

TEST(aio_read_sigev_signal) {
	struct aiocb cb = {0};
	cb.aio_fildes = 1;
	cb.aio_buf = (void *)"test";
	cb.aio_nbytes = 4;
	cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
	int r = aio_read(&cb);
	ASSERT_EQ(-1, r);
}

TEST(aio_read_sigev_thread) {
	struct aiocb cb = {0};
	cb.aio_fildes = 1;
	cb.aio_buf = (void *)"test";
	cb.aio_nbytes = 4;
	cb.aio_sigevent.sigev_notify = SIGEV_THREAD;
	int r = aio_read(&cb);
	ASSERT_EQ(-1, r);
}

TEST(aio_read_sigev_null_function) {
	struct aiocb cb = {0};
	cb.aio_fildes = 1;
	cb.aio_buf = (void *)"test";
	cb.aio_nbytes = 4;
	cb.aio_sigevent.sigev_notify = SIGEV_THREAD;
	cb.aio_sigevent.sigev_notify_function = NULL;  /* NULL function pointer */

	int r = aio_read(&cb);
	/* Should reject with EINVAL */
	ASSERT_EQ(-1, r);
	ASSERT_EQ(EINVAL, errno);
}

TEST(aio_read_sigev_invalid_signo) {
	struct aiocb cb = {0};
	cb.aio_fildes = 1;
	cb.aio_buf = (void *)"test";
	cb.aio_nbytes = 4;
	cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
	cb.aio_sigevent.sigev_signo = -1;  /* Invalid signal number */

	int r = aio_read(&cb);
	/* Should reject with EINVAL */
	ASSERT_EQ(-1, r);
	ASSERT_EQ(EINVAL, errno);
}

/* ============================================================================ */

TEST_SUITE(aio_write);

TEST(aio_write_valid_file) {
	int fd = __test_create_temp_file("aio_write", NULL, 0);
	ASSERT_GE(fd, 0);

	const char *data = "Written via AIO";
	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = (void *)data;
	cb.aio_nbytes = strlen(data);
	cb.aio_offset = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;

	int r = aio_write(&cb);
	int tries = 0;
	while (aio_error(&cb) == EINPROGRESS && tries++ < 100) usleep(1000);

	ASSERT_EQ((ssize_t)strlen(data), aio_return(&cb));

	close(fd);
}

TEST(aio_write_zero_bytes) {
	int fd = __test_create_temp_file("zerowrite", NULL, 0);
	ASSERT_GE(fd, 0);
	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = (void *)"test";
	cb.aio_nbytes = 0;
	cb.aio_offset = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_write(&cb);
	ASSERT_EQ(0, r);
	close(fd);
}

TEST(aio_write_null_cb) {
	ASSERT_EQ(-1, aio_write(NULL));
}

TEST(aio_write_submissions) {
	aio_init(NULL);

	int fd = __test_create_temp_file("threads", NULL, 0);
	ASSERT_GE(fd, 0);

	int success_count = 0;
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

	pthread_t t1, t2;
	struct __test_thread_arg arg1 = {fd, 0, &success_count, &lock};
	struct __test_thread_arg arg2 = {fd, 1, &success_count, &lock};

	pthread_create(&t1, NULL, __test_thread_aio_write, &arg1);
	pthread_create(&t2, NULL, __test_thread_aio_write, &arg2);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	/* Verify both writes succeeded */
	ASSERT_EQ(2, success_count);

	/* Verify file contents */
	char buf[2] = {0};
	lseek(fd, 0, SEEK_SET);
	read(fd, buf, 2);
	ASSERT_EQ('X', buf[0]);
	ASSERT_EQ('X', buf[1]);

	pthread_mutex_destroy(&lock);
	close(fd);
}

TEST(aio_write_many_threads) {
	aio_init(NULL);
	int fd = __test_create_temp_file("many_threads", NULL, 0);
	ASSERT_GE(fd, 0);
	int success_count = 0;
	pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	pthread_t threads[8];
	struct __test_thread_arg args[8];
	for (int i = 0; i < 8; i++) {
		args[i] = (struct __test_thread_arg){fd, i, &success_count, &lock};
		pthread_create(&threads[i], NULL, __test_thread_aio_write, &args[i]);
	}
	for (int i = 0; i < 8; i++) {
		pthread_join(threads[i], NULL);
	}
	ASSERT_EQ(8, success_count);
	char buf[8] = {0};
	lseek(fd, 0, SEEK_SET);
	read(fd, buf, 8);
	for (int i = 0; i < 8; i++) {
		ASSERT_EQ('X', buf[i]);
	}
	pthread_mutex_destroy(&lock);
	close(fd);
}

TEST(aio_write_invalid_sigev) {
	struct aiocb cb = {0};
	cb.aio_sigevent.sigev_notify = 999;
	ASSERT_EQ(-1, aio_write(&cb));
}

TEST(aio_write_bad_fd) {
	struct aiocb cb = {0};
	cb.aio_fildes = -99;
	cb.aio_buf = "test";
	cb.aio_nbytes = 4;
	cb.aio_offset = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_write(&cb);
	ASSERT_EQ(-1, r);
}

TEST(aio_write_readonly_fd) {
	int fd = open("/dev/null", O_RDONLY);
	ASSERT_GE(fd, 0);
	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = (void *)"test";
	cb.aio_nbytes = 4;
	cb.aio_offset = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_write(&cb);
	ASSERT_EQ(-1, r);
	close(fd);
}

TEST(aio_write_closed_fd) {
	int fd = __test_create_temp_file("closed", NULL, 0);
	ASSERT_GE(fd, 0);
	close(fd);
	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = (void *)"test";
	cb.aio_nbytes = 4;
	cb.aio_offset = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_write(&cb);
	ASSERT_EQ(-1, r);
}

TEST(aio_write_null_buf) {
	int fd = __test_create_temp_file("nullbuf", NULL, 0);
	ASSERT_GE(fd, 0);
	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = NULL;
	cb.aio_nbytes = 4;
	cb.aio_offset = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_write(&cb);
	ASSERT_EQ(-1, r);
	close(fd);
}

TEST(aio_write_queue_exhaustion) {
	struct sigaction sa = {0};
	sa.sa_handler = __test_signal_handler;
	sa.sa_flags = SA_NOCLDWAIT;
	sigaction(SIGUSR1, &sa, NULL);

	int fd = __test_create_temp_file("sigq_exhaust", NULL, 0);
	ASSERT_GE(fd, 0);

	/* Flood the process signal queue */
	for (int i = 0; i < 100; i++) {
		union sigval val = {.sival_int = i};
		sigqueue(getpid(), SIGUSR1, val);
		usleep(100);
	}

	/* Now attempt AIO write */
	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = (void *)"X";
	cb.aio_nbytes = 1;
	cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;

	int r = aio_write(&cb);
	ASSERT_TRUE(r == 0 || r == -1);

	aio_destroy();
	close(fd);
}

TEST(aio_write_disk_full_simulation) {
	/* Write to /dev/full if available, or verify error path */
	int fd = open("/dev/full", O_WRONLY);
	if (fd >= 0) {
		struct aiocb cb = {0};
		cb.aio_fildes = fd;
		cb.aio_buf = (void *)"X";
		cb.aio_nbytes = 1;
		cb.aio_sigevent.sigev_notify = SIGEV_NONE;

		int r = aio_write(&cb);
		/* Should fail with ENOSPC or similar */
		ASSERT_TRUE(r == -1);
		close(fd);
	} else {
		/* /dev/full not available - skip but pass */
		ASSERT_TRUE(1);
	}
}

TEST(aio_write_sigev_null_function) {
	struct aiocb cb = {0};
	cb.aio_fildes = 1;
	cb.aio_buf = (void *)"test";
	cb.aio_nbytes = 4;
	cb.aio_sigevent.sigev_notify = SIGEV_THREAD;
	cb.aio_sigevent.sigev_notify_function = NULL;

	int r = aio_write(&cb);
	ASSERT_EQ(-1, r);
	ASSERT_EQ(EINVAL, errno);
}

/* ============================================================================ */

TEST_SUITE(aio_fsync);

TEST(aio_fsync_valid) {
	int fd = __test_create_temp_file("fsync", "data", 4);
	ASSERT_GE(fd, 0);
	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_fsync(&cb);
	ASSERT_EQ(0, r);
	close(fd);
}

TEST(aio_fsync_null_cb) {
	ASSERT_EQ(-1, aio_fsync(NULL));
}

TEST(aio_fsync_invalid_sigev) {
	struct aiocb cb = {0};
	cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
	ASSERT_EQ(-1, aio_fsync(&cb));
}

TEST(aio_fsync_bad_fd) {
	struct aiocb cb = {0};
	cb.aio_fildes = -99;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_fsync(&cb);
	ASSERT_EQ(-1, r);
}

TEST(aio_fsync_closed_fd) {
	int fd = __test_create_temp_file("closed", "data", 4);
	ASSERT_GE(fd, 0);
	close(fd);
	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	int r = aio_fsync(&cb);
	ASSERT_EQ(-1, r);
}

TEST(aio_fsync_sigev_null_function) {
	struct aiocb cb = {0};
	cb.aio_fildes = 1;
	cb.aio_sigevent.sigev_notify = SIGEV_THREAD;
	cb.aio_sigevent.sigev_notify_function = NULL;

	int r = aio_fsync(&cb);
	ASSERT_EQ(-1, r);
	ASSERT_EQ(EINVAL, errno);
}

/* ============================================================================ */

TEST_SUITE(aio_mlock);

TEST(aio_mlock_returns_enosys) {
	struct aiocb cb = {0};
	cb.aio_fildes = 1;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	ASSERT_EQ(-1, aio_mlock(&cb));
	ASSERT_EQ(ENOSYS, errno);
}

TEST(aio_mlock_null_cb) {
	ASSERT_EQ(-1, aio_mlock(NULL));
}

TEST(aio_mlock_unsupported) {
	struct aiocb cb = {0};
	cb.aio_fildes = 1;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	ASSERT_EQ(-1, aio_mlock(&cb));
	ASSERT_EQ(ENOSYS, errno);
}

/* ============================================================================ */

TEST_SUITE(aio_error);

TEST(aio_error_pending) {
	struct aiocb cb = {0};
	cb.__jacl_state = 0;
	ASSERT_EQ(EINPROGRESS, aio_error(&cb));
}

TEST(aio_error_completed_success) {
	struct aiocb cb = {0};
	cb.__jacl_state = 1;
	cb.__jacl_result = 42;
	cb.__jacl_errno_val = 0;
	ASSERT_EQ(0, aio_error(&cb));
}

TEST(aio_error_completed_error) {
	struct aiocb cb = {0};
	cb.__jacl_state = 2;
	cb.__jacl_result = -1;
	cb.__jacl_errno_val = EIO;
	ASSERT_EQ(EIO, aio_error(&cb));
}

TEST(aio_error_canceled) {
	struct aiocb cb = {0};
	cb.__jacl_state = 3;
	cb.__jacl_errno_val = ECANCELED;
	ASSERT_EQ(ECANCELED, aio_error(&cb));
}

TEST(aio_error_null) {
	ASSERT_EQ(EINVAL, aio_error(NULL));
}

TEST(aio_error_invalid_state) {
	struct aiocb cb = {0};
	cb.__jacl_state = 99;
	int err = aio_error(&cb);
	ASSERT_EQ(EINVAL, err);
}

TEST(aio_error_uninitialized) {
	struct aiocb cb;
	memset(&cb, 0xFF, sizeof(cb));
	int err = aio_error(&cb);
	(void)err;
	ASSERT_TRUE(1);
}

/* ============================================================================ */

TEST_SUITE(aio_return);

TEST(aio_return_completed_success) {
	struct aiocb cb = {0};
	cb.__jacl_state = 1;
	cb.__jacl_result = 123;
	cb.__jacl_errno_val = 0;
	ASSERT_EQ(123, aio_return(&cb));
}

TEST(aio_return_twice) {
	struct aiocb cb = {0};
	cb.__jacl_state = 1;
	cb.__jacl_result = 42;
	ssize_t r1 = aio_return(&cb);
	ssize_t r2 = aio_return(&cb);
	ASSERT_EQ(r1, r2);
}

TEST(aio_return_null) {
	ASSERT_EQ(-1, aio_return(NULL));
}

TEST(aio_return_pending) {
	struct aiocb cb = {0};
	cb.__jacl_state = 0;
	ASSERT_EQ(-1, aio_return(&cb));
	ASSERT_EQ(EINPROGRESS, errno);
}

TEST(aio_return_completed_error) {
	struct aiocb cb = {0};
	cb.__jacl_state = 2;
	cb.__jacl_result = -1;
	cb.__jacl_errno_val = EACCES;
	ASSERT_EQ(-1, aio_return(&cb));
	ASSERT_EQ(EACCES, errno);
}

TEST(aio_return_canceled) {
	struct aiocb cb = {0};
	cb.__jacl_state = 3;
	cb.__jacl_result = -1;
	cb.__jacl_errno_val = ECANCELED;
	ASSERT_EQ(-1, aio_return(&cb));
	ASSERT_EQ(ECANCELED, errno);
}

/* ============================================================================ */

TEST_SUITE(aio_suspend);

TEST(aio_suspend_all_completed) {
	struct aiocb cb = {0};
	cb.__jacl_state = 1;
	const struct aiocb *cbs[] = {&cb};
	ASSERT_EQ(0, aio_suspend(cbs, 1, NULL));
}

TEST(aio_suspend_mixed_state) {
	struct aiocb cb1 = {0}, cb2 = {0};
	cb1.__jacl_state = 1;
	cb2.__jacl_state = 0;
	const struct aiocb *cbs[] = {&cb1, &cb2};
	int r = aio_suspend(cbs, 2, NULL);
	ASSERT_EQ(0, r);
}

TEST(aio_suspend_null_cbs) {
	ASSERT_EQ(-1, aio_suspend(NULL, 1, NULL));
}

TEST(aio_suspend_zero_n) {
	const struct aiocb *cbs[1] = {NULL};
	ASSERT_EQ(-1, aio_suspend(cbs, 0, NULL));
}

TEST(aio_suspend_negative_n) {
	const struct aiocb *cbs[1] = {NULL};
	int r = aio_suspend(cbs, -1, NULL);
	ASSERT_EQ(-1, r);
}

TEST(aio_suspend_all_timeout) {
	struct aiocb cb = {0};
	cb.__jacl_state = 0;
	const struct aiocb *cbs[] = {&cb};
	struct timespec ts = {0, 1000000};  /* 1ms timeout */
	int r = aio_suspend(cbs, 1, &ts);
	ASSERT_EQ(-1, r);
}

TEST(aio_suspend_partial_completion) {
	struct aiocb cb1 = {0}, cb2 = {0};
	cb1.__jacl_state = 1;  /* Already complete */
	cb2.__jacl_state = 0;  /* Still pending */

	const struct aiocb *cbs[] = {&cb1, &cb2};
	int r = aio_suspend(cbs, 2, NULL);

	/* Should return 0 immediately since cb1 is done */
	ASSERT_EQ(0, r);
}

TEST(aio_suspend_invalid_timeout) {
	struct aiocb cb = {0};
	cb.__jacl_state = 0;
	const struct aiocb *cbs[] = {&cb};

	struct timespec ts = {-1, 0};  /* Invalid: negative tv_sec */
	int r = aio_suspend(cbs, 1, &ts);

	/* Should fail with EINVAL */
	ASSERT_EQ(-1, r);
	ASSERT_EQ(EINVAL, errno);
}

TEST(aio_suspend_invalid_nsec) {
	struct aiocb cb = {0};
	cb.__jacl_state = 0;
	const struct aiocb *cbs[] = {&cb};

	struct timespec ts = {0, 1000000000};  /* tv_nsec == 1e9 (invalid) */
	int r = aio_suspend(cbs, 1, &ts);

	ASSERT_EQ(-1, r);
	ASSERT_EQ(EINVAL, errno);
}

/* ============================================================================ */

TEST_SUITE(aio_cancel);

TEST(aio_cancel_already_done) {
	struct aiocb cb = {0};
	cb.__jacl_state = 1;
	ASSERT_EQ(AIO_ALLDONE, aio_cancel(1, &cb));
}

TEST(aio_cancel_already_canceled) {
	struct aiocb cb = {0};
	cb.__jacl_state = 3;
	ASSERT_EQ(AIO_CANCELED, aio_cancel(1, &cb));
}

TEST(aio_cancel_null_cb) {
	ASSERT_EQ(AIO_NOTCANCELED, aio_cancel(1, NULL));
}

TEST(aio_cancel_already_error) {
	struct aiocb cb = {0};
	cb.__jacl_state = 2;
	ASSERT_EQ(AIO_ALLDONE, aio_cancel(1, &cb));
}

TEST(aio_cancel_during_operation) {
	int fd = __test_create_persistent_file("cancel_race", NULL, 0);
	ASSERT_GE(fd, 0);

	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = (void *)"test";
	cb.aio_nbytes = 4;
	cb.aio_offset = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;

	aio_write(&cb);
	/* Cancel immediately while operation may be in flight */
	int r = aio_cancel(fd, &cb);
	ASSERT_TRUE(r == AIO_CANCELED || r == AIO_ALLDONE || r == AIO_NOTCANCELED);

	close(fd);
}

TEST(aio_cancel_bad_fd) {
	struct aiocb cb = {0};
	cb.__jacl_state = 0;
	int r = aio_cancel(-99, &cb);
	ASSERT_EQ(AIO_NOTCANCELED, r);
}

TEST(aio_cancel_immediate_race) {
	int fd = __test_create_persistent_file("cancel_race", NULL, 0);
	ASSERT_GE(fd, 0);

	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = (void *)"test";
	cb.aio_nbytes = 4;
	cb.aio_offset = 0;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;

	/* Submit and cancel in tight loop */
	for (int i = 0; i < 10; i++) {
		aio_write(&cb);
		int r = aio_cancel(fd, &cb);
		ASSERT_TRUE(r == AIO_CANCELED || r == AIO_ALLDONE || r == AIO_NOTCANCELED);
	}

	close(fd);
}

TEST(aio_cancel_null_cb_with_fd) {
	int r = aio_cancel(1, NULL);  /* fd=1 is valid but cb is NULL */
	ASSERT_EQ(AIO_NOTCANCELED, r);  /* Should return immediately */
}

/* ============================================================================ */

TEST_SUITE(lio_listio);

TEST(lio_listio_nowait_empty_ops) {
	struct aiocb cb = {0};
	cb.aio_lio_opcode = LIO_NOP;
	struct aiocb *cbs[] = {&cb};
	int r = lio_listio(LIO_NOWAIT, cbs, 1, NULL);
	ASSERT_EQ(0, r);
}

TEST(lio_listio_wait_mode) {
	const char *data = "sync";
	int fd = __test_create_temp_file("lio_wait", NULL, 0);
	ASSERT_GE(fd, 0);

	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = (void *)data;
	cb.aio_nbytes = 4;
	cb.aio_offset = 0;
	cb.aio_lio_opcode = LIO_WRITE;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;

	struct aiocb *cbs[] = {&cb};
	int r = lio_listio(LIO_WAIT, cbs, 1, NULL);
	ASSERT_EQ(0, r);

	close(fd);
}

TEST(lio_listio_submit_counts) {
	int fd = __test_create_temp_file("lio_count", NULL, 0);
	ASSERT_GE(fd, 0);

	struct aiocb cbs[3];
	for (int i = 0; i < 3; i++) {
		cbs[i] = (struct aiocb){0};
		cbs[i].aio_fildes = fd;
		cbs[i].aio_buf = (void *)"x";
		cbs[i].aio_nbytes = 1;
		cbs[i].aio_offset = i;
		cbs[i].aio_lio_opcode = LIO_WRITE;
		cbs[i].aio_sigevent.sigev_notify = SIGEV_NONE;
	}

	struct aiocb *arr[] = {&cbs[0], &cbs[1], &cbs[2]};
	int r = lio_listio(LIO_NOWAIT, arr, 3, NULL);
	ASSERT_EQ(0, r);
	close(fd);
}

TEST(lio_listio_mixed_ops) {
	int fd = __test_create_temp_file("mixed", "data", 4);
	ASSERT_GE(fd, 0);

	struct aiocb cb1 = {0}, cb2 = {0};
	cb1.aio_fildes = fd;
	cb1.aio_lio_opcode = LIO_READ;
	cb1.aio_buf = malloc(4);
	cb1.aio_nbytes = 4;
	cb1.aio_offset = 0;
	cb1.aio_sigevent.sigev_notify = SIGEV_NONE;

	cb2.aio_fildes = fd;
	cb2.aio_lio_opcode = LIO_NOP;
	cb2.aio_sigevent.sigev_notify = SIGEV_NONE;

	struct aiocb *cbs[] = {&cb1, &cb2};
	int r = lio_listio(LIO_WAIT, cbs, 2, NULL);
	ASSERT_EQ(0, r);

	free((void *)cb1.aio_buf);
	close(fd);
}

TEST(lio_listio_null_array) {
	ASSERT_EQ(-1, lio_listio(LIO_NOWAIT, NULL, 1, NULL));
}

TEST(lio_listio_zero_count) {
	struct aiocb *cbs[1] = {NULL};
	ASSERT_EQ(-1, lio_listio(LIO_NOWAIT, cbs, 0, NULL));
}

TEST(lio_listio_invalid_sigev) {
	struct aiocb cb = {0};
	cb.aio_lio_opcode = LIO_READ;
	cb.aio_fildes = 1;
	cb.aio_buf = (void *)"x";
	cb.aio_nbytes = 1;
	cb.aio_sigevent.sigev_notify = 999;
	struct aiocb *cbs[] = {&cb};
	int r = lio_listio(LIO_NOWAIT, cbs, 1, NULL);
	ASSERT_EQ(-1, r);
}

TEST(lio_listio_invalid_opcode) {
	struct aiocb cb = {0};
	cb.aio_lio_opcode = 99;
	cb.aio_fildes = 1;
	cb.aio_buf = (void *)"x";
	cb.aio_nbytes = 1;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	struct aiocb *cbs[] = {&cb};
	int r = lio_listio(LIO_NOWAIT, cbs, 1, NULL);
	ASSERT_EQ(-1, r);
}

TEST(lio_listio_negative_count) {
	struct aiocb *cbs[1] = {NULL};
	int r = lio_listio(LIO_NOWAIT, cbs, -1, NULL);
	ASSERT_EQ(-1, r);
}

TEST(lio_listio_wait_timeout) {
	/* Test LIO_WAIT with operation that will timeout */
	int fd = __test_create_persistent_file("lio_timeout", NULL, 0);
	ASSERT_GE(fd, 0);

	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = (void *)"x";
	cb.aio_nbytes = 1;
	cb.aio_offset = 0;
	cb.aio_lio_opcode = LIO_WRITE;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;

	struct aiocb *cbs[] = {&cb};
	int r = lio_listio(LIO_WAIT, cbs, 1, NULL);
	/* Should complete or timeout, not hang */
	ASSERT_TRUE(r == 0 || r == -1);

	close(fd);
}

TEST(lio_listio_all_null_elements) {
	struct aiocb *cbs[3] = {NULL, NULL, NULL};
	int r = lio_listio(LIO_NOWAIT, cbs, 3, NULL);

	/* Should succeed (no ops to execute) or fail gracefully */
	ASSERT_TRUE(r == 0 || r == -1);
}

TEST(lio_listio_mixed_fd_validity) {
	int fd = __test_create_temp_file("lio_mix", NULL, 0);
	ASSERT_GE(fd, 0);

	struct aiocb cb1 = {0}, cb2 = {0};
	cb1.aio_fildes = fd;	  /* Valid */
	cb1.aio_lio_opcode = LIO_WRITE;
	cb1.aio_buf = (void *)"X";
	cb1.aio_nbytes = 1;
	cb1.aio_sigevent.sigev_notify = SIGEV_NONE;

	cb2.aio_fildes = -99;	 /* Invalid */
	cb2.aio_lio_opcode = LIO_WRITE;
	cb2.aio_buf = (void *)"Y";
	cb2.aio_nbytes = 1;
	cb2.aio_sigevent.sigev_notify = SIGEV_NONE;

	struct aiocb *cbs[] = {&cb1, &cb2};
	int r = lio_listio(LIO_NOWAIT, cbs, 2, NULL);

	/* Should fail validation before executing any */
	ASSERT_EQ(-1, r);
	ASSERT_EQ(EBADF, errno);

	close(fd);
}

TEST(lio_listio_all_nops_wait) {
	struct aiocb cb[3];
	for (int i = 0; i < 3; i++) {
		cb[i].aio_lio_opcode = LIO_NOP;
		cb[i].aio_sigevent.sigev_notify = SIGEV_NONE;
	}

	struct aiocb *cbs[] = {&cb[0], &cb[1], &cb[2]};
	int r = lio_listio(LIO_WAIT, cbs, 3, NULL);

	/* Should return immediately (NOPs are "complete") */
	ASSERT_EQ(0, r);
	for (int i = 0; i < 3; i++) {
		ASSERT_EQ(1, cb[i].__jacl_state);
		ASSERT_EQ(0, cb[i].__jacl_result);
	}
}

/* ============================================================================ */

TEST_SUITE(aio_submit_batch);

TEST(aio_submit_batch_valid) {
	int fd = __test_create_temp_file("batch", NULL, 0);
	ASSERT_GE(fd, 0);
	struct aiocb *cbs[2];
	struct aiocb cb1 = {0}, cb2 = {0};
	cb1.aio_fildes = fd; cb1.aio_buf = (void *)"A"; cb1.aio_nbytes = 1;
	cb1.aio_offset = 0; cb1.aio_lio_opcode = LIO_WRITE; cb1.aio_sigevent.sigev_notify = SIGEV_NONE;
	cb2.aio_fildes = fd; cb2.aio_buf = (void *)"B"; cb2.aio_nbytes = 1;
	cb2.aio_offset = 1; cb2.aio_lio_opcode = LIO_WRITE; cb2.aio_sigevent.sigev_notify = SIGEV_NONE;
	cbs[0] = &cb1; cbs[1] = &cb2;
	int r = aio_submit_batch(cbs, 2);
	ASSERT_EQ(2, r);
	close(fd);
}

TEST(aio_submit_batch_single) {
	int fd = __test_create_temp_file("single", NULL, 0);
	ASSERT_GE(fd, 0);
	struct aiocb *cbs[1];
	struct aiocb cb = {0};
	cb.aio_fildes = fd;
	cb.aio_buf = (void *)"X";
	cb.aio_nbytes = 1;
	cb.aio_offset = 0;
	cb.aio_lio_opcode = LIO_WRITE;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	cbs[0] = &cb;
	int r = aio_submit_batch(cbs, 1);
	ASSERT_EQ(1, r);
	close(fd);
}

TEST(aio_submit_batch_large) {
	int fd = __test_create_temp_file("large", NULL, 0);
	ASSERT_GE(fd, 0);
	struct aiocb *cbs[10];
	struct aiocb cb[10];
	for (int i = 0; i < 10; i++) {
		cb[i].aio_fildes = fd;
		cb[i].aio_buf = (void *)"X";
		cb[i].aio_nbytes = 1;
		cb[i].aio_offset = i;
		cb[i].aio_lio_opcode = LIO_WRITE;
		cb[i].aio_sigevent.sigev_notify = SIGEV_NONE;
		cbs[i] = &cb[i];
	}
	int r = aio_submit_batch(cbs, 10);
	ASSERT_EQ(10, r);
	close(fd);
}

TEST(aio_submit_batch_null_cbs) {
	ASSERT_EQ(-1, aio_submit_batch(NULL, 1));
}

TEST(aio_submit_batch_zero_n) {
	ASSERT_EQ(-1, aio_submit_batch(NULL, 0));
}

TEST(aio_submit_batch_negative_n) {
	struct aiocb *cbs[1] = {NULL};
	int r = aio_submit_batch(cbs, -1);
	ASSERT_EQ(-1, r);
}

TEST(aio_submit_batch_null_middle) {
	int fd = __test_create_temp_file("batch_null", NULL, 0);
	ASSERT_GE(fd, 0);

	struct aiocb cb1 = {0}, cb2 = {0};
	cb1.aio_fildes = fd;
	cb1.aio_buf = (void *)"A";
	cb1.aio_nbytes = 1;
	cb1.aio_lio_opcode = LIO_WRITE;
	cb1.aio_sigevent.sigev_notify = SIGEV_NONE;

	cb2.aio_fildes = fd;
	cb2.aio_buf = (void *)"B";
	cb2.aio_nbytes = 1;
	cb2.aio_lio_opcode = LIO_WRITE;
	cb2.aio_sigevent.sigev_notify = SIGEV_NONE;

	struct aiocb *cbs[] = {&cb1, NULL, &cb2};
	int r = aio_submit_batch(cbs, 3);

	/* Should process valid elements, skip NULL */
	ASSERT_GE(r, 0);

	close(fd);
}

TEST(aio_submit_batch_all_null) {
	struct aiocb *cbs[3] = {NULL, NULL, NULL};
	int r = aio_submit_batch(cbs, 3);

	/* Should return 0 (no ops submitted) or error gracefully */
	ASSERT_TRUE(r == 0 || r == -1);
}

/* ============================================================================ */

#if JACL_HAS_LFS

TEST_SUITE(aio_alias);

TEST(aio_alias_read64) {
	struct aiocb cb = {0};
	cb.aio_fildes = -1;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	ASSERT_EQ(aio_read(&cb), aio_read64(&cb));
	ASSERT_EQ(aio_read(NULL), aio_read64(NULL));
}

TEST(aio_alias_write64) {
	struct aiocb cb = {0};
	cb.aio_fildes = -1;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	ASSERT_EQ(aio_write(&cb), aio_write64(&cb));
	ASSERT_EQ(aio_write(NULL), aio_write64(NULL));
}

TEST(aio_alias_fsync64) {
	struct aiocb cb = {0};
	cb.aio_fildes = 1;
	cb.aio_sigevent.sigev_notify = SIGEV_NONE;
	ASSERT_EQ(aio_fsync(&cb), aio_fsync64(&cb));
	ASSERT_EQ(aio_fsync(NULL), aio_fsync64(NULL));
}

TEST(aio_alias_mlock64) {
	struct aiocb cb = {0};
	ASSERT_EQ(aio_mlock(&cb), aio_mlock64(&cb));
	ASSERT_EQ(aio_mlock(NULL), aio_mlock64(NULL));
}

TEST(aio_alias_error64) {
	struct aiocb cb = {0};
	cb.__jacl_state = 1;
	ASSERT_EQ(aio_error(&cb), aio_error64(&cb));
	ASSERT_EQ(aio_error(NULL), aio_error64(NULL));
}

TEST(aio_alias_return64) {
	struct aiocb cb = {0};
	cb.__jacl_state = 1;
	cb.__jacl_result = 42;
	ASSERT_EQ(aio_return(&cb), aio_return64(&cb));
	ASSERT_EQ(aio_return(NULL), aio_return64(NULL));
}

TEST(aio_alias_suspend64) {
	struct aiocb cb = {0};
	cb.__jacl_state = 1;
	const struct aiocb *cbs[] = {&cb};
	ASSERT_EQ(aio_suspend(cbs, 1, NULL), aio_suspend64(cbs, 1, NULL));
	ASSERT_EQ(aio_suspend(NULL, 1, NULL), aio_suspend64(NULL, 1, NULL));
}

TEST(aio_alias_cancel64) {
	struct aiocb cb = {0};
	cb.__jacl_state = 1;
	ASSERT_EQ(aio_cancel(1, &cb), aio_cancel64(1, &cb));
	ASSERT_EQ(aio_cancel(1, NULL), aio_cancel64(1, NULL));
}

TEST(aio_alias_listio64) {
	struct aiocb cb = {0};
	cb.aio_lio_opcode = LIO_NOP;
	struct aiocb *cbs[] = {&cb};
	ASSERT_EQ(lio_listio(LIO_NOWAIT, cbs, 1, NULL), lio_listio64(LIO_NOWAIT, cbs, 1, NULL));
	ASSERT_EQ(lio_listio(LIO_NOWAIT, NULL, 1, NULL), lio_listio64(LIO_NOWAIT, NULL, 1, NULL));
}

#endif /* JACL_HAS_LFS */

#endif /* JACL_HAS_POSIX */

TEST_MAIN_IF(JACL_HAS_POSIX, "aio.h requires POSIX\n")
