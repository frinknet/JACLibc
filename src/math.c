// (c) 2025 FRINKnet & Friends – MIT licence

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

double j0(double x) {
	double ax = fabs(x);

	if (ax < 8.0) {
		double y = x * x;
		double n = 57568490574.0 + y * (-13362590354.0 + y * (651619640.7 + y * (-11214424.18 + y * (77392.33017 + y * (-184.9052456)))));
		double d = 57568490411.0 + y * (1029532985.0 + y * (9494680.718 + y * (59272.64853 + y * (267.8532712 + y))));

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

#ifdef __cplusplus
}
#endif
