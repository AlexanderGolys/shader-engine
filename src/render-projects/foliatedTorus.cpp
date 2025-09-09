#include "../engine/specific.hpp"
#include "../utils/randomUtils.hpp"
#include "../engine/interface.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared, std::cout, std::endl;

RenderSettings settings = RenderSettings(
	GRAY_PALLETTE[3],	// background color
		true, 			// alpha blending
		true, 			// depth test
		true, 			// time uniform
		0.1f, 			// animation speed
		30,				// max FPS
		false,			// take screenshots
		HD2K,			// resolution
		-1.0f,			// screenshot frequency
		"0/0??"		// window title
);


float r = 1.5;
float R = 4;
float k = 0;


// auto dupin = DupinCyclide(4, 4, 2, 1);
auto dupin = sudaneseMobius();

SmoothParametricCurve leavesDupin(float a) {
	return SmoothParametricCurve(
	[dup=dupin, a](float u) {
		return dup(u, a);
	}, randomID(), 0, TAU, true, 0.01f);
}

int main() {
	Renderer renderer = Renderer(settings);
	renderer.initMainWindow();

	ParametricSurfaceFoliation foliation = ParametricSurfaceFoliation(leavesDupin, vec2(-PI, PI), true);

	auto foliation_mesh = FoliatedParametricSurfaceMesh(
		foliation, 0, 150, 8, 400,
		[](float t) {
			return sin(t*4) >= 0 ? BLUE_PALLETTE[5] : BLUE_PALLETTE[2];
		}, {},
		[](float t) {
			return sin(t*4) >= 0 ? .01 : .002;
		}, {},
		[](float t) {
			return vec4(t, 0, 0, 1);
		}, {});

	auto mesh = make_shared<FoliatedParametricSurfaceMesh>(foliation_mesh);



	vec4 blue = BLUE_PALLETTE[5];
	vec4 white = WHITE;
	vec4 blueball = BLUE_PALLETTE[8];
	// blue.w = .1;

	auto ballmat = MaterialPhong(blue, .25, .151536, 0.42112348, 70.0);
	auto bdmat = MaterialPhong(white, .1, .3151536, 1.59112348, 250.0);

	PointLight light1 = PointLight(vec3(5,-1, 0), .003, .0005);
	PointLight light2 = PointLight(vec3(-4,-10, -2), .003, .05, BLUE_PALLETTE[0]);
	PointLight light3 = PointLight(vec3(2,6, 5), .03, .005, REDPINK_PALLETTE[0]);

	auto lights = vector<Light>({light1, light2, light3});

	auto floor = make_shared<IndexedMesh>(paraleblahblapid(vec3(-5, -5, -2), e1*10, e2*10, e3*.1));


	auto shader_curve = ShaderProgram(
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\curve.vert)",
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\foliatedTorus.frag)");


	auto shader_floor = ShaderProgram(
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\basic.vert)",
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\basic.frag)");

	shared_ptr<Camera> camera = make_shared<Camera>(
		make_shared<SmoothParametricCurve>([](float t){
			return vec3(sin(-2*t)*10, cos(-2*t)*10, 3);
		}),
		vec3(0, 0, 0),  vec3(0, 0, 1),  PI/3);

	renderer.setCamera(camera);
	renderer.setLights(lights);

	// renderer.addMeshStep(shader_floor, floor, bdmat);
	renderer.addMeshStep(shader_curve, mesh, ballmat);


	return renderer.mainLoop();
}
