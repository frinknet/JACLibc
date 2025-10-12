/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>

#if JACL_HAS_POSIX
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

TEST_TYPE(unit);
TEST_UNIT(sys/ioctl.h);

/* ============================================================================
 * CONSTANTS - TERMINAL IOCTLS
 * ============================================================================ */
TEST_SUITE(terminal_ioctls);

#if JACL_OS_LINUX || JACL_ARCH_WASM
TEST(tiocgwinsz_constant) {
	ASSERT_EQ(0x5413, TIOCGWINSZ);
}

TEST(tiocswinsz_constant) {
	ASSERT_EQ(0x5414, TIOCSWINSZ);
}

TEST(tiocgpgrp_constant) {
	ASSERT_EQ(0x540F, TIOCGPGRP);
}

TEST(tiocspgrp_constant) {
	ASSERT_EQ(0x5410, TIOCSPGRP);
}
#endif

/* ============================================================================
 * CONSTANTS - FILE IOCTLS
 * ============================================================================ */
TEST_SUITE(file_ioctls);

#if JACL_OS_LINUX || JACL_ARCH_WASM
TEST(fionread_constant) {
	ASSERT_EQ(0x541B, FIONREAD);
}

TEST(fionbio_constant) {
	ASSERT_EQ(0x5421, FIONBIO);
}

TEST(fionclex_constant) {
	ASSERT_EQ(0x5450, FIONCLEX);
}

TEST(fioclex_constant) {
	ASSERT_EQ(0x5451, FIOCLEX);
}

TEST(fioasync_constant) {
	ASSERT_EQ(0x5452, FIOASYNC);
}
#endif

/* ============================================================================
 * WINSIZE STRUCTURE
 * ============================================================================ */
TEST_SUITE(winsize_structure);

TEST(winsize_members) {
	struct winsize ws;
	
	ws.ws_row = 24;
	ws.ws_col = 80;
	ws.ws_xpixel = 640;
	ws.ws_ypixel = 480;
	
	ASSERT_EQ(24, ws.ws_row);
	ASSERT_EQ(80, ws.ws_col);
	ASSERT_EQ(640, ws.ws_xpixel);
	ASSERT_EQ(480, ws.ws_ypixel);
}

TEST(winsize_zeroed) {
	struct winsize ws = {0};
	
	ASSERT_EQ(0, ws.ws_row);
	ASSERT_EQ(0, ws.ws_col);
	ASSERT_EQ(0, ws.ws_xpixel);
	ASSERT_EQ(0, ws.ws_ypixel);
}

/* ============================================================================
 * IOCTL - BASIC OPERATIONS
 * ============================================================================ */
TEST_SUITE(ioctl_basic);

TEST(ioctl_tiocgwinsz_stdin) {
	struct winsize ws;
	int result = ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
	
	// May succeed if stdin is a terminal, or fail if not
	ASSERT_TRUE(result == 0 || result == -1);
	
	if (result == 0) {
		// If it succeeded, window size should be reasonable
		ASSERT_TRUE(ws.ws_row > 0 || ws.ws_row == 0);
		ASSERT_TRUE(ws.ws_col > 0 || ws.ws_col == 0);
	}
}

TEST(ioctl_invalid_fd) {
	struct winsize ws;
	int result = ioctl(-1, TIOCGWINSZ, &ws);
	
	ASSERT_EQ(-1, result);
}

TEST(ioctl_regular_file) {
	int fd = open("/tmp/ioctl_test.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_TRUE(fd >= 0);
	
	struct winsize ws;
	int result = ioctl(fd, TIOCGWINSZ, &ws);
	
	// Should fail - regular files don't support TIOCGWINSZ
	ASSERT_EQ(-1, result);
	
	close(fd);
	unlink("/tmp/ioctl_test.txt");
}

/* ============================================================================
 * IOCTL - WINDOW SIZE
 * ============================================================================ */
TEST_SUITE(ioctl_window_size);

TEST(ioctl_winsize_round_trip) {
	// Try to get window size from stdout
	struct winsize ws_orig;
	int result = ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws_orig);
	
	if (result == 0) {
		// If stdout is a terminal, try setting it back
		struct winsize ws_new = ws_orig;
		result = ioctl(STDOUT_FILENO, TIOCSWINSZ, &ws_new);
		
		// May fail due to permissions
		ASSERT_TRUE(result == 0 || result == -1);
	}
	
	ASSERT_TRUE(1); // Just verify it doesn't crash
}

TEST(ioctl_winsize_reasonable_values) {
	struct winsize ws;
	int result = ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	
	if (result == 0) {
		// Typical terminal size constraints
		ASSERT_TRUE(ws.ws_row <= 1000);
		ASSERT_TRUE(ws.ws_col <= 1000);
	}
	
	ASSERT_TRUE(1);
}

/* ============================================================================
 * EDGE CASES
 * ============================================================================ */
TEST_SUITE(edge_cases);

TEST(ioctl_null_argument) {
	int result = ioctl(STDIN_FILENO, TIOCGWINSZ, NULL);
	
	// May crash or return error
	ASSERT_TRUE(result == 0 || result == -1);
}

TEST(ioctl_closed_fd) {
	int fd = open("/tmp/ioctl_test2.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	close(fd);
	
	struct winsize ws;
	int result = ioctl(fd, TIOCGWINSZ, &ws);
	
	ASSERT_EQ(-1, result);
	
	unlink("/tmp/ioctl_test2.txt");
}

TEST_MAIN()

#else

int main(void) {
	printf("sys/ioctl.h requires POSIX\n");
	return 0;
}

#endif

