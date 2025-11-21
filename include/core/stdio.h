/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef CORE_STDIO_H
#define CORE_STDIO_H

#include <stdio.h>
#include <core/format.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================= */
/* Internal Variable                                             */
/* ============================================================= */
STATIC_FILE(stdin,  0, __SRD, _IOLBF);
STATIC_FILE(stdout, 1, __SWR, _IOLBF);
STATIC_FILE(stderr, 2, __SWR, _IONBF);

typedef struct __jacl_stream_node {
	FILE* stream;
	struct __jacl_stream_node *next;
} STREAM_NODE;

static STREAM_NODE* __jacl_stream_list;
int __jacl_stdio_init = 0;

/* ============================================================= */
/* Stream Registry                                               */
/* ============================================================= */
void __jacl_stream_register(FILE *stream) {
	STREAM_NODE *node = (STREAM_NODE *)malloc(sizeof(STREAM_NODE));

	if (!node) return;

	node->stream = stream;
	node->next = __jacl_stream_list;

	__jacl_stream_list = node;
}
void __jacl_stream_unregister(FILE *stream) {
	STREAM_NODE** curr = &__jacl_stream_list;

	while (*curr) {
		if ((*curr)->stream == stream) {
			STREAM_NODE* to_free = *curr;
			*curr = (*curr)->next;

			free(to_free);

			return;
		}

		curr = &(*curr)->next;
	}
}
int __jacl_stream_flush(FILE* stream) {
	__jacl_init_stdio();

	if (stream) return (stream->_flags & __SWR) ? __jacl_buffer_flush(stream) : 0;

	int result = 0;

	if (__jacl_buffer_flush(stdout) == EOF) result = EOF;
	if (__jacl_buffer_flush(stderr) == EOF) result = EOF;

	for (STREAM_NODE* n = __jacl_stream_list; n; n = n->next) {
		FILE* s = n->stream;

		if (s && (s->_flags & __SWR) && s != stdout && s != stderr && __jacl_buffer_flush(s) == EOF) result = EOF;
	}

	return result;
}

/* ============================================================= */
/* Printf Implementations                                        */
/* ============================================================= */
int vprintf(const char * restrict fmt, va_list ap) { return __jacl_printf(stdout, NULL, 0, fmt, ap); }
int vfprintf(FILE * restrict stream, const char * restrict fmt, va_list ap) { return __jacl_printf(stream, NULL, 0, fmt, ap); }
int vsprintf(char * restrict s, const char * restrict fmt, va_list ap) { return __jacl_printf(NULL, s, SIZE_MAX, fmt, ap); }

int printf(const char* restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vprintf(fmt, ap); va_end(ap); return r; }
int fprintf(FILE* restrict stream, const char* restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vfprintf(stream, fmt, ap); va_end(ap); return r; }
int sprintf(char * restrict s, const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vsprintf(s, fmt, ap); va_end(ap); return r; }

#if JACL_HAS_C99
int vsnprintf(char * restrict s, size_t n, const char * restrict fmt, va_list ap) { return __jacl_printf(NULL, s, n, fmt, ap); }
int vdprintf(int fd, const char* restrict fmt, va_list ap) {
	if (fd < 0) { errno = EBADF; return -1; }

	__jacl_init_stdio();

	for (STREAM_NODE* n = __jacl_stream_list; n; n = n->next) {
		if (n->stream && n->stream->_fd == fd) return __jacl_printf(n->stream, NULL, 0, fmt, ap);
	}

	char dummy[1] = {0};
	FILE temp = FILE_INIT(fd, __SWR, _IONBF, dummy);

	return __jacl_printf(&temp, NULL, 0, fmt, ap);
}
int vasprintf(char **strp, const char *fmt, va_list ap) {
	if (!strp || !fmt) { errno = EINVAL; return -1; }

	va_list ap_copy;

	va_copy(ap_copy, ap);

	int len = vsnprintf(NULL, 0, fmt, ap_copy);

	va_end(ap_copy);

	if (len < 0) return -1;

	*strp = (char *)malloc(len + 1);

	if (!*strp) return -1;

	return vsnprintf(*strp, len + 1, fmt, ap);
}
int snprintf(char * restrict s, size_t n, const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vsnprintf(s, n, fmt, ap); va_end(ap); return r; }
int dprintf(int fd, const char* restrict fmt, ...) {
	va_list ap;

	va_start(ap, fmt);

	int r = vdprintf(fd, fmt, ap);

	va_end(ap);

	return r;
}
int asprintf(char **strp, const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);

	int r = vasprintf(strp, fmt, ap);

	va_end(ap);

	return r;
}
#endif

/* ============================================================= */
/* Scanf Implementations                                         */
/* ============================================================= */
int vscanf(const char * restrict fmt, va_list ap) { return __jacl_scanf(stdin, NULL, fmt, ap); }
int scanf(const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vscanf(fmt, ap); va_end(ap); return r; }
int vfscanf(FILE * restrict stream, const char * restrict fmt, va_list ap) { return __jacl_scanf(stream, NULL, fmt, ap); }
int fscanf(FILE* restrict stream, const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vfscanf(stream, fmt, ap); va_end(ap); return r; }
int vsscanf(const char * restrict s, const char* restrict fmt, va_list ap) { const char *in = s; return __jacl_scanf(NULL, &in, fmt, ap); }
int sscanf(const char * restrict s, const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vsscanf(s, fmt, ap); va_end(ap); return r; }

#ifdef __cplusplus
}
#endif

#endif /* CORE_STDIO_H */
