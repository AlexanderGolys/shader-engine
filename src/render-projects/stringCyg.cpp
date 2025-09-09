#include "../engine/specific.hpp"
#include "../geometry/pdeDiscrete.hpp"
#include "../engine/interface.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;





int main() {
	Renderer renderer = Renderer(1, vec4(.0, .0, 0.0, 1.0f));
	renderer.initMainWindow(FHD, "flows");

	vec2 dom = vec2(0, 10);

	auto u0 = expImpulse_k(3, .4, .5)*.3;

	auto mat = MaterialPhong(REDPINK_PALLETTE[1], .037, .556, 0.248, 2.0);
	auto floormat = MaterialPhong(BLUE_PALLETTE[5], .02, .536, .2348, 2.0);

	float c = .1;
	float r = .02;

	// DiscreteRealFunctionR2 snt = DiscreteRealFunctionR2(
	// 	[](float t, float u) {
	// 		return sin(t*u);
	// 	}, vec2(-2, 2), vec2(0, 5), 100, 100);

	// auto fn = DiscreteRealFunction(max(2-abs(3*X_R-3), 0), vec2(0, 9), 400);
	auto fn = DiscreteRealFunction(1-pow(1-EXP_R(-pow(X_R-2, 12)*12), 22) + max(2-abs(3*(X_R-6)), 0), vec2(0, 10), 256 );
	// auto cn = fn.convolve(kn)/100;
	float k = 0.081;

	float T_MAX = 8;
	auto heat__ = HeatRealLineHomoDiscrete(fn, k, T_MAX, 64);
	float t_step = heat__.t_step();
	auto heat = heat__.solution();


	// auto pip = make_shared<PipeCurveVertexShader>(fn, r, 21);
	auto mesh = make_shared<SurfacePlotDiscretisedMesh>(heat);

	// mesh->flipNormals();
	// mesh->recalculateNormals();
	// mesh->shift(vec3(-dom.y/2, 0, 0));



	PointLight light1 = PointLight(vec3(-1,3, 3), .03, .032);
	PointLight light2 = PointLight(vec3(1, -1, 6), .09, .0132);
	PointLight light3 = PointLight(vec3(-1,-2, -2), .01, .02);

	auto lights = vector<Light>({light1, light2, light3});

	IndexedMesh b0 = disk3d(7, vec3(0, 0, -.6), e1, e2, 7, 4, randomID());

	auto shader = ShaderProgram(
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\curve_heat.vert)",
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\heat.frag)");

	auto shader_floor = ShaderProgram(
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\guitar.vert)",
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\heat.frag)");

	shared_ptr<Camera> camera = make_shared<Camera>(
		make_shared<SmoothParametricCurve>([](float t) { return vec3(5-7*sin(t/15), -3-7*cos(t/15), 3.5); }),
		vec3(5, 0, 1),  vec3(0, 0, 1),  PI/4);
	renderer.setCamera(camera);
	renderer.setLights(lights);
	// renderer.addMeshStep(shader_floor, make_shared<IndexedMesh>(b0), floormat);
	renderer.addMeshStep(shader_floor, mesh, floormat);
	// renderer.addMeshStep(shader, pip, floormat);


	int last_waterfall_slice = 0;
	auto dheatdx = heat.dfdx();
	PolyGroupID id = PolyGroupID("69");
	bool* swiitch = new bool(false);

	// renderer.addCustomAction([&last_waterfall_slice, &heat, &dheatdx, &pip, &id, &swiitch, &T_MAX](float t, float dt) {
	// 	float s = 25;
	// 	t /= s;
	// 	// if (t >= T_MAX) return;
	// 	float freq = heat(t).L2_norm()*15;
	// 	pip->updateCurve(heat(t), dheatdx(t));
	// 	 if (floor(t*freq) > last_waterfall_slice ) {
	// 		last_waterfall_slice = floor(t*freq)+1;
	// 		id = PolyGroupID("69"+std::to_string(last_waterfall_slice));
	//
	// 		pip->duplicateCurve(id);
	// 		pip->deformPerVertex(id, [t, s, last_waterfall_slice, &swiitch](BufferedVertex &b) {
	// 			b.setColor(.01+.005*(*swiitch), 3);
	// 			*swiitch = !(*swiitch);
	// 			b.setExtra0(vec4(t*s, 1, 0, 0));
	//
	// 		});
	//
	// 	}
	// });
	//
	renderer.addTimeUniform();
	return renderer.mainLoop();
}
