#include "src/physics/rigid.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;



int main() {
	Renderer renderer = Renderer(.4f, vec4(.89, .89, 0.89, 1.0f), R"(C:\Users\PC\Desktop\ogl-master\screenshots\)", 5.0f);
	renderer.initMainWindow(UHD, "flows");


	float camSpeed = .00;


	PointLight light1 = PointLight(vec3(-1,3, 1), .01, .032);
	PointLight light2 = PointLight(vec3(3, 26, 20), vec4(1, 1, .8, 1), .009, .00132);
	PointLight light3 = PointLight(vec3(1,10, 2), .001, .0302);
	auto lights = vector<Light>({light1, light2, light3});
	auto bdmat = MaterialPhong(BLUE_PALLETTE[5], .07, .6, .48, 15.0);
	auto floormat = MaterialPhong(GRAY_PALLETTE[7], .13, .1, .01, 155.0);
	auto graymat = MaterialPhong(GRAY_PALLETTE[4], .13, .1, .01, 155.0);


//	auto monkey = make_shared<IndexedMesh>("C:\\Users\\PC\\Desktop\\meshes\\monkLow.obj", 21);
//	monkey->deformWithAmbientMap(SpaceAutomorphism::rotation(vec3(1, 0, 0), PI/2, vec3(0)));
	auto surf =		SpaceAutomorphism::affine(mat3(1), vec3(0, 0, 0)) &
										SpaceAutomorphism::scaling(vec3(1.3, 1, 2.2)) &
											SpaceAutomorphism::rotation(vec3(0, 0, 1), PI/2, vec3(0)) &
												DupinCyclide(2, 1.9, .8);

//	auto heli =		SpaceAutomorphism::affine(mat3(1), vec3(-3, 0, 0)) &
//										SpaceAutomorphism::rotation(vec3(0, 0, 1), PI/2, vec3(0)) &
//										hyperbolic_helicoid(TAU);

	auto heli =		SpaceAutomorphism::affine(mat3(1), vec3(2
		, 0, 0)) &
									twistedTorus(2, 1, 2, 1, 1);


	surf.changeDomain(vec2(0, TAU), vec2(3*PI/8, PI + 7*PI/8), true, false);
//	heli.changeDomain(vec2(0, PI/3), vec2(0, PI), true, false);

	auto surfmesh = make_shared<IndexedMesh>(surf, 50, 60, 222) ;
	auto helimesh = make_shared<IndexedMesh>(heli, 50, 60, 223) ;

	auto wire = make_shared<Wireframe>(surf, .01, 15, 15, 80, 7);
	auto wire2 = make_shared<Wireframe>(heli, .01, 5, 5, 50, 7);
//
	surfmesh->orientFaces();

	// surfmesh->deformPerVertex([&surf](BufferedVertex &v) {
	// 	v.setColor(surf.meanCurvature(v.getSurfaceParams()), 3); });
	// helimesh->deformPerVertex([&heli](BufferedVertex &v) {
	// 	v.setColor(heli.meanCurvature(-v.getSurfaceParams()), 3); });


	auto shader = ShaderProgram( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\curvFlowShd.frag)");

	auto shaderWire = ShaderProgram( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.frag)");


	renderer.setLights(lights);
//	renderer.addMeshStep(shader, surfmesh, bdmat);
	renderer.addMeshStep(shader, helimesh, bdmat);

	renderer.addMeshStep(shaderWire, wire, floormat);
	renderer.addMeshStep(shaderWire, wire2, floormat);




	shared_ptr<Camera> camera = make_shared<Camera>(13, .0, 4);
	renderer.setCamera(camera);
	renderer.addTimeUniform();

//
//	float timer = 0;
//	renderer.addCustomAction([&surfmesh, &timer](float t, float dt) {
//		if (timer < 1) {
//			timer += dt;
//			return;
//		}
//		surfmesh->meanCurvatureFlowDeform(dt, 222);
//		timer = 0;
//    });

	return renderer.mainLoop();

}
