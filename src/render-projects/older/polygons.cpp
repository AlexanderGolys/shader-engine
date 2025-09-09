#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "common/glsl_utils.hpp"

#include <vector>
#include <map>


using namespace glm;
using std::vector, std::string;

GLFWwindow* window;


void initEvenInits()
{
	if (!glfwInit())
		exit(2137);


}


map<string, GLuint> initAttributeBuffers() {

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	GLuint uvBuffer;
	glGenBuffers(1, &uvBuffer);

	map<string, GLuint> buffers = { {"position", vertexbuffer}, {"normal", normalbuffer}, {"color", colorbuffer}, {"uv", uvBuffer} };
	return buffers;
}




void setAttributes(map<string, GLuint> attributeBufferInfo, Model3D model)
{
	glBindBuffer(GL_ARRAY_BUFFER, attributeBufferInfo["position"]);
	vector<vec3> vertex_buffer_data = model.mesh->posBuff;
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(vec3), &vertex_buffer_data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, attributeBufferInfo["normal"]);
	vector<vec3> normal_buffer_data = model.mesh->normBuff;
	glBufferData(GL_ARRAY_BUFFER, normal_buffer_data.size() * sizeof(vec3), &normal_buffer_data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, attributeBufferInfo["color"]);
	vector<vec4> color_buffer_data = model.mesh->colorBuff;
	glBufferData(GL_ARRAY_BUFFER, color_buffer_data.size() * sizeof(vec4), &color_buffer_data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, attributeBufferInfo["uv"]);
	vector<vec2> uv_buffer_data = model.mesh->uvBuff;
	glBufferData(GL_ARRAY_BUFFER, uv_buffer_data.size() * sizeof(vec2), &uv_buffer_data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, attributeBufferInfo["position"]);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, attributeBufferInfo["normal"]);
	glVertexAttribPointer(
		1,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, attributeBufferInfo["color"]);
	glVertexAttribPointer(
		2,
		4,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);

	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, attributeBufferInfo["uv"]);
	glVertexAttribPointer(
		3,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);
}


void setPerObjectUniforms(GLuint programID, Model3D model, vec3 camPosition, float screenRatio)
{
	mat4 meshTransform = model.transform;
	mat4 mvp = generateMVP(camPosition, vec3(0, 0, 0), vec3(0, 0, 1), PI / 4.f, screenRatio, 0.1f, 100.0f, meshTransform);
	GLuint MatrixID = glGetUniformLocation(programID, "mvp");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

	mat4 materialMat = model.material->compressToMatrix();
	GLuint materialID = glGetUniformLocation(programID, "material");
	glUniformMatrix4fv(materialID, 1, GL_FALSE, &materialMat[0][0]);

}


void setPerFrameUniforms(GLuint programID, vec3 camPosition)
{
	GLuint camPosUni = glGetUniformLocation(programID, "camPosition");
	glUniform3f(camPosUni, camPosition.x, camPosition.y, camPosition.z);
}


float initFrame(GLuint programID, Window* win, float speed)
{

	glViewport(0, 0, win->width, win->height);
	glClearColor(0.0f, 0.025f, 0.05f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return glfwGetTime() * speed;
}


vector<Model3D> setupModelsAndMaterials()
{
	MaterialPhong material2 = MaterialPhong(vec4(.5, .4, .3, 1.), vec4(.6, .5, 0, 1.), vec4(1, 1, 1, 1), .03f, .4f, .7f, 60.0f);
	MaterialPhong material1 = MaterialPhong(vec4(.9, .5, .3, 1.), vec4(.9, .7, 0, 1.), vec4(1, 1, 1, 1), .03f, .5f, .7f, 50.0f);
	MaterialPhong material3 = MaterialPhong(vec4(.75, .65, .65, 1), vec4(.85, .55, .65, 1), vec4(1, .2, 1, 1), .1f, .5f, .3f, 10.0f);


	vector<TriangularMesh> meshes = {	BoxMesh(vec3(-1, -1, -1), vec3(1, 1, 1), vec4(.7, .3, .1, 1)) + vec3(-5, 0, 0),
										PlanarConvexPolygon({vec2(0, 0), vec2(1, 0), vec2(1, 1), vec2(0, 2), vec2(-2, 1)}).embeddInR3(0) ,
										TriangularMesh("C:\\Users\\PC\\Desktop\\meshes\\surface17.obj") + vec3(5, 0, 0) };

	meshes[2].flipNormals();
	for (int i = 0; i < meshes.size(); i++)
		meshes[i].precomputeBuffers();

	vector<Model3D> models = { Model3D(meshes[0], material1, mat4(1)),
								Model3D(meshes[1], material2, mat4(1)),
								Model3D(meshes[2], material3, mat4(1)), };
	return models;
}


void setupLights(GLuint programID)
{
	PointLight light1 = PointLight(vec3(-2, 5, 5), vec4(1, 1, 1, 1), 140.0f);
	mat4 lightMat1 = light1.compressToMatrix();
	GLuint light1ID = glGetUniformLocation(programID, "light1");

	PointLight light2 = PointLight(vec3(-7, -5, 9), vec4(.5, 1, .5, 1), 150.0f);
	mat4 lightMat2 = light2.compressToMatrix();
	GLuint light2ID = glGetUniformLocation(programID, "light2");

	PointLight light3 = PointLight(vec3(10, 5, 2), vec4(1., .5, .5, 1), 150.0f);
	mat4 lightMat3 = light3.compressToMatrix();
	GLuint light3ID = glGetUniformLocation(programID, "light3");

	glUniformMatrix4fv(light1ID, 1, GL_FALSE, &lightMat1[0][0]);
	glUniformMatrix4fv(light2ID, 1, GL_FALSE, &lightMat2[0][0]);
	glUniformMatrix4fv(light3ID, 1, GL_FALSE, &lightMat3[0][0]);
}










int main(void)
{
	initEvenInits();

	Window window = Window(FHD, "polys");

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
		exit(2138);

	GLuint programID = LoadShaders("C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\polygons.vert",
								   "C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\polygons.frag");
	glUseProgram(programID);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


	vector<Model3D> models = setupModelsAndMaterials();
	GLuint vao = bindVAO();

	setupLights(programID);
	map <string, GLuint> attributeBufferInfo = initAttributeBuffers();

	while (window.isOpen()) {
		float speed = .1;
		float time = initFrame(programID, &window, speed);

		vec3 camPosition = vec3(15 * sin(time), 15 * cos(time), 3);
		setPerFrameUniforms(programID, camPosition);

		for (Model3D& model : models) {
			setAttributes(attributeBufferInfo, model);
			setPerObjectUniforms(programID, model, camPosition, window.aspectRatio);

			glDrawArrays(GL_TRIANGLES, 0, model.mesh->triangles.size() * 3);
			disableAttributeArrays(4);
		}

		window.renderFramebufferToScreen();
	}

	return window.destroy();
}
