#pragma once

#include <functional>
#include "macros.hpp"
#include <string>


template<typename T>
HOM(T, T) identity() {return  [](T x) { return x; }; }

template<typename X, typename Y, typename Z>
HOM(X, Z) compose(HOM(Y, Z) g, HOM(X, Y) f) {
	return [f, g](X x) { return g(f(x)); };
}

template<typename T, typename U>
bool comp_identity(HOM(T, U) f, T x) {
	HOM(T, U) f_id = compose<T, U, U>(f, identity<U>());
	HOM(T, U) id_f = compose<T, T, U>(identity<T>(), f);
	return f(x) == f_id(x) && f(x) == id_f(x);
}

template<typename X, typename Y, typename Z>
HOM(X, HOM(Y, Z)) curry(BIHOM(X, Y, Z) F) {
	return [F](X x) {
		return [F, x](Y y) {
			return F(x, y);
		};
	};
};

template<typename Y>
HOM(vec2, Y) wrap(const BIHOM(float, float, Y) &f_) {
	return [f=f_](vec2 v) {
		return f(v.x, v.y); };
}

template<typename Y>
BIHOM(float, float, Y) unwrap(const HOM(vec2, Y) &f_) { return [f=f_](float x, float y) {
	return f(vec2(x, y)); };
}

template<typename Y>
HOM(vec3, Y) wrap(const TRIHOM(float, float, float, Y) &f_) { return [f=f_](vec3 v) {
	return f(v.x, v.y, v.z); }; }

template<typename Y>
TRIHOM(float, float, float, Y) unwrap(const HOM(vec3, Y) &f_) { return [f=f_](float x, float y, float z) {
	return f(vec3(x, y, z)); }; }

template<typename C1, typename C2=C1, typename D>
BIHOM(C1, C2, D) uncurry(HOM(C1, HOM(C2, D)) f) {
	return [f=f](C1 x, C2 y) {
		return f(x)(y); }; }

template<typename C1, typename C2=C1, typename D>
BIHOM(C1, C2, D) uncurry_perm(const HOM(C2, HOM(C1, D)) &f) {
	return [F=f](C1 x, C2 y) {
		return F(y)(x); }; }

template<typename C1, typename C2=C1, typename D>
HOM(C1, HOM(C2, D)) uncurry(const BIHOM(C1, C2, D) &f) {
	return [F=f](C1 x) { return [F, x](C2 y) { return F(x, y); }; }; }

template<typename C1, typename C2=C1, typename D>
HOM(C2, HOM(C1, D)) uncurry_perm(const BIHOM(C1, C2, D) &f) {
	return [F=f](C2 y) { return [F, y](C1 x) { return F(x, y); }; }; }
