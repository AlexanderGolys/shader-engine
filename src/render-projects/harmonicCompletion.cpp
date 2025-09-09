#include "../engine/specific.hpp"
#include "../geometry/pde.hpp"
#include "../engine/interface.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;




int main() {
	Renderer renderer = Renderer(.25f, vec4(.0, .0, 0.0, 1.0f));
	renderer.initMainWindow(FHD, "flows");


	vec2 dom = vec2(-5, 5);
	auto ut0 = expImpulse_k(2, .6, .2);
	auto ut1 = expImpulse_k(0, .9, 1);

	auto Fl = [](float l) {return RealFunction([l](float x) {return pow4(x)*sq(x-l)/sq(l-1);}, 1e-3);};
	auto u0 =expImpulse_k(1, 1, .5)*0/30;
	auto bdmat = MaterialPhong(GRAY_PALLETTE[2], .017, .56, .948, 15.0);
	auto bdmat2 = MaterialPhong(GRAY_PALLETTE[2], .017, .56, .948, 15.0);

	float T = 1000;
	auto s = LaplaceRectangleD0D0(ut0, ut1, 5, 2);
	auto surf = SmoothParametricSurface([f=s.solution(10)](float x, float t) {
		return vec3(x, t, f(x, t)); }, vec2(0.1, 1.9), vec2(0.1, 1.9));
	auto m = make_shared<IndexedMesh>(surf, 10, 10, randomID());
	// auto curv1 = std::make_shared<DevelopingSurface>(s.solution(10), vec2(-5, 5), 10, 0, .03, 13, 3);

	PointLight light1 = PointLight(vec3(-1,3, 3), .01, .032);
	PointLight light2 = PointLight(vec3(0, -1, 6), .09, .0132);
	PointLight light3 = PointLight(vec3(1,10, 2), .01, .02);
	auto lights = vector<Light>({light1, light2, light3});

	auto shader = ShaderProgram(
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\waves.vert)",
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\waves.frag)");



	shared_ptr<Camera> camera = make_shared<Camera>(vec3(0, -.3, 4), vec3(0, 0, 0), vec3(0, 0, 1), PI/4);
	renderer.setCamera(camera);
	renderer.setLights(lights);
	renderer.addMeshStep(shader, m, bdmat);
	// renderer.connect_system(shader, curv1, bdmat2);
	renderer.addTimeUniform();

	return renderer.mainLoop();
}
