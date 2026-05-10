/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _CORE__MATCH_H
#define _CORE__MATCH_H

#include <config.h>
#include <wctype.h>

#ifndef MATCH_MAX_GROUPS
#define MATCH_MAX_GROUPS 32
#endif

#ifndef MATCH_MAX_DEPTH
#define MATCH_MAX_DEPTH 2000
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef long matchoff_t;
typedef struct matcher matcher_t;

typedef enum {
	MTOK_END, MTOK_CHAR, MTOK_CLASS, MTOK_ANY, MTOK_BOL,
	MTOK_EOL, MTOK_CAP, MTOK_ALT, MTOK_SEQ, MTOK_REP,
	MTOK_LOOK, MTOK_BACKREF, MTOK_WB, MTOK_NWB, MTOK_FLAGS,
	MTOK_ATOMIC, MTOK_COND, MTOK_CALL
} match_tok_t;

typedef enum {
	M_SUCCESS     = 0,   /* No error */
	M_NOMATCH     = 1,   /* No match found */

	/* --- Pattern Syntax Errors --- */
	M_BADPAT      = 2,   /* Invalid regular expression */
	M_EESCAPE     = 5,   /* Trailing backslash */
	M_ESUBMATCH   = 6,   /* Invalid backreference */
	M_EBRACK      = 7,   /* Unmatched [ or [: */
	M_EPAREN      = 8,   /* Unmatched ( or \( */
	M_ECOLLATE    = 9,   /* Invalid collating element */
	M_ECTYPE      = 10,  /* Invalid character class name */
	M_BADBR       = 11,  /* Invalid content of \\{\\} */
	M_EBRACE      = 12,  /* Unmatched \\{ or \\} */
	M_ERANGE      = 13,  /* Invalid range endpoint */
	M_BADRPT      = 14,  /* Invalid repetition operator */

	/* --- Resource Errors --- */
	M_ESPACE      = 15,  /* Out of memory */
	M_EDEPTH      = 16,  /* Recursion depth exceeded */

	/* --- Runtime Errors --- */
	M_EFLAGS      = 17,  /* Invalid flags */
	M_EINTERNAL   = 18,  /* Internal engine error */
	M_EUNSAFE     = 19,  /* Unsafe pattern detected */

	/* --- PCRE Extensions --- */
	M_ECALL       = 20,  /* Invalid subroutine call */
	M_ECOND       = 21,  /* Invalid conditional */
	M_ELOOK       = 22,  /* Invalid lookahead/lookbehind */
	M_EVERB       = 23,  /* Invalid verb (*SKIP, *FAIL, etc.) */

	/* --- Sentinel --- */
	M_LAST        = 24   /* For bounds checking */
} match_err_t;

typedef enum {
	/* --- Syntax Features (Parser Gates) --- */
	MCOMP_ANCHOR       = 0x01,   /* Enable ^ and $ anchors */
	MCOMP_ALTERN       = 0x02,   /* Enable | alternation syntax */
	MCOMP_QUANT        = 0x04,   /* Enable {} quantifier syntax */
	MCOMP_ESCAPE       = 0x08,   /* Enable \d \w \s character classes */

	MCOMP_BOUNDARY     = 0x10,   /* Enable \b and \B word boundaries */
	MCOMP_ABSOLUTE     = 0x20,   /* Enable \A and \z absolute anchors */
	MCOMP_UNGREEDY     = 0x40,   /* Enables *? uncreedy qualifier */

	MCOMP_CAPTURE      = 0x100,   /* Enable () capture groups */
	MCOMP_BACKREF      = 0x200,   /* Enable \1 \2 numbered backrefs */
	MCOMP_NAMEREF      = 0x400,  /* Enable (?<name>...) named captures */
	MCOMP_RECURSE      = 0x800,  /* Enable (?R) recursion */

	MCOMP_LOOKFWD      = 0x1000,  /* Enable (?=) (?! ) lookahead */
	MCOMP_LOOKBACK     = 0x2000,  /* Enable (?<=) (?<!) lookbehind */
	MCOMP_ATOMIC       = 0x4000, /* Enable (?>) atomic groups */
	MCOMP_COND         = 0x8000, /* Enable (?(condition)yes|no) */

	MCOMP_CALLOUT      = 0x10000, /* Enable (?{code}) callouts */
	MCOMP_VERB         = 0x20000, /* Enable (*SKIP), (*FAIL), (*MARK) */
} mcomp_flag_t;

typedef enum {
	MEXEC_DEFAULT      = 0,

	/* --- Matching Behavior --- */
	MEXEC_ICASE        = 0x01,   /* Case insensitive matching */
	MEXEC_NEWLINE      = 0x02,   /* \n affects ^ and $ */
	MEXEC_NOTBOL       = 0x04,   /* Don't match ^ at start of subject */
	MEXEC_NOTEOL       = 0x08,   /* Don't match $ at end of subject */

	MEXEC_DOTALL       = 0x10,   /* . matches newline characters */
	MEXEC_MULTILINE    = 0x20,   /* ^ and $ match line boundaries */
	MEXEC_UNGREEDY     = 0x40,   /* Quantifiers are lazy by default */
	MEXEC_LONGEST      = 0x80,   /* Prefer longest match (POSIX) */

	MEXEC_UTF          = 0x100,  /* Treat subject as UTF-8 */
	MEXEC_UCP          = 0x200,  /* Unicode character properties */
	MEXEC_NOSUB        = 0x400,  /* Don't report subexpression matches */
	MEXEC_ANYCRLF      = 0x800,  /* Recognize \r\n, \n, \r as newline */

	/* --- Partial Matching --- */
	MEXEC_PARTSOFT     = 0x1000, /* Partial match allowed (soft) */
	MEXEC_PARTHARD     = 0x2000, /* Partial match preferred (hard) */

	/* --- Optimization Control --- */
	MEXEC_NOOPTIM      = 0x4000, /* Disable optimizations (debugging) */
	MEXEC_JIT          = 0x8000, /* Enable JIT compilation if available */
} mexec_flag_t;

#define MCOMP_RE2        (MCOMP_ANCHOR|MCOMP_ALTERN|MCOMP_QUANT|MCOMP_ESCAPE|MCOMP_BOUNDARY)
#define MCOMP_BASIC      (MCOMP_RE2|MCOMP_CAPTURE|MCOMP_BACKREF|MCOMP_UNGREEDY)
#define MCOMP_EXTENDED   (MCOMP_RE2|MCOMP_CAPTURE|MCOMP_UNGREEDY)
#define MCOMP_PCRE2      (MCOMP_BASIC|MCOMP_NAMEREF|MCOMP_LOOKFWD|MCOMP_LOOKBACK|MCOMP_ATOMIC|MCOMP_COND|MCOMP_RECURSE|MCOMP_VERB)
#define MEXEC_RE2        (MEXEC_UTF|MEXEC_NOOPTIM)
#define MEXEC_POSIX      (MEXEC_LONGEST)
#define MEXEC_PCRE2      (MEXEC_DOTALL|MEXEC_UNGREEDY|MEXEC_UTF|MEXEC_UCP)

typedef struct match_node {
	match_tok_t type;
	uint32_t val;
	uint8_t neg, cap_id, min, max, lazy;
	mexec_flag_t eflags;
	struct match_node *a, *b, *alloc_next;
} match_node_t;

typedef struct match_parser {
	const char *p, *end;
	matcher_t *m;
	int err, cap, nsub;
	mcomp_flag_t cflags;
	mexec_flag_t eflags;
	match_node_t *alloc_head;
} match_parser_t;

typedef struct match_find {
	matchoff_t rm_so;
	matchoff_t rm_eo;
} match_find_t;

typedef struct matcher {
	size_t m_nsub;
	mcomp_flag_t cflags;
	mexec_flag_t eflags;
	uint8_t nlit;
	char lit[32];
	match_node_t *root;
	int nr;
	uint32_t rlo[32];
	uint32_t rhi[32];
	uint8_t rcid[32];
	uint8_t rneg[32];
	char *cname[32];
	char *nsub_names[32];
	match_node_t *groups[32]; /* Lookup table for (?1), (?&name) calls */
	uint8_t has_unicode;
} matcher_t;

#define JACL_PARSE_FAIL(p, ERR) do { p->err = ERR; return 0; } while(0)
#define JACL_PARSE_FLAG(p, FET) if (!(p->cflags & FET)) JACL_PARSE_FAIL(p, M_BADPAT);
#define JACL_PARSE_PAREN(p) if (*p->p == ')') { p->p++; } else JACL_PARSE_FAIL(p, M_EPAREN);
#define JACL_PARSE_NAME(dst_name, delim) \
	char dst_name[32] = {0}, __close_delim = (delim == '<') ? '>' : delim; int _ni = 0; \
	while (*p->p && _ni < 31 && *p->p != __close_delim) dst_name[_ni++] = *p->p++;
#define JACL_MAKE_CLASS(...) do { \
	n = __jacl_match_node(p, MTOK_CLASS); \
	if (n) { uint8_t __cid = p->m->nr; __VA_ARGS__ n->val = __cid; } \
	p->p++; \
} while(0)
#define JACL_NODE(var, tok) match_node_t *var = __jacl_match_node(p, tok); if (!var) return 0
#define JACL_NODE_FLAGS(var, tok) JACL_NODE(var, tok); var->eflags = p->eflags
#define JACL_CLASS_SPACE JACL_MAKE_CLASS( \
	__jacl_match_add_range(p->m, 0x0020, 0x0020, __cid, 0); \
	__jacl_match_add_range(p->m, 0x0009, 0x0009, __cid, 0); \
	__jacl_match_add_range(p->m, 0x000A, 0x000A, __cid, 0); \
	__jacl_match_add_range(p->m, 0x000D, 0x000D, __cid, 0); \
)
#define JACL_CLASS_DIGIT JACL_MAKE_CLASS(__jacl_match_add_range(p->m, '0', '9', __cid, 0);)
#define JACL_CLASS_WORD JACL_MAKE_CLASS( \
	__jacl_match_add_range(p->m, '0', '9', __cid, 0); \
	__jacl_match_add_range(p->m, 'A', 'Z', __cid, 0); \
	__jacl_match_add_range(p->m, 'a', 'z', __cid, 0); \
	__jacl_match_add_range(p->m, '_', '_', __cid, 0); \
)
#define JACL_ATOM_MAP(X) X('[', __jacl_atom_bracket) X('(', __jacl_atom_paren) X('\\', __jacl_atom_escape)
#define JACL_ATOM_CASE(ch, fn) case ch: n = (fn)(p); break;
#define JACL_UTF8_CONT(ptr) (((uint8_t)*(ptr) & 0xC0) == 0x80)

#define JACL_PRIM_ANCHOR(name, cond, eflag) static inline const char *__jacl_prim_##name(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) { \
	if (ef & eflag) return 0; \
	if (cond) return matchfind(n->b, m, s, end, pos, caps, ic, nl, ef, depth); \
	return 0; \
}
#define JACL_PRIM_RECURSE_ONE(name, target) static inline const char *__jacl_prim_##name(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) { \
	return matchfind(target, m, s, end, pos, caps, ic, nl, ef, depth); \
}
#define JACL_PRIM_RECURSE_COND(name, target, condition) static inline const char *__jacl_prim_##name(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) { \
	if (!(condition)) return 0; \
	return matchfind(target, m, s, end, pos, caps, ic, nl, ef, depth); \
}
#define JACL_PRIM_CAP static inline const char *__jacl_prim_CAP(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) { \
	int idx = n->cap_id * 2; \
	if (idx + 1 >= MATCH_MAX_GROUPS * 2) return 0; \
	matchoff_t sv_so = caps[idx], sv_eo = caps[idx+1]; \
	caps[idx] = pos - s; \
	const char *r = matchfind(n->a, m, s, end, pos, caps, ic, nl, ef, depth+1); \
	if (r) { \
		caps[idx+1] = r - s; \
		const char *res = matchfind(n->b, m, s, end, r, caps, ic, nl, ef, depth+1); \
		if (!res) { caps[idx] = sv_so; caps[idx+1] = sv_eo; } \
		return res; \
	} \
	caps[idx] = sv_so; caps[idx+1] = sv_eo; \
	return 0; \
}

static inline match_node_t *__jacl_match_atom(match_parser_t *p);
static inline const char *matchfind(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth);
static inline const char *matchgate(const matcher_t *m, const char *s, const char *end, int ic);

static inline match_node_t *__jacl_match_node(match_parser_t *p, match_tok_t type) {
	match_node_t *n = calloc(1, sizeof(match_node_t));
	if (!n) JACL_PARSE_FAIL(p, M_ESPACE);
	n->type = type; n->max = 255; n->alloc_next = p->alloc_head; p->alloc_head = n;
	return n;
}
static inline match_node_t *__jacl_match_rep(match_parser_t *p, match_node_t *body, int min, int max, int lazy) {
	match_node_t *n = __jacl_match_node(p, MTOK_REP);
	if (!n) return 0;
	n->a = body; n->min = min; n->max = (max < 0) ? 255 : (uint8_t)max; n->lazy = lazy ? 1 : 0; n->eflags = body->eflags;
	return n;
}
static inline void __jacl_match_free_ast(match_node_t *n) {
	if (!n) return;
	__jacl_match_free_ast(n->a); __jacl_match_free_ast(n->b); free(n);
}
static inline void __jacl_match_cleanup(matcher_t *m, match_parser_t *p) {
	for (match_node_t *c = p->alloc_head; c; ) { match_node_t *t = c->alloc_next; free(c); c = t; }
	memset(m, 0, sizeof(*m));
}
static inline match_node_t *__jacl_match_term(match_parser_t *p) {
	int stop = p->cflags & MCOMP_ALTERN;
	match_node_t *a = __jacl_match_atom(p), *b;
	while (*p->p && *p->p != ')' && !p->err) {
		if ((stop && *p->p == '|') || strchr("*?+{", *p->p)) break;
		b = __jacl_match_atom(p); if (!b) break;
		JACL_NODE_FLAGS(seq, MTOK_SEQ); seq->a = a; seq->b = b; a = seq;
	}
	return a;
}
static inline match_node_t *__jacl_match_expr(match_parser_t *p) {
	match_node_t *a = __jacl_match_term(p), *b;
	if (!(p->cflags & MCOMP_ALTERN)) return a;
	while (*p->p == '|' && !p->err) {
		p->p++; b = __jacl_match_term(p);
		if (!b) break;
		JACL_NODE_FLAGS(alt, MTOK_ALT); alt->a = a; alt->b = b; a = alt;
	}
	return a;
}
static inline uint32_t __jacl_match_utf8_next(const char **p, const char *end) {
	if (*p >= end) return 0xFFFD;
	uint8_t b = (uint8_t)*(*p)++;
	if (b < 0x80) return b;
	uint32_t cp = 0; int len = 0;
	if ((b & 0xE0) == 0xC0) { cp = b & 0x1F; len = 1; }
	else if ((b & 0xF0) == 0xE0) { cp = b & 0x0F; len = 2; }
	else if ((b & 0xF8) == 0xF0) { cp = b & 0x07; len = 3; }
	else { *p = end; return 0xFFFD; }
	while (len-- && *p < end) { cp = (cp << 6) | ((uint8_t)**p & 0x3F); (*p)++; }
	return cp;
}
static inline int __jacl_match_fold(uint32_t cp, uint32_t m, int ic) {
	if (cp == m) return 1;
	if (!ic) return 0;
	if (cp < 128 && m < 128) {
		if (cp >= 'A' && cp <= 'Z' && cp + 32 == m) return 1;
		if (m >= 'A' && m <= 'Z' && m + 32 == cp) return 1;
	} else {
		if (towlower((wint_t)cp) == towlower((wint_t)m)) return 1;
	}
	return 0;
}
static inline int __jacl_match_is_word(uint32_t cp) {
	if (cp < 128) {
		return (cp >= 'a' && cp <= 'z') || (cp >= 'A' && cp <= 'Z') ||
		       (cp >= '0' && cp <= '9') || (cp == '_');
	}
	return iswalnum((wint_t)cp) || cp == '_';
}
static inline void __jacl_match_add_range(matcher_t *m, uint32_t lo, uint32_t hi, uint8_t cid, uint8_t neg) {
	if (lo > hi || m->nr >= 32) return;
	if (lo >= 128) m->has_unicode = 1;
	for (int i = 0; i < m->nr; i++) {
		if (m->rcid[i] != cid || lo > m->rhi[i] + 1 || hi < m->rlo[i] - 1) continue;
		lo = lo < m->rlo[i] ? lo : m->rlo[i]; hi = hi > m->rhi[i] ? hi : m->rhi[i];
		if (m->cname[i]) free(m->cname[i]);
		memmove(&m->rlo[i], &m->rlo[i+1], (m->nr-1-i)*16);
		m->nr--; i--;
	}
	m->rlo[m->nr] = lo; m->rhi[m->nr] = hi; m->rcid[m->nr] = cid; m->rneg[m->nr] = neg; m->cname[m->nr++] = 0;
}
static inline void __jacl_match_resolve_cclass(matcher_t *m, const char *nm, uint8_t cid, uint8_t neg) {
	if (m->nr < 32) { m->rlo[m->nr] = 0; m->rhi[m->nr] = 0; m->rcid[m->nr] = cid; m->rneg[m->nr] = neg; m->cname[m->nr] = strdup(nm); m->nr++; m->has_unicode = 1; }
}
static inline int __jacl_match_in_class(const matcher_t *m, uint32_t cp, uint8_t cid, int ic) {
	int found = 0, in_range = 0, neg = 0;
	for (int i = 0; i < m->nr; i++) {
		if (m->rcid[i] != cid) continue;
		neg = m->rneg[i]; found = 1;
		if (m->cname[i]) { wctype_t w = wctype(m->cname[i]); if (w && iswctype((wint_t)cp, w)) in_range = 1; }
		else if (cp >= m->rlo[i] && cp <= m->rhi[i]) in_range = 1;
	}
	if (!found) return 0; if (neg) return !in_range; if (in_range) return 1;
	if (ic && cp < 0x80) {
		uint8_t f = (cp >= 'A' && cp <= 'Z') ? cp + 32 : (cp >= 'a' && cp <= 'z') ? cp - 32 : cp;
		for (int i = 0; i < m->nr; i++) { if (m->rcid[i] == cid && !m->cname[i] && f >= m->rlo[i] && f <= m->rhi[i]) return 1; }
	}
	return 0;
}

/* Bracket Parser Helpers (Lego Pieces) */
static inline int __jacl_validate_posix_class(const char *nm) {
	wctype_t w = wctype(nm);
	return w ? 1 : 0;
}
static inline int __jacl_parse_collating(match_parser_t *p, uint8_t cid, uint8_t neg) {
	p->p += 2;
	char name[16] = {0}; int i = 0;
	while (*p->p && *p->p != '.' && *p->p != ']') {
		if (i < 15) name[i++] = *p->p;
		p->p++;
	}
	if (*p->p != '.' || p->p[1] != ']') JACL_PARSE_FAIL(p, M_ECOLLATE);
	if (name[0] == '\0') JACL_PARSE_FAIL(p, M_ECOLLATE);
	p->p += 2;
	__jacl_match_add_range(p->m, (uint8_t)name[0], (uint8_t)name[0], cid, neg);
	return 0;
}
static inline int __jacl_parse_equivalence(match_parser_t *p, uint8_t cid, uint8_t neg) {
	p->p += 2;
	char name[16] = {0}; int i = 0;
	while (*p->p && *p->p != '=' && *p->p != ']') {
		if (i < 15) name[i++] = *p->p;
		p->p++;
	}
	if (*p->p != '=' || p->p[1] != ']') JACL_PARSE_FAIL(p, M_ECOLLATE);
	if (name[0] == '\0') JACL_PARSE_FAIL(p, M_ECOLLATE);
	p->p += 2;
	__jacl_match_add_range(p->m, (uint8_t)name[0], (uint8_t)name[0], cid, neg);
	return 0;
}
static inline int __jacl_parse_posix_class(match_parser_t *p, uint8_t cid, uint8_t neg) {
	p->p += 2;
	char nm[16] = {0}; int i = 0;
	while (*p->p && *p->p != ':' && *p->p != ']') {
		if (i < 15) nm[i++] = *p->p;
		p->p++;
	}
	if (*p->p != ':' || p->p[1] != ']') JACL_PARSE_FAIL(p, M_ECTYPE);
	p->p += 2;
	if (!__jacl_validate_posix_class(nm)) JACL_PARSE_FAIL(p, M_ECTYPE);
	if (p->m->nr < 32) {
		p->m->rlo[p->m->nr] = 0; p->m->rhi[p->m->nr] = 0;
		p->m->rcid[p->m->nr] = cid; p->m->rneg[p->m->nr] = neg;
		p->m->cname[p->m->nr] = strdup(nm); p->m->nr++; p->m->has_unicode = 1;
	}
	return 0;
}

static inline match_node_t *__jacl_paren_call(match_parser_t *p, int is_named) {
	p->p++;
	int id = 0;
	if (is_named) {
		JACL_PARSE_NAME(name, ')'); JACL_PARSE_PAREN(p);
		for (int k = 1; k < 32; k++) if (p->m->nsub_names[k] && strcmp(p->m->nsub_names[k], name) == 0) { id = k; break; }
		if (id == 0) JACL_PARSE_FAIL(p, M_ESUBMATCH);
	} else {
		while (*p->p >= '0' && *p->p <= '9') { id = id * 10 + (*p->p - '0'); p->p++; }
		JACL_PARSE_PAREN(p);
	}
	JACL_NODE_FLAGS(n, MTOK_CALL); n->val = id;
	return n;
}
static inline match_node_t *__jacl_paren_cond(match_parser_t *p) {
	JACL_PARSE_FLAG(p, MCOMP_COND);
	p->p++; int cond_id = 0;
	if (*p->p >= '0' && *p->p <= '9') { while (*p->p >= '0' && *p->p <= '9') cond_id = cond_id * 10 + (*p->p++ - '0'); }
	else if (*p->p == '<' || *p->p == '\'') {
		char delim = *p->p++; JACL_PARSE_NAME(name, delim);
		if (*p->p == delim || *p->p == '>') p->p++;
		for (int k = 1; k < 32; k++) if (p->m->nsub_names[k] && strcmp(p->m->nsub_names[k], name) == 0) { cond_id = k; break; }
	}
	JACL_PARSE_PAREN(p); match_node_t *yes = __jacl_match_term(p), *no = (*p->p == '|') ? (p->p++, __jacl_match_term(p)) : __jacl_match_node(p, MTOK_END);
	JACL_NODE_FLAGS(n, MTOK_COND); n->val = cond_id; n->a = yes; n->b = no; p->cap--; p->nsub--; JACL_PARSE_PAREN(p);
	return n;
}
static inline match_node_t *__jacl_paren_branch_reset(match_parser_t *p) {
	int reset = --p->cap, max_cap = reset, first = 1;
	match_node_t *head = 0; p->p++;
	do {
		p->cap = reset; match_node_t *term = __jacl_match_term(p);
		if (p->err) return 0;
		if (p->cap > max_cap) max_cap = p->cap;
		if (first) { head = term; first = 0; }
		else { JACL_NODE_FLAGS(alt, MTOK_ALT); alt->a = head; alt->b = term; head = alt; }
	} while (*p->p == '|' && !p->err && (p->p++));
	p->cap = max_cap; JACL_PARSE_PAREN(p);
	return head;
}
static inline match_node_t *__jacl_paren_named_cap(match_parser_t *p, int cap) {
	JACL_PARSE_FLAG(p, MCOMP_NAMEREF);
	p->p++; JACL_PARSE_NAME(name, '>'); if (*p->p == '>') p->p++;
	if (cap < 32) p->m->nsub_names[cap] = strdup(name);
	JACL_NODE_FLAGS(n, MTOK_CAP);
	if (cap < 32) p->m->groups[cap] = (void *)n;
	n->cap_id = cap; n->a = __jacl_match_expr(p); JACL_PARSE_PAREN(p);
	return n;
}
static inline match_node_t *__jacl_paren_look(match_parser_t *p, int is_lb, int is_neg) {
	JACL_PARSE_FLAG(p, (is_lb ? MCOMP_LOOKBACK : MCOMP_LOOKFWD));
	JACL_NODE_FLAGS(n, MTOK_LOOK);
	p->p += (n->cap_id = is_lb) ? 2 : 1; n->neg = is_neg ? 1 : 0; n->a = __jacl_match_expr(p);
	JACL_PARSE_PAREN(p);
	return n;
}
static inline match_node_t *__jacl_paren_atomic(match_parser_t *p) {
	JACL_PARSE_FLAG(p, MCOMP_ATOMIC);
	p->p++; match_node_t *inner = __jacl_match_expr(p); if (!inner) return 0;
	JACL_NODE_FLAGS(n, MTOK_ATOMIC); n->a = inner; JACL_PARSE_PAREN(p); return n;
}
static inline match_node_t *__jacl_paren_recursion(match_parser_t *p) {
	JACL_PARSE_FLAG(p, MCOMP_RECURSE);
	if (p->p[1] == ')' || p->p[1] == '0') p->p += 2;
	else JACL_PARSE_FAIL(p, M_BADPAT);
	JACL_NODE_FLAGS(n, MTOK_REP);
	return n;
}
static inline match_node_t *__jacl_paren_flags(match_parser_t *p) {
	uint8_t new_flags = p->eflags; int is_scope = 0;
	while (1) { char c = *p->p;
		if (c == 'i') { new_flags |= MEXEC_ICASE; p->p++; }
		else if (c == 's') { new_flags |= MEXEC_DOTALL; p->p++; }
		else if (c == '-') {
			p->p++;
			if (*p->p == 'i') { new_flags &= ~MEXEC_ICASE; p->p++; }
			else if (*p->p == 's') { new_flags &= ~MEXEC_DOTALL; p->p++; }
			else JACL_PARSE_FAIL(p, M_BADPAT);
		}
		else if (c == ':') { is_scope = 1; p->p++; break; }
		else if (c == ')' || c == '\0') { if (c == ')') p->p++; break; }
		else JACL_PARSE_FAIL(p, M_BADPAT);
	}
	if (p->err) return 0;
	if (is_scope) {
		uint8_t saved = p->eflags; p->eflags = new_flags; match_node_t *n = __jacl_match_expr(p);
		if (p->err) { p->eflags = saved; return 0; }
		p->eflags = saved; JACL_PARSE_PAREN(p);
		return n;
	} else { p->eflags = new_flags; JACL_NODE(n, MTOK_END); return n; }
}
static inline match_node_t *__jacl_paren_capture(match_parser_t *p, int cap) {
	JACL_PARSE_FLAG(p, MCOMP_CAPTURE); JACL_NODE_FLAGS(n, MTOK_CAP);
	if (cap < 32) p->m->groups[cap] = (void *)n;
	n->cap_id = cap; n->a = __jacl_match_expr(p); JACL_PARSE_PAREN(p);
	return n;
}
static inline match_node_t *__jacl_atom_paren(match_parser_t *p) {
	int cap = ++p->cap;
	p->p++; p->nsub++;
	if (*p->p != '?') return __jacl_paren_capture(p, cap);
	p->p++;
	switch (*p->p) {
		case '&': return __jacl_paren_call(p, 1);
		case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': return __jacl_paren_call(p, 0);
		case '(': return __jacl_paren_cond(p);
		case '|': return __jacl_paren_branch_reset(p);
		case '<':
			if (p->p[1] == '=' || p->p[1] == '!') return __jacl_paren_look(p, 1, p->p[1] == '!');
			return __jacl_paren_named_cap(p, cap);
		case '>': return __jacl_paren_atomic(p);
		case '=': return __jacl_paren_look(p, 0, 0);
		case '!': return __jacl_paren_look(p, 0, 1);
		case 'R': case '0': return __jacl_paren_recursion(p);
		default: return __jacl_paren_flags(p);
	}
}
static inline match_node_t *__jacl_atom_escape(match_parser_t *p) {
	match_node_t *n = 0; p->p++;
	if (*p->p == 'k') {
		JACL_PARSE_FLAG(p, (MCOMP_BACKREF | MCOMP_NAMEREF));
		p->p++;
		char delim = *p->p;
		if (delim != '<' && delim != '\'') JACL_PARSE_FAIL(p, M_BADPAT);
		p->p++;
		JACL_PARSE_NAME(name, delim);
		if (*p->p == delim || (delim == '<' && *p->p == '>')) p->p++;
		int id = -1;
		for (int k = 1; k < 32; k++) if (p->m->nsub_names[k] && strcmp(p->m->nsub_names[k], name) == 0) { id = k; break; }
		if (id == -1) JACL_PARSE_FAIL(p, M_ESUBMATCH);
		n = __jacl_match_node(p, MTOK_BACKREF);
		if (n) { n->eflags = p->eflags; n->val = id; }
		return n;
	}
	if (*p->p >= '1' && *p->p <= '9') {
		JACL_PARSE_FLAG(p, MCOMP_BACKREF);
		int id = 0;
		while (*p->p >= '0' && *p->p <= '9') { id = id * 10 + (*p->p - '0'); p->p++; }
		n = __jacl_match_node(p, MTOK_BACKREF);
		if (n) { n->eflags = p->eflags; n->val = id; }
		return n;
	}
	if (*p->p == 'b') { JACL_PARSE_FLAG(p, MCOMP_BOUNDARY); JACL_NODE_FLAGS(n, MTOK_WB); p->p++; return n; }
	if (*p->p == 'B') { JACL_PARSE_FLAG(p, MCOMP_BOUNDARY); JACL_NODE_FLAGS(n, MTOK_NWB); p->p++; return n; }
	if (*p->p == 's') { JACL_PARSE_FLAG(p, MCOMP_ESCAPE); JACL_CLASS_SPACE; return n; }
	if (*p->p == 'd') { JACL_PARSE_FLAG(p, MCOMP_ESCAPE); JACL_CLASS_DIGIT; return n; }
	if (*p->p == 'w') { JACL_PARSE_FLAG(p, MCOMP_ESCAPE); JACL_CLASS_WORD; return n; }
	n = __jacl_match_node(p, MTOK_CHAR);
	if (n) { n->eflags = p->eflags; n->val = __jacl_match_utf8_next(&p->p, p->end); if (n->val >= 128) p->m->has_unicode = 1; }
	return n;
}
static inline match_node_t *__jacl_atom_bracket(match_parser_t *p) {
	p->p++;
	int neg = (*p->p == '^');
	if (neg) p->p++;
	uint8_t cid = p->m->nr;
	while (*p->p && *p->p != ']') {
		if (*p->p == '[' && p->p[1] == ':') {
			__jacl_parse_posix_class(p, cid, neg);
		}
		else if (*p->p == '[' && p->p[1] == '.') {
			__jacl_parse_collating(p, cid, neg);
		}
		else if (*p->p == '[' && p->p[1] == '=') {
			__jacl_parse_equivalence(p, cid, neg);
		}
		else if (*p->p == '[') {
			if (*p->p == '\\' && p->p[1]) p->p++;
			uint32_t lo = __jacl_match_utf8_next(&p->p, p->end), hi = lo;
			if (*p->p == '-' && *(p->p+1) != ']') {
				p->p++;
				hi = __jacl_match_utf8_next(&p->p, p->end);
			}
			__jacl_match_add_range(p->m, lo, hi, cid, neg);
		}
		else {
			if (*p->p == '\\' && p->p[1]) p->p++;
			uint32_t lo = __jacl_match_utf8_next(&p->p, p->end), hi = lo;
			if (*p->p == '-' && *(p->p+1) != ']') {
				p->p++;
				hi = __jacl_match_utf8_next(&p->p, p->end);
			}
			__jacl_match_add_range(p->m, lo, hi, cid, neg);
		}
	}
	if (*p->p == ']') p->p++;
	else JACL_PARSE_FAIL(p, M_EBRACK);
	JACL_NODE_FLAGS(n, MTOK_CLASS);
	n->val = cid;
	n->neg = neg;
	return n;
}
static inline match_node_t *__jacl_atom_simple(match_parser_t *p) {
	static const struct {
		char c;
		match_tok_t t;
		mcomp_flag_t flag;
	}
	map[] = {
		{'.', MTOK_ANY, 0},
		{'^', MTOK_BOL, MCOMP_ANCHOR},
		{'$', MTOK_EOL, MCOMP_ANCHOR}
	};
	for (int i = 0; i < 3; i++) {
		if (*p->p == map[i].c) {
			if (map[i].flag) JACL_PARSE_FLAG(p, map[i].flag);
			JACL_NODE_FLAGS(n, map[i].t); p->p++;
			return n;
		}
	}
	if (*p->p == '|' && !(p->cflags & MCOMP_ALTERN)) {
		JACL_NODE_FLAGS(n, MTOK_CHAR); n->val = '|'; p->p++;
		return n;
	}
	if (*p->p == ')' || *p->p == '\0' || *p->p == '|') return 0;
	JACL_NODE_FLAGS(n, MTOK_CHAR);
	if (n) {
		n->val = __jacl_match_utf8_next(&p->p, p->end);
		if (n->val >= 128) p->m->has_unicode = 1;
	}
	return n;
}
static inline void __jacl_match_quant(match_parser_t *p, match_node_t **n) {
	JACL_PARSE_FLAG(p, MCOMP_QUANT);
	char q = *p->p;
	if (!q) return;
	int mn = 0, mx = 255, is_lazy = 0;
	if (q == '*' || q == '?' || q == '+') {
		p->p++; mn = (q == '+'); mx = (q == '?') ? 1 : 255;
	} else if (q == '{') {
		p->p++; mn = 0;
		while (*p->p >= '0' && *p->p <= '9') mn = mn*10 + (*p->p++ - '0');
		mx = mn;
		if (*p->p == ',') {
			p->p++;
			if (*p->p == '}') mx = 255;
			else {
				mx = 0;
				while (*p->p >= '0' && *p->p <= '9') mx = mx*10 + (*p->p++ - '0');
			}
		}
		if (*p->p != '}') JACL_PARSE_FAIL(p, M_EBRACE);
		p->p++;
	} else return;
	if (*p->p == '?') {
		JACL_PARSE_FLAG(p, MCOMP_UNGREEDY); p->eflags |= MEXEC_UNGREEDY; p->p++; is_lazy = 1;
	}
	match_node_t *rep = __jacl_match_rep(p, *n, mn, mx, is_lazy);
	if (*p->p == '+') {
		p->p++; match_node_t *atomic = __jacl_match_node(p, MTOK_ATOMIC);
		if (atomic) { atomic->a = rep; atomic->eflags = rep->eflags; *n = atomic; }
	} else *n = rep;
}
static inline match_node_t *__jacl_match_atom(match_parser_t *p) {
	match_node_t *n = 0;
	switch (*p->p) { JACL_ATOM_MAP(JACL_ATOM_CASE) default: n = __jacl_atom_simple(p); }
	if (n) __jacl_match_quant(p, &n);
	return n;
}
#undef JACL_PARSE_PAREN
#undef JACL_PARSE_NAME
#undef JACL_MAKE_CLASS
#undef JACL_ATOM_MAP
#undef JACL_ATOM_CASE

static inline const char *__jacl_prim_END(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) { return pos; }
static inline const char *__jacl_prim_CHAR(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) {
	if (pos >= end) return 0;
	uint8_t b = (uint8_t)*pos;
	int ascii = (int)extra;
	if (ascii && b >= 128) return 0;
	if (b < 0x80) {
		if (__jacl_match_fold(b, n->val, ic)) return matchfind(n->b, m, s, end, pos+1, caps, ic, nl, ef, depth);
		return 0;
	}
	uint32_t cp = __jacl_match_utf8_next(&pos, end);
	if (__jacl_match_fold(cp, n->val, ic)) return matchfind(n->b, m, s, end, pos, caps, ic, nl, ef, depth);
	return 0;
}
static inline const char *__jacl_prim_CLASS(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) {
	if (pos >= end) return 0;
	uint32_t cp;
	const char *next = pos;
	if (!m->has_unicode && (uint8_t)*pos < 128) { cp = (uint8_t)*pos; next++; }
	else { cp = __jacl_match_utf8_next(&next, end); }
	if (__jacl_match_in_class(m, cp, n->val, ic)) return matchfind(n->b, m, s, end, next, caps, ic, nl, ef, depth);
	return 0;
}
static inline const char *__jacl_prim_ANY(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) {
	if (pos >= end) return 0;
	uint8_t b = (uint8_t)*pos;
	if (!(n->eflags & MEXEC_DOTALL) && b == '\n') return 0;
	const char *next = pos + 1;
	while (b >= 0x80 && next < end && JACL_UTF8_CONT(next)) next++;
	return matchfind(n->b, m, s, end, next, caps, ic, nl, ef, depth);
}
JACL_PRIM_ANCHOR(BOL, pos == s || (nl && pos > s && *(pos-1) == '\n'), MEXEC_NOTBOL)
JACL_PRIM_ANCHOR(EOL, pos == end || (nl && pos < end && *pos == '\n'), MEXEC_NOTEOL)
JACL_PRIM_CAP
JACL_PRIM_RECURSE_ONE(FLAGS, n->a)
JACL_PRIM_RECURSE_COND(CALL, m->groups[n->val], n->val < 32 && m->groups[n->val])
static inline const char *__jacl_prim_SEQ(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) {
	while (n->type == MTOK_SEQ) {
		const char *r = matchfind(n->a, m, s, end, pos, caps, ic, nl, ef, depth+1);
		if (!r) return 0;
		pos = r; n = n->b; ic = (n->eflags & MEXEC_ICASE) ? 1 : ic;
	}
	return matchfind(n, m, s, end, pos, caps, ic, nl, ef, depth);
}
static inline const char *__jacl_prim_ALT(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) {
	matchoff_t sc[MATCH_MAX_GROUPS * 2];
	const char *sv = pos;
	memcpy(sc, caps, sizeof(sc));
	const char *r = matchfind(n->a, m, s, end, pos, caps, ic, nl, ef, depth+1);
	if (r) return r;
	memcpy(caps, sc, sizeof(sc));
	return matchfind(n->b, m, s, end, sv, caps, ic, nl, ef, depth+1);
}
static inline const char *__jacl_prim_REP(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) {
	const char *path[256];
	matchoff_t cpath[256][MATCH_MAX_GROUPS * 2];
	int k = 0;
	path[0] = pos;
	memcpy(cpath[0], caps, sizeof(cpath[0]));
	while (k < 255 && (n->max == 255 || k < n->max)) {
		memcpy(cpath[k+1], cpath[k], sizeof(cpath[0]));
		const char *next = matchfind(n->a, m, s, end, path[k], cpath[k+1], ic, nl, ef, depth+1);
		if (!next || next == path[k]) break;
		path[++k] = next;
	}
	for (int i = n->lazy ? n->min : k, lim = n->lazy ? k : n->min, step = n->lazy ? 1 : -1; n->lazy ? i <= lim : i >= lim; i += step) {
		memcpy(caps, cpath[i], sizeof(cpath[0]));
		const char *r = n->b ? matchfind(n->b, m, s, end, path[i], caps, ic, nl, ef, depth+1) : path[i];
		if (r) return r;
	}
	return 0;
}
static inline const char *__jacl_prim_LOOK(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) {
	matchoff_t scap[MATCH_MAX_GROUPS * 2];
	memcpy(scap, caps, sizeof(scap));
	int is_lb = extra & 1, is_neg = (extra >> 1) & 1, found = 0;
	if (is_lb) {
		for (const char *cp = pos; cp >= s; cp--) {
			memcpy(caps, scap, sizeof(caps));
			if (matchfind(n->a, m, s, end, cp, caps, ic, nl, ef, 0) == pos) {
				found = 1; break;
			}
			if (cp == s) break;
			while (cp > s && JACL_UTF8_CONT(cp)) cp--;
		}
	} else {
		found = !!matchfind(n->a, m, s, end, pos, scap, ic, nl, ef, 0);
	}
	return (is_neg ? !found : found) ? matchfind(n->b, m, s, end, pos, caps, ic, nl, ef, depth) : 0;
}
static inline const char *__jacl_prim_BACKREF(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) {
	int id = n->val, idx = id * 2;
	if (idx + 1 >= MATCH_MAX_GROUPS * 2) return 0;
	matchoff_t so = caps[idx], eo = caps[idx+1];
	if (so == -1) return 0;
	const char *ref_start = s + so, *ref_end = s + eo, *curr = pos;
	while (ref_start < ref_end && curr < end) {
		uint32_t cp_ref = __jacl_match_utf8_next(&ref_start, ref_end), cp_curr = __jacl_match_utf8_next(&curr, end);
		if (!__jacl_match_fold(cp_curr, cp_ref, ic)) return 0;
	}
	if (ref_start != ref_end) return 0;
	return matchfind(n->b, m, s, end, curr, caps, ic, nl, ef, depth+1);
}
static inline const char *__jacl_prim_ATOMIC(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) {
	const char *r = matchfind(n->a, m, s, end, pos, caps, ic, nl, ef, depth+1);
	if (!r) return 0;
	if (n->b) return matchfind(n->b, m, s, end, r, caps, ic, nl, ef, depth+1);
	return r;
}
static inline const char *__jacl_prim_COND(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) {
	int idx = n->val * 2, matched = (idx < MATCH_MAX_GROUPS * 2 && caps[idx] != -1);
	match_node_t *b = matched ? n->a : n->b;
	if (b) return matchfind(b, m, s, end, pos, caps, ic, nl, ef, depth+1);
	return pos;
}
static inline const char *__jacl_prim_WB(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) {
	int neg = (int)extra;
	int prev = 0, curr = 0;
	int at_start = (pos == s);
	int at_end = (pos == end);
	if (at_start || at_end) {
		return (neg ? 0 : 1) ? matchfind(n->b, m, s, end, pos, caps, ic, nl, ef, depth) : 0;
	}
	if (pos > s) {
		const char *pp = pos - 1;
		while (pp > s && JACL_UTF8_CONT(pp)) pp--;
		if (__jacl_match_is_word(__jacl_match_utf8_next((const char**)&pp, pos))) prev = 1;
	}
	if (pos < end) {
		const char *cp = pos;
		if (__jacl_match_is_word(__jacl_match_utf8_next(&cp, end))) curr = 1;
	}
	if (neg ? (prev == curr) : (prev != curr)) return matchfind(n->b, m, s, end, pos, caps, ic, nl, ef, depth);
	return 0;
}
static inline const char *__jacl_prim_NWB(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth, uint32_t extra) {
	return __jacl_prim_WB(n, m, s, end, pos, caps, ic, nl, ef, depth, 1);
}

#define JACL_MATCH_LIST(X) X(END,0) X(CHAR,!m->has_unicode) X(CLASS,0) X(ANY,0) X(BOL,0) X(EOL,0) X(CAP,0) X(SEQ,0) X(ALT,0) X(REP,0) X(LOOK,((uint32_t)n->cap_id)|((uint32_t)n->neg<<1)) X(BACKREF,0) X(FLAGS,0) X(ATOMIC,0) X(CALL,0) X(COND,0) X(WB,0) X(NWB,1)
#define JACL_CASE(TOK, EXTRA) case MTOK_##TOK: return __jacl_prim_##TOK(n, m, s, end, pos, caps, local_ic, nl, ef, depth, EXTRA);
static inline const char *matchfind(match_node_t *n, const matcher_t *m, const char *s, const char *end, const char *pos, matchoff_t *caps, int ic, int nl, int ef, int depth) {
	if (depth > MATCH_MAX_DEPTH) return 0;
	if (!n) return pos;
	if (!m) return 0;
	int local_ic = ((n)->eflags & MEXEC_ICASE) ? 1 : ic;
	switch (n->type) { JACL_MATCH_LIST(JACL_CASE) }
	return 0;
}
static inline const char *matchgate(const matcher_t *m, const char *s, const char *end, int ic) {
	if (m->nlit == 0) return s;
	for (const char *p = s; p + m->nlit <= end; p++) {
		int ok = 1;
		for (int i = 0; i < m->nlit; i++) {
			uint8_t a = ic && m->lit[i] >= 'A' && m->lit[i] <= 'Z' ? m->lit[i] + 32 : m->lit[i];
			uint8_t b = ic && p[i] >= 'A' && p[i] <= 'Z' ? p[i] + 32 : p[i];
			if (a != b) { ok = 0; break; }
		}
		if (ok) return p;
	}
	return NULL;
}

static inline int matchcomp(matcher_t *restrict m, const char *pat, mcomp_flag_t fl) {
	memset(m, 0, sizeof(*m));
	m->cflags = fl; m->has_unicode = 0; match_parser_t p = {pat, pat+strlen(pat), m, 0, 0, 0, fl, 0, 0}; m->root = __jacl_match_expr(&p);
	if (p.err) { __jacl_match_cleanup(m, &p); return p.err; }
	m->m_nsub = p.cap;
	if (m->root && ((match_node_t*)m->root)->type == MTOK_ALT) {
		m->nlit = 0;
	} else {
		m->nlit = 0;
		const char *pp = pat;
		if (*pp == '^') pp++;
		while (*pp && *pp != '.' && *pp != '[' && *pp != '(' && *pp != '|' && *pp != ')' && *pp != '*' && *pp != '+' && *pp != '?' && *pp != '{' && *pp != '\\' && *pp != '$') {
			m->lit[m->nlit++] = *pp++;
			if (m->nlit >= 31) break;
		}
		if (m->nlit > 0) {
			char next = *pp;
			if (next == '*' || next == '?' || next == '+' || next == '{') m->nlit = 0;
		}
	}
	return M_SUCCESS;
}
static inline int matchexec(const matcher_t *restrict m, const char *s, const char *end, match_find_t *pm, size_t nm, mexec_flag_t fl) {
	int ic = (m->eflags | fl) & MEXEC_ICASE, nl = (m->eflags | fl) & MEXEC_NEWLINE, anchored = 0;
	if (m->root && ((match_node_t*)m->root)->type == MTOK_BOL) anchored = 1;
	const char *start = anchored ? s : matchgate(m, s, end, ic);
	if (!start) return M_NOMATCH;
	matchoff_t caps[MATCH_MAX_GROUPS * 2];
	for (; start <= end; start++) {
		for(int i=0; i<MATCH_MAX_GROUPS * 2; i++) caps[i] = -1;
		const char *r = matchfind((match_node_t*)m->root, m, s, end, start, caps, ic, nl, fl, 0);
		if (r) {
			matchoff_t so = start - s, eo = r - s;
			if (pm && nm > 0) {
				pm[0].rm_so = so; pm[0].rm_eo = eo;
				for (size_t i = 1; i < nm; i++) { pm[i].rm_so = caps[i*2]; pm[i].rm_eo = caps[i*2+1]; }
			}
			return 0;
		}
		if (anchored) return M_NOMATCH;
		const char *next_gate = matchgate(m, start + 1, end, ic);
		if (!next_gate) return M_NOMATCH;
		start = next_gate - 1;
	}
	return M_NOMATCH;
}
static inline void matchfree(matcher_t *restrict m) {
	if (!m) return;
	__jacl_match_free_ast((match_node_t*)m->root);
	for (int i = 0; i < m->nr; i++) if (m->cname[i]) free(m->cname[i]);
	for (int i = 0; i < 32; i++) if (m->nsub_names[i]) free(m->nsub_names[i]);
}

#ifdef __cplusplus
}
#endif

#endif /* _CORE__MATCH_H */
