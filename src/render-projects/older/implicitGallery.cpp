#include "src/physics/rigid.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;



int main() {
	Renderer renderer = Renderer(.1f, vec4(.9, .9, 0.9, 1.0f), R"(C:\Users\PC\Desktop\ogl-master\screenshots\)", 5.0f);
	renderer.initMainWindow(UHD, "flows");


	float camSpeed = .00;


	PointLight light1 = PointLight(vec3(-1,0, 6), .01, .032);
	PointLight light2 = PointLight(vec3(3, 26, 20), vec4(1, 1, .8, 1), .009, .00132);
	PointLight light3 = PointLight(vec3(1,10, 2), .001, .0302);
	auto lights = vector<Light>({light1, light2, light3});
	auto bdmat = MaterialPhong(BLUE_PALLETTE[4], .08, .7, .7, 50.0);
	auto floormat = MaterialPhong(GRAY_PALLETTE[7], .13, .1, .01, 155.0);
	auto graymat = MaterialPhong(GRAY_PALLETTE[4], .13, .1, .01, 155.0);



//	auto torus = make_shared<SmoothImplicitSurface>(torusImplicit(.2, 1, ORIGIN, .01));
//	auto chunkTorus = MarchingCubeChunk(vec3(-1.3, -1.3, -.25), vec3(1.3, 1.3, .25), ivec3(100, 100, 50), torus);
//	auto meshTorus = make_shared<IndexedMesh>(chunkTorus.generateMesh(false, .0001, 30));
//
//	auto g2 = make_shared<SmoothImplicitSurface>(genus2Implicit(.001).scale(.7));
//	auto chunkg2 = MarchingCubeChunk(vec3(-1.3, -1.3, -1), vec3(1.3, 1.3, 1), ivec3(100, 100, 100), g2);
//	auto meshg2 = make_shared<IndexedMesh>(chunkg2.generateMesh(false, .0001, 30));
////	meshg2->shift(vec3(0, -3, 0));
//
//	auto wine = make_shared<SmoothImplicitSurface>(wineGlass(.001).scale(.2));
//	auto chunkWine = MarchingCubeChunk(vec3(-1, -1, -.58), vec3(1, 1, 1), ivec3(100, 100, 200), wine);
//	auto meshWine = make_shared<IndexedMesh>(chunkWine.generateMesh(false, .0001, 30));
////	meshWine->shift(vec3(0, 3, 0));
//
//	auto equi = make_shared<SmoothImplicitSurface>(equipotentialSurface({vec3(0, .5, 0), vec3(0, -.6, 0), vec3(0, 0, 0)}, {1, 1, 1}, 7));
//	auto chunkEqui = MarchingCubeChunk(vec3(-1, -1, -1), vec3(1, 1, 1), ivec3(100, 100, 100), equi);
//	auto meshEqui = make_shared<IndexedMesh>(chunkEqui.generateMesh(false, .0001, 30));

//	auto wine = make_shared<SmoothImplicitSurface>(chair().scale(.15));
//	auto chunkWine = MarchingCubeChunk(vec3(-1, -1, -1), vec3(1, 1, 1), ivec3(300, 300, 300), wine);
//	auto meshWine = make_shared<IndexedMesh>(chunkWine.generateMesh(false, .0001, 50));

//	auto surf = make_shared<SmoothImplicitSurface>(gumdrop().scale(.5).rotate(e3, PI/2));
//	auto chunkSurf = MarchingCubeChunk(vec3(-1, -1, -1), vec3(1, 1, 1), ivec3(200, 200, 200), surf);
//	auto meshSurf = make_shared<IndexedMesh>(chunkSurf.generateMesh(false, .0001, 50));

	auto surf = make_shared<SmoothImplicitSurface>(K3Surface222( .001).scale(.3));
	auto chunkSurf = MarchingCubeChunk(vec3(1, 1, 1), ivec3(303, 303, 303), surf);
	auto meshSurf = make_shared<IndexedMesh>(chunkSurf.generateMesh(false, .001, 50));


	auto shader = ShaderProgram( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.frag)");



	renderer.setLights(lights);


//	renderer.addMeshStep(shader, meshTorus, bdmat);
//	renderer.addMeshStep(shader, meshg2, bdmat);
//	renderer.addMeshStep(shader, meshWine, bdmat);
//	renderer.addMeshStep(shader, meshEqui, bdmat);
	renderer.addMeshStep(shader, meshSurf, bdmat);


	shared_ptr<Camera> camera = make_shared<Camera>(3, .5, 1, vec3(0, 0, 0));
	renderer.setCamera(camera);
	renderer.addTimeUniform();


	return renderer.mainLoop();

}
