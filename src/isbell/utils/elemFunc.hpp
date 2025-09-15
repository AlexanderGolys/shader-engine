#pragma once
#include "func.hpp"


/**
 * @brief canonically computed from Abramowitz-Stegun approximation (err < 5e-4) with optional more precise approximation (err < 3e-7)
 */
float AS_app_erf(float t);
float high_prec_erf(float t);


class erf final : public RealFunction  {
	erf();
};






class GaussianKernel: public RealFunctionR2 {
	float k;
public:
	explicit GaussianKernel(float k);
	RealFunction conv_uncarry(float t) const;
};

class Gaussian final: public RealFunction {
	float center, stdev;
public:
	explicit Gaussian(float center, float stdev) : center(center), stdev(stdev),
	RealFunction([center, stdev](float x) {
		return std::exp(-::pow2(x-center)/(2*::pow2(stdev)))/(stdev*::sqrt(2*PI));
	}, 0.0001f, Regularity::SMOOTH) {}
};



template<typename FunctionClass=RealFunction>
class OrthonormalSystem {
	FunctionClass weight;
	float a, b;
	int prec;
public:
	OrthonormalSystem(FunctionClass w, float a, float b, int prec) : weight(std::move(w)), a(a), b(b), prec(prec) {}
	virtual FunctionClass phi_n(int n);
	float a_n(const FunctionClass &f, int n) const {
		return (phi_n(n)*weight*f).integral(a, b, prec);
	}

	virtual ~OrthonormalSystem() = default;
};
