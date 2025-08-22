// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef MATH_H
#define MATH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Constants */
#define HUGE_VAL		(__builtin_huge_val())
#define HUGE_VALF		((float)HUGE_VAL)
#define HUGE_VALL		((long double)HUGE_VAL)
#define INFINITY		(__builtin_inff())
#define NAN					(__builtin_nanf(""))

#define M_E					2.7182818284590452354
#define M_LOG2E			1.4426950408889634074
#define M_LOG10E		0.43429448190325182765
#define M_LN2				0.69314718055994530942
#define M_LN10			2.30258509299404568402
#define M_PI				3.14159265358979323846
#define M_PI_2			1.57079632679489661923
#define M_PI_4			0.78539816339744830962
#define M_1_PI			0.31830988618379067154
#define M_2_PI			0.63661977236758134308
#define M_2_SQRTPI	1.12837916709551257390
#define M_SQRT2			1.41421356237309504880
#define M_SQRT1_2		0.70710678118654752440

// Classification & Comparison
#define FP_INFINITE		1
#define FP_NAN				2
#define FP_NORMAL			3
#define FP_SUBNORMAL	4
#define FP_ZERO				5

/* Floating-point operation speed indicators */
#define FP_FAST_FMA			1
#define FP_FAST_FMAF		1
#define FP_FAST_FMAL		1

/* ilogb domain error return values */
#define FP_ILOGB0				INT_MIN
#define FP_ILOGBNAN			INT_MIN

/* math_errhandling modes */
#define MATH_ERRNO			1
#define MATH_ERREXCEPT	2
#define math_errhandling	(MATH_ERRNO | MATH_ERREXCEPT)

int isnan(double x);
int isinf(double x);
int isfinite(double x);
int isnormal(double x);
int isgreater(double x, double y);
int isgreaterequal(double x, double y);
int isless(double x, double y);
int islessequal(double x, double y);
int islessgreater(double x, double y);
int isunordered(double x, double y);

int signbit(double x);
int fpclassify(double x);

/* Generator macros for 1-, 2-, and 3-arg functions */
#define MATH1(name)										\
	double name(double x);							\
	float  name##f(float x);						\
	long double name##l(long double x);

#define MATH2(name)									\
	double name(double x, double y);	\
	float  name##f(float x, float y);	\
	long double name##l(long double x, long double y);


#define MATH3(name)														\
	double name(double x, double y, double z);	\
	float  name##f(float x, float y, float z);	\
	long double name##l(long double x, long double y, long double z);

/* Basic operations */
MATH1(fabs);
MATH1(ceil);
MATH1(floor);
MATH1(trunc);
MATH1(round);
MATH1(rint);
MATH1(nearbyint);
MATH2(copysign);
MATH2(fdim);
MATH2(fmax);
MATH2(fmin);
MATH3(fma);

double nan(const char *s);
float  nanf(const char *s);
long double nanl(const char *s);

long	 lround(double x);
long long llround(double x);
long	 lrint(double x);
long long llrint(double x);

/* Exponential & logarithmic */
MATH1(exp);
MATH1(exp2);
MATH1(expm1);
MATH1(log);
MATH1(log2);
MATH1(log10);
MATH1(log1p);
MATH1(logb);
MATH2(ldexp);
MATH2(scalbn);
MATH2(scalbln);
MATH1(ilogb);

double frexp(double x, int *exp);
float  frexpf(float x, int *exp);
long double frexpl(long double x, int *exp);

double modf(double x, double *ip);
float  modff(float x, float *ip);
long double modfl(long double x, long double *ip);

/* Power & root */
MATH2(pow);
MATH1(sqrt);
MATH1(cbrt);
MATH2(hypot);

/* Trigonometric */
MATH1(sin);
MATH1(cos);
MATH1(tan);
MATH1(asin);
MATH1(acos);
MATH1(atan);
MATH2(atan2);

/* Hyperbolic */
MATH1(sinh);
MATH1(cosh);
MATH1(tanh);
MATH1(asinh);
MATH1(acosh);
MATH1(atanh);

/* Error & gamma */
MATH1(erf);
MATH1(erfc);
MATH1(lgamma);
MATH1(tgamma);

/* Remainder & quotient */
MATH2(fmod);
MATH2(remainder);

double remquo(double x, double y, int *q);
float  remquof(float x, float y, int *q);
long double remquol(long double x, long double y, int *q);

/* Next */
MATH2(nextafter);
MATH2(nexttoward);

#undef MATH1
#undef MATH2
#undef MATH3

double j0(double x);
double j1(double x);
double jn(int n, double x);

double y0(double x);
double y1(double x);
double yn(int n, double x);

extern int signgam;

#ifdef __cplusplus
}
#endif

#endif /* MATH_H */

