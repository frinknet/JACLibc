/* (c) 2025 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <sys/ioctl.h>

TEST_TYPE(unit);
TEST_UNIT(sys/ioctl.h);

/* ============================================================================ */

TEST_SUITE(constants);

#if JACL_OS_LINUX || JACL_ARCH_WASM
TEST(constants_tiocgwinsz) {
	ASSERT_EQ(0x5413, TIOCGWINSZ);
}
TEST(constants_tiocswinsz) {
	ASSERT_EQ(0x5414, TIOCSWINSZ);
}
TEST(constants_tiocgpgrp) {
	ASSERT_EQ(0x540F, TIOCGPGRP);
}
TEST(constants_tiocspgrp) {
	ASSERT_EQ(0x5410, TIOCSPGRP);
}
TEST(constants_fionread) {
	ASSERT_EQ(0x541B, FIONREAD);
}
TEST(constants_fionbio) {
	ASSERT_EQ(0x5421, FIONBIO);
}
TEST(constants_fionclex) {
	ASSERT_EQ(0x5450, FIONCLEX);
}
TEST(constants_fioclex) {
	ASSERT_EQ(0x5451, FIOCLEX);
}
TEST(constants_fioasync) {
	ASSERT_EQ(0x5452, FIOASYNC);
}
TEST(constants_tiocsptlck) {
	ASSERT_EQ(0x40045448, TIOCSPTLCK);
}
TEST(constants_tiocgptn) {
	ASSERT_EQ(0x80045447, TIOCGPTN);
}
#endif

#if JACL_OS_BSD || JACL_OS_DARWIN
TEST(constants_tiocgwinsz) {
	ASSERT_EQ(0x40087468, TIOCGWINSZ);
}
TEST(constants_tiocswinsz) {
	ASSERT_EQ(0x80087467, TIOCSWINSZ);
}
TEST(constants_tiocgpgrp) {
	ASSERT_EQ(0x40047477, TIOCGPGRP);
}
TEST(constants_tiocspgrp) {
	ASSERT_EQ(0x80047476, TIOCSPGRP);
}
TEST(constants_fionread) {
	ASSERT_EQ(0x4004667F, FIONREAD);
}
TEST(constants_fionbio) {
	ASSERT_EQ(0x8004667E, FIONBIO);
}
TEST(constants_fionclex) {
	ASSERT_EQ(0x20006602, FIONCLEX);
}
TEST(constants_fioclex) {
	ASSERT_EQ(0x20006601, FIOCLEX);
}
TEST(constants_fioasync) {
	ASSERT_EQ(0x8004667D, FIOASYNC);
}
#endif

/* ============================================================================ */

TEST_SUITE(winsize);

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

/* ============================================================================ */

TEST_SUITE(ioctl);

TEST(ioctl_tiocgwinsz_stdin) {
	struct winsize ws;
	int result = ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
	ASSERT_TRUE(result == 0 || result == -1);
	if (result == 0) {
		ASSERT_TRUE(ws.ws_row >= 0 && ws.ws_row <= 1000);
		ASSERT_TRUE(ws.ws_col >= 0 && ws.ws_col <= 1000);
	}
}

TEST(ioctl_invalid_fd) {
	struct winsize ws;
	errno = 0;
	ASSERT_EQ(ioctl(-1, TIOCGWINSZ, &ws), -1);
	ASSERT_ERRNO(EBADF);
}

TEST(ioctl_regular_file) {
	int fd = open("/tmp/ioctl_test.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	ASSERT_TRUE(fd >= 0);
	struct winsize ws;
	errno = 0;
	ASSERT_EQ(ioctl(fd, TIOCGWINSZ, &ws), -1);
	ASSERT_ERRNO(ENOTTY);
	close(fd);
	unlink("/tmp/ioctl_test.txt");
}

TEST(ioctl_winsize_round_trip) {
	struct winsize ws_orig;
	int result = ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws_orig);
	if (result == 0) {
		struct winsize ws_new = ws_orig;
		result = ioctl(STDOUT_FILENO, TIOCSWINSZ, &ws_new);
		ASSERT_TRUE(result == 0 || result == -1);
	}
}

TEST(ioctl_null_argument) {
	errno = 0;
	ASSERT_EQ(ioctl(STDIN_FILENO, TIOCGWINSZ, NULL), -1);
	ASSERT_ERRNO(EFAULT);
}

TEST(ioctl_closed_fd) {
	int fd = open("/tmp/ioctl_test2.txt", O_CREAT | O_RDWR | O_TRUNC, 0644);
	close(fd);
	struct winsize ws;
	errno = 0;
	ASSERT_EQ(ioctl(fd, TIOCGWINSZ, &ws), -1);
	ASSERT_ERRNO(EBADF);
	unlink("/tmp/ioctl_test2.txt");
}

TEST(ioctl_fionread_pipe) {
	int p[2];
	ASSERT_EQ(pipe(p), 0);
	ASSERT_EQ(write(p[1], "0123456789", 10), 10);
	int avail = 0;
	ASSERT_EQ(ioctl(p[0], FIONREAD, &avail), 0);
	ASSERT_EQ(avail, 10);
	char buf[4];
	ASSERT_EQ(read(p[0], buf, 3), 3);
	ASSERT_EQ(ioctl(p[0], FIONREAD, &avail), 0);
	ASSERT_EQ(avail, 7);
	close(p[0]);
	close(p[1]);
}

#if !JACL_HASSYS(ioctl)
TEST(ioctl_enosys_fallback) {
	struct winsize ws;
	errno = 0;
	ASSERT_EQ(ioctl(STDIN_FILENO, TIOCGWINSZ, &ws), -1);
	ASSERT_ERRNO(ENOSYS);
}
#endif

/* ============================================================================ */

TEST_MAIN_IF(JACL_HAS_POSIX, "sys/ioctl.h requires POSIX\n")
