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

void handleOneHyperbolic(HyperbolicPlane &plane, Renderer &r, MaterialPhong &material, MaterialPhong &bd_material, MaterialPhong &geo_material, vec3 position, vector<pair<Complex, Complex>> geodesics, 
                            Shader &prog, float speed, float power, int radial, int circular, vec4 col)
{
    RenderingStep step = RenderingStep(std::make_shared<Shader>(prog));
	step.addConstFloats({ {"speed", speed}, {"power", power}});

    for (auto endpoints : geodesics) {
	    shared_ptr<ComplexCurve> geodesic = make_shared<ComplexCurve> (plane.geodesic(endpoints.first, endpoints.second));
    	float speed_geo = 8.f + pseudorandomizer((*geodesic)(0.420f).z.x)/.4f;
    	float phase = pseudorandomizer((*geodesic)(21.37f).z.x);
		float size_mult = 1 + pseudorandomizer((*geodesic)(0.2137f).z.y)/4.f;

    	step.addConstFloats({ {"gobluego", 1} } );
    	step.addUniforms({ {"radiusBulb", FLOAT}, {"extrudeNearHotspotSize", FLOAT}, {"secondaryColor", VEC4} },


    		{{"radiusBulb", spec_uni([phase](float t, shd shader){shader->setUniform("radiusBulb", .13f + .05f*abs(std::sin(23.f*t + phase)) + .04f*std::cos(31.1f*t + phase ));})},
    			{"secondaryColor", spec_uni([phase, col](float t, shd shader){shader->setUniform("secondaryColor", col*(.8f + .45f*abs(std::sin(43.f*t + phase))));})},
				{"extrudeNearHotspotSize", spec_uni([phase](float t, shd shader){shader->setUniform("extrudeNearHotspotSize", 0.005f + .01f*std::cos(42.2f*t + phase));})}});

        auto geodesicMesh = TriangularMesh(geodesic.get(), 60, .019f*size_mult, .012f*size_mult, .003f);
        geodesicMesh += position;
		geodesicMesh.randomizeFaceColors(vec3(.1, .02, 0), vec3(.2, .1, 0));
        Model3D geodesicModel = Model3D(geodesicMesh, geo_material, mat4(1));
        step.setModel(std::make_shared<Model3D>(geodesicModel));

		step.addUniform("currentPt", VEC2, make_shared<std::function<void(float, shared_ptr<Shader>)>>(
            [geodesic, speed_geo, phase](float t, shared_ptr<Shader> shader) {
				shader->setUniform("currentPt", vec2((*geodesic)(((std::sin(t * speed_geo + phase)/2. + .5f) * (geodesic->t1 - geodesic->t0) + geodesic->t0))));
		}));
    	step.addUniform("currentPtEmbedded", VEC3, make_shared<std::function<void(float, shared_ptr<Shader>)>>(
				[geodesic, speed_geo, phase, position](float t, shared_ptr<Shader> shader) {
				shader->setUniform("currentPtEmbedded", vec3(vec2((*geodesic)(((std::sin(t * speed_geo + phase)/2. + .5f) * (geodesic->t1 - geodesic->t0) + geodesic->t0))), 0) + position);
		}));


        r.addRenderingStep(std::make_shared<RenderingStep>(step));
    }

	step.addConstFloats({ {"speed", speed}, {"power", power}, {"gobluego", 0}, {"radiusBulb", 0} , {"extrudeNearHotspotSize", .0} });
    auto mesh = plane.mesh(radial, circular);
    auto mesh3d = mesh.embeddInR3(0) + position;
    mesh3d.randomizeFaceColors();
    Model3D model = Model3D(mesh3d, material, mat4(1));
    step.setModel(std::make_shared<Model3D>(model));
    step.addConstFloats({ {"gobluego", 0} });
    r.addRenderingStep(std::make_shared<RenderingStep>(step));

	auto bd_curve = mesh.kerbBoundaryEmbedding(.03f, .04f, .025f, .2);
	for (auto bd_mesh : bd_curve)
	{
        bd_mesh += position;
		bd_mesh.precomputeBuffers();
		Model3D bd_model = Model3D(bd_mesh, bd_material, mat4(1));
		step.setModel(std::make_shared<Model3D>(bd_model));
		r.addRenderingStep(std::make_shared<RenderingStep>(step));
	}
}

















int main(void)
{
    Renderer renderer = Renderer(.05f, vec4(.08f, .0709f, 0.0585f, 1.0f));
    renderer.initMainWindow(UHD, "flows");
    Shader geoShader1 = Shader("C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\hyperbolicRandom.vert",
                    "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\hyperbolicRandom.frag");



	shared_ptr<SmoothCurve> curve = std::make_shared<SmoothCurve>([](float t) { return vec3(2.4 * sin(4*t+1), 2.4 * cos(4*t+1), -1*sin(7.21f * t) + 1.6); }, .01f);
    shared_ptr<Camera> camera = std::make_shared<Camera>(curve, vec3(-1.15, -1.15, 0), vec3(0, 0, 1));
    auto lights = vector<shared_ptr<PointLight>>({std::make_shared<PointLight>(vec3(-1, -4, 6), vec4(.95, .79, .861, 1), 11.0f),
                                                 std::make_shared<PointLight>(vec3(-4, -1, 4), vec4(.69, .864, .964, 1), 11.0f),
                                                 std::make_shared<PointLight>(vec3(1, 2, 7), vec4(.9, 1, .9, 1), 20.0f)});


    HyperbolicPlane disk = PoincareDisk();
    HyperbolicPlane plane = HyperbolicPlane();
    HyperbolicPlane disk4 = PoincareDisk();
    HyperbolicPlane disk2 = PoincareDisk();
    HyperbolicPlane disk3 = PoincareDisk();

    MaterialPhong reeed = MaterialPhong(PALETTE2[0], PALETTE2[0], PALETTE3[3], .01845, .489762, 0.9, 19); // left, 
    MaterialPhong gree = MaterialPhong(PALETTE2[1], PALETTE2[1], WHITE, .01045, .8163862, .4, 1.0); // left, 
    MaterialPhong oczojebnyroz = MaterialPhong(PALETTE2[2], PALETTE2[2], WHITE, .1245, .78, .9, 40.0); // right, reddish
    MaterialPhong materialmid = MaterialPhong(PALETTE3[1], PALETTE3[1], WHITE, .1031423, .86, .739731, 50.0); // mid 
    MaterialPhong black = MaterialPhong(PALETTE2[4], PALETTE2[4], WHITE, .1245, .98, .975, 10.0); // right, reddish
	MaterialPhong yelue = MaterialPhong(PALETTE3[3], PALETTE3[3], PALETTE3[1], .2155, 1.295, .97, 50.0); // floor
    MaterialPhong yelue2 = MaterialPhong(PALETTE3[3], PALETTE3[3], PALETTE2[2], .01155, .72795, .97, 50.0); // floor

    vector<Complex> starts1 = allRootsOf1(17);
    Complex z0 = rootOfUnity(11, 4);
    Complex z1 = rootOfUnity(13, 3);
    Complex z2 = rootOfUnity(10, 1);
    vector<pair<Complex, Complex>> endpoints1;
    for (Complex z : starts1)
    {
		endpoints1.emplace_back(z*.999, z0*z*.999);
		endpoints1.emplace_back(z*z2*.999, z1*z*.999);
		}


    handleOneHyperbolic(disk, renderer, oczojebnyroz, black, oczojebnyroz, vec3(1.15, 1.15, 0),
                        { {Complex(.9, 0), Complex(-.5, -.5)}, {Complex(.7, .3), Complex(-.6, -.5)}, {Complex(.5, .7), Complex(-.7, -.5)}, { Complex(.1, .95), Complex(-.85, -.4) },  },
                        geoShader1, 5.0f, .008011f, 40, 13, vec4(.2, .1, .0, 1.));

   // handleOneHyperbolic(plane, renderer, materialmid, black, yelue, vec3(0, 1.3, 0),
   //                     { {Complex(0, 0.01), Complex(1, .01)}, {Complex(0.5, 0.01), Complex(-3, .01)} }, geoShader1, 6.10f, .202, 30, 15, vec4(.8, .6, .6, 1.));

    handleOneHyperbolic(disk4, renderer, gree, black, gree, vec3(-1.15, 1.15, -0),
                        {{Complex(-.83, .55), Complex(.83, .55) }, {Complex(.999, 0), Complex(-.999, 0)}, {Complex(.83, -.55), Complex(-.83, -.55) }}, geoShader1, 16.0f, .014f, 50, 15, vec4(.5, .85, .5, 1.));

    handleOneHyperbolic(disk2, renderer, reeed, black, reeed, vec3(-1.15, -1.15,0),
                        endpoints1, geoShader1, 0.0f, .0f, 60, 25, vec4(.74, .1, .0, 1.));

    handleOneHyperbolic(disk3, renderer, materialmid, black, materialmid, vec3(1.15, -1.15, 0),
                        { {Complex(.999, 0), Complex(-.77, .63)}, {Complex(.97, 0.2), Complex(.72, -.68)} }, geoShader1, 36.0f, .014f, 22, 7, vec4(.8, .9, .1, 1.));
    
    


    renderer.setCamera(camera);
    renderer.setLights(lights);


    renderer.addTimeUniform();
    renderer.mainLoop();

}
