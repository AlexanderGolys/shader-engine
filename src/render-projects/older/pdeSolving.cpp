#include "src/physics/fluidsSpecific.hpp"
#include "src/fundamentals/solvers.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;



int main() {
	Renderer renderer = Renderer(1.0f, vec4(.0555f, .05504, 0.065, 1.0f));
	renderer.initMainWindow(FHD, "flows");
	float camSpeed = .00f;

	auto camCurve = make_shared<SmoothParametricCurve>([camSpeed](float t) { return vec3(2.f*cos(t*camSpeed)+0, 2.f*sin(t*camSpeed)+0, 1+sin(t*camSpeed) ); }, 0, 0, TAU, true, .01);
	auto camCurve2 = make_shared<SmoothParametricCurve>([](float t) { return vec3(0.0, 0, 1); }, 0, 0, TAU, true, .01);

	shared_ptr<Camera> camera = make_shared<Camera>(camCurve, camCurve2, vec3(0, 0, 1));
	auto lights = vector<shared_ptr<Light>>({std::make_shared<PointLightQuadric>(vec3(-.3, -2.8, 1.5), vec4(.90952795, .9, .94197285861, 1), 9.0f),
						    std::make_shared<PointLight>(vec3(2.9, .1, 2.5), vec4(.9, .9, .903, 1), 1, .35, .44),
						    std::make_shared<PointLight>(vec3(0,.0, -10.7), vec4(.9698, .9292598, .9399785938, 1), 10.f)});

	auto midmat =  MaterialPhong(vec4(.25, .35, .45, 1), 0.041031423, .4962453956641656 , 0.021931160145739731, 50.0, "");
	auto redmat =  MaterialPhong(REDPINK_PALLETTE[5], 0.031031423, .5962453956641656 , 0.21931160145739731, 50.0, "");



	auto f = [](float t, vec3 x) { return vec3(2*x.y, -2*x.x, sin(3*t)/2); };
	auto curveBd = [](float t) { return vec3(t/2, t/3 , sin(6*t)/5); };

	int n = 10;
	int m = 10;
	vector<vec3> initCurve = mapLinspace<float, vec3>(0, 1, n, curveBd);
//	RK4 solver = RK4(, 0, vec3(.4, .2, 0), .1);
	auto solutions = developSolutionsAlongCurve(f, 0, 3, initCurve, m);
	auto streamlines =   streamLinesFromSolution(solutions, 0, 3, 3, .01, 2);
//	auto surf = SplineSurfaceFromSolution(solutions, vec2(0, 1) , 0, 2, 3, .01);

//	auto mesh = make_shared<WeakSuperMesh>(surf, 15, 10, "v");
	auto meshstream = make_shared<WeakSuperMesh>(streamlines[0].pipe(.005), 20, 10, "yyy");

	for (auto &curve: rangeFrom(streamlines, 1))
		meshstream->addUniformSurface(curve.pipe(.005), 20, 10, randomID());






//	mesh->addGlobalMaterial(midmat);
	meshstream->addGlobalMaterial(redmat);

	auto shader = make_shared<ShaderProgram>(
			R"(C:\Users\PC\Desktop\ogl-master\src\shaders\pde.vert)",
R"(C:\Users\PC\Desktop\ogl-master\src\shaders\pde.frag)");

//	auto step = make_shared<RenderingStep>(shader);
//	step->setWeakSuperMesh(mesh);
	auto step2 = make_shared<RenderingStep>(shader);
	step2->setWeakSuperMesh(meshstream);

//	renderer.addRenderingStep(step);
	renderer.addRenderingStep(step2);
	renderer.addTimeUniform();

	renderer.setCamera(camera);
	renderer.setLights(lights);


	return renderer.mainLoop();

}
