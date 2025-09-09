#include "physics/fluidsSpecific.hpp"
#include "src/common/specific.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;



float pseudorand(float seed) {
	return sin(sin(seed * 12.9898) * 43758.5453);
}



int main() {
	Renderer renderer = Renderer(.05, vec4(.02, .02, 0.1, 1.0f), R"(C:\Users\PC\Desktop\ogl-master\screenshots\)", 5.0f);
	renderer.initMainWindow(UHD, "flows");


	float camSpeed = .00;

	auto flow = PousevillePlanarFlow(2, 10, 1, 0);
	auto part = make_shared<WeakSuperMesh>(particles(3000, vec3(-2, -2, 0), vec3(2, 2, 1), .03));


	PointLight light1 = PointLight(vec3(-1,3, 1), .01, .0032);
	PointLight light2 = PointLight(vec3(3, 26, 20), vec4(1, 1, .8, 1), .0009, .000132);
	PointLight light3 = PointLight(vec3(1,10, 2), .001, .00302);
	auto lights = vector<Light>({light1, light2, light3});

	auto midmat =  MaterialPhong(BLUE_PALLETTE[7], 0.031, 1.5932, 1.2, 15.0);
	//	midmat.addAmbientTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture2.bmp)");
	//	midmat.addDiffuseTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\metal4.bmp)");
	//	midmat.addSpecularTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\metal4.bmp)");

	auto redmat =  MaterialPhong(REDPINK_PALLETTE[7], .04, .72 , 0.74, 85.0);
	//	redmat.addAmbientTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture6.bmp)");
	redmat.addDiffuseTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture_red2.bmp)");
	redmat.addSpecularTexture(R"(C:\Users\PC\Desktop\ogl-master\src\textures\texture_red3.bmp)");

	auto bdmat = MaterialPhong(BLUE_PALLETTE[7], .06, .5, .8, 55.0);
	auto floormat = MaterialPhong(BLUE_PALLETTE[3], .09, .2, .1, 155.0);

	auto field  = VectorField([](vec3 v) {return vec3(v.y+sin(5*v.y+v.x*3)/1.5, -v.x-sin(13*v.y*v.x+.1)*2, 0); }, .01);
//	vector<vec3> gridArrowPlacement = flatten(grid(vec3(0, 0, 0), e1/60.f, e2/60.f, 120, 120));
	vector<vec3> gridArrowPlacement = flatten(flatten(grid(vec3(-.5, -.5, 0), e1/5, e2/5, e3/5, 10, 20, 10)));

	for (auto &v : gridArrowPlacement) v +=vec3(pseudorand(v.y+2*v.z), pseudorand(v.x+v.z), 0)*2;

	auto l =  [](float s) {return lerpClamp(0.001f, 0.3f, s*s);};
	auto head_r =  [](float s) {return lerpClamp(0.001, 0.01, s);};
	auto r =  [](float s) {return lerpClamp(0.001, 0.005,s*s );};
	auto head_len =  [](float s){return lerpClamp(0.001, 0.04, s);};
	auto meshArr = make_shared<WeakSuperMesh>(drawVectorFieldArrows(flow , gridArrowPlacement, l, r, head_len, head_r, 5, 5, .1, randomID()));

	auto wave = freeSurface({1, 2, 3.2}, {1, .2, 2.5}, {vec2(.1), vec2(.1), vec2(.2)}, {1, .1, .5}, 2);
	auto meshWave = make_shared<WeakSuperMesh>(wave(0), 50, 50);

	auto plates =make_shared<WeakSuperMesh>( disk3d(3, vec3(0, 0, -.1), e1, e2, 10, 10, randomID()));
	plates->merge(disk3d(3, vec3(0, 0, 1.1), e1, e2, 10, 10, randomID()));
	plates->merge(disk3d(3, vec3(0, 0, 1.15), e1, e2, 10, 10, randomID()));
	plates->merge(disk3d(3, vec3(0, 0, 1.2), e1, e2, 10, 10, randomID()));

	auto shader = ShaderProgram( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.frag)");
	auto lightShd = ShaderProgram( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\lightShader.frag)");
	auto shdDevelop = ShaderProgram( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemoDevelop.frag)");
	auto shdDevelop2 = ShaderProgram( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemo.vert)",
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\shadingDemoDevelop2.frag)");

//	renderer.addMeshStep(shader, part, redmat);
//	renderer.addMeshStep(shader, meshWave, bdmat);
//	renderer.addMeshStep(shdDevelop, plates, bdmat);
	renderer.addMeshStep(shader, meshArr, redmat);

//	renderer.addMultiMeshStep(shader, {ico, ico2}, midmat);
	renderer.addTimeUniform();

//	renderer.setLightsWithMesh(lights, .1, .5, 1, 100, lightShd, .07);
	renderer.setLights(lights);

	shared_ptr<Camera> camera = make_shared<Camera>(10, 1, 2, vec3(0, 0, 1), vec3(0, 0, 1), PI/4);
	renderer.setCamera(camera);

//	renderer.addSurfaceFamilyDeformer(wave, *meshWave);




//	renderer.addCustomAction([&flow, &part](float t, float dt) {
//		part->deformPerVertex( [&flow, dt, t](BufferedVertex &v) {
//			vec3 p = vec3(v.getColor());
//			v.setPosition(v.getPosition() + flow(p + vec3(pseudorand(p.x), pseudorand(p.x+1), pseudorand(p.x+2))*dt/1.1)*dt);
//			v.setColor(vec4(p + flow(p+ vec3(pseudorand(p.x), pseudorand(p.x+1), pseudorand(p.x+2))*dt/1.1)*dt , 0));
//		});
//	});
	return renderer.mainLoop();

}
