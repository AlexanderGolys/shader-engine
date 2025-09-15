#pragma once
#include "pde.hpp"


class HeatRealLineHomoDiscrete{
protected:
	DiscreteRealFunction u0;
	float k, t_max;
	int t_res;
	DiscreteRealFunction green(float t, float x) const;
public:
	HeatRealLineHomoDiscrete (DiscreteRealFunction u0, float k, float t_max, int t_res) : u0(std::move(u0)), k(k), t_max(t_max), t_res(t_res) {}
	virtual DiscreteRealFunctionR2 solution() const;
	virtual ~HeatRealLineHomoDiscrete() = default;

	float t_step() const {return t_max/t_res;}
};


class HeatRealLineDiscreteNotHomo : public HeatRealLineHomoDiscrete {
	DiscreteRealFunctionR2 f_free;
	DiscreteRealFunctionR2 green2(float t, float x) const;

public:
	HeatRealLineDiscreteNotHomo (DiscreteRealFunctionR2 f_free, DiscreteRealFunction u0, float k, float t_max, int t_res) : HeatRealLineHomoDiscrete(std::move(u0), k, t_max, t_res), f_free(std::move(f_free)) {}
	DiscreteRealFunctionR2 solution() const override;

};
