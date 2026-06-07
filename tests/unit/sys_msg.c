/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <sys/msg.h>
#include <sys/ipc.h>
#include <testing.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

TEST_TYPE(unit);
TEST_UNIT(sys_msg.h);

struct test_msg {
	long mtype;
	char mtext[64];
};

static void __test_msg_cleanup(key_t key) {
	int id = msgget(key, 0);
	if (id >= 0) msgctl(id, IPC_RMID, NULL);
}

/* ============================================================================ */

TEST_SUITE(msgqnum_t);
TEST(msgqnum_t_size) {
	msgqnum_t x = 0;
	ASSERT_TRUE(sizeof(x) >= sizeof(unsigned short));
}

/* ============================================================================ */

TEST_SUITE(msglen_t);
TEST(msglen_t_size) {
	msglen_t x = 0;
	ASSERT_TRUE(sizeof(x) >= sizeof(unsigned short));
}

/* ============================================================================ */

TEST_SUITE(msqid_ds);
TEST(msqid_ds_init) {
	struct msqid_ds ds = {0};
	ds.msg_perm.uid = 1000;
	ds.msg_qbytes = 8192;
	ds.msg_qnum = 5;
	ASSERT_INT_EQ(ds.msg_perm.uid, 1000);
	ASSERT_INT_EQ(ds.msg_qbytes, 8192);
	ASSERT_INT_EQ(ds.msg_qnum, 5);
}

/* ============================================================================ */

TEST_SUITE(msgget);
TEST(msgget_create_private) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	ASSERT_TRUE(id >= 0);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgget_create_named) {
	__test_msg_cleanup(200001);
	int id = msgget(200001, IPC_CREAT | IPC_EXCL | 0600);
	ASSERT_TRUE(id >= 0);
	ASSERT_INT_EQ(msgget(200001, 0), id);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgget_excl_fails) {
	__test_msg_cleanup(200002);
	int id = msgget(200002, IPC_CREAT | 0600);
	errno = 0;
	ASSERT_INT_EQ(msgget(200002, IPC_CREAT | IPC_EXCL | 0600), -1);
	ASSERT_INT_EQ(errno, EEXIST);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgget_no_create_fails) {
	__test_msg_cleanup(200003);
	errno = 0;
	ASSERT_INT_EQ(msgget(200003, 0600), -1);
	ASSERT_INT_EQ(errno, ENOENT);
}
TEST(msgget_private_excl_always_creates) {
	int id1 = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0600);
	int id2 = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0600);
	ASSERT_TRUE(id1 >= 0);
	ASSERT_TRUE(id2 >= 0);
	ASSERT_TRUE(id1 != id2);
	msgctl(id1, IPC_RMID, NULL);
	msgctl(id2, IPC_RMID, NULL);
}
TEST(msgget_registry_exhaustion) {
	int ids[JACL_MSQ_MAX_QUEUES];
	int count = 0;
	for (int i = 0; i < JACL_MSQ_MAX_QUEUES; i++) {
		ids[i] = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
		if (ids[i] < 0) break;
		count++;
	}
	ASSERT_INT_EQ(count, JACL_MSQ_MAX_QUEUES);
	errno = 0;
	int extra = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	ASSERT_INT_EQ(extra, -1);
	ASSERT_INT_EQ(errno, ENOSPC);
	for (int i = 0; i < count; i++) {
		msgctl(ids[i], IPC_RMID, NULL);
	}
}

/* ============================================================================ */

TEST_SUITE(msgsnd);
TEST(msgsnd_basic) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct test_msg m = {1, "hello"};
	ASSERT_INT_EQ(msgsnd(id, &m, strlen(m.mtext) + 1, 0), 0);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgsnd_invalid_type) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct test_msg m = {0, "bad"};
	errno = 0;
	ASSERT_INT_EQ(msgsnd(id, &m, 4, 0), -1);
	ASSERT_INT_EQ(errno, EINVAL);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgsnd_exceeds_qbytes) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct msqid_ds ds;
	msgctl(id, IPC_STAT, &ds);
	ds.msg_qbytes = 10;
	msgctl(id, IPC_SET, &ds);

	struct test_msg m = {1, "this is way too long for 10 bytes"};
	errno = 0;
	ASSERT_INT_EQ(msgsnd(id, &m, strlen(m.mtext) + 1, IPC_NOWAIT), -1);
	ASSERT_INT_EQ(errno, EAGAIN);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgsnd_multiple_fifo) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct test_msg m1 = {1, "first"};
	struct test_msg m2 = {1, "second"};
	msgsnd(id, &m1, strlen(m1.mtext) + 1, 0);
	msgsnd(id, &m2, strlen(m2.mtext) + 1, 0);

	struct test_msg r1, r2;
	msgrcv(id, &r1, sizeof(r1.mtext), 0, 0);
	msgrcv(id, &r2, sizeof(r2.mtext), 0, 0);
	ASSERT_STR_EQ(r1.mtext, "first");
	ASSERT_STR_EQ(r2.mtext, "second");
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgsnd_eidrm) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	msgctl(id, IPC_RMID, NULL);
	struct test_msg m = {1, "dead"};
	errno = 0;
	ASSERT_INT_EQ(msgsnd(id, &m, 5, 0), -1);
	ASSERT_INT_EQ(errno, EINVAL);
}
TEST(msgsnd_zero_length) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct test_msg m = {1, ""};
	ASSERT_INT_EQ(msgsnd(id, &m, 0, 0), 0);

	struct test_msg r;
	ssize_t len = msgrcv(id, &r, sizeof(r.mtext), 0, 0);
	ASSERT_INT_EQ(len, 0);
	ASSERT_INT_EQ(r.mtype, 1);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgsnd_null_pointer) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	errno = 0;
	ASSERT_INT_EQ(msgsnd(id, NULL, 10, 0), -1);
	ASSERT_INT_EQ(errno, EINVAL);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgsnd_qnum_tracking) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct msqid_ds ds;
	msgctl(id, IPC_STAT, &ds);
	ASSERT_INT_EQ(ds.msg_qnum, 0);

	struct test_msg m = {1, "track"};
	msgsnd(id, &m, 6, 0);
	msgctl(id, IPC_STAT, &ds);
	ASSERT_INT_EQ(ds.msg_qnum, 1);

	msgsnd(id, &m, 6, 0);
	msgctl(id, IPC_STAT, &ds);
	ASSERT_INT_EQ(ds.msg_qnum, 2);

	struct test_msg r;
	msgrcv(id, &r, sizeof(r.mtext), 0, 0);
	msgctl(id, IPC_STAT, &ds);
	ASSERT_INT_EQ(ds.msg_qnum, 1);

	msgctl(id, IPC_RMID, NULL);
}

/* ============================================================================ */

TEST_SUITE(msgrcv);
TEST(msgrcv_basic) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct test_msg m1 = {1, "data1"};
	msgsnd(id, &m1, strlen(m1.mtext) + 1, 0);

	struct test_msg m2;
	ssize_t len = msgrcv(id, &m2, sizeof(m2.mtext), 0, 0);
	ASSERT_TRUE(len > 0);
	ASSERT_INT_EQ(m2.mtype, 1);
	ASSERT_STR_EQ(m2.mtext, "data1");
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgrcv_by_type) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct test_msg m1 = {1, "first"};
	struct test_msg m2 = {2, "second"};
	msgsnd(id, &m1, strlen(m1.mtext) + 1, 0);
	msgsnd(id, &m2, strlen(m2.mtext) + 1, 0);

	struct test_msg m3;
	msgrcv(id, &m3, sizeof(m3.mtext), 2, 0);
	ASSERT_INT_EQ(m3.mtype, 2);
	ASSERT_STR_EQ(m3.mtext, "second");
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgrcv_negative_type) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct test_msg m1 = {5, "five"};
	struct test_msg m2 = {3, "three"};
	msgsnd(id, &m1, strlen(m1.mtext) + 1, 0);
	msgsnd(id, &m2, strlen(m2.mtext) + 1, 0);

	struct test_msg m3;
	msgrcv(id, &m3, sizeof(m3.mtext), -5, 0);
	ASSERT_INT_EQ(m3.mtype, 3);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgrcv_lowest_negative_type) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct test_msg m1 = {4, "four"};
	struct test_msg m2 = {2, "two"};
	struct test_msg m3 = {1, "one"};
	msgsnd(id, &m1, strlen(m1.mtext) + 1, 0);
	msgsnd(id, &m2, strlen(m2.mtext) + 1, 0);
	msgsnd(id, &m3, strlen(m3.mtext) + 1, 0);

	struct test_msg m4;
	msgrcv(id, &m4, sizeof(m4.mtext), -5, 0);
	ASSERT_INT_EQ(m4.mtype, 1);
	ASSERT_STR_EQ(m4.mtext, "one");
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgrcv_e2big) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct test_msg m1 = {1, "long message"};
	msgsnd(id, &m1, strlen(m1.mtext) + 1, 0);

	struct test_msg m2;
	errno = 0;
	ASSERT_INT_EQ(msgrcv(id, &m2, 2, 0, 0), -1);
	ASSERT_INT_EQ(errno, E2BIG);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgrcv_msg_noerror) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct test_msg m1 = {1, "long message"};
	msgsnd(id, &m1, strlen(m1.mtext) + 1, 0);

	struct test_msg m2;
	ssize_t len = msgrcv(id, &m2, 4, 0, MSG_NOERROR);
	ASSERT_INT_EQ(len, 4);
	ASSERT_INT_EQ(memcmp(m2.mtext, "long", 4), 0);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgrcv_enomsg) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct test_msg m;
	errno = 0;
	ASSERT_INT_EQ(msgrcv(id, &m, sizeof(m.mtext), 0, IPC_NOWAIT), -1);
	ASSERT_INT_EQ(errno, ENOMSG);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgrcv_eidrm) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	msgctl(id, IPC_RMID, NULL);
	struct test_msg m;
	errno = 0;
	ASSERT_INT_EQ(msgrcv(id, &m, sizeof(m.mtext), 0, 0), -1);
	ASSERT_INT_EQ(errno, EINVAL);
}
TEST(msgrcv_fifo_within_type) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct test_msg m1 = {2, "A"};
	struct test_msg m2 = {1, "B"};
	struct test_msg m3 = {2, "C"};
	msgsnd(id, &m1, 2, 0);
	msgsnd(id, &m2, 2, 0);
	msgsnd(id, &m3, 2, 0);

	struct test_msg r1, r2;
	msgrcv(id, &r1, sizeof(r1.mtext), 2, 0);
	msgrcv(id, &r2, sizeof(r2.mtext), 2, 0);
	ASSERT_STR_EQ(r1.mtext, "A");
	ASSERT_STR_EQ(r2.mtext, "C");
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgrcv_null_pointer) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	errno = 0;
	ASSERT_INT_EQ(msgrcv(id, NULL, 10, 0, 0), -1);
	ASSERT_INT_EQ(errno, EINVAL);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgrcv_zero_length) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct test_msg m = {7, ""};
	msgsnd(id, &m, 0, 0);

	struct test_msg r;
	r.mtype = 0;
	ssize_t len = msgrcv(id, &r, 0, 0, 0);
	ASSERT_INT_EQ(len, 0);
	ASSERT_INT_EQ(r.mtype, 7);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgrcv_e2big_on_zero_msgsz) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct test_msg m = {7, "data"};
	msgsnd(id, &m, 4, 0);

	struct test_msg r;
	errno = 0;
	ssize_t len = msgrcv(id, &r, 0, 0, 0);
	ASSERT_INT_EQ(len, -1);
	ASSERT_INT_EQ(errno, E2BIG);
	msgctl(id, IPC_RMID, NULL);
}

/* ============================================================================ */

TEST_SUITE(msgctl);
TEST(msgctl_stat) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct msqid_ds ds;
	ASSERT_INT_EQ(msgctl(id, IPC_STAT, &ds), 0);
	ASSERT_INT_EQ(ds.msg_qbytes, JACL_MSQ_DEFAULT_QBYTES);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgctl_set) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct msqid_ds ds;
	msgctl(id, IPC_STAT, &ds);
	ds.msg_qbytes = 4096;
	ASSERT_INT_EQ(msgctl(id, IPC_SET, &ds), 0);

	struct msqid_ds ds2;
	msgctl(id, IPC_STAT, &ds2);
	ASSERT_INT_EQ(ds2.msg_qbytes, 4096);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgctl_rmid) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	ASSERT_INT_EQ(msgctl(id, IPC_RMID, NULL), 0);
	errno = 0;
	ASSERT_INT_EQ(msgctl(id, IPC_STAT, NULL), -1);
	ASSERT_INT_EQ(errno, EINVAL);
}
TEST(msgctl_timestamps) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct msqid_ds ds1;
	msgctl(id, IPC_STAT, &ds1);

	sleep(1);
	struct test_msg m = {1, "time"};
	msgsnd(id, &m, 5, 0);

	struct msqid_ds ds2;
	msgctl(id, IPC_STAT, &ds2);
	ASSERT_TRUE(ds2.msg_stime > ds1.msg_ctime);

	msgctl(id, IPC_RMID, NULL);
}
TEST(msgctl_info) {
	struct msqid_ds ds;
	ASSERT_INT_EQ(msgctl(0, IPC_INFO, &ds), 0);
	ASSERT_INT_EQ(ds.msg_qbytes, JACL_MSQ_MAX_QUEUES);
}
TEST(msgctl_rmid_clear_messages) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	struct test_msg m = {1, "data"};
	msgsnd(id, &m, 5, 0);

	struct msqid_ds ds;
	msgctl(id, IPC_STAT, &ds);
	ASSERT_INT_EQ(ds.msg_qnum, 1);

	ASSERT_INT_EQ(msgctl(id, IPC_RMID, NULL), 0);

	int id2 = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	msgctl(id2, IPC_STAT, &ds);
	ASSERT_INT_EQ(ds.msg_qnum, 0);
	msgctl(id2, IPC_RMID, NULL);
}
TEST(msgctl_stat_null_buffer) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	errno = 0;
	ASSERT_INT_EQ(msgctl(id, IPC_STAT, NULL), -1);
	ASSERT_INT_EQ(errno, EINVAL);
	msgctl(id, IPC_RMID, NULL);
}
TEST(msgctl_set_null_buffer) {
	int id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
	errno = 0;
	ASSERT_INT_EQ(msgctl(id, IPC_SET, NULL), -1);
	ASSERT_INT_EQ(errno, EINVAL);
	msgctl(id, IPC_RMID, NULL);
}

TEST_MAIN()
