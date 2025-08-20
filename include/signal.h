// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef SIGNAL_H
#define SIGNAL_H

typedef void (*sig_t)(int);
typedef uint32_t sigset_t;

#define SIG_DFL		 ((sig_t)0)
#define SIG_IGN		 ((sig_t)1)
#define SIG_ERR		 ((sig_t)-1)
#define SIG_BLOCK		0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

#endif
