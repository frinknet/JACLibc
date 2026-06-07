/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _SYS_UTSNAME_H
#define _SYS_UTSNAME_H

#pragma once

#include <config.h>
#include <string.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

struct utsname {
	char sysname[65];
	char nodename[65];
	char release[65];
	char version[65];
	char machine[65];
	char domainname[65];
};

static inline int uname(struct utsname *buf) {
	if (!buf) return (__errno_set(EFAULT), -1);

#if JACL_OS_LINUX || JACL_OS_ANDROID
	#include <sys/syscall.h>
	if (syscall(SYS_uname, buf) < 0) return -1;

	return 0;
#elif JACL_OS_WINDOWS
	strncpy(buf->sysname, "Windows", sizeof(buf->sysname) - 1);
	strncpy(buf->nodename, "localhost", sizeof(buf->nodename) - 1);
	strncpy(buf->release, "10.0", sizeof(buf->release) - 1);
	strncpy(buf->version, "JACL-Polyglot", sizeof(buf->version) - 1);
	strncpy(buf->machine, "x86_64", sizeof(buf->machine) - 1);
	buf->domainname[0] = '\0';

	return 0;
#else
	/* Bare Metal / Polyglot Default */
	strncpy(buf->sysname, "JACL", sizeof(buf->sysname) - 1);
	strncpy(buf->nodename, "polyglot", sizeof(buf->nodename) - 1);
	strncpy(buf->release, "1.0", sizeof(buf->release) - 1);
	strncpy(buf->version, "bare-metal", sizeof(buf->version) - 1);
	strncpy(buf->machine, "unknown", sizeof(buf->machine) - 1);
	buf->domainname[0] = '\0';

	return 0;
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* _SYS_UTSNAME_H */
