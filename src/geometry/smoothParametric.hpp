#pragma once

#include "complexGeo.hpp"


class AffinePlane;
class SmoothParametricPlaneCurve;
class SmoothParametricSurface;
class AffineLine;
class RealFunctionPS;
class Differential1FormPS;
class Differential2FormPS;

class SmoothParametricCurve {
protected:
    Foo13 _f;
	Foo13 _df;
	Foo13 _ddf;
	std::function<Foo13(int)> _der_higher = [this](int n){return n == 1 ? _df : n == 2 ? _ddf : derivativeOperator(_der_higher(n-1), this->eps);};
	float eps;
    float t0;
    float t1;
    bool periodic;
    PolyGroupID id;
public:
    SmoothParametricCurve(Foo13 f, Foo13 df, Foo13 ddf, PolyGroupID id=DFLT_CURV, float t0=0, float t1=TAU, bool periodic=true, float epsilon=0.01f);
	SmoothParametricCurve(Foo13 f, Foo13 df, PolyGroupID id=DFLT_CURV,  float t0=0, float t1=TAU, bool periodic=true, float epsilon=0.01f);
    explicit SmoothParametricCurve(const Foo13 &f, PolyGroupID id=DFLT_CURV,  float t0=0, float t1=TAU, bool periodic=true, float epsilon=0.01f) : SmoothParametricCurve(f, derivativeOperator(f, epsilon), id, t0, t1, periodic, epsilon) {}
	SmoothParametricCurve(const Foo13 &f,   vec2 dom, PolyGroupID id=DFLT_CURV, bool periodic=false, float epsilon=0.01f) : SmoothParametricCurve(f, derivativeOperator(f, epsilon), id, dom.x, dom.y, periodic, epsilon) {}

	SmoothParametricCurve( const RealFunction& fx, const RealFunction& fy, const RealFunction& fz, PolyGroupID id=DFLT_CURV, float t0=0, float t1=TAU, bool periodic=true, float epsilon=0.01f);
    SmoothParametricCurve(Foo13 f, std::function<Foo13(int)> derivativeOperator, PolyGroupID id=DFLT_CURV, float t0=0, RP1 t1=TAU, bool periodic=true, float epsilon=0.01f);
    SmoothParametricCurve(Foo13 f, std::vector<Foo13> derivatives, PolyGroupID id=DFLT_CURV, float t0=0, float t1=TAU, bool periodic=true, float epsilon=0.01f);
    SmoothParametricCurve(const SmoothParametricCurve &other);
    SmoothParametricCurve(SmoothParametricCurve &&other) noexcept;
    SmoothParametricCurve &operator=(const SmoothParametricCurve &other);
    SmoothParametricCurve &operator=(SmoothParametricCurve &&other) noexcept;



    PolyGroupID getID() const { return id; }
    void setID(PolyGroupID id) { this->id = id; }
    void copyID (const SmoothParametricCurve &other) { this->id = other.id; }

	SmoothParametricCurve shift(vec3 v) const {return precompose(SpaceEndomorphism::translation(v));}
	SmoothParametricCurve rotate(vec3 axis, float angle, vec3 center=ORIGIN) const {return precompose(SpaceAutomorphism::rotation(axis, angle).applyWithShift(center));}
	SmoothParametricCurve scale(float a, vec3 center=ORIGIN) const {return precompose(SpaceAutomorphism::scaling(a, center));}


	vec3 derivative(float t) const { return _df(t); }
	vec3 df(float t) const { return derivative(t); }
    vec3 operator()(float t) const;
    vec2 bounds() const { return vec2(t0, t1); }
    float getT0() const { return t0; }
    float getT1() const { return t1; }

	vec3 second_derivative(float t) const { return _ddf(t); }
	vec3 higher_derivative(float t, int n) const { return _der_higher(n)(t); }
	vec3 ddf(float t) const { return second_derivative(t); }
	vec3 tangent(float t) const;
	vec3 binormal(float t) const;
	vec3 normal(float t) const;
	float length(float t0, float t1, int n) const;

	SmoothParametricCurve precompose(SpaceEndomorphism g_) const;
	void precomposeInPlace(SpaceEndomorphism g);

	mat3 FrenetFrame(float t) const;
	float curvature(float t) const;
    float curvature_radius(float t) const { return 1/curvature(t); }
	float torsion(float t) const;
	vec3 curvature_vector(float t) const;
    float speed(float t) const { return norm(df(t));}

    bool isPeriodic() const { return periodic; }
    float getEps() const { return eps; }
    AffinePlane osculatingPlane(float t) const;

	SmoothParametricSurface surfaceOfRevolution(const AffineLine& axis) const;
	SmoothParametricSurface screwMotion(float speed, int iterations) const;
	SmoothParametricSurface cylinder(vec3 direction, float h) const;
	SmoothParametricSurface pipe(float radius, bool useFrenetFrame = true) const;
	SmoothParametricSurface pipe(float radius, float eps, bool useFrenetFrame = true) const;
	SmoothParametricSurface canal(const std::function<float(float)> &radius) const;

	static SmoothParametricCurve constCurve(vec3 v);
	static SmoothParametricCurve span(vec3 p1, vec3 p2);
};





class AffineLine : public SmoothParametricCurve {
	vec3 p0, v; // p0 + tv
public:
	AffineLine(vec3 p0, vec3 v);
	static AffineLine spanOfPts(vec3 p0, vec3 p1);
	float distance(vec3 p) const;
	RealFunctionR3 distanceField() const;
	bool contains(vec3 p, float eps=1e-6) const;
	vec3 orthogonalProjection(vec3 p) const;
	vec3 pivot() const;
	vec3 direction() const;
	float distance(AffineLine &l) const;
	AffineLine operator+(vec3 v) const;
	SmoothParametricSurface tube(float radius, float t0, float t1) const;
};



// R2 -> R3
class SmoothParametricSurface {
  Foo113 _f;
  Foo113 _df_t;
  Foo113 _df_u;
  float t0, t1, u0, u1;
  bool t_periodic, u_periodic;
  float epsilon;
  PolyGroupID id = 420;
  SmoothParametricCurve precompose(const SmoothParametricPlaneCurve& c, PolyGroupID id=420) const;
  SmoothParametricSurface postcompose(const SpaceEndomorphism& g) const;
	SmoothParametricSurface precompose(const PlaneSmoothEndomorphism& g, vec2 t_bounds, vec2 u_bounds) const;
	SmoothParametricSurface precompose(const PlaneAutomorphism& g) const;
public:
  SmoothParametricSurface(const Foo113& f, const Foo113& df_t, const Foo113& df_u, vec2 t_range, vec2 u_range, bool t_periodic=false, bool u_periodic=false, float epsilon=.01);
  SmoothParametricSurface(const Foo113& f, vec2 t_range, vec2 u_range, bool t_periodic=false, bool u_periodic=false, float epsilon=.01);
  SmoothParametricSurface(const std::function<SmoothParametricCurve(float)>& pencil, vec2 t_range, vec2 u_range, bool t_periodic=false, bool u_periodic=false, float eps=.01);
  SmoothParametricSurface(RealFunctionR2 plot, vec2 t_range, vec2 u_range);

  vec3 operator()(float t, float s) const;
  vec3 operator()(vec2 tu) const;
  vec3 parametersNormalised(vec2 tu) const { return operator()(t0 + tu.x*(t1-t0), u0 + tu.y*(u1-u0)); }
  vec3 parametersNormalised(float t, float u) const { return operator()(t0 + t*(t1-t0), u0 + u*(u1-u0)); }

	SmoothParametricSurface operator+(const SmoothParametricSurface &S) const;
	SmoothParametricSurface operator*(float a) const;
	SmoothParametricSurface operator-(const SmoothParametricSurface &S) const { return *this + S*(-1); }
	SmoothParametricSurface normaliseParameters() const;

  friend SmoothParametricCurve operator & (const SmoothParametricSurface &S, const SmoothParametricPlaneCurve &c) { return S.precompose(c); }
	friend SmoothParametricSurface operator & (const SpaceEndomorphism &f, const SmoothParametricSurface &S) { return S.postcompose(f); }
	friend SmoothParametricSurface operator & (const SmoothParametricSurface &S, const PlaneAutomorphism &c) { return S.precompose(c); }

	SmoothParametricSurface shift(vec3 v) const;
	SmoothParametricSurface rotate(vec3 axis, float angle, vec3 center=ORIGIN) const;
	SmoothParametricSurface scale(float a, vec3 center=ORIGIN) const;

	SmoothParametricCurve restrictToInterval(vec2 p0, vec2 p1, PolyGroupID id=420) const;
	SmoothParametricCurve restrictToInterval(vec2 p0, vec2 p1, bool periodic, PolyGroupID id=420) const;

	SmoothParametricCurve constT(float t0) const;
	SmoothParametricCurve constU(float u0) const;


  vec2 boundsT() const { return vec2(t0, t1); }
    vec2 boundsU() const { return vec2(u0, u1); }
    float tMin() const { return t0; }
    float tMax() const { return t1; }
    float uMin() const { return u0; }
    float uMax() const { return u1; }
    bool isPeriodicT() const { return t_periodic; }
    bool isPeriodicU() const { return u_periodic; }
    float periodT() const { return t_periodic ? t1 - t0 : 0; }
    float periodU() const { return u_periodic ? u1 - u0 : 0; }

    vec3 normal(float t, float s) const;
    vec3 normal(vec2 tu) const { return normal(tu.x, tu.y); }

	void changeDomain(vec2 t_range, vec2 u_range, bool t_periodic, bool u_periodic);



  mat2 metricTensor(float t, float s) const;
	EuclideanSpace<vec2, mat2> toTangentStdBasis(float t, float s, vec3 v) const;
	vec3 embeddTangentVector(float t, float s, EuclideanSpace<vec2, mat2> v) const;

	mat3 DarbouxFrame(float t, float s) const;
	mat2x3 tangentStandardBasis(float t, float s ) const { return mat2x3(_df_t(t, s), _df_u(t, s)); }


	mat2 changeOfTangentBasisToPrincipal(float t, float s) const;
	mat2 changeOfPrincipalBasisToStandard(float t, float s) const;
	mat2x3 tangentSpacePrincipalBasis(float t, float s) const;


	mat2 firstFundamentalForm(float t, float s) const;
	mat2 secondFundamentalForm(float t, float s) const;
	float _E(float t, float u) const;
	float _F(float t, float u) const;
	float _G(float t, float u) const;
	float _L(float t, float u) const;
	float _M(float t, float u) const;
	float _N(float t, float u) const;
	vec3 d2f_tt(float t, float u) const;
	vec3 d2f_uu(float t, float u) const;
	vec3 d2f_tu(float t, float u) const;


	vec3 normalCurvature(float t, float s, vec2 v) const;
	mat2 shapeOperator(float t, float s) const;
	float meanCurvature(float t, float s) const;
	float gaussianCurvature(float t, float s) const;
	mat2x3 principalDirections(float t, float s) const;
	std::pair<float, float> principalCurvatures(float t, float s) const;
	void normaliseDomainToI2();
	vec3 Laplacian(float t, float s) const;

	float meanCurvature(vec2 tu) const { return meanCurvature(tu.x, tu.y); }
	float globalAreaIntegral(const RealFunctionPS &f) const;
	float DirichletFunctional() const;
	float biharmonicFunctional() const;


	SmoothParametricSurface meanCurvatureFlow(float dt) const;
};

SmoothParametricSurface ruledSurfaceJoinT(const SmoothParametricCurve &c1, const SmoothParametricCurve &c2, float u0=0, float u1=1);
SmoothParametricSurface ruledSurfaceJoinU(const SmoothParametricCurve &c1, const SmoothParametricCurve &c2, float t0=0, float t1=1);
inline SmoothParametricSurface ruledSurfaceJoinT(const SmoothParametricCurve &c1, const SmoothParametricCurve &c2, vec2 bounds) { return ruledSurfaceJoinT(c1, c2, bounds.x, bounds.y); }
inline SmoothParametricSurface ruledSurfaceJoinU(const SmoothParametricCurve &c1, const SmoothParametricCurve &c2, vec2 bounds) { return ruledSurfaceJoinU(c1, c2, bounds.x, bounds.y); }
SmoothParametricSurface bilinearSurface(const vec3 &p00, const vec3 &p01, const vec3 &p10, const vec3 &p11, vec2 t_range, vec2 u_range);
SmoothParametricSurface CoonsPatch(const SmoothParametricCurve &cDown, const SmoothParametricCurve &cLeft, const SmoothParametricCurve &cUp, const SmoothParametricCurve &cRight);
SmoothParametricSurface cone(const SmoothParametricCurve &c, float h, float r);
SmoothParametricSurface polarCone(const SmoothParametricCurve &r, vec3 center);


inline SmoothParametricSurface CoonsPatchDisjoint(const SmoothParametricCurve &c1, const SmoothParametricCurve &c2) {
	SmoothParametricCurve c_bd=SmoothParametricCurve::span(c1(c1.bounds().x), c2(c2.bounds().x));
	return CoonsPatch(c1, c_bd, c2, c_bd);
}


class SurfaceParametricPencil {
    HOM(float, SmoothParametricSurface) pencil;
public:
	explicit SurfaceParametricPencil(const HOM(float, SmoothParametricSurface) &pencil) : pencil(pencil) {}
	explicit SurfaceParametricPencil(const BIHOM(float, vec2, vec3) &foo, vec2 range_t=vec2(0, TAU), vec2 range_u=vec2(0, TAU), float eps=.01);

	SmoothParametricSurface operator()(float t) const { return pencil(t); }
	vec3 operator()(float t, float u, float s) const { return pencil(t)(u, s); }
	vec3 operator()(float t, vec2 us) const { return pencil(t)(us); }
};


class CurveParametricPencil {
	HOM(float, SmoothParametricCurve) pencil;
public:
	explicit CurveParametricPencil(const HOM(float, SmoothParametricCurve) &pencil) : pencil(pencil) {}
	explicit CurveParametricPencil(BIHOM(float, float, vec3) foo, vec2 bounds=vec2(0, 1), float eps=.01);

	SmoothParametricCurve operator()(float t) const { return pencil(t); }
	vec3 operator()(float t, float u) const { return pencil(t)(u); }
};


class ParametricSurfaceFoliation {
	HOM(float, SmoothParametricCurve) pencil_of_leaves;
	vec2 pencil_domain;
	bool pencil_periodic;
	vector<SmoothParametricCurve> special_leaves;
public:
	explicit ParametricSurfaceFoliation(HOM(float, SmoothParametricCurve) pencil, vec2 pencil_domain=vec2(0, 1), bool periodic=true, const vector<SmoothParametricCurve> &special_leaves={});

	SmoothParametricCurve getLeaf(float t) const { return pencil_of_leaves(t); }
	SmoothParametricCurve getSpecialLeaf(int i) const { return special_leaves[i];}

	SmoothParametricSurface getFoliatedSurface() const;
	vector<SmoothParametricCurve> sampleLeaves(int res) const;
	vector<SmoothParametricCurve> getSpecialLeaves() const { return special_leaves; }
	vec2 getDomain() const { return pencil_domain; }
};


class Differential1FormPS;

class RealFunctionPS {
    Foo111 _f;
    Foo112 _df;
    shared_ptr<SmoothParametricSurface> surface;
public:
    RealFunctionPS(const std::function<float(float, float)> &f, const shared_ptr<SmoothParametricSurface> &surface) : _f(f), surface(surface) {}
    RealFunctionPS(const Foo21 &f, const shared_ptr<SmoothParametricSurface> &surface) : surface(surface), _f( pack(f, f, vec2, float)), _df(pack(f, derivativeOperator(f, .01), vec2, float)) {}
    RealFunctionPS(const Foo31 &emb_pullback, const shared_ptr<SmoothParametricSurface> &surface);
    float operator()(float t, float s) const;

    static RealFunctionPS constant(float c, const shared_ptr<SmoothParametricSurface> &surface) { return RealFunctionPS([c](float, float) { return c; }, surface); }
    RealFunctionPS constant(float c) const { return constant(c, surface); }

    RealFunctionPS operator*(float a) const { return RealFunctionPS([f=_f, a](float t, float s) { return f(t, s)*a; }, surface); }
    RealFunctionPS operator/(float a) const { return (*this)*(1/a); }
    RealFunctionPS operator-() const { return (*this)*(-1); }
    RealFunctionPS operator+(const RealFunctionPS& f) const { return RealFunctionPS([f1=_f, f2=f._f](float t, float s) { return f1(t, s) + f2(t, s); }, surface); }
    RealFunctionPS operator-(const RealFunctionPS& f) const { return *this + (-f); }
    RealFunctionPS operator*(const RealFunctionPS& f) const { return RealFunctionPS([f1=_f, f2=f._f](float t, float s) { return f1(t, s) * f2(t, s); }, surface); }
    RealFunctionPS operator/(const RealFunctionPS& f) const { return RealFunctionPS([f1=_f, f2=f._f](float t, float s) { return f1(t, s) / f2(t, s); }, surface); }

    friend RealFunctionPS operator/(float a, const RealFunctionPS& g) { return g.constant(a)/g; }
    friend RealFunctionPS operator+(float a, const RealFunctionPS& g) { return g.constant(a)+g; }
    friend RealFunctionPS operator-(float a, const RealFunctionPS& g) { return g.constant(a)-g; }
    friend RealFunctionPS operator*(float a, const RealFunctionPS& g) { return g.constant(a)*g; }

    Differential1FormPS df();
};

template <typename V>
class Linear1Form2D {
    V v1, v2;
    vec2 coefs;
public:
    Linear1Form2D(vec2 omega, V basis1, V basis2) : v1(basis1), v2(basis2), coefs(omega) {}
    float operator()(V v) const { return dot(vec2(glm::dot(v, v1), glm::dot(v, v2)), coefs); }
    Linear1Form2D operator*(float a) const { return Linear1Form2D(coefs*a, v1, v2 ); }
    Linear1Form2D operator/(float a) const { return Linear1Form2D(coefs/a, v1, v2 ); }
    Linear1Form2D operator+(const Linear1Form2D &other) const { return Linear1Form2D(coefs + other.coefs, v1, v2); }
    Linear1Form2D operator-(const Linear1Form2D &other) const { return Linear1Form2D(coefs - other.coefs, v1, v2); }

    static Linear1Form2D dual(V v, V v1, V v2) { return Linear1Form2D(vec2(glm::dot(v, v1), glm::dot(v, v2)), v1, v2); }
    static Linear1Form2D dx(V x, V y) { return Linear1Form2D(vec2(1, 0), x, y); }
    static Linear1Form2D dy(V x, V y) { return Linear1Form2D(vec2(0, 1), x, y); }
    static std::pair<Linear1Form2D, Linear1Form2D> basisForms (V v1, V v2) { return {dx(v1, v2), dy(v1, v2)}; }
    vec2 localCoefs() const { return coefs; }
};

template <typename V>
class Linear2Form2D {
    V v1, v2;
    float coef;
public:
    Linear2Form2D(float c, V basis1, V basis2) : v1(basis1), v2(basis2), coef(c) {}
    float operator()(V a, V b) const { return coef*glm::dot(a, v1)*glm::dot(b, v2); }
    Linear2Form2D operator*(float a) const { return Linear2Form2D(coef*a, v1, v2 ); }
    Linear2Form2D operator/(float a) const { return Linear2Form2D(coef/a, v1, v2 ); }
    Linear2Form2D operator+(const Linear2Form2D &other) const { return Linear2Form2D(coef + other.coef, v1, v2); }
    Linear2Form2D operator-(const Linear2Form2D &other) const { return Linear2Form2D(coef - other.coef, v1, v2); }

    float localCoef() const { return coef; }
};


class Differential2FormPS;

class Differential1FormPS {
    std::function<Linear1Form2D<vec3>(float, float)> _omega;
    shared_ptr<SmoothParametricSurface> surface;
public:
    Differential1FormPS(const std::function<Linear1Form2D<vec3>(float, float)> &omega, const shared_ptr<SmoothParametricSurface> &surface) : _omega(omega), surface(surface) {}
    Differential1FormPS(const std::function<Linear1Form2D<vec3>(vec2)> &omega, const shared_ptr<SmoothParametricSurface> &surface) : _omega(pack(omega, omega, vec2, float)), surface(surface) {}
    Differential1FormPS(const std::function<Linear1Form2D<vec3>(vec3)> &emb_pullback, const shared_ptr<SmoothParametricSurface> &surface);
    Linear1Form2D<vec3> operator()(float t, float s) const { return _omega(t, s); }
    Linear1Form2D<vec3> operator()(vec2 tu) const { return _omega(tu.x, tu.y); }
    float operator()(float t, float s, vec3 v) const { return _omega(t, s)(v); }
    float operator()(vec2 tu, vec3 v) const { return unpack(w=_omega, w, vec2)(tu)(v); }
    Differential1FormPS operator*(float a) const { return Differential1FormPS([w=_omega, a](float t, float s) { return w(t, s)*a; }, surface); }
    Differential1FormPS operator/(float a) const { return (*this)*(1/a); }
    Differential1FormPS operator-() const { return (*this)*(-1); }
    Differential1FormPS operator+(const Differential1FormPS& eta) const { return Differential1FormPS([w1=_omega, w2=eta._omega](float t, float s) { return w1(t, s) + w2(t, s); }, surface); }
    Differential1FormPS operator-(const Differential1FormPS& eta) const {return *this + (-eta); }
    Differential1FormPS operator*(const RealFunctionPS& f) const { return Differential1FormPS([w=_omega, f](float t, float s) { return w(t, s)*f(t, s); }, surface); }
    Differential1FormPS operator/(const RealFunctionPS& f) const { return Differential1FormPS([w=_omega, f](float t, float s) { return w(t, s)/f(t, s); }, surface); }

    // Differential1FormPS<glm::vec3> d() const { return Differential1FormPS<glm::vec3>([w=_omega](float t, float s) { return w(t, s).v1; }, surface); }
};

class Differential2FormPS {
    BIHOM(float, float, Linear2Form2D<vec3>) _omega;
    shared_ptr<SmoothParametricSurface> surface;
public:
    Differential2FormPS(const BIHOM(float, float, Linear2Form2D<vec3>) &omega, const shared_ptr<SmoothParametricSurface> &surface) : _omega(omega), surface(surface) {}
    Differential2FormPS(const HOM(vec2, Linear2Form2D<vec3>) &omega, const shared_ptr<SmoothParametricSurface> &surface) : _omega([omega](float t, float s) { return omega(vec2(t, s)); }), surface(surface) {}
    Differential2FormPS(const HOM(vec3, Linear2Form2D<vec3>) &emb_pullback, const shared_ptr<SmoothParametricSurface> &surface);
    Linear2Form2D<vec3> operator()(float t, float s) const;
    Linear2Form2D<vec3> operator()(vec2 tu) const;

    float operator()(float t, float s, vec3 v, vec3 w) const { return _omega(t, s)(v, w); }
    float operator()(vec2 tu, vec3 v, vec3 w) const { return [om=_omega](vec2 tu, vec3 v, vec3 w) { return om(tu.x, tu.y)(v, w); }(tu, v, w); }

    Differential2FormPS operator*(float a) const { return Differential2FormPS([w=_omega, a](float t, float s) { return w(t, s)*a; }, surface); }
    Differential2FormPS operator/(float a) const { return (*this)*(1/a); }
    Differential2FormPS operator-() const { return (*this)*(-1); }
    Differential2FormPS operator+(const Differential2FormPS& eta) const { return Differential2FormPS([w1=_omega, w2=eta._omega](float t, float s) { return w1(t, s) + w2(t, s); }, surface); }
    Differential2FormPS operator-(const Differential2FormPS& eta) const {return *this + (-eta); }
    Differential2FormPS operator*(const RealFunctionPS& f) const { return Differential2FormPS([w=_omega, f](float t, float s) { return w(t, s)*f(t, s); }, surface); }
    Differential2FormPS operator/(const RealFunctionPS& f) const { return Differential2FormPS([w=_omega, f](float t, float s) { return w(t, s)/f(t, s); }, surface); }
};

class VectorFieldPS {
	std::function<vec3(float, float)> _f_dt;
	std::function<vec3(float, float)> _f_ds;
	shared_ptr<SmoothParametricSurface> surface;
	public:
	VectorFieldPS(const std::function<vec3(float, float)> &f_dt, const std::function<vec3(float, float)> &f_du, const shared_ptr<SmoothParametricSurface> &surface) : _f_dt(f_dt), _f_ds(f_du), surface(surface) {}
	vec3 operator()(float t, float s) const { return surface->tangentStandardBasis(t, s)[0]*_f_dt(t, s) + surface->tangentStandardBasis(t, s)[1]*_f_ds(t, s); }
	VectorFieldPS operator*(float a) const { return VectorFieldPS([f=_f_dt, a](float t, float s) { return f(t, s)*a; }, [f=_f_ds, a](float t, float s) { return f(t, s)*a; }, surface); }
	VectorFieldPS operator/(float a) const { return (*this)*(1/a); }
	VectorFieldPS operator+(const VectorFieldPS& v) const { return VectorFieldPS([f1=_f_dt, f2=v._f_dt](float t, float s) { return f1(t, s) + f2(t, s); }, [f1=_f_ds, f2=v._f_ds](float t, float s) { return f1(t, s) + f2(t, s); }, surface); }
	VectorFieldPS operator-(const VectorFieldPS& v) const { return *this + (-v); }
	VectorFieldPS operator-() const { return *this*(-1); }
	VectorFieldPS operator*(const RealFunctionPS& f) const { return VectorFieldPS([f1=_f_dt, f](float t, float s) { return f(t, s)*f1(t, s); }, [f2=_f_ds, f](float t, float s) { return f(t, s)*f2(t, s); }, surface); }
	VectorFieldPS operator/(const RealFunctionPS& f) const { return VectorFieldPS([f1=_f_dt, f](float t, float s) { return f1(t, s)/f(t, s); }, [f2=_f_ds, f](float t, float s) { return f2(t, s)/f(t, s); }, surface); }
	vec2 deform(vec2 tu, vec2 dv) const { return tu + dv.x*vec2(_f_dt(tu.x, tu.y)) + dv.y*vec2(_f_ds(tu.x, tu.y)); }
	vec3 shiftAmbient(vec2 tu, vec2 dv) const { return (*surface)(deform(tu, dv)) - (*surface)(tu); }
//	vector<vec3> generate_trajectory(vec2 tu, vec2 v, int n, float h) const { return RK4([v, h, s=surface](float t, vec3 w) { return (*s)(t*v); }, 0, (*surface)(v), h).solution(n); }
};





class FunctionalPartitionOfUnity {
	std::vector<Fooo> _F_i;
public:
	explicit  FunctionalPartitionOfUnity(const std::vector<Fooo>& F_i) : _F_i(F_i) {}
	Fooo operator[](int i) const { return _F_i[i]; }
	int size() const { return _F_i.size(); }
};





inline Fooo BernsteinPolynomial(int n, int i, float t0, float t1) { return [n, i, t0, t1](float t) { return binomial(n, i)*std::pow(t-t0, i)*std::pow(t1-t, n-i)/std::pow(t1-t0, n); }; }
inline Fooo BernsteinPolynomial(int n, int i) { return [n, i](float t) { return binomial(n, i)*std::pow(t, i)*std::pow(1-t, n-i); }; }

FunctionalPartitionOfUnity BernsteinBasis(int n);
FunctionalPartitionOfUnity BernsteinBasis(int n, float t0, float t1);

inline Fooo BSpline(int i, int k, const std::vector<float>& knots);
FunctionalPartitionOfUnity BSplineBasis(int n, int k, const std::vector<float>& knots);
vector<float> uniformKnots(int n, int k);

SmoothParametricCurve freeFormCurve(const FunctionalPartitionOfUnity& family, const std::vector<vec3>& controlPts, vec2 domain, float eps=0.0001);

inline SmoothParametricCurve BezierCurve(const std::vector<vec3>& controlPoints, float t0=0, float t1=1, float eps=.001) {
	return freeFormCurve(BernsteinBasis(controlPoints.size()-1, t0, t1), controlPoints, vec2(t0, t1), eps);
}

SmoothParametricCurve BSplineCurve(const std::vector<vec3>& controlPoints, const std::vector<float>& knots, int k, float eps=.001);

inline SmoothParametricCurve BSplineCurve(const std::vector<vec3>& controlPoints, float t0, float t1, int k, float eps=.001) {
	return BSplineCurve(controlPoints, uniformKnots(controlPoints.size()-1, k), k, eps);
}

SmoothParametricSurface freeFormSurface(const FunctionalPartitionOfUnity &F_i, const FunctionalPartitionOfUnity &G_i, const std::vector<std::vector<vec3>> &controlPts, vec2 range_t, vec2 range_u, float eps=0.01);

inline SmoothParametricSurface BezierSurface(const std::vector<std::vector<vec3>> &controlPoints, float t0=0, float t1=1, float u0=0, float u1=1, float eps=.01) {
	return freeFormSurface(BernsteinBasis(controlPoints.size()-1, t0, t1), BernsteinBasis(controlPoints[0].size()-1, u0, u1), controlPoints, vec2(t0, t1), vec2(u0, u1), eps);
}

SmoothParametricSurface BSplineSurface(const std::vector<std::vector<vec3>> &controlPoints, const std::vector<float> &knots_t, const std::vector<float> &knots_u, int k=3, float eps=.01);
inline SmoothParametricSurface BSplineSurfaceUniform(const std::vector<std::vector<vec3>> &controlPoints, vec2 t_range, vec2 u_range, int k=3, float eps=.01) {
	return BSplineSurface(controlPoints, linspace(t_range.x, t_range.y, controlPoints.size()+k+1), linspace(u_range.x, u_range.y, controlPoints[0].size()+k+1), k, eps);
}
