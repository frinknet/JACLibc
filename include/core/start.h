/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef CORE_START_H
#define CORE_START_H
#pragma once

#include <config.h>
#include <stdlib.h>

/* Get TLS functions from format */
#define __FMT_INIT
#include JACL_HEADER(fmt, JACL_FMT)

#ifdef __cplusplus
extern "C" {
#endif

/* Global environment pointer */
char **environ = NULL;

/* Work with any main */
extern int main();

/* OS specific init */
#define __OS_INIT
#include JACL_HEADER(os, JACL_OS)

/* Convergence Start */
void _start_main(long *p) {
	int   argc = (int)p[0];
	char **argv = (char **)(p + 1);
	char **envp = argv + argc + 1;

	environ = envp;

	/* Initialize os */
	__jacl_init_os();

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
#include JACL_HEADER(arch, JACL_ARCH)

#ifdef __cplusplus
}
#endif

#endif /* CORE_START_H */
