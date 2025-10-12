/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(unistd.h);

/* ============================================================================
 * CONSTANTS
 * ============================================================================ */
TEST_SUITE(constants);

TEST(standard_file_descriptors) {
	ASSERT_EQ(0, STDIN_FILENO);
	ASSERT_EQ(1, STDOUT_FILENO);
	ASSERT_EQ(2, STDERR_FILENO);
}

TEST(access_modes) {
	ASSERT_EQ(0, F_OK);
	ASSERT_EQ(4, R_OK);
	ASSERT_EQ(2, W_OK);
	ASSERT_EQ(1, X_OK);
}

/* ============================================================================
 * PROCESS IDS
 * ============================================================================ */
TEST_SUITE(process_ids);

TEST(getpid_returns_value) {
	pid_t pid = getpid();
	ASSERT_TRUE(pid > 0);
}

TEST(getpid_consistent) {
	pid_t pid1 = getpid();
	pid_t pid2 = getpid();
	
	ASSERT_EQ(pid1, pid2);
}

TEST(getppid_returns_value) {
	pid_t ppid = getppid();
	ASSERT_TRUE(ppid > 0);
}

/* ============================================================================
 * USER/GROUP IDS
 * ============================================================================ */
TEST_SUITE(user_group_ids);

TEST(getuid_returns_value) {
	uid_t uid = getuid();
	ASSERT_TRUE(uid >= 0);
}

TEST(getgid_returns_value) {
	gid_t gid = getgid();
	ASSERT_TRUE(gid >= 0);
}

TEST(geteuid_returns_value) {
	uid_t euid = geteuid();
	ASSERT_TRUE(euid >= 0);
}

TEST(getegid_returns_value) {
	gid_t egid = getegid();
	ASSERT_TRUE(egid >= 0);
}

TEST(uid_consistency) {
	uid_t uid1 = getuid();
	uid_t uid2 = getuid();
	
	ASSERT_EQ(uid1, uid2);
}

/* ============================================================================
 * FILE I/O - BASIC
 * ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(file_io_basic);

TEST(read_from_file) {
	int fd = open("/tmp/unistd_test_read.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_TRUE(fd >= 0);
	
	write(fd, "test", 4);
	lseek(fd, 0, SEEK_SET);
	
	char buf[10];
	ssize_t n = read(fd, buf, 4);
	
	ASSERT_EQ(4, n);
	ASSERT_EQ(0, memcmp(buf, "test", 4));
	
	close(fd);
	unlink("/tmp/unistd_test_read.txt");
}

TEST(write_to_file) {
	int fd = open("/tmp/unistd_test_write.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_TRUE(fd >= 0);
	
	ssize_t n = write(fd, "hello", 5);
	
	ASSERT_EQ(5, n);
	
	close(fd);
	unlink("/tmp/unistd_test_write.txt");
}

TEST(close_file) {
	int fd = open("/tmp/unistd_test_close.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_TRUE(fd >= 0);
	
	int result = close(fd);
	
	ASSERT_EQ(0, result);
	
	unlink("/tmp/unistd_test_close.txt");
}

TEST(lseek_basic) {
	int fd = open("/tmp/unistd_test_lseek.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	write(fd, "0123456789", 10);
	
	off_t pos = lseek(fd, 5, SEEK_SET);
	ASSERT_EQ(5, pos);
	
	char c;
	read(fd, &c, 1);
	ASSERT_EQ('5', c);
	
	close(fd);
	unlink("/tmp/unistd_test_lseek.txt");
}

TEST(lseek_seek_end) {
	int fd = open("/tmp/unistd_test_lseek2.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	write(fd, "test", 4);
	
	off_t pos = lseek(fd, 0, SEEK_END);
	ASSERT_EQ(4, pos);
	
	close(fd);
	unlink("/tmp/unistd_test_lseek2.txt");
}
#endif

/* ============================================================================
 * FILE DESCRIPTORS
 * ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(file_descriptors);

TEST(dup_basic) {
	int fd = open("/tmp/unistd_test_dup.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_TRUE(fd >= 0);
	
	int fd2 = dup(fd);
	ASSERT_TRUE(fd2 >= 0);
	ASSERT_NE(fd, fd2);
	
	close(fd);
	close(fd2);
	unlink("/tmp/unistd_test_dup.txt");
}

TEST(dup2_basic) {
	int fd = open("/tmp/unistd_test_dup2.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_TRUE(fd >= 0);
	
	int fd2 = dup2(fd, 100);
	ASSERT_EQ(100, fd2);
	
	close(fd);
	close(fd2);
	unlink("/tmp/unistd_test_dup2.txt");
}

TEST(pipe_basic) {
	int pipefd[2];
	int result = pipe(pipefd);
	
	ASSERT_EQ(0, result);
	ASSERT_TRUE(pipefd[0] >= 0);
	ASSERT_TRUE(pipefd[1] >= 0);
	
	close(pipefd[0]);
	close(pipefd[1]);
}

TEST(pipe_read_write) {
	int pipefd[2];
	pipe(pipefd);
	
	write(pipefd[1], "data", 4);
	
	char buf[10];
	ssize_t n = read(pipefd[0], buf, 4);
	
	ASSERT_EQ(4, n);
	ASSERT_EQ(0, memcmp(buf, "data", 4));
	
	close(pipefd[0]);
	close(pipefd[1]);
}
#endif

/* ============================================================================
 * FILE OPERATIONS
 * ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(file_operations);

TEST(access_file_exists) {
	int fd = open("/tmp/unistd_test_access.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	close(fd);
	
	int result = access("/tmp/unistd_test_access.txt", F_OK);
	ASSERT_EQ(0, result);
	
	unlink("/tmp/unistd_test_access.txt");
}

TEST(access_file_not_exists) {
	int result = access("/tmp/nonexistent_file_xyz.txt", F_OK);
	ASSERT_EQ(-1, result);
}

TEST(unlink_file) {
	int fd = open("/tmp/unistd_test_unlink.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	close(fd);
	
	int result = unlink("/tmp/unistd_test_unlink.txt");
	ASSERT_EQ(0, result);
	
	// Verify file is gone
	ASSERT_EQ(-1, access("/tmp/unistd_test_unlink.txt", F_OK));
}

TEST(link_file) {
	int fd = open("/tmp/unistd_test_link1.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	close(fd);
	
	int result = link("/tmp/unistd_test_link1.txt", "/tmp/unistd_test_link2.txt");
	ASSERT_EQ(0, result);
	
	ASSERT_EQ(0, access("/tmp/unistd_test_link2.txt", F_OK));
	
	unlink("/tmp/unistd_test_link1.txt");
	unlink("/tmp/unistd_test_link2.txt");
}

TEST(symlink_basic) {
	int fd = open("/tmp/unistd_test_symlink_target.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	close(fd);
	
	int result = symlink("/tmp/unistd_test_symlink_target.txt", "/tmp/unistd_test_symlink.txt");
	ASSERT_EQ(0, result);
	
	unlink("/tmp/unistd_test_symlink.txt");
	unlink("/tmp/unistd_test_symlink_target.txt");
}

TEST(readlink_basic) {
	int fd = open("/tmp/unistd_test_readlink_target.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	close(fd);
	
	symlink("/tmp/unistd_test_readlink_target.txt", "/tmp/unistd_test_readlink.txt");
	
	char buf[256];
	ssize_t n = readlink("/tmp/unistd_test_readlink.txt", buf, sizeof(buf));
	
	ASSERT_TRUE(n > 0);
	
	unlink("/tmp/unistd_test_readlink.txt");
	unlink("/tmp/unistd_test_readlink_target.txt");
}

TEST(truncate_file) {
	int fd = open("/tmp/unistd_test_truncate.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	write(fd, "0123456789", 10);
	close(fd);
	
	int result = truncate("/tmp/unistd_test_truncate.txt", 5);
	ASSERT_EQ(0, result);
	
	// Verify size
	fd = open("/tmp/unistd_test_truncate.txt", O_RDONLY);
	lseek(fd, 0, SEEK_END);
	off_t size = lseek(fd, 0, SEEK_CUR);
	close(fd);
	
	ASSERT_EQ(5, size);
	
	unlink("/tmp/unistd_test_truncate.txt");
}

TEST(ftruncate_file) {
	int fd = open("/tmp/unistd_test_ftruncate.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	write(fd, "0123456789", 10);
	
	int result = ftruncate(fd, 3);
	ASSERT_EQ(0, result);
	
	off_t size = lseek(fd, 0, SEEK_END);
	ASSERT_EQ(3, size);
	
	close(fd);
	unlink("/tmp/unistd_test_ftruncate.txt");
}
#endif

/* ============================================================================
 * DIRECTORY OPERATIONS
 * ============================================================================ */
#if JACL_HAS_POSIX
TEST_SUITE(directory_operations);

TEST(getcwd_basic) {
	char buf[1024];
	char *result = getcwd(buf, sizeof(buf));
	
	ASSERT_NOT_NULL(result);
	ASSERT_TRUE(strlen(buf) > 0);
}

TEST(chdir_getcwd) {
	char original[1024];
	getcwd(original, sizeof(original));
	
	int result = chdir("/tmp");
	ASSERT_EQ(0, result);
	
	char buf[1024];
	getcwd(buf, sizeof(buf));
	ASSERT_STR_EQ("/tmp", buf);
	
	// Restore
	chdir(original);
}
#endif

/* ============================================================================
 * ISATTY
 * ============================================================================ */
TEST_SUITE(isatty_test);

TEST(isatty_stdin) {
	int result = isatty(STDIN_FILENO);
	// May or may not be a TTY
	ASSERT_TRUE(result == 0 || result == 1);
}

TEST(isatty_invalid_fd) {
	int result = isatty(9999);
	ASSERT_EQ(0, result);
}

/* ============================================================================
 * SLEEP
 * ============================================================================ */
TEST_SUITE(sleep_test);

TEST(sleep_zero) {
	unsigned int result = sleep(0);
	ASSERT_EQ(0, result);
}

TEST(sleep_short) {
	unsigned int result = sleep(1);
	ASSERT_EQ(0, result);
}

/* ============================================================================
 * FORK (if supported)
 * ============================================================================ */
#if JACL_HAS_POSIX && !JACL_ARCH_WASM
TEST_SUITE(fork_test);

TEST(fork_basic) {
	pid_t pid = fork();
	
	if (pid == 0) {
		// Child process
		_exit(0);
	} else if (pid > 0) {
		// Parent process
		ASSERT_TRUE(pid > 0);
		
		// Wait for child (simple wait)
		sleep(1);
	} else {
		// Fork failed
		ASSERT_TRUE(0);
	}
}
#endif

/* ============================================================================
 * NON-POSIX STUBS
 * ============================================================================ */
#if !JACL_HAS_POSIX
TEST_SUITE(non_posix_stubs);

TEST(read_returns_enosys) {
	char buf[10];
	ssize_t result = read(0, buf, 10);
	
	ASSERT_EQ(-1, result);
	ASSERT_EQ(ENOSYS, errno);
}

TEST(write_returns_enosys) {
	ssize_t result = write(1, "test", 4);
	
	ASSERT_EQ(-1, result);
	ASSERT_EQ(ENOSYS, errno);
}

TEST(pipe_returns_enosys) {
	int pipefd[2];
	int result = pipe(pipefd);
	
	ASSERT_EQ(-1, result);
	ASSERT_EQ(ENOSYS, errno);
}

TEST(fork_returns_enosys) {
	pid_t result = fork();
	
	ASSERT_EQ(-1, result);
	ASSERT_EQ(ENOSYS, errno);
}

TEST(getpid_returns_default) {
	pid_t pid = getpid();
	ASSERT_EQ(1, pid);
}

TEST(getuid_returns_zero) {
	uid_t uid = getuid();
	ASSERT_EQ(0, uid);
}
#endif

TEST_MAIN()
