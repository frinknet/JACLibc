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
    int nins;
    uint8_t  op[1024];
    uint16_t a[1024];
    uint16_t b[1024];
    int nr;
    uint32_t rlo[32], rhi[32];
    uint8_t  rcid[32], rneg[32];
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

/* Opcodes */
enum { __jacl_CHAR, __jacl_CLASS, __jacl_ANY, __jacl_BOL, __jacl_EOL,
       __jacl_SAVE, __jacl_SPLIT, __jacl_JUMP, __jacl_MATCH,
       __jacl_LA_POS, __jacl_LA_NEG, __jacl_LB_POS, __jacl_LB_NEG };

typedef struct { const char *p, *end; regex_t *re; int err, flags, cap, nsub; } __jacl_par_t;
typedef struct { int pc; const char *pos; regoff_t caps[16]; } __jacl_thread_t;

/* ===================================================================== */
/* Forward Declarations                                                  */
/* ===================================================================== */
static int  __jacl_emit(regex_t *re, int op, uint16_t a, uint16_t b);
static void __jacl_patch(regex_t *re, int pc, int a, int b);
static void __jacl_expr(__jacl_par_t *p, int *start, int *end);
static void __jacl_term(__jacl_par_t *p, int *start, int *end);
static void __jacl_factor(__jacl_par_t *p, int *start, int *end);
static void __jacl_atom(__jacl_par_t *p, int *start, int *end);
static int  __jacl_test_assertion(const regex_t *re, const char *s, const char *end,
                                  const char *pos, int target, int type,
                                  int icase, int nl, int ef);
static int  __jacl_run(const regex_t *re, const char *s, const char *end,
                       __jacl_thread_t *th, int icase, int nl, int ef);

/* ===================================================================== */
/* Bytecode Emitter (~10 lines each)                                     */
/* ===================================================================== */
static int __jacl_emit(regex_t *re, int op, uint16_t a, uint16_t b)
{
    if (re->nins >= 1024) return -1;
    re->op[re->nins] = op;
    re->a[re->nins] = a;
    re->b[re->nins] = b;
    return re->nins++;
}

static void __jacl_patch(regex_t *re, int pc, int a, int b)
{
    if (pc >= 0 && pc < re->nins) { re->a[pc] = a; re->b[pc] = b; }
}

/* ===================================================================== */
/* Range Management (~15 lines each)                                     */
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

/* ===================================================================== */
/* UTF-8 & Case Fold (~15 lines each)                                    */
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

static int __jacl_in_class(const regex_t *re, uint32_t cp, uint8_t cid, int ic)
{
    int hit = 0; for (int i = 0; i < re->nr; i++) if (re->rcid[i] == cid && cp >= re->rlo[i] && cp <= re->rhi[i]) hit = 1;
    if (re->nr > 0 && re->rneg[0] && re->rcid[0] == cid) hit = !hit;
    return hit || (cp < 0x80 && (isalpha(ic && cp>='A' && cp<='Z' ? cp+32 : cp) || isdigit(ic && cp>='A' && cp<='Z' ? cp+32 : cp)));
}

/* ===================================================================== */
/* Parser: Quantifier & Atom (~20 lines each)                            */
/* ===================================================================== */
static void __jacl_quant(__jacl_par_t *p, int start, int end)
{
    char q = *p->p; if (!q) return;
    if (q == '*' || q == '?' || q == '+') {
        p->p++; int mn = (q=='*')?0:1, mx = (q=='?')?1:-1;
        int split = __jacl_emit(p->re, __jacl_SPLIT, 0, 0);
        int jump  = __jacl_emit(p->re, __jacl_JUMP, 0, 0);
        __jacl_patch(p->re, split, end, jump+1);
        __jacl_emit(p->re, __jacl_JUMP, 0, 0);
        __jacl_patch(p->re, p->re->nins-1, split, 0);
        __jacl_patch(p->re, jump, p->re->nins, 0);
    } else if (q == '{') {
        p->p++; int m = 0, n = -1;
        while (isdigit(*p->p)) { m = m*10 + (*p->p-'0'); p->p++; }
        if (*p->p == ',') { p->p++; n = 0; while (isdigit(*p->p)) { n = n*10 + (*p->p-'0'); p->p++; } }
        if (*p->p != '}') { p->err = REG_EBRACE; return; } p->p++;
        if (m > 1) { int len = end-start; for (int k=1; k<m; k++) for (int i=start; i<end; i++) __jacl_emit(p->re, p->re->op[i], p->re->a[i], p->re->b[i]); }
        if (n != m) { int split = __jacl_emit(p->re, __jacl_SPLIT, 0, 0); int jump = __jacl_emit(p->re, __jacl_JUMP, 0, 0);
                      __jacl_patch(p->re, split, p->re->nins, jump+1); __jacl_emit(p->re, __jacl_JUMP, 0, 0); __jacl_patch(p->re, p->re->nins-1, split, 0);
                      __jacl_patch(p->re, jump, p->re->nins, 0); }
    }
}

static void __jacl_atom(__jacl_par_t *p, int *start, int *end)
{
    *start = p->re->nins; int ere = p->flags & REG_EXTENDED;
    if (*p->p == '[') {
        p->p++; int neg = (*p->p=='^'); if(neg) p->p++; uint8_t cid = p->re->nr;
        if (*p->p==':' && ere) { p->p++; char nm[8]={0}; int i=0; while(*p->p && *p->p!=':') nm[i++]=*p->p++; p->p+=2; __jacl_resolve_cclass(p->re, nm, nm[0]=='w', cid); }
        else { while(*p->p && *p->p!=']') { uint32_t lo = __jacl_utf8_next(&p->p, p->end), hi = lo;
              if(*p->p=='-' && *(p->p+1)!=']') { p->p++; hi = __jacl_utf8_next(&p->p, p->end); } __jacl_add_range(p->re, lo, hi, cid, neg); } }
        if (*p->p==']') p->p++; else { p->err = REG_EBRACK; return; }
        __jacl_emit(p->re, __jacl_CLASS, cid, 0);
    } else if (*p->p == '(') {
        if (!ere) { __jacl_emit(p->re, __jacl_CHAR, '(', 0); p->p++; *end = p->re->nins; return; }
        p->p++; int cap = ++p->cap; p->nsub++; __jacl_emit(p->re, __jacl_SAVE, cap*2, 0);
        __jacl_expr(p, start, end); __jacl_emit(p->re, __jacl_SAVE, cap*2+1, 0);
        if (*p->p==')') p->p++; else { p->err = REG_EPAREN; return; }
    } else if (*p->p == '\\') {
        p->p++;
        if (!ere && *p->p == '(') { p->p++; int cap = ++p->cap; p->nsub++; __jacl_emit(p->re, __jacl_SAVE, cap*2, 0);
                                    __jacl_expr(p, start, end); __jacl_emit(p->re, __jacl_SAVE, cap*2+1, 0);
                                    if (*p->p==')') p->p++; else { p->err = REG_EPAREN; return; } }
        else if (!ere && *p->p == '|') { __jacl_emit(p->re, __jacl_CHAR, '|', 0); p->p++; }
        else if (*p->p == 's') { __jacl_emit(p->re, __jacl_ANY, 1, 0); p->p++; }
        else if (*p->p == 'd') { __jacl_emit(p->re, __jacl_CLASS, 0, 1); p->p++; }
        else if (*p->p == 'w') { __jacl_emit(p->re, __jacl_CLASS, 0, 2); p->p++; }
        else if (*p->p == '(') { /* lookahead/lookbehind */
            int type = __jacl_LA_POS;
            if (*++p->p == '!') { type = __jacl_LA_NEG; p->p++; }
            else if (*p->p == '<') { p->p++; type = (*p->p=='!') ? __jacl_LB_NEG : __jacl_LB_POS; if(type>=__jacl_LB_POS) p->p++; }
            int sub_start, sub_end; __jacl_expr(p, &sub_start, &sub_end);
            __jacl_emit(p->re, type, sub_start, sub_end);
            if (*p->p==')') p->p++; else { p->err = REG_EPAREN; return; }
        }
        else { __jacl_emit(p->re, __jacl_CHAR, __jacl_utf8_next(&p->p, p->end), 0); }
    } else if (*p->p == '.') { __jacl_emit(p->re, __jacl_ANY, 0, 0); p->p++; }
    else if (*p->p == '^') { __jacl_emit(p->re, __jacl_BOL, 0, 0); p->p++; }
    else if (*p->p == '$') { __jacl_emit(p->re, __jacl_EOL, 0, 0); p->p++; }
    else if (*p->p == '|' && !ere) { __jacl_emit(p->re, __jacl_CHAR, '|', 0); p->p++; }
    else if (*p->p == ')' || *p->p == '\0' || *p->p == '|') { *end = p->re->nins; return; }
    else { __jacl_emit(p->re, __jacl_CHAR, __jacl_utf8_next(&p->p, p->end), 0); }
    __jacl_quant(p, *start, p->re->nins); *end = p->re->nins;
}

/* ===================================================================== */
/* Parser: Factor / Term / Expr (~15 lines each)                         */
/* ===================================================================== */
static void __jacl_factor(__jacl_par_t *p, int *start, int *end) { __jacl_atom(p, start, end); }

static void __jacl_term(__jacl_par_t *p, int *start, int *end)
{
    __jacl_factor(p, start, end);
    while (*p->p && *p->p != ')' && *p->p != '|' && !p->err) {
        int nstart, nend; __jacl_factor(p, &nstart, &nend);
        __jacl_emit(p->re, __jacl_JUMP, nstart, 0); *end = p->re->nins;
    }
}

static void __jacl_expr(__jacl_par_t *p, int *start, int *end)
{
    __jacl_term(p, start, end);
    while (*p->p == '|' && !p->err) {
        p->p++; int split = __jacl_emit(p->re, __jacl_SPLIT, 0, 0), jump = __jacl_emit(p->re, __jacl_JUMP, 0, 0);
        int tstart, tend; __jacl_term(p, &tstart, &tend);
        __jacl_patch(p->re, split, *start, jump+1);
        __jacl_patch(p->re, jump, p->re->nins, 0);
        *start = split; *end = p->re->nins;
    }
}

/* ===================================================================== */
/* Lookaround Assertion Tester (~20 lines)                               */
/* ===================================================================== */
static int __jacl_test_assertion(const regex_t *re, const char *s, const char *end,
                                 const char *pos, int target, int type,
                                 int icase, int nl, int ef)
{
    __jacl_thread_t th = {target, pos, {0}};
    for(int i=0;i<16;i++) th.caps[i]=-1;
    int r = __jacl_run(re, s, end, &th, icase, nl, ef);
    return (type==__jacl_LA_POS||type==__jacl_LB_POS) ? r : !r;
}

/* ===================================================================== */
/* NFA Simulator (~20 lines)                                             */
/* ===================================================================== */
static int __jacl_step(const regex_t *re, const char *s, const char *end, __jacl_thread_t *th, int icase, int nl, int ef)
{
    if (th->pos > end) return 0;
    switch (re->op[th->pc]) {
        case __jacl_CHAR: if (th->pos < end && __jacl_fold(__jacl_utf8_next(&th->pos, end), re->a[th->pc], icase)) { th->pc++; return 1; } return 0;
        case __jacl_CLASS: if (th->pos < end && __jacl_in_class(re, __jacl_utf8_next(&th->pos, end), re->a[th->pc], icase)) { th->pc++; return 1; } return 0;
        case __jacl_ANY: { if (th->pos >= end) return 0; uint32_t cp = __jacl_utf8_next(&th->pos, end); if (re->a[th->pc]) return (cp==' '||cp=='\t'||cp=='\n'||cp=='\r'); if (nl && cp=='\n') return 0; th->pc++; return 1; }
        case __jacl_BOL: { int m = (th->pos==s)||(nl&&th->pos>s&&*(th->pos-1)=='\n'); if(ef&REG_NOTBOL)m=0; if(m){th->pc++;return 1;} return 0; }
        case __jacl_EOL: { int m = (th->pos==end)||(nl&&th->pos<end&&*th->pos=='\n'); if(ef&REG_NOTEOL)m=0; if(m){th->pc++;return 1;} return 0; }
        case __jacl_SAVE: th->caps[re->a[th->pc]] = th->pos - s; th->pc++; return 1;
        case __jacl_LA_POS: case __jacl_LA_NEG: case __jacl_LB_POS: case __jacl_LB_NEG:
            if (__jacl_test_assertion(re, s, end, th->pos, re->a[th->pc], re->op[th->pc], icase, nl, ef)) { th->pc++; return 1; } return 0;
        case __jacl_MATCH: return 2;
        default: return 0;
    }
}

static int __jacl_run(const regex_t *re, const char *s, const char *end, __jacl_thread_t *th, int icase, int nl, int ef)
{
    __jacl_thread_t pool[128]; int np = 1; pool[0] = *th;
    int steps = 0;
    while (np > 0 && steps++ < 10000) {
        int nn = 0;
        for (int i = 0; i < np; i++) {
            int r = __jacl_step(re, s, end, &pool[i], icase, nl, ef);
            if (r == 2) { th->pos = pool[i].pos; memcpy(th->caps, pool[i].caps, 128); return 1; }
            if (r == 1) {
                if (nn < 128) pool[nn++] = pool[i];
            } else if (re->op[pool[i].pc] == __jacl_SPLIT) {
                if (nn+1 < 128) { pool[nn] = pool[i]; pool[nn].pc = re->b[pool[i].pc]; nn++; pool[i].pc = re->a[pool[i].pc]; }
                if (nn < 128) pool[nn++] = pool[i];
            }
        }
        np = nn;
    }
    return 0;
}

/* ===================================================================== */
/* Compile & Exec (~15 lines each)                                       */
/* ===================================================================== */
static int __jacl_compile(regex_t *re, const char *pat, int fl)
{
    memset(re, 0, sizeof(*re)); re->cflags = fl; __jacl_par_t p = {pat, pat+strlen(pat), re, 0, fl, 0, 0};
    int s, e; __jacl_expr(&p, &s, &e); __jacl_emit(re, __jacl_MATCH, 0, 0);
    re->re_nsub = p.nsub; re->nlit = 0; return p.err;
}

static int __jacl_exec(const regex_t *re, const char *s, const char *end, regmatch_t *pm, size_t nm, int ef)
{
    int ic = re->cflags & REG_ICASE, nl = re->cflags & REG_NEWLINE;
    regoff_t bso = -1, beo = -1;
    for (const char *start = s; start <= end; start++) {
        __jacl_thread_t th = {0, start, {0}}; for(int i=0;i<16;i++) th.caps[i]=-1;
        if (__jacl_run(re, s, end, &th, ic, nl, ef)) {
            regoff_t so = th.caps[0]>=0?th.caps[0]:(start-s), eo = th.pos-s;
            if (bso==-1||so<bso||(so==bso&&eo>beo)) {
                bso = so; beo = eo;
                if (pm && nm>0) { pm[0].rm_so = so; pm[0].rm_eo = eo;
                    for(size_t i=1;i<nm;i++){pm[i].rm_so=th.caps[i*2];pm[i].rm_eo=th.caps[i*2+1];} }
            }
        }
    }
    return bso != -1 ? 0 : REG_NOMATCH;
}

/* ===================================================================== */
/* POSIX API                                                             */
/* ===================================================================== */
int regcomp(regex_t *restrict re, const char *restrict pat, int fl) { return (!re||!pat)?REG_BADPAT:__jacl_compile(re,pat,fl); }
int regexec(const regex_t *restrict re, const char *restrict s, size_t nm, regmatch_t pm[restrict], int ef) {
    if (!re||!s) return REG_BADPAT; if (re->cflags&REG_NOSUB) nm=0; return __jacl_exec(re, s, s+strlen(s), pm, nm, ef); }
void regfree(regex_t *restrict re) { if (re) memset(re, 0, sizeof(*re)); }
size_t regerror(int ec, const regex_t *restrict re, char *restrict buf, size_t sz) {
    static const char *m[]={"No match","Bad pattern","Bad collation","Bad class","Bad escape","Bad sub","Bad bracket","Unmatched paren","Unmatched brace","Bad repeat","Bad range","Memory exhausted","Bad repeat"};
    const char *msg=(ec>=1&&ec<=13)?m[ec-1]:"Unknown error"; size_t l=strlen(msg)+1;
    if(buf&&sz>0){strncpy(buf,msg,sz-1);buf[sz-1]='\0';} return l;}

#ifdef __cplusplus
}
#endif
#endif /* _REGEX_H */
