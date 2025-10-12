/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

TEST_TYPE(unit);
TEST_UNIT(poll.h);

/* ============================================================================
 * CONSTANTS
 * ============================================================================ */
TEST_SUITE(constants);

TEST(poll_event_constants) {
	ASSERT_EQ(0x0001, POLLIN);
	ASSERT_EQ(0x0010, POLLOUT);
	ASSERT_EQ(0x0008, POLLPRI);
	ASSERT_EQ(0x0100, POLLERR);
	ASSERT_EQ(0x0200, POLLHUP);
	ASSERT_EQ(0x0400, POLLNVAL);
}

TEST(poll_timeout_constants) {
	ASSERT_EQ(-1, INFTIM);
}

/* ============================================================================
 * POLLFD STRUCTURE
 * ============================================================================ */
TEST_SUITE(pollfd_structure);

TEST(pollfd_members) {
	struct pollfd pfd;
	
	pfd.fd = 5;
	pfd.events = POLLIN;
	pfd.revents = 0;
	
	ASSERT_EQ(5, pfd.fd);
	ASSERT_EQ(POLLIN, pfd.events);
	ASSERT_EQ(0, pfd.revents);
}

TEST(pollfd_init) {
	struct pollfd pfd;
	
	pollfd_init(&pfd, 10, POLLIN | POLLOUT);
	
	ASSERT_EQ(10, pfd.fd);
	ASSERT_EQ(POLLIN | POLLOUT, pfd.events);
	ASSERT_EQ(0, pfd.revents);
}

/* ============================================================================
 * UTILITY FUNCTIONS
 * ============================================================================ */
TEST_SUITE(utility_functions);

TEST(pollfd_has_input) {
	struct pollfd pfd = {0, 0, POLLIN};
	
	ASSERT_TRUE(pollfd_has_input(&pfd));
}

TEST(pollfd_has_input_false) {
	struct pollfd pfd = {0, 0, POLLOUT};
	
	ASSERT_FALSE(pollfd_has_input(&pfd));
}

TEST(pollfd_has_output) {
	struct pollfd pfd = {0, 0, POLLOUT};
	
	ASSERT_TRUE(pollfd_has_output(&pfd));
}

TEST(pollfd_has_error) {
	struct pollfd pfd = {0, 0, POLLERR};
	
	ASSERT_TRUE(pollfd_has_error(&pfd));
}

TEST(pollfd_has_priority) {
	struct pollfd pfd = {0, 0, POLLPRI};
	
	ASSERT_TRUE(pollfd_has_priority(&pfd));
}

TEST(pollfd_has_hangup) {
	struct pollfd pfd = {0, 0, POLLHUP};
	
	ASSERT_TRUE(pollfd_has_hangup(&pfd));
}

/* ============================================================================
 * VALIDATION
 * ============================================================================ */
TEST_SUITE(validation);

TEST(poll_validate_null_fds) {
	int result = poll_validate(NULL, 1);
	ASSERT_EQ(-1, result);
}

TEST(poll_validate_zero_nfds) {
	struct pollfd pfd;
	int result = poll_validate(&pfd, 0);
	ASSERT_EQ(0, result);
}

TEST(poll_validate_invalid_fd) {
	struct pollfd pfd = {-100, POLLIN, 0};
	int result = poll_validate(&pfd, 1);
	ASSERT_EQ(-1, result);
}

TEST(poll_validate_valid) {
	struct pollfd pfd = {0, POLLIN, 0};
	int result = poll_validate(&pfd, 1);
	ASSERT_EQ(0, result);
}

/* ============================================================================
 * EVENT STRING CONVERSION
 * ============================================================================ */
TEST_SUITE(event_strings);

TEST(poll_events_to_string_r_pollin) {
	char buf[64];
	poll_events_to_string_r(POLLIN, buf, sizeof(buf));
	
	ASSERT_STR_EQ("POLLIN", buf);
}

TEST(poll_events_to_string_r_pollout) {
	char buf[64];
	poll_events_to_string_r(POLLOUT, buf, sizeof(buf));
	
	ASSERT_STR_EQ("POLLOUT", buf);
}

TEST(poll_events_to_string_r_multiple) {
	char buf[128];
	poll_events_to_string_r(POLLIN | POLLOUT, buf, sizeof(buf));
	
	ASSERT_TRUE(strstr(buf, "POLLIN") != NULL);
	ASSERT_TRUE(strstr(buf, "POLLOUT") != NULL);
}

TEST(poll_events_to_string_r_error) {
	char buf[64];
	poll_events_to_string_r(POLLERR, buf, sizeof(buf));
	
	ASSERT_STR_EQ("POLLERR", buf);
}

TEST(poll_events_to_string_non_threadsafe) {
	const char *str = poll_events_to_string(POLLIN);
	
	ASSERT_NOT_NULL(str);
	ASSERT_TRUE(strstr(str, "POLLIN") != NULL);
}

/* ============================================================================
 * POLL - BASIC TESTS
 * ============================================================================ */
#if !POLL_DUMMY
TEST_SUITE(poll_basic);

TEST(poll_zero_nfds) {
	int result = poll(NULL, 0, 0);
	ASSERT_EQ(0, result);
}

TEST(poll_timeout_zero) {
	struct pollfd pfd = {-1, POLLIN, 0};
	int result = poll(&pfd, 1, 0);
	
	// Should return immediately
	ASSERT_TRUE(result >= 0);
}

TEST(poll_invalid_fd) {
	struct pollfd pfd = {-100, POLLIN, 0};
	int result = poll(&pfd, 1, 0);
	
	if (result == 1) {
		ASSERT_TRUE(pfd.revents & POLLNVAL);
	}
}

TEST(poll_stdin_immediate) {
	struct pollfd pfd;
	pollfd_init(&pfd, STDIN_FILENO, POLLIN);
	
	int result = poll(&pfd, 1, 0);
	ASSERT_TRUE(result >= 0);
}
#endif

/* ============================================================================
 * POLL - PIPE TESTS
 * ============================================================================ */
#if !POLL_DUMMY && !JACL_OS_WINDOWS
TEST_SUITE(poll_pipes);

TEST(poll_pipe_readable) {
	int pipefd[2];
	ASSERT_EQ(0, pipe(pipefd));
	
	// Write to pipe
	write(pipefd[1], "test", 4);
	
	struct pollfd pfd;
	pollfd_init(&pfd, pipefd[0], POLLIN);
	
	int result = poll(&pfd, 1, 0);
	
	ASSERT_EQ(1, result);
	ASSERT_TRUE(pfd.revents & POLLIN);
	
	close(pipefd[0]);
	close(pipefd[1]);
}

TEST(poll_pipe_writable) {
	int pipefd[2];
	ASSERT_EQ(0, pipe(pipefd));
	
	struct pollfd pfd;
	pollfd_init(&pfd, pipefd[1], POLLOUT);
	
	int result = poll(&pfd, 1, 0);
	
	ASSERT_EQ(1, result);
	ASSERT_TRUE(pfd.revents & POLLOUT);
	
	close(pipefd[0]);
	close(pipefd[1]);
}

TEST(poll_pipe_closed) {
	int pipefd[2];
	ASSERT_EQ(0, pipe(pipefd));
	
	// Close write end
	close(pipefd[1]);
	
	struct pollfd pfd;
	pollfd_init(&pfd, pipefd[0], POLLIN);
	
	int result = poll(&pfd, 1, 0);
	
	ASSERT_EQ(1, result);
	ASSERT_TRUE(pfd.revents & (POLLIN | POLLHUP));
	
	close(pipefd[0]);
}
#endif

/* ============================================================================
 * POLL - FILE TESTS
 * ============================================================================ */
#if !POLL_DUMMY
TEST_SUITE(poll_files);

TEST(poll_file_readable) {
	// Create temp file
	int fd = open("/tmp/poll_test.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_TRUE(fd >= 0);
	
	write(fd, "test", 4);
	lseek(fd, 0, SEEK_SET);
	
	struct pollfd pfd;
	pollfd_init(&pfd, fd, POLLIN);
	
	int result = poll(&pfd, 1, 0);
	
	ASSERT_EQ(1, result);
	ASSERT_TRUE(pfd.revents & POLLIN);
	
	close(fd);
	unlink("/tmp/poll_test.txt");
}

TEST(poll_file_writable) {
	int fd = open("/tmp/poll_test2.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_TRUE(fd >= 0);
	
	struct pollfd pfd;
	pollfd_init(&pfd, fd, POLLOUT);
	
	int result = poll(&pfd, 1, 0);
	
	ASSERT_EQ(1, result);
	ASSERT_TRUE(pfd.revents & POLLOUT);
	
	close(fd);
	unlink("/tmp/poll_test2.txt");
}
#endif

/* ============================================================================
 * POLL - MULTIPLE FDS
 * ============================================================================ */
#if !POLL_DUMMY && !JACL_OS_WINDOWS
TEST_SUITE(poll_multiple);

TEST(poll_multiple_pipes) {
	int pipe1[2], pipe2[2];
	pipe(pipe1);
	pipe(pipe2);
	
	// Write to first pipe only
	write(pipe1[1], "data", 4);
	
	struct pollfd pfds[2];
	pollfd_init(&pfds[0], pipe1[0], POLLIN);
	pollfd_init(&pfds[1], pipe2[0], POLLIN);
	
	int result = poll(pfds, 2, 0);
	
	ASSERT_EQ(1, result);
	ASSERT_TRUE(pfds[0].revents & POLLIN);
	ASSERT_FALSE(pfds[1].revents & POLLIN);
	
	close(pipe1[0]); close(pipe1[1]);
	close(pipe2[0]); close(pipe2[1]);
}

TEST(poll_mixed_events) {
	int pipefd[2];
	pipe(pipefd);
	
	struct pollfd pfds[2];
	pollfd_init(&pfds[0], pipefd[0], POLLIN);
	pollfd_init(&pfds[1], pipefd[1], POLLOUT);
	
	int result = poll(pfds, 2, 0);
	
	// Write end should be ready
	ASSERT_TRUE(result >= 1);
	ASSERT_TRUE(pfds[1].revents & POLLOUT);
	
	close(pipefd[0]);
	close(pipefd[1]);
}
#endif

/* ============================================================================
 * PPOLL - BASIC TESTS
 * ============================================================================ */
#if !POLL_DUMMY && !JACL_OS_WINDOWS
TEST_SUITE(ppoll);

TEST(ppoll_basic) {
	struct pollfd pfd = {-1, POLLIN, 0};
	struct timespec timeout = {0, 0};
	
	int result = ppoll(&pfd, 1, &timeout, NULL);
	ASSERT_TRUE(result >= 0);
}

TEST(ppoll_with_timeout) {
	int pipefd[2];
	pipe(pipefd);
	
	struct pollfd pfd;
	pollfd_init(&pfd, pipefd[0], POLLIN);
	
	struct timespec timeout = {0, 10000000};  // 10ms
	
	int result = ppoll(&pfd, 1, &timeout, NULL);
	ASSERT_EQ(0, result);  // Timeout
	
	close(pipefd[0]);
	close(pipefd[1]);
}
#endif

/* ============================================================================
 * ERROR CASES
 * ============================================================================ */
TEST_SUITE(error_cases);

TEST(poll_null_fds_nonzero_nfds) {
	int result = poll(NULL, 1, 0);
	ASSERT_EQ(-1, result);
}

TEST(pollfd_init_null) {
	pollfd_init(NULL, 0, 0);
	// Should not crash
	ASSERT_TRUE(1);
}

TEST(pollfd_helpers_null) {
	ASSERT_FALSE(pollfd_has_input(NULL));
	ASSERT_FALSE(pollfd_has_output(NULL));
	ASSERT_FALSE(pollfd_has_error(NULL));
	ASSERT_FALSE(pollfd_has_priority(NULL));
	ASSERT_FALSE(pollfd_has_hangup(NULL));
}

/* ============================================================================
 * WASM DUMMY TESTS
 * ============================================================================ */
#if POLL_DUMMY
TEST_SUITE(wasm_dummy);

TEST(poll_returns_enosys) {
	struct pollfd pfd;
	int result = poll(&pfd, 1, 0);
	
	ASSERT_EQ(-1, result);
	ASSERT_EQ(ENOSYS, errno);
}

TEST(ppoll_returns_enosys) {
	struct pollfd pfd;
	int result = ppoll(&pfd, 1, NULL, NULL);
	
	ASSERT_EQ(-1, result);
	ASSERT_EQ(ENOSYS, errno);
}
#endif

TEST_MAIN()
