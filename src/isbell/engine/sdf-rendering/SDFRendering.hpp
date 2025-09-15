#pragma once
#include "SDFObjects.hpp"

CodeMacro sdfToolsMacro();
CodeMacro mathToolsMacro();
CodeMacro structsMacro();
CodeMacro lightsMacro();


ShaderProgram programGeneratedFromSDFObj(const CodeFileDescriptor &frag_template, const CodeFileDescriptor &vert, const SDFObject &obj, const Path &targetFragentFile, bool applyNumberMacro=true);
CodeFileDescriptor codeGeneratedFromSDFObj(const CodeFileDescriptor &frag_template, const SDFObject &obj, const Path &targetFragentFile, bool applyNumberMacro = true);
std::pair<ShaderProgram, SDFObject> programGeneratedFromSDFObjects(const CodeFileDescriptor &frag_template, const CodeFileDescriptor &vert, const vector<SDFObject> &objs, const Path &targetFragentFile);

TemplateCodeFile vanillaSDFTemplate(const CodeFileDescriptor &source, const CodeFileDescriptor &target);


class SDFMaterial {
	vec4 ambientColor;
	vec4 diffuseColor;
	float reflectivity;
	float transparency;
	float indexOfRefraction;
	float translucency;
public:
	SDFMaterial(vec4 ambientColor, vec4 diffuseColor, float reflectivity, float transparency, float indexOfRefraction, float translucency);
	virtual ~SDFMaterial() = default;
	virtual vector<vec4> compress() const;
	vec4 getAmbientColor() const;
	vec4 getDiffuseColor() const;
	float getReflectivity() const;
	float getAlpha() const;
	float getIndexOfRefraction() const;
	virtual int size() const;
	virtual vec4 operator[](int i) const;
};

class SDFMaterialPlus : public SDFMaterial {
	vector<vec4> extraParams;
	vector<string> extraNames;
public:
	SDFMaterialPlus(vec4 ambientColor, vec4 diffuseColor,
					float reflectivity, float transparency, float indexOfRefraction, float translucency,
					const vector<vec4> &extraParams={}, const vector<string> &extraNames={});

	int getExtraIndex(const string &name) const;
	vec4 getExtraParam(int i) const;
	float getExtraParam(int i, int j) const;
	vec4 getExtraParam(const string &name) const;
	float getExtraParam(const string &name, int j) const;
	int size() const override;
	vec4 operator[](int i) const override;
};

struct SDFObjectInstance {
	SDFObject obj;
	vec3 position;
	mat3 rotation;
	SDFMaterialPlus material;
	vector<vec3> parameters;
public:
	SDFObjectInstance(const SDFObject &obj, const vec3 &position, const mat3 &rotation, const SDFMaterialPlus &material, const vector<vec3> &parameters={});
	SDFObjectInstance(const SDFObject &obj, const vec3 &position, const mat3 &rotation,  const SDFMaterialPlus &material, const vector<std::map<string, vec3>> &parameters={});
	SDFObjectInstance(const SDFObject &obj, const vec3 &position, const mat3 &rotation, const SDFMaterialPlus &material, const std::map<string, std::map<string, vec3>> &parameters={});
	SDFObjectInstance(const SDFObject &obj, const vec3 &position, const SDFMaterialPlus &material, const vector<vec3> &parameters={});
    SDFObjectInstance(const SDFObject &obj, const SDFMaterialPlus &material, const vector<vec3> &parameters={});

	SDFObject getObject() const;
	vector<vec3> getParameters() const;
	vector<vec4> materialBuffer() const;
	void resolveName(const string &name);
	SDFObjectInstance smoothUnion(const SDFObjectInstance &other, float k) const;
	void addParameterToMain(const string &key, vec3 value);
	void addTranslationAndRotationParameters(vec3 center, mat3 rotation);
};


class SDFScene {
	vector<SDFObject> objects;
	int materialParamSize;
	vector<vec3> parameterBuffer;
	vector<vec4> materialBuffer;
	string objectNumberKey;
	string paramNumberKey;
	string sdfKey;

public:
	SDFScene(const vector<SDFObjectInstance> &objectInstances,
	         const string &objectNumberKey= "__N__", const string &paramNumberKey= "__P__", const string &sdfKey= "__SDF__");


	string objectsCode() const;
	string sdfPerObjectCode() const;
	string totalSDFCode() const;
	string closestObjectCode() const;
	string materialCode() const;
	string totalCode() const;


	void addMainParameterToObject(const string &name, int objectIndex, const vec3 &value);
	CodeMacro mainMacro() const;
	CodeMacro objNumberMacro() const;
	CodeMacro paramNumberMacro() const;

	TemplateCodeFile templateCode(const CodeFileDescriptor &source, const CodeFileDescriptor &target) const;
	Shader fragmentShader(const CodeFileDescriptor &source, const CodeFileDescriptor &target) const;

	void* getMaterialBuffer() const;
	int materialBufferSize() const;

	void* getParameterBuffer() const;
	int getParameterSize() const;
};




class SDFRenderingStep : public RenderingStep {
	GLuint paramsUniformLoc = 0;
	GLuint materialUniformLoc = 0;

	int paramsSize;
	int materialSize = 0;

	vector<vec3> trs = {vec3(-1, -1, 1), vec3(-1, 1, 1), vec3(1, -1, 1),
						vec3(-1, 1, 1), vec3(1, -1, 1), vec3(1, 1, 1)};
	void* paramsData;
	void* materialData = nullptr;
public:
	SDFRenderingStep(const std::shared_ptr<ShaderProgram> &shader, const SDFScene &object);
	void addSDFUniforms();
	void loadSDFUniforms();
	void init(const std::shared_ptr<Camera> &cam, const std::vector<Light> &lights) override;
	void renderStep(float t) override;
	void addCameraUniforms(const std::shared_ptr<Camera> &camera) override;

};

class SDFRenderer : public Renderer {
	vector<std::shared_ptr<SDFRenderingStep>> sdfSteps = {};
public:
	using Renderer::Renderer;
	void addSDFStep(const std::shared_ptr<SDFRenderingStep> &step);
	void renderAllSteps() override;
	void initRendering() override;
	int mainLoop() override;
};


SDFObjectInstance sphereSDF(const SDFMaterialPlus &material, float radius, vec3 center=ORIGIN, const string &name = "sphere_sdf");
SDFObjectInstance boxSDF(const SDFMaterialPlus &material, vec3 size, vec3 center=ORIGIN, mat3 rotation = mat3(1), string name = "sdf");
SDFObjectInstance planeSDF(const SDFMaterialPlus &material, vec3 normal=e3, float d=0, const string &name = "sdf");
SDFObjectInstance roundBoxSDF(const SDFMaterialPlus &material, vec3 size, float r, vec3 center=ORIGIN, mat3 rotation=mat3(1), string name = "sdf");
SDFObjectInstance torusSDF(const SDFMaterialPlus &material, float r, float R, vec3 center=ORIGIN, mat3 rotation=mat3(1), string name = "sdf");
SDFObjectInstance smoothTubeSDF(const SDFMaterialPlus &material, float r1, float r2, float R, float k, vec3 center=ORIGIN, const mat3 &rotation=mat3(1), const string &name="sdf");
SDFObjectInstance triangleSDF(const SDFMaterialPlus &material, vec3 a, vec3 b, vec3 c, string name = "sdf");
SDFObjectInstance quadSDF(const SDFMaterialPlus &material, vec3 a, vec3 b, vec3 c, vec3 d, string name = "sdf");
