// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef STDIO_C
#define STDIO_C

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <wchar.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

// — FILE Type Stub —
struct _FILE { int dummy; };

FILE* stdin = NULL;
FILE* stdout = NULL;
FILE* stderr = NULL;

// — JS-Implemented I/O Hooks (stubbed until async IndexedDB) —
FILE* fopen(const char* path, const char* mode) {
		(void)path; (void)mode;
		return NULL;	/* TODO */
}
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream) {
		(void)ptr; (void)size; (void)nmemb; (void)stream;
		return 0;			/* TODO */
}
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream) {
		(void)ptr; (void)size; (void)nmemb; (void)stream;
		return 0;			/* TODO */
}
int fclose(FILE* stream) {
		(void)stream;
		return 0;			/* TODO */
}
int fflush(FILE* stream) {
		(void)stream;
		return 0;			/* TODO */
}

// — Character I/O (stub) —
int fputc(int c, FILE* s) { (void)s; return c; }
int getc(FILE* s)					{ (void)s; return EOF; }
int putc(int c, FILE* s)	{ (void)s; return c; }
int ungetc(int c, FILE* s) { (void)s; return c; }
int getchar(void)					{ return EOF; }
int putchar(int c)				{ return c; }
int puts(const char* s)		{ (void)s; return EOF; }
int fgetc(FILE *s) { return getc(s); }
int fputs(const char *s, FILE *stream) { (void)stream; while (*s) putchar(*s++); return 0; }
char* fgets(char *s, int n, FILE *stream) {
	(void)stream;
	int i = 0, c;

	while (i + 1 < n && (c = getchar()) != EOF && c != '\n') { s[i++] = c; }

	if (c == '\n') s[i++] = '\n';

	s[i] = '\0';

	return i ? s : NULL;
}

// — Formatted I/O (console only) —
int vprintf(const char *fmt, va_list ap) { return __builtin_vprintf(fmt, ap); }
int vsnprintf(char *s, size_t n, const char *fmt, va_list ap) {
	int r = __builtin_vsnprintf(s, n, fmt, ap);

	if (r < 0 || (size_t)r >= n) s[n - 1] = '\0';

	return r;
}
int snprintf(char *s, size_t n, const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);

	int r = vsnprintf(s, n, fmt, ap);

	va_end(ap);

	return r;
}
int printf(const char* fmt, ...) {
		va_list ap; va_start(ap, fmt);
		int r = vprintf(fmt, ap);
		va_end(ap);
		return r;
}
int fprintf(FILE* s, const char* fmt, ...) {
		(void)s;
		va_list ap; va_start(ap, fmt);
		int r = vprintf(fmt, ap);
		va_end(ap);
		return r;
}
int sprintf(char *s, const char *fmt, ...) {
	va_list ap; va_start(ap, fmt);
	// Use vsnprintf with a large buffer assumption; ensure null-termination
	int r = __builtin_vsnprintf(s, (size_t)-1, fmt, ap);
	va_end(ap);
	return r;
}

// — Buffering (no-op) —
int setvbuf(FILE* s, char* b, int m, size_t sz) {
		(void)s; (void)b; (void)m; (void)sz;
		return 0;
}
void setbuf(FILE* s, char* buf) {
		(void)s; (void)buf;
}

// — File Positioning & Status (stub) —
int fseek(FILE* s, long off, int whence) {
		(void)s; (void)off; (void)whence;
		return -1;
}
long ftell(FILE* s) {
		(void)s;
		return -1L;
}
void rewind(FILE* s) {
		(void)s;
}
int feof(FILE* s) {
		(void)s;
		return 0;
}
int ferror(FILE* s) {
		(void)s;
		return 0;
}
void clearerr(FILE* s) {
		(void)s;
}

// — Unlocked Character I/O (POSIX) —
int getchar_unlocked(void) { return getchar(); }
int putchar_unlocked(int c) { return putchar(c); }
int fgetc_unlocked(FILE *s) { return fgetc(s); }
int fputc_unlocked(int c, FILE *s) { return fputc(c, s); }
char *fgets_unlocked(char *s, int n, FILE *stream) { return fgets(s, n, stream); }
int fputs_unlocked(const char *s, FILE *stream) { return fputs(s, stream); }

// — Wide-character support (stub) —
int fputwc(wchar_t wc, FILE *s) { (void)s; return (int)wc; }
int fgetwc(FILE *s) { (void)s; return WEOF; }
wint_t putwc(wchar_t wc, FILE *s) { return fputwc(wc, s); }
wint_t getwc(FILE *s) { return fgetwc(s); }

// — Error state helpers (stub) —
void clearerr_unlocked(FILE *s) { clearerr(s); }
int feof_unlocked(FILE *s) { return feof(s); }
int ferror_unlocked(FILE *s) { return ferror(s); }

// — Misc Error Reporting (stub) —
void perror(const char* s) {
		(void)s;
}

/* Scans */
int scanf(const char * restrict fmt, ...) {
		va_list ap; va_start(ap, fmt);
		int r = __builtin_vscanf(fmt, ap);

		va_end(ap);

		return r;
}
int fscanf(FILE* restrict f, const char * restrict fmt, ...) {
		va_list ap; va_start(ap, fmt);
		int r = __builtin_vfscanf(f, fmt, ap);

		va_end(ap);

		return r;
}
int vscanf(const char * restrict fmt, va_list ap) {
		return __builtin_vscanf(fmt, ap);
}
/*— at the top, after va_list declarations —*/
int sscanf(const char * restrict s, const char * restrict fmt, ...)
{
		va_list ap;
		va_start(ap, fmt);
		int r = __builtin_vsscanf(s, fmt, ap);
		va_end(ap);
		return r;
}

int vfscanf(FILE * restrict f, const char * restrict fmt, va_list ap)
{
		return __builtin_vfscanf(f, fmt, ap);
}
int vsscanf(const char * restrict s, const char* restrict fmt, va_list ap) {
		return __builtin_vsscanf(s, fmt, ap);
}

#ifdef __cplusplus
}
#endif

#endif /* STDIO_C */

