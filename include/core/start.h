/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _CORE_START_H
#define _CORE_START_H
#pragma once

#include <config.h>
#include <stdlib.h>
#include <setjmp.h>

/* Get TLS functions from format */
#define __BIN_INIT
#include JACL_BIN_FILE

#ifdef __cplusplus
extern "C" {
#endif

/* Global environment pointer */
char **environ = NULL;

/* Work with any main */
extern int main();

/* OS specific init */
#define __OS_INIT
#include JACL_OS_FILE

/* Futex fallback state */
#if JACL_HAS_POSIX && !JACL_OS_LINUX && !JACL_OS_NETBSD
#include <pthread.h>
pthread_mutex_t __futex_locks[__FUTEX_BUCKETS] = { [0 ... __FUTEX_BUCKETS-1] = PTHREAD_MUTEX_INITIALIZER };
pthread_cond_t  __futex_conds[__FUTEX_BUCKETS] = { [0 ... __FUTEX_BUCKETS-1] = PTHREAD_COND_INITIALIZER };
#endif

/* Stuff for for getopt */
char *optarg = NULL;
int optind = 1;
int opterr = 1;
int optopt = 0;

/* Real Start */
void _start_main(long *p) {
	int   argc = (int)p[0];
	char **argv = (char **)(p + 1);
	char **envp = argv + argc + 1;

	environ = envp;

	/* Initialize os */
	__jacl_os_init();

	/* Initialize constructors */
	if (INIT_START && INIT_END) {
		for (init_func_t *func = INIT_START; func < INIT_END; func++) {
			if (*func) (*func)();
		}
	}

	int r = main(argc, argv, envp);

	_exit(r);
}

/* Arch _start is ASM */
#define __ARCH_START
#include JACL_ARCH_FILE

#ifdef __cplusplus
}
#endif

#endif /* _CORE_START_H */
