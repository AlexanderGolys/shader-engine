#include "src/physics/fluidsSpecific.hpp"


using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;



int main() {
	Renderer renderer = Renderer(1.0f, vec4(.0555f, .05504, 0.065, 1.0f));
	renderer.initMainWindow(UHD, "flows");
	float camSpeed = .09f;

	auto camCurve = make_shared<SmoothParametricCurve>([camSpeed](float t) { return vec3(-3.f*cos(t*camSpeed)+0, 3.f*sin(t*camSpeed)+0, 4+sin(t*camSpeed) ); }, 0, 0, TAU, true, .01);
	auto camCurve2 = make_shared<SmoothParametricCurve>([](float t) { return vec3(0.0, 0, 1); }, 0, 0, TAU, true, .01);

	shared_ptr<Camera> camera = make_shared<Camera>(camCurve, camCurve2, vec3(0, 0, 1));
	auto lights = vector({std::make_shared<PointLightQuadric>(vec3(-.3, -2.8, 1.5), vec4(.90952795, .9, .94197285861, 1), 9.0f),
						    std::make_shared<PointLightQuadric>(vec3(2.9, .1, 2.5), vec4(.9, .9, .903, 1), 9.0f),
						    std::make_shared<PointLightQuadric>(vec3(0,.0, -1.7), vec4(.9698, .9292598, .9399785938, 1), 9.0f)});

	auto midmat =  MaterialPhong(vec4(.25, .35, .45, 1), 0.041031423, .4962453956641656 , 0.021931160145739731, 50.0, "");
	auto redmat =  MaterialPhong(REDPINK_PALLETTE[5], 0.031031423, .5962453956641656 , 0.21931160145739731, 50.0, "");

	int r = 390/4;
	int R = 600/4;
	int r2 = 300/4;
	int res = 36;

	std::map<std::string, HOM(glm::ivec2, float)> attr = {{"T",
		[res, r2](glm::ivec2 i) {
			return i.x + i.y < res || i.x + i.y > 1.5*res ? 0.f : 1.f;
	}}};
//
//	std::map<std::string, HOM(glm::ivec2, float)> attr = {{"T",
//		[](glm::ivec2 i) {
//			return randomFloat();
//	}}};
//	auto h = [](ivec2 i) { return .1f +  .0000f*abs(10.f-i.x*(15-i.x)*i.y*(4-i.y)); };
	vector<ivec2> removed = {};
	for (int i = 0; i < res; i++)
		for (int j = 0; j < res; j++)
			if (pow(i-res/2, 2) + pow(j-res/2, 2) > r && pow(i-res/2, 2) + pow(j-res/2, 2) < R
				&& abs(i-res/2) > 2 )
				removed.emplace_back(i, j);

	auto grid = flatCylinder(ivec2(res, res), .25, .5, 2, attr);

	auto sim = HeatFlow(grid, 5.8, 1);

	auto mesh = make_shared<WeakSuperMesh>(sim.bdMesh({"T"}));
	mesh->addGlobalMaterial(midmat);

	auto shader = make_shared<ShaderProgram>(
			R"(C:\Users\PC\Desktop\ogl-master\src\shaders\heat.vert)",
R"(C:\Users\PC\Desktop\ogl-master\src\shaders\heat.frag)");

	auto step = make_shared<RenderingStep>(shader);
	step->setWeakSuperMesh(mesh);

	renderer.addRenderingStep(step);
	renderer.addTimeUniform();

	renderer.setCamera(camera);
	renderer.setLights(lights);

	renderer.addCustomAction([&sim, &mesh](float t, float dt) {
		sim.update(dt);
		sim.updateBdMesh({"T"}, *mesh);

	});
	return renderer.mainLoop();

}
