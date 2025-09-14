#pragma once
#pragma GCC diagnostic ignored "-Wliteral-suffix"

#include <cmath>
#include <vector>
#include <array>
#include <algorithm>
#include <format>
#include <memory>

#include "metaUtils.hpp"



template<typename vec>
vector<float> vecToVecHeHe(vec v) {
	vector<float> res;
	for (int i = 0; i < v.length(); i++)
		res.push_back(v[i]);
	return res;
};




template<typename domain, typename codomain=float>
class Morphism {

protected:
	HOM(domain, codomain) _f;

public:
	explicit Morphism(HOM(domain, codomain) f) : _f(f) {}

	codomain operator()(domain x) const { return _f(x); }
	Morphism operator+(const Morphism &g) const requires AbelianSemigroup<codomain> { return Morphism([f_=_f, g_=g._f](domain x){ return f_(x) + g_(x); }); }
	Morphism operator-(const Morphism &g) const requires AbelianGroupConcept<codomain>;
	Morphism operator*(const Morphism &g) const requires Semigroup<codomain>;
	Morphism operator/(const Morphism &g) const requires DivisionRing<codomain>;
	Morphism operator*(codomain a) const requires Semigroup<codomain>;

	template<DivisionRing K>
	Morphism operator/(K a) const requires VectorSpaceConcept<codomain, K> { return Morphism([this, a](domain x){ return (*this)(x) / a; }); }

	template<Rng R>
	Morphism operator*(R a) const requires RModule<codomain, R> { return Morphism([this, a](domain x){ return (*this)(x) * a; }); }
};




template<typename X, typename Y, typename Z>
Morphism<X, Z> compose(const Morphism<Y, Z> &f, const Morphism<X, Y> &g) {
	return Morphism<X, Z>([f_=f, g_=g](X x){ return f_(g_(x)); });
}

template<typename X, typename Y, typename Z>
Morphism<X, Z> operator&(const Morphism<Y, Z> &f, const Morphism<X, Y> &g) {
	return Morphism<X, Z>([f_=f, g_=g](X x){ return f_(g_(x)); });
}




// -----------------------------  MATRICES  ----------------------------------




template<Rng T>
class Vector {
	int n;
	vector<T> coefs;

public:
	explicit Vector(vector<T> c);
	explicit Vector(T scalar) : Vector(vector<T>({scalar})) {}
	explicit Vector(std::initializer_list<T> c) : Vector(vector<T>(c)) {}
	explicit Vector(int n, HOM(int, T) f);

	Vector(const Vector &other);
	Vector(Vector &&other) noexcept;
	Vector &operator=(const Vector &other);
	Vector &operator=(Vector &&other) noexcept;

	Vector operator*(const T &f) const;
	Vector operator+(const T &f) const;
	Vector operator-(const T &f) const;
	Vector operator/(const T &f) const requires DivisionRing<T>;
	Vector operator+(const Vector &v) const;
	Vector operator-(const Vector &v) const;
	Vector operator-() const;
	explicit operator string() const;

	constexpr int length() const;
	constexpr int size() const;

	bool operator==(const Vector &v) const;
	bool operator!=(const Vector &v) const;

	const T& at(int i) const;
	T& operator[](int i);
	const T& operator[](int i) const { return at(i); }

	T max() const;
	T min() const;
	T sum() const;
	T mean() const;
	T dot(const Vector &v) const;
	int argmax() const;
	int argmin() const;
	int argmaxAbs() const;
	int argminAbs() const;

	Vector slice(int start, int end, int step = 1) const;
	Vector slice_to(int end) const;
	Vector slice_from(int start) const;
	Vector reverse() const;


	Vector pointwise_product(const Vector &v) const;
	Vector pointwise_division(const Vector &v) const requires DivisionRing<T>;
	Vector concat(const Vector &v) const;

	vector<T> vec() const;
	static Vector zeros(int n);

	template<Rng S>
	Vector<S> base_change(const HOM(T, S) &phi) const;

	template<Rng S>
	Vector<S> base_change() const;
};




template<Rng T>
class FiniteSequence {
	vector<T> coefs_positive;
	vector<T> coefs_negative;

public:
	FiniteSequence(const vector<T> &coefs_positive, const vector<T> &coefs_negative);
	explicit FiniteSequence(const vector<T> &coefs_positive);
	FiniteSequence();

	int size() const;
	int n_min() const;
	int n_max() const;

	FiniteSequence(const FiniteSequence &other);
	FiniteSequence(FiniteSequence &&other) noexcept;
	FiniteSequence &operator=(const FiniteSequence &other);
	FiniteSequence &operator=(FiniteSequence &&other) noexcept;

	T at(int i) const;
	T operator[](int i) const;
	void set(int i, T val);

	FiniteSequence operator+(const FiniteSequence &other) const;
	FiniteSequence operator-(const FiniteSequence &other) const;
	FiniteSequence operator*(const T &scalar) const;
	FiniteSequence operator/(const T &scalar) const;
	FiniteSequence operator-() const;

	T dot(const FiniteSequence &other) const;
	T dot(const Vector<T> &other) const { return dot(FiniteSequence(other.vec())); }
	friend T dot(const FiniteSequence &a, const FiniteSequence &b) { return a.dot(b); }

	FiniteSequence convolve(const FiniteSequence &other) const;
	FlatteningIterator<T> begin();
	FlatteningIterator<T> end();


	template<Rng R>
	FiniteSequence<R> base_change(const HOM(T, R) &phi) const;

	template<Rng R>
	FiniteSequence<R> base_change() const;
};





template<Rng T>
 T& Vector<T>::operator[](int i) {
	if (i < 0)
		i = n + i;
	if (i < 0)
		throw IndexOutOfBounds(i+n, n, "Vector::at: index out of bounds", __FILE__, __LINE__);
	if (i >= n)
		throw IndexOutOfBounds(i, n, "Vector::at: index out of bounds", __FILE__, __LINE__);
	return coefs[i];
}


template<Rng T>
 const T& Vector<T>::at(int i) const {
	if (i < 0)
		i = n + i;
	if (i < 0)
		throw IndexOutOfBounds(i+n, n, "Vector::at: index out of bounds", __FILE__, __LINE__);
	if (i >= n)
		throw IndexOutOfBounds(i, n, "Vector::at: index out of bounds", __FILE__, __LINE__);
	return coefs.at(i);
}

template<Rng T>
T Vector<T>::max() const {
	T m = coefs[0];
	for (int i = 1; i < n; i++) {
		m = max(m, coefs[i]);
	}
	return m;
}

template<Rng T>
T Vector<T>::min() const {
	T m = coefs[0];
	for (int i = 1; i < n; i++) {
		m = min(m, coefs[i]);
	}
	return m;
}

template<Rng T>
T Vector<T>::sum() const {
	T s = coefs[0];
	for (int i = 1; i < n; i++) {
		s += coefs[i];
	}
	return s;
}

template<Rng T>
T Vector<T>::mean() const {
	T s = sum();
	return s / n;
}

template<Rng T>
T Vector<T>::dot(const Vector &v) const {
	if (n != v.n) throw std::runtime_error("Vector::dot: incompatible sizes");
	T s = coefs[0] * v[0];
	for (int i = 1; i < n; i++) {
		s += coefs[i] * v[i];
	}
	return s;
}

template<Rng T>
int Vector<T>::argmax() const {
	int i_max = 0;
	T m = coefs[0];
	for (int i = 1; i < n; i++) {
		if (coefs[i] > m) {
			m = coefs[i];
			i_max = i;
		}
	}
	return i_max;
}

template<Rng T>
int Vector<T>::argmin() const {
	int i_min = 0;
	T m = coefs[0];
	for (int i = 1; i < n; i++) {
		if (coefs[i] < m) {
			m = coefs[i];
			i_min = i;
		}
	}
	return i_min;
}

template<Rng T>
int Vector<T>::argmaxAbs() const {
	int i_max = 0;
	T m = abs(coefs[0]);
	for (int i = 1; i < n; i++) {
		if (abs(coefs[i]) > m) {
			m = abs(coefs[i]);
			i_max = i;
		}
	}
	return i_max;
}

template<Rng T>
int Vector<T>::argminAbs() const {
	int i_min = 0;
	T m = abs(coefs[0]);
	for (int i = 1; i < n; i++) {
		if (abs(coefs[i]) < m) {
			m = abs(coefs[i]);
			i_min = i;
		}
	}
	return i_min;
}

template<Rng T>
Vector<T> Vector<T>::slice(int start, int end, int step) const {
	if (step == 0) throw std::range_error("Vector::slice: step cannot be 0");
	if (start < 0) start = n - start;
	if (end < 0) end = n - end;
	if (start > end && step > 0 || start < end && step < 0) throw std::range_error("Vector::slice: start cannot be greater than end");
	if (start < 0 || end < 0) throw std::out_of_range("Vector::slice: negative indices conversion failed");
	if (end > n || start > n) throw std::out_of_range("Vector::slice: end cannot be greater than n");
	if (start == end) return Vector(static_cast<T>(0));

	return Vector((end - start + step - 1) / step, [this, start, step](int i){ return coefs[start + i * step]; });
}

template<Rng T>
Vector<T> Vector<T>::slice_to(int end) const {
	if (end > n) throw std::runtime_error("Vector::slice_to: end cannot be greater than n");
	return slice(0, end);
}

template<Rng T>
Vector<T> Vector<T>::slice_from(int start) const {
	if (start < 0) start = length() - start;
	return slice(start, n);
}

template<Rng T>
Vector<T> Vector<T>::reverse() const {
	return Vector(n, [this](int i){ return coefs[n - 1 - i]; });
}

template<Rng T>
Vector<T> Vector<T>::pointwise_product(const Vector &v) const {
	vector<T> new_coefs(n);
	for (int i = 0; i < n; i++)
		new_coefs[i] = coefs[i] * v.coefs[i];
	return Vector(new_coefs);
}

template<Rng T>
Vector<T> Vector<T>::pointwise_division(const Vector &v) const requires DivisionRing<T> {
	vector<T> new_coefs(n);
	for (int i = 0; i < n; i++)
		new_coefs[i] = coefs[i] / v.coefs[i];
	return Vector(new_coefs);
}

template<Rng T>
Vector<T> Vector<T>::concat(const Vector &v) const { return Vector(n + v.n, [this, v](int i){ return i < n ? coefs[i] : v[i - n]; }); }

template<Rng T>
vector<T> Vector<T>::vec() const { return coefs; }

template<Rng T>
Vector<T> Vector<T>::zeros(int n) { return Vector(vector<T>(n)); }

template<Rng T>
template<Rng S>
Vector<S> Vector<T>::base_change(const std::function<S(T)> &phi) const {
	return Vector<S>(n, [this, phi](int i){ return phi(coefs[i]); });
}

template<Rng T>
template<Rng S>
Vector<S> Vector<T>::base_change() const {
	return Vector<S>(n, [this](int i){ return S(coefs[i]); });
}

template<Rng T>
FiniteSequence<T>::FiniteSequence(const vector<T> &coefs_positive, const vector<T> &coefs_negative)
: coefs_positive(coefs_positive),
  coefs_negative(coefs_negative) {}




template<Rng T>
FiniteSequence<T>::FiniteSequence(const vector<T> &coefs_positive)
: coefs_positive(coefs_positive),
  coefs_negative(vector<T>()) {}

template<Rng T>
FiniteSequence<T>::FiniteSequence(): FiniteSequence(vector<T>()) {}




template<Rng T>
int FiniteSequence<T>::size() const { return coefs_positive.size() + coefs_negative.size(); }

template<Rng T>
int FiniteSequence<T>::n_min() const { return -coefs_negative.size(); }

template<Rng T>
int FiniteSequence<T>::n_max() const { return coefs_positive.size() - 1; }

template<Rng T>
FiniteSequence<T>::FiniteSequence(const FiniteSequence &other)
: coefs_positive(other.coefs_positive),
  coefs_negative(other.coefs_negative) {}

template<Rng T>
FiniteSequence<T>::FiniteSequence(FiniteSequence &&other) noexcept
: coefs_positive(std::move(other.coefs_positive)),
  coefs_negative(std::move(other.coefs_negative)) {}

template<Rng T>
FiniteSequence<T> &FiniteSequence<T>::operator=(const FiniteSequence &other) {
	if (this == &other)
		return *this;
	coefs_positive = other.coefs_positive;
	coefs_negative = other.coefs_negative;
	return *this;
}

template<Rng T>
FiniteSequence<T> &FiniteSequence<T>::operator=(FiniteSequence &&other) noexcept {
	if (this == &other)
		return *this;
	coefs_positive = std::move(other.coefs_positive);
	coefs_negative = std::move(other.coefs_negative);
	return *this;
}




template<Rng T>
T FiniteSequence<T>::at(int i) const {
	if (i < -coefs_negative.size() || i >= coefs_positive.size())
		throw IndexOutOfBounds(i, n_max() - n_min() + 1, "FiniteSequence::at: index out of bounds", __FILE__, __LINE__);
	if (i >= 0)
		return coefs_positive[i];
	return coefs_negative[-i - 1];
}




template<Rng T>
T FiniteSequence<T>::operator[](const int i) const {
	return at(i);
}




template<Rng T>
void FiniteSequence<T>::set(int i, T val) {
	if (i >= 0) {
		while (i >= coefs_positive.size())
			coefs_positive.push_back(T(0));
		coefs_positive[i] = val;
	} else {
		while (-i - 1 >= coefs_negative.size())
			coefs_negative.push_back(T(0));
		coefs_negative[-i - 1] = val;
	}
}




template<Rng T>
FiniteSequence<T> FiniteSequence<T>::operator+(const FiniteSequence &other) const {
	vector<T> new_coefs_positive = coefs_positive;
	vector<T> new_coefs_negative = coefs_negative;
	for (int i = 0; i < other.coefs_positive.size(); i++)
		new_coefs_positive[i] += other.coefs_positive[i];
	for (int i = 0; i < other.coefs_negative.size(); i++)
		new_coefs_negative[i] += other.coefs_negative[i];
	return FiniteSequence(new_coefs_positive, new_coefs_negative);
}




template<Rng T>
FiniteSequence<T> FiniteSequence<T>::operator-(const FiniteSequence &other) const {
	vector<T> new_coefs_positive = coefs_positive;
	vector<T> new_coefs_negative = coefs_negative;
	for (int i = 0; i < other.coefs_positive.size(); i++)
		new_coefs_positive[i] -= other.coefs_positive[i];
	for (int i = 0; i < other.coefs_negative.size(); i++)
		new_coefs_negative[i] -= other.coefs_negative[i];
	return FiniteSequence(new_coefs_positive, new_coefs_negative);
}

template<Rng T>
FiniteSequence<T> FiniteSequence<T>::operator*(const T &scalar) const {
	vector<T> new_coefs_positive = coefs_positive;
	vector<T> new_coefs_negative = coefs_negative;
	for (int i = 0; i < new_coefs_positive.size(); i++)
		new_coefs_positive[i] *= scalar;
	for (int i = 0; i < new_coefs_negative.size(); i++)
		new_coefs_negative[i] *= scalar;
	return FiniteSequence(new_coefs_positive, new_coefs_negative);
}

template<Rng T>
FiniteSequence<T> FiniteSequence<T>::operator/(const T &scalar) const{
	vector<T> new_coefs_positive = coefs_positive;
	vector<T> new_coefs_negative = coefs_negative;
	for (int i = 0; i < new_coefs_positive.size(); i++)
		new_coefs_positive[i] /= scalar;
	for (int i = 0; i < new_coefs_negative.size(); i++)
		new_coefs_negative[i] /= scalar;
	return FiniteSequence(new_coefs_positive, new_coefs_negative);
}

template<Rng T>
FiniteSequence<T> FiniteSequence<T>::operator-() const {
	vector<T> new_coefs_positive = coefs_positive;
	vector<T> new_coefs_negative = coefs_negative;
	for (int i = 0; i < new_coefs_positive.size(); i++)
		new_coefs_positive[i] *= -1;
	for (int i = 0; i < new_coefs_negative.size(); i++)
		new_coefs_negative[i] *= -1;
	return FiniteSequence(new_coefs_positive, new_coefs_negative);
}

template<Rng T>
T FiniteSequence<T>::dot(const FiniteSequence &other) const {
	T res = max(n_max(), other.n_max());
	for (int i = min(n_min(), other.n_min()); i < max(n_max(), other.n_max()); ++i)
		res += at(i) * other.at(i);
	return res;
}

template<Rng T>
FiniteSequence<T> FiniteSequence<T>::convolve(const FiniteSequence<T> &other) const {
	FiniteSequence<T> res;
	for (int i = n_min(); i < n_max(); ++i) {
		T sum = T(0);
		for (int j = other.n_min(); j < other.n_max(); ++j) {
			sum += at(i - j) * other[j];
		}
		res.set(i, sum);
	}
	return res;
}

template<Rng T>
FlatteningIterator<T> FiniteSequence<T>::begin() {
	return FlatteningIterator<T>({coefs_negative, coefs_positive});
}

template<Rng T>
FlatteningIterator<T> FiniteSequence<T>::end() {
	return FlatteningIterator<T>();
}

template<Rng T>
template<Rng R>
FiniteSequence<R> FiniteSequence<T>::base_change(const std::function<R(T)> &phi) const {
	FiniteSequence<R> res;
	for (int i = 0; i < size(); i++)
		res.set(i, phi(at(i)));
	return res;
}

template<Rng T>
template<Rng R>
FiniteSequence<R> FiniteSequence<T>::base_change() const {
	return base_change<R>([](T t){ return R(t); });
}



// After
// template<>
// class Vector<float> {
// public:
// 	explicit Vector(float scalar)
// 	: n(1),
// 	  coefs({scalar}) {}
//
// 	explicit Vector(const vector<float> &c)
// 	: n(c.size()),
// 	  coefs(c) {}
//
// 	explicit Vector(vec2 v)
// 	: n(2),
// 	  coefs({v.x, v.y}) {}
//
// 	explicit Vector(vec3 v)
// 	: n(3),
// 	  coefs({v.x, v.y, v.z}) {}
//
// 	explicit Vector(vec4 v)
// 	: n(4),
// 		coefs({v.x, v.y, v.z, v.w}) {}
//
// 	static Vector zero(int n) { return Vector(vector<float>(n, 0)); }
//
// 	Vector(const Vector<float> &other)
// 	: n(other.n),
// 	  coefs(other.coefs) {}
//
// 	Vector(Vector<float> &&other) noexcept
// 	: n(other.n),
// 	  coefs(std::move(other.coefs)) {}
//
// 	Vector<float> & operator=(const Vector<float> &other) {
// 		if (this == &other)
// 			return *this;
// 		n = other.n;
// 		coefs = other.coefs;
// 		return *this;
// 	}
//
// 	Vector<float> & operator=(Vector<float> &&other) noexcept {
// 		if (this == &other)
// 			return *this;
// 		n = other.n;
// 		coefs = std::move(other.coefs);
// 		return *this;
// 	}
//
// private:
// 	int n;
// 	vector<float> coefs;
// };




template<typename R=float>
class Matrix {
	vector<R> coefs;
	int rows, cols;

public:
	Matrix(int rows, int cols);

	explicit Matrix(vector<vector<R> > c);
	explicit Matrix(vector<R> c, int rows, int cols);
	explicit Matrix(vector<Vector<R>> c);
	explicit Matrix(int rows, int cols, BIHOM(int, int, R) c);
	explicit Matrix(int n = 3, R diag = R(0));
	explicit Matrix(Vector<R> v);

	Matrix(R a, R b, R c, R d);
	Matrix(R a, R b, R c, R d, R e, R f, R g, R h, R i);

	explicit Matrix(mat2 m) : Matrix(R(m[0][0]), R(m[0][1]), R(m[1][0]), R(m[1][1])) {}
	explicit Matrix(mat3 m) : Matrix(R(m[0][0]), R(m[0][1]), R(m[0][2]), R(m[1][0]), R(m[1][1]), R(m[1][2]), R(m[2][0]), R(m[2][1]), R(m[2][2])) {}
	explicit Matrix(mat4 m);

	ivec2 size() const;
	Matrix operator*(const R &c) const;
	Matrix operator/(const R &c) const;
	Matrix operator+(const Matrix &M) const;
	Matrix operator*(const Matrix &M) const;

	Matrix pointwise_product(const Matrix &M) const;
	Matrix pointwise_division(const Matrix &M) const;
	Matrix operator-(const Matrix &M) const;
	Matrix transpose() const;
	Matrix operator-() const;
	Matrix operator~() const;

	bool operator==(const Matrix &M) const;
	bool operator!=(const Matrix &M) const;

	bool square() const;
	bool symmetric() const;

	R det() const;
	R minor(int i, int j) const;
	R trace() const;
	Matrix adjugate() const;
	Matrix inv() const;
	Matrix pow(int p) const;

	float normL_inf() const;
	bool nearly_equal(const Matrix &M) const;

	explicit operator string() const { return std::format("({})", coefs); }


	Vector<R> column(int j) const;
	Vector<R> row(int i) const;

	R& at(int i, int j);
	R& operator[](int i, int j) { return at(i, j); }

	Vector<R> operator*(const Vector<R> &v) const;

	R a() const;
	R b() const;
	R c() const;
	R d() const;
	R e() const;
	R f() const;
	R g() const;
	R h() const;
	R i() const;
	R mobius(R z) const { return (a() * z + b()) / (c() * z + d()); }
	R mobius_derivative(R z) const { return (a() * d() - b() * c()) / ((c() * z + d()) * (c() * z + d())); }
};

template<typename R=float>
Matrix<R> norm2(const Matrix<R> &M) {
	return pow2(M.normL_inf());
}



class SparseMatrix {
	vector<vector<std::pair<int, float> > > data;
	int n, m;

public:
	SparseMatrix(int n, int m);

	void set(int i, int j, float val);
	float get(int i, int j);
	ivec2 size() const { return ivec2(n, m); }

	float operator()(int i, int j) { return get(i, j); }
	SparseMatrix operator*(float f);
	SparseMatrix operator+(const SparseMatrix &M);
	SparseMatrix operator-(const SparseMatrix &M);
};




class FloatVector {
	vector<float> data;

public:
	explicit FloatVector(const vector<float> &data) { this->data = data; }
	explicit FloatVector(vec2 data);
	explicit FloatVector(vec3 data);
	explicit FloatVector(vec4 data);
	explicit FloatVector(const vector<vector<float> > &data);
	FloatVector(int n, float val);
	explicit FloatVector(int n);

	FloatVector(const FloatVector &other) = default;
	FloatVector(FloatVector &&other) noexcept;
	FloatVector &operator=(const FloatVector &other);
	FloatVector &operator=(FloatVector &&other) noexcept;


	float operator[](int i) const;
	FloatVector operator*(float f) const;
	FloatVector operator/(float f) const;
	FloatVector operator+(const FloatVector &v) const;
	FloatVector operator-(const FloatVector &v) const;
	FloatVector operator-() const;
	void operator+=(const FloatVector &v);
	void operator-=(const FloatVector &v);
	void operator*=(float f);
	void operator/=(float f);

	void append(float f);
	void append(const FloatVector &v);
	void append(const vec69 &v);

	vector<float> getVec() const;
	int size();

	friend float dot(const FloatVector &a, const FloatVector &b);
	friend FloatVector concat(const FloatVector &a, const FloatVector &b);
};


class FloatMatrix {
protected:
	MATR$X data;

public:
	FloatMatrix(int n, int m);
	explicit FloatMatrix(const MATR$X &data);

	explicit FloatMatrix(const vector<float> &data) : data({data}) {}
	explicit FloatMatrix(const vector<vec2> &m) : data(map_cr<vec2, vector<float> >(m, vecToVecHeHe<vec2>)) {}
	explicit FloatMatrix(const vector<vec3> &data) : data(map_cr<vec3, vector<float> >(data, vecToVecHeHe<vec3>)) {}
	explicit FloatMatrix(const vector<vec4> &data) : data(map_cr<vec4, vector<float> >(data, vecToVecHeHe<vec4>)) {}

	explicit FloatMatrix(MATR$X &&data);
	explicit FloatMatrix(vec69 &&data);

	void set(int i, int j, float val);
	float get(int i, int j) const;
	bool isSquare() const;
	float det();
	FloatMatrix transpose() const;
	FloatMatrix operator*(float f) const;
	FloatMatrix operator*(int f) const {return *this * float(f); }

	FloatMatrix operator+(const FloatMatrix &M) const;
	FloatMatrix operator-(const FloatMatrix &M) const;
	FloatMatrix operator*(const FloatMatrix &M) const;
	FloatMatrix operator*(const MATR$X &M) const;

	vec69 operator*(const vec69 &v) const;
	FloatVector operator*(const FloatVector &v) const;

	FloatMatrix operator-() const;
	FloatMatrix operator/(float x) const;
	FloatMatrix operator/(int x) const { return *this / float(x); }
	FloatMatrix inv();
	FloatMatrix pow(int p);
	FloatMatrix operator~();
	FloatMatrix GramSchmidtProcess();
	FloatMatrix submatrix(int i, int j);
	FloatMatrix diagonalComponent() const;
	FloatMatrix invertedDiagonal() const;
	FloatMatrix subtractedDiagonal() const;

	vec69 operator[](int i);
	friend FloatMatrix operator*(const MATR$X &M, const FloatMatrix &B);

	ivec2 size() const;
	int n() const;
	int m() const;

	explicit operator float();
	explicit operator vec2();
	explicit operator vec3();
	explicit operator vec4();
};




template<Rng R=float, RModule<R> M=R>
class GenericTensor {
	vector<M> data;
	int dim_;

public:
	GenericTensor(int length, M fill, int dim = 1)
	: dim_(dim) {
		data = vector<M>();
		data.reserve(length);
		for (int i = 0; i < length; i++) data.push_back(fill);
	}

	explicit GenericTensor(vector<M> data, int dim = 1)
	: data(data),
	  dim_(dim) {}

	M operator[](int i) const { return this->data[i]; }

	template<RModule<R> E0=R>
	E0 at(int i, int j) const { return this->data[i][j]; }

	template<RModule<R> E0=R>
	E0 at(int i, int j, int k) const { return this->data[i][j][k]; }

	template<RModule<R> E0=R>
	E0 at(vector<int> indices) const;

	template<RModule<R> E0=R>
	E0 at(int i) const { return this->data[i]; }

	template<RModule<R> E0=R>
	void set(vector<int> ind, E0 val);

	template<RModule<R> E0=R>
	void set(int i, E0 val) { this->data[i] = val; }

	template<RModule<R> E0=R>
	void set(int i, int j, E0 val) { this->data[i][j] = val; }

	template<RModule<R> E0=R>
	void set(int i, int j, int k, E0 val) { this->data[i][j][k] = val; }


	GenericTensor<R, GenericTensor> pretendFat() const { return GenericTensor<R, GenericTensor>({*this}, dim_ + 1); }
	int dim() const { return this->dim_; }
	int size() const { return this->data.size(); }

	GenericTensor operator*(R c) const { return GenericTensor(map(this->data, [c](M x){ return x * c; }), dim); }
	GenericTensor operator/(R c) const requires DivisionRing<R> { return *this * (1.f / c); }
	GenericTensor operator+(const GenericTensor &T) const { return GenericTensor(map(this->data, T.data, [](M x, M y){ return x + y; }), dim); }
	GenericTensor operator-(const GenericTensor &T) const { return *this + T * -1; }
	GenericTensor operator*(const GenericTensor &T) const { return GenericTensor(map(this->data, T.data, [](M x, M y){ return x * y; }), dim); }
	GenericTensor operator-() const { return *this * -1; }

	template<typename dom>
	Morphism<dom, GenericTensor> switchEvaluationOrder(GenericTensor<R, Morphism<dom, M> > M_f) const {
		return Morphism<dom, GenericTensor>([this, M_f](dom x){ return GenericTensor(map(this->data, [x, M_f](M f){ return M_f(x)(f); }), dim); });
	}

	template<typename dom>
	GenericTensor<R, Morphism<dom, M> > switchEvaluationOrder(Morphism<dom, GenericTensor> f_M) const {
		return Morphism<dom, GenericTensor>([this, f_M](dom x){ return GenericTensor(map(this->data, [x, f_M](M f){ return f_M(x)(f); }), dim); });
	}
};




template<Rng R>
ivec2 mat_size(const GEN_MAT(R) &M) { return ivec2(M.size(), M[0].size()); }



template<Rng R>
GEN_MAT(R) operator*(const GEN_MAT(R) &M1, const GEN_MAT(R) &M2) {
	if (mat_size(M1).y != mat_size(M2).x) throw std::format_error("wrong dimensions of matrices");
	GEN_MAT(R) res = GEN_MAT(R)(mat_size(M1).x, GEN_VEC(R)(mat_size(M2).y, R(0)));
	for (int i = 0; i < mat_size(M1).x; i++)
		for (int j = 0; j < mat_size(M2).y; j++)
			for (int k = 0; k < mat_size(M1).y; k++)
				res.set(i, j, res.at(i, j) + M1[i][k] * M2[k][j]);
	return res;
}




class Complex {
public:
	vec2 z;

	Complex();
	explicit Complex(vec2 z);
	Complex(float x, float y);
	Complex(int x, float y);
	Complex(float x, int y);
	Complex(int x, int y);
	explicit Complex(float x);
	explicit Complex(int x);

	Complex operator+(Complex c) const;
	Complex operator-(Complex c) const;
	Complex operator*(Complex c) const;
	Complex operator/(Complex c) const;
	Complex operator+(float c) const;
	Complex operator-(float c) const;
	Complex operator*(float c) const;
	Complex operator/(float c) const;
	Complex operator+(int c) const;
	Complex operator-(int c) const;
	Complex operator*(int c) const;
	Complex operator/(int c) const;
	Complex operator~() const;
	Complex operator-() const;

	void operator+=(const Complex &c);
	void operator-=(const Complex &c);
	void operator*=(const Complex &c);
	void operator/=(const Complex &c);

	friend Complex operator*(float f, const Complex &c);
	friend Complex operator/(float f, const Complex &c);
	friend Complex operator+(float f, const Complex &c);
	friend Complex operator-(float f, const Complex &c);
	friend Complex operator*(int f, const Complex &c);
	friend Complex operator/(int f, const Complex &c);
	friend Complex operator+(int f, const Complex &c);
	friend Complex operator-(int f, const Complex &c);


	Complex inv() const;
	auto operator==(Complex c) const -> bool;
	auto operator==(float f) const -> bool;
	vec2 expForm() const;
	float arg() const;
	Complex conj() const;
	Complex pow(int k) const;
	Complex pow(float exponent) const;

	float re() const;
	float im() const;
	float real() const;
	float imag() const;
	float x() const { return re(); }
	float y() const { return im(); }

	auto square() const -> Complex;
	auto sqrt() const -> Complex;

	explicit operator glm::vec2() const;
	explicit operator string() const;

	bool nearlyEqual(Complex c) const;
	bool nearlyZero() const { return nearlyEqual(Complex(0, 0)); }

	string to_str() const { return std::format("{0}+{1}i", z[0], z[1]); }
	friend std::ostream &operator<<(std::ostream &_stream, Complex const &z);
};




Complex operator""i(long double im);
Complex operator""i(unsigned long long int im);

float norm2(Complex c);
float abs(Complex c);


Complex fromExpForm(vec2 e);
Complex fromExpForm(float r, float theta);

Complex rootOfUnity(int n, int k);
vector<Complex> allRootsOf1(int n);
Complex exp(Complex c);
Complex log(Complex c);
Complex sin(Complex c);
Complex cos(Complex c);
Complex tan(Complex c);
Complex sinh(Complex c);
Complex cosh(Complex c);
Complex tanh(Complex c);
Complex asin(Complex c);
Complex acos(Complex c);
Complex atan(Complex c);
Complex asinh(Complex c);
Complex acosh(Complex c);
Complex atanh(Complex c);
Complex cot(Complex c);
Complex sqrt(Complex c);
Complex sq(Complex c);
Complex pow2(Complex c);
Complex pow3(Complex c);
Complex pow4(Complex c);
Complex pow(Complex c, float n);
Complex pow(Complex c, Complex n);
Complex pow(float x, Complex n);



Complex intersectLines(Complex p1, Complex p2, Complex q1, Complex q2);




class Quaternion {
	vec4 q;

public:
	explicit Quaternion(vec4 q) : q(q) {}
	Quaternion(Complex z) : q(vec4(z.re(), z.im(), 0, 0)) {}


	Quaternion(float x, float y, float z, float w) : q(vec4(x, y, z, w)) {}
	explicit Quaternion(float x) : q(vec4(x, 0, 0, 0)) {}
	explicit Quaternion(vec3 im) : q(vec4(0, im)) {}

	Quaternion operator*(Quaternion r) const;
	Quaternion operator*(float f) const { return Quaternion(q * f); }
	Quaternion operator/(float f) const { return *this * (1.f / f); }
	Quaternion operator+(Quaternion r) const { return Quaternion(q + r.q); }
	Quaternion operator-(Quaternion r) const { return *this + r * -1; }
	Quaternion operator-() const { return *this * -1; }
	float norm2() const { return dot(q, q); }
	float norm() const { return std::sqrt(norm2()); }
	Quaternion inv() const { return conj() / norm2(); }
	Quaternion operator~() const { return inv(); }
	Quaternion pow(int p) const;
	Quaternion operator+(float f) const { return *this + Quaternion(f); }
	Quaternion operator-(float f) const { return *this - Quaternion(f); }
	Quaternion operator/(Quaternion r) const { return *this * r.inv(); }
	friend Quaternion operator*(float f, Quaternion x) { return x * f; }
	friend Quaternion operator/(float f, Quaternion x) { return Quaternion(f) / x; }
	friend Quaternion operator+(float f, Quaternion x) { return x + f; }
	friend Quaternion operator-(float f, Quaternion x) { return Quaternion(f) - x; }
	friend float dot(Quaternion a, Quaternion b) { return dot(a.q, b.q); }
	explicit operator vec4() const { return q; }
	explicit operator string() const { return std::format("{0}+{1}i+{2}j+{3}k", q.x, q.y, q.z, q.w); }
	constexpr float re() const { return q.x; }
	constexpr vec3 im() const { return vec3(q.y, q.z, q.w); }

	Quaternion conj() const { return Quaternion(q.x, -q.y, -q.z, -q.w); }
	Quaternion normalise() const { return *this / norm(); }
	vec3 rotate(vec3 im_x) const;
	HOM(vec3, vec3) rotation() const;
	vec3 Hopf_map() const;
	vec4 rotateS3(vec4 x) const;

	static Quaternion one() { return Quaternion(1, 0, 0, 0); }
	static Quaternion zero() { return Quaternion(0, 0, 0, 0); }
	static Quaternion i() { return Quaternion(0, 1, 0, 0); }
	static Quaternion j() { return Quaternion(0, 0, 1, 0); }
	static Quaternion k() { return Quaternion(0, 0, 0, 1); }
};




const Quaternion one_H = Quaternion::one();
const Quaternion zero_H = Quaternion::zero();
const Quaternion i_H = Quaternion::i();
const Quaternion j_H = Quaternion::j();
const Quaternion k_H = Quaternion::k();




inline float frac(float x) { return x - std::floor(x); }
inline int sgn(float x) { return x < 0 ? -1 : 1; }
inline int sign(float x) { return sgn(x); }

float pseudorandomizer(float x, float seed = 0.f);
int binomial(int n, int k);


template<typename M>
float det(const M &m) {
	return determinant(m);
}

template<typename T>
T normalise(T v) {
	return norm(v) < 1e-6 ? v * 0.f : v / norm(v);
}

template<typename T, typename U=T>
bool nearlyEqual(T a, U b) {
	return norm(a - b) < 1e-4;
}

template<typename M>
bool nearlyEqual_mat(M a, M b) {
	return a.nearly_equal(b);
}





// inline bool nearlyEqual(float a, int b) { return norm(a - b) < 1e-6; }
// inline bool nearlyEqual(int a, float b) { return norm(a - b) < 1e-6; }

template<typename T>
bool nearlyEqual(T a) { return norm(a) < 1e-6; }



template<typename vec>
vec barycenter(vec a, vec b, vec c) { return (a + b + c) / 3.f; };



vec2 intersectLines(vec2 p1, vec2 p2, vec2 q1, vec2 q2);
inline vec2 projectVectorToVector(vec2 v, vec2 n) {
	return v - dot(v, n) * n;
}
inline mat2 scaleMatrix2(vec2 s) {
	return mat2(s.x, 0, 0, s.y);
}
inline mat2 scaleMatrix2(float s) {
	return scaleMatrix2(vec2(s));
}
inline mat2 changeOfBasis(vec2 t1, vec2 t2) {
	return inverse(mat2(t1, t2));
}
inline mat2 changeOfBasis(vec2 s1, vec2 s2, vec2 t1, vec2 t2) {
	return changeOfBasis(t1, t2) * mat2(s1, s2);
}
mat2 rotationMatrix2(float angle);
mat2 rotationBetween(vec2 v0, vec2 v1);

inline vec2 orthogonalComplement(vec2 v) { return vec2(-v.y, v.x); }
inline mat2 GramSchmidtProcess(mat2 m) { return mat2(normalise<vec2>(m[0]), normalise(m[1] - normalise(m[0]) * dot(normalise(m[0]), m[1]))); }



// TODO: make proper tensors, this is just a crime agains humanity or at least against Grothendieck
template<VectorSpaceConcept<float> V, VectorSpaceConcept<float> M>
float bilinearForm(V a, V b, M m) {
	float res = 0;
	for (int i = 0; i < a.length(); i++)
		for (int j = 0; j < b.length(); j++)
			res += a[i] * m[i][j] * b[j];
	return res;
}


std::pair<Complex, Complex> eigenvalues(mat2 m);
inline vec2 eigenvaluesReal(mat2 m) { return vec2(eigenvalues(m).first.re(), eigenvalues(m).second.re()); }
inline bool eigenvectorExists(mat2 m) { return !isClose(m, mat2(0)) && isClose(eigenvalues(m).first.im(), 0); }
bool eigenbasisExists(mat2 m);
vec2 eigenvector(mat2 m, float eigenvalue);
std::pair<vec2, mat2> eigendecomposition(mat2 m);




template<typename V, typename M>
class EuclideanSpace {
	V value;
	std::shared_ptr<M> metric;

public:
	int dim = value.length();

	EuclideanSpace(V value, std::shared_ptr<M> metric)
	: value(value),
	  metric(metric) {}

	EuclideanSpace(V value, M &&metric)
	: value(value),
	  metric(std::make_shared<M>(metric)) {}

	M metricTensor() { return *metric; }

	explicit operator V() { return value; }
	EuclideanSpace operator+(EuclideanSpace other) { return EuclideanSpace(value + other.value, metric); }
	EuclideanSpace operator-(EuclideanSpace other) { return EuclideanSpace(value - other.value, metric); }
	EuclideanSpace operator*(float f) { return EuclideanSpace(value * f, metric); }
	EuclideanSpace operator/(float f) { return EuclideanSpace(value / f, metric); }
	EuclideanSpace operator-() { return EuclideanSpace(-value, metric); }

	friend EuclideanSpace operator*(float f, EuclideanSpace v) { return v * f; }
	friend EuclideanSpace operator/(float f, EuclideanSpace v) { return EuclideanSpace(f / v.value, v.metric); }
	friend float dot(EuclideanSpace a, EuclideanSpace b) { return bilinearForm(a.value, b.value, *a.metric); }

	float norm2() { return dot(*this, *this); }
	float norm() { return sqrt(norm2()); }
	EuclideanSpace normalise() { return *this / norm(); }
	EuclideanSpace orthogonalProjection(EuclideanSpace v) { return v - normalise(*this) * dot(v, *this); }
	M GSProcess(const M &basis);




	vec2 toVec2() {
		if (dim != 2) throw std::format_error("wrong dimension of vector (not 2)");
		return vec2(value);
	}

	vec3 toVec3() {
		if (dim != 3) throw std::format_error("wrong dimension of vector (not 2)");
		return vec3(value);
	}

	vec4 toVec4() {
		if (dim != 4) throw std::format_error("wrong dimension of vector (not 2)");
		return vec4(value);
	}

	vector<Complex> eigenvalues(M m) {
		if (dim != 2) throw std::format_error("eigendecomposition currently implemented only in dimension 2");
		return eigenvalues(static_cast<mat2>(m));
	}

	M orthogonalEigenbasis() {
		if (dim != 2) throw std::format_error("eigendecomposition currently implemented only in dimension 2");
		return GSProcess(static_cast<M>(eigendecomposition(static_cast<mat2>(metric)).second));
	}

	bool orthogonalEigenbasisExists() {
		if (dim != 2) throw std::format_error("eigendecomposition currently implemented only in dimension 2");
		return eigenbasisExists(static_cast<mat2>(metric));
	}
};




template<VectorSpaceConcept<float> V, VectorSpaceConcept<float> M>
float norm(const EuclideanSpace<V, M> &v) { return v.norm(); }

template<VectorSpaceConcept<float> V, VectorSpaceConcept<float> M>
float norm2(EuclideanSpace<V, M> v) { return v.norm2(); }

template<VectorSpaceConcept<float> V, VectorSpaceConcept<float> M>
EuclideanSpace<V, M> normalise(EuclideanSpace<V, M> v) { return v / norm(v); }

vec3 projectVectorToPlane(vec3 v, vec3 n);
mat3 scaleMatrix3(vec3 s);
mat3 scaleMatrix3(float s);
inline mat3 changeOfBasis(vec3 t1, vec3 t2, vec3 t3) { return inverse(mat3(t1, t2, t3)); }
inline mat3 changeOfBasis(vec3 s1, vec3 s2, vec3 s3, vec3 t1, vec3 t2, vec3 t3) { return changeOfBasis(t1, t2, t3) * mat3(s1, s2, s3); }
mat3 rotationMatrix3(float angle);
mat3 rotationMatrix3(vec3 axis, float angle);
mat3 rotationBetween(vec3 v0, vec3 v1);

std::pair<vec3, vec3> orthogonalComplementBasis(vec3 v);
mat3 GramSchmidtProcess(mat3 m);


inline float min(float a, float b) { return a < b ? a : b; }
inline float max(float a, float b) { return a > b ? a : b; }
inline vec3 max(vec3 a, float b) { return vec3(max(a.x, b), max(a.y, b), max(a.z, b)); }
inline vec3 min(vec3 a, float b) { return vec3(min(a.x, b), min(a.y, b), min(a.z, b)); }
inline float sin2(float x) { return sin(x) * sin(x); }
inline float cos2(float x) { return cos(x) * cos(x); }




class vec5 {
public:
	float x, y, z, w, v;

	vec5();
	explicit vec5(float x);
	vec5(float x, float y, float z, float w, float v);
	vec5(std::initializer_list<float> list);

	float operator[](int i) const;
	friend vec5 operator+(const vec5 &a, const vec5 &b);
	vec5 operator-(const vec5 &b) const;
	vec5 operator-() const;
	vec5 operator*(float scalar) const;
	friend vec5 operator*(float scalar, const vec5 &a);
	vec5 operator/(float scalar) const;

	static vec5 zero();
};




template<typename T>
int binSearch(vector<T> v, T x) {
	int l = 0;
	int r = v.size() - 1;
	while (l <= r) {
		int m = l + (r - l) / 2;

		if (v[m] == x) return m;
		if (v[m] < x) l = m + 1;
		else r = m - 1;
	}
	return 0;
}




template<typename domain, typename codomain>
Morphism<domain, codomain> Morphism<domain, codomain>::operator-(const Morphism &g) const requires AbelianGroupConcept<codomain> {
	return Morphism([this, g](domain x){ return (*this)(x) - g(x); });
}

template<typename domain, typename codomain>
Morphism<domain, codomain> Morphism<domain, codomain>::operator*(const Morphism &g) const requires Semigroup<codomain> {
	return Morphism([this, g](domain x){ return (*this)(x) * g(x); });
}

template<typename domain, typename codomain>
Morphism<domain, codomain> Morphism<domain, codomain>::operator/(const Morphism &g) const requires DivisionRing<codomain> {
	return Morphism([this, g](domain x){ return (*this)(x) / g(x); });
}

template<typename domain, typename codomain>
Morphism<domain, codomain> Morphism<domain, codomain>::operator*(codomain a) const requires Semigroup<codomain> {
	return Morphism([this, a](domain x){ return (*this)(x) * a; });
}

template<Rng T>
Vector<T>::Vector(vector<T> c)
: n(c.size()), coefs(c) {}

template<Rng T>
Vector<T>::Vector(int n, std::function<T(int)> f)
: n(n)
{
	coefs = vector<T>();
	for (int i = 0; i < n; i++)
		coefs.emplace_back(f(i));
}

template<Rng T>
Vector<T>::Vector(const Vector &other)
: n(other.n),
  coefs(other.coefs) {}

template<Rng T>
Vector<T>::Vector(Vector &&other) noexcept
: n(other.n),
  coefs(std::move(other.coefs)) {}

template<Rng T>
Vector<T> &Vector<T>::operator=(const Vector &other) {
	if (this == &other)
		return *this;
	n = other.n;
	coefs = other.coefs;
	return *this;
}

template<Rng T>
Vector<T> &Vector<T>::operator=(Vector &&other) noexcept {
	if (this == &other)
		return *this;
	n = other.n;
	coefs = std::move(other.coefs);
	return *this;
}

template<Rng T>
Vector<T> Vector<T>::operator*(const T &f) const {
	return Vector(n, [this, f](int i){ return coefs[i] * f; });
}

template<Rng T>
Vector<T> Vector<T>::operator+(const T &f) const {
	return Vector(n, [this, f](int i){ return coefs[i] + f; });
}

template<Rng T>
Vector<T> Vector<T>::operator-(const T &f) const {
	return Vector(n, [this, f](int i){ return coefs[i] - f; });
}

template<Rng T>
Vector<T> Vector<T>::operator/(const T &f) const requires DivisionRing<T> {
	return Vector(n, [this, f](int i){ return coefs[i]/f; });
}

template<Rng T>
Vector<T> Vector<T>::operator+(const Vector &v) const {
	return Vector(n, [this, v](int i){ return coefs[i] + v[i]; });
}

template<Rng T>
Vector<T> Vector<T>::operator-(const Vector &v) const {
	return *this + v * -1;
}

template<Rng T>
Vector<T> Vector<T>::operator-() const {
	return *this * -T(1);
}

template<Rng T>
Vector<T>::operator string() const {
	return std::format("({})", coefs);
}

template<Rng T>
constexpr int Vector<T>::length() const {
	return n;
}

template<Rng T>
constexpr int Vector<T>::size() const {
	return n;
}

template<Rng T>
bool Vector<T>::operator==(const Vector &v) const {
	return coefs == v.coefs;
}

template<Rng T>
bool Vector<T>::operator!=(const Vector &v) const {
	return !(*this == v);
}




template<typename T>
Matrix<T>::Matrix(int rows, int cols, std::function<T(int, int)> c)
: rows(rows), cols(cols), coefs()
{
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			coefs.emplace_back(c(i, j));
}

template<typename R>
Matrix<R>::Matrix(int n, R diag): Matrix(n, n, [diag](int i, int j){ return i == j ? diag : diag * 0; }) {}

template<typename R>
Matrix<R>::Matrix(Vector<R> v): Matrix(v.size(), 1, [v](int i, int j){ return v[i]; }) {}

template<typename R>
Matrix<R>::Matrix(R a, R b, R c, R d)
: Matrix(vector<vector<R> >{{a, b}, {c, d}}) {}

template<typename R>
Matrix<R>::Matrix(R a, R b, R c, R d, R e, R f, R g, R h, R i)
: Matrix(vector<vector<R> >{{a, b, c}, {d, e, f}, {g, h, i}}) {}




//
// template<Rng T>
// T Matrix<T, 2>::mobius_derivative(T z) const requires DivisionRing<T> { // todo
// 	return (a * d - b * c) / ((c * z + d) * (c * z + d));
// }
//
//
// template<int n, int m>
// bool nearlyEqual(Matrix<Complex, n, m> a, Matrix<Complex, n, m> b) { // todo
// 	for (int i = 0; i < m; i++)
// 		for (int j = 0; j < n; j++)
// 			if (!a.at(i, j).nearlyEqual(b.at(i, j)))
// 				return false;
// 	return true;
// }



template<typename V, typename M>
M EuclideanSpace<V, M>::GSProcess(const M &basis) {
	M result = basis;
	for (int i = 0; i < dim; i++) {
		EuclideanSpace v = EuclideanSpace(basis[i], metric);
		for (int j = 0; j < i; j++) {
			EuclideanSpace u = EuclideanSpace(result[j], metric);
			v = v - normalise(u) * dot(v, u);
		}
		result[i] = static_cast<V>(normalise(v));
	}
	return result;
}



template<typename T>
vector<T> reverse(vector<T> v) {
	std::reverse(v.begin(), v.end());
	return v;
}

template<AbelianSemigroup V>
vector<V> arange(V a, V b, V step) {
	if (step == V(0)) throw std::invalid_argument("step cannot be 0");

	vector<V> res = vector<V>();
	res.reserve(abs(b - a) / abs(step));

	if (step > 0) {
		for (V i = a; i < b; i += step)
			res.push_back(i);
		return res;
	}

	for (V i = b; i > a; i += step)
		res.push_back(i);
	return res;
}

template<typename T>
vector<T> smartRange(vector<T> v, int a, int b, int step = 1) {
	if (step == 0) throw std::invalid_argument("step cannot be 0");

	vector<T> res = vector<T>();
	res.reserve(abs(b - a) / abs(step));

	if (step > 0) {
		for (int i = a; i < b; i += step)
			res.push_back(v[i]);
		return res;
	}

	for (int i = b; i > a; i += step)
		res.push_back(v[i]);
	return res;
}



template<typename R>
Matrix<R> Matrix<R>::transpose() const {
	vector<R> transposed_coefs = vector<R>(rows * cols);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			transposed_coefs[j * rows + i] = coefs[i * cols + j];
		}
	}
	return Matrix(transposed_coefs, cols, rows);
}

template<typename R>
Matrix<R> Matrix<R>::operator-() const { return *this * -1; }

template<typename R>
Matrix<R> Matrix<R>::operator~() const {
	if (rows != cols) throw std::format_error("Matrix must be square");
	if (rows == 1) return Matrix(1, 1, [this](int i, int j){ return 1.f / coefs[0][0]; });
	if (rows == 2) return Matrix(coefs[1][1], -coefs[0][1], -coefs[1][0], coefs[0][0]) / det();
	return adjugate() / det();
}

template<typename R>
bool Matrix<R>::operator==(const Matrix &M) const {
	if (rows != M.rows || cols != M.cols)
		return false;
	for (int i = 0; i < coefs.size(); i++)
		if (coefs[i] != M.coefs[i])
			return false;
	return true;

}

template<typename R>
bool Matrix<R>::operator!=(const Matrix &M) const { return !(*this == M); }

template<typename R>
bool Matrix<R>::square() const { return rows == cols; }

template<typename R>
bool Matrix<R>::symmetric() const { return square() and *this == transpose(); }

template<typename T>
T Matrix<T>::det() const {
	if (rows != cols) throw std::invalid_argument("Matrix must be square");
	if (rows == 1) return coefs[0][0];
	if (rows == 2) return coefs[0][0] * coefs[1][1] - coefs[0][1] * coefs[1][0];
	T result = 0;
	for (int j = 0; j < cols; j++)
		result += coefs[0][j] * minor(0, j) * (j % 2 == 0 ? 1 : -1);
	return result;
}

template<typename R>
R Matrix<R>::minor(int i, int j) const {
	if (rows != cols) throw std::invalid_argument("Matrix must be square");
	Matrix<R> result = Matrix<R>(rows - 1, cols - 1);
	for (int k = 0; k < rows; k++)
		for (int l = 0; l < cols; l++)
			if (k != i && l != j) result[k < i ? k : k - 1][l < j ? l : l - 1] = coefs[k][l];
	return result.det();
}


template<typename R>
Matrix<R>::Matrix(mat4 m): Matrix(vector<vector<R> >{{R(m[0][0]), R(m[0][1]), R(m[0][2]), R(m[0][3])}, {R(m[1][0]), R(m[1][1]), R(m[1][2]), R(m[1][3])}, {R(m[2][0]), R(m[2][1]), R(m[2][2]), R(m[2][3])}, {R(m[3][0]), R(m[3][1]), R(m[3][2]), R(m[3][3])}}) {}

template<typename R>
ivec2 Matrix<R>::size() const { return ivec2(rows, cols); }

template<typename R>
Matrix<R> Matrix<R>::operator*(const R &c) const {
	vector<R> new_coefs = coefs;
	for (int i = 0; i < new_coefs.size(); i++)
		new_coefs[i] = new_coefs[i] * c;
	return Matrix(std::move(new_coefs), rows, cols);
}
template<typename R>
Matrix<R> Matrix<R>::operator/(const R &c) const {
	vector<R> new_coefs = coefs;
	for (int i = 0; i < new_coefs.size(); i++)
		new_coefs[i] = new_coefs[i] / c;
	return Matrix(std::move(new_coefs), rows, cols);
}
template<typename R>
Matrix<R> Matrix<R>::operator+(const Matrix &M) const {
	return Matrix(rows, cols, [this, M](int i, int j){
		if (M.size() != size()) throw std::invalid_argument("Matrix dimensions must agree");
		return coefs[i][j] + M[i][j];
	});
}

template<typename R>
Matrix<R> Matrix<R>::operator*(const Matrix &M) const {
	if (cols != M.rows) throw std::invalid_argument("Matrix dimensions must agree");
	return Matrix(rows, M.cols, [this, M](int i, int j){
		R sum = coefs[0][0] * 0;
		for (int k = 0; k < cols; k++)
			sum += coefs[i][k] * M[k][j];
		return sum;
	});
}

template<typename R>
Matrix<R> Matrix<R>::pointwise_product(const Matrix &M) const { return Matrix(rows, cols, [this, M](int i, int j){ return at(i, j) * M.at(i, j); }); }

template<typename R>
Matrix<R> Matrix<R>::pointwise_division(const Matrix &M) const { return Matrix(rows, cols, [this, M](int i, int j){ return at(i, j) / M.at(i, j); }); }

template<typename R>
Matrix<R> Matrix<R>::operator-(const Matrix &M) const { return *this + M * -1; }

template<typename T>
Matrix<T>::Matrix(int rows, int cols)
: coefs(vector<T>(rows * cols, T(0))),
  rows(rows),
  cols(cols) {}


template<typename T>
Matrix<T>::Matrix(vector<vector<T> > c)
: rows(c.size()),
  cols(c[0].size()),
  coefs(flatten<T>(c)) {}

template<typename R>
Matrix<R>::Matrix(vector<R> c, int rows, int cols): rows(rows), cols(cols), coefs(std::move(c)) {}

template<typename T>
Matrix<T>::Matrix(vector<Vector<T> > c)
: rows(c.size()),
  cols(c[0].size()) {
	coefs = vector<T>();
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			coefs.push_back(c[i][j]);
}




template<typename R>
R Matrix<R>::trace() const {
	if (not square()) throw std::invalid_argument("Matrix must be square");
	R res = coefs[0][0];
	for (int i = 1; i < rows; i++) res += coefs[i][i];
	return res;
}

template<typename R>
Matrix<R> Matrix<R>::adjugate() const { return Matrix(rows, cols, [this](int i, int j){ return minor(j, i) * ((i + j) % 2 == 0 ? 1 : -1); }); }

template<typename R>
Matrix<R> Matrix<R>::inv() const { return adjugate() / det(); }

template<typename R>
Matrix<R> Matrix<R>::pow(int p) const {
	if (!square()) throw std::invalid_argument("Matrix must be square");
	if (p < 0) return ~(*this).pow(-p);
	return p == 0 ? Matrix(rows, R(1)) : p == 1 ? *this : p % 2 == 0 ? pow(p / 2) * pow(p / 2) : *this * pow(p / 2) * pow(p / 2);
}

template<typename R>
float Matrix<R>::normL_inf() const {
	float res = 0;
	for (int i = 0; i < rows; i++)
		res = std::max(res, abs(coefs[i][0]));
	return res;
}

template<typename R>
bool Matrix<R>::nearly_equal(const Matrix &M) const {
	return abs(normL_inf() - M.normL_inf()) < 1e-6;;
}


template<typename R>
Vector<R> Matrix<R>::column(int j) const {
	vector<R> c = vector<R>();
	for (int i = 0; i < rows; i++)
		c.emplace_back(coefs[j + i * cols]);
	return Vector<R>(c);
}

template<typename R>
Vector<R> Matrix<R>::row(int i) const {
	vector<R> c = vector<R>();
	for (int j = 0; j < rows; j++)
		c.emplace_back(coefs[i * cols + j]);
	return Vector<R>(c);
}

template<typename R>
R& Matrix<R>::at(int i, int j) {
	if (i < -rows || i >= rows || j < -cols || j >= cols)
		throw IndexOutOfBounds(std::format("({}, {})", i, j), std::format("({}, {})", rows, cols), "Matrix", __FILE__, __LINE__);
	if (i < 0) i = rows + i;
	if (j < 0) j = cols + j;
	return coefs[i * cols + j];
}

template<typename R>
Vector<R> Matrix<R>::operator*(const Vector<R> &v) const {
	if (v.size() != cols)
		throw ValueError("Matrix sizes do not match for multiplication", __FILE__, __LINE__);
	Vector<R> res = Vector(rows);
	for (int i = 0; i < rows; i++) {
		R sum = R(0);
		for (int j = 0; j < cols; j++)
			sum += at(i, j) * v[j];
	}
	return res;
}

template<typename R>
R Matrix<R>::a() const {
	if (!square() || rows > 3) throw ValueError("Matrix must be 1x1 or 2x2 or 3x3", __FILE__, __LINE__);
	return at();
}




template<typename R>
R Matrix<R>::b() const {
	if (!square() || rows > 3 || rows < 2) throw ValueError("Matrix must be 2x2 or 3x3", __FILE__, __LINE__);
	return at(0, 1);
}




template<typename R>
R Matrix<R>::c() const {
	if (!square() || rows > 3 || rows < 2) throw ValueError("Matrix must be 2x2 or 3x3", __FILE__, __LINE__);
	return cols == 3 ? at(0, 2) : at(1, 0);
}




template<typename R>
R Matrix<R>::d() const {
	if (!square() || rows > 3 || rows < 2) throw ValueError("Matrix must be 2x2 or 3x3", __FILE__, __LINE__);
	return cols == 3 ? at(1, 0) : at(1, 1);
}




template<typename R>
R Matrix<R>::e() const {
	if (!square() || cols != 3) throw ValueError("Matrix must be 3x3", __FILE__, __LINE__);
	return at(0, 2);
}




template<typename R>
R Matrix<R>::f() const {
	if (!square() || cols != 3) throw ValueError("Matrix must be 3x3", __FILE__, __LINE__);
	return at(1, 2);
}




template<typename R>
R Matrix<R>::g() const {
	if (!square() || cols != 3) throw ValueError("Matrix must be 3x3", __FILE__, __LINE__);
	return at(2, 0);
}




template<typename R>
R Matrix<R>::h() const {
	if (!square() || cols != 3) throw ValueError("Matrix must be 3x3", __FILE__, __LINE__);
	return at(2, 1);
}




template<typename R>
R Matrix<R>::i() const {
	if (!square() || cols != 3) throw ValueError("Matrix must be 3x3", __FILE__, __LINE__);
	return at(2, 2);
}

template<typename T>
vector<T> range(vector<T> v, int a, int b, int step = 1) {
	return smartRange(v, a, b, step);
}

template<typename T>
vector<T> rangeFrom(vector<T> v, int a) {
	return smartRange(v, a, v.size());
}

template<typename T>
vector<T> rangeTo(vector<T> v, int a) {
	return smartRange(v, 0, a);
}

template<typename T>
vector<T> rangeStep(vector<T> v, int s) {
	return smartRange(v, 0, v.size(), s);
}

float randomUniform(float a, float b);
vec2 randomUniform(vec2 a, vec2 b);
vec3 randomUniform(vec3 a, vec3 b);

inline float angle(vec3 v1, vec3 v2) {
	return atan2(length(cross(v1, v2)), dot(v1, v2));
}

float rotationAngle(const mat3 &M);
float rotationCosAngle(mat3 M);

vec3 rotationAxis(mat3 M);
mat3 spinTensor(vec3 omega);
mat3 rotationMatrix(vec3 axis, float angle);
mat3 rotationMatrix(vec3 omega);

float polarAngle(vec3 v, vec3 t1, vec3 t2);
float polarAngle(vec3 v, vec3 n);

float cot(float x);
vec3 stereoProjection(vec4 v);
vec3 stereoProjection(Quaternion v);
vec2 stereoProjection(vec3 v);
float stereoProjection(vec2 v);

vec4 stereoProjectionInverse(vec3 v);
vec3 stereoProjectionInverse(vec2 v);
vec2 stereoProjectionInverse(float t);


inline float square(float x) { return x * x; }
inline float cube(float x) { return x * x * x; }
inline float pow3(float x) { return x * x * x; }
inline float pow2(float x) { return x * x; }
inline float pow4(float x) { return pow2(pow2(x)); }
inline float pow5(float x) { return pow2(x) * pow3(x); }
inline float pow6(float x) { return pow2(pow3(x)); }
inline float pow7(float x) { return pow2(x) * pow5(x); }
inline float pow8(float x) { return pow2(pow2(pow2(x))); }
inline float sq(float x) { return x * x; }
inline float saturate(float x) { return max(0.f, min(1.f, x)); }



// #pragma clang diagnostic pop
