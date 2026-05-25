/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <mqueue.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdatomic.h>

static volatile sig_atomic_t g_mq_notify_got_sig = 0;
static volatile sig_atomic_t g_mq_notify_sig_count = 0;

static void __mq_notify_handler_usr1(int s) { (void)s; g_mq_notify_got_sig = 1; }
static void __mq_notify_handler_usr2(int s) { (void)s; g_mq_notify_sig_count++; }

static volatile sig_atomic_t g_mq_notify_thread_fired = 0;

static void __mq_notify_thread_fn(union sigval sv) {
	(void)sv;
	g_mq_notify_thread_fired = 1;
}

TEST_TYPE(unit);
TEST_UNIT(mqueue.h);

/* ============================================================================ */

TEST_SUITE(mqd_t);

TEST(mqd_t_type) {
	mqd_t q;
	ASSERT_SIZE(q, sizeof(int));
}

/* ============================================================================ */

TEST_SUITE(mq_attr);

TEST(mq_attr_members) {
	struct mq_attr attr;
	ASSERT_OFFSET(struct mq_attr, mq_flags, offsetof(struct mq_attr, mq_flags));
	ASSERT_OFFSET(struct mq_attr, mq_maxmsg, offsetof(struct mq_attr, mq_maxmsg));
	ASSERT_OFFSET(struct mq_attr, mq_msgsize, offsetof(struct mq_attr, mq_msgsize));
	ASSERT_OFFSET(struct mq_attr, mq_curmsgs, offsetof(struct mq_attr, mq_curmsgs));
}

TEST(mq_attr_initialization) {
	struct mq_attr attr = {0};
	ASSERT_EQ(attr.mq_flags, 0);
	ASSERT_EQ(attr.mq_maxmsg, 0);
	ASSERT_EQ(attr.mq_msgsize, 0);
	ASSERT_EQ(attr.mq_curmsgs, 0);
}

/* ============================================================================ */

TEST_SUITE(mq_open);

TEST(mq_open_basic) {
	mqd_t q = mq_open("/mq_h1", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_h1"), 0);
}

TEST(mq_open_with_attr) {
	struct mq_attr attr = {0, 10, 512, 0};
	mqd_t q = mq_open("/mq_h2", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_h2"), 0);
}

TEST(mq_open_null_name) {
	errno = 0;
	ASSERT_EQ(mq_open(NULL, O_CREAT | O_RDWR, 0644, NULL), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(mq_open_empty_name) {
	errno = 0;
	ASSERT_EQ(mq_open("", O_CREAT | O_RDWR, 0644, NULL), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(mq_open_invalid_slash) {
	errno = 0;
	ASSERT_EQ(mq_open("/foo/bar", O_CREAT | O_RDWR, 0644, NULL), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(mq_open_excl_existing) {
	mqd_t q = mq_open("/mq_s1", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	errno = 0;
	ASSERT_EQ(mq_open("/mq_s1", O_CREAT | O_EXCL | O_RDWR, 0644, NULL), -1);
	ASSERT_ERRNO(EEXIST);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_s1"), 0);
}

TEST(mq_open_pool_exhaustion) {
	mqd_t q[65];
	int i;
	for (i = 0; i < 64; i++) {
		char name[32];
		snprintf(name, sizeof(name), "/mq_pool_%d", i);
		q[i] = mq_open(name, O_CREAT | O_RDWR, 0644, NULL);
		if (q[i] < 0) break;
	}
	ASSERT_EQ(i, 64);
	errno = 0;
	ASSERT_EQ(mq_open("/mq_pool_max", O_CREAT | O_RDWR, 0644, NULL), -1);
	ASSERT_ERRNO(ENFILE);
	for (int j = 0; j < i; j++) {
		char name[32];
		snprintf(name, sizeof(name), "/mq_pool_%d", j);
		mq_close(q[j]);
		mq_unlink(name);
	}
}

TEST(mq_open_excl_without_creat) {
	errno = 0;
	ASSERT_EQ(mq_open("/mq_excl_no_creat", O_EXCL | O_RDWR), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(mq_open_reopen_existing_preserves_attr) {
	struct mq_attr attr = {0, 20, 256, 0};
	mqd_t q1 = mq_open("/mq_reopen", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q1 >= 0);
	mqd_t q2 = mq_open("/mq_reopen", O_RDWR);
	ASSERT_TRUE(q2 >= 0);
	struct mq_attr got;
	ASSERT_EQ(mq_getattr(q2, &got), 0);
	ASSERT_EQ(got.mq_maxmsg, 20);
	ASSERT_EQ(got.mq_msgsize, 256);
	ASSERT_EQ(mq_close(q1), 0);
	ASSERT_EQ(mq_close(q2), 0);
	ASSERT_EQ(mq_unlink("/mq_reopen"), 0);
}

TEST(mq_open_name_just_slash) {
	errno = 0;
	ASSERT_EQ(mq_open("/", O_CREAT | O_RDWR, 0644, NULL), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(mq_open_invalid_attr_zero_maxmsg) {
	struct mq_attr attr = {0, 0, 64, 0};
	errno = 0;
	ASSERT_EQ(mq_open("/mq_bad_attr1", O_CREAT | O_RDWR, 0644, &attr), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(mq_open_same_name_shares_queue) {
	mqd_t q1 = mq_open("/mq_same", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q1 >= 0);
	mqd_t q2 = mq_open("/mq_same", O_RDWR);
	ASSERT_TRUE(q2 >= 0);
	mq_send(q1, "from1", 5, 0);
	char buf[8];
	ssize_t n = mq_receive(q2, buf, sizeof(buf), NULL);
	ASSERT_EQ(n, 5);
	ASSERT_MEM_EQ(buf, "from1", 5);
	ASSERT_EQ(mq_close(q1), 0);
	ASSERT_EQ(mq_close(q2), 0);
	ASSERT_EQ(mq_unlink("/mq_same"), 0);
}

static void *__mq_open_close_thread(void *arg) {
	int id = *(int *)arg;
	char name[32];
	snprintf(name, sizeof(name), "/mq_stress_%d", id);
	mqd_t q = mq_open(name, O_CREAT | O_RDWR | O_NONBLOCK, 0644, NULL);
	if (q >= 0) { mq_close(q); mq_unlink(name); }
	return NULL;
}

TEST(mq_open_concurrent_create_destroy) {
	pthread_t threads[32];
	int ids[32];
	for (int i = 0; i < 32; i++) ids[i] = i;
	for (int i = 0; i < 32; i++) pthread_create(&threads[i], NULL, __mq_open_close_thread, &ids[i]);
	for (int i = 0; i < 32; i++) pthread_join(threads[i], NULL);
}

TEST(mq_open_desc_exhaustion) {
	/* Open 256 descriptors on same queue to exhaust desc table */
	mqd_t first = mq_open("/mq_desc_exh", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(first >= 0);
	mqd_t descs[256];
	int count = 0;
	for (int i = 0; i < 255; i++) {
		descs[i] = mq_open("/mq_desc_exh", O_RDWR);
		if (descs[i] < 0) break;
		count++;
	}
	/* Next open must fail with EMFILE */
	errno = 0;
	ASSERT_EQ(mq_open("/mq_desc_exh", O_RDWR), -1);
	ASSERT_ERRNO(EMFILE);
	for (int i = 0; i < count; i++) mq_close(descs[i]);
	ASSERT_EQ(mq_close(first), 0);
	ASSERT_EQ(mq_unlink("/mq_desc_exh"), 0);
}

TEST(mq_open_invalid_attr_zero_msgsize) {
	struct mq_attr attr = {0, 10, 0, 0};
	errno = 0;
	ASSERT_EQ(mq_open("/mq_bad_msgsz", O_CREAT | O_RDWR, 0644, &attr), -1);
	ASSERT_ERRNO(EINVAL);
}

/* ============================================================================ */

TEST_SUITE(mq_close);

TEST(mq_close_basic) {
	mqd_t q = mq_open("/mq_c1", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_c1"), 0);
}

TEST(mq_close_negative) {
	errno = 0;
	ASSERT_EQ(mq_close(-1), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(mq_close_out_of_bounds) {
	errno = 0;
	ASSERT_EQ(mq_close(1000), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(mq_close_double) {
	mqd_t q = mq_open("/mq_c2", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_close(q), 0);
	errno = 0;
	ASSERT_EQ(mq_close(q), -1);
	ASSERT_ERRNO(EBADF);
	ASSERT_EQ(mq_unlink("/mq_c2"), 0);
}

TEST(mq_close_invalidated_descriptor) {
	mqd_t q = mq_open("/mq_inv_desc", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_unlink("/mq_inv_desc"), 0);
	ASSERT_EQ(mq_close(q), 0);
	/* Descriptor is now invalid; send must fail */
	errno = 0;
	ASSERT_EQ(mq_send(q, "x", 1, 0), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(mq_close_does_not_affect_other_descriptors) {
	mqd_t q1 = mq_open("/mq_close_iso", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q1 >= 0);
	mqd_t q2 = mq_open("/mq_close_iso", O_RDWR);
	ASSERT_TRUE(q2 >= 0);
	ASSERT_NE(q1, q2);
	ASSERT_EQ(mq_close(q1), 0);
	/* q2 must still work */
	ASSERT_EQ(mq_send(q2, "alive", 5, 0), 0);
	char buf[8];
	ssize_t n = mq_receive(q2, buf, sizeof(buf), NULL);
	ASSERT_EQ(n, 5);
	ASSERT_MEM_EQ(buf, "alive", 5);
	ASSERT_EQ(mq_close(q2), 0);
	ASSERT_EQ(mq_unlink("/mq_close_iso"), 0);
}

TEST(mq_close_frees_descriptor_for_reuse) {
	mqd_t q1 = mq_open("/mq_reuse_desc", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q1 >= 0);
	ASSERT_EQ(mq_close(q1), 0);
	ASSERT_EQ(mq_unlink("/mq_reuse_desc"), 0);
	/* New open should succeed; descriptor slot was freed */
	mqd_t q2 = mq_open("/mq_reuse_desc2", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q2 >= 0);
	ASSERT_EQ(mq_close(q2), 0);
	ASSERT_EQ(mq_unlink("/mq_reuse_desc2"), 0);
}

TEST(mq_close_unlinked_queue_with_pending_messages) {
	struct mq_attr attr = {O_NONBLOCK, 10, 64, 0};
	mqd_t q = mq_open("/mq_close_pend", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);
	mq_send(q, "msg1", 4, 0);
	mq_send(q, "msg2", 4, 0);
	ASSERT_EQ(mq_unlink("/mq_close_pend"), 0);
	/* Close with pending messages on unlinked queue must not leak or crash */
	ASSERT_EQ(mq_close(q), 0);
}

/* ============================================================================ */

TEST_SUITE(mq_unlink);

TEST(mq_unlink_basic) {
	mqd_t q = mq_open("/mq_u1", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_unlink("/mq_u1"), 0);
	ASSERT_EQ(mq_close(q), 0);
}

TEST(mq_unlink_null_name) {
	errno = 0;
	ASSERT_EQ(mq_unlink(NULL), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(mq_unlink_nonexistent) {
	errno = 0;
	ASSERT_EQ(mq_unlink("/mq_ghost"), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(mq_unlink_double) {
	mqd_t q = mq_open("/mq_u2", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_unlink("/mq_u2"), 0);
	errno = 0;
	ASSERT_EQ(mq_unlink("/mq_u2"), -1);
	ASSERT_ERRNO(ENOENT);
	ASSERT_EQ(mq_close(q), 0);
}

TEST(mq_unlink_while_open_still_usable) {
	mqd_t q = mq_open("/mq_unlink_use", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_unlink("/mq_unlink_use"), 0);
	/* Queue still usable after unlink */
	ASSERT_EQ(mq_send(q, "post", 4, 0), 0);
	char buf[8];
	ssize_t n = mq_receive(q, buf, sizeof(buf), NULL);
	ASSERT_EQ(n, 4);
	ASSERT_MEM_EQ(buf, "post", 4);
	/* Reopen should fail since unlinked */
	errno = 0;
	ASSERT_EQ(mq_open("/mq_unlink_use", O_RDWR), -1);
	ASSERT_ERRNO(ENOENT);
	ASSERT_EQ(mq_close(q), 0);
}

TEST(mq_unlink_destroys_when_last_ref_closed) {
	mqd_t q = mq_open("/mq_unref", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_unlink("/mq_unref"), 0);
	ASSERT_EQ(mq_close(q), 0);
	/* Should be fully destroyed now */
	errno = 0;
	ASSERT_EQ(mq_open("/mq_unref", O_RDWR), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(mq_unlink_name_reuse_after_destroy) {
	mqd_t q = mq_open("/mq_reuse", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_unlink("/mq_reuse"), 0);
	ASSERT_EQ(mq_close(q), 0);
	/* Name should be reusable */
	mqd_t q2 = mq_open("/mq_reuse", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q2 >= 0);
	ASSERT_EQ(mq_close(q2), 0);
	ASSERT_EQ(mq_unlink("/mq_reuse"), 0);
}

TEST(mq_unlink_multiple_refs_last_close_destroys) {
	mqd_t q1 = mq_open("/mq_multi_ref", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q1 >= 0);
	mqd_t q2 = mq_open("/mq_multi_ref", O_RDWR);
	ASSERT_TRUE(q2 >= 0);
	ASSERT_EQ(mq_unlink("/mq_multi_ref"), 0);
	ASSERT_EQ(mq_send(q1, "via_q1", 6, 0), 0);
	char buf[8];
	ssize_t n = mq_receive(q2, buf, sizeof(buf), NULL);
	ASSERT_EQ(n, 6);
	ASSERT_EQ(mq_close(q1), 0);
	ASSERT_EQ(mq_send(q2, "via_q2", 6, 0), 0);
	ASSERT_EQ(mq_close(q2), 0);
	errno = 0;
	ASSERT_EQ(mq_open("/mq_multi_ref", O_RDWR), -1);
	ASSERT_ERRNO(ENOENT);
}

/* ============================================================================ */

TEST_SUITE(mq_send);

TEST(mq_send_basic) {
	mqd_t q = mq_open("/mq_s1", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	char msg[] = "hello";
	ASSERT_EQ(mq_send(q, msg, 5, 1), 0);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_s1"), 0);
}

TEST(mq_send_zero_length) {
	mqd_t q = mq_open("/mq_s2", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	char msg[] = "test";
	ASSERT_EQ(mq_send(q, msg, 0, 0), 0);
	char buf[1];
	ASSERT_EQ(mq_receive(q, buf, 1, NULL), 0);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_s2"), 0);
}

TEST(mq_send_priority_order) {
	mqd_t q = mq_open("/mq_s3", O_CREAT | O_RDWR | O_NONBLOCK, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	mq_send(q, "low", 3, 1);
	mq_send(q, "high", 4, 10);
	char buf[5];
	ASSERT_EQ(mq_receive(q, buf, 5, NULL), 4);
	ASSERT_MEM_EQ(buf, "high", 4);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_s3"), 0);
}

TEST(mq_send_null_ptr) {
	mqd_t q = mq_open("/mq_s4", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	errno = 0;
	ASSERT_EQ(mq_send(q, NULL, 10, 0), -1);
	ASSERT_ERRNO(EINVAL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_s4"), 0);
}

TEST(mq_send_exceeds_maxmsg) {
	struct mq_attr attr = {0, 1, 64, 0};
	mqd_t q = mq_open("/mq_s5", O_CREAT | O_RDWR | O_NONBLOCK, 0644, &attr);
	ASSERT_TRUE(q >= 0);
	char m[2] = "a";
	ASSERT_EQ(mq_send(q, m, 1, 0), 0);
	errno = 0;
	ASSERT_EQ(mq_send(q, m, 1, 0), -1);
	ASSERT_ERRNO(EAGAIN);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_s5"), 0);
}

TEST(mq_send_rdonly) {
	mqd_t q = mq_open("/mq_s6", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_close(q), 0);
	q = mq_open("/mq_s6", O_RDONLY);
	ASSERT_TRUE(q >= 0);
	errno = 0;
	ASSERT_EQ(mq_send(q, "x", 1, 0), -1);
	ASSERT_ERRNO(EBADF);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_s6"), 0);
}

TEST(mq_send_priority_fifo_same_prio) {
	struct mq_attr attr = {O_NONBLOCK, 10, 64, 0};
	mqd_t q = mq_open("/mq_prio_fifo", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);
	mq_send(q, "first", 5, 5);
	mq_send(q, "second", 6, 5);
	char buf[8];
	ssize_t n = mq_receive(q, buf, sizeof(buf), NULL);
	ASSERT_EQ(n, 5);
	ASSERT_MEM_EQ(buf, "first", 5);
	n = mq_receive(q, buf, sizeof(buf), NULL);
	ASSERT_EQ(n, 6);
	ASSERT_MEM_EQ(buf, "second", 6);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_prio_fifo"), 0);
}

TEST(mq_send_msgsize_boundary) {
	struct mq_attr attr = {O_NONBLOCK, 10, 64, 0};
	mqd_t q = mq_open("/mq_size_bound", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);
	char msg[64];
	memset(msg, 'A', 64);
	ASSERT_EQ(mq_send(q, msg, 64, 0), 0);
	errno = 0;
	ASSERT_EQ(mq_send(q, msg, 65, 0), -1);
	ASSERT_ERRNO(EMSGSIZE);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_size_bound"), 0);
}

TEST(mq_send_wronly_succeeds) {
	mqd_t q = mq_open("/mq_wronly", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_close(q), 0);
	q = mq_open("/mq_wronly", O_WRONLY);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_send(q, "x", 1, 0), 0);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_wronly"), 0);
}

TEST(mq_send_three_priority_levels) {
	struct mq_attr attr = {O_NONBLOCK, 10, 64, 0};
	mqd_t q = mq_open("/mq_prio3", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);
	mq_send(q, "low", 3, 1);
	mq_send(q, "mid", 3, 5);
	mq_send(q, "high", 4, 10);
	char buf[8]; unsigned prio;
	mq_receive(q, buf, sizeof(buf), &prio);
	ASSERT_EQ(prio, 10); ASSERT_MEM_EQ(buf, "high", 4);
	mq_receive(q, buf, sizeof(buf), &prio);
	ASSERT_EQ(prio, 5); ASSERT_MEM_EQ(buf, "mid", 3);
	mq_receive(q, buf, sizeof(buf), &prio);
	ASSERT_EQ(prio, 1); ASSERT_MEM_EQ(buf, "low", 3);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_prio3"), 0);
}

TEST(mq_send_max_priority_value) {
	struct mq_attr attr = {O_NONBLOCK, 10, 64, 0};
	mqd_t q = mq_open("/mq_maxprio", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);
	mq_send(q, "norm", 4, 0);
	mq_send(q, "max", 3, UINT_MAX);
	char buf[8]; unsigned prio;
	mq_receive(q, buf, sizeof(buf), &prio);
	ASSERT_EQ(prio, UINT_MAX); ASSERT_MEM_EQ(buf, "max", 3);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_maxprio"), 0);
}

TEST(mq_send_fill_then_drain_preserves_order) {
	struct mq_attr attr = {O_NONBLOCK, 5, 64, 0};
	mqd_t q = mq_open("/mq_fill_drain", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);
	for (int i = 0; i < 5; i++) {
		char msg[4]; snprintf(msg, sizeof(msg), "%d", i);
		ASSERT_EQ(mq_send(q, msg, strlen(msg), 0), 0);
	}
	errno = 0;
	ASSERT_EQ(mq_send(q, "overflow", 8, 0), -1);
	ASSERT_ERRNO(EAGAIN);
	for (int i = 0; i < 5; i++) {
		char buf[4]; unsigned prio;
		ssize_t n = mq_receive(q, buf, sizeof(buf), &prio);
		ASSERT_TRUE(n > 0);
		char expected[4]; snprintf(expected, sizeof(expected), "%d", i);
		ASSERT_MEM_EQ(buf, expected, (size_t)n);
	}
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_fill_drain"), 0);
}

static mqd_t __mp_q;
static atomic_int __mp_recv_total = 0;

static void *__mp_worker(void *arg) {
    int count = *(int *)arg;
    for (int i = 0; i < count; i++) {
        char msg[4];
        snprintf(msg, 4, "%d", i);
        mq_send(__mp_q, msg, 3, i % 4);
    }
    return NULL;
}

static void *__mp_consumer(void *arg) {
    (void)arg;
    char buf[4];
    int received = 0;
    while (received < 100) {
        ssize_t r = mq_receive(__mp_q, buf, 4, NULL);
        if (r > 0) { received++; atomic_fetch_add(&__mp_recv_total, 1); }
        else usleep(1000);
    }
    return NULL;
}

TEST(mq_send_multi_producer_priority_integrity) {
    struct mq_attr attr = {0, 200, 64, 0};
    __mp_q = mq_open("/mq_mp1", O_CREAT | O_RDWR | O_NONBLOCK, 0644, &attr);
    ASSERT_TRUE(__mp_q >= 0);
    atomic_store(&__mp_recv_total, 0);
    int c1 = 50, c2 = 50;
    pthread_t t1, t2, t3;
    pthread_create(&t3, NULL, __mp_consumer, NULL);
    pthread_create(&t1, NULL, __mp_worker, &c1);
    pthread_create(&t2, NULL, __mp_worker, &c2);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    ASSERT_EQ(__mp_recv_total, 100);
    ASSERT_EQ(mq_close(__mp_q), 0);
    ASSERT_EQ(mq_unlink("/mq_mp1"), 0);
}

TEST(mq_send_bad_descriptor) {
	errno = 0;
	ASSERT_EQ(mq_send(-1, "x", 1, 0), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(mq_send_after_close_fails) {
	mqd_t q = mq_open("/mq_send_closed", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_close(q), 0);
	errno = 0;
	ASSERT_EQ(mq_send(q, "x", 1, 0), -1);
	ASSERT_ERRNO(EBADF);
	ASSERT_EQ(mq_unlink("/mq_send_closed"), 0);
}

/* ============================================================================ */

TEST_SUITE(mq_receive);

TEST(mq_receive_basic) {
	mqd_t q = mq_open("/mq_r1", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	char msg[] = "data";
	ASSERT_EQ(mq_send(q, msg, 4, 1), 0);
	char buf[8] = {0}; /* Zero-init to prevent garbage read */
	unsigned prio = 0;
	ssize_t r = mq_receive(q, buf, sizeof(buf), &prio);
	ASSERT_EQ(r, 4);
	ASSERT_EQ(prio, 1);
	ASSERT_STR_EQ(buf, "data");
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_r1"), 0);
}

TEST(mq_receive_exact_buffer) {
	mqd_t q = mq_open("/mq_r2", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	char msg[] = "exact";
	ASSERT_EQ(mq_send(q, msg, 5, 0), 0);
	char buf[5];
	ASSERT_EQ(mq_receive(q, buf, 5, NULL), 5);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_r2"), 0);
}

TEST(mq_receive_null_buffer) {
	mqd_t q = mq_open("/mq_r3", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	errno = 0;
	ASSERT_EQ(mq_receive(q, NULL, 10, NULL), -1);
	ASSERT_ERRNO(EINVAL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_r3"), 0);
}

TEST(mq_receive_buffer_small) {
	mqd_t q = mq_open("/mq_r4", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	char msg[] = "toolong";
	ASSERT_EQ(mq_send(q, msg, 7, 0), 0);
	char buf[4];
	errno = 0;
	ASSERT_EQ(mq_receive(q, buf, 4, NULL), -1);
	ASSERT_ERRNO(EMSGSIZE);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_r4"), 0);
}

TEST(mq_receive_wronly) {
	mqd_t q = mq_open("/mq_r5", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_close(q), 0);
	q = mq_open("/mq_r5", O_WRONLY);
	ASSERT_TRUE(q >= 0);
	char buf[4];
	errno = 0;
	ASSERT_EQ(mq_receive(q, buf, 4, NULL), -1);
	ASSERT_ERRNO(EBADF);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_r5"), 0);
}

TEST(mq_receive_zero_length_message) {
	mqd_t q = mq_open("/mq_zerolen", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_send(q, "", 0, 0), 0);
	unsigned char buf[1] = {0xAA};
	ssize_t n = mq_receive(q, (char *)buf, sizeof(buf), NULL);
	ASSERT_EQ(n, 0);
	ASSERT_EQ(buf[0], 0xAA);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_zerolen"), 0);
}

TEST(mq_receive_empty_nonblock) {
	struct mq_attr attr = {O_NONBLOCK, 10, 64, 0};
	mqd_t q = mq_open("/mq_empty_nb", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);
	char buf[8];
	errno = 0;
	ASSERT_EQ(mq_receive(q, buf, sizeof(buf), NULL), -1);
	ASSERT_ERRNO(EAGAIN);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_empty_nb"), 0);
}

TEST(mq_receive_preserves_message_content) {
	mqd_t q = mq_open("/mq_content", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	char msg[32];
	for (int i = 0; i < 32; i++) msg[i] = (char)(i + 1);
	ASSERT_EQ(mq_send(q, msg, 32, 0), 0);
	char buf[32] = {0};
	ssize_t n = mq_receive(q, buf, 32, NULL);
	ASSERT_EQ(n, 32);
	ASSERT_MEM_EQ(buf, msg, 32);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_content"), 0);
}

TEST(mq_receive_sequential_fill_drain_integrity) {
	struct mq_attr attr = {0, 100, 64, 0};
	mqd_t q = mq_open("/mq_seq_fd", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);
	for (int i = 0; i < 100; i++) {
		char msg[4]; snprintf(msg, sizeof(msg), "%d", i);
		ASSERT_EQ(mq_send(q, msg, strlen(msg), i % 10), 0);
	}
	for (int i = 0; i < 100; i++) {
		char buf[4]; unsigned prio;
		ssize_t n = mq_receive(q, buf, sizeof(buf), &prio);
		ASSERT_TRUE(n > 0);
	}
	struct mq_attr got;
	ASSERT_EQ(mq_getattr(q, &got), 0);
	ASSERT_EQ(got.mq_curmsgs, 0);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_seq_fd"), 0);
}

TEST(mq_receive_bad_descriptor) {
	char buf[4];
	errno = 0;
	ASSERT_EQ(mq_receive(-1, buf, sizeof(buf), NULL), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(mq_receive_after_close_fails) {
	mqd_t q = mq_open("/mq_recv_closed", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_close(q), 0);
	char buf[4];
	errno = 0;
	ASSERT_EQ(mq_receive(q, buf, sizeof(buf), NULL), -1);
	ASSERT_ERRNO(EBADF);
	ASSERT_EQ(mq_unlink("/mq_recv_closed"), 0);
}

/* ============================================================================ */

TEST_SUITE(mq_timedsend);

TEST(mq_timedsend_null_timeout) {
	mqd_t q = mq_open("/mq_ts1", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	errno = 0;
	ASSERT_EQ(mq_timedsend(q, "x", 1, 0, NULL), -1);
	ASSERT_ERRNO(EINVAL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_ts1"), 0);
}

TEST(mq_timedsend_invalid_nsec) {
	mqd_t q = mq_open("/mq_ts2", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_nsec = 9999999999LL;
	errno = 0;
	ASSERT_EQ(mq_timedsend(q, "x", 1, 0, &ts), -1);
	ASSERT_ERRNO(EINVAL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_ts2"), 0);
}

TEST(mq_timedsend_success_before_timeout) {
	struct mq_attr attr = {0, 10, 64, 0};
	mqd_t q = mq_open("/mq_ts_ok", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += 5;
	ASSERT_EQ(mq_timedsend(q, "ok", 2, 0, &ts), 0);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_ts_ok"), 0);
}

TEST(mq_timedsend_negative_nsec) {
	mqd_t q = mq_open("/mq_ts_neg", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	struct timespec ts = {0, -1};
	errno = 0;
	ASSERT_EQ(mq_timedsend(q, "x", 1, 0, &ts), -1);
	ASSERT_ERRNO(EINVAL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_ts_neg"), 0);
}

TEST(mq_timedsend_full_queue_times_out) {
	struct mq_attr attr = {0, 1, 64, 0};
	mqd_t q = mq_open("/mq_ts_full_to", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_send(q, "x", 1, 0), 0);
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_nsec += 50000000; /* 50ms */
	if (ts.tv_nsec >= 1000000000) { ts.tv_sec++; ts.tv_nsec -= 1000000000; }
	errno = 0;
	ASSERT_EQ(mq_timedsend(q, "y", 1, 0, &ts), -1);
	ASSERT_ERRNO(ETIMEDOUT);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_ts_full_to"), 0);
}

TEST(mq_timedsend_rdonly_fails) {
	mqd_t q = mq_open("/mq_ts_rdonly", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_close(q), 0);
	q = mq_open("/mq_ts_rdonly", O_RDONLY);
	ASSERT_TRUE(q >= 0);
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += 1;
	errno = 0;
	ASSERT_EQ(mq_timedsend(q, "x", 1, 0, &ts), -1);
	ASSERT_ERRNO(EBADF);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_ts_rdonly"), 0);
}

TEST(mq_timedsend_msgsize_exceeded) {
	struct mq_attr attr = {O_NONBLOCK, 10, 32, 0};
	mqd_t q = mq_open("/mq_ts_big", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);
	char msg[33];
	memset(msg, 'X', 33);
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += 1;
	errno = 0;
	ASSERT_EQ(mq_timedsend(q, msg, 33, 0, &ts), -1);
	ASSERT_ERRNO(EMSGSIZE);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_ts_big"), 0);
}

/* ============================================================================ */

TEST_SUITE(mq_timedreceive);

TEST(mq_timedreceive_past_time) {
	mqd_t q = mq_open("/mq_tr1", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	struct timespec ts = {0, 0};
	errno = 0;
	char buf[10];
	ASSERT_EQ(mq_timedreceive(q, buf, 10, NULL, &ts), -1);
	ASSERT_ERRNO(ETIMEDOUT);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_tr1"), 0);
}

TEST(mq_timedreceive_null_timeout) {
	mqd_t q = mq_open("/mq_tr_null", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	char buf[8];
	errno = 0;
	ASSERT_EQ(mq_timedreceive(q, buf, 8, NULL, NULL), -1);
	ASSERT_ERRNO(EINVAL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_tr_null"), 0);
}

TEST(mq_timedreceive_success_before_timeout) {
	mqd_t q = mq_open("/mq_tr_ok", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	mq_send(q, "data", 4, 0);
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += 5;
	char buf[8];
	ssize_t n = mq_timedreceive(q, buf, sizeof(buf), NULL, &ts);
	ASSERT_EQ(n, 4);
	ASSERT_MEM_EQ(buf, "data", 4);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_tr_ok"), 0);
}

TEST(mq_timedreceive_empty_times_out) {
	mqd_t q = mq_open("/mq_tr_empty_to", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_nsec += 50000000;
	if (ts.tv_nsec >= 1000000000) { ts.tv_sec++; ts.tv_nsec -= 1000000000; }
	char buf[8];
	errno = 0;
	ASSERT_EQ(mq_timedreceive(q, buf, sizeof(buf), NULL, &ts), -1);
	ASSERT_ERRNO(ETIMEDOUT);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_tr_empty_to"), 0);
}

TEST(mq_timedreceive_invalid_nsec_negative) {
	mqd_t q = mq_open("/mq_tr_neg_ns", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	struct timespec ts = {0, -1};
	char buf[8];
	errno = 0;
	ASSERT_EQ(mq_timedreceive(q, buf, sizeof(buf), NULL, &ts), -1);
	ASSERT_ERRNO(EINVAL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_tr_neg_ns"), 0);
}

TEST(mq_timedreceive_wronly_fails) {
	mqd_t q = mq_open("/mq_tr_wronly", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_close(q), 0);
	q = mq_open("/mq_tr_wronly", O_WRONLY);
	ASSERT_TRUE(q >= 0);
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += 1;
	char buf[8];
	errno = 0;
	ASSERT_EQ(mq_timedreceive(q, buf, sizeof(buf), NULL, &ts), -1);
	ASSERT_ERRNO(EBADF);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_tr_wronly"), 0);
}

TEST(mq_timedreceive_buffer_too_small) {
	mqd_t q = mq_open("/mq_tr_small", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	mq_send(q, "toolong", 7, 0);
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += 1;
	char buf[4];
	errno = 0;
	ASSERT_EQ(mq_timedreceive(q, buf, 4, NULL, &ts), -1);
	ASSERT_ERRNO(EMSGSIZE);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_tr_small"), 0);
}

/* ============================================================================ */

TEST_SUITE(mq_getattr);

TEST(mq_getattr_basic) {
	mqd_t q = mq_open("/mq_ga1", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	struct mq_attr attr;
	ASSERT_EQ(mq_getattr(q, &attr), 0);
	ASSERT_TRUE(attr.mq_maxmsg > 0);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_ga1"), 0);
}

TEST(mq_getattr_null) {
	mqd_t q = mq_open("/mq_ga2", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	errno = 0;
	ASSERT_EQ(mq_getattr(q, NULL), -1);
	ASSERT_ERRNO(EINVAL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_ga2"), 0);
}

TEST(mq_getattr_curmsgs_reflects_queue_state) {
	struct mq_attr attr = {O_NONBLOCK, 10, 64, 0};
	mqd_t q = mq_open("/mq_curmsgs", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);
	struct mq_attr got;
	ASSERT_EQ(mq_getattr(q, &got), 0);
	ASSERT_EQ(got.mq_curmsgs, 0);
	mq_send(q, "a", 1, 0);
	mq_send(q, "b", 1, 0);
	ASSERT_EQ(mq_getattr(q, &got), 0);
	ASSERT_EQ(got.mq_curmsgs, 2);
	char buf[4];
	mq_receive(q, buf, sizeof(buf), NULL);
	ASSERT_EQ(mq_getattr(q, &got), 0);
	ASSERT_EQ(got.mq_curmsgs, 1);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_curmsgs"), 0);
}

TEST(mq_getattr_bad_descriptor) {
	struct mq_attr got;
	errno = 0;
	ASSERT_EQ(mq_getattr(-1, &got), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(mq_getattr_independent_of_descriptor_access_mode) {
	mqd_t qw = mq_open("/mq_ga_mode", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(qw >= 0);
	ASSERT_EQ(mq_close(qw), 0);
	mqd_t qr = mq_open("/mq_ga_mode", O_RDONLY);
	ASSERT_TRUE(qr >= 0);
	struct mq_attr got;
	ASSERT_EQ(mq_getattr(qr, &got), 0);
	ASSERT_TRUE(got.mq_maxmsg > 0);
	ASSERT_TRUE(got.mq_msgsize > 0);
	ASSERT_EQ(mq_close(qr), 0);
	ASSERT_EQ(mq_unlink("/mq_ga_mode"), 0);
}

TEST(mq_getattr_after_setattr_reflects_change) {
	mqd_t q = mq_open("/mq_ga_sa", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	struct mq_attr set = {O_NONBLOCK, 0, 0, 0};
	ASSERT_EQ(mq_setattr(q, &set, NULL), 0);
	struct mq_attr got;
	ASSERT_EQ(mq_getattr(q, &got), 0);
	ASSERT_TRUE(got.mq_flags & O_NONBLOCK);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_ga_sa"), 0);
}

/* ============================================================================ */

TEST_SUITE(mq_setattr);

TEST(mq_setattr_toggle_flags) {
	mqd_t q = mq_open("/mq_sa1", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	struct mq_attr old;
	ASSERT_EQ(mq_getattr(q, &old), 0);
	struct mq_attr set = {O_NONBLOCK, 0, 0, 0};
	ASSERT_EQ(mq_setattr(q, &set, NULL), 0);
	struct mq_attr verify;
	ASSERT_EQ(mq_getattr(q, &verify), 0);
	ASSERT_TRUE(verify.mq_flags & O_NONBLOCK);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_sa1"), 0);
}

TEST(mq_setattr_immutable_fields) {
	mqd_t q = mq_open("/mq_sa2", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	struct mq_attr old;
	ASSERT_EQ(mq_getattr(q, &old), 0);
	struct mq_attr set = {0, 99, 99, 0};
	ASSERT_EQ(mq_setattr(q, &set, NULL), 0);
	struct mq_attr verify;
	ASSERT_EQ(mq_getattr(q, &verify), 0);
	ASSERT_EQ(verify.mq_maxmsg, old.mq_maxmsg);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_sa2"), 0);
}

TEST(mq_setattr_returns_old_attrs) {
	mqd_t q = mq_open("/mq_sa_old", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	struct mq_attr set = {O_NONBLOCK, 0, 0, 0};
	struct mq_attr old;
	ASSERT_EQ(mq_setattr(q, &set, &old), 0);
	ASSERT_FALSE(old.mq_flags & O_NONBLOCK);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_sa_old"), 0);
}

TEST(mq_setattr_only_changes_nonblock) {
	mqd_t q = mq_open("/mq_sa_only", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	struct mq_attr orig;
	ASSERT_EQ(mq_getattr(q, &orig), 0);
	struct mq_attr set = {O_NONBLOCK, 999, 999, 999};
	ASSERT_EQ(mq_setattr(q, &set, NULL), 0);
	struct mq_attr after;
	ASSERT_EQ(mq_getattr(q, &after), 0);
	ASSERT_TRUE(after.mq_flags & O_NONBLOCK);
	ASSERT_EQ(after.mq_maxmsg, orig.mq_maxmsg);
	ASSERT_EQ(after.mq_msgsize, orig.mq_msgsize);
	ASSERT_EQ(after.mq_curmsgs, orig.mq_curmsgs);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_sa_only"), 0);
}

TEST(mq_setattr_bad_descriptor) {
	struct mq_attr set = {0, 0, 0, 0};
	errno = 0;
	ASSERT_EQ(mq_setattr(-1, &set, NULL), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(mq_setattr_null_mqstat_fails) {
	mqd_t q = mq_open("/mq_sa_null", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	errno = 0;
	ASSERT_EQ(mq_setattr(q, NULL, NULL), -1);
	ASSERT_ERRNO(EINVAL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_sa_null"), 0);
}

TEST(mq_setattr_preserves_nonblock_across_operations) {
	mqd_t q = mq_open("/mq_sa_persist", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	struct mq_attr set = {O_NONBLOCK, 0, 0, 0};
	ASSERT_EQ(mq_setattr(q, &set, NULL), 0);
	/* Send/receive must respect the new NONBLOCK flag */
	char buf[4];
	errno = 0;
	ASSERT_EQ(mq_receive(q, buf, sizeof(buf), NULL), -1);
	ASSERT_ERRNO(EAGAIN);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_sa_persist"), 0);
}

/* ============================================================================ */

TEST_SUITE(mq_notify);

TEST(mq_notify_register_and_fire_signal) {
	g_mq_notify_got_sig = 0;
	signal(SIGUSR1, __mq_notify_handler_usr1);

	struct mq_attr attr = {O_NONBLOCK, 10, 64, 0};
	mqd_t q = mq_open("/mq_not_sig", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);

	struct sigevent sev = {0};
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGUSR1;
	ASSERT_EQ(mq_notify(q, &sev), 0);

	ASSERT_EQ(mq_send(q, "x", 1, 0), 0);
	usleep(50000);
	ASSERT_EQ(g_mq_notify_got_sig, 1);

	signal(SIGUSR1, SIG_DFL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_not_sig"), 0);
}

TEST(mq_notify_one_shot) {
	g_mq_notify_sig_count = 0;
	signal(SIGUSR2, __mq_notify_handler_usr2);

	struct mq_attr attr = {O_NONBLOCK, 10, 64, 0};
	mqd_t q = mq_open("/mq_not_once", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);

	struct sigevent sev = {0};
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGUSR2;
	ASSERT_EQ(mq_notify(q, &sev), 0);

	mq_send(q, "a", 1, 0);
	usleep(50000);
	ASSERT_EQ(g_mq_notify_sig_count, 1);

	mq_send(q, "b", 1, 0);
	usleep(50000);
	ASSERT_EQ(g_mq_notify_sig_count, 1);

	signal(SIGUSR2, SIG_DFL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_not_once"), 0);
}

TEST(mq_notify_auto_deregister_on_receive) {
	g_mq_notify_got_sig = 0;
	signal(SIGUSR1, __mq_notify_handler_usr1);

	struct mq_attr attr = {O_NONBLOCK, 10, 64, 0};
	mqd_t q = mq_open("/mq_not_recv", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);

	struct sigevent sev = {0};
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGUSR1;
	ASSERT_EQ(mq_notify(q, &sev), 0);

	char buf[4];
	mq_send(q, "pre", 3, 0);
	mq_receive(q, buf, sizeof(buf), NULL);
	g_mq_notify_got_sig = 0;

	mq_send(q, "post", 4, 0);
	usleep(50000);
	ASSERT_EQ(g_mq_notify_got_sig, 0);

	signal(SIGUSR1, SIG_DFL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_not_recv"), 0);
}

TEST(mq_notify_deregister_null_no_prior_registration) {
	mqd_t q = mq_open("/mq_not_null_nop", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);
	/* NULL with no prior registration is a no-op, returns 0 */
	ASSERT_EQ(mq_notify(q, NULL), 0);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_not_null_nop"), 0);
}

TEST(mq_notify_reregister_after_fire) {
	g_mq_notify_got_sig = 0;
	signal(SIGUSR1, __mq_notify_handler_usr1);

	struct mq_attr attr = {O_NONBLOCK, 10, 64, 0};
	mqd_t q = mq_open("/mq_not_refire", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);

	struct sigevent sev = {0};
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGUSR1;

	/* First registration and fire */
	ASSERT_EQ(mq_notify(q, &sev), 0);
	mq_send(q, "a", 1, 0);
	usleep(50000);
	ASSERT_EQ(g_mq_notify_got_sig, 1);

	/* Drain queue BEFORE re-registering so receive doesn't cancel new registration */
	char buf[4];
	mq_receive(q, buf, sizeof(buf), NULL);

	/* Re-register after auto-deregister must succeed */
	g_mq_notify_got_sig = 0;
	ASSERT_EQ(mq_notify(q, &sev), 0);

	/* Now send to empty queue triggers second notification */
	mq_send(q, "b", 1, 0);
	usleep(50000);
	ASSERT_EQ(g_mq_notify_got_sig, 1);

	signal(SIGUSR1, SIG_DFL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_not_refire"), 0);
}

TEST(mq_notify_timedreceive_auto_deregisters) {
	g_mq_notify_got_sig = 0;
	signal(SIGUSR1, __mq_notify_handler_usr1);

	struct mq_attr attr = {O_NONBLOCK, 10, 64, 0};
	mqd_t q = mq_open("/mq_not_trecv", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);

	struct sigevent sev = {0};
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGUSR1;
	ASSERT_EQ(mq_notify(q, &sev), 0);

	/* timedreceive must also auto-deregister */
	mq_send(q, "pre", 3, 0);
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += 1;
	char buf[4];
	mq_timedreceive(q, buf, sizeof(buf), NULL, &ts);
	g_mq_notify_got_sig = 0;

	mq_send(q, "post", 4, 0);
	usleep(50000);
	ASSERT_EQ(g_mq_notify_got_sig, 0);

	signal(SIGUSR1, SIG_DFL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_not_trecv"), 0);
}

TEST(mq_notify_timedsend_triggers) {
	g_mq_notify_got_sig = 0;
	signal(SIGUSR1, __mq_notify_handler_usr1);

	struct mq_attr attr = {O_NONBLOCK, 10, 64, 0};
	mqd_t q = mq_open("/mq_not_tsend", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);

	struct sigevent sev = {0};
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGUSR1;
	ASSERT_EQ(mq_notify(q, &sev), 0);

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += 1;
	ASSERT_EQ(mq_timedsend(q, "x", 1, 0, &ts), 0);
	usleep(50000);
	ASSERT_EQ(g_mq_notify_got_sig, 1);

	signal(SIGUSR1, SIG_DFL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_not_tsend"), 0);
}

TEST(mq_notify_nonempty_queue_does_not_fire) {
	g_mq_notify_got_sig = 0;
	signal(SIGUSR1, __mq_notify_handler_usr1);

	struct mq_attr attr = {O_NONBLOCK, 10, 64, 0};
	mqd_t q = mq_open("/mq_not_nonempty", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);

	/* Put a message in first so queue is NOT empty */
	mq_send(q, "existing", 8, 0);

	struct sigevent sev = {0};
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = SIGUSR1;
	ASSERT_EQ(mq_notify(q, &sev), 0);

	/* Send to non-empty queue must NOT trigger notification */
	mq_send(q, "second", 6, 0);
	usleep(50000);
	ASSERT_EQ(g_mq_notify_got_sig, 0);

	signal(SIGUSR1, SIG_DFL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_not_nonempty"), 0);
}

TEST(mq_notify_close_deregisters) {
	mqd_t q = mq_open("/mq_not_cls_dereg", O_CREAT | O_RDWR, 0644, NULL);
	ASSERT_TRUE(q >= 0);

	struct sigevent sev = {0};
	sev.sigev_notify = SIGEV_NONE;
	ASSERT_EQ(mq_notify(q, &sev), 0);
	ASSERT_EQ(mq_close(q), 0);

	/* Reopen same name; register must succeed because close deregistered */
	q = mq_open("/mq_not_cls_dereg", O_RDWR);
	ASSERT_TRUE(q >= 0);
	ASSERT_EQ(mq_notify(q, &sev), 0);

	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_not_cls_dereg"), 0);
}

TEST(mq_notify_sigev_thread) {
	g_mq_notify_thread_fired = 0;

	struct mq_attr attr = {O_NONBLOCK, 10, 64, 0};
	mqd_t q = mq_open("/mq_not_thread", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);

	struct sigevent sev = {0};
	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = __mq_notify_thread_fn;
	sev.sigev_notify_attributes = NULL;
	ASSERT_EQ(mq_notify(q, &sev), 0);

	mq_send(q, "x", 1, 0);
	usleep(100000);
	ASSERT_EQ(g_mq_notify_thread_fired, 1);

	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_not_thread"), 0);
}

TEST(mq_notify_bad_descriptor_null_sev) {
	errno = 0;
	ASSERT_EQ(mq_notify(-1, NULL), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(mq_notify_sigev_signal_default_signo) {
	g_mq_notify_got_sig = 0;
	signal(SIGUSR1, __mq_notify_handler_usr1);

	struct mq_attr attr = {O_NONBLOCK, 10, 64, 0};
	mqd_t q = mq_open("/mq_not_defsig", O_CREAT | O_RDWR, 0644, &attr);
	ASSERT_TRUE(q >= 0);

	struct sigevent sev = {0};
	sev.sigev_notify = SIGEV_SIGNAL;
	sev.sigev_signo = 0; /* Must default to SIGUSR1 */
	ASSERT_EQ(mq_notify(q, &sev), 0);

	mq_send(q, "x", 1, 0);
	usleep(50000);
	ASSERT_EQ(g_mq_notify_got_sig, 1);

	signal(SIGUSR1, SIG_DFL);
	ASSERT_EQ(mq_close(q), 0);
	ASSERT_EQ(mq_unlink("/mq_not_defsig"), 0);
}

/* ============================================================================ */

TEST_MAIN()
