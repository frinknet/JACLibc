/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef INTTYPES_H
#define INTTYPES_H
#pragma once

#include <config.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#if !JACL_HAS_C99
  #error "inttypes.h requires C99"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum‐width integer types */
typedef intmax_t	 imax_t;
typedef uintmax_t  umax_t;

/* imaxdiv_t and imaxdiv */
typedef struct { imax_t quot; imax_t rem; } imaxdiv_t;

static inline imaxdiv_t imaxdiv(imax_t numer, imax_t denom) {
	imaxdiv_t r;
	r.quot = numer / denom;
	r.rem  = numer % denom;

	return r;
}

/* Conversion functions */
static inline intmax_t strtoimax(const char *nptr, char **endptr, int base) {
	return (intmax_t)strtoll(nptr, endptr, base);
}

static inline uintmax_t strtoumax(const char *nptr, char **endptr, int base) {
	return (uintmax_t)strtoull(nptr, endptr, base);
}

/* Format specifiers for printf */
#define PRId8		 "hhd"
#define PRIi8		 "hhi"
#define PRIo8		 "hho"
#define PRIu8		 "hhu"
#define PRIx8		 "hhx"
#define PRIX8		 "hhX"
#define PRId16	 "hd"
#define PRIi16	 "hi"
#define PRIo16	 "ho"
#define PRIu16	 "hu"
#define PRIx16	 "hx"
#define PRIX16	"hX"
#define PRId32	 "d"
#define PRIi32	 "i"
#define PRIo32	 "o"
#define PRIu32	 "u"
#define PRIx32	 "x"
#define PRIX32	"X"

#if JACL_64BIT && !JACL_OS_WINDOWS
	#define PRId64 "ld"
	#define PRIi64 "li"
	#define PRIo64 "lo"
	#define PRIu64 "lu"
	#define PRIx64 "lx"
	#define PRIX64 "lX"
#else
	#define PRId64 "lld"
	#define PRIi64 "lli"
	#define PRIo64 "llo"
	#define PRIu64 "llu"
	#define PRIx64 "llx"
	#define PRIX64 "llX"
#endif

#define PRIdMAX  PRId64
#define PRIiMAX  PRIi64
#define PRIoMAX  PRIo64
#define PRIuMAX  PRIu64
#define PRIxMAX  PRIx64
#define PRIXMAX  PRIX64

#if JACL_32BIT
  #define PRIdPTR PRId32
  #define PRIiPTR PRIi32
  #define PRIoPTR PRIo32
  #define PRIuPTR PRIu32
  #define PRIxPTR PRIx32
  #define PRIXPTR PRIX32
#else
  #define PRIdPTR PRId64
  #define PRIiPTR PRIi64
  #define PRIoPTR PRIo64
  #define PRIuPTR PRIu64
  #define PRIxPTR PRIx64
  #define PRIXPTR PRIX64
#endif

	/* Scanf macros */
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

#if JACL_64BIT && !JACL_OS_WINDOWS
	#define SCNd64 "ld"
	#define SCNi64 "li"
	#define SCNo64 "lo"
	#define SCNu64 "lu"
	#define SCNx64 "lx"
#else
	#define SCNd64 "lld"
	#define SCNi64 "lli"
	#define SCNo64 "llo"
	#define SCNu64 "llu"
	#define SCNx64 "llx"
#endif

#define SCNdMAX  SCNd64
#define SCNiMAX  SCNi64
#define SCNoMAX  SCNo64
#define SCNuMAX  SCNu64
#define SCNxMAX  SCNx64

#if JACL_32BIT
  #define SCNdPTR SCNd32
  #define SCNiPTR SCNi32
  #define SCNoPTR SCNo32
  #define SCNuPTR SCNu32
  #define SCNxPTR SCNx32
#else
  #define SCNdPTR SCNd64
  #define SCNiPTR SCNi64
  #define SCNoPTR SCNo64
  #define SCNuPTR SCNu64
  #define SCNxPTR SCNx64
#endif

#ifdef __cplusplus
}
#endif

#endif // INTTYPES_H
