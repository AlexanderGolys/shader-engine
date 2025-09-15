#pragma once
#include "func.hpp"
#include <forward_list>

std::forward_list<int> prime_factors(int n);

enum ElementaryFunction1Var {
	EF_SIN,
	EF_COS,
	EF_EXP,
	EF_LOG,
	EF_ABS,
	EF_ASIN,
	EF_ATAN,
};

enum ElementaryFunction2Var {
	EF_ADD,
	EF_MUL,
};



template<int vars=1, typename R=float>
class CompositionGraphNode {
public:
	virtual ~CompositionGraphNode() = default;
	virtual R operator()(array<R, vars> x) const;
	virtual CompositionGraphNode derivative(int var) const;
	virtual string toString(array<string, vars> x) const;
};

template<int vars=1, typename R=float>
class Monomial : public CompositionGraphNode<vars, R> {
	array<int, vars> powers;
	R coeff;
public:
	Monomial(R coef, array<int, vars> powers) : powers(powers), coeff(coef) {}
	R operator()(array<R, vars> x) const override {
		R res = coeff;
		for (int i = 0; i < vars; i++)
			res *= pow(x[i], powers[i]);
		return res;
	}
	CompositionGraphNode<vars, R> derivative(int var) const override {
		if (powers[var] == 0)
			return Monomial(coeff*0, powers); // derivative is 0
		auto new_powers = powers;
		--new_powers[var];
		return Monomial(coeff*powers[var], new_powers);
	}
	string toString(array<string, vars> x) const override;
};
