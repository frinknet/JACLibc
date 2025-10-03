/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef STDIO_H
#define STDIO_H
#pragma once

#include <config.h>
#include <stddef.h>  // size_t, ptrdiff_t, NULL
#include <stdarg.h>  // va_list, va_start(), va_end(), va_arg()
#include <sys/types.h>  // FILE
#include <wchar.h>   // wchar_t, wint_t, WEOF
#include <assert.h>  // assert()
#include <errno.h>   // errno, EINVAL, EBADF, ERANGE, etc.
#include <limits.h>  // INT_MAX, LONG_MAX, CHAR_BIT
#include <stdint.h>  // uint8_t, int32_t, uint64_t, etc.

#if JACL_HAS_C23
#define __STDC_VERSION_STDIO_H__ 202311L
#endif

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
#ifndef EOF
#define EOF (-1)
#endif
#ifndef WEOF
#define WEOF ((wint_t)-1)
#endif

#ifdef __cplusplus
	#define restrict __restrict__
#elif !JACL_HAS_C99
	#define restrict	 /* nothing */
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef long long fpos_t;  // Stub for fpos_t (implementation-defined)

/* Standard Streams */
extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

/* File Operations */
FILE* fopen(const char* restrict path, const char* restrict mode);
int fclose(FILE* stream);
int fflush(FILE* stream);
int remove(const char *filename);
int rename(const char *old, const char *new);
FILE *tmpfile(void);
char *tmpnam(char *s);
char *tempnam(const char *dir, const char *pfx);

/* File Access */
size_t fread(void* restrict ptr, size_t size, size_t nmemb, FILE* restrict stream);
size_t fwrite(const void* restrict ptr, size_t size, size_t nmemb, FILE* restrict stream);

/* Direct I/O */
int fgetc(FILE* stream);
int fputc(int c, FILE* stream);
char* fgets(char* restrict s, int n, FILE* restrict stream);
int fputs(const char* restrict s, FILE* restrict stream);
int getc(FILE* stream);
int getchar(void);
int putc(int c, FILE* stream);
int putchar(int c);
int puts(const char* s);
int ungetc(int c, FILE* stream);

/* Formatted I/O */
int fprintf(FILE* restrict stream, const char* restrict fmt, ...);
int fscanf(FILE* restrict stream, const char* restrict fmt, ...);
int printf(const char* restrict fmt, ...);
int fprintf(FILE* restrict stream, const char* restrict fmt, ...);
int scanf(const char* restrict fmt, ...);
int sprintf(char* restrict s, const char* restrict fmt, ...);
int sscanf(const char* restrict s, const char* restrict fmt, ...);
int vfprintf(FILE* restrict stream, const char* restrict fmt, va_list ap);
int vfscanf(FILE* restrict stream, const char* restrict fmt, va_list ap);
int vprintf(const char* restrict fmt, va_list ap);
int vscanf(const char* restrict fmt, va_list ap);
int vsprintf(char* restrict s, const char* restrict fmt, va_list ap);
int vsscanf(const char* restrict s, const char* restrict fmt, va_list ap);

#if JACL_HAS_C99
int snprintf(char* restrict s, size_t n, const char* restrict fmt, ...);
int vsnprintf(char* restrict s, size_t n, const char* restrict fmt, va_list ap);
#endif

/* Error Handling */
void clearerr(FILE* stream);
int feof(FILE* stream);
int ferror(FILE* stream);
void perror(const char* s);

/* File Positioning */
int fgetpos(FILE* restrict stream, fpos_t* restrict pos);
int fseek(FILE* stream, long offset, int whence);
int fsetpos(FILE* stream, const fpos_t* pos);
long ftell(FILE* stream);
void rewind(FILE* stream);

/* Operations on Files (buffering) */
void setbuf(FILE* restrict stream, char* restrict buf);
int setvbuf(FILE* restrict stream, char* restrict buf, int mode, size_t size);

/* Unlocked I/O (POSIX extensions, C17 compatible) */
int fgetc_unlocked(FILE *stream);
int fputc_unlocked(int c, FILE *stream);
char *fgets_unlocked(char *s, int n, FILE *stream);
int fputs_unlocked(const char *s, FILE *stream);
int getchar_unlocked(void);
int putchar_unlocked(int c);
size_t fread_unlocked(void *ptr, size_t size, size_t n, FILE *stream);
size_t fwrite_unlocked(const void *ptr, size_t size, size_t n, FILE *stream);
void clearerr_unlocked(FILE *stream);
int feof_unlocked(FILE *stream);
int ferror_unlocked(FILE *stream);

#if JACL_HAS_C99

/* Get / Put Helpers */
wint_t fgetwc(FILE *stream);
wint_t fputwc(wchar_t wc, FILE *stream);
wint_t getwc(FILE *stream);
wint_t putwc(wchar_t wc, FILE *stream);

/* Wide Char Support */
wchar_t *fgetws(wchar_t * restrict ws, int n, FILE * restrict stream);
int fputws(const wchar_t * restrict ws, FILE * restrict stream);
wint_t getwchar(void);
wint_t putwchar(wchar_t wc);

#endif /* JACL_HAS_C99 */

#if JACL_HAS_POSIX

int fileno(FILE *stream);
FILE *popen(const char *command, const char *mode);
int pclose(FILE *stream);
FILE *funopen(const void *, int (*)(void *, char *, int), int (*)(void *, const char *, int), long (*)(void *, long, int), int (*)(void *));

#endif /* JACL_HAS_POSIX */

#if JACL_HAS_THREADS

void flockfile(FILE *stream);
int ftrylockfile(FILE *stream);
void funlockfile(FILE *stream);

#endif /* JACL_HAS_THREADS */


#ifdef __cplusplus
}
#endif

#endif /* STDIO_H */
