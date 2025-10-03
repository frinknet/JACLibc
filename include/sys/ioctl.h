/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef SYS_IOCTL_H
#define SYS_IOCTL_H

#include <config.h>
#include <errno.h> // ENOSYS
#include <sys/types.h> // POSIX

#if JACL_HAS_POSIX

#include <sys/syscall.h> // syscall()
#include <stdarg.h> // va_start(), va_arg(), va_end()

/* Window size structure for terminal ioctls */
struct winsize {
	unsigned short ws_row;      /* rows, in characters */
	unsigned short ws_col;      /* columns, in characters */
	unsigned short ws_xpixel;   /* horizontal size, pixels */
	unsigned short ws_ypixel;   /* vertical size, pixels */
};

/* Terminal window size ioctls */
#if JACL_OS_LINUX || JACL_ARCH_WASM
  #define TIOCGWINSZ  0x5413
  #define TIOCSWINSZ  0x5414
  #define TIOCGPGRP   0x540F
  #define TIOCSPGRP   0x5410
  #define FIONREAD    0x541B
  #define FIONBIO     0x5421
  #define FIONCLEX    0x5450
  #define FIOCLEX     0x5451
  #define FIOASYNC    0x5452
#elif JACL_OS_BSD || JACL_OS_DARWIN
  #define TIOCGWINSZ  0x40087468
  #define TIOCSWINSZ  0x80087467
  #define TIOCGPGRP   0x40047477
  #define TIOCSPGRP   0x80047476
  #define FIONREAD    0x4004667F
  #define FIONBIO     0x8004667E
  #define FIONCLEX    0x20006602
  #define FIOCLEX     0x20006601
  #define FIOASYNC    0x8004667D
#endif

/* ioctl() - device control operations */
static inline int ioctl(int fd, unsigned long request, ...) {
#if defined(SYS_ioctl)
	va_list ap;
	void *arg;

	va_start(ap, request);
	arg = va_arg(ap, void*);
	va_end(ap);

	return (int)syscall(SYS_ioctl, fd, request, arg);
#else
	errno = ENOSYS;

	return -1;
#endif
}

#endif /* JACL_HAS_POSIX */

#endif /* SYS_IOCTL_H */
