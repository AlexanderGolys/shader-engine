//
// Created by PC on 28.03.2025.
//

#include "elemFunc.hpp"

float AS_app_erf(float t) {
	float a1 = 0.278393, a2 = 0.230389, a3 = 0.000972, a4 = 0.078108;
	return 1 - 1.f/::pow4(1 + a1 * t + a2 * ::pow2(t) + a3 * ::pow3(t) + a4 * ::pow4(t));
}

float high_prec_erf(float t) {
	float a1 = 0.0705230784, a2 = 0.0422820123, a3 = 0.0092705272, a4 = 0.0001520143, a5 = 0.0002765672, a6 = 0.0000430638;
	return 1 - 1.f/::pow4(::pow4(1 + a1 * t + a2 * ::pow2(t) + a3 * ::pow3(t) + a4 * ::pow4(t)+a5 * ::pow5(t)+a6 * ::pow6(t)));
}

erf::erf(): RealFunction([](float x) {
	return x < 0 ? -AS_app_erf(-x) : AS_app_erf(x);
}, [](float x) {
	return 2.f/::sqrt(PI)*std::exp(-x*x);
}, 0.01) {}

GaussianKernel::GaussianKernel(float k): RealFunctionR2([k](float x, float t) {
	return std::exp(-pow2(x)/(4*k*t))/(2*::sqrt(k*PI*t));
}, 0.01), k(k) {}

RealFunction GaussianKernel::conv_uncarry(float t) const {
	return RealFunction([this, t](float x) { return std::exp(-pow2(x)/(4*k*t))/(2*::sqrt(k*PI*t)); }, 0.01);
}
