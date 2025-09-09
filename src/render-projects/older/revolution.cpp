#include "common/glsl_utils.hpp"
#include "common/specific.hpp"
#include <memory>
#include <cmath>
#include <stdlib.h>

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;

float width2(float t)
{
    return .06f + .03f*sin(12*t);
}

int main(void) {
	Renderer renderer = Renderer(.05f, vec4(.14f, .14f, 0.14f, 1.0f));
	renderer.initMainWindow(FHD, "flows");
	float camSpeed = 6.5f;

	PolyGroupID curveID = PolyGroupID(420);
	PolyGroupID axisId = PolyGroupID(69);


	SmoothParametricSurface surface = DupinCyclide(1, .99, .26, .01);
	SmoothParametricSurface surface2 = torusKnot23(1.299, 2, .01).canal(width2)
		.postcompose(SpaceAutomorphism::rotation(vec3(1, 0, 0), PI), "a")
		.postcompose(SpaceAutomorphism::rotation(vec3(0, 0, 1), PI*1.4), "a")
		.postcompose(SpaceAutomorphism::translation(vec3(-1, 0, 0)), "a");

	WeakSuperMesh icoBig = icosphere(8, 6, ORIGIN, 1111, vec4(0, 0, 0, 0));
	icoBig.flipNormals();



	auto camCurve = make_shared<SmoothParametricCurve>([camSpeed](float t) {
		return vec3(-3*cos(t*camSpeed), 3*sin(t*camSpeed),  -1.3+.05*cos(t*camSpeed/2)); }, curveID, 0, TAU, true, .01);
	auto camCurve2 = make_shared<SmoothParametricCurve>([](float t) { return vec3(0); }, curveID, 0, TAU, true, .01);

	shared_ptr<Camera> camera = make_shared<Camera>(camCurve, camCurve2, vec3(0, 0, -1));
	auto lights = vector({std::make_shared<PointLightQuadric>(vec3(1, 4, 0), vec4(.9, .99, .99, 1), 9.0f),
						  std::make_shared<PointLightQuadric>(vec3(1, 0, -3.1), vec4(.99, .99, .903, 1), 9.0f),
						  std::make_shared<PointLightQuadric>(vec3(-2, -.1, -4), vec4(.99, .99, .99, 1), 5.0f)});

	auto bgmatconst =  MaterialPhong(vec4(.13, .13, .13, 1), .3, .50, .81, 7.0, "");

	auto surfmat =  MaterialPhong(R"(C:\Users\PC\Desktop\ogl-master\src\textures\plastic1.bmp)", .07, .32 , .0953, 50.0);
	auto bgmat =  MaterialPhong(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture7.bmp)", .012, .5 , .53, 9.0);
	auto knotmat =  MaterialPhong(R"(C:\Users\PC\Desktop\ogl-master\src\textures\metal4.bmp)", .07, .32 , .0953, 50.0);


	auto shader = make_shared<ShaderProgram>(R"(C:\Users\PC\Desktop\ogl-master\src\shaders\rev.vert)",
									R"(C:\Users\PC\Desktop\ogl-master\src\shaders\rev.frag)");
	auto shader2 = make_shared<ShaderProgram>(   R"(C:\Users\PC\Desktop\ogl-master\src\shaders\rev.vert)",
										R"(C:\Users\PC\Desktop\ogl-master\src\shaders\rev2.frag)");

	auto step = make_shared<RenderingStep>(shader);
	auto step2 = make_shared<RenderingStep>(shader);
	auto step3 = make_shared<RenderingStep>(shader2);

	PolyGroupID sphereID = PolyGroupID(421);


	auto s = make_shared<WeakSuperMesh>(surface, 100, 100, curveID);
	auto knot = make_shared<WeakSuperMesh>(surface2, 500, 60, "a");


    s->addGlobalMaterial(surfmat);
	icoBig.addGlobalMaterial(bgmatconst);
    knot->addGlobalMaterial(knotmat);

    step->setWeakSuperMesh(s);
	step->addConstVec4("bdColor", vec4(85.f/255, 193.f/255, 255.f/255, 1.0f));

	step2->setWeakSuperMesh(make_shared<WeakSuperMesh>(icoBig));
	step2->addConstVec4("bdColor", vec4(85.f/255, 193.f/255, 255.f/255, 1.0f));

	step3->setWeakSuperMesh(knot);
	step3->addConstVec4("bdColor", vec4(85.f/255, 193.f/255, 255.f/255, 1.0f));

    renderer.addRenderingStep(step);
//	renderer.addRenderingStep(step2);
	renderer.addRenderingStep(step3);

	renderer.addTimeUniform();
	renderer.setCamera(camera);
	renderer.setLights(lights);

	vector deformations = {surface};
	float dt = .1;
	for (int i = 0; i < 4; i++)
		deformations.push_back(deformations[deformations.size()-1].meanCurvatureFlow(dt));


	s->deformPerVertex(curveID, [&surface](BufferedVertex &v) {
		float t = v.getColor().x;
		float u = v.getColor().y;
		v.setColor(vec4(t, u, surface.gaussianCurvature(t, u), surface.meanCurvature(t, u)));
	});
	knot->deformPerVertex("a", [&surface](BufferedVertex &v) {
			float t = v.getColor().x;
			float u = v.getColor().y;
			v.setColor(vec4(t, u, surface.gaussianCurvature(t, u), surface.meanCurvature(t, u)));
		});


	return renderer.mainLoop();

}
