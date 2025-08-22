// (c) 2025 FRINKnet & Friends – MIT licence
#ifndef MATH_C
#define MATH_C

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

int isnan(double x)											{ return __builtin_isnan(x); }
int isinf(double x)											{ return __builtin_isinf(x); }
int isfinite(double x)									{ return __builtin_isfinite(x); }
int isnormal(double x)									{ return __builtin_isnormal(x); }
int isgreater(double x, double y)				{ return __builtin_isgreater(x,y); }
int isgreaterequal(double x, double y)	{ return __builtin_isgreaterequal(x,y); }
int isless(double x, double y)					{ return __builtin_isless(x,y); }
int islessequal(double x, double y)			{ return __builtin_islessequal(x,y); }
int islessgreater(double x, double y)		{ return __builtin_islessgreater(x,y); }
int isunordered(double x, double y)			{ return __builtin_isunordered(x,y); }

int signbit(double x)										{ return __builtin_signbit(x); }
int fpclassify(double x)								{ return __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x); }

/* Generator macros for 1-, 2-, and 3-arg functions */
#ifdef __wasm__
#define MATH1(name)																										\
	double name(double x)					 { return __builtin_##name(x); } \
	float  name##f(float x)				 { return __builtin_##name##f(x); } \
	long double name##l(long double x) { return (long double)__builtin_##name((double)x); }

#define MATH2(name)																										\
	double name(double x, double y)				{ return __builtin_##name(x,y); } \
	float  name##f(float x, float y)			{ return __builtin_##name##f(x,y); } \
	long double name##l(long double x, long double y) { return (long double)__builtin_##name((double)x,(double)y); }

#define MATH3(name)																										\
	double name(double x, double y, double z)					 { return __builtin_##name(x,y,z); } \
	float  name##f(float x, float y, float z)						{ return __builtin_##name##f(x,y,z); } \
	long double name##l(long double x, long double y, long double z) { return (long double)__builtin_##name((double)x,(double)y,(double)z); }
#else
#define MATH1(name)																										\
	double name(double x)					 { return __builtin_##name(x); } \
	float  name##f(float x)				 { return __builtin_##name##f(x); } \
	long double name##l(long double x) { return __builtin_##name##l(x); }

#define MATH2(name)																										\
	double name(double x, double y)				{ return __builtin_##name(x,y); } \
	float  name##f(float x, float y)			{ return __builtin_##name##f(x,y); } \
	long double name##l(long double x, long double y) { return __builtin_##name##l(x,y); }

#define MATH3(name)																										\
	double name(double x, double y, double z)					 { return __builtin_##name(x,y,z); } \
	float  name##f(float x, float y, float z)						{ return __builtin_##name##f(x,y,z); } \
	long double name##l(long double x, long double y, long double z) { return __builtin_##name##l(x,y,z); }
#endif

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

double nan(const char *s)			 { return __builtin_nan(s); }
float  nanf(const char *s)		 { return __builtin_nanf(s); }
long double nanl(const char *s){ return __builtin_nanl(s); }

long	 lround(double x)			{ return __builtin_lround(x); }
long long llround(double x) { return __builtin_llround(x); }
long	 lrint(double x)			{ return __builtin_lrint(x); }
long long llrint(double x)	{ return __builtin_llrint(x); }

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

double frexp(double x, int *exp)				 { return __builtin_frexp(x, exp); }
float  frexpf(float x, int *exp)				 { return __builtin_frexpf(x, exp); }
long double frexpl(long double x, int *exp){ return __builtin_frexpl(x, exp); }

double modf(double x, double *ip)							{ return __builtin_modf(x, ip); }
float  modff(float x, float *ip)							{ return __builtin_modff(x, ip); }
long double modfl(long double x, long double *ip){ return __builtin_modfl(x, ip); }

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

double remquo(double x, double y, int *q)							{ return __builtin_remquo(x, y, q); }
float  remquof(float x, float y, int *q)							{ return __builtin_remquof(x, y, q); }
long double remquol(long double x, long double y, int *q){ return __builtin_remquol(x, y, q); }

/* Next */
MATH2(nextafter);
MATH2(nexttoward);

#undef MATH1
#undef MATH2
#undef MATH3

double j0(double x) {
	double ax = fabs(x);

	if (ax < 8.0) {
		double y = x * x;
		double n = 57568490574.0 + y * (-13362590354.0 + y * (651619640.7 + y * (-11214424.18 + y * (77392.33017 + y * (-184.9052456)))));
		double d = 57568490411.0 + y * (1029532985.0 + y * (9494680.718 + y * (59272.64853 + y * (267.8532712 + y * 1.0))));

		return n / d;
	} else {
		double z = 8.0 / ax;
		double y = z * z;
		double xx = ax - 0.785398164;
		double p = 1.0 + y * (-0.1098628627e-2 + y * (0.2734510407e-4 + y * (-0.2073370639e-5 + y * 0.2093887211e-6)));
		double q = -0.1562499995e-1 + y * (0.1430488765e-3 + y * (-0.6911147651e-5 + y * (0.7621095161e-6 - y * 0.934945152e-7)));

		return sqrt(0.636619772/ax) * (p * cos(xx) - z * q * sin(xx));
	}
}
double j1(double x) {
	double ax = fabs(x);

	if (ax < 8.0) {
		double y = x * x;
		double n = x * 72362614232.0 + y * (-7895059235.0 + y * (242396853.1 + y * (-2972611.439 + y * (15704.48260 + y * (-30.16036606)))));
		double d = 144725228442.0 + y * (2300535178.0 + y * (18583304.74 + y * (99447.43394 + y * (376.9991397 + y))));

		return n / d;
	} else {
		double z = 8.0 / ax;
		double y = z * z;
		double xx = ax - 2.356194491;
		double p = 1.0 + y * (0.183105e-2 + y * (-0.3516396496e-4 + y * (0.2457520174e-5 + y * -0.240337019e-6)));
		double q = 0.04687499995 + y * (-0.2002690873e-3 + y * (0.8449199096e-5 + y * (-0.88228987e-6 + y * 0.105787412e-6)));
		double ans = sqrt(0.636619772/ax) * (p * cos(xx) - z * q * sin(xx));

		return x < 0.0 ? -ans : ans;
	}
}
double jn(int n, double x) {
	if (n == 0) return j0(x);
	if (n == 1) return j1(x);

	double tox = 2.0 / fabs(x);
	double bjm = j0(x), bj = j1(x), bjp;

	for (int j = 1; j < n; j++) {
		bjp = j * tox * bj - bjm;
		bjm = bj;
		bj = bjp;
	}

	return bj;
}

double y0(double x) {
	if (x < 8.0) {
		double j0val = j0(x);
		double y = x * x;
		double num = -2957821389.0 + y * (7062834065.0 + y * (-512359803.6 + y * (10879881.29 + y * (-86327.92757 + y * 228.4622733))));
		double den = 40076544269.0 + y * (745249964.8 + y * (7189466.438 + y * (47447.26470 + y * (226.1030244 + y))));

		return (2.0/M_PI) * (log(x/2.0) * j0val + num/den);
	} else {
		double z = 8.0/x, y = z*z;
		double xx = x - 0.785398164;
		double p = 1.0 + y * (-0.1098628627e-2 + y * (0.2734510407e-4 + y * (-0.2073370639e-5 + y * 0.2093887211e-6)));
		double q = -0.1562499995e-1 + y * (0.1430488765e-3 + y * (-0.6911147651e-5 + y * (0.7621095161e-6  + y * -0.934945152e-7)));

		return sqrt(0.636619772/x) * (p*sin(xx) + z*q*cos(xx));
	}
}
double y1(double x) {
	if (x < 8.0) {
		double j1val = j1(x);
		double y = x * x;
		double num = x * (-0.4900604943e13 + y * (0.1275274390e13 + y * (-0.5153438139e11 + y * (0.7349264551e9 + y * (-0.4237922726e7 + y * 0.8511937935e4)))));
		double den = 0.2499580570e14 + y * (0.4244419664e12 + y * (0.3733650367e10 + y * (0.2245904002e8 + y * (0.1020426050e6 + y * (0.3549632885e3 + y)))));

		return (2.0/M_PI) * (log(x/2.0) * j1val + num/den);
	} else {
		double z = 8.0/x, y = z*z;
		double xx = x - 2.356194491;
		double p = 1.0 + y * (0.183105e-2 + y * (-0.3516396496e-4 + y * (0.2457520174e-5 + y * -0.240337019e-6)));
		double q = 0.04687499995 + y * (-0.2002690873e-3 + y * (0.8449199096e-5 + y * (-0.88228987e-6 + y * 0.105787412e-6)));

		return sqrt(0.636619772/x) * (p*sin(xx) + z*q*cos(xx));
	}
}
double yn(int n, double x) {
	if (n == 0) return y0(x);
	if (n == 1) return y1(x);

	double tox = 2.0/x, by = y1(x), bym = y0(x), byp;

	for (int j = 1; j < n; j++) {
		byp = j * tox * by - bym;
		bym = by;
		by = byp;
	}

	return by;
}

extern int signgam;

#ifdef __cplusplus
}
#endif

#endif /* MATH_H */
