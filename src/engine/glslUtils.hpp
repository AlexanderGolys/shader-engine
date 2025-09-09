#pragma once

#include "indexedRendering.hpp"
#include "renderingUtils.hpp"
#include "../utils/file-management/filesUtils.hpp"
#include "../utils/logging.hpp"

#include <cstdio>
#include <cstdlib>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "macroParsing.hpp"




class Texture;

void error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

GLuint bindVAO();
void disableAttributeArrays(int how_many=4);
mat4 generateMVP(vec3 camPosition, vec3 camLookAt, vec3 upVector, float fov, float aspectRatio, float clippingRangeMin, float clippingRangeMax, const mat4 &modelTransform);
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
void setUniformTextureSampler(GLuint programID, Texture* texture, int textureSlot);


enum Resolution {
	FHD,
	HD2K,
	UHD,
	UNKNOWN
};

int predefinedWidth(Resolution res);
int predefinedHeight(Resolution res);


class Window {
public:
	GLFWwindow* window;
	int width;
	int height;
	float aspectRatio;
	Window(int width, int height, const char* title);
	Window(Resolution resolution, const char* title);
	~Window();
	int destroy();

	void initViewport();

	void showCursor();
	void disableCursor();
	void hideCursorWithinWindow();
	void stickyKeys(bool sticky);
	void stickyMouseButtons(bool sticky);
	void setCallbacks(const GLFWkeyfun* keyCallback = nullptr, const GLFWcharfun* charCallback = nullptr, const GLFWmousebuttonfun* mouseButtonCallback = nullptr, GLFWcursorposfun* cursorPosCallback = nullptr, GLFWcursorenterfun* cursorEnterCallback = nullptr, GLFWscrollfun* scrollCallback = nullptr, GLFWdropfun* dropCallback = nullptr);
	bool isOpen();

	void renderFramebufferToScreen();
};



size_t sizeOfGLSLType(GLSLType type);
int lengthOfGLSLType(GLSLType type);
GLenum primitiveGLSLType(GLSLType type);

enum ShaderType {
	CLASSIC,
	GEOMETRY1,
};



class Shader {
protected:
	GLenum shaderType;
	GLuint shaderID;
	string code = "";
public:
	explicit Shader(CodeFileDescriptor &file);
	explicit Shader(CodeFileDescriptor &&file);
	explicit Shader(const Path &file) : Shader(CodeFileDescriptor(file)) {}
	explicit Shader(const TemplateCodeFile &file) : Shader(file.generatedCodeFile()) {}

	Shader(const string &code, GLenum shaderType);

	Shader(const Shader &other);
	Shader(Shader &&other) noexcept;
	Shader & operator=(const Shader &other);
	Shader & operator=(Shader &&other) noexcept;
	virtual ~Shader() = default;


	GLuint getID() const;
	void compile();
	string getCode() const;

	static GLenum getTypeFromExtension(const string &extension);
};


class ShaderProgram {
protected:
	Shader vertexShader;
	Shader fragmentShader;
	std::optional<Shader> geometryShader;

	ShaderType shaderType;

public:
	std::unordered_map<string, GLuint> uniformLocations;
	std::unordered_map<string, GLSLType> uniformTypes;

	GLuint programID;

	ShaderProgram(const Shader &vertexShader, const Shader &fragmentShader);
	ShaderProgram(const Shader &vertexShader, const Shader &fragmentShader, const Shader &geometryShader);
	ShaderProgram(const string &vertexPath, const string &fragPath);


	void linkShaders();

	explicit ShaderProgram(const string &standard_file_path);
	~ShaderProgram();

	void use();
	void initUniforms(const std::unordered_map<string, GLSLType> &uniforms);
	void initTextureSampler(const Texture* texture);

	void setTextureSampler(const Texture* texture) const;
	void setUniforms(const std::unordered_map<string, const GLfloat*> &uniformValues);
	void setUniform(const string &uniformName, const GLfloat* uniformValue);
	void setUniform(const string &uniformName, float uniformValue);
	void setUniform(const string &uniformName, int uniformValue);
	void setUniform(const string &uniformName, vec2 uniformValue);
	void setUniform(const string &uniformName, vec3 uniformValue);
	void setUniform(const string &uniformName, vec4 uniformValue);
	void setUniform(const string &uniformName, mat2 uniformValue);
	void setUniform(const string &uniformName, mat3 uniformValue);
	void setUniform(const string& uniformName, mat4 uniformValue);
	void setUniform(const string& uniformName, float x, float y);
	void setUniform(const string &uniformName, float x, float y, float z);
	void setUniform(const string &uniformName, float x, float y, float z, float w);
};

class Camera {
public:
	float fov_x;
	float aspectRatio;
	float clippingRangeMin;
	float clippingRangeMax;
	bool moving;
	shared_ptr<SmoothParametricCurve> trajectory;
    shared_ptr<SmoothParametricCurve> lookAtFunc;
    std::function<vec3(float)> up;
	mat4 projectionMatrix;

	Camera();

	Camera(vec3 position, vec3 lookAtPos, vec3 upVector=vec3(0, 0, 1), float fov_x=PI/4,
		   float aspectRatio=16/9.f, float clippingRangeMin=.01f, float clippingRangeMax=100.f);

	Camera(float radius, float speed, float height, vec3 lookAtPos=vec3(0), vec3 upVector=vec3(0, 0, 1), float fov_x=PI/4,
		   float aspectRatio=16/9.f, float clippingRangeMin=.01f, float clippingRangeMax=100.f);

	Camera(const shared_ptr<SmoothParametricCurve> &trajectory, vec3 lookAtPos, vec3 upVector, float fov_x = PI / 4,
	       float aspectRatio = 16 / 9.f, float clippingRangeMin=.01f, float clippingRangeMax=100.f);

    Camera(const shared_ptr<SmoothParametricCurve> &trajectory, const shared_ptr<SmoothParametricCurve> &lookAtPos, vec3 upVector, float fov_x = PI / 4,
       float aspectRatio = 16 / 9.f, float clippingRangeMin=.01f, float clippingRangeMax=100.f);

    Camera(const shared_ptr<SmoothParametricCurve> &trajectory, const shared_ptr<SmoothParametricCurve> &lookAtPos, const std::function<vec3(float)> &upVector, float fov_x = PI / 4,
   float aspectRatio = 16 / 9.f, float clippingRangeMin=.01f, float clippingRangeMax=100.f);

	vec3 position(float t) const;
	vec3 lookAtPoint(float t) const;
	vec3 upVector(float t) const;
	mat4 mvp(float t, const mat4 &modelTransform);
	mat4 viewMatrix(float t);
	mat4 vp(float t);
};

class Attribute {
public:
	string name;
	GLuint bufferAddress;
	size_t size;
	GLSLType type;
	int inputNumber;
	bool enabled;
	bool bufferInitialized;
	CommonBufferType bufferType;

	Attribute(const string &name, GLSLType type, int inputNumber, CommonBufferType bufferType);
	virtual ~Attribute();

	virtual void initBuffer();
	virtual void enable();
	virtual void disable();
	virtual void load(const void* firstElementAdress, int bufferLength);
	virtual void freeBuffer();
};






class RenderingStep {
protected:
	shared_ptr<ShaderProgram> shader;
	vector<shared_ptr<Attribute>> attributes;
	shared_ptr<IndexedMesh> weak_super = nullptr;
	GLuint elementBufferLoc = 0;
	shared_ptr<MaterialPhong> material = nullptr;

	void weakMeshRenderStep(float t);

public:
	explicit RenderingStep(const shared_ptr<ShaderProgram> &shader);
	RenderingStep(const shared_ptr<ShaderProgram> &shader, const shared_ptr<MaterialPhong> &material);
	RenderingStep(const RenderingStep &other);

	virtual ~RenderingStep();

	std::map<string, GLSLType> uniforms;
	std::map<string, shared_ptr<std::function<void(float, shared_ptr<ShaderProgram>)>>> uniformSetters;
	std::function<void(float)> customStep;

	void setWeakSuperMesh(const shared_ptr<IndexedMesh> &super);

	int findAttributeByName(const string &name);
	void initStdAttributes();
    void initElementBuffer();
	void resetAttributeBuffers();
	void initUnusualAttributes(const vector<shared_ptr<Attribute>>& attributes);
	void initWeakMeshAttributes();

	void loadMeshAttributes();
    void loadElementBuffer();
	void enableAttributes();
	void disableAttributes();

	void initMaterialTextures();
	void bindTextures();

	void addCustomAction(const std::function<void(float)> &action);
    bool weakSuperLoaded() const;
	virtual void init(const shared_ptr<Camera> &cam, const vector<Light> &lights);

	void addUniforms(const std::map<string, GLSLType> &uniforms, std::map<string, shared_ptr<std::function<void(float, shared_ptr<ShaderProgram>)>>> setters);
	void addUniform(string uniformName, GLSLType uniformType, shared_ptr<std::function<void(float, shared_ptr<ShaderProgram>)>> setter);
	void addConstFloats(const std::map<string, float>& uniforms);
	void addConstVec4(const string& name, vec4 value);
	void addConstColor(const string &name, vec4 value);
	void addMaterialUniforms();
	void setUniforms(float t);

    virtual void addCameraUniforms(const shared_ptr<Camera>& camera);
	void addLightUniform(const Light &pointLight, int lightIndex = 1);
	void addLightsUniforms(const vector<Light> &lights);

	virtual void renderStep(float t);

};


class RenderSettings {
public:
	vec4 bgColor;
	bool alphaBlending;
	bool depthTest;
	bool timeUniform;
	float speed;
	int maxFPS;
	Resolution resolution;
	string windowTitle;

	bool takeScreenshots;
	float screenshotFrequency;
	Path screenshotDirectory;

	RenderSettings(vec4 bgColor, bool alphaBlending, bool depthTest, bool timeUniform, float speed, int maxFPS,
		bool takeScreenshots, Resolution resolution, float screenshotFrequency=0.f, const string &windowTitle="window");
};






class Renderer {
protected:
	float last_time_capture = 0;
	float since_last_scr = 0;

	unique_ptr<Window> window;
	GLuint vao;
	vector<shared_ptr<RenderingStep>> renderingSteps;

	shared_ptr<Camera> camera;
	vector<Light> lights;

	float time = 0;
    float dt = 0;
	END(float) animSpeed;
	std::function<void(float, float)> perFrameFunction;
	RenderSettings settings;

public:
	explicit Renderer(float animSpeed=1.f, vec4 bgColor=BLACK, const string &screenshotDirectory="screenshots/", float screenshotFrequency=-1);
	explicit Renderer(const RenderSettings &settings);
	virtual ~Renderer();
	
	void initMainWindow(int width, int height, const char* title);
	void initMainWindow(Resolution resolution, const char* title);
	void initMainWindow();
	void resetTimer();

	void setCamera(const shared_ptr<Camera> &camera);
	void setLights(const vector<Light> &lights);

	void setLightWithMesh(const Light &light, const shared_ptr<MaterialPhong> &material, const ShaderProgram &shader, float radius);
	void setLightsWithMesh(const vector<Light> &lights, const shared_ptr<MaterialPhong> &material, const ShaderProgram &shader, float radius);
	void setLightWithMesh(const Light &light, float ambient, float diff, float spec, float shine, const ShaderProgram &shader, float radius);
	void setLightsWithMesh(const vector<Light> &lights, float ambient, float diff, float spec, float shine, const ShaderProgram &shader, float radius);

	void addRenderingStep(shared_ptr<RenderingStep> renderingStep);
	void addMeshStep(const ShaderProgram& shader, const shared_ptr<IndexedMesh> &model, const shared_ptr<MaterialPhong>& material);

	float initFrame();
	float lastDeltaTime() const;

	void addPerFrameUniforms(const std::map<string, GLSLType> &uniforms, const std::map<string, shared_ptr<std::function<void(float, shared_ptr<ShaderProgram>)>>> &setters);
	void addPerFrameUniform(const string &uniformName, GLSLType uniformType, const shared_ptr<std::function<void(float, shared_ptr<ShaderProgram>)>> &setter);
	void addConstUniforms(const std::map<string, GLSLType>& uniforms, std::map<string, shared_ptr<std::function<void(shared_ptr<ShaderProgram>)>>> setters);
	void addConstUniform(const string &uniformName, GLSLType uniformType, shared_ptr<std::function<void(shared_ptr<ShaderProgram>)>> setter);
	void addTimeUniform();
	void addConstFloats(const std::map<string, float> &uniforms);
	void addCustomAction(std::function<void(float)> action);
    void addCustomAction(std::function<void(float, float)> action);
	void nonlinearSpeed(const END(float) &speed);
	void addSurfaceFamilyDeformer(SurfaceParametricPencil &pencil, IndexedMesh &surface);

	void addFloorWorkingArea(vec3 corner1, vec3 corner2, vec3 corner3, vec3 corner4, const MaterialPhong &material, float height, float pyramid_height);

	virtual void initRendering();
	virtual void renderAllSteps();
	virtual int mainLoop();
};
