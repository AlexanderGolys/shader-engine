#include "specific.hpp"
#include "interface.hpp"
#include "mat.hpp"
#include "glslUtils.hpp"

using namespace glm;


RenderSettings settings = RenderSettings(
	GRAY_PALLETTE[3],	// background color
		true, 			// alpha blending
		true, 			// depth test
		true, 			// time uniform
		0.02f, 			// animation speed
		75,				// max FPS
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
	.radius = 0.02f,
	.horRes = 3033,
	.radialRes = 8,
	.extra_defaults = {},
	.bounding = true,
	.bound_min = vec3(-10, -10, -10),
	.bound_max = vec3(10, 10, 10),
	.discontinuities = {-sqrt(6.f), -1, 1, sqrt(6.f)},
};

PIPE_SETTINGS pipe_infaxis = PIPE_SETTINGS{
	.radius = 0.02f,
	.horRes = 50,
	.radialRes = 13,
	.extra_defaults = {}
};

PIPE_SETTINGS pipe_ass = PIPE_SETTINGS{
	.radius = 0.01f,
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


	auto f = [](float t){
		return t*t/(t*t-1)*(t*t-4)/(t*t-6);
	};

	auto projcurve = [f](float x){
		return SmoothParametricCurve(
			[f=f, x](float t){
				float xx = saturate(x);
				vec2 p = stereoProjectionInverse(f(t))*xx + vec2(f(t), 0)*(1-xx);
				return vec3(t, p.x, p.y);
		}, "p1", -10, 10, false, 0.001f);
	};



	auto projsurf = [](float x){
		return SmoothParametricSurface([x](float t, float u){
			auto xx = saturate(x);
			vec2 p = stereoProjectionInverse(u)*xx + vec2(u, 0)*(1-xx);
			return vec3(t, p.x, p.y);
		}, vec2(-10, 10), vec2(-10, 10), false, false, 0.01f);
	};


	auto flatcurve = SmoothParametricCurve([f=f](float t){
		return vec3(t, f(t), 0);
	}, "p1", -10, 10, false, 0.01f);


	auto infline = SmoothParametricCurve([](float t){
		return vec3(t, 0, 1);
	}, "p1", -10, 10, false, 0.01f);

	auto zeroline = SmoothParametricCurve([](float t){
		return vec3(t, 0, 0);
	}, "p1", -10, 10, false, 0.01f);


	auto flatsurf = SmoothParametricSurface([](float t, float u){
		return vec3(t, u, 0);
	}, vec2(-10, 10), vec2(-10, 10), false, false, 0.01f);


	auto floor = make_shared<IndexedMesh>(flatsurf, 100, 300);
	auto floor_proj = make_shared<IndexedMesh>(projsurf(0), 100, 300);

	auto proj = make_shared<PipeCurveVertexShader>(projcurve(0), pipe_settings_comp);
	auto flat = make_shared<IndexedMesh>(PipeCurveVertexShader(flatcurve, pipe_settings));

	auto infaxis = make_shared<IndexedMesh>(PipeCurveVertexShader(infline, pipe_infaxis));
	infaxis->merge(PipeCurveVertexShader(zeroline, pipe_infaxis));

	PointLight light1 = PointLight(vec3(5,-1, 2), .003f, .005f);
	PointLight light2 = PointLight(vec3(-4,-10, -2), .003, .005, BLUE_PALLETTE[0]);
	PointLight light3 = PointLight(vec3(2,6, 5), .003, .005, REDPINK_PALLETTE[0]);

	auto lights = vector<Light>({light1, light2, light3});


	auto floormat = make_shared<MaterialPhong>(BLUE_PALLETTE[1], .25, .151536, 0.42112348, 70.0);
	auto projmat = make_shared<MaterialPhong>(BLUE_PALLETTE[5], .15, .39, 0.59, 2.0);
	auto flatmat = make_shared<MaterialPhong>(BLUE_PALLETTE[2], .15, .39, 0.59, 2.0);
	auto inflinemat = make_shared<MaterialPhong>(REDPINK_PALLETTE[5], .15, .73, 0.59, 50.0);


	auto shader_curve = ShaderProgram(
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders2\curve.vert)",
		R"(C:\Users\PC\Desktop\ogl-master\src\shaders\shaders_curves\curv_basic.frag)");


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

	renderer.addMeshStep(shader_curve, proj, projmat);
	renderer.addMeshStep(shader_curve, infaxis, inflinemat);


	renderer.addMeshStep(shader_floor_front, floor_proj, floormat);
	renderer.addMeshStep(shader_floor_back, floor_proj, floormat);

	renderer.addCustomAction([&floor_proj, &projsurf, &projcurve, &proj](float t){
		floor_proj->adjustToNewSurface(projsurf(t));
		proj->updateCurve(projcurve(t));
	});

	return renderer.mainLoop();
}
