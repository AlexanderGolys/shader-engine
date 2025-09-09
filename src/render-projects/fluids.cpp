#include "../engine/specific.hpp"
// #include "../utils/macros.hpp"
#include "../utils/randomUtils.hpp"
#include "../geometry/sph.hpp"
#include "../engine/interface.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared, std::cout, std::endl;

RenderSettings settings = RenderSettings(
	GRAY_PALLETTE[2],	// background color
		true, 			// alpha blending
		true, 			// depth test
		true, 			// time uniform
		1.0f, 			// animation speed
		30,				// max FPS
		false,			// take screenshots
		FHD,			// resolution
		-1.0f,			// screenshot frequency
		"0/0??"		// window title
);



int main() {



	Renderer renderer = Renderer(settings);
	renderer.initMainWindow();

	vec4 blue = BLUE_PALLETTE[0];
	vec4 white = WHITE;
	vec4 blueball = BLUE_PALLETTE[2];
	// blue.w = .1;

	auto ballmat = MaterialPhong(blueball, .25, .151536, 0.2112348, 30.0);
	auto bdmat = MaterialPhong(blue, .1, .3151536, 1.59112348, 250.0);





	PointLight light1 = PointLight(vec3(5,-1, 0), .003, .0005);
	PointLight light2 = PointLight(vec3(-4,-10, -2), .003, .05, BLUE_PALLETTE[0]);
	PointLight light3 = PointLight(vec3(2,6, 5), .03, .005, REDPINK_PALLETTE[0]);

	auto lights = vector<Light>({light1, light2, light3});

	ImplicitVolume vessel = implicitVolumeEllipsoid(1, 1, 2);
	ImplicitVolume init_region = implicitVolumeEllipsoid(.5, .5, 1.1, vec3(0.45,.2, -.1));

	SPH_SETTINGS params = SPH_SETTINGS(
		4.5f,			// rest density
		0.3,			// stiffness
		1.3,			// compressibility
		0.2			// viscosity
	);
	auto gravity = [](vec3 p) { return vec3(0, 0, -4.f); };
	vector<FluidParticle> particles = {};

	for (int i = 0; i < 1000; i++)
		particles.emplace_back(init_region.uniform_random_sample(), 1.f, params.viscosity);

	FluidParticleSystem fluid = FluidParticleSystem(particles, gravity, vessel, Poly6Kernel(.3), params);

	// for (int i = 0; i < 20; i++)
	// 	fluid.update(0.2f);


	float r = .045;
	auto particle_mesh = make_shared<IndexedMesh>(fluid.particle_mesh(r, 1));
	// auto march = fluid.boundary_surface_march(ivec3(50, 50, 50));
	// auto march = fluid.free_surface_march(ivec3(50, 50, 50), 0.1f);
	// auto bd_mesh = make_shared<IndexedMesh>(march, 100, 100);
	auto bd_mesh = make_shared<IndexedMesh>(ellipsoid(1 + r, 1 + r, 2 + r), 100, 100);


	auto shader = ShaderProgram(
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\basic.vert)",
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\fluids.frag)");

	auto shader_bd_back = ShaderProgram(
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\basic.vert)",
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\glass_back.frag)");

	auto shader_bd_front = ShaderProgram(
R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\basic.vert)",
R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\glass_front.frag)");

	shared_ptr<Camera> camera = make_shared<Camera>(
		make_shared<SmoothParametricCurve>([](float t) { return vec3(0, 4, 1); }),
		vec3(0, 0, 0),  vec3(0, 0, 1),  PI/3);

	renderer.setCamera(camera);
	renderer.setLights(lights);

	renderer.addMeshStep(shader_bd_front, bd_mesh, bdmat);
	renderer.addMeshStep(shader, particle_mesh, ballmat);
	renderer.addMeshStep(shader_bd_back, bd_mesh, bdmat);


	renderer.addCustomAction([&](float t, float dt) {
		fluid.update(dt);
		particle_mesh->deformPerId([&](BufferedVertex &b, PolyGroupID id) {
			int i = std::get<int>(id);
			vec4 col = b.getColor();
			vec3 pos0 = vec3(col.y, col.z, col.w);
			vec3 pos1 = fluid[i].pos();
			b.setPosition(b.getPosition() + (pos1 - pos0));
			b.setColor(vec4(fluid[i].p(), pos1.x, pos1.y, pos1.z));
		});

	});

	return renderer.mainLoop();
}
