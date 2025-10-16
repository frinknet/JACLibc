/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef CORE_STDIO_H
#define CORE_STDIO_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================= */
/* Internal Variable                                             */
/* ============================================================= */
#define JACL_FILE(name, flag, fd, bufmode) \
static char __jacl_##name##_buf[BUFSIZ]; \
static FILE __jacl_##name##_file = { \
	flag, __jacl_##name##_buf, __jacl_##name##_buf, __jacl_##name##_buf + BUFSIZ, \
	BUFSIZ, fd, 0, 0, NULL, bufmode, 1 \
}; \
FILE* name = &__jacl_##name##_file

JACL_FILE(stdin, __SRD, 0, _IOLBF);
JACL_FILE(stdout, __SWR, 1, _IOLBF);
JACL_FILE(stderr, __SWR, 2, _IONBF);

typedef struct __jacl_stream_node {
	FILE* stream;
	struct __jacl_stream_node *next;
} STREAM_NODE;

static STREAM_NODE* __jacl_stream_list;

static const unsigned long long __jacl_pow10[20] = {
	0xff77b1fcbebcdc4fULL, 0x9faacf3df73609b1ULL, 0xc795830d75038c1dULL, 0xf97ae3d0d2446f25ULL,
	0x9becce62836ac577ULL, 0xc2e801fb244576d5ULL, 0xf3a20279ed56d48aULL, 0x9845418c345644d6ULL,
	0xbe5691ef416bd60cULL, 0xedec366b11c6cb8fULL, 0x94b3a202eb1c3f39ULL, 0xb9e08a83a5e34f07ULL,
	0xe858ad248f5c22c9ULL, 0x91376c36d99995beULL, 0xb58547448ffffb2dULL, 0xe2e69915b3fff9f9ULL,
	0x8dd01fad907ffc3bULL, 0xb1442798f49ffb4aULL, 0xdd95317f31c7fa1dULL, 0x8a7d3eef7f1cfc52ULL
};

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
  if (stream) return (stream->_flags & __SWR) ? __jacl_buffer_output(stream) : 0;

  int result = 0;

  if (__jacl_buffer_output(stdout) == EOF) result = EOF;
  if (__jacl_buffer_output(stderr) == EOF) result = EOF;

  for (STREAM_NODE* n = __jacl_stream_list; n; n = n->next) {
    FILE* s = n->stream;

    if (s && (s->_flags & __SWR) && s != stdout && s != stderr && __jacl_buffer_output(s) == EOF) result = EOF;
  }

  return result;
}

static void __attribute__((constructor)) __jacl_stream_init(void) {
	__jacl_stream_register(stdin);
	__jacl_stream_register(stdout);
	__jacl_stream_register(stderr);

	stdout->_bufmode = isatty(stdout->_fd) ? _IOLBF : _IOFBF;
}

/* ============================================================= */
/* Buffer Helpers                                                */
/* ============================================================= */
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

/* ============================================================= */
/* String Helpers                                                */
/* ============================================================= */
static inline int __jacl_mod_length(const char **restrict pfmt) {
	const char *fmt = *pfmt;
	int length = (*fmt == 'l') ? (fmt[1] == 'l' ? (fmt += 2, 2) : (fmt++, 1)) :
	             (*fmt == 'h') ? (fmt[1] == 'h' ? (fmt += 2, -2) : (fmt++, -1)) : 0;

#if JACL_HAS_C99
	if (!length && (*fmt == 'z' || *fmt == 't' || *fmt == 'j')) {
		length = (*fmt == 'z') ? 3 : (*fmt == 't') ? 4 : 5;

		fmt++;
	}
#endif

	*pfmt = fmt;

	return length;
}

#define CASE(N,type) case N: *va_arg(ap, type*) = (type)v; break
static inline void __jacl_store_signed(int length, va_list ap, uintptr_t v) {
	switch (length) {
		CASE(-2, signed char); CASE(-1, short); CASE(1, long);
	#if JACL_HAS_C99
		CASE(2, long long); CASE(3, ssize_t); CASE(4, ptrdiff_t); CASE(5, intmax_t);
	#endif
		default: *va_arg(ap, int*) = (int)v; break;
	}
}

static inline void __jacl_store_unsigned(int length, va_list ap, uintptr_t v) {
	switch (length) {
		CASE(-2, unsigned char); CASE(-1, unsigned short); CASE(1, unsigned long);
	#if JACL_HAS_C99
		CASE(2, unsigned long long); CASE(3, size_t); CASE(4, size_t); CASE(5, uintmax_t);
	#endif
		default: *va_arg(ap, unsigned int*) = (unsigned int)v; break;
	}
}
#undef CASE

#define __jacl_isspace(c) ((c)==' '||(c)=='\t'||(c)=='\n'||(c)=='\r'||(c)=='\f'||(c)=='\v')
#define __jacl_skip_space(in) do { if ((in) && *(in)) while (__jacl_isspace(**(in))) (*(in))++; } while(0)
#define __jacl_getch(in, s) (((in) && **(in)) ? *(*in)++ : fgetc((s)?(s):stdin))
#define __jacl_ungetch(ch, in, s) do { if ((in) && *(in)) (*(in))--; else ungetc((ch), (s)); } while(0)
#define __jacl_set_u32(w,fmt) w = 0; while ((unsigned)(*fmt - '0') <= 9u) { w = w * 10 + (*fmt - '0'); fmt++; }

/* ============================================================= */
/* String Formatting                                             */
/* ============================================================= */
static inline int __jacl_format_string(char **out, const char *s, size_t remaining, int prec, int width, int flags) {
	if (!s) s = "(null)";

	int len = 0;
	const char *p = s;

	while (*p && (prec < 0 || len < prec)) { p++; len++; }

	int pad = width - len;

	if (pad < 0) pad = 0;

	if (pad > 0 && !(flags & 0x01) && out && *out) {
		for (int i = 0; i < pad; i++) *(*out)++ = ' ';
	}

	if (out && *out && remaining > 0) {
		p = s;

		int written = 0;

		while (*p && remaining-- > 0 && (prec < 0 || written < prec)) { *(*out)++ = *p++; written++; }
	}

	if (pad > 0 && (flags & 0x01) && out && *out) {
		for (int i = 0; i < pad; i++) *(*out)++ = ' ';
	}

	return len + pad;
}

static inline int __jacl_format_num(char * restrict *out, uintptr_t num, int base, int sign, int width, int uppercase, int flags) {
	static const char LD[] = "0123456789abcdef";
	static const char UD[] = "0123456789ABCDEF";
	const char *digits = uppercase ? UD : LD;
	char buffer[64];
	int len = 0, neg = 0;

	if (sign && (intptr_t)num < 0) {
		neg = 1;
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

	if (neg) {
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

	if (num != num || num > DBL_MAX || num < -DBL_MAX) {
		int is_inf = (num > DBL_MAX || num < -DBL_MAX);

		if (is_inf && num < 0) buf[pos++] = '-';

		const char *s = is_inf ? (upper ? "INF" : "inf") : (upper ? "NAN" : "nan");

		buf[pos++] = s[0]; buf[pos++] = s[1]; buf[pos++] = s[2];

		goto done;
	} else if (num == 0.0) {
		buf[pos++] = '0';

		if (prec > 0) {
			buf[pos++] = '.';

			for (int i = 0; i < prec; i++) buf[pos++] = '0';
		}

		buf[pos++] = upper ? 'E' : 'e';
		buf[pos++] = '+'; buf[pos++] = '0'; buf[pos++] = '0';

		goto done;
	} else if (num < 0) {
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

#define MULSHIFT64(m, mul, shift, res) do { \
	unsigned long long a = (unsigned long long)(m), b = (mul); \
	unsigned long long a_lo = a & 0xFFFFFFFFULL, a_hi = a >> 32; \
	unsigned long long b_lo = b & 0xFFFFFFFFULL, b_hi = b >> 32; \
	unsigned long long p0 = a_lo * b_lo, p1 = a_lo * b_hi; \
	unsigned long long p2 = a_hi * b_lo, p3 = a_hi * b_hi; \
	unsigned long long m1 = p1 + (p0 >> 32), m2 = p2 + (m1 & 0xFFFFFFFFULL); \
	unsigned long long hi = p3 + (m1 >> 32) + (m2 >> 32); \
	unsigned long long lo = (m2 << 32) | (p0 & 0xFFFFFFFFULL); \
	(res) = ((shift) >= 64) ? (hi >> ((shift) - 64)) : ((hi << (64 - (shift))) | (lo >> (shift))); \
} while(0)
static inline int __jacl_format_float(char * restrict *out, const char *fmt_pos, int prec, int width, int flags, va_list ap) {
	double val = va_arg(ap, double);
	char spec = *fmt_pos, digits[32], buf[64], *bp = buf, *p = out ? *out : NULL;
	unsigned long long bits, mbit, m2, mv, mp, mm, vr, vp, vm;
	unsigned int exp, mant, w;
	int neg, e2, q, e10, k, idx, mm_shift, even, vm_trail, vr_trail, last, ndigits, dec_pos, len, i;

	if (spec == 'e' || spec == 'E') return __jacl_format_exp(out, val, prec, (spec == 'E'));

	if (spec == 'g' || spec == 'G') {
		int exp_val = 0;
		double absval = (val < 0) ? -val : val;
		int p = (prec < 0) ? 6 : (prec == 0) ? 1 : prec;

		while (absval >= 10.0) { absval /= 10.0; exp_val++; }
		while (absval > 0.0 && absval < 1.0) { absval *= 10.0; exp_val--; }

		if (exp_val < -4 || exp_val >= p) return __jacl_format_exp(out, val, p - 1, (spec == 'G'));

		prec = p - exp_val - 1;
	}

	if (prec < 0) prec = 6;
	if (prec > 15) prec = 15;

	memcpy(&bits, &val, sizeof(double));

	exp = (bits >> 52) & 0x7FF;
	mbit = bits & 0xFFFFFFFFFFFFFULL;
	neg = (bits >> 63);
	bp = buf;
	p = out ? *out : NULL;

	if (exp == 0x7FF) {
		const char *s = mbit ? "nan" : "inf";

		if (p) { memcpy(p, s, 3); p += 3; }
		if (out) *out = p;

		return 3;
	}

	if (!(exp | mbit)) {
		if (neg) *bp++ = '-';

		*bp++ = '0';

		if (prec > 0) { *bp++ = '.'; for (i = 0; i < prec; i++) *bp++ = '0'; }

		len = bp - buf;

		if (p) { memcpy(p, buf, len); p += len; }
		if (out) *out = p;

		return len;
	}

	e2 = exp ? (int)exp - 1075 : -1074;
	m2 = exp ? (1ULL << 52) | mbit : mbit;
	even = !(m2 & 1);
	mm_shift = (mbit == 0 && exp > 1);
	mv = m2 << 2;
	mp = mv + 2;
	mm = mv - (mm_shift ? 2 : 1);
	e2 -= 2;
	q = (int)((unsigned int)e2 * 1262611u) >> 22;

	if (e2 < 0) q--;

	e10 = q;
	k = 124 + q - e2;
	idx = -e10;

	if (idx < 0 || idx > 19) return __jacl_format_exp(out, val, prec, 0);

	MULSHIFT64(mv, __jacl_pow10[idx], k, vr);
	MULSHIFT64(mp, __jacl_pow10[idx], k, vp);
	MULSHIFT64(mm, __jacl_pow10[idx], k, vm);

	vm_trail = mm_shift;
	vr_trail = !mm_shift && !(vr & 1);
	last = 0;

	while (vp / 10 > vm / 10) {
		vm_trail = vm_trail && !(vm % 10);
		vr_trail = vr_trail && !last;
		last = vr % 10;
		vr /= 10; vp /= 10; vm /= 10; e10++;
	}

	if (vm_trail) {
		while (!(vm % 10)) {
			vr_trail = vr_trail && !last;
			last = vr % 10;
			vr /= 10; vp /= 10; vm /= 10; e10++;
		}
	}

	if (vr_trail && last == 5 && !(vr & 1)) last = 4;

	mant = (unsigned int)vr + ((vr == vm) ? (!even || !vm_trail) : (last >= 5));
	ndigits = 0;
	w = mant ? mant : 1;

	if (mant) { while (w) { digits[ndigits++] = '0' + w % 10; w /= 10; } }
	else digits[ndigits++] = '0';

	if (neg) *bp++ = '-';

	dec_pos = e10 + 1;

	if (dec_pos > 0 && dec_pos <= ndigits) {
		for (i = ndigits - 1; i >= ndigits - dec_pos; i--) *bp++ = digits[i];

		if (prec > 0) {
			*bp++ = '.';

			for (i = 0; i < prec; i++) {
				int idx = ndigits - dec_pos - 1 - i;

				*bp++ = (idx >= 0) ? digits[idx] : '0';
			}
		}
	} else if (dec_pos > ndigits) {
		for (i = ndigits - 1; i >= 0; i--) *bp++ = digits[i];
		for (i = 0; i < dec_pos - ndigits; i++) *bp++ = '0';

		if (prec > 0) { *bp++ = '.'; for (i = 0; i < prec; i++) *bp++ = '0'; }
	} else {
		*bp++ = '0';

		if (prec > 0) {
			*bp++ = '.';

			for (i = 0; i < -dec_pos && i < prec; i++) *bp++ = '0';
			for (len = -dec_pos; i < prec && ndigits - 1 - (i - len) >= 0; i++) *bp++ = digits[ndigits - 1 - (i - len)];
			for (; i < prec; i++) *bp++ = '0';
		}
	}

	len = bp - buf;

	if (p) { memcpy(p, buf, len); p += len; }
	if (out) *out = p;

	return len;
}
#undef MULSHIFT64

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

static inline int __jacl_format(char * restrict out, size_t n, const char * restrict fmt, va_list ap) {
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
		else if ((unsigned)(*fmt - '0') <= 9u) { __jacl_set_u32(width, fmt); }

		if (*fmt == '.') {
			fmt++;

			if (*fmt == '*') { prec = va_arg(ap, int); if (prec < 0) prec = -1; fmt++; }
			else { __jacl_set_u32(prec, fmt); }
		}

		int length = __jacl_mod_length(&fmt);
		size_t remaining = (out && len < (int)n - 1) ? n - len - 1 : 0;

		switch (*fmt++) {
			case '%': if (out && len < (int)n - 1) *out++ = '%'; len++; break;
			case 'c': if (out && len < (int)n - 1) *out++ = (char)va_arg(ap, int); len++; break;
			case 's': len += __jacl_format_string(out ? &out : NULL, va_arg(ap, const char*), remaining, prec, width, flags); break;
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

/* ============================================================= */
/* String Parsing                                                */
/* ============================================================= */
static inline int __jacl_parse_string(const char **in, FILE *stream, int width, int suppress, char type, va_list ap) {
	char *p = suppress ? NULL : va_arg(ap, char*);
	int is_str = (type == 's'), w = is_str ? width : ((width == INT_MAX) ? 1 : width), i = 0, ch;

	if (is_str) __jacl_skip_space(in);

	while (i < w && (ch = __jacl_getch(in, stream)) != EOF && (!is_str || !__jacl_isspace(ch))) {
		if (p) p[i] = ch;

		i++;
	}

	if (p && is_str) p[i] = '\0';

	return (suppress || (is_str && !i)) ? 0 : 1;
}

static inline int __jacl_parse_scanset(const char **in, FILE *stream, const char **fmt, int width, int suppress, va_list ap) {
	char *p = suppress ? NULL : va_arg(ap, char*), charset[256] = {0};
	int i = 0, invert = (**fmt == '^' ? ((*fmt)++, 1) : 0), has_chars = 0;

	if (**fmt == ']') { charset[(unsigned char)']'] = 1; (*fmt)++; has_chars = 1; }

	while (**fmt && **fmt != ']') {
		if ((*fmt)[1] == '-' && (*fmt)[2] != ']' && (*fmt)[2] != '\0') {
			unsigned char start = (unsigned char)**fmt;
			unsigned char end = (unsigned char)(*fmt)[2];
			unsigned char c;

			for (c = start; c <= end; c++) charset[c] = 1;

			(*fmt) += 3;
		} else {
			charset[(unsigned char)**fmt] = 1;

			(*fmt)++;
		}

		has_chars = 1;
	}

	if (**fmt == ']') (*fmt)++;
	if (!has_chars) return 0;

	__jacl_skip_space(in);

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

static inline int __jacl_parse_num(const char **in, FILE *stream, uintptr_t *out, int base, int sign) {
	uintptr_t result = 0;
	int neg = 0, digits = 0, ch;

	__jacl_skip_space(in);

	ch = __jacl_getch(in, stream);

	if (sign && ch == '-') neg = 1;
	else if (ch != '+' && ch != EOF) __jacl_ungetch(ch, in, stream);

	while ((ch = __jacl_getch(in, stream)) != EOF) {
		int digit_val = -1;

		if (ch >= '0' && ch <= '9') {
			digit_val = ch - '0';

			if (digit_val >= base) digit_val = -1;
		} else if (base > 10) {
			if (ch >= 'a' && ch <= 'f') digit_val = ch - 'a' + 10;
			else if (ch >= 'A' && ch <= 'F') digit_val = ch - 'A' + 10;
		}

		if (digit_val < 0) {
			__jacl_ungetch(ch, in, stream);

			break;
		}

		result = result * base + digit_val;

		digits++;
	}

	if (digits > 0) {
		if (neg) result = (uintptr_t)(-(intptr_t)result);

		*out = result;

		return digits;
	}

	return 0;
}

static inline int __jacl_parse_ptr(const char **in, FILE *stream, int suppress, va_list ap) {
	void **out = suppress ? NULL : va_arg(ap, void**);
	uintptr_t val;

	if (__jacl_parse_num(in, stream, &val, 16, 0)) {
		if (out) *out = (void*)val;

		return suppress ? 0 : 1;
	}

	return 0;
}

static inline int __jacl_parse_int(const char **in, FILE *stream, const char *fmt_pos, int length, int suppress, va_list ap) {
	char spec = *fmt_pos;
	int is_signed = (spec == 'i' || spec == 'd'), base = 10;

	if (spec == 'i' && in && *in) {
		__jacl_skip_space(in);

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

	if (__jacl_parse_num(in, stream, &val, base, is_signed) && !suppress) {
		if (is_signed) __jacl_store_signed(length, ap, val);
		else __jacl_store_unsigned(length, ap, val);

		return 1;
	}

	return 0;
}

static inline int __jacl_parse_float(const char **in, FILE *stream, int length, int suppress, va_list ap) {
	double result = 0.0;
	int neg = 0, digits = 0, ch;

	__jacl_skip_space(in);

	ch = __jacl_getch(in, stream);

	if (ch == '-') neg = 1;
	else if (ch != '+' && ch != EOF) __jacl_ungetch(ch, in, stream);

	while ((ch = __jacl_getch(in, stream)) != EOF && ch >= '0' && ch <= '9') {
		result = result * 10.0 + (ch - '0');

		digits++;
	}

	if (ch == '.') {
		double divisor = 10.0;

		while ((ch = __jacl_getch(in, stream)) != EOF && ch >= '0' && ch <= '9') {
			result += (ch - '0') / divisor;
			divisor *= 10.0;

			digits++;
		}
	}

	if (!digits) {
		if (ch != EOF) __jacl_ungetch(ch, in, stream);

		return 0;
	}

	if (ch == 'e' || ch == 'E') {
		int exp_neg = 0, exp = 0, exp_digits = 0;

		ch = __jacl_getch(in, stream);

		if (ch == '-') exp_neg = 1;
		else if (ch != '+' && ch != EOF) __jacl_ungetch(ch, in, stream);

		while ((ch = __jacl_getch(in, stream)) != EOF && ch >= '0' && ch <= '9') {
			exp = exp * 10 + (ch - '0');
			exp_digits++;
		}

		if (ch != EOF) __jacl_ungetch(ch, in, stream);

		if (exp_digits > 0) {
			if (exp > 308) exp = 308;
			else if (exp_neg) exp = -exp;

			if (exp > 0) {
				for (int i = 0; i < exp; i++) result *= 10.0;
			} else if (exp < 0) {
				for (int i = 0; i < -exp; i++) result /= 10.0;
			}
		}
	} else if (ch != EOF) {
		__jacl_ungetch(ch, in, stream);
	}

	if (neg) result = -result;

	if (!suppress) {
		if (length == 1) *va_arg(ap, double*) = result;
		else *va_arg(ap, float*) = (float)result;
	}

	return suppress ? 0 : 1;
}

static inline int __jacl_parse(const char **input, FILE *stream, const char * restrict fmt, va_list ap) {
	const char *in = input ? *input : NULL;
	int count = 0, suppress = 0;

	while (*fmt) {
		if (*fmt != '%') {
			__jacl_skip_space(in);

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

		if ((unsigned)(*fmt - '0') <= 9u) __jacl_set_u32(width, fmt);

		int length = __jacl_mod_length(&fmt);
		char type = *fmt++;

		switch (type) {
			case 'c':
			case 's': count += __jacl_parse_string(&in, stream, width, suppress, type, ap); break;
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
			case 'X': count += __jacl_parse_int(&in, stream, fmt - 1, length, suppress, ap); break;
			case 'p': count += __jacl_parse_ptr(&in, stream, suppress, ap); break;
			case 'e':
			case 'E':
			case 'g':
			case 'G':
			case 'f': count += __jacl_parse_float(&in, stream, length, suppress, ap); break;
			case '%':
				__jacl_skip_space(in);

				if (__jacl_getch(&in, stream) == '%') count++;

				break;
		}

		suppress = 0;
	}

	if (input) *input = in;

	return count;
}

/* ============================================================= */
/* Printf Implementations                                        */
/* ============================================================= */
int vprintf(const char * restrict fmt, va_list ap) { char buf[BUFSIZ]; int len = __jacl_format(buf, BUFSIZ, fmt, ap); size_t to_write = (len >= BUFSIZ) ? BUFSIZ - 1 : (size_t)len; if (fwrite(buf, 1, to_write, stdout) != to_write) return -1; return len; }
int printf(const char* restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vprintf(fmt, ap); va_end(ap); return r; }
int vfprintf(FILE * restrict stream, const char * restrict fmt, va_list ap) { char buf[BUFSIZ]; int len = __jacl_format(buf, BUFSIZ, fmt, ap); size_t to_write = (len >= BUFSIZ) ? BUFSIZ - 1 : (size_t)len; if (fwrite(buf, 1, to_write, stream) != to_write) return -1; return len; }
int fprintf(FILE* restrict stream, const char* restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vfprintf(stream, fmt, ap); va_end(ap); return r; }
int vsprintf(char * restrict s, const char * restrict fmt, va_list ap) { return __jacl_format(s, SIZE_MAX, fmt, ap); }
int sprintf(char * restrict s, const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vsprintf(s, fmt, ap); va_end(ap); return r; }

#if JACL_HAS_C99
int vsnprintf(char * restrict s, size_t n, const char * restrict fmt, va_list ap) { return __jacl_format(s, n, fmt, ap); }
int snprintf(char * restrict s, size_t n, const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vsnprintf(s, n, fmt, ap); va_end(ap); return r; }
#endif

/* ============================================================= */
/* Scanf Implementations                                         */
/* ============================================================= */
int vscanf(const char * restrict fmt, va_list ap) { return __jacl_parse(NULL, stdin, fmt, ap); }
int scanf(const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = __jacl_parse(NULL, stdin, fmt, ap); va_end(ap); return r; }
int vfscanf(FILE * restrict stream, const char * restrict fmt, va_list ap) { return __jacl_parse(NULL, stream, fmt, ap); }
int fscanf(FILE* restrict stream, const char * restrict fmt, ...) { va_list ap; va_start(ap, fmt); int r = vfscanf(stream, fmt, ap); va_end(ap); return r; }
int vsscanf(const char * restrict s, const char* restrict fmt, va_list ap) { const char *in = s; return __jacl_parse(&in, NULL, fmt, ap); }
int sscanf(const char * restrict s, const char * restrict fmt, ...) { const char *in = s; va_list ap; va_start(ap, fmt); int r = __jacl_parse(&in, NULL, fmt, ap); va_end(ap); return r; }

#ifdef __cplusplus
}
#endif

#endif /* CORE_STDIO_H */
