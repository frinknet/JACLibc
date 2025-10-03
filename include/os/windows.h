/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef OS_WINDOWS_H
#define OS_WINDOWS_H
#pragma once

/* Windows doesn't support Unix syscalls */
static inline long syscall(long number, ...) {
	(void)number;
	return -1; /* Not supported */
}

#endif // OS_WINDOWS_H
