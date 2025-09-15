#pragma once

#include "planarGeometry.hpp"



// const Mob CayleyTransform = Mob(1, -1.0i, 1, 1.0i);
// const Mob CayleyTransformInv = ~Mob(1, -1.0i, 1, -1.0i);
// const Mob Imob = Mob(1, 0, 0, 1);

class ComplexCurve // TODO: make this shit modern
{
public:
    HOM(float, Complex) _f;
    float epsilon = .01;
    HOM(float, Complex) _df;
    HOM(float, Complex) N;
    bool cyclic;
    float period;
    float t0, t1;

	explicit ComplexCurve(HOM(float, Complex) f, bool cyclic=true, float t0=0, float t1=TAU);
    explicit ComplexCurve(const SmoothParametricPlaneCurve& curve);
	Complex operator()(float t) const;
	vector<Complex> sample(float x0, float x1, int n);

	vector<Complex> sample(int n);
	ComplexCurve disjointUnion(ComplexCurve &other);

    static ComplexCurve line(Complex z0, Complex z1);
    static ComplexCurve circle(Complex z0, float r);
    static ComplexCurve arc(Complex center, Complex z0, Complex z1);
};


class Meromorphism {
public:
	endC _f; // TODO: is this shared ptr even making sense
	endC _df;

	Meromorphism(const Meromorphism &other);
	Meromorphism(Meromorphism &&other) noexcept;
	Meromorphism & operator=(const Meromorphism &other);
	Meromorphism & operator=(Meromorphism &&other) noexcept;
	Meromorphism();
	Meromorphism(endC f, endC df);
	explicit Meromorphism(endC f, float eps=.01f);

	Complex operator()(Complex z) const;
	Complex operator()(vec2 z) const;
	Complex df(Complex z) const;
	Complex df(vec2 z) const;
	Meromorphism compose(const Meromorphism &g) const;
	Meromorphism operator&(const Meromorphism &g) const;
	Meromorphism operator+(const Meromorphism& g) const;
	Meromorphism operator*(const Meromorphism& g) const;
	Meromorphism operator-() const;
	Meromorphism operator-(const Meromorphism& g) const;
};


class Biholomorphism : public Meromorphism {
public:
	Meromorphism f_inv;
	Biholomorphism(endC f, endC df, endC f_inv) : Meromorphism(std::move(f), std::move(df)), f_inv(std::move(f_inv)) {}
	Biholomorphism(endC f, endC f_inv, float eps=.01) : Meromorphism(std::move(f), eps), f_inv(std::move(f_inv), eps) {}
	Biholomorphism(Meromorphism f, Meromorphism f_inv) : Meromorphism(f), f_inv(f_inv) {}

	Complex inv(Complex z) const;
	Biholomorphism operator~() const;
	Biholomorphism operator*(Complex a) const;
	Biholomorphism operator+(Complex a) const;
	Biholomorphism operator-(Complex a) const;
	Biholomorphism operator/(Complex a) const;
	Biholomorphism inv() const;
	Complex operator()(Complex z) const;
	Biholomorphism compose(Biholomorphism g) const;

	static Biholomorphism mobius(Matrix<Complex> m);
	static Biholomorphism linear(Complex a, Complex b);
	static Biholomorphism _LOG();
	static Biholomorphism _EXP();
	static Biholomorphism power(float a);
};







inline Biholomorphism Biholomorphism::operator*(Complex a) const {
	return linear(a, 0i).compose(*this);
}

inline Biholomorphism Biholomorphism::operator+(Complex a) const {
	return linear(1i, a).compose(*this);
}

inline Biholomorphism Biholomorphism::operator-(Complex a) const {
	return linear(1i, -a).compose(*this);
}

inline Biholomorphism Biholomorphism::operator/(Complex a) const {
	return linear(1/a, 0i).compose(*this);
}


inline Biholomorphism Biholomorphism::mobius(Matrix<Complex> m) {
	Complex a = m.at(0, 0);
	Complex b = m.at(0, 1);
	Complex c = m.at(1, 0);
	Complex d = m.at(1, 1);
	return Biholomorphism([a, b, c, d](Complex z) {return (a * z + b) / (c * z + d); },
		[a, b, c, d](Complex z) {return (a * d - b * c) / ((c * z + d) * (c * z + d)); },
		[a, b, c, d](Complex z) {return (d * z - b) / (-c * z + a); });
}
