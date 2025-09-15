#pragma once

#include <cmath>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include "file-management/filesUtils.hpp"
#include "mat.hpp"
#include "randomUtils.hpp"


std::string polyGroupIDtoString(PolyGroupID id);
PolyGroupID prefix(const PolyGroupID &id, const std::string& prefix);


PolyGroupID make_unique_id(const PolyGroupID &id);
PolyGroupID bdGroup(int n);
PolyGroupID curveGroup(int n);
PolyGroupID randomCurvaID();
PolyGroupID randomID();

PolyGroupID randomBdID();
bool idBelongsToCurve(const PolyGroupID &id);
bool idBelongsToBd(const PolyGroupID &id);

class RealFunctionR3;
typedef RealFunctionR3 SteadyScalarField;










enum class Regularity {
	PATHOLOGICAL=-99,
    UNKNOWN = -2,
	DISCONTINUOUS=-1,
	C0 = 0,
	C1 = 1,
	C2 = 2,
	C3 = 3,
	C4 = 4,
    C5 = 5,
	POSSIBLY_FINITE_REGULARITY_AT_LEAST_C6 = 6,
	SMOOTH = 69,
    ANALYTIC = 70,
	CONFORMAL= 71,
	MEROMORPHIC = 420,
	HOLOMORPHIC = 2137
};

Regularity max(Regularity a, Regularity b);
Regularity min(Regularity a, Regularity b);
Regularity operator-(Regularity a, int b);
Regularity operator+(Regularity a, int b);
Regularity operator+(Regularity a, Regularity b);
Regularity operator--(Regularity a);
Regularity operator++(Regularity a);


template<typename codomain>
std::function<codomain(float)> derivativeOperator(std::function<codomain(float)> f, float epsilon);

template<typename  domain, typename  codomain>
std::function<codomain(domain)> directionalDerivativeOperator(std::function<codomain(domain)> f, domain v, float epsilon);

Foo31 partialDerivativeOperator(Foo31 f, int i, float epsilon);
Foo21 partialDerivativeOperator(Foo21 f, int i, float epsilon);

Foo33 derivativeOperator(const Foo31 &f, float epsilon);
Foo22 derivativeOperator(const Foo21 &f, float epsilon);

Foo13 derivativeOperator(const Foo13 &f, float epsilon);
Foo12 derivativeOperator(const Foo12 &f, float epsilon);


class VectorField;
class RealFunction;
class RealFunctionR3;

class RealFunctionR2 {
	Foo21 _f;
	Foo22 _df;
    float eps = 0.01;
    Regularity regularity;
public:
	RealFunctionR2();
    RealFunctionR2(const RealFunctionR2 &other);
    RealFunctionR2(RealFunctionR2 &&other) noexcept;
    RealFunctionR2 & operator=(const RealFunctionR2 &other);
	RealFunctionR2 & operator=(RealFunctionR2 &&other) noexcept;
	RealFunctionR2(Foo21 f, Foo22 df, float eps=.01, Regularity regularity = Regularity::SMOOTH);
	RealFunctionR2(Foo21 f, Foo21 dfdx, Foo21 dfdy, float eps=.01, Regularity regularity = Regularity::SMOOTH);
	explicit RealFunctionR2(Foo21 f, float epsilon=0.01, Regularity regularity = Regularity::SMOOTH);
	explicit RealFunctionR2(Foo111 f, float epsilon=0.01, Regularity regularity = Regularity::SMOOTH) : RealFunctionR2([f](vec2 x) { return f(x.x, x.y); }, epsilon, regularity) {}
	explicit RealFunctionR2(float constant, float epsilon=0.01, Regularity regularity = Regularity::SMOOTH) : RealFunctionR2([constant](vec2 x) { return constant; }, epsilon, regularity) {}

	float operator()(vec2 v) const;
	vec2 df(vec2 v) const;
	float operator()(float x, float y) const;
	vec2 df(float x, float y) const;


	RealFunctionR2 operator-() const;
	RealFunctionR2 operator*(float a) const;
	RealFunctionR2 operator+(float a) const;
	RealFunctionR2 operator-(float a) const;
	RealFunctionR2 operator/(float a) const;
	RealFunctionR2 operator+(const RealFunctionR2 &g) const;
	RealFunctionR2 operator-(const RealFunctionR2 &g) const;
	RealFunctionR2 operator*(const RealFunctionR2 &g) const;
	RealFunctionR2 operator/(const RealFunctionR2 &g) const;
	friend RealFunctionR2 operator*(float a, const RealFunctionR2 &f);
	friend RealFunctionR2 operator+(float a, const RealFunctionR2 &f);
	friend RealFunctionR2 operator-(float a, const RealFunctionR2 &f);
	friend RealFunctionR2 operator/(float a, const RealFunctionR2 &f);
	RealFunctionR2 operator~() const;
	RealFunctionR2 pow(float a) const;
	RealFunctionR2 sqrt() const;
	float getEps() const;

	float dx(vec2 x) const;
	float dx(float x, float y) const { return dx(vec2(x, y)); }
	RealFunctionR2 dx() const { return RealFunctionR2([F=*this](vec2 x) { return F.dx(x); }, eps, regularity-1); }
	float dy(vec2 x) const;
	float dy(float x, float y) const { return dy(vec2(x, y)); }
	RealFunctionR2 dy() const { return RealFunctionR2([this](vec2 x) { return dy(x); }, eps, regularity-1); }
	RealFunctionR2 dxi_yj(int i, int j) const;
	float dxi_yj(int i, int j, vec2 x) const { return dxi_yj(i, j)(x); }
	float dxi_yj(int i, int j, float x, float y) const { return dxi_yj(i, j, vec2(x, y)); }
	float dxx(vec2 x) const { return dxi_yj(2, 0, x); }
	float dxx(float x, float y) const { return dxx(vec2(x, y)); }
	float dyy(vec2 x) const { return dxi_yj(0, 2, x); }
	float dyy(float x, float y) const { return dyy(vec2(x, y)); }
	float dxy(vec2 x) const { return dxi_yj(1, 1, x); }
	float dxy(float x, float y) const { return dxy(vec2(x, y)); }
	float Laplacian(vec2 x) const { return dxx(x) + dyy(x); }

	float integrate_rect(vec2 a, vec2 b, int precision = 100) const;
	RealFunction partially_evaulate(int variable_ind, float value) const;
	RealFunction partially_integrate_along_x(float x0, float x1, int precision = 100) const;
	RealFunction partially_integrate_along_y(float x0, float x1, int precision = 100) const;
	RealFunctionR2 convolve_x(const RealFunction &g, float L, int precision = 100) const;
	RealFunctionR2 convolve_y(const RealFunction &g, float L, int precision = 100) const;

	static RealFunctionR2 linear(vec2 v);
	static RealFunctionR2 constant(float a);
	static RealFunctionR2 projection(int i);
};


class RealFunctionR3 {
	Foo31 _f;
	Foo33 _df;
    float eps = 0.01f;
    Regularity regularity;
public:
	RealFunctionR3();
    RealFunctionR3(const RealFunctionR3 &other);
    RealFunctionR3(RealFunctionR3 &&other) noexcept;
    RealFunctionR3 & operator=(const RealFunctionR3 &other);
    RealFunctionR3 & operator=(RealFunctionR3 &&other) noexcept;
    RealFunctionR3(Foo31 f,Foo33 df, float eps=.01f, Regularity regularity = Regularity::SMOOTH);;
	explicit RealFunctionR3(Foo31 f, float epsilon=0.01f, Regularity regularity = Regularity::SMOOTH);;
	RealFunctionR3(Foo1111 f,Foo1113 df, float eps=.01f, Regularity regularity = Regularity::SMOOTH);;
	explicit RealFunctionR3(Foo1111 f, float epsilon=0.01f, Regularity regularity = Regularity::SMOOTH);;

	float operator()(vec3 v) const;
	vec3 df(vec3 v) const;
	float operator()(float x, float y, float z) const;
	vec3 df(float x, float y, float z) const;
	RealFunctionR3 operator-() const;

	RealFunctionR3 operator*(float a) const;
    RealFunctionR3 operator+(float a) const;
    RealFunctionR3 operator-(float a) const;
    RealFunctionR3 operator/(float a) const;
	RealFunctionR3 operator+(const RealFunctionR3 &g) const;
	RealFunctionR3 operator-(const RealFunctionR3 &g) const;
	RealFunctionR3 operator*(const RealFunctionR3 &g) const;
	RealFunctionR3 operator/(const RealFunctionR3 &g) const;
    friend RealFunctionR3 operator*(float a, const RealFunctionR3 &f);
	friend RealFunctionR3 operator+(float a, const RealFunctionR3 &f);
	friend RealFunctionR3 operator-(float a, const RealFunctionR3 &f);
	friend RealFunctionR3 operator/(float a, const RealFunctionR3 &f);

	friend RealFunctionR3 min(const RealFunctionR3 &f, float a);
	friend RealFunctionR3 min(const RealFunctionR3 &f, const RealFunctionR3 &g);
	friend RealFunctionR3 min(float a, const RealFunctionR3 &f);
	friend RealFunctionR3 max(const RealFunctionR3 &f, float a);
	friend RealFunctionR3 max(const RealFunctionR3 &f, const RealFunctionR3 &g);
	friend RealFunctionR3 max(float a, const RealFunctionR3 &f);

	friend RealFunctionR3 precompose(Foo111 F, const RealFunctionR3 &g1, const RealFunctionR3 &g2);


	RealFunctionR3 operator~() const;
	RealFunctionR3 pow(float a) const;
	RealFunctionR3 sqrt() const;
	float getEps() const;

	VectorField gradient() const;
    RealFunctionR3 Laplacian() const;
    float dx (vec3 x) const;
	float dy (vec3 x) const;
	float dz (vec3 x) const;

	static RealFunctionR3 linear(vec3 v);
    static RealFunctionR3 constant(float a);
    static RealFunctionR3 projection(int i);
};

inline RealFunctionR3 pow(const RealFunctionR3 &f, float a) { return f.pow(a); }
inline RealFunctionR3 sqrt(const RealFunctionR3 &f) { return f.sqrt(); }
inline RealFunctionR3 pow2(const RealFunctionR3 &f) { return f * f; }
inline RealFunctionR3 pow3(const RealFunctionR3 &f) { return f * f * f; }
inline RealFunctionR3 pow4(const RealFunctionR3 &f) { return f * f * f * f; }



const auto X_R3 = RealFunctionR3::projection(0);
const auto Y_R3 = RealFunctionR3::projection(1);
const auto Z_R3 = RealFunctionR3::projection(2);
const auto NORM2_R3 = pow2(X_R3) + pow2(Y_R3) + pow2(Z_R3);
const auto NORM_R3 = sqrt(NORM2_R3);




// R3 -> R3
class SpaceEndomorphism {
protected:
	Foo33 _f;
	Foo3Foo33 _df;
    float eps = 0.01f;
	virtual SpaceEndomorphism compose(const SpaceEndomorphism &g) const;


public:
	virtual ~SpaceEndomorphism();

	SpaceEndomorphism(const SpaceEndomorphism &other);
  SpaceEndomorphism(SpaceEndomorphism &&other) noexcept;
  SpaceEndomorphism(Foo33 f, Foo3Foo33 df, float eps=.01);;
  explicit SpaceEndomorphism(Foo33 f, float epsilon=0.01);
  SpaceEndomorphism &operator=(const SpaceEndomorphism &other);
  SpaceEndomorphism &operator=(SpaceEndomorphism &&other) noexcept;
  explicit SpaceEndomorphism(mat3 A);
  explicit SpaceEndomorphism(mat4 A);

	vec3 directional_derivative(vec3 x, vec3 v) const;
	vec3 dfdv(vec3 x, vec3 v) const;
	mat3 df(vec3 x) const;
	vec3 operator()(vec3 x) const;
	virtual SpaceEndomorphism operator&(const SpaceEndomorphism &g) const;

	float getEps() const;

	static SpaceEndomorphism linear(const mat3 &A);
	static SpaceEndomorphism translation(vec3 v);
	static SpaceEndomorphism scaling(float x, float y, float z);
	static SpaceEndomorphism affine(mat3 A, vec3 v);
};




class ScalarField {
	BIHOM(vec3, float, float) F;
	float eps;
public:
	ScalarField();
	explicit ScalarField(BIHOM(vec3, float, float) F, float eps=.01);
	explicit ScalarField(HOM(float, SteadyScalarField) pencil);
	explicit ScalarField(SteadyScalarField steady_field);

	float operator()(vec3 x, float t) const;
	SteadyScalarField operator()(float t) const;
	SteadyScalarField fix_time(float t) const;

	ScalarField operator+(const ScalarField &Y) const;
	ScalarField operator*(float a) const;
	ScalarField operator-() const;
	ScalarField operator-(const ScalarField &Y) const;
	ScalarField operator*(const SteadyScalarField &f) const;
	ScalarField operator/(const SteadyScalarField &f) const;
	ScalarField operator*(const ScalarField &f) const;
	ScalarField operator/(const ScalarField &f) const;

	ScalarField time_derivative() const;
	ScalarField spatial_partial(int i) const;
};




	// 	Flow spatial_partial_derivative(int i) const {
// 		return Flow([F_=F, i, e=eps](vec3 x, float v){
// 			return partialDerivativeOperator([F=F_, i=i, v](vec3 y) {
// 				return F(y, v);
// 			}, i, e);
// }
// 	}

// aut(R3)
class SpaceAutomorphism : public SpaceEndomorphism {
	Foo33 _f_inv;
public:
	SpaceAutomorphism(Foo33 f, Foo33 f_inv, std::function<mat3(vec3)> df);
	SpaceAutomorphism(Foo33 f, Foo33 f_inv, float epsilon=0.01);

	vec3 inv(vec3 v) const;
	SpaceAutomorphism operator~() const;
	SpaceAutomorphism inv() const;
	SpaceAutomorphism compose(SpaceAutomorphism g) const;
    SpaceAutomorphism operator&(const SpaceAutomorphism &g) const;
	SpaceAutomorphism applyWithBasis(mat3 A) const;
	SpaceAutomorphism applyWithBasis(vec3 v1, vec3 v2, vec3 v3) const;
	SpaceAutomorphism applyWithShift(vec3 v) const;

	static SpaceAutomorphism linear(mat3 A);
    static SpaceAutomorphism translation(vec3 v);
    static SpaceAutomorphism scaling(float x, float y, float z);
    static SpaceAutomorphism scaling(vec3 factors);

	static SpaceAutomorphism scaling(float x);
	static SpaceAutomorphism scaling(float x, float y, float z, vec3 center);
	static SpaceAutomorphism scaling(float x, vec3 center);
	static SpaceAutomorphism scaling(vec3 factors, vec3 center);

	static SpaceAutomorphism affine(mat3 A, vec3 v);
    static SpaceAutomorphism rotation(float angle);
    static SpaceAutomorphism rotation(vec3 axis, float angle);
	static SpaceAutomorphism rotation(vec3 axis, float angle, vec3 center);
	static SpaceAutomorphism deltaRotation(vec3 v1, vec3 v2);
	static SpaceAutomorphism deltaRotation(vec3 v1, vec3 v2, vec3 center);

};

class CompactlySupportedRealFunction;


/**
 * @class RealFunction
 * @brief Encoding real line continuous endomorphisms
 *
 *
 *  @note precomposition syntax
 *  - both () and & operators act by precomposition on RealFunction
 *		@example
 *  @code
 *  (f & g)(x) = f(g)(x) = f(g(x))
 *  @endcode
 *  - on real numbers () act as evaluation (and thus differs from &)
 *  - on real numbers & act as precomposition with linear reparametrization
 *	@example
 *  @code
 *  (f & a)(x) = f(X_R *a)(x) = f(a *x)
 *  @endcode
 *
 *  @note Predefined elementary functions
 *	@example
 *  @code
 *  ONE_R
 *  X_R
 *  EXP_R
 *  LOG_R
 *  SIN_R
 *  COS_R
 *  SQRT_R
 *  @endcode
 *
 *   @example
 *  @code
 *  SQRT_R == pow(X_R, 0.5)
 *  SQRT_R & F == F.sqrt()
 *  F*F* F == F.pow3()
 *  SQRT & (F*F) == F
 *  F & 2 == F(X_R*2)
 *  F & 2 == F & (X_R & 2)
 *  X_R & 2 == X_R*2
 *  SQRT_R & 4 == SQRT_R*2
 *  @endcode
 */
class RealFunction {
protected:
	Fooo _f;
	Fooo _df;
	Fooo _ddf;
	float eps = 0.01;
	bool is_zero;
public:
	Regularity regularity;

	RealFunction(Fooo f, Fooo df, Fooo ddf, float epsilon=0.01, Regularity regularity = Regularity::SMOOTH);
	RealFunction(Fooo f, Fooo df, float epsilon=0.01, Regularity regularity = Regularity::SMOOTH);
	explicit RealFunction(Fooo f, float epsilon=0.01, Regularity regularity = Regularity::SMOOTH);
	explicit RealFunction(float constant, float epsilon=0.01, Regularity regularity = Regularity::SMOOTH);

	RealFunction(const RealFunction &other);
	RealFunction(RealFunction &&other) noexcept;
	RealFunction & operator=(const RealFunction &other);
	RealFunction & operator=(RealFunction &&other) noexcept;

	float operator()(float x) const;
	float df(float x) const;
	float ddf(float x) const;
	RealFunction df() const;
	float getEps() const { return eps; }


	bool isZero(float a=-10, float b=10, int prec=23);
	float sup(float a, float b, int prec) const;
	float inf(float a, float b, int prec) const;
	float argmax(float a, float b, int prec) const;
	float argmin(float a, float b, int prec) const;

	RealFunction operator+(const RealFunction &g) const;
	RealFunction operator*(float a) const;
	RealFunction operator+(float a) const;
	RealFunction operator-(float a) const;
	RealFunction operator*(const RealFunction &g_) const;
	RealFunction operator-() const;
	RealFunction operator-(const RealFunction &g) const;
	RealFunction operator/(const RealFunction &g_) const;
	RealFunction operator/(float a) const;
	RealFunction pow(float a) const;

	friend RealFunction pow(const RealFunction &f, float a);
	friend RealFunction max(const RealFunction &f, const RealFunction &g);
	friend RealFunction max(const RealFunction &f, float a);
	friend RealFunction min(const RealFunction &f, const RealFunction &g);
	friend RealFunction min(const RealFunction &f, float a);
	friend RealFunction abs(const RealFunction &f);

	RealFunction sqrt() const { return pow(.5f); }
	RealFunction pow2() const { return pow(2); }
	RealFunction pow3() const { return pow(3); }
	RealFunction pow4() const { return pow(4); }

	friend RealFunction operator*(float a, const RealFunction &f);
	friend RealFunction operator+(float a, const RealFunction &f);
	friend RealFunction operator-(float a, const RealFunction &f);
	friend RealFunction operator/(float a, const RealFunction &f);
	// RealFunction operator~() const { return 1._f/(*this); }

	RealFunction operator& (const RealFunction &g_) const;
	RealFunctionR3 operator& (const RealFunctionR3 &g_) const;
	RealFunction operator& (float a) const;
	RealFunction operator()(const RealFunction &g_) const;

	float integral(float a, float b, int prec) const;
	RealFunction antiderivative(float a, int prec) const;
	float L2_norm(vec2 I, int prec) const;
	float L2_product(const RealFunction &g, vec2 I, int prec) const;
	RealFunction convolve(CompactlySupportedRealFunction kernel, int prec=1000) const;
	RealFunction convolve(const RealFunction &kernel, float L, int prec=100) const;
	float repeated_integral(float a, float b, int n, int prec) const;
	RealFunction repeated_antiderivative(float a, int prec) const; // calculated using Cauchy formula for repeated integrals

	friend RealFunctionR2 separated_product(const RealFunction &f_x, const RealFunction &f_t);
	friend RealFunctionR2 separated_sum(const RealFunction &f_x, const RealFunction &f_t);


	static RealFunction constant(float a);
	static RealFunction x();
	static RealFunction one();
	static RealFunction zero();
	static RealFunction linear(float a, float b);
	static RealFunction quadratic(float a, float b, float c);
	static RealFunction monomial(float n);
	static RealFunction polynomial(std::vector<float> coeffs);
	static RealFunction sin();
	static RealFunction cos();
	static RealFunction exp();
	static RealFunction log();
	static RealFunction SQRT();
};


RealFunction max(const RealFunction &f, float a);
RealFunction max(const RealFunction &f, const RealFunction &g);
RealFunction min(const RealFunction &f, float a);
RealFunction min(const RealFunction &f, const RealFunction &g);
RealFunction abs(const RealFunction &f);
RealFunction pow(const RealFunction &f, float a);




class CompactlySupportedRealFunction: public RealFunction {
	vec2 support;
	vec2 support_sum(vec2 other) const;
	vec2 support_intersect(vec2 other) const;
	int default_interval_arithmetic_prec=100;

public:
	CompactlySupportedRealFunction(Fooo f, Fooo df, Fooo ddf, vec2 support, float epsilon=0.01, Regularity regularity = Regularity::SMOOTH);
	CompactlySupportedRealFunction(Fooo f, Fooo df, vec2 support, float epsilon=0.01, Regularity regularity = Regularity::SMOOTH);
	CompactlySupportedRealFunction(Fooo f, vec2 support, float epsilon=0.01, Regularity regularity = Regularity::SMOOTH);
	CompactlySupportedRealFunction(const RealFunction &f, vec2 support);
	CompactlySupportedRealFunction(const RealFunction &f, vec2 support, float epsilon);
	CompactlySupportedRealFunction(const RealFunction &f, const RealFunction &df, vec2 support, float epsilon);
	CompactlySupportedRealFunction(const RealFunction &f, const RealFunction &df, const RealFunction &ddf, vec2 support, float epsilon);

	CompactlySupportedRealFunction df() const;
	float improper_integral(RP1 a, RP1 b, int prec) const;
	float full_domain_integral(int prec) const;
	CompactlySupportedRealFunction antiderivative(float a, int prec) const;

	CompactlySupportedRealFunction operator+(const CompactlySupportedRealFunction &g) const;
	CompactlySupportedRealFunction operator*(float a) const;
	CompactlySupportedRealFunction operator/(float a) const;
	CompactlySupportedRealFunction operator*(const CompactlySupportedRealFunction &g_) const;
	CompactlySupportedRealFunction operator*(const RealFunction &g_) const;

	CompactlySupportedRealFunction operator-() const;
	CompactlySupportedRealFunction operator-(const CompactlySupportedRealFunction &g) const;
	friend CompactlySupportedRealFunction operator*(float a, const CompactlySupportedRealFunction &f);
	friend CompactlySupportedRealFunction operator*(const RealFunction &g, const CompactlySupportedRealFunction &f);



	RealFunction convolve(const RealFunction &f, int prec=1000) const {
		auto K = RealFunctionR2 ([f, kernel=_f](vec2 v) {
			return kernel(v.x-v.y) * f(v.y);
		}, eps, regularity + f.regularity);
		return K.partially_integrate_along_y(support[0], support[1], prec);
	}
};

class RealLineAutomorphism : public RealFunction {
	RealFunction inverse;
public:
	RealLineAutomorphism(RealFunction f, RealFunction inv);
	RealLineAutomorphism(Fooo f, Fooo f_inv, float epsilon=0.01);
	RealLineAutomorphism(Fooo f, Fooo df, Fooo f_inv, float epsilon=0.01);
	RealLineAutomorphism(Fooo f, Fooo df, Fooo f_inv, Fooo df_inv, float epsilon=0.01);
	RealLineAutomorphism(Fooo f, Fooo df, Fooo ddf, Fooo f_inv, Fooo df_inv, Fooo ddf_inv, float epsilon=0.01);

	float inv(float x) const;
	RealLineAutomorphism inv() const;
	RealLineAutomorphism operator~() const;

	RealLineAutomorphism operator& (const RealLineAutomorphism &g) const;
	RealLineAutomorphism operator*(float c) const;
	RealLineAutomorphism operator+(float c) const;
	RealLineAutomorphism operator/(float c) const;
	RealLineAutomorphism operator-(float c) const;
	RealLineAutomorphism operator-() const;
	friend RealLineAutomorphism operator*(float c, const RealLineAutomorphism &f);
	friend RealLineAutomorphism operator+(float c, const RealLineAutomorphism &f);
	friend RealLineAutomorphism operator-(float c, const RealLineAutomorphism &f);

	static RealLineAutomorphism Id();
	static RealLineAutomorphism x();
	static RealLineAutomorphism pow(int n);
};




class PlaneSmoothEndomorphism {
protected:
	shared_ptr<Foo22> _f;
	shared_ptr<std::function<mat2(vec2)>> _df;
public:
	PlaneSmoothEndomorphism();
	PlaneSmoothEndomorphism(shared_ptr<Foo22> f, shared_ptr<std::function<mat2(vec2)>> _df);
	vec2 operator()(vec2 x) const;
	vec2 operator()(float t, float u) const;
	vec2 df(vec2 x, vec2 v) const;
	mat2 df(vec2 x) const;
	vec2 df(float t, float u, vec2 v) const;
	mat2 df(float t, float u) const;
};

class PlaneAutomorphism : public PlaneSmoothEndomorphism {
protected:
	shared_ptr<Foo22> _f_inv;
public:
	PlaneAutomorphism(shared_ptr<Foo22> f, shared_ptr<std::function<mat2(vec2)>> _df, shared_ptr<Foo22> f_inv);
	vec2 f_inv(vec2 x) const;
	vec2 inv(vec2 x) const;
	vec2 inv(float t, float u) const;
	PlaneAutomorphism operator~() const;
	PlaneAutomorphism inv() const;
};

template<typename A, typename B, typename C>
	HOM(A, C) compose(const HOM(B, C) &f, const HOM(A, B) &g);


class HolomorphicFunction {
	HOM(Complex, Complex) _f, _df;
	float eps;

public:
	HolomorphicFunction(HOM(Complex, Complex) f, HOM(Complex, Complex) df, float epsilon);
	HolomorphicFunction(HOM(Complex, Complex) f, float epsilon);
	HolomorphicFunction(const PlaneSmoothEndomorphism &f, float epsilon);
	HolomorphicFunction(const RealFunctionR2 &re, const RealFunctionR2 &im, float epsilon);
	Complex operator()(Complex z) const;

	HolomorphicFunction df() const;
	HolomorphicFunction d2f() const;
	HolomorphicFunction dnf(int n) const;

	Complex df(Complex z) const;
	Complex d2f(Complex z) const;
	Complex dnf(Complex z, int n) const;

	HolomorphicFunction operator-() const;
	HolomorphicFunction operator+(const HolomorphicFunction &g) const;
	HolomorphicFunction operator+(Complex a) const;
	HolomorphicFunction operator+(float a) const;
	HolomorphicFunction operator+(int a) const;
	friend HolomorphicFunction operator+(Complex a, const HolomorphicFunction &f);
	friend HolomorphicFunction operator+(float a, const HolomorphicFunction &f);
	friend HolomorphicFunction operator+(int a, const HolomorphicFunction &f);

	HolomorphicFunction operator-(const HolomorphicFunction &g) const;
	HolomorphicFunction operator-(Complex a) const;
	HolomorphicFunction operator-(float a) const;
	HolomorphicFunction operator-(int a) const;
	friend HolomorphicFunction operator-(Complex a, const HolomorphicFunction &f);
	friend HolomorphicFunction operator-(float a, const HolomorphicFunction &f);
	friend HolomorphicFunction operator-(int a, const HolomorphicFunction &f);

	HolomorphicFunction operator*(const HolomorphicFunction &g) const;
	HolomorphicFunction operator*(Complex a) const;
	HolomorphicFunction operator*(float a) const;
	HolomorphicFunction operator*(int a) const;
	friend HolomorphicFunction operator*(Complex a, const HolomorphicFunction &f);
	friend HolomorphicFunction operator*(float a, const HolomorphicFunction &f);
	friend HolomorphicFunction operator*(int a, const HolomorphicFunction &f);

	HolomorphicFunction operator/(Complex a) const;
	HolomorphicFunction operator/(float a) const;
	HolomorphicFunction operator/(int a) const;
	friend HolomorphicFunction operator/(Complex a, const HolomorphicFunction &f);
	friend HolomorphicFunction operator/(float a, const HolomorphicFunction &f);
	friend HolomorphicFunction operator/(int a, const HolomorphicFunction &f);
	HolomorphicFunction operator/(const HolomorphicFunction &g) const;

	HolomorphicFunction operator& (const HolomorphicFunction &g) const;

	HolomorphicFunction pow(int a) const;
	HolomorphicFunction pow(float a) const;
	HolomorphicFunction sqrt() const;
	HolomorphicFunction sq() const;
	HolomorphicFunction pow2() const;
	HolomorphicFunction pow3() const;
};

const HolomorphicFunction ONE_C = HolomorphicFunction([](Complex z) { return Complex(1); }, [](Complex z) { return Complex(0); }, .001f);
const HolomorphicFunction ZERO_C = HolomorphicFunction([](Complex z) { return Complex(0); }, [](Complex z) { return Complex(0); }, .001f);
const HolomorphicFunction X_C = HolomorphicFunction([](Complex z) { return z; }, [](Complex z) { return Complex(1); }, .001f);
const HolomorphicFunction X2_C = X_C * X_C;
const HolomorphicFunction EXP_C = HolomorphicFunction([](Complex z) { return exp(z); }, [](Complex z) { return exp(z); }, .001f);
const HolomorphicFunction LOG_C = HolomorphicFunction([](Complex z) { return log(z); }, [](Complex z) { return 1/z; }, .001f);
const HolomorphicFunction SIN_C = HolomorphicFunction([](Complex z) { return sin(z); }, [](Complex z) { return cos(z); }, .001f);
const HolomorphicFunction COS_C = HolomorphicFunction([](Complex z) { return cos(z); }, [](Complex z) { return -sin(z); }, .001f);
const HolomorphicFunction TAN_C = SIN_C / COS_C;
const HolomorphicFunction COT_C = 1/TAN_C;
const HolomorphicFunction SINH_C = HolomorphicFunction([](Complex z) { return sinh(z); }, [](Complex z) { return cosh(z); }, .001f);
const HolomorphicFunction COSH_C = HolomorphicFunction([](Complex z) { return cosh(z); }, [](Complex z) { return sinh(z); }, .001f);
const HolomorphicFunction TANH_C = SINH_C / COSH_C;
const HolomorphicFunction COTH_C = 1/TANH_C;
const HolomorphicFunction ASIN_C = HolomorphicFunction([](Complex z) { return asin(z); }, [](Complex z) { return 1/sqrt(1-z*z); }, .001f);
const HolomorphicFunction ACOS_C = HolomorphicFunction([](Complex z) { return acos(z); }, [](Complex z) { return -1/sqrt(1-z*z); }, .001f);
const HolomorphicFunction ATAN_C = HolomorphicFunction([](Complex z) { return atan(z); }, [](Complex z) { return 1/(1+z*z); }, .001f);
const HolomorphicFunction ACOT_C = HolomorphicFunction([](Complex z) { return atan(1/z); }, [](Complex z) { return -1/(1+1/(z*z)); }, .001f);
const HolomorphicFunction ASINH_C = HolomorphicFunction([](Complex z) { return asinh(z); }, [](Complex z) { return 1/sqrt(z*z+1); }, .001f);

class BiholomorphicFunction {
	HolomorphicFunction _f, _f_inv;

public:
	BiholomorphicFunction(const HolomorphicFunction &f, const HolomorphicFunction &f_inv) : _f(f), _f_inv(f_inv) {}
	Complex operator()(Complex z) const { return _f(z); }
	Complex inv(Complex z) const { return _f_inv(z); }
	Complex df(Complex z) const { return _f.df(z); }

	BiholomorphicFunction operator~() const { return BiholomorphicFunction(_f_inv, _f);}
	BiholomorphicFunction inv() const { return ~(*this); }
	BiholomorphicFunction operator& (const BiholomorphicFunction &g) const { return BiholomorphicFunction(_f & g._f, g._f_inv & _f_inv); }
	operator HolomorphicFunction() const { return _f; }
};

class ComplexValuedFunction {
	HOM(float, Complex) _f;
	float eps;
public:
	explicit ComplexValuedFunction(Complex c, float eps=.001) : _f([c](float) { return c; }), eps(eps) {}
	explicit ComplexValuedFunction(float c, float eps=.001) : ComplexValuedFunction(Complex(c), eps) {}
	explicit ComplexValuedFunction(int c, float eps=.001) : ComplexValuedFunction(Complex(c), eps) {}

	explicit ComplexValuedFunction(HOM(float, Complex) f, float eps=.001);
	explicit ComplexValuedFunction(RealFunction re, float eps=.001);
	ComplexValuedFunction(RealFunction f_re, RealFunction f_im, float eps=.001);
	ComplexValuedFunction(RealFunction f_power, RealFunction f_phase, bool dump);

	explicit ComplexValuedFunction(HOM(float, float) re, float eps=.001);
	ComplexValuedFunction(HOM(float, float) f_re, HOM(float, float) f_im, float eps=.001);

	ComplexValuedFunction(const ComplexValuedFunction &other);
	ComplexValuedFunction(ComplexValuedFunction &&other) noexcept;
	ComplexValuedFunction & operator=(const ComplexValuedFunction &other);
	ComplexValuedFunction & operator=(ComplexValuedFunction &&other) noexcept;



	Complex operator()(float x) const { return _f(x); }
	Complex operator()(int x) const { return _f(x*1.f); }

	ComplexValuedFunction df() const { return ComplexValuedFunction([f=_f, eps=eps](float x) { return (f(x+eps) - f(x-eps))/(2.f*eps); }, eps); }
	ComplexValuedFunction conj() const { return ComplexValuedFunction(re(), -im(), eps); }

	ComplexValuedFunction operator+(const ComplexValuedFunction &g) const { return ComplexValuedFunction([f=_f, g=g](float x) { return f(x) + g(x); }, eps); }
	ComplexValuedFunction operator+(const RealFunction &g) const { return ComplexValuedFunction([f=_f, g=g](float x) { return f(x) + g(x); }, eps); }
	ComplexValuedFunction operator+(const Complex &g) const { return ComplexValuedFunction([f=_f, g=g](float x) { return f(x) + g; }, eps); }
	ComplexValuedFunction operator+(const float &g) const { return *this + Complex(g); }
	ComplexValuedFunction operator+(const int &g) const { return *this + Complex(g); }
	friend ComplexValuedFunction operator+(const RealFunction &g, const ComplexValuedFunction &f) { return f + g; }
    friend ComplexValuedFunction operator+(const Complex &g, const ComplexValuedFunction &f) { return f + g; }
	friend ComplexValuedFunction operator+(float a, const ComplexValuedFunction &f) { return f + a; }
	friend ComplexValuedFunction operator+(int a, const ComplexValuedFunction &f) { return f + a; }

	ComplexValuedFunction operator-() const { return ComplexValuedFunction([f=_f](float x) { return -f(x); }, eps); }
	ComplexValuedFunction operator-(const ComplexValuedFunction &g) const { return (*this) + (-g); }
	ComplexValuedFunction operator-(const RealFunction &g) const { return (*this) + (-g); }
	ComplexValuedFunction operator-(const Complex &g) const { return (*this) + (-g); }
	ComplexValuedFunction operator-(const float &g) const { return *this + -g; }
	ComplexValuedFunction operator-(const int &g) const { return *this + -g; }
	friend ComplexValuedFunction operator-(const RealFunction &g, const ComplexValuedFunction &f) { return -f + g; }
	friend ComplexValuedFunction operator-(const Complex &g, const ComplexValuedFunction &f) { return -f + g; }
	friend ComplexValuedFunction operator-(float a, const ComplexValuedFunction &f) { return a + -f;}
	friend ComplexValuedFunction operator-(int a, const ComplexValuedFunction &f) { return a + -f;}

	ComplexValuedFunction operator*(const ComplexValuedFunction &g) const { return ComplexValuedFunction([f=_f, g=g](float x) { return f(x) * g(x); }, eps); }
	ComplexValuedFunction operator*(const RealFunction &g) const { return ComplexValuedFunction([f=_f, g=g](float x) { return f(x) * g(x); }, eps); }
	ComplexValuedFunction operator*(const Complex &g) const { return ComplexValuedFunction([f=_f, g=g](float x) { return f(x) * g; }, eps); }
	ComplexValuedFunction operator*(const float &g) const { return *this * Complex(g); }
	ComplexValuedFunction operator*(const int &g) const { return *this * Complex(g); }
	friend ComplexValuedFunction operator*(const RealFunction &g, const ComplexValuedFunction &f) { return f * g; }
	friend ComplexValuedFunction operator*(const Complex &g, const ComplexValuedFunction &f) { return f * g; }
	friend ComplexValuedFunction operator*(float a, const ComplexValuedFunction &f) { return f*a; }
	friend ComplexValuedFunction operator*(int a, const ComplexValuedFunction &f) { return f*a; }


	ComplexValuedFunction operator/(const ComplexValuedFunction &g) const { return ComplexValuedFunction([f=_f, g=g](float x) { return f(x) / g(x); }, eps); }
	ComplexValuedFunction operator/(const RealFunction &g) const { return ComplexValuedFunction([f=_f, g=g](float x) { return f(x) / g(x); }, eps); }
	ComplexValuedFunction operator/(const Complex &g) const { return ComplexValuedFunction([f=_f, g=g](float x) { return f(x) / g; }, eps); }
	ComplexValuedFunction operator/(const float &g) const { return *this * (1.f/g); }
	ComplexValuedFunction operator/(const int &g) const { return *this * (1.f/g); }
	friend ComplexValuedFunction operator/(const RealFunction &g, const ComplexValuedFunction &f) { return ComplexValuedFunction([f=f, g=g](float x) { return Complex(g(x)) / f(x); }, f.eps); }
	friend ComplexValuedFunction operator/(const Complex &g, const ComplexValuedFunction &f) { return ComplexValuedFunction([f=f, g=g](float x) { return Complex(g) / f(x); }, f.eps); }
	friend ComplexValuedFunction operator/(float a, const ComplexValuedFunction &f) { return Complex(a) / f; }
	friend ComplexValuedFunction operator/(int a, const ComplexValuedFunction &f) { return Complex(a) / f; }


	ComplexValuedFunction operator & (float t) const { return ComplexValuedFunction([f=_f, t](float x) { return f(x*t); }, eps); }
	ComplexValuedFunction operator & (const RealFunction &g) const { return ComplexValuedFunction([f=_f, g=g](float x) { return f(g(x)); }, eps); }
	ComplexValuedFunction operator() (const RealFunction &g) const { return (*this) & g; }

	RealFunction re() const { return RealFunction([f=_f](float x) { return f(x).real(); }, eps); }
	RealFunction im() const { return RealFunction([f=_f](float x) { return f(x).imag(); }, eps); }
	RealFunction abs() const { return RealFunction([f=_f](float x) { return norm(f(x)); }, eps); }
	RealFunction arg() const { return RealFunction([f=_f](float x) { return f(x).arg(); }, eps); }
	RealFunction norm2() const { return RealFunction([f=_f](float x) { return ::norm2(f(x)); }, eps); }

	Complex integral(float a, float b, int prec) const { return Complex(re().integral(a, b, prec), im().integral(a, b, prec)); }
};


class ComplexFunctionR2 {
	BIHOM(float, float,  Complex) _f;
	float eps;
public:
	explicit ComplexFunctionR2(Complex c) : _f([c](float x, float y) { return c; }), eps(0.01) {}
	explicit ComplexFunctionR2(BIHOM(float, float,  Complex) f, float eps=.01) : _f(std::move(f)), eps(eps) {}
	explicit ComplexFunctionR2(RealFunctionR2 re, float eps=.01) : ComplexFunctionR2([re](float x, float y) { return Complex(re(x, y), 0); }, eps) {}
	ComplexFunctionR2(RealFunctionR2 f_re, RealFunctionR2 f_im, float eps=.01) : ComplexFunctionR2([f_re, f_im](float x, float y) { return Complex(f_re(x, y), f_im(x, y)); }, eps) {}
	explicit ComplexFunctionR2(HOM(vec2,  Complex) f, float eps=.01) : ComplexFunctionR2([f](float x, float y) { return f(vec2(x, y)); }, eps) {}


	ComplexFunctionR2(const ComplexFunctionR2 &other);
	ComplexFunctionR2(ComplexFunctionR2 &&other) noexcept;
	ComplexFunctionR2 & operator=(const ComplexFunctionR2 &other);
	ComplexFunctionR2 & operator=(ComplexFunctionR2 &&other) noexcept;


	Complex operator()(float x, float y) const { return _f(x, y); }
	Complex operator()(vec2 x) const { return _f(x.x, x.y); }
	ComplexFunctionR2 dfdx() const { return ComplexFunctionR2([f=_f, eps=eps](float x, float y) { return (f(x+eps, y) - f(x-eps, y))/(2.f*eps); }, eps); }
	ComplexFunctionR2 conj() const { return ComplexFunctionR2(re(), -im(), eps); }

	ComplexFunctionR2 operator+(const ComplexFunctionR2 &g) const { return ComplexFunctionR2([f=_f, g=g](float x, float y) { return f(x, y) + g(x, y); }, eps); }
	ComplexFunctionR2 operator+(const RealFunctionR2 &g) const { return ComplexFunctionR2([f=_f, g=g](float x, float y) { return f(x, y) + g(x, y); }, eps); }
	ComplexFunctionR2 operator+(const Complex &g) const { return ComplexFunctionR2([f=_f, g=g](float x, float y) { return f(x, y) + g; }, eps); }
	friend ComplexFunctionR2 operator+(const RealFunctionR2 &g, const ComplexFunctionR2 &f) { return f + g; }
    friend ComplexFunctionR2 operator+(const Complex &g, const ComplexFunctionR2 &f) { return f + g; }

	ComplexFunctionR2 operator-() const { return ComplexFunctionR2([f=_f](float x, float y) { return -f(x, y); }, eps); }
	ComplexFunctionR2 operator-(const ComplexFunctionR2 &g) const { return (*this) + (-g); }
	ComplexFunctionR2 operator-(const RealFunctionR2 &g) const { return (*this) + (-g); }
	ComplexFunctionR2 operator-(const Complex &g) const { return (*this) + (-g); }
	friend ComplexFunctionR2 operator-(const RealFunctionR2 &g, const ComplexFunctionR2 &f) { return -f + g; }
	friend ComplexFunctionR2 operator-(const Complex &g, const ComplexFunctionR2 &f) { return -f + g; }

	ComplexFunctionR2 operator*(const ComplexFunctionR2 &g) const { return ComplexFunctionR2([f=_f, g=g](float x, float y) { return f(x, y) * g(x, y); }, eps); }
	ComplexFunctionR2 operator*(const RealFunctionR2 &g) const { return ComplexFunctionR2([f=_f, g=g](float x, float y) { return f(x, y) * g(x, y); }, eps); }
	ComplexFunctionR2 operator*(const Complex &g) const { return ComplexFunctionR2([f=_f, g=g](float x, float y) { return f(x, y) * g; }, eps); }
	friend ComplexFunctionR2 operator*(const RealFunctionR2 &g, const ComplexFunctionR2 &f) { return f * g; }
	friend ComplexFunctionR2 operator*(const Complex &g, const ComplexFunctionR2 &f) { return f * g; }

	ComplexFunctionR2 operator/(const ComplexFunctionR2 &g) const { return ComplexFunctionR2([f=_f, g=g](float x, float y) { return f(x, y) / g(x, y); }, eps); }
	ComplexFunctionR2 operator/(const RealFunctionR2 &g) const { return ComplexFunctionR2([f=_f, g=g](float x, float y) { return f(x, y) / g(x, y); }, eps); }
	ComplexFunctionR2 operator/(const Complex &g) const { return ComplexFunctionR2([f=_f, g=g](float x, float y) { return f(x, y) / g; }, eps); }
	friend ComplexFunctionR2 operator/(const RealFunctionR2 &g, const ComplexFunctionR2 &f) { return ComplexFunctionR2([f=f, g=g](float x, float y) { return Complex(g(x, y)) / f(x, y); }, f.eps); }
	friend ComplexFunctionR2 operator/(const Complex &g, const ComplexFunctionR2 &f) { return ComplexFunctionR2([f=f, g=g](float x, float y) { return Complex(g) / f(x, y); }, f.eps); }

	RealFunctionR2 re() const { return RealFunctionR2([f=_f](float x, float y) { return f(x, y).real(); }, eps); }
	RealFunctionR2 im() const { return RealFunctionR2([f=_f](float x, float y) { return f(x, y).imag(); }, eps); }
	RealFunctionR2 abs() const { return RealFunctionR2([f=_f](float x, float y) { return norm(f(x, y)); }, eps); }
	RealFunctionR2 arg() const { return RealFunctionR2([f=_f](float x, float y) { return f(x, y).arg(); }, eps); }
	RealFunctionR2 norm2() const { return RealFunctionR2([f=_f](float x, float y) { return ::norm2(f(x, y)); }, eps); }
};




const auto EXP_it = ComplexValuedFunction([](float t) { return exp(t*1.0i); }, 0.01);
const auto LOG_RC = ComplexValuedFunction([](float t) { return log(Complex(t)); }, 0.01);
const auto SQRT_RC = ComplexValuedFunction([](float t) { return sqrt(Complex(t)); }, 0.01);
const auto ONE_RC = ComplexValuedFunction(1.f);
const auto X_RC = ComplexValuedFunction([](float t) { return Complex(t); }, 0.01);
const auto SIN_RC = ComplexValuedFunction([](float t) { return sin(Complex(t)); }, 0.01);
const auto COS_RC = ComplexValuedFunction([](float t) { return cos(Complex(t)); }, 0.01);
const auto EXP_RC = ComplexValuedFunction([](float t) { return exp(Complex(t)); }, 0.01);



template<typename Domain, typename Codomain>
class DiscreteSingleVariableFunction {
	int size = 2;
public:
	Domain dom_a, dom_b;

	DiscreteSingleVariableFunction(Domain a, Domain b, int size) : size(size), dom_a(a), dom_b(b) {}
	virtual ~DiscreteSingleVariableFunction() = default;

	virtual int samples() const {
		return size;
	}

	float sampling_step() const {
		if (size < 2) return dom_b - dom_a;
		return (dom_b -dom_a) / (size-1);
	}

	bool starts_from_zero() const { return isClose(0, norm(dom_a)); }

	bool symmetric() const { return isClose(0, norm(dom_a + dom_b)); }

	void setDom(Domain a, Domain b) {
		dom_a = a;
		dom_b = b;
	}
};



class DiscreteRealFunction;


class DiscreteComplexFunction : public DiscreteSingleVariableFunction<float, Complex> {
	Vector<Complex> fn;
public:
	DiscreteComplexFunction(const Vector<Complex> &fn, vec2 domain) : DiscreteSingleVariableFunction(domain.x, domain.y, fn.length()), fn(fn) {}
	DiscreteComplexFunction(const Vector<Complex> &fn, float dom_a, float dom_b) : DiscreteComplexFunction(fn, vec2(dom_a, dom_b)) {}
	DiscreteComplexFunction(const vector<Complex> &fn, vec2 domain) : DiscreteSingleVariableFunction(domain.x, domain.y, fn.size()), fn(fn){}
	DiscreteComplexFunction(const HOM(float, Complex) &fn, vec2 domain, int sampling);


	Vector<Complex> getVector() const { return fn; }
	vec2 domain() const { return vec2(dom_a, dom_b); }

	vec2 getDomain() const { return domain(); }
	Complex& operator[](int i) { return fn[i]; }
	const Complex & operator[](int i) const { return fn[i]; }
	void setDomain(vec2 domain) { setDom(domain.x, domain.y); }

	int samples() const override { return fn.size(); }
	vector<float> args() const { return linspace(dom_a, dom_b, samples()); }

	DiscreteComplexFunction(const DiscreteComplexFunction &other);
	DiscreteComplexFunction(DiscreteComplexFunction &&other) noexcept;
	DiscreteComplexFunction & operator=(const DiscreteComplexFunction &other);
	DiscreteComplexFunction & operator=(DiscreteComplexFunction &&other) noexcept;


	explicit DiscreteComplexFunction(const DiscreteRealFunction &other);

	DiscreteComplexFunction operator+(const DiscreteComplexFunction &g) const;
	DiscreteComplexFunction operator+(Complex a) const;
	DiscreteComplexFunction operator+(float a) const;
	friend DiscreteComplexFunction operator+(float a, const DiscreteComplexFunction &f) { return f + a; }
	friend DiscreteComplexFunction operator+(Complex a, const DiscreteComplexFunction &f) { return f + a; }

	DiscreteComplexFunction operator-() const { return DiscreteComplexFunction(-fn, domain()); }
	DiscreteComplexFunction operator-(const DiscreteComplexFunction &g) const { return (*this) + (-g); }
	DiscreteComplexFunction operator-(float a) const { return (*this) + (-a); }
	DiscreteComplexFunction operator-(Complex a) const { return (*this) + (-a); }
	friend DiscreteComplexFunction operator-(float a, const DiscreteComplexFunction &f) { return f + -a; }
	friend DiscreteComplexFunction operator-(Complex a, const DiscreteComplexFunction &f) { return f + -a; }

	DiscreteComplexFunction operator*(const DiscreteComplexFunction &g) const;
	DiscreteComplexFunction operator*(Complex a) const;
	DiscreteComplexFunction operator*(float a) const {return this->operator*(Complex(a));}
	friend DiscreteComplexFunction operator*(float a, const DiscreteComplexFunction &f) { return f * a; }
	friend DiscreteComplexFunction operator*(Complex a, const DiscreteComplexFunction &f) { return f * a; }


	DiscreteComplexFunction operator/(const DiscreteComplexFunction &g) const;
	DiscreteComplexFunction operator/(float a) const { return this->operator/(Complex(1.0f/a)); }
	DiscreteComplexFunction operator/(Complex a) const { return this->operator*(1.0f/a); }

	Complex operator()(float x) const;
	DiscreteRealFunction re() const;
	DiscreteRealFunction im() const;
	DiscreteRealFunction abs() const;
	DiscreteRealFunction arg() const;
	DiscreteComplexFunction conj() const;
	// vector<float> sampling_args() const { return linspace(dom_a, dom_b, samples()); }


	DiscreteComplexFunction fft() const;
	DiscreteComplexFunction ifft_() const;
	DiscreteComplexFunction ifft() const { return ifft_()*(1.f/samples()); }

	Complex integral() const { return sum<Complex>(fn) * sampling_step(); }
	float L2_norm() const { return norm((*this * conj()).integral()); }
	float supp_len() const { return dom_b-dom_a; }

	DiscreteComplexFunction shift_domain_left() const;
	DiscreteComplexFunction shift_domain_right() const;
	DiscreteComplexFunction downsample(int factor, int shift=0) const;
};



class DiscreteRealFunction {
	Vector<float> fn;
	vec2 domain;
public:
	DiscreteRealFunction() = delete;
	DiscreteRealFunction(const Vector<float> &fn, vec2 domain=vec2(0, 1));
	DiscreteRealFunction(const vector<float> &fn, vec2 domain=vec2(0, 1)) : DiscreteRealFunction(Vector(fn), domain) {}
	DiscreteRealFunction(const HOM(float, float) &f, vec2 domain, int sampling);

	int samples() const { return fn.size(); }
	float sampling_step() const { return (domain[1] - domain[0]) / (samples()-1); }
	float operator[](int i) const;
	Vector<float> getVector() const { return fn; }
	vec2 getDomain() const { return domain; }
	void setDomain(vec2 dom) { this->domain = dom; }

	DiscreteRealFunction(const DiscreteRealFunction &other);
	DiscreteRealFunction(DiscreteRealFunction &&other) noexcept;
	DiscreteRealFunction & operator=(const DiscreteRealFunction &other);
	DiscreteRealFunction & operator=(DiscreteRealFunction &&other) noexcept;

	vector<float> args() const { return linspace(domain[0], domain[1], samples()); }

	DiscreteRealFunction operator+(const DiscreteRealFunction &g) const;
	DiscreteRealFunction operator+(float a) const;
	friend DiscreteRealFunction operator+(float a, const DiscreteRealFunction &f) { return f + a; }
	DiscreteRealFunction operator-() const { return DiscreteRealFunction(-fn, domain); }
	DiscreteRealFunction operator-(const DiscreteRealFunction &g) const { return (*this) + (-g); }
	DiscreteRealFunction operator-(float a) const { return (*this) + (-a); }
	friend DiscreteRealFunction operator-(float a, const DiscreteRealFunction &f) { return f + -a; }
	DiscreteRealFunction operator*(const DiscreteRealFunction &g) const { return DiscreteRealFunction(fn.pointwise_product(g.fn), domain); }
	DiscreteRealFunction operator*(float a) const { return DiscreteRealFunction(fn * a, domain); }
	friend DiscreteRealFunction operator*(float a, const DiscreteRealFunction &f) { return f * a; }
	DiscreteRealFunction operator/(const DiscreteRealFunction &g) const { return DiscreteRealFunction(fn.pointwise_division(g.fn), domain); }
	DiscreteRealFunction operator/(float a) const { return this->operator*(1.0f/a); }

	friend DiscreteRealFunction max(const DiscreteRealFunction &f, const DiscreteRealFunction &g);
	friend DiscreteRealFunction max(const DiscreteRealFunction &f, float g);
	friend DiscreteRealFunction max(float f, const DiscreteRealFunction &g);
	friend DiscreteRealFunction min(const DiscreteRealFunction &f, const DiscreteRealFunction &g);
	friend DiscreteRealFunction min(const DiscreteRealFunction &f, float g);
	friend DiscreteRealFunction min(float f, const DiscreteRealFunction &g);
	friend DiscreteRealFunction maxAbs(const DiscreteRealFunction &f, const DiscreteRealFunction &g);

	DiscreteRealFunction slice(int a, int b) const;

	float operator()(float x) const;
	DiscreteRealFunction two_sided_zero_padding(int target_size) const;
	DiscreteComplexFunction fft() const;

	DiscreteRealFunction convolve(const DiscreteRealFunction &kernel) const;
	DiscreteRealFunction smoothen(float L) const {
		auto k = DiscreteRealFunction([L](float x){return exp(-x*x/L/L);}, domain, samples());
		k = k / L2_norm();
		return convolve(k);
	}
	DiscreteRealFunction derivative() const;
	float integral() const { return sum(fn) * sampling_step(); }
	float L2_norm() const { return ::sqrt((*this * *this).integral()); }
	float integral(int b) const { return sum(fn.slice_to(b)) * sampling_step(); }
	float integral(int a, int b) const { return sum(fn.slice(a, b)) * sampling_step(); }



	float supp_len() const { return domain[1] - domain[0]; }

	DiscreteRealFunction shift_domain_left() const;
	DiscreteRealFunction shift_domain_right() const;
	DiscreteRealFunction downsample(int factor) const;
	DiscreteRealFunction downsample(int factor, int shift) const;
	DiscreteRealFunction downsample_max(int factor) const;


	bool starts_from_zero() const { return isClose(0, domain[0]); }
	bool symmetric() const { return isClose(domain[0], -domain[1]); }

};

class DiscreteRealFunctionR2;


class DiscreteRealFunctionNonUniform {
	DiscreteRealFunction sampling_args, values;
public:
	DiscreteRealFunctionNonUniform(const DiscreteRealFunction &args, const DiscreteRealFunction &values);
	DiscreteRealFunctionNonUniform(const RealFunction &f, float arclen_step, float linear_eps, vec2 bounds, bool include_bounds);

	float operator()(float x) const;
	vec2 point(int i) const;
	float operator[](int i) const;
	DiscreteRealFunctionNonUniform refine_domain(const DiscreteRealFunction &new_args) const;
	DiscreteRealFunctionNonUniform refine_domain(float x) const;
	DiscreteRealFunctionNonUniform derivative() const;

	Vector<float> arclens() const;
	float arclen() const { return arclens().sum(); }

	DiscreteRealFunctionNonUniform operator+(const DiscreteRealFunctionNonUniform &g) const;
	DiscreteRealFunctionNonUniform operator+(float a) const;
	friend DiscreteRealFunctionNonUniform operator+(float a, const DiscreteRealFunctionNonUniform &f);
	DiscreteRealFunctionNonUniform operator-() const;
	DiscreteRealFunctionNonUniform operator-(const DiscreteRealFunctionNonUniform &g) const;
	DiscreteRealFunctionNonUniform operator-(float a) const;
	friend DiscreteRealFunctionNonUniform operator-(float a, const DiscreteRealFunctionNonUniform &f);
	DiscreteRealFunctionNonUniform operator*(const DiscreteRealFunctionNonUniform &g) const;
	DiscreteRealFunctionNonUniform operator*(float a) const;
	friend DiscreteRealFunctionNonUniform operator*(float a, const DiscreteRealFunctionNonUniform &f);
	DiscreteRealFunctionNonUniform operator/(const DiscreteRealFunctionNonUniform &g) const;
	DiscreteRealFunctionNonUniform operator/(float a) const;
	friend DiscreteRealFunctionNonUniform operator/(float a, const DiscreteRealFunctionNonUniform &f);

	Vector<float> args_vector() const { return sampling_args.getVector(); }
	Vector<float> values_vector() const { return values.getVector(); }
	int size() const { return sampling_args.samples(); }
	int samples() const { return sampling_args.samples(); }

	DiscreteRealFunctionNonUniform operator&(const DiscreteRealFunctionNonUniform &g) const;
	friend DiscreteRealFunctionNonUniform operator&(const DiscreteRealFunction &f, const DiscreteRealFunctionNonUniform &g);
	friend DiscreteRealFunctionNonUniform operator&(const DiscreteRealFunctionNonUniform &f, const DiscreteRealFunction &g);
	friend DiscreteRealFunctionNonUniform operator&(const DiscreteRealFunction &f, const DiscreteRealFunction &g);
};

class DiscreteComplexFunctionR2 {
	vector<DiscreteComplexFunction> fn;
	vec2 domain;
public:
	DiscreteComplexFunctionR2(const vector<DiscreteComplexFunction> &fn, vec2 domain);
	DiscreteComplexFunctionR2(const vector<HOM(float, Complex)> &f, vec2 domain, int sampling);

	DiscreteComplexFunctionR2(const DiscreteComplexFunctionR2 &other) = default;
	DiscreteComplexFunctionR2(DiscreteComplexFunctionR2 &&other) noexcept = default;
	DiscreteComplexFunctionR2 & operator=(const DiscreteComplexFunctionR2 &other) = default;
	DiscreteComplexFunctionR2 & operator=(DiscreteComplexFunctionR2 &&other) noexcept = default;

	int samples_t () const;
	int samples_x () const;
	ivec2 samples() const { return ivec2(samples_t(), samples_x()); }
	float sampling_step_t() const;
	float sampling_step_x() const;
	vector<float> args_t() const;
	vector<float> args_x() const;
	void setDomain(vec2 dom) { this->domain = dom; }

	DiscreteComplexFunctionR2 operator+(const DiscreteComplexFunctionR2 &g) const;
	DiscreteComplexFunctionR2 operator+(Complex a) const;
	DiscreteComplexFunctionR2 operator+(float a) const;
	friend DiscreteComplexFunctionR2 operator+(float a, const DiscreteComplexFunctionR2 &f);
	friend DiscreteComplexFunctionR2 operator+(Complex a, const DiscreteComplexFunctionR2 &f);
	DiscreteComplexFunctionR2 operator-() const;
	DiscreteComplexFunctionR2 operator-(const DiscreteComplexFunctionR2 &g) const;
	DiscreteComplexFunctionR2 operator-(float a) const;
	DiscreteComplexFunctionR2 operator-(Complex a) const;
	friend DiscreteComplexFunctionR2 operator-(float a, const DiscreteComplexFunctionR2 &f);
	friend DiscreteComplexFunctionR2 operator-(Complex a, const DiscreteComplexFunctionR2 &f);
	DiscreteComplexFunctionR2 operator*(const DiscreteComplexFunctionR2 &g) const;
	DiscreteComplexFunctionR2 operator*(Complex a) const;
	DiscreteComplexFunctionR2 operator*(float a) const;
	friend DiscreteComplexFunctionR2 operator*(float a, const DiscreteComplexFunctionR2 &f);
	friend DiscreteComplexFunctionR2 operator*(Complex a, const DiscreteComplexFunctionR2 &f);
	DiscreteComplexFunctionR2 operator/(const DiscreteComplexFunctionR2 &g) const;
	DiscreteComplexFunctionR2 operator/(float a) const;
	DiscreteComplexFunctionR2 operator/(Complex a) const;

	DiscreteComplexFunction operator[](int t) const;
	Complex operator()(float t, float x) const;
	Complex operator()(vec2 x) const;



	DiscreteComplexFunctionR2 integrate_t(int i_t) const;
	DiscreteComplexFunctionR2 integrate_x(int i_x) const;
	DiscreteComplexFunctionR2 transpose() const;
	Complex double_integral() const;
	Complex double_integral(int i_t) const;
	Complex double_integral(int i_t, int i_x) const;

	DiscreteComplexFunctionR2 fft_t() const;
	DiscreteComplexFunctionR2 ifft_t() const;
	DiscreteComplexFunctionR2 fft_x() const;
	DiscreteComplexFunctionR2 ifft_x() const;
	DiscreteComplexFunctionR2 fft() const;
	DiscreteComplexFunctionR2 fft(int var) const;;
	DiscreteComplexFunctionR2 ifft(int var) const;;
	DiscreteComplexFunctionR2 ifft() const;

	DiscreteRealFunctionR2 re() const;
	DiscreteRealFunctionR2 im() const;
	DiscreteRealFunctionR2 abs() const;
	DiscreteRealFunctionR2 arg() const;

	DiscreteComplexFunctionR2 downsample_t(int factor) const;
	DiscreteComplexFunctionR2 downsample_x(int factor) const;
	DiscreteComplexFunctionR2 downsample(int factor_t, int factor_x) const;
};


class DiscreteRealFunctionR2 {
	vector<DiscreteRealFunction> fn;
	vec2 domain;
public:
	DiscreteRealFunctionR2(const vector<DiscreteRealFunction> &fn, vec2 domain);
	DiscreteRealFunctionR2(const vector<HOM(float, float)> &f, vec2 domain, int sampling);
	DiscreteRealFunctionR2(const DiscreteRealFunctionR2 &other) = default;
	DiscreteRealFunctionR2(DiscreteRealFunctionR2 &&other) noexcept = default;
	DiscreteRealFunctionR2 & operator=(const DiscreteRealFunctionR2 &other) = default;
	DiscreteRealFunctionR2 & operator=(DiscreteRealFunctionR2 &&other) noexcept = default;

	DiscreteRealFunction operator[](int t) const;
	DiscreteRealFunction operator()(float t) const;

	DiscreteRealFunctionR2 operator+(const DiscreteRealFunctionR2 &g) const;
	DiscreteRealFunctionR2 operator+(float a) const;
	friend DiscreteRealFunctionR2 operator+(float a, const DiscreteRealFunctionR2 &f);
	DiscreteRealFunctionR2 operator-() const;
	DiscreteRealFunctionR2 operator-(const DiscreteRealFunctionR2 &g) const;
	DiscreteRealFunctionR2 operator-(float a) const;
	friend DiscreteRealFunctionR2 operator-(float a, const DiscreteRealFunctionR2 &f);
	DiscreteRealFunctionR2 operator*(float a) const;
	DiscreteRealFunctionR2 operator/(float a) const;
	friend DiscreteRealFunctionR2 operator/(float a, const DiscreteRealFunctionR2 &f);
	DiscreteRealFunctionR2 operator/(const DiscreteRealFunctionR2 &g) const;
	DiscreteRealFunctionR2 operator*(const DiscreteRealFunctionR2 &g) const;

	DiscreteRealFunctionR2 dfdx() const;


	int samples_t () const;
	int samples_x () const;
	ivec2 samples() const { return ivec2(samples_t(), samples_x()); }
	float sampling_step_t() const;
	float sampling_step_x() const;
	vector<float> args_t() const;
	vector<float> args_x() const;
	void setDomain_t(vec2 dom) { this->domain = dom; }
	void setDomain_x(vec2 dom) { for (auto &f : fn) f.setDomain(dom); }
	vec2 domain_x() const { return fn[0].getDomain(); }

	DiscreteRealFunctionR2 transpose() const;
	DiscreteRealFunction integrate_t() const;
	DiscreteRealFunction integrate_x() const;
	float double_integral() const;
	float double_integral(int i_t) const;
	float double_integral(int i_t, int i_x) const;
	DiscreteRealFunctionR2 double_convolve(const DiscreteRealFunctionR2 &kernel) const;
	DiscreteRealFunctionR2 convolve_x(const DiscreteRealFunction &kernel) const;
	DiscreteRealFunctionR2 convolve_x(const DiscreteRealFunctionR2 &kernel) const;
	DiscreteRealFunctionR2 smoothen_x(float L) const;
	DiscreteRealFunctionR2 smoothen_t(float L) const;
	DiscreteComplexFunctionR2 complexify() const;

	DiscreteComplexFunctionR2 fft_t () const;
	DiscreteComplexFunctionR2 fft_x () const;
	DiscreteComplexFunctionR2 fft() const;
	DiscreteComplexFunctionR2 fft(int var) const;;

	DiscreteRealFunctionR2 downsample_t(int factor) const;
	DiscreteRealFunctionR2 downsample_t(int factor, int shift) const;
	DiscreteRealFunctionR2 downsample_t_max(int factor) const;
	DiscreteRealFunctionR2 downsample_x(int factor) const;
	DiscreteRealFunctionR2 downsample_x(int factor, int shift) const;
	DiscreteRealFunctionR2 downsample_x_max(int factor) const;
	DiscreteRealFunctionR2 downsample(int factor_t, int factor_x) const;
	DiscreteRealFunctionR2 downsample_max(int factor_t, int factor_x) const;
};


DiscreteRealFunction loadSequence(const CodeFileDescriptor &file, vec2 domain);

inline float clamp(float x, float a=0, float b=1) { return std::max(a, std::min(b, x));}

RealFunction smoothenReLu(float c0, float x_change_to_id);
RealFunction expImpulse(float peak, float decay=1);
RealFunction expImpulse_k(float peak, float decay=1, float k=1);
RealFunction cubicShroom(float center, float margin);
RealFunction powerShroom(float begin, float end, float zeroExponent, float oneExponent);
RealFunction toneMap(float k);
RealFunction rationalInfiniteShroom(float steepFactor, float center);
RealFunction smoothstep(float x0, float x1);
RealFunction smootherstep(float x0, float x1);
RealFunctionR2 smoothUnion(float k);
RealFunctionR2 smoothSubtract(float k);
RealFunctionR2 smoothIntersect(float k);



const RealFunction SIN_R = RealFunction::sin();
const RealFunction COS_R = RealFunction::cos();
const RealFunction EXP_R = RealFunction::exp();
const RealFunction LOG_R = RealFunction::log();
const RealFunction SQRT_R = RealFunction::SQRT();
const RealFunction ONE_R = RealFunction::one();
const RealFunction X_R = RealFunction::linear(1, 0);
const RealFunction RELU = max(X_R, 0);
