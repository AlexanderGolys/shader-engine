#pragma once
#include "macros.hpp"


// template<typename A>
// A one = A(1);
//
// template<typename A>
// A zero = A(0);

template<typename T>
concept AbelianSemigroup = requires(T a, T b) {
		{ a + b } -> std::same_as<T>;
	};

template<typename T>
concept Semigroup = requires(T a, T b) {
		{ a*b } -> std::same_as<T>;
	};

template<typename T>
concept TotalOrder = requires(T a, T b) {
		{ a < b } -> std::same_as<bool>;
		{ a == b } -> std::same_as<bool>;
	};

template<typename T>
concept TotallyOrderedAbelianSemigroup = AbelianSemigroup<T> && TotalOrder<T>;

template<typename M>
concept AbelianMonoid = AbelianSemigroup<M> && (
		requires { M(0); }
		||
		requires { { 0 } -> std::convertible_to<M>; }
		||
		requires { { M::zero() } -> std::same_as<M>; });

template<typename M>
concept Monoid = Semigroup<M> && (
	requires { M(1); }
	||
	requires { { M::one() } -> std::same_as<M>; }
	||
	requires { { 1 } -> std::convertible_to<M>; }
	||
	requires { { M::I()} -> std::same_as<M>; });

template<typename G>
concept GroupConcept = Monoid<G> && (
    	requires(G g) { { ~g } -> std::same_as<G>; }
    	|| requires(G g) {{ g.inv() } -> std::same_as<G>; }
    	|| requires(G g, G h) {{ g/h } -> std::same_as<G>; });

template<typename G>
concept AbelianGroupConcept = AbelianMonoid<G> && requires(G g) {
		{ -g } -> std::same_as<G>;
};

template<typename T>
concept Rng = AbelianSemigroup<T> && Semigroup<T>;

template<typename R>
concept RingConcept = Rng<R> && Monoid<R>;


template<typename D>
concept DivisionRing = Rng<D> && requires(D g, D h) {
	{ g/h } -> std::same_as<D>;
};

template<typename A, typename R>
concept RModule = Rng<R> && AbelianGroupConcept<A> && requires(A a, R r) {
	{ a*r } -> std::same_as<A>;
};

template<typename A, typename R>
concept RAlgebra =  Rng<A> && RModule<A, R>;


template<typename V, typename K=float>
concept VectorSpaceConcept = RModule<V, K> && DivisionRing<K>;

template<typename T>
concept Normed = requires(T a) {
	{norm(a)} -> std::convertible_to<float>;
};

template<typename V>
concept EuclideanSpaceConcept = VectorSpaceConcept<V, float> && requires(V a, V b) {
	{ dot(a, b) } -> std::convertible_to<float>;
};
