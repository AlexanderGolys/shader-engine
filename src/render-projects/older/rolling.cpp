#include "src/physics/rigid.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;



int main() {
	Renderer renderer = Renderer(.4f, vec4(.69, .69, 0.69, 1.0f), R"(C:\Users\PC\Desktop\ogl-master\screenshots\)", 5.0f);
	renderer.initMainWindow(FHD, "flows");


	float camSpeed = .00;


	PointLight light1 = PointLight(vec3(-1,3, 1), .01, .0032);
	PointLight light2 = PointLight(vec3(3, 26, 20), vec4(1, 1, .8, 1), .0009, .000132);
	PointLight light3 = PointLight(vec3(1,10, 2), .001, .00302);
	auto lights = vector<Light>({light1, light2, light3});



	auto bdmat = MaterialPhong(BLUE_PALLETTE[7], .04, .5, .8, 55.0);
	auto floormat = MaterialPhong(BLUE_PALLETTE[2], .09, .2, .1, 155.0);


	auto bd = SmoothParametricCurve([](float t) { return vec3(-.1, -.5, 0)*cos(t) + vec3(.25, -.05, 0)*sin(t)*1.5; }, randomID(), 0, TAU, true, .01);
	auto floor = SmoothParametricCurve([](float t) { return vec3(-.1, -.5, 0) + vec3(5*t, -t, 0)/sqrt(26); }, randomID(), 0, 6, false, .01);
	auto rolling = RollingBody(bd, floor, vec3(0, 0, 0), vec3(0, -0.098, 0), 100, 100, .03);

	auto fl = make_shared<IndexedMesh>(icosphere(15, 4, vec3(0, -15, 0), randomID()));



	auto shader = ShaderProgram( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.frag)");

	auto meshRolling = rolling.getMesh();
	auto center = rolling.getCenterMesh();

	auto box = boxRigid(vec3(.1, 3, 1.5), vec3(0, 2, 2), 4, vec3(.2, 1.9, 0),
		vec3(.1, -.1, 2.5),
		vec3(0),  vec3(vec3(0, -.5, 0)), rotationMatrix(vec3(.1, .2, 0.2)));

	auto boxmesh = make_shared<IndexedMesh>(box.second);

	renderer.addTimeUniform();
	renderer.setLights(lights);
//	renderer.addMeshStep(shader, meshRolling, bdmat);
//	renderer.addMeshStep(shader, center, bdmat);
//	renderer.addMeshStep(shader, rolling.getFloorMesh(), floormat);
	renderer.addMeshStep(shader, boxmesh, bdmat);
	renderer.addMeshStep(shader, fl, floormat);

//	box.first.freeMotion(.1, *boxmesh);



	SmoothParametricCurve camCurve = SmoothParametricCurve([](float t) { return vec3(3, 8, 10); }, 0, TAU, .1, true, .001);
	SmoothParametricCurve look = SmoothParametricCurve([&box](float t) { return box.first.getCM(); }, 0, TAU, .1, true, .01);
	shared_ptr<Camera> camera = make_shared<Camera>(make_shared<SmoothParametricCurve>(camCurve), make_shared<SmoothParametricCurve>(look), vec3(0, 1, 0), PI/4);
	renderer.setCamera(camera);

	renderer.addCustomAction([&meshRolling, &rolling, &center, &box, &boxmesh](float t, float dt) {
//        rolling.step(.1, *meshRolling, *center);
		box.first.freeMotion(dt, *boxmesh);
    });
//	rolling.step(3, *meshRolling);

	return renderer.mainLoop();

}
