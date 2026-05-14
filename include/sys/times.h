/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef _SYS_TIMES_H
#define _SYS_TIMES_H

#pragma once

#include <config.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * Structure returned by times()
 * All times are in clock ticks (divide by sysconf(_SC_CLK_TCK) for seconds)
 */
struct tms {
	clock_t tms_utime;  /* User CPU time */
	clock_t tms_stime;  /* System CPU time */
	clock_t tms_cutime; /* User CPU time of terminated children */
	clock_t tms_cstime; /* System CPU time of terminated children */
};

/* 
 * Get process times.
 * Returns clock ticks since an arbitrary point in the past (usually boot).
 * On error, returns (clock_t)-1.
 */
static inline clock_t times(struct tms *buf) {
	if (!buf) return (clock_t)-1;

#if JACL_OS_LINUX || JACL_OS_ANDROID
	#include <sys/syscall.h>
	return (clock_t)syscall(SYS_times, buf);
#elif JACL_OS_WINDOWS
	/* Windows doesn't have a direct equivalent. 
	   We can use GetProcessTimes but mapping to tms is complex.
	   For now, return 0 and fill with zeros. */
	memset(buf, 0, sizeof(struct tms));
	return 0;
#else
	/* Bare Metal / WASM / Other */
	memset(buf, 0, sizeof(struct tms));
	return 0;
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* _SYS_TIMES_H */
