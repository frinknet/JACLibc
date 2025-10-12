/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef CORE_STDIO_H
#define CORE_STDIO_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

static char __jacl_stdin_buf[BUFSIZ];
static char __jacl_stdout_buf[BUFSIZ];
static char __jacl_stderr_buf[BUFSIZ];

static FILE __jacl_stdin_file = {
	__SRD,                       /* _flags */
	__jacl_stdin_buf,            /* _ptr */
	__jacl_stdin_buf,            /* _base */
	__jacl_stdin_buf + BUFSIZ,   /* _end */
	BUFSIZ,                      /* _bufsiz */
	0,                           /* _fd */
	0,                           /* _cnt */
	0,                           /* _orientation */
	NULL,                        /* _tmpfname */
	_IOLBF,                      /* _bufmode */
	1                            /* _buf_owned */
};

static FILE __jacl_stdout_file = {
	__SWR,                       /* _flags */
	__jacl_stdout_buf,           /* _ptr */
	__jacl_stdout_buf,           /* _base */
	__jacl_stdout_buf + BUFSIZ,  /* _end */
	BUFSIZ,                      /* _bufsiz */
	1,                           /* _fd */
	0,                           /* _cnt */
	0,                           /* _orientation */
	NULL,                        /* _tmpfname */
	_IOLBF,                      /* _bufmode */
	1                            /* _buf_owned */
};

static FILE __jacl_stderr_file = {
	__SWR,                       /* _flags */
	__jacl_stderr_buf,           /* _ptr */
	__jacl_stderr_buf,           /* _base */
	__jacl_stderr_buf + BUFSIZ,  /* _end */
	BUFSIZ,                      /* _bufsiz */
	2,                           /* _fd */
	0,                           /* _cnt */
	0,                           /* _orientation */
	NULL,                        /* _tmpfname */
	_IONBF,                      /* _bufmode */
	1                            /* _buf_owned */
};

FILE* stdin  = &__jacl_stdin_file;
FILE* stdout = &__jacl_stdout_file;
FILE* stderr = &__jacl_stderr_file;

typedef struct __jacl_stream_node {
	FILE* stream;
	struct __jacl_stream_node *next;
} STREAM_NODE;

static STREAM_NODE* __jacl_stream_list;

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
  if (stream) return (stream->_flags & __SWR) ? __jacl_buffer_output(stream) : 0;

  int result = 0;

  if (__jacl_buffer_output(stdout) == EOF) result = EOF;
  if (__jacl_buffer_output(stderr) == EOF) result = EOF;

  for (STREAM_NODE* n = __jacl_stream_list; n; n = n->next) {
    FILE* s = n->stream;

    if (s && (s->_flags & __SWR) && s != stdout && s != stderr) {
      if (__jacl_buffer_output(s) == EOF) result = EOF;
    }
  }

  return result;
}

#if __has_attribute(constructor)
	static void __attribute__((constructor)) __jacl_stream_init(void) {
		__jacl_stream_register(stdin);
		__jacl_stream_register(stdout);
		__jacl_stream_register(stderr);

		stdout->_bufmode = isatty(stdout->_fd) ? _IOLBF : _IOFBF;
	}
#else
	static int __jacl_stream_init = (
		__jacl_stream_register(stdin),
		__jacl_stream_register(stdout),
		__jacl_stream_register(stderr),
		stdout->_bufmode = isatty(stdout->_fd) ? _IOLBF : _IOFBF,
		0
	);
#endif

int __jacl_buffer_flags(const char *mode) {
	int flags = 0;

	if (!mode || !*mode) return -1;

	switch (*mode) {
		case 'r': flags = O_RDONLY; break;
		case 'w': flags = O_WRONLY | O_CREAT | O_TRUNC; break;
		case 'a': flags = O_WRONLY | O_CREAT | O_APPEND; break;
		default: return -1;
	}

	if (strchr(mode, '+')) {
		if (flags == O_RDONLY) flags = O_RDWR;
		else flags = (flags & ~O_WRONLY) | O_RDWR;
	}

	#if JACL_HAS_C11 || (defined(O_EXCL) && JACL_HAS_POSIX)
		if (strchr(mode, 'x')) flags |= O_EXCL;
	#endif

	return flags;
}

int __jacl_buffer_output(FILE *f) {
	if (JACL_UNLIKELY(!f || !(f->_flags & __SWR))) return 0;

	size_t to_write = f->_ptr - f->_base;
	ssize_t written = write(f->_fd, f->_base, to_write);

	if (JACL_UNLIKELY(written < 0)) { f->_flags |= __SERR; return EOF; }

	if ((size_t)written < to_write) {
		memmove(f->_base, f->_base + written, to_write - written);

		f->_ptr = f->_base + (to_write - written);
	} else {
		f->_ptr = f->_base;
	}

	f->_cnt = f->_bufsiz;

	return 0;
}

int __jacl_buffer_input(FILE *f) {
	if (JACL_UNLIKELY(!f || !(f->_flags & __SRD))) return EOF;

	ssize_t n = read(f->_fd, f->_base, f->_bufsiz);

	if (JACL_UNLIKELY(n <= 0)) {
		f->_flags |= (n == 0) ? __SEOF : __SERR;
		f->_cnt = 0;

		return EOF;
	}

	f->_ptr = f->_base + 1;
	f->_cnt = (int)n - 1;

	return (unsigned char)f->_base[0];
}

static inline int __jacl_getch(const char **in, FILE *stream) {
	if (*in && **in) return *(*in)++;

	return fgetc(stream ? stream : stdin);
}

static inline int __jacl_scan_u32(const char **restrict p) {
	const char *s = *p;
	int v = 0;

	while ((unsigned)(*s - '0') <= 9u) {
		v = v * 10 + (int)(*s - '0');

		s++;
	}

	*p = s;

	return v;
}

static inline int __jacl_mod_length(const char **restrict pfmt) {
	const char *fmt = *pfmt;
	int length = 0;

	if (JACL_UNLIKELY(*fmt == 'l' || *fmt == 'h')) {
		switch (*fmt++) {
			case 'l':
				if (*fmt == 'l') { length = 2; fmt++; }
				else length = 1;

				break;
			case 'h':
				if (*fmt == 'h') { length = -2; fmt++; }
				else length = -1;

				break;
		}
	}

	#if JACL_HAS_C99
		else if (*fmt == 'z' || *fmt == 't' || *fmt == 'j') {
			switch (*fmt++) {
				case 'z': length = 3; break;  // size_t
				case 't': length = 4; break;  // ptrdiff_t
				case 'j': length = 5; break;  // intmax_t
			}
		}
	#endif

	*pfmt = fmt;

	return length;
}

static inline void __jacl_store_signed(int length, va_list ap, uintptr_t v) {
	switch (length) {
		case -2: *va_arg(ap, signed char*) = (signed char)v; break;
		case -1: *va_arg(ap, short*) = (short)v; break;
		case  1: *va_arg(ap, long*) = (long)v; break;
	#if JACL_HAS_C99
		case  2: *va_arg(ap, long long*) = (long long)v; break;
		case  3: *va_arg(ap, ssize_t*) = (ssize_t)v; break;
		case  4: *va_arg(ap, ptrdiff_t*) = (ptrdiff_t)v; break;
		case  5: *va_arg(ap, intmax_t*) = (intmax_t)v; break;
	#endif
		default: *va_arg(ap, int*) = (int)v; break;
	}
}

static inline void __jacl_store_unsigned(int length, va_list ap, uintptr_t v) {
	switch (length) {
		case -2: *va_arg(ap, unsigned char*) = (unsigned char)v; break;
		case -1: *va_arg(ap, unsigned short*) = (unsigned short)v; break;
		case  1: *va_arg(ap, unsigned long*) = (unsigned long)v; break;
	#if JACL_HAS_C99
		case  2: *va_arg(ap, unsigned long long*) = (unsigned long long)v; break;
		case  3: *va_arg(ap, size_t*) = (size_t)v; break;
		case  4: *va_arg(ap, size_t*) = (size_t)v; break;
		case  5: *va_arg(ap, uintmax_t*) = (uintmax_t)v; break;
	#endif
		default: *va_arg(ap, unsigned int*) = (unsigned int)v; break;
	}
}

static inline int __jacl_isspace(int c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

static inline int __jacl_format_string(char **out, const char *s, size_t remaining, int prec) {
	if (!s) s = "(null)";

	int len = 0;

	if (out && *out && remaining > 0) {
		while (*s && remaining-- > 0 && (prec < 0 || len < prec)) {
			*(*out)++ = *s++;

			len++;
		}

		while (*s++) len++;
	} else {
		while (*s++) len++;
	}

	return len;
}

static inline int __jacl_format_num(char * restrict *out, uintptr_t num, int base, int sign, int width, int uppercase, int flags) {
	static const char LD[] = "0123456789abcdef";
	static const char UD[] = "0123456789ABCDEF";
	const char *digits = uppercase ? UD : LD;
	char buffer[64];
	int len = 0, negative = 0;

	if (sign && (intptr_t)num < 0) {
		negative = 1;
		num = (uintptr_t)(-(intptr_t)num);
	}

	int num_start = len;
	int orig_nonzero = (num != 0);

	if (num == 0) {
		buffer[len++] = '0';
	} else {
		while (num > 0) {
			buffer[len++] = digits[num % base];
			num /= base;
		}
	}

	if (flags & 0x08) {
		if (base == 16 && orig_nonzero) {
			buffer[len++] = uppercase ? 'X' : 'x';
			buffer[len++] = '0';
		} else if (base == 8 && buffer[num_start] != '0') {
			buffer[len++] = '0';
		}
	}

	if (negative) {
		buffer[len++] = '-';
	} else if (flags & 0x02) {
		buffer[len++] = '+';
	} else if (flags & 0x04) {
		buffer[len++] = ' ';
	}

	int pad_len = width - len;

	if (pad_len > 0 && (flags & 0x10 || !(flags & 0x01))) {
		char pad = (flags & 0x10) ? '0' : ' ';

		for (int i = 0; i < pad_len; i++) buffer[len++] = pad;
	}

	if (out && *out) {
		for (int i = len - 1; i >= 0; i--) *(*out)++ = buffer[i];

		if ((flags & 0x01) && pad_len > 0) {
			for (int i = 0; i < pad_len; i++) *(*out)++ = ' ';
		}
	}

	return len + ((flags & 0x01) && pad_len > 0 ? pad_len : 0);
}

static inline int __jacl_format_ptr(char **out, void *ptr, int width, int flags) {
	union { void *ptr; uintptr_t u; } pun;

	pun.ptr = ptr;

	return __jacl_format_num(out, pun.u, 16, 0, width, 0, flags);
}

static inline int __jacl_format_exp(char **out, double num, int prec, int upper) {
	char buf[32];
	int pos = 0;

	if (prec < 0) prec = 6;

	if (num != num) {
		const char *s = upper ? "NAN" : "nan";

		buf[pos++] = s[0]; buf[pos++] = s[1]; buf[pos++] = s[2];

		goto done;
	}

	if (num > DBL_MAX || num < -DBL_MAX) {
		if (num < 0) buf[pos++] = '-';
		const char *s = upper ? "INF" : "inf";

		buf[pos++] = s[0]; buf[pos++] = s[1]; buf[pos++] = s[2];

		goto done;
	}

	if (num == 0.0) {
		buf[pos++] = '0';

		if (prec > 0) {
			buf[pos++] = '.';

			for (int i = 0; i < prec; i++) buf[pos++] = '0';
		}

		buf[pos++] = upper ? 'E' : 'e';
		buf[pos++] = '+'; buf[pos++] = '0'; buf[pos++] = '0';

		goto done;
	}

	if (num < 0) {
		buf[pos++] = '-';
		num = -num;
	}

	int exp = 0;

	while (num >= 10.0) { num *= 0.1; exp++; }
	while (num < 1.0) { num *= 10.0; exp--; }

	int d = (int)num;

	buf[pos++] = '0' + d;

	double fpart = num - d;

	if (prec > 0) {
		buf[pos++] = '.';

		for (int i = 0; i < prec; i++) {
			fpart *= 10.0;

			if (i == prec - 1) fpart += 0.5;

			d = (int)fpart;
			buf[pos++] = '0' + d;
			fpart -= d;
		}
	}

	buf[pos++] = upper ? 'E' : 'e';
	buf[pos++] = exp < 0 ? '-' : '+';

	int e = exp < 0 ? -exp : exp;

	if (e >= 100) { buf[pos++] = '0' + e / 100; e %= 100; }

	buf[pos++] = '0' + e / 10;
	buf[pos++] = '0' + e % 10;

done:
	if (out && *out) {
		for (int i = 0; i < pos; i++) *(*out)++ = buf[i];
	}

	return pos;
}

static inline int __jacl_format_float(char * restrict *out, const char *fmt_pos, int prec, int width, int flags, va_list ap) {
	double val = va_arg(ap, double);
	char spec = *fmt_pos;
	int uppercase = (spec == 'E' || spec == 'G');

	if (spec == 'e' || spec == 'E') return __jacl_format_exp(out, val, prec, uppercase);

	if (spec == 'g' || spec == 'G') {
		int p = (prec < 0) ? 6 : (prec == 0) ? 1 : prec;
		double absval = (val < 0) ? -val : val;
		int exp = 0;

		while (absval >= 10.0) { absval /= 10.0; exp++; }
		while (absval > 0.0 && absval < 1.0) { absval *= 10.0; exp--; }

		if (exp < -4 || exp >= p) return __jacl_format_exp(out, val, p - 1, uppercase);

		prec = p - exp - 1;
	} else {
		prec = (prec < 0) ? 6 : prec;
	}

	int len = 0;

	if (val < 0) { if (out && *out) *(*out)++ = '-'; len++; val = -val; }

	#if JACL_HAS_C99
		long long ipart = (long long)val;
	#else
		long ipart = (long)val;
	#endif

	len += __jacl_format_num(out, ipart, 10, 0, 0, 0, 0);

	if (prec > 0) {
		if (out && *out) *(*out)++ = '.';

		len++;

		double fpart = val - (double)ipart;

		for (int i = 0; i < prec; i++) {
			fpart *= 10.0;

			if (i == prec - 1) fpart += 0.5;

			int digit = (int)fpart;

			if (out && *out) *(*out)++ = '0' + digit;

			len++;
			fpart -= digit;
		}
	}

	return len;
}

static inline int __jacl_format_int(char * restrict *out, const char *fmt_pos, int length, int width, int flags, va_list ap) {
	char spec = *fmt_pos;
	int is_signed = (spec == 'i' || spec == 'd');

	#if JACL_HAS_C23
		int base = (spec == 'b' || spec == 'B') ? 2 : (spec == 'o') ? 8 : (spec == 'x' || spec == 'X') ? 16 : 10;
		int upper = (spec == 'X' || spec == 'B');
	#else
		int base = (spec == 'o') ? 8 : (spec == 'x' || spec == 'X') ? 16 : 10;
		int upper = (spec == 'X');
	#endif

	uintptr_t val;

	if (is_signed) {
		switch (length) {
			case 1: val = (uintptr_t)va_arg(ap, long); break;
		#if JACL_HAS_C99
			case 2: val = (uintptr_t)va_arg(ap, long long); break;
			case 3: val = (uintptr_t)va_arg(ap, ssize_t); break;
			case 4: val = (uintptr_t)va_arg(ap, ptrdiff_t); break;
			case 5: val = (uintptr_t)va_arg(ap, intmax_t); break;
		#endif
			default: val = (uintptr_t)va_arg(ap, int); break;
		}
	} else {
		switch (length) {
			case 1: val = va_arg(ap, unsigned long); break;
		#if JACL_HAS_C99
			case 2: val = va_arg(ap, unsigned long long); break;
			case 3: val = va_arg(ap, size_t); break;
			case 4: val = va_arg(ap, size_t); break;
			case 5: val = va_arg(ap, uintmax_t); break;
		#endif
			default: val = va_arg(ap, unsigned int); break;
		}
	}

	return __jacl_format_num(out, val, base, is_signed, width, upper, flags);
}

int __jacl_format(char * restrict out, size_t n, const char * restrict fmt, va_list ap) {
	if (!fmt) return -1;

	char ch;
	int len = 0, width = 0, prec = -1;

	while ((ch = *fmt++)) {
		if (ch != '%') {
			if (out && len < (int)n - 1) *out++ = ch;

			len++;

			continue;
		}

		int flags = 0;
		int parsing = 1;

		while (parsing) {
			switch (*fmt) {
				case '-': flags |= 0x01; fmt++; break;
				case '+': flags |= 0x02; fmt++; break;
				case ' ': flags |= 0x04; fmt++; break;
				case '#': flags |= 0x08; fmt++; break;
				case '0': flags |= 0x10; fmt++; break;
				default: parsing = 0; break;
			}
		}

		width = 0;

		if (*fmt == '*') { width = va_arg(ap, int); if (width < 0) { flags |= 0x01; width = -width; } fmt++; }
		else if ((unsigned)(*fmt - '0') <= 9u) { width = __jacl_scan_u32(&fmt); }

		if (*fmt == '.') {
			fmt++;

			if (*fmt == '*') { prec = va_arg(ap, int); if (prec < 0) prec = -1; fmt++; }
			else { prec = __jacl_scan_u32(&fmt); }
		}

		int length = __jacl_mod_length(&fmt);
		size_t remaining = (out && len < (int)n - 1) ? n - len - 1 : 0;

		switch (*fmt++) {
			case '%': if (out && len < (int)n - 1) *out++ = '%'; len++; break;
			case 'c': if (out && len < (int)n - 1) *out++ = (char)va_arg(ap, int); len++; break;
			case 's': len += __jacl_format_string(out ? &out : NULL, va_arg(ap, const char*), remaining, prec); break;
			case 'i':
			case 'd':
			case 'u':
			case 'o':
		#if JACL_HAS_C23
			case 'b':
			case 'B':
		#endif
			case 'x':
			case 'X': len += __jacl_format_int(out ? &out : NULL, fmt - 1, length, width, flags, ap); break;
			case 'f':
			case 'e':
			case 'E':
			case 'g':
			case 'G': len += __jacl_format_float(out ? &out : NULL, fmt - 1, prec, width, flags, ap); break;
			case 'p': len += __jacl_format_ptr(out ? &out : NULL, va_arg(ap, void*), width, flags); break;
			case 'n': __jacl_store_signed(length, ap, (uintptr_t)len); break;
			default: if (out && len < (int)n - 1) *out++ = ch; len++; break;
		}

		prec = -1;
	}

	if (out && n) out[len < (int)n ? len : n - 1] = '\0';

	return len;
}

static inline void __jacl_parse_next(const char **in) {
	if (!in || !*in) return;

	while (__jacl_isspace(**in)) (*in)++;
}

static inline int __jacl_parse_char(const char **in, FILE *stream, int width, int suppress, va_list ap) {
	char *p = suppress ? NULL : va_arg(ap, char*);
	int w = (width == INT_MAX) ? 1 : width;

	while (w--) {
		int ch = __jacl_getch(in, stream);

		if (ch == EOF) break;
		if (p) *p++ = (char)ch;
	}

	return suppress ? 0 : 1;
}

static inline int __jacl_parse_string(const char **in, FILE *stream, int width, int suppress, va_list ap) {
	char *p = suppress ? NULL : va_arg(ap, char*);

	__jacl_parse_next(in);

	int i = 0;

	while (i < width) {
		int ch = __jacl_getch(in, stream);

		if (ch == EOF || __jacl_isspace(ch)) break;
		if (p) p[i] = (char)ch;

		i++;
	}

	if (p) p[i] = '\0';

	return suppress ? 0 : 1;
}

static inline int __jacl_parse_scanset(const char **in, FILE *stream, const char **fmt, int width, int suppress, va_list ap) {
	char *p = suppress ? NULL : va_arg(ap, char*);
	int i = 0, invert = 0;

	if (**fmt == '^') { invert = 1; (*fmt)++; }

	char charset[256] = {0};
	int has_chars = 0;

	if (**fmt == ']') { charset[(unsigned char)']'] = 1; (*fmt)++; has_chars = 1; }

	while (**fmt && **fmt != ']') {
		if ((*fmt)[1] == '-' && (*fmt)[2] != ']' && (*fmt)[2] != '\0') {
			unsigned char start = (unsigned char)**fmt;
			unsigned char end = (unsigned char)(*fmt)[2];

			for (unsigned char c = start; c <= end; c++) charset[c] = 1;

			(*fmt) += 3;
		} else {
			charset[(unsigned char)**fmt] = 1;
			(*fmt)++;
		}

		has_chars = 1;
	}

	if (**fmt == ']') (*fmt)++;
	if (!has_chars) return 0;

	__jacl_parse_next(in);

	while (i < width) {
		int ch = __jacl_getch(in, stream);

		if (ch == EOF) break;

		int match = charset[(unsigned char)ch] ? 1 : 0;

		if (invert) match = !match;
		if (!match) break;
		if (p) p[i] = (char)ch;

		i++;
	}

	if (i > 0) {
		if (p) p[i] = '\0';

		return suppress ? 0 : 1;
	}

	return 0;
}

static inline int __jacl_parse_digit(unsigned char c, int base) {
	if (c >= '0' && c <= '9') {
		int val = c - '0';

		return val < base ? val : -1;
	}

	if (base > 10) {
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	}

	return -1;
}

static inline int __jacl_parse_num(const char **in, uintptr_t *out, int base, int sign) {
	const char *start = *in;
	uintptr_t result = 0;
	int negative = 0, digits = 0;

	while (__jacl_isspace(**in)) (*in)++;

	if (sign && **in == '-') {
		negative = 1;

		(*in)++;
	} else if (**in == '+') {
		(*in)++;
	}

	while (**in) {
		int digit_val = __jacl_parse_digit((unsigned char)**in, base);

		if (digit_val >= 0) {
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

static inline int __jacl_parse_ptr(const char **in, int suppress, va_list ap) {
	void **out = suppress ? NULL : va_arg(ap, void**);
	uintptr_t val;

	if (__jacl_parse_num(in, &val, 16, 0)) {
		if (out) *out = (void*)val;

		return suppress ? 0 : 1;
	}

	return 0;
}

static inline int __jacl_parse_int(const char **in, const char *fmt_pos, int length, int suppress, va_list ap) {
	char spec = *fmt_pos;
	int is_signed = (spec == 'i' || spec == 'd');
	int base = 10;

	if (spec == 'i') {
		__jacl_parse_next(in);

		if (**in == '0' && ((*in)[1] == 'x' || (*in)[1] == 'X')) {
			(*in) += 2;
			base = 16;
		} else if (**in == '0' && (*in)[1] >= '0' && (*in)[1] <= '7') {
			base = 8;
		}
	} else {
		#if JACL_HAS_C23
			base = (spec == 'b' || spec == 'B') ? 2 : (spec == 'o') ? 8 : (spec == 'x' || spec == 'X') ? 16 : 10;
		#else
			base = (spec == 'o') ? 8 : (spec == 'x' || spec == 'X') ? 16 : 10;
		#endif
	}

	uintptr_t val;

	if (__jacl_parse_num(in, &val, base, is_signed) && !suppress) {
		if (is_signed) __jacl_store_signed(length, ap, val);
		else __jacl_store_unsigned(length, ap, val);

		return 1;
	}

	return 0;
}

static inline int __jacl_parse_float(const char **in, int length, int suppress, va_list ap) {
	uintptr_t ipart;
	int digits = __jacl_parse_num(in, &ipart, 10, 1);
	double result = (double)ipart;

	if (**in == '.') {
		(*in)++;

		double divisor = 10.0;

		while (**in >= '0' && **in <= '9') {
			result += (**in - '0') / divisor;
			divisor *= 10.0;

			(*in)++;
			digits++;
		}
	}

	if ((**in == 'e' || **in == 'E') && digits > 0) {
		(*in)++;

		uintptr_t exp_u;
		int exp_digits = __jacl_parse_num(in, &exp_u, 10, 1);

		if (exp_digits > 0) {
			int exp_val = (int)exp_u;

			if (exp_val > 300) exp_val = 300;
			else if (exp_val < -300) exp_val = -300;

			int abs_exp = (exp_val < 0) ? -exp_val : exp_val;

			if (abs_exp > 100) {
				result *= pow(10.0, (double)exp_val);
			} else if (exp_val > 0) {
				for (int i = 0; i < exp_val; i++) result *= 10.0;
			} else if (exp_val < 0) {
				for (int i = 0; i < abs_exp; i++) result /= 10.0;
			}
		}
	}

	if (digits) {
		if (!suppress) {
			if (length == 1) *va_arg(ap, double*) = result;
			else *va_arg(ap, float*) = (float)result;
		}

		return suppress ? 0 : 1;
	}

	return 0;
}

int __jacl_parse(const char **input, FILE *stream, const char * restrict fmt, va_list ap) {
	const char *in = input ? *input : NULL;
	int count = 0, suppress = 0;

	while (*fmt) {
		if (*fmt != '%') {
			__jacl_parse_next(&in);

			int ch = __jacl_getch(&in, stream);

			if (ch != *fmt) return count;

			fmt++;

			continue;
		}

		fmt++;

		if (*fmt == '*') {
			suppress = 1;
			fmt++;
		}

		int width = INT_MAX;

		if ((unsigned)(*fmt - '0') <= 9u) width = __jacl_scan_u32(&fmt);

		int length = __jacl_mod_length(&fmt);
		char type = *fmt++;

		switch (type) {
			case 'c': count += __jacl_parse_char(&in, stream, width, suppress, ap); break;
			case 's': count += __jacl_parse_string(&in, stream, width, suppress, ap); break;
			case '[': count += __jacl_parse_scanset(&in, stream, &fmt, width, suppress, ap); break;
			case 'i':
			case 'd':
			case 'u':
			case 'o':
			#if JACL_HAS_C23
			case 'b':
			case 'B':
			#endif
			case 'x':
			case 'X': count += __jacl_parse_int(&in, fmt - 1, length, suppress, ap); break;
			case 'p': count += __jacl_parse_ptr(&in, suppress, ap); break;
			case 'e':
			case 'E':
			case 'g':
			case 'G':
			case 'f': count += __jacl_parse_float(&in, length, suppress, ap); break;
			case '%': __jacl_parse_next(&in); if (__jacl_getch(&in, stream) == '%') count++; break;
			default: break;
		}

		suppress = 0;
	}

	if (input) *input = in;

	return count;
}

/* Printf Implementations */
int vprintf(const char * restrict fmt, va_list ap) { char buf[BUFSIZ]; int len = __jacl_format(buf, BUFSIZ, fmt, ap); if (fwrite(buf, 1, len, stdout) != (size_t)len) return -1; return len; }
int printf(const char* restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vprintf(fmt, ap); va_end(ap); return r; }
int vfprintf(FILE * restrict stream, const char * restrict fmt, va_list ap) { char buf[BUFSIZ]; int len = __jacl_format(buf, BUFSIZ, fmt, ap); if (fwrite(buf, 1, len, stream) != (size_t)len) return -1; return len; }
int fprintf(FILE* restrict stream, const char* restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vfprintf(stream, fmt, ap); va_end(ap); return r; }
int vsprintf(char * restrict s, const char * restrict fmt, va_list ap) { return __jacl_format(s, SIZE_MAX, fmt, ap); }
int sprintf(char * restrict s, const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vsprintf(s, fmt, ap); va_end(ap); return r; }

/* Scanf Implementations */
int vscanf(const char * restrict fmt, va_list ap) { return __jacl_parse(NULL, stdin, fmt, ap); }
int scanf(const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = __jacl_parse(NULL, stdin, fmt, ap); va_end(ap); return r; }
int vfscanf(FILE * restrict stream, const char * restrict fmt, va_list ap) { return __jacl_parse(NULL, stream, fmt, ap); }
int fscanf(FILE* restrict stream, const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vfscanf(stream, fmt, ap); va_end(ap); return r; }
int vsscanf(const char * restrict s, const char* restrict fmt, va_list ap) { const char *in = s; return __jacl_parse(&in, NULL, fmt, ap); }
int sscanf(const char * restrict s, const char * restrict fmt, ...) { const char *in = s; va_list ap; va_start(ap, fmt); int r = __jacl_parse(&in, NULL, fmt, ap); va_end(ap); return r; }

#if JACL_HAS_C99
int vsnprintf(char * restrict s, size_t n, const char * restrict fmt, va_list ap) { return __jacl_format(s, n, fmt, ap); }
int snprintf(char * restrict s, size_t n, const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vsnprintf(s, n, fmt, ap); va_end(ap); return r; }
#endif

#ifdef __cplusplus
}
#endif

#endif /* CORE_STDIO_H */
