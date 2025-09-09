#include "common/SDFRendering.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;



int main() {
	SDFRenderer renderer = SDFRenderer(.01f, vec4(.05, .05, 0.07, 1.0f), R"(C:\Users\PC\Desktop\ogl-master\screenshots\)", 5.0f);
	renderer.initMainWindow(UHD, "flows");

	PointLight light1 = PointLight(vec3(-10,3, 7), 1, .004, 0.002, .1);

	renderer.setLights({light1});

	shared_ptr<Camera> camera = make_shared<Camera>(vec3(0, -9, 4), vec3(0, 0, 0));
	renderer.setCamera(camera);

	auto torusBig = torusSDF(1.1, 2.5, ORIGIN);
	auto torusSmall = torusSDF(.9, 2.5, ORIGIN);
	auto torusInner = torusSDF(.8, 2.5, ORIGIN);
	auto plane = planeSDF(vec3(0, 0, 1), 0);
	auto plane2 = planeSDF(vec3(-.8, -1, 0), 0);

	auto floor = plane.smoothSubtract(torusBig, .1);
	auto water = torusSmall.smoothIntersect(plane, .03);
	auto glass = (torusSmall - torusInner).smoothIntersect(plane2, .03);

	floor.setKey("__FLOOR__");
	floor.setName("sdf_floor");

	water.setKey("__WATER__");
	water.setName("sdf_water");

	glass.setKey("__GLASS__");
	glass.setName("sdf_glass");

	auto gw = glass+water;
	auto gwf = gw+floor;

	Path templateShadersDir = Path("SDF\\shader-templates");
	Path shadersDir = Path("SDF\\shaders");
	Path generatedShadersDir = Path("SDF\\auto-generated-shaders");

	CodeFileDescriptor templateShd = CodeFileDescriptor("templateSofts.frag", templateShadersDir);
	CodeFileDescriptor targetFragmentShaderFile = CodeFileDescriptor("generatedSofts.frag", generatedShadersDir);
	CodeFileDescriptor vertFile = CodeFileDescriptor("basicVert.vert", shadersDir);

	TemplateCodeFile temp = vanillaSDFTemplate(templateShd, targetFragmentShaderFile);
	floor.addToTemplate(temp);
	water.addToTemplate(temp);
	glass.addToTemplate(temp);
	gwf.addToTemplate(temp);

	Shader vert = Shader(vertFile);
	Shader frag = Shader(temp);
	ShaderProgram program = ShaderProgram(vert, frag);

	SDFRenderingStep sdfStep = SDFRenderingStep(make_shared<ShaderProgram>(program), gwf);
	renderer.addSDFStep(make_shared<SDFRenderingStep>(sdfStep));
	return renderer.mainLoop();
}
