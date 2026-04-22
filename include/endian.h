/* (c) 2026 FRINKnet & Friends – MIT licence */
#ifndef _ENDIAN_H
#define _ENDIAN_H
#pragma once

#include <config.h>
#include <stdbit.h>

#ifdef __cplusplus
extern "C" {
#endif

// Byte order constants (glibc-style)

// Low-level byte swaps
static inline uint16_t __bswap_16(uint16_t x) {
	return (x >> 8) | (x << 8);
}

static inline uint32_t __bswap_32(uint32_t x) {
	return ((x >> 24) | ((x >>  8) & 0x0000FF00) |
			((x <<  8) & 0x00FF0000) | (x << 24));
}

static inline uint64_t __bswap_64(uint64_t x) {
	return ((x >> 56) | ((x >> 40) & 0x000000000000FF00ULL) |
			((x >> 24) & 0x0000000000FF0000ULL) |
			((x >>  8) & 0x00000000FF000000ULL) |
			((x <<  8) & 0x000000FF00000000ULL) |
			((x << 24) & 0x0000FF0000000000ULL) |
			((x << 40) & 0x00FF000000000000ULL) | (x << 56));
}

// PDP/VAX middle-endian swaps
static inline uint32_t __pdp_to_le32(uint32_t x) {
	return ((x & 0x00FF0000) >> 16) | ((x & 0x0000FF00) << 16) |
	       ((x & 0x000000FF) <<  8) | ((x & 0xFF000000) >>  8);
}

static inline uint64_t __pdp_to_le64(uint64_t x) {
	return ((uint64_t)__pdp_to_le32((uint32_t)x)     ) |
	       ((uint64_t)__pdp_to_le32((uint32_t)(x>>32)) << 32);
}

// Network byte order (always big-endian)
#define htonl(x)  htobe32(x)
#define ntohl(x)  be32toh(x)
#define htons(x)  htobe16(x)
#define ntohs(x)  be16toh(x)

// Host ↔ canonical dispatch
#if JACL_HAS_LE
	#define htole16(x) ((uint16_t)(x))
	#define htole32(x) ((uint32_t)(x))
	#define htole64(x) ((uint64_t)(x))
	#define htobe16(x) __bswap_16(x)
	#define htobe32(x) __bswap_32(x)
	#define htobe64(x) __bswap_64(x)
	#define le16toh(x) ((uint16_t)(x))
	#define le32toh(x) ((uint32_t)(x))
	#define le64toh(x) ((uint64_t)(x))
	#define be16toh(x) __bswap_16(x)
	#define be32toh(x) __bswap_32(x)
	#define be64toh(x) __bswap_64(x)
#elif JACL_HAS_BE
	#define htole16(x) __bswap_16(x)
	#define htole32(x) __bswap_32(x)
	#define htole64(x) __bswap_64(x)
	#define htobe16(x) ((uint16_t)(x))
	#define htobe32(x) ((uint32_t)(x))
	#define htobe64(x) ((uint64_t)(x))
	#define le16toh(x) __bswap_16(x)
	#define le32toh(x) __bswap_32(x)
	#define le64toh(x) __bswap_64(x)
	#define be16toh(x) ((uint16_t)(x))
	#define be32toh(x) ((uint32_t)(x))
	#define be64toh(x) ((uint64_t)(x))
#elif JACL_HAS_PDP
	#define htole16(x) ((uint16_t)(x))
	#define htole32(x) __pdp_to_le32(x)
	#define htole64(x) __pdp_to_le64(x)
	#define htobe16(x) __bswap_16(x)
	#define htobe32(x) __bswap_32(__pdp_to_le32(x))
	#define htobe64(x) __bswap_64(__pdp_to_le64(x))
	#define le16toh(x) ((uint16_t)(x))
	#define le32toh(x) __pdp_to_le32(x)
	#define le64toh(x) __pdp_to_le64(x)
	#define be16toh(x) __bswap_16(x)
	#define be32toh(x) __pdp_to_le32(__bswap_32(x))
	#define be64toh(x) __pdp_to_le64(__bswap_64(x))
#endif

#ifdef __cplusplus
}
#endif

#endif /* _ENDIAN_H */
