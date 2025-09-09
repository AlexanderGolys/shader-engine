#include "common/SDFRendering.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;



int main() {
	SDFRenderer renderer = SDFRenderer(.01f, vec4(.05, .05, 0.07, 1.0f), R"(C:\Users\PC\Desktop\ogl-master\screenshots\)", 5.0f);
	renderer.initMainWindow(UHD, "flows");

	PointLight light1 = PointLight(vec3(-1,3, 3), .01, .032);


	ShaderProgram sdfProgram = ShaderProgram(
		R"(C:\Users\PC\Desktop\ogl-master\src\SDF\shaders\ww.vert)",
		R"(C:\Users\PC\Desktop\ogl-master\src\SDF\shaders\ww.frag)");

	renderer.setLights({light1});


	shared_ptr<Camera> camera = make_shared<Camera>(vec3(0, 0, 0), vec3(0, 1, 0));
	renderer.setCamera(camera);

	auto floor = planeSDF(vec3(0, 0, 1), -2);
	auto sph2 = sphereSDF(1, vec3(0, 9.5, 2));
	auto box = boxSDF(vec3(2, 1, 1), vec3(-1, 10, 0), rotationMatrix(vec3(1, 1, 1), PI/4));
	auto sphSubBox =  sph2.smoothUnion(box, .6) + floor;


	ShaderProgram sdfProgram2 = programGeneratedFromSDFObj(
			CodeFileDescriptor(R"(C:\Users\PC\Desktop\ogl-master\src\SDF\templateShaders\template1.frag)", false),
			CodeFileDescriptor(R"(C:\Users\PC\Desktop\ogl-master\src\SDF\shaders\basicVert.vert)", false),
			sphSubBox, Path(R"(C:\Users\PC\Desktop\ogl-master\src\SDF\generatedShaders\generatedShd1.frag)"), true);

	SDFRenderingStep sdfStep = SDFRenderingStep(make_shared<ShaderProgram>(sdfProgram2), sphSubBox);
	renderer.addSDFStep(make_shared<SDFRenderingStep>(sdfStep));

	renderer.addCustomAction([&sphSubBox](float t) {
		sphSubBox.updateParameter(vec3(1.1 + sin(2*TAU*t)/1.5,0, 0), 1);
		sphSubBox.updateParameter(vec3(0, 9, .8) + vec3(sin(5*TAU*t), -sin(7*TAU*t)/2, cos(5*TAU*t)*1.1), 0);

	});
	return renderer.mainLoop();
}
