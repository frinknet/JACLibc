// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>
#include <stdarg.h>
#include <wchar.h>
#include <assert.h>

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define EOF (-1)

#ifdef __cplusplus
	#define restrict __restrict__
#elif !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
	#define restrict	 /* nothing */
#endif

#ifdef __cplusplus
extern "C" {
#endif

// — FILE Type Stub —
typedef struct _FILE FILE;


// — Standard Streams —
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

// — JS-Implemented I/O Hooks (stubbed until async IndexedDB) —
FILE* fopen(const char* path, const char* mode);
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);
int fclose(FILE* stream);
int fflush(FILE* stream);

// — Character I/O (stub) —
int fputc(int c, FILE* s);
int getc(FILE* s);
int putc(int c, FILE* s);
int ungetc(int c, FILE* s);
int getchar(void);
int putchar(int c);
int puts(const char* s);
int fgetc(FILE *s);
int fputs(const char *s, FILE *stream);
char* fgets(char *s, int n, FILE *stream);

// — Formatted I/O (console only) —
int vprintf(const char *fmt, va_list ap);
int vsnprintf(char *s, size_t n, const char *fmt, va_list ap);
int snprintf(char *s, size_t n, const char *fmt, ...);
int printf(const char* fmt, ...);
int fprintf(FILE* s, const char* fmt, ...);
int sprintf(char *s, const char *fmt, ...);

// — Buffering (no-op) —
int setvbuf(FILE* s, char* b, int m, size_t sz);
void setbuf(FILE* s, char* buf);

// — File Positioning & Status (stub) —
int fseek(FILE* s, long off, int whence);
long ftell(FILE* s);
int feof(FILE* s);
int ferror(FILE* s);
void clearerr(FILE* s);

// — Unlocked Character I/O (POSIX) —
int getchar_unlocked(void);
int putchar_unlocked(int c);
int fgetc_unlocked(FILE *s);
int fputc_unlocked(int c, FILE *s);
char *fgets_unlocked(char *s, int n, FILE *stream);
int fputs_unlocked(const char *s, FILE *stream);

// — Wide-character support (stub) —
int fputwc(wchar_t wc, FILE *s);
int fgetwc(FILE *s);
wint_t putwc(wchar_t wc, FILE *s);
wint_t getwc(FILE *s);

// — Error state helpers (stub) —
void clearerr_unlocked(FILE *s);
int feof_unlocked(FILE *s);
int ferror_unlocked(FILE *s);

// — Misc Error Reporting (stub) —
void perror(const char* s);

/* Scans */
int scanf(const char * restrict fmt, ...);
int fscanf(FILE* restrict f, const char * restrict fmt, ...);
int vscanf(const char * restrict fmt, va_list ap);
int sscanf(const char * restrict s, const char * restrict fmt, ...);
int vfscanf(FILE * restrict f, const char * restrict fmt, va_list ap);
int vsscanf(const char * restrict s, const char* restrict fmt, va_list ap);

#ifdef __cplusplus
}
#endif

#endif /* STDIO_H */
