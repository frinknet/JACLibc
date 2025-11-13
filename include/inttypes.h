/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef INTTYPES_H
#define INTTYPES_H
#pragma once

#include <config.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#if JACL_HAS_C23
#define __STDC_VERSION_INTTYPES_H__ 202311L
#elif JACL_HAS_C11
#define __STDC_VERSION_INTTYPES_H__ 201112L
#elif JACL_HAS_C99
#define __STDC_VERSION_INTTYPES_H__ 199901L
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if JACL_HAS_C99
typedef struct {
	intmax_t quot;
	intmax_t rem;
} imaxdiv_t;

static inline imaxdiv_t imaxdiv(intmax_t numer, intmax_t denom) {
	imaxdiv_t r;

	r.quot = numer / denom;
	r.rem  = numer % denom;

	if (r.rem != 0 && ((numer < 0 && r.rem > 0) || (numer > 0 && r.rem < 0))) {
		r.quot -= (denom > 0) ? 1 : -1;
		r.rem += denom;
	}

	return r;
}
static inline intmax_t strtoimax(const char *nptr, char **endptr, int base) {
	return (intmax_t)strtoll(nptr, endptr, base);
}
static inline uintmax_t strtoumax(const char *nptr, char **endptr, int base) {
	return (uintmax_t)strtoull(nptr, endptr, base);
}
#endif /* JACL_HAS_C99 */

// Format for printf
#define PRId8		"hhd"
#define PRIi8		"hhi"
#define PRIo8		"hho"
#define PRIu8		"hhu"
#define PRIx8		"hhx"
#define PRIX8		"hhX"

#define PRId16	"hd"
#define PRIi16	"hi"
#define PRIo16	"ho"
#define PRIu16	"hu"
#define PRIx16	"hx"
#define PRIX16	"hX"

#define PRId32	"d"
#define PRIi32	"i"
#define PRIo32	"o"
#define PRIu32	"u"
#define PRIx32	"x"
#define PRIX32	"X"

#if JACL_HAS_INT64
	#define PRId64 "lld"
	#define PRIi64 "lli"
	#define PRIo64 "llo"
	#define PRIu64 "llu"
	#define PRIx64 "llx"
	#define PRIX64 "llX"
#endif

// MAX printf format
#if JACL_HAS_INT64
	#define PRIdMAX  PRId64
	#define PRIiMAX  PRIi64
	#define PRIoMAX  PRIo64
	#define PRIuMAX  PRIu64
	#define PRIxMAX  PRIx64
	#define PRIXMAX  PRIX64
#elif JACL_HAS_INT32
	#define PRIdMAX  PRId32
	#define PRIiMAX  PRIi32
	#define PRIoMAX  PRIo32
	#define PRIuMAX  PRIu32
	#define PRIxMAX  PRIx32
	#define PRIXMAX  PRIX32
#else
	#define PRIdMAX  PRId16
	#define PRIiMAX  PRIi16
	#define PRIoMAX  PRIo16
	#define PRIuMAX  PRIu16
	#define PRIxMAX  PRIx16
	#define PRIXMAX  PRIX16
#endif

// PTR printf format
#if JACL_64BIT
  #define PRIdPTR PRId64
  #define PRIiPTR PRIi64
  #define PRIoPTR PRIo64
  #define PRIuPTR PRIu64
  #define PRIxPTR PRIx64
  #define PRIXPTR PRIX64
#elif JACL_32BIT
  #define PRIdPTR PRId32
  #define PRIiPTR PRIi32
  #define PRIoPTR PRIo32
  #define PRIuPTR PRIu32
  #define PRIxPTR PRIx32
  #define PRIXPTR PRIX32
#elif JACL_16BIT
  #define PRIdPTR PRId16
  #define PRIiPTR PRIi16
  #define PRIoPTR PRIo16
  #define PRIuPTR PRIu16
  #define PRIxPTR PRIx16
  #define PRIXPTR PRIX16
#else
  #define PRIdPTR PRId8
  #define PRIiPTR PRIi8
  #define PRIoPTR PRIo8
  #define PRIuPTR PRIu8
  #define PRIxPTR PRIx8
  #define PRIXPTR PRIX8
#endif

// Scanf macros
#define SCNd8		 "hhd"
#define SCNi8		 "hhi"
#define SCNo8		 "hho"
#define SCNu8		 "hhu"
#define SCNx8		 "hhx"

#define SCNd16	 "hd"
#define SCNi16	 "hi"
#define SCNo16	 "ho"
#define SCNu16	 "hu"
#define SCNx16	 "hx"

#define SCNd32	 "d"
#define SCNi32	 "i"
#define SCNo32	 "o"
#define SCNu32	 "u"
#define SCNx32	 "x"

#if JACL_HAS_INT64
	#define SCNd64 "lld"
	#define SCNi64 "lli"
	#define SCNo64 "llo"
	#define SCNu64 "llu"
	#define SCNx64 "llx"
#endif

// MAX scanf format
#if JACL_HAS_INT64
	#define SCNdMAX  SCNd64
	#define SCNiMAX  SCNi64
	#define SCNoMAX  SCNo64
	#define SCNuMAX  SCNu64
	#define SCNxMAX  SCNx64
#elif JACL_HAS_INT32
	#define SCNdMAX  SCNd32
	#define SCNiMAX  SCNi32
	#define SCNoMAX  SCNo32
	#define SCNuMAX  SCNu32
	#define SCNxMAX  SCNx32
#else
	#define SCNdMAX  SCNd16
	#define SCNiMAX  SCNi16
	#define SCNoMAX  SCNo16
	#define SCNuMAX  SCNu16
	#define SCNxMAX  SCNx16
	#define SCNxMAX  SCNx8
#endif

// PTR scanf format
#if JACL_64BIT
  #define SCNdPTR SCNd64
  #define SCNiPTR SCNi64
  #define SCNoPTR SCNo64
  #define SCNuPTR SCNu64
  #define SCNxPTR SCNx64
#elif JACL_32BIT
  #define SCNdPTR SCNd32
  #define SCNiPTR SCNi32
  #define SCNoPTR SCNo32
  #define SCNuPTR SCNu32
  #define SCNxPTR SCNx32
#elif JACL_16BIT
  #define SCNdPTR SCNd16
  #define SCNiPTR SCNi16
  #define SCNoPTR SCNo16
  #define SCNuPTR SCNu16
  #define SCNxPTR SCNx16
#else
  #define SCNdPTR SCNd8
  #define SCNiPTR SCNi8
  #define SCNoPTR SCNo8
  #define SCNuPTR SCNu8
  #define SCNxPTR SCNx8
#endif

#ifdef __cplusplus
}
#endif

#endif /* INTTYPES_H */
