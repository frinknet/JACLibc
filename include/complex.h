/* (c) 2025 FRINKnet & Friends – MIT licence */
#ifndef COMPLEX_H
#define COMPLEX_H
#pragma once

#include <config.h>
#include <math.h>

#if !JACL_HAS_C99
  #error "complex.h requires C99 or later"
#endif
#ifdef __cplusplus
extern "C" {
#endif

#ifndef complex
# define complex _Complex
#endif

#ifndef I
# define I _Complex_I
#endif

#ifndef _Complex_I
	#if defined(__GNUC__) || defined(__clang__)
		#define _Complex_I (__extension__ 1.0iF)
	#else
		#define _Complex_I (1.0F * I)
	#endif
#endif

#if JACL_HAS_C11
	#define CMPLX(x, y) ((double complex)((double)(x) + I * (double)(y)))
	#define CMPLXF(x, y) ((float complex)((float)(x) + I * (float)(y)))
	#define CMPLXL(x, y) ((long double complex)((long double)(x) + I * (long double)(y)))
#endif

#define __jacl_cmath(F) \
	__jacl_##F(float,				f) \
	__jacl_##F(double,			) \
	__jacl_##F(long double, l)

/* ================================================================ */
/* Manipulation functions                                           */
/* ================================================================ */

#define __jacl_creal(type, suf) \
	static inline type creal##suf(type complex z) { \
		return __real__ z; \
	}

#define __jacl_cimag(type, suf) \
	static inline type cimag##suf(type complex z) { \
		return __imag__ z; \
	}

#define __jacl_conj(type, suf) \
	static inline type complex conj##suf(type complex z) { \
		return creal##suf(z) - I * cimag##suf(z); \
	}

// cproj(z) = z if finite, else infinite projection on Riemann sphere
#define __jacl_cproj(type, suf) \
	static inline type complex cproj##suf(type complex z) { \
		if (isfinite(creal##suf(z)) && isfinite(cimag##suf(z))) return z; \
		if (isnan(creal##suf(z)) || isnan(cimag##suf(z))) return z; \
		return (type complex)INFINITY + (type)0.0 * cimag##suf(z); \
	}

// sqrt(creal(z)^2 + cimag(z)^2)
#define __jacl_cabs(type, suf) \
	static inline type cabs##suf(type complex z) { \
		type r = (type)creal##suf(z); \
		type i = (type)cimag##suf(z); \
		return hypot##suf(r, i); \
	}

// atan2(cimag(z), creal(z))
#define __jacl_carg(type, suf) \
	static inline type carg##suf(type complex z) { \
		type r = (type)creal##suf(z); \
		type i = (type)cimag##suf(z); \
		return atan2##suf(i, r); \
	}

/* ================================================================ */
/* Trigonometry functions                                           */
/* ================================================================ */

// ccos(z) = cos(creal(z)) * cosh(cimag(z)) - I * sin(creal(z)) * sinh(cimag(z))
#define __jacl_ccos(type, suf) \
	static inline type complex ccos##suf(type complex z) { \
		type r = (type)creal##suf(z); \
		type i = (type)cimag##suf(z); \
		return cos##suf(r) * cosh##suf(i) - I * (sin##suf(r) * sinh##suf(i)); \
	}

// csin(z) = sin(creal(z)) * cosh(cimag(z)) + I * cos(creal(z)) * sinh(cimag(z))
#define __jacl_csin(type, suf) \
	static inline type complex csin##suf(type complex z) { \
		type r = (type)creal##suf(z); \
		type i = (type)cimag##suf(z); \
		return sin##suf(r) * cosh##suf(i) + I * (cos##suf(r) * sinh##suf(i)); \
	}

// ctan(z) = csin(z) / ccos(z)
#define __jacl_ctan(type, suf) \
	static inline type complex ctan##suf(type complex z) { \
		type complex c = ccos##suf(z); \
		if (c == (type complex)0) return (type complex)(0.0 / 0.0); /* NaN */ \
		return csin##suf(z) / c; \
	}

// cacos(z) = -I * clog(z + I * csqrt(1 - z*z))
#define __jacl_cacos(type, suf) \
	static inline type complex cacos##suf(type complex z) { \
		type complex temp = csqrt##suf((type complex)1 - z * z); \
		return -I * clog##suf(z + I * temp); \
	}

// casin(z) = -I * clog(I * z + csqrt(1 - z*z))
#define __jacl_casin(type, suf) \
	static inline type complex casin##suf(type complex z) { \
		type complex temp = csqrt##suf((type complex)1 - z * z); \
		return -I * clog##suf(I * z + temp); \
	}

// catan(z) = (I / 2) * clog((I + z) / (I - z))
#define __jacl_catan(type, suf) \
	static inline type complex catan##suf(type complex z) { \
		return (I / (type)2) * clog##suf((I + z) / (I - z)); \
	}

/* ================================================================ */
/* Hyperbolic functions                                             */
/* ================================================================ */

// ccosh(z) = cosh(creal(z)) * cos(cimag(z)) + I * sinh(creal(z)) * sin(cimag(z))
#define __jacl_ccosh(type, suf) \
	static inline type complex ccosh##suf(type complex z) { \
		type r = (type)creal##suf(z); \
		type i = (type)cimag##suf(z); \
		return cosh##suf(r) * cos##suf(i) + I * (sinh##suf(r) * sin##suf(i)); \
	}

// csinh(z) = sinh(creal(z)) * cos(cimag(z)) + I * cosh(creal(z)) * sin(cimag(z))
#define __jacl_csinh(type, suf) \
	static inline type complex csinh##suf(type complex z) { \
		type r = (type)creal##suf(z); \
		type i = (type)cimag##suf(z); \
		return sinh##suf(r) * cos##suf(i) + I * (cosh##suf(r) * sin##suf(i)); \
	}

// ctanh(z) = csinh(z) / ccosh(z)
#define __jacl_ctanh(type, suf) \
	static inline type complex ctanh##suf(type complex z) { \
		type complex c = ccosh##suf(z); \
		if (c == (type complex)0) return (type complex)(0.0 / 0.0); /* NaN */ \
		return csinh##suf(z) / c; \
	}

// cacosh(z) = clog(z + csqrt(z+1) * csqrt(z-1))
#define __jacl_cacosh(type, suf) \
	static inline type complex cacosh##suf(type complex z) { \
		return clog##suf(z + csqrt##suf(z + (type complex)1) * csqrt##suf(z - (type complex)1)); \
	}

// casinh(z) = clog(z + csqrt(z*z + 1))
#define __jacl_casinh(type, suf) \
	static inline type complex casinh##suf(type complex z) { \
		return clog##suf(z + csqrt##suf(z * z + (type complex)1)); \
	}

// catanh(z) = 0.5 * clog((1 + z) / (1 - z))
#define __jacl_catanh(type, suf) \
	static inline type complex catanh##suf(type complex z) { \
		return (type complex)0.5 * clog##suf(((type complex)1 + z) / ((type complex)1 - z)); \
	}

/* ================================================================ */
/* Exponential functions                                            */
/* ================================================================ */

// cexp(z) = exp(creal(z)) * (cos(cimag(z)) + I * sin(cimag(z)))
#define __jacl_cexp(type, suf) \
	static inline type complex cexp##suf(type complex z) { \
		type r = (type)creal##suf(z); \
		type i = (type)cimag##suf(z); \
		type e = exp##suf(r); \
		return e * cos##suf(i) + I * (e * sin##suf(i)); \
	}

// clog(z) = log(cabs(z)) + I * carg(z)
#define __jacl_clog(type, suf) \
	static inline type complex clog##suf(type complex z) { \
		return log##suf(cabs##suf(z)) + I * carg##suf(z); \
	}

/* cpow(z, w) = cexp(w * clog(z)) */
#define __jacl_cpow(type, suf) \
	static inline type complex cpow##suf(type complex z, type complex w) { \
		return cexp##suf(w * clog##suf(z)); \
	}

// csqrt(z) = cexp(0.5 * clog(z)) (principal branch)
#define __jacl_csqrt(type, suf) \
	static inline type complex csqrt##suf(type complex z) { \
		return cexp##suf((type complex)0.5 * clog##suf(z)); \
	}

// Manipulation Functions
__jacl_cmath(creal)
__jacl_cmath(cimag)
__jacl_cmath(conj)
__jacl_cmath(cproj)
__jacl_cmath(cabs)
__jacl_cmath(carg)

// Trogonometry Functions
__jacl_cmath(ccos)
__jacl_cmath(csin)
__jacl_cmath(ctan)
__jacl_cmath(cacos)
__jacl_cmath(casin)
__jacl_cmath(catan)

// Hyperbolic Functions
__jacl_cmath(ccosh)
__jacl_cmath(csinh)
__jacl_cmath(ctanh)
__jacl_cmath(cacosh)
__jacl_cmath(casinh)
__jacl_cmath(catanh)

// Exponential Functions
__jacl_cmath(cexp)
__jacl_cmath(clog)
__jacl_cmath(cpow)
__jacl_cmath(csqrt)

#ifdef __cplusplus
}
#endif

#endif /* COMPLEX_H */
