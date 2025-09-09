#include "common/specific.hpp"
#include "fundamentals/macros.hpp"
#include "fundamentals/prob.hpp"
#include "fundamentals/flows.hpp"


using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;


vec4 color(vec2 t) {
	float a = (1.0f + t.x)/4.5f;
	float b = (1.0f + t.y)/4.5f;
	return vec4(.7, .3+b, .2+a, 1.f);
}

float time_dump(float t) {
	return min(t, 1.f)/(1.01+4.f*t*t);
}

float width_ratio_dump(float t) {
	return sqrt(t);
}


int main() {
	Renderer renderer = Renderer(.1f, vec4(.0, .0, 0.0, 1.0f));
	renderer.initMainWindow(FHD, "flows");


	float camSpeed = .00;


	auto bdmat = MaterialPhong(BLUE_PALLETTE[5], .07, .6, .48, 15.0);

	auto X = VectorFieldR2::linearFlow(vec2(0.3, .6)) + VectorFieldR2::flowElement(vec2(-.3, -.2), .2, 3) + VectorFieldR2::flowElement(vec2(.7, .3), .5, -2);
	float r = .05;
	auto circle = SmoothParametricPlaneCurve([r](float t) { return vec2(r*cos(t), r*sin(t)); }, 0, TAU, true, 0.01);

	int N = 15;
	vector<vec2> random_grid = {};
	vector<vec4> colors = {};
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
		{
		random_grid.emplace_back(-2 + 4.f*i/(N-1), -1 + 2.f*j/(N-1));
		colors.push_back(color(random_grid[i]));
	}

	// auto lines = PlanarDiffusedInterval(X, .005, 700, color, vec2(.1, .1), vec2(.2, .4), 0);
	// auto c0 = PlanarDiffusedCurve(X, .005, 700, vec4(.7, 3, .2, 1.f), time_dump, width_ratio_dump, circle, vec2(0, TAU), 5, 0);
	auto c1 = PlanarDiffusedPatterns(X, .005, 700, colors, random_grid, time_dump, width_ratio_dump, circle, vec2(0, TAU), 9, 1.5);

	// c0.merge(c1);

	auto shader = ShaderProgram( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\flowart2.vert)",
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\flowart2.frag)");



	renderer.addMeshStep(shader, make_shared<WeakSuperMesh>(c1), bdmat);

	shared_ptr<Camera> camera = make_shared<Camera>(vec3(0, 0.2, 3), vec3(0), vec3(0, 0, 1), PI/4);
	renderer.setCamera(camera);
	renderer.addTimeUniform();


	return renderer.mainLoop();

}
