// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef STDIO_H
#define STDIO_H

#ifndef EOF
#define EOF (-1)
#endif

#if defined(__cplusplus)
	#define restrict __restrict__
#elif !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
	#define restrict	 /* nothing */
#endif

#include <stddef.h>
#include <stdarg.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

// — FILE Type Stub —
typedef int FILE;

// — Standard Streams —
extern FILE* stdin;		/* TODO: hook to async storage */
extern FILE* stdout;
extern FILE* stderr;

// — JS-Implemented I/O Hooks (stubbed until async IndexedDB) —
static inline FILE* fopen(const char* path, const char* mode) {
		(void)path; (void)mode;
		return NULL;	/* TODO */
}
static inline size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream) {
		(void)ptr; (void)size; (void)nmemb; (void)stream;
		return 0;			/* TODO */
}
static inline size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream) {
		(void)ptr; (void)size; (void)nmemb; (void)stream;
		return 0;			/* TODO */
}
static inline int fclose(FILE* stream) {
		(void)stream;
		return 0;			/* TODO */
}
static inline int fflush(FILE* stream) {
		(void)stream;
		return 0;			/* TODO */
}

// — Character I/O (stub) —
static inline int fputc(int c, FILE* s) { (void)s; return c; }
static inline int getc(FILE* s)					{ (void)s; return EOF; }
static inline int putc(int c, FILE* s)	{ (void)s; return c; }
static inline int ungetc(int c, FILE* s) { (void)s; return c; }
static inline int getchar(void)					{ return EOF; }
static inline int putchar(int c)				{ return c; }
static inline int puts(const char* s)		{ (void)s; return EOF; }
static inline int fgetc(FILE *s) { return getc(s); }
static inline int fputs(const char *s, FILE *stream) { (void)stream; while (*s) putchar(*s++); return 0; }
static inline char* fgets(char *s, int n, FILE *stream) {
	(void)stream;
	int i = 0, c;

	while (i + 1 < n && (c = getchar()) != EOF && c != '\n') { s[i++] = c; }

	if (c == '\n') s[i++] = '\n';

	s[i] = '\0';

	return i ? s : NULL;
}

// — Formatted I/O (console only) —
static inline int vprintf(const char *fmt, va_list ap) { return __builtin_vprintf(fmt, ap); }
static inline int vsnprintf(char *s, size_t n, const char *fmt, va_list ap) {
	int r = __builtin_vsnprintf(s, n, fmt, ap);

	if (r < 0 || (size_t)r >= n) s[n - 1] = '\0';

	return r;
}
static inline int snprintf(char *s, size_t n, const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);

	int r = vsnprintf(s, n, fmt, ap);

	va_end(ap);

	return r;
}
static inline int printf(const char* fmt, ...) {
		va_list ap; va_start(ap, fmt);
		int r = vprintf(fmt, ap);
		va_end(ap);
		return r;
}
static inline int fprintf(FILE* s, const char* fmt, ...) {
		(void)s;
		va_list ap; va_start(ap, fmt);
		int r = vprintf(fmt, ap);
		va_end(ap);
		return r;
}

// — Buffering (no-op) —
static inline int setvbuf(FILE* s, char* b, int m, size_t sz) {
		(void)s; (void)b; (void)m; (void)sz;
		return 0;
}
static inline void setbuf(FILE* s, char* buf) {
		(void)s; (void)buf;
}

// — File Positioning & Status (stub) —
static inline int fseek(FILE* s, long off, int whence) {
		(void)s; (void)off; (void)whence;
		return -1;
}
static inline long ftell(FILE* s) {
		(void)s;
		return -1L;
}
static inline void rewind(FILE* s) {
		(void)s;
}
static inline int feof(FILE* s) {
		(void)s;
		return 0;
}
static inline int ferror(FILE* s) {
		(void)s;
		return 0;
}
static inline void clearerr(FILE* s) {
		(void)s;
}

// — Unlocked Character I/O (POSIX) —
static inline int getchar_unlocked(void) { return getchar(); }
static inline int putchar_unlocked(int c) { return putchar(c); }
static inline int fgetc_unlocked(FILE *s) { return fgetc(s); }
static inline int fputc_unlocked(int c, FILE *s) { return fputc(c, s); }
static inline char *fgets_unlocked(char *s, int n, FILE *stream) { return fgets(s, n, stream); }
static inline int fputs_unlocked(const char *s, FILE *stream) { return fputs(s, stream); }

// — Wide-character support (stub) —
static inline int fputwc(wchar_t wc, FILE *s) { (void)s; return (int)wc; }
static inline int fgetwc(FILE *s) { (void)s; return WEOF; }
static inline wint_t putwc(wchar_t wc, FILE *s) { return fputwc(wc, s); }
static inline wint_t getwc(FILE *s) { return fgetwc(s); }

// — Error state helpers (stub) —
static inline void clearerr_unlocked(FILE *s) { clearerr(s); }
static inline int feof_unlocked(FILE *s) { return feof(s); }
static inline int ferror_unlocked(FILE *s) { return ferror(s); }

// — Misc Error Reporting (stub) —
static inline void perror(const char* s) {
		(void)s;
}

/* Scans */
static inline int scanf(const char * restrict fmt, ...) {
		va_list ap; va_start(ap, fmt);
		int r = __builtin_vscanf(fmt, ap);

		va_end(ap);

		return r;
}
static inline int fscanf(FILE* restrict f, const char * restrict fmt, ...) {
		va_list ap; va_start(ap, fmt);
		int r = __builtin_vfscanf(f, fmt, ap);

		va_end(ap);

		return r;
}
static inline int vscanf(const char * restrict fmt, va_list ap) {
		return __builtin_vscanf(fmt, ap);
}
/*— at the top, after va_list declarations —*/
static inline int sscanf(const char * restrict s, const char * restrict fmt, ...)
{
		va_list ap;
		va_start(ap, fmt);
		int r = __builtin_vsscanf(s, fmt, ap);
		va_end(ap);
		return r;
}

static inline int vfscanf(FILE * restrict f, const char * restrict fmt, va_list ap)
{
		return __builtin_vfscanf(f, fmt, ap);
}
static inline int vsscanf(const char * restrict s, const char* restrict fmt, va_list ap) {
		return __builtin_vsscanf(s, fmt, ap);
}

#ifdef __cplusplus
}
#endif

#endif /* STDIO_H */
