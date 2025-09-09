#include "../engine/specific.hpp"
#include "../engine/interface.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared, std::cout, std::endl;

RenderSettings settings = RenderSettings(
	GRAY_PALLETTE[8],	// background color
		true, 			// alpha blending
		true, 			// depth test
		true, 			// time uniform
		2.0f, 			// animation speed
		30,				// max FPS
		false,			// take screenshots
		FHD,			// resolution
		-1.0f,			// screenshot frequency
		"0/0??"		// window title
);



int main() {



	Renderer renderer = Renderer(settings);
	renderer.initMainWindow();

	vec4 blue = BLUE_PALLETTE[5];
	blue.w = .8;

	auto floormat = MaterialPhong(blue, .25, .71536, .2112348, 1.0);
	auto material_curve = MaterialPhong(REDPINK_PALLETTE[5], .15, .71536, .35112348, 15.0);

	auto hyperbola2 = DiscreteRealFunctionNonUniform(1.0f/X_R, .01, .001, vec2(.2, 8), true);
	auto hyperbola = DiscreteRealFunctionNonUniform(1.0f/X_R, .01, .001, vec2(-8, -.2), true);

	auto c1 = make_shared<PipeCurveVertexShader>(hyperbola, .03, 15);
	auto c2 = make_shared<PipeCurveVertexShader>(hyperbola2, .03, 15);




	PointLight light1 = PointLight(vec3(1,-3, 15), .003, .0005);
	auto lights = vector<Light>({light1, light1, light1});

	auto floor = make_shared<IndexedMesh>(paraleblahblapid(vec3(-8, -5, -.1), e1*16, e2*10, -e3*1));


	auto shader_curve = ShaderProgram(
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\curve.vert)",
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\basic_alpha1.frag)");


	auto shader_floor = ShaderProgram(
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\basic.vert)",
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\basic.frag)");

	shared_ptr<Camera> camera = make_shared<Camera>(
		make_shared<SmoothParametricCurve>([](float t) { return vec3(8, -5, 9); }),
		vec3(0, 0, 0),  vec3(0, 0, 1),  PI/3);

	renderer.setCamera(camera);
	renderer.setLights(lights);

	renderer.addMeshStep(shader_floor, floor, floormat);
	renderer.addMeshStep(shader_curve, c1, material_curve);
	renderer.addMeshStep(shader_curve, c2, material_curve);


	return renderer.mainLoop();
}
