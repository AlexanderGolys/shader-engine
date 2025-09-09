#include "src/physics/rigid.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;



int main() {
	Renderer renderer = Renderer(.1f, vec4(.89, .89, 0.89, 1.0f), R"(C:\Users\PC\Desktop\ogl-master\screenshots\)", 5.0f);
	renderer.initMainWindow(UHD, "flows");


	float camSpeed = .00;


	PointLight light1 = PointLight(vec3(-1,3, 1), .01, .032);
	PointLight light2 = PointLight(vec3(3, 26, 20), vec4(1, 1, .8, 1), .009, .00132);
	PointLight light3 = PointLight(vec3(1,10, 2), .001, .0302);
	auto lights = vector<Light>({light1, light2, light3});
	auto bdmat = MaterialPhong(BLUE_PALLETTE[5], .07, .6, .48, 15.0);
	auto floormat = MaterialPhong(GRAY_PALLETTE[7], .13, .1, .01, 155.0);
	auto graymat = MaterialPhong(GRAY_PALLETTE[4], .13, .1, .01, 155.0);


//	auto surf =		make_shared<SmoothImplicitSurface>(torusImplicit(.7, 3, ORIGIN, .01));
//	auto surf2 =		make_shared<SmoothImplicitSurface>(torusImplicit(.9, 2.5, ORIGIN, .01));

	auto surf =		make_shared<SmoothImplicitSurface>(chair(5.3, .98, .9, .001));
	auto surf2 =		make_shared<SmoothImplicitSurface>(chair(5, .95, .8, .001));
	auto pen = interpolate(*surf, *surf2);

	auto chunk = MarchingCubeChunk(vec3(-5.3, -5.1, -5), vec3(5.3, 5.3, 5), ivec3(100, 100, 400), surf);
	auto meshChunkProject = make_shared<WeakSuperMesh>(chunk.generateMesh(false, .0001, 30));



	auto shader = ShaderProgram( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.frag)");



	renderer.setLights(lights);


	renderer.addMeshStep(shader, meshChunkProject, bdmat);

	shared_ptr<Camera> camera = make_shared<Camera>(14, .1, 4, vec3(0, 0, 0));
	renderer.setCamera(camera);
	renderer.addTimeUniform();

	renderer.addCustomAction([&meshChunkProject, &pen](float t) {
		pen.deformMesh(t, *meshChunkProject, .001, 100);
	});


	return renderer.mainLoop();

}
