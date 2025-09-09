#pragma once

#include <functional>
#include "macros.hpp"
#include <string>





template<typename C1, typename C2, typename D>
HOM(C1, HOM(C2, D)) hom_adjunction(const BIHOM(C1, C2, D) &F) {
	return [f_=F](C1 x) {
		return [f=f_, x](C2 y) {
			return f(x, y);
		};
	};
}

template<typename C1, typename C2, typename D>
BIHOM(C1, C2, D) hom_adjunction(const HOM(C1, HOM(C2, D)) &F) {
	return [f_=F](C1 x, C2 y) {
		return f_(x)(y);
	};
}
//
// template<typename C2, typename C1, typename D>
// BIHOM(C1, C2, D) product_monoidal_symmetriser(const BIHOM(C2, C1, D) &F) {
// 	return [_f=F](C1 x, C2 y) {
// 		return _f(y, x);
// 	};
// }
//
//
//
// // ------------------------- source file embedded cause templates are weird -------------------------
//
//
//
// template<typename C>
// maybe_monad<C> & maybe_monad<C>::operator=(const maybe_monad &other) {
// 	if (this == &other)
// 		return *this;
// 	value = other.value;
// 	specified = other.specified;
// 	return *this;
// }
//
// template<typename C>
// maybe_monad<C> & maybe_monad<C>::operator=(maybe_monad &&other) noexcept {
// 	if (this == &other)
// 		return *this;
// 	value = std::move(other.value);
// 	specified = other.specified;
// 	return *this;
// }
