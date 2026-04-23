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
    uint8_t cflags;
    uint8_t nlit;
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

/* ===================================================================== */
/* UTF-8 & Helpers (~15 lines each)                                      */
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
/* Class Management (~20 lines each)                                     */
/* ===================================================================== */
static void __jacl_add_range(regex_t *re, uint32_t lo, uint32_t hi, uint8_t cid, uint8_t neg)
{
    if (lo > hi || re->nr >= 32) return;
    for (int i = 0; i < re->nr; i++) {
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

static void __jacl_resolve_cclass(regex_t *re, const char *nm, int wide, uint8_t cid)
{
    uint32_t mx = wide ? 0xFFFF : 0xFF, i = 0;
    wctype_t w = wctype(nm); if (!w) return;
    while (i <= mx) {
        int m = wide ? iswctype(i, w) : isalpha((unsigned char)i);
        if (m) { uint32_t s = i; while (i <= mx && (wide ? iswctype(i, w) : isalpha((unsigned char)i))) i++;
                 __jacl_add_range(re, s, i-1, cid, 0); } else i++;
    }
}

static int __jacl_in_class(const regex_t *re, uint32_t cp, uint8_t cid, int ic)
{
    int hit = 0; for (int i = 0; i < re->nr; i++) if (re->rcid[i] == cid && cp >= re->rlo[i] && cp <= re->rhi[i]) hit = 1;
    if (re->nr > 0 && re->rneg[0] && re->rcid[0] == cid) hit = !hit;
    return hit || (cp < 0x80 && (isalpha(ic && cp>='A' && cp<='Z' ? cp+32 : cp) || isdigit(ic && cp>='A' && cp<='Z' ? cp+32 : cp)));
}

/* ===================================================================== */
/* AST Allocation (~10 lines each)                                       */
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

/* ===================================================================== */
/* Parser: Quantifier & Escape (~15 lines each)                          */
/* ===================================================================== */
static void __jacl_quant(__jacl_par_t *p, __jacl_node_t **n)
{
    char q = *p->p; if (!q) return;
    if (q == '*' || q == '?' || q == '+') {
        p->p++; int mn = (q=='*')?0:1, mx = (q=='?')?1:-1;
        *n = __jacl_rep(*n, mn, mx);
    } else if (q == '{') {
        p->p++; int m = 0, n = -1;
        while (isdigit(*p->p)) { m = m*10 + (*p->p-'0'); p->p++; }
        if (*p->p == ',') { p->p++; n = 0; while (isdigit(*p->p)) { n = n*10 + (*p->p-'0'); p->p++; } }
        if (*p->p != '}') { p->err = REG_EBRACE; return; } p->p++;
        *n = __jacl_rep(*n, m, n);
    }
}

static __jacl_node_t *__jacl_parse_escape(__jacl_par_t *p)
{
    p->p++; __jacl_node_t *n = 0;
    if (*p->p == 's') {
        n = __jacl_node(__jacl_CLASS);
        if (n) { uint8_t cid = p->re->nr;
            __jacl_add_range(p->re, ' ', ' ', cid, 0);
            __jacl_add_range(p->re, '\t', '\t', cid, 0);
            __jacl_add_range(p->re, '\n', '\n', cid, 0);
            __jacl_add_range(p->re, '\r', '\r', cid, 0);
            n->val = cid; }
        p->p++;
    } else if (*p->p == 'd') {
        n = __jacl_node(__jacl_CLASS);
        if (n) { uint8_t cid = p->re->nr; __jacl_add_range(p->re, '0', '9', cid, 0); n->val = cid; }
        p->p++;
    } else if (*p->p == 'w') {
        n = __jacl_node(__jacl_CLASS);
        if (n) { uint8_t cid = p->re->nr;
            __jacl_add_range(p->re, '0', '9', cid, 0);
            __jacl_add_range(p->re, 'A', 'Z', cid, 0);
            __jacl_add_range(p->re, 'a', 'z', cid, 0);
            __jacl_add_range(p->re, '_', '_', cid, 0);
            n->val = cid; }
        p->p++;
    } else {
        n = __jacl_node(__jacl_CHAR);
        if (n) n->val = __jacl_utf8_next(&p->p, p->end);
    }
    return n;
}

/* ===================================================================== */
/* Parser: Atom (~20 lines)                                              */
/* ===================================================================== */
static __jacl_node_t *__jacl_atom(__jacl_par_t *p)
{
    int ere = p->flags & REG_EXTENDED; __jacl_node_t *n = 0;
    if (*p->p == '[') {
        p->p++; int neg = (*p->p == '^'); if (neg) p->p++; uint8_t cid = p->re->nr;
        if (*p->p == ':' && ere) {
            p->p++; char nm[8] = {0}; int i = 0;
            while (*p->p && *p->p != ':') nm[i++] = *p->p++; p->p += 2;
            __jacl_resolve_cclass(p->re, nm, nm[0] == 'w', cid);
        } else {
            while (*p->p && *p->p != ']') {
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
        if (*p->p == ')') p->p++; else { p->err = REG_EPAREN; return 0; }
    } else if (*p->p == '\\') {
        if (!ere) {
            p->p++;
            if (*p->p == '(') {
                p->p++; int cap = ++p->cap; p->nsub++;
                n = __jacl_node(__jacl_CAP); if (n) { n->cap_id = cap; n->a = __jacl_expr(p); }
                if (*p->p == ')') p->p++; else { p->err = REG_EPAREN; return 0; }
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

/* ===================================================================== */
/* Parser: Factor / Term / Expr (~15 lines each)                         */
/* ===================================================================== */
static __jacl_node_t *__jacl_factor(__jacl_par_t *p) { return __jacl_atom(p); }

static __jacl_node_t *__jacl_term(__jacl_par_t *p)
{
    __jacl_node_t *a = __jacl_factor(p), *b;
    while (*p->p && *p->p != ')' && *p->p != '|' && !p->err) {
        b = __jacl_factor(p); if (!b) break;
        __jacl_node_t *seq = __jacl_node(__jacl_SEQ); if (seq) { seq->a = a; seq->b = b; } a = seq;
    }
    return a;
}

static __jacl_node_t *__jacl_expr(__jacl_par_t *p)
{
    __jacl_node_t *a = __jacl_term(p), *b;
    while (*p->p == '|' && !p->err) {
        p->p++; b = __jacl_term(p); if (!b) break;
        __jacl_node_t *alt = __jacl_node(__jacl_ALT); if (alt) { alt->a = a; alt->b = b; } a = alt;
    }
    return a;
}

/* ===================================================================== */
/* Matcher Primitives (~15 lines each)                                   */
/* ===================================================================== */
static int __jacl_match_char(__jacl_node_t *n, const char **pos, const char *end, int ic)
{
    if (*pos >= end) return 0;
    uint32_t cp = __jacl_utf8_next(pos, end);
    return __jacl_fold(cp, n->val, ic);
}

static int __jacl_match_class(__jacl_node_t *n, const regex_t *re, const char **pos, const char *end, int ic)
{
    if (*pos >= end) return 0;
    uint32_t cp = __jacl_utf8_next(pos, end);
    return __jacl_in_class(re, cp, n->val, ic);
}

static int __jacl_match_any(__jacl_node_t *n, const char **pos, const char *end, int nl)
{
    if (*pos >= end) return 0;
    uint32_t cp = __jacl_utf8_next(pos, end);
    return !nl || cp != '\n';
}

static int __jacl_match_anchor(__jacl_node_t *n, const char *pos, const char *start, const char *end, int nl, int ef)
{
    if (n->type == __jacl_BOL) {
        int at = (pos == start) || (nl && pos > start && *(pos-1) == '\n');
        return (ef & REG_NOTBOL) ? 0 : at;
    }
    int at = (pos == end) || (nl && pos < end && *pos == '\n');
    return (ef & REG_NOTEOL) ? 0 : at;
}

/* ===================================================================== */
/* Lookaround Tester (~15 lines)                                         */
/* ===================================================================== */
static int __jacl_test_assertion(__jacl_node_t *n, const regex_t *re, const char *s, const char *end,
                                 const char *pos, regoff_t *caps, int ic, int nl, int ef)
{
    const char *tmp = pos; regoff_t scap[16]; memcpy(scap, caps, 128);
    const char *r = __jacl_match(n->a, re, s, end, tmp, scap, ic, nl, ef, 0);
    int ok = (r != 0);
    if (n->type == __jacl_LA_NEG || n->type == __jacl_LB_NEG) ok = !ok;
    return ok;
}

/* ===================================================================== */
/* Recursive Matcher Core (~20 lines)                                    */
/* ===================================================================== */
static const char *__jacl_match(__jacl_node_t *n, const regex_t *re, const char *s, const char *end,
                                const char *pos, regoff_t *caps, int ic, int nl, int ef, int depth)
{
    if (depth > 1000 || !n) return (n ? 0 : pos);
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
            if (r) caps[n->cap_id*2+1] = r - s; else { caps[n->cap_id*2] = sv_so; caps[n->cap_id*2+1] = sv_eo; }
            return r ? __jacl_match(n->b, re, s, end, r, caps, ic, nl, ef, depth+1) : 0;
        }
        case __jacl_SEQ: {
            const char *r = __jacl_match(n->a, re, s, end, pos, caps, ic, nl, ef, depth+1);
            return r ? __jacl_match(n->b, re, s, end, r, caps, ic, nl, ef, depth+1) : 0;
        }
        case __jacl_ALT: {
            const char *sv = pos; regoff_t sc[16]; memcpy(sc, caps, 128);
            const char *r = __jacl_match(n->a, re, s, end, pos, caps, ic, nl, ef, depth+1);
            if (r) return __jacl_match(n->b, re, s, end, r, caps, ic, nl, ef, depth+1);
            pos = sv; memcpy(caps, sc, 128);
            r = __jacl_match(n->b, re, s, end, pos, caps, ic, nl, ef, depth+1);
            return r ? __jacl_match(n->b, re, s, end, r, caps, ic, nl, ef, depth+1) : 0;
        }
        case __jacl_REP: {
            const char *best = 0;
            for (int k = n->max; k >= n->min; k--) {
                const char *tmp = pos; regoff_t sc[16]; memcpy(sc, caps, 128);
                int ok = 1; for (int i = 0; i < k; i++) {
                    tmp = __jacl_match(n->a, re, s, end, tmp, sc, ic, nl, ef, depth+1);
                    if (!tmp) { ok = 0; break; }
                }
                if (ok) {
                    const char *r = __jacl_match(n->b, re, s, end, tmp, sc, ic, nl, ef, depth+1);
                    if (r && (!best || r > best)) best = r;
                }
            }
            return best;
        }
        case __jacl_LA_POS: case __jacl_LA_NEG: case __jacl_LB_POS: case __jacl_LB_NEG:
            return __jacl_test_assertion(n, re, s, end, pos, caps, ic, nl, ef) ? __jacl_match(n->b, re, s, end, pos, caps, ic, nl, ef, depth+1) : 0;
    }
    return 0;
}

/* ===================================================================== */
/* Compile & Exec (~15 lines each)                                       */
/* ===================================================================== */
static int __jacl_compile(regex_t *re, const char *pat, int fl)
{
    memset(re, 0, sizeof(*re)); re->cflags = fl;
    __jacl_par_t p = {pat, pat+strlen(pat), re, 0, fl, 0, 0};
    re->root = __jacl_expr(&p); re->re_nsub = p.nsub;
    re->nlit = 0; const char *pp = pat;
    while (*pp && *pp != '^' && *pp != '.' && *pp != '[' && *pp != '(' && *pp != '|' && *pp != ')' &&
           *pp != '*' && *pp != '+' && *pp != '?' && *pp != '{') {
        if (*pp == '\\' && pp[1]) { pp++; re->lit[re->nlit++] = *pp++; }
        else re->lit[re->nlit++] = *pp++;
        if (re->nlit >= 31) break;
    }
    return p.err;
}

static int __jacl_exec(const regex_t *re, const char *s, const char *end, regmatch_t *pm, size_t nm, int ef)
{
    int ic = re->cflags & REG_ICASE, nl = re->cflags & REG_NEWLINE;
    regoff_t bso = -1, beo = -1;
    for (const char *start = s; start <= end; start++) {
        regoff_t caps[16]; for (int i=0;i<16;i++) caps[i] = -1;
        const char *r = __jacl_match(re->root, re, s, end, start, caps, ic, nl, ef, 0);
        if (r) {
            regoff_t so = caps[0] >= 0 ? caps[0] : (start - s), eo = r - s;
            if (bso == -1 || so < bso || (so == bso && eo > beo)) {
                bso = so; beo = eo;
                if (pm && nm > 0) {
                    pm[0].rm_so = so; pm[0].rm_eo = eo;
                    for (size_t i = 1; i < nm; i++) { pm[i].rm_so = caps[i*2]; pm[i].rm_eo = caps[i*2+1]; }
                }
            }
        }
    }
    return bso != -1 ? 0 : REG_NOMATCH;
}

/* ===================================================================== */
/* POSIX API                                                             */
/* ===================================================================== */
int regcomp(regex_t *restrict re, const char *restrict pat, int fl)
{ return (!re || !pat) ? REG_BADPAT : __jacl_compile(re, pat, fl); }

int regexec(const regex_t *restrict re, const char *restrict s, size_t nm, regmatch_t pm[restrict], int ef)
{ if (!re || !s) return REG_BADPAT; if (re->cflags & REG_NOSUB) nm = 0; return __jacl_exec(re, s, s+strlen(s), pm, nm, ef); }

void regfree(regex_t *restrict re) { if (re) memset(re, 0, sizeof(*re)); }

size_t regerror(int ec, const regex_t *restrict re, char *restrict buf, size_t sz)
{
    static const char *m[] = {"No match","Bad pattern","Bad collation","Bad class","Bad escape","Bad sub","Bad bracket","Unmatched paren","Unmatched brace","Bad repeat","Bad range","Memory exhausted","Bad repeat"};
    const char *msg = (ec>=1&&ec<=13) ? m[ec-1] : "Unknown error"; size_t l = strlen(msg)+1;
    if (buf && sz > 0) { strncpy(buf, msg, sz-1); buf[sz-1] = '\0'; }
    return l;
}

#ifdef __cplusplus
}
#endif
#endif /* _REGEX_H */
