// #include "common/glsl_utils.hpp"
// #include "common/specific.hpp"
// #include <memory>
// #include <cmath>
//
// using namespace glm;
// using std::vector, std::string, std::map, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;
//
// #define spec_uni make_shared<std::function<void(float, shared_ptr<Shader>)>>
// #define setter_uni shared_ptr<std::function<void(float, shared_ptr<Shader>)>>
// #define shd shared_ptr<Shader>
//
//
//
// float debugSin(float t)
// {
// 	return -cos(t)/2 + 0.5;
// }
//
// float w(float t)
// {
// 	return .007f + .00f*sin(1.5*t)*sin(1.5*t) + .01f/(1.5f + abs(.5*t));;
// }
//
//
//
// MaterialPhong mater(float t)
// {
// 	auto c = lerp(REDPINK_PALLETTE[6], REDPINK_PALLETTE[6], sin(1.5*t)*sin(1.5*t));
// 	return MaterialPhong(c, c, REDPINK_PALLETTE[1], .2031423, .756641656 , .131160145739731, 10.0);
// }
//
// float w2(float t)
// {
//     return .005f + .00f*sin(1.5*t)*sin(1.5*t) + .005f/(1.f + abs(.5*t));;
// }
//
//
//
// MaterialPhong mater2(float t)
// {
//     auto c = lerp(BLUE_PALLETTE[6], BLUE_PALLETTE[7], cos(1.5*t)*cos(1.5*t));
//     return MaterialPhong(c, c, BLUE_PALLETTE[1], .2031423, .756641656 , .131160145739731, 10.0);
// }
//
//
//
//
// int main(void)
// {
//     Renderer renderer = Renderer(.05f, vec4(.08f, .0809f, 0.1385f, 1.0f));
//     renderer.initMainWindow(UHD, "flows");
//
// 	shared_ptr<SmoothParametricCurve> camCurve = make_shared<SmoothParametricCurve>([](float t) { return vec3(cos(t*0)*3, sin(t*0)*3, 2); }, .01f);
//     shared_ptr<Camera> camera = make_shared<Camera>(camCurve, vec3(0.0, 0.0, 0), vec3(0, 0, 1), PI/4);
//     auto lights = vector({std::make_shared<PointLight>(vec3(-0.4, -1, 3), vec4(.95, .79, .861, 1), 15.0f),
//                           std::make_shared<PointLight>(vec3(0, 3, -0.1), vec4(.9498769, .69864, .694764, 1), 15.0f),
//                           std::make_shared<PointLight>(vec3(3, -3, 0.5), vec4(.98, .98, .938, 1), 20.0f)});
//
//
//     auto curva = [](float t) { return sphericalSpiral(.13-.08*sin(t*2), 1.0f, TAU*2, DEFAULT_POLY_GROUP_ID); };
//
//     // auto curva = [](float t) { return sphericalSpiral(.13-.08*sin(t*2), TAU*2, DEFAULT_POLY_GROUP_ID).precompose(SpaceAutomorphism::rotation(-t)); };
// 	SuperCurve circ = SuperCurve(curva(0), w, mater, 300, -TAU*2, TAU*2, false);
//
// 	circ.generateWeakMesh(13, TUBE);
//
//     // auto curva2 = [](float t) { return sphericalSpiral(.13-.08*cos(t*2), .8, TAU*2, PolyGroupID(2137)).precompose(SpaceAutomorphism::rotation(t*5+2)); };
//     auto curva2 = [](float t) { return sphericalSpiral(.13-.08*cos(t*2), .8, TAU*2, PolyGroupID(2137)); };
//
//     SuperCurve circ2 = SuperCurve(curva2(0), w2, mater2, 300, -TAU*2, TAU*2, false);
//     circ2.addToWeakMesh(13, TUBE, circ.getWeakMesh());
//
//     auto step = make_shared<RenderingStep>(make_shared<Shader>(
//             "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\hyperbolicAut.vert",
//             "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\hyperbolicAut.frag"));
//
// 	step->setWeakSuperMesh(circ.getWeakMesh());
//
//     float speed = 1.0f;
//
//     auto deformer = [&circ, &circ2, speed, &curva, &curva2](float t) {
//         auto m = curva(t*speed);
//         circ.updateCurve(m);
//         circ2.updateCurve(curva2(t*speed));
//     };
//
//     renderer.addRenderingStep(step);
//     renderer.setCamera(camera);
//     renderer.setLights(lights);
// 	renderer.addCustomAction(deformer);
//
//     return renderer.mainLoop();
// }
