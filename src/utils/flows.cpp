#include "flows.hpp"

#include "monads.hpp"



VectorFieldR2::VectorFieldR2()
: field([](vec2 v) {
	return vec2(0, 0);
}) {}


vec2 VectorFieldR2::operator()(vec2 v) const {
	return field(v);
}

vec2 VectorFieldR2::normal(vec2 x) const {
	return normalise(vec2(-field(x).y, field(x).x));
}

float VectorFieldR2::speed(vec2 x) const {
	return length(field(x));
}


VectorField::VectorField(std::function<vec3(vec3)> X, std::function<mat3(vec3)> dX, float eps)
: _X(X),
  _dX(std::move(dX)),
  eps(eps) {}

VectorField VectorField::operator-() const {
	return *this * -1;
}

VectorField VectorField::operator-(const VectorField &Y) const {
	return *this + (-Y);
}

RealFunctionR3 VectorField::F_x() const {
	return RealFunctionR3([this](vec3 x) {
							  return _X(x).x;
						  }, [this](vec3 x) {
							  return _dX(x)[0];
						  });
}

RealFunctionR3 VectorField::F_y() const {
	return RealFunctionR3([this](vec3 x) {
							  return _X(x).y;
						  }, [this](vec3 x) {
							  return _dX(x)[1];
						  });
}

RealFunctionR3 VectorField::F_z() const {
	return RealFunctionR3([this](vec3 x) {
							  return _X(x).z;
						  }, [this](vec3 x) {
							  return _dX(x)[2];
						  });
}

std::array<RealFunctionR3, 3> VectorField::components() const {
	return {F_x(), F_y(), F_z()};
}

glm::vec3 VectorField::operator()(glm::vec3 v) const {
	return _X(v);
}

VectorField operator*(const mat3 &A, const VectorField &X) {
	return VectorField([f=X._X, A](vec3 v) {
						   return A * f(v);
					   }, [df=X._dX, A](vec3 v) {
						   return A * df(v);
					   }, X.eps);
}

RealFunctionR3 VectorField::norm() const {
	return norm2().sqrt();
}

VectorField VectorField::cross(const VectorField &Y) const {
	return VectorField(F_y() * Y.F_z() - F_z() * Y.F_y(), F_z() * Y.F_x() - F_x() * Y.F_z(), F_x() * Y.F_y() - F_y() * Y.F_x());
}

RealFunctionR3 VectorField::dot(const VectorField &Y) const {
	return F_x() * Y.F_x() + F_y() * Y.F_y() + F_z() * Y.F_z();
}

vec3 VectorField::moveAlong(vec3 v, float dt) const {
	return v + _X(v) * dt;
}

ScalarField DifferentialOperator::operator()(const ScalarField &f) const {
	return ScalarField([f_X=coefs_X, f_t=coef_t, F=f](vec3 x, float t) {
		vec3 a_i = f_X(x, t);
		float c_t = f_t(x, t);
		return a_i.x * F.spatial_partial(0)(x, t) + a_i.y * F.spatial_partial(1)(x, t) + a_i.z * F.spatial_partial(2)(x, t) + c_t * F.time_derivative()(x, t);
	});
}

SteadyScalarField DifferentialOperator::operator()(const SteadyScalarField &f) const { return (*this)(ScalarField(f))(0); }



VectorFieldR2::VectorFieldR2(const VectorFieldR2 &other): field(other.field), eps(other.eps) {}

VectorFieldR2::VectorFieldR2(VectorFieldR2 &&other) noexcept: field(std::move(other.field)), eps(other.eps) {}

VectorFieldR2 & VectorFieldR2::operator=(const VectorFieldR2 &other) { field = other.field; eps = other.eps; return *this; }

VectorFieldR2 & VectorFieldR2::operator=(VectorFieldR2 &&other) noexcept { field = std::move(other.field); eps = other.eps; return *this; }

VectorFieldR2::VectorFieldR2(Foo22 field, float eps): field(field), eps(eps) {}


VectorFieldR2 VectorFieldR2::operator+(const VectorFieldR2 &Y) const {
	return VectorFieldR2([f=field, g=Y.field](vec2 v) {
		return f(v) + g(v);
	}, eps);
}

VectorFieldR2 VectorFieldR2::operator*(float a) const {
	return VectorFieldR2([f=field, a](vec2 v) {
		return f(v) * a;
	}, eps);
}

VectorFieldR2 VectorFieldR2::operator-() const {
	return VectorFieldR2([f=field](vec2 v) {
		return -f(v);
	}, eps);
}

VectorFieldR2 VectorFieldR2::operator-(const VectorFieldR2 &Y) const {
	return *this + (-Y);
}



VectorFieldR2 VectorFieldR2::operator*(const RealFunctionR2 &f) const {
	return VectorFieldR2([X=field, g=f](vec2 v) {
		return X(v) * g(v);
	}, eps);
}

RealFunctionR2 VectorFieldR2::F_x() const {
	return RealFunctionR2([field=field](vec2 v) {
							  return field(v).x;
						  }, [field=field](vec2 v) {
							  return vec2(field(v).x, 0);
						  }, eps);
}

RealFunctionR2 VectorFieldR2::F_y() const {
	return RealFunctionR2([field=field](vec2 v) {
							  return field(v).y;
						  }, [field=field](vec2 v) {
							  return vec2(0, field(v).y);
						  }, eps);
}

std::array<RealFunctionR2, 2> VectorFieldR2::components() const {
	return {F_x(), F_y()};
}

VectorFieldR2 VectorFieldR2::linearFlow(vec2 v) { return VectorFieldR2([v](vec2 x) { return v; }); }

VectorFieldR2 VectorFieldR2::flowElement(vec2 point, float source_strength, float rotation_strength) {
	mat2 M = mat2(source_strength, -rotation_strength, rotation_strength, source_strength);
	return M*VectorFieldR2([point](vec2 x) { return (x - point)/dot(x-point, x-point); }, .01);
}

VectorFieldR2 operator*(const mat2 &A, const VectorFieldR2 &X) {
	return VectorFieldR2([A, X](vec2 v) {
		return A * X(v);
	}, X.eps);
}

DifferentialOperator::DifferentialOperator(): DifferentialOperator(vec3(0), 0) {}

DifferentialOperator::DifferentialOperator(vec3 consts_X, float const_t): coefs_X(Flow([consts_X](vec3, float) { return consts_X; })), coef_t(ScalarField([const_t](vec3, float) { return const_t; })) {}

DifferentialOperator::DifferentialOperator(const Flow &f_X, const ScalarField &f_t): coefs_X(f_X), coef_t(f_t) {}

DifferentialOperator::DifferentialOperator(const ScalarField &f_x, const ScalarField &f_y, const ScalarField &f_z, const ScalarField &f_t): coefs_X(f_x, f_y, f_z), coef_t(f_t) {}

DifferentialOperator DifferentialOperator::operator*(const ScalarField &f) const { return DifferentialOperator(coefs_X * f, coef_t * f); }

DifferentialOperator DifferentialOperator::operator*(const SteadyScalarField &f) const { return DifferentialOperator(coefs_X * f, coef_t * f); }

DifferentialOperator DifferentialOperator::operator*(float a) const { return DifferentialOperator(coefs_X * a, coef_t * a); }

DifferentialOperator DifferentialOperator::operator+(const DifferentialOperator &g) const { return DifferentialOperator(coefs_X + g.coefs_X, coef_t + g.coef_t); }

DifferentialOperator DifferentialOperator::operator-(const DifferentialOperator &g) const { return DifferentialOperator(coefs_X - g.coefs_X, coef_t - g.coef_t); }

DifferentialOperator DifferentialOperator::operator/(const ScalarField &f) const { return DifferentialOperator(coefs_X / f, coef_t / f); }

DifferentialOperator DifferentialOperator::operator/(const SteadyScalarField &f) const { return DifferentialOperator(coefs_X / f, coef_t / f); }


RealFunctionR3 VectorField::norm2() const {
	return F_x() * F_x() + F_y() * F_y() + F_z() * F_z();
}



VectorField::VectorField()
: eps(0.01) {
	_X = [](vec3 v) {
		return vec3(0, 0, 0);
	};
}

VectorField::VectorField(Foo33 X, float eps)
: _X(std::move(X)),
  eps(eps) {
	RealFunctionR3 Fx = RealFunctionR3([X=_X](vec3 x) {
		return X(x).x;
	}, eps);
	RealFunctionR3 Fy = RealFunctionR3([X=_X](vec3 x) {
		return X(x).y;
	}, eps);
	RealFunctionR3 Fz = RealFunctionR3([X=_X](vec3 x) {
		return X(x).z;
	}, eps);
	_dX = [Fx, Fy, Fz](vec3 v) {
		return mat3(Fx.df(v), Fy.df(v), Fz.df(v));
	};
}

VectorField::VectorField(RealFunctionR3 Fx, RealFunctionR3 Fy, RealFunctionR3 Fz, float epsilon)
: eps(epsilon) {
	_X = [fx = Fx, fy = Fy, fz = Fz](vec3 v) {
		return vec3(fx(v), fy(v), fz(v));
	};
	_dX = [fx = Fx, fy = Fy, fz = Fz](vec3 v) {
		return mat3(fx.df(v), fy.df(v), fz.df(v));
	};
}


VectorField::VectorField(VectorFieldR2 f)
: VectorField([_f=f](vec3 v) {
	return vec3(_f(vec2(v.x, v.y)), 0);
}, 0.01) {}



VectorField VectorField::constant(vec3 v) {
	return VectorField([v](vec3 x) {
		return v;
	}, 0.01);
}


VectorField VectorField::operator+(const VectorField &Y) const {
	return VectorField([f=_X, g=Y._X](vec3 v) {
						   return f(v) + g(v);
					   }, [df=_dX, dg=Y._dX](vec3 v) {
						   return df(v) + dg(v);
					   }, eps);
}

VectorField VectorField::operator*(float a) const {
	return VectorField([f=_X, a](vec3 v) {
						   return f(v) * a;
					   }, [df=_dX, a](vec3 v) {
						   return df(v) * a;
					   }, eps);
}

VectorField VectorField::operator*(const RealFunctionR3 &f) const {
	return VectorField([X=_X, g=f](vec3 v) {
						   return X(v) * g(v);
					   }, [dX=_dX, g=f](vec3 v) {
						   return dX(v) * g(v);
					   }, eps);
}

VectorField VectorField::linear(mat3 A) {
	return VectorField([A](vec3 v) {
						   return A * v;
					   }, [A](vec3 v) {
						   return A;
					   }, 0.01);
}

VectorField VectorField::radial(vec3 scale) {
	return VectorField([scale](vec3 v) {
						   return vec3(v.x * scale.x, v.y * scale.y, v.z * scale.z);
					   }, [scale](vec3 v) {
						   return mat3(scale.x, 0, 0, 0, scale.y, 0, 0, 0, scale.z);
					   }, 0.01);
}


RealFunctionR3 VectorField::divergence() const {
	return RealFunctionR3([f = _X](vec3 v) {
		return glm::dot(f(v), vec3(1, 1, 1));
	});
}

VectorField VectorField::curl() const {
	RealFunctionR3 F1 = F_x();
	RealFunctionR3 F2 = F_y();
	RealFunctionR3 F3 = F_z();
	return VectorField([F1, F2, F3](vec3 v) {
		return vec3(F3.dy(v) - F2.dz(v), F1.dz(v) - F3.dx(v), F2.dx(v) - F1.dy(v));
	}, 0.01);
}



class Flow Flow::spatial_partial(int i) const {
	return Flow([F=F, e=eps](vec3 x, float t) {
		return derivativeOperator(curry(F)(x), e)(t);
	}, eps);
}

Flow::Flow(): F([](vec3 x, float t) { return vec3(0, 0, 0); }), eps(0.01) {}

Flow::Flow(ScalarField Fx, ScalarField Fy, ScalarField Fz, float epsilon): Flow([fx=Fx, fy=Fy, fz=Fz](vec3 x, float t) { return vec3(fx(x, t), fz(x, t), fz(x, t)); }, epsilon) {}

vec3 Flow::operator()(vec3 x, float t) const { return F(x, t); }

SteadyFlow Flow::operator()(float t) const { return SteadyFlow([F=F, t](vec3 x) { return F(x, t); }, eps); }

SteadyFlow Flow::fix_time(float t) const { return operator()(t); }

std::function<vec3(float, float, float, float)> Flow::separate_input_vector() const { return [f=F](float x, float y, float z, float t) { return f(vec3(x, y, z), t); }; }

Flow Flow::operator+(const Flow &Y) const { return Flow([F=F, Y=Y.F](vec3 x, float t) { return F(x, t) + Y(x, t); }, eps); }

Flow Flow::operator*(float a) const { return Flow([F=F, a=a](vec3 x, float t) { return F(x, t) * a; }, eps); }

Flow Flow::operator-() const { return *this * -1; }

Flow Flow::operator-(const Flow &Y) const { return *this + (-Y); }

Flow Flow::operator*(const SteadyScalarField &f) const { return Flow([F=F, f=f](vec3 x, float t) { return F(x, t) * f(x); }, eps); }

Flow Flow::operator/(const SteadyScalarField &f) const { return Flow([F=F, f=f](vec3 x, float t) { return F(x, t) * f(x); }, eps); }

Flow Flow::operator*(const ScalarField &f) const { return Flow([F=F, f=f](vec3 x, float t) { return F(x, t) * f(x, t); }, eps); }

Flow Flow::operator/(const ScalarField &f) const { return Flow([F=F, f=f](vec3 x, float t) { return F(x, t) * f(x, t); }, eps); }

ScalarField Flow::F_x() const { return ScalarField([F=F](vec3 x, float t) { return F(x, t).x; }, eps); }

ScalarField Flow::F_y() const { return ScalarField([F=F](vec3 x, float t) { return F(x, t).y; }, eps); }

ScalarField Flow::F_z() const { return ScalarField([F=F](vec3 x, float t) { return F(x, t).z; }, eps); }

Flow Flow::time_derivative() const {
	return Flow([F=F, e=eps](vec3 x, float t) {
		return derivativeOperator(curry(F)(x), e)(t);
	}, eps);
}



Flow::Flow(std::function<vec3(vec3, float)> X, float eps)
: F(X),
  eps(eps) {}

Flow::Flow(std::function<SteadyFlow(float)> pencil)
: F([pencil](vec3 x, float t) {
	  return pencil(t)(x);
  }),
  eps(pencil(0).getEps()) {}

Flow::Flow(SteadyFlow steady_flow)
: F([steady_flow](vec3 x, float t) {
	return steady_flow(x);
}),
  eps(steady_flow.getEps()) {}
