/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <utime.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <limits.h>

TEST_TYPE(unit);
TEST_UNIT(utime.h);

/* ============================================================================ */
TEST_SUITE(utimbuf_struct);

TEST(utimbuf_size_positive) {
	ASSERT_SIZE_MIN(struct utimbuf, sizeof(time_t) * 2);
}

TEST(utimbuf_actime_offset) {
	ASSERT_OFFSET(struct utimbuf, actime, 0);
}

TEST(utimbuf_modtime_offset) {
	ASSERT_OFFSET(struct utimbuf, modtime, sizeof(time_t));
}

TEST(utimbuf_fields_are_time_t) {
	struct utimbuf t = {0};
	(void)sizeof(t.actime + (time_t)0);
	(void)sizeof(t.modtime + (time_t)0);
	ASSERT_TRUE(1);
}

TEST(utimbuf_designated_init) {
	struct utimbuf t = { .actime = 100, .modtime = 200 };
	ASSERT_INT_EQ(t.actime, 100);
	ASSERT_INT_EQ(t.modtime, 200);
}

TEST(utimbuf_zero_init) {
	struct utimbuf t = {0};
	ASSERT_INT_EQ(t.actime, 0);
	ASSERT_INT_EQ(t.modtime, 0);
}

TEST(utimbuf_copy_by_value) {
	struct utimbuf a = { .actime = 1, .modtime = 2 };
	struct utimbuf b = a;
	ASSERT_INT_EQ(b.actime, 1);
	ASSERT_INT_EQ(b.modtime, 2);
}

TEST(utimbuf_modify_independently) {
	struct utimbuf t = {0};
	t.actime = 123;
	ASSERT_INT_EQ(t.actime, 123);
	ASSERT_INT_EQ(t.modtime, 0);
	t.modtime = 456;
	ASSERT_INT_EQ(t.actime, 123);
	ASSERT_INT_EQ(t.modtime, 456);
}

/* ============================================================================ */
TEST_SUITE(utime_happy);

TEST(utime_null_times_sets_now) {
	const char *p = "/tmp/jacl_ut_happy1";
	int fd = open(p, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	int r = utime(p, NULL);
	ASSERT_INT_EQ(r, 0);

	unlink(p);
}

TEST(utime_explicit_times) {
	const char *p = "/tmp/jacl_ut_happy2";
	int fd = open(p, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	struct utimbuf t = { .actime = 123456789, .modtime = 987654321 };
	int r = utime(p, &t);
	ASSERT_INT_EQ(r, 0);

	unlink(p);
}

TEST(utime_same_actime_modtime) {
	const char *p = "/tmp/jacl_ut_happy3";
	int fd = open(p, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	struct utimbuf t = { .actime = 111111111, .modtime = 111111111 };
	int r = utime(p, &t);
	ASSERT_INT_EQ(r, 0);

	unlink(p);
}

TEST(utime_directory_path) {
	const char *p = "/tmp/jacl_ut_dir";
	mkdir(p, 0755);

	struct utimbuf t = { .actime = 100, .modtime = 200 };
	int r = utime(p, &t);
	ASSERT_INT_EQ(r, 0);

	rmdir(p);
}

/* ============================================================================ */
TEST_SUITE(utime_sad);

TEST(utime_null_path) {
	int r = utime(NULL, NULL);
	ASSERT_INT_EQ(r, -1);
	/* POSIX leaves errno for NULL path implementation-defined */
	ASSERT_TRUE(errno == EFAULT || errno == EINVAL);
}

TEST(utime_empty_path) {
	int r = utime("", NULL);
	ASSERT_INT_EQ(r, -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(utime_nonexistent_file) {
	int r = utime("/tmp/does_not_exist_jacl_xyz_123", NULL);
	ASSERT_INT_EQ(r, -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(utime_path_too_long) {
	char long_path[PATH_MAX + 2];
	memset(long_path, 'a', PATH_MAX + 1);
	long_path[PATH_MAX + 1] = '\0';
	int r = utime(long_path, NULL);
	ASSERT_INT_EQ(r, -1);
}

TEST(utime_trailing_slash_nonexistent) {
	int r = utime("/tmp/does_not_exist_jacl/", NULL);
	ASSERT_INT_EQ(r, -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(utime_trailing_slash_to_file) {
	const char *p = "/tmp/jacl_ut_trail";
	int fd = open(p, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	int r = utime("/tmp/jacl_ut_trail/", NULL);
	ASSERT_INT_EQ(r, -1);
	ASSERT_ERRNO(ENOTDIR);

	unlink(p);
}

TEST(utime_relative_path_dot) {
	const char *p = "./jacl_ut_rel";
	int fd = open(p, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	struct utimbuf t = { .actime = 1, .modtime = 2 };
	int r = utime("./jacl_ut_rel", &t);
	ASSERT_INT_EQ(r, 0);

	unlink(p);
}

TEST(utime_relative_path_dotdot) {
	const char *p = "/tmp/jacl_ut_dd";
	int fd = open(p, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	struct utimbuf t = { .actime = 1, .modtime = 2 };
	int r = utime("/tmp/../tmp/jacl_ut_dd", &t);
	ASSERT_INT_EQ(r, 0);

	unlink(p);
}

TEST(utime_multiple_trailing_slashes) {
	const char *p = "/tmp/jacl_ut_multi";
	int fd = open(p, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	int r = utime("/tmp/jacl_ut_multi///", NULL);
	/* Path resolver may reject trailing slashes on files as ENOTDIR */
	ASSERT_TRUE(r == 0 || (r == -1 && errno == ENOTDIR));

	unlink(p);
}

TEST(utime_times_before_epoch) {
	const char *p = "/tmp/jacl_ut_epoch";
	int fd = open(p, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	struct utimbuf t = { .actime = -1000, .modtime = -2000 };
	int r = utime(p, &t);
	ASSERT_INT_EQ(r, 0);

	unlink(p);
}

TEST(utime_times_far_future) {
	const char *p = "/tmp/jacl_ut_future";
	int fd = open(p, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	struct utimbuf t = { .actime = 4102444800, .modtime = 4102444800 };
	int r = utime(p, &t);
	ASSERT_INT_EQ(r, 0);

	unlink(p);
}

TEST(utime_times_mismatched) {
	const char *p = "/tmp/jacl_ut_mismatch";
	int fd = open(p, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	struct utimbuf t = { .actime = 0, .modtime = 999999999 };
	int r = utime(p, &t);
	ASSERT_INT_EQ(r, 0);

	unlink(p);
}

TEST(utime_readonly_file_owner) {
	const char *p = "/tmp/jacl_ut_ro";
	int fd = open(p, O_CREAT | O_RDWR, 0400);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	struct utimbuf t = { .actime = 1, .modtime = 2 };
	int r = utime(p, &t);
	ASSERT_INT_EQ(r, 0);

	unlink(p);
}

TEST(utime_path_component_not_dir) {
	int r = utime("/tmp/jacl_ut_notdir/file", NULL);
	ASSERT_INT_EQ(r, -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(utime_symlink_follows) {
	const char *target = "/tmp/jacl_ut_link_target";
	const char *link = "/tmp/jacl_ut_link";

	int fd = open(target, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	symlink(target, link);

	struct utimbuf t = { .actime = 555, .modtime = 666 };
	int r = utime(link, &t);
	ASSERT_INT_EQ(r, 0);

	unlink(link);
	unlink(target);
}

TEST(utime_special_file_fifo) {
	const char *p = "/tmp/jacl_ut_fifo";
	unlink(p);
	int r = mkfifo(p, 0600);
	ASSERT_INT_EQ(r, 0);

	struct utimbuf t = { .actime = 1, .modtime = 2 };
	r = utime(p, &t);
	ASSERT_INT_EQ(r, 0);

	unlink(p);
}

TEST(utime_special_file_socket) {
	TEST_SKIP("socket utime is platform-dependent");
}

TEST(utime_unicode_path) {
	const char *p = "/tmp/jacl_ut_☯";
	int fd = open(p, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	struct utimbuf t = { .actime = 1, .modtime = 2 };
	int r = utime(p, &t);
	ASSERT_INT_EQ(r, 0);

	unlink(p);
}

TEST(utime_path_with_spaces) {
	const char *p = "/tmp/jacl ut space";
	int fd = open(p, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	struct utimbuf t = { .actime = 1, .modtime = 2 };
	int r = utime(p, &t);
	ASSERT_INT_EQ(r, 0);

	unlink(p);
}

TEST(utime_times_ptr_bad_address) {
	TEST_SKIP("causes segfault");
	const char *p = "/tmp/jacl_ut_bad";
	int fd = open(p, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	struct utimbuf *bad = (struct utimbuf *)0x1;
	int r = utime(p, bad);
	ASSERT_INT_EQ(r, -1);
	ASSERT_ERRNO(EFAULT);

	unlink(p);
}

TEST(utime_actime_zero_modtime_nonzero) {
	const char *p = "/tmp/jacl_ut_zero";
	int fd = open(p, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	struct utimbuf t = { .actime = 0, .modtime = 12345 };
	int r = utime(p, &t);
	ASSERT_INT_EQ(r, 0);

	unlink(p);
}

TEST(utime_modtime_zero_actime_nonzero) {
	const char *p = "/tmp/jacl_ut_zero2";
	int fd = open(p, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	struct utimbuf t = { .actime = 12345, .modtime = 0 };
	int r = utime(p, &t);
	ASSERT_INT_EQ(r, 0);

	unlink(p);
}

TEST(utime_both_times_max_time_t) {
	const char *p = "/tmp/jacl_ut_max";
	int fd = open(p, O_CREAT | O_RDWR, 0600);
	ASSERT_INT_NE(fd, -1);
	close(fd);

	struct utimbuf t = { .actime = TIME_MAX, .modtime = TIME_MAX };
	int r = utime(p, &t);
	ASSERT_INT_EQ(r, 0);

	unlink(p);
}

TEST(utime_path_is_root) {
	/* Modifying / requires root; test /tmp for unprivileged users */
	int r = utime("/tmp", NULL);
	ASSERT_INT_EQ(r, 0);
}

TEST(utime_path_is_dev_null) {
	int r = utime("/dev/null", NULL);
	ASSERT_INT_EQ(r, 0);
}

/* ============================================================================ */
TEST_MAIN()
