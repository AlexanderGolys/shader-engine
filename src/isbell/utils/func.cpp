#include "func.hpp"
#include "randomUtils.hpp"
#include "monads.hpp"


#include <chrono>
#include <functional>
#include <iosfwd>
#include <iostream>
#include <memory>
#include <random>
#include <utility>
#include <vector>
// #include <glm/gtx/transform.hpp>

#include "file-management/filesUtils.hpp"


using namespace glm;
using std::vector, std::shared_ptr, std::make_shared, std::max, std::min;


Regularity operator+(Regularity a, int b);


std::string polyGroupIDtoString(std::variant<int, std::string> id) {
	return std::holds_alternative<int>(id) ? std::to_string(std::get<int>(id)) : std::get<std::string>(id);
}

std::variant<int, std::string> prefix(const std::variant<int, std::string> &id, const std::string &prefix) {
	return PolyGroupID(prefix + polyGroupIDtoString(id));
}

std::variant<int, std::string> make_unique_id(const std::variant<int, std::string> &id) {
	return prefix(id, randomStringNumeric());
}

std::variant<int, std::string> bdGroup(int n) {
	return PolyGroupID("bd" + std::to_string(n));
}

std::variant<int, std::string> curveGroup(int n) {
	return PolyGroupID("curva" + std::to_string(n));
}

std::variant<int, std::string> randomCurvaID() {
	return PolyGroupID("curva" + randomStringNumeric());
}

std::variant<int, std::string> randomID() {
	return PolyGroupID(randomStringNumeric());
}

std::variant<int, std::string> randomBdID() {
	return PolyGroupID("bd" + randomStringNumeric() + randomStringNumeric() + randomStringNumeric());
}

bool idBelongsToCurve(const std::variant<int, std::string> &id) {
	return std::holds_alternative<std::string>(id) && std::get<std::string>(id).starts_with("curva");
}

bool idBelongsToBd(const std::variant<int, std::string> &id) {
	return std::holds_alternative<std::string>(id) && std::get<std::string>(id).starts_with("bd");
}

Regularity max(Regularity a, Regularity b) {
	return static_cast<Regularity>(std::max(INT(a), INT(b)));
}

Regularity min(Regularity a, Regularity b) {
	return static_cast<Regularity>(std::min(INT(a), INT(b)));
}

Regularity operator--(Regularity a) {
	return a - 1;
}

Regularity operator++(Regularity a) {
	return a + 1;
}

template<typename codomain>
std::function<codomain(float)> derivativeOperator(std::function<codomain(float)> f, float epsilon) {
	return [f, epsilon](float x) {
		return (f(x + epsilon) - f(x - epsilon)) / (2 * epsilon);
	};
}

template<typename domain, typename codomain>
std::function<codomain(domain)> directionalDerivativeOperator(std::function<codomain(domain)> f, domain v, float epsilon) {
	return [f, epsilon, v](domain x) {
		return (f(x + v * epsilon) - f(x - v * epsilon)) / (2 * epsilon);
	};
}



RealFunctionR3::RealFunctionR3()
: _f([](vec3 v) {
	  return 0;
  }),
  _df([](vec3 v) {
	  return vec3(0);
  }),
  regularity(Regularity::ANALYTIC) {}

RealFunctionR3::RealFunctionR3(const RealFunctionR3 &other) = default;

RealFunctionR3::RealFunctionR3(std::function<float(vec3)> f, std::function<vec3(vec3)> df, float eps, Regularity regularity)
: _f(f),
  _df(df),
  eps(eps),
  regularity(regularity) {}

RealFunctionR3::RealFunctionR3(std::function<float(vec3)> f, float epsilon, Regularity regularity)
: _f(f),
  _df(derivativeOperator(f, epsilon)),
  eps(epsilon),
  regularity(regularity) {}

RealFunctionR3::RealFunctionR3(std::function<float(float, float, float)> f, std::function<vec3(float, float, float)> df, float eps, Regularity regularity)
: _f(wrap(f)),
  _df(wrap(df)),
  eps(eps),
  regularity(regularity) {}

RealFunctionR3::RealFunctionR3(std::function<float(float, float, float)> f, float epsilon, Regularity regularity)
: _f(wrap(f)),
  _df(derivativeOperator(wrap(f), epsilon)),
  eps(epsilon),
  regularity(regularity) {}

float RealFunctionR3::operator()(float x, float y, float z) const {
	return _f(vec3(x, y, z));
}

vec3 RealFunctionR3::df(float x, float y, float z) const {
	return _df(vec3(x, y, z));
}

RealFunctionR3 RealFunctionR3::operator-() const {
	return *this * -1;
}

RealFunctionR3 RealFunctionR3::operator-(const RealFunctionR3 &g) const {
	return *this + (-g);
}

RealFunctionR3 operator*(float a, const RealFunctionR3 &f) {
	return f * a;
}

RealFunctionR3 operator+(float a, const RealFunctionR3 &f) {
	return f + a;
}

RealFunctionR3 operator-(float a, const RealFunctionR3 &f) {
	return -f + a;
}

RealFunctionR3 operator/(float a, const RealFunctionR3 &f) {
	return RealFunctionR3::constant(a) / f;
}

RealFunctionR3 min(const RealFunctionR3 &f, float a) { return RealFunctionR3([f, a](vec3 x) { return std::min(f(x), a); }, f.getEps(), f.regularity); }

RealFunctionR3 min(const RealFunctionR3 &f, const RealFunctionR3 &g) { return RealFunctionR3([f, g](vec3 x) { return std::min(f(x),g(x)); }, f.getEps(), f.regularity); }

RealFunctionR3 min(float a, const RealFunctionR3 &f) { return min(f, a); }

RealFunctionR3 max(const RealFunctionR3 &f, float a) { return RealFunctionR3([f, a](vec3 x) { return std::max(f(x), a); }, f.getEps(), f.regularity); }

RealFunctionR3 max(const RealFunctionR3 &f, const RealFunctionR3 &g) { return RealFunctionR3([f, g](vec3 x) { return std::max(f(x),g(x)); }, f.getEps(), f.regularity); }

RealFunctionR3 max(float a, const RealFunctionR3 &f) {
	return max(f, a);
}

RealFunctionR3 precompose(std::function<float(float, float)> F, const RealFunctionR3 &g1, const RealFunctionR3 &g2) {
	return RealFunctionR3([F, g1, g2](vec3 v) {
		return F(g1(v), g2(v));
	}, g1.getEps(), min(g1.regularity, g2.regularity));
}

RealFunctionR3 RealFunctionR3::operator~() const {
	return 1 / (*this);
}

RealFunctionR3 RealFunctionR3::pow(float a) const {
	return RealFunctionR3([this, a](vec3 v) {
							  return std::pow(this->operator()(v), a);
						  }, [this, a](vec3 v) {
							  return this->df(v) * a * std::pow(this->operator()(v), a - 1);
						  });
}

RealFunctionR3 RealFunctionR3::sqrt() const {
	return pow(0.5f);
}

float RealFunctionR3::getEps() const {
	return eps;
}

float RealFunctionR3::dx(vec3 x) const {
	return _df(x).x;
}

float RealFunctionR3::dy(vec3 x) const {
	return _df(x).y;
}

float RealFunctionR3::dz(vec3 x) const {
	return _df(x).z;
}

SpaceEndomorphism::~SpaceEndomorphism() = default;

SpaceEndomorphism::SpaceEndomorphism(const SpaceEndomorphism &other)
: _f(other._f),
  _df(other._df) {}

SpaceEndomorphism::SpaceEndomorphism(SpaceEndomorphism &&other) noexcept
: _f(std::move(other._f)),
  _df(std::move(other._df)) {}

SpaceEndomorphism::SpaceEndomorphism(std::function<vec3(vec3)> f, std::function<mat3(vec3)> df, float eps)
: _f(std::move(f)),
  _df(std::move(df)),
  eps(eps) {}

SpaceEndomorphism::SpaceEndomorphism(mat3 A)
: _f([A](vec3 x) {
	  return A * x;
  }),
  _df([A](vec3 x) {
	  return A;
  }) {}

SpaceEndomorphism::SpaceEndomorphism(mat4 A)
: _f([A](vec3 x) {
	  return vec3(A * vec4(x, 1));
  }),
  _df([A](vec3 x) {
	  return mat3(A);
  }) {}

vec3 SpaceEndomorphism::directional_derivative(vec3 x, vec3 v) const {
	return _df(x) * v;
}

vec3 SpaceEndomorphism::dfdv(vec3 x, vec3 v) const {
	return directional_derivative(x, v);
}

mat3 SpaceEndomorphism::df(vec3 x) const {
	return _df(x);
}

vec3 SpaceEndomorphism::operator()(vec3 x) const {
	return _f(x);
}

SpaceEndomorphism SpaceEndomorphism::operator&(const SpaceEndomorphism &g) const {
	return compose(g);
}

float SpaceEndomorphism::getEps() const {
	return eps;
}

SpaceEndomorphism SpaceEndomorphism::linear(const mat3 &A) {
	return SpaceEndomorphism(A);
}

SpaceAutomorphism SpaceAutomorphism::inv() const {
	return ~(*this);
}

SpaceAutomorphism SpaceAutomorphism::operator&(const SpaceAutomorphism &g) const {
	return compose(g);
}

SpaceAutomorphism SpaceAutomorphism::applyWithBasis(mat3 A) const {
	return linear(A) & *this & linear(inverse(A));
}

SpaceAutomorphism SpaceAutomorphism::applyWithBasis(vec3 v1, vec3 v2, vec3 v3) const {
	return applyWithBasis(mat3(v1, v2, v3));
}

SpaceAutomorphism SpaceAutomorphism::applyWithShift(vec3 v) const {
	return translation(v) & *this & translation(-v);
}

SpaceAutomorphism SpaceAutomorphism::scaling(vec3 factors) {
	return scaling(factors.x, factors.y, factors.z);
}

SpaceAutomorphism SpaceAutomorphism::scaling(float x) {
	return scaling(x, x, x);
}

SpaceAutomorphism SpaceAutomorphism::scaling(float x, float y, float z, vec3 center) {
	return translation(center) & scaling(x, y, z) & translation(-center);
}

SpaceAutomorphism SpaceAutomorphism::scaling(float x, vec3 center) {
	return scaling(x, x, x, center);
}

SpaceAutomorphism SpaceAutomorphism::scaling(vec3 factors, vec3 center) {
	return scaling(factors.x, factors.y, factors.z, center);
}

SpaceAutomorphism SpaceAutomorphism::rotation(vec3 axis, float angle, vec3 center) {
	return rotation(axis, angle).applyWithShift(center);
}

SpaceAutomorphism SpaceAutomorphism::deltaRotation(vec3 v1, vec3 v2) {
	return linear(rotationBetween(v1, v2));
}

SpaceAutomorphism SpaceAutomorphism::deltaRotation(vec3 v1, vec3 v2, vec3 center) {
	return translation(center) & deltaRotation(v1, v2) & translation(-center);
}

RealFunction::RealFunction(std::function<float(float)> f, std::function<float(float)> df, std::function<float(float)> ddf, float epsilon, Regularity regularity)
: _f(std::move(f)),
  _df(std::move(df)),
  _ddf(std::move(ddf)),
  eps(epsilon),
  regularity(regularity),
  is_zero(false){}

RealFunction::RealFunction(std::function<float(float)> f, std::function<float(float)> df, float epsilon, Regularity regularity)
: RealFunction(f, df, derivativeOperator(df, epsilon), epsilon, regularity) {}

RealFunction::RealFunction(std::function<float(float)> f, float epsilon, Regularity regularity)
: RealFunction(f, derivativeOperator(f, epsilon), epsilon, regularity) {}

RealFunction::RealFunction(float constant, float epsilon, Regularity regularity): RealFunction([constant](float x) {
	return constant;
}, epsilon, regularity) {
	if (abs(constant) < epsilon) {
		is_zero = true;
	}
}

float RealFunction::operator()(float x) const {
	return _f(x);
}

float RealFunction::df(float x) const {
	return _df(x);
}

float RealFunction::ddf(float x) const {
	return _ddf(x);
}

RealFunction RealFunction::df() const {
	return RealFunction(_df, _ddf, eps, regularity - 1);
}

bool RealFunction::isZero(float a, float b, int prec) {
	if (is_zero) return true;
	for (float x: linspace(a, b, prec)) {
		if (!nearlyEqual(_f(x), 0)) {
			this->is_zero = false;
			return false;
		}
	}
	this->is_zero = true;
	return true;
}

float RealFunction::sup(float a, float b, int prec) const {
	float m = this->operator()(a);
	for (int i = 0; i < prec; i++)
		m = std::max(m, this->operator()(a + i * (b - a) / prec));
	return m;
}

float RealFunction::inf(float a, float b, int prec) const {
	float m = this->operator()(a);
	for (int i = 0; i < prec; i++)
		m = std::min(m, this->operator()(a + i * (b - a) / prec));
	return m;
}

float RealFunction::argmax(float a, float b, int prec) const {
	float m = this->operator()(a);
	float x = a;
	for (int i = 0; i < prec; i++) {
		float p = this->operator()(a + i * (b - a) / prec);
		if (p > m) {
			m = p;
			x = a + i * (b - a) / prec;
		}
	}
	return x;
}

float RealFunction::argmin(float a, float b, int prec) const {
	float m = this->operator()(a);
	float x = a;
	for (int i = 0; i < prec; i++) {
		float p = this->operator()(a + i * (b - a) / prec);
		if (p < m) {
			m = p;
			x = a + i * (b - a) / prec;
		}
	}
	return x;
}

RealFunction RealFunction::operator-(float a) const {
	return *this + (-a);
}

RealFunction RealFunction::operator-() const {
	return *this * -1;
}

RealFunction RealFunction::operator-(const RealFunction &g) const {
	return *this + (-g);
}

RealFunction RealFunction::operator/(float a) const {
	return *this * (1 / a);
}

RealFunction RealFunction::pow(float a) const {
	return RealFunction([this, a](float x) { return std::pow(this->operator()(x), a); }, eps, regularity);
}

RealFunction pow(const RealFunction &f, float a) {
	return f.pow(a);
}

RealFunction max(const RealFunction &f, const RealFunction &g) {
	return RealFunction([f, g](float x) { return std::max(f(x), g(x)); }, f.eps, min(f.regularity, g.regularity));
}

RealFunction max(const RealFunction &f, float a) {
	return RealFunction([f, a](float x) { return std::max(f(x), a); }, f.eps, f.regularity);
}

RealFunction min(const RealFunction &f, const RealFunction &g) {
	return RealFunction([f, g](float x) { return std::min(f(x), g(x)); }, f.eps, min(f.regularity, g.regularity));
}

RealFunction min(const RealFunction &f, float a) {
	return RealFunction([f, a](float x) { return std::min(f(x), a); }, f.eps, f.regularity);
}

RealFunction abs(const RealFunction &f) {
	return max(f, -f);
}

RealFunction operator*(float a, const RealFunction &f) {
	return f * a;
}

RealFunction operator+(float a, const RealFunction &f) {
	return f + a;
}

RealFunction operator-(float a, const RealFunction &f) {
	return -f + a;
}

RealFunction operator/(float a, const RealFunction &f) {
	return RealFunction::constant(a) / f;
}

float RealFunction::integral(float a, float b, int prec) const {
	float sum = 0;
	float dx = (b - a) / prec;
	float x = a;
	for (int i = 0; i < prec; i++) {
		float v0 = operator()(x);
		float v1 = operator()(x + dx / 4);
		float v2 = operator()(x + dx / 2);
		float v3 = operator()(x + 3 * dx / 4);
		float v4 = operator()(x + dx);
		sum += 2 * dx / 45.f * (7 * v0 + 32 * v1 + 12 * v2 + 32 * v3 + 7 * v4);
		x += dx;
	}
	return sum;
}

RealFunction RealFunction::antiderivative(float a, int prec) const {
	return RealFunction([this, a, prec](float x) {
							return this->integral(a, x, prec);
						},
						[f=this->_f](float x) {
							return f(x);
						},
						eps,
						regularity + 1);
}

RealFunctionR2 separated_product(const RealFunction &f_x, const RealFunction &f_t) {
	return RealFunctionR2([f_x=f_x, f_t=f_t](vec2 v) { return f_x(v.x) * f_t(v.y); }, f_x.eps, min(f_x.regularity, f_t.regularity));
}

RealFunctionR2 separated_sum(const RealFunction &f_x, const RealFunction &f_t) {
	return RealFunctionR2([f_x=f_x, f_t=f_t](vec2 v) { return f_x(v.x) + f_t(v.y); }, f_x.eps, min(f_x.regularity, f_t.regularity));
}

RealFunction RealFunction::constant(float a) {
	return RealFunction(a);
}

RealFunction RealFunction::x() {
	return RealFunction(
			[](float x) {
				return x;
			},
			[](float) {
				return 1.0f;
			},
			[](float) {
				return 0.0f;
			},
			.01,
			Regularity::ANALYTIC);
}

RealFunction RealFunction::one() {
	return RealFunction::constant(1);
}

RealFunction RealFunction::zero() {
	return RealFunction::constant(0);
}

RealFunction RealFunction::linear(float a, float b) {
	return x() * a + b;
}

RealFunction RealFunction::quadratic(float a, float b, float c) {
	return x() * x() * a + x() * b + c;
}

RealFunction RealFunction::sin() {
	return RealFunction(
			[](float x) {
				return std::sin(x);
			},
			[](float x) {
				return std::cos(x);
			},
			[](float x) {
				return -std::sin(x);
			},
			.01,
			Regularity::ANALYTIC);
}

RealFunction RealFunction::cos() {
	return RealFunction(
			[](float x) {
				return std::cos(x);
			},
			[](float x) {
				return -std::sin(x);
			},
			[](float x) {
				return -std::cos(x);
			},
			.01,
			Regularity::ANALYTIC);
}

RealFunction RealFunction::exp() {
	return RealFunction(
			[](float x) {
				return std::exp(x);
			},
			[](float x) {
				return std::exp(x);
			},
			[](float x) {
				return std::exp(x);
			},
			.001,
			Regularity::ANALYTIC);
}

RealFunction RealFunction::log() {
	return RealFunction(
			[](float x) {
				return std::log(x);
			},
			[](float x) {
				return 1 / x;
			},
			[](float x) {
				return -1 / sq(x);
			},
			.001,
			Regularity::ANALYTIC);
}

RealFunction RealFunction::SQRT() {
	return RealFunction(
			[](float x) {
				return std::sqrt(x);
			},
			[](float x) {
				return 1 / (2 * std::sqrt(x));
			},
			[](float x) {
				return -1 / (4 * x * std::sqrt(x));
			},
			.001,
			Regularity::ANALYTIC);
}

vec2 CompactlySupportedRealFunction::support_sum(vec2 other) const {
	return vec2(std::min(support[0], other[0]), std::max(support[1], other[1]));
}

vec2 CompactlySupportedRealFunction::support_intersect(vec2 other) const {
	vec2 s = vec2(std::max(support[0], other[0]), std::min(support[1], other[1]));
	if (s[0] > s[1])
		return vec2(0, 0);
	return s;
}




CompactlySupportedRealFunction::CompactlySupportedRealFunction(std::function<float(float)> f, std::function<float(float)> df, std::function<float(float)> ddf, vec2 support, float epsilon, Regularity regularity)
: RealFunction(f, df, ddf, epsilon, regularity),
  support(support) {}

CompactlySupportedRealFunction::CompactlySupportedRealFunction(std::function<float(float)> f, std::function<float(float)> df, vec2 support, float epsilon, Regularity regularity)
: RealFunction(f, df, epsilon, regularity),
  support(support) {}

CompactlySupportedRealFunction::CompactlySupportedRealFunction(std::function<float(float)> f, vec2 support, float epsilon, Regularity regularity)
: RealFunction(f, epsilon, regularity),
  support(support) {}

CompactlySupportedRealFunction::CompactlySupportedRealFunction(const RealFunction &f, vec2 support)
: RealFunction(f),
  support(support) {}

CompactlySupportedRealFunction::CompactlySupportedRealFunction(const RealFunction &f, vec2 support, float epsilon)
: RealFunction(f, epsilon),
  support(support) {}

CompactlySupportedRealFunction::CompactlySupportedRealFunction(const RealFunction &f, const RealFunction &df, vec2 support, float epsilon)
: RealFunction(f, df, epsilon),
  support(support) {}

CompactlySupportedRealFunction::CompactlySupportedRealFunction(const RealFunction &f, const RealFunction &df, const RealFunction &ddf, vec2 support, float epsilon)
: RealFunction(f, df, ddf, epsilon),
  support(support) {}

CompactlySupportedRealFunction CompactlySupportedRealFunction::df() const {
	return CompactlySupportedRealFunction(RealFunction::df(), support);
}

float CompactlySupportedRealFunction::improper_integral(optional<float> a, optional<float> b, int prec) const {
	float a_ = support[0];
	float b_ = support[1];
	if (a.has_value() and a.value() > a_)
		a_ = a.value();
	if (b.has_value() and b.value() < b_)
		b_ = b.value();
	return integral(a_, b_, prec);
}

float CompactlySupportedRealFunction::full_domain_integral(int prec) const {
	return improper_integral(UNDEFINED, UNDEFINED, prec);
}

CompactlySupportedRealFunction CompactlySupportedRealFunction::antiderivative(float a, int prec) const {
	return CompactlySupportedRealFunction(RealFunction::antiderivative(a, prec), support);
}

CompactlySupportedRealFunction CompactlySupportedRealFunction::operator+(const CompactlySupportedRealFunction &g) const {
	return CompactlySupportedRealFunction(RealFunction::operator+(g), support_sum(g.support));
}

CompactlySupportedRealFunction CompactlySupportedRealFunction::operator*(float a) const {
	return CompactlySupportedRealFunction(RealFunction::operator*(a), support);
}

CompactlySupportedRealFunction CompactlySupportedRealFunction::operator/(float a) const {
	return CompactlySupportedRealFunction(RealFunction::operator/(a), support);
}

CompactlySupportedRealFunction CompactlySupportedRealFunction::operator*(const CompactlySupportedRealFunction &g_) const {
	return CompactlySupportedRealFunction(RealFunction::operator*(g_), support_intersect(support));
}

CompactlySupportedRealFunction CompactlySupportedRealFunction::operator*(const RealFunction &g_) const {
	return CompactlySupportedRealFunction(RealFunction::operator*(g_), support);
}

CompactlySupportedRealFunction CompactlySupportedRealFunction::operator-() const {
	return CompactlySupportedRealFunction(RealFunction::operator-(), support);
}

CompactlySupportedRealFunction CompactlySupportedRealFunction::operator-(const CompactlySupportedRealFunction &g) const {
	return CompactlySupportedRealFunction(RealFunction::operator-(g), support_sum(g.support));
}

CompactlySupportedRealFunction operator*(float a, const CompactlySupportedRealFunction &f) {
	return f * a;
}




CompactlySupportedRealFunction operator*(const RealFunction &g, const CompactlySupportedRealFunction &f) {
	return f * g;
}

RealLineAutomorphism::RealLineAutomorphism(RealFunction f, RealFunction inv)
: RealFunction(f),
  inverse(inv) {}

RealLineAutomorphism::RealLineAutomorphism(std::function<float(float)> f, std::function<float(float)> f_inv, float epsilon)
: RealFunction(f, epsilon),
  inverse(f_inv, epsilon) {}

RealLineAutomorphism::RealLineAutomorphism(std::function<float(float)> f, std::function<float(float)> df, std::function<float(float)> f_inv, float epsilon)
: RealFunction(f, df, epsilon),
  inverse(f_inv, epsilon) {}

RealLineAutomorphism::RealLineAutomorphism(std::function<float(float)> f, std::function<float(float)> df, std::function<float(float)> f_inv, std::function<float(float)> df_inv, float epsilon)
: RealFunction(f, df, epsilon),
  inverse(f_inv, df_inv, epsilon) {}

RealLineAutomorphism::RealLineAutomorphism(std::function<float(float)> f, std::function<float(float)> df, std::function<float(float)> ddf, std::function<float(float)> f_inv, std::function<float(float)> df_inv, std::function<float(float)> ddf_inv, float epsilon)
: RealFunction(f, df, ddf, epsilon),
  inverse(f_inv, df_inv, ddf_inv, epsilon) {}

float RealLineAutomorphism::inv(float x) const {
	return inverse(x);
}

RealLineAutomorphism RealLineAutomorphism::inv() const {
	return RealLineAutomorphism(inverse, *this);
}

RealLineAutomorphism RealLineAutomorphism::operator~() const {
	return inv();
}

RealLineAutomorphism RealLineAutomorphism::operator*(float c) const {
	return RealLineAutomorphism(static_cast<RealFunction>(*this) * c, inverse / c);
}

RealLineAutomorphism RealLineAutomorphism::operator+(float c) const {
	return RealLineAutomorphism(static_cast<RealFunction>(*this) + c, inverse - c);
}

RealLineAutomorphism RealLineAutomorphism::operator/(float c) const {
	return (*this) * (1 / c);
}

RealLineAutomorphism RealLineAutomorphism::operator-(float c) const {
	return (*this) + (-c);
}

RealLineAutomorphism RealLineAutomorphism::operator-() const {
	return RealLineAutomorphism(-static_cast<RealFunction>(*this), -inverse);
}

RealLineAutomorphism operator*(float c, const RealLineAutomorphism &f) {
	return f * c;
}

RealLineAutomorphism operator+(float c, const RealLineAutomorphism &f) {
	return f + c;
}

RealLineAutomorphism operator-(float c, const RealLineAutomorphism &f) {
	return -f + c;
}

RealLineAutomorphism RealLineAutomorphism::Id() {
	return RealLineAutomorphism(RealFunction::x(), RealFunction::x());
}

RealLineAutomorphism RealLineAutomorphism::x() {
	return Id();
}

vec2 PlaneSmoothEndomorphism::operator()(float t, float u) const {
	return (*this)(vec2(t, u));
}

vec2 PlaneSmoothEndomorphism::df(float t, float u, vec2 v) const {
	return df(vec2(t, u), v);
}

mat2 PlaneSmoothEndomorphism::df(float t, float u) const {
	return df(vec2(t, u));
}

vec2 PlaneAutomorphism::inv(vec2 x) const {
	return f_inv(x);
}

vec2 PlaneAutomorphism::inv(float t, float u) const {
	return f_inv(vec2(t, u));
}

PlaneAutomorphism PlaneAutomorphism::inv() const {
	return ~(*this);
}

HolomorphicFunction::HolomorphicFunction(std::function<Complex(Complex)> f, std::function<Complex(Complex)> df, float epsilon)
: _f(f), _df(df), eps(epsilon) {}

HolomorphicFunction::HolomorphicFunction(std::function<Complex(Complex)> f, float epsilon)
: _f(f), eps(epsilon) {
	_df = [F=_f, eps=epsilon](Complex z) {
		return (F(z + eps) - F(z - eps)) / (2.f * eps);
	};
}

HolomorphicFunction::HolomorphicFunction(const PlaneSmoothEndomorphism &f, float epsilon)
: HolomorphicFunction([f=f](Complex z) { return Complex(f(vec2(z.real(), z.imag()))); }, epsilon) {}

HolomorphicFunction::HolomorphicFunction(const RealFunctionR2 &re, const RealFunctionR2 &im, float epsilon)
: HolomorphicFunction([real=re, imag=im](Complex z) { return Complex(real(z.real(), z.imag()), imag(z.real(), z.imag())); }, epsilon) {}

Complex HolomorphicFunction::operator()(Complex z) const { return _f(z); }

HolomorphicFunction HolomorphicFunction::df() const { return HolomorphicFunction(_df, eps); }

HolomorphicFunction HolomorphicFunction::d2f() const { return df().df(); }

HolomorphicFunction HolomorphicFunction::dnf(int n) const {
	if (n == 0) return *this;
	if (n == 1) return df();
	if (n < 0) throw ValueError("Derivative order must be non-negative", __FILE__, __LINE__);
	return df().dnf(n-1);
}

Complex HolomorphicFunction::df(Complex z) const {
	return _df(z);
}

Complex HolomorphicFunction::d2f(Complex z) const {
	return df().df(z);
}

Complex HolomorphicFunction::dnf(Complex z, int n) const {
	return dnf(n)(z);
}

HolomorphicFunction HolomorphicFunction::operator-() const {
	return HolomorphicFunction([f=_f](Complex z) { return -f(z); }, [df=_df](Complex z) { return -df(z); }, eps);
}

HolomorphicFunction HolomorphicFunction::operator+(const HolomorphicFunction &g) const {
	return HolomorphicFunction([F=_f, G=g._f](Complex z) { return F(z) + G(z); }, [df=_df, dg=g._df](Complex z) { return df(z) + dg(z); }, eps);
}

HolomorphicFunction HolomorphicFunction::operator+(Complex a) const {
	return HolomorphicFunction([f=_f, a](Complex z) { return f(z) + a; }, _df, eps);
}

HolomorphicFunction HolomorphicFunction::operator+(float a) const {
	return *this + Complex(a);
}

HolomorphicFunction HolomorphicFunction::operator+(int a) const {
	return *this + Complex(a);
}

HolomorphicFunction operator+(Complex a, const HolomorphicFunction &f) { return f + a; }

HolomorphicFunction operator+(float a, const HolomorphicFunction &f) { return f + a; }

HolomorphicFunction operator+(int a, const HolomorphicFunction &f) { return f + a; }

HolomorphicFunction HolomorphicFunction::operator-(const HolomorphicFunction &g) const {
	return *this + (-g);
}

HolomorphicFunction HolomorphicFunction::operator-(Complex a) const {
	return *this + (-a);
}

HolomorphicFunction HolomorphicFunction::operator-(float a) const {
	return *this + (-a);
}

HolomorphicFunction HolomorphicFunction::operator-(int a) const {
	return *this + (-a);
}

HolomorphicFunction operator-(Complex a, const HolomorphicFunction &f) { return -f + a; }

HolomorphicFunction operator-(float a, const HolomorphicFunction &f) { return -f + a; }

HolomorphicFunction operator-(int a, const HolomorphicFunction &f) { return -f + a; }

HolomorphicFunction HolomorphicFunction::operator*(const HolomorphicFunction &g) const {return HolomorphicFunction([f=_f, g=g._f](Complex z) { return f(z) * g(z); }, [df=_df, dg=g._df, f=_f, g=g._f](Complex z) { return df(z) * g(z) + f(z) * dg(z); }, eps); }

HolomorphicFunction HolomorphicFunction::operator*(Complex a) const {return HolomorphicFunction([f=_f, a](Complex z) { return f(z) * a; }, [df=_df, a](Complex z) { return df(z) * a; }, eps); }

HolomorphicFunction HolomorphicFunction::operator*(float a) const {return *this * Complex(a); }

HolomorphicFunction HolomorphicFunction::operator*(int a) const {return *this * Complex(a); }

HolomorphicFunction operator*(Complex a, const HolomorphicFunction &f) { return f * a; }

HolomorphicFunction operator*(float a, const HolomorphicFunction &f) { return f * a; }

HolomorphicFunction operator*(int a, const HolomorphicFunction &f) { return f * a; }

HolomorphicFunction HolomorphicFunction::operator/(Complex a) const { return HolomorphicFunction([f=_f, a](Complex z) { return f(z) / a; }, [df=_df, a](Complex z) { return df(z) / a; }, eps); }

HolomorphicFunction HolomorphicFunction::operator/(float a) const { return *this / Complex(a); }

HolomorphicFunction HolomorphicFunction::operator/(int a) const { return *this / Complex(a); }

HolomorphicFunction operator/(Complex a, const HolomorphicFunction &f) { return HolomorphicFunction([a, f=f._f](Complex z) { return a / f(z); }, [a, df=f._df, f=f._f](Complex z) { return -a * df(z) / (f(z) * f(z)); }, f.eps); }

HolomorphicFunction operator/(float a, const HolomorphicFunction &f) { return Complex(a)/f; }

HolomorphicFunction operator/(int a, const HolomorphicFunction &f) { return Complex(a)/f; }

HolomorphicFunction HolomorphicFunction::operator/(const HolomorphicFunction &g) const {return *this * (1/ g);}

HolomorphicFunction HolomorphicFunction::operator&(const HolomorphicFunction &g) const {
	return HolomorphicFunction([f=_f, g=g._f](Complex z) { return f(g(z)); }, [df=_df, dg=g._df,g=g._f](Complex z) { return df(g(z)) * dg(z); }, eps);
}

HolomorphicFunction HolomorphicFunction::pow(int a) const {
	if (a < 0) return 1 / pow(-a);
	if (a == 0) return HolomorphicFunction([](Complex z) { return 0.0i+1; }, [](Complex z) { return 0i; }, eps);
	if (a == 1) return *this;
	if (a == 2) return *this * *this;
	if (a == 3) return *this * *this * *this;
	return pow(a/2) * pow(a-a/2);
}

HolomorphicFunction HolomorphicFunction::pow(float a) const {
	if (nearlyEqual(a, ::floor(a))) return pow((int)a);
	return HolomorphicFunction([f=_f, a](Complex z) { return f(z).pow(a); }, [df=_df, f=_f, a](Complex z) { return df(z) * a * f(z).pow(a-1); }, eps);
}

HolomorphicFunction HolomorphicFunction::sqrt() const { return pow(.5f); }

HolomorphicFunction HolomorphicFunction::sq() const { return pow(2); }

HolomorphicFunction HolomorphicFunction::pow2() const { return pow(2); }

HolomorphicFunction HolomorphicFunction::pow3() const { return pow(3); }

ComplexValuedFunction::ComplexValuedFunction(HOM(float, Complex) f, float eps)
: _f(std::move(f)), eps(eps) {}

ComplexValuedFunction::ComplexValuedFunction(RealFunction re, float eps)
: ComplexValuedFunction([re](float x) { return Complex(re(x), 0); }, eps) {}

ComplexValuedFunction::ComplexValuedFunction(RealFunction f_re, RealFunction f_im, float eps)
:  ComplexValuedFunction([f_re, f_im](float x) { return Complex(f_re(x), f_im(x)); }, eps) {}

ComplexValuedFunction::ComplexValuedFunction(RealFunction f_power, RealFunction f_phase, bool dumb):ComplexValuedFunction([p=f_power, ph=f_phase](float x) { return fromExpForm(::sqrt(p(x)), ph(x)); }, f_power.getEps()) {}

ComplexValuedFunction::ComplexValuedFunction(std::function<float(float)> re, float eps)
: ComplexValuedFunction(re, [re](float x) { return 0; }, eps) {}

ComplexValuedFunction::ComplexValuedFunction(std::function<float(float)> f_re, std::function<float(float)> f_im, float eps)
: ComplexValuedFunction([f_re, f_im](float x) { return Complex(f_re(x), f_im(x)); }, eps) {}

ComplexValuedFunction::ComplexValuedFunction(const ComplexValuedFunction &other): _f(other._f),
																				  eps(other.eps) {}

ComplexValuedFunction::ComplexValuedFunction(ComplexValuedFunction &&other) noexcept: _f(std::move(other._f)),
																					  eps(other.eps) {}

ComplexValuedFunction & ComplexValuedFunction::operator=(const ComplexValuedFunction &other) {
	if (this == &other)
		return *this;
	_f = other._f;
	eps = other.eps;
	return *this;
}

ComplexValuedFunction & ComplexValuedFunction::operator=(ComplexValuedFunction &&other) noexcept {
	if (this == &other)
		return *this;
	_f = std::move(other._f);
	eps = other.eps;
	return *this;
}

ComplexFunctionR2::ComplexFunctionR2(const ComplexFunctionR2 &other): _f(other._f),
																	  eps(other.eps) {}

ComplexFunctionR2::ComplexFunctionR2(ComplexFunctionR2 &&other) noexcept: _f(std::move(other._f)),
																		  eps(other.eps) {}

ComplexFunctionR2 & ComplexFunctionR2::operator=(const ComplexFunctionR2 &other) {
	if (this == &other)
		return *this;
	_f = other._f;
	eps = other.eps;
	return *this;
}

ComplexFunctionR2 & ComplexFunctionR2::operator=(ComplexFunctionR2 &&other) noexcept {
	if (this == &other)
		return *this;
	_f = std::move(other._f);
	eps = other.eps;
	return *this;
}

DiscreteComplexFunction::DiscreteComplexFunction(const HOM(float, Complex) &fn, vec2 domain, int sampling)
: fn(map<float, Complex>(linspace(domain[0], domain[1], sampling), fn)),
  DiscreteSingleVariableFunction(domain.x, domain.y, sampling) {}



DiscreteComplexFunction::DiscreteComplexFunction(const DiscreteComplexFunction &other): DiscreteSingleVariableFunction<float, Complex>(other),
																						fn(other.fn) {}

DiscreteComplexFunction::DiscreteComplexFunction(DiscreteComplexFunction &&other) noexcept: DiscreteSingleVariableFunction<float, Complex>(std::move(other)),
																							fn(std::move(other.fn)) {}

DiscreteComplexFunction & DiscreteComplexFunction::operator=(const DiscreteComplexFunction &other) {
	if (this == &other)
		return *this;
	DiscreteSingleVariableFunction<float, Complex>::operator =(other);
	fn = other.fn;
	return *this;
}

DiscreteComplexFunction & DiscreteComplexFunction::operator=(DiscreteComplexFunction &&other) noexcept {
	if (this == &other)
		return *this;
	DiscreteSingleVariableFunction<float, Complex>::operator =(std::move(other));
	fn = std::move(other.fn);
	return *this;
}




DiscreteComplexFunction::DiscreteComplexFunction(const DiscreteRealFunction &other): DiscreteComplexFunction(other.getVector().base_change<Complex>(), other.getDomain()) {}

DiscreteComplexFunction DiscreteComplexFunction::operator+(const DiscreteComplexFunction &g) const {
	if (g.samples() != samples()) throw std::runtime_error("DiscreteComplexFunction: incompatible sizes");
	return DiscreteComplexFunction(fn + g.fn, domain());
}

DiscreteComplexFunction DiscreteComplexFunction::operator+(Complex a) const {
	auto v = fn;
	for (int i=0; i<samples(); ++i) v[i] = v[i] + a;
	return DiscreteComplexFunction(v, domain());
}

DiscreteComplexFunction DiscreteComplexFunction::operator+(float a) const {
	return this->operator+(Complex(a));
}

DiscreteComplexFunction DiscreteComplexFunction::operator*(const DiscreteComplexFunction &g) const {
	if (samples() != g.samples()) {
		throw std::runtime_error("DiscreteComplexFunction: incompatible sizes");
	}
	return DiscreteComplexFunction(fn.pointwise_product(g.fn), domain());
}

DiscreteComplexFunction DiscreteComplexFunction::operator*(Complex a) const {
	vector<Complex> v;
	v.reserve(samples());
	for (int i=0; i<samples(); ++i) {
		v.emplace_back(fn[i] * a);
	}
	return DiscreteComplexFunction(v, domain());
}

DiscreteComplexFunction DiscreteComplexFunction::operator/(const DiscreteComplexFunction &g) const {
	if (samples() != g.samples()) {
		throw std::runtime_error("DiscreteComplexFunction: incompatible sizes");
	}
	auto v = fn;
	for (int i=0; i<samples(); ++i) {
		v[i] = fn[i] / g[i];
	}
	return DiscreteComplexFunction(v, domain());}

Complex DiscreteComplexFunction::operator()(float x) const {
	float a = dom_a;
	float b = dom_b;
	float L = b - a;
	int n_x = floor(samples()*(x - a)/L);
	float delta_x = (x - a)*samples()/L - n_x;
	return fn[n_x] + delta_x * (fn[n_x+1] - fn[n_x]);
}

DiscreteRealFunction DiscreteComplexFunction::re() const {
	return DiscreteRealFunction(fn.base_change<float>([](Complex c) { return c.real(); }), domain());
}

DiscreteRealFunction DiscreteComplexFunction::im() const {
	return DiscreteRealFunction(fn.base_change<float>([](Complex c) { return c.imag(); }), domain());
}

DiscreteRealFunction DiscreteComplexFunction::abs() const {
	return DiscreteRealFunction(fn.base_change<float>([](Complex c) { return norm(c.z); }), domain());
}

DiscreteRealFunction DiscreteComplexFunction::arg() const {
	return DiscreteRealFunction(fn.base_change<float>([](Complex c) { return c.arg(); }), domain());
}

DiscreteComplexFunction DiscreteComplexFunction::conj() const {
	return DiscreteComplexFunction(fn.base_change<Complex>([](Complex c) { return c.conj(); }), domain());
}

DiscreteComplexFunction DiscreteComplexFunction::fft() const {
	if (samples() == 1) {
		return DiscreteComplexFunction(vector{fn[0]}, domain());
	}
	if (samples() % 2 == 0) {
		auto f_e = downsample(2).fft();
		auto f_o = downsample(2, 1).fft();
		Complex w = exp(1.0i * TAU / samples());
		vector<Complex> res = vector<Complex>(samples());
		for (int k = 0; k < samples()/2; ++k) {
			res[k] = f_e[k] + w.pow(k) * f_o[k];
			res[k + samples()/2] = f_e[k] - w.pow(k) * f_o[k];
		}
		return DiscreteComplexFunction(res, domain());
	}

	auto v = Vector<Complex>(samples(), [fn=fn, n=samples()](int k) {
		Complex c = 0i;
		for (int j=0; j<n; j++)
			c = c + exp(-1.0i * TAU/n  * k * j)*fn[j];
		return c;
	});
	return DiscreteComplexFunction(v, domain());
}

DiscreteComplexFunction DiscreteComplexFunction::ifft_() const {
	float d = dom_b;
	if (samples() == 1) {
		return DiscreteComplexFunction(vector{fn[0]},domain());
	}
	if (samples() % 2 == 0) {
		auto f_e = downsample(2).ifft_();
		auto f_o = downsample(2, 1).ifft_();
		Complex w = exp(-1.0i * TAU / samples());
		vector<Complex> res = vector<Complex>(samples());
		for (int k = 0; k < samples()/2; ++k) {
			res[k] = f_e[k] + w.pow(k) * f_o[k];
			res[k + samples()/2] = f_e[k] - w.pow(k) * f_o[k];
		}
		return DiscreteComplexFunction(res,domain());
	}
	auto v = Vector<Complex>(samples(), [fn=fn, n=samples()](int i) {
		Complex c = 0i;
		Complex w = exp(1.0i * (TAU/n  * i));
		for (int j=0; j<n; ++j) {
			c += w.pow(j)*fn[j];
		}
		return c;
	});
	return DiscreteComplexFunction(v, domain());
}

DiscreteComplexFunction DiscreteComplexFunction::shift_domain_left() const {
	return DiscreteComplexFunction(
			fn.slice_from(samples()/2).concat(fn.slice_to(samples()/2)),
			domain()-vec2(supp_len())/2
			);}

DiscreteComplexFunction DiscreteComplexFunction::shift_domain_right() const {
	return DiscreteComplexFunction(
			fn.slice_from(samples()/2).concat(fn.slice_to(samples()/2)),
			domain()+vec2(supp_len())/2
			);
}

DiscreteComplexFunction DiscreteComplexFunction::downsample(int factor, int shift) const {
	vector<Complex> res;
	res.reserve(samples()/factor);
	for (int i=shift; i<samples(); i+=factor) {
		res.push_back(fn[i]);
	}
	return DiscreteComplexFunction(res, domain());
}

DiscreteRealFunction::DiscreteRealFunction(const Vector<float> &fn, vec2 domain): fn(fn), domain(domain) {
	// if (fn.size() % 2 != 0) {
	// 	throw NotImplementedError("Assuming the length is even, as there are differences in shifts, so just assume even for now not to write more dubious code");
	// }
}

DiscreteRealFunction::DiscreteRealFunction(const HOM(float, float) &f, vec2 domain, int sampling)
: fn(sampling, [domain, sampling, &f](int i) { return f(domain[0] + (domain[1] - domain[0]) * i / (sampling-1)); }),
  domain(domain) {
	// if (fn.size() % 2 != 0) {
	// 	throw NotImplementedError("Assuming the length is even, as there are differences in shifts, so just assume even for now not to write more dubious code");
	// }
}

float DiscreteRealFunction::operator[](int i) const {
	if (i < 0) i += samples();
	return fn[i];
}

DiscreteRealFunction::DiscreteRealFunction(const DiscreteRealFunction &other): fn(other.fn),
																			   domain(other.domain) {}

DiscreteRealFunction::DiscreteRealFunction(DiscreteRealFunction &&other) noexcept: fn(std::move(other.fn)),
																				   domain(std::move(other.domain)) {}

DiscreteRealFunction & DiscreteRealFunction::operator=(const DiscreteRealFunction &other) {
	if (this == &other)
		return *this;
	fn = other.fn;
	domain = other.domain;
	return *this;
}

DiscreteRealFunction & DiscreteRealFunction::operator=(DiscreteRealFunction &&other) noexcept {
	if (this == &other)
		return *this;
	fn = std::move(other.fn);
	domain = std::move(other.domain);
	return *this;
}

DiscreteRealFunction DiscreteRealFunction::operator+(const DiscreteRealFunction &g) const {
	if (g.samples() != samples()) throw std::runtime_error("DiscreteRealFunction: incompatible sizes");
	return DiscreteRealFunction(fn + g.fn, domain);
}

DiscreteRealFunction DiscreteRealFunction::operator+(float a) const {
	return DiscreteRealFunction(fn + a, domain);
}

DiscreteRealFunction DiscreteRealFunction::slice(int a, int b) const {
	return DiscreteRealFunction(fn.slice(a, b), vec2(domain[0] + a * sampling_step(), domain[0] + b * sampling_step()));
}

float DiscreteRealFunction::operator()(float x) const {
	int n_x = ::floor(samples()*(x - domain[0])/(domain[1] - domain[0]));
	if (n_x >= samples()-2)
		return fn[samples()-2];

	float delta_x = (x - domain[0])*samples()/(domain[1] - domain[0]) - n_x;
	return fn[n_x] + delta_x * (fn[n_x+1] - fn[n_x]);
}

DiscreteRealFunction DiscreteRealFunction::two_sided_zero_padding(int target_size) const {
	int dif = target_size - samples();
	if (dif <= 0)
		return *this;
	int left = dif / 2;
	int right = dif - left;
	auto left_part = Vector<float>::zeros(left);
	auto right_part = Vector<float>::zeros(right);
	vec2 dom = domain + vec2(-left*sampling_step(), right*sampling_step());
	return DiscreteRealFunction(left_part.concat(fn).concat(right_part), dom);
}

DiscreteComplexFunction DiscreteRealFunction::fft() const {
	if (samples() == 1) {
		return DiscreteComplexFunction(vector{Complex(fn[0])}, domain);
	}
	if (samples() % 2 == 0) {
		auto f_e = downsample(2, 0).fft();
		auto f_o = downsample(2, 1).fft();
		Complex w = exp(1.0i * TAU / samples());
		vector<Complex> res = vector<Complex>(samples());
		for (int k = 0; k < samples()/2; ++k) {
			res[k] = f_e[k] + w.pow(k) * f_o[k];
			res[k + samples()/2] = f_e[k] - w.pow(k) * f_o[k];
		}
		return DiscreteComplexFunction(res, domain);
	}

	auto v = Vector<Complex>(samples(), [fn=fn, n=samples()](int k) {
		Complex c = 0i;
		for (int j=0; j<n; j++)
			c = c + exp(-1.0i * TAU/n  * k * j)*fn[j];
		return c;
	});
	return DiscreteComplexFunction(v, domain);
}

DiscreteRealFunction DiscreteRealFunction::convolve(const DiscreteRealFunction &kernel) const {
	auto k_ = kernel;
	if (kernel.samples() > samples()) throw std::runtime_error("DiscreteRealFunction: kernel too long to convolve");
	if (kernel.samples() < samples()) {
		k_ = k_.two_sided_zero_padding(samples());
	}
	auto f_fft = fft();
	auto k_fft = k_.fft();

	// if (f_fft.starts_from_zero()) f_fft = f_fft.shift_domain_left();
	// if (k_fft.starts_from_zero()) k_fft = k_fft.shift_domain_left();
	// auto prod = (f_fft * k_fft);
	// auto sh = prod.shift_domain_right();
	// auto shiifft = sh.ifft().re();

	return (f_fft * k_fft).ifft().re();

	// return (f_fft * k_fft).shift_domain_right().ifft().re();

}

DiscreteRealFunction DiscreteRealFunction::derivative() const {
	auto dft = fft();
	auto v = DiscreteComplexFunction(Vector<Complex>(samples(), [n=samples()](int k) {
		return 1.0i*(TAU/n)*k ;
	}), dft.domain());
	return (dft*v).ifft().re();
}
// TODO test that, it will be annoying to debug in practice
DiscreteRealFunction DiscreteRealFunction::shift_domain_left() const {
	return DiscreteRealFunction(
			fn.slice_from(samples()/2).concat(fn.slice_to(samples()/2)),
			domain-vec2(supp_len())/2
			);
}

DiscreteRealFunction DiscreteRealFunction::shift_domain_right() const {
	return DiscreteRealFunction(
			fn.slice_from(samples()/2).concat(fn.slice_to(samples()/2)),
			domain+vec2(supp_len())/2
			);}

DiscreteRealFunction DiscreteRealFunction::downsample(int factor) const {
	vector<float> res;
	res.reserve(samples()/factor);
	for (int i=0; i<samples(); i+=factor) {
		float a = 0;
		for (int j=0; j<factor; ++j)
			a += fn[(j + i) % samples()]/factor;
		res.push_back(a);
	}
	return DiscreteRealFunction(res, getDomain());
}

DiscreteRealFunction DiscreteRealFunction::downsample(int factor, int shift) const {
	vector<float> res;
	res.reserve(samples()/factor);
	for (int i=shift; i<samples(); i+=factor) {
		res.push_back(fn[i]);
	}
	return DiscreteRealFunction(res, getDomain());}

DiscreteRealFunction DiscreteRealFunction::downsample_max(int factor) const {
	vector<float> res;
	res.reserve(samples()/factor);
	for (int i=0; i<samples(); i+=factor) {
		float a = fn[i];
		for (int j=1; j<factor; ++j)
			if (float b = fn[(i + j)%samples()]; abs(a) < abs(b))
				a = b;

		res.push_back(a);
	}
	return DiscreteRealFunction(res, getDomain());}

DiscreteRealFunctionNonUniform::DiscreteRealFunctionNonUniform(const DiscreteRealFunction &args, const DiscreteRealFunction &values): sampling_args(args), values(values) {}

DiscreteRealFunctionNonUniform::DiscreteRealFunctionNonUniform(const RealFunction &f, float arclen_step, float linear_eps, vec2 bounds, bool include_bounds): DiscreteRealFunctionNonUniform(DiscreteRealFunction(vector<float>{}), DiscreteRealFunction(vector<float>{})) {
	vector<float> args = {};
	vector<float> values = {};
	float x = bounds[0];
	float realised = 0;
	if (include_bounds) {
		args.emplace_back(x);
		values.emplace_back(f(x));
	}
	while (x < bounds[1]) {
		while (realised < arclen_step)
			realised += linear_eps * ::sqrt(1 + f.df(x) * f.df(x));
		x += realised;
		realised = 0;
		args.emplace_back(x);
		values.emplace_back(f(x));
	}
	if (include_bounds) {
		args.emplace_back(bounds[1]);
		values.emplace_back(f(bounds[1]));
	}

	this->sampling_args = DiscreteRealFunction(args);
	this->values = DiscreteRealFunction(values);
}

float DiscreteRealFunctionNonUniform::operator()(float x) const {
	if ((x < sampling_args[0] || x >= sampling_args[-1]) && sampling_args.samples() == 1) return values[0];
	if (x < sampling_args[0]) {
		float t = (x - sampling_args[0]) / (sampling_args[1] - sampling_args[0]);
		return values[0] + t * (values[1] - values[0]);
	}
	if (x >= sampling_args[-1]) {
		float t = (x - sampling_args[-2]) / (sampling_args[-1] - sampling_args[-2]);
		return values[-2] + t * (values[-1] - values[-2]);
	}
	int i = 0;
	while (i < sampling_args.samples() && sampling_args[i] < x) i++;
	float t = (x - sampling_args[i-1]) / (sampling_args[i] - sampling_args[i-1]);
	return values[i-1] + t * (values[i] - values[i-1]);

}

vec2 DiscreteRealFunctionNonUniform::point(int i) const { return vec2(sampling_args[i], values[i]); }

float DiscreteRealFunctionNonUniform::operator[](int i) const { return values[i]; }

DiscreteRealFunctionNonUniform DiscreteRealFunctionNonUniform::refine_domain(const DiscreteRealFunction &new_args) const {
	vector<float> new_values = {};
	vector<float> all_args = {};
	int i = 0, j = 0;
	while (i < sampling_args.samples() || j < new_args.samples()) {
		if (i == sampling_args.samples()) {
			all_args.push_back(new_args[j]);
			new_values.push_back((*this)(new_args[j]));
			j++;
		}
		else if (j == new_args.samples()) {
			all_args.push_back(sampling_args[i]);
			new_values.push_back(values[i]);
			i++;
		}
		else if (nearlyEqual(sampling_args[i],  new_args[j]))
		{
			all_args.push_back(sampling_args[i]);
			new_values.push_back(values[i]);
			i++;
			j++;
		} else if (sampling_args[i] > new_args[j]) {
			all_args.push_back(new_args[j]);
			new_values.push_back((*this)(new_args[j]));
			j++;
		} else {
			all_args.push_back(sampling_args[i]);
			new_values.push_back(values[i]);
			i++;
		}
	}
	return DiscreteRealFunctionNonUniform(DiscreteRealFunction(all_args), DiscreteRealFunction(new_values));
}

DiscreteRealFunctionNonUniform DiscreteRealFunctionNonUniform::refine_domain(float x) const { return refine_domain(DiscreteRealFunction(vector{x})); }

DiscreteRealFunctionNonUniform DiscreteRealFunctionNonUniform::derivative() const {
	if (size() == 1)
		return DiscreteRealFunctionNonUniform(sampling_args, DiscreteRealFunction(vector{0.f}));

	vector<float> values = {((*this)[1] - (*this)[0])/(sampling_args[1] - sampling_args[0])};
	for (int i = 1; i < sampling_args.samples()-1; i++)
		values.push_back((this->operator()(sampling_args[i+1]) - this->operator()(sampling_args[i-1])) / (sampling_args[i+1] - sampling_args[i-1]));
	values.push_back(((*this)[-1] - (*this)[-2])/(sampling_args[-1] - sampling_args[-2]));
	return DiscreteRealFunctionNonUniform(sampling_args, DiscreteRealFunction(values));
}

Vector<float> DiscreteRealFunctionNonUniform::arclens() const {
	vector<float> arcs = {};
	for (int i = 0; i < sampling_args.samples()-1; i++) {
		arcs.push_back(norm(point(i)-point(i+1)));
	}
	return Vector(arcs);
}

DiscreteRealFunctionNonUniform DiscreteRealFunctionNonUniform::operator+(const DiscreteRealFunctionNonUniform &g) const {
	auto f_ = refine_domain(g.sampling_args);
	auto g_ = g.refine_domain(sampling_args);
	return DiscreteRealFunctionNonUniform(f_.sampling_args, f_.values + g_.values);
}

DiscreteRealFunctionNonUniform DiscreteRealFunctionNonUniform::operator+(float a) const { return DiscreteRealFunctionNonUniform(sampling_args, values + a); }

DiscreteRealFunctionNonUniform operator+(float a, const DiscreteRealFunctionNonUniform &f) { return f + a; }

DiscreteRealFunctionNonUniform DiscreteRealFunctionNonUniform::operator-() const { return DiscreteRealFunctionNonUniform(sampling_args, -values); }

DiscreteRealFunctionNonUniform DiscreteRealFunctionNonUniform::operator-(const DiscreteRealFunctionNonUniform &g) const { return (*this) + (-g); }

DiscreteRealFunctionNonUniform DiscreteRealFunctionNonUniform::operator-(float a) const { return DiscreteRealFunctionNonUniform(sampling_args, values - a); }

DiscreteRealFunctionNonUniform operator-(float a, const DiscreteRealFunctionNonUniform &f) { return f + -a; }

DiscreteRealFunctionNonUniform DiscreteRealFunctionNonUniform::operator*(const DiscreteRealFunctionNonUniform &g) const {
	auto f_ = refine_domain(g.sampling_args);
	auto g_ = g.refine_domain(sampling_args);
	return DiscreteRealFunctionNonUniform(f_.sampling_args, f_.values * g_.values);}

DiscreteRealFunctionNonUniform DiscreteRealFunctionNonUniform::operator*(float a) const { return DiscreteRealFunctionNonUniform(sampling_args, values * a); }

DiscreteRealFunctionNonUniform operator*(float a, const DiscreteRealFunctionNonUniform &f) { return f * a; }

DiscreteRealFunctionNonUniform DiscreteRealFunctionNonUniform::operator/(const DiscreteRealFunctionNonUniform &g) const { return (*this) * (1.f/g); }

DiscreteRealFunctionNonUniform DiscreteRealFunctionNonUniform::operator/(float a) const { return DiscreteRealFunctionNonUniform(sampling_args, values / a); }

DiscreteRealFunctionNonUniform operator/(float a, const DiscreteRealFunctionNonUniform &f) {
	vector <float> new_values;
	for (int i = 0; i < f.values.samples(); ++i)
		new_values.emplace_back(a / f.values[i]);
	return DiscreteRealFunctionNonUniform(f.sampling_args, DiscreteRealFunction(new_values));
}

DiscreteRealFunctionNonUniform DiscreteRealFunctionNonUniform::operator&(const DiscreteRealFunctionNonUniform &g) const {
	vector<float> values = {};
	for (int i=0; i<g.size(); i++) {
		values.push_back((*this)(g.values[i]));
	}
	return DiscreteRealFunctionNonUniform(g.sampling_args, values);
}

DiscreteRealFunctionNonUniform operator&(const DiscreteRealFunction &f, const DiscreteRealFunctionNonUniform &g) {
	vector<float> values = {};
	for (int i = 0; i < g.size(); ++i) {
		values.push_back(f(g[i]));
	}
	return DiscreteRealFunctionNonUniform(g.args_vector(), values);
}

DiscreteRealFunctionNonUniform operator&(const DiscreteRealFunctionNonUniform &f, const DiscreteRealFunction &g) {
	vector<float> values = {};
	for (int i = 0; i < f.size(); ++i) {
		values.push_back(f(g[i]));
	}
	vector<float> args = linspace(g.getDomain()[0], g.getDomain()[1], f.samples());
	return DiscreteRealFunctionNonUniform(args, values);
}

DiscreteRealFunctionNonUniform operator&(const DiscreteRealFunction &f, const DiscreteRealFunction &g) {
	vector<float> values = {};
	for (int i = 0; i < f.samples(); ++i) {
		values.push_back(f(g[i]));
	}
	vector<float> args = linspace(g.getDomain()[0], g.getDomain()[1], f.samples());
	return DiscreteRealFunctionNonUniform(args, values);
}

DiscreteComplexFunctionR2::DiscreteComplexFunctionR2(const vector<DiscreteComplexFunction> &fn, vec2 domain): fn(fn), domain(domain) {}

// DiscreteComplexFunctionR2::DiscreteComplexFunctionR2(const vector<std::function<Complex(float)>> &_f, vec2 domain, int sampling)
// : fn(_f.size(), [_f, domain, sampling](int i) {
// 		return DiscreteComplexFunction(_f[i], domain, sampling);
// 	})
// {}

int DiscreteComplexFunctionR2::samples_t() const { return fn.size(); }

int DiscreteComplexFunctionR2::samples_x() const { return fn[0].samples(); }

float DiscreteComplexFunctionR2::sampling_step_t() const { return (domain[1] - domain[0]) / (samples_t()-1); }

float DiscreteComplexFunctionR2::sampling_step_x() const { return fn[0].sampling_step(); }

vector<float> DiscreteComplexFunctionR2::args_t() const { return linspace(domain[0], domain[1], samples_t()); }

vector<float> DiscreteComplexFunctionR2::args_x() const { return fn[0].args(); }

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::operator+(const DiscreteComplexFunctionR2 &g) const {
	vector<DiscreteComplexFunction> result;
	result.reserve(samples_t());
	for (int i = 0; i < samples_t(); ++i) {
		result.push_back(fn[i] + g.fn[i]);
	}
	return DiscreteComplexFunctionR2(result, domain);
}

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::operator+(Complex a) const {
	vector<DiscreteComplexFunction> result;
	result.reserve(samples_t());
	for (int i = 0; i < samples_t(); ++i) {
		result.push_back(fn[i] + a);
	}
	return DiscreteComplexFunctionR2(result, domain);
}

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::operator+(float a) const {return this->operator+(Complex(a));}

DiscreteRealFunction max(const DiscreteRealFunction &f, const DiscreteRealFunction &g) {
	vector<float> res;
	res.reserve(f.samples());
	for (int i = 0; i < f.samples(); ++i) {
		res.emplace_back(std::max(f[i], g[i]));
	}
	return DiscreteRealFunction(res, f.domain);
}

DiscreteRealFunction max(const DiscreteRealFunction &f, float g) {
	vector<float> res;
	res.reserve(f.samples());
	for (int i = 0; i < f.samples(); ++i)
		res.emplace_back(std::max(f[i], g));

	return DiscreteRealFunction(res, f.domain);
}

DiscreteRealFunction max(float f, const DiscreteRealFunction &g) {
	return max(g, f);
}

DiscreteRealFunction min(const DiscreteRealFunction &f, const DiscreteRealFunction &g) {
	vector<float> res;
	res.reserve(f.samples());
	for (int i = 0; i < f.samples(); ++i) {
		res.push_back(std::min(f[i], g[i]));
	}
	return DiscreteRealFunction(res, f.domain);}

DiscreteRealFunction min(const DiscreteRealFunction &f, float g) {
	vector<float> res;
	res.reserve(f.samples());
	for (int i = 0; i < f.samples(); ++i)
		res.push_back(std::min(f[i], g));

	return DiscreteRealFunction(res, f.domain);
}
DiscreteRealFunction min(float f, const DiscreteRealFunction &g) {
	return min(g, f);
}

DiscreteRealFunction maxAbs(const DiscreteRealFunction &f, const DiscreteRealFunction &g) {
	vector<float> res;
	res.reserve(f.samples());
	for (int i = 0; i < f.samples(); ++i) {
		float a = f[i];
		float b = g[i];
		if (abs(a) > abs(b))
			res.push_back(a);
		 else
			res.push_back(b);

	}
	return DiscreteRealFunction(res, f.domain);
}


DiscreteComplexFunctionR2 operator+(float a, const DiscreteComplexFunctionR2 &f) { return f + a; }

DiscreteComplexFunctionR2 operator+(Complex a, const DiscreteComplexFunctionR2 &f) { return f + a; }

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::operator-() const {
	return DiscreteComplexFunctionR2(map<DiscreteComplexFunction, DiscreteComplexFunction>(fn, [](const DiscreteComplexFunction &f) { return -f; }), domain);
}

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::operator-(const DiscreteComplexFunctionR2 &g) const { return (*this) + (-g); }

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::operator-(float a) const { return (*this) + (-a); }

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::operator-(Complex a) const { return (*this) + (-a); }

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::operator*(const DiscreteComplexFunctionR2 &g) const {
	vector<DiscreteComplexFunction> res;
	res.reserve(samples_t());
	for (int i = 0; i < samples_t(); ++i) {
		res.push_back(fn[i] * g.fn[i]);
	}
	return DiscreteComplexFunctionR2(res, domain);
}

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::operator*(Complex a) const {
	vector<DiscreteComplexFunction> res;
	for (int i = 0; i < samples_t(); ++i) {
		res.push_back(fn[i] * a);
	}
	return DiscreteComplexFunctionR2(res, domain);
}

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::operator*(float a) const { return this->operator*(Complex(a)); }

DiscreteComplexFunctionR2 operator*(float a, const DiscreteComplexFunctionR2 &f) { return f * a; }

DiscreteComplexFunctionR2 operator*(Complex a, const DiscreteComplexFunctionR2 &f) { return f * a; }

DiscreteComplexFunctionR2 operator-(float a, const DiscreteComplexFunctionR2 &f) { return f + -a; }

DiscreteComplexFunctionR2 operator-(Complex a, const DiscreteComplexFunctionR2 &f) { return f + -a; }


DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::operator/(const DiscreteComplexFunctionR2 &g) const {
	vector<DiscreteComplexFunction> res;
	for (int i = 0; i < samples_t(); ++i) {
		res.push_back(fn[i] / g.fn[i]);
	}
	return DiscreteComplexFunctionR2(res, domain);
}

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::operator/(float a) const { return this->operator*(1.0f/a); }

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::operator/(Complex a) const { return this->operator*(1.0f/a); }

DiscreteComplexFunction DiscreteComplexFunctionR2::operator[](int t) const { return fn[t]; }

Complex DiscreteComplexFunctionR2::operator()(float t, float x) const {
	int t1 = (int) ::floor((t - domain[0]) / sampling_step_t());
	int t2 = (int) ::ceil((t - domain[0]) / sampling_step_t());
	float u = (t - domain[0]) / sampling_step_t() - t1;
	return fn[t1](x) * (1 - u) + fn[t2](x) * u;
}

Complex DiscreteComplexFunctionR2::operator()(vec2 x) const { return operator()(x.x, x.y); }

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::transpose() const {
		vector<DiscreteComplexFunction> res = vector<DiscreteComplexFunction>();
		for (int i=0; i<samples_x(); i++) {
			res.push_back(DiscreteComplexFunction(Vector<Complex>(samples_t(), [&](int j) { return fn[j][i]; }), domain));
		}
		return DiscreteComplexFunctionR2(res, fn[0].getDomain());
	}

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::fft_t() const {
	vector<DiscreteComplexFunction> res = vector<DiscreteComplexFunction>();
	for (auto &f : fn) {
		res.push_back(f.fft());
	}
	return DiscreteComplexFunctionR2(res, domain);
}

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::ifft_t() const {
	vector<DiscreteComplexFunction> res = vector<DiscreteComplexFunction>();
	for (auto &f : fn) {
		res.push_back(f.ifft());
	}
	return DiscreteComplexFunctionR2(res, domain);
}

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::fft_x() const {
	return transpose().fft_t().transpose();
}

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::ifft_x() const {
	return transpose().ifft_t().transpose();
}

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::fft() const {
	float Lt=domain[1]-domain[0];
	float Lx=fn[0].domain()[1]-fn[0].domain()[0];
	vector<DiscreteComplexFunction> res = vector<DiscreteComplexFunction>();


	for (int it=0; it<samples_t(); ++it) {
		const vector<Complex> v = Vector<Complex>(samples_x(), [fn=fn, nx=samples_x(), Lx, Lt, nt=samples_t(), it](int ix) {
			Complex c = 0i;
			for (int jx = 0; jx < nx; ++jx)
				for (int kt = 0; kt < nt; ++kt)
					c += exp(-1.0i * (TAU / nx * ix * jx) - 1.0i * (TAU / nt * it * kt)) * fn[kt][jx];
			return c;
		}).vec();
		res.push_back(DiscreteComplexFunction(v, vec2(0, TAU/Lx)));
	}
	return DiscreteComplexFunctionR2(res, vec2(0, TAU/Lt));
}

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::fft(int var) const {
	if (var == 0) return fft_x();
	return fft_t();
}

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::ifft(int var) const {
	if (var == 0) return ifft_x();
	return ifft_t();
}

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::ifft() const {
	float wt=domain[1];
	float wx=fn[0].domain()[1];
	vector<DiscreteComplexFunction> res = vector<DiscreteComplexFunction>();
	res.reserve(samples_t());

	for (int it=0; it<samples_t(); ++it) {
		auto v = Vector<Complex>(samples_x(), [fn=fn, nx=samples_x(), nt=samples_t(), it](int ix) {
			Complex c = 0i;
			for (int jx=0; jx<nx; ++jx)
				for (int kt=0; kt<nt; ++kt)
					c += exp(1.0i * (TAU/nx  * ix*jx) + 1.0i * (TAU/nt * it*kt))*fn[kt][jx];
			return c/(nt*nx);
		}).vec();
		res.push_back(DiscreteComplexFunction(v, vec2(0, TAU/wx)));
	}
	return DiscreteComplexFunctionR2(res, vec2(0, TAU/wt));
}

DiscreteRealFunctionR2 DiscreteComplexFunctionR2::re() const {
	vector<DiscreteRealFunction> res;
	res.reserve(fn.size());
	for (int i=0; i<fn.size(); i++) {
		res.emplace_back(fn[i].re());
	}
	return DiscreteRealFunctionR2(res, domain);
}

DiscreteRealFunctionR2 DiscreteComplexFunctionR2::im() const {
	vector<DiscreteRealFunction> res;
	res.reserve(fn.size());
	for (int i=0; i<fn.size(); i++) {
		res.emplace_back(fn[i].im());
	}
	return DiscreteRealFunctionR2(res, domain);
}

DiscreteRealFunctionR2 DiscreteComplexFunctionR2::abs() const {
	vector<DiscreteRealFunction> res;
	res.reserve(fn.size());
	for (int i=0; i<fn.size(); i++) {
		res.push_back(fn[i].abs());
	}
	return DiscreteRealFunctionR2(res, domain);
}

DiscreteRealFunctionR2 DiscreteComplexFunctionR2::arg() const {
	vector<DiscreteRealFunction> res;
	res.reserve(fn.size());
	for (int i=0; i<fn.size(); i++) {
		res.push_back(fn[i].arg());
	}
	return DiscreteRealFunctionR2(res, domain);
}

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::downsample_t(int factor) const {
	vector<DiscreteComplexFunction> res = vector<DiscreteComplexFunction>();
	for (int i=0; i<fn.size(); i+=factor) {
		res.push_back(fn[i]);
	}
	return DiscreteComplexFunctionR2(res, domain);
}


DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::downsample_x(int factor) const {
	vector<DiscreteComplexFunction> res = vector<DiscreteComplexFunction>();
	for (auto &f : fn) {
		res.push_back(f.downsample(factor));
	}
	return DiscreteComplexFunctionR2(res, domain);
}

DiscreteComplexFunctionR2 DiscreteComplexFunctionR2::downsample(int factor_t, int factor_x) const {
	return downsample_x(factor_x).downsample_t(factor_t);
}

DiscreteRealFunctionR2::DiscreteRealFunctionR2(const vector<DiscreteRealFunction> &fn, vec2 domain): fn(fn), domain(domain) {}

DiscreteRealFunctionR2::DiscreteRealFunctionR2(const vector<std::function<float(float)>> &f, vec2 domain, int sampling) {
	fn = vector<DiscreteRealFunction>();
	for (int i=0; i<f.size(); i++) {
		fn.emplace_back(f[i], domain, sampling);
	}
}

DiscreteRealFunction DiscreteRealFunctionR2::operator[](int t) const { return fn[t]; }

DiscreteRealFunction DiscreteRealFunctionR2::operator()(float t) const {
	int i0 = ::floor((t-domain[0]) / (domain[1]-domain[0]) * (samples_t()-1));
	float d = (t-domain[0]) / (domain[1]-domain[0]) * (samples_t()-1) - i0;
	return DiscreteRealFunction((*this)[i0] * (1.f-d) + (*this)[i0+1] * d);
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::operator+(const DiscreteRealFunctionR2 &g) const {
	vector<DiscreteRealFunction> res;
	for (int i=0; i<fn.size(); i++)
		res.push_back(fn[i] + g.fn[i]);

	return DiscreteRealFunctionR2(res, domain);
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::operator+(float a) const {
	vector<DiscreteRealFunction> res;
	for (int i=0; i<fn.size(); i++) {
		res.push_back(fn[i] + a);
	}
	return DiscreteRealFunctionR2(res, domain);
}

DiscreteRealFunctionR2 operator+(float a, const DiscreteRealFunctionR2 &f) { return f + a; }

DiscreteRealFunctionR2 DiscreteRealFunctionR2::operator-() const {
	vector<DiscreteRealFunction> res;
	for (int i=0; i<fn.size(); i++) {
		res.push_back(-fn[i]);
	}
	return DiscreteRealFunctionR2(res, domain);
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::operator-(const DiscreteRealFunctionR2 &g) const {
	vector<DiscreteRealFunction> res;
	for (int i=0; i<fn.size(); i++) {
		res.push_back(fn[i] - g.fn[i]);
	}
	return DiscreteRealFunctionR2(res, domain);
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::operator-(float a) const {
	vector<DiscreteRealFunction> res;
	for (int i=0; i<fn.size(); i++) {
		res.push_back(fn[i] - a);
	}
	return DiscreteRealFunctionR2(res, domain);
}

DiscreteRealFunctionR2 operator-(float a, const DiscreteRealFunctionR2 &f) { return f + -a; }

DiscreteRealFunctionR2 DiscreteRealFunctionR2::operator*(float a) const {
	vector<DiscreteRealFunction> res;
	for (int i=0; i<fn.size(); i++) {
		res.push_back(fn[i] * a);
	}
	return DiscreteRealFunctionR2(res, domain);
}

DiscreteRealFunctionR2 operator*(float a, const DiscreteRealFunctionR2 &f) { return f * a; }

DiscreteRealFunctionR2 DiscreteRealFunctionR2::operator/(float a) const {
	vector<DiscreteRealFunction> res;
	for (int i=0; i<fn.size(); i++) {
		res.push_back(fn[i] / a);
	}
	return DiscreteRealFunctionR2(res, domain);
}

DiscreteRealFunctionR2 operator/(float a, const DiscreteRealFunctionR2 &f) {
	vector<DiscreteRealFunction> res;
	for (int i=0; i<f.fn.size(); i++) {
		res.push_back(DiscreteRealFunction([a](float x) { return a/x; }, f.domain, f.fn[i].samples()));
	}
	return DiscreteRealFunctionR2(res, f.domain);
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::operator/(const DiscreteRealFunctionR2 &g) const {
	vector<DiscreteRealFunction> res;
	for (int i=0; i<fn.size(); i++) {
		res.push_back(fn[i] / g.fn[i]);
	}
	return DiscreteRealFunctionR2(res, domain);
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::dfdx() const {
	vector<DiscreteRealFunction> res;
	for (int i=0; i<fn.size(); i++) {
		res.push_back(fn[i].derivative());
	}
	return DiscreteRealFunctionR2(res, domain);
}

int DiscreteRealFunctionR2::samples_t() const { return fn.size(); }

int DiscreteRealFunctionR2::samples_x() const { return fn[0].samples(); }

float DiscreteRealFunctionR2::sampling_step_t() const { return (domain[1] - domain[0]) / (samples_t()-1); }

float DiscreteRealFunctionR2::sampling_step_x() const { return fn[0].sampling_step(); }

vector<float> DiscreteRealFunctionR2::args_t() const { return linspace(domain[0], domain[1], samples_t()); }

vector<float> DiscreteRealFunctionR2::args_x() const { return fn[0].args(); }

DiscreteRealFunctionR2 DiscreteRealFunctionR2::operator*(const DiscreteRealFunctionR2 &g) const {
	vector<DiscreteRealFunction> res;
	res.reserve(fn.size());
	for (int i=0; i<fn.size(); i++)
		res.emplace_back(fn[i] + g.fn[i]);

	return DiscreteRealFunctionR2(res, domain);
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::transpose() const {
	vector<DiscreteRealFunction> res = vector<DiscreteRealFunction>();
	res.reserve(samples_x());
	for (int i=0; i<samples_x(); i++)
		res.emplace_back(Vector<float>(samples_t(), [&](int j) { return fn[j][i]; }), domain);

	return DiscreteRealFunctionR2(res, fn[0].getDomain());
}

DiscreteRealFunction DiscreteRealFunctionR2::integrate_t() const {return transpose().integrate_x(); }

DiscreteRealFunction DiscreteRealFunctionR2::integrate_x() const {return DiscreteRealFunction(Vector<float>(fn.size(), [&](int i) { return fn[i].integral(); }), domain); }

float DiscreteRealFunctionR2::double_integral() const { return integrate_x().integral(); }

float DiscreteRealFunctionR2::double_integral(int i_t) const { return integrate_x().integral(i_t); }

DiscreteComplexFunctionR2 DiscreteRealFunctionR2::complexify() const {
	vector<DiscreteComplexFunction> res = vector<DiscreteComplexFunction>();
	res.reserve(fn.size());
	for (auto &f : fn)
		res.emplace_back(f);

	return DiscreteComplexFunctionR2(res, domain);
}

DiscreteComplexFunctionR2 DiscreteRealFunctionR2::fft_t() const {
	return complexify().fft_t();
}

DiscreteComplexFunctionR2 DiscreteRealFunctionR2::fft_x() const {
	return complexify().fft_x();
}

DiscreteComplexFunctionR2 DiscreteRealFunctionR2::fft() const {
	return complexify().fft();
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::double_convolve(const DiscreteRealFunctionR2 &kernel) const {
	return (this->fft()*(kernel.fft())).ifft().re();
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::convolve_x(const DiscreteRealFunction &kernel) const {
	vector<DiscreteRealFunction> convolved;
	convolved.reserve(fn.size());
	auto k_ = kernel;
	if (kernel.samples() > samples_x()) throw std::runtime_error("DiscreteRealFunction: kernel too long to convolve");
	if (kernel.samples() < samples_x()) {
		k_ = k_.two_sided_zero_padding(samples_x());
	}
	auto k_fft = k_.fft();
	for (const auto &f : fn) {
		convolved.push_back((f.fft() * k_fft).ifft().re());
	}
	return DiscreteRealFunctionR2(convolved, domain);
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::convolve_x(const DiscreteRealFunctionR2 &kernel) const {
	vector<DiscreteRealFunction> convolved;
	convolved.reserve(fn.size());

	for (int i = 0; i < fn.size(); ++i) {
		convolved.push_back(fn[i].convolve(kernel[i]));
	}
	return DiscreteRealFunctionR2(convolved, domain);
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::smoothen_x(float L) const {
	auto k = DiscreteRealFunction([L](float x){return ::exp(-x*x/L/L);}, domain_x(), samples_x());
	k = k / k.L2_norm();
	return convolve_x(k);
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::smoothen_t(float L) const {
	return transpose().smoothen_x(L).transpose();
}

DiscreteComplexFunctionR2 DiscreteRealFunctionR2::fft(int var) const {
	if (var == 0) return fft_x();
	return fft_t();
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::downsample_t(int factor) const {
	vector<DiscreteRealFunction> res = vector<DiscreteRealFunction>();
	res.reserve(fn.size()/factor);
	for (int i=0; i<fn.size(); i+=factor) {
		res.push_back(fn[i]);
	}
	return DiscreteRealFunctionR2(res, domain);
}
DiscreteRealFunctionR2 DiscreteRealFunctionR2::downsample_t(int factor, int shift) const {
	vector<DiscreteRealFunction> res = vector<DiscreteRealFunction>();
	res.reserve(fn.size()/factor);
	for (int i=shift; i<fn.size(); i+=factor) {
		res.push_back(fn[i]);
	}
	return DiscreteRealFunctionR2(res, domain);
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::downsample_t_max(int factor) const {
	vector<DiscreteRealFunction> res = vector<DiscreteRealFunction>();
	res.reserve(fn.size()/factor);
	for (int i=0; i<fn.size(); i+=factor) {
		auto a = fn[i];
		for (int j=1; j<factor; ++j)
			a = max(a, fn[(j+i)%fn.size()]);
		res.push_back(a);
	}
	return DiscreteRealFunctionR2(res, domain);
}


DiscreteRealFunctionR2 DiscreteRealFunctionR2::downsample_x(int factor) const {
	vector<DiscreteRealFunction> res = vector<DiscreteRealFunction>();
	res.reserve(fn.size());
	for (auto &f : fn) {
		res.push_back(f.downsample(factor));
	}
	return DiscreteRealFunctionR2(res, domain);
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::downsample_x(int factor, int shift) const {
	vector<DiscreteRealFunction> res = vector<DiscreteRealFunction>();
	res.reserve(fn.size());
	for (auto &f : fn) {
		res.push_back(f.downsample(factor, shift));
	}
	return DiscreteRealFunctionR2(res, domain);
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::downsample(int factor_t, int factor_x) const {
	return downsample_t(factor_t).downsample_x(factor_x);
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::downsample_x_max(int factor) const {
	vector<DiscreteRealFunction> res = vector<DiscreteRealFunction>();
	res.reserve(fn.size());
	for (auto &f : fn) {
		res.push_back(f.downsample_max(factor));
	}
	return DiscreteRealFunctionR2(res, domain);
}

DiscreteRealFunctionR2 DiscreteRealFunctionR2::downsample_max(int factor_t, int factor_x) const {
	return downsample_t_max(factor_t).downsample_x_max(factor_x);
}



Regularity operator-(Regularity a, int b) {
	if (b < 0)
		return a + (-b);
	if (INT(a) < 0 || INT(a) > 6)
		return a;
	return static_cast<Regularity>(std::max(-1, INT(a) - b));
}


Regularity operator+(Regularity a, int b) {
	if (b < 0)
		return a - (-b);
	if (INT(a) < -1 || INT(a) > 5)
		return a;
	return static_cast<Regularity>(std::min(6, INT(a) + b));
}


Regularity operator+(Regularity a, Regularity b) {
	if (INT(a) < 0 && INT(b) < 0)
		return max(a, b);
	if (INT(a) < 68 && INT(b) < 68)
		return static_cast<Regularity>(std::min(6, std::max(0, INT(a)) + std::max(0, INT(b))));
	return max(a, b);
}

Foo31 partialDerivativeOperator(Foo31 f, int i, float epsilon) {
	return directionalDerivativeOperator<vec3, float>(std::move(f), vec3(i == 0, i == 1, i == 2), epsilon);
}

Foo21 partialDerivativeOperator(Foo21 f, int i, float epsilon) {
	return directionalDerivativeOperator<vec2, float>(std::move(f), vec2(i == 0, i == 1), epsilon);
}

Foo33 derivativeOperator(const Foo31 &f, float epsilon) {
	return [f, epsilon](vec3 v) {
		return
				vec3(partialDerivativeOperator(f, 0, epsilon)(v), partialDerivativeOperator(f, 1, epsilon)(v), partialDerivativeOperator(f, 2, epsilon)(v));
	};
}

Foo22 derivativeOperator(const Foo21 &f, float epsilon) {
	return [f, epsilon](vec2 v) {
		return
				vec2(partialDerivativeOperator(f, 0, epsilon)(v), partialDerivativeOperator(f, 1, epsilon)(v));
	};
}

Foo13 derivativeOperator(const Foo13 &f, float epsilon) {
	Fooo f1 = [f](float x) {
		return f(x).x;
	};
	Fooo f2 = [f](float x) {
		return f(x).y;
	};
	Fooo f3 = [f](float x) {
		return f(x).z;
	};
	return [f1, f2, f3, epsilon](float x) {
		return vec3(derivativeOperator(f1, epsilon)(x), derivativeOperator(f2, epsilon)(x), derivativeOperator(f3, epsilon)(x));
	};
}

Foo12 derivativeOperator(const Foo12 &f, float epsilon) {
	Fooo f1 = [f](float x) {
		return f(x).x;
	};
	Fooo f2 = [f](float x) {
		return f(x).y;
	};
	return [f1, f2, epsilon](float x) {
		return vec2(derivativeOperator(f1, epsilon)(x), derivativeOperator(f2, epsilon)(x));
	};
}

RealFunctionR2::RealFunctionR2(): RealFunctionR2([](vec2 v) { return 0; }) {}

RealFunctionR2::RealFunctionR2(const RealFunctionR2 &other): _f(other._f), _df(other._df), eps(other.eps), regularity(other.regularity) {}

RealFunctionR2::RealFunctionR2(RealFunctionR2 &&other) noexcept: _f(std::move(other._f)), _df(std::move(other._df)), eps(other.eps), regularity(other.regularity) {}

RealFunctionR2 & RealFunctionR2::operator=(const RealFunctionR2 &other) { _f = other._f; _df = other._df; eps = other.eps; regularity = other.regularity; return *this; }

RealFunctionR2 & RealFunctionR2::operator=(RealFunctionR2 &&other) noexcept { _f = std::move(other._f); _df = std::move(other._df); eps = other.eps; regularity = other.regularity; return *this; }

RealFunctionR2::RealFunctionR2(std::function<float(vec2)> f, std::function<vec2(vec2)> df, float eps, Regularity regularity): _f(f), _df(df), eps(eps), regularity(regularity) {}

RealFunctionR2::RealFunctionR2(std::function<float(vec2)> f, std::function<float(vec2)> dfdx, std::function<float(vec2)> dfdy, float eps, Regularity regularity): RealFunctionR2(f, [dfdx, dfdy](vec2 x) { return vec2(dfdx(x), dfdy(x)); }, eps, regularity) {}

RealFunctionR2::RealFunctionR2(std::function<float(vec2)> f, float epsilon, Regularity regularity): _f(f), _df(derivativeOperator(f, epsilon)), eps(epsilon), regularity(regularity) {}

float RealFunctionR2::operator()(vec2 v) const {
	return _f(v);
}

vec2 RealFunctionR2::df(vec2 v) const {
	return _df(v);
}

float RealFunctionR2::operator()(float x, float y) const {
	return (*this)(vec2(x, y));
}

vec2 RealFunctionR2::df(float x, float y) const { return _df(vec2(x, y)); }

RealFunctionR2 RealFunctionR2::operator-() const { return RealFunctionR2([f=_f](vec2 v) { return -f(v); }, [df=_df](vec2 v) { return -df(v); }, eps, regularity); }

RealFunctionR2 RealFunctionR2::operator*(float a) const { return RealFunctionR2([f=_f, a](vec2 v) { return f(v) * a; }, [df=_df, a](vec2 v) { return df(v) * a; }, eps, regularity); }

RealFunctionR2 RealFunctionR2::operator+(float a) const { return RealFunctionR2([f=_f, a](vec2 v) { return f(v) + a; }, _df, eps, regularity); }

RealFunctionR2 RealFunctionR2::operator-(float a) const { return RealFunctionR2([f=_f, a](vec2 v) { return f(v) - a; }, _df, eps, regularity); }

RealFunctionR2 RealFunctionR2::operator/(float a) const { return RealFunctionR2([f=_f, a](vec2 v) { return f(v) / a; }, [df=_df, a](vec2 v) { return df(v) / a; }, eps, regularity); }

RealFunctionR2 RealFunctionR2::operator+(const RealFunctionR2 &g) const { return RealFunctionR2([f=_f, g_=g._f](vec2 v) { return f(v) + g_(v); }, [df=_df, dg=g._df](vec2 v) { return df(v) + dg(v); }, eps, regularity); }

RealFunctionR2 RealFunctionR2::operator-(const RealFunctionR2 &g) const { return RealFunctionR2([f=_f, g_=g._f](vec2 v) { return f(v) - g_(v); }, [df=_df, dg=g._df](vec2 v) { return df(v) - dg(v); }, eps, regularity); }

RealFunctionR2 RealFunctionR2::operator*(const RealFunctionR2 &g) const { return RealFunctionR2([f=_f, g_=g._f](vec2 v) { return f(v) * g_(v); }, [df=_df, f=_f, g](vec2 v) { return df(v) * g(v) + f(v) * g.df(v); }, eps, regularity); }

RealFunctionR2 RealFunctionR2::operator/(const RealFunctionR2 &g) const { return RealFunctionR2([f=_f, g_=g._f](vec2 v) { return f(v) / g_(v); }, [df=_df, f=_f, g](vec2 v) { return (df(v) * g(v) - f(v) * g.df(v)) / (g(v) * g(v)); }, eps, regularity); }

RealFunctionR2 operator*(float a, const RealFunctionR2 &f) { return f * a; }

RealFunctionR2 operator+(float a, const RealFunctionR2 &f) { return f + a; }

RealFunctionR2 operator-(float a, const RealFunctionR2 &f) { return -f + a; }

RealFunctionR2 operator/(float a, const RealFunctionR2 &f) {return RealFunctionR2([f, a](vec2 v) { return a / f(v); }, [f, a](vec2 v) { return -a * f.df(v) / (f(v) * f(v)); }, f.eps, f.regularity); }

RealFunctionR2 RealFunctionR2::operator~() const { return RealFunctionR2([f=_f](vec2 v) { return 1.f/f(v); }, eps, regularity); }

RealFunctionR2 RealFunctionR2::pow(float a) const { return RealFunctionR2([f=_f, a](vec2 v) { return std::pow(f(v), a); }, [df=_df, a, f=_f](vec2 v) { return a * std::pow(f(v), a - 1) * df(v); }, eps, regularity); }

RealFunctionR2 RealFunctionR2::sqrt() const {return pow(0.5f);}

float RealFunctionR2::getEps() const { return eps; }

float RealFunctionR2::dx(vec2 x) const { return _df(x).x; }

float RealFunctionR2::dy(vec2 x) const { return _df(x).y; }

RealFunctionR2 RealFunctionR2::dxi_yj(int i, int j) const {
	if (i+j == 0 && i*j == 0) return *this;
	if (i + j > 1 && i > 0) return dx().dxi_yj(i-1, j);
	if (i + j > 1 && j > 0) return dy().dxi_yj(i, j-1);
	if (i + j == 1 && i > 0) return dx();
	if (i + j == 1 && j > 0) return dy();
	throw std::runtime_error("dxi_yj: i+j must be nonegative");
}


RealFunction RealFunctionR2::partially_evaulate(int variable_ind, float value) const {
	if (variable_ind == 0)
			return RealFunction(
			[this, value](float y) { return (*this)(value, y); },
			[this, value](float y) { return dy(value, y); },
			[this, value](float y) { return dyy(value, y); }, eps, regularity);
	return RealFunction(
		[this, value](float x) { return (*this)(x, value); },
		[this, value](float x) { return dx(x, value); },
		[this, value](float x) { return dxx(x, value); }, eps, regularity);
}

RealFunction RealFunctionR2::partially_integrate_along_x(float x0, float x1, int precision) const {
	auto f_ev = uncurry_perm<float, float>(unwrap(_f));
	return RealFunction(
	[f_ev, x0, x1, precision](float y) {
			auto fy = f_ev(y);
			return mean(mapLinspace(x0, x1, precision, fy, true));
	}, eps, regularity);
}

RealFunction RealFunctionR2::partially_integrate_along_y(float y0, float y1, int precision) const {
	auto f_ev = uncurry<float, float>(unwrap(_f));
	return RealFunction(
	[f_ev, y0, y1, precision](float x) {
			auto fx = f_ev(x);
			return mean(mapLinspace(y0, y1, precision, fx, true));
	}, eps, regularity);
}

RealFunctionR2 RealFunctionR2::convolve_x(const RealFunction &g, float L, int precision) const {
	return RealFunctionR2([F=_f, g=g, L, precision](float x, float y) {
		return RealFunction([F, g, L, x, y](float s) {
			return F(vec2(x-s, y)) * g(s);
		}).integral(-L, L, precision);
	}, eps, regularity);
}

RealFunctionR2 RealFunctionR2::convolve_y(const RealFunction &g, float L, int precision) const {
	return RealFunctionR2([F=_f, g, L, precision](float x, float y) {
		return RealFunction([F, g, L, x, y](float s) {
			return F(vec2(x, y-s)) * g(s);
		}).integral(-L, L, precision);
	}, eps, regularity);
}

float RealFunctionR2::integrate_rect(vec2 a, vec2 b, int precision) const {
	return partially_integrate_along_x(a.x, b.x, precision).integral(a.y, b.y, precision);
}


RealFunctionR2 RealFunctionR2::linear(vec2 v) { return RealFunctionR2([v](vec2 x) { return dot(v, x); }, [v](vec2 x) { return v; }); }

RealFunctionR2 RealFunctionR2::constant(float a) { return RealFunctionR2([a](vec2 x) { return a; }, [a](vec2 x) { return vec2(0); }); }

RealFunctionR2 RealFunctionR2::projection(int i) { return RealFunctionR2([i](vec2 x) { return x[i]; }, [i](vec2 x) { return vec2(i == 0, i == 1); }); }


template<typename A, typename B, typename C>
std::function<C(A)> compose(const std::function<C(B)> &f, const std::function<B(A)> &g) {
	return [f, g](A x) { return f(g(x)); };
}

RealFunctionR3::RealFunctionR3(RealFunctionR3 &&other) noexcept
: _f(std::move(other._f)),
  _df(std::move(other._df)),
  eps(other.eps),
  regularity(other.regularity) {}

RealFunctionR3 &RealFunctionR3::operator=(const RealFunctionR3 &other) {
	if (this == &other)
		return *this;
	_f = other._f;
	_df = other._df;
	eps = other.eps;
	regularity = other.regularity;
	return *this;
}

RealFunctionR3 &RealFunctionR3::operator=(RealFunctionR3 &&other) noexcept {
	if (this == &other)
		return *this;
	_f = std::move(other._f);
	_df = std::move(other._df);
	eps = other.eps;
	regularity = other.regularity;
	return *this;
}

float RealFunctionR3::operator()(vec3 v) const {
	return _f(v);
}

vec3 RealFunctionR3::df(vec3 v) const {
	return _df(v);
}


RealFunctionR3 RealFunctionR3::operator*(float a) const {
	return RealFunctionR3([this, a](vec3 v) {
							  return this->_f(v) * a;
						  },
						  [this, a](vec3 v) {
							  return this->_df(v) * a;
						  });
}

RealFunctionR3 RealFunctionR3::operator+(float a) const {
	return RealFunctionR3([this, a](vec3 v) {
							  return this->_f(v) + a;
						  },
						  [this](vec3 v) {
							  return this->_df(v);
						  });
}

RealFunctionR3 RealFunctionR3::operator-(float a) const {
	return RealFunctionR3([this, a](vec3 v) {
							  return this->_f(v) - a;
						  },
						  [this](vec3 v) {
							  return this->_df(v);
						  });
}

RealFunctionR3 RealFunctionR3::operator/(float a) const {
	return RealFunctionR3([this, a](vec3 v) {
							  return this->_f(v) / a;
						  }, [this, a](vec3 v) {
							  return this->_df(v) / a;
						  });
}

RealFunctionR3 RealFunctionR3::operator+(const RealFunctionR3 &g) const {
	return RealFunctionR3([f=_f, g_=g](vec3 x) {
							  return f(x) + g_(x);
						  },
						  [df=_df, g_=g](vec3 x) {
							  return df(x) + g_.df(x);
						  });
}


RealFunctionR3 RealFunctionR3::operator*(const RealFunctionR3 &g) const {
	return RealFunctionR3([f=_f, g_=g](vec3 x) {
							  return f(x) * g_(x);
						  },
						  [f=_f, df=_df, g_=g](vec3 x) {
							  return f(x) * g_.df(x) + df(x) * g_(x);
						  });
}

RealFunctionR3 RealFunctionR3::operator/(const RealFunctionR3 &g) const {
	return RealFunctionR3([f=_f, g_=g](vec3 x) {
							  return f(x) / g_(x);
						  },
						  [f=_f, df=_df, g_=g](vec3 x) {
							  return (f(x) * g_.df(x) - df(x) * g_(x)) / (g_(x) * g_(x));
						  });
}

RealFunctionR3 RealFunctionR3::linear(vec3 v) {
	return RealFunctionR3([v](vec3 x) {
							  return dot(x, v);
						  },
						  [v](vec3 x) {
							  return v;
						  });
}

RealFunctionR3 RealFunctionR3::projection(int i) {
	return RealFunctionR3([i](vec3 x) {
							  return x[i];
						  }, [](vec3) {
							  return vec3(0, 0, 0);
						  });
}

SpaceEndomorphism &SpaceEndomorphism::operator=(const SpaceEndomorphism &other) {
	if (this == &other)
		return *this;
	_f = other._f;
	_df = other._df;
	return *this;
}

SpaceEndomorphism &SpaceEndomorphism::operator=(SpaceEndomorphism &&other) noexcept {
	if (this == &other)
		return *this;
	_f = std::move(other._f);
	_df = std::move(other._df);
	return *this;
}


RealFunctionR3 RealFunctionR3::constant(float a) {
	return RealFunctionR3([a](vec3 x) {
							  return a;
						  },
						  [a](vec3 x) {
							  return vec3(0, 0, 0);
						  });
}




RealFunction::RealFunction(const RealFunction &other)
: _f(other._f),
  _df(other._df),
  _ddf(other._ddf),
  eps(other.eps) {}

RealFunction::RealFunction(RealFunction &&other) noexcept
: _f(std::move(other._f)),
  _df(std::move(other._df)),
  _ddf(std::move(other._ddf)),
  eps(other.eps) {}

RealFunction &RealFunction::operator=(const RealFunction &other) {
	if (this == &other)
		return *this;
	_f = other._f;
	_df = other._df;
	_ddf = other._ddf;
	eps = other.eps;
	return *this;
}

RealFunction &RealFunction::operator=(RealFunction &&other) noexcept {
	if (this == &other)
		return *this;
	_f = std::move(other._f);
	_df = std::move(other._df);
	_ddf = std::move(other._ddf);
	eps = other.eps;
	return *this;
}

RealFunction RealFunction::operator+(const RealFunction &g) const {
	return RealFunction([f=_f, g](float x) {
							return f(x) + g(x);
						},
						[df=_df, dg=g._df](float x) {
							return df(x) + dg(x);
						},
						[ddf=_ddf, ddg=g._ddf](float x) {
							return ddf(x) + ddg(x);
						});
}

RealFunction RealFunction::operator*(float a) const {
	return RealFunction([f=_f, a](float x) {
							return f(x) * a;
						},
						[df=_df, a](float x) {
							return df(x) * a;
						},
						[ddf=_ddf, a](float x) {
							return ddf(x) * a;
						});
}

RealFunction RealFunction::operator+(float a) const {
	return RealFunction([_f=_f, a](float x) {
							return _f(x) + a;
						},
						[_df=_df](float x) {
							return _df(x);
						},
						[_ddf=_ddf](float x) {
							return _ddf(x);
						});
}

RealFunction RealFunction::operator*(const RealFunction &g_) const {
	return RealFunction(
						[f=_f, g=g_._f](float x) {
							return f(x) * g(x);
						},
						[f=_f, g=g_._f, df=_df, dg=g_._df](float x) {
							return f(x) * dg(x) + g(x) * df(x);
						},
						[f=_f, g=g_._f, df=_df, dg=g_._df, ddf=_ddf, ddg=g_._ddf](float x) {
							return f(x) * ddg(x) + 2 * df(x) * dg(x) + g(x) * ddf(x);
						}, eps);
}

RealFunction RealFunction::operator/(const RealFunction &g_) const {
	return RealFunction(
						[f=_f, g=g_._f](float x) {
							return f(x) / g(x);
						},
						[f=_f, g=g_._f, df=_df, dg=g_._df](float x) {
							return (f(x) * dg(x) - g(x) * df(x)) / (g(x) * g(x));
						},
						[f=_f, g=g_._f, df=_df, dg=g_._df, ddf=_ddf, ddg=g_._ddf](float x) {
							return (ddf(x) * g(x) * g(x) - 2 * df(x) * dg(x) * g(x) + f(x) * ddg(x) * g(x) * g(x) - f(x) * g(x) * g(x) * g(x) * g(x)) / (g(x) * g(x) * g(x) * g(x));
						}, eps);
}

RealFunction RealFunction::operator&(const RealFunction &g_) const {
	return RealFunction([f=_f, g=g_._f](float x) {
							return f(g(x));
						},
						[df=_df, dg=g_._df, g=g_._f](float x) {
							return df(g(x)) * dg(x);
						},
						[ddf=_ddf, ddg=g_._ddf, dg=g_._df, g=g_._f, df=_df](float x) {
							return ddf(g(x)) * dg(x) * dg(x) + df(g(x)) * ddg(x);
						});
}

RealFunctionR3 RealFunction::operator&(const RealFunctionR3 &g_) const {
	return RealFunctionR3([f=*this, g=g_](vec3 v) {
		return f(g(v));
	}, eps);
}

RealFunction RealFunction::operator&(float a) const { return (*this) & (x()*a); }

RealFunction RealFunction::operator()(const RealFunction &g_) const { return (*this) & g_; }

float RealFunction::L2_norm(vec2 I, int prec) const {
	return pow2().integral(I[0], I[1], prec);
}

float RealFunction::L2_product(const RealFunction &g, vec2 I, int prec) const {
	return (*this * g).integral(I[0], I[1], prec);
}

RealFunction RealFunction::convolve(CompactlySupportedRealFunction kernel, int prec) const {
	return kernel.convolve(*this, prec);
}

RealFunction RealFunction::convolve(const RealFunction &kernel, float L, int prec) const {
	auto K = RealFunctionR2 ([f=_f, kernel=kernel](vec2 v) {
			return kernel(v.x-v.y) * f(v.y);
		}, eps, regularity + kernel.regularity);
	return K.partially_integrate_along_y(-L, L, prec);
}

float RealFunction::repeated_integral(float a, float b, int n, int prec) const {
	auto P = RealFunction([n, b](float t){return std::pow(b-t, n-1)/factorial(n-1);}, eps, regularity);
	return L2_product(P, vec2(a, b), prec);
}

RealFunction RealFunction::repeated_antiderivative(float a, int prec) const { return RealFunction([this, a, prec](float x) { return this->repeated_integral(a, x, 1, prec); }, eps, regularity); }


RealFunction RealFunction::monomial(float n) {
	if (n == 0) return constant(1);
	return RealFunction([n](float x) {
							return std::pow(x, n);
						}, [n](float x) {
							return n * std::pow(x, n - 1);
						}, [n](float x) {
							return n * (n - 1) * std::pow(x, n - 2);
						});
}

RealFunction RealFunction::polynomial(std::vector<float> coeffs) {
	if (coeffs.size() == 1) return constant(coeffs[0]);
	std::vector<float> c_lower = rangeFrom(coeffs, 1);
	int degree = coeffs.size() - 1;
	return monomial(degree) * coeffs[0] + polynomial(c_lower);
}

RealLineAutomorphism RealLineAutomorphism::operator&(const RealLineAutomorphism &g) const {
	return RealLineAutomorphism(static_cast<RealFunction>(*this) & static_cast<RealFunction>(g), g.inverse & inverse);
}

RealLineAutomorphism RealLineAutomorphism::pow(int n) {
	if (n % 2 == 0) throw IllegalArgumentError("Only odd degree monomials are invertible", __FILE__, __LINE__);
	return RealLineAutomorphism(RealFunction::monomial(n), RealFunction::monomial(1.f / n));
}

PlaneSmoothEndomorphism::PlaneSmoothEndomorphism() {
	_f = make_shared<std::function<vec2(vec2)> >([](vec2 v) {
		return v;
	});
	_df = make_shared<std::function<mat2(vec2)> >([](vec2 v) {
		return mat2(1, 0, 0, 1);
	});
}

PlaneSmoothEndomorphism::PlaneSmoothEndomorphism(shared_ptr<std::function<vec2(vec2)> > f, shared_ptr<std::function<mat2(vec2)> > df) {
	_f = f;
	_df = df;
}

vec2 PlaneSmoothEndomorphism::operator()(vec2 x) const {
	return (*_f)(x);
}

vec2 PlaneSmoothEndomorphism::df(vec2 x, vec2 v) const {
	return (*_df)(x) * v;
}

mat2 PlaneSmoothEndomorphism::df(vec2 x) const {
	return (*_df)(x);
}


PlaneAutomorphism::PlaneAutomorphism(shared_ptr<std::function<vec2(vec2)> > f, shared_ptr<std::function<mat2(vec2)> > df,
									 shared_ptr<std::function<vec2(vec2)> > f_inv)
: PlaneSmoothEndomorphism(f, df) {
	_f_inv = f_inv;
}

vec2 PlaneAutomorphism::f_inv(vec2 x) const {
	return (*_f_inv)(x);
}

PlaneAutomorphism PlaneAutomorphism::operator~() const {
	auto df_cpy = _df;
	auto f_inv_cpy = _f_inv;
	auto df_inv = [df_cpy, f_inv_cpy](vec2 x) {
		return inverse((*df_cpy)((*f_inv_cpy)(x)));
	};
	return PlaneAutomorphism(f_inv_cpy, make_shared<std::function<mat2(vec2)> >(df_inv), _f);
}

RealFunction expStep(int exponentDegree, float center) {
	if (exponentDegree < 1) throw IllegalArgumentError("Exponent should be positive (and at least 2).", __FILE__, __LINE__);
	if (exponentDegree < 2) throw IllegalArgumentError("Step with exponent 1 is not differentiable at zero.", __FILE__, __LINE__);
	return RealFunction([exponentDegree, center](float t) {
		if (t < center) return 1.f;
		return (float) exp(-1.f * exp(exponentDegree) * pow(t, exponentDegree));
	});
}

RealFunction flatFunctionStep(float center) {
	return RealFunction([ center](float t) {
		return t > center ? exp(-1 / (t - center)) : 0;
	});
}

RealFunction flatAtRoofShroom(float center) {
	return RealFunction([ center](float t) {
		return 1 - exp(-1 / sq(t - center));
	});
}




SpaceEndomorphism::SpaceEndomorphism(std::function<vec3(vec3)> f, float epsilon) {
	_f = f;
	_df = [f, epsilon](vec3 x) {
		return mat3(
					(f(x + vec3(epsilon, 0, 0)) - f(x - vec3(epsilon, 0, 0))) / (2 * epsilon),
					(f(x + vec3(0, epsilon, 0)) - f(x - vec3(0, epsilon, 0))) / (2 * epsilon),
					(f(x + vec3(0, 0, epsilon)) - f(x - vec3(0, 0, epsilon))) / (2 * epsilon)
				   );
	};
}


SpaceEndomorphism SpaceEndomorphism::compose(const SpaceEndomorphism &g) const {
	return SpaceEndomorphism([f=_f, g](vec3 v) {
								 return f(g(v));
							 },
							 [d=_df, g](vec3 x) {
								 return d(g(x)) * g.df(x);
							 });
}

SpaceEndomorphism SpaceEndomorphism::translation(vec3 v) {
	return affine(mat3(1), v);
}

SpaceEndomorphism SpaceEndomorphism::scaling(float x, float y, float z) {
	return linear(mat3(x, 0, 0, 0, y, 0, 0, 0, z));
}

SpaceEndomorphism SpaceEndomorphism::affine(mat3 A, vec3 v) {
	return SpaceEndomorphism([A, v](vec3 x) {
								 return A * x + v;
							 },
							 [A](vec3 x) {
								 return A;
							 });
}

SpaceAutomorphism SpaceAutomorphism::linear(mat3 A) {
	return SpaceAutomorphism([A](vec3 v) {
								 return A * v;
							 },
							 [A](vec3 v) {
								 return inverse(A) * v;
							 },
							 [A](vec3 x) {
								 return A;
							 });
}

SpaceAutomorphism SpaceAutomorphism::translation(vec3 v) {
	return SpaceAutomorphism([v](vec3 x) {
								 return x + v;
							 },
							 [v](vec3 x) {
								 return x - v;
							 },
							 [](vec3 x) {
								 return mat3(1);
							 });
}

SpaceAutomorphism SpaceAutomorphism::scaling(float x, float y, float z) {
	return SpaceAutomorphism([x, y, z](vec3 v) {
								 return vec3(v.x * x, v.y * y, v.z * z);
							 },
							 [x, y, z](vec3 v) {
								 return vec3(v.x / x, v.y / y, v.z / z);
							 },
							 [x, y, z](vec3 v) {
								 return mat3(x, 0, 0, 0, y, 0, 0, 0, z);
							 });
}


SpaceAutomorphism SpaceAutomorphism::affine(mat3 A, vec3 v) {
	return SpaceAutomorphism([A, v](vec3 x) {
								 return A * x + v;
							 }, [A, v](vec3 x) {
								 return inverse(A) * (x - v);
							 },
							 [A](vec3 x) {
								 return A;
							 });
}

SpaceAutomorphism SpaceAutomorphism::rotation(float angle) {
	return linear(rotationMatrix3(angle));
}


SpaceAutomorphism SpaceAutomorphism::rotation(vec3 axis, float angle) {
	vec3 n = normalize(axis);
	float nx = n.x;
	float ny = n.y;
	float nz = n.z;
	return linear(rotationMatrix3(n, angle));
}




ScalarField::ScalarField()
: F([](vec3 x, float t) {
	  return 0;
  }),
  eps(0.01) {}

ScalarField::ScalarField(std::function<float(vec3, float)> F, float eps)
: F(F),
  eps(eps) {}

ScalarField::ScalarField(std::function<SteadyScalarField(float)> pencil)
: F([p=pencil](vec3 x, float t) {
	  return p(t)(x);
  }),
  eps(pencil(0).getEps()) {}

ScalarField::ScalarField(SteadyScalarField steady_field)
: F([steady_field](vec3 x, float t) {
	  return steady_field(x);
  }),
  eps(steady_field.getEps()) {}

float ScalarField::operator()(vec3 x, float t) const { return F(x, t); }

SteadyScalarField ScalarField::operator()(float t) const { return SteadyScalarField([F=F, t](vec3 x) { return F(x, t); }, eps); }

SteadyScalarField ScalarField::fix_time(float t) const { return operator()(t); }

ScalarField ScalarField::operator+(const ScalarField &Y) const { return ScalarField([F=F, Y=Y.F](vec3 x, float t) { return F(x, t) + Y(x, t); }, eps); }

ScalarField ScalarField::operator*(float a) const { return ScalarField([F=F, a=a](vec3 x, float t) { return F(x, t) * a; }, eps); }

ScalarField ScalarField::operator-() const { return *this * -1; }

ScalarField ScalarField::operator-(const ScalarField &Y) const { return *this + (-Y); }

ScalarField ScalarField::operator*(const SteadyScalarField &f) const { return ScalarField([F=F, f=f](vec3 x, float t) { return F(x, t) * f(x); }, eps); }

ScalarField ScalarField::operator/(const SteadyScalarField &f) const { return ScalarField([F=F, f=f](vec3 x, float t) { return F(x, t) * f(x); }, eps); }

ScalarField ScalarField::operator*(const ScalarField &f) const { return ScalarField([F=F, f=f](vec3 x, float t) { return F(x, t) * f(x, t); }, eps); }

ScalarField ScalarField::operator/(const ScalarField &f) const { return ScalarField([F=F, f=f](vec3 x, float t) { return F(x, t) * f(x, t); }, eps); }

ScalarField ScalarField::time_derivative() const {
	return ScalarField([F=F, e=eps](vec3 x, float t) {
		return derivativeOperator(curry(F)(x), e)(t);
	}, eps);
}

ScalarField ScalarField::spatial_partial(int i) const {
	return ScalarField([F=F, e=eps](vec3 x, float t) {
		return derivativeOperator(curry(F)(x), e)(t);
	}, eps);
}






	// 	Flow spatial_partial_derivative(int i) const {
	// 		return Flow([F_=F, i, e=eps](vec3 x, float t){
	// 			return partialDerivativeOperator([F=F_, i=i, t](vec3 y) {
	// 				return F(y, t);
	// 			}, i, e);
	// }
	// 	}



SpaceAutomorphism::SpaceAutomorphism(std::function<vec3(vec3)> f, std::function<vec3(vec3)> f_inv, std::function<mat3(vec3)> df)
: SpaceEndomorphism(f, df) {
	this->_f_inv = f_inv;
}

SpaceAutomorphism::SpaceAutomorphism(std::function<vec3(vec3)> f, std::function<vec3(vec3)> f_inv, float epsilon)
: SpaceEndomorphism(f, epsilon) {
	this->_f_inv = f_inv;
}

vec3 SpaceAutomorphism::inv(vec3 v) const {
	return _f_inv(v);
}

SpaceAutomorphism SpaceAutomorphism::operator~() const {
	return SpaceAutomorphism(_f_inv, _f, [d=_df](vec3 x) {
		return inverse(d(x));
	});
}

SpaceAutomorphism SpaceAutomorphism::compose(SpaceAutomorphism g) const {
	return SpaceAutomorphism([f = _f, g](vec3 v) {
								 return f(g(v));
							 }, [f_inv = _f_inv, g](vec3 v) {
								 return g.inv(f_inv(v));
							 },
							 [d = _df, g](vec3 x) {
								 return d(g(x)) * g.df(x);
							 });
}

DiscreteRealFunction loadSequence(const CodeFileDescriptor &file, vec2 domain) {
	string code = file.readCode();
	vector<string> lines = split(code, '\n');
	vector<float> values;
	for (const auto &line: lines) {
		if (line.empty()) continue;
		if (line[0] == '#') continue;
		if (line[0] == '/') continue;
		if (line[0] == ' ') continue;
		if (line[0] == '\t') continue;
		values.push_back(stof(line));
	}
	return DiscreteRealFunction(values, domain);
}



float smoothstep(float x0, float x1, float x) {
	if (x < x0) return 0;
	if (x > x1) return 1;
	float t = (x - x0) / (x1 - x0);
	return t * t * (3 - 2 * t);
}

float smootherstep(float x0, float x1, float x) {
	if (x < x0) return 0;
	if (x > x1) return 1;
	float t = (x - x0) / (x1 - x0);
	return pow5(t)*6-15*pow4(t)+10*pow3(t);
}
RealFunction smoothstep(float x0, float x1) {
	return RealFunction([x0, x1](float x) {
		return smoothstep(x0, x1, x);
	}, 0.01);
}

RealFunction smootherstep(float x0, float x1) {
	return RealFunction([x0, x1](float x) {
		return smootherstep(x0, x1, x);
	}, 0.01);
}

RealFunctionR2 smoothUnion(float k) {
	return RealFunctionR2([k](float x, float y) {
		float h = ::clamp( 0.5 + 0.5*(y-x)/k, 0.0, 1.0 );
		return glm::mix( y, x, h ) - k*h*(1.0-h);
	});
}

RealFunctionR2 smoothIntersect(float k) {
	return RealFunctionR2([k](float x, float y) {
		float h = clamp( 0.5 - 0.5*(y-x)/k, 0.0, 1.0 );
		return glm::mix( y, x, h ) + k*h*(1.0-h);
	});
}

RealFunctionR2 smoothSubtract(float k) {
	return RealFunctionR2([k](float x, float y) {
		float h = clamp( 0.5 - 0.5*(y+x)/k, 0.0, 1.0 );
		return glm::mix( y, -x, h ) + k*h*(1.0-h);
	});
}
