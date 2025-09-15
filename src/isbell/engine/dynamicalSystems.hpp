#pragma once
#include "indexedRendering.hpp"
#include "../geometry/pde.hpp"

class DynamicalInterface : public IndexedMesh {
public:
	RealFunction time_reparam = X_R;
	float t=0;
	float delayed = 0;
	virtual void update(float t) {
		this->t = time_reparam(t);
	}
	virtual void timeReparametrize(const RealFunction &f) {
		time_reparam = f;
	}
	virtual void delay(float dt) {
		time_reparam = RELU(X_R - dt);
		delayed = dt;
	}
};


class AreaUnderWave : public DynamicalInterface {
	RealFunctionR2 f;
	PolyGroupID id;
public:
	AreaUnderWave(RealFunctionR2 f, vec2 dom, int sampling_res, float h);
	void update(float t) override;
};


class MovingCurve : public DynamicalInterface {
protected:
	RealFunctionR2 f;
	PolyGroupID id;
	float r, h;
	int radial_res;
	int sampling_res;
public:
	MovingCurve(RealFunctionR2 f, vec2 dom, int sampling_res, float h, float r, int radial_res);
	void update(float t) override;
	RealFunctionR2 getSolution() const {return f;}
	float operator()(float x, float t) const {return f(x, t);}
};


class DevelopingSurface : public MovingCurve {
	SmoothParametricSurface s;
	PolyGroupID surfaceID = randomID();
	float T_max;
public:
	DevelopingSurface(RealFunctionR2 F, vec2 dom, int sampling_res, float h, float r, int radial_res, float T_max);
};


class Waterfall : public MovingCurve {
	float period;
	float last_plot_t = 0;
	float last_t;
public:
	Waterfall(const RealFunctionR2 &f, vec2 dom, int sampling_res, float h, float r, int radial_res, float period) :
		MovingCurve(f, dom, sampling_res, h, r, radial_res), period(period) { last_t = 0; }

	void update(float t) override {
		MovingCurve::update(t);
		t = time_reparam(t);
		float dt = t - last_t;
		last_t = t;
		last_plot_t += dt;
		if (last_plot_t > period) {
			last_plot_t -= period;
			PolyGroupID newid = randomID();
			copyPolygroup(id, newid);
			deformPerVertex(newid, [this, t](BufferedVertex &v) {
				auto col = v.getColor();
				v.setColor(vec4(col.x, col.y, col.z, t));
			});

		}
	}

	void timeReparametrize(const RealFunction &f) override {
		MovingCurve::timeReparametrize(f);
		last_t = time_reparam(last_t);
	}
};



class String : public MovingCurve {
	float density, tension, l;
	vec2 shift;
	RealFunction f_x0, ft_x0;
	StringEquation eq;
	int prec;

	float linear_density() const {return density;}
	float c() const {return std::sqrt(l*tension/linear_density());}
public:
	String(float density, float tension, float r, float l,
		   vec2 shift, const RealFunction& f_x0, const RealFunction &ft_x0,
		   int n_max, int prec, int radial_res, int sampling_res);

	void set_l(float l1);
	void set_tension(float t);
	vec4 get_params() const {return vec4(r, h, tension, delayed);}
	void update(float t) override;

};
