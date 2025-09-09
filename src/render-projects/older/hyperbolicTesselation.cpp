#include "common/glsl_utils.hpp"
#include "common/hyperbolic.hpp"
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
	// Mob M = Mob(-I*.7+1/2._f, I*1.1-2.14, I/0.8+.07, I*0.8f+1.8);
	Mob M = Imob;
	vec4 bgColor = vec4(.050f, .050009f, 0.1222f, 1.0f);
	Resolution res = UHD;

    Renderer renderer = Renderer(.05f, bgColor);
    renderer.initMainWindow(res, "flows");
    Shader geoShader1 = Shader("C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\hyperbolicTess.vert",
                    "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\hyperbolicTess.frag");



	shared_ptr<SmoothParametricCurve> curve = std::make_shared<SmoothParametricCurve>([](float t) { return vec3(-0.5, -1.0, 1.0); }, .01f);
    shared_ptr<Camera> camera = std::make_shared<Camera>(curve, vec3(0, 0, 0), vec3(0, 0, 1));
    auto lights = vector<shared_ptr<PointLight>>({std::make_shared<PointLight>(vec3(-1.3, 1, 4), vec4(.95, .9979, .9861, 1), 6.0f),
                                                 std::make_shared<PointLight>(vec3(1.3, .2, 4), vec4(.9969, .9864, .964, 1), 6.0f),
                                                 std::make_shared<PointLight>(vec3(0,-2.3, 2), vec4(.999, 1, .999, 1), 3.5f)});


    MaterialPhong materialmid = MaterialPhong(BLUE_PALLETTE[9], BLUE_PALLETTE[9], WHITE, .2031423, .4752856, .05285739731, 2.0); // mid
    MaterialPhong bdMat = MaterialPhong(BLUE_PALLETTE[4], BLUE_PALLETTE[4], WHITE, .3031423, .86656, .25739731, 20.0); // mid
	MaterialPhong bdMat2 = MaterialPhong(POWDER_PINK, POWDER_PINK, WHITE, .3031423, .2656, .25739731, 20.0); // mid
	MaterialPhong materialmid2 = MaterialPhong(POWDER_PINK, POWDER_PINK, WHITE, .2031423, .14752856, .05285739731, 2.0); // mid



	float cut_bd = .99;
	// HyperbolicTesselation tesselation = HyperbolicTesselation::ringsInH(20, 125, .3, .4, 1);
	// HyperbolicTesselation tesselation = HyperbolicTesselation::stripsInH(-.4, 0, -.4, .4, 1.1, 15);
	// HyperbolicTesselation tesselation = HyperbolicTesselation::stripsInD(-.4, 0, .2, .4, 1.1, cut_bd);
	HyperbolicTesselation tesselation = HyperbolicTesselation::modular(.06, .01, 3);

	tesselation.transformInPlace(Biholomorphism::mobius(CayleyTransform));
	// tesselation.transformInPlace(Biholomorphism::linear(.99, 0));

	// HyperbolicTesselation tesselation = HyperbolicTesselation::cyclicD(17, 55, 25, cut_bd);

	// tesselation.transformInPlace(Biholomorphism::mobius(Mob(-1, I/3, -I/3, -1)));

	// tesselation.transformInPlace(Biholomorphism::mobius(Mob(-1, I/3, -I/3, -1)));
	// tesselation.transformInPlace(Biholomorphism::linear(cut_bd, 0));
	// tesselation.transformInPlace(Biholomorphism::mobius(CayleyTransform.inv()));



	// tesselation.transformInPlace(Biholomorphism::mobius(Mob(1-I, I/13, -I/13, 1+I)));
	// tesselation.transformInPlace(Biholomorphism::linear(cut_bd, 0));
	// tesselation.transformInPlace(Biholomorphism::mobius(CayleyTransform.inv()));
	// tesselation.transformInPlace(Biholomorphism::power(1.7));

	// tesselation.transformInPlace(Biholomorphism::mobius(Mob(-1, I/3, -I/3, -1)));
	// tesselation.transformInPlace(Biholomorphism::linear(cut_bd, 0));
	// tesselation.transformInPlace(Biholomorphism::mobius(CayleyTransform.inv()));
	// tesselation.transformInPlace(Biholomorphism::power(.7));

	// tesselation.transformInPlace(Biholomorphism::mobius(Mob(-1, I/3, -I/3, -1)));
	// tesselation.transformInPlace(Biholomorphism::mobius(CayleyTransform.inv()));
	// tesselation.transformInPlace(Biholomorphism::power(.4));
	// tesselation.transformInPlace(Biholomorphism::mobius(CayleyTransform));


	// tesselation.transformInPlace(Biholomorphism::mobius(Mob(-1, I/3, -I/3, -1)));
	// tesselation.transformInPlace(Biholomorphism::linear(PI*.97, I*PI/2));
	// tesselation.transformInPlace(Biholomorphism::_EXP());
	// tesselation.transformInPlace(Biholomorphism::linear(I, 0));

	// tesselation.transformInPlace(Biholomorphism::mobius(Mob(-1, I/3, -I/3, -1)));
	tesselation.transformInPlace(Biholomorphism::linear(PI, I*PI));
	tesselation.transformInPlace(Biholomorphism::_LOG());
	tesselation.transformInPlace(Biholomorphism::linear(I, 1));

	// auto homotopy = mobiusHomotopy(Mob(1, I/2, -I/2, 1));
	// homotopy = precompose(homotopy, Biholomorphism::linear(I, 0));







	auto style = BoundaryEmbeddingStyle{KERB, .005f, .01f, .005f, .2, 0, 0, 0, 200};
	SuperPencilPlanar mesh = tesselation.domain()->superPencil({}, materialmid, bdMat, bdMat, 400, 100, 145.f, .9999, style, style);

	auto h = [](int i){return 0.003f + .00f*i;};
	auto pieces = tesselation.realiseTesselation(100, h);
	for (int i = 0; i < pieces.size(); i++)
	{
		MaterialFamily1P mat = MaterialFamily1P(BLUE_PALLETTE[2], BLUE_PALLETTE[8], .2531423, .975856, .0285739731, 2.0);
		// MaterialFamily1P mat = MaterialFamily1P(INTENSE_DARKER_RED_PALLETTE[0], INTENSE_DARKER_RED_PALLETTE[4], .2531423, .975856, .0285739731, 2.0);

		mesh.addPolyGroup(pieces.at(i), mat(1.f*i/pieces.size()));
	}
	mesh.precomputeBuffers();
	// mesh.makePencil(homotopy);


	auto step = make_shared<RenderingStep>(make_shared<Shader>(geoShader1));
	step->setSuperMesh(make_shared<SuperMesh>(mesh));

	renderer.addRenderingStep(step);

	// float speed = 5.5f;
	// auto deform = [&mesh, &speed](float t){mesh.transformMesh(debugSin(speed*t));};
	// renderer.addCustomAction(deform);

    renderer.setCamera(camera);
    renderer.setLights(lights);
    renderer.mainLoop();

}
