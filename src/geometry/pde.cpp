//
// Created by PC on 22.03.2025.
//

#include "pde.hpp"

#include <cmath>


BoundaryCondition::BoundaryCondition(RealFunction h): _h(std::move(h)) {
	zero = _h.isZero();
}

RobinBoundaryCondition::RobinBoundaryCondition(float a_N, float a, RealFunction h): BoundaryCondition(std::move(h)), alpha_N(a_N), alpha(a) {
	if (nearlyEqual(alpha_N, 0)) {
		alpha_N = 0;
		_h = _h/alpha;
		alpha = 1;
	}
	else {
		_h = _h/alpha_N;
		alpha = alpha/alpha_N;
		alpha_N = 1;
	}
}

RobinBoundaryCondition::RobinBoundaryCondition(float alpha_N, float alpha): RobinBoundaryCondition(alpha_N, alpha, RealFunction(0)) {
	zero = true;
}

WaveEquation1DCauchy::WaveEquation1DCauchy(RealFunction u0, RealFunction ut0, RealFunctionR2 f, float c)
: u0(std::move(u0)), ut0(std::move(ut0)), f(std::move(f)), c(c) {}

RealFunctionR2 WaveEquation1DCauchy::solution(int precision) const {
	return RealFunctionR2([F=f, g=u0, h=ut0, c=c, prec=precision](vec2 v) {
		float x = v.x;
		float t = v.y;
		float lambda0 = x - c*t;
		float lambda1 = x + c*t;

		return (g(lambda0) + g(lambda1))/2 + h.integral(lambda0, lambda1, prec)/(c*2) +
			RealFunction([F, c, x, precision=prec](float T) {
				return F.partially_integrate_along_x(x-c*T, x+c*T, (int)sqrt(precision))(T);
		}, prec).integral(0, t,  (int)sqrt(prec))/(2*c);
	}, f.getEps());
}

StringEquation::StringEquation(float c, float l, RealFunction u0, RealFunction ut0, int n_max, int precision)
: l(l),
  u0(std::move(u0)),
  ut0(std::move(ut0)),
  n_max(n_max),
  prec(precision),
  c(c)
{}

float StringEquation::omega(int n) const {
	return PI*n/l;
}

RealFunction StringEquation::Xn(int n) const {
	return RealFunction([omega=omega(n)](float x) {
		return std::sin(omega*x);
	}, u0.getEps());
}

float StringEquation::An(int n) const {
	return Xn(n).L2_product(u0, vec2(0, l), prec)*2/l;
}

float StringEquation::Bn(int n) const {
	return Xn(n).L2_product(ut0, vec2(0, l), prec)*2/(l*n*PI);
}

RealFunctionR2 StringEquation::simple_solution(int n) const {
	float A = An(n);
	float B = Bn(n);
	auto X = Xn(n);
	auto T = RealFunction([A, B, c=c, w=omega(n)](float t) {
		return A*std::cos(c*w*t) + B*std::sin(c*w*t);
	}, u0.getEps());
	return separated_product(X, T);
}

void StringEquation::set_l(float l1) {
	l = l1;
}

void StringEquation::set_c(float c1) {
	c = c1;
}

RealFunctionR2 StringEquation::solution() const {
	FourierSeriesR1 Fg = FourierSeriesR1(u0, vec2(0, l), 0, n_max, prec, false, true);
	FourierSeriesR1 Fh = FourierSeriesR1(ut0, vec2(0, l), 0, n_max, prec, false, true);
	RealFunctionR2 u = RealFunctionR2(0);
	for (int n = 1; n < n_max; n++) {
		float omega = PI*n/l;
		float An = Fg.b(n);
		float Bn = Fh.b(n)/(omega*c);
		u = u + RealFunctionR2([omega, c=c, An](vec2 v) {
			float x = v.x;
			float t = v.y;
			return An*std::cos(c*omega*t)*std::sin(omega*x);
		}, u0.getEps());
		u = u + RealFunctionR2([omega, c=c, Bn](vec2 v) {
			float x = v.x;
			float t = v.y;
			return Bn*std::sin(c*omega*t)*std::sin(omega*x);
		}, u0.getEps());
	}
	return u;
}

// RealFunctionR2 StringEquation::simple_solution(int n) const {
// 	float A = An(n);
// 	float B = Bn(n);
// 	auto X = Xn(n);
// 	auto w = omega(n);
// 	auto T = (COS_R & c*w)*A + (SIN_R & c*w)*B;
// 	return separated_product(X, T);
// }
//
// RealFunctionR2 StringEquation::solution() const {
// 	return sum<RealFunctionR2>(mapRange<RealFunctionR2>(1, n_max, [omega=omega, c=c, u0=u0,ut0=ut0, l=l, prec=prec](int n) {
// 		RealFunction X = SIN_R & omega(n);
// 		float A = X.L2_product(u0, vec2(0, l), prec)*2/l;
// 		float B = X.L2_product(ut0, vec2(0, l), prec)*2/(l*n*PI);
// 		float w = omega(n);
// 		RealFunction T = (COS_R & c*w)*A + (SIN_R & c*w)*B;
// 		return separated_product(X, T);
// 	}));
// }

RealFunctionR2 LaplaceHalfPlaneDirichlet::solution(int precision) const {
	auto kernel = [](vec2 xy) {return RealFunction([xy](float t) {
		return xy.y/(pow2(xy.x-t) + pow2(xy.y))/PI;
	});};

	return RealFunctionR2([f=u0, kernel=kernel, L=L, prec=precision](vec2 v) {
		return f.L2_product(kernel(v), vec2(-L, L), prec);
	}, u0.getEps());
}

RealFunctionR2 LaplaceHalfPlaneNeumann::solution(int precision) const {
	auto kernel = [](vec2 xy) {return RealFunction([xy](float t) {
		return std::log(pow2(xy.x-t) + pow2(xy.y))/TAU;
	});};

	return RealFunctionR2([f=uy0, kernel=kernel, L=L, prec=precision, c=C](vec2 v) {
		return f.L2_product(kernel(v), vec2(-L, L), prec) + c;
	}, uy0.getEps()); }

RealFunction LaplaceStripDirichlet::coef1(float y) const {
	return RealFunction([y, a=a](float t) {
		return std::sinh(abs(t)*(y-a))/std::sinh(abs(t)*a);
	});
}

RealFunction LaplaceStripDirichlet::coef2(float y) const {
	return RealFunction([y, a=a](float t) {
		return std::sinh(abs(t)*y)/std::sinh(abs(t)*a);
	});
}

LaplaceStripDirichlet::LaplaceStripDirichlet(RealFunction u0, RealFunction ua, float L, float a)
: u0(std::move(u0)), ua(std::move(ua)), L(L), a(a) {}

RealFunctionR2 LaplaceStripDirichlet::solution(int precision) const {
	return RealFunctionR2([this, precision](vec2 v) {
		float x = v.x;
		float t = v.y;
		auto K0 = FourierTransform(L, precision).inv(RealFunction([a=a, x](float omega) {
			return sinh(abs(omega)*(a-x))/(sinh(abs(omega)*a));
		}));
		auto Ka = FourierTransform(L, precision).inv(RealFunction([a=a, x](float omega) {
			return sinh(abs(omega)*x)/(sinh(abs(omega)*a));
		}));
		return (u0.convolve(K0, L, precision) + ua.convolve(Ka, L, precision))(t);
	});
}

RealFunctionR2 heat_kernel(float k) {
	return RealFunctionR2([k](float x, float t) {
		return exp(-pow2(x)/(4*k*t))/(2*sqrt(PI*t*k));
	}, 0.01f);
}

RealFunctionR2 HeatInhomogeneousHalfPlane::solution_homogeneous() const {
	return heat_kernel(k).convolve_x(u0, L, precision);
}

RealFunctionR2 HeatInhomogeneousHalfPlane::solution_homogeneous_bd() const {
	return RealFunctionR2([f=free_term, k=k, L=L, precision=precision](float x, float t) {
		return RealFunctionR2([f, k, x, t](float y, float tau) {
			return heat_kernel(k)(x-y, t-tau)*f(y, tau);
		}, 0.01f).integrate_rect(vec2(0, L), vec2(0, t), sqrt(precision));
	}, 0.01f);
}

RealFunctionR2 HeatInhomogeneousHalfPlane::solution() const {
	return solution_homogeneous() + solution_homogeneous_bd();
}
