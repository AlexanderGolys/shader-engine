#include "planarGeometry.hpp"
// #include "glm/glm.hpp"
#include "../utils/func.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;

SmoothParametricPlaneCurve::SmoothParametricPlaneCurve(Foo12 f, std::function<vec2(float)> df,
                                                       Foo12 ddf, float t0, float t1, bool period, float epsilon) {
    this->_f = f;
    this->t0 = t0;
    this->t1 = t1;
    this->periodic = period;
    this->eps = epsilon;
    this->_df = df;
    this->_ddf =ddf;
}
SmoothParametricPlaneCurve::SmoothParametricPlaneCurve(const SmoothParametricPlaneCurve &other) :
    _f(other._f), _df(other._df), _ddf(other._ddf), _der_higher(other._der_higher), eps(other.eps), t0(other.t0), t1(other.t1),
    periodic(other.periodic) {}
SmoothParametricPlaneCurve::SmoothParametricPlaneCurve(SmoothParametricPlaneCurve &&other) noexcept :
    _f(std::move(other._f)), _df(std::move(other._df)), _ddf(std::move(other._ddf)), _der_higher(std::move(other._der_higher)),
    eps(other.eps), t0(std::move(other.t0)), t1(std::move(other.t1)), periodic(other.periodic) {}
SmoothParametricPlaneCurve &SmoothParametricPlaneCurve::operator=(const SmoothParametricPlaneCurve &other) {
    if (this == &other)
        return *this;
    _f = other._f;
    _df = other._df;
    _ddf = other._ddf;
    _der_higher = other._der_higher;
    eps = other.eps;
    t0 = other.t0;
    t1 = other.t1;
    periodic = other.periodic;
    return *this;
}
SmoothParametricPlaneCurve &SmoothParametricPlaneCurve::operator=(SmoothParametricPlaneCurve &&other) noexcept {
    if (this == &other)
        return *this;
    _f = std::move(other._f);
    _df = std::move(other._df);
    _ddf = std::move(other._ddf);
    _der_higher = std::move(other._der_higher);
    eps = other.eps;
    t0 = std::move(other.t0);
    t1 = std::move(other.t1);
    periodic = other.periodic;
    return *this;
}

SmoothParametricPlaneCurve::SmoothParametricPlaneCurve( std::function<vec2(float)> f,
                                                        const std::function<vec2(float)>& df, float t0, float t1,
                                                        bool period, float epsilon) :
                            SmoothParametricPlaneCurve(f, df, derivativeOperator(df, epsilon), t0, t1, period, epsilon) {}



SmoothParametricPlaneCurve::SmoothParametricPlaneCurve( const std::function<vec2(float)>& curve, float t0, float t1,
                                                        bool period, float epsilon) :
                            SmoothParametricPlaneCurve(curve, derivativeOperator(curve, epsilon), t0, t1, period, epsilon) {}


SmoothParametricPlaneCurve SmoothParametricPlaneCurve::operator+(vec2 v) const {
    return SmoothParametricPlaneCurve([f=_f, v](float t){return f(t)+v;}, _df, _ddf, t0.value(), t1.value(), periodic, eps);
}

SmoothParametricPlaneCurve SmoothParametricPlaneCurve::operator-(vec2 v) const {
    return SmoothParametricPlaneCurve([f=_f, v](float t){return f(t)-v;}, _df, _ddf, t0.value(), t1.value(), periodic, eps);
}

vector<vec2> SmoothParametricPlaneCurve::sample(float t0, float t1, int n) const {
	vector<vec2> result = std::vector<vec2>();
	result.reserve(n);
	float dt = (t1 - t0) / (n-1);
	for (int i = 0; i < n; i++)
	{
		result.push_back(_f(t0 + dt*i));
	}
	return result;
}

vector<vec3> SmoothParametricPlaneCurve::adjacency_lines_buffer(float t0,
                                                                float t1, int n,
                                                                float z) const {
  vector<vec3> result = vector<vec3>();
  result.reserve(4 * n - 4);
  float dt = (t1 - t0) / n;
  for (int i = 1; i < n; i++) {
    vec3 p1 = vec3(_f(t0 + (i - 1) * dt), z);
    vec3 p2 = vec3(_f(t0 + i * dt), z);
    vec3 p0 = p1 + vec3(normal(t0 + (i - 1) * dt), 0);
    vec3 p3 = p2 + vec3(normal(t0 + i * dt), 0);
    result.push_back(p0);
    result.push_back(p1);
    result.push_back(p2);
    result.push_back(p3);
  }
  return result;
}
