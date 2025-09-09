#include "../engine/specific.hpp"
#include "../geometry/pde.hpp"
#include "../engine/interface.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;




int main() {
	BackStageInterface renderer = BackStageInterface(.25f, vec4(.0, .0, 0.0, 1.0f));
	renderer.initMainWindow(FHD, "flows");

	vec2 dom = vec2(0, 5);

	auto ut0 = expImpulse_k(3, .9, .3)*0 + expImpulse_k(7, .9, .3)*0;
	auto ut1 = RealFunction([](float x) {return sin(PI*(x-2))/(x-2+1e-6);}, 1e-3)*0/4;
	auto u0 =expImpulse_k(5, 1, .5)*13/30;
	auto mat = MaterialPhong(GRAY_PALLETTE[7], .017, .56, 1.948, 55.0);
	auto floormat = MaterialPhong(BLUE_PALLETTE[7], .02, .36, .348, 15.0);

	float T = 4;
	String s1 = String(1, T, .03, 10, vec2(-5, 0), u0, ut1, 21, 103, 10, 100);
	s1.delay(.5);
	auto curv1 = std::make_shared<String>(s1);

	String s2 = String(1, T, .02, 9, vec2(-5, .2), u0, ut1, 15, 103, 10, 100);
	s2.delay(.7);
	auto curv2 = std::make_shared<String>(s2);

	String s3 = String(1, T, .02, 9.5, vec2(-5, .4), u0, ut1, 15, 103, 10, 100);
	s3.delay(.9);
	auto curv3 = std::make_shared<String>(s3);

	String s4 = String(1, T, .02, 10, vec2(-5, .6), u0, ut1, 15, 103, 10, 100);
	s4.delay(1.1);
	auto curv4 = std::make_shared<String>(s4);

	PointLight light1 = PointLight(vec3(-1,3, 3), .03, .032);
	PointLight light2 = PointLight(vec3(0, -1, 6), .09, .0132);
	PointLight light3 = PointLight(vec3(1,10, 2), .01, .02);
	auto lights = vector<Light>({light1, light2, light3});

	IndexedMesh b0 = disk3d(7, vec3(0, 0, -1), e1, e2, 7, 4, randomID());
	// IndexedMesh b1 = disk3d(.1, vec3(4, 0, 0), e2, e3, 4, 4, randomID());

	auto shader = ShaderProgram(
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\waves.vert)",
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\guitar.frag)");
	auto shader_floor = ShaderProgram(
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\waves.vert)",
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\waves.frag)");

	// SmoothParametricSurface solution = coolLawson();
	shared_ptr<Camera> camera = make_shared<Camera>(vec3(0, -2, 9), vec3(0, 0, 0), vec3(0, 0, 1), PI/4);
	renderer.setCamera(camera);
	renderer.setLights(lights);
	renderer.addMeshStep(shader_floor, make_shared<IndexedMesh>(b0), floormat);
	renderer.connect_system(shader, curv1, mat);
	// renderer.connect_system(shader, curv2, bdmat2);
	// renderer.connect_system(shader, curv3, bdmat2);
	// renderer.connect_system(shader, curv4, bdmat2);

	renderer.addTimeUniform();
	return renderer.mainLoop();
}
