#include "src/fundamentals/solvers.hpp"
#include "src/physics/fluidsSpecific.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;



int main() {
	Renderer renderer = Renderer(2.0f, vec4(.69, .69, 0.69, 1.0f), R"(C:\Users\PC\Desktop\ogl-master\screenshots\)", 5.0f);
	renderer.initMainWindow(UHD, "flows");


	float camSpeed = .09;
	shared_ptr<Camera> camera = make_shared<Camera>(6, camSpeed, 5, vec3(0, 0, 1));

	PointLight light1 = PointLight(vec3(-1,3, 1), .01, .032);
	PointLight light2 = PointLight(vec3(3, .1, 0), vec4(1, 1, .8, 1), .09, .0132);
	PointLight light3 = PointLight(vec3(1,-3, 2), .01, .032);
	auto lights = vector<Light>({light1, light2, light3});

	auto midmat =  MaterialPhong(BLUE_PALLETTE[8], 0.021, .932, 1.2, 15.0);
//	midmat.addAmbientTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture2.bmp)");
//	midmat.addDiffuseTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\metal4.bmp)");
//	midmat.addSpecularTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\metal4.bmp)");

	auto redmat =  MaterialPhong(REDPINK_PALLETTE[7], .07, .2 , 0.4, 55.0);
//	redmat.addAmbientTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture6.bmp)");
	redmat.addDiffuseTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture_red2.bmp)");
	redmat.addSpecularTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture_red3.bmp)");

	auto bdmat = MaterialPhong(BLUE_PALLETTE[9], .03, .5, .8, 55.0);
	auto floormat = MaterialPhong(BLUE_PALLETTE[4], .07, .5, .1, 55.0);

//	floormat.addAmbientTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture7.bmp)");
//	floormat.addDiffuseTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture7.bmp)");


	auto floor = make_shared<WeakSuperMesh>(disk3d(20, vec3(0, 0, -1), e1, e2, 100, 5, "v"));

	auto dupin = make_shared<WeakSuperMesh>(DupinCyclide(1, 1.1, .6).shift(vec3(3, 0, 0)), 200, 100, "d");
	auto sph = make_shared<WeakSuperMesh>(circle(1).embedding(e1, e3).pipe(.25).shift(vec3(3, -3, 0)), 100, 40, randomID());
	auto tref = WeakSuperMesh(trefoil(.33, .7, .1).pipe(.07).shift(vec3(3, 3, 0)), 300, 30, randomID());

	dupin->merge(tref);

	SmoothParametricCurve cDown = SmoothParametricCurve([](float t) {return vec3(t, -1, .2*sin(PI*t)*sin(PI*t)); }, randomID(),-1, 1, false, .01);
	SmoothParametricCurve cUp = SmoothParametricCurve([](float t) {return vec3(t, 1, .2*sin(TAU*t)*sin(TAU*t)); }, randomID(),-1, 1, false, .01);
	SmoothParametricCurve cUpInv = SmoothParametricCurve([](float t) {return vec3(-t, 1, .2*sin(-TAU*t)*sin(-TAU*t)); }, randomID(),-1, 1, false, .01);
	SmoothParametricCurve cLeft = SmoothParametricCurve([](float t) {return vec3(-1, t, .5*t*sin(TAU*t)); }, randomID(),-1, 1, false, .01);
	SmoothParametricCurve cRight = SmoothParametricCurve([](float t) {return vec3(1, t, .5*(t-1)*(t+1)); }, randomID(),-1, 1, false, .01);
	SmoothParametricCurve cRightInv = SmoothParametricCurve([](float t) {return vec3(1, -t, .5*(-t-1)*(-t+1)); }, randomID(),-1, 1, false, .01);

	SmoothParametricCurve cLeft2 = SmoothParametricCurve([](float t) {return vec3(-1, -1-.5*(t+1)/2*((t+1)/2-1)*((t+1)/2-.5)+(t+1)/2/2.f, 2*(t+1)/2); }, randomID(),-1, 1, false, .01);
	SmoothParametricCurve cRight2 = SmoothParametricCurve([](float t) {return vec3(1, -1+.5*(t+1)/2*cos(3*PI*(t+1)/2)+(t+1)/4, 2*(t+1)/2); }, randomID(),-1, 1, false, .01);
	SmoothParametricCurve cUp2 = SmoothParametricCurve([](float t) {return vec3(t, -1-.3*sin(PI*t)*sin(PI*t)-(t-1)/4, 2); }, randomID(),-1, 1, false, .01);

	SmoothParametricCurve diag = SmoothParametricCurve([](float t) {return vec3(t-t*(t-1)*(t+1)/4, -t-(t+1)/4-(t-1)*(t+1)+(t+1)/4, t+1-1.1*(t-1)*(t+1)); }, randomID(),-1, 1, false, .01);
//	SmoothParametricCurve diag2 = SmoothParametricCurve([](float t) {return vec3(-t-(-t-1)*(-t+1)/4, t-(-t+1)/4-(-t-1)*(-t+1)+(t+1)/4, -t+1-1.5*(-t-1)*(-t+1)); }, randomID(),-1, 1, false, .01);

//	SmoothParametricCurve closed1 = SmoothParametricCurve([](float t) {return vec3(cos(3*t)/5-1.1, -cos(t)/1.9, sin(t)/1.8+ .1*sin(5*t)+1); }, randomID(), 0, TAU, true, .01);
//	SmoothParametricCurve closed2 = SmoothParametricCurve([](float t) {return vec3(sin(3*t)/13+1, -cos(t)/5 + sin(t)*sin(t)/14, sin(t)/5+1); }, randomID(), 0, TAU, true, .01);

	auto bdCoon = make_shared<WeakSuperMesh>(cDown.pipe(.01, .1, false), 100, 5, randomID());
	bdCoon->addUniformSurface(cUp.pipe(.01, .03, false), 100, 15, randomID());
	bdCoon->addUniformSurface(cLeft.pipe(.01, .1, false), 100, 5, randomID());
	bdCoon->addUniformSurface(cRight.pipe(.01, .05, false), 100, 5, randomID());

	bdCoon->addUniformSurface(cLeft2.pipe(.01, .1, false), 100, 5, randomID());
	bdCoon->addUniformSurface(cRight2.pipe(.01, .05, false), 100, 5, randomID());
	bdCoon->addUniformSurface(cUp2.pipe(.01, .05, false), 100, 5, randomID());
	bdCoon->addUniformSurface(diag.pipe(.01, .1, false), 100, 15, randomID());
//	bdCoon->addUniformSurface(closed2.pipe(.01, .05, false), 100, 5, randomID());


	auto coon = CoonsPatch(cDown, cLeft, cUp, cRight);
	auto coon2 = CoonsPatch(cDown,cLeft2 , cUp2, cRight2);
//	auto coon3 = CoonsPatchDisjoint(closed1, closed2);
//	auto coon2 = coon.meanCurvatureFlow(.01);
	auto coon3 = CoonsPatch(cLeft, cLeft2, cUp2, diag);
	auto coon4 = CoonsPatch(cUp, cRightInv, cRight2, diag);

	auto mesh_coon1 = make_shared<WeakSuperMesh>(coon,  100, 100, randomID());
//	mesh_coon1->addUniformSurface(coon2, 100, 100, randomID());
	auto mesh_coon2 = make_shared<WeakSuperMesh>(coon2, 100, 100, randomID());
	auto mesh_coon3 = make_shared<WeakSuperMesh>(coon3, 100, 100, randomID());
	auto mesh_coon4 = make_shared<WeakSuperMesh>(coon4, 100, 100, randomID());


	mesh_coon1->flipNormals();
//	mesh_coon2->flipNormals();
	mesh_coon3->flipNormals();
//	mesh_coon4->flipNormals();


	auto shader = ShaderProgram( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.frag)");
	auto lightShd = ShaderProgram( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\lightShader.frag)");
	auto shdDevelop = ShaderProgram( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemoDevelop.frag)");
	auto shdDevelop2 = ShaderProgram( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemoDevelop2.frag)");

	renderer.addMeshStep(shader, floor, floormat);
//	renderer.addMeshStep(shader, sph, midmat);
//	renderer.addMeshStep(shader, dupin, redmat);

	renderer.addMeshStep(shader, bdCoon, bdmat);
	renderer.addMeshStep(shdDevelop, mesh_coon1, midmat);
	renderer.addMeshStep(shdDevelop, mesh_coon2, midmat);
	renderer.addMeshStep(shdDevelop2, mesh_coon3, midmat);
	renderer.addMeshStep(shdDevelop2, mesh_coon4, midmat);

//	renderer.addMultiMeshStep(shader, {ico, ico2}, midmat);
	renderer.addTimeUniform();

	renderer.setCamera(camera);
//	renderer.setLightsWithMesh(lights, .1, .5, 1, 100, lightShd, .07);
	renderer.setLights(lights);


	return renderer.mainLoop();

}
