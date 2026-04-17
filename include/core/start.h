/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _CORE_START_H
#define _CORE_START_H
#pragma once

#include <config.h>
#include <stdlib.h>

/* Get TLS functions from format */
#define __FMT_INIT
#include JACL_FMT_FILE

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

/* Convergence Start */
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
