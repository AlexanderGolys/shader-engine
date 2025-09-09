#include "../engine/specific.hpp"
#include "../engine/interface.hpp"

using namespace glm;


RenderSettings settings = RenderSettings(
	GRAY_PALLETTE[3],	// background color
		true, 			// alpha blending
		true, 			// depth test
		true, 			// time uniform
		0.1f, 			// animation speed
		30,				// max FPS
		false,			// take screenshots
		HD2K,			// resolution
		-1.0f,			// screenshot frequency
		"0/0??"		// window title
);

PIPE_SETTINGS pipe_settings = PIPE_SETTINGS{
	.radius = 0.02f,
	.horRes = 10000,
	.radialRes = 13,
	.extra_defaults = {},
	.bounding = true,
	.bound_min = vec3(-10, -10, -10),
	.bound_max = vec3(10, 10, 10),
	.discontinuities = {-sqrt(6.f), -1, 1, sqrt(6.f)},

};
PIPE_SETTINGS pipe_settings_comp = PIPE_SETTINGS{
	.radius = 0.04f,
	.horRes = 600,
	.radialRes = 13,
	.extra_defaults = {}
};

PIPE_SETTINGS pipe_infaxis = PIPE_SETTINGS{
	.radius = 0.02f,
	.horRes = 50,
	.radialRes = 13,
	.extra_defaults = {}
};

PIPE_SETTINGS pipe_ass = PIPE_SETTINGS{
	.radius = 0.005f,
	.horRes = 500,
	.radialRes = 13,
	.extra_defaults = {}
};

float cam_speed = 1.f;
vec3 cam_pos = vec3(0, 0, 3);
float cam_r = 5.5f; // camera radius



int main() {
	Renderer renderer = Renderer(settings);
	renderer.initMainWindow();


	auto ico = make_shared<IndexedMesh>(icosahedron(1.f, e3*2.f, randomID()));
	auto floor = make_shared<IndexedMesh>(paraleblahblapid(vec3(-8, -5, -.1), e1*16, e2*10, e3*1));


	PointLight light1 = PointLight(vec3(5,-1, 2), .003, .005);
	PointLight light2 = PointLight(vec3(-4,-10, -2), .003, .005, BLUE_PALLETTE[0]);
	PointLight light3 = PointLight(vec3(2,6, 5), .003, .005, REDPINK_PALLETTE[0]);

	auto lights = vector<Light>({light1, light2, light3});


	auto floormat = MaterialPhong(BLUE_PALLETTE[1], .25, .151536, 0.42112348, 70.0);
	auto projmat = MaterialPhong(BLUE_PALLETTE[4], .15, .39, 0.59, 2.0);
	auto flatmat = MaterialPhong(BLUE_PALLETTE[2], .15, .39, 0.59, 2.0);
	auto inflinemat = MaterialPhong(REDPINK_PALLETTE[3], .15, .73, 0.59, 50.0);


	auto shader_curve = ShaderProgram(
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\basic.vert)",
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\basic.frag)");


	auto shader_floor_back = ShaderProgram(
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\basic.vert)",
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\glass_back.frag)");

	auto shader_floor_front = ShaderProgram(
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\basic.vert)",
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\glass_front.frag)");

	shared_ptr<Camera> camera = make_shared<Camera>(
		make_shared<SmoothParametricCurve>([cam_speed=cam_speed, cam_pos=cam_pos, cam_r=cam_r](float t){
			return cam_pos + vec3(sin(t*cam_speed)*cam_r, cos(t*cam_speed)*cam_r, 0);
		}), vec3(0, 0, 0),  vec3(0, 0, 1),  PI/3);

	renderer.setCamera(camera);
	renderer.setLights(lights);

	renderer.addMeshStep(shader_curve, ico, projmat);


	renderer.addMeshStep(shader_floor_front, floor, floormat);
	renderer.addMeshStep(shader_floor_back, floor, floormat);

	return renderer.mainLoop();
}
