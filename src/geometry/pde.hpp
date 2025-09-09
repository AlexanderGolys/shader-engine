#pragma once
#include <utility>

#include "smoothParametric.hpp"
#include "../utils/integralTransforms.hpp"

using std::move;


class SurfaceBoundaryPiece {
public:
	float a, b;
	SmoothParametricCurve curve;
	SurfaceBoundaryPiece(float a, float b, SmoothParametricCurve curve) : a(a), b(b), curve(std::move(curve)) {}
};

class BoundaryCondition {
public:
	RealFunction _h;
	bool zero = false;

	explicit BoundaryCondition(RealFunction h);
	explicit BoundaryCondition() : _h(0) {
		zero = true;
	}
};



/**
 * @brief du/dn * alpha_N + u * alpha = h  on the boundary
 * du/dn : normal derivative relative to the boundary
 *
 * @attention [alpha_N : alpha] projective coordinates on RP1, so reduced to
 * affine projection [1:alpha/alpha_N] or point at infinity [0:1] (Dirichlet)
 */
class RobinBoundaryCondition : public BoundaryCondition {
public:
	float alpha_N, alpha;
	explicit RobinBoundaryCondition(float a_N, float a, RealFunction h);
	explicit RobinBoundaryCondition(float alpha_N, float alpha);
};

class DirichletBoundaryCondition : public RobinBoundaryCondition {
public:
	explicit DirichletBoundaryCondition(RealFunction h) : RobinBoundaryCondition(0, 1, std::move(h)) {}
};

class NeumannBoundaryCondition : public RobinBoundaryCondition {
public:
	explicit NeumannBoundaryCondition(RealFunction h) : RobinBoundaryCondition(1, 0, std::move(h)) {}
};










class WaveEquation1DCauchy {
	public:
	RealFunction u0, ut0;
	RealFunctionR2 f;
	float c;
	WaveEquation1DCauchy(RealFunction u0, RealFunction ut0, RealFunctionR2 f, float c);
	RealFunctionR2 solution(int precision) const;
};

class WaveEquation1DDirichlet {
public:
	RealFunction u0, ut0, p;
	RealFunctionR2 f;
	float c;
	WaveEquation1DDirichlet(RealFunction u0, RealFunction ut0, RealFunction p0, RealFunctionR2 f, float c);
	RealFunctionR2 solution(int precision) const;
};

class StringEquation {
	float l;
	RealFunction u0, ut0;
	int n_max, prec;
	float c;
public:
	StringEquation(float c, float l, RealFunction u0, RealFunction ut0, int n_max, int precision);

	float omega(int n) const;
	RealFunction Xn(int n) const;
	float An(int n) const;
	float Bn(int n) const;
	RealFunctionR2 simple_solution(int n) const;
	RealFunctionR2 solution() const;
	void set_l(float l1);
	void set_c(float c1);
};



/**
 * @brief for uniqueness, we need additional condition that solutions are BOUNDED UNIFORMLY
 */
class LaplaceHalfPlaneDirichlet {
	RealFunction u0;
	float L;
public:
	LaplaceHalfPlaneDirichlet(RealFunction u0, float L) : u0(std::move(u0)), L(L) {}
	RealFunctionR2 solution(int precision) const;
};

class LaplaceHalfPlaneNeumann {
	RealFunction uy0;
	float L;
	float C = 0;
public:
	explicit LaplaceHalfPlaneNeumann(RealFunction uy0, float L) : uy0(std::move(uy0)), L(L) {}
	RealFunctionR2 solution(int precision) const;
};

class LaplaceStripDirichlet {
	RealFunction u0, ua;
	float L, a;
	RealFunction coef1(float y) const;
	RealFunction coef2(float y) const;

public:
	LaplaceStripDirichlet(RealFunction u0, RealFunction ua, float L, float a);
	RealFunctionR2 solution(int precision) const;
};


RealFunctionR2 heat_kernel(float k);

/**
 * \brief u_t = k u_xx + f(x, t), u(x, 0) = u0(x)
 */
class HeatInhomogeneousHalfPlane {
	RealFunction u0;
	RealFunctionR2 free_term;
	float k, L;
	int precision;
public:
	HeatInhomogeneousHalfPlane(RealFunction u0, RealFunctionR2 free_term, float k, float L, int prec) : u0(std::move(u0)), free_term(std::move(free_term)), k(k), L(L), precision(prec) {}
	RealFunctionR2 solution_homogeneous() const;
	RealFunctionR2 solution_homogeneous_bd() const;
	RealFunctionR2 solution() const;
};

class LaplaceRectangleD0D0 {
	RealFunction u0, uh;
	float h, w;
	int precision;
public:
	LaplaceRectangleD0D0(RealFunction u0, RealFunction ua, float h, float w, int precision) : u0(std::move(u0)), uh(std::move(ua)), h(h), w(w), precision(precision) {}
	RealFunctionR2 solution(int precision) const;
};


class HeatLineHomogeneousDiscrete {
};
