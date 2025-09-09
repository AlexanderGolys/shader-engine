#include "common/glsl_utils.hpp"

using namespace glm;
using std::vector, std::string, std::map;


void initEvenInits()
{
	if (!glfwInit())
		exit(2137);

	if (glewInit() != GLEW_OK)
		exit(2138);
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




void setAttributes(map<string, GLuint> attributeBufferInfo, Model3D* model)
{
	glBindBuffer(GL_ARRAY_BUFFER, attributeBufferInfo["position"]);
	vector<vec3> vertex_buffer_data = model->mesh->posBuff;
	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_data.size() * sizeof(vec3), &vertex_buffer_data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, attributeBufferInfo["normal"]);
	vector<vec3> normal_buffer_data = model->mesh->normBuff;
	glBufferData(GL_ARRAY_BUFFER, normal_buffer_data.size() * sizeof(vec3), &normal_buffer_data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, attributeBufferInfo["color"]);
	vector<vec4> color_buffer_data = model->mesh->colorBuff;
	glBufferData(GL_ARRAY_BUFFER, color_buffer_data.size() * sizeof(vec4), &color_buffer_data[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, attributeBufferInfo["uv"]);
	vector<vec2> uv_buffer_data = model->mesh->uvBuff;
	glBufferData(GL_ARRAY_BUFFER, uv_buffer_data.size() * sizeof(vec2), &uv_buffer_data[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, attributeBufferInfo["position"]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, attributeBufferInfo["normal"]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);


	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, attributeBufferInfo["color"]);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);


	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, attributeBufferInfo["uv"]);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

}



void setPerObjectUniforms(Shader* shader, Model3D* model, vec3 camPosition, float screenRatio)
{
	mat4 meshTransform = model->transform;
	mat4 mvp = generateMVP(camPosition, vec3(.5, .5, 0), vec3(0, 0, 1), PI / 4.f, screenRatio, 0.1f, 100.0f, meshTransform);
	shader->setUniform("mvp", mvp);

	mat4 materialMat = model->material->compressToMatrix();
	shader->setUniform("material", materialMat);
}


void setPerFrameUniforms(Shader* shader, vec3 camPosition)
{
	shader->setUniform("camPosition", camPosition);
}

float initFrame(int width, int height, float speed)
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return glfwGetTime()*speed;
}




vector<Model3D> setupModelsAndMaterials()
{
	MaterialPhong material = MaterialPhong(vec4(.5, .4, .3, 1.), vec4(.6, .5, 0, 1.), vec4(1, 1, 1, 1), 1.0f, .0f, .00f, 60.0f);
	MaterialPhong bd_material = MaterialPhong(vec4(.5, .4, .3, 1.), vec4(.6, .5, 0, 1.), vec4(1, 1, 1, 1), .0f, .2f, .3f, 3.0f);

	auto ring = PlanarRing(100, 20, vec2(.5, .5), .5, .2);
	auto boundary_meshes = ring.kerbBoundaryEmbedding(.01, .01, .000, .2);
	ring.addRotationalField(1);
	auto mesh = ring.embeddInR3();
	mesh.precomputeBuffers();
	vector<Model3D> models = { Model3D(mesh, material, mat4(1)) };
	for (auto bd_mesh : boundary_meshes) {
		bd_mesh.precomputeBuffers();
		Model3D bd_model = Model3D(bd_mesh, bd_material, mat4(1));
		models.push_back(bd_model);
	}
	return models;
}

void setupLights(Shader* mainShader)
{
	PointLight light1 = PointLight(vec3(-2, 2, 3), vec4(1, 1, 1, 1), 40.0f);
	mat4 lightMat1 = light1.compressToMatrix();
	mainShader->setUniform("light1", lightMat1);

	PointLight light2 = PointLight(vec3(1, -5, 4), vec4(.5, 1, .5, 1), 50.0f);
	mat4 lightMat2 = light2.compressToMatrix();
	mainShader->setUniform("light2", lightMat2);

	PointLight light3 = PointLight(vec3(-1, 3, 1), vec4(1., .8, .8, 1), 50.0f);
	mat4 lightMat3 = light3.compressToMatrix();
	mainShader->setUniform("light3", lightMat3);
}



void setFlowUniforms(float speedCoef, float deltaTime, vec4 bgColor, Shader flowShader)
{
	flowShader.setUniform("bg_color", bgColor);
	flowShader.setUniform("flowPower", speedCoef * deltaTime);
}











int main(void)
{
	if (!glfwInit())
		exit(2137);

	Window window = Window(1920, 1080, "flows");

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
		exit(2138);

	glewExperimental = true; // Needed in core profile



	Shader mainShader = Shader("C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\flowsShader");
	mainShader.initUniforms({
		{"mvp", MAT4},
		{"material", MAT4},
		{"camPosition", VEC3},
		{"light1", MAT4},
		{"light2", MAT4},
		{"light3", MAT4},
		{"tex", SAMPLER2D} });

	Shader flowShader = Shader("C:\\Users\\PC\\Desktop\\ogl-master\\src\\shader-templates\\flowsTexShader");
	flowShader.initUniforms({
		{"bg_color", VEC4},
		{"flowPower", FLOAT},
		{"tex", SAMPLER2D} });

	GLuint vao = bindVAO();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	map<string, GLuint> attributeBufferInfo = initAttributeBuffers();
	auto models = setupModelsAndMaterials();

	float flowSpeedCoeff = 10.1f;
	float speed = 1.0f;
	
	Texture texture1 = Texture("C:\\Users\\PC\\Desktop\\ogl-master\\src\\textures\\texture1.bmp");
	Texture renderedTexture = Texture(texture1.width, texture1.height, 1);

	renderedTexture.bindToFrameBuffer();
	renderedTexture.addFilters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	texture1.addFilters(GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	texture1.bind();

	flowShader.use();
	flowShader.setTextureSampler(&texture1, 0);
	setFlowUniforms(flowSpeedCoeff, 1.0f, (vec4(0, 1, 0, 1)), flowShader);
	glDrawArrays(GL_TRIANGLES, 0, models.at(0).mesh->triangles.size() * 3);


	float lastFrameTime = glfwGetTime();

	while (window.isOpen()) {

		float time = initFrame(window.width, window.height, speed);

		float deltaTime = time - lastFrameTime;

		if (deltaTime > 1.f / 30) {

			lastFrameTime = time;
			setAttributes(attributeBufferInfo, &(models.at(0)));



			// ------------ rendering the flow field --------------------
			flowShader.use();
			flowShader.setTextureSampler(&renderedTexture, 0);
			setFlowUniforms(flowSpeedCoeff, deltaTime, (vec4(0, 1, 0, 1)), flowShader);
			renderedTexture.bind();
			renderedTexture.bindToFrameBuffer();
			// glDrawArrays(GL_TRIANGLES, 0, models.at(0).mesh.triangles.size() * 3);





			// ------------ rendering the mesh --------------------
			mainShader.use();
			renderedTexture.bind();
			window.bindToFramebuffer();

			setupLights(&mainShader);

			vec3 camPosition = vec3(1, 1.5, 1);
			setPerFrameUniforms(&mainShader, camPosition);
			setPerObjectUniforms(&mainShader, &models.at(0), camPosition, window.aspectRatio);
			mainShader.setTextureSampler(&renderedTexture, 0);


			// glDrawArrays(GL_TRIANGLES, 0, models.at(0).mesh.triangles.size() * 3);
			disableAttributeArrays(4);

			for (int i = 1; i < models.size(); i++) {
				Model3D* bd_model = &models.at(i);
				setPerObjectUniforms(&mainShader, &models.at(i), camPosition, window.aspectRatio);
				setAttributes(attributeBufferInfo, bd_model);
				// glDrawArrays(GL_TRIANGLES, 0, bd_model->mesh.triangles.size() * 3);
				disableAttributeArrays(4);
			}

			window.renderFramebufferToScreen();
		}
		
	}

	return window.destroy();
}
