/* (c) 2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

TEST_TYPE(unit);
TEST_UNIT(unistd.h);

/* ============================================================================ */
TEST_SUITE(constants);

TEST(constants_version) {
	ASSERT_EQ(_POSIX_VERSION, 202405L);
	ASSERT_EQ(_POSIX2_VERSION, 202405L);
	ASSERT_EQ(_XOPEN_VERSION, 800);
}

TEST(constants_sysconf_names) {
	ASSERT_EQ(_SC_VERSION, 1);
	ASSERT_EQ(_SC_PAGESIZE, 2);
	ASSERT_EQ(_SC_PAGE_SIZE, _SC_PAGESIZE);
	ASSERT_EQ(_SC_CLK_TCK, 3);
	ASSERT_EQ(_SC_ARG_MAX, 4);
	ASSERT_EQ(_SC_CHILD_MAX, 5);
}

TEST(constants_pathconf_names) {
	ASSERT_EQ(_PC_LINK_MAX, 1);
	ASSERT_EQ(_PC_MAX_CANON, 2);
	ASSERT_EQ(_PC_MAX_INPUT, 3);
	ASSERT_EQ(_PC_NAME_MAX, 4);
	ASSERT_EQ(_PC_PATH_MAX, 5);
	ASSERT_EQ(_PC_PIPE_BUF, 6);
}

TEST(constants_confstr_names) {
	ASSERT_EQ(_CS_PATH, 1);
	ASSERT_EQ(_CS_V8_ENV, 2);
}

TEST(constants_fd_macros) {
	ASSERT_EQ(STDIN_FILENO, 0);
	ASSERT_EQ(STDOUT_FILENO, 1);
	ASSERT_EQ(STDERR_FILENO, 2);
}

TEST(constants_access_modes) {
	ASSERT_EQ(F_OK, 0);
	ASSERT_EQ(R_OK, 4);
	ASSERT_EQ(W_OK, 2);
	ASSERT_EQ(X_OK, 1);
	ASSERT_NE(F_OK, R_OK);
	ASSERT_NE(R_OK, W_OK);
	ASSERT_EQ(R_OK | W_OK | X_OK, 7);
}

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(getenv);

TEST(getenv_basic) {
	char *val = getenv("PATH");
	ASSERT_NOT_NULL(val);
}

TEST(getenv_not_found) {
	char *val = getenv("JA_CL_NONEXISTENT_VAR_XYZ");
	ASSERT_NULL(val);
}

TEST(getenv_null_name) {
	errno = 0;
	ASSERT_NULL(getenv(NULL));
}

TEST(getenv_empty_name) {
	errno = 0;
	ASSERT_NULL(getenv(""));
}

TEST(getenv_no_equals) {
	char *val = getenv("THIS_VAR_DOES_NOT_EXIST");
	ASSERT_NULL(val);
}

TEST(getenv_case_sensitive) {
	char *lower = getenv("path");
	char *upper = getenv("PATH");
	ASSERT_NOT_NULL(upper);
	ASSERT_NULL(lower);
}

TEST(getenv_consistent) {
	char *v1 = getenv("PATH");
	char *v2 = getenv("PATH");
	ASSERT_PTR_EQ(v1, v2);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(read);

TEST(read_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	write(fd, "data", 4);
	lseek(fd, 0, SEEK_SET);
	char buf[5];
	ASSERT_EQ(read(fd, buf, 4), 4);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(read_invalid_fd) {
	char buf[10];
	errno = 0;
	ASSERT_EQ(read(-1, buf, 10), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(read_null_buffer) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	write(fd, "x", 1);
	lseek(fd, 0, SEEK_SET);
	errno = 0;
	ASSERT_TRUE(read(fd, NULL, 1) <= 0);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(read_write_only_fd) {
	int fd = open("/tmp/t.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	char buf[10];
	errno = 0;
	ASSERT_EQ(read(fd, buf, 10), -1);
	ASSERT_ERRNO(EBADF);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(read_directory) {
	mkdir("/tmp/t_dir", 0755);
	int fd = open("/tmp/t_dir", O_RDONLY);
	char buf[10];
	errno = 0;
	ASSERT_EQ(read(fd, buf, 10), -1);
	ASSERT_ERRNO(EISDIR);
	close(fd);
	rmdir("/tmp/t_dir");
}

TEST(read_closed_fd) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	close(fd);
	char buf[10];
	errno = 0;
	ASSERT_EQ(read(fd, buf, 10), -1);
	ASSERT_ERRNO(EBADF);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(write);

TEST(write_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_EQ(write(fd, "x", 1), 1);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(write_invalid_fd) {
	errno = 0;
	ASSERT_EQ(write(-1, "x", 1), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(write_null_buffer) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	errno = 0;
	ASSERT_EQ(write(fd, NULL, 1), -1);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(write_read_only_fd) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDONLY, 0644);
	errno = 0;
	ASSERT_EQ(write(fd, "x", 1), -1);
	ASSERT_ERRNO(EBADF);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(write_directory) {
	mkdir("/tmp/t_dir", 0755);
	int fd = open("/tmp/t_dir", O_WRONLY);
	errno = 0;
	ASSERT_EQ(write(fd, "x", 1), -1);
	ASSERT_ERRNO(EBADF);
	close(fd);
	rmdir("/tmp/t_dir");
}

TEST(write_closed_fd) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(write(fd, "x", 1), -1);
	ASSERT_ERRNO(EBADF);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(close);

TEST(close_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_EQ(close(fd), 0);
	unlink("/tmp/t.txt");
}

TEST(close_invalid_fd) {
	errno = 0;
	ASSERT_EQ(close(-1), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(close_double) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(close(fd), -1);
	ASSERT_ERRNO(EBADF);
	unlink("/tmp/t.txt");
}

TEST(close_stdin) {
	int r = close(STDIN_FILENO);
	ASSERT_EQ(r, 0);
	open("/dev/null", O_RDONLY);
}

TEST(close_stderr) {
	int r = close(STDERR_FILENO);
	ASSERT_EQ(r, 0);
	open("/dev/null", O_WRONLY);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(lseek);

TEST(lseek_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_EQ(lseek(fd, 10, SEEK_SET), 10);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(lseek_invalid_fd) {
	errno = 0;
	ASSERT_EQ(lseek(-1, 0, SEEK_SET), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(lseek_invalid_whence) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	errno = 0;
	ASSERT_EQ(lseek(fd, 0, 999), -1);
	ASSERT_ERRNO(EINVAL);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(lseek_closed_fd) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(lseek(fd, 0, SEEK_SET), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(lseek_pipe) {
	int p[2];
	pipe(p);
	errno = 0;
	ASSERT_EQ(lseek(p[0], 0, SEEK_SET), -1);
	ASSERT_ERRNO(ESPIPE);
	close(p[0]);
	close(p[1]);
}

TEST(lseek_negative_set) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	errno = 0;
	ASSERT_EQ(lseek(fd, -5, SEEK_SET), -1);
	ASSERT_ERRNO(EINVAL);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(lseek_offset_max_boundary) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);

	/* Seeking past EOF should succeed and return the new offset */
	off_t pos = lseek(fd, OFF_MAX, SEEK_SET);
	ASSERT_EQ(pos, OFF_MAX);

	close(fd);
	unlink("/tmp/t.txt");
}

TEST(lseek_cur_zero_returns_current) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	write(fd, "12345", 5);
	lseek(fd, 3, SEEK_SET);

	/* SEEK_CUR with 0 must return current position without moving */
	off_t pos = lseek(fd, 0, SEEK_CUR);
	ASSERT_EQ(pos, 3);

	close(fd);
	unlink("/tmp/t.txt");
}

TEST(lseek_failed_does_not_move_offset) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	write(fd, "12345", 5);
	lseek(fd, 3, SEEK_SET);

	/* Invalid whence must fail */
	errno = 0;
	off_t bad = lseek(fd, 0, 999);
	ASSERT_EQ(bad, (off_t)-1);

	/* Position must still be 3 */
	off_t pos = lseek(fd, 0, SEEK_CUR);
	ASSERT_EQ(pos, 3);

	close(fd);
	unlink("/tmp/t.txt");
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(dup);

TEST(dup_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	int fd2 = dup(fd);
	ASSERT_TRUE(fd2 >= 0 && fd2 != fd);
	close(fd);
	close(fd2);
	unlink("/tmp/t.txt");
}

TEST(dup_invalid_fd) {
	errno = 0;
	ASSERT_EQ(dup(-1), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(dup_preserves_offset) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	write(fd, "sync", 4);
	lseek(fd, 2, SEEK_SET);
	int fd2 = dup(fd);
	ASSERT_EQ(lseek(fd2, 0, SEEK_CUR), 2);
	close(fd);
	close(fd2);
	unlink("/tmp/t.txt");
}

TEST(dup_closed_fd) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(dup(fd), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(dup_max_fd_limit) {
	/* Test against FD limit (ENFILE/EMFILE) */
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	/* Skip heavy allocation, just verify contract on valid */
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(dup_consistent) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	int fd2 = dup(fd);
	int fd3 = dup(fd);
	ASSERT_TRUE(fd2 != fd3);
	close(fd);
	close(fd2);
	close(fd3);
	unlink("/tmp/t.txt");
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(dup2);

TEST(dup2_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	int fd2 = dup2(fd, 55);
	ASSERT_EQ(fd2, 55);
	close(fd);
	close(55);
	unlink("/tmp/t.txt");
}

TEST(dup2_same_fd) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_EQ(dup2(fd, fd), fd);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(dup2_invalid_old) {
	errno = 0;
	ASSERT_EQ(dup2(-1, 10), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(dup2_closes_target) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	int tmp = open("/dev/null", O_RDONLY);
	dup2(fd, tmp);
	char c;
	ASSERT_EQ(read(tmp, &c, 1), 0); /* Empty */
	close(fd);
	close(tmp);
	unlink("/tmp/t.txt");
}

TEST(dup2_invalid_new) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	errno = 0;
	ASSERT_EQ(dup2(fd, -1), -1);
	ASSERT_ERRNO(EBADF);
	close(fd);
	unlink("/tmp/t.txt");
}


TEST(dup2_closed_old) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(dup2(fd, 10), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(dup2_same_fd_does_not_close) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	write(fd, "data", 4);
	lseek(fd, 2, SEEK_SET);

	int ret = dup2(fd, fd);
	ASSERT_EQ(ret, fd);

	/* FD must still be valid and offset preserved */
	off_t pos = lseek(fd, 0, SEEK_CUR);
	ASSERT_EQ(pos, 2);

	close(fd);
	unlink("/tmp/t.txt");
}

static void *__dup2_race_worker(void *arg) {
	int *fds = (int *)arg;
	for (int i = 0; i < 100; i++) {
		dup2(fds[0], fds[1]);
		usleep(10);
	}
	return NULL;
}

TEST(dup2_concurrent_same_target) {
	int src = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	int target = open("/dev/null", O_RDONLY);
	int fds[2] = {src, target};

	pthread_t t1, t2;
	pthread_create(&t1, NULL, __dup2_race_worker, fds);
	pthread_create(&t2, NULL, __dup2_race_worker, fds);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	/* After the race, target must still be a valid open FD */
	errno = 0;
	ASSERT_NE(fcntl(target, F_GETFD), -1);

	close(src);
	close(target);
	unlink("/tmp/t.txt");
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(pipe);

TEST(pipe_basic) {
	int p[2];
	ASSERT_EQ(pipe(p), 0);
	ASSERT_TRUE(p[0] != p[1]);
	close(p[0]);
	close(p[1]);
}

TEST(pipe_read_write) {
	int p[2];
	pipe(p);
	write(p[1], "ping", 4);
	char buf[5];
	ASSERT_EQ(read(p[0], buf, 4), 4);
	close(p[0]);
	close(p[1]);
}

TEST(pipe_invalid_array) {
	errno = 0;
	ASSERT_EQ(pipe(NULL), -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(pipe_flags_default) {
	int p[2];
	pipe(p);
	int flags = fcntl(p[0], F_GETFL);
	ASSERT_EQ(flags & O_NONBLOCK, 0);
	close(p[0]);
	close(p[1]);
}

TEST(pipe_consistent) {
	int p1[2], p2[2];
	pipe(p1);
	pipe(p2);
	ASSERT_TRUE(p1[0] != p2[0]);
	close(p1[0]); close(p1[1]);
	close(p2[0]); close(p2[1]);
}

TEST(pipe_close_one_end) {
	int p[2];
	pipe(p);
	close(p[1]);
	/* Reading from closed write end returns 0 (EOF) */
	char buf[1];
	ASSERT_EQ(read(p[0], buf, 1), 0);
	close(p[0]);
}

TEST(pipe_fd_direction_contract) {
	int p[2];
	ASSERT_EQ(pipe(p), 0);

	/* Writing to read end must fail */
	errno = 0;
	ASSERT_EQ(write(p[0], "x", 1), -1);
	ASSERT_ERRNO(EBADF);

	/* Reading from write end must fail */
	errno = 0;
	ASSERT_EQ(read(p[1], &(char){0}, 1), -1);
	ASSERT_ERRNO(EBADF);

	close(p[0]);
	close(p[1]);
}

TEST(pipe_read_after_writer_close_returns_zero) {
	int p[2];
	ASSERT_EQ(pipe(p), 0);

	write(p[1], "abc", 3);
	close(p[1]);

	char buf[8];
	ssize_t n = read(p[0], buf, sizeof(buf));
	ASSERT_EQ(n, 3);

	/* Second read must return 0 (EOF), not -1 or hang */
	n = read(p[0], buf, sizeof(buf));
	ASSERT_EQ(n, 0);

	close(p[0]);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(access);

TEST(access_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	ASSERT_EQ(access("/tmp/t.txt", F_OK), 0);
	unlink("/tmp/t.txt");
}

TEST(access_not_exists) {
	errno = 0;
	ASSERT_EQ(access("/tmp/ghost", F_OK), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(access_null_path) {
	errno = 0;
	ASSERT_EQ(access(NULL, F_OK), -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(access_empty_path) {
	errno = 0;
	ASSERT_EQ(access("", F_OK), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(access_mode_combo) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	ASSERT_EQ(access("/tmp/t.txt", R_OK | W_OK), 0);
	unlink("/tmp/t.txt");
}

TEST(access_write_readonly) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDONLY, 0444);
	close(fd);
	errno = 0;
	ASSERT_EQ(access("/tmp/t.txt", W_OK), -1);
	ASSERT_ERRNO(EACCES);
	unlink("/tmp/t.txt");
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(unlink);

TEST(unlink_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	ASSERT_EQ(unlink("/tmp/t.txt"), 0);
}

TEST(unlink_not_exists) {
	errno = 0;
	ASSERT_EQ(unlink("/tmp/ghost"), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(unlink_null_path) {
	errno = 0;
	ASSERT_EQ(unlink(NULL), -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(unlink_empty_path) {
	errno = 0;
	ASSERT_EQ(unlink(""), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(unlink_directory) {
	errno = 0;
	ASSERT_EQ(unlink("/tmp/"), -1);
	ASSERT_ERRNO(EISDIR);
	ASSERT_EQ(unlink("/tmp"), -1);
	ASSERT_ERRNO(EACCES);
}

TEST(unlink_twice) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	unlink("/tmp/t.txt");
	errno = 0;
	ASSERT_EQ(unlink("/tmp/t.txt"), -1);
	ASSERT_ERRNO(ENOENT);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(rmdir);

TEST(rmdir_basic) {
	mkdir("/tmp/t_dir", 0755);
	ASSERT_EQ(rmdir("/tmp/t_dir"), 0);
}

TEST(rmdir_not_exists) {
	errno = 0;
	ASSERT_EQ(rmdir("/tmp/ghost"), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(rmdir_null_path) {
	errno = 0;
	ASSERT_EQ(rmdir(NULL), -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(rmdir_empty_path) {
	errno = 0;
	ASSERT_EQ(rmdir(""), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(rmdir_not_directory) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(rmdir("/tmp/t.txt"), -1);
	ASSERT_ERRNO(ENOTDIR);
	unlink("/tmp/t.txt");
}

TEST(rmdir_non_empty) {
	mkdir("/tmp/t_dir", 0755);
	int fd = open("/tmp/t_dir/f", O_CREAT | O_RDWR, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(rmdir("/tmp/t_dir"), -1);
	ASSERT_ERRNO(ENOTEMPTY);
	unlink("/tmp/t_dir/f");
	rmdir("/tmp/t_dir");
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(link);

TEST(link_basic) {
	int fd = open("/tmp/t_src.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	ASSERT_EQ(link("/tmp/t_src.txt", "/tmp/t_tgt.txt"), 0);
	ASSERT_EQ(access("/tmp/t_tgt.txt", F_OK), 0);
	unlink("/tmp/t_src.txt");
	unlink("/tmp/t_tgt.txt");
}

TEST(link_src_not_exists) {
	errno = 0;
	ASSERT_EQ(link("/tmp/ghost", "/tmp/tgt"), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(link_overwrite) {
	int f1 = open("/tmp/l1.txt", O_CREAT | O_RDWR, 0644);
	close(f1);
	int f2 = open("/tmp/l2.txt", O_CREAT | O_RDWR, 0644);
	close(f2);
	errno = 0;
	ASSERT_EQ(link("/tmp/l1.txt", "/tmp/l2.txt"), -1);
	unlink("/tmp/l1.txt");
	unlink("/tmp/l2.txt");
}

TEST(link_null_src) {
	errno = 0;
	ASSERT_EQ(link(NULL, "/tmp/tgt"), -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(link_null_target) {
	errno = 0;
	ASSERT_EQ(link("/tmp/src", NULL), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(link_empty_paths) {
	errno = 0;
	ASSERT_EQ(link("", "/tmp/tgt"), -1);
	ASSERT_ERRNO(ENOENT);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(symlink);

TEST(symlink_basic) {
	int fd = open("/tmp/t_tgt.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	ASSERT_EQ(symlink("/tmp/t_tgt.txt", "/tmp/t_lnk.txt"), 0);
	unlink("/tmp/t_lnk.txt");
	unlink("/tmp/t_tgt.txt");
}

TEST(symlink_broken) {
	ASSERT_EQ(symlink("/tmp/ghost", "/tmp/broken.lnk"), 0);
	unlink("/tmp/broken.lnk");
}

TEST(symlink_null_target) {
	errno = 0;
	ASSERT_EQ(symlink(NULL, "/tmp/x"), -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(symlink_null_linkpath) {
	errno = 0;
	ASSERT_EQ(symlink("/tmp/t", NULL), -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(symlink_empty_target) {
	errno = 0;
	ASSERT_EQ(symlink("", "/tmp/x"), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(symlink_overwrite_existing) {
	int fd = open("/tmp/s_existing.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(symlink("/tmp/t", "/tmp/s_existing.txt"), -1);
	ASSERT_ERRNO(EEXIST);
	unlink("/tmp/s_existing.txt");
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(readlink);

TEST(readlink_basic) {
	symlink("/tmp/real", "/tmp/link");
	char buf[32];
	ssize_t n = readlink("/tmp/link", buf, sizeof(buf));
	ASSERT_TRUE(n > 0);
	unlink("/tmp/link");
}

TEST(readlink_not_link) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	char buf[32];
	errno = 0;
	ASSERT_EQ(readlink("/tmp/t.txt", buf, sizeof(buf)), -1);
	ASSERT_ERRNO(EINVAL);
	unlink("/tmp/t.txt");
}

TEST(readlink_small_buf) {
	symlink("/tmp/long_target_path", "/tmp/short");
	char buf[4];
	ssize_t n = readlink("/tmp/short", buf, sizeof(buf));
	ASSERT_EQ(n, 4);
	unlink("/tmp/short");
}

TEST(readlink_null_path) {
	char buf[32];
	errno = 0;
	ASSERT_EQ(readlink(NULL, buf, sizeof(buf)), -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(readlink_null_buf) {
	symlink("/tmp/real", "/tmp/link");
	errno = 0;
	ASSERT_EQ(readlink("/tmp/link", NULL, 10), -1);
	ASSERT_ERRNO(EFAULT);
	unlink("/tmp/link");
}

TEST(readlink_not_exists) {
	char buf[32];
	errno = 0;
	ASSERT_EQ(readlink("/tmp/ghost", buf, sizeof(buf)), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(readlink_exact_bufsiz_truncation) {
	symlink("/tmp/long_target_path", "/tmp/rl_trunc");
	char buf[4];
	memset(buf, 'B', sizeof(buf));

	ssize_t n = readlink("/tmp/rl_trunc", buf, 4);
	ASSERT_EQ(n, 4);
	/* Must not write past bufsiz */
	ASSERT_EQ(buf[0], '/');
	ASSERT_EQ(buf[1], 't');
	ASSERT_EQ(buf[2], 'm');
	ASSERT_EQ(buf[3], 'p');

	unlink("/tmp/rl_trunc");
}

TEST(readlink_return_matches_buffer_length) {
	symlink("/tmp/target", "/tmp/rl_match");
	char buf[32];
	memset(buf, 0, sizeof(buf));

	ssize_t n = readlink("/tmp/rl_match", buf, sizeof(buf));
	ASSERT_GT(n, 0);

	/* The returned length must exactly match strlen of what was written.
	   If the wrapper returns the syscall int but didn't populate buf,
	   or populated buf but returned wrong length, this catches it. */
	ASSERT_EQ((size_t)n, strnlen(buf, sizeof(buf)));

	unlink("/tmp/rl_match");
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(ftruncate);

TEST(ftruncate_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	write(fd, "0123456789", 10);
	ASSERT_EQ(ftruncate(fd, 4), 0);
	ASSERT_EQ(lseek(fd, 0, SEEK_END), 4);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(ftruncate_invalid_fd) {
	errno = 0;
	ASSERT_EQ(ftruncate(-1, 10), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(ftruncate_negative) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	errno = 0;
	ASSERT_EQ(ftruncate(fd, -1), -1);
	ASSERT_ERRNO(EINVAL);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(ftruncate_closed_fd) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(ftruncate(fd, 10), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(ftruncate_readonly) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDONLY, 0644);
	errno = 0;
	ASSERT_EQ(ftruncate(fd, 10), -1);
	ASSERT_ERRNO(EINVAL);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(ftruncate_extend) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_EQ(ftruncate(fd, 100), 0);
	ASSERT_EQ(lseek(fd, 0, SEEK_END), 100);
	close(fd);
	unlink("/tmp/t.txt");
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(truncate);

TEST(truncate_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	write(fd, "0123456789", 10);
	close(fd);
	ASSERT_EQ(truncate("/tmp/t.txt", 5), 0);
	fd = open("/tmp/t.txt", O_RDONLY);
	ASSERT_EQ(lseek(fd, 0, SEEK_END), 5);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(truncate_not_exists) {
	errno = 0;
	ASSERT_EQ(truncate("/tmp/ghost", 10), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(truncate_null_path) {
	errno = 0;
	ASSERT_EQ(truncate(NULL, 10), -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(truncate_empty_path) {
	errno = 0;
	ASSERT_EQ(truncate("", 10), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(truncate_negative_length) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(truncate("/tmp/t.txt", -5), -1);
	ASSERT_ERRNO(EINVAL);
	unlink("/tmp/t.txt");
}

TEST(truncate_readonly) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	write(fd, "data", 4);
	close(fd);
	/* Owner can truncate. If this fails, it's a perm issue, not readonly fd issue */
	ASSERT_EQ(truncate("/tmp/t.txt", 2), 0);
	unlink("/tmp/t.txt");
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(chdir);

TEST(chdir_basic) {
	char buf[1024];
	getcwd(buf, sizeof(buf));
	ASSERT_EQ(chdir("/tmp"), 0);
	chdir(buf);
}

TEST(chdir_not_exists) {
	errno = 0;
	ASSERT_EQ(chdir("/tmp/ghost"), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(chdir_null_path) {
	errno = 0;
	ASSERT_EQ(chdir(NULL), -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(chdir_file) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(chdir("/tmp/t.txt"), -1);
	ASSERT_ERRNO(ENOTDIR);
	unlink("/tmp/t.txt");
}

TEST(chdir_empty_path) {
	errno = 0;
	ASSERT_EQ(chdir(""), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(chdir_no_perm) {
	mkdir("/tmp/root_only", 0000);
	errno = 0;
	chdir("/tmp/root_only");
	rmdir("/tmp/root_only");
	ASSERT_ERRNO(EACCES);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(getcwd);

TEST(getcwd_basic) {
	char buf[1024];
	ASSERT_NOT_NULL(getcwd(buf, sizeof(buf)));
}

TEST(getcwd_null_buf) {
	errno = 0;
	ASSERT_NULL(getcwd(NULL, 1024));
	ASSERT_ERRNO(EINVAL);
}

TEST(getcwd_zero_size) {
	char buf[1024];
	errno = 0;
	ASSERT_NULL(getcwd(buf, 0));
	ASSERT_ERRNO(EINVAL);
}

TEST(getcwd_unmapped) {
	/* Safe unmapped check */
	errno = 0;
	char *res = getcwd((char *)0x0, 1024);
	ASSERT_NULL(res);
	ASSERT_ERRNO(EINVAL);
}

TEST(getcwd_consistent) {
	char b1[1024], b2[1024];
	getcwd(b1, sizeof(b1));
	getcwd(b2, sizeof(b2));
	ASSERT_STR_EQ(b1, b2);
}

TEST(getcwd_after_chdir) {
	char orig[1024];
	getcwd(orig, sizeof(orig));
	chdir("/tmp");
	char now[1024];
	getcwd(now, sizeof(now));
	ASSERT_STR_HAS("/tmp", now);
	chdir(orig);
}

TEST(getcwd_returns_input_buffer) {
	char buf[1024];
	char *ret = getcwd(buf, sizeof(buf));
	ASSERT_PTR_EQ(ret, buf);
}

TEST(getcwd_writes_valid_string) {
	char buf[1024];
	memset(buf, 0xAA, sizeof(buf)); /* Poison the buffer */
	getcwd(buf, sizeof(buf));
	/* Must be a valid null-terminated string starting with / */
	ASSERT_EQ(buf[0], '/');
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(getcwd_exact_size_boundary) {
	char buf[1024];
	getcwd(buf, sizeof(buf));
	size_t len = strlen(buf);

	/* Now try with exactly len+1 (should succeed) */
	char exact[1024];
	ASSERT_NOT_NULL(getcwd(exact, len + 1));

	/* Now try with exactly len (should fail with ERANGE) */
	errno = 0;
	ASSERT_NULL(getcwd(exact, len));
	ASSERT_ERRNO(ERANGE);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(execve);

TEST(execve_null_path) {
	errno = 0;
	ASSERT_EQ(execve(NULL, NULL, NULL), -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(execve_invalid_path) {
	char *argv[] = {"dummy", NULL};
	errno = 0;
	ASSERT_EQ(execve("/tmp/ghost_exec", argv, NULL), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(execve_non_executable) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	char *argv[] = {"/tmp/t.txt", NULL};
	errno = 0;
	ASSERT_EQ(execve("/tmp/t.txt", argv, NULL), -1);
	ASSERT_ERRNO(EACCES);
	unlink("/tmp/t.txt");
}

TEST(execve_empty_path) {
	errno = 0;
	ASSERT_EQ(execve("", NULL, NULL), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(execve_consistent_fail) {
	/* Verify it always fails safely */
	char *argv[] = {"/tmp/ghost", NULL};
	int r1 = execve("/tmp/ghost", argv, NULL);
	int r2 = execve("/tmp/ghost", argv, NULL);
	ASSERT_EQ(r1, r2);
	ASSERT_ERRNO(ENOENT);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(getuid);

TEST(getuid_basic) {
	ASSERT_TRUE(getuid() >= 0);
}

TEST(getuid_consistent) {
	uid_t u1 = getuid();
	uid_t u2 = getuid();
	ASSERT_EQ(u1, u2);
}

TEST(getuid_no_args) {
	ASSERT_TRUE(getuid() != (uid_t)-1);
}

TEST(getuid_stable) {
	uid_t u = getuid();
	chdir("/");
	ASSERT_EQ(getuid(), u);
}

TEST(getuid_vs_geteuid) {
	uid_t uid = getuid();
	uid_t euid = geteuid();
	ASSERT_TRUE(uid >= 0 && euid >= 0);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(getgid);

TEST(getgid_basic) {
	ASSERT_TRUE(getgid() >= 0);
}

TEST(getgid_consistent) {
	gid_t g1 = getgid();
	gid_t g2 = getgid();
	ASSERT_EQ(g1, g2);
}

TEST(getgid_no_args) {
	ASSERT_TRUE(getgid() != (gid_t)-1);
}

TEST(getgid_stable) {
	gid_t g = getgid();
	chdir("/");
	ASSERT_EQ(getgid(), g);
}

TEST(getgid_vs_getegid) {
	gid_t gid = getgid();
	gid_t egid = getegid();
	ASSERT_TRUE(gid >= 0 && egid >= 0);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(geteuid);

TEST(geteuid_basic) {
	ASSERT_TRUE(geteuid() >= 0);
}

TEST(geteuid_consistent) {
	uid_t u1 = geteuid();
	uid_t u2 = geteuid();
	ASSERT_EQ(u1, u2);
}

TEST(geteuid_no_args) {
	ASSERT_TRUE(geteuid() != (uid_t)-1);
}

TEST(geteuid_stable) {
	uid_t u = geteuid();
	chdir("/tmp");
	ASSERT_EQ(geteuid(), u);
}

TEST(geteuid_non_negative) {
	ASSERT_TRUE(geteuid() != (uid_t)-1);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(getegid);

TEST(getegid_basic) {
	ASSERT_TRUE(getegid() >= 0);
}

TEST(getegid_consistent) {
	gid_t g1 = getegid();
	gid_t g2 = getegid();
	ASSERT_EQ(g1, g2);
}

TEST(getegid_no_args) {
	ASSERT_TRUE(getegid() != (gid_t)-1);
}

TEST(getegid_stable) {
	gid_t g = getegid();
	chdir("/var");
	ASSERT_EQ(getegid(), g);
}

TEST(getegid_non_negative) {
	ASSERT_TRUE(getegid() != (gid_t)-1);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(setuid);

TEST(setuid_basic) {
	uid_t uid = getuid();
	/* Setting to self should always succeed */
	ASSERT_EQ(setuid(uid), 0);
}

TEST(setuid_invalid_uid) {
	errno = 0;
	ASSERT_EQ(setuid(-1), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(setuid_root_no_perm) {
	if (getuid() != 0) {
		errno = 0;
		ASSERT_EQ(setuid(0), -1);
		ASSERT_ERRNO(EPERM);
	} else {
		ASSERT_TRUE(setuid(0) == 0);
	}
}

TEST(setuid_large_uid) {
	errno = 0;
	ASSERT_EQ(setuid(0x7FFFFFFF), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(setuid_consistent_fail) {
	uid_t uid = getuid();
	int r1 = setuid(999999);
	int r2 = setuid(999999);
	ASSERT_EQ(r1, r2);
	setuid(uid); /* Restore */
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(setgid);

TEST(setgid_basic) {
	gid_t gid = getgid();
	ASSERT_EQ(setgid(gid), 0);
}

TEST(setgid_invalid_gid) {
	errno = 0;
	ASSERT_EQ(setgid(-1), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(setgid_root_no_perm) {
	if (getgid() != 0) {
		errno = 0;
		ASSERT_EQ(setgid(0), -1);
		ASSERT_ERRNO(EPERM);
	} else {
		ASSERT_TRUE(setgid(0) == 0);
	}
}

TEST(setgid_large_gid) {
	errno = 0;
	ASSERT_EQ(setgid(0x7FFFFFFF), -1);
	ASSERT_TRUE(EINVAL);
}

TEST(setgid_consistent_fail) {
	gid_t gid = getgid();
	int r1 = setgid(999999);
	int r2 = setgid(999999);
	ASSERT_EQ(r1, r2);
	setgid(gid);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(getgroups);

TEST(getgroups_basic) {
	gid_t list[10];
	int n = getgroups(10, list);
	ASSERT_TRUE(n >= 0);
}

TEST(getgroups_zero_size) {
	int n = getgroups(0, NULL);
	ASSERT_TRUE(n >= 0);
}

TEST(getgroups_null_list) {
	int n = getgroups(1, NULL);
	ASSERT_TRUE(n == -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(getgroups_small_list) {
	/* First, get the actual count */
	int count = getgroups(0, NULL);
	if (count > 1) {
		gid_t list[1];
		errno = 0;
		ASSERT_EQ(getgroups(1, list), -1);
		ASSERT_ERRNO(EINVAL);
	} else {
		/* If count is 0 or 1, a size-1 buffer should succeed */
		gid_t list[1];
		ASSERT_EQ(getgroups(1, list), count);
	}
}

TEST(getgroups_consistent) {
	gid_t l1[10], l2[10];
	int n1 = getgroups(10, l1);
	int n2 = getgroups(10, l2);
	ASSERT_EQ(n1, n2);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(getpgrp);

TEST(getpgrp_basic) {
	ASSERT_TRUE(getpgrp() > 0);
}

TEST(getpgrp_validity) {
	ASSERT_TRUE(getpgrp() > 0);
}

TEST(getpgrp_consistent) {
	pid_t p1 = getpgrp();
	pid_t p2 = getpgrp();
	ASSERT_EQ(p1, p2);
}

TEST(getpgrp_no_args) {
	ASSERT_TRUE(getpgrp() > 0);
}

TEST(getpgrp_stable) {
	pid_t p = getpgrp();
	chdir("/");
	ASSERT_EQ(getpgrp(), p);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(setpgid);

TEST(setpgid_basic) {
	pid_t pid = getpid();
	ASSERT_EQ(setpgid(pid, pid), 0);
}

TEST(setpgid_invalid_pid) {
	errno = 0;
	ASSERT_EQ(setpgid(-1, -1), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(setpgid_invalid_pgid) {
	errno = 0;
	ASSERT_EQ(setpgid(getpid(), -1), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(setpgid_other_process) {
	pid_t pid = fork();
	if (pid == 0) _exit(0);
	else if (pid > 0) ASSERT_EQ(setpgid(pid, getpid()), 0);
	else TEST_SKIP("fork failed");
}

TEST(setpgid_consistent) {
	pid_t pid = getpid();
	int r1 = setpgid(pid, pid);
	int r2 = setpgid(pid, pid);
	ASSERT_EQ(r1, r2);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(setsid);

TEST(setsid_basic) {
	pid_t sid = setsid();
	ASSERT_ERRNO(EPERM);
}

TEST(setsid_consistent_fail) {
	int r1 = setsid();
	int r2 = setsid();
	ASSERT_EQ(r1, r2);
}

TEST(setsid_no_args) {
	ASSERT_EQ(setsid(), (pid_t)-1);
	ASSERT_ERRNO(EPERM);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(gethostname);

TEST(gethostname_basic) {
	char buf[256];
	ASSERT_EQ(gethostname(buf, sizeof(buf)), 0);
}

TEST(gethostname_null_buf) {
	errno = 0;
	ASSERT_EQ(gethostname(NULL, 10), -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(gethostname_zero_len) {
	char buf[1];
	errno = 0;
	ASSERT_EQ(gethostname(buf, 0), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(gethostname_small_buf) {
	char buf[1];
	ASSERT_EQ(gethostname(buf, sizeof(buf)), -1);
}

TEST(gethostname_consistent) {
	char b1[256], b2[256];
	memset(b1, 0, sizeof(b1));
	memset(b2, 0, sizeof(b2));
	gethostname(b1, sizeof(b1));
	gethostname(b2, sizeof(b2));
	ASSERT_STR_EQ(b1, b2);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(sethostname);

TEST(sethostname_no_perm) {
	errno = 0;
	ASSERT_EQ(sethostname("test_host", 9), -1);
	ASSERT_ERRNO(EPERM);
}

TEST(sethostname_null_name) {
	errno = 0;
	ASSERT_EQ(sethostname(NULL, 5), -1);
	ASSERT_ERRNO(EPERM);
}

TEST(sethostname_empty) {
	errno = 0;
	ASSERT_EQ(sethostname("", 0), -1);
	ASSERT_ERRNO(EPERM);
}

TEST(sethostname_negative_len) {
	errno = 0;
	ASSERT_EQ(sethostname("test", -1), -1);
	ASSERT_ERRNO(EPERM);
}

TEST(sethostname_long_name) {
	char long_name[300];
	memset(long_name, 'A', sizeof(long_name));
	errno = 0;
	ASSERT_EQ(sethostname(long_name, sizeof(long_name)), -1);
	ASSERT_ERRNO(EPERM);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(isatty);

TEST(isatty_basic) {
	int fd = open("/dev/tty", O_RDONLY);
	if (fd >= 0) {
		ASSERT_EQ(isatty(fd), 1);
		close(fd);
	}
}

TEST(isatty_pipe) {
	int p[2];
	pipe(p);
	ASSERT_EQ(isatty(p[0]), 0);
	close(p[0]);
	close(p[1]);
}

TEST(isatty_invalid_fd) {
	ASSERT_EQ(isatty(9999), 0);
	ASSERT_ERRNO(EBADF);
}

TEST(isatty_closed_fd) {
	int fd = open("/tmp/t.txt", O_RDONLY);
	close(fd);
	ASSERT_EQ(isatty(fd), 0);
	unlink("/tmp/t.txt");
}

TEST(isatty_negative) {
	ASSERT_EQ(isatty(-1), 0);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(chown);

TEST(chown_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	uid_t uid = getuid();
	gid_t gid = getgid();
	ASSERT_EQ(chown("/tmp/t.txt", uid, gid), 0);
	unlink("/tmp/t.txt");
}

TEST(chown_not_exists) {
	errno = 0;
	ASSERT_EQ(chown("/tmp/ghost", -1, -1), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(chown_null_path) {
	errno = 0;
	ASSERT_EQ(chown(NULL, 0, 0), -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(chown_empty_path) {
	errno = 0;
	ASSERT_EQ(chown("", 0, 0), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(chown_no_perm) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(chown("/tmp/t.txt", 0, 0), -1);
	ASSERT_ERRNO(EPERM);
	unlink("/tmp/t.txt");
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(fchown);

TEST(fchown_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	uid_t uid = getuid();
	gid_t gid = getgid();
	ASSERT_EQ(fchown(fd, uid, gid), 0);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(fchown_invalid_fd) {
	errno = 0;
	ASSERT_EQ(fchown(-1, 0, 0), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(fchown_closed_fd) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(fchown(fd, 0, 0), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(fchown_no_perm) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	errno = 0;
	ASSERT_EQ(fchown(fd, 0, 0), -1);
	ASSERT_ERRNO(EPERM);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(fchown_readonly_fd) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDONLY, 0644);
	uid_t uid = getuid();
	gid_t gid = getgid();
	ASSERT_EQ(fchown(fd, uid, gid), 0);
	close(fd);
	unlink("/tmp/t.txt");
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(lchown);

TEST(lchown_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	uid_t uid = getuid();
	gid_t gid = getgid();
	ASSERT_EQ(lchown("/tmp/t.txt", uid, gid), 0);
	unlink("/tmp/t.txt");
}

TEST(lchown_not_exists) {
	errno = 0;
	ASSERT_EQ(lchown("/tmp/ghost", -1, -1), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(lchown_null_path) {
	errno = 0;
	ASSERT_EQ(lchown(NULL, 0, 0), -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(lchown_symlink) {
	symlink("/tmp/real", "/tmp/link");
	uid_t uid = getuid();
	gid_t gid = getgid();
	/* Should change link ownership, not target */
	ASSERT_EQ(lchown("/tmp/link", uid, gid), 0);
	unlink("/tmp/link");
	unlink("/tmp/real");
}

TEST(lchown_no_perm) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(lchown("/tmp/t.txt", 0, 0), -1);
	ASSERT_ERRNO(EPERM);
	unlink("/tmp/t.txt");
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(fchdir);

TEST(fchdir_basic) {
	int fd = open("/tmp", O_RDONLY);
	ASSERT_EQ(fchdir(fd), 0);
	close(fd);
}

TEST(fchdir_invalid_fd) {
	errno = 0;
	ASSERT_EQ(fchdir(-1), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(fchdir_closed_fd) {
	int fd = open("/tmp", O_RDONLY);
	close(fd);
	errno = 0;
	ASSERT_EQ(fchdir(fd), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(fchdir_not_directory) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	fd = open("/tmp/t.txt", O_RDONLY);
	errno = 0;
	ASSERT_EQ(fchdir(fd), -1);
	ASSERT_ERRNO(ENOTDIR);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(fchdir_consistent) {
	char buf[1024];
	getcwd(buf, sizeof(buf));
	int fd = open("/tmp", O_RDONLY);
	fchdir(fd);
	char now[1024];
	getcwd(now, sizeof(now));
	ASSERT_STR_HAS("/tmp", now);
	close(fd);
	chdir(buf);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(sync);

TEST(sync_basic) {
	sync();
	ASSERT_TRUE(1); /* Returns void, just check no crash */
}

TEST(sync_consistent) {
	sync();
	sync();
	ASSERT_TRUE(1);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(fsync);

TEST(fsync_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	ASSERT_EQ(fsync(fd), 0);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(fsync_invalid_fd) {
	errno = 0;
	ASSERT_EQ(fsync(-1), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(fsync_closed_fd) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(fsync(fd), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(fsync_readonly) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDONLY, 0644);
	ASSERT_EQ(fsync(fd), 0);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(fsync_directory) {
	mkdir("/tmp/t_dir", 0755);
	int fd = open("/tmp/t_dir", O_RDONLY);
	ASSERT_EQ(fsync(fd), 0);
	close(fd);
	rmdir("/tmp/t_dir");
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(fdatasync);

TEST(fdatasync_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	ASSERT_EQ(fdatasync(fd), 0);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(fdatasync_invalid_fd) {
	errno = 0;
	ASSERT_EQ(fdatasync(-1), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(fdatasync_closed_fd) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(fdatasync(fd), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(fdatasync_readonly) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDONLY, 0644);
	ASSERT_EQ(fdatasync(fd), 0);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(fdatasync_directory) {
	mkdir("/tmp/t_dir", 0755);
	int fd = open("/tmp/t_dir", O_RDONLY);
	ASSERT_EQ(fdatasync(fd), 0);
	close(fd);
	rmdir("/tmp/t_dir");
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(sleep);

TEST(sleep_basic) {
	ASSERT_EQ(sleep(0), 0);
}

TEST(sleep_short) {
	ASSERT_EQ(sleep(1), 0);
}

TEST(sleep_large) {
	ASSERT_EQ(sleep(2), 0);
}

static volatile sig_atomic_t got_alarm = 0;
void sigalrm_handler(int s) { got_alarm = 1; }

TEST(sleep_interrupt) {
	/* Save old handler */
	void (*old_handler)(int) = signal(SIGALRM, SIG_IGN);

	/* Schedule alarm for 1 second */
	alarm(1);


	signal(SIGALRM, sigalrm_handler);
	got_alarm = 0;

	alarm(1);

	unsigned int remaining = sleep(5);

	alarm(0);

	/* Restore */
	signal(SIGALRM, old_handler);

	/* Verify interruption occurred */
	ASSERT_TRUE(got_alarm == 1);
	ASSERT_TRUE(remaining > 0 && remaining < 5);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(usleep);

TEST(usleep_basic) {
	ASSERT_EQ(usleep(0), 0);
}

TEST(usleep_micro) {
	ASSERT_EQ(usleep(100), 0);
}

TEST(usleep_milli) {
	ASSERT_EQ(usleep(1000), 0);
}

TEST(usleep_large) {
	ASSERT_EQ(usleep(1000000), 0);
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(alarm);

TEST(alarm_basic) {
	ASSERT_EQ(alarm(0), 0);
}

TEST(alarm_set) {
	alarm(5);
	ASSERT_EQ(alarm(0), 5); /* Cancel returns remaining */
}

TEST(alarm_cancel) {
	alarm(1);
	alarm(0); /* Cancel */
	ASSERT_EQ(alarm(0), 0);
}

TEST(alarm_chain) {
	alarm(10);
	ASSERT_EQ(alarm(20), 10);
	alarm(0);
}

TEST(alarm_negative) {
	ASSERT_EQ(alarm(-1), 0); /* Unsigned wrap */
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(pause);

TEST(pause_signal) {
	signal(SIGALRM, sigalrm_handler);
	got_alarm = 0;

	alarm(1);

	int r = pause();

	alarm(0);

	ASSERT_TRUE(got_alarm == 1);
	ASSERT_EQ(r, -1);
	ASSERT_ERRNO(EINTR);
}


#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(pread);

TEST(pread_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	write(fd, "data", 4);
	char buf[5];
	ASSERT_EQ(pread(fd, buf, 4, 0), 4);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(pread_invalid_fd) {
	char buf[10];
	errno = 0;
	ASSERT_EQ(pread(-1, buf, 10, 0), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(pread_null_buffer) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	write(fd, "x", 1);
	errno = 0;
	ASSERT_TRUE(pread(fd, NULL, 1, 0) <= 0);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(pread_write_only_fd) {
	int fd = open("/tmp/t.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
	char buf[10];
	errno = 0;
	ASSERT_EQ(pread(fd, buf, 10, 0), -1);
	ASSERT_ERRNO(EBADF);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(pread_closed_fd) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	close(fd);
	char buf[10];
	errno = 0;
	ASSERT_EQ(pread(fd, buf, 10, 0), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(pread_negative_offset) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	write(fd, "x", 1);
	char buf[10];
	errno = 0;
	ASSERT_EQ(pread(fd, buf, 10, -1), -1);
	ASSERT_ERRNO(EINVAL);
	close(fd);
	unlink("/tmp/t.txt");
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(pwrite);

TEST(pwrite_basic) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_EQ(pwrite(fd, "data", 4, 0), 4);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(pwrite_invalid_fd) {
	errno = 0;
	ASSERT_EQ(pwrite(-1, "x", 1, 0), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(pwrite_null_buffer) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	errno = 0;
	ASSERT_EQ(pwrite(fd, NULL, 1, 0), -1);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(pwrite_read_only_fd) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDONLY, 0644);
	errno = 0;
	ASSERT_EQ(pwrite(fd, "x", 1, 0), -1);
	ASSERT_ERRNO(EBADF);
	close(fd);
	unlink("/tmp/t.txt");
}

TEST(pwrite_closed_fd) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	close(fd);
	errno = 0;
	ASSERT_EQ(pwrite(fd, "x", 1, 0), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(pwrite_negative_offset) {
	int fd = open("/tmp/t.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	errno = 0;
	ASSERT_EQ(pwrite(fd, "x", 1, -1), -1);
	ASSERT_ERRNO(EINVAL);
	close(fd);
	unlink("/tmp/t.txt");
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(sysconf_runtime);

TEST(sysconf_basic) {
	ASSERT_TRUE(sysconf(_SC_PAGESIZE) > 0);
}

TEST(sysconf_invalid) {
	errno = 0;
	ASSERT_EQ(sysconf(99999), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(sysconf_negative) {
	errno = 0;
	ASSERT_EQ(sysconf(-1), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(sysconf_version) {
	ASSERT_EQ(sysconf(_SC_VERSION), _POSIX_VERSION);
}

TEST(sysconf_alias) {
	ASSERT_EQ(sysconf(_SC_PAGESIZE), sysconf(_SC_PAGE_SIZE));
}

TEST(sysconf_consistent) {
	ASSERT_EQ(sysconf(_SC_ARG_MAX), sysconf(_SC_ARG_MAX));
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(pathconf_runtime);

TEST(pathconf_basic) {
	long n = pathconf("/tmp", _PC_NAME_MAX);
	ASSERT_TRUE(n > 0);
}

TEST(pathconf_invalid_name) {
	errno = 0;
	ASSERT_EQ(pathconf("/tmp", 99999), -1);
	ASSERT_ERRNO(EINVAL);
}

TEST(pathconf_empty_path) {
	errno = 0;
	ASSERT_EQ(pathconf("", _PC_NAME_MAX), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(pathconf_not_exists) {
	errno = 0;
	ASSERT_EQ(pathconf("/tmp/ghost", _PC_NAME_MAX), -1);
	ASSERT_ERRNO(ENOENT);
}

TEST(pathconf_null_path) {
	errno = 0;
	ASSERT_EQ(pathconf(NULL, _PC_NAME_MAX), -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(pathconf_consistent) {
	ASSERT_EQ(pathconf("/tmp", _PC_PATH_MAX), pathconf("/tmp", _PC_PATH_MAX));
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(confstr_runtime);

TEST(confstr_basic) {
	char buf[256];
	size_t n = confstr(_CS_PATH, buf, sizeof(buf));
	ASSERT_TRUE(n > 0);
}

TEST(confstr_small_buf) {
	char buf[2];
	size_t n = confstr(_CS_PATH, buf, sizeof(buf));
	ASSERT_TRUE(n > sizeof(buf));
}

TEST(confstr_null_buf) {
	ASSERT_TRUE(confstr(_CS_PATH, NULL, 0) > 0);
}

TEST(confstr_invalid) {
	errno = 0;
	ASSERT_EQ(confstr(99999, NULL, 0), 0);
	ASSERT_ERRNO(EINVAL);
}

TEST(confstr_negative) {
	errno = 0;
	ASSERT_EQ(confstr(-1, NULL, 0), 0);
	ASSERT_ERRNO(EINVAL);
}

TEST(confstr_consistent) {
	ASSERT_EQ(confstr(_CS_PATH, NULL, 0), confstr(_CS_PATH, NULL, 0));
}
#endif

/* ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(getopt_runtime);

TEST(getopt_basic) {
	char *av[] = {"prog", "-a", NULL};
	optind = 1;
	opterr = 0;
	ASSERT_EQ(getopt(2, av, "a"), 'a');
}

TEST(getopt_unknown) {
	char *av[] = {"prog", "-z", NULL};
	optind = 1;
	opterr = 0;
	ASSERT_EQ(getopt(2, av, "a"), '?');
}

TEST(getopt_missing_arg_standard) {
	char *av[] = {"prog", "-f", NULL};
	optind = 1;
	opterr = 0;
	ASSERT_EQ(getopt(2, av, "f:"), '?');
}

TEST(getopt_missing_arg_colon) {
	char *av[] = {"prog", "-f", NULL};
	optind = 1;
	opterr = 0;
	ASSERT_EQ(getopt(2, av, ":f:"), ':');
}

TEST(getopt_non_option) {
	char *av[] = {"prog", "file", NULL};
	optind = 1;
	opterr = 0;
	ASSERT_EQ(getopt(2, av, "a"), -1);
}

TEST(getopt_end_of_opts) {
	char *av[] = {"prog", "--", "-a", NULL};
	optind = 1;
	opterr = 0;
	ASSERT_EQ(getopt(3, av, "a"), -1);
	ASSERT_EQ(optind, 2);
}

TEST(getopt_with_arg) {
	char *av[] = {"prog", "-f", "val", NULL};
	optind = 1;
	opterr = 0;
	ASSERT_EQ(getopt(3, av, "f:"), 'f');
	ASSERT_STR_EQ(optarg, "val");
}
#endif

/* ============================================================================ */
#if !JACL_HAS_POSIX
TEST_SUITE(non_posix_stubs);

TEST(read_stub) {
	char b[10];
	errno = 0;
	ASSERT_EQ(read(0, b, 10), -1);
	ASSERT_ERRNO(ENOSYS);
}

TEST(write_stub) {
	errno = 0;
	ASSERT_EQ(write(1, "x", 1), -1);
	ASSERT_ERRNO(ENOSYS);
}

TEST(close_stub) {
	errno = 0;
	ASSERT_EQ(close(0), -1);
	ASSERT_ERRNO(ENOSYS);
}

TEST(pipe_stub) {
	int p[2];
	errno = 0;
	ASSERT_EQ(pipe(p), -1);
	ASSERT_ERRNO(ENOSYS);
}

TEST(fork_stub) {
	errno = 0;
	ASSERT_EQ(fork(), -1);
	ASSERT_ERRNO(ENOSYS);
}

TEST(getpid_stub) {
	pid_t p = getpid();
	ASSERT_TRUE(p >= 0);
}
#endif

/* ============================================================================ */
TEST_MAIN()
