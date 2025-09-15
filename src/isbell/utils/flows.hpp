#pragma once
#include <utility>

#include "func.hpp"

class VectorFieldR2;
typedef VectorField SteadyFlow;
typedef VectorFieldR2 PlaneSteadyFlow;

class VectorFieldR2 {
	Foo22 field;
	float eps = 0.01;
public:
	VectorFieldR2();
	VectorFieldR2(const VectorFieldR2 &other);
	VectorFieldR2(VectorFieldR2 &&other) noexcept;
	VectorFieldR2 & operator=(const VectorFieldR2 &other);
	VectorFieldR2 & operator=(VectorFieldR2 &&other) noexcept;
	explicit VectorFieldR2(Foo22 field, float eps=0.01);

	vec2 operator()(vec2 v) const;

	VectorFieldR2 operator+(const VectorFieldR2 &Y) const;
	VectorFieldR2 operator*(float a) const;
	VectorFieldR2 operator-() const;
	VectorFieldR2 operator-(const VectorFieldR2 &Y) const;
	VectorFieldR2 operator*(const RealFunctionR2 &f) const;
	friend VectorFieldR2 operator*(const mat2 &A, const VectorFieldR2 &X);

	RealFunctionR2 F_x() const;
	RealFunctionR2 F_y() const;
	std::array<RealFunctionR2, 2> components() const;
	vec2 normal(vec2 v) const;
	float speed(vec2 v) const;

	static VectorFieldR2 linearFlow(vec2 v);
	static VectorFieldR2 flowElement(vec2 point, float source_strength, float rotation_strength);
};



// R3 -> R3
class VectorField {
	Foo33 _X;
	Foo3Foo33 _dX;
	float eps;
public:
	VectorField();
	VectorField(Foo33 X, Foo3Foo33 dX, float eps=.01);
	explicit VectorField(Foo33 X, float eps=.01);
	VectorField(RealFunctionR3 Fx , RealFunctionR3 Fy, RealFunctionR3 Fz, float epsilon=0.01);
	explicit VectorField(Foo33 field);
	explicit VectorField(VectorFieldR2 f);
	VectorField operator+(const VectorField &Y) const;
	VectorField operator*(float a) const;
	VectorField operator-() const;
	VectorField operator-(const VectorField &Y) const;
	VectorField operator*(const RealFunctionR3 &f) const;
	RealFunctionR3 F_x() const;
	RealFunctionR3 F_y() const;
	RealFunctionR3 F_z() const;
	std::array<RealFunctionR3, 3> components() const;
	R3 operator()(R3 v) const;
	float getEps() const { return eps; }

	friend VectorField operator*(const mat3 &A, const VectorField &X);

	static VectorField constant(vec3 v);
	static VectorField linear(mat3 A);
	static VectorField radial(vec3 scale);

	RealFunctionR3 divergence() const;
	VectorField curl() const;
	RealFunctionR3 norm2() const;
	RealFunctionR3 norm() const;
	VectorField cross(const VectorField &Y) const;
	RealFunctionR3 dot(const VectorField &Y) const;
	vec3 moveAlong(vec3 v, float dt=.01) const;
};


inline VectorField RealFunctionR3::gradient() const { return VectorField(_df, eps); }
inline RealFunctionR3 RealFunctionR3::Laplacian() const { return gradient().divergence(); }


class Flow {
	BIHOM(vec3, float, vec3) F;
	float eps;
public:
	Flow();
	explicit Flow(BIHOM(vec3, float, vec3) X, float eps=.01);
	explicit Flow(HOM(float, SteadyFlow) pencil);
	explicit Flow(SteadyFlow steady_flow);
	Flow(ScalarField Fx, ScalarField Fy, ScalarField Fz, float epsilon=0.01);

	vec3 operator()(vec3 x, float t) const;
	SteadyFlow operator()(float t) const;
	SteadyFlow fix_time(float t) const;
	QUADHOM(float, float, float, float, vec3) separate_input_vector() const;

	Flow operator+(const Flow &Y) const;
	Flow operator*(float a) const;
	Flow operator-() const;
	Flow operator-(const Flow &Y) const;
	Flow operator*(const SteadyScalarField &f) const;
	Flow operator/(const SteadyScalarField &f) const;
	Flow operator*(const ScalarField &f) const;
	Flow operator/(const ScalarField &f) const;

	ScalarField F_x() const;
	ScalarField F_y() const;
	ScalarField F_z() const;

	Flow time_derivative() const;
	Flow spatial_partial(int i) const;
};



class DifferentialOperator{
	Flow coefs_X;
	ScalarField coef_t;
public:
	DifferentialOperator();
	DifferentialOperator(vec3 consts_X, float const_t);
	DifferentialOperator(const Flow &f_X, const ScalarField &f_t);
	DifferentialOperator(const ScalarField &f_x, const ScalarField &f_y, const ScalarField &f_z, const ScalarField &f_t);
	DifferentialOperator operator*(const ScalarField &f) const;
	DifferentialOperator operator*(const SteadyScalarField &f) const;
	DifferentialOperator operator*(float a) const;
	DifferentialOperator operator+(const DifferentialOperator &g) const;
	DifferentialOperator operator-(const DifferentialOperator &g) const;
	DifferentialOperator operator/(const ScalarField &f) const;
	DifferentialOperator operator/(const SteadyScalarField &f) const;

	ScalarField operator()(const ScalarField &f) const;
	SteadyScalarField operator()(const SteadyScalarField &f) const;
};

// const DifferentialOperator DABBA_T = DifferentialOperator(vec3(0, 0, 0), 1);
// const DifferentialOperator DABBA_X = DifferentialOperator(vec3(1, 0, 0), 0);
// const DifferentialOperator DABBA_Y = DifferentialOperator(vec3(0, 1, 0), 0);
// const DifferentialOperator DABBA_Z = DifferentialOperator(vec3(0, 0, 1), 0);
