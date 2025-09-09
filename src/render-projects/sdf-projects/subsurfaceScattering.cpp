#include "src/engine/sdf-rendering/SDFRendering.hpp"

using namespace glm;



int main() {
	SDFRenderer renderer = SDFRenderer(.01f, vec4(.05, .05, 0.07, 1.0f), R"(C:\Users\PC\Desktop\ogl-master\screenshots\)", 5.0f);
	renderer.initMainWindow(UHD, "flows");

	PointLight light1 = PointLight(vec3(-5,-1.5, 5), 1, .01, 0.001, .05);

	renderer.setLights({light1});

	shared_ptr<Camera> camera = make_shared<Camera>(vec3(0, -9, 5), vec3(0, 0, 0));
	renderer.setCamera(camera);



	vec4 s1Ambient = vec4(24, 30, 50, 255)/255.f;
	vec4 s1Diffuse = vec4(60, 70, 111, 255)/255.f;

	vec4 s2Ambient = vec4(41, 13, 20, 255)/255.f;
	vec4 s2Diffuse = vec4(70, 40, 50, 255)/255.f;

	vec4 floorAmbient = vec4(10, 10, 15, 255)/255.f;
	vec4 floorDiffuse = vec4(100, 100, 150, 255)/255.f;

	SDFMaterialPlus floorMat = SDFMaterialPlus(floorAmbient, floorDiffuse, 0, .5, 1, 2.5);
	SDFMaterialPlus torMat = SDFMaterialPlus(s2Diffuse, s2Diffuse, 0, .7, 1.33, 2.1);
	SDFMaterialPlus boxMat = SDFMaterialPlus(s1Diffuse, s1Diffuse, 0 , 2, 1.06, 9.1);

	auto box1 = roundBoxSDF(boxMat, vec3(.15, 0.7, 2.3), .15, vec3(-1, 0, 1), rotationMatrix(vec3(-2.3, -2.2, 0), 1.4));
	auto tor = torusSDF(torMat, 1, 1.5, vec3(2, 0, 1)).smoothUnion(sphereSDF(torMat, 1, vec3(0, -1, 1)), .2);
	auto floor = planeSDF(floorMat, vec3(0, 0, 1),1);
//		auto tor = torusSDF(torMat, .5, 1.5, vec3(1, 0, .5), rotationMatrix(vec3(1, -2, .5), -0.4));

	SDFScene scene = SDFScene({box1, tor, floor});


	Path templateShadersDir = Path("SDF\\shader-templates");
	Path shadersDir = Path("SDF\\shaders");
	Path generatedShadersDir = Path("SDF\\auto-generated-shaders");

	CodeFileDescriptor templateShd = CodeFileDescriptor("templateSS.frag", templateShadersDir);
	CodeFileDescriptor targetFragmentShaderFile = CodeFileDescriptor("generatedSS.frag", generatedShadersDir);
	CodeFileDescriptor vertFile = CodeFileDescriptor("basicVert.vert", shadersDir);


	Shader vert = Shader(vertFile);
	Shader frag = scene.fragmentShader(templateShd, targetFragmentShaderFile);
	ShaderProgram program = ShaderProgram(vert, frag);

	SDFRenderingStep sdfStep = SDFRenderingStep(make_shared<ShaderProgram>(program), scene);
	renderer.addSDFStep(make_shared<SDFRenderingStep>(sdfStep));
	return renderer.mainLoop();
}
