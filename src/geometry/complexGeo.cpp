#include "complexGeo.hpp"
#include <map>

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;




ComplexCurve ComplexCurve::line(Complex z0, Complex z1)
{
	return ComplexCurve([&z0, &z1](float t) {
		return z0 + (z1 - z0) * t;
		}, 0, 1);
}

ComplexCurve ComplexCurve::circle(Complex z0, float r)
{
	return ComplexCurve([z0, r](float t) {
		return z0 + exp(1.0i * t) * r;
	});
}

ComplexCurve ComplexCurve::arc(Complex center, Complex z0, Complex z1)
{
	float r = abs(z0 - center);
	float phi0 = (z0 - center).arg();
	float phi1 = (z1 - center).arg();
	return ComplexCurve([&center, &r, &phi0, &phi1](float t) {
		return center + exp(1.0i * t) * r;
		}, phi0, phi1);
}

Meromorphism::Meromorphism(const Meromorphism &other): _f(other._f),
													   _df(other._df) {}

Meromorphism::Meromorphism(Meromorphism &&other) noexcept: _f(std::move(other._f)),
														   _df(std::move(other._df)) {}

Meromorphism & Meromorphism::operator=(const Meromorphism &other) {
	if (this == &other)
		return *this;
	_f = other._f;
	_df = other._df;
	return *this;
}

Meromorphism & Meromorphism::operator=(Meromorphism &&other) noexcept {
	if (this == &other)
		return *this;
	_f = std::move(other._f);
	_df = std::move(other._df);
	return *this;
}

Meromorphism::Meromorphism(): _f([](Complex){return Complex(0.f);}), _df([](Complex){return Complex(0.f);}) {}

Meromorphism::Meromorphism(std::function<Complex(Complex)> f, std::function<Complex(Complex)> df): _f(std::move(f)), _df(std::move(df)) {}

Meromorphism::Meromorphism(std::function<Complex(Complex)> f, float eps): _f(std::move(f)), _df([F=f, e=eps](Complex z){return (F(z + Complex(e)) - F(z))/e;}) {}

Complex Meromorphism::operator()(Complex z) const {return _f(z);}

Complex Meromorphism::operator()(vec2 z) const { return _f(Complex(z)); }

Complex Meromorphism::df(Complex z) const { return _df(z); }

Complex Meromorphism::df(vec2 z) const { return _df(Complex(z)); }

Meromorphism Meromorphism::compose(const Meromorphism &g) const {return Meromorphism([f=_f, g=g._f](Complex z){return f(g(z));}, [f=_f, df=_df, g=g._f, dg=g._df](Complex z){return df(g(z)) * dg(z);});}

Meromorphism Meromorphism::operator&(const Meromorphism &g) const {return (this)->compose(g);}

Meromorphism Meromorphism::operator+(const Meromorphism &g) const {return Meromorphism([f=_f, g=g._f](Complex z){return f(z) + g(z);}, [df=_df, dg=g._df](Complex z){return df(z) + dg(z);});}

Meromorphism Meromorphism::operator*(const Meromorphism &g) const {return Meromorphism([f=_f, g=g._f](Complex z){return f(z) * g(z);}, [f=_f, df=_df, g=g._f, dg=g._df](Complex z){return f(z) * dg(z) + df(z) * g(z);});}

Meromorphism Meromorphism::operator-() const {return Meromorphism([f=_f](Complex z){return -f(z);}, [df=_df](Complex z){return -df(z);});}

Meromorphism Meromorphism::operator-(const Meromorphism &g) const {return *this + -g;}

Complex Biholomorphism::inv(Complex z) const { return f_inv(z); }

Biholomorphism Biholomorphism::operator~() const {return Biholomorphism(f_inv, f_inv._df, _f);}

Biholomorphism Biholomorphism::inv() const { return ~(*this); }

Complex Biholomorphism::operator()(Complex z) const { return _f(z); }

Biholomorphism Biholomorphism::compose(Biholomorphism g) const {
	return Biholomorphism([f=_f, g=g._f](Complex z) {
							  return f(g(z));
						  }, [f=_f, df=_df, g=g._f, dg=g._df](Complex z) {
							  return df(g(z)) * dg(z);
						  }, [f=f_inv, g=g.f_inv](Complex z) {
							  return f(g(z));
						  });
}

Biholomorphism Biholomorphism::linear(Complex a, Complex b) {
	return Biholomorphism([a, b](Complex z) {return a * z + b; }, [a, b](Complex z) {return a; }, [a, b](Complex z) {return (z - b) / a; });
}

Biholomorphism Biholomorphism::_LOG() {
	return Biholomorphism([](Complex z) {return log(z); }, [](Complex z) {return 1 / z; }, [](Complex z) {return exp(z); });
}

Biholomorphism Biholomorphism::_EXP() {
	return Biholomorphism([](Complex z) {return exp(z); }, [](Complex z) {return exp(z); }, [](Complex z) {return log(z); });
}

Biholomorphism Biholomorphism::power(float a) {
	return Biholomorphism([a](Complex z) {return pow(z, a); }, [a](Complex z) {return a * pow(z, a - 1); }, [a](Complex z) {return pow(z, 1 / a); });
}

ComplexCurve::ComplexCurve(std::function<Complex(float)> f, bool cyclic, float t0, float t1): _f(move(f)), cyclic(cyclic), t0(t0), t1(t1) {
	_df = [F=_f, eps=epsilon](float t){ return Complex(F(t+eps) - F(t))/eps;};
	N = [d=_df](float t){ return Complex(orthogonalComplement(d(t).z));};
	period = 0;
	if (cyclic) period = t1-t0;
}

ComplexCurve::ComplexCurve(const SmoothParametricPlaneCurve &curve)
: cyclic(false),
  t0(0),
  t1(0) {
	_f = [F=curve](float t) { return Complex(F(t)); };
	_df = [C=curve](float t) { return Complex(C.df(t)); };
	N = [d=_df](float t) { return Complex(orthogonalComplement(d(t).z)); };
	period = 0;
}

Complex ComplexCurve::operator()(float t) const {return _f(t);}

vector<Complex> ComplexCurve::sample(float x0, float x1, int n) {
	vector<Complex> res ={};
	for (int i=0; i<n; i++)
		res.push_back((*this)(lerp(x0, x1, 1.f*i/n)));
	return res;
}

vector<Complex> ComplexCurve::sample(int n) {
	return sample(t0, t1, n);
}
