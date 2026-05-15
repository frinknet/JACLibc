/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _WORDEXP_H
#define _WORDEXP_H
#pragma once

#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================ */
/* 0. Public Interface                                                          */
/* ============================================================================ */

typedef struct { size_t we_wordc; char **we_wordv; size_t we_offs; } wordexp_t;

#define WRDE_APPEND  1
#define WRDE_DOOFFS  2
#define WRDE_NOCMD   4
#define WRDE_REUSE   8
#define WRDE_SHOWERR 16
#define WRDE_UNDEF   32

#define WRDE_BADCHAR 1
#define WRDE_BADVAL  2
#define WRDE_CMDSUB  3
#define WRDE_NOSPACE 4
#define WRDE_SYNTAX  5

/* ============================================================================ */
/* 1. Dynamic Buffer                                                            */
/* ============================================================================ */

typedef struct { char *data; size_t len; size_t cap; } __we_buf_t;

static inline int __we_buf_init(__we_buf_t *b) {
    b->cap = 64; b->len = 0;
    b->data = malloc(b->cap);
    return b->data ? 0 : WRDE_NOSPACE;
}

static inline int __we_buf_put(__we_buf_t *b, char c) {
    if (b->len + 2 > b->cap) {
        b->cap = (b->len + 2) * 2;
        char *t = realloc(b->data, b->cap);
        if (!t) return WRDE_NOSPACE;
        b->data = t;
    }
    b->data[b->len++] = c;
    return 0;
}

static inline int __we_buf_str(__we_buf_t *b, const char *s, size_t n) {
    for (size_t i = 0; i < n; i++) { int r = __we_buf_put(b, s[i]); if (r) return r; }
    return 0;
}

static inline void __we_buf_free(__we_buf_t *b) {
    free(b->data); b->data = NULL; b->len = b->cap = 0;
}

/* ============================================================================ */
/* 2. Expansion Handlers                                                        */
/* ============================================================================ */

static inline int __we_exec_cmd(const char *cmd, __we_buf_t *buf) {
    if (!cmd || !*cmd) return 0;
    FILE *fp = popen(cmd, "r");
    if (!fp) return 0;
    char out[4096];
    while (fgets(out, sizeof(out), fp)) {
        size_t ol = strlen(out);
        if (ol > 0 && out[ol-1] == '\n') out[ol-1] = '\0';
        int r = __we_buf_str(buf, out, strlen(out));
        if (r) { pclose(fp); return r; }
    }
    pclose(fp);
    return 0;
}

static inline int __we_scan_tilde(const char **p, __we_buf_t *buf) {
    (*p)++;
    const char *home = getenv("HOME");
    return home ? __we_buf_str(buf, home, strlen(home)) : __we_buf_put(buf, '~');
}

static inline int __we_scan_var(const char **p, __we_buf_t *buf, int flags) {
    (*p)++;
    char var[64] = {0}; size_t vi = 0;
    while (**p && ((**p >= 'A' && **p <= 'Z') || (**p >= 'a' && **p <= 'z') || 
                   (**p >= '0' && **p <= '9') || **p == '_') && vi < 63) {
        var[vi++] = **p; (*p)++;
    }
    if (vi == 0) return __we_buf_put(buf, '$');
    const char *val = getenv(var);
    if (val) return __we_buf_str(buf, val, strlen(val));
    return (flags & WRDE_UNDEF) ? WRDE_BADVAL : 0;
}

static inline int __we_scan_cmd(const char **p, __we_buf_t *buf, int flags) {
    if (flags & WRDE_NOCMD) return WRDE_CMDSUB;
    int paren = (**p == '$' && *(*p + 1) == '(');
    (*p) += paren ? 2 : 1;

    const char *start = *p;
    int depth = 1, q = 0;
    while (**p && depth > 0) {
        if (q) { if (**p == q) q = 0; }
        else {
            if (**p == '\'' || **p == '"') q = **p;
            else if (paren) { if (**p == '(') depth++; if (**p == ')') depth--; }
            else if (**p == '`') depth--;
        }
        if (depth) (*p)++;
    }
    if (!**p) return WRDE_SYNTAX;

    size_t cmd_len = *p - start;
    char *cmd = malloc(cmd_len + 1);
    if (!cmd) return WRDE_NOSPACE;
    memcpy(cmd, start, cmd_len); cmd[cmd_len] = '\0';
    (*p)++;

    int ret = __we_exec_cmd(cmd, buf);
    free(cmd);
    return ret;
}

/* ============================================================================ */
/* 3. Word Parser                                                               */
/* ============================================================================ */

static inline int __we_parse_word(const char **p, char **out, int flags) {
    __we_buf_t buf;
    if (__we_buf_init(&buf)) return WRDE_NOSPACE;

    int q = 0;
    while (**p) {
        char c = **p;
        if (q) {
            if (c == q) { q = 0; (*p)++; continue; }
            if (q == '"' && c == '$') {
                (*p)++;
                int r = (**p == '(') ? __we_scan_cmd(p, &buf, flags)
                                     : __we_scan_var(p, &buf, flags);
                if (r) { __we_buf_free(&buf); return r; }
                continue;
            }
            if (__we_buf_put(&buf, c)) { __we_buf_free(&buf); return WRDE_NOSPACE; }
            (*p)++; continue;
        }

        if (c == ' ' || c == '\t') break;
        if (c == '\\') {
            (*p)++; if (!**p) { __we_buf_free(&buf); return WRDE_SYNTAX; }
            if (__we_buf_put(&buf, **p)) { __we_buf_free(&buf); return WRDE_NOSPACE; }
            (*p)++; continue;
        }
        if (c == '\'' || c == '"') { q = c; (*p)++; continue; }

        switch (c) {
            case '|': case '&': case ';': case '<': case '>':
            case '#': case '\n': case '(': case ')': case '{': case '}':
                __we_buf_free(&buf); return WRDE_BADCHAR;
        }

        if (c == '~' && buf.len == 0) {
            int r = __we_scan_tilde(p, &buf); if (r) { __we_buf_free(&buf); return r; } continue;
        }
        if (c == '`' || (c == '$' && *(*p + 1) == '(')) {
            int r = __we_scan_cmd(p, &buf, flags); if (r) { __we_buf_free(&buf); return r; } continue;
        }
        if (c == '$') {
            int r = __we_scan_var(p, &buf, flags); if (r) { __we_buf_free(&buf); return r; } continue;
        }

        if (__we_buf_put(&buf, c)) { __we_buf_free(&buf); return WRDE_NOSPACE; }
        (*p)++;
    }

    if (q) { __we_buf_free(&buf); return WRDE_SYNTAX; }
    if (__we_buf_put(&buf, '\0')) { __we_buf_free(&buf); return WRDE_NOSPACE; }
    *out = buf.data;
    return 0;
}

/* ============================================================================ */
/* 4. Vector & Public API                                                       */
/* ============================================================================ */

static inline void wordfree(wordexp_t *pwordexp) {
    if (!pwordexp || !pwordexp->we_wordv) return;
    for (size_t i = 0; i < pwordexp->we_wordc; i++) free(pwordexp->we_wordv[i]);
    free(pwordexp->we_wordv);
    pwordexp->we_wordc = 0;
    pwordexp->we_wordv = NULL;
}

static inline int wordexp(const char *restrict words, wordexp_t *restrict pwordexp, int flags) {
    if (!words || !pwordexp) return WRDE_SYNTAX;
    if (flags & WRDE_REUSE) wordfree(pwordexp);

    size_t off = (flags & WRDE_DOOFFS) ? pwordexp->we_offs : 0;
    /* FIX: Reserve offset space first. If appending, start after existing words. */
    size_t idx = off;
    if (flags & WRDE_APPEND && pwordexp->we_wordc > off) {
        idx = pwordexp->we_wordc;
    }

    size_t cap = idx + 16;
    char **vec = realloc(pwordexp->we_wordv, cap * sizeof(char *));
    if (!vec) return WRDE_NOSPACE;
    memset(vec + idx, 0, (cap - idx) * sizeof(char *));
    pwordexp->we_wordv = vec;

    const char *p = words;
    while (*p) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;

        char *w = NULL;
        int r = __we_parse_word(&p, &w, flags);
        if (r) return r;

        if (idx + 2 >= cap) {
            cap *= 2;
            char *t = realloc(vec, cap * sizeof(char *));
            if (!t) { free(w); return WRDE_NOSPACE; }
            vec = t;
            memset(vec + idx, 0, (cap - idx) * sizeof(char *));
            pwordexp->we_wordv = vec;
        }
        vec[idx++] = w;
        vec[idx] = NULL;
    }

    pwordexp->we_wordc = idx;
    return 0;
}

#ifdef __cplusplus
}
#endif
#endif /* _WORDEXP_H */
