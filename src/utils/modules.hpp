#pragma once

#include "mat.hpp"


class Finset {
	int i, max;

	Finset(int i, int max) : i(i), max(max) {}
	bool operator==(Finset f) const { return i == f.i && max == f.max; }
	bool operator!=(Finset f) const { return !(*this == f); }
	bool operator<(Finset f) const { return i < f.i;}
};



template<RingConcept R, typename BasisIndexingSet>
class FreeModule {
	HOM(BasisIndexingSet, R) coefs;
public:
	int rank;

	FreeModule(int rank, HOM(BasisIndexingSet, R) coefs) : rank(rank), coefs(coefs) {}
	explicit FreeModule(int rank) : rank(rank), coefs([rank](BasisIndexingSet i) { return R(0); }) {}
	FreeModule operator+(const FreeModule &M) const { return FreeModule(rank, [this, M](BasisIndexingSet i) { return coefs(i) + M.coefs(i); }); }
	FreeModule operator*(const R &c) const { return FreeModule(rank, [this, c](BasisIndexingSet i) { return coefs(i) * c; }); }
	FreeModule operator/(const R &c) const requires DivisionRing<R> { return *this * (R(1) / c); }
	FreeModule operator-(const FreeModule &M) const { return *this + M * -1; }
	FreeModule operator-() const { return *this * -1; }
	bool operator==(const FreeModule &M) const { return coefs == M.coefs; }
	bool operator!=(const FreeModule &M) const { return !(*this == M); }
};




inline int multinomial_coef(const vector<int>& lst) {
	return factorial(sum<int, vector<int>>(lst))/mult<int, vector<int>>(map_cr<int, int>(lst, factorial));
}


template<typename V, typename R=float>
class SymmetricPower{
	HOM(vector<int>, V) f;
	int n;

public:
	SymmetricPower(HOM(vector<int>, V) f, int n): f(f), n(n) {}
	V operator()(vector<int> v) const;
	int grading() const { return n; }
	SymmetricPower tensor_product(const SymmetricPower& other) const { return SymmetricPower([F=f, G=other.f](vector<int> lst) { return F(sublist(lst, 0, F.size())) * G(sublist(lst, F.size(), lst.size())); }, n + other.n); }
	SymmetricPower operator*(const SymmetricPower& other) const { return tensor_product(other); }
	SymmetricPower operator+(const SymmetricPower& other) const { return tensor_product(other); }
	SymmetricPower operator-(const SymmetricPower& other) const { return tensor_product(other); }
	SymmetricPower operator*(R c) const { return SymmetricPower([F=f, c](vector<int> lst) { return F(lst)*c; }, n); }
	SymmetricPower operator/(R c) const requires DivisionRing<R> { return SymmetricPower([F=f, c](vector<int> lst) { return F(lst)/c; }, n); }
};


template<RingConcept K=float, ModuleConcept<K> V=vec2>
class TensorPower{
	HOM(vector<int>, V) f;
public:
	int n;
	TensorPower(HOM(vector<int>, V) f, int n): f(f), n(n) {}
	explicit TensorPower(const V& v): f([v](vector<int> lst) { return v; }), n(0) {}
	explicit TensorPower(const vector<V>& v): f([v](vector<int> lst) { return v[lst[0]]; }), n(1) {}

	V operator()(vector<int> v) const {
		if (v.size() != n)
			throw std::invalid_argument("TensorPower: wrong number of arguments");
		return f(v);
	}
	int order() const { return n; }
	TensorPower tensor_product(const TensorPower& other) const {
		return TensorPower([F=f, G=other.f](vector<int> lst) { return F(sublist(lst, 0, F.size())) * G(sublist(lst, F.size(), lst.size())); }, n + other.n);
	}
	TensorPower operator*(const TensorPower& other) const { return tensor_product(other);}

};




template<typename V, typename R>
V SymmetricPower<V, R>::operator()(vector<int> v) const {
	if (v.size() != n)
		throw std::invalid_argument("SymmetricPower: wrong number of arguments");
	vector<int> v_sorted = v;
	std::sort(v_sorted.begin(), v_sorted.end());
	vector<int> v_count = vector<int>(n);
	for (int i: v) v_count[i]++;
	return f(v_sorted)*multinomial_coef(v_count);
};
