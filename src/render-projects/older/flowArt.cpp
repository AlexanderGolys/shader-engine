#include "physics/rigid.hpp"
//#include "SDF/SDFRendering.hpp"

using namespace glm;
using std::vector, std::string, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;

float w(float t, float t0, float speed, vec2 x, vec2 x0) {
	float speed_wave = 9.0 - 4*norm(x);
	float phase = 0.07;
	float dumping = .1;
	float w0 = .008;
	return (.3 + 2*abs(cos2(t*speed_wave + phase)))*w0/(dumping*t+1);
}

vec4 color(float t, float t0, float speed, vec2 x, vec2 x0) {
	// return lerp(vec4(.8, .6, 0, 1), vec4(.9, .7, 0, 1), sin(PI*t));
	float d = sin2(x0.x*10 - pow3(x0.y)*5)/10.f;
	float dumping = 2.01;
	float phase = 2.0 + .9*speed;
	return lerp(vec4(.3, .7-d, .9, 1.0), vec4(.8, .3, .3+d, 1.0), .5 - .5*sin(0.4*TAU*t + phase))*saturate(1-1/(pow4(2*t)+1));

}


int main() {
	Renderer renderer = Renderer(.1f, vec4(.0, .0, 0.0, 1.0f));
	renderer.initMainWindow(FHD, "flows");


	float camSpeed = .00;



	auto bdmat = MaterialPhong(BLUE_PALLETTE[5], .07, .6, .48, 15.0);



	// auto X = VectorFieldR2([](vec2 x) { return vec2(1.1*(x.x*x.y + 1.3)*cos(x.y*x.y + x.x*x.x + .1), .7*(-.8 + x.y + 3*cos(1.1+2*x.x-x.y))); });
	auto X = VectorFieldR2([](vec2 x) { return vec2(.3+1.5*x.y*(2+cos(x.y - x.x + 7*norm(x))), .5-2*pow3(x.x)); });

	auto lines = PlanarFlowLines(X, .02, 600, w, color);
	lines.generateGrid(vec2(-1, -1), vec2(1, 1), ivec2(26, 26));
	// lines.generateRandomUniform(vec2(-2.9, -1.9), vec2(2.9, 1.9), 900);

	// lines.generateStartTimesUniform(.6);
	lines.generateStartTimesAll0();
	lines.generateLines();



	auto shader = ShaderProgram( R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\flowart.vert)",
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders2\flowart.frag)");



	renderer.addMeshStep(shader, make_shared<WeakSuperMesh>(lines), bdmat);

	shared_ptr<Camera> camera = make_shared<Camera>(vec3(0, 0.2, 8), vec3(0), vec3(0, 0, 1), PI/8);
	renderer.setCamera(camera);
	renderer.addTimeUniform();


	return renderer.mainLoop();

}
