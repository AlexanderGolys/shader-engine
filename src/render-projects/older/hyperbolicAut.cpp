#include "common/glsl_utils.hpp"
#include "common/hyperbolic.hpp"
#include "common/specific.hpp"
#include <memory>
#include <cmath>

using namespace glm;
using std::vector, std::string, std::map, std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;

#define spec_uni make_shared<std::function<void(float, shared_ptr<Shader>)>>
#define setter_uni shared_ptr<std::function<void(float, shared_ptr<Shader>)>>
#define shd shared_ptr<Shader>

std::function<Biholomorphism(float)> mobiusHomotopy(Matrix<Complex, 2> M)
{
	auto Id = Matrix<Complex, 2>(ONE, ZERO, ZERO, ONE);
	return [M, Id](float t) { return Biholomorphism::mobius(Id*(1-t) + M*t); };
}

float debugSin(float t)
{
	return -cos(t)/2 + 0.5;
}


int main(void)
{
	auto M = Matrix<Complex, 2>(-I*.7+1/2.f, I*1.1-2.14, I/0.8+.07, I*0.8f+1.8);

    Renderer renderer = Renderer(.05f, vec4(.05f, .0209f, 0.0285f, 1.0f));
    renderer.initMainWindow(FHD, "flows");
    Shader geoShader1 = Shader("C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\hyperbolicAut.vert",
                    "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\hyperbolicAut.frag");



	shared_ptr<SmoothParametricCurve> curve = std::make_shared<SmoothParametricCurve>([](float t) { return vec3(-1.5, -1.55, 1.6); }, .01f);
    shared_ptr<Camera> camera = std::make_shared<Camera>(curve, vec3(0.45, 0.45, 0), vec3(0, 0, 1));
    auto lights = vector<shared_ptr<PointLight>>({std::make_shared<PointLight>(vec3(-1, 4, 6), vec4(.95, .79, .861, 1), 11.0f),
                                                 std::make_shared<PointLight>(vec3(4, -1, 4), vec4(.69, .864, .964, 1), 11.0f),
                                                 std::make_shared<PointLight>(vec3(1, 2, 7), vec4(.9, 1, .9, 1), 20.0f)});


    MaterialPhong materialmid = MaterialPhong(PALETTE3[1], PALETTE3[1], WHITE, .1031423, .856, .5739731, 2.0); // mid
    MaterialPhong black = MaterialPhong(PALETTE2[3], PALETTE2[3], WHITE, .1545, .38, .275, 100.0); // right, reddish
	MaterialPhong yelue = MaterialPhong(PALETTE2[4], PALETTE2[4], WHITE, .22155, .95, .997, 5.0); // floor
	MaterialPhong yelue2 = MaterialPhong(PALETTE2[4]*1.05f, PALETTE2[4], WHITE, .29155, .95, .997, 30.0);

	vector<pair<Complex, Complex>> endpoints = {{I+.01, I*2+.02}, {I*.02+1, I*.02+3},
												{I*1.5f+.02, I*2.5f+.02}, {I*.02+1.5, I*.02+3.5},
												{I*2.f+.02, I*3.f+.02}, {I*.02+2.f, I*.02+4.f},
	{I+.01, I*.02+1}, {I*3.f+.025, I*.02+4.f}, {I/2.f+.01, I*.02+5.5f}};
	HyperbolicPlane plane = HyperbolicPlane();
	plane = plane.transform(Biholomorphism::power(2));
	auto style = BoundaryEmbeddingStyle(CURVE,0.f, 0.f, 0.f, 0.f, 0.006, 0.002, 0.002, 80);
	SuperPencilPlanar mesh = plane.superPencil(endpoints, materialmid, black, black, 100, 20, 20, .99, STD_KERB, style);
	mesh.precomputeBuffers();
	mesh.makePencil(mobiusHomotopy(M));
	mesh.randomiseMaterialsDynamically(.01);



	auto step = make_shared<RenderingStep>(make_shared<Shader>(geoShader1));
	step->setSuperMesh(make_shared<SuperMesh>(mesh));

	float speed = 5.0f;
	auto deform = [&mesh, &speed](float t){mesh.transformMesh(debugSin(speed*t));};
	renderer.addCustomAction(deform);
	renderer.addRenderingStep(step);

    renderer.setCamera(camera);
    renderer.setLights(lights);
    renderer.mainLoop();

}
