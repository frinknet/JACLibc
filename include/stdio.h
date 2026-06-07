/* (c) 2025-2026 FRINKnet & Friends – MIT licence */
#ifndef _STDIO_H
#define _STDIO_H
#pragma once

#include <config.h>
#include <errno.h>
#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>

#if JACL_HAS_C23
#define __STDC_VERSION_STDIO_H__ 202311L
#endif

#ifndef BUFSIZ
#define BUFSIZ 1024
#endif
#ifndef OVRSIZ
#define OVRSIZ 32
#endif
#define OVRBUF (BUFSIZ + OVRSIZ)
#ifndef FILENAME_MAX
#define FILENAME_MAX 4096
#endif
#ifndef FOPEN_MAX
#define FOPEN_MAX 16
#endif
#ifndef TMP_MAX
#define TMP_MAX 32767
#endif
#ifndef L_ctermid
#define L_ctermid 32
#endif
#ifndef EOF
#define EOF (-1)
#endif

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define _IOFBF 0  /* Full buffering */
#define _IOLBF 1  /* Line buffering */
#define _IONBF 2  /* No buffering */

#define __SRD     0x01  /* read mode */
#define __SWR     0x02  /* write mode */
#define __SEOF    0x04  /* end of file reached */
#define __SERR    0x08  /* error occurred */
#define __SMEM    0x10  /* memory stream (fmemopen/open_memstream) */
#define __SCUSTOM 0x20  /* custom I/O (funopen) */
#define __SPOPEN  0x40  /* popen stream */
#define __SMODE   (__SRD | __SWR)
#define __SSTATE  (__SEOF | __SERR)
#define __STYPE   (__SMEM | __SCUSTOM | __SPOPEN)

#if JACL_OS_ANDROID
	#define JACL_SHELL_PATH "/system/bin/sh"
#elif JACL_OS_WINDOWS
	#define JACL_SHELL_PATH "cmd.exe"
#else
	#define JACL_SHELL_PATH "/bin/sh"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef TEMP_DIR
#ifdef JACL_OS_WINDOWS
#define TEMP_DIR "C:\\Temp\\"
#else
#define TEMP_DIR "/tmp/"
#endif
#endif

#ifndef L_tmpnam
#define L_tmpnam 64
#endif

#ifdef __cplusplus
#define restrict __restrict__
#elif !JACL_HAS_C99
#define restrict	 /* nothing */
#endif

typedef long long fpos_t;
typedef struct __jacl_file {
	// IO fields
	int _cnt;          // Characters remaining
	int _fd;           // File descriptor
	char *_ptr;        // Current position in buffer
	char *_end;        // End of buffer
	char *_base;       // Buffer base

	// Control fields
	unsigned char _flags;        // Status flags (__SRD, __SWR, __SEOF, __SERR)
	unsigned char _bufmode;      // Buffering mode (_IOFBF, _IOLBF, _IONBF)
	unsigned char _orientation;  // Stream orientation (0=unset, 1=byte, 2=wide)
	unsigned char _last_op;      // Last operation (0=none, 1=read, 2=write)
	unsigned int _buf_owned;     // 0=user buffer, 1=internal buffer

	// Position fields
	off_t _read_pos;   // File position of read buffer start
	off_t _write_pos;  // File position of write buffer start

	// File fields
	char *_tmpfname;   // Temp filename or pid (tmpfile/popen)

	// Buffer size fields
	size_t _bufsiz;    // Buffer size (excluding overflow)
	size_t _ovrsiz;    // Overflow/pushback size

	#if JACL_HAS_THREADS
		pthread_mutex_t _lock;
	#endif
} FILE;
struct __jacl_memstream_state {
	char **ptr_loc;
	size_t *size_loc;
};
struct __jacl_custom_io {
	void *cookie;
	int (*readfn)(void *cookie, char *buf, int size);
	int (*writefn)(void *cookie, const char *buf, int size);
	fpos_t (*seekfn)(void *cookie, fpos_t offset, int whence);
	int (*closefn)(void *cookie);
};

void __jacl_stream_register(FILE *f);
void __jacl_stream_unregister(FILE *f);
int __jacl_stream_flush(FILE *f);

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;
extern int __jacl_stdio_init;

/* IO Initialization */
static inline void __jacl_init_stdio(void) {
	if (!__jacl_stdio_init) {
		__jacl_stream_register(stdin);
		__jacl_stream_register(stdout);
		__jacl_stream_register(stderr);

		stdout->_bufmode = isatty(stdout->_fd) ? _IOLBF : _IOFBF;

		__jacl_stdio_init = 1;
	}
}

/* File Macros */
#define FILE_INIT(fd, flags, mode, buf) { 0, fd, buf + OVRSIZ, buf + OVRBUF, buf, flags, mode, 0, 0, 1, 0, 0, NULL, BUFSIZ, OVRSIZ }
#define STATIC_FILE(name, fd, flags, mode) \
static char __jacl_##name##_buf[OVRBUF]; \
static FILE __jacl_##name##_file = FILE_INIT(fd, flags, mode, __jacl_##name##_buf); \
FILE* name = &__jacl_##name##_file
#define __JACL_BUF_DATA(f)  ((f)->_base + OVRSIZ)
#define __JACL_BUF_WRITE(f)  ((f)->_ptr - __JACL_BUF_DATA(f))
#define __JACL_BUF_RESET(f) do { (f)->_cnt = 0; (f)->_ptr = __JACL_BUF_DATA(f); } while(0)
#define __JACL_HAS_MODE(f, m)  ((f)->_flags & (m))
#define __JACL_SET_MODE(f, m)  ((f)->_flags |= (m))
#define __JACL_CLR_MODE(f, m)  ((f)->_flags &= ~(m))
#define __JACL_CLR_STATE(f)    ((f)->_flags &= ~__SSTATE)
#define __JACL_RTN_ERROR(f, rtn) do { __JACL_SET_MODE(f, __SERR); return (rtn); } while(0)
#define __JACL_RTN_UNLOCK(f, expr) do { flockfile(f); __typeof__(expr) __r = (expr); funlockfile(f); return __r; } while(0)

/* Buffer Helpers */
static inline void __jacl_flush_partial(FILE *f, size_t written, size_t to_write) {
	int more = (size_t)written < to_write;

	if (more) memmove(f->_base + OVRSIZ, f->_base + OVRSIZ + written, to_write - written);

	f->_ptr = more ? f->_base + OVRSIZ + (to_write - written) : f->_base + OVRSIZ;
}
static inline int __jacl_flush_fd(FILE *f) {
	size_t to_write = f->_ptr - (f->_base + OVRSIZ);

	if (to_write == 0) return 0;

	ssize_t written = write(f->_fd, f->_base + OVRSIZ, to_write);

	if (JACL_UNLIKELY(written < 0)) __JACL_RTN_ERROR(f, EOF);
	else __jacl_flush_partial(f, written, to_write);

	struct stat st;

	f->_write_pos = (fstat(f->_fd, &st) == 0 && S_ISFIFO(st.st_mode)) ? f->_write_pos + written : lseek(f->_fd, 0, SEEK_CUR);
	f->_last_op = 2;

	return 0;
}
static inline int __jacl_flush_mem(FILE *f) {
	size_t written = f->_ptr - f->_base;

	if (f->_ptr >= f->_end) {
		if (f->_tmpfname) { /* open_memstream can realloc */
			size_t old_size = f->_bufsiz, new_size = old_size * 2;
			char *new_mem = (char *)realloc(f->_base, new_size);

			if (!new_mem) { f->_flags |= __SERR; return EOF; }
			else memset(new_mem + old_size, 0, new_size - old_size);

			f->_ptr = new_mem + written; f->_base = new_mem; f->_end = new_mem + new_size; f->_bufsiz = new_size;
		} else { __JACL_RTN_ERROR(f, EOF); }
	}

	if (f->_tmpfname) {
		struct __jacl_memstream_state *state = (struct __jacl_memstream_state *)f->_tmpfname;

		if (written < f->_bufsiz) f->_base[written] = '\0';

		*state->ptr_loc = f->_base; *state->size_loc = written;
	}

	f->_last_op = 2;

	return 0;
}
static inline int __jacl_buffer_flags(const char *mode) {
	int flags = 0;

	if (!mode || !*mode) return -1;

	switch (*mode) {
		case 'r': flags = O_RDONLY; break;
		case 'w': flags = O_WRONLY | O_CREAT | O_TRUNC; break;
		case 'a': flags = O_WRONLY | O_CREAT | O_APPEND; break;
		default: return -1;
	}

	if (strchr(mode, '+')) flags = (flags & ~O_WRONLY) | O_RDWR;

	#if JACL_HAS_C11 || (defined(O_EXCL) && JACL_HAS_POSIX)
	if (strchr(mode, 'x')) flags |= O_EXCL;
	#endif

	return flags;
}
static inline int __jacl_buffer_reload(FILE *f) {
	if (JACL_UNLIKELY(!f || !(f->_flags & __SRD))) return EOF;

	off_t cur_pos = 0;
	struct stat st;

	if (fstat(f->_fd, &st) == 0 && !S_ISFIFO(st.st_mode)) {
		cur_pos = lseek(f->_fd, 0, SEEK_CUR);

		if (JACL_UNLIKELY(cur_pos < 0)) return f->_flags |= __SERR, EOF;
		if (f->_cnt > 0) cur_pos -= f->_cnt;
	}

	__JACL_BUF_RESET(f);

	ssize_t n = read(f->_fd, f->_ptr, BUFSIZ);

	if (JACL_UNLIKELY(n < 0)) __JACL_RTN_ERROR(f, EOF);
	if (JACL_UNLIKELY(n == 0)) return __JACL_SET_MODE(f, __SEOF), EOF;

	f->_read_pos = cur_pos; f->_cnt = (int)n - 1; f->_last_op = 1;

	return (unsigned char)*f->_ptr++;
}
static inline int __jacl_buffer_flush(FILE *f) {
	if (JACL_UNLIKELY(!f || !(f->_flags & __SWR))) return 0;
	if (f->_flags & __SMEM) return __jacl_flush_mem(f);

	if (f->_flags & __SCUSTOM) {
		struct __jacl_custom_io *io = (struct __jacl_custom_io *)f->_tmpfname;
		size_t to_write = f->_ptr - (f->_base + OVRSIZ);

		if (to_write == 0) return 0;

		int written = io->writefn(io->cookie, f->_base + OVRSIZ, (int)to_write);

		if (written < 0) __JACL_RTN_ERROR(f, EOF);
		else { __jacl_flush_partial(f, written, to_write); f->_last_op = 2; }

		return 0;
	}

	return __jacl_flush_fd(f);
}
static inline int __jacl_buffer_input(FILE *f) {
	__jacl_init_stdio();

	if (JACL_UNLIKELY(!f || !(f->_flags & __SRD))) return EOF;
	if (f->_last_op == 2) { __jacl_buffer_flush(f); __JACL_BUF_RESET(f); }
	if (f->_flags & __SMEM) return f->_ptr >= f->_end ? (f->_flags |= __SEOF, EOF) : (f->_last_op = 1, (unsigned char)*f->_ptr++);

	if (f->_flags & __SCUSTOM) {
		struct __jacl_custom_io *io = (struct __jacl_custom_io *)f->_tmpfname;

		if (f->_cnt > 0) return f->_cnt--, (unsigned char)*f->_ptr++;

		f->_ptr = f->_base + OVRSIZ;

		int n = io->readfn(io->cookie, (char *)f->_ptr, (int)(f->_end - f->_ptr));

		return (n <= 0) ? (__JACL_SET_MODE(f, (n == 0) ? __SEOF : __SERR), EOF) : (f->_cnt = n - 1, f->_last_op = 1, (unsigned char)*f->_ptr++);
	}

	if (f->_cnt > 0) return f->_cnt--, (unsigned char)*f->_ptr++;
	else return __jacl_buffer_reload(f);
}
static inline int __jacl_buffer_output(FILE *f) {
	__jacl_init_stdio();

	if (JACL_UNLIKELY(!f || !__JACL_HAS_MODE(f, __SWR))) return 0;

	int flush = (f->_bufmode == _IONBF) || (f->_ptr >= f->_end) || (f->_bufmode == _IOLBF && f->_ptr > __JACL_BUF_DATA(f) && *(f->_ptr - 1) == '\n');

	return flush ? __jacl_buffer_flush(f) : 0;
}
static inline int __jacl_stream_validate(FILE *f, int required_mode) { return (!f || (required_mode && !(f->_flags & required_mode))) ? (__errno_set(EBADF), 0) : 1; }
static inline FILE *__jacl_stream_alloc(int fd, int flags, int mode) {
	FILE *f = (FILE *)malloc(sizeof(FILE));

	if (!f) { return (__errno_set(ENOMEM), NULL); }

	char *buf = (char *)malloc(OVRBUF);

	if (!buf) { free(f); return (__errno_set(ENOMEM), NULL); }

	*f = (FILE)FILE_INIT(fd, flags, mode, buf);

	#if JACL_HAS_THREADS
	pthread_mutex_init(&f->_lock, NULL);
	#endif

	__jacl_stream_register(f);

	return f;
}
static inline void __jacl_stream_free(FILE *f) {
	if (f->_base && f->_buf_owned) free(f->_base);
	if (f->_tmpfname) free(f->_tmpfname);

	#if JACL_HAS_THREADS
	pthread_mutex_destroy(&f->_lock);
	#endif

	if (f != stdin && f != stdout && f != stderr) { __jacl_stream_unregister(f); free(f); }
}
static inline off_t __jacl_special_seek(FILE *f, off_t offset, int whence) {
	if (f->_flags & __SMEM) {
		off_t new_pos;

		switch (whence) {
			case SEEK_SET: new_pos = offset; break;
			case SEEK_CUR: new_pos = (f->_ptr - f->_base) + offset; break;
			case SEEK_END: new_pos = (off_t)f->_bufsiz + offset; break;
			default: return (__errno_set(EINVAL), -1);
		}

		if (new_pos < 0 || new_pos > (off_t)f->_bufsiz) { return (__errno_set(EINVAL), -1); }

		f->_ptr = f->_base + new_pos; f->_flags &= ~__SEOF; f->_last_op = 0;

		return new_pos;
	}

	if (f->_flags & __SCUSTOM) {
		struct __jacl_custom_io *io = (struct __jacl_custom_io *)f->_tmpfname;

		if (!io || !io->seekfn) { return (__errno_set(ESPIPE), -1); }

		fpos_t result = io->seekfn(io->cookie, (fpos_t)offset, whence);

		if (result < 0) return -1;

		__JACL_BUF_RESET(f); f->_flags &= ~__SEOF; f->_last_op = 0;

		return (off_t)result;
	}

	return -2;
}
static inline int __jacl_switch_to_read(FILE *f) {
	if (f->_flags & __SWR) {
	if (__jacl_buffer_flush(f) == EOF) return -1;
		__JACL_CLR_MODE(f, __SWR);
		__JACL_SET_MODE(f, __SRD);
		__JACL_BUF_RESET(f);
	}

	return 0;
}
static inline void __jacl_switch_to_write(FILE *f) { if ((f->_flags & __SRD) && !(f->_flags & __SWR)) { __JACL_CLR_MODE(f, __SRD); __JACL_SET_MODE(f, __SWR); __JACL_BUF_RESET(f); } }
static inline off_t __jacl_special_tell(FILE *f) {
	if (f->_flags & __SMEM) return (off_t)(f->_ptr - f->_base);
	if (f->_flags & __SCUSTOM) {
		struct __jacl_custom_io *io = (struct __jacl_custom_io *)f->_tmpfname;

		if (!io || !io->seekfn) { return (__errno_set(ESPIPE), -1); }

		fpos_t pos = io->seekfn(io->cookie, 0, SEEK_CUR);

		if (pos < 0) return -1;

		return (off_t)(pos + (__JACL_HAS_MODE(f, __SWR) ? __JACL_BUF_WRITE(f) : (__JACL_HAS_MODE(f, __SRD) ? -f->_cnt : 0)));
	}

	return -2;
}

/* String IO */
int vprintf(const char * restrict fmt, va_list ap);
int vfprintf(FILE * restrict f, const char * restrict fmt, va_list ap);
int vsprintf(char * restrict s, const char * restrict fmt, va_list ap);
int printf(const char* restrict fmt, ...);
int fprintf(FILE* restrict f, const char* restrict fmt, ...);
int sprintf(char * restrict s, const char * restrict fmt, ...);

#if JACL_HAS_C99
int vsnprintf(char * restrict s, size_t n, const char * restrict fmt, va_list ap);
int vdprintf(int fd, const char* restrict fmt, va_list ap);
int vasprintf(char **strp, const char *fmt, va_list ap);
int snprintf(char * restrict s, size_t n, const char * restrict fmt, ...);
int dprintf(int fd, const char* restrict fmt, ...);
int asprintf(char **strp, const char *fmt, ...);
#endif

int vscanf(const char * restrict fmt, va_list ap);
int vfscanf(FILE * restrict f, const char * restrict fmt, va_list ap);
int vsscanf(const char * restrict s, const char* restrict fmt, va_list ap);
int scanf(const char * restrict fmt, ...);
int fscanf(FILE* restrict f, const char * restrict fmt, ...);
int sscanf(const char * restrict s, const char * restrict fmt, ...);

/* Error Handling */
static inline int feof(FILE* f) { return f ? __JACL_HAS_MODE(f, __SEOF) : 0; }
static inline int ferror(FILE* f) { return f ? __JACL_HAS_MODE(f, __SERR) : 0; }
static inline void clearerr(FILE* f) { if (f) __JACL_CLR_STATE(f); __errno_clr(); }
static inline void perror(const char* s) { fprintf(stderr, "%s%s%s\n", s ? s : "", s ? ": " : "", strerror(errno)); }

/* Buffering IO */
static inline int setvbuf(FILE* restrict f, char* restrict buf, int mode, size_t size) {
	__jacl_init_stdio();

	if (!f) { return (__errno_set(EBADF), EOF); }
	if (mode != _IOFBF && mode != _IOLBF && mode != _IONBF) { return (__errno_set(EINVAL), EOF); }
	if (f->_flags & __SWR) __jacl_buffer_output(f);
	if (f->_buf_owned && f->_base) free(f->_base);

	f->_bufmode = mode;

	if (mode == _IONBF) {
		f->_base = f->_ptr = f->_end = NULL; f->_bufsiz = f->_ovrsiz = f->_buf_owned = 0;
	} else if (buf) {
		f->_base = f->_ptr = buf; f->_ovrsiz = f->_buf_owned = 0; f->_bufsiz = size; f->_end = buf + size;
	} else {
		size_t bufsize = size > 0 ? size : BUFSIZ;

		f->_base = malloc(bufsize + OVRSIZ);

		if (!f->_base) { return (__errno_set(ENOMEM), EOF); }

		f->_ptr = f->_base + OVRSIZ; f->_end = f->_base + bufsize + OVRSIZ; f->_bufsiz = bufsize; f->_ovrsiz = OVRSIZ; f->_buf_owned = 1;
	}

	f->_cnt = 0; f->_flags &= ~(__SERR | __SEOF);

	return 0;
}
static inline void setbuf(FILE* restrict f, char* restrict buf) { setvbuf(f, buf, buf ? _IOFBF : _IONBF, BUFSIZ); }

/* Thread Support */
#if JACL_HAS_THREADS
#include <pthread.h>
static inline void flockfile(FILE *f) { if (f) pthread_mutex_lock(&f->_lock); }
static inline int ftrylockfile(FILE *f) { return f ? pthread_mutex_trylock(&f->_lock) : 0; }
static inline void funlockfile(FILE *f) { if (f) pthread_mutex_unlock(&f->_lock); }
#else
#define flockfile(s) ((void)0)
#define ftrylockfile(s) (0)
#define funlockfile(s) ((void)0)
#endif

/* Unlocked IO */
static inline int fgetc_unlocked(FILE* f) { return (!__jacl_stream_validate(f, 0) || __jacl_switch_to_read(f) < 0) ? EOF : __jacl_buffer_input(f); }
static inline int fputc_unlocked(int c, FILE* f) {
	if (!__jacl_stream_validate(f, 0)) return EOF;

	__jacl_switch_to_write(f);

	if (!__JACL_HAS_MODE(f, __SWR)) { return (__errno_set(EBADF), EOF); }
	if (f->_last_op == 1) __JACL_BUF_RESET(f);

	if (f->_bufmode == _IONBF && !__JACL_HAS_MODE(f, __SMEM | __SCUSTOM)) {
		unsigned char ch = (unsigned char)c;

		if (write(f->_fd, &ch, 1) < 0) __JACL_RTN_ERROR(f, EOF);
		else { f->_last_op = 2; return (unsigned char)c; }
	}

	if (f->_ptr >= f->_end && __jacl_buffer_output(f) == EOF) return EOF;
	else { *f->_ptr++ = (unsigned char)c; f->_last_op = 2; }

	return (f->_bufmode == _IOLBF && c == '\n' && __jacl_buffer_output(f) == EOF) ? EOF : (unsigned char)c;
}
static inline char* fgets_unlocked(char* restrict s, int n, FILE* restrict f) {
	if (!s || !f || n <= 0) { return (__errno_set(EINVAL), NULL); }

	int i = 0, c;

	while (i + 1 < n && (c = fgetc_unlocked(f)) != EOF) { s[i++] = (char)c; if (c == '\n') break; }

	return (i == 0 && c == EOF) ? NULL : (s[i] = '\0', s);
}
static inline int fputs_unlocked(const char* restrict s, FILE* restrict f) {
	if (!s || !f) { return (__errno_set(EINVAL), EOF); }

	while (*s) { if (fputc_unlocked(*s++, f) == EOF) return EOF; }

	return 0;
}
static inline size_t fread_unlocked(void* restrict ptr, size_t size, size_t nmemb, FILE* restrict f) {
	if (!f || !ptr || size == 0 || nmemb == 0) return 0;

	size_t total = size * nmemb, copied = 0;
	unsigned char *buf = (unsigned char *)ptr;

	while (copied < total) {
		if (JACL_LIKELY(f->_cnt > 0)) {
			size_t avail = (size_t)f->_cnt < total - copied ? (size_t)f->_cnt : total - copied;

			memcpy(buf + copied, f->_ptr, avail);

			f->_ptr += avail; f->_cnt -= avail; copied += avail;
		} else {
			int c = __jacl_buffer_input(f);

			if (JACL_UNLIKELY(c == EOF)) break;

			buf[copied++] = (unsigned char)c;
		}
	}

	return copied / size;
}
static inline size_t fwrite_unlocked(const void* restrict ptr, size_t size, size_t nmemb, FILE* restrict f) {
	if (JACL_UNLIKELY(!f || !ptr || size == 0 || nmemb == 0)) return 0;

	size_t total = size * nmemb;
	const unsigned char *buf = (const unsigned char *)ptr;

	if (f->_bufmode == _IONBF && !__JACL_HAS_MODE(f, __SMEM | __SCUSTOM)) {
		ssize_t written = write(f->_fd, buf, total);

		if (written < 0) __JACL_RTN_ERROR(f, 0);
		else return written / size;
	}

	if (!__JACL_HAS_MODE(f, __SMEM | __SCUSTOM) && f->_ptr == f->_base && total >= f->_bufsiz) {
		ssize_t written = write(f->_fd, buf, total);

		if (written < 0) __JACL_RTN_ERROR(f, 0);
		else return written / size;
	}

	size_t written = 0;
	int has_newline = 0;

	while (written < total) {
		size_t space = f->_end - f->_ptr;

		if (JACL_UNLIKELY(space == 0)) {
			if (JACL_UNLIKELY(__jacl_buffer_output(f) == EOF)) break;
			else space = f->_bufsiz;
		}

		size_t to_copy = (total - written < space) ? total - written : space;

		memcpy(f->_ptr, buf + written, to_copy);

		if (f->_bufmode == _IOLBF && !has_newline) {
			for (size_t i = 0; i < to_copy; i++) { if (buf[written + i] == '\n') { has_newline = 1; break; } }
		}

		f->_ptr += to_copy; written += to_copy;
	}

	if (f->_bufmode == _IOLBF && has_newline) __jacl_buffer_output(f);

	return written / size;
}
static inline int getc_unlocked(FILE *f) { return fgetc_unlocked(f); }
static inline int putc_unlocked(int c, FILE *f) { return fputc_unlocked(c, f); }
static inline int getchar_unlocked(void) { return fgetc_unlocked(stdin); }
static inline int putchar_unlocked(int c) { return fputc_unlocked(c, stdout); }
static inline void clearerr_unlocked(FILE *f) { clearerr(f); }
static inline int feof_unlocked(FILE *f) { return feof(f); }
static inline int ferror_unlocked(FILE *f) { return ferror(f); }
static inline int setlinebuf(FILE *f) { return setvbuf(f, NULL, _IOLBF, BUFSIZ); }

/* Character IO */
static inline int fgetc(FILE* f) { __JACL_RTN_UNLOCK(f, fgetc_unlocked(f)); }
static inline int fputc(int c, FILE* f) { __JACL_RTN_UNLOCK(f, fputc_unlocked(c, f)); }
static inline int getc(FILE* f) { return fgetc(f); }
static inline int putc(int c, FILE* f) { return fputc(c, f); }
static inline int getchar(void) { return fgetc(stdin); }
static inline int putchar(int c) { return fputc(c, stdout); }
static inline int ungetc(int c, FILE* f) {
	if (!f || c == EOF) return EOF;

	flockfile(f);

	if (__jacl_switch_to_read(f) < 0 || f->_ptr == f->_base) return funlockfile(f), EOF;

	return *(--f->_ptr) = (unsigned char)c, f->_cnt++, __JACL_CLR_MODE(f, __SEOF), funlockfile(f), (unsigned char)c;
}
static inline char* fgets(char* restrict s, int n, FILE* restrict f) { __JACL_RTN_UNLOCK(f, fgets_unlocked(s, n, f)); }
static inline int fputs(const char* restrict s, FILE* restrict f) { __JACL_RTN_UNLOCK(f, fputs_unlocked(s, f)); }
static inline int puts(const char* s) { return !s ? (__errno_set(EINVAL), EOF) : (fputs(s, stdout) == EOF ? EOF : (putchar('\n') == EOF ? EOF : 0)); }
#if !JACL_HAS_C11
static inline char* gets(char *s) { return fgets(s, INT_MAX, stdin); }
#endif

/* File Positioning */
static inline int fseek(FILE* f, long offset, int whence) {
	if (!__jacl_stream_validate(f, 0)) return -1;
	if (whence < SEEK_SET || whence > SEEK_END) { return (__errno_set(EINVAL), -1); }
	if (__JACL_HAS_MODE(f, __SWR) && __jacl_buffer_flush(f) == EOF) return -1;

	off_t special = __jacl_special_seek(f, offset, whence);

	if (special != -2) return (special < 0) ? -1 : 0;
	if (whence == SEEK_CUR && __JACL_HAS_MODE(f, __SRD) && f->_cnt > 0) offset -= f->_cnt;

	off_t result = lseek(f->_fd, offset, whence);

	return result < 0 ? -1 : (f->_cnt = 0, f->_ptr = __JACL_BUF_DATA(f), __JACL_CLR_MODE(f, __SEOF), f->_end = f->_base + OVRBUF, f->_read_pos = f->_write_pos = result, f->_last_op = 0, 0);
}
static inline long ftell(FILE* f) {
	if (!f) { return (__errno_set(EBADF), -1L); }

	off_t special = __jacl_special_tell(f);

	if (special != -2) return (long)special;

	off_t pos = lseek(f->_fd, 0, SEEK_CUR);

	return (pos < 0) ? -1L : (long)(pos + (__JACL_HAS_MODE(f, __SWR) ? __JACL_BUF_WRITE(f) : (__JACL_HAS_MODE(f, __SRD) ? -f->_cnt : 0)));
}
static inline void rewind(FILE* f) {
	if (!f) return;
	if (__JACL_HAS_MODE(f, __SWR)) __jacl_buffer_flush(f);
	if (__JACL_HAS_MODE(f, __SRD)) { f->_cnt = 0; f->_ptr = __JACL_HAS_MODE(f, __SMEM) ? f->_base : __JACL_BUF_DATA(f); }

	fseek(f, 0L, SEEK_SET), clearerr(f);
}
static inline int fgetpos(FILE* restrict f, fpos_t* restrict pos) {
	if (!f || !pos) { return (__errno_set(EINVAL), -1); }

	long offset = ftell(f);

	return (offset < 0) ? -1 : (*pos = (fpos_t)offset, 0);
}
static inline int fsetpos(FILE* f, const fpos_t* pos) { return (!f || !pos) ? (__errno_set(EINVAL), -1) : fseek(f, (long)*pos, SEEK_SET); }

/* File IO */
static inline FILE *fdopen(int fd, const char *mode) {
	if (fd < 0 || !mode || (mode[0] != 'r' && mode[0] != 'w' && mode[0] != 'a')) { return (__errno_set(EINVAL), NULL); }

	struct stat st;

	if (fstat(fd, &st) < 0) return NULL;

	return __jacl_stream_alloc(fd, strchr(mode, '+') ? (__SRD|__SWR) : (mode[0] == 'r' ? __SRD : __SWR), _IOFBF);
}
static inline FILE* fopen(const char* restrict path, const char* restrict mode) {
	if (JACL_UNLIKELY(!path || !mode)) { return (__errno_set(EINVAL), NULL); }

	int open_flags = __jacl_buffer_flags(mode);

	if (JACL_UNLIKELY(open_flags < 0)) { return (__errno_set(EINVAL), NULL); }

	int fd = open(path, open_flags, 0666);

	if (JACL_UNLIKELY(fd < 0)) return NULL;
	if (open_flags & O_APPEND) lseek(fd, 0, SEEK_END);

	int stdio_flags = (open_flags & O_RDWR) ? (__SRD|__SWR) : (open_flags & O_WRONLY) ? __SWR : __SRD;
	FILE *f = __jacl_stream_alloc(fd, stdio_flags, _IOFBF);

	return f ? f : (close(fd), NULL);
}
static inline int fclose(FILE* f) {
	if (!f) { return (__errno_set(EBADF), EOF); }

	int ret = 0;

	if (__JACL_HAS_MODE(f, __SMEM)) {

	ret = (__JACL_HAS_MODE(f, __SWR) && __jacl_buffer_flush(f) == EOF) ? EOF : 0;

	if (f->_tmpfname) {
		struct __jacl_memstream_state *state = (struct __jacl_memstream_state *)f->_tmpfname;
		size_t written = f->_ptr - f->_base;

		if (written < f->_bufsiz) f->_base[written] = '\0';

		*state->ptr_loc = f->_base, *state->size_loc = written;
	}
	} else if (__JACL_HAS_MODE(f, __SCUSTOM)) {
		ret = (__JACL_HAS_MODE(f, __SWR) && __jacl_buffer_flush(f) == EOF) ? EOF : 0;

		struct __jacl_custom_io *io = (struct __jacl_custom_io *)f->_tmpfname;

		if (io && io->closefn && io->closefn(io->cookie) != 0) ret = EOF;
	} else {
		if (__JACL_HAS_MODE(f, __SWR)) ret = (__jacl_buffer_flush(f) == EOF || fsync(f->_fd) == -1) ? EOF : 0;
		if (f->_fd >= 0 && close(f->_fd) == -1) ret = EOF;
	}

	__jacl_stream_free(f);

	return ret;
}
static inline int fflush(FILE* f) { return f ? __jacl_stream_flush(f) : (__jacl_stream_flush(stdout) | __jacl_stream_flush(stderr)); }
static inline size_t fread(void* restrict ptr, size_t size, size_t nmemb, FILE* restrict f) { __JACL_RTN_UNLOCK(f, fread_unlocked(ptr, size, nmemb, f)); }
static inline size_t fwrite(const void* restrict ptr, size_t size, size_t nmemb, FILE* restrict f) { __JACL_RTN_UNLOCK(f, fwrite_unlocked(ptr, size, nmemb, f)); }
static inline FILE *freopen(const char* restrict path, const char* restrict mode, FILE* restrict f) {
	if (!f) { return (__errno_set(EBADF), NULL); }
	if (f->_flags & __SWR && __jacl_buffer_flush(f) == EOF) { return (__errno_set(EIO), NULL); }

	if (!path) {
		#if defined(__linux__)
			char procpath[64], realpath_buf[PATH_MAX];

			snprintf(procpath, sizeof(procpath), "/proc/self/fd/%d", f->_fd);

			ssize_t len = readlink(procpath, realpath_buf, sizeof(realpath_buf) - 1);

			if (len > 0) { realpath_buf[len] = '\0'; path = realpath_buf; }
			else { return NULL; }
		#else
			return (__errno_set(EINVAL), NULL);
		#endif
	}

	if (f->_fd >= 0) close(f->_fd);

	int flags = __jacl_buffer_flags(mode);

	if (flags < 0) { return (__errno_set(EINVAL), NULL); }

	int fd = open(path, flags, 0666);

	if (fd < 0) return NULL;

	f->_fd = fd;
	__JACL_BUF_RESET(f);
	f->_flags = (flags & O_RDWR) ? (__SRD|__SWR) : (flags & O_WRONLY ? __SWR : __SRD);
	f->_flags &= ~(__SEOF | __SERR | __SMEM | __SCUSTOM | __SPOPEN);

	return f;
}
static inline int remove(const char *filename) {
	if (!filename) { return (__errno_set(EINVAL), -1); }
	else { int r = unlink(filename); return (r == -1 && __errno_chk(EISDIR)) ? rmdir(filename) : r; }
}
static inline int rename(const char *old, const char *newpath) { return (!old || !newpath) ? (__errno_set(EINVAL), -1) : renameat(AT_FDCWD, old, AT_FDCWD, newpath); }

/* File IO */
#if JACL_HAS_POSIX
static inline FILE *popen(const char *command, const char *mode) {
	if (!command || !mode || (mode[0] != 'r' && mode[0] != 'w')) { return (__errno_set(EINVAL), NULL); }

	int is_read = (mode[0] == 'r'), pipefd[2];

	if (pipe(pipefd) < 0) return NULL;

	pid_t pid = fork();

	if (pid < 0) return close(pipefd[0]), close(pipefd[1]), NULL;

	if (pid == 0) {
		close(pipefd[!is_read]), dup2(pipefd[is_read], is_read ? STDOUT_FILENO : STDIN_FILENO), close(pipefd[is_read]);

		#if JACL_OS_WINDOWS
			execl(JACL_SHELL_PATH, "cmd.exe", "/C", command, (char *)NULL);
		#else
			execl(JACL_SHELL_PATH, "sh", "-c", command, (char *)NULL);
		#endif

		_exit(127);
	}

	int fd = is_read ? pipefd[0] : pipefd[1];

	close(is_read ? pipefd[1] : pipefd[0]);

	FILE *f = fdopen(fd, mode);

	if (!f) return close(fd), kill(pid, SIGKILL), waitpid(pid, NULL, 0), NULL;

	return (f->_tmpfname = (char *)(intptr_t)pid, f->_flags |= __SPOPEN, f);
}
static inline int pclose(FILE *f) {
	if (!f) { return (__errno_set(EBADF), -1); }
	if (!__JACL_HAS_MODE(f, __SPOPEN)) { return (__errno_set(EINVAL), -1); }

	pid_t pid = (pid_t)(intptr_t)f->_tmpfname; f->_tmpfname = NULL;

	__JACL_CLR_MODE(f, __SPOPEN);

	if (__JACL_HAS_MODE(f, __SWR)) __jacl_buffer_output(f);

	close(f->_fd); __jacl_stream_free(f);

	int status;

	while (waitpid(pid, &status, 0) < 0) { if (!__errno_chk(EINTR)) return -1; }

	return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}
typedef int (*readfn_t)(void *cookie, char *buf, int size);
typedef int (*writefn_t)(void *cookie, const char *buf, int size);
typedef fpos_t (*seekfn_t)(void *cookie, fpos_t offset, int whence);
typedef int (*closefn_t)(void *cookie);
static inline FILE *funopen(const void *cookie, readfn_t readfn, writefn_t writefn, seekfn_t seekfn, closefn_t closefn) {
	if (!readfn && !writefn) { return (__errno_set(EINVAL), NULL); }

	struct __jacl_custom_io *io = (struct __jacl_custom_io *)malloc(sizeof(struct __jacl_custom_io));

	if (!io) return (__errno_set(ENOMEM), NULL);
	else *io = (struct __jacl_custom_io){(void *)cookie, readfn, writefn, seekfn, closefn};

	FILE *f = __jacl_stream_alloc(-1, __SCUSTOM | (readfn ? __SRD : 0) | (writefn ? __SWR : 0), _IOFBF);

	return f ? (f->_buf_owned = 1, f->_tmpfname = (char *)io, f) : (free(io), NULL);
}
static inline int fileno(FILE *f) { if (!f) { return (__errno_set(EBADF), -1); } return f->_fd; }
static inline ssize_t getdelim(char **lineptr, size_t *n, int delim, FILE *f) {
	if (!lineptr || !n || !f) { return (__errno_set(EINVAL), -1); }

	size_t pos = 0;
	int c;

	while ((c = fgetc(f)) != EOF) {
		if (pos + 1 >= *n) {
			size_t new_size = *n ? *n * 2 : 128;
			char *new_ptr = (char *)realloc(*lineptr, new_size);

			if (!new_ptr) return -1;

			*lineptr = new_ptr; *n = new_size;
		}

		(*lineptr)[pos++] = (char)c;

		if (c == delim) break;
	}

	if (pos == 0 && c == EOF) return -1;
	if (pos >= *n) {
		char *new_ptr = (char *)realloc(*lineptr, pos + 1);

		if (!new_ptr) return -1;

		*lineptr = new_ptr;
		*n = pos + 1;
	}

	(*lineptr)[pos] = '\0';

	return (ssize_t)pos;
}
static inline ssize_t getline(char **lineptr, size_t *n, FILE *f) { return getdelim(lineptr, n, '\n', f); }
static inline int fseeko(FILE* f, off_t offset, int whence) { return fseek(f, (long)offset, whence); }
static inline off_t ftello(FILE* f) { return (off_t)ftell(f); }
static inline char *ctermid(char *s) {
	static char buf[L_ctermid] = "/dev/tty";
	char *target = s ? s : buf;

	strcpy(target, "/dev/tty");

	return target;
}
static inline FILE *fmemopen(void *restrict buf, size_t size, const char *restrict mode) {
	if (!mode || size == 0) { return (__errno_set(EINVAL), NULL); }

	int flags = __jacl_buffer_flags(mode);

	if (flags < 0) { return (__errno_set(EINVAL), NULL); }

	void *mem = buf;

	if (!mem) {
		mem = malloc(size);

		if (!mem) { return (__errno_set(ENOMEM), NULL); }
		if (flags & O_TRUNC) memset(mem, 0, size);
	}

	FILE *f = __jacl_stream_alloc(-1, __SMEM | ((flags & O_RDWR) ? (__SRD|__SWR) : (flags & O_WRONLY) ? __SWR : __SRD), _IOFBF);

	if (!f) { if (!buf) free(mem); return NULL; }
	else { f->_base = f->_ptr = (char *)mem; f->_end = (char *)mem + size; f->_bufsiz = size; f->_buf_owned = buf ? 0 : 1; }

	if (flags & O_APPEND) {
		char *p = (char *)mem;

		while (p < (char *)mem + size && *p != '\0') p++;

		f->_ptr = p;
	}

	return f;
}
static inline FILE *open_memstream(char **ptr, size_t *sizeloc) {
	if (!ptr || !sizeloc) { return (__errno_set(EINVAL), NULL); }

	size_t initial_size = BUFSIZ;
	char *mem = (char *)malloc(initial_size);

	if (!mem) { return (__errno_set(ENOMEM), NULL); }

	memset(mem, 0, initial_size);

	struct __jacl_memstream_state *state = (struct __jacl_memstream_state *)malloc(sizeof(struct __jacl_memstream_state));

	if (!state) { free(mem); return (__errno_set(ENOMEM), NULL); }

	FILE *f = __jacl_stream_alloc(-1, __SMEM | __SWR, _IOFBF);

	if (!f) { free(state); free(mem); return NULL; }

	*state = (struct __jacl_memstream_state){ptr, sizeloc};
	f->_base = f->_ptr = mem; f->_end = mem + initial_size; f->_bufsiz = initial_size; f->_ovrsiz = 0;
	f->_buf_owned = 0; f->_tmpfname = (char *)state; *ptr = mem; *sizeloc = 0;

	return f;
}
#endif /* JACL_HAS_POSIX */

// Tempfile
static inline FILE *tmpfile(void) {
	char tmpl[] = TEMP_DIR "/tmpXXXXXX";
	int fd = mkstemp(tmpl);

	if (fd < 0) return NULL;

	unlink(tmpl);

	FILE *f = fdopen(fd, "w+");

	return f ? f : (close(fd), NULL);
}
static inline char *tmpnam(char *s) {
	static char buf[L_tmpnam];
	static unsigned long counter = 0;
	char *target = s ? s : buf;

	do { snprintf(target, L_tmpnam, "%stmp_%lu_%ld", TEMP_DIR, counter++, (long)time(NULL)); } while (access(target, F_OK) == 0 && counter < TMP_MAX);

	return (counter < TMP_MAX) ? target : NULL;
}

#ifdef __cplusplus
}
#endif

#include <wchar.h>

#endif /* _STDIO_H */
