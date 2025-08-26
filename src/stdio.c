// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef STDIO_C
#define STDIO_C

#include <stdio.h>	// C17-compliant prototypes
#include <stddef.h>
#include <stdarg.h>
#include <wchar.h>
#include <assert.h>
#include <errno.h>	// For errno handling
#include <limits.h> // For BUFSIZ, etc.
#include <stdint.h> // For uintptr_t, etc.
#include <string.h> // Added for strerror in perror (C17 allows, though not required)
#include <stdio.h>	// Double-include safe

#ifdef __cplusplus
extern "C" {
#endif

// — Macros (C17 7.21.1) —
#ifndef BUFSIZ
#define BUFSIZ 1024
#endif
#ifndef FILENAME_MAX
#define FILENAME_MAX 4096
#endif
#ifndef FOPEN_MAX
#define FOPEN_MAX 16
#endif
#ifndef TMP_MAX
#define TMP_MAX 32767
#endif
#ifndef L_tmpnam
#define L_tmpnam 20
#endif
#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif
#ifndef _IOFBF
#define _IOFBF 0
#define _IOLBF 1
#define _IONBF 2
#endif

// — FILE Type (opaque) —
typedef struct _FILE FILE;
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

// — JS-Implemented I/O Hooks —
FILE* fopen(const char* restrict path, const char* restrict mode) {
		(void)path; (void)mode;
		errno = ENOSYS;
		return NULL;
}
size_t fread(void* restrict ptr, size_t size, size_t nmemb, FILE* restrict stream) {
		(void)ptr; (void)size; (void)nmemb; (void)stream;
		errno = ENOSYS;
		return 0;
}
size_t fwrite(const void* restrict ptr, size_t size, size_t nmemb, FILE* restrict stream) {
		(void)ptr; (void)size; (void)nmemb; (void)stream;
		errno = ENOSYS;
		return 0;
}
int fclose(FILE* stream) {
		(void)stream;
		errno = ENOSYS;
		return EOF;
}
int fflush(FILE* stream) {
		(void)stream;
		return 0;
}

// — Character I/O —
int fputc(int c, FILE* stream) { (void)stream; return (unsigned char)c; }
int getc(FILE* stream) { (void)stream; errno = ENODATA; return EOF; }
int putc(int c, FILE* stream) { return fputc(c, stream); }
int ungetc(int c, FILE* stream) { (void)stream; return (unsigned char)c; }
int getchar(void) { return getc(stdin); }
int putchar(int c) { return putc(c, stdout); }
int puts(const char* s) { while (*s) if (putchar(*s++) == EOF) return EOF; return putchar('\n') == EOF ? EOF : 0; }
int fgetc(FILE* stream) { return getc(stream); }
int fputs(const char* restrict s, FILE* restrict stream) { while (*s) if (putc(*s++, stream) == EOF) return EOF; return 0; }
char* fgets(char* restrict s, int n, FILE* restrict stream) {
		int i = 0, c;
		while (i + 1 < n && (c = getc(stream)) != EOF && c != '\n') s[i++] = (char)c;
		if (c == '\n') s[i++] = '\n';
		s[i] = '\0';
		return (i || c != EOF) ? s : NULL;
}

// — Formatted Output (expanded with basic %f stub) —
static int print_float(char **out, double num, int prec) {
		// Basic integer part only; TODO: full float
		long inum = (long)num; int len = print_num(out ? &out : NULL, inum, 10, 1, 0);
		if (prec > 0) { if (out) *(*out)++ = '.'; len++; /* Stub decimals */ }
		return len;
}

static int vformat(char * restrict out, size_t n, const char * restrict fmt, va_list ap) {
		char *start = out, ch; int len = 0, width = 0, prec = -1;
		while ((ch = *fmt++)) {
				if (ch != '%') { if (out && len < (int)n - 1) *out++ = ch; len++; continue; }
				width = 0; while (*fmt >= '0' && *fmt <= '9') width = width * 10 + (*fmt++ - '0');
				if (*fmt == '.') { fmt++; prec = 0; while (*fmt >= '0' && *fmt <= '9') prec = prec * 10 + (*fmt++ - '0'); }
				switch (*fmt++) {
						case 'c': { int c = va_arg(ap, int); if (out && len < (int)n - 1) *out++ = (char)c; len++; break; }
						case 's': { const char *s = va_arg(ap, const char*); while (*s && len < (int)n - 1) { if (out) *out++ = *s++; len++; } break; }
						case 'd': len += print_num(out ? &out : NULL, va_arg(ap, int), 10, 1, width); break;
						case 'u': len += print_num(out ? &out : NULL, va_arg(ap, unsigned), 10, 0, width); break;
						case 'x': len += print_num(out ? &out : NULL, va_arg(ap, unsigned), 16, 0, width); break;
						case 'p': len += print_num(out ? &out : NULL, (uintptr_t)va_arg(ap, void*), 16, 0, width); break;
						case 'f': len += print_float(out ? &out : NULL, va_arg(ap, double), prec < 0 ? 6 : prec); break;	// Basic %f
						case '%': if (out && len < (int)n - 1) *out++ = '%'; len++; break;
						default: if (out && len < (int)n - 1) *out++ = ch; len++; break;
				}
				prec = -1;
		}
		if (out && n) out[len < (int)n ? len : n-1] = '\0';
		return len;
}

int vprintf(const char * restrict fmt, va_list ap) {
		char buf[BUFSIZ]; int len = vformat(buf, BUFSIZ, fmt, ap);
		for (int i = 0; i < len; i++) if (putchar(buf[i]) == EOF) return -1;
		return len;
}
int vsnprintf(char * restrict s, size_t n, const char * restrict fmt, va_list ap) { return vformat(s, n, fmt, ap); }
int snprintf(char * restrict s, size_t n, const char * restrict fmt, ...) {
		va_list ap; va_start(ap, fmt); int r = vsnprintf(s, n, fmt, ap); va_end(ap); return r;
}
int printf(const char* restrict fmt, ...) {
		va_list ap; va_start(ap, fmt); int r = vprintf(fmt, ap); va_end(ap); return r;
}
int fprintf(FILE* restrict stream, const char* restrict fmt, ...) {
		va_list ap; va_start(ap, fmt); int r = vfprintf(stream, fmt, ap); va_end(ap); return r;
}
int sprintf(char * restrict s, const char * restrict fmt, ...) {
		va_list ap; va_start(ap, fmt); int r = vformat(s, SIZE_MAX, fmt, ap); va_end(ap); return r;
}
int vfprintf(FILE * restrict stream, const char * restrict fmt, va_list ap) {
		char buf[BUFSIZ]; int len = vformat(buf, BUFSIZ, fmt, ap);
		for (int i = 0; i < len; i++) if (putc(buf[i], stream) == EOF) return -1;
		return len;
}

// — Formatted Input (with basic %f stub) —
static int scan_float(const char **in, double *out) {
		// Basic stub: parse integer part only
		uintptr_t num; int digits = scan_num(in, &num, 10, 1);
		if (**in == '.') (*in)++; /* Skip decimal, stub fractions */
		if (digits) *out = (double)num;
		return digits;
}

static int vscan(const char **input, const char * restrict fmt, va_list ap, int is_file) {
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

int scanf(const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vscan(NULL, fmt, ap, 1); va_end(ap); return r; }
int fscanf(FILE* restrict stream, const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vfscanf(stream, fmt, ap); va_end(ap); return r; }
int vscanf(const char * restrict fmt, va_list ap) { return vscan(NULL, fmt, ap, 1); }
int sscanf(const char * restrict s, const char * restrict fmt, ...) { const char *in = s; va_list ap; va_start(ap, fmt); int r = vscan(&in, fmt, ap, 0); va_end(ap); return r; }
int vfscanf(FILE * restrict stream, const char * restrict fmt, va_list ap) { (void)stream; return vscan(NULL, fmt, ap, 1); }
int vsscanf(const char * restrict s, const char* restrict fmt, va_list ap) { const char *in = s; return vscan(&in, fmt, ap, 0); }

// — Buffering —
int setvbuf(FILE* restrict stream, char* restrict buf, int mode, size_t size) {
		(void)stream; (void)buf; (void)size;
		if (mode != _IOFBF && mode != _IOLBF && mode != _IONBF) return EOF;
		return 0;
}
void setbuf(FILE* restrict stream, char* restrict buf) { (void)setvbuf(stream, buf, buf ? _IOFBF : _IONBF, BUFSIZ); }

// — File Positioning (added fgetpos/fsetpos) —
int fseek(FILE* stream, long offset, int whence) {
		(void)stream; (void)offset;
		if (whence != SEEK_SET && whence != SEEK_CUR && whence != SEEK_END) { errno = EINVAL; return -1; }
		errno = ENOSYS; return -1;
}
long ftell(FILE* stream) { (void)stream; errno = ENOSYS; return -1L; }
void rewind(FILE* stream) { (void)fseek(stream, 0L, SEEK_SET); clearerr(stream); }
int fgetpos(FILE* restrict stream, fpos_t* restrict pos) { (void)stream; (void)pos; errno = ENOSYS; return -1; }
int fsetpos(FILE* stream, const fpos_t* pos) { (void)stream; (void)pos; errno = ENOSYS; return -1; }
int feof(FILE* stream) { (void)stream; return 0; }
int ferror(FILE* stream) { (void)stream; return errno != 0; }
void clearerr(FILE* stream) { (void)stream; errno = 0; }

// — Unlocked I/O —
int getchar_unlocked(void) { return getchar(); }
int putchar_unlocked(int c) { return putchar(c); }
int fgetc_unlocked(FILE *stream) { return fgetc(stream); }
int fputc_unlocked(int c, FILE *stream) { return fputc(c, stream); }
char *fgets_unlocked(char *s, int n, FILE *stream) { return fgets(s, n, stream); }
int fputs_unlocked(const char *s, FILE *stream) { return fputs(s, stream); }
size_t fread_unlocked(void *ptr, size_t size, size_t n, FILE *stream) { return fread(ptr, size, n, stream); }
size_t fwrite_unlocked(const void *ptr, size_t size, size_t n, FILE *stream) { return fwrite(ptr, size, n, stream); }

// — Wide-character (added fwide stub) —
wint_t fputwc(wchar_t wc, FILE *stream) { (void)stream; return wc; }
wint_t fgetwc(FILE *stream) { (void)stream; return WEOF; }
wint_t putwc(wchar_t wc, FILE *stream) { return fputwc(wc, stream); }
wint_t getwc(FILE *stream) { return fgetwc(stream); }
int fwide(FILE *stream, int mode) { (void)stream; return mode > 0 ? 1 : (mode < 0 ? -1 : 0); }	// Stub orientation

// — Error Helpers —
void clearerr_unlocked(FILE *stream) { clearerr(stream); }
int feof_unlocked(FILE *stream) { return feof(stream); }
int ferror_unlocked(FILE *stream) { return ferror(stream); }

// — Error Reporting —
void perror(const char* s) { fprintf(stderr, "%s%s%s\n", s ? s : "", s ? ": " : "", strerror(errno)); }

// — Additional Stubs —
int remove(const char *filename) { (void)filename; errno = ENOSYS; return -1; }
int rename(const char *old, const char *new) { (void)old; (void)new; errno = ENOSYS; return -1; }
FILE *tmpfile(void) { errno = ENOSYS; return NULL; }
char *tmpnam(char *s) { if (s) snprintf(s, L_tmpnam, "tmp.%d", 0); return s; }	// Basic stub
char *tempnam(const char *dir, const char *pfx) { (void)dir; (void)pfx; errno = ENOSYS; return NULL; }	// POSIX-ish

#ifdef __cplusplus
}
#endif

#endif /* STDIO_C */
