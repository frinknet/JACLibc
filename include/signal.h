// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef SIGNAL_H
#define SIGNAL_H

/* C99 Signal Handling */

typedef void (*sig_t)(int);

/* Standard signals */
#define SIGABRT  6
#define SIGFPE	 8
#define SIGILL	 4
#define SIGINT	 2
#define SIGSEGV 11
#define SIGTERM 15

/* Special handlers */
#define SIG_DFL ((sig_t)0)
#define SIG_IGN ((sig_t)1)
#define SIG_ERR ((sig_t)-1)

/* Raise a signal */
static inline int raise(int sig) { (void)sig; return 0; }

/* Set signal handler */
static inline sig_t signal(int sig, sig_t handler) { (void)sig; (void)handler; return SIG_ERR; }

#endif /* SIGNAL_H */
