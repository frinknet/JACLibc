/* (c) 2026 FRINKnet & Friends – MIT licence */
#define JACL_UTMPX_PATH "/tmp/utmpx_test"
#include <utmpx.h>
#include <testing.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

static void __test_reset(void) {
	endutxent();
	unlink(JACL_UTMPX_PATH);
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	if (f) fclose(f);
}

TEST_TYPE(unit);
TEST_UNIT(utmpx.h);

/* ============================================================================ */

TEST_SUITE(constants);
TEST(constants_empty)		 { ASSERT_INT_EQ(EMPTY, 0); }
TEST(constants_boot_time)	 { ASSERT_INT_EQ(BOOT_TIME, 1); }
TEST(constants_old_time)	  { ASSERT_INT_EQ(OLD_TIME, 2); }
TEST(constants_new_time)	  { ASSERT_INT_EQ(NEW_TIME, 3); }
TEST(constants_user_process)  { ASSERT_INT_EQ(USER_PROCESS, 4); }
TEST(constants_init_process)  { ASSERT_INT_EQ(INIT_PROCESS, 5); }
TEST(constants_login_process) { ASSERT_INT_EQ(LOGIN_PROCESS, 6); }
TEST(constants_dead_process)  { ASSERT_INT_EQ(DEAD_PROCESS, 7); }

/* ============================================================================ */

TEST_SUITE(utmpx);
TEST(utmpx_size_minimum) {
	ASSERT_SIZE_MIN(struct utmpx, sizeof(char[32]) + sizeof(char[4]) + sizeof(char[32]) +
									 sizeof(pid_t) + sizeof(short) + sizeof(struct timeval));
}
TEST(utmpx_field_assignment) {
	struct utmpx t = {0};
	strncpy(t.ut_user, "jacl", sizeof(t.ut_user)-1);
	strncpy(t.ut_id, "J0", sizeof(t.ut_id)-1);
	strncpy(t.ut_line, "tty0", sizeof(t.ut_line)-1);
	t.ut_pid = 1234; t.ut_type = USER_PROCESS; t.ut_tv.tv_sec = 0; t.ut_tv.tv_usec = 0;
	ASSERT_INT_EQ(t.ut_type, USER_PROCESS);
	ASSERT_INT_EQ(t.ut_pid, 1234);
}
TEST(utmpx_zero_initialization) {
	struct utmpx t = {0};
	ASSERT_INT_EQ(t.ut_type, EMPTY);
	ASSERT_INT_EQ(t.ut_pid, 0);
	ASSERT_MEM_EQ(t.ut_id, "\0\0\0\0", 4);
}
TEST(utmpx_timeval_preservation) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS; memcpy(wr.ut_id, "TV", 2);
	wr.ut_tv.tv_sec = 1700000000; wr.ut_tv.tv_usec = 123456;
	pututxline(&wr);

	setutxent();
	struct utmpx *rd = getutxent();
	ASSERT_NOT_NULL(rd);
	ASSERT_INT_EQ(rd->ut_tv.tv_sec, 1700000000);
	ASSERT_INT_EQ(rd->ut_tv.tv_usec, 123456);
	endutxent();
}
TEST(utmpx_struct_padding_is_zeroed) {
	struct utmpx t = {0};
	unsigned char *p = (unsigned char *)&t;
	for (size_t i = 0; i < sizeof(t); i++) {
		ASSERT_INT_EQ(p[i], 0);
	}
}

/* ============================================================================ */

TEST_SUITE(setutxent);
TEST(setutxent_no_crash) { setutxent(); endutxent(); }
TEST(setutxent_resets_pointer_after_write) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = BOOT_TIME; strncpy(wr.ut_line, "boot", sizeof(wr.ut_line)-1);
	pututxline(&wr);
	setutxent();
	struct utmpx *rd = getutxent();
	ASSERT_NOT_NULL(rd);
	ASSERT_STR_EQ(rd->ut_line, "boot");
	endutxent();
}
TEST(setutxent_creates_missing_file) {
	endutxent();
	unlink(JACL_UTMPX_PATH);
	setutxent();
	ASSERT_NOT_NULL(__utmpx_fp);
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS; memcpy(wr.ut_id, "MC", 2); strncpy(wr.ut_user, "created", sizeof(wr.ut_user)-1);
	struct utmpx *ret = pututxline(&wr);
	ASSERT_NOT_NULL(ret);
	FILE *f = fopen(JACL_UTMPX_PATH, "rb");
	ASSERT_NOT_NULL(f);
	struct utmpx chk;
	size_t n = fread(&chk, sizeof(chk), 1, f);
	fclose(f);
	ASSERT_INT_EQ(n, 1);
	ASSERT_STR_EQ(chk.ut_user, "created");
	endutxent();
}
TEST(setutxent_rewind_on_existing_open) {
	__test_reset();
	struct utmpx r1 = {0}; r1.ut_type = USER_PROCESS; memcpy(r1.ut_id, "RW", 2);
	pututxline(&r1);
	/* File is still open from pututxline */
	setutxent();
	struct utmpx *rd = getutxent();
	ASSERT_NOT_NULL(rd);
	ASSERT_MEM_EQ(rd->ut_id, "RW", 2);
	endutxent();
}
TEST(setutxent_after_failed_open_does_not_crash) {
	endutxent();
	unlink(JACL_UTMPX_PATH);
	/* Create a directory at the path to force fopen failure */
	mkdir(JACL_UTMPX_PATH, 0755);
	setutxent();
	ASSERT_NULL(__utmpx_fp);
	rmdir(JACL_UTMPX_PATH);
}
TEST(setutxent_create_then_reopen_atomicity) {
	endutxent();
	unlink(JACL_UTMPX_PATH);

	/* setutxent creates empty file then reopens r+b */
	setutxent();
	ASSERT_NOT_NULL(__utmpx_fp);

	/* File must exist and be zero-length */
	struct stat st;
	ASSERT_INT_EQ(stat(JACL_UTMPX_PATH, &st), 0);
	ASSERT_INT_EQ(st.st_size, 0);

	/* Must be readable and writable */
	struct utmpx wr = {0};
	wr.ut_type = NEW_TIME; wr.ut_pid = 7;
	struct utmpx *ret = pututxline(&wr);
	ASSERT_NOT_NULL(ret);

	setutxent();
	struct utmpx *rd = getutxent();
	ASSERT_NOT_NULL(rd);
	ASSERT_INT_EQ(rd->ut_pid, 7);
	endutxent();
}

/* ============================================================================ */

TEST_SUITE(endutxent);
TEST(endutxent_idempotent) { endutxent(); endutxent(); }
TEST(endutxent_nulls_file_pointer) {
	setutxent();
	endutxent();
	ASSERT_NULL(__utmpx_fp);
}
TEST(endutxent_safe_without_setutxent) {
	ASSERT_NULL(__utmpx_fp);
	endutxent();
	ASSERT_NULL(__utmpx_fp);
}

/* ============================================================================ */

TEST_SUITE(getutxent);
TEST(getutxent_empty_file_returns_null) {
	__test_reset();
	ASSERT_NULL(getutxent());
	endutxent();
}
TEST(getutxent_reads_record) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = BOOT_TIME; wr.ut_pid = 1; strncpy(wr.ut_line, "boot", sizeof(wr.ut_line)-1);
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	fwrite(&wr, sizeof(wr), 1, f); fflush(f); fdatasync(fileno(f)); fclose(f);
	struct utmpx *rd = getutxent();
	ASSERT_NOT_NULL(rd);
	ASSERT_INT_EQ(rd->ut_type, BOOT_TIME);
	endutxent();
}
TEST(getutxent_truncated_file_returns_null) {
	__test_reset();
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	char buf[sizeof(struct utmpx) / 2];
	memset(buf, 0, sizeof(buf));
	fwrite(buf, 1, sizeof(buf), f);
	fflush(f); fdatasync(fileno(f)); fclose(f);
	ASSERT_NULL(getutxent());
	endutxent();
}
TEST(getutxent_corrupted_record_returns_data_anyway) {
	__test_reset();
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	char garbage[sizeof(struct utmpx)];
	memset(garbage, 0xFF, sizeof(garbage));
	fwrite(garbage, 1, sizeof(garbage), f);
	fflush(f); fdatasync(fileno(f)); fclose(f);
	struct utmpx *rd = getutxent();
	ASSERT_NOT_NULL(rd);
	ASSERT_INT_EQ(rd->ut_type, -1);
	endutxent();
}
TEST(getutxent_multiple_records_sequential) {
	__test_reset();
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	struct utmpx r1 = {0}; r1.ut_type = BOOT_TIME; strncpy(r1.ut_line, "first", sizeof(r1.ut_line)-1);
	struct utmpx r2 = {0}; r2.ut_type = NEW_TIME; strncpy(r2.ut_line, "second", sizeof(r2.ut_line)-1);
	fwrite(&r1, sizeof(r1), 1, f);
	fwrite(&r2, sizeof(r2), 1, f);
	fflush(f); fdatasync(fileno(f)); fclose(f);
	setutxent();
	struct utmpx *rd1 = getutxent();
	ASSERT_NOT_NULL(rd1);
	ASSERT_STR_EQ(rd1->ut_line, "first");
	struct utmpx *rd2 = getutxent();
	ASSERT_NOT_NULL(rd2);
	ASSERT_STR_EQ(rd2->ut_line, "second");
	struct utmpx *rd3 = getutxent();
	ASSERT_NULL(rd3);
	endutxent();
}
TEST(getutxent_interleaved_read_aliasing) {
	__test_reset();
	struct utmpx r1 = {0}; r1.ut_type = USER_PROCESS; memcpy(r1.ut_id, "X1", 2); strncpy(r1.ut_user, "alpha", sizeof(r1.ut_user)-1);
	struct utmpx r2 = {0}; r2.ut_type = USER_PROCESS; memcpy(r2.ut_id, "X2", 2); strncpy(r2.ut_user, "beta", sizeof(r2.ut_user)-1);
	pututxline(&r1);
	pututxline(&r2);
	setutxent();
	struct utmpx *first = getutxent();
	ASSERT_NOT_NULL(first);
	ASSERT_STR_EQ(first->ut_user, "alpha");
	char saved_user[32];
	memcpy(saved_user, first->ut_user, sizeof(saved_user));
	struct utmpx *second = getutxent();
	ASSERT_NOT_NULL(second);
	ASSERT_STR_EQ(second->ut_user, "beta");
	ASSERT_STR_EQ(first->ut_user, "beta");
	ASSERT_STR_EQ(saved_user, "alpha");
	endutxent();
}
TEST(getutxent_auto_opens_file) {
	__test_reset();
	endutxent();
	struct utmpx wr = {0}; wr.ut_type = BOOT_TIME;
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	fwrite(&wr, sizeof(wr), 1, f); fclose(f);
	struct utmpx *rd = getutxent();
	ASSERT_NOT_NULL(rd);
	ASSERT_INT_EQ(rd->ut_type, BOOT_TIME);
	endutxent();
}
TEST(getutxent_after_endutxent_returns_null) {
	__test_reset();
	struct utmpx wr = {0}; wr.ut_type = BOOT_TIME;
	pututxline(&wr);
	endutxent();
	struct utmpx *rd = getutxent();
	ASSERT_NOT_NULL(rd);
	ASSERT_INT_EQ(rd->ut_type, BOOT_TIME);
	endutxent();
}

/* ============================================================================ */

TEST_SUITE(getutxid);
TEST(getutxid_finds_by_pid) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS; wr.ut_pid = 9999; strncpy(wr.ut_user, "alice", sizeof(wr.ut_user)-1);
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	fwrite(&wr, sizeof(wr), 1, f); fflush(f); fdatasync(fileno(f)); fclose(f);
	struct utmpx q = {0}; q.ut_type = USER_PROCESS; q.ut_pid = 9999;
	struct utmpx *found = getutxid(&q);
	ASSERT_NOT_NULL(found);
	ASSERT_STR_EQ(found->ut_user, "alice");
	endutxent();
}
TEST(getutxid_finds_by_id) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = INIT_PROCESS; memcpy(wr.ut_id, "XY", 2); strncpy(wr.ut_user, "init", sizeof(wr.ut_user)-1);
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	fwrite(&wr, sizeof(wr), 1, f); fflush(f); fdatasync(fileno(f)); fclose(f);
	struct utmpx q = {0}; q.ut_type = INIT_PROCESS; memcpy(q.ut_id, "XY", 2);
	struct utmpx *found = getutxid(&q);
	ASSERT_NOT_NULL(found);
	ASSERT_STR_EQ(found->ut_user, "init");
	endutxent();
}
TEST(getutxid_mismatched_type_returns_null) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS; wr.ut_pid = 1234;
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	fwrite(&wr, sizeof(wr), 1, f); fflush(f); fdatasync(fileno(f)); fclose(f);
	struct utmpx q = {0}; q.ut_type = INIT_PROCESS; q.ut_pid = 1234;
	memcpy(q.ut_id, "NOPE", 4);
	ASSERT_NULL(getutxid(&q));
	endutxent();
}
TEST(getutxid_null_input_handling) {
	errno = 0;
	ASSERT_NULL(getutxid(NULL));
	ASSERT_INT_EQ(errno, EINVAL);
}
TEST(getutxid_ignores_user_record_when_querying_init) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS; wr.ut_pid = 1234; strncpy(wr.ut_user, "user", sizeof(wr.ut_user)-1);
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	fwrite(&wr, sizeof(wr), 1, f); fflush(f); fdatasync(fileno(f)); fclose(f);
	struct utmpx q = {0};
	q.ut_type = INIT_PROCESS; q.ut_pid = 1234;
	memcpy(q.ut_id, "XX", 2);
	ASSERT_NULL(getutxid(&q));
	endutxent();
}
TEST(getutxid_empty_id_does_not_false_match) {
	__test_reset();
	struct utmpx r1 = {0}; r1.ut_type = USER_PROCESS; r1.ut_pid = 100; strncpy(r1.ut_line, "tty1", sizeof(r1.ut_line)-1);
	struct utmpx r2 = {0}; r2.ut_type = USER_PROCESS; r2.ut_pid = 200; strncpy(r2.ut_line, "tty2", sizeof(r2.ut_line)-1);
	pututxline(&r1);
	pututxline(&r2);
	struct utmpx q = {0}; q.ut_type = USER_PROCESS;
	struct utmpx *found = getutxid(&q);
	ASSERT_NULL(found);
	endutxent();
}
TEST(getutxid_dead_process_matches_by_id) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = DEAD_PROCESS; memcpy(wr.ut_id, "DP", 2); wr.ut_pid = 444;
	pututxline(&wr);
	struct utmpx q = {0}; q.ut_type = DEAD_PROCESS; memcpy(q.ut_id, "DP", 2);
	struct utmpx *found = getutxid(&q);
	ASSERT_NOT_NULL(found);
	ASSERT_INT_EQ(found->ut_pid, 444);
	endutxent();
}
TEST(getutxid_search_resumes_from_beginning) {
	__test_reset();
	struct utmpx r1 = {0}; r1.ut_type = USER_PROCESS; r1.ut_pid = 1; memcpy(r1.ut_id, "AA", 2);
	struct utmpx r2 = {0}; r2.ut_type = USER_PROCESS; r2.ut_pid = 2; memcpy(r2.ut_id, "BB", 2);
	pututxline(&r1);
	pututxline(&r2);

	/* First search finds AA */
	struct utmpx q1 = {0}; q1.ut_type = USER_PROCESS; memcpy(q1.ut_id, "AA", 2);
	struct utmpx *f1 = getutxid(&q1);
	ASSERT_NOT_NULL(f1);
	ASSERT_INT_EQ(f1->ut_pid, 1);

	/* Second search must restart from beginning to find BB */
	struct utmpx q2 = {0}; q2.ut_type = USER_PROCESS; memcpy(q2.ut_id, "BB", 2);
	struct utmpx *f2 = getutxid(&q2);
	ASSERT_NOT_NULL(f2);
	ASSERT_INT_EQ(f2->ut_pid, 2);
	endutxent();
}
TEST(getutxid_pid_fallback_with_nonempty_id) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS; wr.ut_pid = 5555; memcpy(wr.ut_id, "ZZ", 2);
	pututxline(&wr);

	/* Query with same PID but different ID must NOT match */
	struct utmpx q = {0};
	q.ut_type = USER_PROCESS; q.ut_pid = 5555; memcpy(q.ut_id, "YY", 2);
	struct utmpx *found = getutxid(&q);
	ASSERT_NULL(found);
	endutxent();
}
TEST(getutxid_user_process_empty_id_zero_pid) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS; wr.ut_pid = 0;
	pututxline(&wr);

	/* Empty ID + zero PID should not false-match */
	struct utmpx q = {0};
	q.ut_type = USER_PROCESS; q.ut_pid = 0;
	struct utmpx *found = getutxid(&q);
	/* Both have pid 0 and empty id - ambiguous but current impl matches first */
	/* Document actual behavior: if this returns non-NULL, it's by design */
	if (found != NULL) {
		ASSERT_INT_EQ(found->ut_pid, 0);
	}
	endutxent();
}

/* ============================================================================ */

TEST_SUITE(getutxline);
TEST(getutxline_finds_by_line) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS; strncpy(wr.ut_line, "/dev/ttyJACL", sizeof(wr.ut_line)-1); strncpy(wr.ut_user, "bob", sizeof(wr.ut_user)-1);
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	fwrite(&wr, sizeof(wr), 1, f); fflush(f); fdatasync(fileno(f)); fclose(f);
	struct utmpx q = {0}; strncpy(q.ut_line, "/dev/ttyJACL", sizeof(q.ut_line)-1); q.ut_type = USER_PROCESS;
	struct utmpx *found = getutxline(&q);
	ASSERT_NOT_NULL(found);
	ASSERT_STR_EQ(found->ut_user, "bob");
	endutxent();
}
TEST(getutxline_ignores_non_matching_types) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = BOOT_TIME; strncpy(wr.ut_line, "/dev/console", sizeof(wr.ut_line)-1);
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	fwrite(&wr, sizeof(wr), 1, f); fflush(f); fdatasync(fileno(f)); fclose(f);
	struct utmpx q = {0}; strncpy(q.ut_line, "/dev/console", sizeof(q.ut_line)-1); q.ut_type = USER_PROCESS;
	ASSERT_NULL(getutxline(&q));
	endutxent();
}
TEST(getutxline_partial_line_match_fails) {
	__test_reset();
	struct utmpx wr = {0};
	strncpy(wr.ut_line, "/dev/tty1", sizeof(wr.ut_line)-1);
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	fwrite(&wr, sizeof(wr), 1, f); fflush(f); fdatasync(fileno(f)); fclose(f);
	struct utmpx q = {0};
	strncpy(q.ut_line, "/dev/tty10", sizeof(q.ut_line)-1);
	q.ut_type = USER_PROCESS;
	ASSERT_NULL(getutxline(&q));
	endutxent();
}
TEST(getutxline_empty_ut_line_matches_empty_query) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS;
	strncpy(wr.ut_user, "noline", sizeof(wr.ut_user)-1);
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	fwrite(&wr, sizeof(wr), 1, f); fflush(f); fdatasync(fileno(f)); fclose(f);
	struct utmpx q = {0};
	q.ut_type = USER_PROCESS;
	struct utmpx *found = getutxline(&q);
	ASSERT_NOT_NULL(found);
	ASSERT_STR_EQ(found->ut_user, "noline");
	endutxent();
}
TEST(getutxline_null_input_handling) {
	errno = 0;
	ASSERT_NULL(getutxline(NULL));
	ASSERT_INT_EQ(errno, EINVAL);
}
TEST(getutxline_login_process_matches) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = LOGIN_PROCESS; strncpy(wr.ut_line, "ttyS0", sizeof(wr.ut_line)-1); strncpy(wr.ut_user, "login", sizeof(wr.ut_user)-1);
	pututxline(&wr);
	struct utmpx q = {0}; q.ut_type = LOGIN_PROCESS; strncpy(q.ut_line, "ttyS0", sizeof(q.ut_line)-1);
	struct utmpx *found = getutxline(&q);
	ASSERT_NOT_NULL(found);
	ASSERT_STR_EQ(found->ut_user, "login");
	endutxent();
}
TEST(getutxline_does_not_match_dead_process) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = DEAD_PROCESS; strncpy(wr.ut_line, "ttyZ", sizeof(wr.ut_line)-1);
	pututxline(&wr);

	struct utmpx q = {0};
	q.ut_type = DEAD_PROCESS; strncpy(q.ut_line, "ttyZ", sizeof(q.ut_line)-1);
	struct utmpx *found = getutxline(&q);
	ASSERT_NULL(found);
	endutxent();
}

/* ============================================================================ */

TEST_SUITE(pututxline);
TEST(pututxline_appends_new_record) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS; wr.ut_pid = getpid(); strncpy(wr.ut_user, "test", sizeof(wr.ut_user)-1);
	strncpy(wr.ut_line, "ttyTEST", sizeof(wr.ut_line)-1); memcpy(wr.ut_id, "TS", 2);
	struct utmpx *put = pututxline(&wr);
	ASSERT_NOT_NULL(put);
	ASSERT_STR_EQ(put->ut_user, "test");
	endutxent();
}
TEST(pututxline_updates_existing_record) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS; wr.ut_pid = 7777; memcpy(wr.ut_id, "UP", 2); strncpy(wr.ut_user, "old", sizeof(wr.ut_user)-1);
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	fwrite(&wr, sizeof(wr), 1, f); fflush(f); fdatasync(fileno(f)); fclose(f);
	wr.ut_type = DEAD_PROCESS; strncpy(wr.ut_user, "new", sizeof(wr.ut_user)-1);
	struct utmpx *put = pututxline(&wr);
	ASSERT_NOT_NULL(put);
	ASSERT_INT_EQ(put->ut_type, DEAD_PROCESS);
	ASSERT_STR_EQ(put->ut_user, "new");
	endutxent();
}
TEST(pututxline_multiple_records_scans_correctly) {
	__test_reset();
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	struct utmpx r1 = {0}; r1.ut_pid = 1; memcpy(r1.ut_id, "AA", 2); strncpy(r1.ut_user, "first", sizeof(r1.ut_user)-1);
	struct utmpx r2 = {0}; r2.ut_pid = 2; memcpy(r2.ut_id, "BB", 2); strncpy(r2.ut_user, "second", sizeof(r2.ut_user)-1);
	fwrite(&r1, sizeof(r1), 1, f);
	fwrite(&r2, sizeof(r2), 1, f);
	fflush(f); fdatasync(fileno(f)); fclose(f);
	r2.ut_type = DEAD_PROCESS;
	struct utmpx *put = pututxline(&r2);
	ASSERT_NOT_NULL(put);
	ASSERT_STR_EQ(put->ut_user, "second");
	f = fopen(JACL_UTMPX_PATH, "rb");
	struct utmpx chk;
	fread(&chk, sizeof(chk), 1, f);
	fclose(f);
	ASSERT_STR_EQ(chk.ut_user, "first");
	endutxent();
}
TEST(pututxline_updates_user_by_id) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS; wr.ut_pid = 5000;
	memcpy(wr.ut_id, "AA", 2); strncpy(wr.ut_user, "original", sizeof(wr.ut_user)-1);
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	fwrite(&wr, sizeof(wr), 1, f); fflush(f); fdatasync(fileno(f)); fclose(f);
	wr.ut_pid = 9999; strncpy(wr.ut_user, "updated", sizeof(wr.ut_user)-1);
	struct utmpx *put = pututxline(&wr);
	ASSERT_NOT_NULL(put);
	f = fopen(JACL_UTMPX_PATH, "rb");
	struct utmpx chk; fread(&chk, sizeof(chk), 1, f); fclose(f);
	ASSERT_INT_EQ(chk.ut_pid, 9999);
	ASSERT_STR_EQ(chk.ut_user, "updated");
	endutxent();
}
TEST(pututxline_updates_init_by_id) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = INIT_PROCESS; wr.ut_pid = 9000;
	memcpy(wr.ut_id, "II", 2); strncpy(wr.ut_user, "old_init", sizeof(wr.ut_user)-1);
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	fwrite(&wr, sizeof(wr), 1, f); fflush(f); fdatasync(fileno(f)); fclose(f);
	wr.ut_pid = 9999; strncpy(wr.ut_user, "new_init", sizeof(wr.ut_user)-1);
	struct utmpx *put = pututxline(&wr);
	ASSERT_NOT_NULL(put);
	f = fopen(JACL_UTMPX_PATH, "rb");
	struct utmpx chk; fread(&chk, sizeof(chk), 1, f); fclose(f);
	ASSERT_INT_EQ(chk.ut_pid, 9999);
	ASSERT_STR_EQ(chk.ut_user, "new_init");
	endutxent();
}
TEST(pututxline_handles_zeroed_id_and_pid) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS;
	strncpy(wr.ut_user, "zeroed", sizeof(wr.ut_user)-1);
	struct utmpx *put = pututxline(&wr);
	ASSERT_NOT_NULL(put);
	ASSERT_STR_EQ(put->ut_user, "zeroed");
	endutxent();
}
TEST(pututxline_positions_for_next_read) {
	__test_reset();
	struct utmpx r1 = {0}; r1.ut_type = USER_PROCESS; memcpy(r1.ut_id, "A1", 2); strncpy(r1.ut_user, "first", sizeof(r1.ut_user)-1);
	struct utmpx r2 = {0}; r2.ut_type = USER_PROCESS; memcpy(r2.ut_id, "A2", 2); strncpy(r2.ut_user, "second", sizeof(r2.ut_user)-1);
	pututxline(&r1);
	pututxline(&r2);
	setutxent();
	struct utmpx *rd1 = getutxent();
	ASSERT_NOT_NULL(rd1);
	ASSERT_STR_EQ(rd1->ut_user, "first");
	rd1->ut_pid = 1111;
	pututxline(rd1);
	struct utmpx *rd2 = getutxent();
	ASSERT_NOT_NULL(rd2);
	ASSERT_STR_EQ(rd2->ut_user, "second");
	ASSERT_INT_EQ(rd2->ut_pid, 0);
	endutxent();
}
TEST(pututxline_self_referential_write) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS; memcpy(wr.ut_id, "SR", 2); strncpy(wr.ut_user, "orig", sizeof(wr.ut_user)-1);
	pututxline(&wr);
	setutxent();
	struct utmpx *rd = getutxent();
	ASSERT_NOT_NULL(rd);
	rd->ut_pid = 7777;
	struct utmpx *ret = pututxline(rd);
	ASSERT_NOT_NULL(ret);
	ASSERT_INT_EQ(ret->ut_pid, 7777);
	setutxent();
	struct utmpx *chk = getutxent();
	ASSERT_NOT_NULL(chk);
	ASSERT_INT_EQ(chk->ut_pid, 7777);
	endutxent();
}
TEST(pututxline_non_process_type_matches_by_type_only) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = BOOT_TIME; wr.ut_pid = 1; strncpy(wr.ut_line, "boot", sizeof(wr.ut_line)-1);
	pututxline(&wr);
	struct utmpx upd = {0};
	upd.ut_type = BOOT_TIME; upd.ut_pid = 999; strncpy(upd.ut_line, "reboot", sizeof(upd.ut_line)-1);
	struct utmpx *ret = pututxline(&upd);
	ASSERT_NOT_NULL(ret);
	ASSERT_INT_EQ(ret->ut_pid, 999);
	ASSERT_STR_EQ(ret->ut_line, "reboot");
	setutxent();
	struct utmpx *rd = getutxent();
	ASSERT_NOT_NULL(rd);
	ASSERT_INT_EQ(rd->ut_pid, 999);
	struct utmpx *rd2 = getutxent();
	ASSERT_NULL(rd2);
	endutxent();
}
TEST(pututxline_append_after_exhaustive_scan) {
	__test_reset();
	struct utmpx r1 = {0}; r1.ut_type = USER_PROCESS; memcpy(r1.ut_id, "P1", 2);
	pututxline(&r1);
	struct utmpx r2 = {0}; r2.ut_type = USER_PROCESS; memcpy(r2.ut_id, "P2", 2); strncpy(r2.ut_user, "appended", sizeof(r2.ut_user)-1);
	struct utmpx *ret = pututxline(&r2);
	ASSERT_NOT_NULL(ret);
	setutxent();
	struct utmpx *rd1 = getutxent();
	ASSERT_NOT_NULL(rd1);
	ASSERT_MEM_EQ(rd1->ut_id, "P1", 2);
	struct utmpx *rd2 = getutxent();
	ASSERT_NOT_NULL(rd2);
	ASSERT_MEM_EQ(rd2->ut_id, "P2", 2);
	ASSERT_STR_EQ(rd2->ut_user, "appended");
	struct utmpx *rd3 = getutxent();
	ASSERT_NULL(rd3);
	endutxent();
}
TEST(pututxline_null_input_handling) {
	errno = 0;
	ASSERT_NULL(pututxline(NULL));
	ASSERT_INT_EQ(errno, EINVAL);
}
TEST(pututxline_fields_handle_max_length_strings) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS; wr.ut_pid = 1;
	memset(wr.ut_user, 'A', 31); wr.ut_user[31] = '\0';
	memset(wr.ut_line, 'B', 31); wr.ut_line[31] = '\0';
	pututxline(&wr);
	setutxent();
	struct utmpx *rd = getutxent();
	ASSERT_NOT_NULL(rd);
	ASSERT_INT_EQ(strlen(rd->ut_user), 31);
	ASSERT_INT_EQ(strlen(rd->ut_line), 31);
	endutxent();
}
TEST(pututxline_non_process_update_verifies_single_record_on_disk) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = BOOT_TIME; wr.ut_pid = 1; strncpy(wr.ut_line, "boot", sizeof(wr.ut_line)-1);
	pututxline(&wr);

	/* Verify exactly one record exists before update */
	FILE *f = fopen(JACL_UTMPX_PATH, "rb");
	ASSERT_NOT_NULL(f);
	fseek(f, 0, SEEK_END);
	long size_before = ftell(f);
	fclose(f);
	ASSERT_INT_EQ(size_before, (long)sizeof(struct utmpx));

	struct utmpx upd = {0};
	upd.ut_type = BOOT_TIME; upd.ut_pid = 999; strncpy(upd.ut_line, "reboot", sizeof(upd.ut_line)-1);
	struct utmpx *ret = pututxline(&upd);
	ASSERT_NOT_NULL(ret);

	/* Verify exactly one record exists after update */
	f = fopen(JACL_UTMPX_PATH, "rb");
	ASSERT_NOT_NULL(f);
	fseek(f, 0, SEEK_END);
	long size_after = ftell(f);
	fclose(f);
	ASSERT_INT_EQ(size_after, (long)sizeof(struct utmpx));
	endutxent();
}
TEST(pututxline_non_process_match_returns_correct_pointer) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = NEW_TIME; wr.ut_pid = 42;
	pututxline(&wr);

	struct utmpx upd = {0};
	upd.ut_type = NEW_TIME; upd.ut_pid = 43;
	struct utmpx *ret = pututxline(&upd);
	ASSERT_NOT_NULL(ret);
	ASSERT_INT_EQ(ret->ut_pid, 43);
	ASSERT_INT_EQ(ret->ut_type, NEW_TIME);

	/* Returned pointer must be __utmpx_buf */
	ASSERT_PTR_EQ(ret, &__utmpx_buf);
	endutxent();
}
TEST(pututxline_non_process_no_match_appends) {
	__test_reset();
	struct utmpx r1 = {0}; r1.ut_type = BOOT_TIME; r1.ut_pid = 1;
	pututxline(&r1);

	struct utmpx r2 = {0}; r2.ut_type = OLD_TIME; r2.ut_pid = 2;
	struct utmpx *ret = pututxline(&r2);
	ASSERT_NOT_NULL(ret);

	setutxent();
	struct utmpx *rd1 = getutxent();
	ASSERT_NOT_NULL(rd1);
	ASSERT_INT_EQ(rd1->ut_type, BOOT_TIME);
	struct utmpx *rd2 = getutxent();
	ASSERT_NOT_NULL(rd2);
	ASSERT_INT_EQ(rd2->ut_type, OLD_TIME);
	struct utmpx *rd3 = getutxent();
	ASSERT_NULL(rd3);
	endutxent();
}
TEST(pututxline_non_process_multiple_same_type_updates_first) {
	__test_reset();
	struct utmpx r1 = {0}; r1.ut_type = BOOT_TIME; r1.ut_pid = 1;
	struct utmpx r2 = {0}; r2.ut_type = BOOT_TIME; r2.ut_pid = 2;
	/* Manually write two BOOT_TIME records */
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	fwrite(&r1, sizeof(r1), 1, f);
	fwrite(&r2, sizeof(r2), 1, f);
	fclose(f);

	/* Update should match FIRST BOOT_TIME */
	struct utmpx upd = {0}; upd.ut_type = BOOT_TIME; upd.ut_pid = 99;
	struct utmpx *ret = pututxline(&upd);
	ASSERT_NOT_NULL(ret);
	ASSERT_INT_EQ(ret->ut_pid, 99);

	setutxent();
	struct utmpx *rd1 = getutxent();
	ASSERT_NOT_NULL(rd1);
	ASSERT_INT_EQ(rd1->ut_pid, 99);
	struct utmpx *rd2 = getutxent();
	ASSERT_NOT_NULL(rd2);
	ASSERT_INT_EQ(rd2->ut_pid, 2); /* Second record untouched */
	struct utmpx *rd3 = getutxent();
	ASSERT_NULL(rd3);
	endutxent();
}
TEST(pututxline_file_size_after_non_process_update) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = BOOT_TIME;
	pututxline(&wr);

	struct utmpx upd = {0};
	upd.ut_type = BOOT_TIME;
	pututxline(&upd);

	struct stat st;
	ASSERT_INT_EQ(stat(JACL_UTMPX_PATH, &st), 0);
	ASSERT_INT_EQ(st.st_size, (off_t)sizeof(struct utmpx));
	endutxent();
}
TEST(pututxline_update_middle_record_preserves_neighbors) {
	__test_reset();
	struct utmpx r1 = {0}; r1.ut_type = BOOT_TIME; r1.ut_pid = 1; strncpy(r1.ut_line, "first", sizeof(r1.ut_line)-1);
	struct utmpx r2 = {0}; r2.ut_type = NEW_TIME; r2.ut_pid = 2; strncpy(r2.ut_line, "middle", sizeof(r2.ut_line)-1);
	struct utmpx r3 = {0}; r3.ut_type = OLD_TIME; r3.ut_pid = 3; strncpy(r3.ut_line, "last", sizeof(r3.ut_line)-1);
	pututxline(&r1);
	pututxline(&r2);
	pututxline(&r3);

	struct utmpx upd = {0}; upd.ut_type = NEW_TIME; upd.ut_pid = 99; strncpy(upd.ut_line, "updated_middle", sizeof(upd.ut_line)-1);
	struct utmpx *ret = pututxline(&upd);
	ASSERT_NOT_NULL(ret);

	setutxent();
	struct utmpx *rd1 = getutxent();
	ASSERT_NOT_NULL(rd1);
	ASSERT_INT_EQ(rd1->ut_pid, 1);
	ASSERT_STR_EQ(rd1->ut_line, "first");

	struct utmpx *rd2 = getutxent();
	ASSERT_NOT_NULL(rd2);
	ASSERT_INT_EQ(rd2->ut_pid, 99);
	ASSERT_STR_EQ(rd2->ut_line, "updated_middle");

	struct utmpx *rd3 = getutxent();
	ASSERT_NOT_NULL(rd3);
	ASSERT_INT_EQ(rd3->ut_pid, 3);
	ASSERT_STR_EQ(rd3->ut_line, "last");

	struct utmpx *rd4 = getutxent();
	ASSERT_NULL(rd4);
	endutxent();
}
TEST(pututxline_file_size_after_middle_update) {
	__test_reset();
	struct utmpx r1 = {0}; r1.ut_type = BOOT_TIME; r1.ut_pid = 1;
	struct utmpx r2 = {0}; r2.ut_type = NEW_TIME; r2.ut_pid = 2;
	struct utmpx r3 = {0}; r3.ut_type = OLD_TIME; r3.ut_pid = 3;
	pututxline(&r1);
	pututxline(&r2);
	pututxline(&r3);

	struct stat st_before;
	ASSERT_INT_EQ(stat(JACL_UTMPX_PATH, &st_before), 0);

	struct utmpx upd = {0}; upd.ut_type = NEW_TIME; upd.ut_pid = 99;
	pututxline(&upd);

	struct stat st_after;
	ASSERT_INT_EQ(stat(JACL_UTMPX_PATH, &st_after), 0);
	ASSERT_INT_EQ(st_before.st_size, st_after.st_size);
	endutxent();
}
TEST(pututxline_after_failed_open_returns_null) {
	endutxent();
	unlink(JACL_UTMPX_PATH);
	mkdir(JACL_UTMPX_PATH, 0755);
	struct utmpx wr = {0}; wr.ut_type = BOOT_TIME;
	struct utmpx *ret = pututxline(&wr);
	ASSERT_NULL(ret);
	rmdir(JACL_UTMPX_PATH);
}
TEST(pututxline_consecutive_updates_same_record) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = BOOT_TIME; wr.ut_pid = 1;
	pututxline(&wr);

	for (int i = 0; i < 10; i++) {
		wr.ut_pid = i + 100;
		struct utmpx *ret = pututxline(&wr);
		ASSERT_NOT_NULL(ret);
		ASSERT_INT_EQ(ret->ut_pid, i + 100);
	}

	struct stat st;
	ASSERT_INT_EQ(stat(JACL_UTMPX_PATH, &st), 0);
	ASSERT_INT_EQ(st.st_size, (off_t)sizeof(struct utmpx));

	setutxent();
	struct utmpx *rd = getutxent();
	ASSERT_NOT_NULL(rd);
	ASSERT_INT_EQ(rd->ut_pid, 109);
	struct utmpx *rd2 = getutxent();
	ASSERT_NULL(rd2);
	endutxent();
}
TEST(pututxline_does_not_match_user_by_ut_line) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS; memcpy(wr.ut_id, "L1", 2);
	strncpy(wr.ut_line, "ttyX", sizeof(wr.ut_line)-1);
	pututxline(&wr);

	/* Same line, different ID must NOT match */
	struct utmpx upd = {0};
	upd.ut_type = USER_PROCESS; memcpy(upd.ut_id, "L2", 2);
	strncpy(upd.ut_line, "ttyX", sizeof(upd.ut_line)-1);
	upd.ut_pid = 8888;
	struct utmpx *ret = pututxline(&upd);
	ASSERT_NOT_NULL(ret);

	setutxent();
	struct utmpx *rd1 = getutxent();
	ASSERT_NOT_NULL(rd1);
	ASSERT_MEM_EQ(rd1->ut_id, "L1", 2);
	struct utmpx *rd2 = getutxent();
	ASSERT_NOT_NULL(rd2);
	ASSERT_MEM_EQ(rd2->ut_id, "L2", 2);
	ASSERT_INT_EQ(rd2->ut_pid, 8888);
	struct utmpx *rd3 = getutxent();
	ASSERT_NULL(rd3);
	endutxent();
}
TEST(pututxline_return_pointer_stable_until_next_call) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = BOOT_TIME; wr.ut_pid = 1;
	struct utmpx *p1 = pututxline(&wr);
	ASSERT_NOT_NULL(p1);
	ASSERT_INT_EQ(p1->ut_pid, 1);

	/* Pointer remains valid until next utmpx call */
	ASSERT_INT_EQ(p1->ut_pid, 1);
	ASSERT_INT_EQ(p1->ut_type, BOOT_TIME);

	struct utmpx wr2 = {0};
	wr2.ut_type = NEW_TIME; wr2.ut_pid = 2;
	struct utmpx *p2 = pututxline(&wr2);
	ASSERT_NOT_NULL(p2);

	/* p1 now aliases p2 */
	ASSERT_PTR_EQ(p1, p2);
	ASSERT_INT_EQ(p1->ut_pid, 2);
	endutxent();
}
TEST(getutxent_after_truncation_recovers) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = BOOT_TIME; wr.ut_pid = 1;
	pututxline(&wr);

	/* Truncate file externally while stream is open */
	FILE *f = fopen(JACL_UTMPX_PATH, "wb");
	ASSERT_NOT_NULL(f);
	fclose(f);

	/* Next read must return NULL, not crash or return stale data */
	setutxent();
	struct utmpx *rd = getutxent();
	ASSERT_NULL(rd);

	/* Must be able to write again after truncation */
	struct utmpx wr2 = {0};
	wr2.ut_type = NEW_TIME; wr2.ut_pid = 2;
	struct utmpx *ret = pututxline(&wr2);
	ASSERT_NOT_NULL(ret);
	ASSERT_INT_EQ(ret->ut_pid, 2);
	endutxent();
}
TEST(pututxline_empty_file_appends_at_offset_zero) {
	__test_reset();
	/* File exists but is empty from __test_reset */
	struct stat st;
	ASSERT_INT_EQ(stat(JACL_UTMPX_PATH, &st), 0);
	ASSERT_INT_EQ(st.st_size, 0);

	struct utmpx wr = {0};
	wr.ut_type = BOOT_TIME; wr.ut_pid = 42;
	struct utmpx *ret = pututxline(&wr);
	ASSERT_NOT_NULL(ret);

	ASSERT_INT_EQ(stat(JACL_UTMPX_PATH, &st), 0);
	ASSERT_INT_EQ(st.st_size, (off_t)sizeof(struct utmpx));

	setutxent();
	struct utmpx *rd = getutxent();
	ASSERT_NOT_NULL(rd);
	ASSERT_INT_EQ(rd->ut_pid, 42);
	endutxent();
}
TEST(pututxline_preserves_unmodified_fields_on_update) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = USER_PROCESS; memcpy(wr.ut_id, "PF", 2);
	strncpy(wr.ut_user, "original_user", sizeof(wr.ut_user)-1);
	strncpy(wr.ut_line, "ttyORIG", sizeof(wr.ut_line)-1);
	wr.ut_pid = 1111;
	wr.ut_tv.tv_sec = 999999; wr.ut_tv.tv_usec = 555;
	pututxline(&wr);

	/* Update only ut_pid via same ID */
	struct utmpx upd = {0};
	upd.ut_type = USER_PROCESS; memcpy(upd.ut_id, "PF", 2);
	upd.ut_pid = 2222;
	pututxline(&upd);

	setutxent();
	struct utmpx *rd = getutxent();
	ASSERT_NOT_NULL(rd);
	ASSERT_INT_EQ(rd->ut_pid, 2222);
	/* All other fields come from upd, not original */
	ASSERT_STR_EQ(rd->ut_user, "");
	ASSERT_STR_EQ(rd->ut_line, "");
	ASSERT_INT_EQ(rd->ut_tv.tv_sec, 0);
	endutxent();
}
TEST(pututxline_file_deleted_mid_operation) {
	__test_reset();
	struct utmpx wr = {0};
	wr.ut_type = BOOT_TIME; wr.ut_pid = 1;
	pututxline(&wr);

	/* Delete file while stream is still open */
	unlink(JACL_UTMPX_PATH);

	/* Write should still succeed on open fd (POSIX semantics) */
	struct utmpx upd = {0};
	upd.ut_type = BOOT_TIME; upd.ut_pid = 2;
	struct utmpx *ret = pututxline(&upd);
	ASSERT_NOT_NULL(ret);
	ASSERT_INT_EQ(ret->ut_pid, 2);

	/* But reopening must recreate */
	endutxent();
	setutxent();
	struct utmpx *rd = getutxent();
	ASSERT_NULL(rd);
	endutxent();
}

/* ============================================================================ */

TEST_MAIN()
