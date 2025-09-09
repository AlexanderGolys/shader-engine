#include "common/SDFRendering.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;



int main() {
	SDFRenderer renderer = SDFRenderer(.01f, vec4(.05, .05, 0.07, 1.0f), R"(C:\Users\PC\Desktop\ogl-master\screenshots\)", 5.0f);
	renderer.initMainWindow(UHD, "flows");

	PointLight light1 = PointLight(vec3(-6,-.5, 2), 1, .001, 0.0001, .05);

	renderer.setLights({light1});

	shared_ptr<Camera> camera = make_shared<Camera>(vec3(0, -9, 5), vec3(0, 0, 0));
	renderer.setCamera(camera);

	auto tube1 = smoothTubeSDF(.9, .98, 2.5, .1, vec3(0, 0, 0), rotationMatrix(vec3(1, 0, 0), PI/2), "sdf");
	auto tube2 = smoothTubeSDF(.9, .98, 2.5, .1, vec3(2.5, 0, 0), "sdf");

	auto waterTube1 = torusSDF(.89, 2.5, vec3(0, 0, 0), rotationMatrix(vec3(1, 0, 0), PI/2));
	auto waterTube2 = torusSDF(.89, 2.5, vec3(2.5, 0, 0));
	auto waterLevel2 = planeSDF(vec3(0, 0, -1), .1);
	auto waterLevel1 = planeSDF(vec3(0, 0, -1), -1.5);
	auto water = waterTube1.smoothSubtract(waterLevel1, .02) + waterTube2.smoothSubtract(waterLevel2, .02);


	auto b1 = planeSDF(vec3(-3, -1, 0), -2) + planeSDF(vec3(0, -.3, 1), -1) + planeSDF(vec3(-.5, -1, 0), -1.5) + ~sphereSDF(15, ORIGIN);
	auto box = b1
	- torusSDF(1.05, 2.5, vec3(0, 0, 0), rotationMatrix(vec3(1, 0, 0), PI/2))
	- torusSDF(1.05, 2.5, vec3(2.5, 0, 0));

	auto floor = box;
//	auto water = torusSmall.smoothIntersect(box, .03);
	auto glass = tube1 + tube2;

	floor.setName("sdf_floor");
	water.setName("sdf_water");
	glass.setName("sdf_glass");

	vec4 waterAmbient = vec4(24, 20, 50, 255)/555.f;
	vec4 waterDiffuse = vec4(100, 123, 221, 255)/255.f;

	vec4 floorAmbient = vec4(34, 15, 20, 255)/555.f;
	vec4 floorDiffuse = vec4(50, 35, 50, 255)/255.f;

	vec4 glassAmbient = vec4(150, 200, 250, 255)/555.f;
	vec4 glassDiffuse = vec4(200, 200, 250, 255)/255.f;

	SDFMaterial floorMat = SDFMaterial(floorAmbient, floorDiffuse, 0, 1, 1);
	SDFMaterial waterMat = SDFMaterial(waterAmbient, waterDiffuse, .2, .5, 1.33);
	SDFMaterial glassMat = SDFMaterial(glassAmbient, glassDiffuse, .7 , .12, 1.06);

	SDFScene merged = SDFScene({floor, water, glass}, {floorMat, waterMat, glassMat});


	Path templateShadersDir = Path("SDF\\shader-templates");
	Path shadersDir = Path("SDF\\shaders");
	Path generatedShadersDir = Path("SDF\\auto-generated-shaders");

	CodeFileDescriptor templateShd = CodeFileDescriptor("templateWater.frag", templateShadersDir);
	CodeFileDescriptor targetFragmentShaderFile = CodeFileDescriptor("generatedWater.frag", generatedShadersDir);
	CodeFileDescriptor vertFile = CodeFileDescriptor("basicVert.vert", shadersDir);

	TemplateCodeFile temp = vanillaSDFTemplate(templateShd, targetFragmentShaderFile);
	merged.addToTemplate(temp);

	Shader vert = Shader(vertFile);
	Shader frag = Shader(temp);
	ShaderProgram program = ShaderProgram(vert, frag);

	SDFRenderingStep sdfStep = SDFRenderingStep(make_shared<ShaderProgram>(program), merged);
	renderer.addSDFStep(make_shared<SDFRenderingStep>(sdfStep));
	return renderer.mainLoop();
}
