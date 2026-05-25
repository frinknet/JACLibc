/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#ifndef _TERMIOS_H
#define _TERMIOS_H
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <config.h>
#include <sys/types.h>
#include <errno.h>

#if JACL_OS_LINUX
	#include <sys/syscall.h>
	#include <unistd.h>
#endif

/* ============================================================================ */
/* Types                                                                        */
/* ============================================================================ */

typedef unsigned int  tcflag_t;
typedef unsigned char cc_t;
typedef unsigned int  speed_t;

/* glibc uses NCCS=32 for external ABI; kernel uses 19 internally */
#define NCCS 32

struct termios {
	tcflag_t c_iflag;
	tcflag_t c_oflag;
	tcflag_t c_cflag;
	tcflag_t c_lflag;
	cc_t     c_line;
	cc_t     c_cc[NCCS];
	speed_t  __ispeed;
	speed_t  __ospeed;
};

/* ============================================================================ */
/* Control character indices (c_cc)                                             */
/* ============================================================================ */

#define VINTR     0
#define VQUIT     1
#define VERASE    2
#define VKILL     3
#define VEOF      4
#define VTIME     5
#define VMIN      6
#define VSWTC     7
#define VSTART    8
#define VSTOP     9
#define VSUSP    10
#define VEOL     11
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE  14
#define VLNEXT   15
#define VEOL2    16

/* ============================================================================ */
/* Input flags (c_iflag)                                                        */
/* ============================================================================ */

#define IGNBRK  0000001
#define BRKINT  0000002
#define IGNPAR  0000004
#define PARMRK  0000010
#define INPCK   0000020
#define ISTRIP  0000040
#define INLCR   0000100
#define IGNCR   0000200
#define ICRNL   0000400
#define IUCLC   0001000
#define IXON    0002000
#define IXANY   0004000
#define IXOFF   0010000
#define IMAXBEL 0020000
#define IUTF8   0040000

/* ============================================================================ */
/* Output flags (c_oflag)                                                       */
/* ============================================================================ */

#define OPOST   0000001
#define OLCUC   0000002
#define ONLCR   0000004
#define OCRNL   0000010
#define ONOCR   0000020
#define ONLRET  0000040
#define OFILL   0000100
#define OFDEL   0000200

#define NLDLY   0000400
#define NL0     0000000
#define NL1     0000400
#define CRDLY   0003000
#define CR0     0000000
#define CR1     0001000
#define CR2     0002000
#define CR3     0003000
#define TABDLY  0014000
#define TAB0    0000000
#define TAB1    0004000
#define TAB2    0010000
#define TAB3    0014000
#define XTABS   0014000
#define BSDLY   0020000
#define BS0     0000000
#define BS1     0020000
#define VTDLY   0040000
#define VT0     0000000
#define VT1     0040000
#define FFDLY   0100000
#define FF0     0000000
#define FF1     0100000

/* ============================================================================ */
/* Control flags (c_cflag)                                                      */
/* ============================================================================ */

#define CBAUD   0010017
#define CSIZE   0000060
#define CS5     0000000
#define CS6     0000020
#define CS7     0000040
#define CS8     0000060
#define CSTOPB  0000100
#define CREAD   0000200
#define PARENB  0000400
#define PARODD  0001000
#define HUPCL   0002000
#define CLOCAL  0004000
#define CBAUDEX 0010000
#define CMSPAR  010000000000
#define CRTSCTS 020000000000

/* ============================================================================ */
/* Baud rates (encoded values for c_cflag, Linux ABI)                           */
/* ============================================================================ */

#define B0       0000000
#define B50      0000001
#define B75      0000002
#define B110     0000003
#define B134     0000004
#define B150     0000005
#define B200     0000006
#define B300     0000007
#define B600     0000010
#define B1200    0000011
#define B1800    0000012
#define B2400    0000013
#define B4800    0000014
#define B9600    0000015
#define B19200   0000016
#define B38400   0000017
#define B57600   0010001
#define B115200  0010002
#define B230400  0010003
#define B460800  0010004
#define B500000  0010005
#define B576000  0010006
#define B921600  0010007
#define B1000000 0010010
#define B1152000 0010011
#define B1500000 0010012
#define B2000000 0010013
#define B2500000 0010014
#define B3000000 0010015
#define B3500000 0010016
#define B4000000 0010017

/* ============================================================================ */
/* Local flags (c_lflag)                                                        */
/* ============================================================================ */

#define ISIG    0000001
#define ICANON  0000002
#define XCASE   0000004
#define ECHO    0000010
#define ECHOE   0000020
#define ECHOK   0000040
#define ECHONL  0000100
#define NOFLSH  0000200
#define TOSTOP  0000400
#define ECHOCTL 0001000
#define ECHOPRT 0002000
#define ECHOKE  0004000
#define FLUSHO  0010000
#define PENDIN  0040000
#define IEXTEN  0100000
#define EXTPROC 0200000

/* ============================================================================ */
/* Action constants                                                             */
/* ============================================================================ */

#define TCSANOW   0
#define TCSADRAIN 1
#define TCSAFLUSH 2

#define TCIFLUSH  0
#define TCOFLUSH  1
#define TCIOFLUSH 2

#define TCOOFF 0
#define TCOON  1
#define TCIOFF 2
#define TCION  3

/* ============================================================================ */
/* Linux ioctl numbers                                                          */
/* ============================================================================ */

#if JACL_OS_LINUX
	#define __JACL_TCGETS     0x5401
	#define __JACL_TCSETS     0x5402
	#define __JACL_TCSETSW    0x5403
	#define __JACL_TCSETSF    0x5404
	#define __JACL_TCSBRK     0x5409
	#define __JACL_TCXONC     0x540A
	#define __JACL_TCFLSH     0x540B
	#define __JACL_TIOCGWINSZ 0x5413
	#define __JACL_TIOCSWINSZ 0x5414
	#define __JACL_TIOCGSID   0x5429
#endif

/* ============================================================================ */
/* Internal: kernel ABI struct (19 c_cc, no speed fields)                       */
/* ============================================================================ */

struct __jacl_kernel_termios {
	tcflag_t c_iflag;
	tcflag_t c_oflag;
	tcflag_t c_cflag;
	tcflag_t c_lflag;
	cc_t     c_line;
	cc_t     c_cc[19];
};

/* ============================================================================ */
/* Internal helpers                                                             */
/* ============================================================================ */

#if JACL_OS_LINUX
static inline int __jacl_ioctl(int fd, unsigned long req, void *arg) {
	return (int)syscall(SYS_ioctl, fd, req, arg);
}

static inline void __jacl_to_kernel(const struct termios *u, struct __jacl_kernel_termios *k) {
	k->c_iflag = u->c_iflag;
	k->c_oflag = u->c_oflag;
	k->c_cflag = u->c_cflag;
	k->c_lflag = u->c_lflag;
	k->c_line  = u->c_line;
	for (int i = 0; i < 19; i++) k->c_cc[i] = u->c_cc[i];
	/* Apply __ispeed to c_cflag baud bits if set */
	if (u->__ispeed)
		k->c_cflag = (k->c_cflag & ~(CBAUD | CBAUDEX)) | (u->__ispeed & (CBAUD | CBAUDEX));
}

static inline void __jacl_from_kernel(struct termios *u, const struct __jacl_kernel_termios *k) {
	u->c_iflag = k->c_iflag;
	u->c_oflag = k->c_oflag;
	u->c_cflag = k->c_cflag;
	u->c_lflag = k->c_lflag;
	u->c_line  = k->c_line;
	for (int i = 0; i < 19; i++) u->c_cc[i] = k->c_cc[i];
	for (int i = 19; i < NCCS; i++) u->c_cc[i] = 0;
	u->__ispeed = k->c_cflag & (CBAUD | CBAUDEX);
	u->__ospeed = k->c_cflag & (CBAUD | CBAUDEX);
}
#endif

/* ============================================================================ */
/* POSIX Public ABI                                                             */
/* ============================================================================ */

#if JACL_OS_LINUX

static inline speed_t cfgetispeed(const struct termios *t) {
	if (!t) return 0;
	return t->__ispeed ? t->__ispeed : (t->c_cflag & (CBAUD | CBAUDEX));
}

static inline speed_t cfgetospeed(const struct termios *t) {
	if (!t) return 0;
	return t->__ospeed ? t->__ospeed : (t->c_cflag & (CBAUD | CBAUDEX));
}

static inline int cfsetispeed(struct termios *t, speed_t speed) {
	if (!t) { errno = EINVAL; return -1; }
	t->__ispeed = speed;
	return 0;
}

static inline int cfsetospeed(struct termios *t, speed_t speed) {
	if (!t) { errno = EINVAL; return -1; }
	t->__ospeed = speed;
	return 0;
}

static inline int cfsetspeed(struct termios *t, speed_t speed) {
	if (!t) { errno = EINVAL; return -1; }
	t->__ispeed = speed;
	t->__ospeed = speed;
	t->c_cflag = (t->c_cflag & ~(CBAUD | CBAUDEX)) | (speed & (CBAUD | CBAUDEX));
	return 0;
}

static inline int tcgetattr(int fd, struct termios *t) {
	if (!t) { errno = EINVAL; return -1; }
	struct __jacl_kernel_termios k;
	int r = __jacl_ioctl(fd, __JACL_TCGETS, &k);
	if (r < 0) return -1;
	__jacl_from_kernel(t, &k);
	return 0;
}

static inline int tcsetattr(int fd, int action, const struct termios *t) {
	if (!t) { errno = EINVAL; return -1; }
	if (action != TCSANOW && action != TCSADRAIN && action != TCSAFLUSH) {
		errno = EINVAL; return -1;
	}
	struct __jacl_kernel_termios k;
	__jacl_to_kernel(t, &k);
	unsigned long req;
	switch (action) {
		case TCSANOW:   req = __JACL_TCSETS;  break;
		case TCSADRAIN: req = __JACL_TCSETSW; break;
		case TCSAFLUSH: req = __JACL_TCSETSF; break;
		default: errno = EINVAL; return -1;
	}
	return __jacl_ioctl(fd, req, &k);
}

static inline int tcsendbreak(int fd, int duration) {
	(void)duration;
	return __jacl_ioctl(fd, __JACL_TCSBRK, (void *)0);
}

static inline int tcdrain(int fd) {
	return __jacl_ioctl(fd, __JACL_TCSBRK, (void *)1);
}

static inline int tcflush(int fd, int queue) {
	if (queue != TCIFLUSH && queue != TCOFLUSH && queue != TCIOFLUSH) {
		errno = EINVAL; return -1;
	}
	return __jacl_ioctl(fd, __JACL_TCFLSH, (void *)(long)queue);
}

static inline int tcflow(int fd, int action) {
	if (action != TCOOFF && action != TCOON && action != TCIOFF && action != TCION) {
		errno = EINVAL; return -1;
	}
	return __jacl_ioctl(fd, __JACL_TCXONC, (void *)(long)action);
}

static inline pid_t tcgetsid(int fd) {
	int sid = 0;
	if (__jacl_ioctl(fd, __JACL_TIOCGSID, &sid) < 0) return (pid_t)-1;
	return (pid_t)sid;
}

static inline void cfmakeraw(struct termios *t) {
	if (!t) return;
	t->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	t->c_oflag &= ~OPOST;
	t->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	t->c_cflag &= ~(CSIZE | PARENB);
	t->c_cflag |= CS8;
	t->c_cc[VMIN]  = 1;
	t->c_cc[VTIME] = 0;
}

static inline int tcgetwinsize(int fd, struct winsize *w) {
	if (!w) { errno = EINVAL; return -1; }
	return __jacl_ioctl(fd, __JACL_TIOCGWINSZ, w);
}

static inline int tcsetwinsize(int fd, const struct winsize *w) {
	if (!w) { errno = EINVAL; return -1; }
	return __jacl_ioctl(fd, __JACL_TIOCSWINSZ, (void *)w);
}

#else /* !JACL_OS_LINUX */

static inline speed_t cfgetispeed(const struct termios *t) { (void)t; return 0; }
static inline speed_t cfgetospeed(const struct termios *t) { (void)t; return 0; }
static inline int cfsetispeed(struct termios *t, speed_t s) { (void)t; (void)s; errno = ENOSYS; return -1; }
static inline int cfsetospeed(struct termios *t, speed_t s) { (void)t; (void)s; errno = ENOSYS; return -1; }
static inline int cfsetspeed(struct termios *t, speed_t s) { (void)t; (void)s; errno = ENOSYS; return -1; }
static inline int tcgetattr(int fd, struct termios *t) { (void)fd; (void)t; errno = ENOSYS; return -1; }
static inline int tcsetattr(int fd, int a, const struct termios *t) { (void)fd; (void)a; (void)t; errno = ENOSYS; return -1; }
static inline int tcsendbreak(int fd, int d) { (void)fd; (void)d; errno = ENOSYS; return -1; }
static inline int tcdrain(int fd) { (void)fd; errno = ENOSYS; return -1; }
static inline int tcflush(int fd, int q) { (void)fd; (void)q; errno = ENOSYS; return -1; }
static inline int tcflow(int fd, int a) { (void)fd; (void)a; errno = ENOSYS; return -1; }
static inline pid_t tcgetsid(int fd) { (void)fd; errno = ENOSYS; return (pid_t)-1; }
static inline void cfmakeraw(struct termios *t) { (void)t; }
static inline int tcgetwinsize(int fd, struct winsize *w) { (void)fd; (void)w; errno = ENOSYS; return -1; }
static inline int tcsetwinsize(int fd, const struct winsize *w) { (void)fd; (void)w; errno = ENOSYS; return -1; }

#endif

#ifdef __cplusplus
}
#endif

#endif /* _TERMIOS_H */
