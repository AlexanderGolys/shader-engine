#include "../engine/specific.hpp"
#include "../engine/interface.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared, std::cout, std::endl;





int main() {

	Renderer renderer = Renderer(1, vec4(.0, .0, 0.0, 1.0f));
	renderer.initMainWindow(FHD, "flows");

	auto u0 = expImpulse_k(3, .4, .5)*.3;

	auto mat = MaterialPhong(BLUE_PALLETTE[3], .037, .41556, 0.16248, 52.0);
	auto floormat = MaterialPhong(BLUE_PALLETTE[5], .05, .41536, .0112348, 1.0);

	auto fn = loadSequence(CodeFileDescriptor("cs.txt", "sounds", true), vec2(0, 254)).downsample_max(8);

	cout << "Size: " << fn.samples() << endl;
	cout << "Sampling : " << fn.samples()/512.f << "Hz" << endl;

	int window = 512;
	int step = 1024;

	cout << "Window: " << 512.f/fn.samples()*window << "s" << endl;
	cout << "Step: " << 512.f/fn.samples()*step << "s" << endl;


	auto heat_ = DiscreteGaborTransform(window, step).transform_print(fn);

	// auto heat = heat_.abs().downsample_x(4, 0)/32 + heat_.abs().downsample_x(4, 1)/8 + heat_.abs().downsample_x(4,2)/8 + heat_.abs().downsample_x(4, 3)/32;
	auto heat = heat_.abs()/10;
	// auto heat = heat_.abs().smoothen_x(.04)/400;

	cout << "Size: (" << heat.samples().x << ", " << heat.samples().y << ")" << endl;



	auto mesh = make_shared<SurfacePolarPlotDiscretisedMesh>(heat, 2.0, .35);
	// auto m = IndexedMesh(torusImplicit())

	renderer.resetTimer();

	PointLight light1 = PointLight(vec3(-1,3, 3), .03, .032);
	PointLight light2 = PointLight(vec3(1, 2, 6), .09, .0132);
	PointLight light3 = PointLight(vec3(0,3, 3), .01, .02);

	auto lights = vector<Light>({light1, light2, light3});



	auto shader = ShaderProgram(
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\music.vert)",
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\music.frag)");


	shared_ptr<Camera> camera = make_shared<Camera>(
		make_shared<SmoothParametricCurve>([](float t) { return vec3(0, 4, 0); }),
		vec3(0, 0, 0),  vec3(0, 0, 1),  PI/3);

	renderer.setCamera(camera);
	renderer.setLights(lights);
	renderer.addMeshStep(shader, mesh, floormat);
	renderer.addTimeUniform();

	return renderer.mainLoop();
}
