/* (c) 2025 FRINKnet & Friends – MIT licence */

#if defined(JACL_OS)
	#include JACL_HEADER(os,JACL_OS)
#elif defined(__linux__)
	#include <os/linux.h>
#elif defined(_WIN32)
	#include <os/windows.h>
#elif defined(__APPLE__) && defined(__MACH__)
	#include <os/darwin.h>
#elif defined(FreeBSD)
#include <os/freebsd.h>
#elif defined(NetBSD)
#include <os/netbsd.h>
#elif defined(OpenBSD)
#include <os/openbsd.h>
#elif defined(DragonFly)
#include <os/dragonfly.h>
#elif defined(__wasi__)
	#include <os/wasi.h>
#elif defined(__wasm__)
	#include <os/jsrun.h>
#elif defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0)
	#define JACL_OS none
		#include <os/none.h>
#else /* unknown */
	#error "JACLibc - Unsupported Operating System"
#endif /* os check */
