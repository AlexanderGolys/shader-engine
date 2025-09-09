#include "common/glsl_utils.hpp"
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

float width1(float t)
{
    return .03f + .00f*sin(4*t)*sin(4*t);
}

float width2(float t)
{
    return .03f + .00f*sin(4*t)*sin(4*t);
}

int main(void)
{
    Renderer renderer = Renderer(.19f, vec4(.06f, .0509f, 0.05585f, 1.0f));
    renderer.initMainWindow(UHD, "flows");
    float camSpeed = 1.f;

    PolyGroupID curveID = PolyGroupID(420);
    PolyGroupID curvatureId = PolyGroupID(69);

    shared_ptr<SmoothParametricCurve> curve = make_shared<SmoothParametricCurve>([](float t) { return vec3(.9*sin(t+1), cos(t+1)*(1 + .5*sin(2*t)), cos(4*t)/5.f); },

        curveID, 0, TAU, true, .01);
    SmoothParametricCurve curvatureCenter = SmoothParametricCurve([curve](float t) { return (*curve)(t) - curve->normal(t)*(min(2.f, curve->curvature_radius(t))); },
        curvatureId, curve->getT0(), curve->getT1(), curve->isPeriodic(), curve->getEps());

	shared_ptr<SmoothParametricCurve> camCurve = make_shared<SmoothParametricCurve>([&curve, &curvatureCenter](float t) { return vec3((*curve)(t).x*4.f, (*curve)(t).y*4.f, 2.1+curvatureCenter(t).z/5.f); }, 0, TAU, .1, true, .01);
    shared_ptr<SmoothParametricCurve> lookCamCurve = make_shared<SmoothParametricCurve>([&curve](float t) { return vec3(cos(t+.1), 1.5*sin(t+.1), sin(5*t)/5.f); }, 0, TAU, .1, true, .01);

    shared_ptr<Camera> camera = make_shared<Camera>(camCurve, curve, [curve](float t){return vec3(0, 0, 1);}, PI/4);
    auto lights = vector({std::make_shared<PointLightQuadric>(vec3(-0.4, -3, 2), vec4(.90952795, .785579, .94197285861, 1), 9.0f),
                          std::make_shared<PointLightQuadric>(vec3(2, 3, 2.1), vec4(.898769, .75369864, .903, 1), 9.0f),
                          std::make_shared<PointLightQuadric>(vec3(-4, -.1, -2), vec4(.9698, .9292598, .9399785938, 1), 9.0f)});

    auto midmat1 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\plastic1.bmp", 0, "texture_ambient");
    auto midmat2 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\texture_red.bmp", 1, "texture_diffuse");
    auto midmat3 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\texture_red4.bmp", 2, "texture_specular");

    auto icomat1 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\metal1.bmp", 0, "texture_ambient");
    auto icomat2 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\metal2.bmp", 1, "texture_diffuse");
    auto icomat3 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\metal3.bmp", 2, "texture_specular");

    auto cmat1 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\texture_red.bmp", 0, "texture_ambient");
    auto cmat2 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\texture_red4.bmp", 1, "texture_diffuse");
    auto cmat3 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\texture_red4.bmp", 2, "texture_specular");

    auto outmat1 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\metal1.bmp", 0, "texture_ambient");
    auto outmat2 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\metal3.bmp", 1, "texture_diffuse");
    auto outmat3 = make_shared<Texture>("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\metal3.bmp", 2, "texture_specular");

    auto cmat =  MaterialPhong(std::move(cmat1), std::move(cmat2),std::move(cmat3), .0182257031423, .466483956641656 , .2219131160145739731, 2.0);
    auto midmat =  MaterialPhong(std::move(midmat1), std::move(midmat2),std::move(midmat3), 0.031031423, .5962453956641656 , 1.21931160145739731, 20.0);

    auto icomat =  MaterialPhong(std::move(icomat1), std::move(icomat2),std::move(icomat3), 0.041031423, .873962453956641656 , 1.2931160145739731, 7.0);
    auto outmat =  MaterialPhong(std::move(outmat1), std::move(outmat2),std::move(outmat3), 0.0321031423, .63962453956641656 , .9931160145739731, 18.0);

    auto step = make_shared<RenderingStep>(make_shared<ShaderProgram>(
            "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\curvaCircle.vert",
            "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\curvaCircle.frag"));
    auto step2 = make_shared<RenderingStep>(make_shared<ShaderProgram>(
            "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\curvaCircle.vert",
            "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\curvaCircle.frag"));
    auto step3 = make_shared<RenderingStep>(make_shared<ShaderProgram>(
            "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\curvaCircle.vert",
            "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\curvaCircle.frag"));
    auto step4 = make_shared<RenderingStep>(make_shared<ShaderProgram>(
            "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\curvaCircle.vert",
            "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\curvaCircle.frag"));


    PolyGroupID sphereID = PolyGroupID(421);


    auto oscCircMid = make_shared<Disk3D>("C:\\Users\\PC\\Desktop\\meshes\\wheel5.obj", vec3(0), vec3(0, 1, 0) ,vec3(0, 0, -1), sphereID);
    auto oscCircBand = make_shared<Disk3D>("C:\\Users\\PC\\Desktop\\meshes\\wheel4band.obj", vec3(0), vec3(0, 1, 0) ,vec3(0, 0, -1), sphereID);
    auto ico = make_shared<WeakSuperMesh>(icosphere(.04, 3, (*curve)(0), sphereID));
    auto c1 = make_shared<WeakSuperMesh>(SmoothParametricSurface(*curve, width1), 200, 25, curveID);

    c1->addGlobalMaterial(cmat);
    ico->addGlobalMaterial(midmat);
    oscCircMid->addGlobalMaterial(midmat);
    oscCircBand->addGlobalMaterial(outmat);
    //
    // oscCircMid->scaleR(oscCircBand->getR()*2, false);
    // oscCircMid->scaleR(oscCircBand->getR()/2, true);

    float rRatio = oscCircMid->getR()/oscCircBand->getR();
    rRatio = 1.f;
    // supeer->addUniformSurface(curva1(0), 150, 10, curveID);

    step->setWeakSuperMesh(c1);
    step2->setWeakSuperMesh(oscCircBand);
    step3->setWeakSuperMesh(oscCircMid);
    step4->setWeakSuperMesh(ico);


    // step->addUniform("intencities", MAT4, spec_uni([camera](float t, const shared_ptr<Shader> &shader) {shader->setUniform("mvp", camera->mvp(t));}));
    // renderer.addRenderingStep(step2);
    renderer.addRenderingStep(step);
    renderer.addRenderingStep(step3);
    renderer.addRenderingStep(step4);


    renderer.setCamera(camera);
    renderer.setLights(lights);
    // renderer.addConstUniform("intencities", VEC4

    std::function<void(float, float)> def = [&oscCircBand, &oscCircMid, &curvatureCenter, &curve, rRatio, &ico, sphereID](float t, float dt){
        float angle = oscCircBand->moveRotate(curvatureCenter(t), curve->tangent(t), curve->normal(t));
        oscCircBand->scaleR(min(curve->curvature_radius(t), 2.f), false);

        oscCircMid->move(curvatureCenter(t), curve->tangent(t), curve->normal(t), false);
        oscCircMid->scaleR(min(curve->curvature_radius(t)*rRatio, 2.f*rRatio), false);
        oscCircMid->rotate(angle);

        ico->shift((*curve)(t) - (*curve)(t-dt), sphereID);
        // v.applyFunction(SpaceAutomorphism::deltaRotation(curve->normal(t-dt), curve->normal(t), curvatureCenter(t)));
        // v.applyFunction(SpaceAutomorphism::scaling(curve->curvature_radius(t)/curve->curvature_radius(t-dt), curvatureCenter(t)));
    };

	renderer.addCustomAction(def);

    return renderer.mainLoop();
}
