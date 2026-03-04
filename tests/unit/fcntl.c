/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(fcntl.h);

/* ============================================================================
 * CONSTANTS
 * ============================================================================ */
TEST_SUITE(constants);

TEST(constants_open_flags) {
	#define X(name, value) ASSERT_TRUE(name >= 0 || name < 0);
	FCNTL_O_FLAGS(X)
	#undef X
}

TEST(constants_fcntl_commands) {
	#define X(name, value) ASSERT_TRUE(name >= 0 || name < 0);
	FCNTL_F_COMMANDS(X)
	#undef X
}

TEST(constants_fd_flags) {
	#define X(name, value) ASSERT_TRUE(name >= 0 || name < 0);
	FCNTL_FD_FLAGS(X)
	#undef X
}

TEST(constants_lock_types) {
	#define X(name, value) ASSERT_TRUE(name >= 0 || name < 0);
	FCNTL_LOCK_TYPES(X)
	#undef X
}

TEST(constants_owner_types) {
	#define X(name, value) ASSERT_TRUE(name >= 0 || name < 0);
	FCNTL_OWNER_TYPES(X)
	#undef X
}

TEST(constants_at_flags) {
	#define X(name, value) ASSERT_TRUE(name >= 0 || name < 0);
	FCNTL_AT_FLAGS(X)
	#undef X
}

TEST(constants_posix_fadv) {
	#define X(name, value) ASSERT_TRUE(name >= 0);
	FCNTL_POSIX_FADV(X)
	#undef X
}

TEST(constants_seek) {
	#define X(name, value) ASSERT_TRUE(name >= 0);
	FCNTL_SEEK(X)
	#undef X
}

/* ============================================================================
 * OPEN
 * ============================================================================ */
TEST_SUITE(open);

TEST(open_rdonly) {
	int fd = open("/tmp/test_open_rdonly.txt", O_CREAT | O_RDONLY, 0644);
	ASSERT_TRUE(fd >= 0);
	close(fd);
	unlink("/tmp/test_open_rdonly.txt");
}

TEST(open_wronly) {
	int fd = open("/tmp/test_open_wronly.txt", O_CREAT | O_WRONLY, 0644);
	ASSERT_TRUE(fd >= 0);
	close(fd);
	unlink("/tmp/test_open_wronly.txt");
}

TEST(open_rdwr) {
	int fd = open("/tmp/test_open_rdwr.txt", O_CREAT | O_RDWR, 0644);
	ASSERT_TRUE(fd >= 0);
	close(fd);
	unlink("/tmp/test_open_rdwr.txt");
}

TEST(open_creat) {
	unlink("/tmp/test_open_creat.txt");
	int fd = open("/tmp/test_open_creat.txt", O_CREAT | O_RDWR, 0644);
	ASSERT_TRUE(fd >= 0);
	close(fd);
	unlink("/tmp/test_open_creat.txt");
}

TEST(open_trunc) {
	int fd = open("/tmp/test_open_trunc.txt", O_CREAT | O_RDWR, 0644);
	write(fd, "data", 4);
	close(fd);
	
	fd = open("/tmp/test_open_trunc.txt", O_RDWR | O_TRUNC);
	ASSERT_TRUE(fd >= 0);
	
	off_t size = lseek(fd, 0, SEEK_END);
	ASSERT_EQ(0, size);
	
	close(fd);
	unlink("/tmp/test_open_trunc.txt");
}

TEST(open_append) {
	int fd = open("/tmp/test_open_append.txt", O_CREAT | O_RDWR | O_APPEND, 0644);
	write(fd, "first", 5);
	lseek(fd, 0, SEEK_SET);
	write(fd, "second", 6);
	
	off_t size = lseek(fd, 0, SEEK_END);
	ASSERT_EQ(11, size);
	
	close(fd);
	unlink("/tmp/test_open_append.txt");
}

TEST(open_excl_creates_new) {
	unlink("/tmp/test_open_excl.txt");
	int fd = open("/tmp/test_open_excl.txt", O_CREAT | O_EXCL | O_RDWR, 0644);
	ASSERT_TRUE(fd >= 0);
	close(fd);
	unlink("/tmp/test_open_excl.txt");
}

TEST(open_excl_fails_existing) {
	int fd = open("/tmp/test_open_excl2.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	
	fd = open("/tmp/test_open_excl2.txt", O_CREAT | O_EXCL | O_RDWR, 0644);
	ASSERT_EQ(-1, fd);
	
	unlink("/tmp/test_open_excl2.txt");
}

TEST(open_nonexistent_fails) {
	unlink("/tmp/test_open_nonexistent.txt");
	int fd = open("/tmp/test_open_nonexistent.txt", O_RDONLY);
	ASSERT_EQ(-1, fd);
}

TEST(open_null_pathname) {
	int fd = open(NULL, O_RDONLY);
	ASSERT_EQ(-1, fd);
}

/* ============================================================================
 * OPENAT
 * ============================================================================ */
TEST_SUITE(openat);

TEST(openat_basic) {
	int fd = openat(AT_FDCWD, "/tmp/test_openat_basic.txt", O_CREAT | O_RDWR, 0644);
	ASSERT_TRUE(fd >= 0);
	close(fd);
	unlink("/tmp/test_openat_basic.txt");
}

TEST(openat_null_pathname) {
	int fd = openat(AT_FDCWD, NULL, O_RDONLY);
	ASSERT_EQ(-1, fd);
}

/* ============================================================================
 * CREAT
 * ============================================================================ */
TEST_SUITE(creat);

TEST(creat_new_file) {
	unlink("/tmp/test_creat_new.txt");
	int fd = creat("/tmp/test_creat_new.txt", 0644);
	ASSERT_TRUE(fd >= 0);
	close(fd);
	unlink("/tmp/test_creat_new.txt");
}

TEST(creat_truncates_existing) {
	int fd = creat("/tmp/test_creat_trunc.txt", 0644);
	write(fd, "data", 4);
	close(fd);
	
	fd = creat("/tmp/test_creat_trunc.txt", 0644);
	off_t size = lseek(fd, 0, SEEK_END);
	ASSERT_EQ(0, size);
	
	close(fd);
	unlink("/tmp/test_creat_trunc.txt");
}

TEST(creat_null_pathname) {
	int fd = creat(NULL, 0644);
	ASSERT_EQ(-1, fd);
}

/* ============================================================================
 * FCNTL
 * ============================================================================ */
TEST_SUITE(fcntl);

TEST(fcntl_dupfd_basic) {
	int fd = open("/tmp/test_fcntl_dupfd.txt", O_CREAT | O_RDWR, 0644);
	ASSERT_TRUE(fd >= 0);
	
	int new_fd = fcntl(fd, F_DUPFD, 0);
	ASSERT_TRUE(new_fd >= 0);
	ASSERT_NE(fd, new_fd);
	
	close(fd);
	close(new_fd);
	unlink("/tmp/test_fcntl_dupfd.txt");
}

TEST(fcntl_dupfd_min_fd) {
	int fd = open("/tmp/test_fcntl_dupfd_min.txt", O_CREAT | O_RDWR, 0644);
	
	int new_fd = fcntl(fd, F_DUPFD, 100);
	ASSERT_TRUE(new_fd >= 100);
	
	close(fd);
	close(new_fd);
	unlink("/tmp/test_fcntl_dupfd_min.txt");
}

TEST(fcntl_getfd_returns_flags) {
	int fd = open("/tmp/test_fcntl_getfd.txt", O_CREAT | O_RDWR, 0644);
	
	int flags = fcntl(fd, F_GETFD);
	ASSERT_TRUE(flags >= 0);
	
	close(fd);
	unlink("/tmp/test_fcntl_getfd.txt");
}

TEST(fcntl_setfd_cloexec) {
	int fd = open("/tmp/test_fcntl_setfd.txt", O_CREAT | O_RDWR, 0644);
	
	int result = fcntl(fd, F_SETFD, FD_CLOEXEC);
	ASSERT_EQ(0, result);
	
	int flags = fcntl(fd, F_GETFD);
	ASSERT_TRUE(flags & FD_CLOEXEC);
	
	close(fd);
	unlink("/tmp/test_fcntl_setfd.txt");
}

TEST(fcntl_getfl_returns_flags) {
	int fd = open("/tmp/test_fcntl_getfl.txt", O_CREAT | O_RDWR, 0644);
	
	int flags = fcntl(fd, F_GETFL);
	ASSERT_TRUE(flags >= 0);
	
	close(fd);
	unlink("/tmp/test_fcntl_getfl.txt");
}

TEST(fcntl_setfl_nonblock) {
	int fd = open("/tmp/test_fcntl_setfl.txt", O_CREAT | O_RDWR, 0644);
	
	int flags = fcntl(fd, F_GETFL);
	int result = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	ASSERT_EQ(0, result);
	
	close(fd);
	unlink("/tmp/test_fcntl_setfl.txt");
}

TEST(fcntl_invalid_fd) {
	int result = fcntl(-1, F_GETFD);
	ASSERT_EQ(-1, result);
}

/* ============================================================================
 * FCNTL_VALID_FD
 * ============================================================================ */
TEST_SUITE(fcntl_valid_fd);

TEST(fcntl_valid_fd_valid) {
	int fd = open("/tmp/test_valid_fd.txt", O_CREAT | O_RDWR, 0644);
	ASSERT_TRUE(fcntl_valid_fd(fd));
	close(fd);
	unlink("/tmp/test_valid_fd.txt");
}

TEST(fcntl_valid_fd_negative) {
	ASSERT_FALSE(fcntl_valid_fd(-1));
}

TEST(fcntl_valid_fd_closed) {
	int fd = open("/tmp/test_valid_fd_closed.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	ASSERT_FALSE(fcntl_valid_fd(fd));
	unlink("/tmp/test_valid_fd_closed.txt");
}

TEST(fcntl_valid_fd_large_invalid) {
	ASSERT_FALSE(fcntl_valid_fd(9999));
}

/* ============================================================================
 * FCNTL_SET_CLOEXEC
 * ============================================================================ */
TEST_SUITE(fcntl_set_cloexec);

TEST(fcntl_set_cloexec_sets_flag) {
	int fd = open("/tmp/test_set_cloexec.txt", O_CREAT | O_RDWR, 0644);
	
	int result = fcntl_set_cloexec(fd);
	ASSERT_EQ(0, result);
	
	int flags = fcntl(fd, F_GETFD);
	ASSERT_TRUE(flags & FD_CLOEXEC);
	
	close(fd);
	unlink("/tmp/test_set_cloexec.txt");
}

TEST(fcntl_set_cloexec_invalid_fd) {
	int result = fcntl_set_cloexec(-1);
	ASSERT_EQ(-1, result);
}

/* ============================================================================
 * FCNTL_CLEAR_CLOEXEC
 * ============================================================================ */
TEST_SUITE(fcntl_clear_cloexec);

TEST(fcntl_clear_cloexec_clears_flag) {
	int fd = open("/tmp/test_clear_cloexec.txt", O_CREAT | O_RDWR, 0644);
	fcntl_set_cloexec(fd);
	
	int result = fcntl_clear_cloexec(fd);
	ASSERT_EQ(0, result);
	
	int flags = fcntl(fd, F_GETFD);
	ASSERT_FALSE(flags & FD_CLOEXEC);
	
	close(fd);
	unlink("/tmp/test_clear_cloexec.txt");
}

TEST(fcntl_clear_cloexec_invalid_fd) {
	int result = fcntl_clear_cloexec(-1);
	ASSERT_EQ(-1, result);
}

/* ============================================================================
 * FCNTL_SET_NONBLOCK
 * ============================================================================ */
TEST_SUITE(fcntl_set_nonblock);

TEST(fcntl_set_nonblock_sets_flag) {
	int fd = open("/tmp/test_set_nonblock.txt", O_CREAT | O_RDWR, 0644);
	
	int result = fcntl_set_nonblock(fd);
	ASSERT_EQ(0, result);
	
	int flags = fcntl(fd, F_GETFL);
	ASSERT_TRUE(flags & O_NONBLOCK);
	
	close(fd);
	unlink("/tmp/test_set_nonblock.txt");
}

TEST(fcntl_set_nonblock_invalid_fd) {
	int result = fcntl_set_nonblock(-1);
	ASSERT_EQ(-1, result);
}

/* ============================================================================
 * FCNTL_CLEAR_NONBLOCK
 * ============================================================================ */
TEST_SUITE(fcntl_clear_nonblock);

TEST(fcntl_clear_nonblock_clears_flag) {
	int fd = open("/tmp/test_clear_nonblock.txt", O_CREAT | O_RDWR, 0644);
	fcntl_set_nonblock(fd);
	
	int result = fcntl_clear_nonblock(fd);
	ASSERT_EQ(0, result);
	
	int flags = fcntl(fd, F_GETFL);
	ASSERT_FALSE(flags & O_NONBLOCK);
	
	close(fd);
	unlink("/tmp/test_clear_nonblock.txt");
}

TEST(fcntl_clear_nonblock_invalid_fd) {
	int result = fcntl_clear_nonblock(-1);
	ASSERT_EQ(-1, result);
}

/* ============================================================================
 * FCNTL_DUPFD_CLOEXEC
 * ============================================================================ */
TEST_SUITE(fcntl_dupfd_cloexec);

TEST(fcntl_dupfd_cloexec_duplicates_with_cloexec) {
	int fd = open("/tmp/test_dupfd_cloexec.txt", O_CREAT | O_RDWR, 0644);
	
	int new_fd = fcntl_dupfd_cloexec(fd, 0);
	ASSERT_TRUE(new_fd >= 0);
	ASSERT_NE(fd, new_fd);
	
	int flags = fcntl(new_fd, F_GETFD);
	ASSERT_TRUE(flags & FD_CLOEXEC);
	
	close(fd);
	close(new_fd);
	unlink("/tmp/test_dupfd_cloexec.txt");
}

TEST(fcntl_dupfd_cloexec_min_fd) {
	int fd = open("/tmp/test_dupfd_cloexec_min.txt", O_CREAT | O_RDWR, 0644);
	
	int new_fd = fcntl_dupfd_cloexec(fd, 100);
	ASSERT_TRUE(new_fd >= 100);
	
	int flags = fcntl(new_fd, F_GETFD);
	ASSERT_TRUE(flags & FD_CLOEXEC);
	
	close(fd);
	close(new_fd);
	unlink("/tmp/test_dupfd_cloexec_min.txt");
}

TEST(fcntl_dupfd_cloexec_invalid_fd) {
	int new_fd = fcntl_dupfd_cloexec(-1, 0);
	ASSERT_EQ(-1, new_fd);
}

/* ============================================================================
 * STRUCT FLOCK
 * ============================================================================ */
TEST_SUITE(struct_flock);

TEST(struct_flock_members) {
	struct flock lock;
	
	lock.l_type = F_RDLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 100;
	lock.l_pid = 1234;
	
	ASSERT_EQ(F_RDLCK, lock.l_type);
	ASSERT_EQ(SEEK_SET, lock.l_whence);
	ASSERT_EQ(0, lock.l_start);
	ASSERT_EQ(100, lock.l_len);
	ASSERT_EQ(1234, lock.l_pid);
}

TEST_MAIN()

