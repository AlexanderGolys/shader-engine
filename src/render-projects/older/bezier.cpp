#include "common/glsl_utils.hpp"
#include "common/specific.hpp"
#include <memory>
#include <cmath>

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;

float debugSin(float t)
{
	return -cos(t)/2 + 0.5;
}

float width1(float t)
{
    return .03f + .00f*sin(4*t)*sin(4*t);
}

float width2(float t)
{
    return .03f + .00f*sin(4*t)*sin(4*t);
}

int main(void) {
	Renderer renderer = Renderer(.15f, vec4(.016f, .01509f, 0.05585f, 1.0f));
	renderer.initMainWindow(UHD, "flows");
	float camSpeed = 1.f;

	PolyGroupID curveID = PolyGroupID(420);
	PolyGroupID axisId = PolyGroupID(69);

	vector<vec3> verts1 = {vec3(3, 3, -1), vec3(4, -1, 0), vec3(2, -2, 1), vec3(-1,-1, 0)};
	vector<vec3> verts2 = {vec3(3, 3, -1), vec3(4, -1, 0), vec3(2, -2, 1), vec3(-1,-1, 0), vec3(0,-3, -2)};
	vector<vec3> verts3 = {vec3(3, 3, -1), vec3(4, -1, 0), vec3(2, -2, 1), vec3(-1,-1, 0), vec3(0,-3, -2), vec3(3, -4, 0)};
	vector<vec3> verts4 = {vec3(3, 3, -1), vec3(4, -1, 0), vec3(2, -2, 1), vec3(-1,-1, 0), vec3(0,-3, -2), vec3(3, -4, 0), vec3(2, -2, 2)};

	SmoothParametricCurve curve1 = BezierCurve(verts1, 0, 1, .01);
	SmoothParametricCurve curve2 = BezierCurve(verts2, 1, 2, .01);
	SmoothParametricCurve curve3 = BezierCurve(verts3, 2, 3, .01);
	SmoothParametricCurve curve4 = BezierCurve(verts4, 3, 4, .01);


	auto s1 = make_shared<WeakSuperMesh>(icosphere(.05, 3, verts3[0], axisId, vec4(0, 0, 0, 1)));

	for (int i = 1; i < verts1.size(); i++)
	{
		s1->addUniformSurface(segment(verts3[i-1], verts3[i], 0, 1).pipe(.02, false), 2, 15, PolyGroupID(i+999));
		s1->merge(icosphere(.05, 3, verts3[i], PolyGroupID(i+666)));
	}
	s1->addUniformSurface(segment(verts3[3], verts3[4], 1, 2).pipe(.02, false), 2, 15, PolyGroupID(4+999));
	s1->merge(icosphere(.05, 3, verts3[4], PolyGroupID(4+666), vec4(1, 1, 0, 1)));
	s1->addUniformSurface(segment(verts3[4], verts3[5], 2, 3).pipe(.02, false), 2, 15, PolyGroupID(5+999));
	s1->merge(icosphere(.05, 3, verts3[5], PolyGroupID(5+666), vec4(2, 2, 0, 1)));
	s1->addUniformSurface(segment(verts4[5], verts4[6], 3, 4).pipe(.02, false), 2, 15, PolyGroupID(7+999));
	s1->merge(icosphere(.05, 3, verts4[6], PolyGroupID(7+666), vec4(3, 3, 0, 1)));

	auto s = make_shared<WeakSuperMesh>(curve1.pipe(.03), 100, 15, "v");
	s->addUniformSurface(curve2.pipe(.03), 100, 15, "a");
	s->addUniformSurface(curve3.pipe(.03), 100, 15, "b");
	s->addUniformSurface(curve4.pipe(.03), 100, 15, "aaa");
//	s->addUniformSurface(segments.pipe(.01), 15, 100, "PL");

	auto camCurve = make_shared<SmoothParametricCurve>([camSpeed](float t) { return vec3(10*cos(t*camSpeed), 10*sin(t*camSpeed), 10 ); }, curveID, 0, TAU, true, .01);
	auto camCurve2 = make_shared<SmoothParametricCurve>([](float t) { return vec3(0); }, curveID, 0, TAU, true, .01);

	shared_ptr<Camera> camera = make_shared<Camera>(camCurve, camCurve2, vec3(0, 0, 1));
	auto lights = vector({std::make_shared<PointLightQuadric>(vec3(1, 4, 0), vec4(.90952795, .785579, .94197285861, 1), 9.0f),
						  std::make_shared<PointLightQuadric>(vec3(1, 0, -3.1), vec4(.898769, .75369864, .903, 1), 9.0f),
						  std::make_shared<PointLightQuadric>(vec3(-2, -.1, -4), vec4(.9698, .9292598, .9399785938, 1), 5.0f)});

	auto midmat =  MaterialPhong(BLUE_PALLETTE[5], 0.031031423, .5962453956641656 , 0.21931160145739731, 50.0, "");
	auto redmat =  MaterialPhong(REDPINK_PALLETTE[5], 0.031031423, .5962453956641656 , 0.21931160145739731, 50.0, "");


	auto shader = make_shared<ShaderProgram>(
			"C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\rev.vert",
			"C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\bezier.frag");

	auto step = make_shared<RenderingStep>(shader);
		auto step2 = make_shared<RenderingStep>(shader);


//	auto F = RealFunctionR3([](vec3 p) {return pow(p.x, 4) + pow(p.y, 4) + pow(p.z, 4) - 9;});
//	auto ts = TriangulatedImplicitSurface(F);
//	auto s = make_shared<WeakSuperMesh>(ts.compute(10, vec3(1), .05));

//	auto axis_s = make_shared<WeakSuperMesh>(axisSurface, 15, 100, axisId);

//	s->addUniformSurface(axisSurface, 15, 100, axisId);

    s->addGlobalMaterial(midmat);
	s1->addGlobalMaterial(redmat);
//	axis_s->addGlobalMaterial(axmat);
    //
    // oscCircMid->scaleR(oscCircBand->getR()*2, false);
    // oscCircMid->scaleR(oscCircBand->getR()/2, true);


    // supeer->addUniformSurface(curva1(0), 150, 10, curveID);

    step->setWeakSuperMesh(s);
	step->addConstVec4("bdColor", vec4(85.f/255, 193.f/255, 255.f/255, 1.0f));

	step2->setWeakSuperMesh(s1);
	step2->addConstVec4("bdColor", PINK);


    // step->addUniform("intencities", MAT4, spec_uni([camera](float t, const shared_ptr<Shader> &shader) {shader->setUniform("mvp", camera->mvp(t));}));
    renderer.addRenderingStep(step);
	renderer.addRenderingStep(step2);
//	renderer.nonlinearSpeed([speedScrew](float t) { return .4;} );
	renderer.addTimeUniform();

	renderer.setCamera(camera);
	renderer.setLights(lights);
	// renderer.addConstUniform("intencities", VEC4

	return renderer.mainLoop();

}
