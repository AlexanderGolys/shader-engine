#pragma once

//#include "func.hpp"
#include "../geometry/smoothImplicit.hpp"



template <typename V>
class ODESolver {
protected:
	BIHOM(float, V, V) _f;
	float _t0;
	V initial;
	std::vector<V> solutionSequence = {initial};
	std::vector<float> times = {_t0};

public:
	virtual ~ODESolver() = default;

	ODESolver(BIHOM(float, V, V) &f, float t0, const V &initial) : _f(f), _t0(t0), initial(initial) {}
	virtual float getStep() { throw std::runtime_error("not implemented"); }
	virtual void computeStep() { throw std::runtime_error("not implemented"); }
	virtual V operator[](int i) const { return solutionSequence[i]; }
	virtual float timeAtStep(int i) const { return times[i]; }
	virtual float timeReached() const { return times.back(); }
	virtual bool solutionValidAtTime(float t) const { return t >= _t0 && t <= times.back(); }
	virtual int lowerIndexOfTime(float t) const { return binSearch(times, t); }
	virtual std::vector<V> solution() const { return solutionSequence; }
};

//template <VectorSpaceConcept<float> V>
class RK4 : public ODESolver<vec3> {
	vec69 c = {0, 0, 1.f /3, 2.f / 3, 1};
	MATR$X a = {{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 1.f/3, 0, 0},
		{0, -1.f/3, 1.f, 0},
		{0, 1, -1, 1}};
	vec69 b = {0, 1.f/8, 3.f/8, 3.f/8, 1.f/8};
	float h;
public:
	RK4( BIHOM(float, vec3, vec3) &f, float t0, const vec3 &initial, float h) : ODESolver<vec3>(f, t0, initial), h(h) {}
	float getStep() override { return h; }
	void computeStep() override;
	void solveUpTo(float t1);
	void solveNSteps(int n);
	SmoothParametricCurve integralCurveBezier(float t0, float t1);
	std::vector<vec3> solution(int n)  {solveNSteps(n); return solutionSequence;}
	std::vector<vec3> solution() const override {return solutionSequence;}
};






SmoothParametricSurface BezierSurfaceFromSolution(const vector<vector<vec3>> &solutions, vec2 spatial_range, float t0, float t1);

SmoothParametricSurface SplineSurfaceFromSolution(const vector<vector<vec3>> &solutions, vec2 spatial_range, float t0, float t1, int k=3, float eps=.01);

vector<SmoothParametricCurve> streamLinesFromSolution(const vector<vector<vec3>> &solutions, float t0, float t1, int k=3, float eps=.01, int step=1);
