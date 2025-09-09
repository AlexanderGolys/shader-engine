#include "common/buffer_utils.hpp"
#include "common/specific.hpp"
#include <memory>
#include <cmath>
#include <stdlib.h>


using namespace glm;
using std::vector, std::string, std::map, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;



float debugSin(float t)
{
	return -cos(t)/2 + 0.5;
}

float w(float t)
{
	return .007f + .00f*sin(1.5*t)*sin(1.5*t) + .01f/(1.5f + abs(.5*t));;
}




int main(void)
{
    shared_ptr<SmoothParametricCurve> camCurve = make_shared<SmoothParametricCurve>([](float t) { return vec3(cos(t*1)*sqrt(8), sin(t*1)*2, sin(t*1)*2); }, 0, TAU, .1, true, .01);
    Camera camera = Camera(camCurve, vec3(0.0, 0.0, 0), vec3(0, 0, 1), PI/4);
    auto lights = vector({PointLight(vec3(-0.4, -1, 3), vec4(.95, .79, .861, 1), 15.0f),
                          PointLight(vec3(0, 3, -0.1), vec4(.9498769, .69864, .694764, 1), 15.0f),
                          PointLight(vec3(3, -3, 0.5), vec4(.98, .98, .938, 1), 20.0f)});

    RENDER_SETTINGS settings = RENDER_SETTINGS(FHD, "vao", GL_SMOOTH, true, vec3(.08f, .0809f, 0.1385f), 20, TRADITIONAL_MEDIA_WITHOUT_ADBLOCK);
    VAORenderer renderer = VAORenderer(settings, &camera, lights);

    auto shader = Shader(   "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\vaoTest.vert",
                                "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\vaoTest.frag");




    auto m3 =  MaterialPhong(BLUE_PALLETTE[8], BLUE_PALLETTE[9], BLUE_PALLETTE[1], .2031423, .756641656 , .131160145739731, 10.0);
    // auto supeer = icosphere(1, 0, vec3(0, 0, 0), m3, PolyGroupID(222));
    WeakSuperMesh supeer = WeakSuperMesh({Vertex(vec3(0, 0, 0)), Vertex(vec3(1, 0, 0)), Vertex(vec3(0, 1, 0))},
                                         {ivec3(0, 1, 2)}, PolyGroupID(222), nullptr);

    auto step = VAORenderingObject(&shader, &supeer);
    renderer.addStep(step);
    return renderer.mainLoop();
}
