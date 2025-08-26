// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef WASI_C
#define WASI_C

#include <wasi.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// — String length helper —
size_t wasi_strlen(const char* s) {
		size_t len = 0;
		while (s[len]) len++;
		return len;
}

// — Core WASI write function —
int wasi_write_fd(const char* buf, size_t len, __wasi_fd_t fd) {
		if (!buf || len == 0) return 0;

		__wasi_ciovec_t iov = { buf, len };
		__wasi_size_t nwritten = 0;
		__wasi_errno_t err = __wasi_fd_write(fd, &iov, 1, &nwritten);

		// Return bytes written, or -1 on error
		return (err == 0) ? (int)nwritten : -1;
}

// — Convenience functions for standard streams —
int wasi_write_stdout(const char* buf, size_t len) {
		return wasi_write_fd(buf, len, WASI_STDOUT_FD);
}

int wasi_write_stderr(const char* buf, size_t len) {
		return wasi_write_fd(buf, len, WASI_STDERR_FD);
}

#ifdef __cplusplus
}
#endif

#endif /* WASI_C */

