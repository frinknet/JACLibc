/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _ICONV_H
#define _ICONV_H
#pragma once

#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================ */
/* Public Types                                                                 */
/* ============================================================================ */

struct iconv_t {
    int from; /* 0:ASCII, 1:UTF-8, 2:UTF-16LE, 3:UTF-32LE */
    int to;
    unsigned int state; /* UTF-8 decoder state */
};

typedef struct iconv_t *iconv_t;
#define ICONV_CONST const

/* ============================================================================ */
/* Encoding Dispatchers                                                         */
/* ============================================================================ */

static inline int __iconv_codepoint_from(int enc, const char **in, size_t *inlen, unsigned int *cp, unsigned int *state) {
    if (*inlen == 0) return 0;

    switch (enc) {
        case 0: /* ASCII */
            *cp = (unsigned char)(**in);
            (*in)++; (*inlen)--; return 1;
        case 1: /* UTF-8 */
            if (state && *state) { /* Continuation byte expected */
                if ((**in & 0xC0) != 0x80) { *state = 0; errno = EILSEQ; return -1; }
                *cp = (*cp << 6) | (**in & 0x3F);
                (*in)++; (*inlen)--;
                if (!state || --*state == 0) return 1;
                return 0;
            }
            unsigned char b = (unsigned char)(**in);
            if (b <= 0x7F) { *cp = b; (*in)++; (*inlen)--; return 1; }
            if ((b & 0xE0) == 0xC0) { *cp = b & 0x1F; if(state) *state = 1; (*in)++; (*inlen)--; return 0; }
            if ((b & 0xF0) == 0xE0) { *cp = b & 0x0F; if(state) *state = 2; (*in)++; (*inlen)--; return 0; }
            if ((b & 0xF8) == 0xF0) { *cp = b & 0x07; if(state) *state = 3; (*in)++; (*inlen)--; return 0; }
            errno = EILSEQ; return -1;
        case 2: /* UTF-16LE */
            if (*inlen < 2) return 0;
            unsigned int u = (unsigned char)(*in)[0] | ((unsigned char)(*in)[1] << 8);
            
            /* High surrogate? */
            if (u >= 0xD800 && u <= 0xDBFF) {
                if (*inlen < 4) return 0; /* Need full pair */
                unsigned int l = (unsigned char)(*in)[2] | ((unsigned char)(*in)[3] << 8);
                if (l < 0xDC00 || l > 0xDFFF) { errno = EILSEQ; return -1; } /* Bad low surrogate */
                (*in) += 4; (*inlen) -= 4;
                *cp = 0x10000 + ((u - 0xD800) << 10) + (l - 0xDC00);
                return 1;
            }
            /* Stray low surrogate? */
            if (u >= 0xDC00 && u <= 0xDFFF) { errno = EILSEQ; return -1; }
            
            (*in) += 2; (*inlen) -= 2;
            *cp = u;
            return 1;
        case 3: /* UTF-32LE */
            if (*inlen < 4) return 0;
            *cp = (unsigned char)(*in)[0] | ((unsigned char)(*in)[1] << 8) |
                  ((unsigned char)(*in)[2] << 16) | ((unsigned char)(*in)[3] << 24);
            (*in) += 4; (*inlen) -= 4; return 1;
    }
    errno = EINVAL; return -1;
}

static inline int __iconv_codepoint_to(int enc, char **out, size_t *outlen, unsigned int cp) {
    size_t need = 0;
    switch (enc) {
        case 0: need = 1; if (*outlen < 1) return 0; (*out)[0] = (char)cp; break;
        case 1: /* UTF-8 */
            if (cp <= 0x7F) need = 1; else if (cp <= 0x7FF) need = 2; else if (cp <= 0xFFFF) need = 3; else if (cp <= 0x10FFFF) need = 4; else { errno = EILSEQ; return -1; }
            if (*outlen < need) return 0;
            if (need == 1) { (*out)[0] = cp; }
            else if (need == 2) { (*out)[0] = 0xC0 | (cp >> 6); (*out)[1] = 0x80 | (cp & 0x3F); }
            else if (need == 3) { (*out)[0] = 0xE0 | (cp >> 12); (*out)[1] = 0x80 | ((cp >> 6) & 0x3F); (*out)[2] = 0x80 | (cp & 0x3F); }
            else { (*out)[0] = 0xF0 | (cp >> 18); (*out)[1] = 0x80 | ((cp >> 12) & 0x3F); (*out)[2] = 0x80 | ((cp >> 6) & 0x3F); (*out)[3] = 0x80 | (cp & 0x3F); }
            break;
        case 2: /* UTF-16LE */
            if (cp >= 0x10000 && cp <= 0x10FFFF) {
                need = 4; if (*outlen < 4) return 0;
                unsigned int c = cp - 0x10000;
                unsigned int hi = 0xD800 | (c >> 10);
                unsigned int lo = 0xDC00 | (c & 0x3FF);
                (*out)[0] = hi & 0xFF; (*out)[1] = hi >> 8;
                (*out)[2] = lo & 0xFF; (*out)[3] = lo >> 8;
            } else {
                if (cp > 0xFFFF) { errno = EILSEQ; return -1; }
                need = 2; if (*outlen < 2) return 0;
                (*out)[0] = cp & 0xFF; (*out)[1] = cp >> 8;
            }
            break;
        case 3: /* UTF-32LE */
            need = 4; if (*outlen < 4) return 0;
            (*out)[0] = cp & 0xFF; (*out)[1] = (cp >> 8) & 0xFF; (*out)[2] = (cp >> 16) & 0xFF; (*out)[3] = (cp >> 24) & 0xFF; break;
    }
    *out += need; *outlen -= need; return 1;
}

/* ============================================================================ */
/* Public API                                                                   */
/* ============================================================================ */

static inline iconv_t iconv_open(const char *tocode, const char *fromcode) {
	if (!tocode) tocode = "UTF-8";
	if (!fromcode) fromcode = "UTF-8";

	iconv_t h = malloc(sizeof(struct iconv_t));
	if (!h) return (iconv_t)-1;

	h->from = (!strcmp(fromcode, "UTF-8") || !strcmp(fromcode, "utf-8")) ? 1 :
	          (!strcmp(fromcode, "UTF-16LE") || !strcmp(fromcode, "utf-16le")) ? 2 :
	          (!strcmp(fromcode, "UTF-32LE") || !strcmp(fromcode, "utf-32le")) ? 3 : 0;
	h->to   = (!strcmp(tocode, "UTF-8") || !strcmp(tocode, "utf-8")) ? 1 :
	          (!strcmp(tocode, "UTF-16LE") || !strcmp(tocode, "utf-16le")) ? 2 :
	          (!strcmp(tocode, "UTF-32LE") || !strcmp(tocode, "utf-32le")) ? 3 : 0;
	h->state = 0;
	return (iconv_t)h;
}

static inline size_t iconv(iconv_t cd, char **restrict inbuf, size_t *restrict inbytesleft, char **restrict outbuf, size_t *restrict outbytesleft) {
	if (!cd || cd == (iconv_t)-1) { errno = EBADF; return (size_t)-1; }
	if (!inbuf || !inbytesleft || !outbuf || !outbytesleft) { errno = EINVAL; return (size_t)-1; }

	iconv_t h = cd;

	size_t converted = 0;
	unsigned int cp = 0;

	while (*inbytesleft > 0) {
		int r = __iconv_codepoint_from(h->from, (const char **)inbuf, inbytesleft, &cp, &h->state);

		if (r == 0) continue;
		if (r < 0) return (size_t)-1;

		size_t out_avail = *outbytesleft;
		char *out_start = *outbuf;

		r = __iconv_codepoint_to(h->to, &out_start, &out_avail, cp);

		if (r == 0) { errno = E2BIG; break; }
		if (r < 0) return (size_t)-1;

		*outbuf = out_start;
		*outbytesleft = out_avail;
		converted++;
	}

	return converted;
}

static inline int iconv_close(iconv_t cd) {
    if (!cd || cd == (iconv_t)-1) { errno = EBADF; return -1; }
    free(cd);
    return 0;
}

#ifdef __cplusplus
}
#endif
#endif /* _ICONV_H */
