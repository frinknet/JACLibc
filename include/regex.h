/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _REGEX_H
#define _REGEX_H

#include <config.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <wctype.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===================================================================== */
/* POSIX Surface                                                         */
/* ===================================================================== */
typedef long regoff_t;
typedef struct { regoff_t rm_so, rm_eo; } regmatch_t;

typedef struct {
    size_t re_nsub;
    uint8_t cflags, nlit;
    char lit[32];
    void *root;
    int nr;
    uint32_t rlo[32], rhi[32];
    uint8_t rcid[32], rneg[32];
} regex_t;

#define REG_EXTENDED 0x0001
#define REG_ICASE    0x0002
#define REG_NOSUB    0x0004
#define REG_NEWLINE  0x0008
#define REG_NOTBOL   0x0100
#define REG_NOTEOL   0x0200
#define REG_NOMATCH   1
#define REG_BADPAT    2
#define REG_ECOLLATE  3
#define REG_ECTYPE    4
#define REG_EESCAPE   5
#define REG_ESUBREG   6
#define REG_EBRACK    7
#define REG_EPAREN    8
#define REG_EBRACE    9
#define REG_BADBR    10
#define REG_ERANGE   11
#define REG_ESPACE   12
#define REG_BADRPT   13

/* ===================================================================== */
/* AST Node Types                                                        */
/* ===================================================================== */
enum {
    __jacl_END, __jacl_CHAR, __jacl_CLASS, __jacl_ANY,
    __jacl_BOL, __jacl_EOL, __jacl_CAP, __jacl_ALT,
    __jacl_SEQ, __jacl_REP, __jacl_LA_POS, __jacl_LA_NEG,
    __jacl_LB_POS, __jacl_LB_NEG
};

typedef struct __jacl_node {
    uint8_t type;
    uint32_t val;
    uint8_t neg, cap_id, min, max;
    struct __jacl_node *a, *b;
} __jacl_node_t;

typedef struct {
    const char *p, *end;
    regex_t *re;
    int err, flags, cap, nsub;
} __jacl_par_t;

/* ===================================================================== */
/* Forward Declarations                                                  */
/* ===================================================================== */
static __jacl_node_t *__jacl_expr(__jacl_par_t *p);
static __jacl_node_t *__jacl_term(__jacl_par_t *p);
static __jacl_node_t *__jacl_factor(__jacl_par_t *p);
static __jacl_node_t *__jacl_atom(__jacl_par_t *p);
static const char *__jacl_match(__jacl_node_t *n, const regex_t *re,
                                const char *s, const char *end,
                                const char *pos, regoff_t *caps,
                                int icase, int nl, int ef, int depth);
static const char *__jacl_gate(const regex_t *re, const char *s, const char *end, int ic);

/* ===================================================================== */
/* UTF-8 & Helpers                                                       */
/* ===================================================================== */
static inline uint32_t __jacl_utf8_next(const char **p, const char *end)
{
    if (*p >= end) return 0xFFFD;
    uint8_t b = (uint8_t)*(*p)++; if (b < 0x80) return b;
    uint32_t cp = 0; const char *ptr = *p;
    if ((b&0xE0)==0xC0 && ptr<end) cp=((b&0x1F)<<6)|((uint8_t)*ptr++&0x3F);
    else if ((b&0xF0)==0xE0 && ptr+1<end) cp=((b&0x0F)<<12)|(((uint8_t)*ptr++&0x3F)<<6)|((uint8_t)*ptr++&0x3F);
    else if ((b&0xF8)==0xF0 && ptr+2<end) cp=((b&0x07)<<18)|(((uint8_t)*ptr++&0x3F)<<12)|(((uint8_t)*ptr++&0x3F)<<6)|((uint8_t)*ptr++&0x3F);
    else return 0xFFFD; *p = ptr; return cp;
}

static inline int __jacl_fold(uint32_t cp, uint32_t m, int ic)
{
    if (cp == m) return 1; if (!ic) return 0;
    if (cp >= 'A' && cp <= 'Z' && cp + 32 == m) return 1;
    return (m >= 'A' && m <= 'Z' && m + 32 == cp);
}

/* ===================================================================== */
/* Class Management                                                      */
/* ===================================================================== */
static void __jacl_add_range(regex_t *re, uint32_t lo, uint32_t hi, uint8_t cid, uint8_t neg)
{
    if (lo > hi || re->nr >= 32) return;
    for (int i = 0; i < re->nr; i++) {
        if (re->rcid[i] != cid) continue;
        if (lo <= re->rhi[i] + 1 && hi >= re->rlo[i] - 1) {
            lo = (lo < re->rlo[i]) ? lo : re->rlo[i];
            hi = (hi > re->rhi[i]) ? hi : re->rhi[i];
            memmove(&re->rlo[i], &re->rlo[i+1], (re->nr-1-i)*4);
            memmove(&re->rhi[i], &re->rhi[i+1], (re->nr-1-i)*4);
            memmove(&re->rcid[i], &re->rcid[i+1], re->nr-1-i);
            memmove(&re->rneg[i], &re->rneg[i+1], re->nr-1-i);
            re->nr--; i--;
        }
    }
    re->rlo[re->nr] = lo; re->rhi[re->nr] = hi;
    re->rcid[re->nr] = cid; re->rneg[re->nr] = neg; re->nr++;
}

static void __jacl_resolve_cclass(regex_t *re, const char *nm, uint8_t cid)
{
    /* FIX: Handle walpha/walnum explicitly */
    if (strcmp(nm, "walpha") == 0) {
        __jacl_add_range(re, 'a', 'z', cid, 0);
        __jacl_add_range(re, 'A', 'Z', cid, 0);
        /* Add wide alpha */
        wctype_t w = wctype("alpha");
        if (w) {
            for (uint32_t i = 256; i <= 0xFFFF; i++) {
                if (iswctype(i, w)) {
                    uint32_t s = i;
                    while (i <= 0xFFFF && iswctype(i, w)) i++;
                    __jacl_add_range(re, s, i - 1, cid, 0);
                }
            }
        }
        return;
    } else if (strcmp(nm, "walnum") == 0) {
        __jacl_add_range(re, 'a', 'z', cid, 0);
        __jacl_add_range(re, 'A', 'Z', cid, 0);
        __jacl_add_range(re, '0', '9', cid, 0);
        /* Add wide alnum */
        wctype_t w = wctype("alnum");
        if (w) {
            for (uint32_t i = 256; i <= 0xFFFF; i++) {
                if (iswctype(i, w)) {
                    uint32_t s = i;
                    while (i <= 0xFFFF && iswctype(i, w)) i++;
                    __jacl_add_range(re, s, i - 1, cid, 0);
                }
            }
        }
        return;
    }

    if (strcmp(nm, "alpha") == 0 || strcmp(nm, "lower") == 0) {
        __jacl_add_range(re, 'a', 'z', cid, 0); __jacl_add_range(re, 'A', 'Z', cid, 0);
    } else if (strcmp(nm, "upper") == 0) {
        __jacl_add_range(re, 'A', 'Z', cid, 0);
    } else if (strcmp(nm, "digit") == 0) {
        __jacl_add_range(re, '0', '9', cid, 0);
    } else if (strcmp(nm, "alnum") == 0) {
        __jacl_add_range(re, 'a', 'z', cid, 0); __jacl_add_range(re, 'A', 'Z', cid, 0);
        __jacl_add_range(re, '0', '9', cid, 0);
    } else if (strcmp(nm, "space") == 0) {
        __jacl_add_range(re, ' ', ' ', cid, 0); __jacl_add_range(re, '\t', '\t', cid, 0);
        __jacl_add_range(re, '\n', '\n', cid, 0); __jacl_add_range(re, '\r', '\r', cid, 0);
        __jacl_add_range(re, '\f', '\f', cid, 0); __jacl_add_range(re, '\v', '\v', cid, 0);
    } else if (strcmp(nm, "word") == 0) {
        __jacl_add_range(re, 'a', 'z', cid, 0); __jacl_add_range(re, 'A', 'Z', cid, 0);
        __jacl_add_range(re, '0', '9', cid, 0); __jacl_add_range(re, '_', '_', cid, 0);
    }
}

static int __jacl_in_class(const regex_t *re, uint32_t cp, uint8_t cid, int ic)
{
    int in_range = 0, is_negated = 0, found = 0;
    for (int i = 0; i < re->nr; i++) {
        if (re->rcid[i] != cid) continue;
        found = 1; if (re->rneg[i]) is_negated = 1;
        if (cp >= re->rlo[i] && cp <= re->rhi[i]) in_range = 1;
    }
    if (!found) return 0;
    if (is_negated) return !in_range;
    if (in_range) return 1;
    if (ic && cp < 0x80) {
        uint8_t f = (cp >= 'A' && cp <= 'Z') ? cp + 32 : (cp >= 'a' && cp <= 'z') ? cp - 32 : cp;
        for (int i = 0; i < re->nr; i++)
            if (re->rcid[i] == cid && f >= re->rlo[i] && f <= re->rhi[i]) return 1;
    }
    return 0;
}

/* ===================================================================== */
/* AST Allocation & Free                                                 */
/* ===================================================================== */
static __jacl_node_t *__jacl_node(uint8_t type)
{
    __jacl_node_t *n = calloc(1, sizeof(__jacl_node_t));
    if (!n) return 0;
    n->type = type; n->max = 255;
    return n;
}

static __jacl_node_t *__jacl_rep(__jacl_node_t *body, int min, int max)
{
    __jacl_node_t *n = __jacl_node(__jacl_REP);
    if (!n) return 0;
    n->a = body; n->min = min; n->max = (max < 0) ? 255 : (uint8_t)max;
    return n;
}

static void __jacl_free_ast(__jacl_node_t *n)
{
    if (!n) return;
    __jacl_free_ast(n->a); __jacl_free_ast(n->b);
    free(n);
}

/* ===================================================================== */
/* Parser                                                                */
/* ===================================================================== */
static void __jacl_quant(__jacl_par_t *p, __jacl_node_t **n)
{
    char q = *p->p; if (!q) return;
    if (q == '*' || q == '?' || q == '+') {
        p->p++; int mn = (q=='*')?0:1, mx = (q=='?')?1:-1;
        *n = __jacl_rep(*n, mn, mx);
    } else if (q == '{') {
        p->p++; int m = 0, max_val = -1;
        while (isdigit(*p->p)) { m = m*10 + (*p->p-'0'); p->p++; }
        if (*p->p == ',') { p->p++; max_val = 0; while (isdigit(*p->p)) { max_val = max_val*10 + (*p->p-'0'); p->p++; } }
        if (*p->p != '}') { p->err = REG_EBRACE; return; } p->p++;
        *n = __jacl_rep(*n, m, max_val);
    }
}

static __jacl_node_t *__jacl_parse_escape(__jacl_par_t *p)
{
    p->p++; __jacl_node_t *n = 0;
    if (*p->p == 's') {
        n = __jacl_node(__jacl_CLASS);
        if (n) { uint8_t cid = p->re->nr;
            __jacl_add_range(p->re, ' ', ' ', cid, 0); __jacl_add_range(p->re, '\t', '\t', cid, 0);
            __jacl_add_range(p->re, '\n', '\n', cid, 0); __jacl_add_range(p->re, '\r', '\r', cid, 0);
            n->val = cid; } p->p++;
    } else if (*p->p == 'd') {
        n = __jacl_node(__jacl_CLASS);
        if (n) { uint8_t cid = p->re->nr; __jacl_add_range(p->re, '0', '9', cid, 0); n->val = cid; } p->p++;
    } else if (*p->p == 'w') {
        n = __jacl_node(__jacl_CLASS);
        if (n) { uint8_t cid = p->re->nr;
            __jacl_add_range(p->re, '0', '9', cid, 0); __jacl_add_range(p->re, 'A', 'Z', cid, 0);
            __jacl_add_range(p->re, 'a', 'z', cid, 0); __jacl_add_range(p->re, '_', '_', cid, 0);
            n->val = cid; } p->p++;
    } else {
        n = __jacl_node(__jacl_CHAR);
        if (n) n->val = __jacl_utf8_next(&p->p, p->end);
    }
    return n;
}

static __jacl_node_t *__jacl_atom(__jacl_par_t *p)
{
    int ere = p->flags & REG_EXTENDED; __jacl_node_t *n = 0;
    if (*p->p == '[') {
        p->p++; int neg = (*p->p == '^'); if (neg) p->p++; uint8_t cid = p->re->nr;
        if (*p->p == '[' && p->p[1] == ':') {
            p->p += 2;
            char nm[16] = {0}; int i = 0;
            while (*p->p && *p->p != ':' && *p->p != ']') {
                if (i < 15) nm[i++] = *p->p;
                p->p++;
            }
            if (*p->p == ':' && p->p[1] == ']') {
                p->p += 2;
                __jacl_resolve_cclass(p->re, nm, cid);
            } else {
                p->err = REG_EBRACK; return 0;
            }
        } else {
            while (*p->p && *p->p != ']') {
                if (*p->p == '\\' && p->p[1]) p->p++;
                uint32_t lo = __jacl_utf8_next(&p->p, p->end), hi = lo;
                if (*p->p == '-' && *(p->p+1) != ']') { p->p++; hi = __jacl_utf8_next(&p->p, p->end); }
                __jacl_add_range(p->re, lo, hi, cid, neg);
            }
        }
        if (*p->p == ']') p->p++; else { p->err = REG_EBRACK; return 0; }
        n = __jacl_node(__jacl_CLASS); if (n) { n->val = cid; n->neg = neg; }
    } else if (*p->p == '(') {
        if (!ere) { n = __jacl_node(__jacl_CHAR); if (n) n->val = '('; p->p++; return n; }
        p->p++; int cap = ++p->cap; p->nsub++;
        n = __jacl_node(__jacl_CAP); if (n) { n->cap_id = cap; n->a = __jacl_expr(p); }
        if (!ere) {
            if (*p->p == '\\') { p->p++; if (*p->p == ')') p->p++; else { p->err = REG_EPAREN; return 0; } }
            else if (*p->p == ')') p->p++;
            else { p->err = REG_EPAREN; return 0; }
        } else {
            if (*p->p == ')') p->p++; else { p->err = REG_EPAREN; return 0; }
        }
    } else if (*p->p == '\\') {
        if (!ere) {
            p->p++;
            if (*p->p == '(') {
                p->p++; int cap = ++p->cap; p->nsub++;
                n = __jacl_node(__jacl_CAP); if (n) { n->cap_id = cap; n->a = __jacl_expr(p); }
                if (*p->p == '\\') { p->p++; if (*p->p == ')') p->p++; else { p->err = REG_EPAREN; return 0; } }
                else if (*p->p == ')') p->p++;
                else { p->err = REG_EPAREN; return 0; }
            } else if (*p->p == '|') { n = __jacl_node(__jacl_CHAR); if (n) n->val = '|'; p->p++; }
            else { p->err = REG_EESCAPE; return 0; }
        } else {
            if (*(p->p+1) == '?') {
                p->p += 2; int type = __jacl_LA_POS;
                if (*p->p == '!') { type = __jacl_LA_NEG; p->p++; }
                else if (*p->p == '<') { p->p++; type = (*p->p == '!') ? __jacl_LB_NEG : __jacl_LB_POS; if (type == __jacl_LB_POS) p->p++; }
                n = __jacl_node(type); if (n) n->a = __jacl_expr(p);
                if (*p->p == ')') p->p++; else { p->err = REG_EPAREN; return 0; }
            } else {
                n = __jacl_parse_escape(p);
            }
        }
    } else if (*p->p == '.') { n = __jacl_node(__jacl_ANY); p->p++; }
    else if (*p->p == '^') { n = __jacl_node(__jacl_BOL); p->p++; }
    else if (*p->p == '$') { n = __jacl_node(__jacl_EOL); p->p++; }
    else if (*p->p == '|' && !ere) { n = __jacl_node(__jacl_CHAR); if (n) n->val = '|'; p->p++; }
    else if (*p->p == ')' || *p->p == '\0' || *p->p == '|') { return 0; }
    else { n = __jacl_node(__jacl_CHAR); if (n) n->val = __jacl_utf8_next(&p->p, p->end); }
    if (n) __jacl_quant(p, &n);
    return n;
}

static __jacl_node_t *__jacl_factor(__jacl_par_t *p) { return __jacl_atom(p); }

static __jacl_node_t *__jacl_term(__jacl_par_t *p)
{
    int stop_at_pipe = p->flags & REG_EXTENDED;
    __jacl_node_t *a = __jacl_factor(p), *b;
    while (*p->p && *p->p != ')' && !p->err) {
        if (stop_at_pipe && *p->p == '|') break;
        if (!stop_at_pipe && *p->p == '\\' && p->p[1] == ')') break;
        if (*p->p == '*' || *p->p == '?' || *p->p == '+' || *p->p == '{') break;
        
        b = __jacl_factor(p); if (!b) break;
        __jacl_node_t *seq = __jacl_node(__jacl_SEQ); if (seq) { seq->a = a; seq->b = b; } a = seq;
    }
    return a;
}

static __jacl_node_t *__jacl_expr(__jacl_par_t *p)
{
    __jacl_node_t *a = __jacl_term(p), *b;
    if (!(p->flags & REG_EXTENDED)) return a;
    while (*p->p == '|' && !p->err) {
        p->p++; b = __jacl_term(p); if (!b) break;
        __jacl_node_t *alt = __jacl_node(__jacl_ALT);
        if (alt) { alt->a = a; alt->b = b; }
        a = alt;
    }
    return a;
}

/* ===================================================================== */
/* Matcher Primitives & Core                                             */
/* ===================================================================== */
static int __jacl_match_char(__jacl_node_t *n, const char **pos, const char *end, int ic)
{ if (*pos >= end) return 0; uint32_t cp = __jacl_utf8_next(pos, end); return __jacl_fold(cp, n->val, ic); }

static int __jacl_match_class(__jacl_node_t *n, const regex_t *re, const char **pos, const char *end, int ic)
{ if (*pos >= end) return 0; uint32_t cp = __jacl_utf8_next(pos, end); return __jacl_in_class(re, cp, n->val, ic); }

static int __jacl_match_any(__jacl_node_t *n, const char **pos, const char *end, int nl)
{ if (*pos >= end) return 0; uint32_t cp = __jacl_utf8_next(pos, end); return !nl || cp != '\n'; }

static int __jacl_match_anchor(__jacl_node_t *n, const char *pos, const char *start, const char *end, int nl, int ef)
{
    if (n->type == __jacl_BOL) {
        if (ef & REG_NOTBOL) return 0;
        if (pos == start) return 1;
        if (nl && pos > start && *(pos-1) == '\n') return 1;
        return 0;
    }
    if (n->type == __jacl_EOL) {
        if (ef & REG_NOTEOL) return 0;
        if (pos == end) return 1;
        if (nl && pos < end && *pos == '\n') return 1;
        return 0;
    }
    return 0;
}

static int __jacl_test_assertion(__jacl_node_t *n, const regex_t *re, const char *s, const char *end,
                                 const char *pos, regoff_t *caps, int ic, int nl, int ef)
{
    const char *tmp = pos; regoff_t scap[16]; memcpy(scap, caps, sizeof(scap));
    const char *r = __jacl_match(n->a, re, s, end, tmp, scap, ic, nl, ef, 0);
    int ok = (r != 0);
    if (n->type == __jacl_LA_NEG || n->type == __jacl_LB_NEG) ok = !ok;
    return ok;
}

static const char *__jacl_match(__jacl_node_t *n, const regex_t *re, const char *s, const char *end,
                                const char *pos, regoff_t *caps, int ic, int nl, int ef, int depth)
{
    if (depth > 2000) return 0;
    if (!n) return pos;
    switch (n->type) {
        case __jacl_END: return pos;
        case __jacl_CHAR: return __jacl_match_char(n, &pos, end, ic) ? __jacl_match(n->b, re, s, end, pos, caps, ic, nl, ef, depth+1) : 0;
        case __jacl_CLASS: return __jacl_match_class(n, re, &pos, end, ic) ? __jacl_match(n->b, re, s, end, pos, caps, ic, nl, ef, depth+1) : 0;
        case __jacl_ANY: return __jacl_match_any(n, &pos, end, nl) ? __jacl_match(n->b, re, s, end, pos, caps, ic, nl, ef, depth+1) : 0;
        case __jacl_BOL: case __jacl_EOL: return __jacl_match_anchor(n, pos, s, end, nl, ef) ? __jacl_match(n->b, re, s, end, pos, caps, ic, nl, ef, depth+1) : 0;
        case __jacl_CAP: {
            regoff_t sv_so = caps[n->cap_id*2], sv_eo = caps[n->cap_id*2+1];
            caps[n->cap_id*2] = pos - s;
            const char *r = __jacl_match(n->a, re, s, end, pos, caps, ic, nl, ef, depth+1);
            if (r) {
                caps[n->cap_id*2+1] = r - s;
                const char *res = __jacl_match(n->b, re, s, end, r, caps, ic, nl, ef, depth+1);
                if (!res) {
                    caps[n->cap_id*2] = sv_so;
                    caps[n->cap_id*2+1] = sv_eo;
                }
                return res;
            } else {
                caps[n->cap_id*2] = sv_so; 
                caps[n->cap_id*2+1] = sv_eo;
                return 0;
            }
        }
        case __jacl_SEQ: {
            const char *r = __jacl_match(n->a, re, s, end, pos, caps, ic, nl, ef, depth+1);
            return r ? __jacl_match(n->b, re, s, end, r, caps, ic, nl, ef, depth+1) : 0;
        }
        case __jacl_ALT: {
            regoff_t sc[16]; memcpy(sc, caps, sizeof(sc));
            const char *sv = pos;
            const char *r = __jacl_match(n->a, re, s, end, pos, caps, ic, nl, ef, depth+1);
            if (r) return r;
            pos = sv; memcpy(caps, sc, sizeof(sc));
            return __jacl_match(n->b, re, s, end, pos, caps, ic, nl, ef, depth+1);
        }
        case __jacl_REP: {
            const char *path[1024];
            regoff_t cpath[1024][16];
            int k = 0;
            
            path[0] = pos;
            memcpy(cpath[0], caps, sizeof(cpath[0]));
            
            while (k < 1023) {
                memcpy(cpath[k+1], cpath[k], sizeof(cpath[0]));
                const char *next = __jacl_match(n->a, re, s, end, path[k], cpath[k+1], ic, nl, ef, depth+1);
                if (!next || next == path[k]) break;
                path[++k] = next;
            }
            
            for (; k >= n->min; k--) {
                memcpy(caps, cpath[k], sizeof(cpath[0]));
                if (n->b) {
                    const char *res = __jacl_match(n->b, re, s, end, path[k], caps, ic, nl, ef, depth+1);
                    if (res) return res;
                } else {
                    return path[k];
                }
            }
            return 0;
        }
        case __jacl_LA_POS: case __jacl_LA_NEG: case __jacl_LB_POS: case __jacl_LB_NEG:
            return __jacl_test_assertion(n, re, s, end, pos, caps, ic, nl, ef) ? __jacl_match(n->b, re, s, end, pos, caps, ic, nl, ef, depth+1) : 0;
    }
    return 0;
}

/* ===================================================================== */
/* Gate & Compile                                                        */
/* ===================================================================== */
static const char *__jacl_gate(const regex_t *re, const char *s, const char *end, int ic) {
    if (re->nlit == 0) return s;
    for (const char *p = s; p + re->nlit <= end; p++) {
        int ok = 1;
        for (int i = 0; i < re->nlit; i++) {
            uint8_t a = ic && re->lit[i] >= 'A' && re->lit[i] <= 'Z' ? re->lit[i] + 32 : re->lit[i];
            uint8_t b = ic && p[i] >= 'A' && p[i] <= 'Z' ? p[i] + 32 : p[i];
            if (a != b) { ok = 0; break; }
        }
        if (ok) return p;
    }
    return NULL;
}

static int __jacl_compile(regex_t *re, const char *pat, int fl)
{
    memset(re, 0, sizeof(*re)); re->cflags = fl;
    __jacl_par_t p = {pat, pat+strlen(pat), re, 0, fl, 0, 0};
    re->root = __jacl_expr(&p); re->re_nsub = p.nsub; if (p.err) return p.err;
    
    if (re->root && ((__jacl_node_t*)re->root)->type == __jacl_ALT) {
        re->nlit = 0;
    } else {
        re->nlit = 0; 
        const char *pp = pat;
        if (*pp == '^') pp++;
        while (*pp && *pp != '.' && *pp != '[' && *pp != '(' && *pp != '|' && *pp != ')' &&
               *pp != '*' && *pp != '+' && *pp != '?' && *pp != '{' && *pp != '\\' && *pp != '$') {
            re->lit[re->nlit++] = *pp++;
            if (re->nlit >= 31) break;
        }
        if (re->nlit > 0) {
            char next = *pp;
            if (next == '*' || next == '?' || next == '+' || next == '{') re->nlit = 0;
        }
    }
    return 0;
}

/* ===================================================================== */
/* Exec & API                                                            */
/* ===================================================================== */
static int __jacl_exec(const regex_t *re, const char *s, const char *end, regmatch_t *pm, size_t nm, int ef)
{
    int ic = re->cflags & REG_ICASE, nl = re->cflags & REG_NEWLINE;
    
    int anchored = 0;
    if (re->root && ((__jacl_node_t*)re->root)->type == __jacl_BOL) anchored = 1;

    const char *start = __jacl_gate(re, s, end, ic);
    if (!start) return REG_NOMATCH;

    for (; start <= end; start++) {
        regoff_t caps[16]; for(int i=0;i<16;i++) caps[i] = -1;
        const char *r = __jacl_match(re->root, re, s, end, start, caps, ic, nl, ef, 0);
        if (r) {
            regoff_t so = start - s, eo = r - s;
            if (pm && nm > 0) {
                pm[0].rm_so = so; pm[0].rm_eo = eo;
                for (size_t i = 1; i < nm; i++) { pm[i].rm_so = caps[i*2]; pm[i].rm_eo = caps[i*2+1]; }
            }
            return 0;
        }
        if (anchored) return REG_NOMATCH;
        
        const char *next_gate = __jacl_gate(re, start + 1, end, ic);
        if (!next_gate) return REG_NOMATCH;
        start = next_gate - 1; 
    }
    return REG_NOMATCH;
}

int regcomp(regex_t *restrict re, const char *restrict pat, int fl)
{ return (!re||!pat) ? REG_BADPAT : __jacl_compile(re, pat, fl); }

int regexec(const regex_t *restrict re, const char *restrict s, size_t nm, regmatch_t pm[restrict], int ef)
{ if(!re||!s) return REG_BADPAT; if(re->cflags&REG_NOSUB) nm=0; return __jacl_exec(re, s, s+strlen(s), pm, nm, ef); }

void regfree(regex_t *restrict re)
{ if(re) { __jacl_free_ast(re->root); memset(re, 0, sizeof(*re)); } }

size_t regerror(int ec, const regex_t *restrict re, char *restrict buf, size_t sz)
{
    static const char *m[]={"No match","Bad pattern","Bad collation","Bad class","Bad escape","Bad sub","Bad bracket","Unmatched paren","Unmatched brace","Bad repeat","Bad range","Memory exhausted","Bad repeat"};
    const char *msg = (ec>=1&&ec<=13) ? m[ec-1] : "Unknown error"; size_t l = strlen(msg)+1;
    if(buf && sz>0) { strncpy(buf, msg, sz-1); buf[sz-1]='\0'; } return l;
}

#ifdef __cplusplus
}
#endif
#endif /* _REGEX_H */
