#include "SDFRendering.hpp"

#include "configFiles.hpp"

using std::shared_ptr, std::make_shared, std::vector, std::string;


CodeMacro sdfToolsMacro() {
	ConfigFile config = ConfigFile();
	return CodeMacro("__SDFTOOLS__", CodeFileDescriptor(config.getSDFMacroShader()).getCode());
}

CodeMacro mathToolsMacro() {
	ConfigFile config = ConfigFile();
	return CodeMacro("__MATHTOOLS__", CodeFileDescriptor(config.getMathToolsShaderMacro()).getCode());
}

CodeMacro structsMacro() {
	ConfigFile config = ConfigFile();
	return CodeMacro("__STRUCTS__", CodeFileDescriptor(config.getStructsShaderMacro()).getCode());
}

CodeMacro lightsMacro() {
	ConfigFile config = ConfigFile();
	return CodeMacro("__LIGHTTOOLS__", CodeFileDescriptor(config.getLightToolsShaderMacro()).getCode());
}




TemplateCodeFile vanillaSDFTemplate(const CodeFileDescriptor &source, const CodeFileDescriptor &target) {
	vector macros = {sdfToolsMacro(), mathToolsMacro(), structsMacro(), lightsMacro()};
	return TemplateCodeFile(source, target, macros);
}

SDFObjectInstance sphereSDF(const SDFMaterialPlus &material, float radius, vec3 center, const string &name) {
	return SDFObjectInstance(sphereSDF(radius, name), center, mat3(1), material, {vec3(radius, 0, 0)});
}

SDFObjectInstance planeSDF(const SDFMaterialPlus &material, vec3 normal, float d, const string &name) {

	mat3 m = GramSchmidtProcess(mat3(e1, e2, normal));
	return SDFObjectInstance(planeSDF(name), vec3(0, 0, -d), m, material, vector<vec3>());
}

SDFObjectInstance torusSDF(const SDFMaterialPlus &material, float r, float R, vec3 center, mat3 rotation, string name) {
	return SDFObjectInstance(torusSDF(r, R, name), center, rotation, material, {vec3(r, R, 0)});
}

SDFObjectInstance boxSDF(const SDFMaterialPlus &material, vec3 size, vec3 center, mat3 rotation, string name) {
	return SDFObjectInstance(boxSDF(size, name), center, rotation, material, {size});
}

SDFObjectInstance roundBoxSDF(const SDFMaterialPlus &material, vec3 size, float r, vec3 center, mat3 rotation, string name) {
	return SDFObjectInstance(roundBoxSDF(size, r, name), center, rotation, material, vector<vec3> {size, vec3(r, 0, 0)});
}

SDFMaterial::SDFMaterial(vec4 ambientColor, vec4 diffuseColor, float reflectivity, float transparency, float indexOfRefraction, float translucency)
		: ambientColor(ambientColor), diffuseColor(diffuseColor), reflectivity(reflectivity), transparency(transparency), indexOfRefraction(indexOfRefraction), translucency(translucency) {
}

vector<vec4> SDFMaterial::compress() const {
	return {ambientColor, diffuseColor, vec4(reflectivity, transparency, indexOfRefraction, translucency)};
}



vec4 SDFMaterial::operator[](int i) const {
	switch (i) {
		case 0:
			return ambientColor;
		case 1:
			return diffuseColor;
		case 2:
			return vec4(reflectivity, transparency, indexOfRefraction, translucency);
		default:
			throw IndexOutOfBounds(i, 3, "SDFMaterial", __FILE__, __LINE__);
	}
}

float SDFMaterial::getIndexOfRefraction() const {
	return indexOfRefraction;
}

float SDFMaterial::getAlpha() const {
	return transparency;
}

float SDFMaterial::getReflectivity() const {
	return reflectivity;
}

vec4 SDFMaterial::getDiffuseColor() const {
	return diffuseColor;
}

vec4 SDFMaterial::getAmbientColor() const {
	return ambientColor;
}

int SDFMaterial::size() const {
	return 3;
}


SDFMaterialPlus::SDFMaterialPlus(vec4 ambientColor, vec4 diffuseColor, float reflectivity, float transparency, float indexOfRefraction, float translucency, const vector<vec4> &extraParams,
                                 const vector<string> &extraNames)
		: SDFMaterial(ambientColor, diffuseColor, reflectivity, transparency, indexOfRefraction, translucency), extraParams(extraParams), extraNames(extraNames) {
}

vec4 SDFMaterialPlus::getExtraParam(int i) const {
	return extraParams[i];
}

float SDFMaterialPlus::getExtraParam(int i, int j) const {
	return extraParams[i][j];
}

vec4 SDFMaterialPlus::getExtraParam(const string &name) const {
	return extraParams[getExtraIndex(name)];
}

float SDFMaterialPlus::getExtraParam(const string &name, int j) const {
	return extraParams[getExtraIndex(name)][j];
}

vec4 SDFMaterialPlus::operator[](int i) const {
	return i < 3 ? SDFMaterial::operator[](i) : i < size() ? extraParams[i - 3] : throw IndexOutOfBounds(i, size(), "SDFMaterialPlus", __FILE__, __LINE__);
}

int SDFMaterialPlus::size() const {
	return 3 + extraParams.size();
}

int SDFMaterialPlus::getExtraIndex(const string &name) const {
	return indexOf(name, extraNames);
}


SDFScene::SDFScene(const vector<SDFObjectInstance> &objectInstances, const string &objectNumberKey, const string &paramNumberKey, const string &sdfKey)
		: objects({}), materialParamSize(objectInstances[0].material.size()), materialBuffer({}), parameterBuffer({}), objectNumberKey(objectNumberKey), paramNumberKey(paramNumberKey),
		  sdfKey(sdfKey) {
	for (auto &obj: objectInstances) {
		int ind = 0;
		if (objects.size() > 0) ind = objects.back().firstFreeParamIndex() - ind;
		objects.push_back(obj.getObject());
		objects.back().increaseParameters(ind);
		objects.back().resolveConflictsWithName("sdf");
		addAll(parameterBuffer, obj.getParameters());
		addAll(materialBuffer, obj.materialBuffer());
	}
}

CodeMacro SDFScene::mainMacro() const {
	return CodeMacro(sdfKey, totalCode());
}



string SDFScene::sdfPerObjectCode() const {
	string code = "float sdf(vec3 pos, int objNb){\n ";
	for (int i = 0; i < objects.size() - 1; i++)
		code += "\tif (objNb == " + std::to_string(i) + ") return " + objects[i].sdfName() + "(pos);\n";
	code += "\treturn " + objects[objects.size() - 1].sdfName() + "(pos);\n}\n";
	return code;
}

string SDFScene::closestObjectCode() const {
	string code = "int closestObject(vec3 pos){\n "
	              "\tfloat minDist = sdf(pos, 0);\n"
	              "\tint minObj = 0;\n"
	              "\tfor (int i = 1; i < " + objectNumberKey + "; i++){\n"
	                                                           "\t\tfloat d = sdf(pos, i);\n"
	                                                           "\t\tif (d < minDist){\n"
	                                                           "\t\t\tminDist = d;\n"
	                                                           "\t\t\tminObj = i;\n"
	                                                           "\t\t}\n"
	                                                           "\t}\n"
	                                                           "\treturn minObj;\n"
	                                                           "}\n";
	return code;
}

string SDFScene::totalSDFCode() const {
	string code = "float sdf(vec3 pos){\n "
	              "\tfloat minDist = sdf(pos, 0);\n"
	              "\tfor (int i = 1; i < " + objectNumberKey + "; i++){\n"
	                                                           "\t\tfloat d = sdf(pos, i);\n"
	                                                           "\t\tif (d < minDist){\n"
	                                                           "\t\t\tminDist = d;\n"
	                                                           "\t\t}\n"
	                                                           "\t}\n"
	                                                           "\treturn minDist;\n"
	                                                           "}\n";
	return code;
}

string SDFScene::materialCode() const {
	return "Material material(int i){\n"
	       "\treturn materialConstructor(materials[i*3], materials[i*3+1], materials[i*3+2]);\n"
	       "}\n\n"
	       "Material material(vec3 x){\n"
	       "\treturn material(closestObject(x));\n"
	       "}\n";
}

string SDFScene::totalCode() const {
	return "//-------SDF-------\n\n" + objectsCode() + "\n\n" + sdfPerObjectCode() + "\n" + closestObjectCode() + "\n" + totalSDFCode() + "\n" + materialCode();
}

CodeMacro SDFScene::objNumberMacro() const {
	return CodeMacro(objectNumberKey, std::to_string(objects.size()));
}

CodeMacro SDFScene::paramNumberMacro() const {
	return CodeMacro(paramNumberKey, std::to_string(parameterBuffer.size()));
}

TemplateCodeFile SDFScene::templateCode(const CodeFileDescriptor &source, const CodeFileDescriptor &target) const {
	return TemplateCodeFile(source, target, {structsMacro(), mathToolsMacro(), lightsMacro(), sdfToolsMacro(), mainMacro(), objNumberMacro(), paramNumberMacro()});
}

Shader SDFScene::fragmentShader(const CodeFileDescriptor &source, const CodeFileDescriptor &target) const {
	return Shader(templateCode(source, target).generatedCodeFile());
}

string SDFScene::objectsCode() const {
	string code = objects[0].sdfCode();
	for (int i = 1; i < objects.size(); i++)
		code += "\n\n" + objects[i].sdfCode();
	return code;
}

void *SDFScene::getMaterialBuffer() const {
	return (void *) materialBuffer.data();
}

int SDFScene::materialBufferSize() const {
	return materialBuffer.size();
}

void *SDFScene::getParameterBuffer() const {
	return (void *) parameterBuffer.data();
}

int SDFScene::getParameterSize() const {
	return parameterBuffer.size();
}

void SDFScene::addMainParameterToObject(const string &name, int objectIndex, const vec3 &value) {
	int bufferIndex = objects[objectIndex].firstFreeParamIndex();
	objects[objectIndex].addParameterToMain(name);
	parameterBuffer.insert(parameterBuffer.begin() + bufferIndex, value);
}



SDFRenderingStep::SDFRenderingStep(const std::shared_ptr<ShaderProgram> &shader, const SDFScene &object)
		: RenderingStep(shader) {
	paramsData = object.getParameterBuffer();
	paramsSize = object.getParameterSize();
	materialData = object.getMaterialBuffer();
	materialSize = object.materialBufferSize();
}

void SDFRenderingStep::addSDFUniforms() {
	paramsUniformLoc = glGetUniformLocation(shader->programID, "params");
	if (materialSize > 0)
		materialUniformLoc = glGetUniformLocation(shader->programID, "materials");
}

void SDFRenderingStep::loadSDFUniforms() {
	glUniform3fv(paramsUniformLoc, paramsSize, (float *) paramsData);
	if (materialSize > 0)
		glUniform4fv(materialUniformLoc, materialSize, (float *) materialData);
}



void SDFRenderingStep::init(const std::shared_ptr<Camera> &cam, const std::vector<Light> &lights) {
	shader->use();
	addCameraUniforms(cam);
	addLightsUniforms(lights);
	addSDFUniforms();
	attributes = {std::make_shared<Attribute>("position", VEC3, 0, POSITION)};
	attributes[0]->initBuffer();
	attributes[0]->load(&trs[0][0], 6);
	loadSDFUniforms();
}

void SDFRenderingStep::renderStep(float t) {
	shader->use();
	setUniforms(t);
	loadSDFUniforms();
	attributes[0]->load(&trs[0][0], 6);

	enableAttributes();
	glDrawArrays(GL_TRIANGLES, 0, 6);
	disableAttributes();
}

void SDFRenderingStep::addCameraUniforms(const std::shared_ptr<Camera> &camera) {
	auto MVPsetter = [camera, this](float t, const std::shared_ptr<ShaderProgram> &shader) {
		mat4 mvp = camera->vp(t);
		shader->setUniform("mvp", mvp);
	};
	auto positionSetter = [camera, this](float t, const std::shared_ptr<ShaderProgram> &shader) {
		vec3 camPos = camera->position(t);
		shader->setUniform("camPosition", camPos);
	};
	addUniform("mvp", MAT4, std::make_shared<std::function<void(float, shared_ptr<ShaderProgram>)> >(MVPsetter));
	addUniform("camPosition", VEC3, std::make_shared<std::function<void(float, shared_ptr<ShaderProgram>)> >(positionSetter));
}



void SDFRenderer::renderAllSteps() {
	for (auto &step: sdfSteps)
		step->renderStep(time);
}

void SDFRenderer::initRendering() {
	window->initViewport();
	glBindVertexArray(vao);
//	glShadeModel(GL_SMOOTH);
//	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (const auto &renderingStep: sdfSteps)
		renderingStep->init(camera, lights);
}

int SDFRenderer::mainLoop() {
	initRendering();
	while (window->isOpen()) {
		initFrame();
		perFrameFunction(time, dt);
		for (auto &step: sdfSteps)
			step->renderStep(time);
		window->renderFramebufferToScreen();
	}
	return window->destroy();
}

void SDFRenderer::addSDFStep(const shared_ptr<SDFRenderingStep> &step) {
	sdfSteps.push_back(step);
}

SDFObjectInstance::SDFObjectInstance(const SDFObject &obj, const vec3 &position, const mat3 &rotation, const SDFMaterialPlus &material, const vector<vec3> &parameters)
		: obj(obj), position(position), rotation(rotation), material(material), parameters(parameters) {
	this->obj.addAffineTransformToCppSDF(position, rotation);
	addTranslationAndRotationParameters(position, rotation);
}

SDFObjectInstance::SDFObjectInstance(const SDFObject &obj, const vec3 &position, const mat3 &rotation, const SDFMaterialPlus &material, const vector<std::map<string, vec3>> &parametersPerTemplate)
		: obj(obj), position(position), rotation(rotation), material(material), parameters({}) {

	for (int i = 0; i < parametersPerTemplate.size(); i++) {
		for (string key: obj.sortedParameterNamesPerFunction(i))
			parameters.push_back(parametersPerTemplate[i].at(key));
	}
	this->obj.addAffineTransformToCppSDF(position, rotation);
	addTranslationAndRotationParameters(position, rotation);

}

SDFObjectInstance::SDFObjectInstance(const SDFObject &obj, const vec3 &position, const mat3 &rotation, const SDFMaterialPlus &material,
                                     const std::map<string, std::map<string, vec3>> &parametersPerTemplate)
		: obj(obj), position(position), rotation(rotation), material(material), parameters({}) {
	for (string helper: keys(parametersPerTemplate))
		for (string key: obj.getHelper(helper).getSortedKeys())
			parameters.push_back(parametersPerTemplate.at(helper).at(key));

	this->obj.addAffineTransformToCppSDF(position, rotation);
	addTranslationAndRotationParameters(position, rotation);
}

vector<vec3> SDFObjectInstance::getParameters() const {
	return parameters;
}

vector<vec4> SDFObjectInstance::materialBuffer() const {
	return material.compress();
}

SDFObject SDFObjectInstance::getObject() const {
	return obj;
}

SDFObjectInstance::SDFObjectInstance(const SDFObject &obj, const vec3 &position, const SDFMaterialPlus &material, const vector<vec3> &parameters)
		: SDFObjectInstance(obj, position, mat3(1), material, parameters) {
}

SDFObjectInstance::SDFObjectInstance(const SDFObject &obj, const SDFMaterialPlus &material, const vector<vec3> &parameters)
		: SDFObjectInstance(obj, vec3(0), mat3(1), material, parameters) {
}

void SDFObjectInstance::resolveName(const string &name) {
	obj.resolveConflictsWithName(name);
}

SDFObjectInstance SDFObjectInstance::smoothUnion(const SDFObjectInstance &other, float k) const {
	return SDFObjectInstance(obj.smoothUnion(other.obj, k), vec3(0), mat3(1), material, concat<vec3>(getParameters(), other.getParameters()));
}

void SDFObjectInstance::addParameterToMain(const string &key, vec3 value) {
	obj.addParameterToMain(key);
	parameters.push_back(value);
}

void SDFObjectInstance::addTranslationAndRotationParameters(vec3 center, mat3 rotation) {
	obj.addTranslationAndRotationParameters();
	parameters.push_back(center);
	parameters.push_back(rotation[0]);
	parameters.push_back(rotation[1]);
	parameters.push_back(rotation[2]);
}
