// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef STDALIGN_H
#define STDALIGN_H

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
	/* C11: use native specifiers */
	#define alignas(e)	 _Alignas(e)
	#define alignof(e)	 _Alignof(e)
#else
	/* Pre-C11: no-op or compiler-specific */
	#if defined(__GNUC__) || defined(__clang__)
		#define alignas(e)	 __attribute__((aligned(e)))
		#define alignof(e)	 __alignof__
	#else
		#define alignas(e)	 /* unsupported */
		#define alignof(e)	 /* unsupported */
	#endif
#endif


#endif /* STDALIGN_H */

