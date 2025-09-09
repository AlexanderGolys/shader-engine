#include "src/physics/rigid.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;



int main() {
	Renderer renderer = Renderer(2.0f, vec4(.69, .69, 0.69, 1.0f), R"(C:\Users\PC\Desktop\ogl-master\screenshots\)", 5.0f);
	renderer.initMainWindow(UHD, "flows");

	float camSpeed = .00;

	PointLight light1 = PointLight(vec3(-1,3, 1), .01, .0032);
	PointLight light2 = PointLight(vec3(3, 26, 20), vec4(1, 1, .8, 1), .0009, .000132);
	PointLight light3 = PointLight(vec3(1,10, 2), .001, .00302);
	auto lights = vector<Light>({light1, light2, light3});

	auto midmat =  MaterialPhong(BLUE_PALLETTE[7], 0.031, 1.5932, 1.2, 15.0);
//	midmat.addAmbientTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture2.bmp)");
//	midmat.addDiffuseTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\metal4.bmp)");
//	midmat.addSpecularTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\metal4.bmp)");

	auto redmat =  MaterialPhong(REDPINK_PALLETTE[7], .07, .62 , 0.64, 55.0);
//	redmat.addAmbientTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture6.bmp)");
	redmat.addDiffuseTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture_red2.bmp)");
	redmat.addSpecularTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture_red3.bmp)");

	auto bdmat = MaterialPhong(BLUE_PALLETTE[7], .04, .5, .8, 55.0);
	auto floormat = MaterialPhong(BLUE_PALLETTE[3], .09, .2, .1, 155.0);

//	floormat.addAmbientTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture7.bmp)");
//	floormat.addDiffuseTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture7.bmp)");

	auto floor = make_shared<WeakSuperMesh>(disk3d(30, vec3(0, 0, -1), e1, e2, 50, 5, "v"));

	auto shader = Shader( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
						  R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.frag)");

	auto lightShd = Shader( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
							R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\lightShader.frag)");

	auto shdDevelop = Shader( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
							  R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemoDevelop.frag)");

	auto shdDevelop2 = Shader( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
							   R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemoDevelop2.frag)");

	renderer.addMeshStep(shader, floor, floormat);

//	renderer.addMultiMeshStep(shader, {ico, ico2}, midmat);
	renderer.addTimeUniform();

//	renderer.setLightsWithMesh(lights, .1, .5, 1, 100, lightShd, .07);
	renderer.setLights(lights);


	float mass1 = 1;
	float mass2 = 2;
	float shift2 = 5;

	auto mesh = make_shared<WeakSuperMesh>(disk3d(1, vec3(0, 0, 0), e1, e2, 100, 5, "v"));
	mesh->deformWithAmbientMap("v", SpaceEndomorphism([](vec3 v) {return vec3(v.x*v.x*v.x-v.y*v.y*2-v.x*2, -2*v.y, v.z); }, .01));

	auto F = vec3(0.01, 0, 0);
	vec3 p0 = mesh->getVertices("v")[221].getPosition();
	RigidBodyTriangulated2D body = RigidBodyTriangulated2D(mesh, vec3(0), vec3(0, 0, 0), vec3(0, 0, .0), vec3(0));
	auto centerico = make_shared<WeakSuperMesh>(icosphere(.04, 2, body.getCm(), randomID()));
	auto forceArrow = make_shared<WeakSuperMesh>(SmoothParametricCurve::span(p0, p0+normalise(F)*3).pipe(.02, false), 20, 20, randomID());
	auto p0Ico = make_shared<WeakSuperMesh>(icosphere(.04, 2, p0, randomID()));

	renderer.addMeshStep(shader, mesh, bdmat);
	renderer.addMeshStep(shader, p0Ico, redmat);
	renderer.addMeshStep(shader, centerico, redmat);
	renderer.addMeshStep(shader, forceArrow, redmat);

	renderer.addCustomAction([&body, F, mass1, &p0, &mesh, &p0Ico, &centerico, &forceArrow](float t, float dt) {
			body.setLinearAcceleration(F/mass1);
			vec3 p0old = p0;
			vec3 center = body.getCm();
			body.setAngularAcceleration(-cross(p0-center, F)/(mass1*body.getI()[2][2]));
			body.update(dt);
			p0 = mesh->getVertices("v")[221].getPosition();

			p0Ico->shift(p0 - p0old);
			centerico->shift(body.getCm() - center);
			forceArrow->shift(p0 - p0old);
		});









	auto mesh2 = make_shared<WeakSuperMesh>(disk3d(1, vec3(0, 0, 0), e1, e2, 100, 5, "v"));
	mesh2->deformWithAmbientMap("v", SpaceEndomorphism([shift2](vec3 v) {return vec3(v.x*v.x*v.x-v.y*v.y*2-v.x*2, -2*v.y+shift2, v.z); }, .01));

	vec3 p02 = mesh2->getVertices("v")[221].getPosition();
	RigidBodyTriangulated2D body2 = RigidBodyTriangulated2D(mesh2, vec3(0), vec3(0, 0, 0), vec3(0, 0, .0), vec3(0));
	auto centerico2 = make_shared<WeakSuperMesh>(icosphere(.04, 2, body2.getCm(), randomID()));
	auto forceArrow2 = make_shared<WeakSuperMesh>(SmoothParametricCurve::span(p02, p02+normalise(F)*3).pipe(.02, false), 20, 20, randomID());
	auto p0Ico2 = make_shared<WeakSuperMesh>(icosphere(.04, 2, p02, randomID()));

	renderer.addMeshStep(shader, mesh2, bdmat);
	renderer.addMeshStep(shader, p0Ico2, redmat);
	renderer.addMeshStep(shader, centerico2, redmat);
	renderer.addMeshStep(shader, forceArrow2, redmat);

	renderer.addCustomAction([&body2, F, mass2, &p02, &mesh2, &p0Ico2, &centerico2, &forceArrow2](float t, float dt) {
			body2.setLinearAcceleration(F/mass2);
			vec3 p0old = p02;
			vec3 center = body2.getCm();
			body2.setAngularAcceleration(-cross(p02-center, F)/(mass2*body2.getI()[2][2]));
			body2.update(dt);
			p02 = mesh2->getVertices("v")[221].getPosition();

			p0Ico2->shift(p02 - p0old);
			centerico2->shift(body2.getCm() - center);
			forceArrow2->shift(p02 - p0old);
		});








	SmoothParametricCurve camCurve = SmoothParametricCurve([](float t) { return vec3(-4, 0, 15); }, 0, TAU, .1, true, .01);
	SmoothParametricCurve look = SmoothParametricCurve([&body](float t) { return body.getCm(); }, 0, TAU, .1, true, .01);
	shared_ptr<Camera> camera = make_shared<Camera>(make_shared<SmoothParametricCurve>(camCurve), make_shared<SmoothParametricCurve>(look), vec3(0, 0, 1), PI/4);
	renderer.setCamera(camera);

	return renderer.mainLoop();

}
