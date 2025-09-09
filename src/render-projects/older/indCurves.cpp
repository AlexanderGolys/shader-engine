#include "common/glsl_utils.hpp"
#include "common/specific.hpp"
#include <memory>
#include <cmath>
#include <stdlib.h>


using namespace glm;
using std::vector, std::string,  std::shared_ptr, std::unique_ptr, std::pair, std::make_unique, std::make_shared;

#define spec_uni make_shared<std::function<void(float, shared_ptr<Shader>)>>
#define setter_uni shared_ptr<std::function<void(float, shared_ptr<Shader>)>>
#define shd shared_ptr<Shader>



float debugSin(float t)
{
	return -cos(t)/2 + 0.5;
}





float width1(float t)
{
    return .01f + .01f*sin(4*t)*sin(4*t);
}






int main(void)
{
    Renderer renderer = Renderer(.05f, vec4(.07f, .0409f, 0.05585f, 1.0f));
    renderer.initMainWindow(FHD, "flows");
    float camSpeed = 1.5/4;
	shared_ptr<SmoothParametricCurve> camCurve = make_shared<SmoothParametricCurve>([camSpeed](float t) { return vec3(sin(t*camSpeed)*sqrt(8), -cos(t*camSpeed)*2, -cos(t*camSpeed)*2); }, 0, TAU, .1, true, .01);
    shared_ptr<Camera> camera = make_shared<Camera>(camCurve, vec3(0.0, 0.0, 0), vec3(0, 0, 1), PI/4);
    auto lights = vector({std::make_shared<PointLightQuadric>(vec3(-0.4, -3, 2), vec4(.0952795, .785579, .4197285861, 1), 15.0f),
                          std::make_shared<PointLightQuadric>(vec3(0, -2, 2.1), vec4(.898769, .75369864, .03, 1), 15.0f),
                          std::make_shared<PointLightQuadric>(vec3(2, -1, -2), vec4(.698, .292598, .39785938, 1), 15.0f)});

    auto tex1 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\texture1.bmp", 0, "texture_ambient", true);
    auto tex2 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\texture_red.bmp", 1, "texture_diffuse");
    auto tex3 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\texture1.bmp", 2, "texture_specular", true);

    auto curvatex1 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\texture_red.bmp", 0, "texture_ambient");
    auto curvatex2 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\texture1.bmp", 1, "texture_diffuse" , true);
    auto curvatex3 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\texture1.bmp", 2, "texture_specular", true);

    auto matcurva =  MaterialPhong(std::move(curvatex1), std::move(curvatex2),std::move(curvatex3), .082257031423, .666483956641656 , .5219131160145739731, 90.0);
    auto matsp =  MaterialPhong(std::move(tex1), std::move(tex2),std::move(tex3), 0.051031423, .3962453956641656 , .0931160145739731, 60.0);


    auto step = make_shared<RenderingStep>(make_shared<ShaderProgram>(
            "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\curvaCircle.vert",
            "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\curvaCircle.frag"));
    auto step2 = make_shared<RenderingStep>(make_shared<ShaderProgram>(
            "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\curvaCircle.vert",
            "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\curvaCircle.frag"));


    shared_ptr<WeakSuperMesh> sph = make_shared<WeakSuperMesh>(icosphere(.995, 4, vec3(0, 0, 0), PolyGroupID(222)));

    PolyGroupID curveID = PolyGroupID(2139);
    auto curva1 = [&curveID](float t) {
        return SmoothParametricSurface(sphericalSpiral(.1 + .07 * sin(t), 1.0f, TAU * 4, curveID, .01), width1);
    };

    shared_ptr<WeakSuperMesh> cuuurva = make_shared<WeakSuperMesh>(curva1(0), 650, 17, curveID);

    cuuurva->addGlobalMaterial(matcurva);
    sph->addGlobalMaterial(matsp);

    // supeer->addUniformSurface(curva1(0), 150, 10, curveID);

    step->setWeakSuperMesh(cuuurva);
    step2->setWeakSuperMesh(sph);
    // step->addUniform("intencities", MAT4, spec_uni([camera](float t, const shared_ptr<Shader> &shader) {shader->setUniform("mvp", camera->mvp(t));}));
    renderer.addRenderingStep(step);
    renderer.addRenderingStep(step2);


    renderer.setCamera(camera);
    renderer.setLights(lights);
    // renderer.addConstUniform("intencities", VEC4
    float defSpeed = .75;
    auto surfaceDeformer = [defSpeed](float t, auto &curva) {
        return [t, &curva, defSpeed](BufferedVertex &v) {v.setPosition(curva(defSpeed*t-.5*sin(3*t*defSpeed)-.2*sin(5*t*defSpeed)).parametersNormalised(v.getUV()));};
    };


    auto deformer = [&surfaceDeformer, &cuuurva, &curveID, &curva1](float t) {
        cuuurva->deformPerVertex(curveID, surfaceDeformer(t, curva1));
    };

	renderer.addCustomAction(deformer);
    return renderer.mainLoop();
}
