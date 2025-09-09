#include "common/glsl_utils.hpp"
#include <memory>

using namespace glm;
using std::vector, std::string, std::map, std::shared_ptr, std::unique_ptr;

int main(void)
{

    Renderer renderer = Renderer(.05f, vec4(.08f, .1209f, 0.1185f, 1.0f));
    renderer.initMainWindow(UHD, "flows");
    Shader geoShader1 = Shader("C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\randomTriangleExtrude.vert",
                     "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\randomTriangleExtrudeTess.geo",
                    "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\randomTriangleExtrude.frag");



	shared_ptr<SmoothParametricCurve> curve = std::make_shared<SmoothParametricCurve>([](float t) { return vec3(-5 * sin(t+1), -7 * cos(t+2.2), 3*cos(-4.21f * t) + 1.5); }, .01f);
    shared_ptr<Camera> camera = std::make_shared<Camera>(curve, vec3(0, 0, 0), vec3(0, 0, 1));
    auto lights = vector<shared_ptr<PointLight>>({std::make_shared<PointLight>(vec3(-5, -5, 6), vec4(.95, .79, .61, 1), 70.0f),
                                                 std::make_shared<PointLight>(vec3(6, -8, -4), vec4(.69, .864, .64, 1), 45.0f),
                                                 std::make_shared<PointLight>(vec3(0, 10, 0), vec4(.8, 1, .8, 1), 100.0f)});
    renderer.setCamera(camera);
    renderer.setLights(lights);
    TriangularMesh mesh1 = TriangularMesh("C:\\Users\\PC\\Desktop\\meshes\\ball0.obj");
    TriangularMesh mesh2 = TriangularMesh("C:\\Users\\PC\\Desktop\\meshes\\ball1.obj");
    TriangularMesh meshFloor = TriangularMesh("C:\\Users\\PC\\Desktop\\meshes\\floor4.obj");

    MaterialPhong materialleft = MaterialPhong(vec4(.9f, .5f, 0.35f, 1.0f), vec4(.98f, .8f, 0.15f, 1.0f), vec4(1.0f, 1.0f, 1.0f, 1.0f), .45, .62, 1.0, 30.0); // left, 
    MaterialPhong materialright = MaterialPhong(vec4(.99f, .7f, 0.15f, 1.0f), vec4(.99f, .8f, 0.25f, 1.0f), vec4(1.0f, 1.0f, .2f, 1.0f), .45, .8, .75, 90.0); // right, reddish
    MaterialPhong materialmid = MaterialPhong(vec4(.789f, .7899f, 0.25f, 1.0f), vec4(.89f, .79f, 0.15f, 1.0f), vec4(.974f, .75f, 0.195f, 1.0f), .31423, 1.1, .739731, 50.0); // mid 
    MaterialPhong materialFloor = MaterialPhong(vec4(.08f, .209f, 0.185f, 1.0f), vec4(.209f, .09f, 0.3185f, 1.0f), vec4(.08f, .408f, 0.42195f, 1.0f), .55, .95, .97, 10.0); // floor

	//mat3 rot1 = rot1 * rotate(mat4(1), radians(90.0f), vec3(1, 0, 0));
    mat3 rot1 = rotationMatrix3(1);
	mat3 rot2 = rotationMatrix3(2);

    Model3D middleBall = Model3D(mesh2*rot1 + vec3(0,  0, -0), materialmid, mat4(1));
	Model3D leftBall = Model3D(mesh1*rot2 + vec3(3,  0, -0),   materialleft, mat4(1));
	Model3D rightBall = Model3D(mesh1 + vec3(-3, 0, -0),  materialright, mat4(1));
    Model3D floorModel = Model3D(meshFloor + vec3(0, 0, 0), materialFloor, mat4(1));

    leftBall.mesh.randomizeFaceColors();
    rightBall.mesh.randomizeFaceColors();
	middleBall.mesh.randomizeFaceColors();
    floorModel.mesh.randomizeFaceColors();

    

    RenderingStep middleBallDeformer = RenderingStep(shared_ptr<Shader>(&geoShader1));
    middleBallDeformer.addConstFloats({{"speed", 65.0f}, {"power", .1011f}});

    RenderingStep leftBallDeformer = RenderingStep(shared_ptr<Shader>(&geoShader1));
    leftBallDeformer.addConstFloats({{"speed", 26.0f}, {"power", .423f}});

    RenderingStep rightBallDeformer = RenderingStep(shared_ptr<Shader>(&geoShader1));
    rightBallDeformer.addConstFloats({{"speed", 36.0f}, {"power", .14f}});

    RenderingStep floorDeformer = RenderingStep(shared_ptr<Shader>(&geoShader1));
    floorDeformer.addConstFloats({{"speed", 10.0f}, {"power", 0.55f}});



    middleBallDeformer.setModel(shared_ptr<Model3D>(&middleBall));
    leftBallDeformer.setModel(shared_ptr<Model3D>(&leftBall));
    rightBallDeformer.setModel(shared_ptr<Model3D>(&rightBall));
    floorDeformer.setModel(shared_ptr<Model3D>(&floorModel));


	renderer.addRenderingStep(shared_ptr<RenderingStep>(&middleBallDeformer));
	renderer.addRenderingStep(shared_ptr<RenderingStep>(&leftBallDeformer));
    renderer.addRenderingStep(shared_ptr<RenderingStep>(&rightBallDeformer));
    renderer.addRenderingStep(shared_ptr<RenderingStep>(&floorDeformer));

    renderer.addTimeUniform();
    //renderer.addConstFloats({{"speed", 30.0f}, {"power", .315f}});
    renderer.mainLoop();

}
