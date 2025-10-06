/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STDIO_H
#define STDIO_H
#pragma once

#include <config.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>

#define __STDC_VERSION_STDIO_H__ 202311L
#endif

#ifndef BUFSIZ
#define BUFSIZ 1024

#ifndef FILENAME_MAX
#define FILENAME_MAX 4096

#ifndef FOPEN_MAX
#define FOPEN_MAX 16

#ifndef TMP_MAX
#define TMP_MAX 32767

#ifndef L_tmpnam
#define L_tmpnam 20

#ifndef SEEK_SET
#define SEEK_SET 0

#define SEEK_END 2
#endif

#define _IOFBF 0
#define _IOLBF 1

#endif
#ifndef EOF
#define EOF (-1)
#endif

#ifndef TEMP_DIR
	#ifdef JACL_OS_WINDOWS
		#define TEMP_DIR "C:\\Temp\\"
	#else
		#define TEMP_DIR "/tmp/"
	#endif
#endif

#ifdef __cplusplus
	#define restrict __restrict__
#elif !JACL_HAS_C99
	#define restrict	 /* nothing */
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Memory API */
void* malloc(size_t n);
void free(void* ptr);

/* main FILE */
typedef struct __jacl_file {
	int _flags;               // File status flags
	char *_ptr;               // Current position in buffer
	char *_base;              // Base of buffer
	char *_end;               // End of buffer
	size_t _bufsiz;	          // Buffer size
	int _fd;                  // File descriptor
	int _cnt;                 // Characters left in buffer
	int _orientation;         // -1=byte, 0=unset, 1=wide
	unsigned char *_tmpfname; // Temp file name (if any)
} FILE;

typedef long long fpos_t;  // Stub for fpos_t (implementation-defined)

/* Standard Streams */
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;


// Buffer I/O
static inline int __file_mode_flags(const char *mode) {
	int flags = 0;

	if (!mode || !*mode) return -1;
	switch (*mode) {
		case 'r': flags = O_RDONLY; break;
		case 'w': flags = O_WRONLY | O_CREAT | O_TRUNC; break;
		case 'a': flags = O_WRONLY | O_CREAT | O_APPEND; break;
		default: return -1;
	}

	if (strchr(mode, '+')) { // update for read/write mode
		if (flags == O_RDONLY) flags = O_RDWR;
		else if (flags == O_WRONLY) flags = O_RDWR | (flags & ~O_WRONLY);
	}

	return flags;
}

static inline int __file_buffer_output(FILE *f) {
	if (!f || !(f->_flags & __SWR)) return 0;

	size_t to_write = f->_ptr - f->_base;
	ssize_t written = write(f->_fd, f->_base, to_write);

	if (written < 0) {
		f->_flags |= __SERR;

		return EOF;
	}

	if ((size_t)written < to_write) {
		memmove(f->_base, f->_base + written, to_write - written);

		f->_ptr = f->_base + (to_write - written);
	} else {
		f->_ptr = f->_base;
	}

	f->_cnt = f->_bufsiz;

	return 0;
}

static inline int __file_buffer_input(FILE *f) {
	if (!f || !(f->_flags & __SRD)) return EOF;

	ssize_t n = read(f->_fd, f->_base, f->_bufsiz);

	if (n <= 0) {
		f->_flags |= (n == 0) ? __SEOF : __SERR;
		f->_cnt = 0;

		return EOF;
	}

	f->_ptr = f->_base;
	f->_cnt = n - 1;

	return (unsigned char)*f->_ptr++;
}

// File I/O
static inline FILE* fopen(const char* restrict path, const char* restrict mode) {
	// Check arguments and parse mode
	if (!path || !mode) {
		errno = EINVAL;

		return NULL;
	}

	int flags = __file_mode_flags(mode);

	if (flags < 0) {
		errno = EINVAL;

		return NULL;
	}

	// Open the file
	int fd = open(path, flags, 0666);

	if (fd < 0) return NULL;

	// Allocate FILE object
	FILE *f = (FILE *)malloc(sizeof(FILE));

	if (!f) {
		close(fd);
		errno = ENOMEM;

		return NULL;
	}

	// Allocate buffer
	f->_base = malloc(BUFSIZ);

	if (!f->_base) {
		close(fd);
		free(f);

		errno = ENOMEM;

		return NULL;
	}

	f->_ptr = f->_base;
	f->_end = f->_base + BUFSIZ;
	f->_bufsiz = BUFSIZ;
	f->_fd = fd;
	f->_cnt = 0;
	f->_flags = (flags & O_RDWR)
		? (__SRD|__SWR)
		: (flags & O_WRONLY
			? __SWR
			: __SRD);
	f->_orientation = 0;
	f->_tmpfname = NULL;

	return f;
}

static inline int fclose(FILE* stream) {
	if (!stream) {
		errno = EBADF;

		return EOF;
	}

	if (stream->_flags & __SWR) {
		if (__file_buffer_output(stream) == EOF) return EOF;
	}

	if (close(stream->_fd) == -1) return EOF;
	if (stream->_base) free(stream->_base);
	if (stream->_tmpfname) free(stream->_tmpfname);

	/* Do not free stdin/stdout/stderr */
	if (stream != stdin && stream != stdout && stream != stderr) free(stream);

	return 0;
}

static inline size_t fread(void* restrict ptr, size_t size, size_t nmemb, FILE* restrict stream) {
	if (!stream || !ptr || size == 0 || nmemb == 0) return 0;

	size_t total = size * nmemb;
	unsigned char *buf = (unsigned char *)ptr;
	size_t copied = 0;

	while (copied < total) {
		if (stream->_cnt > 0) {
			size_t avail = (size_t)stream->_cnt < total - copied ? (size_t)stream->_cnt : total - copied;

			memcpy(buf + copied, stream->_ptr, avail);

			stream->_ptr += avail;
			stream->_cnt -= avail;

			copied += avail;
		} else {
			int c = __file_buffer_input(stream);

			if (c == EOF) break;

			buf[copied++] = (unsigned char)c;
		}
	}

	return copied / size;
}

static inline size_t fwrite(const void* restrict ptr, size_t size, size_t nmemb, FILE* restrict stream) {
	if (!stream || !ptr || size == 0 || nmemb == 0) return 0;

	size_t total = size * nmemb;

	const unsigned char *buf = (const unsigned char *)ptr;
	size_t written = 0;

	while (written < total) {
		size_t space = stream->_end - stream->_ptr;

		if (space == 0) {
			if (__file_buffer_output(stream) == EOF) break;

			space = stream->_bufsiz;
		}

		size_t to_copy = total - written < space ? total - written : space;

		memcpy(stream->_ptr, buf + written, to_copy);

		stream->_ptr += to_copy;

		written += to_copy;
	}

	return written / size;
}

static inline int fflush(FILE* stream) {
	if (!stream) return 0;
	if (stream->_flags & __SWR) return __file_buffer_output(stream);

	return 0;
}

static inline int remove(const char *filename) {
	if (!filename) {
		errno = EINVAL;

		return -1;
	}

	return unlink(filename);
}

static inline int rename(const char *old, const char *newpath) {
	if (!old || !newpath) {
		errno = EINVAL;

		return -1;
	}

	return renameat(AT_FDCWD, old, AT_FDCWD, newpath);
}

static inline FILE *tmpfile(void) {
	char tmpl[] = TEMP_DIR "/tmpXXXXXX";
	int fd = mkstemp(tmpl);

	if (fd < 0) return NULL;

	unlink(tmpl);

	FILE *f = fdopen(fd, "w+");

	if (!f) close(fd);

	return f;
}

static inline char *tmpnam(char *s) {
	static char buf[L_tmpnam];

	char tmpl[] = TEMP_DIR "/tmpXXXXXX";
	int fd = mkstemp(tmpl);

	if (fd < 0) return NULL;

	close(fd);

	if (s) strcpy(s, tmpl);
	else s = buf;

	strcpy(buf, tmpl);

	return s;
}

static inline char *tempnam(const char *dir, const char *pfx) {
    static char buf[L_tmpnam];

    const char *d = dir ? dir : TEMP_DIR;
    const char *p = pfx ? pfx : "tmp";

    snprintf(buf, sizeof(buf), "%s/%sXXXXXX", d, p);

    int fd = mkstemp(buf);

    if (fd < 0) return NULL;

    close(fd);

    return buf;
}


// Character I/O
static inline int fgetc(FILE* stream) {
	if (!stream || !(stream->_flags & __SRD)) {

		errno = EBADF;

		return EOF;
	}

	if (stream->_cnt <= 0) {
		int c = __file_buffer_input(stream);

		if (c == EOF) return EOF;

		stream->_cnt--;

		return c;
	}

	stream->_cnt--;

	return (unsigned char)*(stream->_ptr)++;
}

static inline int fputc(int c, FILE* stream) {
	if (!stream || !(stream->_flags & __SWR)) {
		errno = EBADF;

		return EOF;
	}

	if (stream->_ptr >= stream->_end) {
		if (__file_buffer_output(stream) == EOF) return EOF;
	}

	*(stream->_ptr)++ = (unsigned char)c;

	return (unsigned char)c;
}

static inline int putc(int c, FILE* stream) { return fputc(c, stream); }
static inline int getc(FILE* stream)	   { return fgetc(stream); }

static inline int ungetc(int c, FILE* stream) {
	if (!stream || stream->_ptr == stream->_base) return EOF;

	*(--stream->_ptr) = (unsigned char)c;

	stream->_cnt++;

	return (unsigned char)c;
}

static inline int getchar(void)	  { return getc(stdin); }
static inline int putchar(int c)	 { return putc(c, stdout); }

static inline int puts(const char* s) {
	if (!s) {
		errno = EINVAL;

		return EOF;
	}

	while (*s) {
		if (putchar(*s++) == EOF) return EOF;
	}

	return putchar('\n') == EOF ? EOF : 0;
}

static inline int fputs(const char* restrict s, FILE* restrict stream) {
	if (!s || !stream) {
		errno = EINVAL;

		return EOF;
	}

	while (*s) {
		if (putc(*s++, stream) == EOF) return EOF;
	}

	return 0;
}

static inline char* fgets(char* restrict s, int n, FILE* restrict stream) {
	if (!s || !stream || n <= 0) {
		errno = EINVAL;

		return NULL;
	}

	int i = 0, c;

	while (i + 1 < n && (c = getc(stream)) != EOF) {
		s[i++] = (char)c;

		if (c == '\n') break;
	}

	if (i == 0 && c == EOF) return NULL;

	s[i] = '\0';

	return s;
}

/* Formatted Output */
static inline int print_num(char * restrict *out, uintptr_t num, int base, int sign, int width) {
		char digits[] = "0123456789abcdef";
		char buffer[32];
		int len = 0, negative = 0;

		if (sign && (intptr_t)num < 0) {
				negative = 1;
				num = (uintptr_t)(-(intptr_t)num);
		}

		if (num == 0) {
				buffer[len++] = '0';
		} else {
				while (num > 0) {
						buffer[len++] = digits[num % base];
						num /= base;
				}
		}

		if (negative) buffer[len++] = '-';
		while (len < width) buffer[len++] = ' ';

		if (out && *out) {
				for (int i = len - 1; i >= 0; i--) {
						*(*out)++ = buffer[i];
				}
		}

		return len;
}
static inline int print_float(char * restrict *out, double num, int prec) {
		// Basic integer part only; TODO: full float
		long inum = (long)num; int len = print_num(out, inum, 10, 1, 0);

		if (prec > 0) { if (out) *(*out)++ = '.'; len++; /* Stub decimals */ }
		return len;
}
static inline void skip_ws(const char **in) {
	if (!in || !*in) return;

	while (**in == ' ' || **in == '\t' || **in == '\n' || **in == '\r' || **in == '\f' || **in == '\v') {
		(*in)++;
	}
}
static inline int vformat(char * restrict out, size_t n, const char * restrict fmt, va_list ap) {
	char *start = out, ch; int len = 0, width = 0, prec = -1;

	while ((ch = *fmt++)) {
		if (ch != '%') {
			if (out && len < (int)n - 1) *out++ = ch; len++;

			continue;
		}

		width = 0;

		while (*fmt >= '0' && *fmt <= '9') {
			width = width * 10 + (*fmt++ - '0');
		}

		if (*fmt == '.') {
			fmt++;
			prec = 0;

			while (*fmt >= '0' && *fmt <= '9') {
				prec = prec * 10 + (*fmt++ - '0');
			}
		}

		switch (*fmt++) {
			case '%': if (out && len < (int)n - 1) *out++ = '%'; len++; break;
			case 'c': { int c = va_arg(ap, int); if (out && len < (int)n - 1) *out++ = (char)c; len++; break; }
			case 's': { const char *s = va_arg(ap, const char*); while (*s && len < (int)n - 1) { if (out) *out++ = *s++; len++; } break; }
			case 'd': len += print_num(out ? &out : NULL, va_arg(ap, int), 10, 1, width); break;
			case 'u': len += print_num(out ? &out : NULL, va_arg(ap, unsigned), 10, 0, width); break;
			case 'x': len += print_num(out ? &out : NULL, va_arg(ap, unsigned), 16, 0, width); break;
			case 'p': { union { void *ptr; uintptr_t u; } pun; pun.ptr = va_arg(ap, void*); len += print_num(out ? &out : NULL, pun.u, 16, 0, width); break; }
			case 'f': len += print_float(out ? &out : NULL, va_arg(ap, double), prec < 0 ? 6 : prec); break;	// Basic %f
			default: if (out && len < (int)n - 1) *out++ = ch; len++; break;
		}

		prec = -1;
	}

	if (out && n) out[len < (int)n ? len : n-1] = '\0';

	return len;
}

static inline int vprintf(const char * restrict fmt, va_list ap) {
	char buf[BUFSIZ]; int len = vformat(buf, BUFSIZ, fmt, ap);

	for (int i = 0; i < len; i++) {
		if (putchar(buf[i]) == EOF) return -1;
	}

	return len;
}
static inline int printf(const char* restrict fmt, ...) {
	va_list ap; va_start(ap, fmt); int r = vprintf(fmt, ap); va_end(ap); return r;
}
static inline int fprintf(FILE* restrict stream, const char* restrict fmt, ...) {
	va_list ap; va_start(ap, fmt); int r = vfprintf(stream, fmt, ap); va_end(ap); return r;
}
static inline int sprintf(char * restrict s, const char * restrict fmt, ...) {
	va_list ap; va_start(ap, fmt); int r = vformat(s, SIZE_MAX, fmt, ap); va_end(ap); return r;
}
static inline int vfprintf(FILE * restrict stream, const char * restrict fmt, va_list ap) {
	char buf[BUFSIZ]; int len = vformat(buf, BUFSIZ, fmt, ap);

	for (int i = 0; i < len; i++) {
		if (putc(buf[i], stream) == EOF) return -1;
	}

	return len;
}

/* Formatted Input */
static inline int scan_num(const char **in, uintptr_t *out, int base, int sign) {
		const char *start = *in;
		uintptr_t result = 0;
		int negative = 0, digits = 0;

		while (**in == ' ' || **in == '\t' || **in == '\n' || **in == '\r' || **in == '\f' || **in == '\v') {
				(*in)++;
		}

		if (sign && **in == '-') {
				negative = 1;
				(*in)++;
		} else if (**in == '+') {
				(*in)++;
		}

		while (**in) {
				int digit_val = -1;
				if (**in >= '0' && **in <= '9') {
						digit_val = **in - '0';
				} else if (base == 16 && **in >= 'a' && **in <= 'f') {
						digit_val = **in - 'a' + 10;
				} else if (base == 16 && **in >= 'A' && **in <= 'F') {
						digit_val = **in - 'A' + 10;
				}

				if (digit_val >= 0 && digit_val < base) {
						result = result * base + digit_val;
						digits++;
						(*in)++;
				} else {
						break;
				}
		}

		if (digits > 0) {
				if (negative) result = (uintptr_t)(-(intptr_t)result);
				*out = result;
				return digits;
		}

		*in = start;
		return 0;
}
static inline int scan_float(const char **in, double *out) {
		// Basic stub: parse integer part only
		uintptr_t num; int digits = scan_num(in, &num, 10, 1);
		if (**in == '.') (*in)++; /* Skip decimal, stub fractions */
		if (digits) *out = (double)num;
		return digits;
}
static inline int vscan(const char **input, const char * restrict fmt, va_list ap, int is_file) {
		const char *in = input ? *input : NULL; int count = 0, suppress = 0;
		while (*fmt) {
				if (*fmt != '%') { skip_ws(&in); if (in && *in++ != *fmt) return count; if (!in) { int c = getchar(); if (c != *fmt) return count; } fmt++; continue; }
				fmt++; if (*fmt == '*') { suppress = 1; fmt++; }
				int width = INT_MAX; if (*fmt >= '0' && *fmt <= '9') width = 0; while (*fmt >= '0' && *fmt <= '9') width = width * 10 + (*fmt++ - '0');
				char type = *fmt++;
				switch (type) {
						case 'c': { char *p = suppress ? NULL : va_arg(ap, char*); int w = width ? width : 1; while (w--) { int ch = in ? *in++ : getchar(); if (ch == EOF) break; if (p) *p++ = (char)ch; } count++; break; }
						case 's': { char *p = suppress ? NULL : va_arg(ap, char*); skip_ws(&in); int i = 0; while (i < width) { int ch = in ? *in : getchar(); if (ch == EOF || ch == ' ' || ch == '\t' || ch == '\n' || ch == '\v' || ch == '\f' || ch == '\r') break; if (in) in++; if (p) p[i++] = (char)ch; } if (p) p[i] = '\0'; count++; break; }
						case 'd': { int *p = suppress ? NULL : va_arg(ap, int*); uintptr_t val; if (scan_num(&in, &val, 10, 1) && p) *p = (int)val; count++; break; }
						case 'u': { unsigned *p = suppress ? NULL : va_arg(ap, unsigned*); uintptr_t val; if (scan_num(&in, &val, 10, 0) && p) *p = (unsigned)val; count++; break; }
						case 'x': { unsigned *p = suppress ? NULL : va_arg(ap, unsigned*); uintptr_t val; if (scan_num(&in, &val, 16, 0) && p) *p = (unsigned)val; count++; break; }
						case 'f': { float *p = suppress ? NULL : va_arg(ap, float*); double val; if (scan_float(&in, &val) && p) *p = (float)val; count++; break; }  // Basic %f
						case '%': skip_ws(&in); if ((in ? *in++ : getchar()) == '%') count++; break;
						default: break;
				}
				suppress = 0;
		}
		if (input) *input = in;
		return count;
}

static inline int scanf(const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vscan(NULL, fmt, ap, 1); va_end(ap); return r; }
static inline int fscanf(FILE* restrict stream, const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vfscanf(stream, fmt, ap); va_end(ap); return r; }
static inline int vscanf(const char * restrict fmt, va_list ap) { return vscan(NULL, fmt, ap, 1); }
static inline int sscanf(const char * restrict s, const char * restrict fmt, ...) { const char *in = s; va_list ap; va_start(ap, fmt); int r = vscan(&in, fmt, ap, 0); va_end(ap); return r; }
static inline int vfscanf(FILE * restrict stream, const char * restrict fmt, va_list ap) { (void)stream; return vscan(NULL, fmt, ap, 1); }
static inline int vsscanf(const char * restrict s, const char* restrict fmt, va_list ap) { const char *in = s; return vscan(&in, fmt, ap, 0); }

#if JACL_HAS_C99
static inline int snprintf(char * restrict s, size_t n, const char * restrict fmt, ...) {
	va_list ap;

	va_start(ap, fmt);

	int r = vsnprintf(s, n, fmt, ap);

	va_end(ap);

	return r;
}
static inline int vsnprintf(char * restrict s, size_t n, const char * restrict fmt, va_list ap) {
	return vformat(s, n, fmt, ap);
}
#endif

static inline /* Error Handling */
static inline int feof(FILE* stream) { (void)stream; return 0; }
static inline int ferror(FILE* stream) { (void)stream; return errno != 0; }
static inline void clearerr(FILE* stream) { (void)stream; errno = 0; }
static inline void perror(const char* s) { fprintf(stderr, "%s%s%s\n", s ? s : "", s ? ": " : "", strerror(errno)); }

/* File Positioning */
static inline int fseek(FILE* stream, long offset, int whence) {
	(void)stream; (void)offset;

	if (whence != SEEK_SET && whence != SEEK_CUR && whence != SEEK_END) {
		errno = EINVAL;

		return -1;
	}

	errno = ENOSYS;

	return -1;
}
static inline long ftell(FILE* stream) { (void)stream; errno = ENOSYS; return -1L; }
static inline void rewind(FILE* stream) { (void)fseek(stream, 0L, SEEK_SET); clearerr(stream); }
static inline int fgetpos(FILE* restrict stream, fpos_t* restrict pos) { (void)stream; (void)pos; errno = ENOSYS; return -1; }
static inline int fsetpos(FILE* stream, const fpos_t* pos) { (void)stream; (void)pos; errno = ENOSYS; return -1; }

/* Buffering */
static inline int setvbuf(FILE* restrict stream, char* restrict buf, int mode, size_t size) {
	(void)stream; (void)buf; (void)size;

	if (mode != _IOFBF && mode != _IOLBF && mode != _IONBF) return EOF;

	return 0;
}
static inline void setbuf(FILE* restrict stream, char* restrict buf) { (void)setvbuf(stream, buf, buf ? _IOFBF : _IONBF, BUFSIZ); }

// Unlocked I/O
static inline int getchar_unlocked(void) { return getchar(); }
static inline int putchar_unlocked(int c) { return putchar(c); }
static inline int fgetc_unlocked(FILE *stream) { return fgetc(stream); }
static inline int fputc_unlocked(int c, FILE *stream) { return fputc(c, stream); }
static inline char *fgets_unlocked(char *s, int n, FILE *stream) { return fgets(s, n, stream); }
static inline int fputs_unlocked(const char *s, FILE *stream) { return fputs(s, stream); }
static inline size_t fread_unlocked(void *ptr, size_t size, size_t n, FILE *stream) { return fread(ptr, size, n, stream); }
static inline size_t fwrite_unlocked(const void *ptr, size_t size, size_t n, FILE *stream) { return fwrite(ptr, size, n, stream); }
static inline void clearerr_unlocked(FILE *stream) { clearerr(stream); }
static inline int feof_unlocked(FILE *stream) { return feof(stream); }
static inline int ferror_unlocked(FILE *stream) { return ferror(stream); }

#if JACL_OS_BSD || JACL_OS_DARWIN
static inline int setlinebuf(FILE *stream) { return setvbuf(stream, NULL, _IOLBF, BUFSIZ); }
#endif

#if JACL_HAS_POSIX

static inline int fileno(FILE *stream) {
	(void)stream;

	errno = EBADF;

	return -1;
}

static inline FILE *popen(const char *command, const char *mode) {
	(void)command; (void)mode;

	errno = ENOSYS;

	return NULL;
}

static inline int pclose(FILE *stream) {
	(void)stream;

	errno = ENOSYS;

	return -1;
}

typedef int (*fun_readfn_t)(void *, char *, int);
typedef int (*fun_writefn_t)(void *, const char *, int);
typedef long (*fun_seekfn_f)(void *, long, int);
typedef int (*fun_closefn_t)(void *);

static inline FILE *funopen(const void *cookie, readfn_t readfn, writefn_t writefn, seekfn_t seekfn, closefn_t closefn){
  (void)cookie; (void)readfn; (void)writefn; (void)seekfn; (void)closefn;

  errno = ENOSYS;

  return NULL;
}

#endif /* JACL_HAS_POSIX */


#if JACL_HAS_THREADS

static inline void flockfile(FILE *stream) {
  (void)stream;
  /* No-op stub */
}

static inline int ftrylockfile(FILE *stream) {
  (void)stream;
  return 0;  /* Pretend lock always succeeds */
}

static inline void funlockfile(FILE *stream) {
  (void)stream;
  /* No-op stub */
}

#endif /* JACL_HAS_THREADS */


#ifdef __cplusplus
}
#endif

#endif /* STDIO_H */
