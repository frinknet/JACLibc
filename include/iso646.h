/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef ISO646_H
#define ISO646_H
#pragma once

#if !JACL_HAS_C99
  #error "iso646.h requires C99"
#endif

/* Alternative operator spellings */
#ifndef __cplusplus
#define and			 &&
#define and_eq	 &=
#define bitand	 &
#define bitor		 |
#define compl		 ~
#define not			 !
#define not_eq	 !=
#define or			 ||
#define or_eq		 |=
#define xor			 ^
#define xor_eq	 ^=
#endif

#endif /* ISO646_H */
