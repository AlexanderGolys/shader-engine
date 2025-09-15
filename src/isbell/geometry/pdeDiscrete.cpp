//
// Created by PC on 03.04.2025.
//

#include "pdeDiscrete.hpp"

DiscreteRealFunction HeatRealLineHomoDiscrete::green(float t, float x) const {
	return DiscreteRealFunction([k=k, t, x](float y) {
		return 1.f*std::exp(-pow2(x-y)/(4.f*k*t))/(2.f*std::sqrt(PI*t*k));
	}, u0.getDomain(), u0.samples());
}

DiscreteRealFunctionR2 HeatRealLineHomoDiscrete::solution() const {
	vector<DiscreteRealFunction> f_nt = {this->u0};
	for (float t: linspace(0.f, t_max, t_res)) {
		if (t < 0.001f) continue;
		Vector<float> g = Vector<float>(this->u0.samples(), [t, this](int i) {
			return (green(t, this->u0.sampling_step()*i)*this->u0).integral();
		});
		f_nt.emplace_back(g, u0.getDomain());
	}
	return DiscreteRealFunctionR2(f_nt, vec2(0, t_max));
}

DiscreteRealFunctionR2 HeatRealLineDiscreteNotHomo::solution() const {
	vector<DiscreteRealFunction> f_nt = {this->u0};
	for (int i_t=1; i_t<t_res; ++i_t) {
		float t = this->t_max/t_res*i_t;
		Vector<float> g = Vector<float>(this->u0.samples(), [t,i_t,  this](int i) {
			float x = this->u0.sampling_step()*i;
			return (green(t, x)*this->f_free[i_t]).integral();
		});
		DiscreteRealFunction Gfy = DiscreteRealFunction(g, u0.getDomain());
		f_nt.emplace_back(g, u0.getDomain());
	}
	return DiscreteRealFunctionR2(f_nt, vec2(0, t_max));
	return HeatRealLineHomoDiscrete::solution();
}
