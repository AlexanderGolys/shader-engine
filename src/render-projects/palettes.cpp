#include "../engine/specific.hpp"
#include "../engine/interface.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared, std::cout, std::endl;

RenderSettings settings = RenderSettings(
	vec4(.22, .22, .22, 1),	// background color
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

vector<COLOR_PALETTE10> palettes = {
		BLUE_PALLETTE, REDPINK_PALLETTE, GREEN_PALLETTE, GRAY_PALLETTE
};

float cam_speed = 0.f;
vec3 cam_pos = vec3(0, 0, 8);
float cam_r = .1f; // camera radius



int main() {
	Renderer renderer = Renderer(settings);
	renderer.initMainWindow();


	float x_min = -5.0f;
	float x_max = 5.0f;
	float y_min = -4.0f;
	float y_max = 4.0f;
	float dx = (x_max - x_min) / palettes.size();
	float dy = (y_max - y_min) / 10.f;

	auto floor = make_shared<IndexedMesh>(paraleblahblapid(vec3(-10, -10, -.1), e1*0, e2*0, -e3*0));

	for (int i = 0; i < palettes.size(); i++)
		for (int j = 0; j < 10; j++)
			floor->merge(box(vec3(dx*.6, dy*.8, .1), vec3(x_min + dx*i, y_min + dy*j, 0), palettes[i][j], randomID()));




	auto floormat = MaterialPhong();



	auto shader = ShaderProgram(
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\basic.vert)",
	R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\vcolor_paint.frag)");

	shared_ptr<Camera> camera = make_shared<Camera>(
		make_shared<SmoothParametricCurve>([cam_speed=cam_speed, cam_pos=cam_pos, cam_r=cam_r](float t) { return cam_pos + vec3(sin(t*cam_speed)*cam_r, cos(t*cam_speed)*cam_r, 0); }),
		vec3(0, 0, 0),  vec3(0, 0, 1),  PI/3);

	renderer.setCamera(camera);



	renderer.addMeshStep(shader, floor, floormat);


	return renderer.mainLoop();
}
