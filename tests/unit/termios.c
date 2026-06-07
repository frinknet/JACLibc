/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#include <testing.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

static int open_pty_pair(int *master, int *slave) {
	int m = posix_openpt(O_RDWR | O_NOCTTY);
	if (m < 0) return -1;
	if (grantpt(m) < 0 || unlockpt(m) < 0) { close(m); return -1; }
	char *name = ptsname(m);
	if (!name) { close(m); return -1; }
	int s = open(name, O_RDWR | O_NOCTTY);
	if (s < 0) { close(m); return -1; }
	*master = m;
	*slave = s;
	return 0;
}

TEST_TYPE(unit);
TEST_UNIT(termios.h);

/* ============================================================================
 * CONSTANTS
 * ============================================================================ */
TEST_SUITE(constants);

TEST(constants_v_index) {
	int vals[] = {VINTR, VQUIT, VERASE, VKILL, VEOF, VEOL, VSTART, VSTOP, VSUSP};
	int n = sizeof(vals)/sizeof(vals[0]);
	for (int i = 0; i < n; i++) {
		ASSERT_TRUE(vals[i] >= 0);
		ASSERT_TRUE(vals[i] < NCCS);
		for (int j = i+1; j < n; j++) ASSERT_NE(vals[i], vals[j]);
	}
	ASSERT_TRUE(VMIN >= 0 && VMIN < NCCS);
	ASSERT_TRUE(VTIME >= 0 && VTIME < NCCS);
}

TEST(constants_iflag) {
	ASSERT_TRUE(IGNBRK > 0);
	ASSERT_TRUE(BRKINT > 0);
	ASSERT_TRUE(IGNPAR > 0);
	ASSERT_TRUE(INPCK > 0);
	ASSERT_TRUE(ISTRIP > 0);
	ASSERT_TRUE(INLCR > 0);
	ASSERT_TRUE(IGNCR > 0);
	ASSERT_TRUE(ICRNL > 0);
	ASSERT_TRUE(IXON > 0);
	ASSERT_TRUE(IXANY > 0);
	ASSERT_TRUE(IXOFF > 0);
	ASSERT_TRUE(PARMRK > 0);
}

TEST(constants_oflag) {
	ASSERT_TRUE(OPOST > 0);
	ASSERT_TRUE(ONLCR > 0);
	ASSERT_TRUE(OCRNL > 0);
	ASSERT_TRUE(ONOCR > 0);
	ASSERT_TRUE(ONLRET > 0);
	ASSERT_TRUE(OFILL > 0);
	ASSERT_TRUE(OFDEL > 0);
	ASSERT_EQ(0, NL0);
	ASSERT_TRUE(NL1 > 0);
	ASSERT_EQ(0, CR0);
	ASSERT_TRUE(CR3 > 0);
	ASSERT_TRUE(TAB3 > 0);
	ASSERT_TRUE(BS1 > 0);
	ASSERT_TRUE(VT1 > 0);
	ASSERT_TRUE(FF1 > 0);
}

TEST(constants_cflag) {
	ASSERT_EQ(0, CS5);
	ASSERT_TRUE(CS6 > CS5);
	ASSERT_TRUE(CS7 > CS6);
	ASSERT_TRUE(CS8 > CS7);
	ASSERT_TRUE(CSTOPB > 0);
	ASSERT_TRUE(CREAD > 0);
	ASSERT_TRUE(PARENB > 0);
	ASSERT_TRUE(PARODD > 0);
	ASSERT_TRUE(HUPCL > 0);
	ASSERT_TRUE(CLOCAL > 0);
	ASSERT_EQ(0, CS5 & ~CSIZE);
	ASSERT_EQ(0, CS6 & ~CSIZE);
	ASSERT_EQ(0, CS7 & ~CSIZE);
	ASSERT_EQ(0, CS8 & ~CSIZE);
	ASSERT_NE(CS5, CS6);
	ASSERT_NE(CS6, CS7);
	ASSERT_NE(CS7, CS8);
	ASSERT_EQ(CS5, CS5 & CSIZE);
	ASSERT_EQ(CS6, CS6 & CSIZE);
	ASSERT_EQ(CS7, CS7 & CSIZE);
	ASSERT_EQ(CS8, CS8 & CSIZE);
}

TEST(constants_baud) {
	speed_t rates[] = {
		B0, B50, B75, B110, B134, B150, B200, B300,
		B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400
	};
	int n = sizeof(rates)/sizeof(rates[0]);
	for (int i = 0; i < n; i++) {
		for (int j = i+1; j < n; j++) ASSERT_NE(rates[i], rates[j]);
	}
	ASSERT_EQ(0, B0);
}

TEST(constants_lflag) {
	ASSERT_TRUE(ISIG > 0);
	ASSERT_TRUE(ICANON > 0);
	ASSERT_TRUE(ECHO > 0);
	ASSERT_TRUE(ECHOE > 0);
	ASSERT_TRUE(ECHOK > 0);
	ASSERT_TRUE(ECHONL > 0);
	ASSERT_TRUE(NOFLSH > 0);
	ASSERT_TRUE(TOSTOP > 0);
	ASSERT_TRUE(IEXTEN > 0);
}

TEST(constants_action) {
	ASSERT_EQ(0, TCSANOW);
	ASSERT_EQ(1, TCSADRAIN);
	ASSERT_EQ(2, TCSAFLUSH);
	ASSERT_EQ(0, TCIFLUSH);
	ASSERT_EQ(1, TCOFLUSH);
	ASSERT_EQ(2, TCIOFLUSH);
	ASSERT_EQ(0, TCOOFF);
	ASSERT_EQ(1, TCOON);
	ASSERT_EQ(2, TCIOFF);
	ASSERT_EQ(3, TCION);
	ASSERT_NE(TCSANOW, TCSADRAIN);
	ASSERT_NE(TCSADRAIN, TCSAFLUSH);
	ASSERT_NE(TCSANOW, TCSAFLUSH);
	ASSERT_NE(TCIFLUSH, TCOFLUSH);
	ASSERT_NE(TCOFLUSH, TCIOFLUSH);
	ASSERT_NE(TCOOFF, TCOON);
	ASSERT_NE(TCIOFF, TCION);
}

/* ============================================================================
 * STRUCT LAYOUTS
 * ============================================================================ */
TEST_SUITE(termios);

TEST(termios_has_fields) {
	struct termios t;
	memset(&t, 0, sizeof(t));
	t.c_iflag = ICRNL;
	t.c_oflag = OPOST | ONLCR;
	t.c_cflag = CS8 | CREAD | CLOCAL;
	t.c_lflag = ECHO | ICANON;
	t.c_line = 0;
	t.c_cc[VMIN] = 1;
	t.c_cc[VTIME] = 0;
	ASSERT_EQ(ICRNL, t.c_iflag);
	ASSERT_EQ(OPOST | ONLCR, t.c_oflag);
	ASSERT_EQ(CS8 | CREAD | CLOCAL, t.c_cflag);
	ASSERT_EQ(ECHO | ICANON, t.c_lflag);
	ASSERT_EQ(1, t.c_cc[VMIN]);
}

TEST(termios_cc_size) {
	ASSERT_EQ(NCCS, 32);
	struct termios t;
	memset(&t, 0, sizeof(t));
	for (int i = 0; i < NCCS; i++) t.c_cc[i] = (cc_t)i;
	for (int i = 0; i < NCCS; i++) ASSERT_EQ((cc_t)i, t.c_cc[i]);
}

TEST(termios_types_unsigned) {
	cc_t c = (cc_t)-1;
	speed_t s = (speed_t)-1;
	tcflag_t f = (tcflag_t)-1;
	ASSERT_TRUE(c > 0);
	ASSERT_TRUE(s > 0);
	ASSERT_TRUE(f > 0);
}

TEST(termios_types_width) {
	ASSERT_TRUE(sizeof(cc_t) <= sizeof(long));
	ASSERT_TRUE(sizeof(speed_t) <= sizeof(long));
	ASSERT_TRUE(sizeof(tcflag_t) <= sizeof(long));
}

TEST(termios_zero_init) {
	struct termios t;
	memset(&t, 0, sizeof(t));
	ASSERT_EQ(0, t.c_iflag);
	ASSERT_EQ(0, t.c_oflag);
	ASSERT_EQ(0, t.c_cflag);
	ASSERT_EQ(0, t.c_lflag);
	for (int i = 0; i < NCCS; i++) ASSERT_EQ(0, t.c_cc[i]);
}

TEST_SUITE(winsize);

TEST(winsize_fields) {
	struct winsize w;
	memset(&w, 0, sizeof(w));
	w.ws_row = 25;
	w.ws_col = 80;
	w.ws_xpixel = 640;
	w.ws_ypixel = 400;
	ASSERT_EQ(25, w.ws_row);
	ASSERT_EQ(80, w.ws_col);
	ASSERT_EQ(640, w.ws_xpixel);
	ASSERT_EQ(400, w.ws_ypixel);
}

TEST(winsize_size) {
	ASSERT_TRUE(sizeof(struct winsize) >= 2 * sizeof(unsigned short));
	ASSERT_TRUE(sizeof(struct winsize) == 4 * sizeof(unsigned short));
}

/* ============================================================================
 * cfgetispeed
 * ============================================================================ */
TEST_SUITE(cfgetispeed);

TEST(cfgetispeed_basic) {
	struct termios t;
	memset(&t, 0, sizeof(t));
	ASSERT_EQ(B0, cfgetispeed(&t));
	cfsetispeed(&t, B9600);
	ASSERT_EQ(B9600, cfgetispeed(&t));
}

TEST(cfgetispeed_null) {
	ASSERT_EQ(0, cfgetispeed(NULL));
}

TEST(cfgetispeed_reflects_set) {
	struct termios t;
	memset(&t, 0, sizeof(t));
	cfsetispeed(&t, B19200);
	ASSERT_EQ(B19200, cfgetispeed(&t));
	cfsetispeed(&t, B115200);
	ASSERT_EQ(B115200, cfgetispeed(&t));
}

/* ============================================================================
 * cfgetospeed
 * ============================================================================ */
TEST_SUITE(cfgetospeed);

TEST(cfgetospeed_basic) {
	struct termios t;
	memset(&t, 0, sizeof(t));
	ASSERT_EQ(B0, cfgetospeed(&t));
	cfsetospeed(&t, B9600);
	ASSERT_EQ(B9600, cfgetospeed(&t));
}

TEST(cfgetospeed_null) {
	ASSERT_EQ(0, cfgetospeed(NULL));
}

TEST(cfgetospeed_reflects_set) {
	struct termios t;
	memset(&t, 0, sizeof(t));
	cfsetospeed(&t, B19200);
	ASSERT_EQ(B19200, cfgetospeed(&t));
	cfsetospeed(&t, B115200);
	ASSERT_EQ(B115200, cfgetospeed(&t));
}

/* ============================================================================
 * cfsetispeed
 * ============================================================================ */
TEST_SUITE(cfsetispeed);

TEST(cfsetispeed_basic) {
	struct termios t;
	memset(&t, 0, sizeof(t));
	ASSERT_EQ(0, cfsetispeed(&t, B9600));
	ASSERT_EQ(B9600, cfgetispeed(&t));
}

TEST(cfsetispeed_null) {
	errno = 0;
	ASSERT_EQ(-1, cfsetispeed(NULL, B9600));
	ASSERT_EQ(EINVAL, errno);
}

TEST(cfsetispeed_does_not_clobber_ospeed) {
	struct termios t;
	memset(&t, 0, sizeof(t));
	cfsetospeed(&t, B115200);
	cfsetispeed(&t, B9600);
	ASSERT_EQ(B9600, cfgetispeed(&t));
	ASSERT_EQ(B115200, cfgetospeed(&t));
}

TEST(cfsetispeed_multiple_rates) {
	struct termios t;
	speed_t rates[] = {B0, B300, B9600, B19200, B38400};
	int n = sizeof(rates)/sizeof(rates[0]);
	for (int i = 0; i < n; i++) {
		memset(&t, 0, sizeof(t));
		ASSERT_EQ(0, cfsetispeed(&t, rates[i]));
		ASSERT_EQ(rates[i], cfgetispeed(&t));
	}
}

/* ============================================================================
 * cfsetospeed
 * ============================================================================ */
TEST_SUITE(cfsetospeed);

TEST(cfsetospeed_basic) {
	struct termios t;
	memset(&t, 0, sizeof(t));
	ASSERT_EQ(0, cfsetospeed(&t, B115200));
	ASSERT_EQ(B115200, cfgetospeed(&t));
}

TEST(cfsetospeed_null) {
	errno = 0;
	ASSERT_EQ(-1, cfsetospeed(NULL, B9600));
	ASSERT_EQ(EINVAL, errno);
}

TEST(cfsetospeed_does_not_clobber_ispeed) {
	struct termios t;
	memset(&t, 0, sizeof(t));
	cfsetispeed(&t, B9600);
	cfsetospeed(&t, B115200);
	ASSERT_EQ(B9600, cfgetispeed(&t));
	ASSERT_EQ(B115200, cfgetospeed(&t));
}

TEST(cfsetospeed_multiple_rates) {
	struct termios t;
	speed_t rates[] = {B0, B300, B9600, B19200, B38400};
	int n = sizeof(rates)/sizeof(rates[0]);
	for (int i = 0; i < n; i++) {
		memset(&t, 0, sizeof(t));
		ASSERT_EQ(0, cfsetospeed(&t, rates[i]));
		ASSERT_EQ(rates[i], cfgetospeed(&t));
	}
}

/* ============================================================================
 * cfsetspeed
 * ============================================================================ */
TEST_SUITE(cfsetspeed);

TEST(cfsetspeed_basic) {
	struct termios t;
	memset(&t, 0, sizeof(t));
	ASSERT_EQ(0, cfsetspeed(&t, B38400));
	ASSERT_EQ(B38400, cfgetispeed(&t));
	ASSERT_EQ(B38400, cfgetospeed(&t));
}

TEST(cfsetspeed_null) {
	errno = 0;
	ASSERT_EQ(-1, cfsetspeed(NULL, B9600));
	ASSERT_EQ(EINVAL, errno);
}

TEST(cfsetspeed_preserves_cflag) {
	struct termios t;
	memset(&t, 0, sizeof(t));
	t.c_cflag = CLOCAL | CREAD | CS8 | PARENB;
	cfsetspeed(&t, B115200);
	ASSERT_TRUE(t.c_cflag & CLOCAL);
	ASSERT_TRUE(t.c_cflag & CREAD);
	ASSERT_TRUE(t.c_cflag & PARENB);
	ASSERT_EQ(CS8, t.c_cflag & CSIZE);
	ASSERT_EQ(B115200, cfgetispeed(&t));
	ASSERT_EQ(B115200, cfgetospeed(&t));
}

TEST(cfsetspeed_multiple_rates) {
	struct termios t;
	speed_t rates[] = {B0, B50, B75, B110, B134, B150, B200, B300,
	                   B600, B1200, B1800, B2400, B4800, B9600, B19200, B38400};
	int n = sizeof(rates)/sizeof(rates[0]);
	for (int i = 0; i < n; i++) {
		memset(&t, 0, sizeof(t));
		ASSERT_EQ(0, cfsetspeed(&t, rates[i]));
		ASSERT_EQ(rates[i], cfgetispeed(&t));
		ASSERT_EQ(rates[i], cfgetospeed(&t));
	}
}

TEST(cfsetspeed_b0_hangup) {
	struct termios t;
	memset(&t, 0, sizeof(t));
	cfsetspeed(&t, B9600);
	cfsetspeed(&t, B0);
	ASSERT_EQ(B0, cfgetispeed(&t));
	ASSERT_EQ(B0, cfgetospeed(&t));
}

/* ============================================================================
 * cfmakeraw
 * ============================================================================ */
TEST_SUITE(cfmakeraw);

TEST(cfmakeraw_clears_flags) {
	struct termios t;
	memset(&t, 0xFF, sizeof(t));
	cfmakeraw(&t);
	ASSERT_EQ(0, t.c_iflag & (IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON));
	ASSERT_EQ(0, t.c_oflag & OPOST);
	ASSERT_EQ(0, t.c_lflag & (ECHO | ECHONL | ICANON | ISIG | IEXTEN));
	ASSERT_EQ(CS8, t.c_cflag & CSIZE);
	ASSERT_EQ(0, t.c_cflag & PARENB);
	ASSERT_EQ(1, t.c_cc[VMIN]);
	ASSERT_EQ(0, t.c_cc[VTIME]);
}

TEST(cfmakeraw_null) {
	cfmakeraw(NULL);
}

TEST(cfmakeraw_idempotent) {
	struct termios t;
	memset(&t, 0xFF, sizeof(t));
	cfmakeraw(&t);
	tcflag_t iflag = t.c_iflag;
	tcflag_t oflag = t.c_oflag;
	tcflag_t lflag = t.c_lflag;
	cfmakeraw(&t);
	ASSERT_EQ(iflag, t.c_iflag);
	ASSERT_EQ(oflag, t.c_oflag);
	ASSERT_EQ(lflag, t.c_lflag);
}

/* ============================================================================
 * POSIX FUNCTIONS — ERROR PATHS (LINUX)
 * ============================================================================ */
#if JACL_OS_LINUX

TEST_SUITE(tcgetattr);

TEST(tcgetattr_non_tty) {
	int fd = open("/dev/null", O_RDWR);
	struct termios t;
	errno = 0;
	ASSERT_EQ(-1, tcgetattr(fd, &t));
	ASSERT_EQ(ENOTTY, errno);
	close(fd);
}

TEST(tcgetattr_null) {
	int fd = open("/dev/null", O_RDWR);
	errno = 0;
	ASSERT_EQ(-1, tcgetattr(fd, NULL));
	ASSERT_EQ(EINVAL, errno);
	close(fd);
}

TEST(tcgetattr_bad_fd) {
	struct termios t;
	errno = 0;
	ASSERT_EQ(-1, tcgetattr(99999, &t));
	ASSERT_TRUE(errno == EBADF || errno == ENOTTY);
}

TEST_SUITE(tcsetattr);

TEST(tcsetattr_non_tty) {
	int fd = open("/dev/null", O_RDWR);
	struct termios t;
	memset(&t, 0, sizeof(t));
	errno = 0;
	ASSERT_EQ(-1, tcsetattr(fd, TCSANOW, &t));
	ASSERT_EQ(ENOTTY, errno);
	close(fd);
}

TEST(tcsetattr_null) {
	int fd = open("/dev/null", O_RDWR);
	errno = 0;
	ASSERT_EQ(-1, tcsetattr(fd, TCSANOW, NULL));
	ASSERT_EQ(EINVAL, errno);
	close(fd);
}

TEST(tcsetattr_bad_action) {
	int fd = open("/dev/null", O_RDWR);
	struct termios t;
	memset(&t, 0, sizeof(t));
	errno = 0;
	ASSERT_EQ(-1, tcsetattr(fd, 99, &t));
	ASSERT_EQ(EINVAL, errno);
	close(fd);
}

TEST(tcsetattr_all_actions) {
	int fd = open("/dev/null", O_RDWR);
	struct termios t;
	memset(&t, 0, sizeof(t));
	errno = 0; ASSERT_EQ(-1, tcsetattr(fd, TCSANOW, &t)); ASSERT_EQ(ENOTTY, errno);
	errno = 0; ASSERT_EQ(-1, tcsetattr(fd, TCSADRAIN, &t)); ASSERT_EQ(ENOTTY, errno);
	errno = 0; ASSERT_EQ(-1, tcsetattr(fd, TCSAFLUSH, &t)); ASSERT_EQ(ENOTTY, errno);
	close(fd);
}

TEST_SUITE(tcflush);

TEST(tcflush_bad_queue) {
	errno = 0;
	ASSERT_EQ(-1, tcflush(0, 99));
	ASSERT_EQ(EINVAL, errno);
}

TEST(tcflush_all_queues) {
	int fd = open("/dev/null", O_RDWR);
	errno = 0; ASSERT_EQ(-1, tcflush(fd, TCIFLUSH)); ASSERT_EQ(ENOTTY, errno);
	errno = 0; ASSERT_EQ(-1, tcflush(fd, TCOFLUSH)); ASSERT_EQ(ENOTTY, errno);
	errno = 0; ASSERT_EQ(-1, tcflush(fd, TCIOFLUSH)); ASSERT_EQ(ENOTTY, errno);
	close(fd);
}

TEST_SUITE(tcflow);

TEST(tcflow_bad_action) {
	errno = 0;
	ASSERT_EQ(-1, tcflow(0, 99));
	ASSERT_EQ(EINVAL, errno);
}

TEST(tcflow_all_actions) {
	int fd = open("/dev/null", O_RDWR);
	errno = 0; ASSERT_EQ(-1, tcflow(fd, TCOOFF)); ASSERT_EQ(ENOTTY, errno);
	errno = 0; ASSERT_EQ(-1, tcflow(fd, TCOON));  ASSERT_EQ(ENOTTY, errno);
	errno = 0; ASSERT_EQ(-1, tcflow(fd, TCIOFF)); ASSERT_EQ(ENOTTY, errno);
	errno = 0; ASSERT_EQ(-1, tcflow(fd, TCION));  ASSERT_EQ(ENOTTY, errno);
	close(fd);
}

TEST_SUITE(tcsendbreak);

TEST(tcsendbreak_non_tty) {
	int fd = open("/dev/null", O_RDWR);
	errno = 0;
	ASSERT_EQ(-1, tcsendbreak(fd, 0));
	ASSERT_EQ(ENOTTY, errno);
	close(fd);
}

TEST(tcsendbreak_bad_fd) {
	errno = 0;
	ASSERT_EQ(-1, tcsendbreak(99999, 0));
	ASSERT_TRUE(errno == EBADF || errno == ENOTTY);
}

TEST_SUITE(tcdrain);

TEST(tcdrain_non_tty) {
	int fd = open("/dev/null", O_RDWR);
	errno = 0;
	ASSERT_EQ(-1, tcdrain(fd));
	ASSERT_EQ(ENOTTY, errno);
	close(fd);
}

TEST(tcdrain_bad_fd) {
	errno = 0;
	ASSERT_EQ(-1, tcdrain(99999));
	ASSERT_TRUE(errno == EBADF || errno == ENOTTY);
}

TEST_SUITE(tcgetsid);

TEST(tcgetsid_non_tty) {
	int fd = open("/dev/null", O_RDWR);
	errno = 0;
	pid_t sid = tcgetsid(fd);
	ASSERT_EQ((pid_t)-1, sid);
	ASSERT_EQ(ENOTTY, errno);
	close(fd);
}

TEST(tcgetsid_bad_fd) {
	errno = 0;
	pid_t sid = tcgetsid(99999);
	ASSERT_EQ((pid_t)-1, sid);
	ASSERT_TRUE(errno == EBADF || errno == ENOTTY);
}

TEST_SUITE(tcgetwinsize);

TEST(tcgetwinsize_null) {
	errno = 0;
	ASSERT_EQ(-1, tcgetwinsize(0, NULL));
	ASSERT_EQ(EINVAL, errno);
}

TEST(tcgetwinsize_non_tty) {
	int fd = open("/dev/null", O_RDWR);
	struct winsize w;
	errno = 0;
	ASSERT_EQ(-1, tcgetwinsize(fd, &w));
	ASSERT_EQ(ENOTTY, errno);
	close(fd);
}

TEST(tcgetwinsize_bad_fd) {
	struct winsize w;
	errno = 0;
	ASSERT_EQ(-1, tcgetwinsize(99999, &w));
	ASSERT_TRUE(errno == EBADF || errno == ENOTTY);
}

TEST_SUITE(tcsetwinsize);

TEST(tcsetwinsize_null) {
	errno = 0;
	ASSERT_EQ(-1, tcsetwinsize(0, NULL));
	ASSERT_EQ(EINVAL, errno);
}

TEST(tcsetwinsize_non_tty) {
	int fd = open("/dev/null", O_RDWR);
	struct winsize w = {25, 80, 0, 0};
	errno = 0;
	ASSERT_EQ(-1, tcsetwinsize(fd, &w));
	ASSERT_EQ(ENOTTY, errno);
	close(fd);
}

TEST(tcsetwinsize_bad_fd) {
	struct winsize w = {25, 80, 0, 0};
	errno = 0;
	ASSERT_EQ(-1, tcsetwinsize(99999, &w));
	ASSERT_TRUE(errno == EBADF || errno == ENOTTY);
}

#endif /* JACL_OS_LINUX */

/* ============================================================================
 * NON-POSIX FALLBACK
 * ============================================================================ */
#if !JACL_OS_LINUX

TEST_SUITE(enosys_fallback);

TEST(enosys_all_functions) {
	struct termios t;
	struct winsize w = {25, 80, 0, 0};
	errno = 0; ASSERT_EQ(-1, cfsetispeed(&t, B9600));
	errno = 0; ASSERT_EQ(-1, cfsetospeed(&t, B9600));
	errno = 0; ASSERT_EQ(-1, cfsetspeed(&t, B9600));
	errno = 0; ASSERT_EQ(-1, tcgetattr(0, &t));
	errno = 0; ASSERT_EQ(-1, tcsetattr(0, TCSANOW, &t));
	errno = 0; ASSERT_EQ(-1, tcsendbreak(0, 0));
	errno = 0; ASSERT_EQ(-1, tcdrain(0));
	errno = 0; ASSERT_EQ(-1, tcflush(0, TCIFLUSH));
	errno = 0; ASSERT_EQ(-1, tcflow(0, TCOOFF));
	errno = 0; ASSERT_EQ((pid_t)-1, tcgetsid(0));
	errno = 0; ASSERT_EQ(-1, tcgetwinsize(0, &w));
	errno = 0; ASSERT_EQ(-1, tcsetwinsize(0, &w));
}

#endif

TEST_MAIN()
