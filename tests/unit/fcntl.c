/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(fcntl.h);

/* ============================================================================
 * CONSTANTS - ACCESS MODES
 * ============================================================================ */
TEST_SUITE(access_modes);

TEST(access_mode_constants) {
	ASSERT_EQ(0x0000, O_RDONLY);
	ASSERT_EQ(0x0001, O_WRONLY);
	ASSERT_EQ(0x0002, O_RDWR);
	ASSERT_EQ(0x0003, O_ACCMODE);
}

TEST(access_modes_mutually_exclusive) {
	ASSERT_NE(O_RDONLY, O_WRONLY);
	ASSERT_NE(O_WRONLY, O_RDWR);
	ASSERT_NE(O_RDONLY, O_RDWR);
}

/* ============================================================================
 * CONSTANTS - CREATION FLAGS
 * ============================================================================ */
TEST_SUITE(creation_flags);

TEST(creation_flag_constants) {
	ASSERT_EQ(0x0040, O_CREAT);
	ASSERT_EQ(0x0080, O_EXCL);
	ASSERT_EQ(0x0100, O_NOCTTY);
	ASSERT_EQ(0x0200, O_TRUNC);
}

TEST(posix_2008_flags) {
	ASSERT_EQ(0x0400, O_CLOEXEC);
	ASSERT_EQ(0x0800, O_DIRECTORY);
	ASSERT_EQ(0x1000, O_NOFOLLOW);
}

/* ============================================================================
 * CONSTANTS - STATUS FLAGS
 * ============================================================================ */
TEST_SUITE(status_flags);

TEST(status_flag_constants) {
	ASSERT_EQ(0x4000, O_APPEND);
	ASSERT_EQ(0x8000, O_NONBLOCK);
	ASSERT_EQ(O_NONBLOCK, O_NDELAY);
}

TEST(sync_flags) {
	ASSERT_EQ(0x10000, O_SYNC);
	ASSERT_EQ(0x20000, O_DSYNC);
	ASSERT_EQ(0x40000, O_RSYNC);
}

/* ============================================================================
 * CONSTANTS - FCNTL COMMANDS
 * ============================================================================ */
TEST_SUITE(fcntl_commands);

TEST(basic_fcntl_commands) {
	ASSERT_EQ(0, F_DUPFD);
	ASSERT_EQ(1, F_GETFD);
	ASSERT_EQ(2, F_SETFD);
	ASSERT_EQ(3, F_GETFL);
	ASSERT_EQ(4, F_SETFL);
}

TEST(lock_commands) {
	ASSERT_EQ(5, F_GETLK);
	ASSERT_EQ(6, F_SETLK);
	ASSERT_EQ(7, F_SETLKW);
}

TEST(fcntl_commands_unique) {
	ASSERT_NE(F_DUPFD, F_GETFD);
	ASSERT_NE(F_GETFD, F_SETFD);
	ASSERT_NE(F_GETFL, F_SETFL);
}

/* ============================================================================
 * CONSTANTS - LOCK TYPES
 * ============================================================================ */
TEST_SUITE(lock_types);

TEST(lock_type_constants) {
	ASSERT_EQ(0, F_RDLCK);
	ASSERT_EQ(1, F_WRLCK);
	ASSERT_EQ(2, F_UNLCK);
}

TEST(lock_types_unique) {
	ASSERT_NE(F_RDLCK, F_WRLCK);
	ASSERT_NE(F_WRLCK, F_UNLCK);
	ASSERT_NE(F_RDLCK, F_UNLCK);
}

/* ============================================================================
 * CONSTANTS - FD FLAGS
 * ============================================================================ */
TEST_SUITE(fd_flags);

TEST(fd_flag_constants) {
	ASSERT_EQ(1, FD_CLOEXEC);
}

/* ============================================================================
 * CONSTANTS - SEEK
 * ============================================================================ */
TEST_SUITE(seek_constants);

TEST(seek_constants_defined) {
	ASSERT_EQ(0, SEEK_SET);
	ASSERT_EQ(1, SEEK_CUR);
	ASSERT_EQ(2, SEEK_END);
}

/* ============================================================================
 * CONSTANTS - POSIX ADVISORY
 * ============================================================================ */
TEST_SUITE(posix_advisory);

TEST(posix_fadvise_constants) {
	ASSERT_EQ(0, POSIX_FADV_NORMAL);
	ASSERT_EQ(1, POSIX_FADV_RANDOM);
	ASSERT_EQ(2, POSIX_FADV_SEQUENTIAL);
	ASSERT_EQ(3, POSIX_FADV_WILLNEED);
	ASSERT_EQ(4, POSIX_FADV_DONTNEED);
	ASSERT_EQ(5, POSIX_FADV_NOREUSE);
}

/* ============================================================================
 * FLOCK STRUCTURE
 * ============================================================================ */
TEST_SUITE(flock_structure);

TEST(flock_members) {
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

/* ============================================================================
 * OPEN - BASIC
 * ============================================================================ */
TEST_SUITE(open_basic);

TEST(open_rdonly) {
	int fd = open("/tmp/fcntl_test_rdonly.txt", O_CREAT | O_RDONLY, 0644);
	ASSERT_TRUE(fd >= 0);
	
	close(fd);
	unlink("/tmp/fcntl_test_rdonly.txt");
}

TEST(open_wronly) {
	int fd = open("/tmp/fcntl_test_wronly.txt", O_CREAT | O_WRONLY, 0644);
	ASSERT_TRUE(fd >= 0);
	
	close(fd);
	unlink("/tmp/fcntl_test_wronly.txt");
}

TEST(open_rdwr) {
	int fd = open("/tmp/fcntl_test_rdwr.txt", O_CREAT | O_RDWR, 0644);
	ASSERT_TRUE(fd >= 0);
	
	close(fd);
	unlink("/tmp/fcntl_test_rdwr.txt");
}

TEST(open_with_creat) {
	int fd = open("/tmp/fcntl_test_creat.txt", O_CREAT | O_RDWR, 0644);
	ASSERT_TRUE(fd >= 0);
	
	close(fd);
	unlink("/tmp/fcntl_test_creat.txt");
}

TEST(open_with_trunc) {
	// Create file with data
	int fd = open("/tmp/fcntl_test_trunc.txt", O_CREAT | O_RDWR, 0644);
	write(fd, "data", 4);
	close(fd);
	
	// Reopen with truncate
	fd = open("/tmp/fcntl_test_trunc.txt", O_RDWR | O_TRUNC);
	ASSERT_TRUE(fd >= 0);
	
	off_t size = lseek(fd, 0, SEEK_END);
	ASSERT_EQ(0, size);
	
	close(fd);
	unlink("/tmp/fcntl_test_trunc.txt");
}

/* ============================================================================
 * OPEN - FLAGS
 * ============================================================================ */
TEST_SUITE(open_flags);

TEST(open_with_append) {
	int fd = open("/tmp/fcntl_test_append.txt", O_CREAT | O_RDWR | O_APPEND, 0644);
	write(fd, "first", 5);
	
	// Seek to beginning
	lseek(fd, 0, SEEK_SET);
	
	// Write should still append
	write(fd, "second", 6);
	
	// Check size
	off_t size = lseek(fd, 0, SEEK_END);
	ASSERT_EQ(11, size);
	
	close(fd);
	unlink("/tmp/fcntl_test_append.txt");
}

TEST(open_excl_new_file) {
	unlink("/tmp/fcntl_test_excl.txt");
	
	int fd = open("/tmp/fcntl_test_excl.txt", O_CREAT | O_EXCL | O_RDWR, 0644);
	ASSERT_TRUE(fd >= 0);
	
	close(fd);
	unlink("/tmp/fcntl_test_excl.txt");
}

TEST(open_excl_existing_file) {
	int fd = open("/tmp/fcntl_test_excl2.txt", O_CREAT | O_RDWR, 0644);
	close(fd);
	
	// Try to open with O_EXCL
	fd = open("/tmp/fcntl_test_excl2.txt", O_CREAT | O_EXCL | O_RDWR, 0644);
	ASSERT_EQ(-1, fd);
	
	unlink("/tmp/fcntl_test_excl2.txt");
}

/* ============================================================================
 * CREAT
 * ============================================================================ */
TEST_SUITE(creat_test);

TEST(creat_basic) {
	int fd = creat("/tmp/fcntl_test_creat2.txt", 0644);
	ASSERT_TRUE(fd >= 0);
	
	close(fd);
	unlink("/tmp/fcntl_test_creat2.txt");
}

TEST(creat_overwrites) {
	// Create file with data
	int fd = creat("/tmp/fcntl_test_creat3.txt", 0644);
	write(fd, "data", 4);
	close(fd);
	
	// Creat again should truncate
	fd = creat("/tmp/fcntl_test_creat3.txt", 0644);
	off_t size = lseek(fd, 0, SEEK_END);
	
	ASSERT_EQ(0, size);
	
	close(fd);
	unlink("/tmp/fcntl_test_creat3.txt");
}

/* ============================================================================
 * FCNTL - F_DUPFD
 * ============================================================================ */
TEST_SUITE(fcntl_dupfd);

TEST(fcntl_dupfd_basic) {
	int fd = open("/tmp/fcntl_test_dup.txt", O_CREAT | O_RDWR, 0644);
	ASSERT_TRUE(fd >= 0);
	
	int new_fd = fcntl(fd, F_DUPFD, 0);
	ASSERT_TRUE(new_fd >= 0);
	ASSERT_NE(fd, new_fd);
	
	close(fd);
	close(new_fd);
	unlink("/tmp/fcntl_test_dup.txt");
}

TEST(fcntl_dupfd_min_fd) {
	int fd = open("/tmp/fcntl_test_dup2.txt", O_CREAT | O_RDWR, 0644);
	
	int new_fd = fcntl(fd, F_DUPFD, 100);
	ASSERT_TRUE(new_fd >= 100);
	
	close(fd);
	close(new_fd);
	unlink("/tmp/fcntl_test_dup2.txt");
}

/* ============================================================================
 * FCNTL - F_GETFD / F_SETFD
 * ============================================================================ */
TEST_SUITE(fcntl_fd_flags);

TEST(fcntl_getfd) {
	int fd = open("/tmp/fcntl_test_getfd.txt", O_CREAT | O_RDWR, 0644);
	
	int flags = fcntl(fd, F_GETFD);
	ASSERT_TRUE(flags >= 0);
	
	close(fd);
	unlink("/tmp/fcntl_test_getfd.txt");
}

TEST(fcntl_setfd_cloexec) {
	int fd = open("/tmp/fcntl_test_setfd.txt", O_CREAT | O_RDWR, 0644);
	
	int result = fcntl(fd, F_SETFD, FD_CLOEXEC);
	ASSERT_EQ(0, result);
	
	int flags = fcntl(fd, F_GETFD);
	ASSERT_TRUE(flags & FD_CLOEXEC);
	
	close(fd);
	unlink("/tmp/fcntl_test_setfd.txt");
}

/* ============================================================================
 * FCNTL - F_GETFL / F_SETFL
 * ============================================================================ */
TEST_SUITE(fcntl_status_flags);

TEST(fcntl_getfl) {
	int fd = open("/tmp/fcntl_test_getfl.txt", O_CREAT | O_RDWR, 0644);
	
	int flags = fcntl(fd, F_GETFL);
	ASSERT_TRUE(flags >= 0);
	
	close(fd);
	unlink("/tmp/fcntl_test_getfl.txt");
}

TEST(fcntl_setfl_nonblock) {
	int fd = open("/tmp/fcntl_test_setfl.txt", O_CREAT | O_RDWR, 0644);
	
	int flags = fcntl(fd, F_GETFL);
	int result = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	
	ASSERT_EQ(0, result);
	
	close(fd);
	unlink("/tmp/fcntl_test_setfl.txt");
}

/* ============================================================================
 * HELPER FUNCTIONS
 * ============================================================================ */
TEST_SUITE(helper_functions);

TEST(fcntl_valid_fd) {
	int fd = open("/tmp/fcntl_test_valid.txt", O_CREAT | O_RDWR, 0644);
	
	ASSERT_TRUE(fcntl_valid_fd(fd));
	ASSERT_FALSE(fcntl_valid_fd(-1));
	ASSERT_FALSE(fcntl_valid_fd(9999));
	
	close(fd);
	unlink("/tmp/fcntl_test_valid.txt");
}

TEST(fcntl_set_cloexec_helper) {
	int fd = open("/tmp/fcntl_test_helper1.txt", O_CREAT | O_RDWR, 0644);
	
	int result = fcntl_set_cloexec(fd);
	ASSERT_EQ(0, result);
	
	int flags = fcntl(fd, F_GETFD);
	ASSERT_TRUE(flags & FD_CLOEXEC);
	
	close(fd);
	unlink("/tmp/fcntl_test_helper1.txt");
}

TEST(fcntl_clear_cloexec_helper) {
	int fd = open("/tmp/fcntl_test_helper2.txt", O_CREAT | O_RDWR, 0644);
	fcntl_set_cloexec(fd);
	
	int result = fcntl_clear_cloexec(fd);
	ASSERT_EQ(0, result);
	
	int flags = fcntl(fd, F_GETFD);
	ASSERT_FALSE(flags & FD_CLOEXEC);
	
	close(fd);
	unlink("/tmp/fcntl_test_helper2.txt");
}

TEST(fcntl_set_nonblock_helper) {
	int fd = open("/tmp/fcntl_test_helper3.txt", O_CREAT | O_RDWR, 0644);
	
	int result = fcntl_set_nonblock(fd);
	ASSERT_EQ(0, result);
	
	close(fd);
	unlink("/tmp/fcntl_test_helper3.txt");
}

TEST(fcntl_clear_nonblock_helper) {
	int fd = open("/tmp/fcntl_test_helper4.txt", O_CREAT | O_RDWR, 0644);
	fcntl_set_nonblock(fd);
	
	int result = fcntl_clear_nonblock(fd);
	ASSERT_EQ(0, result);
	
	close(fd);
	unlink("/tmp/fcntl_test_helper4.txt");
}

TEST_MAIN()
