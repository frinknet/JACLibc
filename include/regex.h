/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _REGEX_H
#define _REGEX_H

#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <core/_/match.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ===================================================================== */
/* Types                                                                 */
/* ===================================================================== */

/* POSIX types */
typedef matcher_t regex_t;
typedef matchoff_t regoff_t;

typedef struct {
	regoff_t rm_so;
	regoff_t rm_eo;
} regmatch_t;

/* PCRE2 types */
typedef matcher_t pcre2_code;

typedef struct {
	match_find_t matches[MATCH_MAX_GROUPS];
	int count;
} pcre2_match_data;

typedef size_t PCRE2_SIZE;
#define PCRE2_SIZE_MAX SIZE_MAX

/* RE2 types */
typedef matcher_t re2_t;

/* ===================================================================== */
/* Mode Flags (mutually exclusive - pick ONE)                            */
/* ===================================================================== */

#define REG_BASIC       0x0000
#define REG_EXTENDED    0x0001
#define REG_PCRE2       0x0002
#define REG_RE2         0x0004

/* ===================================================================== */
/* Behavior Flags (independent values - explicit translation)            */
/* ===================================================================== */

#define REG_ICASE       0x0010
#define REG_NEWLINE     0x0020
#define REG_NOTBOL      0x0040
#define REG_NOTEOL      0x0080
#define REG_DOTALL      0x0100
#define REG_NOSUB       0x0200
#define REG_UTF         0x0400
#define REG_UCP         0x0800

/* ===================================================================== */
/* Error Codes                                                           */
/* ===================================================================== */

#define REG_SUCCESS     M_SUCCESS     /* No error */
#define REG_NOMATCH     M_NOMATCH     /* No match found */
#define REG_BADPAT      M_BADPAT      /* Invalid regular expression */
#define REG_EESCAPE     M_EESCAPE     /* Trailing backslash */
#define REG_ESUBREG     M_ESUBMATCH   /* Invalid backreference */
#define REG_EBRACK      M_EBRACK      /* Unmatched [ or [: */
#define REG_EPAREN      M_EPAREN      /* Unmatched ( or \( */
#define REG_ECOLLATE    M_ECOLLATE    /* Invalid collating element */
#define REG_ECTYPE      M_ECTYPE      /* Invalid character class name */
#define REG_BADBR       M_BADBR       /* Invalid content of \{\} */
#define REG_EBRACE      M_EBRACE      /* Unmatched \{ or \} */
#define REG_ERANGE      M_ERANGE      /* Invalid range endpoint */
#define REG_ESPACE      M_ESPACE      /* Out of memory */
#define REG_BADRPT      M_BADRPT      /* Invalid repetition operator */
#define REG_EDEPTH      M_EDEPTH      /* Recursion limit */
#define REG_EFLAGS      M_EFLAGS      /* Invalid flags */
#define REG_EINTERNAL   M_EINTERNAL   /* Engine bug */
#define REG_EUNSAFE     M_EUNSAFE     /* ReDoS detected */
#define REG_ECALL       M_ECALL       /* Invalid subroutine */
#define REG_ECOND       M_ECOND       /* Invalid conditional */
#define REG_ELOOK       M_ELOOK       /* Invalid lookaround */
#define REG_EVERB       M_EVERB       /* Invalid verb */

/* ===================================================================== */
/* PCRE2 Constants                                                       */
/* ===================================================================== */

#define PCRE2_CASELESS      0x001
#define PCRE2_MULTILINE     0x002
#define PCRE2_DOTALL        0x004
#define PCRE2_UTF           0x008
#define PCRE2_UCP           0x010
#define PCRE2_NO_UTF_CHECK  0x020
#define PCRE2_NOTBOL           0x00000400
#define PCRE2_NOTEOL           0x00000800
#define PCRE2_PARTIAL_SOFT     0x00001000
#define PCRE2_PARTIAL_HARD     0x00002000
#define PCRE2_NOTEMPTY         0x00004000
#define PCRE2_NOTEMPTY_ATSTART 0x00008000
#define PCRE2_ANCHORED         0x00010000

#define PCRE2_ERROR_NOMATCH   -1
#define PCRE2_ERROR_BADDATA   -2
#define PCRE2_ERROR_BADMAGIC  -3
#define PCRE2_ERROR_NOMEMORY  -4

#define PCRE2_ZERO_TERMINATED (~(PCRE2_SIZE)0)

/* ===================================================================== */
/* Buffer Size                                                           */
/* ===================================================================== */

#ifndef REGEX_MAX_PATTERN
#define REGEX_MAX_PATTERN 4096
#endif

/* ===================================================================== */
/* Flag Translation: POSIX → JACL                                        */
/* ===================================================================== */

static inline mcomp_flag_t __reg_to_mcomp(int fl) {
	switch (fl & (REG_EXTENDED | REG_PCRE2 | REG_RE2)) {
		case REG_PCRE2:     return MCOMP_PCRE2;
		case REG_EXTENDED:  return MCOMP_EXTENDED;
		case REG_RE2:       return MCOMP_RE2;
		default:            return MCOMP_BASIC;
	}
}

static inline mexec_flag_t __reg_to_mexec(int fl) {
	mexec_flag_t e = ((fl & REG_RE2)      ? MEXEC_RE2 : (fl & REG_PCRE2)    ? MEXEC_PCRE2 : MEXEC_POSIX);

	if (fl & REG_ICASE)     e |= MEXEC_ICASE;
	if (fl & REG_NEWLINE)   e |= MEXEC_NEWLINE;
	if (fl & REG_NOTBOL)    e |= MEXEC_NOTBOL;
	if (fl & REG_NOTEOL)    e |= MEXEC_NOTEOL;
	if (fl & REG_DOTALL)    e |= MEXEC_DOTALL;
	if (fl & REG_NOSUB)     e |= MEXEC_NOSUB;
	if (fl & REG_UTF)       e |= MEXEC_UTF;
	if (fl & REG_UCP)       e |= MEXEC_UCP;

	return e;
}

static inline mexec_flag_t __pcre2_to_mexec(uint32_t opts) {
	mexec_flag_t e = MEXEC_PCRE2;

	if (opts & PCRE2_CASELESS)  e |= MEXEC_ICASE;
	if (opts & PCRE2_MULTILINE) e |= MEXEC_MULTILINE;
	if (opts & PCRE2_DOTALL)    e |= MEXEC_DOTALL;
	if (opts & PCRE2_UTF)       e |= MEXEC_UTF;
	if (opts & PCRE2_UCP)       e |= MEXEC_UCP;
	if (opts & PCRE2_NOTBOL)           e |= MEXEC_NOTBOL;
	if (opts & PCRE2_NOTEOL)           e |= MEXEC_NOTEOL;
	if (opts & PCRE2_PARTIAL_SOFT)     e |= MEXEC_PARTSOFT;
	if (opts & PCRE2_PARTIAL_HARD)     e |= MEXEC_PARTHARD;

	return e;
}

/* ===================================================================== */
/* BRE → ERE Translation (POSIX BRE mode only)                           */
/* ===================================================================== */

static inline const char *__jacl_bre_trans(const char *bre, char *ere, int *len, int max_len) {
	const char *specials = ".?*+{}|()^$\\";
	int i = 0, cnt = 0;

	while (bre[i] && cnt < max_len - 1) {
		if (bre[i] == '\\') {
			i++;

			if (!bre[i]) { ere[cnt++] = '\\'; break; }
			if (strchr(specials, bre[i])) { ere[cnt++] = bre[i]; }
			else { ere[cnt++] = '\\'; ere[cnt++] = bre[i]; }
		} else if (strchr(specials, bre[i])) {
			ere[cnt++] = '\\';
			ere[cnt++] = bre[i];
		} else {
			ere[cnt++] = bre[i];
		}

		i++;
	}

	ere[cnt] = '\0'; *len = cnt;

	return ere;
}

/* ===================================================================== */
/* POSIX API                                                             */
/* ===================================================================== */

static inline int regcomp(regex_t *restrict re, const char *restrict pat, int fl) {
	if (!re || !pat) return REG_BADPAT;

	char buf[REGEX_MAX_PATTERN];

	if ((fl & (REG_EXTENDED | REG_PCRE2 | REG_RE2)) == 0) pat = __jacl_bre_trans(pat, buf, &(int){0}, REGEX_MAX_PATTERN);

	mcomp_flag_t mcomp = __reg_to_mcomp(fl);
	mexec_flag_t mexec = __reg_to_mexec(fl);
	int err = matchcomp(re, pat, mcomp);

	if (err != M_SUCCESS) return err;

	re->eflags = mexec;

	return REG_SUCCESS;
}

static inline int regexec(const regex_t *restrict re, const char *restrict s, size_t nm, regmatch_t pm[restrict], int ef) {
	if (!re || !s) return REG_BADPAT;

	mexec_flag_t mexec = re->eflags | __reg_to_mexec(ef);

	return matchexec( re, s, s + strlen(s), (match_find_t *)pm, nm, mexec);
}

static inline size_t regerror(int ec, const regex_t *restrict re, char *restrict buf, size_t sz) {
	static const char *msgs[] = {
		[REG_SUCCESS]     = "Success",
		[REG_NOMATCH]     = "No match",
		[REG_BADPAT]      = "Bad pattern",
		[REG_ECOLLATE]    = "Bad collation",
		[REG_ECTYPE]      = "Bad class",
		[REG_EESCAPE]     = "Bad escape",
		[REG_ESUBREG]     = "Bad sub",
		[REG_EBRACK]      = "Bad bracket",
		[REG_EPAREN]      = "Unmatched paren",
		[REG_EBRACE]      = "Unmatched brace",
		[REG_BADBR]       = "Bad repeat",
		[REG_ERANGE]      = "Bad range",
		[REG_ESPACE]      = "Memory exhausted",
		[REG_BADRPT]      = "Bad repeat",
		[REG_EDEPTH]      = "Recursion depth exceeded",
		[REG_EFLAGS]      = "Invalid flags",
		[REG_EINTERNAL]   = "Internal error",
		[REG_EUNSAFE]     = "Unsafe pattern",
		[REG_ECALL]       = "Invalid call",
		[REG_ECOND]       = "Invalid conditional",
		[REG_ELOOK]       = "Invalid lookaround",
		[REG_EVERB]       = "Invalid verb"
	};

	const char *msg = (ec >= 0 && ec < M_LAST && msgs[ec]) ? msgs[ec] : "Unknown error";

	size_t len = strlen(msg) + 1;
	if (buf && sz > 0) {
		strncpy(buf, msg, sz - 1);
		buf[sz - 1] = '\0';
	}

	return len;
}

static inline void regfree(regex_t *restrict re) { matchfree(re); }

/* ===================================================================== */
/* PCRE2 API                                                             */
/* ===================================================================== */

static inline pcre2_code *pcre2_compile( const char *pattern, PCRE2_SIZE len, uint32_t options, int *errcode, PCRE2_SIZE *erroffset, void *gctx) {
	(void)gctx;

	if (!pattern) {
		if (errcode) *errcode = PCRE2_ERROR_BADDATA;

		return NULL;
	}

	if (len == PCRE2_ZERO_TERMINATED) len = strlen(pattern);

	pcre2_code *re = calloc(1, sizeof(pcre2_code));

	if (!re) {
		if (errcode) *errcode = PCRE2_ERROR_NOMEMORY;

		return NULL;
	}

	int err = matchcomp(re, pattern, MCOMP_PCRE2);

	if (err != M_SUCCESS) {
		if (errcode) *errcode = err;
		if (erroffset) *erroffset = 0;

		free(re);

		return NULL;
	}

	re->eflags = __pcre2_to_mexec(options);

	return re;
}

static inline int pcre2_match(const pcre2_code *re, const uint8_t *subject, PCRE2_SIZE length, PCRE2_SIZE startoffset, int options, pcre2_match_data *md, void *mctx) {
	(void)mctx;

	if (!re || !subject || !md) return PCRE2_ERROR_BADDATA;

	match_find_t matches[MATCH_MAX_GROUPS];
	mexec_flag_t eflags = __pcre2_to_mexec(options);
	const uint8_t *start = subject + startoffset;
	PCRE2_SIZE remaining = length - startoffset;

	int err = matchexec(re, (const char *)start, (const char *)start + remaining, matches, MATCH_MAX_GROUPS, eflags);

	if (err == M_SUCCESS) {
		memcpy(md->matches, matches, sizeof(matches));

		md->count = re->m_nsub + 1;

		return md->count;
	}

	return PCRE2_ERROR_NOMATCH;
}

static inline pcre2_match_data *pcre2_match_data_create(uint32_t n, void *gctx) {
	(void)n;
	(void)gctx;

	return calloc(1, sizeof(pcre2_match_data));
}

static inline void pcre2_match_data_free(pcre2_match_data *md) { free(md); }
static inline void pcre2_code_free(pcre2_code *re) { matchfree(re); free(re); }

/* ===================================================================== */
/* RE2 API                                                               */
/* ===================================================================== */

static inline re2_t *re2comp(const char *pattern) {
	if (!pattern) return NULL;

	re2_t *re = calloc(1, sizeof(re2_t));

	if (!re) return NULL;

	int err = matchcomp(re, pattern, MCOMP_RE2);

	if (err != M_SUCCESS) {
		free(re);

		return NULL;
	}

	re->eflags = MEXEC_RE2;

	return re;
}

static inline int re2match(const re2_t *re, const char *text, size_t textlen) {
	if (!re || !text) return 0;

	match_find_t matches[1];

	int err = matchexec(re, text, text + textlen, matches, 1, re->eflags);

	return (err == M_SUCCESS) ? 1 : 0;
}

static inline int re2fullmatch(const re2_t *re, const char *text, size_t textlen) {
	if (!re || !text) return 0;

	match_find_t matches[1];

	int err = matchexec(re, text, text + textlen, matches, 1, re->eflags);

	return (err == M_SUCCESS && matches[0].rm_so == 0 && (size_t)matches[0].rm_eo == textlen) ? 1 : 0;
}

static inline void re2free(re2_t *re) { matchfree(re); free(re); }

#ifdef __cplusplus
}
#endif

#endif /* _REGEX_H */
