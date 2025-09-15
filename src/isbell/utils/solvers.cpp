#include "solvers.hpp"



vector<vector<vec3>> developSolutionsAlongCurve( BIHOM(float, vec3, vec3) &f, float t0,
													   const vector<vec3> &initials, int iters, float h) {
	vector<vector<vec3>> solutions = {};

	for (vec3 initial : initials) {
		RK4 solver(f, t0, initial, h);
		solver.solveNSteps(iters);
		solutions.push_back(solver.solution());
	}
	return solutions;
}



vector<vector<vec3>> developSolutionsAlongCurve( BIHOM(float, vec3, vec3) &f, float t0, float t1,
													   const vector<vec3> &initials, int iters) {
	vector<vector<vec3>> solutions = {};
	float h = (t1 - t0) / iters;

	for (vec3 initial : initials) {
		RK4 solver(f, t0, initial, h);
		solver.solveUpTo(t1);
		solutions.push_back(solver.solution());
	}
	return solutions;
}

SmoothParametricSurface BezierSurfaceFromSolution(const vector<vector<vec3>> &solutions, float t0, float t1) {
	return BezierSurface(solutions, 0, 1, t0, t1);
}
SmoothParametricSurface SplineSurfaceFromSolution(const vector<vector<vec3>> &solutions, vec2 spatial_range, float t0, float t1, int k, float eps) {
	return BSplineSurfaceUniform(solutions, spatial_range, vec2(t0, t1), 3, eps);
}


vector<SmoothParametricCurve> streamLinesFromSolution(const vector<vector<vec3>> &solutions, float t0, float t1, int k, float eps, int step) {
	vector<SmoothParametricCurve> curves = {};
	for (const vector<vec3>& solution : rangeStep(solutions, step))
		curves.push_back(BezierCurve(solution, t0, t1, eps));
	return curves;
}







//template<VectorSpaceConcept<float> V>
void RK4::computeStep() {
	float tn = this->times.back();
	vec3 yn = this->solutionSequence.back();
	float h  = getStep();

	auto k1 = _f(tn, yn);
	auto k2 = _f(tn + c[2]*h, yn + k1* a[2][1]*h);
	auto k3 = _f(tn + c[3]*h, yn + k1*a[3][1]*h + k2 * a[3][2]*h);
	auto k4 = _f(tn + c[4]*h, yn + k1*a[4][1]*h + k2*a[4][2]*h + k3*a[4][3]*h);

	vec3 y = yn + (k1*b[1] + k2*b[2] + k3*b[3] + k4*b[4]) * h;
	this->solutionSequence.push_back(y);
	this->times.push_back(tn + h);
}

void RK4::solveUpTo(float t1) {
	while (timeReached() < t1)
		computeStep();
}

void RK4::solveNSteps(int n) {
	for (int i = 0; i < n; i++)
		computeStep();
}

SmoothParametricCurve RK4::integralCurveBezier(float t0, float t1) {
	while (!solutionValidAtTime(t1))
		computeStep();
	vector<vec3> points = {};
	for (int i = 0; i < solutionSequence.size(); i++)
		if (times[i] >= t0 && times[i] <= t1)
			points.push_back(solutionSequence[i]);
	return BezierCurve(points, t0, t1);
};
